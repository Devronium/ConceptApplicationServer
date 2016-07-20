//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include "AnsiList.h"
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include "semhh.h"
#include <map>

#ifndef NOSSL
extern "C" {
#ifdef WITH_OPENSSL
 #include <openssl/bio.h>
 #include <openssl/ssl.h>
 #include <openssl/err.h>
#else
 #include "tlse.h"
 #define ERR_print_errors_fp(err)   fprintf(stderr, "TLS error in %s:%i\n", __FILE__, __LINE__)
#endif
}
#endif

#define CACHE_SIZE       0xFFFF
#define CHUNK_SIZE       0x80000
#define MAX_RT_SIZE      0x9FFF


#ifdef _WIN32
 #define _WIN32_WINNT    0x0501

 #include <io.h>
 #include <winsock2.h>
 #include <ws2tcpip.h>
#else
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netinet/tcp.h>
 #include <arpa/inet.h>
 #include <poll.h>
 #include <sys/select.h>
 #include <time.h>    // sleep
 #include <netdb.h>

int eof(int stream) {
    struct pollfd ufds[1];

    ufds[0].fd     = stream;
    ufds[0].events = POLLIN;

    int res = poll(ufds, 1, 0);
    if (res < 0)
        return -1;
    return !res;
}

int is_writeable(int fd) {
    struct pollfd p;
    int           ret;

    p.fd     = fd;
    p.events = POLLOUT;

    ret = poll(&p, 1, 0);

    if (ret < 0)
        return 0;

    return p.revents & POLLOUT;
}
#endif

#define SOCKET_RECREATE_CHECK(forced)                                                           \
    if (mc->event_timeout > 0) {                                                                \
        int new_socket = Invoke(INVOKE_GET_PROTO_SOCKET, PARAMETERS->HANDLER);                  \
        if (CLIENT_SOCKET != new_socket) {                                                      \
            mc->ResetSocket(CLIENT_SOCKET);                                                     \
            mc->send_failed      = 0;                                                           \
            mc->echo_sent        = 0;                                                           \
            mc->last_msg_seconds = time(NULL);                                                  \
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0); \
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0); \
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0); \
            SetVariable(RESULT, VARIABLE_NUMBER, "", 1);                                        \
            ResetConceptPeer(mc, new_socket);                                                   \
            UpdateTLSSocket(mc, new_socket);                                                    \
            mc->ConnectionChanged(Invoke, new_socket);                                          \
            return 0;                                                                           \
        }                                                                                       \
    }


#define SOCKET_RECREATE_CHECK_WAIT_MESSAGE                                                  \
    int new_socket = Invoke(INVOKE_GET_PROTO_SOCKET, PARAMETERS->HANDLER);                  \
    if (CLIENT_SOCKET != new_socket) {                                                      \
        mc->ResetSocket(CLIENT_SOCKET);                                                     \
        mc->last_msg_seconds = time(NULL);                                                  \
        mc->send_failed      = 0;                                                           \
        mc->echo_sent        = 0;                                                           \
        SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0); \
        SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0); \
        SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0); \
        SetVariable(RESULT, VARIABLE_NUMBER, "", 1);                                        \
        ResetConceptPeer(mc, new_socket);                                                   \
        UpdateTLSSocket(mc, new_socket);                                                    \
        mc->ConnectionChanged(Invoke, new_socket);                                          \
        return 0;                                                                           \
    }                                                                                       \

#define GET_METACONTAINER                                                                \
    MetaContainer * mc = NULL;                                                           \
    Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)0, &mc);                       \
    if (!mc) {                                                                           \
        mc = new MetaContainer(REMOTE_PUBLIC_KEY, CLIENT_SOCKET, Invoke(INVOKE_GET_DIRECT_PIPE, PARAMETERS->HANDLER));\
        Invoke(INVOKE_SETPROTODATA, PARAMETERS->HANDLER, (int)0, mc, destroy_metadata);  \
        /*if ((REMOTE_PUBLIC_KEY) && (REMOTE_PUBLIC_KEY[0]))                          */ \
        /*    RestoreSession(mc, CLIENT_SOCKET, REMOTE_PUBLIC_KEY);                   */ \
    }

#define GET_METACONTAINER2                                                                            \
    MetaContainer * mc = NULL;                                                                        \
    LocalInvoker(INVOKE_GETPROTODATA, handler, (int)0, &mc);                                          \
    if (!mc) {                                                                                        \
        char *REMOTE_PUBLIC_KEY = NULL;                                                               \
        LocalInvoker(INVOKE_GET_KEYS, handler, (char **)NULL, (char **)NULL, &REMOTE_PUBLIC_KEY);     \
        mc = new MetaContainer(REMOTE_PUBLIC_KEY, CLIENT_SOCKET, LocalInvoker(INVOKE_GET_DIRECT_PIPE, handler));\
        LocalInvoker(INVOKE_SETPROTODATA, handler, (int)0, mc, destroy_metadata);                     \
        /*    RestoreSession(mc, CLIENT_SOCKET, REMOTE_PUBLIC_KEY);                   */              \
    }

#include "INTERFACE_TO_AES.h"

#ifdef __WITH_INTEL_LIB
extern "C" {
 #include "AES-NI/iaesni.h"
}
#endif

#ifdef _WIN32
 #define pipe(phandles)    _pipe(phandles, 4096, _O_BINARY)
#endif

#ifndef NOSSL
#ifdef WITH_OPENSSL
 #if defined(WIN32)
  #define MUTEX_TYPE    HANDLE
  #define MUTEX_SETUP(x)      (x) = CreateMutex(NULL, FALSE, NULL)
  #define MUTEX_CLEANUP(x)    CloseHandle(x)
  #define MUTEX_LOCK(x)       WaitForSingleObject((x), INFINITE)
  #define MUTEX_UNLOCK(x)     ReleaseMutex(x)
  #define THREAD_ID     GetCurrentThreadId()
 #else
  #include <pthread.h>
  #define MUTEX_TYPE    pthread_mutex_t
  #define MUTEX_SETUP(x)      pthread_mutex_init(&(x), NULL)
  #define MUTEX_CLEANUP(x)    pthread_mutex_destroy(&(x))
  #define MUTEX_LOCK(x)       pthread_mutex_lock(&(x))
  #define MUTEX_UNLOCK(x)     pthread_mutex_unlock(&(x))
  #define THREAD_ID    pthread_self()
 #endif

static MUTEX_TYPE *mutex_buf = NULL;

static void locking_function(int mode, int n, const char *file, int line) {
    if (mode & CRYPTO_LOCK)
        MUTEX_LOCK(mutex_buf[n]);
    else
        MUTEX_UNLOCK(mutex_buf[n]);
}

static unsigned long id_function(void) {
    return (unsigned long)THREAD_ID;
}

int thread_setup(void) {
    int i;

    mutex_buf = (MUTEX_TYPE *)malloc(CRYPTO_num_locks() * sizeof(MUTEX_TYPE));
    if (!mutex_buf)
        return 0;
    for (i = 0; i < CRYPTO_num_locks(); i++)
        MUTEX_SETUP(mutex_buf[i]);
    CRYPTO_set_id_callback(id_function);
    CRYPTO_set_locking_callback(locking_function);
    return 1;
}

int thread_cleanup(void) {
    int i;

    if (!mutex_buf)
        return 0;
    CRYPTO_set_id_callback(NULL);
    CRYPTO_set_locking_callback(NULL);
    for (i = 0; i < CRYPTO_num_locks(); i++)
        MUTEX_CLEANUP(mutex_buf[i]);
    free(mutex_buf);
    mutex_buf = NULL;
    return 1;
}
#endif
#endif

static INVOKE_CALL LocalInvoker     = NULL;
static int         hardware_aes     = 0;
static signed char is_little_endian = 1;
static signed char ref_isWebSocket  = 0;

class MetaContainer {
public:
#ifdef __WITH_INTEL_LIB
    unsigned char in_init_vector[32];
    unsigned char out_init_vector[32];
#endif
    const char *REMOTE_KEY;
    AnsiList BufferedMessages;
    AnsiList ThreadedMessages;

    signed char has_debug;
    int         DebugPipes[2];

    int  is_cached;
    char *cached_buffer;
    int  cached_size;
    int  alloc_cached_size;
    int  RTSOCKET;
    int  direct_pipe;
    struct sockaddr_storage remote_conceptaddr;
    struct sockaddr_storage remote_conceptudpaddr;
    socklen_t   remote_len;
    signed char RTCONFIRMED;

    HHSEM       sem_send;
    HHSEM       sem_recv;
    HHSEM       sem_bufferlist;
    int         send_failed;
    int         event_timeout;
    time_t      last_msg_seconds;
    int         echo_sent;
    signed char force_exit;
#ifndef NOSSL
    SSL_CTX *sslctx;
    SSL     *ssl;
#endif

    AES         EncryptAes;
    AES         DecryptAes;

    signed char En_inited;
    signed char Dec_inited;

    std::map<unsigned int, int> mapped_functions;

    void          **LOOPERS;
    int           LOOPERS_count;
    unsigned long data_in;
    unsigned long data_out;
    unsigned long rt_in;
    unsigned long rt_out;
    signed char   rt_send_enabled;

    void *ConnectionChangedDelegate;

    MetaContainer(const char *REMOTE_PUBLIC_KEY, int socket, int direct_pipe) {
#ifndef NOSSL
        sslctx = NULL;
        ssl    = NULL;
#ifndef WITH_OPENSSL
        if ((ref_isWebSocket) && (REMOTE_PUBLIC_KEY)) {
            if (!memcmp(REMOTE_PUBLIC_KEY, "----SSL DATA----", 16)) {
                // is wss://
                unsigned char *ptr = (unsigned char *)REMOTE_PUBLIC_KEY + 16;
                unsigned short size = ntohs(*(unsigned short *)&ptr[3]) + 5;
                ssl = tls_import_context(ptr, size);
                if (ssl)
                    SSL_set_fd(ssl, socket);
            }
        }
#endif
#endif
        REMOTE_KEY        = REMOTE_PUBLIC_KEY;
        ConnectionChangedDelegate = NULL;
        has_debug         = 0;
        is_cached         = 0;
        cached_buffer     = 0;
        cached_size       = 0;
        alloc_cached_size = 0;
        RTSOCKET          = -1;
        remote_len        = 0;
        RTCONFIRMED       = 0;

        send_failed      = 0;
        event_timeout    = 0;
        last_msg_seconds = 0;
        echo_sent        = 0;

        seminit(sem_send, 1);
        seminit(sem_recv, 1);
        seminit(sem_bufferlist, 1);
        memset(&remote_conceptaddr, 0, sizeof(remote_conceptaddr));
        memset(&remote_conceptudpaddr, 0, sizeof(remote_conceptudpaddr));
        #ifdef __WITH_INTEL_LIB
            memset(in_init_vector, 0, 32);
            memset(out_init_vector, 0, 32);
        #endif
        force_exit      = 0;
        En_inited       = 0;
        Dec_inited      = 0;
        LOOPERS         = 0;
        LOOPERS_count   = 0;
        data_in         = 0;
        data_out        = 0;
        rt_in           = 0;
        rt_out          = 0;
        rt_send_enabled = 1;
        this->direct_pipe = direct_pipe;
    }

    void ResetSocket(int socket) {
#ifdef _WIN32
        closesocket(socket);
#else
        close(socket);
#endif
    }

    int AddLooper(void *DELEGATE, INVOKE_CALL Invoke) {
        for (int i = 0; i < LOOPERS_count; i++) {
            if (!LOOPERS[i]) {
                Invoke(INVOKE_LOCK_VARIABLE, DELEGATE);
                LOOPERS[i] = DELEGATE;
                return i + 1;
            }
        }
        LOOPERS_count++;
        LOOPERS = (void **)realloc(LOOPERS, sizeof(void *) * LOOPERS_count);
        if (LOOPERS) {
            LOOPERS[LOOPERS_count - 1] = DELEGATE;
            Invoke(INVOKE_LOCK_VARIABLE, DELEGATE);
            return LOOPERS_count;
        }
        return -1;
    }

    int SetOnConnectionChanged(void *DELEGATE, INVOKE_CALL Invoke) {
        if (ConnectionChangedDelegate)
            Invoke(INVOKE_FREE_VARIABLE, ConnectionChangedDelegate);

        ConnectionChangedDelegate = DELEGATE;
        Invoke(INVOKE_LOCK_VARIABLE, DELEGATE);
        return 1;
    }

    void ConnectionChanged(INVOKE_CALL Invoke, int new_socket) {
        if (ConnectionChangedDelegate) {
            void *RES       = NULL;
            void *EXCEPTION = NULL;
            Invoke(INVOKE_CALL_DELEGATE, ConnectionChangedDelegate, &RES, &EXCEPTION, (INTEGER)0);
            if (EXCEPTION)
                Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);
            if (RES)
                Invoke(INVOKE_FREE_VARIABLE, RES);
        }
        
        this->LoadNewContextFromPipe(new_socket);
    }

    void RemoveLooper(int pos, INVOKE_CALL Invoke) {
        if ((pos >= 0) && (pos < LOOPERS_count)) {
            if (LOOPERS[pos]) {
                Invoke(INVOKE_FREE_VARIABLE, LOOPERS[pos]);
                LOOPERS[pos] = NULL;
            }
        }
    }

    void Iterate(INVOKE_CALL Invoke) {
        for (int i = 0; i < LOOPERS_count; i++) {
            if (LOOPERS[i]) {
                void *RES       = NULL;
                void *EXCEPTION = NULL;
                Invoke(INVOKE_CALL_DELEGATE, LOOPERS[i], &RES, &EXCEPTION, (INTEGER)0);
                if (EXCEPTION)
                    Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);
                if (RES) {
                    NUMBER  ndata   = 0;
                    char    *szdata = 0;
                    INTEGER TYPE    = 0;
                    Invoke(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

                    if ((TYPE == VARIABLE_NUMBER) && (ndata == 1))
                        RemoveLooper(i, Invoke);

                    Invoke(INVOKE_FREE_VARIABLE, RES);
                }
            }
        }
    }

#ifndef WITH_OPENSSL
    int LoadNewContext(int socket, unsigned char *ptr, int size) {
        if ((ref_isWebSocket) && (ptr) && (size > 0)) {
            if (ssl)
                SSL_free(ssl);

            ssl = tls_import_context(ptr, size);
            if (ssl) {
                SSL_set_fd(ssl, socket);
                return 0;
            }
        }
        return -1;
    }
#endif

    int LoadNewContextFromPipe(int socket) {
        if ((ref_isWebSocket) && (direct_pipe > 0)) {
            int size = 0;
            if (read(direct_pipe, &size, sizeof(int)) != sizeof(int))
                return -1;
#ifndef WITH_OPENSSL
            size = ntohl(size);

            if ((size > 0) && (size < 0x7FFFF)) {
                unsigned char *ptr = (unsigned char *)malloc(size);
                if (ptr) {
                    int rem = size;
                    int processed = 0;
                    unsigned char *ptr_pos = ptr;
                    do {
                        processed = read(direct_pipe, ptr_pos, rem);
                        if (processed > 0) {
                            rem -= processed;
                            ptr_pos += processed;
                            if (rem <= 0)
                                break;
                        }
                    } while (processed > 0);

                    int res = -1;
                    if (processed > 0)
                        res = LoadNewContext(socket, ptr, size);

                    free(ptr);
                    return res;
                }
            } else
#endif
                return 0;
        }
        return -2;
    }

    ~MetaContainer() {
        if (RTSOCKET > 0) {
#ifdef _WIN32
            closesocket(RTSOCKET);
#else
            close(RTSOCKET);
#endif
            RTSOCKET    = -1;
            RTCONFIRMED = 0;
        }

        if (LOOPERS) {
            free(LOOPERS);
            LOOPERS       = NULL;
            LOOPERS_count = 0;
        }
        semdel(sem_send);
        semdel(sem_recv);
        semdel(sem_bufferlist);
#ifndef NOSSL
        if (ssl) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
            ssl = NULL;
        }
        if (sslctx) {
            SSL_CTX_free(sslctx);
            sslctx = NULL;
        }
#endif
    }
};


struct TimerData {
    long          RID;
    int           repeat;
    int           APID;

    MetaContainer *mc;
    unsigned int  ms;
};


#ifndef NOSSL
//int RestoreSession(MetaContainer *mc, SYSTEM_SOCKET CLIENT_SOCKET, char *data);
#endif
void send_message2(MetaContainer *mc, AnsiString Owner, AnsiString Target, int message, AnsiString message_data, char *REMOTE_PUBLIC_KEY, int CLIENT_SOCKET);

void destroy_metadata(void *data, void *handler) {
    if (data)
        delete (MetaContainer *)data;
}

int sock_eof(int stream) {
    if (stream < 0)
        return 0;
#ifdef _WIN32
    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 10;
    fd_set s;
    FD_ZERO(&s);
    FD_SET(stream, &s);

    int val = select(stream + 1, &s, 0, 0, &timeout);

    if (val < 0)
        return 0;
    if (val) {
        if (FD_ISSET(stream, &s))
            return 0;
    }
    return 1;
#else
    struct pollfd ufds[1];
    ufds[0].fd     = stream;
    ufds[0].events = POLLIN;

    int res = poll(ufds, 1, 0);

    return !res;
#endif
}

int sock_eof2(int stream, int t) {
#ifdef _WIN32
    struct timeval timeout;
    timeout.tv_sec  = t / 1000;
    timeout.tv_usec = t % 1000;
    fd_set s;
    FD_ZERO(&s);
    FD_SET(stream, &s);

    int val = select(stream + 1, &s, 0, 0, &timeout);

    if (val < 0)
        return 0;
    if (val) {
        if (FD_ISSET(stream, &s))
            return 0;
    }
    return 1;
#else
    struct pollfd ufds[1];
    ufds[0].fd     = stream;
    ufds[0].events = POLLIN;

    int res = poll(ufds, 1, t);

    return !res;
#endif
}

int sock_eof_ssl(int stream, MetaContainer *mc) {
#ifndef NOSSL
    if ((mc->ssl) && (SSL_pending(mc->ssl)))
        return 0;
#endif
    return sock_eof(stream);
}

int sock_eof_timeout(int stream, int timeoutms) {
//#ifdef _WIN32
    int sec = timeoutms / 1000;
    int ms  = timeoutms % 1000;

    struct timeval timeout;

    timeout.tv_sec  = sec;
    timeout.tv_usec = ms * 1000;

    fd_set s;
    FD_ZERO(&s);
    FD_SET(stream, &s);

    int val = select(stream + 1, &s, 0, 0, &timeout);

    if (val < 0)
        return -1;
    if (val) {
        if (FD_ISSET(stream, &s))
            return 0;
    }
    return 1;

/*#else
    int res = 1;
    do {
        struct pollfd ufds[1];
        ufds[0].fd = stream;
        ufds[0].events = POLLIN;

        int res = poll(ufds, 1, 1);
        if (res)
            break;
    } while (--timeoutms > 0);
    return !res;
 #endif*/
}

void ResetConceptPeer(MetaContainer *mc, SYSTEM_SOCKET socket) {
    // disable UDP traffic on network change
    if (mc->RTSOCKET > 0) {
#ifdef _WIN32
        closesocket(mc->RTSOCKET);
#else
        close(mc->RTSOCKET);
#endif
        mc->RTSOCKET    = -1;
        mc->RTCONFIRMED = 0;
    }
}

int deturnated_recv(MetaContainer *mc, int CLIENT_SOCKET, char *buffer, int size, int flags = 0) {
#ifndef NOSSL
    if (mc->ssl) {
        int res = SSL_read(mc->ssl, buffer, size);
        if (res < 0)
            ERR_print_errors_fp(stderr);
        else
            mc->data_in += res;
        return res;
    }
#endif
    int res = recv(CLIENT_SOCKET, buffer, size, flags);
    if (res > 0)
        mc->data_in += res;
    return res;
}

