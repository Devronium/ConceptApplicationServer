//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include "interface.h"
#ifdef _WIN32
 #ifdef _WIN32_WINNT
  #undef _WIN32_WINNT
 #endif
 #define _WIN32_WINNT    0x501

 #define _WINSOCKAPI_
 #include <io.h>
 #include <winsock2.h>
 #include <ws2tcpip.h>
 #include <map>

#ifndef SO_REUSEPORT
    #if (defined (LINUX) || defined (__linux__))
        #define SO_REUSEPORT    15
    #else
    #ifdef __FreeBSD__ 
        #define SO_REUSEPORT    0x00000200
    #endif
    #endif
#endif

#ifndef IPPROTO_SCTP
    #define IPPROTO_SCTP    132
#endif

#define WITHOUT_LIBSCTP

std::map<SYS_INT, int> UnixSocketEmulation;
const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt) {
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

int inet_pton(int af, const char *src, void *dst) {
    struct sockaddr_storage ss;
    int  size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN + 1];

    ZeroMemory(&ss, sizeof(ss));
    /* stupid non-const API */
    strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
        switch (af) {
            case AF_INET:
                *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
                return 1;

            case AF_INET6:
                *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
                return 1;
        }
    }
    return 0;
}

int getdomainname(char *const domain, unsigned int dsize) {
    HKEY  hkey;
    DWORD rc;
    DWORD valSize;

    /* Works for Win NT/2000/XP */
    rc = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        "System\\CurrentControlSet\\Services\\Tcpip\\Parameters",
        0,
        KEY_READ,
        &hkey
        );

    if (rc == ERROR_SUCCESS) {
        valSize = (DWORD)(dsize - 1);
        memset(domain, 0, dsize);
        rc = RegQueryValueEx(
            hkey,
            "DhcpDomain",
            NULL,
            NULL,
            (LPBYTE)domain,
            &valSize
            );

        if ((rc == ERROR_SUCCESS) && (domain[0] != '\0')) {
            RegCloseKey(hkey);
            return 0;
        }

        valSize = (DWORD)(dsize - 1);
        memset(domain, 0, dsize);
        rc = RegQueryValueEx(
            hkey,
            "Domain",
            NULL,
            NULL,
            (LPBYTE)domain,
            &valSize
            );

        if ((rc == ERROR_SUCCESS) && (domain[0] != '\0')) {
            RegCloseKey(hkey);
            return 0;
        }

        RegCloseKey(hkey);
    }

    rc = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        "System\\CurrentControlSet\\Services\\VxD\\MSTCP",
        0,
        KEY_READ,
        &hkey
        );

    if (rc == ERROR_SUCCESS) {
        valSize = (DWORD)(dsize - 1);
        memset(domain, 0, dsize);
        rc = RegQueryValueEx(
            hkey,
            "Domain",
            NULL,
            NULL,
            (LPBYTE)domain,
            &valSize
            );

        if ((rc == ERROR_SUCCESS) && (domain[0] != '\0')) {
            RegCloseKey(hkey);
            return 0;
        }

        RegCloseKey(hkey);
    }

    memset(domain, 0, dsize);
    return -1;
}

#else
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/un.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <string.h>
 #include <arpa/inet.h>
 #include <netinet/tcp.h>
 #include <sys/select.h>
 #include <sys/poll.h>
 #include <signal.h>
 #include <errno.h>
 #include <fcntl.h>
 #ifndef WITHOUT_LIBSCTP
    #ifdef __APPLE__
        #include <netinet/in.h>
    #else
        #include <netinet/sctp.h>
    #endif
 #endif

 #define INVALID_SOCKET    -1

 #ifndef UNIX_PATH_MAX
  #define UNIX_PATH_MAX    108
 #endif
#endif
#include "websocket.h"
#include "ping.h"

#ifndef IPV6_V6ONLY
 #define IPV6_V6ONLY    27
#endif

#define COPY_MEMBER(arr, events, eventname, member_type) \
    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, #eventname, &type, &str, &nr); \
    if (type == VARIABLE_NUMBER) \
        events->eventname = (member_type)nr;

//=====================================================================================//
#ifdef _WIN32
 #   if defined (EWOULDBLOCK) && (EWOULDBLOCK != WSAEWOULDBLOCK)
  #     undef EWOULDBLOCK
 #   endif/* EWOULDBLOCK */
 #   if defined (EINPROGRESS) && (EINPROGRESS != WSAEINPROGRESS)
  #     undef EINPROGRESS
 #   endif/* EINPROGRESS */
 #   if defined (EALREADY) && (EALREADY != WSAEALREADY)
  #     undef EALREADY
 #   endif/* EALREADY */
 #   if defined (ENOTSOCK) && (ENOTSOCK != WSAENOTSOCK)
  #     undef ENOTSOCK
 #   endif/* ENOTSOCK */
 #   if defined (EDESTADDRREQ) && (EDESTADDRREQ != WSAEDESTADDRREQ)
  #     undef EDESTADDRREQ
 #   endif/* EDESTADDRREQ */
 #   if defined (EMSGSIZE) && (EMSGSIZE != WSAEMSGSIZE)
  #     undef EMSGSIZE
 #   endif/* EMSGSIZE */
 #   if defined (EPROTOTYPE) && (EPROTOTYPE != WSAEPROTOTYPE)
  #     undef EPROTOTYPE
 #   endif/* EPROTOTYPE */
 #   if defined (ENOPROTOOPT) && (ENOPROTOOPT != WSAENOPROTOOPT)
  #     undef ENOPROTOOPT
 #   endif/* ENOPROTOOPT */
 #   if defined (EPROTONOSUPPORT) && (EPROTONOSUPPORT != WSAEPROTONOSUPPORT)
  #     undef EPROTONOSUPPORT
 #   endif/* EPROTONOSUPPORT */
 #   if defined (ESOCKTNOSUPPORT) && (ESOCKTNOSUPPORT != WSAESOCKTNOSUPPORT)
  #     undef ESOCKTNOSUPPORT
 #   endif/* ESOCKTNOSUPPORT */
 #   if defined (EOPNOTSUPP) && (EOPNOTSUPP != WSAEOPNOTSUPP)
  #     undef EOPNOTSUPP
 #   endif/* EOPNOTSUPP */
 #   if defined (EPFNOSUPPORT) && (EPFNOSUPPORT != WSAEPFNOSUPPORT)
  #     undef EPFNOSUPPORT
 #   endif/* EPFNOSUPPORT */
 #   if defined (EAFNOSUPPORT) && (EAFNOSUPPORT != WSAEAFNOSUPPORT)
  #     undef EAFNOSUPPORT
 #   endif/* EAFNOSUPPORT */
 #   if defined (EADDRINUSE) && (EADDRINUSE != WSAEADDRINUSE)
  #     undef EADDRINUSE
 #   endif/* EADDRINUSE */
 #   if defined (EADDRNOTAVAIL) && (EADDRNOTAVAIL != WSAEADDRNOTAVAIL)
  #     undef EADDRNOTAVAIL
 #   endif/* EADDRNOTAVAIL */
 #   if defined (ENETDOWN) && (ENETDOWN != WSAENETDOWN)
  #     undef ENETDOWN
 #   endif/* ENETDOWN */
 #   if defined (ENETUNREACH) && (ENETUNREACH != WSAENETUNREACH)
  #     undef ENETUNREACH
 #   endif/* ENETUNREACH */
 #   if defined (ENETRESET) && (ENETRESET != WSAENETRESET)
  #     undef ENETRESET
 #   endif/* ENETRESET */
 #   if defined (ECONNABORTED) && (ECONNABORTED != WSAECONNABORTED)
  #     undef ECONNABORTED
 #   endif/* ECONNABORTED */
 #   if defined (ECONNRESET) && (ECONNRESET != WSAECONNRESET)
  #     undef ECONNRESET
 #   endif/* ECONNRESET */
 #   if defined (ENOBUFS) && (ENOBUFS != WSAENOBUFS)
  #     undef ENOBUFS
 #   endif/* ENOBUFS */
 #   if defined (EISCONN) && (EISCONN != WSAEISCONN)
  #     undef EISCONN
 #   endif/* EISCONN */
 #   if defined (ENOTCONN) && (ENOTCONN != WSAENOTCONN)
  #     undef ENOTCONN
 #   endif/* ENOTCONN */
 #   if defined (ESHUTDOWN) && (ESHUTDOWN != WSAESHUTDOWN)
  #     undef ESHUTDOWN
 #   endif/* ESHUTDOWN */
 #   if defined (ETOOMANYREFS) && (ETOOMANYREFS != WSAETOOMANYREFS)
  #     undef ETOOMANYREFS
 #   endif/* ETOOMANYREFS */
 #   if defined (ETIMEDOUT) && (ETIMEDOUT != WSAETIMEDOUT)
  #     undef ETIMEDOUT
 #   endif/* ETIMEDOUT */
 #   if defined (ECONNREFUSED) && (ECONNREFUSED != WSAECONNREFUSED)
  #     undef ECONNREFUSED
 #   endif/* ECONNREFUSED */
 #   if defined (ELOOP) && (ELOOP != WSAELOOP)
  #     undef ELOOP
 #   endif/* ELOOP */
 #   if defined (EHOSTDOWN) && (EHOSTDOWN != WSAEHOSTDOWN)
  #     undef EHOSTDOWN
 #   endif/* EHOSTDOWN */
 #   if defined (EHOSTUNREACH) && (EHOSTUNREACH != WSAEHOSTUNREACH)
  #     undef EHOSTUNREACH
 #   endif/* EHOSTUNREACH */
 #   if defined (EPROCLIM) && (EPROCLIM != WSAEPROCLIM)
  #     undef EPROCLIM
 #   endif/* EPROCLIM */
 #   if defined (EUSERS) && (EUSERS != WSAEUSERS)
  #     undef EUSERS
 #   endif/* EUSERS */
 #   if defined (EDQUOT) && (EDQUOT != WSAEDQUOT)
  #     undef EDQUOT
 #   endif/* EDQUOT */
 #   if defined (ESTALE) && (ESTALE != WSAESTALE)
  #     undef ESTALE
 #   endif/* ESTALE */
 #   if defined (EREMOTE) && (EREMOTE != WSAEREMOTE)
  #     undef EREMOTE
 #   endif/* EREMOTE */
 #   if !defined (ETIME)
  #     define ETIME              ERROR_SEM_TIMEOUT
 #   endif/* !ETIME */
 #   if !defined (EWOULDBLOCK)
  #     define EWOULDBLOCK        WSAEWOULDBLOCK
 #   endif/* !EWOULDBLOCK */
 #   if !defined (EINPROGRESS)
  #     define EINPROGRESS        WSAEINPROGRESS
 #   endif/* !EINPROGRESS */
 #   if !defined (EALREADY)
  #     define EALREADY           WSAEALREADY
 #   endif/* !EALREADY */
 #   if !defined (ENOTSOCK)
  #     define ENOTSOCK           WSAENOTSOCK
 #   endif/* !ENOTSOCK */
 #   if !defined (EDESTADDRREQ)
  #     define EDESTADDRREQ       WSAEDESTADDRREQ
 #   endif/* !EDESTADDRREQ */
 #   if !defined (EMSGSIZE)
  #     define EMSGSIZE           WSAEMSGSIZE
 #   endif/* !EMSGSIZE */
 #   if !defined (EPROTOTYPE)
  #     define EPROTOTYPE         WSAEPROTOTYPE
 #   endif/* !EPROTOTYPE */
 #   if !defined (ENOPROTOOPT)
  #     define ENOPROTOOPT        WSAENOPROTOOPT
 #   endif/* !ENOPROTOOPT */
 #   if !defined (EPROTONOSUPPORT)
  #     define EPROTONOSUPPORT    WSAEPROTONOSUPPORT
 #   endif/* !EPROTONOSUPPORT */
 #   if !defined (ESOCKTNOSUPPORT)
  #     define ESOCKTNOSUPPORT    WSAESOCKTNOSUPPORT
 #   endif/* !ESOCKTNOSUPPORT */
 #   if !defined (EOPNOTSUPP)
  #     define EOPNOTSUPP         WSAEOPNOTSUPP
 #   endif/* !EOPNOTSUPP */
 #   if !defined (EPFNOSUPPORT)
  #     define EPFNOSUPPORT       WSAEPFNOSUPPORT
 #   endif/* !EPFNOSUPPORT */
 #   if !defined (EAFNOSUPPORT)
  #     define EAFNOSUPPORT       WSAEAFNOSUPPORT
 #   endif/* !EAFNOSUPPORT */
 #   if !defined (EADDRINUSE)
  #     define EADDRINUSE         WSAEADDRINUSE
 #   endif/* !EADDRINUSE */
 #   if !defined (EADDRNOTAVAIL)
  #     define EADDRNOTAVAIL      WSAEADDRNOTAVAIL
 #   endif/* !EADDRNOTAVAIL */
 #   if !defined (ENETDOWN)
  #     define ENETDOWN           WSAENETDOWN
 #   endif/* !ENETDOWN */
 #   if !defined (ENETUNREACH)
  #     define ENETUNREACH        WSAENETUNREACH
 #   endif/* !ENETUNREACH */
 #   if !defined (ENETRESET)
  #     define ENETRESET          WSAENETRESET
 #   endif/* !ENETRESET */
 #   if !defined (ECONNABORTED)
  #     define ECONNABORTED       WSAECONNABORTED
 #   endif/* !ECONNABORTED */
 #   if !defined (ECONNRESET)
  #     define ECONNRESET         WSAECONNRESET
 #   endif/* !ECONNRESET */
 #   if !defined (ENOBUFS)
  #     define ENOBUFS            WSAENOBUFS
 #   endif/* !ENOBUFS */
 #   if !defined (EISCONN)
  #     define EISCONN            WSAEISCONN
 #   endif/* !EISCONN */
 #   if !defined (ENOTCONN)
  #     define ENOTCONN           WSAENOTCONN
 #   endif/* !ENOTCONN */
 #   if !defined (ESHUTDOWN)
  #     define ESHUTDOWN          WSAESHUTDOWN
 #   endif/* !ESHUTDOWN */
 #   if !defined (ETOOMANYREFS)
  #     define ETOOMANYREFS       WSAETOOMANYREFS
 #   endif/* !ETOOMANYREFS */
 #   if !defined (ETIMEDOUT)
  #     define ETIMEDOUT          WSAETIMEDOUT
 #   endif/* !ETIMEDOUT */
 #   if !defined (ECONNREFUSED)
  #     define ECONNREFUSED       WSAECONNREFUSED
 #   endif/* !ECONNREFUSED */
 #   if !defined (ELOOP)
  #     define ELOOP              WSAELOOP
 #   endif/* !ELOOP */
 #   if !defined (EHOSTDOWN)
  #     define EHOSTDOWN          WSAEHOSTDOWN
 #   endif/* !EHOSTDOWN */
 #   if !defined (EHOSTUNREACH)
  #     define EHOSTUNREACH       WSAEHOSTUNREACH
 #   endif/* !EHOSTUNREACH */
 #   if !defined (EPROCLIM)
  #     define EPROCLIM           WSAEPROCLIM
 #   endif/* !EPROCLIM */
 #   if !defined (EUSERS)
  #     define EUSERS             WSAEUSERS
 #   endif/* !EUSERS */
 #   if !defined (EDQUOT)
  #     define EDQUOT             WSAEDQUOT
 #   endif/* !EDQUOT */
 #   if !defined (ESTALE)
  #     define ESTALE             WSAESTALE
 #   endif/* !ESTALE */
 #   if !defined (EREMOTE)
  #     define EREMOTE            WSAEREMOTE
 #   endif/* !EREMOTE */
 #   if !defined (EADDRINUSE)
  #     define EADDRINUSE         WSAEADDRINUSE
 #   endif/* EADDRINUSE*/
