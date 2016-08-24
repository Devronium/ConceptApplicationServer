//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#ifdef _WIN32
 #include <codec_api.h>
#else
 #include <wels/codec_api.h>
#endif
#include <string.h>
#include <stdlib.h>

#define ENCODER_SET(name)                               \
    res = 0;                                            \
    if (GetArrayKey(Invoke, PARAMETER(0), #name, &res)) \
        param.name = res;

#define ENCODER_SET_CAST(name, type)                      \
    res = 0;                                              \
    if (GetArrayKey(Invoke, PARAMETER(0), #name, &res)) { \
        param.name = (type)res;                           \
    }
//-----------------------------------------------------//
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

typedef struct {
    SParserBsInfo sDstParseInfo;
    SBufferInfo   sDstBufInfo;
    unsigned char *pData[4];
    ISVCDecoder   *decoder;
    int           bits_per_pixel;
} DecoderContext;
//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(CAMERA_VIDEO_REAL_TIME);
    DEFINE_ECONSTANT(SCREEN_CONTENT_REAL_TIME);
    DEFINE_ECONSTANT(CAMERA_VIDEO_NON_REAL_TIME);
    DEFINE_ECONSTANT(LOW_COMPLEXITY)
    DEFINE_ECONSTANT(MEDIUM_COMPLEXITY)
    DEFINE_ECONSTANT(HIGH_COMPLEXITY)
    DEFINE_ECONSTANT(CONSTANT_ID)
    DEFINE_ECONSTANT(INCREASING_ID)
    DEFINE_ECONSTANT(SPS_LISTING)
    DEFINE_ECONSTANT(SPS_LISTING_AND_PPS_INCREASING)
    DEFINE_ECONSTANT(SPS_PPS_LISTING)
    DEFINE_ECONSTANT(LEVEL_UNKNOWN)
    DEFINE_ECONSTANT(LEVEL_1_0)
    DEFINE_ECONSTANT(LEVEL_1_B)
    DEFINE_ECONSTANT(LEVEL_1_1)
    DEFINE_ECONSTANT(LEVEL_1_2)
    DEFINE_ECONSTANT(LEVEL_1_3)
    DEFINE_ECONSTANT(LEVEL_2_0)
    DEFINE_ECONSTANT(LEVEL_2_1)
    DEFINE_ECONSTANT(LEVEL_2_2)
    DEFINE_ECONSTANT(LEVEL_3_0)
    DEFINE_ECONSTANT(LEVEL_3_1)
    DEFINE_ECONSTANT(LEVEL_3_2)
    DEFINE_ECONSTANT(LEVEL_4_0)
    DEFINE_ECONSTANT(LEVEL_4_1)
    DEFINE_ECONSTANT(LEVEL_4_2)
    DEFINE_ECONSTANT(LEVEL_5_0)
    DEFINE_ECONSTANT(LEVEL_5_1)
    DEFINE_ECONSTANT(LEVEL_5_2)
    DEFINE_ECONSTANT(WELS_LOG_QUIET)
    DEFINE_ECONSTANT(WELS_LOG_ERROR)
    DEFINE_ECONSTANT(WELS_LOG_WARNING)
    DEFINE_ECONSTANT(WELS_LOG_INFO)
    DEFINE_ECONSTANT(WELS_LOG_DEBUG)
    DEFINE_ECONSTANT(WELS_LOG_DETAIL)
    DEFINE_ECONSTANT(WELS_LOG_RESV)
    DEFINE_ECONSTANT(WELS_LOG_LEVEL_COUNT)
    DEFINE_ECONSTANT(WELS_LOG_DEFAULT)
    DEFINE_ECONSTANT(dsErrorFree)
    DEFINE_ECONSTANT(dsFramePending)
    DEFINE_ECONSTANT(dsRefLost)
    DEFINE_ECONSTANT(dsBitstreamError)
    DEFINE_ECONSTANT(dsDepLayerLost)
    DEFINE_ECONSTANT(dsNoParamSets)
    DEFINE_ECONSTANT(dsDataErrorConcealed)
    DEFINE_ECONSTANT(dsInvalidArgument)
    DEFINE_ECONSTANT(dsInitialOptExpected)
    DEFINE_ECONSTANT(dsOutOfMemory)
    DEFINE_ECONSTANT(dsDstBufNeedExpan)

    DEFINE_ECONSTANT(ENCODER_OPTION_DATAFORMAT)
    DEFINE_ECONSTANT(ENCODER_OPTION_IDR_INTERVAL)
    DEFINE_ECONSTANT(ENCODER_OPTION_SVC_ENCODE_PARAM_BASE)
    DEFINE_ECONSTANT(ENCODER_OPTION_SVC_ENCODE_PARAM_EXT)
    DEFINE_ECONSTANT(ENCODER_OPTION_FRAME_RATE)
    DEFINE_ECONSTANT(ENCODER_OPTION_BITRATE)
    DEFINE_ECONSTANT(ENCODER_OPTION_MAX_BITRATE)
    DEFINE_ECONSTANT(ENCODER_OPTION_INTER_SPATIAL_PRED)
    DEFINE_ECONSTANT(ENCODER_OPTION_RC_MODE)
    DEFINE_ECONSTANT(ENCODER_OPTION_RC_FRAME_SKIP)
    DEFINE_ECONSTANT(ENCODER_PADDING_PADDING)
    DEFINE_ECONSTANT(ENCODER_OPTION_PROFILE)
    DEFINE_ECONSTANT(ENCODER_OPTION_LEVEL)
    DEFINE_ECONSTANT(ENCODER_OPTION_NUMBER_REF)
    DEFINE_ECONSTANT(ENCODER_OPTION_DELIVERY_STATUS)
    DEFINE_ECONSTANT(ENCODER_LTR_RECOVERY_REQUEST)
    DEFINE_ECONSTANT(ENCODER_LTR_MARKING_FEEDBACK)
    DEFINE_ECONSTANT(ENCODER_LTR_MARKING_PERIOD)
    DEFINE_ECONSTANT(ENCODER_OPTION_LTR)
    DEFINE_ECONSTANT(ENCODER_OPTION_COMPLEXITY)
    DEFINE_ECONSTANT(ENCODER_OPTION_ENABLE_SSEI)
    DEFINE_ECONSTANT(ENCODER_OPTION_ENABLE_PREFIX_NAL_ADDING)
    DEFINE_ECONSTANT(ENCODER_OPTION_ENABLE_SPS_PPS_ID_ADDITION)
    DEFINE_ECONSTANT(ENCODER_OPTION_CURRENT_PATH)
    DEFINE_ECONSTANT(ENCODER_OPTION_DUMP_FILE)
    DEFINE_ECONSTANT(ENCODER_OPTION_TRACE_LEVEL)
    DEFINE_ECONSTANT(ENCODER_OPTION_TRACE_CALLBACK)
    DEFINE_ECONSTANT(ENCODER_OPTION_TRACE_CALLBACK_CONTEXT)
    DEFINE_ECONSTANT(ENCODER_OPTION_GET_STATISTICS)
    DEFINE_ECONSTANT(ENCODER_OPTION_STATISTICS_LOG_INTERVAL)
    DEFINE_ECONSTANT(ENCODER_OPTION_IS_LOSSLESS_LINK)
    DEFINE_ECONSTANT(ENCODER_OPTION_BITS_VARY_PERCENTAGE)

    DEFINE_ECONSTANT(DECODER_OPTION_DATAFORMAT)
    DEFINE_ECONSTANT(DECODER_OPTION_END_OF_STREAM)
    DEFINE_ECONSTANT(DECODER_OPTION_VCL_NAL)
    DEFINE_ECONSTANT(DECODER_OPTION_TEMPORAL_ID)
    DEFINE_ECONSTANT(DECODER_OPTION_FRAME_NUM)
    DEFINE_ECONSTANT(DECODER_OPTION_IDR_PIC_ID)
    DEFINE_ECONSTANT(DECODER_OPTION_LTR_MARKING_FLAG)
    DEFINE_ECONSTANT(DECODER_OPTION_LTR_MARKED_FRAME_NUM)
    DEFINE_ECONSTANT(DECODER_OPTION_ERROR_CON_IDC)
    DEFINE_ECONSTANT(DECODER_OPTION_TRACE_LEVEL)
    DEFINE_ECONSTANT(DECODER_OPTION_TRACE_CALLBACK)
    DEFINE_ECONSTANT(DECODER_OPTION_TRACE_CALLBACK_CONTEXT)
    DEFINE_ECONSTANT(DECODER_OPTION_GET_STATISTICS)

    DEFINE_ECONSTANT(ERROR_CON_DISABLE)
    DEFINE_ECONSTANT(ERROR_CON_FRAME_COPY)
    DEFINE_ECONSTANT(ERROR_CON_SLICE_COPY)
    DEFINE_ECONSTANT(ERROR_CON_FRAME_COPY_CROSS_IDR)
    DEFINE_ECONSTANT(ERROR_CON_SLICE_COPY_CROSS_IDR)
    DEFINE_ECONSTANT(ERROR_CON_SLICE_COPY_CROSS_IDR_FREEZE_RES_CHANGE)
    DEFINE_ECONSTANT(ERROR_CON_SLICE_MV_COPY_CROSS_IDR)
    DEFINE_ECONSTANT(ERROR_CON_SLICE_MV_COPY_CROSS_IDR_FREEZE_RES_CHANGE)

    DEFINE_ECONSTANT(FEEDBACK_NON_VCL_NAL)
    DEFINE_ECONSTANT(FEEDBACK_VCL_NAL)
    DEFINE_ECONSTANT(FEEDBACK_UNKNOWN_NAL)

    DEFINE_ECONSTANT(NON_VIDEO_CODING_LAYER)
    DEFINE_ECONSTANT(VIDEO_CODING_LAYER)

    DEFINE_ECONSTANT(VIDEO_BITSTREAM_AVC)
    DEFINE_ECONSTANT(VIDEO_BITSTREAM_SVC)
    DEFINE_ECONSTANT(VIDEO_BITSTREAM_DEFAULT)
    DEFINE_ECONSTANT(NO_RECOVERY_REQUSET)
    DEFINE_ECONSTANT(LTR_RECOVERY_REQUEST)
    DEFINE_ECONSTANT(IDR_RECOVERY_REQUEST)
    DEFINE_ECONSTANT(NO_LTR_MARKING_FEEDBACK)
    DEFINE_ECONSTANT(LTR_MARKING_SUCCESS)
    DEFINE_ECONSTANT(LTR_MARKING_FAILED)

    DEFINE_ECONSTANT(SM_SINGLE_SLICE)
    DEFINE_ECONSTANT(SM_FIXEDSLCNUM_SLICE)
    DEFINE_ECONSTANT(SM_RASTER_SLICE)
    DEFINE_ECONSTANT(SM_ROWMB_SLICE)
    DEFINE_ECONSTANT(SM_DYN_SLICE)
    DEFINE_ECONSTANT(SM_AUTO_SLICE)
    DEFINE_ECONSTANT(SM_RESERVED)

    DEFINE_ECONSTANT(RC_QUALITY_MODE)
    DEFINE_ECONSTANT(RC_BITRATE_MODE)
    DEFINE_ECONSTANT(RC_BUFFERBASED_MODE)
    DEFINE_ECONSTANT(RC_TIMESTAMP_MODE)
    DEFINE_ECONSTANT(RC_BITRATE_MODE_POST_SKIP)
    DEFINE_ECONSTANT(RC_OFF_MODE)

    DEFINE_ECONSTANT(PRO_UNKNOWN)
    DEFINE_ECONSTANT(PRO_BASELINE)
    DEFINE_ECONSTANT(PRO_MAIN)
    DEFINE_ECONSTANT(PRO_EXTENDED)
    DEFINE_ECONSTANT(PRO_HIGH)
    DEFINE_ECONSTANT(PRO_HIGH10)
    DEFINE_ECONSTANT(PRO_HIGH422)
    DEFINE_ECONSTANT(PRO_HIGH444)
    DEFINE_ECONSTANT(PRO_CAVLC444)

    DEFINE_ECONSTANT(PRO_SCALABLE_BASELINE)
    DEFINE_ECONSTANT(PRO_SCALABLE_HIGH)

    DEFINE_ECONSTANT(cmResultSuccess)
    DEFINE_ECONSTANT(cmInitParaError)
    DEFINE_ECONSTANT(cmUnkonwReason)
    DEFINE_ECONSTANT(cmMallocMemeError)
    DEFINE_ECONSTANT(cmInitExpected)
    DEFINE_ECONSTANT(cmUnsupportedData)

    DEFINE_ECONSTANT(NAL_UNKNOWN)
    DEFINE_ECONSTANT(NAL_SLICE)
    DEFINE_ECONSTANT(NAL_SLICE_DPA)
    DEFINE_ECONSTANT(NAL_SLICE_DPB)
    DEFINE_ECONSTANT(NAL_SLICE_DPC)
    DEFINE_ECONSTANT(NAL_SLICE_IDR)
    DEFINE_ECONSTANT(NAL_SEI)
    DEFINE_ECONSTANT(NAL_SPS)
    DEFINE_ECONSTANT(NAL_PPS)

    DEFINE_ECONSTANT(NAL_PRIORITY_DISPOSABLE)
    DEFINE_ECONSTANT(NAL_PRIORITY_LOW)
    DEFINE_ECONSTANT(NAL_PRIORITY_HIGH)
    DEFINE_ECONSTANT(NAL_PRIORITY_HIGHEST)

    DEFINE_ECONSTANT(DEBLOCKING_IDC_0)
    DEFINE_ECONSTANT(DEBLOCKING_IDC_1)
    DEFINE_ECONSTANT(DEBLOCKING_IDC_2)

    DEFINE_ECONSTANT(ET_NONE)
    DEFINE_ECONSTANT(ET_IP_SCALE)
    DEFINE_ECONSTANT(ET_FMO)
    DEFINE_ECONSTANT(ET_IR_R1)
    DEFINE_ECONSTANT(ET_IR_R2)
    DEFINE_ECONSTANT(ET_IR_R3)
    DEFINE_ECONSTANT(ET_FEC_HALF)
    DEFINE_ECONSTANT(ET_FEC_FULL)
    DEFINE_ECONSTANT(ET_RFS)

    DEFINE_ECONSTANT(videoFormatRGB)
    DEFINE_ECONSTANT(videoFormatRGBA)
    DEFINE_ECONSTANT(videoFormatRGB555)
    DEFINE_ECONSTANT(videoFormatRGB565)
    DEFINE_ECONSTANT(videoFormatBGR)
    DEFINE_ECONSTANT(videoFormatBGRA)
    DEFINE_ECONSTANT(videoFormatABGR)
    DEFINE_ECONSTANT(videoFormatARGB)
    DEFINE_ECONSTANT(videoFormatYUY2)
    DEFINE_ECONSTANT(videoFormatYVYU)
    DEFINE_ECONSTANT(videoFormatUYVY)
    DEFINE_ECONSTANT(videoFormatI420)
    DEFINE_ECONSTANT(videoFormatYV12)
    DEFINE_ECONSTANT(videoFormatInternal)
    DEFINE_ECONSTANT(videoFormatNV12)
    DEFINE_ECONSTANT(videoFormatVFlip)

    DEFINE_ECONSTANT(videoFrameTypeInvalid)
    DEFINE_ECONSTANT(videoFrameTypeIDR)
    DEFINE_ECONSTANT(videoFrameTypeI)
    DEFINE_ECONSTANT(videoFrameTypeP)
    DEFINE_ECONSTANT(videoFrameTypeSkip)
    DEFINE_ECONSTANT(videoFrameTypeIPMixed)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H264Encoder, 1)
    T_ARRAY(H264Encoder, 0)
    ISVCEncoder * encoder = NULL;
    WelsCreateSVCEncoder(&encoder);

//SEncParamBase param;
//memset(&param, 0, sizeof (SEncParamBase));

/*ENCODER_SET_CAST(iUsageType, EUsageType);
   ENCODER_SET_CAST(iPicWidth, int);
   ENCODER_SET_CAST(iPicHeight, int);
   ENCODER_SET_CAST(iTargetBitrate, int);
   ENCODER_SET_CAST(iRCMode, RC_MODES);
   ENCODER_SET_CAST(fMaxFrameRate, int);*/
    SEncParamExt param;
    encoder->GetDefaultParams(&param);

    NUMBER res = 0;
    ENCODER_SET_CAST(iUsageType, EUsageType);
    ENCODER_SET(iPicWidth);
    ENCODER_SET(iPicHeight);
    ENCODER_SET(iTargetBitrate);
    ENCODER_SET_CAST(iRCMode, RC_MODES);
    ENCODER_SET(fMaxFrameRate);
    ENCODER_SET(iTemporalLayerNum);
    ENCODER_SET(iSpatialLayerNum);
    ENCODER_SET_CAST(iComplexityMode, ECOMPLEXITY_MODE);
    ENCODER_SET(uiIntraPeriod);
    ENCODER_SET(iNumRefFrame);
    ENCODER_SET_CAST(eSpsPpsIdStrategy, EParameterSetStrategy);
    ENCODER_SET(bPrefixNalAddingCtrl);
    ENCODER_SET(bEnableSSEI);
    ENCODER_SET(bSimulcastAVC);
    ENCODER_SET(iPaddingFlag);
    ENCODER_SET(iEntropyCodingModeFlag);
    ENCODER_SET(bEnableFrameSkip);
    ENCODER_SET(iMaxBitrate);
    ENCODER_SET(iMaxQp);
    ENCODER_SET(iMinQp);
    ENCODER_SET(uiMaxNalSize);
    ENCODER_SET(bEnableLongTermReference);
    ENCODER_SET(iLTRRefNum);
    ENCODER_SET(iLtrMarkPeriod);
    ENCODER_SET(iMultipleThreadIdc);
    ENCODER_SET(iLoopFilterDisableIdc);
    ENCODER_SET(iLoopFilterAlphaC0Offset);
    ENCODER_SET(iLoopFilterBetaOffset);
    ENCODER_SET(bEnableDenoise);
    ENCODER_SET(bEnableBackgroundDetection);
    ENCODER_SET(bEnableAdaptiveQuant);
    ENCODER_SET(bEnableFrameCroppingFlag);
    ENCODER_SET(bEnableSceneChangeDetect);
    ENCODER_SET(bIsLosslessLink);
    if (param.iSpatialLayerNum <= 0)
        param.iSpatialLayerNum = 1;
//if (param.iTargetBitrate <= 0)
//    param.iTargetBitrate = 18000000;

    for (int i = 0; i < param.iSpatialLayerNum; i++) {
        param.sSpatialLayers[i].iVideoWidth           = param.iPicWidth >> (param.iSpatialLayerNum - i - 1);
        param.sSpatialLayers[i].iVideoHeight          = param.iPicHeight >> (param.iSpatialLayerNum - i - 1);
        param.sSpatialLayers[i].fFrameRate            = param.fMaxFrameRate;
        param.sSpatialLayers[i].sSliceCfg.uiSliceMode = SM_FIXEDSLCNUM_SLICE;
        param.sSpatialLayers[i].sSliceCfg.sSliceArgument.uiSliceNum = 1;
        param.sSpatialLayers[i].iSpatialBitrate = param.iTargetBitrate;
        //param.sSpatialLayers[i].iMaxSpatialBitrate = param.iTargetBitrate;
    }

    encoder->InitializeExt(&param);
//if (param.iTargetBitrate <= 0)
//    param.iTargetBitrate = 10000000;
//encoder->Initialize(&param);
//int on = VIDEO_BITSTREAM_AVC;
//encoder->SetOption(ENCODER_OPTION_DATAFORMAT, &on);
    RETURN_NUMBER((SYS_INT)encoder);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H264EncoderDone, 1)
    T_HANDLE(H264EncoderDone, 0)
    ISVCEncoder * encoder = (ISVCEncoder *)(SYS_INT)PARAM(0);
    encoder->Uninitialize();
    WelsDestroySVCEncoder(encoder);
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H264EncoderSet, 3)
    T_HANDLE(H264EncoderSet, 0)
    T_NUMBER(H264EncoderSet, 1)
    T_NUMBER(H264EncoderSet, 2)

    ISVCEncoder * encoder = (ISVCEncoder *)(SYS_INT)PARAM(0);

    int val = PARAM_INT(2);
    encoder->SetOption((ENCODER_OPTION)PARAM_INT(1), &val);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H264EncoderGet, 3)
    T_HANDLE(H264EncoderGet, 0)
    T_NUMBER(H264EncoderGet, 1)
    T_NUMBER(H264EncoderGet, 2)

    ISVCEncoder * encoder = (ISVCEncoder *)(SYS_INT)PARAM(0);

    int val = PARAM_INT(2);
    encoder->GetOption((ENCODER_OPTION)PARAM_INT(1), &val);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(H264Encode, 5, 8)
    T_HANDLE(H264Encode, 0)
    T_STRING(H264Encode, 1)
    T_NUMBER(H264Encode, 2)
    T_NUMBER(H264Encode, 3)

    ISVCEncoder * encoder = (ISVCEncoder *)(SYS_INT)PARAM(0);
    int width  = PARAM_INT(2);
    int height = PARAM_INT(3);
    EVideoFormatType format    = videoFormatI420;
    long long        timestamp = 0;
    bool             idr_frame = false;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(H264Encode, 5)
        format = (EVideoFormatType)PARAM_INT(5);
        if (PARAMETERS_COUNT > 6) {
            T_NUMBER(H264Encode, 6)
            timestamp = (long long)PARAM(6);
        }
        if (PARAMETERS_COUNT > 7) {
            T_NUMBER(H264Encode, 7)
            if (PARAM_INT(7))
                idr_frame = true;
        }
    }