int deturnated_send(MetaContainer *mc, int CLIENT_SOCKET, char *buffer, int size, int extra = 0, int is_udp = 0, int encrypted = 0) {
    int ssize     = 0;
    int orig_size = size;

    mc->data_out += size;
    int RTSOCKET = mc->RTSOCKET;
    semp(mc->sem_send);
    do {
        int to_send = size;
        if (size > CHUNK_SIZE)
            to_send = CHUNK_SIZE;
        if ((is_udp) && (RTSOCKET > 0) && (mc->RTCONFIRMED) && (mc->rt_send_enabled) && (mc->remote_conceptudpaddr.ss_family)) {
#if defined(_WIN32) || !defined(MSG_NOSIGNAL)
            ssize = sendto(RTSOCKET, buffer, to_send, 0, (struct sockaddr *)&mc->remote_conceptudpaddr, mc->remote_len);
#else
            ssize = sendto(RTSOCKET, buffer, to_send, MSG_NOSIGNAL, (struct sockaddr *)&mc->remote_conceptudpaddr, mc->remote_len);
#endif
            if (ssize <= 0) {
#ifdef _WIN32
                if (WSAGetLastError() != WSAEWOULDBLOCK) {
                    if (WSAGetLastError() == WSAEMSGSIZE) {
#else
                if (errno != EAGAIN) {
                    if (errno == EMSGSIZE) {
#endif
                        fprintf(stderr, "Fall back to TCP socket (errno: %i, size: %i)\n", (int)errno, size);
                        RTSOCKET = -1;
                    } else {
                        RTSOCKET = -1;
                    }
                    if ((encrypted) && (!ref_isWebSocket))
                        return 0;
                }
            } else {
                mc->rt_out += ssize;
                break;
            }
            if (ref_isWebSocket)
                return size;
        } else {
#ifndef NOSSL
            if (mc->ssl) {
                ssize = SSL_write(mc->ssl, buffer, to_send);
                if (ssize < 0)
                    ERR_print_errors_fp(stderr);
            } else
#endif
#if defined(_WIN32) || !defined(MSG_NOSIGNAL)
            ssize = send(CLIENT_SOCKET, buffer, to_send, 0);
#else
            ssize = send(CLIENT_SOCKET, buffer, to_send, MSG_NOSIGNAL);
#endif
        }

        if (ssize <= 0) {
#ifdef _WIN32
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                Sleep(5);
                continue;
            }
#else
            if (errno == EAGAIN) {
                usleep(5000);
                continue;
            }
#endif

            if (ssize < 0)
                mc->send_failed = 1;
            semv(mc->sem_send);
            return -1;
        }

        buffer += ssize;
        size   -= ssize;
    } while (size);

    semv(mc->sem_send);
    return orig_size;
}

//-----------------------------------------------------------------------------------
int deturnated_read(int p, void *addr, int size) {
    int  total     = 0;
    int  remaining = size;
    char *ref_addr = (char *)addr;

    do {
        int r = read(p, ref_addr + total, remaining);
        if (r < 0)
            return r;
        if (!r) {
#ifdef _WIN32
            Sleep(5);
#else
            usleep(5000);
#endif
        } else {
            total     += r;
            remaining -= r;
        }
    } while (total < size);
    return total;
}

//-----------------------------------------------------------------------------------
int Cache(MetaContainer *mc, char *buffer, int size) {
    if (ref_isWebSocket) {
        int old_size = mc->cached_size;
        mc->cached_size += size;
        if (mc->alloc_cached_size < mc->cached_size) {
            mc->alloc_cached_size += (mc->cached_size / CACHE_SIZE + 1) * CACHE_SIZE;
            mc->cached_buffer      = (char *)realloc(mc->cached_buffer, mc->alloc_cached_size);
        }
        memcpy(mc->cached_buffer + old_size, buffer, size);
    } else {
        int old_size = mc->cached_size;
        mc->cached_size += size + sizeof(int);
        if (mc->alloc_cached_size < mc->cached_size) {
            mc->alloc_cached_size += (mc->cached_size / CACHE_SIZE + 1) * CACHE_SIZE;
            mc->cached_buffer      = (char *)realloc(mc->cached_buffer, mc->alloc_cached_size);
        }
        *(int *)(mc->cached_buffer + old_size) = htonl(size);
        memcpy(mc->cached_buffer + old_size + sizeof(int), buffer, size);
    }
    return 1;
}

//-----------------------------------------------------------------------------------
int FlushCache(MetaContainer *mc, int CLIENT_SOCKET) {
    int res = -1;

    if ((mc->cached_size) && (mc->cached_buffer)) {
        res = deturnated_send(mc, CLIENT_SOCKET, mc->cached_buffer, mc->cached_size, 0);
        free(mc->cached_buffer);
        mc->cached_size       = 0;
        mc->alloc_cached_size = 0;
        mc->cached_buffer     = 0;
    }
    return res;
}

//-----------------------------------------------------------------------------------
static char *empty_char = (char *)"\0";
static char *def_msg    = "350";
int DeSerializeBuffer2(char *buffer, int size, char **Owner, int *owner_len, int *message, char **Target, int *len_target, char **Value, int *len_value, unsigned int compressed = 0, char *buf_owner = 0) {
    int available_bytes =  size;

    *Owner = NULL;
    *owner_len = 0;
    *Target = NULL;
    *len_target = 0;
    *Value = 0;
    *len_value = 0;

    if (compressed) {
        if (ref_isWebSocket) {
            if (buf_owner) {
                buf_owner[0] = 0;
                sprintf(buf_owner, "%i", (unsigned int)ntohs(*(unsigned short *)buffer));
                *Owner     = buf_owner;
                *owner_len = strlen(buf_owner);
            }
            *message = 0x1001;
            *Target = def_msg;
            *len_target = 3;
            *Value = buffer + 2;
            *len_value = size - 2;
            return 0;
        } else {
            compressed &= 0xF0000000;
            unsigned int owner   = compressed >> 16;
            int          val_len = compressed & 0xFFFF;
            if (val_len > size)
                return 0;

            if (buf_owner) {
                buf_owner[0] = 0;
                sprintf(buf_owner, "%i", owner);
                *Owner     = buf_owner;
                *owner_len = strlen(buf_owner);
            }
            *message = 0x1001;
            // on buffer
            *Target     = def_msg;
            *len_target = 3;
            *Value      = buffer;
            *len_value  = val_len;
        }
        return 0;
    }
    // OWNER poate avea maxim 256 octeti
    char          *ptr       = buffer;
    unsigned char owner_size = *(unsigned char *)buffer;
    buffer++;
    available_bytes -= owner_size + 7;
    if (available_bytes < 0) {
        *message = 0;
        return -1;
    }
    *owner_len = owner_size;
    if (owner_size) {
        *Owner  = buffer;
        buffer += owner_size;
    } else
        *Owner = empty_char;

    *message = ntohl(*(int *)buffer);
    buffer  += sizeof(int);
    unsigned short target_size = ntohs(*(unsigned short *)buffer);
    buffer     += sizeof(short);
    available_bytes -= target_size;
    if (available_bytes < 0) {
        *message = 0;
        return -1;
    }

    *len_target = target_size;

    if (target_size) {
        *Target = buffer;
        buffer += target_size;
    } else
        *Target = empty_char;

    *len_value = size - ((uintptr_t)buffer - (uintptr_t)ptr);
    if (*len_value > 0)
        *Value = buffer;
    else {
        *Value     = empty_char;
        *len_value = 0;
    }
    return 0;
}

//-----------------------------------------------------------------------------------
int DeSerializeBuffer(char *buffer, int size, AnsiString *Owner, int *message, AnsiString *Target, AnsiString *Value, unsigned int compressed = 0) {
    if (compressed) {
        if (ref_isWebSocket) {
            *Owner = (long)ntohs(*(unsigned short *)buffer);
            *message = 0x1001;
            Target->LoadBuffer(def_msg, 3);
            Value->LoadBuffer(buffer + 2, size - 2);
            return 0;
        } else {
            compressed &= 0x0FFFFFFF;
            int owner   = compressed >> 16;
            int val_len = compressed & 0xFFFF;
            if (val_len > size)
                return 0;

            *Owner = (long)owner;
            // event fired
            *message = 0x1001;
            // on buffer
            Target->LoadBuffer("350", 3);
            Value->LoadBuffer(buffer, val_len);
        }
        return 0;
    }
    int bytes_available = size;
    // OWNER poate avea maxim 256 octeti
    char          *ptr       = buffer;
    unsigned char owner_size = *(unsigned char *)buffer;
    buffer++;
    bytes_available -= owner_size + 7;
    if (bytes_available < 0) {
        fprintf(stderr, "Broken packet %i bytes missing (packet size: %i)\n", -bytes_available, size);
        return -1;
    }
    if (owner_size) {
        Owner->LoadBuffer(buffer, owner_size);
        buffer += owner_size;
    } else
        *Owner = (char *)"";

    *message = ntohl(*(int *)buffer);
    buffer  += sizeof(int);
    unsigned short target_size = ntohs(*(unsigned short *)buffer);
    bytes_available -= target_size;
    if (bytes_available < 0) {
        fprintf(stderr, "Sender: %s, MSG_ID: %i, Broken packet / invalid target parameter %i bytes missing (packet size: %i)\n", Owner->c_str(), *message, -bytes_available, size);
        return -1;
    }
    buffer += sizeof(short);
    if (target_size) {
        Target->LoadBuffer(buffer, target_size);
        buffer += target_size;
    } else
        *Target = (char *)"";

    long last_size = size - ((uintptr_t)buffer - (uintptr_t)ptr);
    if (last_size > 0)
        Value->LoadBuffer(buffer, last_size);
    else
        *Value = (char *)"";
    return 0;
}

//-----------------------------------------------------------------------------------
uint64_t htonll2(uint64_t a) {
    if (is_little_endian)
        a = ((a & 0x00000000000000FFULL) << 56) |
            ((a & 0x000000000000FF00ULL) << 40) |
            ((a & 0x0000000000FF0000ULL) << 24) |
            ((a & 0x00000000FF000000ULL) << 8) |
            ((a & 0x000000FF00000000ULL) >> 8) |
            ((a & 0x0000FF0000000000ULL) >> 24) |
            ((a & 0x00FF000000000000ULL) >> 40) |
            ((a & 0xFF00000000000000ULL) >> 56);
    return a;
}

void WSFrame(char *data_in, int data_len, char *out_frame, int *out_len) {
    uint64_t tmp      = data_len;
    int      size_len = 0;

    out_frame[0] = '\x82';
    out_frame++;
    if (tmp <= 0x7D) {
        *out_frame = tmp;
        size_len++;
        out_frame++;
    } else
    if (tmp < 0xFFFF) {
        *out_frame = 0x7E;
        out_frame++;
        *(unsigned short *)out_frame = htons((unsigned short)tmp);
        size_len  += 3;
        out_frame += 2;
    } else {
        *out_frame = 0x7F;
        out_frame++;
        *(uint64_t *)out_frame = htonll2(tmp);
        size_len  += 9;
        out_frame += 8;
    }
    if (out_frame != data_in)
        memmove(out_frame, data_in, data_len);
    *out_len = data_len + size_len + 1;
}

void IgnoreBytes(MetaContainer *mc, int CLIENT_SOCKET, int size, bool masked = true) {
    if (size < 0)
        return;
    unsigned char buf;
    int           received  = 0;
    int           rec_count = 0;
    if (masked)
        size += 4;
    do {
        received   = deturnated_recv(mc, CLIENT_SOCKET, (char *)&buf, 1, 0);
        rec_count += received;
    } while ((rec_count < size) && (received > 0));
}

long WSGetPacketSize(MetaContainer *mc, int CLIENT_SOCKET, int *code, int *masked) {
    int           received  = 0;
    int           rec_count = 0;
    unsigned char buf[9];

    do {
        received   = deturnated_recv(mc, CLIENT_SOCKET, (char *)buf + rec_count, 2 - rec_count, 0);
        rec_count += received;
    } while ((rec_count < 2) && (received > 0));
    if (received <= 0) {
        // connection drop
        *code = -2;
        return -1;
    }
    rec_count = 0;
    // buf[0] is packet marker
    switch (buf[0] & 0x0F) {
        case 0x0:
            // continuation frame
            *code = 0;
            break;

        case 0x1:
            // text frame;
            *code = 0;
            break;

        case 0x0D:
            // private rtc-extension frame
        case 0x02:
            // binary frame
            *code = 0;
            break;

        case 0x08:
            // connection close
            *code = -1;
            break;

        case 0x09:
            // ping
            *code = 1;
            break;

        case 0x0A:
            // pong
            *code = 2;
            break;

        default:
            // unknown message type, drop it
            *code = 2;
            break;
    }

    char s_buf = buf[1] & 0x7F;
    *masked = buf[1] & 0x80;
    switch (s_buf) {
        case 126:
            do {
                received   = deturnated_recv(mc, CLIENT_SOCKET, (char *)buf + rec_count, 2 - rec_count, 0);
                rec_count += received;
            } while ((rec_count < 2) && (received > 0));
            if (received <= 0)
                return -1;

            // ping or pong
            if (*code > 0) {
                IgnoreBytes(mc, CLIENT_SOCKET, ntohs(*(unsigned short *)buf));
                if (*code == 1) {
                    // ping, must send pong
                    char pong[6];
                    pong[0] = 0x8A;
                    pong[1] = 0x04;
                    pong[2] = 'P';
                    pong[3] = 'o';
                    pong[4] = 'n';
                    pong[5] = 'g';
                    deturnated_send(mc, CLIENT_SOCKET, pong, 6);
                }
                return -1;
            }
            return ntohs(*(unsigned short *)buf);
            break;

        case 127:
            do {
                received   = deturnated_recv(mc, CLIENT_SOCKET, (char *)buf + rec_count, 8 - rec_count, 0);
                rec_count += received;
            } while ((rec_count < 8) && (received > 0));
            if (received <= 0)
                return -1;

            // ping or pong
            if (*code > 0) {
                IgnoreBytes(mc, CLIENT_SOCKET, (long)htonll2(*(uint64_t *)buf));
                return -1;
            }
            return (long)htonll2(*(uint64_t *)buf);
            break;

        default:
            if (s_buf <= 0x7D) {
                if (*code > 0) {
                    IgnoreBytes(mc, CLIENT_SOCKET, s_buf);
                    return -1;
                }
                return s_buf;
            }
    }
    return -1;
}

long WSReceive(MetaContainer *mc, int CLIENT_SOCKET, char *buffer, int size, int masked) {
    char mask[4];
    int  received  = 0;
    int  rec_count = 0;

    if (masked) {
        do {
            received   = deturnated_recv(mc, CLIENT_SOCKET, (char *)&mask + rec_count, 4 - rec_count, 0);
            rec_count += received;
        } while ((rec_count < 4) && (received > 0));

        if (received <= 0)
            return -1;
        if (size == 0)
            return 0;
        rec_count = 0;
    }
    do {
        received   = deturnated_recv(mc, CLIENT_SOCKET, (char *)buffer + rec_count, size - rec_count, 0);
        rec_count += received;
    } while ((rec_count < size) && (received > 0));

    if (received <= 0)
        return -1;

    if (masked) {
        for (int i = 0; i < size; i++)
            buffer[i] = buffer[i] ^ mask[i % 4];
    }
    return size;
}

char *SerializeBuffer(MetaContainer *mc, char **buffer, int *size, AnsiString *Owner, int message, AnsiString *Target, AnsiString *Value, char *key, int *is_udp = NULL, int *remaining = NULL) {
    // OWNER poate avea maxim 256 octeti
    int vlen = Value->Length();
    int tlen = Target->Length();

    if (ref_isWebSocket) {
        if ((message == 0x110) && (!tlen)) {
            if (vlen > 0) {
                unsigned int owner = Owner->ToInt();
                if ((is_udp) && (mc->RTSOCKET > 0) && (mc->RTCONFIRMED) && (mc->rt_send_enabled) && (mc->remote_conceptudpaddr.ss_family) && (owner <= 0xFFFF) && (owner >= 0)) {
                    *is_udp = 1;
                    int offset = 0;
                    if (remaining)
                        offset = *remaining;
                    int vsize2 = vlen - offset;
                    if (vsize2 < 0)
                        vsize2 = 0;
                    if (vsize2 > MAX_RT_SIZE) {
                        vsize2 = MAX_RT_SIZE;

                        if (remaining)
                            *remaining += vsize2;
                    } else {
                        if (remaining)
                            *remaining = 0;
                    }
                    *size = vsize2 + sizeof(short);
                    char *ptr = new char[*size];
                    *(unsigned short *)ptr = htons((unsigned short)owner);
                    char *buf = ptr + sizeof(short);
                    memcpy(buf, Value->c_str() + offset, vsize2);
                    *buffer = ptr;
                    return ptr;
                } else {
                    message = 0x1001;
                    *Target = (char *)"350";
                    tlen    = Target->Length();
                }
            } else {
                message = 0x1001;
                *Target = (char *)"350";
                tlen    = Target->Length();
            }
        }
        if (remaining)
            *remaining = 0;
        *size = sizeof(char) + Owner->Length() + sizeof(int) + sizeof(short) + tlen + vlen;
        //  8 bits packet type + max 64 bits size
        char *ptr = new char[*size + 10];

        //*(int *)ptr=htonl(*size);
        char *buf = ptr + 10;

        *(unsigned char *)buf = (unsigned char)Owner->Length();
        buf += sizeof(char);

        memcpy(buf, Owner->c_str(), Owner->Length());
        buf += Owner->Length();

        *(int *)buf = htonl(message);
        buf        += sizeof(int);

        *(unsigned short *)buf = htons((unsigned short)tlen);
        buf += sizeof(short);

        memcpy(buf, Target->c_str(), tlen);
        buf += Target->Length();

        memcpy(buf, Value->c_str(), vlen);

        WSFrame(ptr + 10, *size, ptr, size);
        *buffer = ptr;
        return ptr;
    } else {
        if ((message == 0x110) && (!tlen)) {
            if ((!key) && (vlen) && (vlen <= MAX_RT_SIZE)) {
                unsigned int owner = Owner->ToInt();
                if (owner <= 0xFFF) {
                    // max udp size
                    if ((is_udp) && (vlen <= 0xFFFF)) {
                        if ((is_udp) && (mc->RTSOCKET > 0) && (mc->RTCONFIRMED) && (mc->rt_send_enabled) && (mc->remote_conceptudpaddr.ss_family))
                            *is_udp = 1;
                    }
                    *size = vlen;
                    char *ptr = new char[*size + sizeof(int)];
                    *(unsigned int *)ptr = htonl(((unsigned int)*size) | (unsigned int)0xF0000000 | (owner << 16));
                    char *buf = ptr + sizeof(int);
                    *buffer = buf;
                    memcpy(buf, Value->c_str(), vlen);
                    return ptr;
                } else {
                    Target->LoadBuffer("1003", 4);
                    tlen = 4;
                }
            } else {
                Target->LoadBuffer("1003", 4);
                tlen = 4;
            }
        }

        *size = sizeof(char) + Owner->Length() + sizeof(int) + sizeof(short) + tlen + vlen;
        char *ptr = new char[*size + sizeof(int)];
        *(int *)ptr = htonl(*size);
        char *buf = ptr + sizeof(int);//new char[*size];
        *buffer = buf;

        *(unsigned char *)buf = (unsigned char)Owner->Length();
        buf += sizeof(char);

        memcpy(buf, Owner->c_str(), Owner->Length());
        buf += Owner->Length();

        *(int *)buf = htonl(message);
        buf        += sizeof(int);

        *(unsigned short *)buf = htons((unsigned short)tlen);
        buf += sizeof(short);

        memcpy(buf, Target->c_str(), tlen);
        buf += Target->Length();

        memcpy(buf, Value->c_str(), vlen);
        return ptr;
    }
}

//-----------------------------------------------------------------------------------
unsigned int AES_encrypt(MetaContainer *mc, char *in_buffer, unsigned int in_buffer_size, char *out_buffer, unsigned int out_buffer_size, char *key, unsigned int key_size, bool cbc = false) {
#ifdef __WITH_INTEL_LIB
    if (hardware_aes) {
        unsigned char buf[32];
        int           blocks;
        int           remainder;

        out_buffer[0] = 'x';
        out_buffer++;
        *(AES_int32 *)out_buffer = htonl(in_buffer_size);
        out_buffer += sizeof(AES_int32);
        blocks = in_buffer_size / key_size;

        switch (key_size) {
            case 16:
                if (blocks) {
                    if (cbc)
                        intel_AES_enc128_CBC((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks, mc->out_init_vector);
                    else
                        intel_AES_enc128((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks);
                }
                remainder = in_buffer_size % key_size;
                if (remainder) {
                    memset(buf, 0, key_size);
                    memcpy(buf, in_buffer + blocks * key_size, remainder);
                    if (cbc)
                        intel_AES_enc128_CBC(buf, (unsigned char *)out_buffer + blocks * key_size, (unsigned char *)key, 1, mc->out_init_vector);
                    else
                        intel_AES_enc128(buf, (unsigned char *)out_buffer + blocks * key_size, (unsigned char *)key, 1);
                    blocks++;
                }
                return blocks * key_size + sizeof(AES_int32) + 1;

            case 24:
                if (blocks) {
                    if (cbc)
                        intel_AES_enc192_CBC((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks, mc->out_init_vector);
                    else
                        intel_AES_enc192((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks);
                }
                remainder = in_buffer_size % key_size;
                if (remainder) {
                    memset(buf, 0, key_size);
                    memcpy(buf, in_buffer + blocks * key_size, remainder);
                    if (cbc)
                        intel_AES_enc192_CBC(buf, (unsigned char *)out_buffer + blocks * key_size, (unsigned char *)key, 1, mc->out_init_vector);
                    else
                        intel_AES_enc192(buf, (unsigned char *)out_buffer + blocks * key_size, (unsigned char *)key, 1);
                    blocks++;
                }
                return blocks * key_size + sizeof(AES_int32) + 1;

            case 32:
                if (blocks) {
                    if (cbc)
                        intel_AES_enc256_CBC((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks, mc->out_init_vector);
                    else
                        intel_AES_enc256((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks);
                }
                remainder = in_buffer_size % key_size;
                if (remainder) {
                    memset(buf, 0, key_size);
                    memcpy(buf, in_buffer + blocks * key_size, remainder);
                    if (cbc)
                        intel_AES_enc256_CBC(buf, (unsigned char *)out_buffer + blocks * key_size, (unsigned char *)key, 1, mc->out_init_vector);
                    else
                        intel_AES_enc256(buf, (unsigned char *)out_buffer + blocks * key_size, (unsigned char *)key, 1);
                    blocks++;
                }
                return blocks * key_size + sizeof(AES_int32) + 1;
        }
    }
#endif

    if (!mc->En_inited)
        mc->En_inited = encrypt_init(&mc->EncryptAes, key, key_size);

    return encrypt(&mc->EncryptAes, in_buffer, in_buffer_size, out_buffer, out_buffer_size, cbc ? AES::CBC : AES::ECB);
}

//-----------------------------------------------------------------------------------
unsigned int AES_decrypt(MetaContainer *mc, char *in_buffer, unsigned int in_buffer_size, char *out_buffer, unsigned int out_buffer_size, char *key, unsigned int key_size, bool cbc = false) {
    if (in_buffer_size < key_size + 5)
        return 0;

    if (*in_buffer != 'x')
        return 0;
#ifdef __WITH_INTEL_LIB
    if (hardware_aes) {
        in_buffer++;
        AES_int32 size = ntohl(*(AES_int32 *)in_buffer);
        if (size > in_buffer_size - 5)
            return 0;
        in_buffer      += sizeof(AES_int32);
        in_buffer_size -= sizeof(AES_int32) + 1;
        int blocks = in_buffer_size / key_size;
        if (!blocks)
            return 0;

        switch (key_size) {
            case 16:
                if (cbc)
                    intel_AES_dec128_CBC((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks, mc->in_init_vector);
                else
                    intel_AES_dec128((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks);
                return size;

            case 24:
                if (cbc)
                    intel_AES_dec192_CBC((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks, mc->in_init_vector);
                else
                    intel_AES_dec192((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks);
                return size;

            case 32:
                if (cbc)
                    intel_AES_dec256_CBC((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks, mc->in_init_vector);
                else
                    intel_AES_dec256((unsigned char *)in_buffer, (unsigned char *)out_buffer, (unsigned char *)key, blocks);
                return size;
        }
    }
#endif
    if (!mc->Dec_inited)
        mc->Dec_inited = decrypt_init(&mc->DecryptAes, key, key_size);

    return decrypt(&mc->DecryptAes, in_buffer, in_buffer_size, out_buffer, out_buffer_size, cbc ? AES::CBC : AES::ECB);
}

//-----------------------------------------------------------------------------------
int UpdateTLSSocket(MetaContainer *mc, SYSTEM_SOCKET socket) {
#ifndef NOSSL
    if ((mc->sslctx) && (mc->ssl)) {
        SSL_shutdown(mc->ssl);
        SSL_free(mc->ssl);

        mc->ssl = SSL_new(mc->sslctx);

        SSL_set_fd(mc->ssl, socket);
        int res = SSL_accept(mc->ssl);
        if (res < 0) {
            SSL_shutdown(mc->ssl);
            SSL_free(mc->ssl);
            mc->ssl = NULL;
        } else {
            return 1;
        }
    }
#endif
    return 0;
}

#ifndef NOSSL

/*
   static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

   void decodeblock( unsigned char in[4], unsigned char out[3] ) {
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
   }

   int decode(char *in_buffer, int in_buffer_size, char *out_buffer ) {
    unsigned char in[4], out[3], v;
    int i, len;

    char *ptr=in_buffer;
    char *out_ptr=out_buffer;

    while ( ptr <= in_buffer+in_buffer_size ) {
        for( len = 0, i = 0; i < 4 && ( ptr <= in_buffer+in_buffer_size ); i++ ) {
            v = 0;
            while( ( ptr <= in_buffer+in_buffer_size ) && v == 0 ) {
                v = (unsigned char) ptr[0];
                ptr++;
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
                if( v ) {
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
            }
            if ( ptr <= in_buffer+in_buffer_size ) {
                len++;
                if( v ) {
                    in[ i ] = (unsigned char) (v - 1);
                }
            }
            else {
                in[i] = 0;
            }
        }
        if( len ) {
            decodeblock( in, out );
            for( i = 0; i < len - 1; i++ ) {
                out_ptr[0]=out[i];
                out_ptr++;
            }
        }
    }
    return (intptr_t)out_ptr-(intptr_t)out_buffer;
   }

   //------------------------------------------------------------------------
   #define SERIAL_POP(size, out, type, val)        if (size >= sizeof(type)) { val = *((type *)out); out+=sizeof(type); size-=sizeof(type); }
   #define SERIAL_POP_STR(size, out, val, len)     if ((size >= len) && (len > 0)) {\
                                                    if (!val)\
                                                        val = (unsigned char *)OPENSSL_malloc(len);\
                                                    memcpy(val, out, len);\
                                                    out+=len;\
                                                    size-=len;\
                                                }
   #define SERIAL_POP_STR3(size, out, val, len)   if ((size >= len) && (len > 0)) {\
                                                    if (!val)\
                                                        val = (char *)OPENSSL_malloc(len);\
                                                    memcpy(val, out, len);\
                                                    out+=len;\
                                                    size-=len;\
                                                }
   #define SERIAL_POP_STR2(size, out, val, len)    if ((size >= len) && (len > 0)) { memcpy(val, out, len); out+=len; size-=len; }

   char *UnserializeBIO(int &size, char *out, BIO **b) {
    if (size < 1)
        return out;
    char method = out[0];
    out++;
    size--;
 * b = NULL;

    switch (method) {
        case 1:
 * b = BIO_new(BIO_s_mem());
            break;
        case 2:
 * b = BIO_new(BIO_s_socket());
            break;
        case 3:
 * b = BIO_new(BIO_s_connect());
            break;
        case 4:
 * b = BIO_new(BIO_s_accept());
            break;
        case 5:
 * b = BIO_new(BIO_s_fd());
            break;
        case 6:
 * b = BIO_new(BIO_s_bio());
            break;
        case 7:
 * b = BIO_new(BIO_s_null());
            break;
        case 8:
 * b = BIO_new(BIO_f_null());
            break;
        case 9:
 * b = BIO_new(BIO_f_buffer());
            break;
        case 10:
 * b = BIO_new(BIO_f_nbio_test());
            break;
        case 11:
 * b = BIO_new(BIO_s_datagram());
            break;
    }
    if (*b) {
        SERIAL_POP(size, out, int, (*b)->init);
        SERIAL_POP(size, out, int, (*b)->shutdown);
        SERIAL_POP(size, out, int, (*b)->flags);
        SERIAL_POP(size, out, int, (*b)->num);
        SERIAL_POP(size, out, int, (*b)->retry_reason);
        SERIAL_POP(size, out, int, (*b)->num_read);
        SERIAL_POP(size, out, int, (*b)->num_write);
    }
    return out;
   }
   //------------------------------------------------------------------------
   SSL *SSLUnserialize(SSL_CTX *sslctx, char *out, int size, int sock) {
    if (sslctx)  {
        SSL *ssl = SSL_new(sslctx);
        memset(ssl, 0, sizeof(SSL));
        if (sock > -1)
            SSL_set_fd(ssl, sock);
        ssl->references = 1;
        ssl->ctx = sslctx;

        char method_index = out[0];
        out++;
        size--;

        // == pana aici == //
 **#ifndef OPENSSL_NO_SSL2
        if (method_index == 1) {
            ssl->method = SSLv2_method();
        } else
        if (method_index == 2) {
            ssl->method = SSLv2_server_method();
        } else
        if (method_index == 3) {
            ssl->method = SSLv2_client_method();
        } else
 **#endif
 **#ifndef OPENSSL_NO_SSL3
        if (method_index == 4) {
            ssl->method = SSLv3_method();
        } else
        if (method_index == 5) {
            ssl->method = SSLv3_server_method();
        } else
        if (method_index == 6) {
            ssl->method = SSLv3_client_method();
        } else
 **#endif
        if (method_index == 7) {
            ssl->method = SSLv23_method();
        } else
        if (method_index == 8) {
            ssl->method = SSLv23_server_method();
        } else
        if (method_index == 9) {
            ssl->method = SSLv23_client_method();
        } else
 **#ifndef OPENSSL_NO_TLS1
        if (method_index == 10) {
            ssl->method = TLSv1_method();
        } else
        if (method_index == 11) {
            ssl->method = TLSv1_server_method();
        } else
        if (method_index == 12) {
            ssl->method = TLSv1_client_method();
        } else
 **#endif
        if (method_index == 13) {
            ssl->method = TLSv1_1_method();
        } else
        if (method_index == 14) {
            ssl->method = TLSv1_1_server_method();
        } else
        if (method_index == 15) {
            ssl->method = TLSv1_1_client_method();
        } else
        if (method_index == 16) {
            ssl->method = TLSv1_2_method();
        } else
        if (method_index == 17) {
            ssl->method = TLSv1_2_server_method();
        } else
        if (method_index == 18) {
            ssl->method = TLSv1_2_client_method();
        } else
        if (method_index == 19) {
            ssl->method = DTLSv1_method();
        } else
        if (method_index == 20) {
            ssl->method = DTLSv1_server_method();
        } else
        if (method_index == 21) {
            ssl->method = DTLSv1_client_method();
        }

        if (ssl->method)
            ssl->handshake_func = ssl->method->ssl_accept;

        SERIAL_POP(size, out, int, ssl->version);
        SERIAL_POP(size, out, int, ssl->type);
        SERIAL_POP(size, out, int, ssl->rwstate);
        SERIAL_POP(size, out, int, ssl->in_handshake);
        SERIAL_POP(size, out, int, ssl->server);
        SERIAL_POP(size, out, int, ssl->new_session);
        SERIAL_POP(size, out, int, ssl->quiet_shutdown);
        SERIAL_POP(size, out, int, ssl->shutdown);
        SERIAL_POP(size, out, int, ssl->state);
        SERIAL_POP(size, out, int, ssl->rstate);
        SERIAL_POP(size, out, int, ssl->init_num);
        SERIAL_POP(size, out, int, ssl->init_off);
        SERIAL_POP(size, out, int, ssl->packet_length);
        SERIAL_POP_STR(size, out, ssl->packet, ssl->packet_length);
        SERIAL_POP(size, out, int, ssl->read_ahead);
        SERIAL_POP(size, out, int, ssl->mac_flags);
        SERIAL_POP(size, out, int, ssl->hit);
        SERIAL_POP(size, out, int, ssl->sid_ctx_length);
        SERIAL_POP_STR2(size, out, ssl->sid_ctx, ssl->sid_ctx_length);
        SERIAL_POP(size, out, int, ssl->verify_mode);
        SERIAL_POP(size, out, int, ssl->error);
        SERIAL_POP(size, out, int, ssl->error_code);
        SERIAL_POP(size, out, int, ssl->debug);
        SERIAL_POP(size, out, long, ssl->verify_result);
        SERIAL_POP(size, out, unsigned long, ssl->options);
        SERIAL_POP(size, out, unsigned long, ssl->mode);
        SERIAL_POP(size, out, long, ssl->max_cert_list);
        SERIAL_POP(size, out, int, ssl->first_packet);
        SERIAL_POP(size, out, int, ssl->client_version);
        SERIAL_POP(size, out, unsigned int, ssl->max_send_fragment);

        unsigned short host_len = 0;
        SERIAL_POP(size, out, unsigned short, host_len);
        SERIAL_POP_STR3(size, out, ssl->tlsext_hostname, host_len);
        SERIAL_POP(size, out, int, ssl->servername_done);
        SERIAL_POP(size, out, int, ssl->tlsext_status_type);
        SERIAL_POP(size, out, int, ssl->tlsext_status_expected);
        SERIAL_POP(size, out, int, ssl->tlsext_ocsp_resplen);
        SERIAL_POP_STR(size, out, ssl->tlsext_ocsp_resp, ssl->tlsext_ocsp_resplen);

        SERIAL_POP(size, out, size_t, ssl->tlsext_ecpointformatlist_length);
        SERIAL_POP_STR(size, out, ssl->tlsext_ecpointformatlist, ssl->tlsext_ecpointformatlist_length);
        SERIAL_POP(size, out, size_t, ssl->tlsext_ellipticcurvelist_length);
        SERIAL_POP_STR(size, out, ssl->tlsext_ellipticcurvelist, ssl->tlsext_ellipticcurvelist_length);

        SERIAL_POP(size, out, size_t, ssl->tlsext_opaque_prf_input_len);
        SERIAL_POP_STR(size, out, ssl->tlsext_opaque_prf_input, ssl->tlsext_opaque_prf_input_len);

        SERIAL_POP(size, out, int, ssl->next_proto_negotiated_len);
        SERIAL_POP_STR(size, out, ssl->next_proto_negotiated, ssl->next_proto_negotiated_len);

        SERIAL_POP(size, out, int, ssl->tlsext_heartbeat);
        SERIAL_POP(size, out, int, ssl->tlsext_hb_pending);
        SERIAL_POP(size, out, int, ssl->tlsext_hb_seq);

        int session_size = 0;
        SERIAL_POP(size, out, int, session_size);
        if ((session_size > 0) && (size >= session_size)) {
            const unsigned char *ptr=(unsigned char *)out;
            ssl->session = d2i_SSL_SESSION((SSL_SESSION **) 0, &ptr, session_size);
            size -= session_size;
            out += session_size;
        }
        out = UnserializeBIO(size, out, &ssl->rbio);
        out = UnserializeBIO(size, out, &ssl->wbio);
        out = UnserializeBIO(size, out, &ssl->bbio);

        if (out[0] == 3) {
            out++;
            size++;
            ssl->s3 = (SSL3_STATE *)OPENSSL_malloc(sizeof(SSL3_STATE));
            memset(ssl->s3, 0, sizeof(SSL3_STATE));
            SERIAL_POP(size, out, long, ssl->s3->flags);
            SERIAL_POP(size, out, int, ssl->s3->delay_buf_pop_ret);
            SERIAL_POP_STR2(size, out, ssl->s3->read_sequence, sizeof(ssl->s3->read_sequence));
            SERIAL_POP(size, out, int, ssl->s3->read_mac_secret_size);
            SERIAL_POP_STR2(size, out, ssl->s3->read_mac_secret, ssl->s3->read_mac_secret_size);
            SERIAL_POP_STR2(size, out, ssl->s3->write_sequence, sizeof(ssl->s3->write_sequence));
            SERIAL_POP(size, out, int, ssl->s3->write_mac_secret_size);
            SERIAL_POP_STR2(size, out, ssl->s3->write_mac_secret, ssl->s3->write_mac_secret_size);
            SERIAL_POP_STR2(size, out, ssl->s3->server_random, SSL3_RANDOM_SIZE);
            SERIAL_POP_STR2(size, out, ssl->s3->client_random, SSL3_RANDOM_SIZE);

            SERIAL_POP(size, out, int, ssl->s3->need_empty_fragments);
            SERIAL_POP(size, out, int, ssl->s3->empty_fragment_done);
            SERIAL_POP(size, out, int, ssl->s3->init_extra);

            SERIAL_POP(size, out, int, ssl->s3->rrec.type);
            SERIAL_POP(size, out, int, ssl->s3->wrec.type);

            SERIAL_POP(size, out, int, ssl->s3->alert_fragment_len);
            SERIAL_POP_STR2(size, out, ssl->s3->alert_fragment, ssl->s3->alert_fragment_len);

            SERIAL_POP(size, out, int, ssl->s3->handshake_fragment_len);
            SERIAL_POP_STR2(size, out, ssl->s3->handshake_fragment, ssl->s3->handshake_fragment_len);

            SERIAL_POP(size, out, unsigned int, ssl->s3->wnum);
            SERIAL_POP(size, out, int, ssl->s3->wpend_tot);
            SERIAL_POP(size, out, int, ssl->s3->wpend_type);
            SERIAL_POP(size, out, int, ssl->s3->wpend_ret);
            out = UnserializeBIO(size, out, &ssl->s3->handshake_buffer);
            int nid = -1;
            do {
                SERIAL_POP(size, out, int, nid);
                //EVP_MD_CTX *ctx = ssl->s3->handshake_dgst[i];
            } while (nid >= 0);
            SERIAL_POP(size, out, int, ssl->s3->change_cipher_spec);
            SERIAL_POP(size, out, int, ssl->s3->warn_alert);
            SERIAL_POP(size, out, int, ssl->s3->fatal_alert);
            SERIAL_POP(size, out, int, ssl->s3->alert_dispatch);
            SERIAL_POP_STR2(size, out, ssl->s3->send_alert, sizeof(ssl->s3->send_alert));

            SERIAL_POP(size, out, int, ssl->s3->renegotiate);
            SERIAL_POP(size, out, int, ssl->s3->total_renegotiations);
            SERIAL_POP(size, out, int, ssl->s3->num_renegotiations);
            SERIAL_POP(size, out, int, ssl->s3->in_read_app_data);

            SERIAL_POP_STR2(size, out, ssl->s3->tmp.cert_verify_md, sizeof(ssl->s3->tmp.cert_verify_md));
            SERIAL_POP(size, out, int, ssl->s3->tmp.finish_md_len);
            SERIAL_POP_STR2(size, out, ssl->s3->tmp.finish_md, ssl->s3->tmp.finish_md_len);
            SERIAL_POP(size, out, int, ssl->s3->tmp.peer_finish_md_len);
            SERIAL_POP_STR2(size, out, ssl->s3->tmp.peer_finish_md, ssl->s3->tmp.peer_finish_md_len);

            SERIAL_POP(size, out, unsigned long, ssl->s3->tmp.message_size);
            SERIAL_POP(size, out, int, ssl->s3->tmp.message_type);

            int c_len = 0;
            SERIAL_POP(size, out, unsigned short, c_len);
            if (c_len) {
                unsigned char *cipher_name = (unsigned char *)malloc(c_len + 1);
                cipher_name[0] = 0;
                cipher_name[c_len] = 0;
                SERIAL_POP_STR(size, out, cipher_name, c_len);
                // search cipher
                // not implemented
                //fprintf(stderr, "CIPHER: %s\n", (int)cipher_name);
                free(cipher_name);
            }

            SERIAL_POP(size, out, int, ssl->s3->tmp.next_state);
            SERIAL_POP(size, out, int, ssl->s3->tmp.reuse_message);
            SERIAL_POP(size, out, int, ssl->s3->tmp.cert_req);
            SERIAL_POP(size, out, int, ssl->s3->tmp.ctype_num);
            SERIAL_POP_STR2(size, out, ssl->s3->tmp.ctype, sizeof(ssl->s3->tmp.ctype));

            SERIAL_POP(size, out, int, ssl->s3->tmp.key_block_length);
            SERIAL_POP_STR(size, out, ssl->s3->tmp.key_block, ssl->s3->tmp.key_block_length);
            nid = -1;
            SERIAL_POP(size, out, int, nid);
            nid = -1;
            SERIAL_POP(size, out, int, nid);
            SERIAL_POP(size, out, int, ssl->s3->tmp.new_mac_pkey_type);
            SERIAL_POP(size, out, int, ssl->s3->tmp.new_mac_secret_size);

            SERIAL_POP(size, out, int, ssl->s3->tmp.cert_request);
            SERIAL_POP(size, out, char, ssl->s3->previous_client_finished_len);
            SERIAL_POP_STR2(size, out, ssl->s3->previous_client_finished, ssl->s3->previous_client_finished_len);
            SERIAL_POP(size, out, char, ssl->s3->previous_server_finished_len);
            SERIAL_POP_STR2(size, out, ssl->s3->previous_server_finished, ssl->s3->previous_server_finished_len);
            SERIAL_POP(size, out, int, ssl->s3->send_connection_binding);
            SERIAL_POP(size, out, int, ssl->s3->next_proto_neg_seen);
            SERIAL_POP(size, out, char, ssl->s3->is_probably_safari);
        } else {
            out++;
            size++;
        }
        return ssl;
    }
    return 0;
   }
   //------------------------------------------------------------------------
   int RestoreSession(MetaContainer *mc, SYSTEM_SOCKET CLIENT_SOCKET, char *data) {
    static char *magic_key = "----SSL DATA----";
    static int magic_key_len = 16;

    if ((!data) || (!data[0]))
        return 0;

    int len = strlen(data);
    if (len <= magic_key_len)
        return 0;

    char *mime_data = strstr(data, magic_key);
    if (mime_data)
        mime_data += magic_key_len;
    if ((!mime_data) || (!mime_data[0]))
        return 0;

    len = strlen(mime_data);
    char *ssl_state = (char *)malloc(len + 1);
    if (!ssl_state)
        return 0;
    ssl_state[0] = 0;
    len = decode(mime_data, len, ssl_state);
    if (len < 0) {
        free(ssl_state);
        return 0;
    }

    if (!mc->sslctx) {
        SSL_load_error_strings();
        SSL_library_init();
        OpenSSL_add_all_algorithms();
    }

    if (mc->ssl) {
        SSL_shutdown(mc->ssl);
        SSL_free(mc->ssl);
        mc->ssl=NULL;
    }
    if (mc->sslctx)
        SSL_CTX_free(mc->sslctx);

    mc->sslctx = SSL_CTX_new(SSLv23_server_method());
    if (mc->sslctx)
        mc->ssl = SSLUnserialize(mc->sslctx, ssl_state, len, CLIENT_SOCKET);
    free(ssl_state);
    return 0;
   }
 */
int SetSSL(MetaContainer *mc, SYSTEM_SOCKET CLIENT_SOCKET, char *cert_file, char *priv_file, char *trustfile, char *trustpath) {
    if (!mc->sslctx) {
        SSL_load_error_strings();
        SSL_library_init();
        OpenSSL_add_all_algorithms();
    }

    if (mc->ssl) {
        SSL_shutdown(mc->ssl);
        SSL_free(mc->ssl);
        mc->ssl = NULL;
    }
    if (mc->sslctx)
        SSL_CTX_free(mc->sslctx);

    mc->sslctx = SSL_CTX_new(SSLv3_server_method());
    if (mc->sslctx) {
#ifdef WITH_OPENSSL
        SSL_CTX_set_options(mc->sslctx, SSL_OP_SINGLE_DH_USE);
        if ((trustfile) || (trustpath))
            SSL_CTX_load_verify_locations(mc->sslctx, trustfile, trustpath);
#endif
        int res = 0;
        if ((SSL_CTX_use_certificate_file(mc->sslctx, cert_file, SSL_FILETYPE_PEM)) && (SSL_CTX_use_PrivateKey_file(mc->sslctx, priv_file, SSL_FILETYPE_PEM)) && (SSL_CTX_check_private_key(mc->sslctx))) {
            mc->ssl = SSL_new(mc->sslctx);

            int res2 = SSL_set_fd(mc->ssl, CLIENT_SOCKET);
            int res  = SSL_accept(mc->ssl);
            if (res < 0) {
                ERR_print_errors_fp(stderr);
                SSL_shutdown(mc->ssl);
                SSL_free(mc->ssl);
                mc->ssl = NULL;
            } else {
                return 1;
            }
        } else
            return -3;
    }
    return 0;
}
#endif
//-----------------------------------------------------------------------------------
void send_message_function(char *Owner, char *Target, int message, char *Value, int len, int CLIENT_SOCKET, void *handler) {
    AnsiString val;

    val.LoadBuffer(Value, len);
    GET_METACONTAINER2
    send_message2(mc, Owner, Target, message, Value, (char *)NULL, CLIENT_SOCKET);
}

#ifdef __GNUC__
static void __cpuid(unsigned int *CPUInfo, int func) {
    __asm__ __volatile__
    (
        "cpuid"
        : "=a" (CPUInfo[0])
        , "=b" (CPUInfo[1])
        , "=c" (CPUInfo[2])
        , "=d" (CPUInfo[3])
        : "a"  (func)
    );
}
#endif

#ifdef __WITH_INTEL_LIB
static int CheckCPUsupportAES() {
    unsigned int CPUInfo[4];

    __cpuid(CPUInfo, 1);
    return CPUInfo[2] & (1 << 25);
}
#endif

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    LocalInvoker = Invoke;
    Invoke(INVOKE_SETSENDMESSAGEFUNCTION, HANDLER, send_message_function);
#ifdef __WITH_INTEL_LIB
    hardware_aes = CheckCPUsupportAES();
#endif
    short int number  = 0x1;
    char      *numPtr = (char *)&number;
    if (numPtr[0] != 1)
        is_little_endian = 0;

    char *wsproto = getenv("CONCEPT_WSPROTO");
    if (wsproto)
        ref_isWebSocket = atoi(wsproto);
    else
        ref_isWebSocket = 0;
#ifndef NOSSL
#ifdef WITH_OPENSSL
    thread_setup();
#endif
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifndef NOSSL
#ifdef WITH_OPENSSL
    thread_cleanup();
#endif
#endif
#ifndef _WIN32
    return (void *)"No unload";
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_GetAPID CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "GetAPID doesn't take any parameters");
    LOCAL_INIT;

    int pipe_in     = 0;
    int pipe_out    = 0;
    int apid        = 0;
    int parent_apid = 0;
    Invoke(INVOKE_GET_APPLICATION_INFO, PARAMETERS->HANDLER, &pipe_in, &pipe_out, &apid, &parent_apid);

    RETURN_NUMBER(apid);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_GetParentAPID CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "GetParentAPID doesn't take any parameters");
    LOCAL_INIT;

    int pipe_in     = 0;
    int pipe_out    = 0;
    int apid        = 0;
    int parent_apid = 0;
    Invoke(INVOKE_GET_APPLICATION_INFO, PARAMETERS->HANDLER, &pipe_in, &pipe_out, &apid, &parent_apid);

    RETURN_NUMBER(parent_apid);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_PeekAPMessage CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "PeekAPMessage doesn't take any parameters");
    LOCAL_INIT;

    int pipe_in     = 0;
    int pipe_out    = 0;
    int apid        = 0;
    int parent_apid = 0;
    Invoke(INVOKE_GET_APPLICATION_INFO, PARAMETERS->HANDLER, &pipe_in, &pipe_out, &apid, &parent_apid);

    if (pipe_in <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }

    int res = eof(pipe_in);
    // nothing to read
    if (res < 0)
        res = 1;
    RETURN_NUMBER(!res);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_GetAPMessage CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "GetAPMessage parameters error: number GetAPMessage(var SenderAPID, var nMSG_ID, var szData)");
    LOCAL_INIT;

    CHECK_STATIC_PARAM(0, "GetAPMessage: parameter 0 should be static (number or string)");
    CHECK_STATIC_PARAM(1, "GetAPMessage: parameter 1 should be static (number or string)");
    CHECK_STATIC_PARAM(2, "GetAPMessage: parameter 2 should be static (number or string)");

    int pipe_in     = 0;
    int pipe_out    = 0;
    int apid        = 0;
    int parent_apid = 0;
    Invoke(INVOKE_GET_APPLICATION_INFO, PARAMETERS->HANDLER, &pipe_in, &pipe_out, &apid, &parent_apid);

    if (pipe_in <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }

    int params[3];
    int total = deturnated_read(pipe_in, (void *)params, sizeof(int) * 3);
    if (total != sizeof(int) * 3) {
        RETURN_NUMBER(0);
        return 0;
    }

    char *data = 0;
    if (params[2] > 0) {
        data   = new char[params[2]];
        total += deturnated_read(pipe_in, (void *)data, params[2]);
    }

    SET_NUMBER(0, params[0]);
    SET_NUMBER(1, params[1]);

    if (data) {
        SET_BUFFER(2, data, params[2]);
        delete[] data;
    } else {
        SET_STRING(2, "");
    }

    RETURN_NUMBER(total == sizeof(int) * 3 + params[2]);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SendAPMessage CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "SendAPMessage parameters error: number SendAPMessage(nToAppID, nMSG_ID, szData)");
    LOCAL_INIT;

    int pipe_in     = 0;
    int pipe_out    = 0;
    int apid        = 0;
    int parent_apid = 0;
    Invoke(INVOKE_GET_APPLICATION_INFO, PARAMETERS->HANDLER, &pipe_in, &pipe_out, &apid, &parent_apid);

    if (apid <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }

    NUMBER nMSG_ID;
    NUMBER nTo;
    char   *szData;
    NUMBER len;

    GET_CHECK_NUMBER(0, nTo, "SendAPMessage: parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, nMSG_ID, "SendAPMessage: parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_BUFFER(2, szData, len, "SendAPMessage : parameter 2 should be a string (STATIC STRING)");

    GET_METACONTAINER
    if ((int)nTo == apid) {
        TParameters *MESSAGE = new TParameters;
        MESSAGE->Sender = (long)nTo;
        MESSAGE->ID     = -0x100;
        MESSAGE->Target = (long)nMSG_ID;
        if (len)
            MESSAGE->Value.LoadBuffer(szData, (int)len);

        semp(mc->sem_bufferlist);
        mc->BufferedMessages.Add(MESSAGE, DATA_MESSAGE);
        semv(mc->sem_bufferlist);

        RETURN_NUMBER(1);
        return 0;
    }

    if (pipe_out <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }

    int params[3];
    params[0] = (int)nTo;
    params[1] = (int)nMSG_ID;
    params[2] = (int)len;


    if ((params[0] < 0) && (params[0] != -1)) {
        RETURN_NUMBER(0);
        return 0;
    }

    /*int total=threaded_write(pipe_out,(void *)params, sizeof(int)*3);
       if (params[2]>0) {
        total+=threaded_write(pipe_out,(void *)szData, params[2]);
       }*/

    int total = write(pipe_out, (void *)params, sizeof(int) * 3);
    if (params[2] > 0) {
        total += write(pipe_out, (void *)szData, params[2]);
    }

    RETURN_NUMBER(total == sizeof(int) * 3 + params[2]);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_CreateDebugPipe CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)"CreateDebugPipe it's a reserved function. You should not use this function.";

    GET_METACONTAINER
    if (mc->has_debug) {
        close(mc->DebugPipes[0]);
        close(mc->DebugPipes[1]);
        mc->has_debug = 0;
    }

    if (pipe(mc->DebugPipes) == -1) {
        SetVariable(RESULT, VARIABLE_NUMBER, "", -1);
        return 0;
    }
    write(mc->DebugPipes[1], (char *)&mc->DebugPipes[1], sizeof(int));
    mc->has_debug = 0;
    SetVariable(RESULT, VARIABLE_NUMBER, "", mc->DebugPipes[0]);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_DestroyDebugPipe CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)"DestroyDebugPipe it's a reserved function. You should not use this function.";
    GET_METACONTAINER
    if (mc->has_debug) {
        close(mc->DebugPipes[0]);
        close(mc->DebugPipes[1]);
        mc->has_debug = 0;
        SetVariable(RESULT, VARIABLE_NUMBER, "", 1);
    } else
        SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_BeginUpdateSession CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "BeginUpdateSession takes no parameters");
    GET_METACONTAINER
    mc->is_cached++;
    RETURN_NUMBER(0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_EndUpdateSession CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "EndUpdateSession takes no parameters");
    GET_METACONTAINER
    if (mc->is_cached)
        mc->is_cached--;
    if (!mc->is_cached) {
        RETURN_NUMBER(FlushCache(mc, CLIENT_SOCKET));
    } else {
        RETURN_NUMBER(0);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_EndUpdateSession2 CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "EndUpdateSession2 takes no parameters");
    GET_METACONTAINER
    if (mc->is_cached)
        mc->is_cached = 0;
    if (!mc->is_cached) {
        RETURN_NUMBER(FlushCache(mc, CLIENT_SOCKET));
    } else {
        RETURN_NUMBER(0);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
void send_message2(MetaContainer *mc, AnsiString Owner, AnsiString Target, int message, AnsiString message_data, char *REMOTE_PUBLIC_KEY, int CLIENT_SOCKET) {
    char *buffer         = 0;
    int  in_content_size = 0;

    SerializeBuffer(mc, &buffer, &in_content_size, &Owner, message, &Target, &message_data, REMOTE_PUBLIC_KEY);
    if (ref_isWebSocket) {
        deturnated_send(mc, CLIENT_SOCKET, buffer, in_content_size, 0);
        delete[] buffer;
    } else {
        if (REMOTE_PUBLIC_KEY) {
            // criptez query-ul ...
            int  out_content_size = (in_content_size / 16 + 1) * 16 + 5;
            char *ptr             = new char[out_content_size + sizeof(int)];

            char *out_content = ptr + sizeof(int);

            semp(mc->sem_send);
            int encrypt_result = AES_encrypt(mc, buffer, in_content_size, out_content, out_content_size, REMOTE_PUBLIC_KEY, 16, true);
            semv(mc->sem_send);

            if (!encrypt_result) {
                delete[] ptr;
                delete[] (buffer - sizeof(int));
                return;
            }

            int size = encrypt_result;
            *(int *)ptr = htonl(size);
            deturnated_send(mc, CLIENT_SOCKET, ptr, size + sizeof(int), 0);
            delete[] ptr;
        } else
            deturnated_send(mc, CLIENT_SOCKET, buffer - sizeof(int), in_content_size + sizeof(int), 0);
        delete[] (buffer - sizeof(int));
    }
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_send_message CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 4)
        return (void *)"send_message: function takes 4 parameters(usage: send_message [static string OWNER_NAME, static number MESSAGE_ID, string TARGET, static any MESSAGE_DATA)]";

    char    *SENDER_NAME;
    char    *MESSAGE_DATA;
    char    *MESSAGE_TARGET;
    NUMBER  MESSAGE_ID;
    NUMBER  nDUMMY_FILL;
    char    *szDUMMY_FILL;
    INTEGER TYPE;

    AnsiString Owner;
    AnsiString Target;
    AnsiString message_data;
    //bool    threaded=false;

    // get SENDER_NAME
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &SENDER_NAME, &nDUMMY_FILL);
    if (TYPE != VARIABLE_STRING)
        return (void *)"send_message: 1st parameter should be of STATIC STRING type";
    else
        Owner.LoadBuffer(SENDER_NAME, (INTEGER)nDUMMY_FILL);

    // message ID
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &szDUMMY_FILL, &MESSAGE_ID);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"send_message: 2nd parameter should be of STATIC NUMBER type";

    // get target
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &MESSAGE_TARGET, &nDUMMY_FILL);
    if (TYPE != VARIABLE_STRING)
        return (void *)"send_message: 3rd parameter should be of STATIC STRING type";
    else
        Target.LoadBuffer(MESSAGE_TARGET, (INTEGER)nDUMMY_FILL);

    // get messsage
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], &TYPE, &MESSAGE_DATA, &nDUMMY_FILL);
    if (TYPE == VARIABLE_STRING) {
        //message_data=MESSAGE_DATA;
        message_data.LoadBuffer(MESSAGE_DATA, (INTEGER)nDUMMY_FILL);
    } else
    if (TYPE == VARIABLE_NUMBER)
        message_data = AnsiString(nDUMMY_FILL);
    else
        return (void *)"send_message: 4th parameter should be of STATIC STRING or STATIC_NUMBER type";

    GET_METACONTAINER

    if ((MESSAGE_ID == 0x1001) && (MESSAGE_TARGET == (char *)"350")) {
        MESSAGE_ID     = 0x110;
        MESSAGE_TARGET = "";
    }

    INTEGER res             = 0;
    char    *buffer         = 0;
    int     in_content_size = 0;
    int     is_udp          = 0;
    int     message         = (int)MESSAGE_ID;
    int     remaining       = 0;
    do {
        SerializeBuffer(mc, &buffer, &in_content_size, &Owner, message, &Target, &message_data, REMOTE_PUBLIC_KEY, &is_udp, &remaining);
        if (ref_isWebSocket) {
            if ((mc->is_cached) && (!is_udp))
                Cache(mc, buffer, in_content_size);
            else
                res = deturnated_send(mc, CLIENT_SOCKET, buffer, in_content_size, 0, is_udp);
            delete[] buffer;
        } else {
            if (REMOTE_PUBLIC_KEY) {
                // criptez query-ul ...
                int  out_content_size = (in_content_size / 16 + 1) * 16 + 5;//(((in_content_size / 48) + 1) * 64) + 256;
                char *ptr             = new char[out_content_size + sizeof(int)];

                char *out_content = ptr + sizeof(int);

                semp(mc->sem_send);
                int encrypt_result = AES_encrypt(mc, buffer, in_content_size, out_content, out_content_size, REMOTE_PUBLIC_KEY, 16, !is_udp);
                semv(mc->sem_send);

                if (!encrypt_result) {
                    delete[] ptr;            //out_content;
                    delete[] (buffer - sizeof(int));
                    return (void *)"send_message: <<AES_encrypt>> error encrypting message.";
                }

                int size = encrypt_result;
                *(int *)ptr = htonl(size);

                if ((mc->is_cached) && (!is_udp)) {
                    Cache(mc, out_content, size);
                } else {
                    res = deturnated_send(mc, CLIENT_SOCKET, ptr, size + sizeof(int), 0, is_udp, 1);
                }
                delete[] ptr;
            } else {
                if (mc->is_cached) {
                    Cache(mc, buffer, in_content_size);
                } else {
                    res = deturnated_send(mc, CLIENT_SOCKET, buffer - sizeof(int), in_content_size + sizeof(int), 0, is_udp);
                }
            }
            delete[] (buffer - sizeof(int));
        }
    } while (remaining > 0);
    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_wait_message_ID CONCEPT_API_PARAMETERS {
    if ((PARAMETERS->COUNT < 4) || (PARAMETERS->COUNT > 5)) {
        SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
        return (void *)"wait_message_ID: function takes 4 to 5 parameters(usage: wait_message_ID(static string OWNER_NAME, static number MESSAGE_ID, string TARGET, static any MESSAGE_DATA[, number no_timeout=false))";
    }
    char       do_timeout = 1;
    NUMBER     nDUMMY_FILL;
    char       *szDUMMY_FILL;
    INTEGER    TYPE;
    char       *buffer   = 0;//[BUFFER_SIZE];
    INTEGER    rec_count = 0;
    INTEGER    MSG_ID    = 0;
    INTEGER    OWNER     = 0;
    AnsiString temp;

    GET_METACONTAINER
    if (mc->send_failed) {
        int new_socket = Invoke(INVOKE_GET_PROTO_SOCKET, PARAMETERS->HANDLER);
        if (CLIENT_SOCKET != new_socket) {
            mc->ResetSocket(CLIENT_SOCKET);
            mc->last_msg_seconds = time(NULL);
            mc->echo_sent        = 0;
            CLIENT_SOCKET        = new_socket;
            ResetConceptPeer(mc, new_socket);
            UpdateTLSSocket(mc, new_socket);
            mc->send_failed = 0;
            mc->ConnectionChanged(Invoke, new_socket);
        }
    }
    if (mc->is_cached)
        FlushCache(mc, CLIENT_SOCKET);

    for (int i = 0; i < 4; i++) {
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[i] - 1], &TYPE, &szDUMMY_FILL, &nDUMMY_FILL);
        if ((TYPE != VARIABLE_STRING) && (TYPE != VARIABLE_NUMBER)) {
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return (void *)"wait_message_ID: all parameters are passed by reference to this function. All parameters should be of STATIC type.";
        }
        if (i == 0) {
            if ((TYPE == VARIABLE_NUMBER) && (nDUMMY_FILL > 0))
                OWNER = (INTEGER)nDUMMY_FILL;
        } else
        if (i == 1) {
            if (TYPE == VARIABLE_NUMBER)
                MSG_ID = (INTEGER)nDUMMY_FILL;
            else {
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                return (void *)"wait_message_ID: 2nd parameter shoult be an integer (STATIC NUMBER)";
            }
        }
    }
    if (PARAMETERS_COUNT > 4) {
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[4] - 1], &TYPE, &szDUMMY_FILL, &nDUMMY_FILL);
        if (TYPE != VARIABLE_NUMBER) {
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return (void *)"wait_message_ID: parameter 5 should be a number.";
        }
        int no_timeout = (int)nDUMMY_FILL;
        if (no_timeout)
            do_timeout = 0;
    }

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);

    semp(mc->sem_bufferlist);
    int len = mc->BufferedMessages.Count();
    if (len) {
        for (int i = 0; i < len; i++) {
            TParameters *PARAM = (TParameters *)mc->BufferedMessages.Item(i);
            if ((PARAM) && (PARAM->ID == MSG_ID) && ((OWNER <= 0) || (OWNER == PARAM->Sender.ToInt()))) {
                mc->BufferedMessages.Remove(i);
                semv(mc->sem_bufferlist);

                if (OWNER <= 0) {
                    SetVariable(
                        LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1],
                        VARIABLE_STRING,
                        PARAM->Sender.c_str(),
                        PARAM->Sender.Length());            //0
                }
                SetVariable(
                    LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1],
                    VARIABLE_NUMBER,
                    "",
                    PARAM->ID);

                SetVariable(
                    LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1],
                    VARIABLE_STRING,
                    PARAM->Target.c_str(),
                    PARAM->Target.Length());            //0

                SetVariable(
                    LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1],
                    VARIABLE_STRING,
                    PARAM->Value.c_str(),
                    PARAM->Value.Length());             //0
                delete PARAM;

                // setez cu true (non-0) rezultatul
                SetVariable(RESULT, VARIABLE_NUMBER, "", i + 1);
                return 0;
            }
        }
    }
    semv(mc->sem_bufferlist);
    // iau marimea ...
    int          size           = 0;
    int          received       = 0;
    unsigned int size2          = 0;
    char         timed_out      = 0;
    time_t       start          = time(NULL);
    int          cachedmessages = 0;
    do {
        int seof  = 1;
        int count = 0;
        while (seof == 1) {
            seof = sock_eof_ssl(CLIENT_SOCKET, mc);
            if (seof == 1) {
#ifdef _WIN32
                Sleep(10);
#else
                usleep(10000);
#endif
            }
            count++;
            // 10 secounds
            if ((count >= 1000) && ((MSG_ID > 0) && (MSG_ID != 0x131) && (MSG_ID != 0x138))) {
                seof      = -1;
                timed_out = 1;
                break;
            }
        }
        if (seof <= 0) {
            SOCKET_RECREATE_CHECK_WAIT_MESSAGE
        }
        if ((do_timeout) && ((timed_out) && (((MSG_ID > 0) && (MSG_ID != 0x131) && (MSG_ID != 0x132) && (MSG_ID != 0x137) && (MSG_ID != 0x138) && (MSG_ID != 0x139) && (MSG_ID != 0x13A)) || (cachedmessages >= 50)))) {
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return 0;
        }

        size      = 0;
        size2     = 0;
        received  = 0;
        rec_count = 0;
        char *out_content = 0;

        if (ref_isWebSocket) {
            int code;
            int masked;
            semp(mc->sem_recv);
            size = WSGetPacketSize(mc, CLIENT_SOCKET, &code, &masked);
            if ((size <= 0) && (code)) {
                semv(mc->sem_recv);
                if (code > 0) {
                    // ping or pong
                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0);
                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
                    SetVariable(RESULT, VARIABLE_NUMBER, "", 1);
                    return 0;
                } else {
                    // connection closed
                    mc->force_exit = 1;
                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
                    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                    return 0;
                }
            }
            // min message is 7 bytes
            if (size < 7)
                size = -1;

            if (size <= 0) {
                if (size == 0) {
                    // flush mask buffer
                    WSReceive(mc, CLIENT_SOCKET, NULL, 0, masked);
                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
                } else
                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0);
                semv(mc->sem_recv);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                mc->force_exit = 1;
                return 0;
            }

            buffer       = new char[size + 1];
            buffer[size] = 0;
            if (!buffer) {
                semv(mc->sem_recv);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                return 0;
            }
            received = WSReceive(mc, CLIENT_SOCKET, buffer, size, masked);
            semv(mc->sem_recv);
            if (received != size) {
                mc->force_exit = 1;
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                delete[] buffer;
                return 0;
            }
        } else {
            semp(mc->sem_recv);
            do {
                received   = deturnated_recv(mc, CLIENT_SOCKET, (char *)&size + rec_count, sizeof(int) - rec_count, 0);
                rec_count += received;
            } while ((rec_count < sizeof(int)) && (received > 0));

            if (!LOCAL_PRIVATE_KEY) {
                size2 = ntohl(*(unsigned int *)&size);
                if (size2 & 0xF0000000) {
                    size = size2 & 0xFFFF;
                } else {
                    size  = ntohl(size);
                    size2 = 0;
                }
            } else
                size = ntohl(size);
            //size=ntohl(size);

            if (received <= 0) {
                semv(mc->sem_recv);

                SOCKET_RECREATE_CHECK(1)

                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                mc->force_exit = 1;
                //return (void *)"wait_message_ID: socket disconnected or host down";
                return 0;
            }

            //if (size > BUFFER_SIZE)
            //   return (void *)"wait_message_ID: received data is too big";
            if (size) {
                buffer       = new char[size + 1];
                buffer[size] = 0;
            }

            // iau xml-ul ...
            rec_count = 0;
            do {
                int to_get = size - rec_count;
                if (to_get > CHUNK_SIZE)
                    to_get = CHUNK_SIZE;
                received   = deturnated_recv(mc, CLIENT_SOCKET, buffer + rec_count, to_get, 0);
                rec_count += received;
            } while ((received > 0) && (rec_count < size));

            if (received <= 0) {
                semv(mc->sem_recv);
                if (buffer)
                    delete[] buffer;

                SOCKET_RECREATE_CHECK(1)

                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);

                mc->force_exit = 1;
                return 0;
            }
            semv(mc->sem_recv);

            if (LOCAL_PRIVATE_KEY) {
                out_content  = new char[size * 2];
                buffer[size] = 0;
                int decrypt_result = AES_decrypt(mc, buffer, size, out_content, size, LOCAL_PRIVATE_KEY, 16, true);
                out_content[decrypt_result] = 0;
                if (!decrypt_result) {
                    delete[] out_content;
                    out_content = 0;
                } else {
                    size = decrypt_result;
                    if (buffer) {
                        delete[] buffer;
                        buffer = 0;
                    }
                }
            }
        }
        int  message_id;
        char *Owner;
        int  OwnerLen;
        char *Target;
        int  TargetLen;
        char *Value;
        int  ValueLen;

        int is_looked = 1;

        char buf_tmp[0xFF];
        if (DeSerializeBuffer2(out_content ? out_content : buffer, size, &Owner, &OwnerLen, &message_id, &Target, &TargetLen, &Value, &ValueLen, size2, buf_tmp) < 0) {
            if (out_content) {
                delete[] out_content;
                out_content = 0;
            }

            if (buffer) {
                delete[] buffer;
                buffer = 0;
            }
            mc->force_exit = 1;
            mc->send_failed = 1;
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
            SetVariable(RESULT, VARIABLE_NUMBER, "", -1);
            return 0;
        }

        if ((message_id == MSG_ID) && (OWNER > 0)) {
            if (OwnerLen) {
                temp.LoadBuffer(Owner, OwnerLen);
                if (OWNER != temp.ToInt())
                    is_looked = 0;
            } else
                is_looked = 0;
        }
        if ((message_id == MSG_ID) && (is_looked)) {
            if (OWNER <= 0) {
                SetVariable(
                    LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1],
                    VARIABLE_STRING,
                    Owner,
                    0);
            }
            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1],
                VARIABLE_NUMBER,
                "",
                message_id);

            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1],
                VARIABLE_STRING,
                Target,
                TargetLen);

            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1],
                VARIABLE_STRING,
                Value,
                ValueLen);

            if (out_content) {
                delete[] out_content;
                out_content = 0;
            }

            if (buffer) {
                delete[] buffer;
                buffer = 0;
            }
            mc->last_msg_seconds = time(NULL);
            mc->echo_sent        = 0;
            SetVariable(RESULT, VARIABLE_NUMBER, "", size);
            return 0;
        } else {
            // nu este mesajul cautat, trec mai departe ...
            TParameters *MESSAGE = new TParameters;
            MESSAGE->ID     = message_id;
            MESSAGE->Sender = Owner;
            if (TargetLen)
                MESSAGE->Target.LoadBuffer(Target, TargetLen);
            if (ValueLen)
                MESSAGE->Value.LoadBuffer(Value, ValueLen);
            semp(mc->sem_bufferlist);
            mc->BufferedMessages.Add(MESSAGE, DATA_MESSAGE);
            semv(mc->sem_bufferlist);
            cachedmessages++;
        }
        if (out_content) {
            delete[] out_content;
            out_content = 0;
        }

        if (buffer) {
            delete[] buffer;
            buffer = 0;
        }
        if ((do_timeout) && ((((MSG_ID > 0) && (MSG_ID != 0x131) && (MSG_ID != 0x132) && (MSG_ID != 0x137) && (MSG_ID != 0x138) && (MSG_ID != 0x139) && (MSG_ID != 0x13A)) || (cachedmessages >= 50)) && (time(NULL) - start >= 10))) {
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return 0;
        }
    } while (1);
    SetVariable(RESULT, VARIABLE_NUMBER, "", size);
    return 0;
}
//-----------------------------------------------------------------------------------
int check_clientaddr(MetaContainer *mc, sockaddr_storage *cliaddr) {
    // accept from any
    if (cliaddr->ss_family != mc->remote_conceptudpaddr.ss_family)
        return 0;
    if (cliaddr->ss_family == AF_INET) {
        struct sockaddr_in *s  = (struct sockaddr_in *)&mc->remote_conceptudpaddr;
        struct sockaddr_in *s2 = (struct sockaddr_in *)cliaddr;
        if (memcmp(&s->sin_addr, &s2->sin_addr, sizeof(s2->sin_addr)))
            return 0;

        if (mc->RTCONFIRMED == 2)
            s->sin_port = s2->sin_port;
        return 1;
    } else
    if (cliaddr->ss_family == AF_INET6) {
        struct sockaddr_in6 *s  = (struct sockaddr_in6 *)&mc->remote_conceptudpaddr;
        struct sockaddr_in6 *s2 = (struct sockaddr_in6 *)cliaddr;
        if (memcmp(&s->sin6_addr, &s2->sin6_addr, sizeof(s2->sin6_addr)))
            return 0;
        if (mc->RTCONFIRMED == 2)
            s->sin6_port = s2->sin6_port;
        return 1;
    }
    return 0;
}

