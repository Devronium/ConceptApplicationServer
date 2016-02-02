#include "ConceptClient.h"
#include "mtrand.h"
#include "messages.h"
#include "BasicMessages.h"
#include "CARoot.h"
extern "C" {
    #include "rsa/rsa.h"
}
#include <time.h>

#ifdef _WIN32
 #ifndef IPV6_V6ONLY
  #define IPV6_V6ONLY    27
 #endif
 #define SHUT_RDWR       SD_BOTH
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

#define DEBUG_SAY_WHERE
#define MAX_GET_TIMEOUT    3000
#define FAST_TCP

#define TRAY_NOTIFY(x, y)

RECV_CALLBACK CConceptClient::recv3 = NULL;
SEND_CALLBACK CConceptClient::send3 = NULL;
SOCK_EOF_CALLBACK CConceptClient::eof3 = NULL;
CLOSE_CALLBACK CConceptClient::close3 = NULL;
CONNECT_CALLBACK CConceptClient::connect3 = NULL;

int sock_eof(int stream) {
    if ((stream == INVALID_SOCKET) || (stream < 0))
        return -1;
    struct timeval timeout;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 10;
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
        if (CConceptClient::eof3)
            res = CConceptClient::eof3(_socket, 1);
        else
            res = sock_eof2(_socket, 1);

        if (!res)
            return recv(_socket, buffer, size, flags);
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

void CConceptClient::GenerateRandomAESKey(AnsiString *res, int len) {
    ResetMessages();
    char key[0xFFF];
    if (len > 0xFFF)
        len = 0xFFF;
#ifdef __APPLE__
    for (int i = 0; i < len; i++) {
        unsigned int v = arc4random() % 0x100;
        key[i] = (char)v;
    }
#else
    #ifdef __linux__
    if (getrandom(key, len, 0) == len) {
        res->LoadBuffer(key, len);
        return;
    }
    #endif
    FILE *fp = fopen("/dev/urandom", "r");
    if (fp) {
        int key_len = fread(key, 1, len, fp);
        fclose(fp);
        if (key_len == len) {
            res->LoadBuffer(key, len);
            return;
        }
    }

    // NOT SAFE - this should not be reached
    fprintf(stderr, "WARNING: Generated AES key is not safe");
    unsigned int t = time(NULL);
    t += (unsigned int)(intptr_t)res;
    unsigned int init[4] = { t, t >> 8, t >> 16, t >> 24 }, length = 4;
    MTRand_int32 irand(init, length);
    char         str[sizeof(int) + 1];

    str[sizeof(int)] = 0;
    for (int i = 0; i < len; i++)
        key[i] = (char)(abs(irand()) % 0x100);
#endif
    res->LoadBuffer(key, len);
}

CConceptClient::CConceptClient(CALLBACK_FUNC cb, int secured, int debug, PROGRESS_API _notify) {
    this->CONCEPT_CALLBACK     = cb;
    this->secured              = secured;
    this->debug                = debug;
    this->connection_err       = false;
    this->notify               = _notify;
    this->NON_CRITICAL_SECTION = 0;
    this->RTSOCKET             = INVALID_SOCKET;
    this->CLIENT_SOCKET        = INVALID_SOCKET;
    this->UserData             = 0;
    this->parser               = NULL;
    this->last_length          = 0;
#ifndef NOSSL
    this->in_ssl_neg           = false;
    this->sslctx               = NULL;
    this->ssl                  = NULL;
#endif
    seminit(sem, 1);
#ifdef _WIN32
    WSADATA data;
    WSAStartup(MAKEWORD(1, 1), &data);
#endif

#ifndef NOSSL
    sslctx     = NULL;
    ssl        = NULL;
    in_ssl_neg = false;

    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
#endif

#ifdef __WITH_TOMCRYPT
 #ifdef USE_LTM
    ltc_mp = ltm_desc;
 #elif defined(USE_TFM)
    ltc_mp = tfm_desc;
 #elif defined(USE_GMP)
    ltc_mp = gmp_desc;
 #else
    extern ltc_math_descriptor EXT_MATH_LIB;
    ltc_mp = EXT_MATH_LIB;
 #endif
    register_prng(&sprng_desc);
    register_hash(&sha1_desc);
#endif
}

void CConceptClient::ResetTLS() {
#ifndef NOSSL
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        ssl = NULL;
    }
#endif
}