#endif

#if !defined (POLLIN)
 #define POLLIN      0x0001
#endif
#if !defined (POLLPRI)
 #define POLLPRI     0x0002
#endif
#if !defined (POLLOUT)
 #define POLLOUT     0x0004
#endif
#if !defined (POLLERR)
 #define POLLERR     0x0008
#endif
#if !defined (POLLHUP)
 #define POLLHUP     0x0010
#endif
#if !defined (POLLNVAL)
 #define POLLNVAL    0x0020
#endif

#ifndef TCP_FASTOPEN
 #define TCP_FASTOPEN   23
#endif


CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
 #ifndef SO_NOSIGPIPE
    signal(SIGPIPE, SIG_IGN);
 #endif
#endif
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PROTOCOL_TCP", "0");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PROTOCOL_UDP", "1");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PROTOCOL_UNIX", "2");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PROTOCOL_UNIXDGRAM", "3");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PROTOCOL_SCTP", "4");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PROTOCOL_SCTP_SEQPACKET", "5");

    DEFINE_ECONSTANT(WS_ERROR_FRAME)
    DEFINE_ECONSTANT(WS_INCOMPLETE_FRAME)
    DEFINE_ECONSTANT(WS_TEXT_FRAME)
    DEFINE_ECONSTANT(WS_BINARY_FRAME)
    DEFINE_ECONSTANT(WS_OPENING_FRAME)
    DEFINE_ECONSTANT(WS_CLOSING_FRAME)
    DEFINE_ECONSTANT(WS_UNSUPPORTED_VERSION)

    DEFINE_ECONSTANT(SOL_SOCKET)
    DEFINE_ECONSTANT(IPPROTO_IP)
    DEFINE_ECONSTANT(IPPROTO_TCP)
    DEFINE_ECONSTANT(IPPROTO_UDP)
    DEFINE_ECONSTANT(IPPROTO_IPV6)
    DEFINE_ECONSTANT(IPV6_V6ONLY)

    DEFINE_ECONSTANT(SO_DEBUG)
    DEFINE_ECONSTANT(SO_ACCEPTCONN)
    DEFINE_ECONSTANT(SO_REUSEADDR)
    DEFINE_ECONSTANT(SO_KEEPALIVE)
    DEFINE_ECONSTANT(SO_DONTROUTE)
    DEFINE_ECONSTANT(SO_BROADCAST)
    //DEFINE_ECONSTANT(SO_USELOOPBACK)
    DEFINE_ECONSTANT(SO_LINGER)
    DEFINE_ECONSTANT(SO_OOBINLINE)

    DEFINE_ECONSTANT(SO_SNDBUF)
    DEFINE_ECONSTANT(SO_RCVBUF)
    DEFINE_ECONSTANT(SO_SNDLOWAT)
    DEFINE_ECONSTANT(SO_RCVLOWAT)
    DEFINE_ECONSTANT(SO_SNDTIMEO)
    DEFINE_ECONSTANT(SO_RCVTIMEO)
    DEFINE_ECONSTANT(SO_ERROR)
    DEFINE_ECONSTANT(SO_TYPE)

    DEFINE_ECONSTANT(TCP_NODELAY)
    DEFINE_ECONSTANT(IP_TOS)
    DEFINE_ECONSTANT(IP_TTL)

    DEFINE_ECONSTANT(EWOULDBLOCK)
    DEFINE_ECONSTANT(EINPROGRESS)
    DEFINE_ECONSTANT(EALREADY)
    DEFINE_ECONSTANT(ENOTSOCK)
    DEFINE_ECONSTANT(EDESTADDRREQ)
    DEFINE_ECONSTANT(EMSGSIZE)
    DEFINE_ECONSTANT(EPROTOTYPE)
    DEFINE_ECONSTANT(ENOPROTOOPT)
    DEFINE_ECONSTANT(EPROTONOSUPPORT)
    DEFINE_ECONSTANT(ESOCKTNOSUPPORT)
    DEFINE_ECONSTANT(EOPNOTSUPP)
    DEFINE_ECONSTANT(EPFNOSUPPORT)
    DEFINE_ECONSTANT(EAFNOSUPPORT)
    DEFINE_ECONSTANT(EADDRINUSE)
    DEFINE_ECONSTANT(EADDRNOTAVAIL)
    DEFINE_ECONSTANT(ENETDOWN)
    DEFINE_ECONSTANT(ENETUNREACH)
    DEFINE_ECONSTANT(ENETRESET)
    DEFINE_ECONSTANT(ECONNABORTED)
    DEFINE_ECONSTANT(ECONNRESET)
    DEFINE_ECONSTANT(ENOBUFS)
    DEFINE_ECONSTANT(EISCONN)
    DEFINE_ECONSTANT(ENOTCONN)
    DEFINE_ECONSTANT(ESHUTDOWN)
    DEFINE_ECONSTANT(ETOOMANYREFS)
    DEFINE_ECONSTANT(ETIMEDOUT)
    DEFINE_ECONSTANT(ECONNREFUSED)
    DEFINE_ECONSTANT(ELOOP)
    DEFINE_ECONSTANT(EHOSTDOWN)
    DEFINE_ECONSTANT(EHOSTUNREACH)
#ifdef EPROCLIM
    DEFINE_ECONSTANT(EPROCLIM)
#endif
    DEFINE_ECONSTANT(EUSERS)
    DEFINE_ECONSTANT(EDQUOT)
    DEFINE_ECONSTANT(ESTALE)
    DEFINE_ECONSTANT(EREMOTE)

    /*DEFINE_ECONSTANT(IPTOS_LOWDELAY)
       DEFINE_ECONSTANT(IPTOS_THROUGHPUT)
       DEFINE_ECONSTANT(IPTOS_RELIABILITY)
       DEFINE_ECONSTANT(IPTOS_LOWCOST)
       DEFINE_ECONSTANT(IPTOS_MINCOST)*/

    DEFINE_ECONSTANT(POLLIN)
    DEFINE_ECONSTANT(POLLOUT)
    DEFINE_ECONSTANT(POLLPRI)
    DEFINE_ECONSTANT(POLLERR)
    DEFINE_ECONSTANT(POLLHUP)
    DEFINE_ECONSTANT(POLLNVAL)