//encoder->ForceIntraFrame(idr_frame);
//CREATE_ARRAY(PARAMETER(4));
    SFrameBSInfo info;
    SFrameBSInfo info_sps;
    memset(&info, 0, sizeof(SFrameBSInfo));
    SSourcePicture pic;
    memset(&pic, 0, sizeof(SSourcePicture));
    pic.iPicWidth    = width;
    pic.iPicHeight   = height;
    pic.iColorFormat = format;
    pic.uiTimeStamp  = timestamp;
    pic.iStride[0]   = pic.iPicWidth;
    pic.iStride[1]   = pic.iStride[2] = pic.iPicWidth >> 1;
    pic.pData[0]     = (unsigned char *)PARAM(1);
    pic.pData[1]     = pic.pData[0] + width * height;
    pic.pData[2]     = pic.pData[1] + (width * height >> 2);
    int rv = encoder->EncodeFrame(&pic, &info);
    if (rv != cmResultSuccess) {
        SET_STRING(4, "");
        RETURN_NUMBER(rv);
        return 0;
    }
    int total_size = 0;
    if (info.eFrameType != videoFrameTypeSkip) {
        for (int i = 0; i < info.iLayerNum; i++) {
            SLayerBSInfo *layer = &info.sLayerInfo[i];
            if (layer->pBsBuf)
                total_size += *layer->pNalLengthInByte;
        }
    }
    if (total_size > 0) {
        unsigned char *buf = 0;
        CORE_NEW(total_size + 1, buf);
        if (buf) {
            buf[total_size] = 0;
            unsigned char *ptr = buf;
            if (info.eFrameType != videoFrameTypeSkip) {
                for (int i = 0; i < info.iLayerNum; i++) {
                    SLayerBSInfo *layer = &info.sLayerInfo[i];
                    if (layer->pBsBuf) {
                        memcpy(ptr, layer->pBsBuf, *layer->pNalLengthInByte);
                        ptr += *layer->pNalLengthInByte;
                    }
                }
            }
            SetVariable(PARAMETER(4), -1, (char *)buf, total_size);
        } else {
            SET_STRING(4, "");
        }
    }
    RETURN_NUMBER(cmResultSuccess)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H264Decoder, 1)
    T_ARRAY(H264Decoder, 0)

    DecoderContext * dec = (DecoderContext *)malloc(sizeof(DecoderContext));
    memset(dec, 0, sizeof(dec));
