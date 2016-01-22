//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
extern "C" {
#include <ares.h>
#include <ares_dns.h>
#include <ares_dns.h>
#include "nameser.h"
}
#ifdef _WIN32
 #include <winsock2.h>
#else
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
#endif
//=====================================================================================//
INVOKE_CALL InvokePtr = 0;
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
#ifdef _WIN32
    WORD    wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);
#endif
    ares_library_init(ARES_LIB_INIT_ALL);

    DEFINE_ECONSTANT(ns_c_invalid)
    DEFINE_ECONSTANT(ns_c_in)
    DEFINE_ECONSTANT(ns_c_2)
    DEFINE_ECONSTANT(ns_c_chaos)
    DEFINE_ECONSTANT(ns_c_hs)
    DEFINE_ECONSTANT(ns_c_none)
    DEFINE_ECONSTANT(ns_c_any)
    DEFINE_ECONSTANT(ns_c_max)

    DEFINE_ECONSTANT(ns_t_invalid)
    DEFINE_ECONSTANT(ns_t_a)
    DEFINE_ECONSTANT(ns_t_ns)
    DEFINE_ECONSTANT(ns_t_md)
    DEFINE_ECONSTANT(ns_t_mf)
    DEFINE_ECONSTANT(ns_t_cname)
    DEFINE_ECONSTANT(ns_t_soa)
    DEFINE_ECONSTANT(ns_t_mb)
    DEFINE_ECONSTANT(ns_t_mg)
    DEFINE_ECONSTANT(ns_t_mr)
    DEFINE_ECONSTANT(ns_t_null)
    DEFINE_ECONSTANT(ns_t_wks)
    DEFINE_ECONSTANT(ns_t_ptr)
    DEFINE_ECONSTANT(ns_t_hinfo)
    DEFINE_ECONSTANT(ns_t_minfo)
    DEFINE_ECONSTANT(ns_t_mx)
    DEFINE_ECONSTANT(ns_t_txt)
    DEFINE_ECONSTANT(ns_t_rp)
    DEFINE_ECONSTANT(ns_t_afsdb)
    DEFINE_ECONSTANT(ns_t_x25)
    DEFINE_ECONSTANT(ns_t_isdn)
    DEFINE_ECONSTANT(ns_t_rt)
    DEFINE_ECONSTANT(ns_t_nsap)
    DEFINE_ECONSTANT(ns_t_nsap_ptr)
    DEFINE_ECONSTANT(ns_t_sig)
    DEFINE_ECONSTANT(ns_t_key)
    DEFINE_ECONSTANT(ns_t_px)
    DEFINE_ECONSTANT(ns_t_gpos)
    DEFINE_ECONSTANT(ns_t_aaaa)
    DEFINE_ECONSTANT(ns_t_loc)
    DEFINE_ECONSTANT(ns_t_nxt)
    DEFINE_ECONSTANT(ns_t_eid)
    DEFINE_ECONSTANT(ns_t_nimloc)
    DEFINE_ECONSTANT(ns_t_srv)
    DEFINE_ECONSTANT(ns_t_atma)
    DEFINE_ECONSTANT(ns_t_naptr)
    DEFINE_ECONSTANT(ns_t_kx)
    DEFINE_ECONSTANT(ns_t_cert)
    DEFINE_ECONSTANT(ns_t_a6)
    DEFINE_ECONSTANT(ns_t_dname)
    DEFINE_ECONSTANT(ns_t_sink)
    DEFINE_ECONSTANT(ns_t_opt)
    DEFINE_ECONSTANT(ns_t_apl)
    DEFINE_ECONSTANT(ns_t_ds)
    DEFINE_ECONSTANT(ns_t_sshfp)
    DEFINE_ECONSTANT(ns_t_rrsig)
    DEFINE_ECONSTANT(ns_t_nsec)
    DEFINE_ECONSTANT(ns_t_dnskey)
    DEFINE_ECONSTANT(ns_t_tkey)
    DEFINE_ECONSTANT(ns_t_tsig)
    DEFINE_ECONSTANT(ns_t_ixfr)
    DEFINE_ECONSTANT(ns_t_axfr)
    DEFINE_ECONSTANT(ns_t_mailb)
    DEFINE_ECONSTANT(ns_t_maila)
    DEFINE_ECONSTANT(ns_t_any)
    DEFINE_ECONSTANT(ns_t_zxfr)
    DEFINE_ECONSTANT(ns_t_max)
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        ares_library_cleanup();
#ifdef _WIN32
        WSACleanup();
