/*******
**
** FILE INFO:
** project:	RTP_lib
** file:	Proto.h
** started on:	05/01/03
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

#ifndef PROTO_H
#define PROTO_H

#include                "Types.h"
#include                "RTP.h"


/* Hdr_Builder.c */
int                     Set_Version(rtp_hdr *);
int                     Set_Padding(rtp_hdr *, int);
int                     Set_Ext(rtp_hdr *, int);
int                     Set_CSRC_Cnt(rtp_hdr *, int);
int                     Set_Marker(rtp_hdr *, int);
int                     Set_Payload_Type(rtp_hdr *, int);

int Set_Seq_Nb(rtp_hdr *, u_int16);
int Set_TS(rtp_hdr *, u_int32);
int Set_SSRC(rtp_hdr *, u_int32);
int RTP_Build_Header(context, rtp_hdr *, int, u_int8, int ext, sess_context_t *, u_int16, u_int32);

/* RTP.c */
int                     RTP_Create(context *);

int RTP_Destroy(context);
int RTP_Send(context, u_int32, u_int8, u_int16, u_int8 *, int);
char *RTP_Send2(context cid, u_int32 tsinc, u_int8 marker, u_int16 pti, u_int8 *payload, int len, int *out_len);

//int			RTP_Receive(context, int, char *, int *, struct sockaddr *);

int RTP_Add_Send_Addr(context, char *, u_int16, u_int8);
int RTP_Rem_Send_Addr(context, char *, u_int16, u_int8);

int Set_Extension_Profile(context, u_int16);
int Add_Extension(context, u_int32);
int Rem_Ext_Hdr(context);

int Add_CRSC(context, u_int32);

u_int32 Get_Period_us(u_int8);

int                     RTP_Context_Create(context *);

int RTP_Context_destroy(context);
int Init_Context(context, u_int32);
int RTP_Session_Save_Cxinfo(context, void *);
int RTP_Session_Restore_Cxinfo(context, void **);
int RTP_Sd_Pkt(context, u_int32, u_int8, u_int16, u_int8 *, int, u_int8);

rtp_pkt *Get_RTP_Hdr(char *, int);
void                    Free_Tmp_Mem(rtp_pkt *);

int RTP_Receive(context, int, char *, int *, struct sockaddr *);

/* Utils.c */
int                     Random_init(void);
u_int32                 Random32(int);

/* Network.c */
int                     Init_Socket(void);
int                     Close_Socket(void);

SOCKET Create_Socket(SOCKADDR_IN);
int    Set_TTL_Unix(SOCKET, u_int8);
int    Set_TTL_Win32(SOCKET, u_int8);

/* Rtp_Exemple_Receive.c */
void                    print_hdr(rtp_pkt *);
#endif /* PROTO_H */