//ISVCDecoder *decoder = NULL;
    WelsCreateDecoder(&dec->decoder);

    SDecodingParam param = { 0 };


    NUMBER res = 0;
    ENCODER_SET_CAST(eOutputColorFormat, EVideoFormatType);
    ENCODER_SET_CAST(uiCpuLoad, unsigned int);
    ENCODER_SET_CAST(uiTargetDqLayer, unsigned char);
    ENCODER_SET_CAST(eEcActiveIdc, ERROR_CON_IDC);
    ENCODER_SET_CAST(bParseOnly, bool);
    ENCODER_SET_CAST(sVideoProperty.size, unsigned int);
    ENCODER_SET_CAST(sVideoProperty.eVideoBsType, VIDEO_BITSTREAM_TYPE);
    if (!param.eOutputColorFormat)
        param.eOutputColorFormat = videoFormatI420;

    switch (param.eOutputColorFormat) {
        case videoFormatI420:
        case videoFormatYV12:
        case videoFormatNV12:
            dec->bits_per_pixel = 12;
            break;

        case videoFormatUYVY:
        case videoFormatYVYU:
        case videoFormatYUY2:
            dec->bits_per_pixel = 16;
            break;

        case videoFormatRGB:
        case videoFormatBGR:
            dec->bits_per_pixel = 24;
            break;

        case videoFormatRGBA:
        case videoFormatBGRA:
        case videoFormatABGR:
        case videoFormatARGB:
            dec->bits_per_pixel = 32;
            break;

        case videoFormatRGB555:
        case videoFormatRGB565:
            dec->bits_per_pixel = 16;
            break;
    }
    dec->decoder->Initialize(&param);

    RETURN_NUMBER((SYS_INT)dec);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H264DecoderDone, 1)
    T_HANDLE(H264DecoderDone, 0)
    DecoderContext * decoder = (DecoderContext *)(SYS_INT)PARAM(0);
    decoder->decoder->Uninitialize();
    WelsDestroyDecoder(decoder->decoder);
    if (decoder->sDstParseInfo.pDstBuff)
        free(decoder->sDstParseInfo.pDstBuff);
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H264DecoderSet, 3)
    T_HANDLE(H264DecoderSet, 0)
    T_NUMBER(H264DecoderSet, 1)
    T_NUMBER(H264DecoderSet, 2)

    DecoderContext * decoder = (DecoderContext *)(SYS_INT)PARAM(0);

    int val = PARAM_INT(2);
    decoder->decoder->SetOption((DECODER_OPTION)PARAM_INT(1), &val);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(H264DecoderGet, 3)
    T_HANDLE(H264DecoderGet, 0)
    T_NUMBER(H264DecoderGet, 1)
    T_NUMBER(H264DecoderGet, 2)

    DecoderContext * decoder = (DecoderContext *)(SYS_INT)PARAM(0);

    int val = PARAM_INT(2);
    decoder->decoder->GetOption((DECODER_OPTION)PARAM_INT(1), &val);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
