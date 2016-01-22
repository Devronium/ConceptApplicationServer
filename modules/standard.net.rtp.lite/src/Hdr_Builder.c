/*******
**
** FILE INFO:
** project:	RTP_lib
** file:	Hdr_Builder.c
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
#include                "Macros.h"
#include                "Types.h"
#include                "Proto.h"



/**
**  Set the version of RTP
**/
int                     Set_Version(rtp_hdr *header) {
    F_V_FLAGS(header->flags, RTP_VERSION);

    return 0;
}

/* Set the padding flag */
int                     Set_Padding(rtp_hdr *header, int value) {
    if (value > 0)
        F_P_FLAGS(header->flags, 1);
    else
        F_P_FLAGS(header->flags, 0);
    return 0;
}

/* Set the extention flag */
int                     Set_Ext(rtp_hdr *header, int ext_len) {
    if (ext_len > 0)
        F_X_FLAGS(header->flags, 1);
    else
        F_X_FLAGS(header->flags, 0);
    return 0;
}

/* Set CSRC count */
int                     Set_CSRC_Cnt(rtp_hdr *header, int cc) {
    F_CC_FLAGS(header->flags, cc);
    return 0;
}

/* Set the marker flag */
int                     Set_Marker(rtp_hdr *header, int marker_flg) {
    F_M_MKPT(header->mk_pt, marker_flg);
    return 0;
}

/* Set the payload type */
int                     Set_Payload_Type(rtp_hdr *header, int pti) {
    F_PT_MKPT(header->mk_pt, pti);
    return 0;
}

/* Set the sequence number */
int                     Set_Seq_Nb(rtp_hdr *header, u_int16 sq_nb) {
    header->sq_nb = htons(sq_nb);
    return 0;
}

/* Set the timestamp */
int                     Set_TS(rtp_hdr *header, u_int32 ts) {
    header->ts = htonl(ts);
    return 0;
}

/* Set the synchronisation source */
int                     Set_SSRC(rtp_hdr *header, u_int32 ssrc) {
    header->ssrc = htonl(ssrc);
    return 0;
}

int                     RTP_Build_Header(
    context        cid,
    rtp_hdr        *rtp_header,
    int            padding,
    u_int8         marker,
    int            ext,
    sess_context_t *context_list,
    u_int16        pti,
    u_int32        ts
    ) {
    Set_Version(rtp_header);
    Set_Padding(rtp_header, padding);
    Set_Ext(rtp_header, ext);
    Set_CSRC_Cnt(rtp_header, context_list->CSRClen);
    Set_Marker(rtp_header, marker);
    Set_Payload_Type(rtp_header, pti);
    Set_Seq_Nb(rtp_header, context_list->seq_no);
    Set_TS(rtp_header, ts);
    Set_SSRC(rtp_header, context_list->my_ssrc);
    return 0;
}
