/*******
*
* FILE INFO:
* project:	RTP_lib
* file:	RTP.c
* started on:	03/26/03
* started by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
*
*
* TODO:
*
* BUGS:
*
* UPDATE INFO:
* updated on:	05/15/03 16:21:40
* updated by:	Julien Dupasquier <jdupasquier@wanadoo.fr>
*
*******/


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
//#include		<err.h>
#include                <math.h>

#include                "Config.h"
#include                "RTP.h"
#include                "Macros.h"
#include                "Proto.h"


/****************************************/
/****                                ****/
/****       GLOBALES VARIABLES       ****/
/****                                ****/
/****************************************/

u_int32 payload_type[] =
{
    8000,   8000,  8000,  8000,  8000,  8000, 16000,  8000,  8000, 8000, /* 0-9 */
    44100, 44100,     0,     0, 90000,  8000, 11025, 22050,     0, 0,    /* 10-19 */
    0,         0,     0,     0,     0, 90000, 90000,     0, 90000, 0,    /* 20-29 */
    0,     90000, 90000, 90000, 90000,     0,     0,     0,     0, 0,    /* 30-39 */
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0,    /* 40-49 */
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0,    /* 50-59 */
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0,    /* 60-69 */
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0,    /* 70-79 */
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0,    /* 80-89 */
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0,    /* 90-99 */
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0,    /* 100-109 */
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0,    /* 110-119 */
    0,         0,     0,     0,     0,     0,     0, 0                   /* 120-127 */
};

//sess_context_t		**context_list	= NULL;
u_int32 nb_context    = 0;
u_int32 above_context = 0;
u_int32 *tab_ssrc     = NULL;



/****************************************/
/****                                ****/
/****           FUNCTIONS            ****/
/****                                ****/
/****************************************/


/**
**  Alloc the context structure and initialize it
**/
int                     RTP_Create(context *the_context) {
    conx_context_t *conx_cc;

    RTP_Context_Create(the_context);
    conx_cc = (conx_context_t *)malloc(sizeof(conx_context_t));
    conx_cc->connection_opened = FALSE;
    conx_cc->send_addr_list    = NULL;
    RTP_Session_Save_Cxinfo(*the_context, conx_cc);

    return 0;
}

/**
**  Destroy the RTP session
**/
int                     RTP_Destroy(context cid) {
    conx_context_t   *conx_cc = NULL;
    remote_address_t *buff;
    remote_address_t *buff2;

    RTP_Session_Restore_Cxinfo(cid, (void **)&conx_cc);
    buff = conx_cc->send_addr_list;
    while (buff) {
        //closesocket(buff->socket);
        buff2 = buff->next;
        free(buff);
        buff = buff2;
    }
    free(conx_cc);
    RTP_Context_destroy(cid);
    return 0;
}

/**
**  Send the packet and update the context
**/
int                     RTP_Send(context cid, u_int32 tsinc, u_int8 marker, u_int16 pti, u_int8 *payload, int len) {
    u_int32 ts;
    u_int8  padd_len;

    padd_len = MAX_PAYLOAD_LEN - len;
    ts       = context_list[cid]->init_RTP_timestamp + context_list[cid]->time_elapsed;

    RTP_Sd_Pkt(cid, ts, marker, pti, payload, len, padd_len);

    if (context_list[cid]->seq_no == 65535)
        context_list[cid]->seq_no = 0;
    else
        context_list[cid]->seq_no++;
    context_list[cid]->time_elapsed += tsinc;
    context_list[cid]->sending_pkt_count++;
    context_list[cid]->sending_octet_count += len;
    return 0;
}