static int write_yuv(unsigned char *buf,
                     unsigned      dst_len,
                     unsigned char *pData[3],
                     int           iStride[2],
                     int           iWidth,
                     int           iHeight) {
    unsigned      req_size;
    unsigned char *dst = buf;
    unsigned char *max = dst + dst_len;
    int           i;
    unsigned char *pPtr = NULL;

    req_size = (iWidth * iHeight) + (iWidth / 2 * iHeight / 2) + (iWidth / 2 * iHeight / 2);
    if (dst_len < req_size)
        return -1;

    pPtr = pData[0];
    for (i = 0; i < iHeight && (dst + iWidth < max); i++) {
        memcpy(dst, pPtr, iWidth);
        pPtr += iStride[0];
        dst  += iWidth;
    }

    if (i < iHeight)
        return -1;

    iHeight = iHeight / 2;
    iWidth  = iWidth / 2;
    pPtr    = pData[1];
    for (i = 0; i < iHeight && (dst + iWidth <= max); i++) {
        memcpy(dst, pPtr, iWidth);
        pPtr += iStride[1];
        dst  += iWidth;
    }

    if (i < iHeight)
        return -1;

    pPtr = pData[2];
    for (i = 0; i < iHeight && (dst + iWidth <= max); i++) {
        memcpy(dst, pPtr, iWidth);
        pPtr += iStride[1];
        dst  += iWidth;
    }

    if (i < iHeight)
        return -1;

    return dst - buf;
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(H264Decode, 3, 7)
    T_HANDLE(H264Decode, 0)
    T_STRING(H264Decode, 1)

    DecoderContext * decoder = (DecoderContext *)(SYS_INT)PARAM(0);

    if (!decoder->bits_per_pixel) {
        SET_STRING(2, "");
        RETURN_NUMBER(-3);
        return 0;
    }
//if (!decoder->sDstParseInfo.pDstBuff)
//decoder->sDstParseInfo.pDstBuff = (unsigned char *)malloc(110401);

//int rv = decoder->decoder->DecodeFrameNoDelay((unsigned char *)PARAM(1), PARAM_LEN(1), decoder->pData, &decoder->sDstBufInfo);
    char *buf    = PARAM(1);
    int  buf_len = PARAM_LEN(1);

    int last_nal        = -1;
    int len             = buf_len - 3;
    int rv              = -1;
    int multiple_frames = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(H264Decode, 3)
        multiple_frames = PARAM_INT(3);
    }
    INTEGER frame_idx = 0;
    if (multiple_frames) {
        CREATE_ARRAY(PARAMETER(2));
    } else {
        SET_STRING(2, "");
    }
    for (int i = 0; i < len; i++) {
        if ((buf[i] == 0) && (buf[i + 1] == 0) && (buf[i + 2] == 1)) {
            if (last_nal >= 0) {
                unsigned char *pData[4] = { NULL, NULL, NULL, NULL };
                rv = decoder->decoder->DecodeFrameNoDelay((unsigned char *)buf + last_nal, i - last_nal, decoder->pData, &decoder->sDstBufInfo);
                if ((rv == cmResultSuccess) && (decoder->sDstBufInfo.iBufferStatus == 1) && (multiple_frames)) {
                    int width  = decoder->sDstBufInfo.UsrData.sSystemBuffer.iWidth;
                    int height = decoder->sDstBufInfo.UsrData.sSystemBuffer.iHeight;
                    int format = decoder->sDstBufInfo.UsrData.sSystemBuffer.iFormat;
                    if (PARAMETERS_COUNT > 4) {
                        SET_NUMBER(4, width);
                    }
                    if (PARAMETERS_COUNT > 5) {
                        SET_NUMBER(5, height);
                    }
                    if (PARAMETERS_COUNT > 6) {
                        SET_NUMBER(6, format);
                    }
                    if (decoder->pData[0] && decoder->pData[1] && decoder->pData[2] && (format == videoFormatI420)) {
                        char *buf_out = NULL;
                        int  size     = width * height * decoder->bits_per_pixel / 8;
                        CORE_NEW(size + 1, buf_out);
                        if (!buf_out) {
                            // out of memory
                            RETURN_NUMBER(-2);
                            return 0;
                        }
                        buf_out[size] = 0;
                        int res = write_yuv((unsigned char *)buf_out, size, decoder->pData, decoder->sDstBufInfo.UsrData.sSystemBuffer.iStride, width, height);
                        if (res <= 0) {
                            CORE_DELETE(buf_out);
                        } else {
                            Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(2), (INTEGER)frame_idx++, (INTEGER)VARIABLE_STRING, buf_out, (double)size);
                            multiple_frames--;
                            if (!multiple_frames) {
                                RETURN_NUMBER(0);
                                return 0;
                            }
                        }
                    }
                }
            }
            last_nal = i;
        }
    }
    if ((last_nal >= 0) && (last_nal < len)) {
        decoder->pData[0] = 0;
        decoder->pData[1] = 0;
        decoder->pData[2] = 0;
        rv = decoder->decoder->DecodeFrameNoDelay((unsigned char *)buf + last_nal, buf_len - last_nal, decoder->pData, &decoder->sDstBufInfo);
        if ((rv == cmResultSuccess) && (decoder->sDstBufInfo.iBufferStatus == 1)) {
            int width  = decoder->sDstBufInfo.UsrData.sSystemBuffer.iWidth;
            int height = decoder->sDstBufInfo.UsrData.sSystemBuffer.iHeight;
            int format = decoder->sDstBufInfo.UsrData.sSystemBuffer.iFormat;
            if (PARAMETERS_COUNT > 4) {
                SET_NUMBER(4, width);
            }
            if (PARAMETERS_COUNT > 5) {
                SET_NUMBER(5, height);
            }
            if (PARAMETERS_COUNT > 6) {
                SET_NUMBER(6, format);
            }
            if (decoder->pData[0] && decoder->pData[1] && decoder->pData[2] && (format == videoFormatI420)) {
                char *buf_out = NULL;
                int  size     = width * height * decoder->bits_per_pixel / 8;
                CORE_NEW(size + 1, buf_out);
                if (!buf_out) {
                    // out of memory
                    RETURN_NUMBER(-2);
                    return 0;
                }
                buf_out[size] = 0;
                int res = write_yuv((unsigned char *)buf_out, size, decoder->pData, decoder->sDstBufInfo.UsrData.sSystemBuffer.iStride, width, height);
                if (res <= 0) {
                    CORE_DELETE(buf_out);
                    //SET_STRING(2, "");
                } else {
                    if (multiple_frames)
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(2), (INTEGER)frame_idx++, (INTEGER)VARIABLE_STRING, buf_out, (double)size);
                    else
                        SetVariable(PARAMETER(2), -1, (char *)buf_out, size);
                }
                //SET_BUFFER(2, (char *)decoder->sDstParseInfo.pDstBuff, size);
            }
        } else {
            SET_STRING(2, "");
        }
    } else {
        SET_STRING(2, "");
    }
    RETURN_NUMBER(rv)
