#include "ConceptClient.h"
#include "messages.h"
#include "BasicMessages.h"
#include <time.h>

#ifdef _WIN32
 #ifndef IPV6_V6ONLY
  #define IPV6_V6ONLY    27
 #endif
 #define SHUT_RDWR       SD_BOTH
 #include <wincrypt.h>
#else
 #include <sys/select.h>
 #include <time.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
#endif
#ifdef __linux__
 #include <linux/random.h>
#endif
#include "tls_root_ca.h"

#define DEBUG_SAY_WHERE
#define MAX_GET_TIMEOUT    3000
#define FAST_TCP

#define TRAY_NOTIFY(x, y)

static const char base_values[65]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

RECV_CALLBACK CConceptClient::recv = NULL;
SEND_CALLBACK CConceptClient::send = NULL;
SOCK_EOF_CALLBACK CConceptClient::eof = NULL;
CLOSE_CALLBACK CConceptClient::close = NULL;
CONNECT_CALLBACK CConceptClient::connect = NULL;

int CConceptClient::sock_eof2(int stream, int sec) {
    if ((stream == INVALID_SOCKET) || (stream < 0))
        return -1;

    struct timeval timeout;

    timeout.tv_sec  = sec;
    timeout.tv_usec = 0;
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
}

int CConceptClient::timedout_recv(SOCKET _socket, char *buffer, int size, int flags, int timeout) {
    int res;

    do {
        if (CConceptClient::eof)
            res = CConceptClient::eof(_socket, 1);
        else
            res = sock_eof2(_socket, 1);

        if (!res)
            return ::recv(_socket, buffer, size, flags);
        timeout--;
        if (timeout > 0)
#ifdef _WIN32
            Sleep(1000);
#else
            usleep(1000000);
#endif
    } while (timeout > 0);
    return -1;
}

CConceptClient::CConceptClient(CALLBACK_FUNC cb, int secured, int debug, PROGRESS_API _notify) {
    this->CONCEPT_CALLBACK     = cb;
    this->secured              = secured;
    this->debug                = debug;
    this->connection_err       = false;
    this->notify               = _notify;
    this->RTSOCKET             = INVALID_SOCKET;
    this->CLIENT_SOCKET        = INVALID_SOCKET;
    this->UserData             = 0;
    this->parser               = NULL;
    this->last_length          = 0;
    this->tls                  = NULL;
    this->ManageTLS            = 1;
    this->NON_OPAQUE_CONCEPT_CALLBACK = NULL;
    SEMAPHORE_INIT(sem);
#ifdef _WIN32
    WSADATA data;
    WSAStartup(MAKEWORD(1, 1), &data);
#endif
}

int CConceptClient::Send(char *buf, int len, bool real_time) {
    if ((secured) && (tls))
        return SSL_write(tls, buf, len);
    if (CConceptClient::send)
        return CConceptClient::send(CLIENT_SOCKET, buf, len, 0);
    return ::send(CLIENT_SOCKET, buf, len, 0);
}

int CConceptClient::BlockingSend(char *buf, int len, bool real_time) {
    if (real_time)
        return this->Send(buf, len, real_time);

    int bytes_sent = 0;
    while (len > 0) {
        int res = this->Send(buf + bytes_sent, len, real_time);
        if (res <= 0)
            return res;
        bytes_sent += res;
        len -= bytes_sent;
    }
    return bytes_sent;
}

int CConceptClient::Recv(char *buf, int len) {
    if ((secured) && (tls)) {
        int res = SSL_read(tls, buf, len);
        return res;
    } else
    if (CConceptClient::recv)
        return CConceptClient::recv(CLIENT_SOCKET, buf, len, 0);

    return ::recv(CLIENT_SOCKET, buf, len, 0);
}

int CConceptClient::RecvTimeout(char *buffer, int size, int timeout) {
    int res;
    do {
        if ((tls) && (SSL_pending(tls))) {
            res = 0;
        } else
        if (CConceptClient::eof) {
            res = CConceptClient::eof(CLIENT_SOCKET, 1);
        } else
            res = sock_eof2(CLIENT_SOCKET, 1);

        if (!res)
            return this->Recv(buffer, size);

        timeout--;
        if (timeout > 0)
#ifdef _WIN32
            Sleep(1000);
#else
            usleep(1000000);
#endif
    } while (timeout > 0);
    return -1;
}