char *RTP_Sd_Pkt2(context cid, u_int32 ts, u_int8 marker, u_int16 pti, u_int8 *payload, int len, u_int8 padd_len, int *out_len) {
    rtp_pkt          *rtp_packet;
    int              padding  = 0;
    int              ext      = 0;
    u_int8           csrc_len = 0;
    u_int8           csrc_sz  = 0;
    SOCKADDR_IN      sin;
    u_int8           *sd_buffer  = NULL;
    u_int8           hd_len      = 12;
    u_int16          hd_ext_len  = 0;
    conx_context_t   *conx_cc    = NULL;
    remote_address_t *remote_dev = NULL;
    int              sd_buffer_len;

    MEM_ALLOC(rtp_packet);
    MEM_ALLOC(rtp_packet->RTP_header);
    if (context_list[cid]->CSRClen) {
        csrc_len = context_list[cid]->CSRClen;
        csrc_sz  = csrc_len * 4;
        rtp_packet->RTP_header->csrc = context_list[cid]->CSRCList;
    }
    rtp_packet->payload     = payload;
    rtp_packet->payload_len = len;
    RTP_Session_Restore_Cxinfo(cid, (void **)&conx_cc);
    hd_len += csrc_sz;
    if (padd_len > 0)
        padding = 1;
    if (context_list[cid]->hdr_extension) {
        ext        = 1;
        hd_ext_len = 4 * (1 + ntohs(context_list[cid]->hdr_extension->ext_len));
        hd_len    += hd_ext_len;
        rtp_packet->RTP_extension = context_list[cid]->hdr_extension;
    }

    sd_buffer     = (u_int8 *)malloc(hd_len + len + padd_len);
    sd_buffer_len = hd_len + len + padd_len;
    RTP_Build_Header(cid, rtp_packet->RTP_header, padding, marker, ext, context_list[cid], pti, ts);
    memcpy(sd_buffer, rtp_packet->RTP_header, 12);
    memcpy(sd_buffer + 12, rtp_packet->RTP_header->csrc, csrc_sz);
    memcpy(sd_buffer + 12 + csrc_sz, rtp_packet->RTP_extension, 4);
    memcpy(sd_buffer + 16 + csrc_sz, rtp_packet->RTP_extension->hd_ext, (hd_ext_len - 4));
    memcpy(sd_buffer + hd_len, rtp_packet->payload, rtp_packet->payload_len);
    memset(sd_buffer + hd_len + len, 0, padd_len);
    if (padd_len > 0)
        sd_buffer[sd_buffer_len - 1] = padd_len;
    *out_len = sd_buffer_len;

    rtp_packet->payload       = NULL;
    rtp_packet->RTP_extension = NULL;
    free(rtp_packet->RTP_header);
    rtp_packet->RTP_header = NULL;
    //free(rtp_packet->RTP_header);
    //free(rtp_packet);
    Free_Tmp_Mem(rtp_packet);
    return sd_buffer;
}

char *RTP_Send2(context cid, u_int32 tsinc, u_int8 marker, u_int16 pti, u_int8 *payload, int len, int *out_len) {
    u_int32 ts;
    u_int8  padd_len;

    padd_len = MAX_PAYLOAD_LEN - len;
    ts       = context_list[cid]->init_RTP_timestamp + context_list[cid]->time_elapsed;

    char *res = RTP_Sd_Pkt2(cid, ts, marker, pti, payload, len, padd_len, out_len);

    if (context_list[cid]->seq_no == 65535)
        context_list[cid]->seq_no = 0;
    else
        context_list[cid]->seq_no++;
    context_list[cid]->time_elapsed += tsinc;
    context_list[cid]->sending_pkt_count++;
    context_list[cid]->sending_octet_count += len;
    return res;
}

/**
**  Receive the packet
**/

/*
   int			RTP_Receive(context cid, int fd, char *msg, int *len, struct sockaddr	*sin)
   {
        return (0);
   }
 */


