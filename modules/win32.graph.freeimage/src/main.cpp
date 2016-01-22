//------------ standard header -----------------------------------//
#include "stdlibrary.h"
#include "AnsiString.h"
//------------ end of standard header ----------------------------//
extern "C" {
#include "FreeImage.h"
}
#include "library.h"
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(FI_RGBA_RED)
    DEFINE_ECONSTANT(FI_RGBA_GREEN)
    DEFINE_ECONSTANT(FI_RGBA_BLUE)
    DEFINE_ECONSTANT(FI_RGBA_ALPHA)
    DEFINE_ECONSTANT(FI_RGBA_RED_MASK)
    DEFINE_ECONSTANT(FI_RGBA_GREEN_MASK)
    DEFINE_ECONSTANT(FI_RGBA_BLUE_MASK)
    DEFINE_ECONSTANT(FI_RGBA_ALPHA_MASK)
    DEFINE_ECONSTANT(FI_RGBA_RED_SHIFT)
    DEFINE_ECONSTANT(FI_RGBA_GREEN_SHIFT)
    DEFINE_ECONSTANT(FI_RGBA_BLUE_SHIFT)
    DEFINE_ECONSTANT(FI_RGBA_ALPHA_SHIFT)


    DEFINE_ECONSTANT(FIF_UNKNOWN)
    DEFINE_ECONSTANT(FIF_BMP)
    DEFINE_ECONSTANT(FIF_ICO)
    DEFINE_ECONSTANT(FIF_JPEG)
    DEFINE_ECONSTANT(FIF_JNG)
    DEFINE_ECONSTANT(FIF_KOALA)
    DEFINE_ECONSTANT(FIF_LBM)
    DEFINE_ECONSTANT(FIF_IFF)
    DEFINE_ECONSTANT(FIF_MNG)
    DEFINE_ECONSTANT(FIF_PBM)
    DEFINE_ECONSTANT(FIF_PBMRAW)
    DEFINE_ECONSTANT(FIF_PCD)
    DEFINE_ECONSTANT(FIF_PCX)
    DEFINE_ECONSTANT(FIF_PGM)
    DEFINE_ECONSTANT(FIF_PGMRAW)
    DEFINE_ECONSTANT(FIF_PNG)
    DEFINE_ECONSTANT(FIF_PPM)
    DEFINE_ECONSTANT(FIF_PPMRAW)
    DEFINE_ECONSTANT(FIF_RAS)
    DEFINE_ECONSTANT(FIF_TARGA)
    DEFINE_ECONSTANT(FIF_TIFF)
    DEFINE_ECONSTANT(FIF_WBMP)
    DEFINE_ECONSTANT(FIF_PSD)
    DEFINE_ECONSTANT(FIF_CUT)
    DEFINE_ECONSTANT(FIF_XBM)
    DEFINE_ECONSTANT(FIF_XPM)
    DEFINE_ECONSTANT(FIF_DDS)
    DEFINE_ECONSTANT(FIF_GIF)
    DEFINE_ECONSTANT(FIF_HDR)


    DEFINE_ECONSTANT(BMP_DEFAULT)
    DEFINE_ECONSTANT(BMP_SAVE_RLE)
    DEFINE_ECONSTANT(CUT_DEFAULT)
    DEFINE_ECONSTANT(DDS_DEFAULT)
    DEFINE_ECONSTANT(GIF_DEFAULT)
    DEFINE_ECONSTANT(GIF_LOAD256)
    DEFINE_ECONSTANT(GIF_PLAYBACK)
    DEFINE_ECONSTANT(HDR_DEFAULT)
    DEFINE_ECONSTANT(ICO_DEFAULT)
    DEFINE_ECONSTANT(ICO_MAKEALPHA)
    DEFINE_ECONSTANT(IFF_DEFAULT)
    DEFINE_ECONSTANT(JPEG_DEFAULT)
    DEFINE_ECONSTANT(JPEG_FAST)
    DEFINE_ECONSTANT(JPEG_ACCURATE)
    DEFINE_ECONSTANT(JPEG_QUALITYSUPERB)
    DEFINE_ECONSTANT(JPEG_QUALITYGOOD)
    DEFINE_ECONSTANT(JPEG_QUALITYNORMAL)
    DEFINE_ECONSTANT(JPEG_QUALITYAVERAGE)
    DEFINE_ECONSTANT(JPEG_QUALITYBAD)
    DEFINE_ECONSTANT(JPEG_CMYK)
    DEFINE_ECONSTANT(KOALA_DEFAULT)
    DEFINE_ECONSTANT(LBM_DEFAULT)
    DEFINE_ECONSTANT(MNG_DEFAULT)
    DEFINE_ECONSTANT(PCD_DEFAULT)
    DEFINE_ECONSTANT(PCD_BASE)
    DEFINE_ECONSTANT(PCD_BASEDIV4)
    DEFINE_ECONSTANT(PCD_BASEDIV16)
    DEFINE_ECONSTANT(PCX_DEFAULT)
    DEFINE_ECONSTANT(PNG_DEFAULT)
    DEFINE_ECONSTANT(PNG_IGNOREGAMMA)
    DEFINE_ECONSTANT(PNM_DEFAULT)
    DEFINE_ECONSTANT(PNM_SAVE_RAW)
    DEFINE_ECONSTANT(PNM_SAVE_ASCII)
    DEFINE_ECONSTANT(PSD_DEFAULT)
    DEFINE_ECONSTANT(RAS_DEFAULT)
    DEFINE_ECONSTANT(TARGA_DEFAULT)
    DEFINE_ECONSTANT(TARGA_LOAD_RGB888)
    DEFINE_ECONSTANT(TIFF_DEFAULT)
    DEFINE_ECONSTANT(TIFF_CMYK)
    DEFINE_ECONSTANT(TIFF_PACKBITS)
    DEFINE_ECONSTANT(TIFF_DEFLATE)
    DEFINE_ECONSTANT(TIFF_ADOBE_DEFLATE)
    DEFINE_ECONSTANT(TIFF_NONE)
    DEFINE_ECONSTANT(TIFF_CCITTFAX3)
    DEFINE_ECONSTANT(TIFF_CCITTFAX4)
    DEFINE_ECONSTANT(TIFF_LZW)
    DEFINE_ECONSTANT(TIFF_JPEG)
    DEFINE_ECONSTANT(WBMP_DEFAULT)
    DEFINE_ECONSTANT(XBM_DEFAULT)
    DEFINE_ECONSTANT(XPM_DEFAULT)

    DEFINE_ECONSTANT(FILTER_BOX)
    DEFINE_ECONSTANT(FILTER_BICUBIC)
    DEFINE_ECONSTANT(FILTER_BILINEAR)
    DEFINE_ECONSTANT(FILTER_BSPLINE)
    DEFINE_ECONSTANT(FILTER_CATMULLROM)
    DEFINE_ECONSTANT(FILTER_LANCZOS3)

    DEFINE_ECONSTANT(FIT_UNKNOWN)
    DEFINE_ECONSTANT(FIT_BITMAP)
    DEFINE_ECONSTANT(FIT_UINT16)
    DEFINE_ECONSTANT(FIT_INT16)
    DEFINE_ECONSTANT(FIT_UINT32)
    DEFINE_ECONSTANT(FIT_INT32)
    DEFINE_ECONSTANT(FIT_FLOAT)
    DEFINE_ECONSTANT(FIT_DOUBLE)
    DEFINE_ECONSTANT(FIT_COMPLEX)
    DEFINE_ECONSTANT(FIT_RGB16)
    DEFINE_ECONSTANT(FIT_RGBA16)
    DEFINE_ECONSTANT(FIT_RGBF)
    DEFINE_ECONSTANT(FIT_RGBAF)

    DEFINE_ECONSTANT(FIQ_WUQUANT)
    DEFINE_ECONSTANT(FIQ_NNQUANT)

    DEFINE_ECONSTANT(FID_FS)
    DEFINE_ECONSTANT(FID_BAYER4x4)
    DEFINE_ECONSTANT(FID_BAYER8x8)
    DEFINE_ECONSTANT(FID_CLUSTER6x6)
    DEFINE_ECONSTANT(FID_CLUSTER8x8)
    DEFINE_ECONSTANT(FID_CLUSTER16x16)

    DEFINE_ECONSTANT(FIJPEG_OP_NONE)
    DEFINE_ECONSTANT(FIJPEG_OP_FLIP_H)
    DEFINE_ECONSTANT(FIJPEG_OP_FLIP_V)
    DEFINE_ECONSTANT(FIJPEG_OP_TRANSPOSE)
    DEFINE_ECONSTANT(FIJPEG_OP_TRANSVERSE)
    DEFINE_ECONSTANT(FIJPEG_OP_ROTATE_90)
    DEFINE_ECONSTANT(FIJPEG_OP_ROTATE_180)
    DEFINE_ECONSTANT(FIJPEG_OP_ROTATE_270)

    DEFINE_ECONSTANT(FITMO_DRAGO03)
    DEFINE_ECONSTANT(FITMO_REINHARD05)
    DEFINE_ECONSTANT(FICC_RGB)
    DEFINE_ECONSTANT(FICC_RED)
    DEFINE_ECONSTANT(FICC_GREEN)
    DEFINE_ECONSTANT(FICC_BLUE)
    DEFINE_ECONSTANT(FICC_ALPHA)
    DEFINE_ECONSTANT(FICC_BLACK)
    DEFINE_ECONSTANT(FICC_REAL)
    DEFINE_ECONSTANT(FICC_IMAG)
    DEFINE_ECONSTANT(FICC_MAG)
    DEFINE_ECONSTANT(FICC_PHASE)


    DEFINE_ECONSTANT(FIDT_NOTYPE)
    DEFINE_ECONSTANT(FIDT_BYTE)
    DEFINE_ECONSTANT(FIDT_ASCII)
    DEFINE_ECONSTANT(FIDT_SHORT)
    DEFINE_ECONSTANT(FIDT_LONG)
    DEFINE_ECONSTANT(FIDT_RATIONAL)
    DEFINE_ECONSTANT(FIDT_SBYTE)
    DEFINE_ECONSTANT(FIDT_UNDEFINED)
    DEFINE_ECONSTANT(FIDT_SSHORT)
    DEFINE_ECONSTANT(FIDT_SLONG)
    DEFINE_ECONSTANT(FIDT_SRATIONAL)
    DEFINE_ECONSTANT(FIDT_FLOAT)
    DEFINE_ECONSTANT(FIDT_DOUBLE)
    DEFINE_ECONSTANT(FIDT_IFD)
    DEFINE_ECONSTANT(FIDT_PALETTE)

    DEFINE_ECONSTANT(FIMD_NODATA)
    DEFINE_ECONSTANT(FIMD_COMMENTS)
    DEFINE_ECONSTANT(FIMD_EXIF_MAIN)
    DEFINE_ECONSTANT(FIMD_EXIF_EXIF)
    DEFINE_ECONSTANT(FIMD_EXIF_GPS)
    DEFINE_ECONSTANT(FIMD_EXIF_MAKERNOTE)
    DEFINE_ECONSTANT(FIMD_EXIF_INTEROP)
    DEFINE_ECONSTANT(FIMD_IPTC)
    DEFINE_ECONSTANT(FIMD_XMP)
    DEFINE_ECONSTANT(FIMD_GEOTIFF)
    DEFINE_ECONSTANT(FIMD_ANIMATION)
    DEFINE_ECONSTANT(FIMD_CUSTOM)

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Initialise CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_Initialise' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Initialise: parameter 1 should be of STATIC NUMBER type";

    // function call
    FreeImage_Initialise((BOOL)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_DeInitialise CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)": 'FreeImage_DeInitialise' parameters error. This fuction takes  no parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    // Specific variables

    // Variable type check

    // function call
    FreeImage_DeInitialise();

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetVersion CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)": 'FreeImage_GetVersion' parameters error. This fuction takes  no parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    char *_C_call_result;
    // Specific variables

    // Variable type check

    // function call
    _C_call_result = (char *)FreeImage_GetVersion();

    SetVariable(RESULT, VARIABLE_STRING, _C_call_result, 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetCopyrightMessage CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)": 'FreeImage_GetCopyrightMessage' parameters error. This fuction takes  no parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    char *_C_call_result;
    // Specific variables

    // Variable type check

    // function call
    _C_call_result = (char *)FreeImage_GetCopyrightMessage();

    SetVariable(RESULT, VARIABLE_STRING, _C_call_result, 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Allocate CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 6)
        return (void *)": 'FreeImage_Allocate' parameters error. This fuction takes 6 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FIBITMAP *_C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;
    NUMBER nParam2;
    NUMBER nParam3;
    NUMBER nParam4;
    NUMBER nParam5;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Allocate: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Allocate: parameter 2 should be of STATIC NUMBER type";
    // Parameter 3
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &STRING_DUMMY, &nParam2);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Allocate: parameter 3 should be of STATIC NUMBER type";
    // Parameter 4
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], &TYPE, &STRING_DUMMY, &nParam3);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Allocate: parameter 4 should be of STATIC NUMBER type";
    // Parameter 5
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[4] - 1], &TYPE, &STRING_DUMMY, &nParam4);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Allocate: parameter 5 should be of STATIC NUMBER type";
    // Parameter 6
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[5] - 1], &TYPE, &STRING_DUMMY, &nParam5);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Allocate: parameter 6 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (FIBITMAP *)FreeImage_Allocate((int)nParam0, (int)nParam1, (int)nParam2, (unsigned)nParam3, (unsigned)nParam4, (unsigned)nParam5);

    SetVariable(RESULT, VARIABLE_NUMBER, "", (SYS_INT)_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_AllocateT CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 7)
        return (void *)": 'FreeImage_AllocateT' parameters error. This fuction takes 6 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FIBITMAP *_C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;
    NUMBER nParam2;
    NUMBER nParam3;
    NUMBER nParam4;
    NUMBER nParam5;
    NUMBER nParam6;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_AllocateT: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_AllocateT: parameter 2 should be of STATIC NUMBER type";
    // Parameter 3
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &STRING_DUMMY, &nParam2);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_AllocateT: parameter 3 should be of STATIC NUMBER type";
    // Parameter 4
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], &TYPE, &STRING_DUMMY, &nParam3);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_AllocateT: parameter 4 should be of STATIC NUMBER type";
    // Parameter 5
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[4] - 1], &TYPE, &STRING_DUMMY, &nParam4);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_AllocateT: parameter 5 should be of STATIC NUMBER type";
    // Parameter 6
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[5] - 1], &TYPE, &STRING_DUMMY, &nParam5);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_AllocateT: parameter 6 should be of STATIC NUMBER type";
    // Parameter 7
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[5] - 1], &TYPE, &STRING_DUMMY, &nParam6);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_AllocateT: parameter 7 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (FIBITMAP *)FreeImage_AllocateT((FREE_IMAGE_TYPE)(int)nParam0, (int)nParam1, (int)nParam2, (int)nParam3, (unsigned)nParam4, (unsigned)nParam5, (unsigned)nParam6);

    SetVariable(RESULT, VARIABLE_NUMBER, "", (SYS_INT)_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Load CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 3)
        return (void *)": 'FreeImage_Load' parameters error. This fuction takes 3 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FIBITMAP *_C_call_result;
    // Specific variables
    NUMBER nParam0;
    char   *szParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Load: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &szParam1, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"FreeImage_Load: parameter 2 should be of STATIC STRING type";
    // Parameter 3
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &STRING_DUMMY, &nParam2);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Load: parameter 3 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (FIBITMAP *)FreeImage_Load((FREE_IMAGE_FORMAT)(int)nParam0, (char *)szParam1, (int)nParam2);

    SetVariable(RESULT, VARIABLE_NUMBER, "", (SYS_INT)_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------

/*CONCEPT_DLL_API CONCEPT__FreeImage_LoadU CONCEPT_API_PARAMETERS {
        if (PARAMETERS->COUNT!=3)
           return (void *)": 'FreeImage_LoadU' parameters error. This fuction takes 3 parameters.";

        // General variables
        NUMBER       NUMBER_DUMMY;
        char *       STRING_DUMMY;
        INTEGER      TYPE;

        // Result
        FIBITMAP*	_C_call_result;
        // Specific variables
        NUMBER       nParam0;
        char *       szParam1;
        NUMBER       nParam2;

        // Variable type check
        // Parameter 1
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0]-1], &TYPE, &STRING_DUMMY, &nParam0);
        if (TYPE!=VARIABLE_NUMBER)
           return (void *)"FreeImage_LoadU: parameter 1 should be of STATIC NUMBER type";
        // Parameter 2
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1]-1], &TYPE, &szParam1, &NUMBER_DUMMY);
        if (TYPE!=VARIABLE_STRING)
           return (void *)"FreeImage_LoadU: parameter 2 should be of STATIC STRING type";
        // Parameter 3
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2]-1], &TYPE, &STRING_DUMMY, &nParam2);
        if (TYPE!=VARIABLE_NUMBER)
           return (void *)"FreeImage_LoadU: parameter 3 should be of STATIC NUMBER type";

        // function call
        _C_call_result=(FIBITMAP*)FreeImage_LoadU((FREE_IMAGE_FORMAT)(int)nParam0, (wchar_t*)szParam1, (int)nParam2);

        SetVariable(RESULT,VARIABLE_NUMBER,"",(SYS_INT)_C_call_result);
        return 0;
   }*/
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Save CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 4)
        return (void *)": 'FreeImage_Save' parameters error. This fuction takes 4 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    BOOL _C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;
    char   *szParam2;
    NUMBER nParam3;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Save: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Save: parameter 2 should be of STATIC NUMBER type";
    // Parameter 3
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &szParam2, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"FreeImage_Save: parameter 3 should be of STATIC STRING type";
    // Parameter 4
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], &TYPE, &STRING_DUMMY, &nParam3);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Save: parameter 4 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (BOOL)FreeImage_Save((FREE_IMAGE_FORMAT)(int)nParam0, (FIBITMAP *)(SYS_INT)nParam1, (char *)szParam2, (int)nParam3);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------

