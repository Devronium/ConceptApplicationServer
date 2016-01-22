//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

extern "C" {
#include <x265.h>
}

#define __WITH_DE265
#ifdef __WITH_DE265
 #include "libde265/en265.h"
 #include "libde265/configparam.h"
 #include "libde265/image.h"
 #include "libde265/encoder/analyze.h"
 #include "libde265/util.h"
 #include "libde265/quality.h"
#endif

#include <string.h>
#include <stdlib.h>

#define ENCODER_SET_CAST(name, type)                          \
    {                                                         \
        NUMBER res = 0;                                       \
        if (GetArrayKey(Invoke, PARAMETER(0), #name, &res)) { \
            pParam->name = (type)res;                         \
        }                                                     \
    }

int GetArrayKey(INVOKE_CALL Invoke, void *ARRAY, char *key, NUMBER *result) {
    if (Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, ARRAY, (INTEGER)-1, key)) {
        INTEGER type = 0;
        char    *str = 0;
        Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, ARRAY, key, &type, &str, result);
        if (type == VARIABLE_NUMBER) {
            return 1;
        }
    }
    return 0;
}

//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef __WITH_DE265
    de265_init();
#endif
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H265Encoder, 1)
    T_ARRAY(0)

    x265_param * pParam = x265_param_alloc();
    x265_param_default(pParam);

    pParam->bRepeatHeaders = 1;    //write sps,pps before keyframe
    pParam->internalCsp    = X265_CSP_I420;
    pParam->sourceWidth    = 1920; //width;
    pParam->sourceHeight   = 1080; //height;
    pParam->fpsNum         = 25;
    pParam->fpsDenom       = 1;
    pParam->logLevel       = 0;
    ENCODER_SET_CAST(bRepeatHeaders, int);
    ENCODER_SET_CAST(internalCsp, int);
    ENCODER_SET_CAST(sourceWidth, int);
    ENCODER_SET_CAST(sourceHeight, int);
    ENCODER_SET_CAST(fpsNum, int);
    ENCODER_SET_CAST(fpsDenom, int);
    ENCODER_SET_CAST(frameNumThreads, int);
    ENCODER_SET_CAST(bEnableWavefront, int);
    ENCODER_SET_CAST(bDistributeModeAnalysis, int);
    ENCODER_SET_CAST(bDistributeMotionEstimation, int);
    ENCODER_SET_CAST(bLogCuStats, int);
    ENCODER_SET_CAST(bEnablePsnr, int);
    ENCODER_SET_CAST(bEnableSsim, int);
    ENCODER_SET_CAST(logLevel, int);
    ENCODER_SET_CAST(internalBitDepth, int);
    ENCODER_SET_CAST(interlaceMode, int);
    ENCODER_SET_CAST(totalFrames, int);
    ENCODER_SET_CAST(levelIdc, int);
    ENCODER_SET_CAST(bHighTier, int);
    ENCODER_SET_CAST(maxNumReferences, int);
    ENCODER_SET_CAST(bAllowNonConformance, int);
    ENCODER_SET_CAST(bRepeatHeaders, int);
    ENCODER_SET_CAST(bAnnexB, int);
    ENCODER_SET_CAST(bEnableAccessUnitDelimiters, int);
    ENCODER_SET_CAST(bEmitHRDSEI, int);
    ENCODER_SET_CAST(bEmitInfoSEI, int);
    ENCODER_SET_CAST(decodedPictureHashSEI, int);
    ENCODER_SET_CAST(bEnableTemporalSubLayers, int);
    ENCODER_SET_CAST(bOpenGOP, int);
    ENCODER_SET_CAST(keyframeMin, int);
    ENCODER_SET_CAST(keyframeMax, int);
    ENCODER_SET_CAST(bframes, int);
    ENCODER_SET_CAST(bFrameAdaptive, int);
    ENCODER_SET_CAST(bBPyramid, int);
    ENCODER_SET_CAST(bFrameBias, int);
    ENCODER_SET_CAST(lookaheadDepth, int);
    ENCODER_SET_CAST(lookaheadSlices, int);
    ENCODER_SET_CAST(scenecutThreshold, int);
    ENCODER_SET_CAST(maxCUSize, int);
    ENCODER_SET_CAST(minCUSize, int);
    ENCODER_SET_CAST(bEnableRectInter, int);
    ENCODER_SET_CAST(bEnableAMP, int);
    ENCODER_SET_CAST(maxTUSize, uint32_t);
    ENCODER_SET_CAST(tuQTMaxInterDepth, uint32_t);
    ENCODER_SET_CAST(tuQTMaxIntraDepth, uint32_t);
    ENCODER_SET_CAST(maxNumMergeCand, uint32_t);
    ENCODER_SET_CAST(limitReferences, uint32_t);

    ENCODER_SET_CAST(rdoqLevel, int);
    ENCODER_SET_CAST(bEnableSignHiding, int);
    ENCODER_SET_CAST(bEnableTransformSkip, int);
    ENCODER_SET_CAST(noiseReductionIntra, int);
    ENCODER_SET_CAST(noiseReductionInter, int);
    ENCODER_SET_CAST(bEnableConstrainedIntra, int);
    ENCODER_SET_CAST(bEnableStrongIntraSmoothing, int);
    ENCODER_SET_CAST(searchMethod, int);
    ENCODER_SET_CAST(subpelRefine, int);
    ENCODER_SET_CAST(searchRange, int);
    ENCODER_SET_CAST(bEnableTemporalMvp, int);
    ENCODER_SET_CAST(bEnableWeightedPred, int);
    ENCODER_SET_CAST(bEnableWeightedBiPred, int);
    ENCODER_SET_CAST(bEnableLoopFilter, int);
    ENCODER_SET_CAST(deblockingFilterTCOffset, int);
    ENCODER_SET_CAST(deblockingFilterBetaOffset, int);
    ENCODER_SET_CAST(bEnableSAO, int);
    ENCODER_SET_CAST(bSaoNonDeblocked, int);
    ENCODER_SET_CAST(rdLevel, int);
    ENCODER_SET_CAST(bEnableEarlySkip, int);
    ENCODER_SET_CAST(bEnableFastIntra, int);
    ENCODER_SET_CAST(bEnableTSkipFast, int);
    ENCODER_SET_CAST(bCULossless, int);
    ENCODER_SET_CAST(bIntraInBFrames, int);
    ENCODER_SET_CAST(rdPenalty, int);

    ENCODER_SET_CAST(psyRd, double);
    ENCODER_SET_CAST(psyRdoq, double);

    ENCODER_SET_CAST(analysisMode, int)
    ENCODER_SET_CAST(bLossless, int)
    ENCODER_SET_CAST(cbQpOffset, int)
    ENCODER_SET_CAST(crQpOffset, int)

    ENCODER_SET_CAST(rc.rateControlMode, int)
    ENCODER_SET_CAST(rc.qp, int)
    ENCODER_SET_CAST(rc.bitrate, int)
    ENCODER_SET_CAST(rc.qCompress, double)
    ENCODER_SET_CAST(rc.ipFactor, double)
    ENCODER_SET_CAST(rc.pbFactor, double)
    ENCODER_SET_CAST(rc.rfConstant, double)
    ENCODER_SET_CAST(rc.qpStep, int)
    ENCODER_SET_CAST(rc.aqMode, int)
    ENCODER_SET_CAST(rc.aqStrength, int)
    ENCODER_SET_CAST(rc.vbvMaxBitrate, int)
    ENCODER_SET_CAST(rc.vbvBufferSize, int)
    ENCODER_SET_CAST(rc.vbvBufferInit, double)
    ENCODER_SET_CAST(rc.cuTree, int)
    ENCODER_SET_CAST(rc.rfConstantMax, double)
    ENCODER_SET_CAST(rc.rfConstantMin, double)
    ENCODER_SET_CAST(rc.bStatWrite, int)
    ENCODER_SET_CAST(rc.bStatRead, int)
    ENCODER_SET_CAST(rc.qblur, double)
    ENCODER_SET_CAST(rc.complexityBlur, double)
    ENCODER_SET_CAST(rc.bEnableSlowFirstPass, int)
    ENCODER_SET_CAST(rc.zoneCount, int)
    ENCODER_SET_CAST(rc.bStrictCbr, int)

    ENCODER_SET_CAST(vui.aspectRatioIdc, int)
    ENCODER_SET_CAST(vui.sarWidth, int)
    ENCODER_SET_CAST(vui.sarHeight, int)
    ENCODER_SET_CAST(vui.bEnableOverscanInfoPresentFlag, int)
    ENCODER_SET_CAST(vui.bEnableOverscanAppropriateFlag, int)
    ENCODER_SET_CAST(vui.bEnableVideoSignalTypePresentFlag, int)
    ENCODER_SET_CAST(vui.videoFormat, int)
    ENCODER_SET_CAST(vui.bEnableVideoFullRangeFlag, int)
    ENCODER_SET_CAST(vui.bEnableColorDescriptionPresentFlag, int)
    ENCODER_SET_CAST(vui.colorPrimaries, int)
    ENCODER_SET_CAST(vui.transferCharacteristics, int)
    ENCODER_SET_CAST(vui.matrixCoeffs, int)
    ENCODER_SET_CAST(vui.bEnableChromaLocInfoPresentFlag, int)
    ENCODER_SET_CAST(vui.chromaSampleLocTypeTopField, int)
    ENCODER_SET_CAST(vui.chromaSampleLocTypeBottomField, int)
    ENCODER_SET_CAST(vui.bEnableDefaultDisplayWindowFlag, int)
    ENCODER_SET_CAST(vui.defDispWinLeftOffset, int)
    ENCODER_SET_CAST(vui.defDispWinRightOffset, int)
    ENCODER_SET_CAST(vui.defDispWinTopOffset, int)
    ENCODER_SET_CAST(vui.defDispWinBottomOffset, int)

    x265_encoder * ectx = x265_encoder_open(pParam);
    x265_param_free(pParam);
    RETURN_NUMBER((SYS_INT)ectx);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H265EncoderDone, 1)
    T_HANDLE(0)
    x265_encoder * ectx = (x265_encoder *)(SYS_INT)PARAM(0);
    x265_encoder_close(ectx);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