/**
**  Add a send address and port
**/
int                     RTP_Add_Send_Addr(context cid, char *addr, u_int16 port, u_int8 ttl) {
    conx_context_t   *conx_cc;
    remote_address_t *remote_dev_data;
    SOCKADDR_IN      sin;
    int ismulticast;

    RTP_Session_Restore_Cxinfo(cid, (void **)&conx_cc);
    remote_dev_data = (remote_address_t *)malloc(sizeof(remote_address_t));
    strcpy(remote_dev_data->address, addr);
    remote_dev_data->port = port;
    ismulticast           = test_multicast(remote_dev_data->address);
    if (ismulticast)
        remote_dev_data->ttl = ttl;
    else
        remote_dev_data->ttl = 0;
    sin.sin_addr.s_addr = inet_addr(remote_dev_data->address);
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(port);
    if ((remote_dev_data->socket = Create_Socket(sin)) == -1)
        return -1;
    if (ismulticast) {
#ifdef UNIX
        Set_TTL_Unix(remote_dev_data->socket, remote_dev_data->ttl);
#endif
#ifdef WINDOWS
        Set_TTL_Win32(remote_dev_data->socket, remote_dev_data->ttl);
#endif
    }
    remote_dev_data->next   = conx_cc->send_addr_list;
    conx_cc->send_addr_list = remote_dev_data;
    RTP_Session_Save_Cxinfo(cid, conx_cc);
    return 0;
}

/**
**  Remove a send address and port
**/
int                     RTP_Rem_Send_Addr(context cid, char *addr, u_int16 port, u_int8 ttl) {
    conx_context_t   *conx_cc;
    remote_address_t *buffer1;
    remote_address_t *buffer2;

    RTP_Session_Restore_Cxinfo(cid, (void **)&conx_cc);
    buffer1 = (remote_address_t *)malloc(sizeof(remote_address_t));
    buffer2 = (remote_address_t *)malloc(sizeof(remote_address_t));
    buffer1 = conx_cc->send_addr_list;
    if (!test_multicast(addr))
        ttl = 0;
    if (buffer1) {
        if (!strcmp(buffer1->address, addr) && (buffer1->port == port) && (buffer1->ttl == ttl))
            conx_cc->send_addr_list = buffer1->next;
        else {
            while (buffer1) {
                buffer2 = buffer1;
                buffer1 = buffer1->next;
                if (buffer1) {
                    if (!strcmp(buffer1->address, addr) && (buffer1->port == port) && (buffer1->ttl == ttl)) {
                        buffer2->next = buffer1->next;
                        break;
                    }
                }
            }
        }
    }
    RTP_Session_Save_Cxinfo(cid, conx_cc);
    return 0;
}

/**
**  Set the profile in the extension header
**/
int                     Set_Extension_Profile(context cid, u_int16 profile) {
    if (context_list[cid]->hdr_extension) {
        free(context_list[cid]->hdr_extension->hd_ext);
        free(context_list[cid]->hdr_extension);
    }
    MEM_ALLOC(context_list[cid]->hdr_extension);
    context_list[cid]->hdr_extension->ext_type = htons(profile);
    return 0;
}

/**
**  Add an extension
**/
int                     Add_Extension(context cid, u_int32 extension) {
    u_int16 nb_ext;

    if (!context_list[cid]->hdr_extension)
        MEM_ALLOC(context_list[cid]->hdr_extension);
    if (!(context_list[cid]->hdr_extension->hd_ext)) {
        MEM_ALLOC(context_list[cid]->hdr_extension->hd_ext);
        context_list[cid]->hdr_extension->hd_ext[0] = htonl(extension);
        context_list[cid]->hdr_extension->ext_len   = htons(1);
    } else {
        nb_ext = ntohs(context_list[cid]->hdr_extension->ext_len);
        nb_ext++;
        context_list[cid]->hdr_extension->ext_len            = htons(nb_ext);
        context_list[cid]->hdr_extension->hd_ext             = realloc(context_list[cid]->hdr_extension->hd_ext, sizeof(u_int32) * nb_ext);
        context_list[cid]->hdr_extension->hd_ext[nb_ext - 1] = htonl(extension);
    }
    return 0;
}

/**
**  Remove extension header
**/
int                     Rem_Ext_Hdr(context cid) {
    free(context_list[cid]->hdr_extension);
    return 0;
}