#endif
    }
    return 0;
}
//=====================================================================================//
void dns_callback(void *arg, int status, int timeouts, struct hostent *host) {
    if (status == ARES_SUCCESS) {
        if (host->h_name)
            InvokePtr(INVOKE_SET_VARIABLE, arg, (INTEGER)VARIABLE_STRING, host->h_name, (NUMBER)0);
        else
            InvokePtr(INVOKE_SET_VARIABLE, arg, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
    }
}

#ifdef _WIN32
const char *inet_ntop(int af, const void *src, char *dst, int cnt) {
    struct sockaddr_in6 srcaddr;

    memset(&srcaddr, 0, sizeof(struct sockaddr_in6));
    memcpy(&(srcaddr.sin6_addr), src, sizeof(srcaddr.sin6_addr));

    srcaddr.sin6_family = af;
    if (WSAAddressToString((struct sockaddr *)&srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD)&cnt) != 0) {
        DWORD rv = WSAGetLastError();
        return NULL;
    }
    return dst;
}
#endif

static const char *inet_ntop4(int add_type, const unsigned char *src, char *dst, size_t size) {
    static const char fmt[] = "%u.%u.%u.%u";
    char tmp[sizeof("255.255.255.255")];

    if (add_type == AF_INET6)
        return inet_ntop(add_type, src, dst, size);

    if ((size_t)sprintf(tmp, fmt, src[0], src[1], src[2], src[3]) >= size) {
        return NULL;
    }
    strcpy(dst, tmp);
    return dst;
}

void dns_callback2(void *arg, int status, int timeouts, struct hostent *host) {
    if (status == ARES_SUCCESS) {
        char **p;
        char addr_buf[46] = "??";
        for (p = host->h_addr_list; *p; p++) {
            char addr_buf[0xFFF] = "??";

            inet_ntop4(host->h_addrtype, (const unsigned char *)*p, addr_buf, sizeof(addr_buf));
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arg, host->h_name, (INTEGER)VARIABLE_STRING, addr_buf, (NUMBER)0);
        }
    }
}