#ifdef __WITH_DE265
CONCEPT_FUNCTION_IMPL(H265DecoderDone, 1)
    T_HANDLE(0)
    de265_decoder_context * dctx = (de265_decoder_context *)(SYS_INT)PARAM(0);
    de265_free_decoder(dctx);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H265Decoder, 1)
    T_ARRAY(0)
    de265_decoder_context * dctx = de265_new_decoder();
    de265_set_parameter_bool(dctx, DE265_DECODER_PARAM_SUPPRESS_FAULTY_PICTURES, false);
    RETURN_NUMBER((SYS_INT)dctx);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H265Decode, 3)
    T_HANDLE(0)
    T_STRING(1)

    de265_decoder_context * dctx = de265_new_decoder();
    int pos  = 0;
    int res  = de265_push_data(dctx, (uint8_t *)PARAM(1), PARAM_LEN(1), pos, NULL);
    int more = 1;
    int res2 = de265_decode(dctx, &more);

    if (de265_decode(dctx, &more) == DE265_OK) {
        const de265_image *img = de265_get_next_picture(dctx);
        if (img) {
            int  size = de265_get_image_width(img, 0) * de265_get_image_height(img, 0) + de265_get_image_width(img, 1) * de265_get_image_height(img, 1) + de265_get_image_width(img, 2) * de265_get_image_height(img, 2);
            char *buf = 0;
            CORE_NEW(size + 1, buf);
            if (buf) {
                buf[size] = 0;
                for (int c = 0; c < 3; c++) {
                    int           stride;
                    const uint8_t *p     = de265_get_image_plane(img, c, &stride);
                    int           width  = de265_get_image_width(img, c);
                    int           height = de265_get_image_height(img, c);

                    char *ptr = buf;
                    if (de265_get_bits_per_pixel(img, c) <= 8) {
                        // --- save 8 bit YUV ---
                        for (int y = 0; y < height; y++) {
                            memcpy(ptr, p + y * stride, width);
                            ptr += width;
                        }
                    }
                }
                SetVariable(PARAMETER(2), -1, (char *)buf, size);
            } else {
                SET_STRING(2, "");
            }
        } else {
            SET_STRING(2, "");
        }
    } else {
        SET_STRING(2, "");
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
#endif
CONCEPT_FUNCTION_IMPL(H265Encode, 5)
    T_HANDLE(0)
    T_STRING(1)
    T_NUMBER(2)
    T_NUMBER(3)
    x265_encoder * ectx = (x265_encoder *)(SYS_INT)PARAM(0);

    int      width  = PARAM_INT(2);
    int      height = PARAM_INT(3);
    int      res    = -2;
    x265_nal *pNals = NULL;
    uint32_t iNal   = 0;

    if ((width > 0) && (height > 0) && (PARAM_LEN(1) >= width * height * 3 / 2)) {
        x265_picture *pPic_in = x265_picture_alloc();
        x265_param   p;
        x265_encoder_parameters(ectx, &p);
        if ((p.sourceWidth != width) || (p.sourceHeight != height))
            return 0;

        x265_picture_init(&p, pPic_in);

        char *buff  = PARAM(1);
        int  y_size = width * height;
        pPic_in->planes[0] = buff;
        pPic_in->planes[1] = buff + y_size;
        pPic_in->planes[2] = buff + y_size * 5 / 4;
        pPic_in->stride[0] = width;
        pPic_in->stride[1] = width / 2;
        pPic_in->stride[2] = width / 2;

        res = x265_encoder_encode(ectx, &pNals, &iNal, pPic_in, NULL);
        x265_picture_free(pPic_in);
        int size = 0;
        for (int j = 0; j < iNal; j++)
            size += pNals[j].sizeBytes;

        if (size > 0) {
            unsigned char *buf = 0;
            CORE_NEW(size + 1, buf);
            if (buf) {
                buf[size] = 0;
                unsigned char *ptr = buf;
                for (int j = 0; j < iNal; j++) {
                    memcpy(ptr, pNals[j].payload, pNals[j].sizeBytes);
                    ptr += pNals[j].sizeBytes;
                }
                SetVariable(PARAMETER(4), -1, (char *)buf, size);
                //fwrite(pNals[j].payload,1,pNals[j].sizeBytes,fp_dst);
            }
        } else {
            SET_STRING(4, "");
        }
    } else {
        res = x265_encoder_encode(ectx, &pNals, &iNal, NULL, NULL);
        int size = 0;
        for (int j = 0; j < iNal; j++)
            size += pNals[j].sizeBytes;

        if (size > 0) {
            unsigned char *buf = 0;
            CORE_NEW(size + 1, buf);
            if (buf) {
                buf[size] = 0;
                unsigned char *ptr = buf;
                for (int j = 0; j < iNal; j++) {
                    memcpy(ptr, pNals[j].payload, pNals[j].sizeBytes);
                    ptr += pNals[j].sizeBytes;
                }
                SetVariable(PARAMETER(4), -1, (char *)buf, size);
                //fwrite(pNals[j].payload,1,pNals[j].sizeBytes,fp_dst);
            }
        } else {
            SET_STRING(4, "");
        }
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
