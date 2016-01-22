/*
 *
 * Copyright 2010  Robert Guan
 * All rights reserved.
 * to use is subject to license terms.
 *
 */

#ifndef __AES_INTERNAL__
#define __AES_INTERNAL__

//#include <xmmintrin.h>

//#include <emmintrin.h>

// block size
#define Nb       4
// key size
#define Nk       4
// number of rounds
#define Nr       10

#define ALIGN    __attribute__ ((aligned(32)))


#ifndef uint8
typedef unsigned char        uint8;
#endif

#ifndef uint16
typedef unsigned short       uint16;
#endif

#ifndef uint32
typedef unsigned int         uint32;
#endif

#ifndef uint64
typedef unsigned long long   uint64;
#endif



typedef struct _AESContext_ {
    uint32 AesEncKey[44];
    uint32 AesDecKey[44];
    uint32 InitialVector[4];
    uint32 iFeedBackSz;
} AESContext;



extern void aes_encrypt(uint32 *input, uint32 *output, uint32 *pKey);
extern void aes_decrypt(uint32 *input, uint32 *output, uint32 *pKey);

//extern void AesMemcpy(void * pDst , void * pSrc,  int Size);

//extern uint32 * AesEncKey;
extern void AddRoundKey(uint32 *pInput, int iKeyIdx);
extern void  AddRoundKeyTo(uint32 *pInput, int iKeyIdx, uint32 *pOutput);


extern ALIGN const uint32 TestTable1[256];
extern ALIGN const uint32 TestTable2[256];
extern ALIGN const uint32 TestTable3[256];
extern ALIGN const uint32 TestTable4[256];


extern ALIGN const uint32 RevTable1[256];
extern ALIGN const uint32 RevTable2[256];
extern ALIGN const uint32 RevTable3[256];
extern ALIGN const uint32 RevTable4[256];

//extern const uint32 RSBRawXTime_e[256];
//extern const uint32 RSBRawXTime_b[256];
//extern const uint32 RSBRawXTime_d[256];
//extern const uint32 RSBRawXTime_9[256];

extern ALIGN const uint32 RevRawTable1[256];
extern ALIGN const uint32 RevRawTable2[256];
extern ALIGN const uint32 RevRawTable3[256];
extern ALIGN const uint32 RevRawTable4[256];

extern ALIGN const uint32 FSB[256];
extern ALIGN const uint32 FSB_8[256];
extern ALIGN const uint32 FSB_16[256];
extern ALIGN const uint32 FSB_24[256];


extern ALIGN const uint32 RSb[256];
extern ALIGN const uint32 RSb_8[256];
extern ALIGN const uint32 RSb_16[256];
extern ALIGN const uint32 RSb_24[256];


extern void InvMixColumns(uint32 *pState);
extern void InvMixColumnsDecKey(uint32 *pInput, uint32 *pOutput);


#define XOR2(a, b, c)    c[0]  = a[0] ^ b[0]; c[1] = a[1] ^ b[1]; c[2] = a[2] ^ b[2]; c[3] = a[3] ^ b[3];
#define XOR(a, b)        a[0] ^= b[0];   a[1] ^= b[1]; a[2] ^= b[2]; a[3] ^= b[3];

void AddCounter(uint8 *pCnt);


extern int padding(void *pToPad, int iSize);

typedef struct _reg_ {
    uint8 al;
    uint8 ah;
    uint8 eal;
    uint8 eah;
} regs;
#endif //__AES_INTERNAL__
