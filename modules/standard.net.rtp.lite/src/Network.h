/*******
**
** FILE INFO:
** project:	RTP_lib
** file:	Network.h
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

#ifndef NETWORK_H
#define NETWORK_H


#include                "Types.h"


typedef struct remote_address {
    SOCKET                socket;                       /* socket */
    char                  address[16];                  /* IP address */
    u_int16               port;                         /* Port number */
    u_int8                ttl;                          /* ttl */
    struct remote_address *next;                        /* next address */
} remote_address_t;


/* Structs */
typedef struct {
    /* Network */
    int              connection_opened;                 /* equal "1" if the connection is opened */
    remote_address_t *send_addr_list;                   /* list of the tragets */
} conx_context_t;
#endif /* NETWORK_H */