#ifndef WITHOUT_LIBSCTP
    DEFINE_ICONSTANT("SCTP_UNORDERED", SCTP_UNORDERED)
    DEFINE_ICONSTANT("SCTP_ABORT", SCTP_ABORT)
    DEFINE_ICONSTANT("SCTP_EOF", SCTP_EOF)
    DEFINE_ICONSTANT("SCTP_BINDX_ADD_ADDR", SCTP_BINDX_ADD_ADDR);
    DEFINE_ICONSTANT("SCTP_BINDX_REM_ADDR", SCTP_BINDX_REM_ADDR);
#endif
#ifdef MSG_NOSIGNAL
    DEFINE_ECONSTANT(MSG_NOSIGNAL)
#endif
    DEFINE_ECONSTANT(TCP_FASTOPEN)
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketCreate CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(0, 2, "SocketCreate: SocketCreate(nTYPE=PROTOCOL_TCP, ip_v6=false)");

    LOCAL_INIT;

    NUMBER type  = 0;
    int    sock  = INVALID_SOCKET;
    int    ip_v6 = 0;

    if (PARAMETERS_COUNT > 0) {
        GET_CHECK_NUMBER(0, type, "SocketCreate: parameter 1 should be a number");
    }

    if (PARAMETERS_COUNT > 1) {
        NUMBER is_ipv6 = 0;
        GET_CHECK_NUMBER(1, is_ipv6, "SocketCreate: parameter 2 should be a number");
        if (is_ipv6)
            ip_v6 = 1;
    }

    int family = AF_INET;
    if (ip_v6)
        family = AF_INET6;

    if ((int)type == 0) {
        sock = socket(family, SOCK_STREAM, IPPROTO_TCP);
    } else
    if ((int)type == 1) {
        sock = socket(family, SOCK_DGRAM, IPPROTO_UDP);
    } else
    if ((int)type == 2) {
#ifdef _WIN32
        sock = 0x7FFFFFFF;
#else
        sock = socket(PF_UNIX, SOCK_STREAM, 0);
#endif
    } else
    if ((int)type == 3) {
#ifdef _WIN32
        sock = -1;
#else
        sock = socket(PF_UNIX, SOCK_DGRAM, 0);
#endif
    } 
#ifdef IPPROTO_SCTP
    else
    if ((int)type == 4) {
        sock = socket(family, SOCK_STREAM, IPPROTO_SCTP);
// #ifdef SCTP_LISTEN_FIX
        // if (sock > 0) {
        //    // apple fix?
        //    const int on = 1;
        //    setsockopt(sock, IPPROTO_SCTP, SCTP_LISTEN_FIX, &on, sizeof(int));
        // }
// #endif
    } else
    if ((int)type == 5) {
        sock = socket(family, SOCK_SEQPACKET, IPPROTO_SCTP);
    }
#endif
    if ((int)type == 0) {
#ifdef _WIN32
        DWORD flag = 1;
#else
        int flag = 1;
#endif
        setsockopt(sock,
                   SOL_SOCKET,
                   SO_KEEPALIVE,
                   (char *)&flag,
                   sizeof(flag));

        setsockopt(sock,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   (char *)&flag,
                   sizeof(flag));
#ifdef _WIN32 
#ifdef SO_EXCLUSIVEADDRUSE
        setsockopt(sock,
                   SOL_SOCKET,
                   SO_EXCLUSIVEADDRUSE,
                   (char *)&flag,
                   sizeof(flag));
#endif
#endif
        struct linger so_linger;
        so_linger.l_onoff  = 1;
        so_linger.l_linger = 1;

        setsockopt(sock,
                   SOL_SOCKET,
                   SO_LINGER,
                   (char *)&so_linger,
                   sizeof(so_linger));
    }
#ifndef WIN32
 #ifdef SO_NOSIGPIPE
    if (sock > -1) {
        int flag = 1;
        setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &flag, sizeof(flag));
    }
 #endif
#endif
    if (type == 1) {
        int flag = 0xFFFF;
        setsockopt(sock,
                   SOL_SOCKET,
                   SO_SNDBUF,
                   (char *)&flag,
                   sizeof(flag));
    }
    RETURN_NUMBER(sock);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketSetOption CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "SocketSetOption: SocketSetOptions(nSocket, nLevel, nOption, nValue)");

    LOCAL_INIT;

    NUMBER sock    = INVALID_SOCKET;
    NUMBER nLevel  = 0;
    NUMBER nOption = 0;
    NUMBER nValue  = 0;

    GET_CHECK_NUMBER(0, sock, "SocketSetOption: parameter 1 should be a number");
    GET_CHECK_NUMBER(1, nLevel, "SocketSetOption: parameter 2 should be a number");
    GET_CHECK_NUMBER(2, nOption, "SocketSetOption: parameter 3 should be a number");
    GET_CHECK_NUMBER(3, nValue, "SocketSetOption: parameter 4 should be a number");

    int val = (int)nValue;
#ifdef _WIN32
    if (UnixSocketEmulation[(SYS_INT)sock]) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(setsockopt((int)sock, (int)nLevel, (int)nOption, (char *)&val, sizeof(int)));
    }
#else
    struct timeval timeout;
    struct linger  so_linger;

    switch ((int)nOption) {
        case SO_RCVTIMEO:
        case SO_SNDTIMEO:
            timeout.tv_sec  = val / 1000;
            timeout.tv_usec = (val % 1000) * 1000;
            RETURN_NUMBER(setsockopt((int)sock, (int)nLevel, (int)nOption, (void *)&timeout, sizeof(timeout)));
            break;

        case SO_LINGER:
            if (val) {
                so_linger.l_onoff  = 1;
                so_linger.l_linger = val;
            } else {
                so_linger.l_onoff  = 0;
                so_linger.l_linger = 0;
            }
            RETURN_NUMBER(setsockopt((int)sock, (int)nLevel, (int)nOption, (void *)&so_linger, sizeof(so_linger)));
            break;

        default:
            RETURN_NUMBER(setsockopt((int)sock, (int)nLevel, (int)nOption, (void *)&val, sizeof(int)));
    }
#endif

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketGetOption CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "SocketGetOption: SocketSetOptions(nSocket, nLevel, nOption, var nValue)");

    LOCAL_INIT;

    NUMBER sock    = INVALID_SOCKET;
    NUMBER nLevel  = 0;
    NUMBER nOption = 0;
    int    nValue  = 0;

    GET_CHECK_NUMBER(0, sock, "SocketGetOption: parameter 1 should be a number");
    GET_CHECK_NUMBER(1, nLevel, "SocketGetOption: parameter 2 should be a number");
    GET_CHECK_NUMBER(2, nOption, "SocketGetOption: parameter 3 should be a number");
    //GET_CHECK_NUMBER(3,nValue,"SocketSetOption: parameter 4 should be a number");


#ifdef _WIN32
    DWORD val = 0;
    int   len = sizeof(val);
    if (UnixSocketEmulation[(SYS_INT)sock]) {
        RETURN_NUMBER(-1);
    } else {
        int r = getsockopt((int)sock, (int)nLevel, (int)nOption, (char *)&val, &len);
        RETURN_NUMBER(r);
    }
#else
    int            val = 0;
    socklen_t      len = sizeof(val);
    struct timeval timeout;
    struct linger  so_linger;
    switch ((int)nOption) {
        case SO_RCVTIMEO:
        case SO_SNDTIMEO:
            RETURN_NUMBER(getsockopt((int)sock, (int)nLevel, (int)nOption, (void *)&timeout, &len));
            val = timeout.tv_sec * 1000 + timeout.tv_usec / 1000;
            len = sizeof(int);
            break;

        case SO_LINGER:
            len = sizeof(so_linger);
            RETURN_NUMBER(getsockopt((int)sock, (int)nLevel, (int)nOption, (void *)&so_linger, &len));
            if (so_linger.l_onoff)
                val = so_linger.l_linger;
            else
                val = 0;
            len = sizeof(int);
            break;

        default:
            RETURN_NUMBER(getsockopt((int)sock, (int)nLevel, (int)nOption, (void *)&val, &len));
    }
#endif
    switch (len) {
        case 0:
            val = 0;
            break;

        case 1:
            val = *((char *)&val);
            break;

        case 2:
            val = *((short *)&val);
            break;

        default:
            val = *((int *)&val);
            break;
    }
    SET_NUMBER(3, (int)val);

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketError CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SocketError: SocketError(nSocket)");

    LOCAL_INIT;

    NUMBER sock = INVALID_SOCKET;

    GET_CHECK_NUMBER(0, sock, "SocketError: parameter 1 should be a number");

    int err = 0;
#ifdef _WIN32
    int res = 0;
    if (UnixSocketEmulation[(SYS_INT)sock]) {
        int err = GetLastError();
    } else {
        int errlen = sizeof(err);
        int res    = getsockopt((int)sock, SOL_SOCKET, SO_ERROR, (char *)&err, &errlen);
    }
#else
    socklen_t errlen = sizeof(err);
    int       res    = getsockopt((int)sock, SOL_SOCKET, SO_ERROR, &err, &errlen);
#endif
    if (res < 0)
        err = -2;

    RETURN_NUMBER(err);

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketConnect CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(3, 4, "SocketConnect: SocketConnect(nSocket, szHostname, nPort, ipv6=false)");

    LOCAL_INIT;

    NUMBER sock      = INVALID_SOCKET;
    char   *hostname = 0;
    NUMBER nPort     = 0;

    GET_CHECK_NUMBER(0, sock, "SocketConnect: parameter 1 should be a number");
    GET_CHECK_STRING(1, hostname, "SocketConnect: parameter 2 should be a string");
    GET_CHECK_NUMBER(2, nPort, "SocketConnect: parameter 3 should be a number");

    int family = AF_INET;

    if (PARAMETERS_COUNT > 3) {
        NUMBER is_ipv6 = 0;
        GET_CHECK_NUMBER(3, is_ipv6, "SocketCreate: parameter 4 should be a number");
        if (is_ipv6)
            family = AF_INET6;
    }

    if (nPort >= 0) {
        if (family == AF_INET6) {
            struct addrinfo hints;
            struct addrinfo *res, *result;

            memset(&hints, 0, sizeof hints);
            hints.ai_family   = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            AnsiString port_str((long)nPort);
            if (getaddrinfo(hostname, port_str.c_str(), &hints, &result) != 0) {
                RETURN_NUMBER(-2);
                return 0;
            }

            int connected = 0;
            RETURN_NUMBER(-1);
            for (res = result; res != NULL; res = res->ai_next) {
                char hostname[NI_MAXHOST] = "";
                if (/*(res->ai_family == AF_INET) || */ (res->ai_family == AF_INET6)) {
                    int error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
                    if (!error) {
                        int cres = connect((int)sock, res->ai_addr, res->ai_addrlen);
                        RETURN_NUMBER(cres);
                        if (!cres)
                            break;
                    }
                }
            }

            if (result)
                freeaddrinfo(result);
        } else {
            struct sockaddr_in sin;
            struct hostent     *hp;

            if ((hp = gethostbyname(hostname)) == 0) {
                RETURN_NUMBER(-2);
                return 0;
            }
            memset(&sin, 0, sizeof(sin));
            sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
            sin.sin_family      = family;
            sin.sin_port        = htons((int)nPort);
            RETURN_NUMBER(connect((int)sock, (struct sockaddr *)&sin, sizeof(sin)));
        }
    } else {
#ifdef _WIN32
        //RETURN_NUMBER(-1);
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        AnsiString          pipename("\\\\.\\pipe\\");
        pipename += hostname;
        HANDLE hpipe = CreateFile(pipename.c_str(), GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE, 0, NULL, OPEN_EXISTING, 0, &sa);
        if (hpipe != INVALID_HANDLE_VALUE) {
            int hpipe_Fd = _open_osfhandle((intptr_t)hpipe, 0);
            UnixSocketEmulation[(SYS_INT)hpipe_Fd] = 1;
            SetVariable(PARAMETER(0), VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)hpipe_Fd);
            RETURN_NUMBER(0);
        } else {
            RETURN_NUMBER(-1);
        }