/**
** Add a CSRC
**/
int                     Add_CRSC(context cid, u_int32 csrc) {
    if (!(context_list[cid]->CSRCList)) {
        MEM_ALLOC(context_list[cid]->CSRCList);
        context_list[cid]->CSRCList[0] = htonl(csrc);
        context_list[cid]->CSRClen     = 1;
    } else {
        context_list[cid]->CSRClen++;
        context_list[cid]->CSRCList = realloc(context_list[cid]->CSRCList, sizeof(u_int32) * context_list[cid]->CSRClen);
        context_list[cid]->CSRCList[context_list[cid]->CSRClen - 1] = htonl(csrc);
    }
    return 0;
}

/**
**  Return the period
**/
u_int32                 Get_Period_us(u_int8 pt) {
    if (payload_type[pt])
        return 1000000 / payload_type[pt];
    else
        return 0;
}

/******************************************************************/
/******************************************************************/

/**
**  Save data about connexion in the context structure
**/
int                     RTP_Session_Save_Cxinfo(context cid, void *info) {
    context_list[cid]->conx_data = info;
    return 0;
}

/**
**  Restore data about connexion in the context structure
**/
int                     RTP_Session_Restore_Cxinfo(context cid, void **info) {
    *info = context_list[cid]->conx_data;
    return 0;
}

/**
**  Initialize the context
**/
int                     Init_Context(context context_num, u_int32 nb_context) {
    Random_init();
    context_list[context_num]->context_num         = context_num;
    context_list[context_num]->CSRClen             = 0;
    context_list[context_num]->CSRCList            = 0;
    context_list[context_num]->init_RTP_timestamp  = Random32(context_num);
    context_list[context_num]->RTP_timestamp       = 0;
    context_list[context_num]->init_seq_no         = (Random32(context_num) % 65536);
    context_list[context_num]->my_previous_ssrc    = 0;
    context_list[context_num]->my_ssrc             = Random32(context_num);
    context_list[context_num]->sending_pkt_count   = 0;
    context_list[context_num]->sending_octet_count = 0;
    context_list[context_num]->seq_no        = context_list[context_num]->init_seq_no;
    context_list[context_num]->time_elapsed  = 0;
    context_list[context_num]->version       = 0;
    context_list[context_num]->marker        = 0;
    context_list[context_num]->padding       = 0;
    context_list[context_num]->pt            = 0;
    context_list[context_num]->hdr_extension = NULL;

    if (nb_context == 1) {
        MEM_ALLOC(tab_ssrc);
        tab_ssrc[0] = context_list[context_num]->my_ssrc;
    } else {
        tab_ssrc = realloc(tab_ssrc, sizeof(u_int32) * nb_context);
        tab_ssrc[nb_context - 1] = context_list[context_num]->my_ssrc;
    }
    return 0;
}

/**
**  Create the context
**/
int                     RTP_Context_Create(context *cid) {
    context i;

    if (nb_context < above_context) {
        for (i = 0; i < above_context; i++) {
            if (context_list[i] == NULL)
                break;
        }
    } else if (context_list == NULL) {
        i             = above_context;
        above_context = MAX_CONTEXT;
        context_list  = (sess_context_t **)calloc(MAX_CONTEXT, sizeof(sess_context_t *));
    } else {
        i              = above_context;
        above_context += 10;
        context_list   = realloc(context_list, sizeof(sess_context_t *) * above_context);
    }
    context_list[i] = malloc(sizeof(sess_context_t));
    nb_context++;
    Init_Context(i, nb_context);
    *cid = i;
    return 0;
}