static void callback_ns(void *arg, int status, int timeouts, unsigned char *abuf, int alen) {
    struct hostent *host = NULL;

    int                 id, qr, opcode, aa, tc, rd, ra, rcode;
    unsigned int        qdcount, ancount, nscount, arcount, i;
    const unsigned char *aptr;

    if (abuf) {
        if (alen < HFIXEDSZ)
            return;
        id      = DNS_HEADER_QID(abuf);
        qr      = DNS_HEADER_QR(abuf);
        opcode  = DNS_HEADER_OPCODE(abuf);
        aa      = DNS_HEADER_AA(abuf);
        tc      = DNS_HEADER_TC(abuf);
        rd      = DNS_HEADER_RD(abuf);
        ra      = DNS_HEADER_RA(abuf);
        rcode   = DNS_HEADER_RCODE(abuf);
        qdcount = DNS_HEADER_QDCOUNT(abuf);
        ancount = DNS_HEADER_ANCOUNT(abuf);
        nscount = DNS_HEADER_NSCOUNT(abuf);
        arcount = DNS_HEADER_ARCOUNT(abuf);

        struct hostent       *host = 0;
        struct ares_addrttl  addrttls[0xFF];
        struct ares_addr6ttl addr6ttls[0xFF];
        int naddrttls = 0xFF;

        void *VAR;
        char **p;

        int rc = ares_parse_a_reply(abuf, alen, &host, addrttls, &naddrttls);
        if (rc == ARES_SUCCESS) {
            if (host) {
                for (p = host->h_addr_list; *p; p++) {
                    InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
                    InvokePtr(INVOKE_CREATE_ARRAY, VAR);
                    char addr_buf[0xFFF] = "??";
                    inet_ntop4(host->h_addrtype, (const unsigned char *)*p, addr_buf, sizeof(addr_buf));
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "A", (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "host", (INTEGER)VARIABLE_STRING, host->h_name ? host->h_name : "", (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "addr", (INTEGER)VARIABLE_STRING, addr_buf, (NUMBER)0);
                }
                ares_free_hostent(host);
            }
        }

        naddrttls = 0xFF;
        rc        = ares_parse_aaaa_reply(abuf, alen, &host, addr6ttls, &naddrttls);
        if (rc == ARES_SUCCESS) {
            if (host) {
                for (p = host->h_addr_list; *p; p++) {
                    InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
                    InvokePtr(INVOKE_CREATE_ARRAY, VAR);
                    char addr_buf[0xFFF] = "??";
                    inet_ntop(host->h_addrtype, (const unsigned char *)*p, addr_buf, sizeof(addr_buf));
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "AAAA", (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "host", (INTEGER)VARIABLE_STRING, host->h_name ? host->h_name : "", (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "addr", (INTEGER)VARIABLE_STRING, addr_buf, (NUMBER)0);
                }
                ares_free_hostent(host);
            }
        }

        rc = ares_parse_ptr_reply(abuf, alen, &addrttls[0], sizeof(addrttls), AF_INET, &host);
        if (rc == ARES_SUCCESS) {
            if (host) {
                for (p = host->h_addr_list; *p; p++) {
                    InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
                    InvokePtr(INVOKE_CREATE_ARRAY, VAR);
                    char addr_buf[0xFFF] = "??";
                    inet_ntop4(host->h_addrtype, (const unsigned char *)*p, addr_buf, sizeof(addr_buf));
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "PTR", (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "host", (INTEGER)VARIABLE_STRING, host->h_name ? host->h_name : "", (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "addr", (INTEGER)VARIABLE_STRING, addr_buf, (NUMBER)0);
                }
                ares_free_hostent(host);
            }
        }

        rc = ares_parse_ptr_reply(abuf, alen, &addr6ttls[0], sizeof(addr6ttls), AF_INET6, &host);
        if (rc == ARES_SUCCESS) {
            if (host) {
                InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
                InvokePtr(INVOKE_CREATE_ARRAY, VAR);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "PTR6", (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "host", (INTEGER)VARIABLE_STRING, host->h_name ? host->h_name : "", (NUMBER)0);
                ares_free_hostent(host);
            }
        }

        rc = ares_parse_ns_reply(abuf, alen, &host);
        if (rc == ARES_SUCCESS) {
            if (host) {
                for (p = host->h_addr_list; *p; p++) {
                    InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
                    InvokePtr(INVOKE_CREATE_ARRAY, VAR);
                    char addr_buf[0xFFF] = "??";
                    inet_ntop4(host->h_addrtype, (const unsigned char *)*p, addr_buf, sizeof(addr_buf));
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "NS", (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "host", (INTEGER)VARIABLE_STRING, host->h_name ? host->h_name : "", (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "addr", (INTEGER)VARIABLE_STRING, addr_buf, (NUMBER)0);
                }
                ares_free_hostent(host);
            }
        }

        struct ares_srv_reply *srv_out;
        rc = ares_parse_srv_reply(abuf, alen, &srv_out);
        if (rc == ARES_SUCCESS) {
            struct ares_srv_reply *iteratorsrv = srv_out;
            if (srv_out) {
                while (iteratorsrv) {
                    InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
                    InvokePtr(INVOKE_CREATE_ARRAY, VAR);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "SRV", (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "host", (INTEGER)VARIABLE_STRING, iteratorsrv->host, (NUMBER)0);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "priority", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)iteratorsrv->priority);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "weight", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)iteratorsrv->weight);
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "port", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)iteratorsrv->port);

                    iteratorsrv = iteratorsrv->next;
                }

                ares_free_data(srv_out);
            }
        }

        struct ares_txt_reply *starttxt;
        struct ares_txt_reply *iteratortxt;
        rc = ares_parse_txt_reply(abuf, alen, &starttxt);
        if (rc == ARES_SUCCESS) {
            iteratortxt = starttxt;
            while (iteratortxt) {
                InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
                InvokePtr(INVOKE_CREATE_ARRAY, VAR);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "TXT", (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "txt", (INTEGER)VARIABLE_STRING, iteratortxt->txt, (NUMBER)iteratortxt->length);

                iteratortxt = iteratortxt->next;
            }
            if (starttxt)
                ares_free_data(starttxt);
        }

        struct ares_mx_reply *startmx;
        struct ares_mx_reply *iteratormx;
        rc = ares_parse_mx_reply(abuf, alen, &startmx);
        if (rc == ARES_SUCCESS) {
            iteratormx = startmx;
            while (iteratormx) {
                InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
                InvokePtr(INVOKE_CREATE_ARRAY, VAR);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "MX", (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "host", (INTEGER)VARIABLE_STRING, iteratormx->host, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "priority", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)iteratormx->priority);

                iteratormx = iteratormx->next;
            }
            if (iteratormx)
                ares_free_data(iteratormx);
        }

        struct ares_naptr_reply *start;
        struct ares_naptr_reply *iterator;
        rc = ares_parse_naptr_reply(abuf, alen, &start);
        if (rc == ARES_SUCCESS) {
            iterator = start;
            while (iterator) {
                InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
                InvokePtr(INVOKE_CREATE_ARRAY, VAR);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "NAPTR", (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "flags", (INTEGER)VARIABLE_STRING, iterator->flags, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "service", (INTEGER)VARIABLE_STRING, iterator->service, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "regexp", (INTEGER)VARIABLE_STRING, iterator->regexp, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "replacement", (INTEGER)VARIABLE_STRING, iterator->replacement, (NUMBER)0);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "order", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)iterator->order);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "preference", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)iterator->preference);
                iterator = iterator->next;
            }
            if (start)
                ares_free_data(start);
        }

        struct ares_soa_reply *startsoa;
        rc = ares_parse_soa_reply(abuf, alen, &startsoa);
        if (rc == ARES_SUCCESS) {
            InvokePtr(INVOKE_ARRAY_VARIABLE, arg, (INTEGER)InvokePtr(INVOKE_GET_ARRAY_COUNT, arg), &VAR);
            InvokePtr(INVOKE_CREATE_ARRAY, VAR);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "type", (INTEGER)VARIABLE_STRING, "SOA", (NUMBER)0);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "nsname", (INTEGER)VARIABLE_STRING, startsoa->nsname, (NUMBER)0);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "hostmaster", (INTEGER)VARIABLE_STRING, startsoa->hostmaster, (NUMBER)0);

            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "serial", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)startsoa->serial);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "refresh", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)startsoa->refresh);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "retry", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)startsoa->retry);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "expire", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)startsoa->expire);
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, VAR, "minttl", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)startsoa->minttl);
            if (startsoa)
                ares_free_data(startsoa);
        }
    }
}