#else
        struct sockaddr_un address;
        memset(&address, 0, sizeof(struct sockaddr_un));
        address.sun_family = AF_UNIX;
        snprintf(address.sun_path, UNIX_PATH_MAX, hostname);

        RETURN_NUMBER(connect((int)sock, (struct sockaddr *)&address, sizeof(address)));
#endif
    }
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketListen CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 5, "SocketListen: SocketListen(nSocket, nPort, nMaxConnections=0x100, interface=\"\", ipv6=false)");

    LOCAL_INIT;

    NUMBER sock    = INVALID_SOCKET;
    NUMBER nMaxCon = 0x100;
    NUMBER nPort   = 0;

    GET_CHECK_NUMBER(0, sock, "SocketListen: parameter 1 should be a number");
    GET_CHECK_NUMBER(1, nPort, "SocketListen: parameter 2 should be a number");

    if (PARAMETERS_COUNT > 2) {
        GET_CHECK_NUMBER(2, nMaxCon, "SocketListen: parameter 3 should be a number");
    }

    char *iface = NULL;
    if (PARAMETERS_COUNT > 3) {
        GET_CHECK_STRING(3, iface, "SocketListen: parameter 4 should be a string");
    }

    int family = AF_INET;

    NUMBER ipv6 = 0;
    if (PARAMETERS_COUNT > 4) {
        GET_CHECK_NUMBER(4, ipv6, "SocketListen: parameter 5 should be a string");
        if (ipv6)
            family = AF_INET6;
    }

    if (nPort >= 0) {
        struct sockaddr addr;
#ifdef _WIN32
        int peerlen = sizeof(addr);
#else
        socklen_t peerlen = sizeof(addr);
#endif
        if (family == AF_INET6) {
            struct sockaddr_in6 sin;
            memset(&sin, 0, sizeof(sin));

            if ((iface) && (iface[0]))
                inet_pton(AF_INET6, iface, (void *)&sin.sin6_addr.s6_addr);
            else
                sin.sin6_addr = in6addr_any;

            sin.sin6_family = AF_INET6;
            sin.sin6_port   = htons((int)nPort);

            if (bind((int)sock, (struct sockaddr *)&sin, sizeof(sin))) {
                RETURN_NUMBER((NUMBER)INVALID_SOCKET);
            }
        } else {
            struct sockaddr_in sin;
            memset(&sin, 0, sizeof(sin));

            if ((iface) && (iface[0]))
                sin.sin_addr.s_addr = inet_addr(iface);
            else
                sin.sin_addr.s_addr = INADDR_ANY;
            sin.sin_family = AF_INET;
            sin.sin_port   = htons((int)nPort);

            if (bind((int)sock, (struct sockaddr *)&sin, sizeof(sin))) {
                RETURN_NUMBER((NUMBER)INVALID_SOCKET);
            }

            RETURN_NUMBER(listen((int)sock, (int)nMaxCon));
        }

        RETURN_NUMBER(listen((int)sock, (int)nMaxCon));
    } else {
#ifdef _WIN32
        AnsiString pipename("\\\\.\\pipe\\");
        pipename += iface;
        if (nMaxCon > 0xFF)
            nMaxCon = 0xFF;
        HANDLE hpipe = CreateNamedPipe(pipename.c_str(), PIPE_ACCESS_DUPLEX, PIPE_WAIT, (int)nMaxCon, 0x3FFF, 0x3FFF, 10000, NULL);
        if (hpipe != INVALID_HANDLE_VALUE) {
            int hpipe_Fd = _open_osfhandle((intptr_t)hpipe, 0);
            UnixSocketEmulation[(SYS_INT)hpipe_Fd] = 1;
            SetVariable(PARAMETER(0), VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)hpipe_Fd);
            RETURN_NUMBER(0);
        } else {
            RETURN_NUMBER(-1);
        }
#else
        struct sockaddr_un address;
        memset(&address, 0, sizeof(struct sockaddr_un));
        address.sun_family = AF_UNIX;

        snprintf(address.sun_path, UNIX_PATH_MAX, iface);

        if (bind((int)sock, (struct sockaddr *)&address, sizeof(address)) == INVALID_SOCKET) {
            RETURN_NUMBER((NUMBER)INVALID_SOCKET);
        }
        RETURN_NUMBER(listen((int)sock, (int)nMaxCon));
#endif
    }
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketBindUDP CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 4, "SocketBindUDP: SocketBindUDP(nSocket, nPort, interface=\"\", ipv6=false)");

    LOCAL_INIT;

    NUMBER sock  = INVALID_SOCKET;
    NUMBER nPort = 0;

    GET_CHECK_NUMBER(0, sock, "SocketBindUDP: parameter 1 should be a number");
    GET_CHECK_NUMBER(1, nPort, "SocketBindUDP: parameter 2 should be a number");

    char *iface = NULL;
    if (PARAMETERS_COUNT > 2) {
        GET_CHECK_STRING(2, iface, "SocketBindUDP: parameter 3 should be a string");
    }

    int family = AF_INET;

    if (PARAMETERS_COUNT > 3) {
        NUMBER is_ipv6 = 0;
        GET_CHECK_NUMBER(3, is_ipv6, "SocketCreate: parameter 4 should be a number");
        if (is_ipv6)
            family = AF_INET6;
    }

    if (family == AF_INET6) {
        struct sockaddr_in6 sin;
        memset(&sin, 0, sizeof(sin));

        if ((iface) && (iface[0]))
            inet_pton(AF_INET6, iface, (void *)&sin.sin6_addr.s6_addr);
        else
            sin.sin6_addr = in6addr_any;

        sin.sin6_family = AF_INET6;
        sin.sin6_port   = htons((int)nPort);

        if (bind((int)sock, (struct sockaddr *)&sin, sizeof(sin))) {
            RETURN_NUMBER((NUMBER)INVALID_SOCKET);
        }
    } else {
        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        if ((iface) && (iface[0]))
            sin.sin_addr.s_addr = inet_addr(iface);
        else
            sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_family = AF_INET;
        sin.sin_port   = htons((int)nPort);

        int res = bind((int)sock, (struct sockaddr *)&sin, sizeof(sin));

        RETURN_NUMBER(res);
    }
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketAccept CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SocketAccept: SocketAccept(nSocket)");

    LOCAL_INIT;

    NUMBER sock  = INVALID_SOCKET;
    NUMBER nPort = 0;

    GET_CHECK_NUMBER(0, sock, "SocketAccept: parameter 1 should be a number");

#ifdef _WIN32
    int ref_count = UnixSocketEmulation[(SYS_INT)sock];
    if (ref_count) {
        ref_count++;
        if (ConnectNamedPipe((HANDLE)_get_osfhandle((SYS_INT)sock), NULL)) {
            UnixSocketEmulation[(SYS_INT)sock] = ref_count;
            RETURN_NUMBER(sock);
        } else {
            RETURN_NUMBER(-1);
        }
        return 0;
    }
#endif
    RETURN_NUMBER(accept((int)sock, NULL, NULL));
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketClose CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(1, 2, "SocketClose: SocketClose(nSocket[, force_close=false])");

    LOCAL_INIT;

    NUMBER sock = INVALID_SOCKET;

    GET_CHECK_NUMBER(0, sock, "SocketClose: parameter 1 should be a number");
    int force = 0;

    if (PARAMETERS_COUNT > 1) {
        NUMBER doForce = 0;
        GET_CHECK_NUMBER(1, doForce, "SocketClose: parameter 2 should be a number");
        if (doForce)
            force = 1;
    }
#ifdef _WIN32
    int ref_count = UnixSocketEmulation[(SYS_INT)sock];
    if (ref_count) {
        ref_count--;
        if (ref_count > 0) {
            UnixSocketEmulation[(SYS_INT)sock] = ref_count;
        } else {
            UnixSocketEmulation.erase((SYS_INT)sock);
            //CloseHandle((HANDLE)(SYS_INT)sock);
            close((int)sock);
        }
    } else {
        if (!force)
            shutdown((int)sock, SD_BOTH);
        RETURN_NUMBER(closesocket((int)sock));
    }
#else
    if (!force)
        shutdown((int)sock, SHUT_RDWR);
    RETURN_NUMBER(close((int)sock));
#endif


    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketRead CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(3, 7, "SocketRead: SocketRead(nSocket, var anyBuffer, nSize[, bSocketIsUDP=false[, bReceiveExactSize=false][, var ip, var port]])");

    LOCAL_INIT;

    NUMBER sock      = INVALID_SOCKET;
    NUMBER nSize     = 0;
    NUMBER isUdp     = 0;
    NUMBER exactSize = 0;

    GET_CHECK_NUMBER(0, sock, "SocketRead: parameter 1 should be a number");
    GET_CHECK_NUMBER(2, nSize, "SocketRead: parameter 3 should be a number");

    if (PARAMETERS_COUNT > 3) {
        GET_CHECK_NUMBER(3, isUdp, "SocketRead: parameter 4 should be a number");

        if (PARAMETERS_COUNT > 4) {
            GET_CHECK_NUMBER(4, exactSize, "SocketRead: parameter 5 should be a number");
        }
    }

    if ((int)nSize <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }

    //char *buf=new char[(int)nSize+1];
    char *buf = 0;
    CORE_NEW((int)nSize + 1, buf);
    if (!buf) {
        RETURN_NUMBER(-3);
        return 0;
    }
    buf[0]          = 0;
    buf[(int)nSize] = 0;
#ifdef _WIN32
    int ref_count = UnixSocketEmulation[(SYS_INT)sock];
    if (ref_count) {
        DWORD outsize = 0;
        if (ReadFile((HANDLE)_get_osfhandle((SYS_INT)sock), buf, (int)nSize, &outsize, NULL)) {
            if (outsize >= 0)
                buf[outsize] = 0;
            SetVariable(PARAMETER(1), -1, buf, outsize);
            RETURN_NUMBER(outsize);
        } else {
            CORE_DELETE(buf);
            SET_STRING(1, "");
            RETURN_NUMBER(-1);
        }
        return 0;
    }