/**
**  Destroy the context
**/
int                     RTP_Context_destroy(context cid) {
    int i1;
    int i2;

    if (nb_context == 1) {
        free(tab_ssrc);
        nb_context--;
    } else {
        for (i1 = 0; i1 < nb_context; i1++) {
            if (context_list[cid]->my_ssrc == tab_ssrc[i1]) {
                for (i2 = i1; i2 < nb_context; i2++)
                    tab_ssrc[i2] = tab_ssrc[i2 + 1];
                tab_ssrc[nb_context - 1] = 0;
                nb_context--;
                tab_ssrc = realloc(tab_ssrc, sizeof(u_int32) * nb_context);
                break;
            }
        }
    }
    if (context_list[cid]->CSRCList)
        free(context_list[cid]->CSRCList);
    if (context_list[cid]->hdr_extension) {
        free(context_list[cid]->hdr_extension->hd_ext);
        free(context_list[cid]->hdr_extension);
    }
    free(context_list[cid]);
    context_list[cid] = NULL;
    return 0;
}

/**
**  Send the packet
**/
int                     RTP_Sd_Pkt(context cid, u_int32 ts, u_int8 marker, u_int16 pti, u_int8 *payload, int len, u_int8 padd_len) {
    rtp_pkt          *rtp_packet;
    int              padding  = 0;
    int              ext      = 0;
    u_int8           csrc_len = 0;
    u_int8           csrc_sz  = 0;
    SOCKADDR_IN      sin;
    u_int8           *sd_buffer  = NULL;
    u_int8           hd_len      = 12;
    u_int16          hd_ext_len  = 0;
    conx_context_t   *conx_cc    = NULL;
    remote_address_t *remote_dev = NULL;
    int              sd_buffer_len;

    MEM_ALLOC(rtp_packet);
    MEM_ALLOC(rtp_packet->RTP_header);
    if (context_list[cid]->CSRClen) {
        csrc_len = context_list[cid]->CSRClen;
        csrc_sz  = csrc_len * 4;
        MEM_SALLOC(rtp_packet->RTP_header->csrc, csrc_sz);
        rtp_packet->RTP_header->csrc = context_list[cid]->CSRCList;
    }
    MEM_SALLOC(rtp_packet->payload, len + padd_len);

    rtp_packet->payload     = payload;
    rtp_packet->payload_len = len;
    RTP_Session_Restore_Cxinfo(cid, (void **)&conx_cc);
    hd_len += csrc_sz;
    if (padd_len > 0)
        padding = 1;
    if (context_list[cid]->hdr_extension) {
        ext        = 1;
        hd_ext_len = 4 * (1 + ntohs(context_list[cid]->hdr_extension->ext_len));
        hd_len    += hd_ext_len;
        MEM_SALLOC(rtp_packet->RTP_extension, hd_ext_len);
        rtp_packet->RTP_extension = context_list[cid]->hdr_extension;
    }

    sd_buffer     = (u_int8 *)malloc(hd_len + len + padd_len);
    sd_buffer_len = hd_len + len + padd_len;
    RTP_Build_Header(cid, rtp_packet->RTP_header, padding, marker, ext, context_list[cid], pti, ts);
    memcpy(sd_buffer, rtp_packet->RTP_header, 12);
    memcpy(sd_buffer + 12, rtp_packet->RTP_header->csrc, csrc_sz);
    memcpy(sd_buffer + 12 + csrc_sz, rtp_packet->RTP_extension, 4);
    memcpy(sd_buffer + 16 + csrc_sz, rtp_packet->RTP_extension->hd_ext, (hd_ext_len - 4));
    memcpy(sd_buffer + hd_len, rtp_packet->payload, rtp_packet->payload_len);
    memset(sd_buffer + hd_len + len, 0, padd_len);
    if (padd_len > 0)
        sd_buffer[sd_buffer_len - 1] = padd_len;

    remote_dev = conx_cc->send_addr_list;
    while (remote_dev) {
        sin.sin_addr.s_addr = inet_addr(remote_dev->address);
        sin.sin_family      = AF_INET;
        sin.sin_port        = htons(remote_dev->port);
        sendto(remote_dev->socket, sd_buffer, sd_buffer_len, 0, (SOCKADDR *)&sin, sizeof(sin));
        remote_dev = remote_dev->next;
    }
    free(sd_buffer);
    return 0;
}