/*CONCEPT_DLL_API CONCEPT__FreeImage_SaveU CONCEPT_API_PARAMETERS {
        if (PARAMETERS->COUNT!=4)
           return (void *)": 'FreeImage_SaveU' parameters error. This fuction takes 4 parameters.";

        // General variables
        NUMBER       NUMBER_DUMMY;
        char *       STRING_DUMMY;
        INTEGER      TYPE;

        // Result
        BOOL	_C_call_result;
        // Specific variables
        NUMBER       nParam0;
        NUMBER       nParam1;
        char *       szParam2;
        NUMBER       nParam3;

        // Variable type check
        // Parameter 1
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0]-1], &TYPE, &STRING_DUMMY, &nParam0);
        if (TYPE!=VARIABLE_NUMBER)
           return (void *)"FreeImage_SaveU: parameter 1 should be of STATIC NUMBER type";
        // Parameter 2
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1]-1], &TYPE, &STRING_DUMMY, &nParam1);
        if (TYPE!=VARIABLE_NUMBER)
           return (void *)"FreeImage_SaveU: parameter 2 should be of STATIC NUMBER type";
        // Parameter 3
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2]-1], &TYPE, &szParam2, &NUMBER_DUMMY);
        if (TYPE!=VARIABLE_STRING)
           return (void *)"FreeImage_SaveU: parameter 3 should be of STATIC STRING type";
        // Parameter 4
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3]-1], &TYPE, &STRING_DUMMY, &nParam3);
        if (TYPE!=VARIABLE_NUMBER)
           return (void *)"FreeImage_SaveU: parameter 4 should be of STATIC NUMBER type";

        // function call
        _C_call_result=(BOOL)FreeImage_SaveU((FREE_IMAGE_FORMAT)(int)nParam0, (FIBITMAP*)(SYS_INT)nParam1, (wchar_t*)szParam2, (int)nParam3);

        SetVariable(RESULT,VARIABLE_NUMBER,"",_C_call_result);
        return 0;
   }*/
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Unload CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_Unload' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_Unload: parameter 1 should be of STATIC NUMBER type";

    // function call
    FreeImage_Unload((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_SetPixelColor CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 6)
        return (void *)": 'FreeImage_SetPixelColor' parameters error. This fuction takes 6 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    BOOL _C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;
    NUMBER nParam2;
    NUMBER nParam3;
    NUMBER nParam4;
    NUMBER nParam5;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 2 should be of STATIC NUMBER type";
    // Parameter 3
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &STRING_DUMMY, &nParam2);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 3 should be of STATIC NUMBER type";
    // Parameter 4
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], &TYPE, &STRING_DUMMY, &nParam3);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 4 should be of STATIC NUMBER type";
    // Parameter 5
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[4] - 1], &TYPE, &STRING_DUMMY, &nParam4);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 5 should be of STATIC NUMBER type";
    // Parameter 6
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[5] - 1], &TYPE, &STRING_DUMMY, &nParam5);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 6 should be of STATIC NUMBER type";

    // function call
    RGBQUAD rgbquad;
    rgbquad.rgbRed   = (unsigned)nParam3;
    rgbquad.rgbGreen = (unsigned)nParam4;
    rgbquad.rgbBlue  = (unsigned)nParam5;

    _C_call_result = (BOOL)FreeImage_SetPixelColor((FIBITMAP *)(SYS_INT)nParam0, (unsigned)nParam1, (unsigned)nParam2, &rgbquad);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetPixelColor CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 6)
        return (void *)": 'FreeImage_GetPixelColor' parameters error. This fuction takes 6 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    BOOL _C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;
    NUMBER nParam2;
    NUMBER nParam3;
    NUMBER nParam4;
    NUMBER nParam5;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 2 should be of STATIC NUMBER type";
    // Parameter 3
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &STRING_DUMMY, &nParam2);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 3 should be of STATIC NUMBER type";
    // Parameter 4
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], &TYPE, &STRING_DUMMY, &nParam3);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 4 should be of STATIC NUMBER type";
    // Parameter 5
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[4] - 1], &TYPE, &STRING_DUMMY, &nParam4);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 5 should be of STATIC NUMBER type";
    // Parameter 6
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[5] - 1], &TYPE, &STRING_DUMMY, &nParam5);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetPixelColor: parameter 6 should be of STATIC NUMBER type";

    // function call
    RGBQUAD rgbquad;
    rgbquad.rgbRed   = (unsigned)nParam3;
    rgbquad.rgbGreen = (unsigned)nParam4;
    rgbquad.rgbBlue  = (unsigned)nParam5;

    _C_call_result = (BOOL)FreeImage_GetPixelColor((FIBITMAP *)(SYS_INT)nParam0, (unsigned)nParam1, (unsigned)nParam2, &rgbquad);

    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], VARIABLE_NUMBER, "", rgbquad.rgbRed);
    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[4] - 1], VARIABLE_NUMBER, "", rgbquad.rgbGreen);
    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[5] - 1], VARIABLE_NUMBER, "", rgbquad.rgbBlue);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetImageType CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetImageType' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FREE_IMAGE_TYPE _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetImageType: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (FREE_IMAGE_TYPE)FreeImage_GetImageType((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetColorsUsed CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetColorsUsed' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetColorsUsed: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetColorsUsed((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetBPP CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetBPP' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetBPP: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetBPP((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetWidth CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetWidth' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetWidth: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetWidth((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetHeight CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetHeight' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetHeight: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetHeight((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetLine CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetLine' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetLine: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetLine((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetPitch CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetPitch' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetPitch: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetPitch((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetDIBSize CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetDIBSize' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetDIBSize: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetDIBSize((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetDotsPerMeterX CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetDotsPerMeterX' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetDotsPerMeterX: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetDotsPerMeterX((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetDotsPerMeterY CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetDotsPerMeterY' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetDotsPerMeterY: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetDotsPerMeterY((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_SetDotsPerMeterX CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)": 'FreeImage_SetDotsPerMeterX' parameters error. This fuction takes 2 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetDotsPerMeterX: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetDotsPerMeterX: parameter 2 should be of STATIC NUMBER type";

    // function call
    FreeImage_SetDotsPerMeterX((FIBITMAP *)(SYS_INT)nParam0, (unsigned)nParam1);

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_SetDotsPerMeterY CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)": 'FreeImage_SetDotsPerMeterY' parameters error. This fuction takes 2 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetDotsPerMeterY: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetDotsPerMeterY: parameter 2 should be of STATIC NUMBER type";

    // function call
    FreeImage_SetDotsPerMeterY((FIBITMAP *)(SYS_INT)nParam0, (unsigned)nParam1);

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetColorType CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetColorType' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FREE_IMAGE_COLOR_TYPE _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetColorType: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (FREE_IMAGE_COLOR_TYPE)FreeImage_GetColorType((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetRedMask CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetRedMask' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetRedMask: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetRedMask((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetGreenMask CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetGreenMask' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetGreenMask: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetGreenMask((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetBlueMask CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetBlueMask' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetBlueMask: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetBlueMask((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetTransparencyCount CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_GetTransparencyCount' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    unsigned _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_GetTransparencyCount: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned)FreeImage_GetTransparencyCount((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_SetTransparent CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)": 'FreeImage_SetTransparent' parameters error. This fuction takes 2 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetTransparent: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_SetTransparent: parameter 2 should be of STATIC NUMBER type";

    // function call
    FreeImage_SetTransparent((FIBITMAP *)(SYS_INT)nParam0, (BOOL)nParam1);

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_IsTransparent CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_IsTransparent' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    BOOL _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_IsTransparent: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (BOOL)FreeImage_IsTransparent((FIBITMAP *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_HasBackgroundColor CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'FreeImage_HasBackgroundColor' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    BOOL _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"FreeImage_HasBackgroundColor: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (BOOL)FreeImage_HasBackgroundColor((FIBITMAP *)(SYS_INT)nParam0);
    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Clone CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'FreeImage_Clone' parameters error. This function takes one parameter");
    LOCAL_INIT;

    NUMBER dib;
    GET_CHECK_NUMBER(0, dib, "FreeImage_Clone: parameter 1 should be a valid handle");

    FIBITMAP *CLONE = FreeImage_Clone((FIBITMAP *)(SYS_INT)dib);

    RETURN_NUMBER((SYS_INT)CLONE);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Rescale CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "'FreeImage_Rescale' parameters error. This function takes 4 parameters");
    LOCAL_INIT;

    NUMBER dib;
    NUMBER dst_width;
    NUMBER dst_height;
    NUMBER filter;

    GET_CHECK_NUMBER(0, dib, "FreeImage_Rescale: parameter 1 should be a valid handle");
    GET_CHECK_NUMBER(1, dst_width, "FreeImage_Rescale: parameter 2 shoult be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(2, dst_height, "FreeImage_Rescale: parameter 3 shoult be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(3, filter, "FreeImage_Rescale: parameter 4 shoult be a number (STATIC NUMBER)");

    FIBITMAP *CLONE = FreeImage_Rescale((FIBITMAP *)(SYS_INT)dib, (int)dst_width, (int)dst_height, (FREE_IMAGE_FILTER)(int)filter);

    RETURN_NUMBER((SYS_INT)CLONE);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_GetFileType CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'FreeImage_GetFileType' parameters error. This function takes one parameter (szFilename)");
    LOCAL_INIT;

    char *filename;

    GET_CHECK_STRING(0, filename, "FreeImage_GetFileType: parameter 1 should be a string");

    int ftype = (int)FreeImage_GetFileType(filename);

    RETURN_NUMBER((SYS_INT)ftype);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Copy CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(5, "'FreeImage_Copy' parameters error. This function takes 5 parameters");
    LOCAL_INIT;

    NUMBER dib;
    NUMBER dst_left;
    NUMBER dst_top;
    NUMBER dst_right;
    NUMBER dst_bottom;

    GET_CHECK_NUMBER(0, dib, "FreeImage_Copy: parameter 1 should be a valid handle");
    GET_CHECK_NUMBER(1, dst_left, "FreeImage_Copy: parameter 2 shoult be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(2, dst_top, "FreeImage_Copy: parameter 3 shoult be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(3, dst_right, "FreeImage_Copy: parameter 4 shoult be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(4, dst_bottom, "FreeImage_Copy: parameter 5 shoult be a number (STATIC NUMBER)");

    FIBITMAP *CLONE = FreeImage_Copy((FIBITMAP *)(SYS_INT)dib, (int)dst_left, (int)dst_top, (int)dst_right, (int)dst_bottom);

    RETURN_NUMBER((SYS_INT)CLONE);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Paste CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(5, "'FreeImage_Paste' parameters error. This function takes 5 parameters");
    LOCAL_INIT;

    NUMBER dst;
    NUMBER src;
    NUMBER left;
    NUMBER top;
    NUMBER alpha;

    GET_CHECK_NUMBER(0, dst, "FreeImage_Paste: parameter 1 should be a valid handle");
    GET_CHECK_NUMBER(1, src, "FreeImage_Paste: parameter 2 should be a valid handle");
    GET_CHECK_NUMBER(2, left, "FreeImage_Paste: parameter 3 shoult be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(3, top, "FreeImage_Paste: parameter 4 shoult be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(4, alpha, "FreeImage_Paste: parameter 5 shoult be a number (STATIC NUMBER)");

    int res = FreeImage_Paste((FIBITMAP *)(SYS_INT)dst, (FIBITMAP *)(SYS_INT)src, (int)left, (int)top, (int)alpha);

    RETURN_NUMBER(res);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Composite CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'FreeImage_Composite' parameters error. This function takes 2 parameters");
    LOCAL_INIT;

    NUMBER dst;
    NUMBER src;
    NUMBER left;
    NUMBER top;
    NUMBER alpha;

    GET_CHECK_NUMBER(0, dst, "FreeImage_Paste: parameter 1 should be a valid handle");
    GET_CHECK_NUMBER(1, src, "FreeImage_Paste: parameter 2 should be a valid handle");

    FIBITMAP *res = FreeImage_Composite((FIBITMAP *)(SYS_INT)dst, 1, NULL, (FIBITMAP *)(SYS_INT)src);

    RETURN_NUMBER((SYS_INT)res);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_FlipVertical CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'FreeImage_FlipVertical' parameters error. This function takes one parameter");
    LOCAL_INIT;

    NUMBER dst;

    GET_CHECK_NUMBER(0, dst, "FreeImage_FlipVertical: parameter 1 should be a valid handle");

    if (dst) {
        bool res = FreeImage_FlipVertical((FIBITMAP *)(SYS_INT)dst);
        RETURN_NUMBER((SYS_INT)res);
    } else {
        RETURN_NUMBER((SYS_INT)0);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_FlipHorizontal CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'FreeImage_FlipHorizontal' parameters error. This function takes one parameter");
    LOCAL_INIT;

    NUMBER dst;

    GET_CHECK_NUMBER(0, dst, "FreeImage_FlipHorizontal: parameter 1 should be a valid handle");

    if (dst) {
        bool res = FreeImage_FlipHorizontal((FIBITMAP *)(SYS_INT)dst);
        RETURN_NUMBER((SYS_INT)res);
    } else {
        RETURN_NUMBER((SYS_INT)0);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FreeImage_Rotate CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'FreeImage_Rotate' parameters error. This function takes 2 parameters");
    LOCAL_INIT;

    NUMBER dst;
    NUMBER src;

    GET_CHECK_NUMBER(0, dst, "FreeImage_Rotate: parameter 1 should be a valid handle");
    GET_CHECK_NUMBER(1, src, "FreeImage_Rotate: parameter 2 should be a number");

    FIBITMAP *res = FreeImage_Rotate((FIBITMAP *)(SYS_INT)dst, (SYS_INT)src);

    RETURN_NUMBER((SYS_INT)res);
    return 0;
}
//-----------------------------------------------------------------------------------