#endif
    int rec_size = 0;
    if (isUdp) {
#ifdef _WIN32
        struct sockaddr_storage host_address;
        int addr_length = sizeof(host_address);
        rec_size = recvfrom((int)sock, buf, (int)nSize, 0, (sockaddr *)&host_address, &addr_length);
#else
        struct sockaddr_storage host_address;
        socklen_t addr_length = sizeof(host_address);
        rec_size = recvfrom((int)sock, buf, (int)nSize, 0, (sockaddr *)&host_address, &addr_length);
#endif
        if (PARAMETERS_COUNT > 5) {
            char *ip  = 0;
            int  port = 0;
            char ipstr[INET6_ADDRSTRLEN];
            if (host_address.ss_family == AF_INET) {
                struct sockaddr_in *s = (struct sockaddr_in *)&host_address;
                ip   = (char *)inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
                port = ntohs(s->sin_port);
            } else
            if (host_address.ss_family == AF_INET6) {
                struct sockaddr_in6 *s = (struct sockaddr_in6 *)&host_address;
                ip   = (char *)inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
                port = ntohs(s->sin6_port);
            }
            if (ip) {
                SET_STRING(5, ip);
            } else {
                SET_STRING(5, "");
            }
            if (PARAMETERS_COUNT > 6) {
                SET_NUMBER(6, port);
            }
        }
    } else {
        int len = 0;
        do {
            len = recv((int)sock, buf + rec_size, (int)nSize - rec_size, 0);

            if (len > 0)
                rec_size += len;
        } while (((int)exactSize) && (len > 0) && (rec_size < (int)nSize));
    }
    if (rec_size > 0) {
        buf[rec_size] = 0;
        SetVariable(PARAMETER(1), -1, buf, rec_size);
    } else {
        CORE_DELETE(buf);
        SET_STRING(1, "");
    }
    //SET_BUFFER(1, buf, rec_size);

    //delete buf;

    RETURN_NUMBER(rec_size);

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketWrite CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 8, "SocketWrite: SocketWrite(nSocket, anyBuffer[, bSocketIsUDP=false, szUDPHost, nUDPPort, udp_ipv6=false, offset = 0, max_size = 0])");

    LOCAL_INIT;

    NUMBER sock = INVALID_SOCKET;
    char   *buffer;
    NUMBER isUdp     = 0;
    NUMBER nSize     = 0;
    NUMBER nPort     = 0;
    char   *hostname = 0;
    NUMBER offset    = 0;

    GET_CHECK_NUMBER(0, sock, "SocketWrite: parameter 1 should be a number");
    GET_CHECK_BUFFER(1, buffer, nSize, "SocketWrite: parameter 2 should be a string buffer");

    struct hostent *hp = NULL;
    if (PARAMETERS_COUNT > 4) {
        GET_CHECK_NUMBER(2, isUdp, "SocketWrite: parameter 3 should be a number");

        GET_CHECK_STRING(3, hostname, "SocketWirte: parameter 4 should be a string");
        GET_CHECK_NUMBER(4, nPort, "SocketWrite: parameter 5 should be a number");
    }
#ifdef _WIN32
    int ref_count = UnixSocketEmulation[(SYS_INT)sock];
    if (ref_count) {
        DWORD outsize = 0;
        if (WriteFile((HANDLE)_get_osfhandle((SYS_INT)sock), buffer, (int)nSize, &outsize, NULL)) {
            RETURN_NUMBER(outsize);
        } else {
            RETURN_NUMBER(-1);
        }
        return 0;
    }
#endif
    int sent_size = 0;
    int family    = AF_INET;
    if (PARAMETERS_COUNT > 5) {
        NUMBER is_ipv6 = 0;
        GET_CHECK_NUMBER(5, is_ipv6, "SocketWrite: parameter 6 should be a number");
        if (is_ipv6)
            family = AF_INET6;
    }
    if (PARAMETERS_COUNT > 6) {
        GET_CHECK_NUMBER(6, offset, "SocketWrite: parameter 7 should be a number");

        buffer += (int)offset;
        nSize  -= (int)offset;
        if (nSize <= 0) {
            RETURN_NUMBER(0);
            return 0;
        }
    }
    if (PARAMETERS_COUNT > 7) {
        NUMBER max_size = 0;
        GET_CHECK_NUMBER(7, max_size, "SocketWrite: parameter 8 should be a number");
        if ((nSize > max_size) && (max_size > 0))
            nSize = max_size;
        if (nSize <= 0) {
            RETURN_NUMBER(0);
            return 0;
        }
    }
    if (isUdp) {
        if (family == AF_INET6) {
            struct addrinfo hints;
            struct addrinfo *res, *result;

            memset(&hints, 0, sizeof hints);
            hints.ai_family   = AF_UNSPEC;
            hints.ai_socktype = SOCK_DGRAM;

            AnsiString port_str((long)nPort);
            if (getaddrinfo(hostname, port_str.c_str(), &hints, &result) != 0) {
                RETURN_NUMBER(-2);
                return 0;
            }
            int connected = 0;
            RETURN_NUMBER(-1);
            for (res = result; res != NULL; res = res->ai_next) {
                char hostname[NI_MAXHOST] = "";
                if (res->ai_family == AF_INET6) {
                    int error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
                    if (!error) {
                        sent_size = sendto((int)sock, buffer, (int)nSize, 0, res->ai_addr, res->ai_addrlen);
                        RETURN_NUMBER(sent_size);
                        break;
                    }
                }
            }

            if (result)
                freeaddrinfo(result);
        } else {
            if ((hp = gethostbyname(hostname)) == 0) {
                RETURN_NUMBER(-2);
                return 0;
            }
#ifdef _WIN32
            struct sockaddr_in host_address;
            int addr_length = sizeof(host_address);
#else
            struct sockaddr_in host_address;
            socklen_t          addr_length = sizeof(host_address);
#endif
            memset(&host_address, 0, sizeof(host_address));
            host_address.sin_family      = AF_INET;
            host_address.sin_port        = htons((unsigned short)nPort);
            host_address.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;

            sent_size = sendto((int)sock, buffer, (int)nSize, 0, (struct sockaddr *)&host_address, addr_length);
        }
    } else {
#if defined(_WIN32) || !defined(MSG_NOSIGNAL)
        sent_size = send((int)sock, buffer, (int)nSize, 0);
#else
        sent_size = send((int)sock, buffer, (int)nSize, MSG_NOSIGNAL);
#endif
    }

    RETURN_NUMBER(sent_size);

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketInfo CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(1, 2, "SocketInfo: SocketInfo(nSocket[, include_local_data = false])");

    LOCAL_INIT;

    NUMBER sock       = INVALID_SOCKET;
    int    local_data = 0;
    GET_CHECK_NUMBER(0, sock, "SocketInfo: parameter 1 should be a number");
    if (PARAMETERS_COUNT > 1) {
        NUMBER ldata = 0;
        GET_CHECK_NUMBER(1, ldata, "SocketInfo: parameter 2 should be a number");
        local_data = (int)ldata;
    }

    struct sockaddr_storage addr;
    char        ipstr[INET6_ADDRSTRLEN];
    int         port     = 0;
    static char *unknown = "unknown";

#ifdef _WIN32
    int peerlen = sizeof(addr);
#else
    socklen_t peerlen = sizeof(addr);
#endif
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);
#ifdef _WIN32
    int ref_count = UnixSocketEmulation[(SYS_INT)sock];
    if (ref_count)
        return 0;
#endif
    const char *ip = 0;
    if (!getpeername((int)sock, (sockaddr *)&addr, &peerlen)) {
        if (addr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            port = ntohs(s->sin_port);
            ip   = inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        } else
        if (addr.ss_family == AF_INET6) {
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
            port = ntohs(s->sin6_port);
            ip   = inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        }
        if (!ip)
            ip = unknown;


        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "address", (INTEGER)VARIABLE_STRING, (char *)ip, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "port", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)port);
    }

    if (local_data) {
        peerlen = sizeof(addr);
        if (getsockname((int)sock, (sockaddr *)&addr, &peerlen))
            return 0;

        ip = 0;
        if (addr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            port = ntohs(s->sin_port);
            ip   = inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        } else
        if (addr.ss_family == AF_INET6) {
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
            port = ntohs(s->sin6_port);
            ip   = inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        }
        if (!ip)
            ip = unknown;

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "local_address", (INTEGER)VARIABLE_STRING, (char *)ip, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "local_port", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)port);
    }
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SocketHasData CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(1, 2, "SocketHasData: SocketHasData(nSocket[, timeout])");

    LOCAL_INIT;

    NUMBER sock = INVALID_SOCKET;

    int has_data = 0;
    GET_CHECK_NUMBER(0, sock, "SocketHasData: parameter 1 should be a number");
    NUMBER t_out = -1;
    if (PARAMETERS_COUNT > 1) {
        GET_CHECK_NUMBER(1, t_out, "SocketHasData: parameter 2 should be a number");
    }
    if ((sock < 0) || (sock == INVALID_SOCKET)) {
        RETURN_NUMBER(-1);
        return 0;
    }
#ifdef _WIN32
    int ref_count = UnixSocketEmulation[(SYS_INT)sock];
    if (ref_count) {
        DWORD has_bytes = 0;
        if (PeekNamedPipe((HANDLE)_get_osfhandle((SYS_INT)sock), NULL, 0, NULL, &has_bytes, NULL)) {
            if (has_bytes) {
                RETURN_NUMBER(1);
            } else {
                RETURN_NUMBER(0);
            }
        } else {
            RETURN_NUMBER(-1);
        }
        return 0;
    }

    struct timeval timeout;
    timeout.tv_sec = 0;
    if (t_out < 0)
        timeout.tv_usec = 0;
    else
        timeout.tv_usec = (int)t_out * 1000;
    fd_set socks;

    FD_ZERO(&socks);
    FD_SET((int)sock, &socks);

    int sel_val = select(FD_SETSIZE, &socks, 0, 0, &timeout);

    has_data = (sel_val != 0);
    if (sel_val < 0) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(has_data);
    }
#else
    struct pollfd ufds[1];
    ufds[0].fd     = sock;
    ufds[0].events = POLLIN;

    if (t_out < 0)
        t_out = 0;
    int res = poll(ufds, 1, (int)t_out);

    RETURN_NUMBER(res);