u_int8                  Get_Padding(rtp_hdr *rtp_hdr_msg, char *msg, int sz) {
    char   padding;
    u_int8 len_padding;

    /*
     * Gestion du padding.
     */
    padding = (rtp_hdr_msg->flags & 0x20) >> 5;
    if (padding)
        memcpy(&len_padding, msg + (sz - 1), 1);
    else
        len_padding = 0;
    return len_padding;
}

rtp_pkt *Get_RTP_Hdr(char *msg, int sz) {
    char    cc;
    char    ext;
    u_int16 hdr_sz;
    u_int8  len_padding;
    rtp_hdr *rtp_hdr_msg;
    rtp_ext *rtp_ext_msg;
    rtp_pkt *rtp_pkt_msg;

    /*
     * Recuperation du rtp_header.
     */
    MEM_SALLOC(rtp_hdr_msg, sizeof(rtp_hdr));
    memcpy(rtp_hdr_msg, msg, 12);
    hdr_sz = 12;

    /*
     * Recuperation du csrc.
     */
    cc = (rtp_hdr_msg->flags) & 0x0f;
    if (cc) {
        MEM_SALLOC(rtp_hdr_msg->csrc, (cc * 4));
        memcpy(rtp_hdr_msg->csrc, msg + 12, (cc * 4));
        hdr_sz += (cc * 4);
    }

    /*
     * Recuperation de Ext_type et Ext_len, puis des Ext.
     */
    ext = (rtp_hdr_msg->flags & 0x10) >> 4;
    if (ext) {
        MEM_ALLOC(rtp_ext_msg);
        memcpy(rtp_ext_msg, msg + 12 + (cc * 4), 4);
        rtp_ext_msg->ext_len = ntohs(rtp_ext_msg->ext_len);
        MEM_SALLOC(rtp_ext_msg->hd_ext, 4 * rtp_ext_msg->ext_len);
        memcpy(rtp_ext_msg->hd_ext, msg + 16 + (cc * 4), (rtp_ext_msg->ext_len * 4));
        hdr_sz += (rtp_ext_msg->ext_len * 4) + 4;
    }

    /*
     * Gestion du padding.
     */
    len_padding = Get_Padding(rtp_hdr_msg, msg, sz);

    /*
     * Revoie du packet.
     */
    MEM_ALLOC(rtp_pkt_msg);
    rtp_pkt_msg->RTP_header    = rtp_hdr_msg;
    rtp_pkt_msg->RTP_extension = rtp_ext_msg;
    rtp_pkt_msg->payload_len   = sz - (hdr_sz + len_padding);
    MEM_SALLOC(rtp_pkt_msg->payload, rtp_pkt_msg->payload_len);
    memcpy(rtp_pkt_msg->payload, msg + hdr_sz, rtp_pkt_msg->payload_len);

    return rtp_pkt_msg;
}

void                    Free_Tmp_Mem(rtp_pkt *pkt) {
    /*
     * Free Memoire.
     */
    if (pkt->RTP_extension) {
        free(pkt->RTP_extension->hd_ext);
        free(pkt->RTP_extension);
    }
    if (pkt->RTP_header) {
        free(pkt->RTP_header->csrc);
        free(pkt->RTP_header);
    }
    free(pkt->payload);
    free(pkt);
    return;
}