int CConceptClient::IsEOF() {
    if ((CLIENT_SOCKET == INVALID_SOCKET) || (CLIENT_SOCKET < 0))
        return 0;

    if ((tls) && (SSL_pending(tls)))
        return 0;

    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 100;
    fd_set socks;
    int sel_val;
    
    if (CConceptClient::eof) {
        int is_eof = CConceptClient::eof(CLIENT_SOCKET, 0);
        if ((!is_eof) || (RTSOCKET == INVALID_SOCKET))
            return is_eof;
        FD_ZERO(&socks);
        FD_SET(RTSOCKET, &socks);
        sel_val = select(FD_SETSIZE, &socks, 0, 0, &timeout);
        return !sel_val;
    }

    FD_ZERO(&socks);
    FD_SET(CLIENT_SOCKET, &socks);
    if (RTSOCKET != INVALID_SOCKET) {
        FD_SET(RTSOCKET, &socks);
    }
    sel_val = select(FD_SETSIZE, &socks, 0, 0, &timeout);

    return !sel_val;
}

int CConceptClient::connect2(int socket, struct sockaddr *addr, socklen_t length) {
    struct timeval tv_timeout;

    tv_timeout.tv_sec  = 5;
    tv_timeout.tv_usec = 0;

#ifdef _WIN32
    DWORD  tv_ms = 5000;
    u_long iMode = 1;
    ioctlsocket(socket, FIONBIO, &iMode);
#else
    int opts = fcntl(socket, F_GETFL);
    fcntl(socket, F_SETFL, opts | O_NONBLOCK);
#endif
    int            res = ::connect(socket, addr, length);
    fd_set         fdset;
    struct timeval tv;
    FD_ZERO(&fdset);
    FD_SET(socket, &fdset);
    tv.tv_sec  = 5;
    tv.tv_usec = 0;

    int so_error = -1;
    if (select(socket + 1, NULL, &fdset, NULL, &tv) == 1) {
        socklen_t len = sizeof so_error;

#ifdef _WIN32
        getsockopt(socket, SOL_SOCKET, SO_ERROR, (char *)&so_error, &len);
#else
        getsockopt(socket, SOL_SOCKET, SO_ERROR, &so_error, &len);
#endif
        if (so_error != 0) {
            if (CConceptClient::close) {
                CConceptClient::close(socket);
            } else {
                shutdown(socket, SHUT_RDWR);
#ifdef _WIN32
                closesocket(socket);
#else
                ::close(socket);
#endif
            }
        } else {
#ifdef _WIN32
            iMode = 0;
            ioctlsocket(socket, FIONBIO, &iMode);
#else
            fcntl(socket, F_SETFL, opts);
#endif
        }
#ifdef _WIN32
        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_ms, sizeof(tv_ms));
        setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv_ms, sizeof(tv_ms));
#else
        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_timeout, sizeof(struct timeval));
        setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv_timeout, sizeof(struct timeval));
#endif
        return so_error;
    } else {
#ifdef _WIN32
        iMode = 0;
        ioctlsocket(socket, FIONBIO, &iMode);
#else
        fcntl(socket, F_SETFL, opts);
#endif
    }

#ifdef _WIN32
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_ms, sizeof(tv_ms));
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv_ms, sizeof(tv_ms));
#else
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_timeout, sizeof(struct timeval));
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv_timeout, sizeof(struct timeval));
#endif
    return so_error;
}

