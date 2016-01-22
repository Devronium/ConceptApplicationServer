/*******
*
* FILE INFO:
* project:	RTP_lib
* file:	Types.h
* started on:	03/26/03
* started by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
*
*
* TODO:
*
* BUGS:
*
* UPDATE INFO:
* updated on:	05/13/03
* updated by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
*
*******/

#ifndef TYPES_H
#define TYPES_H

#ifndef FALSE
 #define         FALSE    0
#endif

#ifndef TRUE
 #define         TRUE    1
#endif

typedef unsigned char    u_int8;
typedef unsigned short   u_int16;
typedef unsigned long    u_int32;

typedef unsigned int     context;


/**
** Declaration for unix
**/
#ifdef UNIX
 #ifndef _WIN32
typedef int                  SOCKET;
 #endif
typedef struct sockaddr      SOCKADDR;
typedef struct sockaddr_in   SOCKADDR_IN;
#endif /* UNIX */
#endif /* TYPES_H */