int CConceptClient::InitTLS(VERIFY_API verify, int flags) {
#ifndef NOSSL
    in_ssl_neg = true;
    if (!sslctx) {
        sslctx = SSL_CTX_new(SSLv23_client_method());
        if (sslctx) {
            X509 *cert;
            BIO  *mem;
            mem = BIO_new(BIO_s_mem());
            if (mem) {
                BIO_puts(mem, DEVRONIUM_CA_ROOT);
                cert = PEM_read_bio_X509(mem, NULL, 0, NULL);
                BIO_free(mem);
                if (cert)
                    X509_STORE_add_cert(SSL_CTX_get_cert_store(sslctx), cert);
            }
        }
    }
    ResetTLS();
    SSL *ssl2 = SSL_new(sslctx);
    if (ssl2) {
        SSL_set_fd(ssl2, CLIENT_SOCKET);
        if (SSL_connect(ssl2) == 1) {
            bool is_ok = false;
            if ((!verify) || (verify(this, SSL_get_verify_result(ssl2), ssl2, flags)))
                is_ok = true;

            if (is_ok) {
                ssl        = ssl2;
                in_ssl_neg = false;
                return 1;
            }
        }
        SSL_shutdown(ssl2);
        SSL_free(ssl2);
        ssl2 = NULL;
    }
    in_ssl_neg = false;
#endif
    return 0;
}

int CConceptClient::Send(char *buf, int len) {
    if (CConceptClient::send3)
        return CConceptClient::send3(CLIENT_SOCKET, buf, len, 0);
#ifndef NOSSL
    if (ssl) {
        int res = SSL_write(ssl, buf, len);
        if (res <= 0)
            ERR_print_errors_fp(stderr);
        return res;
    }
#endif
    return send(CLIENT_SOCKET, buf, len, 0);
}

int CConceptClient::Recv(char *buf, int len) {
    if (CConceptClient::recv3)
        return CConceptClient::recv3(CLIENT_SOCKET, buf, len, 0);

#ifndef NOSSL
    while (in_ssl_neg) {
 #ifdef _WIN32
        Sleep(20);
 #else
        usleep(20000);
 #endif
    }
    if (ssl) {
        int res = SSL_read(ssl, buf, len);
        if (res < 0)
            ERR_print_errors_fp(stderr);
        return res;
    }
#endif
    return recv(CLIENT_SOCKET, buf, len, 0);
}