int CConceptClient::Connect(const char *HOST, int PORT) {
    this->connection_err = false;
    int is_reconnect = 0;
    if (HOST) {
        this->PORT = PORT;
        this->lastHOST = HOST;
    } else {
        HOST = this->lastHOST.c_str();
        is_reconnect = 1;
    }

    if (!HOST)
        return 0;

    int flag = 1;


    if (CConceptClient::connect) {
        unsigned char is_secured = secured;
        CLIENT_SOCKET = CConceptClient::connect(-1, HOST, PORT, is_reconnect, &is_secured);
        if (CLIENT_SOCKET <= 0)
            return 0;
        if ((secured) && (is_secured))
            this->ManageTLS = 0;
        else
            this->ManageTLS = 1;
    } else {
        int ipv6 = 0;

        const char *ptr = HOST;
        while ((ptr) && (*ptr)) {
            if (*ptr == ':') {
                ipv6 = 1;
                break;
            }
            ptr++;
        }
        if (ipv6) {
            struct addrinfo hints;
            struct addrinfo *res, *result;

            memset(&hints, 0, sizeof hints);
            hints.ai_family   = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            AnsiString port_str((long)PORT);

            if (getaddrinfo(HOST, port_str.c_str(), &hints, &result) != 0)
                return 0;

            int connected = 0;
            for (res = result; res != NULL; res = res->ai_next) {
                char hostname[NI_MAXHOST] = "";
                if ((res->ai_family == AF_INET) || (res->ai_family == AF_INET6)) {
                    int error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
                    if (!error) {
                        if ((CLIENT_SOCKET = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == INVALID_SOCKET)
                            return 0;
#ifdef IPV6_V6ONLY
                        int ipv6only = 0;
                        setsockopt(CLIENT_SOCKET, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&ipv6only, sizeof(ipv6only));
#endif
                        int res_c = connect2(CLIENT_SOCKET, res->ai_addr, res->ai_addrlen);
                        if (res_c != INVALID_SOCKET) {
                            memcpy(&this->last_addr, &res->ai_addr, res->ai_addrlen);
                            this->last_length = res->ai_addrlen;
                            connected         = 1;
                            break;
                        } else {
                            if (CConceptClient::close) {
                                CConceptClient::close(CLIENT_SOCKET);
                            } else {
                                shutdown(CLIENT_SOCKET, SHUT_RDWR);
#ifdef _WIN32
                                closesocket(CLIENT_SOCKET);
#else
                                close(CLIENT_SOCKET);
#endif
                            }
                            CLIENT_SOCKET = INVALID_SOCKET;
                        }
                    }
                }
            }

            if (result)
                freeaddrinfo(result);

            if (!connected)
                return 0;
        } else {
            struct hostent *hp;

            if ((hp = gethostbyname(HOST)) == 0)
                return 0;
            struct sockaddr_in sin;
            memset(&sin, 0, sizeof(sin));
            sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
            sin.sin_family      = AF_INET;
            sin.sin_port        = htons(PORT);

            if ((CLIENT_SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
                return 0;

            if (connect2(CLIENT_SOCKET, (struct sockaddr *)&sin, sizeof(sin)) == INVALID_SOCKET)
                return 0;

            memcpy(&this->last_addr, &sin, sizeof(sin));
            this->last_length = sizeof(sin);
        }
    }
#ifdef FAST_TCP
    setsockopt(CLIENT_SOCKET,
                IPPROTO_TCP,
                TCP_NODELAY,
                (char *)&flag,
                sizeof(int));
#endif
    setsockopt(CLIENT_SOCKET,
                SOL_SOCKET,
                SO_KEEPALIVE,
                (char *)&flag,
                sizeof(int));

#ifdef _WIN32
    DWORD timeout = 10000;
#else
    struct timeval timeout;
    timeout.tv_sec  = 10;
    timeout.tv_usec = 0;
#endif
    setsockopt(CLIENT_SOCKET,
                SOL_SOCKET,
                SO_SNDTIMEO,
                (char *)&timeout,
                sizeof(timeout));

    setsockopt(CLIENT_SOCKET,
                SOL_SOCKET,
                SO_RCVTIMEO,
                (char *)&timeout,
                sizeof(timeout));

    setsockopt(CLIENT_SOCKET,
                SOL_SOCKET,
                SO_KEEPALIVE,
                (char *)&flag,
                sizeof(int));

    struct linger so_linger;
    so_linger.l_onoff  = 1;
    so_linger.l_linger = 10;

    setsockopt(CLIENT_SOCKET,
                SOL_SOCKET,
                SO_LINGER,
                (char *)&so_linger,
                sizeof(so_linger));
    if (secured) {
        if (this->ManageTLS) {
            if (tls) {
                SSL_CTX_free(tls);
                tls = NULL;
            }
            
            struct TLSContext *tls_local = SSL_CTX_new(SSLv3_client_method());
            if (tls_local) {
                tls_load_root_certificates(tls_local, (const unsigned char *)ROOT_CA_DEF, ROOT_CA_DEF_LEN);
                SSL_CTX_set_verify(tls_local, SSL_VERIFY_PEER, tls_default_verify);
                // to rewrite (it uses connect)
                SSL_set_fd(tls_local, CLIENT_SOCKET);
                SSL_set_io(tls_local, (void *)CConceptClient::recv, (void *)CConceptClient::send);
                tls_sni_set(tls_local, HOST);
                if (SSL_connect(tls_local) < 0) {
                    SSL_CTX_free(tls_local);
                    return 0;
                }
                tls = tls_local;
            } else
                return 0;
        }
    }

    Called_HOST = HOST;
    return 1;
}

int CConceptClient::CheckReconnect() {
    this->connection_err = true;
    this->Disconnect();

    if (!secured)
        return -2;

    if ((this->SessionID.Length() > 1) && (lastHOST.Length())) {
        if (this->Connect(NULL, PORT)) {
            int err = this->Run(NULL, -1, this->SessionID.c_str());
            // don't try again
            if (err == 403)
                return -2;

            if (err)
                return 0;

            // restored!
            return 1;
        } else
            return 0;
    }
    return -2;
}

int CConceptClient::StackLen() {
    return this->MessageStack.Count();
}

void CConceptClient::QueueMessage(char *sender, unsigned char sender_len, int MSG, char *target, unsigned short target_len, char *value, unsigned long value_len) {
    Parameters *P = new Parameters;

    if (sender_len > 0)
        P->Sender.LoadBuffer(sender, sender_len);
    P->ID     = MSG;
    if (target_len)
        P->Target.LoadBuffer(target, target_len);
    if (value_len)
        P->Value.LoadBuffer(value, value_len);
    P->Owner  = this;
    P->Flags  = 0;
    SEMAPHORE_LOCK(sem);
    this->MessageStack.Add(P, DATA_MESSAGE);
    SEMAPHORE_UNLOCK(sem);
}

int CConceptClient::SendPending() {
    SEMAPHORE_LOCK(sem);
    int count = 0;
    while (this->MessageStack.Count() && (this->CLIENT_SOCKET > 0)) {
        Parameters *TOSEND = (Parameters *)this->MessageStack.Remove(0);
        send_message(this, TOSEND->Sender, TOSEND->ID, TOSEND->Target, TOSEND->Value, (PROGRESS_API)notify);
        delete TOSEND;
        count++;
    }
    SEMAPHORE_UNLOCK(sem);
    return count;
}

long CConceptClient::SendFileMessage(char *sender, int MSG, char *target, char *filename, char force) {
    Parameters *P = new Parameters;

    P->Sender = sender;
    P->ID     = MSG;
    P->Target = target;
    P->Value  = filename;
    P->Owner  = this;
    P->Flags  = 0;
    SEMAPHORE_LOCK(sem);
    this->MessageStack.Add(P, DATA_MESSAGE);
    SEMAPHORE_UNLOCK(sem);
    return 0;
}

int CConceptClient::Disconnect() {
    if (CLIENT_SOCKET == INVALID_SOCKET)
        return 0;

    int res;
    if (CConceptClient::close) {
        res = CConceptClient::close(CLIENT_SOCKET);
    } else {
        shutdown(CLIENT_SOCKET, SHUT_RDWR);
#ifdef _WIN32
        res = closesocket(CLIENT_SOCKET);
#else
        res = ::close(CLIENT_SOCKET);
#endif
    }
    CLIENT_SOCKET = INVALID_SOCKET;
    if (RTSOCKET > 0) {
#ifdef _WIN32
        closesocket(RTSOCKET);
#else
        ::close(RTSOCKET);
#endif
        RTSOCKET = INVALID_SOCKET;
    }
    if (tls) {
        SSL_CTX_free(tls);
        tls = NULL;
    }
    return res;
}

int CConceptClient::ParseCode(const char *buf) {
    if ((!buf) || (!buf[0]))
        return -1;

    const char *str = strchr(buf, ' ');
    if (str) {
        str++;
        return atoi(str);
    }
    return -2;
}

int CConceptClient::Run(const char *app_name, int pipe, char *previous_session) {
    if (app_name)
        lastApp = app_name;
    else
        app_name = lastApp.c_str();

    if (!app_name)
        return -1;
    char buffer[0xFFFF];
    buffer[0] = 0;
    int buffer_size;

    char ws_key[25];
    ws_key[22] = '=';
    ws_key[23] = '=';
    ws_key[24] = 0;

    srand(time(NULL));
    // not crypto-important, any random function is fine
    for (int i = 0; i < sizeof(ws_key) - 3; i++)
        ws_key[i] = base_values[rand() % 64];

    if (previous_session)
        buffer_size = snprintf(buffer, sizeof(buffer), "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Concept/5.0\r\nUpgrade: WebSocket\r\nSec-WebSocket-Extensions: rtc\r\nConnection: keep-alive, Upgrade\r\nSec-WebSocket-Key: %s\r\nSec-WebSocket-Protocol: restore\r\nCookie: _s=%s\r\n\r\n", app_name, lastHOST.c_str(), ws_key, previous_session);
    else
    if ((debug) || (pipe > 0))
        buffer_size = snprintf(buffer, sizeof(buffer), "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Concept/5.0\r\nUpgrade: WebSocket\r\nSec-WebSocket-Extensions: rtc\r\nConnection: keep-alive, Upgrade\r\nSec-WebSocket-Key: %s\r\nSec-WebSocket-Protocol: %i_%i\r\n\r\n", app_name, lastHOST.c_str(), ws_key, (int)pipe, (int)debug);
    else
        buffer_size = snprintf(buffer, sizeof(buffer), "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Concept/5.0\r\nUpgrade: WebSocket\r\nSec-WebSocket-Extensions: rtc\r\nConnection: keep-alive, Upgrade\r\nSec-WebSocket-Key: %s\r\n\r\n", app_name, lastHOST.c_str(), ws_key);

    int sent = 0;
    while (sent < buffer_size) {
        int res = Send(buffer + sent, buffer_size);
        
        if (res <= 0)
            return -1;
        sent += res;
        buffer_size -= res;
    }
                
    buffer[0] = 0;
    int buf_pos = 0;
    buffer_size = 0;

    do {
        // slow but clean
        buf_pos = this->RecvTimeout(buffer + buffer_size, 1, 5);
        if (buf_pos <= 0)
            return -1;
        buffer_size += buf_pos;
        if (buffer_size >= sizeof(buffer))
            return -1;
        buffer[buffer_size] = 0;
    } while (!strstr(buffer, "\r\n\r\n"));
    // HTTP/1.1 101 ?
    int http_code = ParseCode(buffer);
    switch (http_code) {
        case 101:
            break;
        case 302:
            {
                char *location = strstr(buffer, "Location:");
                if (location) {
                    char *end = strstr(location, "\r\n");
                    if (end) {
                        location += 10;
                        end[0] = 0;
                        this->Disconnect();
                        lastRedirect = location;
                        return 302;
                    }
                }
            }
            // no break here;
        default:
            return 403;
    }
    char *cookies = strstr(buffer, "Set-Cookie:");
    if (cookies) {
        char *end = strstr(cookies, "\r\n");
        if (end) {
            end[0] = 0;
            char *cookie = strstr(cookies, "_s=");
            if (cookie) {
                cookie += 3;
                end = strchr(cookie, ';');
                if (end)
                    end[0] = 0;
                SessionID = cookie;
            }
        }
    }

    // no error
    return 0;
}

CConceptClient::~CConceptClient(void) {
    this->Disconnect();
    if (this->parser)
        delete this->parser;
#ifdef _WIN32
    WSACleanup();
#endif
    SEMAPHORE_DONE(sem);
}
