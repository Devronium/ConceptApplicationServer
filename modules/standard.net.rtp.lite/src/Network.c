/*******
**
** FILE INFO:
** project:	RTP_lib
** file:	Network.c
** started on:	04/11/03
** started by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
**
**
** TODO:
**
** BUGS:
**
** UPDATE INFO:
** updated on:	05/13/03
** updated by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
**
********/


#include                <sys/types.h>
#ifdef _WIN32
 #include        <winsock2.h>
#else
 #include            <sys/socket.h>
 #include            <netinet/in.h>
 #include            <arpa/inet.h>
#endif

#include                <stdio.h>
#include                <stdlib.h>
#include                <string.h>
#include                <unistd.h>

#include                "Config.h"
#include                "RTP.h"
#include                "Proto.h"
#include                "Macros.h"
#include                "Network.h"


/***********************/
/** Under Unix system **/
/***********************/

/**
**  Set TTL
**/
int                     Set_TTL_Unix(SOCKET sock, u_int8 ttl) {
#ifndef _WIN32
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(u_int8));
#endif
    return 0;
}

/**************************/
/** Under Windows system **/
/**************************/

/**
**  Set TTL
**/
int                     Set_TTL_Win32(SOCKET sock, u_int8 ttl) {
    //int		buff;

    //WSAIoctl(sock, SIO_MULTICAST_SCOPE, &ttl, sizeof (u_int8), NULL, 0, &buff, NULL, NULL);
    return 0;
}

/**
**  Initialize the sockets under Windows
**/
int                     Init_Socket(void) {
#ifdef _WIN32
    WSADATA WSAData;

    WSAStartup(MAKEWORD(2, 0), &WSAData);
#endif
    return 0;
}

/**
**  Close the sockets under Windows
**/
int                     Close_Socket(void) {
    //WSACleanup();
    return 0;
}

/************************************************/
/************************************************/


/**
**  Return "1" if ip_addr is a multicast address
**/
int                     test_multicast(char *ip_addr) {
    char a_part1[4];
    char i_part1;

    strncpy(a_part1, ip_addr, 3);
    a_part1[3] = '\0';
    i_part1    = atoi(a_part1);
    if ((i_part1 & 0xe0) == 0xe0)
        return 1;
    else
        return 0;
}

/**
**  Create and bind the socket
**/
SOCKET                  Create_Socket(SOCKADDR_IN sin) {
    SOCKET sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    bind(sock, (SOCKADDR *)&sin, sizeof(sin));
    return sock;
}