int CConceptClient::RecvTimeout(char *buffer, int size, int timeout) {
    int res;

    do {
        if ((ssl) && (SSL_pending(ssl)))
            res = 0;
        else {
            if (CConceptClient::eof3)
                res = CConceptClient::eof3(CLIENT_SOCKET, 1);
            else
                res = sock_eof2(CLIENT_SOCKET, 1);
        }

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

    if (CConceptClient::eof3)
        return CConceptClient::eof3(CLIENT_SOCKET, 0);
#ifndef NOSSL
    if ((ssl) && (SSL_pending(ssl)))
        return 0;
#endif
    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 100;
    fd_set socks;

    int sel_val;

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
    int            res = connect(socket, addr, length);
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
            if (CConceptClient::close3) {
                CConceptClient::close3(socket);
            } else {
                shutdown(socket, SHUT_RDWR);
                closesocket(socket);
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

int CConceptClient::Connect(char *HOST, int PORT) {
    this->connection_err = false;
    CConceptClient::PORT = PORT;
    lastHOST             = HOST;

    int flag = 1;
    int ipv6 = 0;

    char *ptr = HOST;
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
        hints.ai_family   = AF_UNSPEC;       // AF_INET or AF_INET6 to force version
        hints.ai_socktype = SOCK_STREAM;
        //hints.ai_flags = AI_PASSIVE;

        AnsiString port_str((long)PORT);

        if (getaddrinfo(HOST, port_str.c_str(), &hints, &result) != 0)
            //fprintf(stderr, "Unable to resolve specified host (getaddrinfo)\n");
            return 0;

        int connected = 0;
        for (res = result; res != NULL; res = res->ai_next) {
            char hostname[NI_MAXHOST] = "";
            if ((res->ai_family == AF_INET) || (res->ai_family == AF_INET6)) {
                int error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
                if (!error) {
                    if ((CLIENT_SOCKET = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == INVALID_SOCKET)
                        return 0;
#ifdef FAST_TCP
                    int flag2 = 0x10;
                    setsockopt(CLIENT_SOCKET,
                               IPPROTO_IP,
                               IP_TOS,
                               (char *)&flag2,
                               sizeof(int));


                    setsockopt(CLIENT_SOCKET,
                               IPPROTO_TCP,
                               TCP_NODELAY,
                               (char *)&flag,
                               sizeof(int));
#endif

                    struct linger so_linger;
                    so_linger.l_onoff  = 1;
                    so_linger.l_linger = 10;

                    setsockopt(CLIENT_SOCKET,
                               SOL_SOCKET,
                               SO_LINGER,
                               (char *)&so_linger,
                               sizeof(so_linger));

#ifdef IPV6_V6ONLY
                    int ipv6only = 0;
                    setsockopt(CLIENT_SOCKET, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&ipv6only, sizeof(ipv6only));
#endif
                    int res_c;
                    if (CConceptClient::connect3)
                        res_c = CConceptClient::connect3(CLIENT_SOCKET, res->ai_addr, res->ai_addrlen);
                    else
                        res_c = connect2(CLIENT_SOCKET, res->ai_addr, res->ai_addrlen);
                    if (res_c != INVALID_SOCKET) {
                        memcpy(&this->last_addr, &res->ai_addr, res->ai_addrlen);
                        this->last_length = res->ai_addrlen;
                        connected         = 1;
                        break;
                    } else {
                        if (CConceptClient::close3) {
                            CConceptClient::close3(CLIENT_SOCKET);
                        } else {
                            shutdown(CLIENT_SOCKET, SHUT_RDWR);
                            closesocket(CLIENT_SOCKET);
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
            // fprintf(stderr, "Unable to resolve specified host (gethostbyname): %s\n", HOST);
            return 0;
        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
        sin.sin_family      = AF_INET;
        sin.sin_port        = htons(PORT);

        if ((CLIENT_SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
            return 0;

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

        struct timeval timeout;
        timeout.tv_sec  = 10;
        timeout.tv_usec = 0;

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


        if (CConceptClient::connect3) {
            if (CConceptClient::connect3(CLIENT_SOCKET, (struct sockaddr *)&sin, sizeof(sin)) == INVALID_SOCKET)
                return 0;
        } else
        if (connect2(CLIENT_SOCKET, (struct sockaddr *)&sin, sizeof(sin)) == INVALID_SOCKET)
            return 0;

        memcpy(&this->last_addr, &sin, sizeof(sin));
        this->last_length = sizeof(sin);
    }

    GenerateRandomAESKey(&LOCAL_PUBLIC_KEY, RANDOM_KEY_SIZE);
    LOCAL_PRIVATE_KEY = LOCAL_PUBLIC_KEY;

    int cert_size = 0;
    int received  = 0;
    int rec_tot   = 0;
    do {
        received = timedout_recv(CLIENT_SOCKET, (char *)&cert_size + rec_tot, sizeof(int) - rec_tot, 0);
        rec_tot += received;
    } while ((received > 0) && (rec_tot < sizeof(int)));
    cert_size = ntohl(cert_size);

    if ((received < 1) || (rec_tot != sizeof(int)) || (cert_size > 0xFFFFF)) {
        if (CConceptClient::close3) {
            CConceptClient::close3(CLIENT_SOCKET);
        } else {
            shutdown(CLIENT_SOCKET, SHUT_RDWR);
            closesocket(CLIENT_SOCKET);
        }
        CLIENT_SOCKET = INVALID_SOCKET;
        return 0;
    }

    received = 0;
    rec_tot  = 0;
    char buffer[4096];
    do {
        received = timedout_recv(CLIENT_SOCKET, buffer + rec_tot, cert_size - rec_tot, 0);
        rec_tot += received;
    } while ((received > 0) && (rec_tot < cert_size));

    if ((received < 1) || (rec_tot != cert_size)) {
        if (CConceptClient::close3) {
            CConceptClient::close3(CLIENT_SOCKET);
        } else {
            shutdown(CLIENT_SOCKET, SHUT_RDWR);
            closesocket(CLIENT_SOCKET);
        }
        CLIENT_SOCKET = INVALID_SOCKET;
        return 0;
    }
    buffer[rec_tot] = 0;
    REMOTE_PUBLIC_KEY.LoadBuffer(buffer, rec_tot);

    AnsiString encrypted_key = this->Encrypt(LOCAL_PUBLIC_KEY.c_str(), LOCAL_PUBLIC_KEY.Length(), &REMOTE_PUBLIC_KEY);

    int size     = encrypted_key.Length();
    int size_net = (int)htonl(size);
    CConceptClient::mod_send(CLIENT_SOCKET, (char *)&size_net, sizeof(int), 0);
    CConceptClient::mod_send(CLIENT_SOCKET, encrypted_key.c_str(), size, 0);

    AnsiString session = "get session\r\n";
    CConceptClient::mod_send(CLIENT_SOCKET, session.c_str(), session.Length(), 0);

    AnsiString session_value;
    AnsiString session_id;
    char       tmp_ses[2];
    tmp_ses[1] = 0;
    int ses_recv   = 0;
    int iterations = MAX_GET_TIMEOUT;
    do {
        ses_recv = 0;
        int eof;
        if (CConceptClient::eof3)
            eof = CConceptClient::eof3(CLIENT_SOCKET, 0);
        else
            eof = sock_eof(CLIENT_SOCKET);
        if (eof < 0) {
            ses_recv = -1;
            break;
        }
        if (!eof) {
            ses_recv = timedout_recv(CLIENT_SOCKET, (char *)tmp_ses, 1, 0);
            if (ses_recv == 1) {
                iterations     = 0xFFFF;
                session_value += tmp_ses[0];
                if (session_value.Pos("\r\n") > 0) {
                    session_id.LoadBuffer(session_value.c_str(), session_value.Length() - 2);
                    break;
                }
            } else {
                ses_recv = -1;
                break;
            }
        } else {
            Sleep(100);
            if ((iterations != 0xFFFF) && (!session_value.Length()))
                iterations -= 100;
        }
    } while ((ses_recv >= 0) && (iterations > 0));

    if (ses_recv < 0) {
        if (CConceptClient::close3) {
            CConceptClient::close3(CLIENT_SOCKET);
        } else {
            shutdown(CLIENT_SOCKET, SHUT_RDWR);
            closesocket(CLIENT_SOCKET);
        }
        CLIENT_SOCKET = INVALID_SOCKET;
        return 0;
    }

    if ((session_id.Length() > 1) && (iterations))
        this->SessionID = session_id;

    // setting hostname
    AnsiString content = "set servername ";
    content += HOST;
    content += "\r\n";

    if (CConceptClient::mod_send(CLIENT_SOCKET, content.c_str(), content.Length(), 0) < 0) {
        if (CConceptClient::close3) {
            CConceptClient::close3(CLIENT_SOCKET);
        } else {
            shutdown(CLIENT_SOCKET, SHUT_RDWR);
            closesocket(CLIENT_SOCKET);
        }
        CLIENT_SOCKET = INVALID_SOCKET;
        return 0;
    }

    REMOTE_PUBLIC_KEY = LOCAL_PUBLIC_KEY;

    Called_HOST = HOST;

    return 1;
}

int CConceptClient::CheckReconnect() {
    if (RTSOCKET != INVALID_SOCKET) {
        closesocket(RTSOCKET);
        RTSOCKET = INVALID_SOCKET;
    }
    this->connection_err = true;
    if (this->SessionID.Length() > 1) {
        if (CLIENT_SOCKET != INVALID_SOCKET) {
            if (CConceptClient::close3) {
                CConceptClient::close3(CLIENT_SOCKET);
            } else {
                shutdown(CLIENT_SOCKET, SHUT_RDWR);
                closesocket(CLIENT_SOCKET);
            }
            CLIENT_SOCKET = INVALID_SOCKET;
        }
        SOCKET CLIENT_SOCKET2 = INVALID_SOCKET;
        this->connection_err = false;
        int flag = 1;
        int ipv6 = 0;

        char *ptr = this->lastHOST;
        while ((ptr) && (*ptr)) {
            if (*ptr == ':') {
                ipv6 = 1;
                break;
            }
            ptr++;
        }
        if (this->last_length <= 0) {
            if (ipv6) {
                struct addrinfo hints;
                struct addrinfo *res, *result;

                memset(&hints, 0, sizeof hints);
                hints.ai_family   = AF_UNSPEC;       // AF_INET or AF_INET6 to force version
                hints.ai_socktype = SOCK_STREAM;
                //hints.ai_flags = AI_PASSIVE;

                AnsiString port_str((long)PORT);

                if (getaddrinfo(this->lastHOST.c_str(), port_str.c_str(), &hints, &result) != 0)
                    return 0;

                int connected = 0;
                for (res = result; res != NULL; res = res->ai_next) {
                    char hostname[NI_MAXHOST] = "";
                    if ((res->ai_family == AF_INET) || (res->ai_family == AF_INET6)) {
                        int error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
                        if (!error) {
                            if ((CLIENT_SOCKET2 = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == INVALID_SOCKET)
                                return 0;
#ifdef FAST_TCP
                            setsockopt(CLIENT_SOCKET2,
                                       IPPROTO_TCP,
                                       TCP_NODELAY,
                                       (char *)&flag,
                                       sizeof(int));
#endif
                            setsockopt(CLIENT_SOCKET2,
                                       SOL_SOCKET,
                                       SO_KEEPALIVE,
                                       (char *)&flag,
                                       sizeof(int));

#ifdef IPV6_V6ONLY
                            int ipv6only = 0;
                            setsockopt(CLIENT_SOCKET2, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&ipv6only, sizeof(ipv6only));
#endif
                            int res_c;
                            if (CConceptClient::connect3)
                                res_c = CConceptClient::connect3(CLIENT_SOCKET2, res->ai_addr, res->ai_addrlen);
                            else
                                res_c = connect2(CLIENT_SOCKET2, res->ai_addr, res->ai_addrlen);
                            if (res_c != INVALID_SOCKET) {
                                connected = 1;
                                break;
                            } else {
                                if (CConceptClient::close3) {
                                    CConceptClient::close3(CLIENT_SOCKET2);
                                } else {
                                    shutdown(CLIENT_SOCKET2, SHUT_RDWR);
                                    closesocket(CLIENT_SOCKET2);
                                }
                                CLIENT_SOCKET2 = INVALID_SOCKET;
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

                if ((hp = gethostbyname(lastHOST.c_str())) == 0)
                    return 0;

                struct sockaddr_in sin;
                memset(&sin, 0, sizeof(sin));
                sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
                sin.sin_family      = AF_INET;
                sin.sin_port        = htons(PORT);

                if ((CLIENT_SOCKET2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
                    return 0;
#ifdef FAST_TCP
                setsockopt(CLIENT_SOCKET2,
                           IPPROTO_TCP,
                           TCP_NODELAY,
                           (char *)&flag,
                           sizeof(int));

                setsockopt(CLIENT_SOCKET2,
                           SOL_SOCKET,
                           SO_KEEPALIVE,
                           (char *)&flag,
                           sizeof(int));
#endif
                if (CConceptClient::connect3) {
                    if (CConceptClient::connect3(CLIENT_SOCKET2, (struct sockaddr *)&sin, sizeof(sin)) == INVALID_SOCKET)
                        return 0;
                } else
                if (connect2(CLIENT_SOCKET2, (struct sockaddr *)&sin, sizeof(sin)) == INVALID_SOCKET)
                    return 0;
            }
        } else {
            if ((CLIENT_SOCKET2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
                return 0;
#ifdef FAST_TCP
            setsockopt(CLIENT_SOCKET2,
                       IPPROTO_TCP,
                       TCP_NODELAY,
                       (char *)&flag,
                       sizeof(int));

            setsockopt(CLIENT_SOCKET2,
                       SOL_SOCKET,
                       SO_KEEPALIVE,
                       (char *)&flag,
                       sizeof(int));
#endif

            if (CConceptClient::connect3) {
                if (CConceptClient::connect3(CLIENT_SOCKET2, (struct sockaddr *)&this->last_addr, this->last_length) == INVALID_SOCKET)
                    return 0;
            } else
            if (connect2(CLIENT_SOCKET2, (struct sockaddr *)&this->last_addr, this->last_length) == INVALID_SOCKET)
                return 0;
        }

        LOCAL_PRIVATE_KEY = LOCAL_PUBLIC_KEY;

        int cert_size = 0;
        int received  = 0;
        int rec_tot   = 0;
        do {
            received = timedout_recv(CLIENT_SOCKET2, (char *)&cert_size + rec_tot, sizeof(int) - rec_tot, 0);
            rec_tot += received;
        } while ((received > 0) && (rec_tot < sizeof(int)));
        cert_size = ntohl(cert_size);

        if ((received < 1) || (rec_tot != sizeof(int)) || (cert_size > 0xFFFFF)) {
            if (CConceptClient::close3) {
                CConceptClient::close3(CLIENT_SOCKET2);
            } else {
                shutdown(CLIENT_SOCKET2, SHUT_RDWR);
                closesocket(CLIENT_SOCKET2);
            }
            CLIENT_SOCKET2 = INVALID_SOCKET;
            TRAY_NOTIFY("Concept protocol breach", "An invalid certificate size was received");
            return 0;
        }

#ifdef PRINT_STATUS
        //std::cerr << "\r[][][][]:::::::::: Receiving RSA certificate from server ...                \r";
#endif

        received = 0;
        rec_tot  = 0;
        char buffer[4096];
        do {
            received = timedout_recv(CLIENT_SOCKET2, buffer + rec_tot, cert_size - rec_tot, 0);
            rec_tot += received;
        } while ((received > 0) && (rec_tot < cert_size));

        if ((received < 1) || (rec_tot != cert_size)) {
            if (CConceptClient::close3) {
                CConceptClient::close3(CLIENT_SOCKET2);
            } else {
                shutdown(CLIENT_SOCKET2, SHUT_RDWR);
                closesocket(CLIENT_SOCKET2);
            }
            CLIENT_SOCKET2 = INVALID_SOCKET;
            //std::cerr << "\nConcept protocol breach (" << cert_size << " bytes waited, received " << rec_tot << ")";
            TRAY_NOTIFY("Concept protocol breach", "Broken connection while receiving public certificate");
            return 0;
        }
        buffer[rec_tot] = 0;
        REMOTE_PUBLIC_KEY.LoadBuffer(buffer, rec_tot);

#ifdef PRINT_STATUS
        //std::cerr << "\r[][][][][]:::::::: Securing ...                                              \r";
#endif

        AnsiString encrypted_key = this->Encrypt(LOCAL_PUBLIC_KEY.c_str(), LOCAL_PUBLIC_KEY.Length(), &REMOTE_PUBLIC_KEY);

        int size     = encrypted_key.Length();
        int size_net = (int)htonl(size);
        CConceptClient::mod_send(CLIENT_SOCKET2, (char *)&size_net, sizeof(int), 0);
        CConceptClient::mod_send(CLIENT_SOCKET2, encrypted_key.c_str(), size, 0);

        AnsiString session = "get restore ";
        session += this->SessionID;
        session += "\r\n";
        CConceptClient::mod_send(CLIENT_SOCKET2, session.c_str(), session.Length(), 0);

        AnsiString session_value;
        AnsiString session_id;
        char       tmp_ses[2];
        tmp_ses[1] = 0;
        int ses_recv   = 0;
        int iterations = MAX_GET_TIMEOUT;

        AnsiString response;
        do {
            ses_recv = 0;
            int eof;
            if (CConceptClient::eof3)
                eof = CConceptClient::eof3(CLIENT_SOCKET2, 0);
            else
                eof = sock_eof(CLIENT_SOCKET2);

            if (eof < 0) {
                ses_recv = -1;
                break;
            }
            if (!eof) {
                ses_recv = timedout_recv(CLIENT_SOCKET2, (char *)tmp_ses, 1, 0);
                if (ses_recv == 1) {
                    iterations     = 0xFFFF;
                    session_value += tmp_ses[0];
                    if (session_value.Pos("\r\n") > 0) {
                        response.LoadBuffer(session_value.c_str(), session_value.Length() - 2);
                        break;
                    }
                } else {
                    ses_recv = -1;
                    break;
                }
            } else {
                Sleep(100);
                if ((iterations != 0xFFFF) && (!session_value.Length()))
                    iterations -= 100;
            }
        } while ((ses_recv >= 0) && (iterations > 0));

        if (ses_recv < 0) {
            if (CConceptClient::close3) {
                CConceptClient::close3(CLIENT_SOCKET2);
            } else {
                shutdown(CLIENT_SOCKET2, SHUT_RDWR);
                closesocket(CLIENT_SOCKET2);
            }
            CLIENT_SOCKET2 = INVALID_SOCKET;
            //this->SessionID=(char *)"";
            return 0;
        }

        if (response.ToInt() == 1) {
            CLIENT_SOCKET = CLIENT_SOCKET2;
            if (ssl) {
                // rest + reinit tls
                InitTLS(NULL);
            }
            return 1;
        } else {
            if (CConceptClient::close3) {
                CConceptClient::close3(CLIENT_SOCKET2);
            } else {
                shutdown(CLIENT_SOCKET2, SHUT_RDWR);
                closesocket(CLIENT_SOCKET2);
            }
            this->SessionID = (char *)"";
        }
        return -1;
    }
    return -2;
}

AnsiString CConceptClient::Encrypt(char *buf, int size, AnsiString *key) {
    AnsiString res;

    if (key->Length() > 5) {
        AnsiString key_type;
        key_type.LoadBuffer(key->c_str(), 4);
#ifdef __WITH_TOMCRYPT
        int err;
        if (key_type == (char *)"rsa-") {
            rsa_key rkey;
            err = rsa_import((unsigned char *)key->c_str() + 4, key->Length() - 4, &rkey);
            if (err) {
                fprintf(stderr, "Error in RSA key: %s\n", (char *)error_to_string(err));
                return res;
            }
            int           in_content_size  = LOCAL_PUBLIC_KEY.Length();
            unsigned long out_content_size = 8192;
            char          *out_content     = (char *)malloc(out_content_size);

            int hash_idx = find_hash("sha1");
            int prng_idx = find_prng("sprng");
            err = rsa_encrypt_key((unsigned char *)buf, size, (unsigned char *)out_content, &out_content_size, (unsigned char *)"Concept", 7, NULL, prng_idx, hash_idx, &rkey);
            if (err) {
                fprintf(stderr, "Error in RSA encryption: %s\n", (char *)error_to_string(err));
            } else {
                res.LoadBuffer(out_content, out_content_size);
            }
            free(out_content);
            rsa_free(&rkey);
        } else
        if (key_type == (char *)"ecc-") {
            ecc_key rkey;
            err = ecc_import((unsigned char *)key->c_str() + 4, key->Length() - 4, &rkey);
            if (err) {
                fprintf(stderr, "Error in ECC key: %s\n", (char *)error_to_string(err));
                return res;
            }
            int           in_content_size  = LOCAL_PUBLIC_KEY.Length();
            unsigned long out_content_size = 8192;
            char          *out_content     = (char *)malloc(out_content_size);

            int hash_idx = find_hash("sha1");
            int prng_idx = find_prng("sprng");
            err = ecc_encrypt_key((unsigned char *)buf, size, (unsigned char *)out_content, &out_content_size, NULL, prng_idx, hash_idx, &rkey);
            if (err) {
                fprintf(stderr, "Error in ECC encryption: %s\n", (char *)error_to_string(err));
            } else {
                res.LoadBuffer(out_content, out_content_size);
            }
            free(out_content);
            ecc_free(&rkey);
        } else
#endif
        if (key_type == (char *)"----") {
            int  in_content_size  = LOCAL_PUBLIC_KEY.Length();
            int  out_content_size = 8192;
            char *out_content     = (char *)malloc(out_content_size);
            int  encrypt_result   = RSA_encrypt(buf, size, out_content, out_content_size, key->c_str(), key->Length());
            if (encrypt_result) {
                res.LoadBuffer(out_content, encrypt_result);
            } else {
                TRAY_NOTIFY("Invalid certificate", "An invalid certificate was received");
            }
            free(out_content);
        }
    }
    return res;
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
    semp(sem);
    this->MessageStack.Add(P, DATA_MESSAGE);
    semv(sem);
}

int CConceptClient::SendPending() {
    semp(sem);
    int count = 0;
    while (this->MessageStack.Count() && (this->CLIENT_SOCKET > 0)) {
        Parameters *TOSEND = (Parameters *)this->MessageStack.Remove(0);
        send_message(this, TOSEND->Sender, TOSEND->ID, TOSEND->Target, TOSEND->Value, this->CLIENT_SOCKET, this->secured ? this->REMOTE_PUBLIC_KEY.c_str() : 0, (PROGRESS_API)notify, false);
        delete TOSEND;
        count++;
    }
    semv(sem);
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
    semp(sem);
    this->MessageStack.Add(P, DATA_MESSAGE);
    semv(sem);
    return 0;
}

int CConceptClient::Disconnect() {
    if (CLIENT_SOCKET == INVALID_SOCKET)
        return 0;
    //send(CLIENT_SOCKET, "close\r\n", 7, 0);
    int res;
    if (CConceptClient::close3) {
        res = CConceptClient::close3(CLIENT_SOCKET);
    } else {
        shutdown(CLIENT_SOCKET, SHUT_RDWR);
        res = closesocket(CLIENT_SOCKET);
    }
    CLIENT_SOCKET = INVALID_SOCKET;
    if (RTSOCKET > 0) {
        closesocket(RTSOCKET);
        RTSOCKET = INVALID_SOCKET;
    }
    return res;
}

INTEGER CConceptClient::NoErrors() {
    return 1;
}

int CConceptClient::Run(char *app_name, int pipe) {
    AnsiString file(app_name);

    int pos = file.Pos("?");

    if (pos > 1)
        lastApp.LoadBuffer(file.c_str(), pos - 1);
    else
        lastApp = file;

    if (pipe > 0) {
        AnsiString szpipe = "pipe ";
        szpipe += AnsiString((long)pipe);
        szpipe += "\r\n";
        if (CConceptClient::mod_send(CLIENT_SOCKET, szpipe.c_str(), szpipe.Length(), 0) <= 0)
            return -1;
    }

    AnsiString S = "run ";
    if (!secured)
        S = "runfast ";
    if (debug)
        S = "rundebug ";
    S += app_name;

    S += (char *)"\r\n";
    // digest any unprocessed data
    while (!CConceptClient::mod_eof(CLIENT_SOCKET)) {
        char dummy[0xFF];
        int  ses_recv = timedout_recv(CLIENT_SOCKET, (char *)dummy, 0xFF, 0);
        if (ses_recv <= 0) {
            if (CConceptClient::close3) {
                CConceptClient::close3(CLIENT_SOCKET);
            } else {
                shutdown(CLIENT_SOCKET, SHUT_RDWR);
                closesocket(CLIENT_SOCKET);
            }
            CLIENT_SOCKET = INVALID_SOCKET;
            return -1;
        }
    }

    return CConceptClient::mod_send(CLIENT_SOCKET, S.c_str(), S.Length(), 0);
}

void CConceptClient::SetCritical(int status) {
    NON_CRITICAL_SECTION = status;
}

int CConceptClient::mod_send(SOCKET socket, const char *buffer, size_t length, int flags) {
    if (CConceptClient::send3)
        return CConceptClient::send3(socket, buffer, length, flags);
    return send(socket, buffer, length, flags);
}

int CConceptClient::mod_eof(SOCKET socket) {
        if (CConceptClient::eof3)
            return CConceptClient::eof3(socket, 0);

        return sock_eof(socket);
}

CConceptClient::~CConceptClient(void) {
    this->Disconnect();
    if (this->parser)
        delete this->parser;
#ifdef _WIN32
    WSACleanup();
#endif
    semdel(sem);
}