void                    Put_Pkt_in_Context(rtp_pkt *pkt, char *msg, int cid, int sz) {
    int i;

    /* SSRC number - send/receive */
    context_list[cid]->my_ssrc = ntohl(pkt->RTP_header->ssrc);

    /* Number of packets sent - send/receive */
    context_list[cid]->sending_pkt_count++;

    /* Number of bytes sent - send/receive */
    context_list[cid]->sending_octet_count += sz;

    /* Version - receive */
    context_list[cid]->version = (pkt->RTP_header->flags & 0xd0) >> 6;

    /* Marker flag - receive */
    context_list[cid]->marker = (pkt->RTP_header->mk_pt & 0x10) >> 7;

    /* Padding length - receive */
    context_list[cid]->padding = Get_Padding(pkt->RTP_header, msg, sz);

    /* CSRC length - send/receive */
    context_list[cid]->CSRClen = (pkt->RTP_header->flags & 0x0f);

    /* Payload type - send/receive */
    context_list[cid]->pt = (pkt->RTP_header->mk_pt & 0x7f);

    /* CSRC list - send/receive */
    MEM_SALLOC(context_list[cid]->CSRCList, (context_list[cid]->CSRClen * 4));
    memcpy(context_list[cid]->CSRCList, pkt->RTP_header->csrc, (context_list[cid]->CSRClen * 4));

    /* current value of timestamp - receive */
    context_list[cid]->RTP_timestamp = ntohl(pkt->RTP_header->ts);

    /* First value of timestamp - send/receive */
    if (context_list[cid]->sending_pkt_count == 1)
        context_list[cid]->init_RTP_timestamp = context_list[cid]->RTP_timestamp;

    /* Time elapsed since the beginning - send/receive */
    if (context_list[cid]->sending_pkt_count == 1)
        context_list[cid]->time_elapsed = 0;
    else
        context_list[cid]->time_elapsed = context_list[cid]->RTP_timestamp - context_list[cid]->init_RTP_timestamp;

    /* Current sequence number - send/receive */
    context_list[cid]->seq_no = ntohs(pkt->RTP_header->sq_nb);

    /* First sequence number - send/receive */
    if (context_list[cid]->sending_pkt_count == 1)
        context_list[cid]->init_seq_no = context_list[cid]->seq_no;

    /* Extension header - send/receive */
    if (context_list[cid]->hdr_extension) {
        free(context_list[cid]->hdr_extension->hd_ext);
        free(context_list[cid]->hdr_extension);
    }
    MEM_ALLOC(context_list[cid]->hdr_extension);
    context_list[cid]->hdr_extension->ext_len  = ntohs(pkt->RTP_extension->ext_len);
    context_list[cid]->hdr_extension->ext_type = ntohs(pkt->RTP_extension->ext_type);
    if (context_list[cid]->hdr_extension->ext_len) {
        MEM_SALLOC(context_list[cid]->hdr_extension->hd_ext, (4 * context_list[cid]->hdr_extension->ext_len));
        memcpy(context_list[cid]->hdr_extension->hd_ext, pkt->RTP_extension->hd_ext, (4 * context_list[cid]->hdr_extension->ext_len));
        for (i = 0; i < context_list[cid]->hdr_extension->ext_len; i++)
            context_list[cid]->hdr_extension->hd_ext[i] = ntohl(context_list[cid]->hdr_extension->hd_ext[i]);
    }
}

int                     RTP_Receive(context cid, int fd, char *payload, int *len, struct sockaddr *sin) {
    char    *msg;
    int     sin_len;
    int     sz;
    char    buf[MAX_PACKET_LEN];
    rtp_pkt *pkt;

    /*
     * Reception du paquets.
     */
    sin_len = sizeof(struct sockaddr_in);
    sz      = recvfrom(fd, buf, MAX_PACKET_LEN, 0, sin, &sin_len);

    /*
     * Stockage du paquets.
     */
    MEM_SALLOC(msg, sz);
    memcpy(msg, buf, sz);
    bzero(buf, MAX_PACKET_LEN);

    /*
     * Recuperation du RTP_Header, RTP_extension, Payload.
     */
    pkt = Get_RTP_Hdr(msg, sz);

    /*
     * Integration du paquets dans le context.
     */
    //Put_Pkt_in_Context(pkt, msg, cid, sz);

    /*
     * Gestion du payload.
     */
    *len = pkt->payload_len;
    memcpy(payload, pkt->payload, *len);
    free(msg);

    /*
     * Print Structure.
     *
     * print_hdr(pkt);
     */

    /*
     * Free Memoire.
     */
    Free_Tmp_Mem(pkt);
}