#endif
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(WSParseHandshake, 1)
    T_STRING(WSParseHandshake, 0)
    struct handshake hs;
    ws_frame_type    frame_type = ws_parse_handshake((uint8_t *)PARAM(0), PARAM_LEN(0), &hs);
    CREATE_ARRAY(RESULT);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "frame_type", (INTEGER)VARIABLE_NUMBER, 0, (NUMBER)frame_type);
    if (frame_type != WS_INCOMPLETE_FRAME) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "resource", (INTEGER)VARIABLE_STRING, hs.resource ? hs.resource : "", (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "host", (INTEGER)VARIABLE_STRING, hs.host ? hs.host : "", (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "origin", (INTEGER)VARIABLE_STRING, hs.origin ? hs.origin : "", (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "protocol", (INTEGER)VARIABLE_STRING, hs.protocol ? hs.protocol : "", (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "version", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)hs.version);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "key", (INTEGER)VARIABLE_STRING, hs.key ? hs.key : "", (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "key1", (INTEGER)VARIABLE_STRING, hs.key1 ? hs.key1 : "", (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "key2", (INTEGER)VARIABLE_STRING, hs.key2 ? hs.key2 : "", (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "key3", (INTEGER)VARIABLE_STRING, hs.key3, (NUMBER)8);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(WSHandshakeAnswer, 1)
    T_ARRAY(WSHandshakeAnswer, 0)
    struct handshake hs;
    nullhandshake(&hs);
    INTEGER type  = 0;
    NUMBER  dummy = 0;
    char    *str  = 0;
    uint8_t buffer[0xFFFF];
    size_t  out_len = 0xFFFF;

    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), "resource", &type, &str, &dummy);
    if (type == VARIABLE_STRING)
        hs.resource = str;
    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), "host", &type, &str, &dummy);
    if (type == VARIABLE_STRING)
        hs.host = str;
    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), "origin", &type, &str, &dummy);
    if (type == VARIABLE_STRING)
        hs.origin = str;
    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), "protocol", &type, &str, &dummy);
    if (type == VARIABLE_STRING)
        hs.protocol = str;
    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), "version", &type, &str, &dummy);
    if (type == VARIABLE_NUMBER)
        hs.version = (char)dummy;
    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), "key", &type, &str, &dummy);
    if (type == VARIABLE_STRING)
        hs.key = str;
    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), "key1", &type, &str, &dummy);
    if (type == VARIABLE_STRING)
        hs.key1 = str;
    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), "key2", &type, &str, &dummy);
    if (type == VARIABLE_STRING)
        hs.key2 = str;
    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(0), "key3", &type, &str, &dummy);
    if ((type == VARIABLE_STRING) && (str)) {
        int len = (int)dummy;
        if (len > 8)
            len = 8;
        memcpy(hs.key3, str, len);
    }
    if ((!hs.origin) || (!hs.host) || (!hs.resource)) {
        RETURN_STRING("");
        return 0;
    }
    ws_get_handshake_answer(&hs, buffer, &out_len);
    if (out_len > 0) {
        RETURN_BUFFER((char *)buffer, out_len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WSMakeFrame, 1, 2)
    T_STRING(WSMakeFrame, 0)

    ws_frame_type ft_orig = WS_TEXT_FRAME;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(WSMakeFrame, 1)
        ft_orig = (ws_frame_type)PARAM_INT(1);
    }
    char   *buffer = 0;
    size_t out_len = PARAM_LEN(0) + 0xFF;
    CORE_NEW(out_len + 1, buffer);

    ws_frame_type frame_type = ws_make_frame((uint8_t *)PARAM(0), PARAM_LEN(0), (uint8_t *)buffer, &out_len, ft_orig);
    if ((frame_type != ft_orig) || (!out_len)) {
        CORE_DELETE(buffer);
        RETURN_STRING("");
    } else
        SetVariable(RESULT, -1, buffer, out_len);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WSParseFrame, 1, 2)
    T_STRING(WSParseFrame, 0)

    ws_frame_type ft_orig = WS_BINARY_FRAME;
    char   *buffer = 0;
    size_t out_len = 0;

    ws_frame_type frame_type = ws_parse_input_frame((uint8_t *)PARAM(0), PARAM_LEN(0), (uint8_t **)&buffer, &out_len);
    if ((out_len) && (buffer)) {
        RETURN_BUFFER(buffer, out_len);
    } else {
        RETURN_STRING("");
    }
    if (PARAMETERS_COUNT > 1) {
        SET_NUMBER(1, frame_type);
    }

END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(GetHostName, 0)
    char buffer[0xFFF];
    buffer[0] = 0;
    int res = gethostname(buffer, 0xFFE);
    if (res) {
        RETURN_STRING("");
    } else {
        RETURN_STRING(buffer);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(GetDomainName, 0)
    char buffer[0xFFF];
    buffer[0] = 0;
    int res = getdomainname(buffer, 0xFFE);
    if (res) {
        RETURN_STRING("");
    } else {
        RETURN_STRING(buffer);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(GetHostByAddr, 1)
    T_STRING(GetHostByAddr, 0);
    struct hostent *he;
#ifdef _WIN32
    struct in_addr addr = {
        0
    };

    addr.s_addr = inet_addr(PARAM(0));
    he          = gethostbyaddr((char *)&addr, 4, AF_INET);

    if (he) {
        RETURN_STRING(he->h_name);
    } else {
        RETURN_STRING("");
    }
#else
    struct in_addr ipv4addr;

    inet_pton(AF_INET, PARAM(0), &ipv4addr);
    he = gethostbyaddr((char *)&ipv4addr, sizeof ipv4addr, AF_INET);

    if (he) {
        RETURN_STRING(he->h_name);
    } else {
        RETURN_STRING("");
    }
#endif
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(GetHostByAddr6, 1)
    T_STRING(GetHostByAddr6, 0);
    struct hostent *he;
#ifdef _WIN32
    struct in_addr6 addr = {
        0
    };

    inet_pton(AF_INET6, PARAM(0), &addr);

    he = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET6);

    if (he) {
        RETURN_STRING(he->h_name);
    } else {
        RETURN_STRING("");
    }
#else
    struct in6_addr addr;

    inet_pton(AF_INET6, PARAM(0), &addr);
    he = gethostbyaddr((char *)&addr, sizeof addr, AF_INET6);

    if (he) {
        RETURN_STRING(he->h_name);
    } else {
        RETURN_STRING("");
    }
#endif
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(GetHostByName, 1)
    T_STRING(GetHostByName, 0);
    struct hostent *he;
    he = gethostbyname(PARAM(0));
#ifdef _WIN32
    struct in_addr addr = {
        0
    };
#else
    struct in_addr addr;
#endif

    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);
    if (he) {
        if (he->h_name)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "h_name", (INTEGER)VARIABLE_STRING, (char *)he->h_name, (NUMBER)0);
        else
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "h_name", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);

        int idx = 0;
        for (char **pAlias = he->h_aliases; *pAlias != 0; pAlias++) {
            AnsiString h_aliases("h_aliases");
            h_aliases += AnsiString((long)idx);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, h_aliases.c_str(), (INTEGER)VARIABLE_STRING, (char *)pAlias, (NUMBER)0);
        }

        if (he->h_addrtype == AF_INET) {
            int i = 0;
            while (he->h_addr_list[i] != 0) {
                addr.s_addr = *(u_long *)he->h_addr_list[i];
                AnsiString h_aliases("h_addr_list");
                h_aliases += AnsiString((long)i);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, h_aliases.c_str(), (INTEGER)VARIABLE_STRING, inet_ntoa(addr), (NUMBER)0);
                i++;
            }
        }
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(Ping, 1, 5)
    T_STRING(Ping, 0);
// count, timeout, message size (last 2 reversed);
    int nCount       = 4;
    int nMessageSize = 32;
    int nTimeOut     = 5000;

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(Ping, 2)
        nCount = PARAM_INT(2);
    }
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(Ping, 3)
        nTimeOut = PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(Ping, 4)
        nMessageSize = PARAM_INT(4);
    }
    CREATE_ARRAY(RESULT);
    int res = Ping(PARAM(0), nCount, nMessageSize, nTimeOut, Invoke, RESULT);
//RETURN_NUMBER(res);
    if (PARAMETERS_COUNT > 1) {
        SET_NUMBER(1, res);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(htonl, 1)
    T_NUMBER(htonl, 0)
    int res = htonl((unsigned int)PARAM(0));
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(htons, 1)
    T_NUMBER(htons, 0)
    int res = htons((unsigned short)PARAM(0));
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(ntohl, 1)
    T_NUMBER(ntohl, 0)
    int res = ntohl((unsigned int)PARAM(0));
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(ntohs, 1)
    T_NUMBER(ntohs, 0)
    int res = ntohs((unsigned short)PARAM(0));
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(ListInterfaces, 0)
    std::vector<DataHolder> holder;
    CREATE_ARRAY(RESULT);
    if (getInterfaces(&holder) == 0) {
        INTEGER idx = 0;
        for (std::vector<DataHolder>::const_iterator i = holder.begin(); i != holder.end(); ++i) {
            DataHolder d = *i;
            if (d.ip.size()) {
                if (!d.mac.size()) {
                    for (std::vector<DataHolder>::const_iterator i2 = holder.begin(); i2 != holder.end(); ++i2) {
                        DataHolder d2 = *i2;
                        if ((d2.adapter == d.adapter) && (d2.mac.size())) {
                            d.mac = d2.mac;
                        }
                    }
                }
                void *elem = 0;
                Invoke(INVOKE_ARRAY_VARIABLE, RESULT, idx, &elem);
                idx++;
                if (elem) {
                    CREATE_ARRAY(elem);

                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, "ip", (INTEGER)VARIABLE_STRING, (char *)d.ip.c_str(), (NUMBER)d.ip.size());
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, "mask", (INTEGER)VARIABLE_STRING, (char *)d.mask.c_str(), (NUMBER)d.mask.size());
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, "mac", (INTEGER)VARIABLE_STRING, (char *)d.mac.c_str(), (NUMBER)d.mac.size());
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, "gateway", (INTEGER)VARIABLE_STRING, (char *)d.gateway.c_str(), (NUMBER)d.gateway.size());
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, "adapter", (INTEGER)VARIABLE_STRING, (char *)d.adapter.c_str(), (NUMBER)d.adapter.size());
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, "description", (INTEGER)VARIABLE_STRING, (char *)d.description.c_str(), (NUMBER)d.description.size());
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, "flags", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)d.flags);
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, "type", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)d.type);
                }
            }
        }
    }
END_IMPL
//=====================================================================================//
int MCAST(char *addr, int port) {
    int sock = -1;
    struct sockaddr_in saddr;

    memset(&saddr, 0, sizeof(struct sockaddr_in));

    saddr.sin_family = PF_INET;
    saddr.sin_port   = htons(port); // listen on port port

    //if (inet_pton(AF_INET, addr, &saddr.sin_addr) < 0)
    //    return -3;
    struct hostent *hp;
    if ((hp = gethostbyname(addr)) == 0)
        return -1;

    saddr.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    saddr.sin_family      = AF_INET;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0)
        return -1;

    int flags = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&flags, sizeof(flags));

    if (bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) < 0) {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return -2;
    }
    return sock;
}

