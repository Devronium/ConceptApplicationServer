/*************************************************************

                                EF AES Library 1.0

   Module Name: AES.h
   Abstract: Advanced Encryption Standard implementation header file
   Author: robert Guan

*************************************************************/

#ifndef __EF_AES__
#define __EF_AES__



typedef struct _AesCtx_ {
    unsigned char space[372];
} AesCtx;

#define AES_PADDING    (16)



typedef enum _AES_BLOCK_MODE_ {
    BLOCKMODE_ECB = 0,
    BLOCKMODE_CBC,
    BLOCKMODE_PCBC,
    BLOCKMODE_OFB,
    BLOCKMODE_CFB,
    BLOCKMODE_CRT
} BlockMode;


// set key and initial vector
void AesSetKey(AesCtx *pContext, int iBlockMode, void *key, void *initialVector);
void AesSetInitVector(AesCtx *pContext, void *initialVector);
void AesSetFeedbackSize(AesCtx *pContext, int iFeedbackSize);

// an util function
int AesRoundSize(int iSize, int iRoundSize);

// encrypt APIs
void AesEncryptECB(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesEncryptCBC(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesEncryptPCBC(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesEncryptCRT(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesEncryptOFB(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesEncryptCFB(AesCtx *pContext, void *pOutput, void *pInput, int iSize);


// decrypt APIs
void AesDecryptECB(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesDecryptCBC(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesDecryptPCBC(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesDecryptCRT(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesDecryptOFB(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
void AesDecryptCFB(AesCtx *pContext, void *pOutput, void *pInput, int iSize);
#endif //__EF_AES__
