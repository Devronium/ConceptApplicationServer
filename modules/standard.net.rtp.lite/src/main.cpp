//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <stdlib.h>
extern "C" {
#include            "Config.h"
#include            "RTP.h"
#include            "Macros.h"
#include            "Proto.h"
}
#ifdef _WIN32
 #ifdef _WIN32_WINNT
  #undef _WIN32_WINNT
 #endif
 #define _WIN32_WINNT    0x501

 #define _WINSOCKAPI_
 #include <io.h>
 #include <winsock2.h>
 #include <ws2tcpip.h>

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
 #include <signal.h>
#endif
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
 #ifndef SO_NOSIGPIPE
    signal(SIGPIPE, SIG_IGN);
 #endif
#endif
    DEFINE_ECONSTANT(MAX_CONTEXT)
    DEFINE_ECONSTANT(RTP_VERSION)
    DEFINE_ECONSTANT(MAX_PAYLOAD_LEN)
    DEFINE_ECONSTANT(MAX_PACKET_LEN)
    Random_init();
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
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Create, 1)

// ... parameter 0 is by reference (int*)
    unsigned int local_parameter_0;

    RETURN_NUMBER(RTP_Create(&local_parameter_0))
    SET_NUMBER(0, (SYS_INT)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Destroy, 1)
    T_NUMBER(0)     // int

    RETURN_NUMBER(RTP_Destroy(PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Send, 5)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // unsigned int
    T_NUMBER(2)     // unsigned char
    T_NUMBER(3)     // unsigned short
    T_STRING(4)     // unsigned char*

    RETURN_NUMBER(RTP_Send(PARAM_INT(0), (unsigned int)PARAM(1), (unsigned char)PARAM(2), (unsigned short)PARAM(3), (unsigned char *)PARAM(4), PARAM_LEN(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Build, 5)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // unsigned int
    T_NUMBER(2)     // unsigned char
    T_NUMBER(3)     // unsigned short
    T_STRING(4)     // unsigned char*

    int len = 0;
    char *res = RTP_Send2(PARAM_INT(0), (unsigned int)PARAM(1), (unsigned char)PARAM(2), (unsigned short)PARAM(3), (unsigned char *)PARAM(4), PARAM_LEN(4), &len);
    if (len > 0) {
        RETURN_BUFFER(res, len);
        free(res);
    } else {
        RETURN_STRING("")
    }
    free(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Add_Send_Addr, 4)
    T_NUMBER(0)     // int
    T_STRING(1)     // char*
    T_NUMBER(2)     // unsigned short
    T_NUMBER(3)     // unsigned char

    RETURN_NUMBER(RTP_Add_Send_Addr(PARAM_INT(0), PARAM(1), (unsigned short)PARAM(2), (unsigned char)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Rem_Send_Addr, 4)
    T_NUMBER(0)     // int
    T_STRING(1)     // char*
    T_NUMBER(2)     // unsigned short
    T_NUMBER(3)     // unsigned char

    RETURN_NUMBER(RTP_Rem_Send_Addr(PARAM_INT(0), PARAM(1), (unsigned short)PARAM(2), (unsigned char)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Set_Extension_Profile, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // unsigned short

    RETURN_NUMBER(Set_Extension_Profile(PARAM_INT(0), (unsigned short)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Add_Extension, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // unsigned int

    RETURN_NUMBER(Add_Extension(PARAM_INT(0), (unsigned int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Rem_Ext_Hdr, 1)
    T_NUMBER(0)     // int

    RETURN_NUMBER(Rem_Ext_Hdr(PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Add_CRSC, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // unsigned int

    RETURN_NUMBER(Add_CRSC(PARAM_INT(0), (unsigned int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Get_Period_us, 1)
    T_NUMBER(0)     // unsigned char

    RETURN_NUMBER(Get_Period_us((unsigned char)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Context_Create, 1)

// ... parameter 0 is by reference (int*)
    unsigned int local_parameter_0;

    RETURN_NUMBER(RTP_Context_Create(&local_parameter_0))
    SET_NUMBER(0, (SYS_INT)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Context_destroy, 1)
    T_NUMBER(0)     // int

    RETURN_NUMBER(RTP_Context_destroy((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Init_Context, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // unsigned int

    RETURN_NUMBER(Init_Context(PARAM_INT(0), (unsigned int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Session_Save_Cxinfo, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*

    RETURN_NUMBER(RTP_Session_Save_Cxinfo(PARAM_INT(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Session_Restore_Cxinfo, 2)
    T_NUMBER(0)     // int

// ... parameter 1 is by reference (void**)
    void *local_parameter_1;

    RETURN_NUMBER(RTP_Session_Restore_Cxinfo(PARAM_INT(0), &local_parameter_1))
    SET_NUMBER(1, (SYS_INT)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Sd_Pkt, 7)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // unsigned int
    T_NUMBER(2)     // unsigned char
    T_NUMBER(3)     // unsigned short
    T_STRING(4)     // unsigned char*
    T_NUMBER(5)     // int
    T_NUMBER(6)     // unsigned char

    RETURN_NUMBER(RTP_Sd_Pkt(PARAM_INT(0), (unsigned int)PARAM(1), (unsigned char)PARAM(2), (unsigned short)PARAM(3), (unsigned char *)PARAM(4), (int)PARAM(5), (unsigned char)PARAM(6)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RTP_Receive, 4)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // int

    CREATE_ARRAY(PARAMETER(3));
    struct sockaddr_storage addr;
    char        msg[MAX_PACKET_LEN];
    char        ipstr[INET6_ADDRSTRLEN];
    static char *unknown = "unknown";
    int         len      = 0;
    int         res      = RTP_Receive(PARAM_INT(0), (int)PARAM(1), msg, &len, (sockaddr *)&addr);

    const char *ip  = 0;
    int        port = 0;
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

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "address", (INTEGER)VARIABLE_STRING, (char *)ip, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "port", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)port);

    RETURN_NUMBER(res);
    if (len > 0) {
        SET_BUFFER(2, msg, len);
    } else {
        SET_STRING(2, "");
    }
END_IMPL