CONCEPT_FUNCTION_IMPL(MulticastSocket, 2)
    T_STRING(MulticastSocket, 0)
    T_NUMBER(MulticastSocket, 1)

    int res = MCAST(PARAM(0), PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MulticastJoin, 2, 3)
    T_NUMBER(MulticastJoin, 0)
    T_STRING(MulticastJoin, 1)
    char *iface = NULL;
    if (PARAMETERS_COUNT > 2) {
        T_STRING(MulticastJoin, 2)
        iface = PARAM(2);
    }

    struct ip_mreq imreq;
    memset(&imreq, 0, sizeof(struct ip_mreq));
    imreq.imr_multiaddr.s_addr = inet_addr(PARAM(1));
    if ((iface) && (iface[0]))
        imreq.imr_interface.s_addr = inet_addr(iface);
    else
        imreq.imr_interface.s_addr = INADDR_ANY;

    int res = setsockopt(PARAM_INT(0), IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&imreq, sizeof(struct ip_mreq));

    if (res) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(1);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MulticastDrop, 2, 3)
    T_NUMBER(MulticastDrop, 0)
    T_STRING(MulticastDrop, 1)
    char *iface = NULL;
    if (PARAMETERS_COUNT > 2) {
        T_STRING(MulticastDrop, 2)
        iface = PARAM(2);
    }

    struct ip_mreq imreq;
    memset(&imreq, 0, sizeof(struct ip_mreq));
    imreq.imr_multiaddr.s_addr = inet_addr(PARAM(1));
    if ((iface) && (iface[0]))
        imreq.imr_interface.s_addr = inet_addr(iface);
    else
        imreq.imr_interface.s_addr = INADDR_ANY;

    int res = setsockopt(PARAM_INT(0), IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&imreq, sizeof(struct ip_mreq));

    if (res) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(1);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(SocketSetBlocking, 2)
    T_NUMBER(SocketSetBlocking, 0)
    T_NUMBER(SocketSetBlocking, 1)

    int fd = PARAM_INT(0);
    if (fd < 0)
        return 0;
#ifdef WIN32
    unsigned long mode = (PARAM_INT(1)) ? 0 : 1;
    int           res  = ((ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false);
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        RETURN_NUMBER(0);
        return 0;
    }
    flags = (PARAM_INT(1)) ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    int res = ((fcntl(fd, F_SETFL, flags) == 0) ? true : false);
#endif
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(SocketErrno, 0)
#ifdef _WIN32
    int res = WSAGetLastError();
#else
    int res = errno;
#endif
    RETURN_NUMBER(res)
END_IMPL
//=====================================================================================//
int STUN(int sockfd, char *stun_server, char *stun_port, char *return_ip, unsigned short *return_port) {
    if ((sockfd < 0) || (sockfd == INVALID_SOCKET))
        return 0;

    struct addrinfo hints;
    struct addrinfo *res = 0;
    unsigned char   buf[300];
    unsigned char   bindingReq[20];
    unsigned short  attr_type;
    unsigned short  attr_length;
    unsigned short  port = 0;

    srand(time(NULL));
    memset(&hints, 0, sizeof hints);
    hints.ai_family            = AF_INET;
    hints.ai_socktype          = SOCK_DGRAM;
    hints.ai_protocol          = IPPROTO_UDP;
    *(short *)(&bindingReq[0]) = htons(0x0001);     // stun_method
    *(short *)(&bindingReq[2]) = htons(0x0000);     // msg_length
    *(int *)(&bindingReq[4])   = htonl(0x2112A442); // magic cookie
    int _id = rand() & 0xff;
    _id |= (rand() & 0xff) << 8;
    _id |= (rand() & 0xff) << 16;
    _id |= (rand() & 0xff) << 24;
    *(int *)(&bindingReq[8])  = htonl(_id);
    *(int *)(&bindingReq[12]) = htonl(0x0714278f);
    *(int *)(&bindingReq[16]) = htonl(0x5ded3221);

    int gerr = getaddrinfo(stun_server, stun_port, &hints, &res);
    if ((gerr != 0) || (!res))
        return 0;
    if ((res->ai_family != AF_INET) && (res->ai_family != AF_INET6)) {
        freeaddrinfo(res);
        return 0;
    }
    int n = sendto(sockfd, (char *)bindingReq, sizeof(bindingReq), 0, (struct sockaddr *)res->ai_addr, res->ai_addrlen);
    if (n == -1) {
#ifdef _WIN32
        closesocket(sockfd);
#else
        close(sockfd);
#endif
        freeaddrinfo(res);
        return 0;
    }
    freeaddrinfo(res);
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    fd_set s;
    FD_ZERO(&s);
    FD_SET(sockfd, &s);

    int val = select(FD_SETSIZE, &s, 0, 0, &timeout);
    if (val < 0)
        return 0;
    if ((!val) || (!FD_ISSET(sockfd, &s)))
        return 0;
    n = recvfrom(sockfd, (char *)buf, 300, 0, NULL, 0); // recv UDP
    if ((n > 0) && (*(short *)(&buf[0]) == htons(0x0101))) {
        // int n2 = htons(*(short *)(&buf[2]));
        if (ntohl(*(int *)(&buf[8])) != _id)
            return 0;
        int i = 20;
        while (i + 2 < n) {
            attr_type   = htons(*(short *)(&buf[i]));
            attr_length = htons(*(short *)(&buf[i + 2]));

            if ((attr_type == 0x0020) || (attr_type == 0x8020)) {
                if (i + 6 >= n)
                    return 0;
                port  = ntohs(*(short *)(&buf[i + 6]));
                port ^= 0x2112;

                *return_port = port;
                //if (ipv6) {
                // not supported
                //    return 0;
                //} else {
                if ((attr_length != 8) || (i + 11 >= n))
                    return 0;
                sprintf(return_ip, "%d.%d.%d.%d", buf[i + 8] ^ 0x21, buf[i + 9] ^ 0x12, buf[i + 10] ^ 0xA4, buf[i + 11] ^ 0x42);
                //}
                return 1;
            }
            i += (4 + attr_length);
        }
    }
    return 0;
}

CONCEPT_FUNCTION_IMPL(STUN, 3)
    T_NUMBER(STUN, 0)
    T_STRING(STUN, 1)
    T_NUMBER(STUN, 2)

    char return_ip[0xFF];
    unsigned short return_port = 0;
    return_ip[0] = 0;

    AnsiString port((long)PARAM_INT(2));
    int        res = STUN(PARAM_INT(0), PARAM(1), port.c_str(), return_ip, &return_port);
    CREATE_ARRAY(RESULT);
    if (res) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "local_address", (INTEGER)VARIABLE_STRING, (char *)return_ip, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "local_port", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)return_port);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SocketPoll, 1, 4)
    T_ARRAY(SocketPoll, 0)
    int timeout = 0;
    int check_write = 0;
    if (PARAMETERS_COUNT > 1) {
        CREATE_ARRAY(PARAMETER(1));

        if (PARAMETERS_COUNT > 2) {
            T_NUMBER(SocketPoll, 2)
            timeout = (int)PARAM_INT(2);

            if (PARAMETERS_COUNT > 3) {
                T_NUMBER(SocketPoll, 3)
                check_write = (int)PARAM_INT(3);
            }
        }
    }
    int  count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));
    void *newpData;
    if (count <= 0) {
        // nothing to do
        RETURN_NUMBER(0)
        return 0;
    }
    int index = 0;
#ifdef _WIN32
    if (count >= FD_SETSIZE) {
        RETURN_NUMBER(-2);
        return 0;
    }

    fd_set socks;
    FD_ZERO(&socks);

    for (int i = 0; i < count; i++) {
        Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(0), i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_NUMBER) {
                if ((nData > 0) && (nData != INVALID_SOCKET)) {
                    FD_SET((int)nData, &socks);
                }
            }
        }
    }

    struct timeval timeout2;
    timeout2.tv_sec  = timeout / 1000;
    timeout2.tv_usec = (timeout % 1000) * 1000;

    int res;
    if (check_write)
        res = select(FD_SETSIZE, 0, &socks, 0, &timeout2);
    else
        res = select(FD_SETSIZE, &socks, 0, 0, &timeout2);
    if ((res > 0) && (PARAMETERS_COUNT > 1)) {
        for (int i = 0; i < count; i++) {
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(0), i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_NUMBER) {
                    if ((nData > 0) && (nData != INVALID_SOCKET)) {
                        if (FD_ISSET((int)nData, &socks))
                            Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(1), index++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, nData);
                    }
                }
            }
        }
    }
#else
    struct pollfd *ufds      = (pollfd *)malloc(sizeof(pollfd) * count);
    int           real_count = 0;
    for (INTEGER i = 0; i < count; i++) {
        Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(0), i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_NUMBER) {
                if ((nData > 0) && (nData != INVALID_SOCKET)) {
                    ufds[real_count].fd     = (int)nData;
                    if (check_write)
                        ufds[real_count].events = POLLOUT;
                    else
                        ufds[real_count].events = POLLIN | POLLPRI;
                    real_count++;
                }
            }
        }
    }
    int res = poll(ufds, count, timeout);
    if ((res > 0) && (PARAMETERS_COUNT > 1)) {
        if (check_write) {
            for (int i = 0; i < count; i++) {
                if (ufds[i].revents & POLLOUT)
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(1), index++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)ufds[i].fd);
            }
        } else {
            for (int i = 0; i < count; i++) {
                if (ufds[i].revents & POLLIN)
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(1), index++, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)ufds[i].fd);
            }
        }
    }
    free(ufds);
#endif
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
#ifndef WITHOUT_LIBSCTP
void GetEvents(INVOKE_CALL Invoke, void *arr, struct sctp_event_subscribe *events) {
    INTEGER type = 0;
    char    *str = 0;
    NUMBER  nr   = 0;

    COPY_MEMBER(arr, events, sctp_data_io_event, unsigned char);
    COPY_MEMBER(arr, events, sctp_association_event, unsigned char);
    COPY_MEMBER(arr, events, sctp_address_event, unsigned char);
    COPY_MEMBER(arr, events, sctp_send_failure_event, unsigned char);
    COPY_MEMBER(arr, events, sctp_peer_error_event, unsigned char);
    COPY_MEMBER(arr, events, sctp_shutdown_event, unsigned char);
    COPY_MEMBER(arr, events, sctp_partial_delivery_event, unsigned char);
    COPY_MEMBER(arr, events, sctp_adaptation_layer_event, unsigned char);
    COPY_MEMBER(arr, events, sctp_authentication_event, unsigned char);
    // COPY_MEMBER(arr, events, sctp_sender_dry_event, unsigned char);
}
#endif

CONCEPT_FUNCTION_IMPL(SCTPSubscribe, 2)
    T_NUMBER(SCTPSubscribe, 0)
    T_ARRAY(SCTPSubscribe, 1)
#ifdef WITHOUT_LIBSCTP
    RETURN_NUMBER(-1);
#else
    int socket = PARAM_INT(0);
    if (socket < 0) {
        RETURN_NUMBER(-1);
        return 0;
    }
    struct sctp_event_subscribe events;
    memset((void *)&events, 0, sizeof(events));
    GetEvents(Invoke, PARAMETER(1), &events);
    int res = setsockopt(socket, SOL_SCTP, SCTP_EVENTS, (const void *)&events, sizeof(events));
    RETURN_NUMBER(res);