int recv2(MetaContainer *mc, int socket, char *buffer, int size, int flags) {
    struct sockaddr_storage cliaddr;
    socklen_t len = sizeof(cliaddr);
    int       res = recvfrom(socket, buffer, size, 0, (struct sockaddr *)&cliaddr, &len);

    if (res > 0) {
        // check host
        if (!check_clientaddr(mc, &cliaddr))
            return 0;
        mc->rt_in += res;
        // ip changed ?
        //memcpy(&mc->remote_conceptudpaddr, &cliaddr, sizeof(cliaddr));
    }
    return res;
}

int peek(MetaContainer *mc, int socket) {
    struct sockaddr_storage cliaddr;
    socklen_t len = sizeof(cliaddr);
    char      buffer[0xFFFF];

    int res = recvfrom(socket, buffer, sizeof(buffer), MSG_PEEK, (struct sockaddr *)&cliaddr, &len);

    if (res > 0) {
        if (!check_clientaddr(mc, &cliaddr)) {
            int res = recvfrom(socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
            if (res < 0)
                return -1;
            return 0;
        }
    }
    return res;
}

int peek_UDP_message(MetaContainer *mc, ParamList *PARAMETERS, VariableDATA **LOCAL_CONTEXT, VariableDATA *RESULT, CALL_BACK_VARIABLE_SET SetVariable, CALL_BACK_VARIABLE_GET GetVariable, char *LOCAL_PUBLIC_KEY, char *LOCAL_PRIVATE_KEY, char *REMOTE_PUBLIC_KEY, CALL_BACK_CLASS_MEMBER_SET CallBACKClassSet, CALL_BACK_CLASS_MEMBER_GET CallBACKClassGet, INVOKE_CALL Invoke, int t) {
    if ((mc->RTSOCKET <= 0) || (!mc->RTCONFIRMED)) {
        return 0;
    }
    SYSTEM_SOCKET CLIENT_SOCKET = mc->RTSOCKET;

    if (sock_eof2(CLIENT_SOCKET, t))
        return 0;
    int peek_check = peek(mc, CLIENT_SOCKET);

    if (peek_check < 0)
        return peek_check;
    if (!peek_check)
        return 0;

    int  size      = 0;
    int  received  = 0;
    int  rec_count = 0;
    int  size2     = 0;
    char buf_temp[0xFFFF];
    char *buffer = 0;

    semp(mc->sem_recv);
    received = recv2(mc, CLIENT_SOCKET, buf_temp, sizeof(buf_temp), 0);
    semv(mc->sem_recv);
    if (ref_isWebSocket) {
        size = received;
        size2 = size;
    } else {
        size = *(unsigned int *)buf_temp;
        if (!LOCAL_PRIVATE_KEY) {
            size2 = ntohl(*(unsigned int *)&size);
            if (size2 & 0xF0000000) {
                size = size2 & 0xFFFF;
            } else {
                size  = ntohl(size);
                size2 = 0;
            }
        } else
            size = ntohl(size);

        if ((size <= 0) || (size > 0xFFFF))
            return 0;

        if (size >= received - sizeof(int))
            size = received - sizeof(int);
    }

    if (received <= 0)
        return -1;

    if (size) {
        if (ref_isWebSocket)
            buffer = buf_temp;
        else
            buffer = buf_temp + sizeof(int);
    }
    char *out_content = 0;
    if (LOCAL_PRIVATE_KEY) {
        out_content  = (char *)malloc(size * 2);
        buffer[size] = 0;
        int decrypt_result = AES_decrypt(mc, buffer, size, out_content, size, LOCAL_PRIVATE_KEY, 16, false);
        out_content[decrypt_result] = 0;
        if (!decrypt_result) {
            free(out_content);
            out_content = 0;
        } else
            size = decrypt_result;
    }

    AnsiString Owner;
    AnsiString Target;
    AnsiString Value;
    int        message_id;

    DeSerializeBuffer(out_content ? out_content : buffer, size, &Owner, &message_id, &Target, &Value, size2);

    int valid_stream_message = 0;
    if ((message_id == 0x1001) && (Target == def_msg)) {
        valid_stream_message = 1;
        SetVariable(
            LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1],
            VARIABLE_STRING,
            Owner.c_str(),
            Owner.Length());        //0
        SetVariable(
            LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1],
            VARIABLE_NUMBER,
            "",
            message_id);

        SetVariable(
            LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1],
            VARIABLE_STRING,
            Target.c_str(),
            Target.Length());        //0

        SetVariable(
            LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1],
            VARIABLE_STRING,
            Value.c_str(),
            Value.Length());
    }

    if (out_content)
        free(out_content);

    // atack ?
    if (valid_stream_message) {
        SetVariable(RESULT, VARIABLE_NUMBER, "", size);
        mc->last_msg_seconds = time(NULL);
        mc->echo_sent        = 0;
    } else
        return 0;
    return 1;
}

