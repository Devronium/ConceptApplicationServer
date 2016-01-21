extern "C" {
#include "httpd.h"
#include "http_config.h"
#include "http_main.h"
#include "http_protocol.h"
#include "http_core.h"
#include "apr_strings.h"
//#include "apr_buckets.h"
#include "http_log.h"

#include "rsa/rsa.h"
#define PROTO_LIST(PROTOTYPE)    PROTOTYPE
#define UINT4    unsigned long
#include "md5/md5.h"

#include "time.h"
}

#define         MY_APR_BUCKET_IS_EOS(e)    (((e)->type) && (!strcmp((e)->type->name, "EOS")))

#ifdef _WIN32
 #ifndef IPV6_V6ONLY
  #define IPV6_V6ONLY    27
 #endif
#endif

#include <map>
#include "AnsiString.h"
#include "mtrand.h"
#include "semhh.h"

#define FAST_TCP
#define RANDOM_KEY_SIZE         16

#ifndef DEFAULT_HOST_STRING
 #define DEFAULT_HOST_STRING    "localhost"
#endif

#ifndef DEFAULT_HOST_PORT
 #define DEFAULT_HOST_PORT    2662
#endif

#ifndef DEFAULT_HTTP_TIMEOUT
 #define DEFAULT_HTTP_TIMEOUT    30
#endif

extern "C" {
static void *create_mod_bridge_config(apr_pool_t *p, server_rec *s);
static int mod_bridge_method_handler(request_rec *r);
static const char *set_mod_bridge_string(cmd_parms *parms, void *mconfig, const char *arg, const char *arg2, const char *arg3);
static void mod_bridge_register_hooks(apr_pool_t *p);
}

static const command_rec mod_bridge_cmds[] =
{
    AP_INIT_TAKE3(
        "ConceptServerHost",
        (const char *(*)())set_mod_bridge_string,
        NULL,
        RSRC_CONF,
        "CAS hostname, CAS port, CAS/HTTP bridge timeout"
        ),
    { NULL }
};

module AP_MODULE_DECLARE_DATA bridge_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    create_mod_bridge_config,
    NULL,
    mod_bridge_cmds,
    mod_bridge_register_hooks,
};

struct ltstr {
    bool operator()(const char *s1, const char *s2) const {
        return strcmp(s1, s2) < 0;
    }
};

HHSEM connection_lock;
static std::map<char *, int, ltstr> connections;
HHSEM lastevents_lock;
static std::map<char *, time_t, ltstr> lastevents;

typedef struct {
    char *host;
    int  port;
    int  timeout;
} mod_bridge_config;

#ifndef max
 #define max(a, b)    a > b ? a : b;
#endif

static const unsigned char *memmem_boyermoore_simplified
    (const unsigned char *haystack, size_t hlen,
    const unsigned char *needle, size_t nlen) {
    size_t occ[UCHAR_MAX + 1];
    size_t a, b, hpos;

    if ((nlen > hlen) || (nlen <= 0) || !haystack || !needle) return NULL;

    for (a = 0; a < UCHAR_MAX + 1; ++a) occ[a] = nlen;

    for (a = 0, b = nlen; a < nlen; ++a) occ[needle[a]] = --b;

    for (hpos = nlen - 1; hpos < hlen; ) {
        size_t npos = nlen - 1;

        while (haystack[hpos] == needle[npos]) {
            if (npos == 0) return haystack + hpos;
            --hpos;
            --npos;
        }
        hpos += max(nlen - npos, occ[haystack[hpos]]);
    }
    return NULL;
}

