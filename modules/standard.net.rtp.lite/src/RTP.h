/*******
*
* FILE INFO:
* project:	RTP_lib
* file:	RTP.h
* started on:	03/26/03
* started by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
*
*
* TODO:
*
* BUGS:
*
* UPDATE INFO:
* updated on:	05/14/03 21:33:46
* updated by:	Julien Dupasquier <jdupasquier@wanadoo.fr>
*
*******/

#ifndef RTP_H
#define RTP_H

#include                "Types.h"
#include                "Network.h"



/****************************************/
/****                                ****/
/****         STRUCTURES             ****/
/****                                ****/
/****************************************/


/**
** Contributor list
**/
typedef struct   contributor {
    u_int32            ssrc;                    /* SSRC for this contributor */
    struct contributor *next;                   /* Next contributor */
} contributor_t;



/**
** RTP header body
**/
typedef struct {
    u_int8  flags;                              /* Version(2),Padding(1), Ext(1), Cc(4) */
    u_int8  mk_pt;                              /* Marker(1), PlayLoad Type(7) */
    u_int16 sq_nb;                              /* Sequence Number */
    u_int32 ts;                                 /* Timestamp */
    u_int32 ssrc;                               /* SSRC */
    u_int32 *csrc;                              /* CSRC's table */
} rtp_hdr;


/**
** RTP header extension
**/
typedef struct {
    u_int16 ext_type;                           /* Extension profile */
    u_int16 ext_len;                            /* Number of extensions */
    u_int32 *hd_ext;                            /* Extension's table */
} rtp_ext;


/**
** RTP Packet
**/
typedef struct {
    rtp_hdr *RTP_header;                        /* Header RTP */
    rtp_ext *RTP_extension;                     /* extension of the header if exist */
    char    *payload;                           /* payload portion of RTP packet */
    long    payload_len;                        /* The length of the payload */
} rtp_pkt;


/**
** RTP Context body
**/
typedef struct {
    int     context_num;                        /* Context number - send/receive */
    u_int32 my_ssrc;                            /* SSRC number - send/receive */
    u_int32 sending_pkt_count;                  /* Number of packets sent - send/receive */
    u_int32 sending_octet_count;                /* Number of bytes sent - send/receive */
    u_int8  version;                            /* Version - receive */
    u_int8  marker;                             /* Marker flag - receive */
    u_int8  padding;                            /* Padding length - receive */
    u_int8  CSRClen;                            /* CSRC length - send/receive */
    u_int8  pt;                                 /* Payload type - send/receive */
    u_int32 *CSRCList;                          /* CSRC list - send/receive */
    u_int32 my_previous_ssrc;                   /* actually unused */
    u_int32 init_RTP_timestamp;                 /* First value of timestamp - send/receive */
    u_int32 RTP_timestamp;                      /* current value of timestamp - receive */
    u_int32 time_elapsed;                       /* Time elapsed since the beginning - send/receive */
    u_int16 init_seq_no;                        /* First sequence number - send/receive */
    u_int16 seq_no;                             /* Current sequence number - send/receive */
    rtp_ext *hdr_extension;                     /* Extension header - send/receive */
    void    *conx_data;                         /* Network data */
} sess_context_t;

/****************************************/
/****                                ****/
/****         VARIABLE GLOBAL        ****/
/****                                ****/
/****************************************/

sess_context_t **context_list;
#endif /* RTP_H */