CONCEPT_DLL_API CONCEPT_get_message CONCEPT_API_PARAMETERS {
    if ((PARAMETERS->COUNT != 4) && (PARAMETERS->COUNT != 5)) {
        SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
        return (void *)"get_message: function takes 4 parameters(usage: get_message [static string OWNER_NAME, static number MESSAGE_ID, string TARGET, static any MESSAGE_DATA, delegate idle_callback=null)]";
    }
    GET_METACONTAINER
    int pipe_in     = 0;
    int pipe_out    = 0;
    int apid        = 0;
    int parent_apid = 0;
    Invoke(INVOKE_GET_APPLICATION_INFO, PARAMETERS->HANDLER, &pipe_in, &pipe_out, &apid, &parent_apid);

    if (mc->force_exit) {
        SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
        SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
        return 0;
    }

    if (mc->send_failed) {
        int new_socket = Invoke(INVOKE_GET_PROTO_SOCKET, PARAMETERS->HANDLER);
        if (CLIENT_SOCKET != new_socket) {
            mc->ResetSocket(CLIENT_SOCKET);
            mc->last_msg_seconds = time(NULL);
            CLIENT_SOCKET        = new_socket;
            ResetConceptPeer(mc, new_socket);
            UpdateTLSSocket(mc, new_socket);
            mc->send_failed = 0;
            mc->echo_sent   = 0;
            mc->ConnectionChanged(Invoke, new_socket);
        } else
        if (mc->send_failed) {
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return 0;
        }
    }

    NUMBER  nDUMMY_FILL;
    char    *szDUMMY_FILL;
    INTEGER TYPE;
    char    *buffer   = 0;//[BUFFER_SIZE];
    INTEGER rec_count = 0;

    if (mc->is_cached)
        FlushCache(mc, CLIENT_SOCKET);

    // get SENDER_NAME
    for (int i = 0; i < 4; i++) {
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[i] - 1], &TYPE, &szDUMMY_FILL, &nDUMMY_FILL);
        if ((TYPE != VARIABLE_STRING) && (TYPE != VARIABLE_NUMBER)) {
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return (void *)"get_message: all parameters are passed by reference to this function. All parameters should be of STATIC type.";
        }
    }

    void *DELEGATE = 0;
    if (PARAMETERS->COUNT == 5) {
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[4] - 1], &TYPE, &szDUMMY_FILL, &nDUMMY_FILL);
        if (TYPE == VARIABLE_DELEGATE) {
            DELEGATE = LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[4] - 1];
        } else
        if ((TYPE != VARIABLE_NUMBER) || (nDUMMY_FILL != 0)) {
            return (void *)"get_message: parameter 5 should be a delegate or null/false/zero(0).";
        }
    }

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    // iau marimea ...
    int size     = 0;
    int received = 0;

    /*struct timeval timeout;
       timeout.tv_sec = 0;
       timeout.tv_usec = 5000;
       fd_set socks;*/

    int sel_val;
    int loop_count = 0;
    int timeout    = 0;
    if ((mc->RTSOCKET > 0) && (mc->RTCONFIRMED))
        timeout = 5;
    else
        timeout = 20;
    do {
        // verificare Buffered messages (daca am chemat wait_message_ID, este posibil sa am ceva in BuffereMessages
        semp(mc->sem_bufferlist);
        if (mc->BufferedMessages.Count()) {
            // setez cu true (non-0) rezultatul
            SetVariable(RESULT, VARIABLE_NUMBER, "", 1);
            TParameters *PARAM = (TParameters *)mc->BufferedMessages.Remove(0);
            semv(mc->sem_bufferlist);
            // pregatesc setarile ...
            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1],
                VARIABLE_STRING,
                PARAM->Sender.c_str(),
                PARAM->Sender.Length());            //0
            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1],
                VARIABLE_NUMBER,
                "",
                PARAM->ID);

            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1],
                VARIABLE_STRING,
                PARAM->Target.c_str(),
                PARAM->Target.Length());            //0

            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1],
                VARIABLE_STRING,
                PARAM->Value.c_str(),
                PARAM->Value.Length());             //0
            delete PARAM;
            mc->last_msg_seconds = time(NULL);
            mc->echo_sent        = 0;
            return 0;
        }
        semv(mc->sem_bufferlist);

        //FD_ZERO(&socks);
        //FD_SET(CLIENT_SOCKET,&socks);

        int res_eof = 1;
        if (pipe_in > 0)
            res_eof = eof(pipe_in);
        // nothing to read
        if (res_eof < 0)
            break;    //res_eof=1;

        if (!res_eof) {
            // to do ... preluare mesaj inter-application
            int params[3];
            int total = deturnated_read(pipe_in, (void *)params, sizeof(int) * 3);

#ifndef _WIN32
            if (total <= 0) {
                sel_val = !eof(CLIENT_SOCKET); //select(FD_SETSIZE, &socks, 0, 0, &timeout);
                if (sel_val != 0)
                    break;
            }
#endif

            if (total != sizeof(int) * 3) {
                RETURN_NUMBER(0);
                return 0;
            }

            char *data = 0;
            if (params[2] > 0) {
                data            = new char[params[2] + 1];
                total           = deturnated_read(pipe_in, (void *)data, params[2]);
                data[params[2]] = 0;
            }

            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1],
                VARIABLE_NUMBER,
                "",
                params[0]);
            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1],
                VARIABLE_NUMBER,
                "",
                -0x100);
            SetVariable(
                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1],
                VARIABLE_NUMBER,
                "",
                params[1]);

            if (data)
                SetVariable(
                    LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1],
                    VARIABLE_STRING,
                    data,
                    params[2]);
            else
                SetVariable(
                    LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1],
                    VARIABLE_STRING,
                    "",
                    0);

            if (data)
                delete[] data;
            SetVariable(RESULT, VARIABLE_NUMBER, "", total);

            mc->last_msg_seconds = time(NULL);
            mc->echo_sent        = 0;
            return 0;
        }
        //sel_val=select(FD_SETSIZE, &socks, 0, 0, &timeout);
        if ((mc->RTSOCKET > 0) && (mc->RTCONFIRMED))
            sel_val = !sock_eof(CLIENT_SOCKET);
        else
            sel_val = !sock_eof2(CLIENT_SOCKET, timeout);

        if (!sel_val) {
            // optimize CPU usage
            if ((timeout >= 1000) || (loop_count % 10 == 0)) {
                int new_socket = Invoke(INVOKE_GET_PROTO_SOCKET, PARAMETERS->HANDLER);
                if (CLIENT_SOCKET != new_socket) {
                    mc->ResetSocket(CLIENT_SOCKET);
                    mc->echo_sent        = 0;
                    mc->last_msg_seconds = time(NULL);
                    CLIENT_SOCKET        = new_socket;
                    ResetConceptPeer(mc, new_socket);
                    UpdateTLSSocket(mc, new_socket);
                    mc->send_failed = 0;
                    mc->ConnectionChanged(Invoke, new_socket);
                }

                if ((mc->event_timeout > 0) && (mc->last_msg_seconds)) {
                    time_t now = time(NULL);
                    if ((mc->event_timeout > 0) && (!mc->echo_sent) && (now - mc->last_msg_seconds >= mc->event_timeout / 2)) {
                        //0x503 = client query
                        send_message2(mc, "%", "Ping", 0x503, "-1", REMOTE_PUBLIC_KEY, CLIENT_SOCKET);
                        mc->echo_sent = 1;
                    } else
                    if (now - mc->last_msg_seconds > mc->event_timeout) {
                        SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
                        SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                        mc->force_exit = 1;
                        return 0;
                    }
                }
            }
            if (mc->send_failed) {
                SOCKET_RECREATE_CHECK(1)

                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                mc->force_exit = 1;
                return 0;
            }

            if (DELEGATE) {
                void *RES       = 0;
                void *EXCEPTION = 0;
                Invoke(INVOKE_CALL_DELEGATE, DELEGATE, &RES, &EXCEPTION, (INTEGER)0);
                if (RES)
                    Invoke(INVOKE_FREE_VARIABLE, RES);
                if (EXCEPTION)
                    Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);
            }
        }

        if ((sel_val == 0) && (mc->RTSOCKET > 0) && (mc->RTCONFIRMED)) {
            int udp_res = peek_UDP_message(mc, PARAMETERS, LOCAL_CONTEXT, RESULT, SetVariable, GetVariable, LOCAL_PUBLIC_KEY, LOCAL_PRIVATE_KEY, REMOTE_PUBLIC_KEY, CallBACKClassSet, CallBACKClassGet, Invoke, timeout);
            if (udp_res == 1) {
                return 0;
            } else
            if (udp_res < 0) {
#ifdef _WIN32
                closesocket(mc->RTSOCKET);
#else
                close(mc->RTSOCKET);
#endif
                mc->RTSOCKET    = -1;
                mc->RTCONFIRMED = 0;
                loop_count      = 0;
            }
        }
        if (sel_val == 0) {
            mc->Iterate(Invoke);
#ifdef _WIN32
            Sleep(5);
#else
            usleep(5000);
#endif
            if ((!mc->LOOPERS) || (mc->RTSOCKET <= 0) || (!mc->RTCONFIRMED)) {
                if (loop_count < 5) {
                    loop_count++;
                } else
                if (loop_count < 10) {
                    timeout = 200;
                    loop_count++;
                } else
                if (loop_count < 15) {
                    timeout = 300;
                    loop_count++;
                } else
                if (loop_count < 25) {
                    timeout = 500;
                    loop_count++;
                } else {
                    /*if (loop_count < 100) {
                        timeout = 300;
                        loop_count++;
                       #ifdef _WIN32
                        Sleep(50);
                       #else
                        usleep(50000);
                       #endif
                       } else*/
                    timeout = 1000;
                    // additional sleep

                    /*#ifdef _WIN32
                                        Sleep(300);
                       #else
                                        usleep(300000);
                     #endif*/
                }
            }
        }
    } while (sel_val == 0);

    unsigned int size2        = 0;
    char         *out_content = 0;

    if (ref_isWebSocket) {
        semp(mc->sem_recv);
        int code;
        int masked;
        size = WSGetPacketSize(mc, CLIENT_SOCKET, &code, &masked);
        if ((size <= 0) && (code)) {
            semv(mc->sem_recv);
            if (code > 0) {
                // ping or pong
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 1);
                return 0;
            } else {
                // connection closed
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                mc->force_exit = 1;
                return 0;
            }
        }
        // min message is 7 bytes
        if (size < 7)
            size = -1;
        if (size <= 0) {
            if (size == 0) {
                // flush mask buffer
                WSReceive(mc, CLIENT_SOCKET, NULL, 0, masked);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
            } else
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0);
            semv(mc->sem_recv);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return 0;
        }

        buffer       = (char *)malloc(size + 1);
        buffer[size] = 0;
        if (!buffer) {
            semv(mc->sem_recv);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return 0;
        }
        received = WSReceive(mc, CLIENT_SOCKET, buffer, size, masked);
        semv(mc->sem_recv);
        if (received != size) {
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], VARIABLE_STRING, "", 0);
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_STRING, "", 0);
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            free(buffer);
            return 0;
        }
    } else {
        semp(mc->sem_recv);
        do {
            received = deturnated_recv(mc, CLIENT_SOCKET, (char *)&size + rec_count, sizeof(int) - rec_count, 0);
            if (received < 0) {
                semv(mc->sem_recv);

                SOCKET_RECREATE_CHECK(1)

                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                return 0;
            }
            rec_count += received;
        } while ((rec_count < sizeof(int)) && (received > /*=*/ 0)); // received > 0
        if (!LOCAL_PRIVATE_KEY) {
            size2 = ntohl(*(unsigned int *)&size);
            if (size2 & 0xF0000000) {
                size = size2 & 0xFFFF;
            } else {
                size  = ntohl(size);
                size2 = 0;
            }
        } else
            size = ntohl(size);
        //size=ntohl(size);

        if ((received <= 0) || (size <= 0)) {
            semv(mc->sem_recv);

            SOCKET_RECREATE_CHECK(1)

            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return 0;
            //return (void *)"get_message: socket disconnected or host down";
        }

        if (size) {
            buffer = (char *)malloc(size + 1);
            if (!buffer) {
                semv(mc->sem_recv);
                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                return 0;
            }
            buffer[size] = 0;
        }

        rec_count = 0;
        do {
            int to_get = size - rec_count;
            if (to_get > CHUNK_SIZE)
                to_get = CHUNK_SIZE;
            received = deturnated_recv(mc, CLIENT_SOCKET, buffer + rec_count, to_get, 0);
            if (received <= 0) {
                semv(mc->sem_recv);
                if (buffer)
                    free(buffer);
                SOCKET_RECREATE_CHECK(1)

                SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
                SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
                return 0;
            }
            rec_count += received;
        } while ((received > /*=*/ 0) && (rec_count < size));

        if (received <= 0) {
            semv(mc->sem_recv);
            if (buffer)
                free(buffer);
            SOCKET_RECREATE_CHECK(1)

            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], VARIABLE_NUMBER, "", 0x500);
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return 0;    //(void *)"get_message: socket disconnected or host down";
        }
        semv(mc->sem_recv);

        if (LOCAL_PRIVATE_KEY) {
            out_content  = (char *)malloc(size * 2);
            buffer[size] = 0;
            int decrypt_result = AES_decrypt(mc, buffer, size, out_content, size, LOCAL_PRIVATE_KEY, 16, true);
            out_content[decrypt_result] = 0;
            if (!decrypt_result) {
                free(out_content);
                out_content = 0;
            } else
                size = decrypt_result;
        }
    }
    AnsiString Owner;
    AnsiString Target;
    AnsiString Value;
    int        message_id;

    if (DeSerializeBuffer(out_content ? out_content : buffer, size, &Owner, &message_id, &Target, &Value, size2)) {
        message_id = 0x500;
        mc->force_exit = 1;
    }

    SetVariable(
        LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1],
        VARIABLE_STRING,
        Owner.c_str(),
        Owner.Length());            //0
    SetVariable(
        LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1],
        VARIABLE_NUMBER,
        "",
        message_id);

    SetVariable(
        LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1],
        VARIABLE_STRING,
        Target.c_str(),
        Target.Length());            //0

    SetVariable(
        LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1],
        VARIABLE_STRING,
        Value.c_str(),
        Value.Length());

    if (out_content)
        free(out_content);

    if (buffer)
        free(buffer);

    SetVariable(RESULT, VARIABLE_NUMBER, "", size);
    mc->last_msg_seconds = time(NULL);
    mc->echo_sent        = 0;
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_confirm_message CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)"confirm_message: function takes 1 parameter. [usage: confirm_message(VALUE)]";

    char    *STR_DATA;
    NUMBER  NUMBER_DATA;
    INTEGER TYPE;

    // get SENDER_NAME
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STR_DATA, &NUMBER_DATA);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"confirm_message: 1st parameter should be of STATIC NUMBER type";

    GET_METACONTAINER
    deturnated_send(mc, CLIENT_SOCKET, (char *)&NUMBER_DATA, sizeof(NUMBER), 0);

    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_raw_deturnated_send CONCEPT_API_PARAMETERS {
    // send_message( TargetName, MESSAGE_ID, MESSAGE_RESERVED, MESSAGE_DATA)
    if (PARAMETERS->COUNT != 1)
        return (void *)"raw_deturnated_send: function takes 1 parameter. [usage: raw_deturnated_send(DATA)]";

    char    *DATA;
    NUMBER  nDUMMY_FILL;
    INTEGER TYPE;

    // get SENDER_NAME
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &DATA, &nDUMMY_FILL);
    if (TYPE != VARIABLE_STRING)
        return (void *)"raw_deturnated_send: 1st parameter should be of STATIC STRING type";

    AnsiString SERVER_QUERY(DATA);

    GET_METACONTAINER
    int size = htonl(SERVER_QUERY.Length() + 1);
    deturnated_send(mc, CLIENT_SOCKET, (char *)&size, sizeof(int), 0);
    INTEGER res = deturnated_send(mc, CLIENT_SOCKET, SERVER_QUERY, size, 0);
    SetVariable(RESULT, VARIABLE_NUMBER, "", res);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_raw_receive CONCEPT_API_PARAMETERS {
    // send_message( TargetName, MESSAGE_ID, MESSAGE_RESERVED, MESSAGE_DATA)
    if (PARAMETERS->COUNT)
        return (void *)"raw_receive: doesn't take any parameters. [usage: raw_receive()]";

    //char    *DATA;
    //NUMBER  nDUMMY_FILL;
    //INTEGER TYPE;
    char buffer[BUFFER_SIZE];
    int  rec_count = 0;

    GET_METACONTAINER
    // iau marimea ...
    int size = 0;
    do {
        rec_count += deturnated_recv(mc, CLIENT_SOCKET, (char *)&size + rec_count, sizeof(int) - rec_count, 0);
    } while ((rec_count < sizeof(int)) && (rec_count > -1));
    size = ntohl(size);

    if (size >= BUFFER_SIZE)
        return (void *)"raw_receive: received data is too big";

    // iau xml-ul ...
    rec_count = 0;
    int received = 0;
    do {
        received   = deturnated_recv(mc, CLIENT_SOCKET, buffer + rec_count, size - rec_count, 0);
        rec_count += received;
    } while ((received > -1) && (rec_count < size));

    buffer[rec_count] = 0;

    SetVariable(RESULT, VARIABLE_STRING, buffer, 0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___DEBUG_PIPEW CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT)
        return (void *)"__DEBUG_PIPEW: doesn't take any parameters.";

    int pipe_in     = 0;
    int pipe_out    = 0;
    int apid        = 0;
    int parent_apid = 0;
    Invoke(INVOKE_GET_APPLICATION_INFO, PARAMETERS->HANDLER, &pipe_in, &pipe_out, &apid, &parent_apid);

    RETURN_NUMBER(pipe_out);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___DEBUG_PIPER CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT)
        return (void *)"__DEBUG_PIPEW: doesn't take any parameters.";

    int pipe_in     = 0;
    int pipe_out    = 0;
    int apid        = 0;
    int parent_apid = 0;
    Invoke(INVOKE_GET_APPLICATION_INFO, PARAMETERS->HANDLER, &pipe_in, &pipe_out, &apid, &parent_apid);

    RETURN_NUMBER(pipe_in);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_CheckPoint CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)"CheckPoint: function takes 1 parameter: the status of checkpoint (0 do not use check points, any other values: time to check point).";

    LOCAL_INIT;
    NUMBER nvariable;

    int status;

    GET_CHECK_NUMBER(0, nvariable, "CheckPoint: status should be a number");

    status = (int)nvariable;

    Invoke(INVOKE_CHECK_POINT, status);

    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(HaveMessage, 0)
    int res = 0;
    GET_METACONTAINER
        semp(mc->sem_bufferlist);
    res = (mc->BufferedMessages.Count() != 0);
    semv(mc->sem_bufferlist);
    if (!res) {
        semp(mc->sem_recv);
        //RETURN_NUMBER((mc->BufferedMessages.Count()!=0) || !sock_eof(CLIENT_SOCKET))
        res = !sock_eof_ssl(CLIENT_SOCKET, mc);
        semv(mc->sem_recv);
    }
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hibernate, 1)
    T_NUMBER(0)

    char c = (char)PARAM_INT(0);

    Invoke(INVOKE_HIBERNATE, PARAMETERS->HANDLER, &c);

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SetApplicationEventTimeout, 1)
    T_NUMBER(0);

    GET_METACONTAINER