static int Connect(char *HOST, int PORT) {
    int CLIENT_SOCKET = -1;

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
        hints.ai_family   = AF_UNSPEC; // AF_INET or AF_INET6 to force version
        hints.ai_socktype = SOCK_STREAM;
        //hints.ai_flags = AI_PASSIVE;

        AnsiString port_str((long)PORT);

        if (getaddrinfo(HOST, port_str.c_str(), &hints, &result) != 0)
            return -1;

        int connected = 0;
        for (res = result; res != NULL; res = res->ai_next) {
            char hostname[NI_MAXHOST] = "";
            if ((res->ai_family == AF_INET) || (res->ai_family == AF_INET6)) {
                int error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
                if (!error) {
                    if ((CLIENT_SOCKET = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == INVALID_SOCKET)
                        return -1;
#ifdef FAST_TCP
                    setsockopt(CLIENT_SOCKET,                     /* socket affected */
                               IPPROTO_TCP,                       /* set option at TCP level */
                               TCP_NODELAY,                       /* name of option */
                               (char *)&flag,                     /* the cast is historical cruft */
                               sizeof(int));                      /* length of option value */
#endif
                    setsockopt(CLIENT_SOCKET,
                               SOL_SOCKET,
                               SO_KEEPALIVE,
                               (char *)&flag,
                               sizeof(int));
#ifdef IPV6_V6ONLY
                    int ipv6only = 0;
                    setsockopt(CLIENT_SOCKET, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&ipv6only, sizeof(ipv6only));
#endif

                    int res_c = connect(CLIENT_SOCKET, res->ai_addr, res->ai_addrlen);
                    if (res_c != INVALID_SOCKET) {
                        connected = 1;
                        break;
                    } else {
                        closesocket(CLIENT_SOCKET);
                        CLIENT_SOCKET = INVALID_SOCKET;
                    }
                }
            }
        }

        if (result)
            freeaddrinfo(result);

        if (!connected)
            return -1;
    } else {
        struct sockaddr_in sin;
        struct hostent     *hp;

        if ((hp = gethostbyname(HOST)) == 0)
            return -1;

        memset(&sin, 0, sizeof(sin));
        //INADDR_ANY;
        sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
        sin.sin_family      = AF_INET;
        sin.sin_port        = htons(PORT);

        // creez socket-ul
        if ((CLIENT_SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
            return -1;

        int flag = 1;
#ifdef FAST_TCP
        setsockopt(CLIENT_SOCKET,                     /* socket affected */
                   IPPROTO_TCP,                       /* set option at TCP level */
                   TCP_NODELAY,                       /* name of option */
                   (char *)&flag,                     /* the cast is historical cruft */
                   sizeof(int));                      /* length of option value */
#endif
        setsockopt(CLIENT_SOCKET,
                   SOL_SOCKET,
                   SO_KEEPALIVE,
                   (char *)&flag,
                   sizeof(int));

        if (connect(CLIENT_SOCKET, (struct sockaddr *)&sin, sizeof(sin)) == INVALID_SOCKET)
            return -1;
    }
    return CLIENT_SOCKET;
}

static AnsiString GenerateRandomAESKey(int len) {
    unsigned long init[4] = { time(NULL), time(NULL) >> 8, time(NULL) >> 16, time(NULL) >> 24 }, length = 4;
    MTRand_int32  irand(init, length);
    char          str[sizeof(long) + 1];

    str[sizeof(long)] = 0;
    AnsiString res;
    for (int i = 0; i < len / 4; i++) {
        *(long *)str = irand();
        for (int j = 0; j < sizeof(long); j++)
            while (!str[j])
                str[j] = (char)irand();
        res += str;
    }
    return res;
}

static int Negociate(int CLIENT_SOCKET) {
    AnsiString LOCAL_PUBLIC_KEY  = GenerateRandomAESKey(RANDOM_KEY_SIZE);
    AnsiString LOCAL_PRIVATE_KEY = LOCAL_PUBLIC_KEY;

    long cert_size = 0;
    int  received  = 0;
    int  rec_tot   = 0;

    do {
        received = recv(CLIENT_SOCKET, (char *)&cert_size + rec_tot, sizeof(long) - rec_tot, 0);
        rec_tot += received;
    } while ((received > 0) && (rec_tot < sizeof(long)));
    cert_size = ntohl(cert_size);

    if ((received < 1) || (rec_tot != sizeof(long)))
        return 0;

    received = 0;
    rec_tot  = 0;
    char buffer[4096];
    do {
        received = recv(CLIENT_SOCKET, buffer + rec_tot, cert_size - rec_tot, 0);
        rec_tot += received;
    } while ((received > 0) && (rec_tot < cert_size));

    if ((received < 1) || (rec_tot != cert_size))
        return 0;

    buffer[rec_tot] = 0;
    AnsiString REMOTE_PUBLIC_KEY = buffer;

    int  in_content_size  = LOCAL_PUBLIC_KEY.Length();
    int  out_content_size = (((in_content_size / 48) + 1) * 64) + 256;
    char *out_content     = new char[out_content_size + 1];

    int encrypt_result = RSA_encrypt(LOCAL_PUBLIC_KEY.c_str(), in_content_size,
                                     out_content, out_content_size,
                                     REMOTE_PUBLIC_KEY.c_str(), REMOTE_PUBLIC_KEY.Length());
    if (!encrypt_result)
        delete[] out_content;

    long size     = encrypt_result;
    long size_net = htonl(size);
    send(CLIENT_SOCKET, (char *)&size_net, sizeof(long), 0);
    send(CLIENT_SOCKET, out_content, size, 0);
    delete[] out_content;
    return 1;
}

static int have_messages(int CLIENT_SOCKET) {
    struct timeval timeout;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 100000;
    fd_set socks;

    int sel_val;

    FD_ZERO(&socks);
    FD_SET(CLIENT_SOCKET, &socks);
    sel_val = select(FD_SETSIZE, &socks, 0, 0, &timeout);

    if (sel_val < 0)
        return -1;
    return sel_val > 0;
}

static int DoneSession(char *session) {
    int res = -1;

    semp(connection_lock);
    try {
        res = connections[session];
        connections.erase(session);
        lastevents.erase(session);
    } catch (std::exception& e) {
        // nothing
    }
    semv(connection_lock);
    if (res > 0) {
#ifdef _WIN32
        closesocket(res);
#else
        close(res);
#endif
    }
    return 0;
}

static time_t last_check = 0;
int CheckTimeouts(mod_bridge_config *s_cfg) {
    time_t now = time(0);

    if (now - last_check > s_cfg->timeout) {
        std::map<char *, time_t, ltstr>::const_iterator it;
        std::map<char *, time_t, ltstr>::const_iterator end = lastevents.end();

        semp(connection_lock);
        for (it = lastevents.begin(); it != end; ++it)
            if (now - it->second > s_cfg->timeout) {
                semv(connection_lock);
                DoneSession(it->first);
                return 1;
            }
        semv(connection_lock);
    }
    return 0;
}

static int InitSession(request_rec *r) {
    mod_bridge_config *s_cfg = (mod_bridge_config *)ap_get_module_config(r->server->module_config, &bridge_module);

    // remove at most 10 zombie connections
    int checks = 0;

    while ((CheckTimeouts(s_cfg)) && (++checks < 10));

    MD5_CTX       CTX;
    unsigned char md5_sum[16];

    MD5Init(&CTX);
    MD5Update(&CTX, (unsigned char *)r, (long)sizeof(request_rec));
    MD5Final(md5_sum, &CTX);

    unsigned char result[33];
    result[32] = 0;
    for (int i = 0; i < 16; i++) {
        unsigned char first = md5_sum[i] / 0x10;
        unsigned char sec   = md5_sum[i] % 0x10;

        if (first < 10)
            result[i * 2] = '0' + first;
        else
            result[i * 2] = 'a' + (first - 10);

        if (sec < 10)
            result[i * 2 + 1] = '0' + sec;
        else
            result[i * 2 + 1] = 'a' + (sec - 10);
    }

    if (s_cfg->host) {
        int res = Connect(s_cfg->host, s_cfg->port);
        if (res > 0) {
            if (Negociate(res) != 1) {
#ifdef _WIN32
                closesocket(res);
#else
                close(res);
#endif
                return DECLINED;
            } else {
                semp(connection_lock);
                connections[(char *)result] = res;
                lastevents[(char *)result]  = time(0);
                semv(connection_lock);
                char *line = apr_psprintf(r->pool, "%s", (char *)result);
                ap_rputs(line, r);
            }
        } else
            return DECLINED;
    }
    return OK;
}

static int GetSocket(char *session) {
    int res = -1;

    semp(connection_lock);
    try {
        res = connections[session];
        lastevents[session] = time(0);
    } catch (std::exception& e) {
        res = -2;
    }
    semv(connection_lock);
    if (!res)
        res = -1;
    return res;
}

static char *memstr(char *haystack, char *needle, int size) {
    char *p;
    char needlesize = strlen(needle);

    for (p = haystack; p <= (haystack - needlesize + size); p++)
        if (memcmp(p, needle, needlesize) == 0)
            return p;             /* found */
    return NULL;
}

static int forward_data(request_rec *r, int CLIENT_SOCK, char *session) {
    char buf[HUGE_STRING_LEN + 1];
    char *buf_ptr = 0;

    int res = 0;

    ap_setup_client_block(r, REQUEST_CHUNKED_DECHUNK);
    ap_should_client_block(r);
    AnsiString marker("\r\n");
    char       has_marker = 0;
    AnsiString buf_block;
    do {
        res = ap_get_client_block(r, buf, HUGE_STRING_LEN);
        if (res > 0)
            buf_block.AddBuffer(buf, res);
        if ((res == 0) && (buf_block.Length())) {
            if (!has_marker) {
                char *ptr = (char *)memmem_boyermoore_simplified((unsigned char *)buf_block.c_str(), buf_block.Length(), (unsigned char *)"\r\n", 2);
                if (ptr) {
                    int delta = (long)ptr - (long)buf_block.c_str();
                    if (delta > 0) {
                        AnsiString temp;
                        temp.LoadBuffer(buf_block.c_str(), delta);
                        marker    += temp;
                        has_marker = 1;
                    }
                }
            }
            if (has_marker) {
                char *ptr_pre = (char *)memmem_boyermoore_simplified((unsigned char *)buf_block.c_str(), buf_block.Length(), (unsigned char *)"\r\n\r\n", 4);
                char *ptr2    = (char *)memmem_boyermoore_simplified((unsigned char *)buf_block.c_str(), buf_block.Length(), (unsigned char *)marker.c_str(), marker.Length());
                if ((ptr2) && (ptr_pre)) {
                    int start_dif = (long)ptr_pre - (long)buf_block.c_str() + 4;
                    int end_dif   = (long)ptr2 - (long)buf_block.c_str();
                    int len       = end_dif - start_dif;
                    if (len > 0) {
                        char *p = buf_block.c_str() + start_dif;
                        p[len] = 0;
                        //ap_log_error(APLOG_MARK, APLOG_ERR, 0, r->server , "%s (%i)", p, len);
                        int total_sent = 0;
                        do {
                            int res = send(CLIENT_SOCK, p + total_sent, len - total_sent, 0);
                            if (res <= 0) {
                                DoneSession(session);
                                return DECLINED;
                            } else
                                total_sent += res;
                        } while (total_sent < len);
                    }
                }
            }
        }
    } while (res > 0);

    /*apr_bucket_brigade *brigade;
     * int seen_eos;
     *
     * brigade = apr_brigade_create(r->pool, r->connection->bucket_alloc);
     * seen_eos = 0;
     *
     * while (!seen_eos) {
     *  apr_status_t rv;
     *  apr_bucket *e;
     *
     *  rv = ap_get_brigade(r->input_filters, brigade, AP_MODE_READBYTES, APR_BLOCK_READ, HUGE_STRING_LEN);
     *  if (rv != APR_SUCCESS) {
     *      return DECLINED;
     *  }
     *  ap_log_error(APLOG_MARK, APLOG_ERR, 0, r->server , "%s", "Here !");
     *
     *  //while (!APR_BRIGADE_EMPTY(brigade)) {
     *  for(e = APR_BRIGADE_FIRST(bb); e != APR_BRIGADE_SENTINEL(bb); e=APR_BUCKET_NEXT(e))
     *
     *     ap_log_error(APLOG_MARK, APLOG_ERR, 0, r->server , "%s", "while !");
     *     apr_bucket *bucket;
     *     const char *data;
     *     apr_size_t data_len;
     *
     *     bucket = APR_BRIGADE_FIRST(brigade);
     *
     *     if (MY_APR_BUCKET_IS_EOS(bucket)) {
     *          seen_eos = 1;
     *          break;
     *     }
     *
     *     rv = apr_bucket_read(bucket, &data, &data_len, APR_BLOCK_READ);
     *     if (rv != APR_SUCCESS) {
     *          return DECLINED;
     *     }
     *
     *     apr_bucket_delete(bucket);
     *  }
     *  apr_brigade_cleanup(brigade);
     * }*/

    return OK;
}

static int mod_bridge_method_handler(request_rec *r) {
    // Send a message to stderr (apache redirects this to the error log)
    char *base       = strstr(r->uri, "/ConceptToHttp/");
    int  CLIENT_SOCK = -1;

    if (!base)
        return DECLINED;
    base += 15;
    if (base[0]) {
        char *session = 0;
        char *base2   = strstr(base, "/");
        if (base2) {
            base2[0] = 0;
            base2++;
            session = base;
            base    = base2;
        }
        if (session) {
            if ((!strcmp(base, "havemessage")) && (r->method_number == M_GET)) {
                CLIENT_SOCK = GetSocket(session);
                if (CLIENT_SOCK > 0) {
                    int hm = have_messages(CLIENT_SOCK);
                    if (hm >= 0) {
                        char *line = apr_psprintf(r->pool, "%i", hm);
                        ap_rputs(line, r);
                        return OK;
                    } else {
                        DoneSession(session);
                        return DECLINED;
                    }
                } else {
                    DoneSession(session);
                    return DECLINED;
                }
            } else
            if ((!strcmp(base, "getmessage")) && (r->method_number == M_GET)) {
                CLIENT_SOCK = GetSocket(session);
                if (CLIENT_SOCK > 0) {
                    apr_table_unset(r->headers_out, "Content-Type");
                    apr_table_set(r->headers_out, "Content-Type", "application/octet-stream");
                    do {
                        int hm = have_messages(CLIENT_SOCK);
                        if (hm >= 0) {
                            if (!hm) {
#ifdef _WIN32
                                Sleep(100);
#else
                                usleep(100000);
#endif
                                return OK;
                            }
                            char buffer[0xFFFF];
                            int  size = recv(CLIENT_SOCK, buffer, 0xFFFF, 0);
                            //ap_log_error(APLOG_MARK, APLOG_ERR, 0, r->server , "SIZE: %i for %i", size, CLIENT_SOCK);
                            if (size <= 0) {
                                DoneSession(session);
                                return DECLINED;
                            } else
                                ap_rwrite(buffer, size, r);
                        } else {
                            DoneSession(session);
                            return DECLINED;
                        }
                    } while (have_messages(CLIENT_SOCK) == 1);
                    return OK;
                } else {
                    DoneSession(session);
                    return DECLINED;
                }
            } else
            if ((!strcmp(base, "sendmessage")) && (r->method_number == M_POST)) {
                CLIENT_SOCK = GetSocket(session);
                if (CLIENT_SOCK > 0)
                    return forward_data(r, CLIENT_SOCK, session);
                else {
                    DoneSession(session);
                    return DECLINED;
                }
            } else
            if ((!strcmp(base, "API")) && (r->method_number == M_POST)) {
                CLIENT_SOCK = GetSocket(session);
                if (CLIENT_SOCK > 0)
                    return forward_data(r, CLIENT_SOCK, session);
                else {
                    DoneSession(session);
                    return DECLINED;
                }
                return OK;
            } else
            if (!strcmp(base, "diagnose")) {
                CLIENT_SOCK = GetSocket(session);
                char *line = apr_psprintf(r->pool, "Session: %s\nSocket: %i\n", session, CLIENT_SOCK);
                ap_rputs(line, r);
                return OK;
            } else {
                DoneSession(session);
                return DECLINED;
            }
        } else {
            if ((!strcmp(base, "initsession")) && (r->method_number == M_GET))
                return InitSession(r);
            else
                return DECLINED;
        }
    } else
        return DECLINED;

    return OK;
    //return dump_request(r);
}

static void mod_bridge_register_hooks(apr_pool_t *p) {
    seminit(connection_lock, 1);
    ap_hook_handler(mod_bridge_method_handler, NULL, NULL, APR_HOOK_LAST);
}

static const char *set_mod_bridge_string(cmd_parms *parms, void *mconfig, const char *arg, const char *arg2, const char *arg3) {
    mod_bridge_config *s_cfg = (mod_bridge_config *)ap_get_module_config(parms->server->module_config, &bridge_module);

    s_cfg->host    = (char *)arg;
    s_cfg->port    = atoi(arg2);
    s_cfg->timeout = atoi(arg3);
    return NULL;
}

static void *create_mod_bridge_config(apr_pool_t *p, server_rec *s) {
    mod_bridge_config *newcfg;

    newcfg          = (mod_bridge_config *)apr_pcalloc(p, sizeof(mod_bridge_config));
    newcfg->host    = DEFAULT_HOST_STRING;
    newcfg->port    = DEFAULT_HOST_PORT;
    newcfg->timeout = DEFAULT_HTTP_TIMEOUT;
    return (void *)newcfg;
}