void main_loop(ares_channel& channel, int timeout = 1000) {
    int            nfds, count;
    fd_set         readers, writers;
    timeval        tv, *tvp;
    struct timeval maxtime;

    maxtime.tv_sec  = timeout / 1000;
    maxtime.tv_usec = timeout % 1000;
    while (1) {
        FD_ZERO(&readers);
        FD_ZERO(&writers);
        nfds = ares_fds(channel, &readers, &writers);
        if (nfds == 0)
            break;
        tvp   = ares_timeout(channel, &maxtime, &tv);
        count = select(nfds, &readers, &writers, NULL, tvp);
        ares_process(channel, &readers, &writers);
        if (!count)
            break;
    }
}

CONCEPT_FUNCTION_IMPL(DNSReversed, 1)
    T_STRING(0)
    ares_channel channel;
    struct in_addr ip;
    int            res;
#ifdef _WIN32
    ip.s_addr = inet_addr(PARAM(0));
#else
    inet_aton(PARAM(0), &ip);
#endif
    RETURN_STRING("");
    if ((res = ares_init(&channel)) != ARES_SUCCESS)
        return 0;

    ares_gethostbyaddr(channel, &ip, sizeof ip, AF_INET, dns_callback, RESULT);
    main_loop(channel);
    ares_destroy(channel);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DNSResolve, 1)
    T_STRING(0)
    ares_channel channel;
    int res;

    CREATE_ARRAY(RESULT);
    if ((res = ares_init(&channel)) != ARES_SUCCESS)
        return 0;

    ares_gethostbyname(channel, PARAM(0), AF_INET, dns_callback2, RESULT);
    main_loop(channel);
    ares_destroy(channel);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(DNSQuery, 1, 3)
    T_STRING(0)
    ares_channel channel;
    int      res;
    ns_class cls = ns_c_any;
    ns_type  typ = ns_t_any;

    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        cls = (ns_class)PARAM_INT(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(1)
        typ = (ns_type)PARAM_INT(1);
    }

    CREATE_ARRAY(RESULT);
    if ((res = ares_init(&channel)) != ARES_SUCCESS)
        return 0;

    ares_query(channel, PARAM(0), cls, typ, callback_ns, RESULT);
    main_loop(channel);
    ares_destroy(channel);
END_IMPL
//=====================================================================================//