// in seconds
    mc->event_timeout = PARAM_INT(0) / 1000;
    mc->last_msg_seconds = time(NULL);
    mc->echo_sent        = 0;
    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
#ifdef _WIN32
DWORD WINAPI LocalThread(LPVOID P) {
#else
void *LocalThread(void *P) {
#endif
    TimerData *td = (TimerData *)P;
    if (td) {
        int           repeat = td->repeat;
        MetaContainer *mc    = td->mc;
        int           apid   = td->APID;
        AnsiString    sender = AnsiString((long)td->RID);
#ifdef _WIN32
        int ms = td->ms;
#else
        int ms = td->ms * 1000;
#endif
        delete td;
        if ((repeat == 0) || (repeat == 1)) {
#ifdef _WIN32
            Sleep(ms);
#else
            usleep(ms);
#endif
            TParameters *MESSAGE = new TParameters;
            MESSAGE->Sender = sender;
            MESSAGE->ID     = 0x1001; // event fired
            MESSAGE->Target = "91";   // on timer

            semp(mc->sem_bufferlist);
            mc->BufferedMessages.Add(MESSAGE, DATA_MESSAGE);
            semv(mc->sem_bufferlist);
        } else
        if (repeat < 0) {
            do {
#ifdef _WIN32
                Sleep(ms);
#else
                usleep(ms);
#endif
                TParameters *MESSAGE = new TParameters;
                MESSAGE->Sender = sender;
                MESSAGE->ID     = 0x1001; // event fired
                MESSAGE->Target = "91";   // on timer

                semp(mc->sem_bufferlist);
                // max 0x8000 messages in stack
                if (mc->BufferedMessages.Count() < 0x2000)
                    mc->BufferedMessages.Add(MESSAGE, DATA_MESSAGE);
                semv(mc->sem_bufferlist);
            } while (true);
        } else {
            do {
#ifdef _WIN32
                Sleep(ms);
#else
                usleep(ms);
#endif
                TParameters *MESSAGE = new TParameters;
                MESSAGE->Sender = sender;
                MESSAGE->ID     = 0x1001; // event fired
                MESSAGE->Target = "91";   // on timer

                semp(mc->sem_bufferlist);
                // max 0x8000 messages in stack
                if (mc->BufferedMessages.Count() < 0x8000)
                    mc->BufferedMessages.Add(MESSAGE, DATA_MESSAGE);
                semv(mc->sem_bufferlist);
            } while (--repeat);
        }
    }
    return 0;
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(LocalTimer, 2, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    int repeat = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        repeat = PARAM_INT(2);
    }

    int pipe_in     = 0;
    int pipe_out    = 0;
    int apid        = 0;
    int parent_apid = 0;
    Invoke(INVOKE_GET_APPLICATION_INFO, PARAMETERS->HANDLER, &pipe_in, &pipe_out, &apid, &parent_apid);

    GET_METACONTAINER
    TimerData *td = new TimerData();
    td->RID    = PARAM_INT(0);
    td->ms     = PARAM_INT(1);
    td->repeat = repeat;
    td->mc     = mc;
    td->APID   = apid;
#ifdef _WIN32
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    DWORD  tid;
    LPVOID local_threadID = CreateThread(&sa, 0, LocalThread, (LPVOID)td, 0, &tid);
#else
    pthread_t local_threadID;
    pthread_create(&local_threadID, NULL, LocalThread, (void *)td);
#endif

    RETURN_NUMBER((SYS_INT)local_threadID);
END_IMPL
//---------------------------------------------------------------------------
#ifdef _WIN32
const char *inet_ntop2(int af, const void *src, char *dst, socklen_t cnt) {
    if (af == AF_INET) {
        struct sockaddr_in in;
        memset(&in, 0, sizeof(in));
        in.sin_family = AF_INET;
        memcpy(&in.sin_addr, src, sizeof(struct in_addr));
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    } else if (af == AF_INET6) {
        struct sockaddr_in6 in;
        memset(&in, 0, sizeof(in));
        in.sin6_family = AF_INET6;
        memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    }
    return NULL;
}
#endif

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(GetRemoteIP, 0, 1)
    struct sockaddr_storage addr;
    char        ipstr[INET6_ADDRSTRLEN];
    int         port     = 0;
    static char *unknown = "unknown";

#ifdef _WIN32
    int peerlen = sizeof(addr);
#else
    socklen_t peerlen = sizeof(addr);
#endif
    if (getpeername(CLIENT_SOCKET, (sockaddr *)&addr, &peerlen)) {
        RETURN_STRING("unknown");
        return 0;
    }

    const char *ip = 0;
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
#ifdef _WIN32
        ip = inet_ntop2(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
#else
        ip = inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
#endif
    } else
    if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        port = ntohs(s->sin6_port);
#ifdef _WIN32
        ip = inet_ntop2(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
#else
        ip = inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
#endif
    }
    if (!ip)
        ip = unknown;
    if (PARAMETERS_COUNT > 0) {
        SET_NUMBER(0, port);
    }
    RETURN_STRING(ip);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(CloseRTSocket, 0)
    GET_METACONTAINER
    if (mc->RTSOCKET > 0) {
#ifdef _WIN32
        closesocket(mc->RTSOCKET);
#else
        close(mc->RTSOCKET);
#endif
        mc->RTSOCKET    = -1;
        mc->RTCONFIRMED = 0;
    }

    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(CreateRTSocket, 1, 2)
    T_NUMBER(0)
    int port = PARAM_INT(0);
    int sockfd = -1;

    GET_METACONTAINER
    if (mc->RTSOCKET > 0) {/* || (port<0))*/
        RETURN_NUMBER(-1);
        return 0;
    }
    mc->RTCONFIRMED     = 0;
    mc->rt_in           = 0;
    mc->rt_out          = 0;
    mc->rt_send_enabled = 1;
    memset(&mc->remote_conceptudpaddr, 0, sizeof(mc->remote_conceptudpaddr));
#ifdef _WIN32
    int peerlen = sizeof(mc->remote_conceptaddr);
#else
    socklen_t peerlen = sizeof(mc->remote_conceptaddr);
#endif
    if (getpeername(CLIENT_SOCKET, (sockaddr *)&mc->remote_conceptaddr, &peerlen)) {
        RETURN_NUMBER(-1);
        return 0;
    }
    int ipv6 = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1);
        ipv6 = PARAM_INT(1);
    }
    if (ipv6)
        sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    else
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd <= 0) {
        RETURN_NUMBER(-1);
        return 0;
    } else {
        int flag = 0xFFFF;
        setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&flag, sizeof(flag));

        int flag2 = 0x10;
        //flag=1;
        setsockopt(sockfd, IPPROTO_IP, IP_TOS, (char *)&flag2, sizeof(flag2));
        //setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
        if (port > 0) {
            int bind_res;
            if (ipv6) {
                struct sockaddr_in6 servaddr;
                memset(&servaddr, 0, sizeof(servaddr));
                servaddr.sin6_family = AF_INET6;
                servaddr.sin6_addr   = in6addr_any;
                servaddr.sin6_port   = htons(port);
                bind_res             = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
            } else {
                struct sockaddr_in servaddr;
                memset(&servaddr, 0, sizeof(servaddr));
                servaddr.sin_family      = AF_INET;
                servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
                servaddr.sin_port        = htons(port);
                bind_res = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
            }
            if (bind_res) {
#ifdef _WIN32
                closesocket(sockfd);
#else
                close(sockfd);
#endif
                RETURN_NUMBER(-1);
                return 0;
            }
        }
        mc->RTSOCKET = sockfd;
    }
    RETURN_NUMBER(mc->RTSOCKET);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ConfirmRTSocket, 2)
    T_STRING(0)
    T_NUMBER(1)
    GET_METACONTAINER
    if (mc->RTSOCKET > 0) {
        struct addrinfo hints;
        struct addrinfo *result;
        struct addrinfo *res;

        memset(&hints, 0, sizeof hints);
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;

        AnsiString port_str((long)PARAM_INT(1));
        if (getaddrinfo(PARAM(0), port_str.c_str(), &hints, &result) != 0) {
            RETURN_NUMBER(0);
            return 0;
        }
        int is_set = 0;
        for (res = result; res != NULL; res = res->ai_next) {
            char hostname[NI_MAXHOST] = "";
            if ((res->ai_family == AF_INET) || (res->ai_family == AF_INET6)) {
                int error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
                if (!error) {
                    memcpy(&mc->remote_conceptudpaddr, res->ai_addr, res->ai_addrlen);
                    mc->remote_len      = res->ai_addrlen;
                    mc->RTCONFIRMED     = 2;
                    mc->rt_in           = 0;
                    mc->rt_out          = 0;
                    mc->rt_send_enabled = 1;
                    is_set = 1;
                    break;
                }
            }
        }
        if (result)
            freeaddrinfo(result);
        RETURN_NUMBER(is_set);
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(WaitRTSocket, 1)
    int res = 0;
    char buffer[0xFF];
    struct sockaddr_storage cliaddr;
    socklen_t len = sizeof(cliaddr);

    T_NUMBER(0)

    GET_METACONTAINER
    if (mc->RTSOCKET > 0) {
        int sock_res = sock_eof_timeout(mc->RTSOCKET, PARAM_INT(0));
        if (sock_res == 0) {
            int res_peek = peek(mc, mc->RTSOCKET);
            if (res_peek == 1) {
                int rec_res = recvfrom(mc->RTSOCKET, buffer, 2, 0, (struct sockaddr *)&mc->remote_conceptudpaddr, &len);
                if (rec_res == 1) {
                    mc->remote_len = len;
#if defined(_WIN32) || !defined(MSG_NOSIGNAL)
                    if (sendto(mc->RTSOCKET, buffer, 1, 0, (struct sockaddr *)&mc->remote_conceptudpaddr, len) == 1) {
#else
                    if (sendto(mc->RTSOCKET, buffer, 1, MSG_NOSIGNAL, (struct sockaddr *)&mc->remote_conceptudpaddr, len) == 1) {
#endif
                        sock_res = sock_eof_timeout(mc->RTSOCKET, PARAM_INT(0) * 3);
                        if (sock_res == 0) {
                            int res_peek = peek(mc, mc->RTSOCKET);
                            if (res_peek == 1) {
                                rec_res = recvfrom(mc->RTSOCKET, buffer, 2, 0, (struct sockaddr *)&cliaddr, &len);
                                if (rec_res == 1) {
                                    res             = 1;
                                    mc->RTCONFIRMED = 1;
                                }
                            }
                        }
                    }
                } else
                    res = 0;
            }
        }
    }
    RETURN_NUMBER(res)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ConceptProtocolSecured, 0)
    GET_METACONTAINER
    if (REMOTE_PUBLIC_KEY) {
#ifndef NOSSL
        if (mc->ssl) {
            RETURN_NUMBER(3);
        } else {
            RETURN_NUMBER(1);
        }
#else
        RETURN_NUMBER(1);
#endif
    } else {
#ifndef NOSSL
        if (mc->ssl) {
            RETURN_NUMBER(2);
        } else {
            RETURN_NUMBER(0);
        }
#else
        RETURN_NUMBER(0);
#endif
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(UpgradeTLS, 2, 4)
    T_STRING(0)
    T_STRING(1);
    char *cert_file = PARAM(0);
    char *priv_file = PARAM(1);
    GET_METACONTAINER
#ifndef NOSSL
    char *trustfile = 0;
    char *trustpath = 0;
    if (PARAMETERS_COUNT > 2) {
        T_STRING(2);
        if (PARAM_LEN(2) > 0)
            trustfile = PARAM(2);
        if (PARAMETERS_COUNT > 3) {
            T_STRING(3)
            if (PARAM_LEN(3) > 0)
                trustpath = PARAM(3);
        }
    }
    int res = SetSSL(mc, CLIENT_SOCKET, cert_file, priv_file, trustfile, trustpath);
    RETURN_NUMBER(res);
#else
    RETURN_NUMBER(-2);
#endif
END_IMPL
//---------------------------------------------------------------------------
AnsiString DoCString(char *value, int len) {
    AnsiString res;

    for (int i = 0; i < len; i++) {
        char c = value[i];
        switch (c) {
            case '\a':
                res += (char *)"\\a";
                break;

            case '\b':
                res += (char *)"\\b";
                break;

            case '\f':
                res += (char *)"\\f";
                break;

            case '\n':
                res += (char *)"\\n";
                break;

            case '\r':
                res += (char *)"\\r";
                break;

            case '\t':
                res += (char *)"\\t";
                break;

            case '\v':
                res += (char *)"\\v";
                break;

            case '\\':
                res += (char *)"\\\\";
                break;

            case '\"':
                res += (char *)"\\\"";
                break;

            case '\0':
                res += (char *)"\\0";
                break;

            default:
                res += c;
        }
    }
    return res;
}

unsigned int murmur_hash(const void *key, long len) {
    if (!key)
        return 0;
    const unsigned int  m     = 0x5bd1e995;
    const int           r     = 24;
    unsigned int        seed  = 0x7870AAFF;
    const unsigned char *data = (const unsigned char *)key;
    //int len = strlen((const char *)data);
    if (!len)
        return 0;

    unsigned int h = seed ^ len;

    while (len >= 4) {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len  -= 4;
    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;

        case 2:
            h ^= data[1] << 8;

        case 1:
            h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

void GetMemberID(void *THIS_REF, void *THIS_VAR, void *pifHandler, char *mname, int max_hops, AnsiString *member_get, AnsiString *member_set, INVOKE_CALL Invoke, void **objects) {
    char    *defined_in;
    INTEGER type, mset, mget, macc;

    if ((!max_hops) || ((!member_get) && (!member_set)))
        return;

    if (IS_OK(Invoke(INVOKE_MEMBER_INFO, THIS_VAR, mname, &defined_in, &type, &mset, &mget, &macc))) {
        // event or property
        if ((type == 2) || (type == 3)) {
            if (mget > 0) {
                char *temp = 0;
                if (IS_OK(Invoke(INVOKE_MEMBER_NAME, pifHandler, mget, &temp))) {
                    if (temp)
                        GetMemberID(THIS_REF, THIS_VAR, pifHandler, temp, max_hops - 1, NULL, member_get, Invoke, objects);
                }
            }
        }

        if (type == 3) {
            if (mset > 0) {
                char *temp = 0;
                if (IS_OK(Invoke(INVOKE_MEMBER_NAME, pifHandler, mset, &temp))) {
                    if (temp)
                        GetMemberID(THIS_REF, THIS_VAR, pifHandler, temp, max_hops - 1, member_set, NULL, Invoke, objects);
                }
            }
        }

        if (type == 1) {
            // function
            if (member_get)
                *member_get = mname;
            else
            if (member_set)
                *member_set = mname;
        }

        if (type == 0) {
            INTEGER type  = -1;
            void    *data = NULL;
            NUMBER  ndata = 0;

            void *VAR_MEMBER = 0;
            if (IS_OK(Invoke(INVOKE_GET_CLASS_VARIABLE, THIS_VAR, mname, &VAR_MEMBER))) {
                if (IS_OK(Invoke(INVOKE_GET_VARIABLE, VAR_MEMBER, &type, &data, &ndata))) {
                    if (type == VARIABLE_CLASS)
                        *objects = VAR_MEMBER;
                }
            }

            /*if (IS_OK(Invoke(INVOKE_GET_CLASS_MEMBER, THIS_REF, mname, &type, &data, &ndata))) {
                if (type == VARIABLE_CLASS) {
                    void *VAR_MEMBER = NULL;
                    if (IS_OK(Invoke(INVOKE_GET_CLASS_VARIABLE, THIS_VAR, mname, &VAR_MEMBER)))
             * objects = VAR_MEMBER;
                }
               }*/
        }
    }
}

AnsiString GenerateCode(int level, void *THIS_REF, void *THIS_VAR, void *pifHandler, void *DELEGATE, INVOKE_CALL Invoke, AnsiString& err, std::map<unsigned int, int> *mapped_func, AnsiString *cl_funcname = NULL, char only_ui = 0) {
    AnsiString code("@");
    AnsiString func;

    TreeContainer *CODE   = 0;
    INTEGER       codeLen = 0;

    TreeVD  *DATA   = 0;
    INTEGER dataLen = 0;

    char    *real_class_name = 0;
    char    *class_name      = 0;
    char    *member_name     = 0;
    char    *temp            = 0;
    char    *defined_in      = 0;
    INTEGER mset, mget, macc, mtype;

    if (!IS_OK(Invoke(INVOKE_GET_DELEGATE_NAMES, DELEGATE, &class_name, &member_name)))
        return AnsiString();


    INTEGER params     = 0;
    INTEGER min_params = 0;
    Invoke(INVOKE_COUNT_DELEGATE_PARAMS2, DELEGATE, &params, &min_params);

    if ((IS_OK(Invoke(INVOKE_MEMBER_INFO, THIS_VAR, member_name, &defined_in, &mtype, &mset, &mget, &macc))) && (defined_in)) {
        code           += defined_in;
        real_class_name = defined_in;
    } else {
        code           += class_name;
        real_class_name = class_name;
    }
    code += ".";
    AnsiString mname(member_name);
    int        len = mname.Length();
    for (int i = 0; i < len; i++) {
        if (member_name[i] == '#')
            mname.c_str()[i] = '_';
    }
    code += mname;
    code += "(";

    AnsiString h(real_class_name);
    h += ".";
    h += mname;
    unsigned int hash   = murmur_hash(h.c_str(), h.Length());
    int          exists = (*mapped_func)[hash];

    if (cl_funcname) {
        *cl_funcname  = real_class_name;
        *cl_funcname += ".";
        *cl_funcname += mname;
    }
    if (exists)
        return AnsiString();

    AnsiString decl;
    AnsiString pre_decl;
    if (THIS_REF) {
        INTEGER type  = -1;
        void    *data = NULL;
        NUMBER  ndata = 0;

        if (IS_OK(Invoke(INVOKE_MEMBER_INFO, THIS_VAR, "RID", &defined_in, &type, &mset, &mget, &macc))) {
            if (((type == 0) || (type == 3)) && (IS_OK(Invoke(INVOKE_GET_CLASS_MEMBER, THIS_REF, "RID", &type, &data, &ndata)))) {
                decl += "this ";
                decl += AnsiString((long)ndata);
                decl += "\n";
            } else
            if (only_ui) {
                if (cl_funcname)
                    *cl_funcname = (char *)"";
                return AnsiString(); // preventing NON-UI objects to run on client
            }
        } else
        if (only_ui) {
            if (cl_funcname)
                *cl_funcname = (char *)"";
            return AnsiString();
        }
    }

    (*mapped_func)[hash] = 1;

    int instructions = 0;
    int res          = Invoke(INVOKE_DELEGATE_BYTECODE, DELEGATE, &CODE, &codeLen, &DATA, &dataLen);
    if ((DATA) && (CODE)) {
        if ((params < dataLen) && (dataLen > 0)) {
            int *refs = (int *)malloc(sizeof(int) * dataLen);
            memset(refs, 0, sizeof(int) * dataLen);

            int *coderefs = (int *)malloc(sizeof(int) * codeLen);
            memset(coderefs, 0, sizeof(int) * codeLen);

            int *used = (int *)malloc(sizeof(int) * dataLen);
            memset(used, 0, sizeof(int) * dataLen);

            void **objects = (void **)malloc(sizeof(void *) * dataLen);
            memset(objects, 0, sizeof(void *) * dataLen);

            for (int i = 0; i < codeLen; i++) {
                TreeContainer *OE = &CODE[i];
                if (OE->Operator_TYPE == TYPE_OPERATOR) {
                    switch (OE->Operator_ID) {
                        case KEY_ASG:
                        case KEY_BY_REF:
                        case KEY_ASU:
                        case KEY_ADI:
                        case KEY_AMU:
                        case KEY_MUL:
                        case KEY_ADV:
                        case KEY_ARE:
                        case KEY_AAN:
                        case KEY_AXO:
                        case KEY_AOR:
                        case KEY_ASL:
                        case KEY_ASR:
                        case KEY_INC:
                        case KEY_INC_LEFT:
                        case KEY_DEC:
                        case KEY_DEC_LEFT:
                            if (refs[OE->OperandLeft_ID - 1])
                                coderefs[i] = refs[OE->OperandLeft_ID - 1];
                            break;
                    }
                }

                if (OE->Operator_TYPE == TYPE_OPERATOR) {
                    switch (OE->Operator_ID) {
                        case KEY_DELETE:
                        case KEY_TYPE_OF:
                        case KEY_CLASS_NAME:
                        case KEY_LENGTH:
                            used[OE->OperandLeft_ID - 1]++;
                            break;

                        case KEY_ASG:
                        case KEY_BY_REF:
                        case KEY_ASU:
                        case KEY_ADI:
                        case KEY_AMU:
                        case KEY_MUL:
                        case KEY_ADV:
                        case KEY_DIV:
                        case KEY_SUM:
                        case KEY_DIF:
                        case KEY_LES:
                        case KEY_LEQ:
                        case KEY_GRE:
                        case KEY_GEQ:
                        case KEY_EQU:
                        case KEY_NEQ:
                        case KEY_CND_NULL:
                        case KEY_REM:
                        case KEY_ARE:
                        case KEY_BOR:
                        case KEY_BAN:
                        case KEY_AAN:
                        case KEY_AND:
                        case KEY_AXO:
                        case KEY_AOR:
                        case KEY_ASL:
                        case KEY_ASR:
                        case KEY_SHL:
                        case KEY_SHR:
                        case KEY_XOR:
                        case KEY_OR:
                        case KEY_INDEX_OPEN:
                            used[OE->OperandRight_ID - 1]++;
                            used[OE->OperandLeft_ID - 1]++;
                            break;

                        case KEY_INC:
                        case KEY_DEC:
                        case KEY_INC_LEFT:
                        case KEY_DEC_LEFT:
                        case KEY_POZ:
                        case KEY_VALUE:
                        case KEY_NOT:
                        case KEY_NEG:
                        case KEY_COM:
                            used[OE->OperandLeft_ID - 1]++;
                            break;

                        case KEY_NEW:
                            if ((OE->OperandLeft_ID == -1) && (!only_ui)) {
                                for (int k = 0; k < OE->ParametersCount; k++)
                                    used[OE->Parameters[k] - 1]++;
                                break;
                            }
                            break;

                        case KEY_SEL:
                            used[OE->OperandLeft_ID - 1]++;

                        case KEY_DLL_CALL:
                            //if (((OE->Function) && (OE->Function[0])) || (!only_ui)) {
                            for (int k = 0; k < OE->ParametersCount; k++)
                                used[OE->Parameters[k] - 1]++;
                            break;
                            //}
                            break;
                    }
                } else
                if (OE->Operator_TYPE == TYPE_OPTIMIZED_KEYWORD) {
                    switch (OE->Operator_ID) {
                        case KEY_OPTIMIZED_GOTO:
                            if (only_ui) {
                                // backwards jump (no loops on client code)
                                if (OE->OperandReserved_ID <= i)
                                    codeLen = OE->OperandReserved_ID;
                            }
                            break;

                        case KEY_OPTIMIZED_THROW:
                        case KEY_OPTIMIZED_RETURN:
                        case KEY_OPTIMIZED_ECHO:
                        case KEY_OPTIMIZED_IF:
                            used[OE->OperandRight_ID - 1]++;
                            if ((only_ui) && (OE->Operator_ID == KEY_OPTIMIZED_IF)) {
                                // backwards jump (no loops on client code)
                                if (OE->OperandReserved_ID <= i)
                                    codeLen = OE->OperandReserved_ID;
                            }
                            break;
                    }
                }
            }

            AnsiString type_temp;
            for (int i = 0; i < params; i++) {
                TreeVD *VD = &DATA[i + 1];
                if (only_ui) {
                    if (i)
                        code += ", ";
                    code       += "v";
                    code       += AnsiString((long)i + 1);
                    used[i + 1] = -1;
                } else {
                    switch (VD->TYPE) {
                        case VARIABLE_NUMBER:
                        case -VARIABLE_NUMBER:
                            if (i)
                                code += ", ";
                            if (VD->IsRef)
                                code += "var v";
                            else
                                code += "v";
                            code += AnsiString((long)i + 1);
                            if (i >= min_params) {
                                code += " ";
                                code += DoCString(VD->Value, VD->Length);
                            }
                            break;

                        case -VARIABLE_ARRAY:
                        case VARIABLE_ARRAY:
                            if (i)
                                code += ", ";
                            code += "array v";
                            code += AnsiString((long)i + 1);
                            if (i >= min_params) {
                                code += "=";
                                code += DoCString(VD->Value, VD->Length);
                            }
                            break;

                        case VARIABLE_STRING:
                        case -VARIABLE_STRING:
                            if (i)
                                code += ", ";
                            code += "string v";
                            code += AnsiString((long)i + 1);
                            if (i >= min_params) {
                                code += " = \"";
                                code += DoCString(VD->Value, VD->Length);
                                code += "\"";
                            }
                            break;

                        case VARIABLE_CLASS:
                        case -VARIABLE_CLASS:
                            if (i)
                                code += ", ";
                            code += "object v";
                            code += AnsiString((long)i + 1);
                            if (i >= min_params) {
                                code += " = ";
                                code += DoCString(VD->Value, VD->Length);
                            }
                            break;

                        case VARIABLE_DELEGATE:
                        case -VARIABLE_DELEGATE:
                            if (i)
                                code += ", ";
                            code += "delegate v";
                            code += AnsiString((long)i + 1);
                            if (i >= min_params) {
                                code += " = ";
                                code += DoCString(VD->Value, VD->Length);
                            }
                            break;

                        default:
                            free(refs);
                            free(coderefs);
                            free(used);
                            free(objects);
                            Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                            return AnsiString();
                    }
                }
            }
            int  estimate_len;
            bool ui_out = false;
            for (int i = 0; i < codeLen; i++) {
                TreeContainer *OE = &CODE[i];
                //decl+="l";
                //decl+=AnsiString(i);
                //decl+=" ";
                //decl+=":";
                if (OE->Operator_TYPE == TYPE_OPTIMIZED_KEYWORD) {
                    if (OE->Operator_ID == KEY_OPTIMIZED_IF) {
                        decl += "if !v";
                        if (only_ui)
                            used[OE->OperandRight_ID - 1] = -1;
                        decl += AnsiString((long)OE->OperandRight_ID - 1);
                        if (OE->OperandReserved_ID >= codeLen) {
                            decl += " return";
                        } else {
                            decl += " goto ";
                            decl += AnsiString((long)OE->OperandReserved_ID);
                        }
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_GOTO) {
                        if (OE->OperandReserved_ID >= codeLen) {
                            decl += "return";
                        } else {
                            decl += "goto ";
                            decl += AnsiString((long)OE->OperandReserved_ID);
                        }
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_RETURN) {
                        if (only_ui)
                            used[OE->OperandRight_ID - 1] = -1;
                        decl += "return v";
                        decl += AnsiString((long)OE->OperandRight_ID - 1);
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_ECHO) {
                        decl += "echo v";
                        decl += AnsiString((long)OE->OperandRight_ID - 1);
                        if (only_ui)
                            used[OE->OperandRight_ID - 1] = -1;
                    } else
                    if (only_ui) {
                        ui_out = true;
                        break;
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_TRY_CATCH) {
                        decl += "try v";
                        decl += AnsiString((long)OE->Result_ID - 1);
                        decl += " goto ";
                        decl += AnsiString((long)OE->OperandReserved_ID);
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_THROW) {
                        decl += "throw v";
                        decl += AnsiString((long)OE->OperandRight_ID - 1);
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_END_CATCH) {
                        decl += "endtry";
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_DEBUG_TRAP)
                        decl += "trap";
                } else
                if (OE->Operator_TYPE == TYPE_OPERATOR) {
                    decl += "";

                    switch (OE->Operator_ID) {
                        case KEY_ASG:
                        case KEY_BY_REF:
                            if (used[OE->Result_ID - 1]) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "v";
                            //if (coderefs[i]) {
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                }
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " = v";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1])
                                decl += ")";

                            objects[OE->OperandLeft_ID - 1] = objects[OE->OperandRight_ID - 1];
                            if (used[OE->Result_ID - 1])
                                objects[OE->OperandLeft_ID - 1] = objects[OE->Result_ID - 1];
                            break;

                        case KEY_INC:
                            if (used[OE->Result_ID - 1]) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ";
                            }
                            //if (coderefs[i]) {
                            if (coderefs[i]) {
                                if (used[OE->Result_ID - 1])
                                    decl += "(";
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += "v";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] += 1";
                                if (used[OE->Result_ID - 1])
                                    decl += ")";
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                }
                            } else {
                                decl += "++ v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                if (only_ui)
                                    used[OE->OperandLeft_ID - 1] = -1;
                            }
                            break;

                        case KEY_INC_LEFT:
                            if (used[OE->Result_ID - 1]) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ";
                                if (coderefs[i]) {
                                    TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                    decl += "v";
                                    decl += AnsiString(OE2->OperandLeft_ID - 1);
                                    decl += "[v";
                                    decl += AnsiString(OE2->OperandRight_ID - 1);
                                    decl += "]";
                                    if (only_ui) {
                                        used[OE2->OperandRight_ID - 1] = -1;
                                        used[OE2->OperandLeft_ID - 1]  = -1;
                                    }
                                }
                            }
                            decl += "v";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] += 1";

                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                }
                            } else {
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " ++";
                            }
                            break;

                        case KEY_DEC:
                            if (used[OE->Result_ID - 1]) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ";
                            }
                            if (coderefs[i]) {
                                if (used[OE->Result_ID - 1])
                                    decl += "(";
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += "v";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] -= 1";
                                if (used[OE->Result_ID - 1])
                                    decl += ")";

                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                }
                            } else {
                                decl += "-- v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            }
                            break;

                        case KEY_DEC_LEFT:
                            if (used[OE->Result_ID - 1]) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ";
                                if (coderefs[i]) {
                                    TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                    decl += "v";
                                    decl += AnsiString(OE2->OperandLeft_ID - 1);
                                    decl += "[v";
                                    decl += AnsiString(OE2->OperandRight_ID - 1);
                                    decl += "]";
                                    if (only_ui) {
                                        used[OE2->OperandRight_ID - 1] = -1;
                                        used[OE2->OperandLeft_ID - 1]  = -1;
                                    }
                                }
                            }
                            decl += "v";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] -= 1";
                            } else {
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " --";
                            }
                            break;

                        case KEY_INDEX_OPEN:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += "]";
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_ASU:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "v";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                }
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " += v";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1] > 1)
                                decl += ")";
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_LES:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " < v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ")";

                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_LEQ:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " <= v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ")";

                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_GRE:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " > v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ")";

                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_GEQ:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " >= v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ")";

                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_SUM:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " + v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += "";

                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_DIF:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " + v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += "";

                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_ADI:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "v";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                }
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " -= v";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1] > 1)
                                decl += ")";
                            else
                                decl += "";
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_AMU:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "v";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                }
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " *= v";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1] > 1)
                                decl += ")";
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_ADV:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "v";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                }
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " /= v";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1] > 1)
                                decl += ")";
                            else
                                decl += "";
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_MUL:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " * v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_DIV:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " / v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_EQU:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " == v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ")";
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_NEQ:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " != v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_CND_NULL:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " ?? v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_REM:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " % v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_ARE:
                            decl += "v";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = ";
                            decl += "v";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " % v";
                            decl += AnsiString(OE->OperandRight_ID - 1);

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " v";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                    used[OE2->Result_ID - 1]       = -1;
                                }
                            }
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_BOR:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " || v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_BAN:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " && v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_AAN:
                            decl += "v";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = v";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " &= v";
                            decl += AnsiString(OE->OperandRight_ID - 1);

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " v";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                    used[OE2->Result_ID - 1]       = -1;
                                }
                            }
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_AND:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " & v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_AXO:
                            decl += "v";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = v";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " ^= v";
                            decl += AnsiString(OE->OperandRight_ID - 1);

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " v";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                    used[OE2->Result_ID - 1]       = -1;
                                }
                            }
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_AOR:
                            decl += "v";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = v";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " |= v";
                            decl += AnsiString(OE->OperandRight_ID - 1);

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " v";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                    used[OE2->Result_ID - 1]       = -1;
                                }
                            }
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_ASL:
                            decl += "v";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = v";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " <<= v";
                            decl += AnsiString(OE->OperandRight_ID - 1);

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " v";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                    used[OE2->Result_ID - 1]       = -1;
                                }
                            }
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_ASR:
                            decl += "v";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = v";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " >>= v";
                            decl += AnsiString(OE->OperandRight_ID - 1);

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " v";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[v";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                if (only_ui) {
                                    used[OE2->OperandRight_ID - 1] = -1;
                                    used[OE2->OperandLeft_ID - 1]  = -1;
                                    used[OE2->Result_ID - 1]       = -1;
                                }
                            }
                            if (only_ui) {
                                used[OE->OperandRight_ID - 1] = -1;
                                used[OE->OperandLeft_ID - 1]  = -1;
                            }
                            break;

                        case KEY_SHL:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " << v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_SHR:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " >> v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_XOR:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " ^ v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_OR:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " | v";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                if (only_ui) {
                                    used[OE->OperandRight_ID - 1] = -1;
                                    used[OE->OperandLeft_ID - 1]  = -1;
                                }
                            }
                            break;

                        case KEY_POZ:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                if (only_ui)
                                    used[OE->OperandLeft_ID - 1] = -1;
                            }
                            break;

                        case KEY_NOT:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ! v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                if (only_ui)
                                    used[OE->OperandLeft_ID - 1] = -1;
                            }
                            break;

                        case KEY_NEG:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = - v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                if (only_ui)
                                    used[OE->OperandLeft_ID - 1] = -1;
                            }
                            break;

                        case KEY_COM:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ~ v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                if (only_ui)
                                    used[OE->OperandLeft_ID - 1] = -1;
                            }
                            break;

                        case KEY_DELETE:
                            decl += "del v";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            if (only_ui)
                                used[OE->OperandLeft_ID - 1] = -1;
                            break;

                        case KEY_TYPE_OF:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = typeof v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                if (only_ui)
                                    used[OE->OperandLeft_ID - 1] = -1;
                            }
                            break;

                        case KEY_CLASS_NAME:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = classof v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                if (only_ui)
                                    used[OE->OperandLeft_ID - 1] = -1;
                            }
                            break;

                        case KEY_VALUE:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = value v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                if (only_ui)
                                    used[OE->OperandLeft_ID - 1] = -1;
                            }
                            break;

                        case KEY_LENGTH:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "v";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = length v";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                if (only_ui)
                                    used[OE->OperandLeft_ID - 1] = -1;
                            }
                            break;

                        case KEY_SEL:
                            if ((only_ui) && (OE->OperandLeft_ID > 0))
                                used[OE->OperandLeft_ID - 1] = -1;

                            if ((OE->OperandLeft_ID == 1) && (OE->Function) && (OE->ParametersCount > -1) && (level > 1)) {
                                void *DELEGATE2 = 0;
                                CREATE_VARIABLE(DELEGATE2);
                                AnsiString fname;
                                if (IS_OK(Invoke(INVOKE_CREATE_DELEGATE, THIS_VAR, DELEGATE2, OE->Function))) {
                                    func += GenerateCode(level - 1, THIS_REF, THIS_VAR, pifHandler, DELEGATE2, Invoke, err, mapped_func, &fname, only_ui);
                                    if (err.Length()) {
                                        FREE_VARIABLE(DELEGATE2);
                                        return err;
                                    }
                                }
                                FREE_VARIABLE(DELEGATE2);
                                if (used[OE->Result_ID - 1] > 0) {
                                    decl += "v";
                                    decl += AnsiString(OE->Result_ID - 1);
                                    decl += " = ";
                                }
                                if (fname.Length())
                                    decl += fname;
                                else {
                                    decl += real_class_name;
                                    decl += ".";
                                    decl += OE->Function;
                                }
                                decl += "(";
                                for (int k = 0; k < OE->ParametersCount; k++) {
                                    if (k)
                                        decl += ", ";
                                    decl += "v";
                                    int param = OE->Parameters[k] - 1;
                                    decl += AnsiString((long)param);
                                    if (only_ui)
                                        used[param] = -1;
                                }
                                decl += ")";
                                instructions++;
                            } else {
                                AnsiString decl2;
                                if (used[OE->Result_ID - 1] > 0) {
                                    decl2 += "v";
                                    decl2 += AnsiString(OE->Result_ID - 1);
                                    decl2 += " = ";
                                }
                                if (OE->OperandLeft_ID == 1) {
                                    decl2 += "this";
                                } else {
                                    decl2 += "v";
                                    decl2 += AnsiString(OE->OperandLeft_ID - 1);
                                }
                                decl2 += " . ";

                                temp = OE->Function;
                                if (!temp)
                                    Invoke(INVOKE_MEMBER_NAME, pifHandler, (INTEGER)OE->OperandRight_ID, &temp);

                                if (temp) {
                                    decl2 += temp;
                                    if (OE->ParametersCount > -1) {
                                        decl2 += "(";
                                        for (int k = 0; k < OE->ParametersCount; k++) {
                                            if (k)
                                                decl2 += ", ";
                                            decl2 += "v";
                                            int param = OE->Parameters[k] - 1;
                                            decl2 += AnsiString((long)param);
                                            if (only_ui)
                                                used[param] = -1;
                                        }
                                        decl2 += ")";
                                    }
                                    if (((OE->OperandLeft_ID == 1) && (THIS_REF)) || (objects[OE->OperandLeft_ID - 1])) {
                                        INTEGER type   = -1;
                                        void    *data  = NULL;
                                        void    *data2 = NULL;
                                        NUMBER  ndata  = 0;

                                        void *USED_REF = THIS_REF;
                                        void *USED_VAR = THIS_VAR;

                                        if (objects[OE->OperandLeft_ID - 1]) {
                                            USED_VAR = objects[OE->OperandLeft_ID - 1];
                                            Invoke(INVOKE_GET_VARIABLE, USED_VAR, &type, &USED_REF, &ndata);
                                        }
                                        // look for RID
                                        type = -1;
                                        if (OE->ParametersCount < 0) {
                                            //if ((IS_OK(Invoke(INVOKE_GET_CLASS_MEMBER, USED_REF, temp, &type, &data, &ndata))) && (type == VARIABLE_CLASS)) {
                                            if ((IS_OK(Invoke(INVOKE_MEMBER_INFO, USED_VAR, temp, (void *)NULL, &type, (void *)NULL, (void *)NULL, (void *)NULL))) && (type == 0)) {
                                                void *TMP_VAR = 0;
                                                if (IS_OK(Invoke(INVOKE_GET_CLASS_VARIABLE, USED_VAR, temp, &TMP_VAR)) && (TMP_VAR)) {
                                                    if ((IS_OK(Invoke(INVOKE_GET_VARIABLE, TMP_VAR, &type, &data, &ndata))) && (type == VARIABLE_CLASS)) {
                                                        if ((IS_OK(Invoke(INVOKE_GET_CLASS_MEMBER, data, "RID", &type, &data2, &ndata))) && (type == VARIABLE_NUMBER)) {
                                                            decl2 += " #";
                                                            decl2 += AnsiString((long)ndata);
                                                        } else
                                                        if ((only_ui) && (OE->OperandLeft_ID >= params)) {
                                                            ui_out = true;
                                                            break;
                                                        }
                                                    }
                                                }
                                            } else
                                            if (type != 3) {
                                                // not a property
                                                if ((only_ui) && (OE->OperandLeft_ID >= params)) {
                                                    ui_out = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (level > 1) {
                                            AnsiString mname_get;
                                            AnsiString mname_set;
                                            AnsiString mname_func;
                                            GetMemberID(USED_REF, USED_VAR, pifHandler, temp, 10, &mname_get, &mname_set, Invoke, &objects[OE->Result_ID - 1]);

                                            decl += decl2;
                                            if (mname_get.Length()) {
                                                void *DELEGATE2 = NULL;
                                                CREATE_VARIABLE(DELEGATE2);
                                                Invoke(INVOKE_CREATE_DELEGATE, USED_VAR, DELEGATE2, mname_get.c_str());
                                                func += GenerateCode(level - 1, USED_REF, USED_VAR, pifHandler, DELEGATE2, Invoke, err, mapped_func, &mname_func, only_ui);
                                                FREE_VARIABLE(DELEGATE2);
                                                if (mname_func.Length()) {
                                                    decl += " <- ";
                                                    decl += mname_func;
                                                }
                                            }
                                            if (mname_set.Length()) {
                                                void *DELEGATE2 = NULL;
                                                CREATE_VARIABLE(DELEGATE2);
                                                Invoke(INVOKE_CREATE_DELEGATE, USED_VAR, DELEGATE2, mname_set.c_str());
                                                func += GenerateCode(level - 1, USED_REF, USED_VAR, pifHandler, DELEGATE2, Invoke, err, mapped_func, &mname_func, only_ui);
                                                FREE_VARIABLE(DELEGATE2);
                                                if (mname_func.Length()) {
                                                    decl += " -> ";
                                                    decl += mname_func;
                                                }
                                            }
                                        } else
                                            decl += decl2;
                                    } else
                                        decl += decl2;
                                    if ((OE->OperandLeft_ID != 1) || (OE->ParametersCount >= 0))
                                        instructions++;
                                }
                            }
                            break;

                        case KEY_DLL_CALL:
                            // ignore profiler
                            if ((OE->OperandLeft_ID == -2) && (OE->Function) && (OE->Function[0]) && (strcmp(OE->Function, "CheckPoint")) && (strcmp(OE->Function, "__profile"))) {
                                if (only_ui) {
                                    if ((!strcmp(OE->Function, "ClsPtr")) || (!strcmp(OE->Function, "VarPtr")) || (!strcmp(OE->Function, "GetAPID")) || (!strcmp(OE->Function, "SendAPMessage")) || (!strcmp(OE->Function, "protect"))) {
                                        ui_out = true;
                                        break;
                                    }
                                }
                                if (used[OE->Result_ID - 1] > 0) {
                                    decl += "v";
                                    decl += AnsiString(OE->Result_ID - 1);
                                    decl += " = ";
                                }
                                decl += OE->Function;
                                decl += "(";
                                for (int k = 0; k < OE->ParametersCount; k++) {
                                    if (k)
                                        decl += ", ";
                                    decl += "v";
                                    int param = OE->Parameters[k] - 1;
                                    decl       += AnsiString((long)param);
                                    used[param] = -1;
                                }
                                decl += ")";
                                if (OE->ParametersCount > 0)
                                    instructions++;
                                break;
                            } else
                            if (OE->OperandLeft_ID != -2) {
                                char *temp2 = NULL;
                                Invoke(INVOKE_CLASS_NAME, pifHandler, (INTEGER)OE->OperandLeft_ID, &temp2);
                                if (temp2) {
                                    if (only_ui) {
                                        if (strcmp(temp2, "CApplication")) {
                                            ui_out = true;
                                            break;
                                        }
                                    }
                                    if (used[OE->Result_ID - 1] > 0) {
                                        decl += "v";
                                        decl += AnsiString(OE->Result_ID - 1);
                                        decl += " = ";
                                    }

                                    decl += temp2;
                                    decl += "::";

                                    temp = OE->Function;
                                    if (!temp)
                                        Invoke(INVOKE_MEMBER_NAME, pifHandler, (INTEGER)OE->OperandRight_ID, &temp);
                                    if (temp) {
                                        decl += temp;
                                        if (OE->ParametersCount > -1) {
                                            decl += "(";
                                            for (int k = 0; k < OE->ParametersCount; k++) {
                                                if (k)
                                                    decl += ", ";
                                                decl += "v";
                                                int param = OE->Parameters[k] - 1;
                                                decl += AnsiString((long)param);
                                                if (only_ui)
                                                    used[param] = -1;
                                            }
                                            decl += ")";
                                        }
                                    }
                                }
                            } else {
                                decl += "nop";
                            }
                            break;

                        case KEY_NEW:
                            if (only_ui) {
                                ui_out = true;
                                break;
                            }
                            if (OE->OperandLeft_ID == -1) {
                                // array creation
                                if (used[OE->Result_ID - 1] > 0) {
                                    decl += "v";
                                    decl += AnsiString(OE->Result_ID - 1);
                                    decl += " = [ ]";
                                }
                                break;
                            } else {
                                if (used[OE->Result_ID - 1] > 0) {
                                    decl += "v";
                                    decl += AnsiString(OE->Result_ID - 1);
                                    decl += " = new ";
                                    Invoke(INVOKE_CLASS_NAME, pifHandler, (INTEGER)OE->OperandLeft_ID, &temp);
                                    if (temp)
                                        decl += temp;
                                    else
                                        decl += "#unknown#object#";
                                    decl += "(";
                                    if (OE->ParametersCount > -1) {
                                        for (int k = 0; k < OE->ParametersCount; k++) {
                                            if (k)
                                                decl += ", ";
                                            decl += "v";
                                            int param = OE->Parameters[k] - 1;
                                            decl += AnsiString((long)param);
                                        }
                                    }
                                    decl += ")";
                                }
                                break;
                            }
                            break;

                        default:
                            if ((only_ui) && (OE->OperandLeft_ID >= params)) {
                                ui_out = true;
                                break;
                            }
                            break;
                    }
                } else {
                    free(refs);
                    free(coderefs);
                    free(used);
                    free(objects);
                    Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                    return AnsiString();
                }
                decl += "\n";
                if (ui_out)
                    break;
            }
            free(coderefs);
            free(refs);
            free(objects);

            if (used) {
                for (int i = params + 1; i < dataLen; i++) {
                    TreeVD *VD    = &DATA[i];
                    bool   is_ref = false;

                    if (used[i]) {
                        switch (VD->TYPE) {
                            case VARIABLE_NUMBER:
                            case -VARIABLE_NUMBER:
                                if (!VD->nValue)
                                    break;
                                pre_decl += "$v";
                                pre_decl += AnsiString((long)i);
                                pre_decl += " ";
                                pre_decl += AnsiString(VD->nValue);
                                pre_decl += "\n";
                                break;

                            case -VARIABLE_ARRAY:
                            case VARIABLE_ARRAY:
                                break;

                            case VARIABLE_STRING:
                            case -VARIABLE_STRING:
                                if (is_ref) {
                                    pre_decl += "$v";
                                    pre_decl += AnsiString((long)i);
                                    pre_decl += " \"\"\n";
                                } else {
                                    if (VD->Length > 0) {
                                        pre_decl += "$v";
                                        pre_decl += AnsiString((long)i);
                                        pre_decl += " \"";
                                        pre_decl += DoCString(VD->Value, VD->Length);
                                        pre_decl += "\"\n";
                                    } else {
                                        pre_decl += "$v";
                                        pre_decl += AnsiString((long)i);
                                        pre_decl += " \"\"\n";
                                    }
                                }
                                break;
                        }
                    }
                }
                free(used);
            }
        }
        Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
    }

    if ((only_ui) && (!instructions)) {
        if (cl_funcname)
            *cl_funcname = (char *)"";
        return AnsiString();
    }

    code += ")\n";
    code += pre_decl;
    code += decl;
    code += "\n";
    return func + code;
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MetaCode, 1, 3)
    T_DELEGATE(0)

    static AnsiString err;
    AnsiString cl_funcname;

    int  param_len = 0;
    int  level     = 0xFFFF;
    void *THIS_REF = 0;
    void *THIS_VAR = 0;
    GET_METACONTAINER

    if (LOCAL_CONTEXT[0]) {
        INTEGER type2 = 0;
        char    *cdata;
        NUMBER  nfill;

        GetVariable(PARAMETER(0), &type2, &cdata, &nfill);
        THIS_REF = cdata;
        CREATE_VARIABLE(THIS_VAR);
        SetVariable(THIS_VAR, VARIABLE_CLASS, cdata, 0);
    }
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        level = PARAM_INT(1);
    }
    std::map<unsigned int, int> mapped_functions;
    std::map<unsigned int, int> *target = &mapped_functions;
    char only_ui = 0;
    if (level < 0) {
        level  *= -1;
        target  = &mc->mapped_functions;
        only_ui = 1;
    }
    AnsiString code = GenerateCode(level, THIS_REF, THIS_VAR, PARAMETERS->HANDLER, PARAMETER(0), Invoke, err, target, &cl_funcname, only_ui);
    if (THIS_VAR) {
        FREE_VARIABLE(THIS_VAR);
    }
    if (PARAMETERS_COUNT > 2) {
        SET_STRING(2, cl_funcname.c_str());
    }
    if (err.Length()) {
        RETURN_STRING("");
        return (void *)err.c_str();
    }
    RETURN_STRING(code.c_str());
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(protect, 0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(IsClient, 0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RegisterLooper, 1)
    T_DELEGATE(0)
    GET_METACONTAINER
    int res = mc->AddLooper(PARAMETER(0), Invoke);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SetOnConnectionChanged, 1)
    T_DELEGATE(0)
    GET_METACONTAINER
    int res = mc->SetOnConnectionChanged(PARAMETER(0), Invoke);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UnregisterLooper, 1)
    T_NUMBER(0)
    GET_METACONTAINER
    mc->RemoveLooper(PARAM_INT(0) - 1, Invoke);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(LinkInfo, 2, 4)
    GET_METACONTAINER
        SET_NUMBER(0, mc->data_in);
    SET_NUMBER(1, mc->data_out);
    if (PARAMETERS_COUNT > 2) {
        SET_NUMBER(2, mc->rt_in);
        if (PARAMETERS_COUNT > 3) {
            SET_NUMBER(3, mc->rt_out);
        }
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GetProtoSocket, 0)
    RETURN_NUMBER(CLIENT_SOCKET);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GetRTSocket, 0)
    GET_METACONTAINER
        RETURN_NUMBER(mc->RTSOCKET);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GetRTPeer, 0)
    GET_METACONTAINER
        CREATE_ARRAY(RESULT);
    if (mc->RTSOCKET > 0) {
        int        port = 0;
        const char *ip  = "";
        char       str[INET6_ADDRSTRLEN + 1];
        str[0] = 0;
        str[INET6_ADDRSTRLEN] = 0;
        if (mc->remote_conceptudpaddr.ss_family == AF_INET) {
            port = ntohs(((struct sockaddr_in *)&mc->remote_conceptudpaddr)->sin_port);
#ifdef _WIN32
            ip = inet_ntoa(((struct sockaddr_in *)&mc->remote_conceptudpaddr)->sin_addr);
#else
            ip = inet_ntop(AF_INET, &((struct sockaddr_in *)&mc->remote_conceptudpaddr)->sin_addr, str, INET6_ADDRSTRLEN);
#endif
        } else
        if (mc->remote_conceptudpaddr.ss_family == AF_INET6) {
            port = ntohs(((struct sockaddr_in6 *)&mc->remote_conceptudpaddr)->sin6_port);
#ifdef _WIN32
            ip = inet_ntop2(AF_INET6, &((struct sockaddr_in6 *)&mc->remote_conceptudpaddr)->sin6_addr, str, INET6_ADDRSTRLEN);
#else
            ip = inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&mc->remote_conceptudpaddr)->sin6_addr, str, INET6_ADDRSTRLEN);