#endif
END_IMPL
//=====================================================================================//
#ifndef WITHOUT_LIBSCTP
void GetInit(INVOKE_CALL Invoke, void *arr, struct sctp_initmsg *init) {
    INTEGER type = 0;
    char    *str = 0;
    NUMBER  nr   = 0;

    COPY_MEMBER(arr, init, sinit_num_ostreams, unsigned short);
    COPY_MEMBER(arr, init, sinit_max_instreams, unsigned short);
    COPY_MEMBER(arr, init, sinit_max_attempts, unsigned short);
    COPY_MEMBER(arr, init, sinit_max_init_timeo, unsigned short);
}
#endif

CONCEPT_FUNCTION_IMPL(SCTPInit, 2)
    T_NUMBER(SCTPInit, 0)
    T_ARRAY(SCTPInit, 1)
#ifdef WITHOUT_LIBSCTP
    RETURN_NUMBER(-1);
#else
    int socket = PARAM_INT(0);
    if (socket < 0) {
        RETURN_NUMBER(-1);
        return 0;
    }

    struct sctp_initmsg init;
    memset((void *)&init, 0, sizeof(init));
    GetInit(Invoke, PARAMETER(1), &init);
    int res = setsockopt(socket, IPPROTO_SCTP, SCTP_INITMSG, &init, sizeof(init));
    RETURN_NUMBER(res);
#endif
END_IMPL
//=====================================================================================//
CONCEPT_DLL_API CONCEPT_SCTPWrite CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 8, "SCTPWrite: SCTPWrite(nSocket, anyBuffer[stream_no = 0, ttl = 0, ppid = 0, flags = 0, offset = 0, max_size = 0])");

    LOCAL_INIT;

    NUMBER sock = INVALID_SOCKET;
    char   *buffer;
    NUMBER nSize     = 0;
    NUMBER offset    = 0;

    GET_CHECK_NUMBER(0, sock, "SocketWrite: parameter 1 should be a number");
    GET_CHECK_BUFFER(1, buffer, nSize, "SocketWrite: parameter 2 should be a string buffer");

    int sent_size = 0;
    NUMBER stream_no = 0;
    NUMBER ttl = 0;
    NUMBER flags = 0;
    NUMBER ppid = 0;
    unsigned int ref_flags = 0;
    if (PARAMETERS_COUNT > 2) {
        GET_CHECK_NUMBER(2, stream_no, "SocketWrite: parameter 3 should be a number");
    }
    if (PARAMETERS_COUNT > 3) {
        GET_CHECK_NUMBER(3, ttl, "SocketWrite: parameter 4 should be a number");
    }
    if (PARAMETERS_COUNT > 4) {
        GET_CHECK_NUMBER(4, ppid, "SocketWrite: parameter 5 should be a number");
    }
    if (PARAMETERS_COUNT > 5) {
        GET_CHECK_NUMBER(5, flags, "SocketWrite: parameter 6 should be a number");
        ref_flags = (unsigned int)flags;
    }
    if (PARAMETERS_COUNT > 6) {
        GET_CHECK_NUMBER(6, offset, "SocketWrite: parameter 7 should be a number");

        buffer += (int)offset;
        nSize  -= (int)offset;
        if (nSize <= 0) {
            RETURN_NUMBER(0);
            return 0;
        }
    }
    if (PARAMETERS_COUNT > 7) {
        NUMBER max_size = 0;
        GET_CHECK_NUMBER(7, max_size, "SocketWrite: parameter 8 should be a number");
        if ((nSize > max_size) && (max_size > 0))
            nSize = max_size;
        if (nSize <= 0) {
            RETURN_NUMBER(0);
            return 0;
        }
    }
#ifdef WITHOUT_LIBSCTP
    #if defined(_WIN32) || !defined(MSG_NOSIGNAL)
        sent_size = send((int)sock, buffer, (int)nSize, 0);
    #else
        sent_size = send((int)sock, buffer, (int)nSize, MSG_NOSIGNAL);
    #endif
#else
    sent_size = sctp_sendmsg((int)sock, buffer, (int)nSize, NULL, 0, (unsigned int)ppid, ref_flags, (unsigned short)stream_no, (unsigned int)ttl, 0);
#endif

    RETURN_NUMBER(sent_size);
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SCTPRead, 3, 4)
    T_NUMBER(SCTPRead, 0)
    T_NUMBER(SCTPRead, 2)
    int sock = PARAM_INT(0);
    int max_size = PARAM_INT(2);
    int res = -1;
    if (PARAMETERS_COUNT > 3) {
        Invoke(INVOKE_CREATE_ARRAY, PARAMETER(3), 0);
    }
    if ((sock < 0) || (max_size < 0)) {
        SET_STRING(1, "")
        RETURN_NUMBER(-1);
        return 0;
    }
    if (!max_size) {
        SET_STRING(1, "")
        RETURN_NUMBER(0);
        return 0;
    }
    char *buffer = 0;
    CORE_NEW(max_size + 1, buffer);
    if (!buffer) {
        RETURN_NUMBER(-3);
        return 0;
    }
    buffer[max_size] = 0;

#ifdef WITHOUT_LIBSCTP
    res = recv((int)sock, buffer, max_size, 0);
#else
    int flags = 0;
    struct sctp_sndrcvinfo sinfo;
    memset(&sinfo, 0, sizeof(sinfo));
    res = sctp_recvmsg((int)sock, buffer, max_size, NULL, 0, &sinfo, &flags);
    if (PARAMETERS_COUNT > 3) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "sinfo_stream", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)sinfo.sinfo_stream);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "sinfo_ssn", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)sinfo.sinfo_ssn);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "sinfo_flags", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)sinfo.sinfo_flags);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "sinfo_ppid", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)sinfo.sinfo_ppid);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "sinfo_context", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)sinfo.sinfo_context);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "sinfo_timetolive", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)sinfo.sinfo_timetolive);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "sinfo_tsn", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)sinfo.sinfo_tsn);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "sinfo_cumtsn", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)sinfo.sinfo_cumtsn);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "sinfo_assoc_id", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)sinfo.sinfo_assoc_id);
    }
#endif
    if (res <= 0) {
        CORE_DELETE(buffer);
        SET_STRING(1, "");
    } else {
        SetVariable(PARAMETER(1), -1, buffer, res);
    }
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(SCTPBindx, 4)
    T_NUMBER(SCTPBindx, 0)
    T_ARRAY(SCTPBindx, 1)
    T_NUMBER(SCTPBindx, 2)
    T_NUMBER(SCTPBindx, 3)

    int res = -1;
    int  count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    void *newpData;
    if (count <= 0) {
        // nothing to do
        RETURN_NUMBER(0)
        return 0;
    }
#ifndef WITHOUT_LIBSCTP
    int family = PARAM_INT(3) ? AF_INET6 : AF_INET;
    unsigned char *ref_addr = (unsigned char *)malloc(sizeof(struct sockaddr) * count);
    struct sockaddr *addr = (struct sockaddr *)ref_addr;
    if (addr) {
        memset(addr, 0, sizeof(struct sockaddr) * count);
        int addr_index = 0;
        for (int i = 0; i < count; i++) {
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING) {
                    if (family == AF_INET6) {
                        // sockaddr array is packed!
                        struct sockaddr_in6 *sin = (struct sockaddr_in6 *)ref_addr;//&addr[addr_index];
                        if ((szData) && (szData[0]))
                            inet_pton(AF_INET6, szData, (void *)sin->sin6_addr.s6_addr);
                        else
                            sin->sin6_addr = in6addr_any;

                        sin->sin6_family = AF_INET6;
                        sin->sin6_port   = htons(0);
                        ref_addr +=  sizeof(struct sockaddr_in6);
                    } else {
                        struct sockaddr_in *sin = (struct sockaddr_in *)ref_addr;//&addr[addr_index];
                        if ((szData) && (szData[0]))
                            sin->sin_addr.s_addr = inet_addr(szData);
                        else
                            sin->sin_addr.s_addr = INADDR_ANY;
                        sin->sin_family = AF_INET;
                        sin->sin_port   = htons(0);
                        ref_addr += sizeof(struct sockaddr_in);
                    }
                    addr_index++;
                }
            }
        }
        if (addr_index > 0)
            res = sctp_bindx(PARAM_INT(0), addr, addr_index, PARAM_INT(2));
        free(addr);
    }
#endif
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(SCTPConnectx, 5)
    T_NUMBER(SCTPConnectx, 0)
    T_ARRAY(SCTPConnectx, 1)
    T_NUMBER(SCTPConnectx, 3)
    T_NUMBER(SCTPConnectx, 4)
    SET_NUMBER(2, -1);  

    int res = -1;
    int  count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    void *newpData;
    if (count <= 0) {
        // nothing to do
        RETURN_NUMBER(0)
        return 0;
    }
#ifndef WITHOUT_LIBSCTP
    int family = PARAM_INT(4) ? AF_INET6 : AF_INET;
    unsigned char *ref_addr = (unsigned char *)malloc(sizeof(struct sockaddr) * count);
    struct sockaddr *addr = (struct sockaddr *)ref_addr;
    if (addr) {
        memset(addr, 0, sizeof(struct sockaddr) * count);
        int addr_index = 0;
        for (int i = 0; i < count; i++) {
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING) {
                    struct addrinfo hints;
                    struct addrinfo *res, *result;

                    memset(&hints, 0, sizeof hints);
                    hints.ai_family   = AF_UNSPEC;
                    hints.ai_socktype = SOCK_STREAM;

                    if (getaddrinfo(szData, NULL, &hints, &result) != 0) {
                        RETURN_NUMBER(-2);
                        return 0;
                    }

                    for (res = result; res != NULL; res = res->ai_next) {
                        char hostname[NI_MAXHOST] = "";
                        if ((family == res->ai_family) || ((family == AF_INET6) && ((res->ai_family == AF_INET) || (res->ai_family == AF_INET6)))) {
                            int error = getnameinfo(res->ai_addr, res->ai_addrlen, szData, NI_MAXHOST, NULL, 0, 0);
                            if (!error) {
                                memcpy(ref_addr, res->ai_addr, res->ai_addrlen);
                                if (res->ai_family == AF_INET6) {
                                    ((struct sockaddr_in6 *)ref_addr)->sin6_port = htons(PARAM_INT(3));
                                } else {
                                    ((struct sockaddr_in *)ref_addr)->sin_port = htons(PARAM_INT(3));
                                }
                                ref_addr += res->ai_addrlen;
                                addr_index++;
                            }
                        }
                    }

                    if (result)
                        freeaddrinfo(result);
                    
                }
            }
        }
        if (addr_index > 0) {
            sctp_assoc_t id = 0;
            res = sctp_connectx(PARAM_INT(0), addr, addr_index, &id);
            SET_NUMBER(2, id);
        }
        free(addr);
    }
#endif
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SocketReusePort, 1, 2)
    T_NUMBER(SocketReusePort, 0)
    int flag = 1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(SocketReusePort, 1)
        flag = PARAM_INT(1);
    }
    int res = -2;
#ifdef SO_REUSEPORT
    res = setsockopt(PARAM_INT(0),
                SOL_SOCKET,
                SO_REUSEPORT,
                (char *)&flag,

                sizeof(flag));
#endif
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