END_IMPL
//------------------------------------------------------------------------
void memcpy_except_value(unsigned char *dest, unsigned char *src, char *dest_u, char *dest_v, char *src_u, char *src_v, int size, unsigned char except_value) {
    unsigned char Y = 0;
    char          U = 0;
    char          V = 0;

    for (int i = 0; i < size / 2; i++) {
        Y = src[i * 2];
        U = (char)src_u[i];
        V = (char)src_v[i];

        int B = abs(1.164 * (Y - 16) + 2.018 * (U - 128));
        int G = abs(1.164 * (Y - 16) - 0.813 * (V - 128) - 0.391 * (U - 128));
        int R = abs(1.164 * (Y - 16) + 1.596 * (V - 128));

        /*int R = current_y + 1.140 * current_v;
           int G = current_y - 0.395 * current_u - 0.581 * current_v;
           int B = current_y + 2.032 * current_u;*/
        //fprintf(stderr, "Y'UV: %i,%i,%i\n", (int)current_y, (int)current_u, (int)current_v);
        //fprintf(stderr, "RGB: %i,%i,%i\n", (int)R, (int)G, (int)B);
        //if /*(((B>G) || (R>G)) || (G<5))*/(G!=255) {
        //if ((G<=B) || (G<=R)) {
        if ((G < R) || (G < B) /*(R > 30) || (B > 30) || ( G < 250) || (G > 255)*/) {
            dest_u[i] = U;
            dest_v[i] = V;

            dest[i * 2]     = src[i * 2];
            dest[i * 2 + 1] = src[i * 2 + 1];
        }
    }
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(YUV420Overlay, 8, 9)
    T_STRING(YUV420Overlay, 0) // target
    T_NUMBER(YUV420Overlay, 1) // width
    T_NUMBER(YUV420Overlay, 2) // height
    T_STRING(YUV420Overlay, 3) // source
    T_NUMBER(YUV420Overlay, 4) // source width
    T_NUMBER(YUV420Overlay, 5) // source height
    T_NUMBER(YUV420Overlay, 6) // target left
    T_NUMBER(YUV420Overlay, 7) // target top

    char *target = PARAM(0);
    int width  = PARAM_INT(1);
    int height = PARAM_INT(2);

    char *source       = PARAM(3);
    int  source_width  = PARAM_INT(4);
    int  source_height = PARAM_INT(5);

    int left = PARAM_INT(6);
    int top  = PARAM_INT(7);

    int transparency = -1;
    if (PARAMETERS_COUNT > 8) {
        T_NUMBER(YUV420Overlay, 8)
        transparency = PARAM_INT(8);
    }

    if (PARAM_LEN(0) < width * height * 3 / 2) {
        RETURN_NUMBER(-1);
        return 0;
    }

    if (PARAM_LEN(3) < source_width * source_height * 3 / 2) {
        RETURN_NUMBER(-2);
        return 0;
    }

    if ((top > height) || (left > width)) {
        RETURN_NUMBER(-3);
        return 0;
    }

    if ((top < 0) || (left < 0)) {
        RETURN_NUMBER(-4);
        return 0;
    }

    int max_width = source_width;
    if (source_width + left > width)
        max_width = width - left;

    int max_height = source_height;
    if (source_height + top > height)
        max_height = height - top;


    char *target_Y = target + left + top * width;
    char *source_Y = source;

    char *target_U = target + height * width + (top / 2) * (width / 2) + (left / 2);
    char *source_U = source + source_height * source_width;

    char *target_V = target + height * width + height / 2 * width / 2 + (top / 2) * (width / 2) + (left / 2);
    char *source_V = source + source_height * source_width + (source_height / 2) * (source_width / 2);

// copy luma (Y')
    char *tgt = target_Y;
    char *src = source_Y;
    if (transparency >= 0) {
        char *tgt_U = target_U;
        char *src_U = source_U;

        char *tgt_V = target_V;
        char *src_V = source_V;

        for (int i = 0; i < max_height; i++) {
            memcpy_except_value((unsigned char *)tgt, (unsigned char *)src, tgt_U, tgt_V, src_U, src_V, max_width, transparency);

            tgt += width;
            src += source_width;

            if (i % 2 == 0) {
                tgt_U += width / 2;
                tgt_V += width / 2;

                src_U += source_width / 2;
                src_V += source_width / 2;
            }
        }
    } else {
        for (int i = 0; i < max_height; i++) {
            memcpy(tgt, src, max_width);
            tgt += width;
            src += source_width;
        }
        // copy U
        // U samples
        tgt = target_U;
        src = source_U;
        for (int i = 0; i < max_height / 2; i++) {
            memcpy(tgt, src, max_width / 2);
            tgt += width / 2;
            src += source_width / 2;
        }

        // V samples
        tgt = target_V;
        src = source_V;
        for (int i = 0; i < max_height / 2; i++) {
            memcpy(tgt, src, max_width / 2);
            tgt += width / 2;
            src += source_width / 2;
        }
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