#endif
        }
        if ((port) && (ip) && (ip[0])) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "address", (INTEGER)VARIABLE_STRING, (char *)ip, (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "port", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)port);
        }
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(EnableRTSend, 1)
    T_NUMBER(0)

    GET_METACONTAINER
    mc->rt_send_enabled = PARAM_INT(0);

    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(HasNewProtoSocket, 0)
    GET_METACONTAINER
    int new_socket = Invoke(INVOKE_GET_PROTO_SOCKET, PARAMETERS->HANDLER);

    if (CLIENT_SOCKET != new_socket) {
        mc->ResetSocket(CLIENT_SOCKET);
        mc->last_msg_seconds = time(NULL);
        CLIENT_SOCKET        = new_socket;
        ResetConceptPeer(mc, new_socket);
        UpdateTLSSocket(mc, new_socket);
        mc->send_failed = 0;
        mc->echo_sent   = 0;
        mc->force_exit  = 0;
        mc->ConnectionChanged(Invoke, new_socket);
        RETURN_NUMBER(1)
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SetInitializationVectors, 2)
    T_STRING(0)
    T_STRING(1)

    GET_METACONTAINER

    int len_send = PARAM_LEN(0);
    if (len_send > 32)
        len_send = 32;
    int len_recv = PARAM_LEN(1);
    if (len_recv > 32)
        len_recv = 32;
#ifdef __WITH_INTEL_LIB
    memcpy(mc->out_init_vector, PARAM(0), len_send);
    memcpy(mc->in_init_vector, PARAM(1), len_recv);
#endif
    memcpy(mc->EncryptAes.buffer, PARAM(0), len_send);
    memcpy(mc->EncryptAes.buffer, PARAM(1), len_recv);
END_IMPL
//------------------------------------------------------------------------
