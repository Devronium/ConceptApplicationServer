/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * Copyright (c) 95-98 Faculte Polytechnique de Mons (TCTS lab)
 * All rights reserved.
 * PERMISSION IS HEREBY DENIED TO USE, COPY, MODIFY, OR DISTRIBUTE THIS
 * SOFTWARE OR ITS DOCUMENTATION FOR ANY PURPOSE WITHOUT WRITTEN
 * AGREEMENT OR ROYALTY FEES.
 *
 * File:    mbrola.h
 * Purpose: Diphone-based MBROLA speech synthesizer.
 *          Interface of the WWW DLL
 * Author:  Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 *
 * 28/08/98: Created from one_channel.h
 */
#ifndef __MBROLA_H__
#define __MBROLA_H__

#include "parser.h"

#define PITCH_SWITCH    ";; F = "
#define TIME_SWITCH     ";; T = "

typedef enum {
    LIN16 = 0, /* same as intern computation format: 16 bits linear  */
    LIN8,      /* unsigned linear 8 bits, worse than telephone        */
    ULAW,      /* MU law -> 8bits, telephone. Roughly equ. to 12bits */
    ALAW       /* A law  -> 8bits, equivallent to mulaw              */
} AudioType;

typedef void (WINAPI * PROCVV)(void);
typedef void (WINAPI * PROCVI)(int);
typedef void (WINAPI * PROCVF)(float);
typedef void (WINAPI * PROCPPAR)(Parser *);
typedef void (WINAPI * PROCVPPHO)(LPPHONE);
typedef void (WINAPI * PROCVPPHOFF)(LPPHONE, float, float);
typedef int (WINAPI * PROCIV)();
typedef int (WINAPI * PROCIC)(char *);
typedef int (WINAPI * PROCISI)(short *, int);
typedef int (WINAPI * PROCICII)(char *, int, int);
typedef int (WINAPI * PROCICCC)(char *, char *, char *);
typedef int (WINAPI * PROCIPVIAT)(void *, int, AudioType);
typedef float (WINAPI * PROCFV)();
typedef char * (WINAPI * PROCVCI)(char *, int);
typedef LPPHONE (WINAPI * PROCPPHOCF)(char *, float);

extern PROCIC     init_MBR;
extern PROCICCC   init_rename_MBR;
extern PROCIC     write_MBR;
extern PROCIV     flush_MBR;
extern PROCISI    read_MBR;
extern PROCIPVIAT readType_MBR;
extern PROCVV     close_MBR;
extern PROCVV     reset_MBR;
extern PROCIV     lastError_MBR;
extern PROCVCI    lastErrorStr_MBR;
extern PROCVI     setNoError_MBR;
extern PROCIV     getNoError_MBR;
extern PROCVI     setFreq_MBR;
extern PROCIV     getFreq_MBR;
extern PROCVF     setVolumeRatio_MBR;
extern PROCFV     getVolumeRatio_MBR;
extern PROCVCI    getVersion_MBR;
extern PROCICII   getDatabaseInfo_MBR;
extern PROCPPAR   setParser_MBR;

extern PROCPPHOCF  init_Phone;
extern PROCVPPHO   reset_Phone;
extern PROCVPPHO   close_Phone;
extern PROCVPPHOFF appendf0_Phone;

BOOL load_MBR();
void unload_MBR();
#endif
