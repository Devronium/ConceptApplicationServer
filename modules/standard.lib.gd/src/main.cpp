//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <gd.h>

INVOKE_CALL InvokePtr = 0;

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
#ifdef WIN32
    AnsiString path = getenv("GDFONTPATH");
    if (!path.Length()) {
        AnsiString windir = getenv("windir");
        if (windir.Length()) {
            _putenv(AnsiString("GDFONTPATH=") + windir + "\\Fonts");
        }
    }
#endif
    DEFINE_ECONSTANT(GD_MAJOR_VERSION)
    DEFINE_ECONSTANT(GD_MINOR_VERSION)
    DEFINE_ECONSTANT(GD_RELEASE_VERSION)
    DEFINE_ECONSTANT(GD_EXTRA_VERSION)
    DEFINE_ECONSTANT(GD_VERSION_STRING)
    DEFINE_ECONSTANT(gdMaxColors)
    DEFINE_ECONSTANT(gdAlphaMax)
    DEFINE_ECONSTANT(gdAlphaOpaque)
    DEFINE_ECONSTANT(gdAlphaTransparent)
    DEFINE_ECONSTANT(gdRedMax)
    DEFINE_ECONSTANT(gdGreenMax)
    DEFINE_ECONSTANT(gdBlueMax)
    DEFINE_ECONSTANT(gdDashSize)
    DEFINE_ECONSTANT(gdStyled)
    DEFINE_ECONSTANT(gdBrushed)
    DEFINE_ECONSTANT(gdStyledBrushed)
    DEFINE_ECONSTANT(gdTiled)
    DEFINE_ECONSTANT(gdTransparent)
    DEFINE_ECONSTANT(gdAntiAliased)
    //DEFINE_ECONSTANT(gdImageCreatePalette)
    DEFINE_ECONSTANT(gdFTEX_LINESPACE)
    DEFINE_ECONSTANT(gdFTEX_CHARMAP)
    DEFINE_ECONSTANT(gdFTEX_RESOLUTION)
    DEFINE_ECONSTANT(gdFTEX_DISABLE_KERNING)
    DEFINE_ECONSTANT(gdFTEX_XSHOW)
    DEFINE_ECONSTANT(gdFTEX_FONTPATHNAME)
    DEFINE_ECONSTANT(gdFTEX_FONTCONFIG)
    DEFINE_ECONSTANT(gdFTEX_RETURNFONTPATHNAME)
    DEFINE_ECONSTANT(gdFTEX_Unicode)
    DEFINE_ECONSTANT(gdFTEX_Shift_JIS)
    DEFINE_ECONSTANT(gdFTEX_Big5)
    DEFINE_ECONSTANT(gdFTEX_Adobe_Custom)

    DEFINE_ECONSTANT(gdDisposalUnknown)
    DEFINE_ECONSTANT(gdDisposalNone)
    DEFINE_ECONSTANT(gdDisposalRestoreBackground)
    DEFINE_ECONSTANT(gdDisposalRestorePrevious)
    DEFINE_ECONSTANT(gdArc)
    DEFINE_ECONSTANT(gdPie)
    DEFINE_ECONSTANT(gdChord)
    DEFINE_ECONSTANT(gdNoFill)
    DEFINE_ECONSTANT(gdEdged)



    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdAlphaBlend, 2)
    T_NUMBER(gdAlphaBlend, 0)     // int
    T_NUMBER(gdAlphaBlend, 1)     // int

    RETURN_NUMBER(gdAlphaBlend((int)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreate, 2)
    T_NUMBER(gdImageCreate, 0)     // int
    T_NUMBER(gdImageCreate, 1)     // int

    RETURN_NUMBER((long)gdImageCreate((int)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateTrueColor, 2)
    T_NUMBER(gdImageCreateTrueColor, 0)     // int
    T_NUMBER(gdImageCreateTrueColor, 1)     // int

    RETURN_NUMBER((long)gdImageCreateTrueColor((int)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromPng, 1)
    T_NUMBER(gdImageCreateFromPng, 0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromPng((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromPngCtx, 1)
    T_NUMBER(gdImageCreateFromPngCtx, 0)     // gdIOCtxPtr

    RETURN_NUMBER((long)gdImageCreateFromPngCtx((gdIOCtxPtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromPngPtr, 2)
    T_NUMBER(gdImageCreateFromPngPtr, 0)     // int
    T_NUMBER(gdImageCreateFromPngPtr, 1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromPngPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGif, 1)
    T_NUMBER(gdImageCreateFromGif, 0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromGif((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGifCtx, 1)
    T_NUMBER(gdImageCreateFromGifCtx, 0)     // gdIOCtxPtr

    RETURN_NUMBER((long)gdImageCreateFromGifCtx((gdIOCtxPtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGifPtr, 2)
    T_NUMBER(gdImageCreateFromGifPtr, 0)     // int
    T_NUMBER(gdImageCreateFromGifPtr, 1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromGifPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromWBMP, 1)
    T_NUMBER(gdImageCreateFromWBMP, 0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromWBMP((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromWBMPCtx, 1)
    T_NUMBER(gdImageCreateFromWBMPCtx, 0)     // gdIOCtx*

    RETURN_NUMBER((long)gdImageCreateFromWBMPCtx((gdIOCtx *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromWBMPPtr, 2)
    T_NUMBER(gdImageCreateFromWBMPPtr, 0)     // int
    T_NUMBER(gdImageCreateFromWBMPPtr, 1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromWBMPPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromJpeg, 1)
    T_NUMBER(gdImageCreateFromJpeg, 0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromJpeg((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromJpegCtx, 1)
    T_NUMBER(gdImageCreateFromJpegCtx, 0)     // gdIOCtx*

    RETURN_NUMBER((long)gdImageCreateFromJpegCtx((gdIOCtx *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromJpegPtr, 2)
    T_NUMBER(gdImageCreateFromJpegPtr, 0)     // int
    T_NUMBER(gdImageCreateFromJpegPtr, 1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromJpegPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromPngSource, 1)
    T_NUMBER(gdImageCreateFromPngSource, 0)     // gdSourcePtr

    RETURN_NUMBER((long)gdImageCreateFromPngSource((gdSourcePtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd, 1)
    T_NUMBER(gdImageCreateFromGd, 0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromGd((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGdCtx, 1)
    T_NUMBER(gdImageCreateFromGdCtx, 0)     // gdIOCtxPtr

    RETURN_NUMBER((long)gdImageCreateFromGdCtx((gdIOCtxPtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGdPtr, 2)
    T_NUMBER(gdImageCreateFromGdPtr, 0)     // int
    T_NUMBER(gdImageCreateFromGdPtr, 1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromGdPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2, 1)
    T_NUMBER(gdImageCreateFromGd2, 0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromGd2((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2Ctx, 1)
    T_NUMBER(gdImageCreateFromGd2Ctx, 0)     // gdIOCtxPtr

    RETURN_NUMBER((long)gdImageCreateFromGd2Ctx((gdIOCtxPtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2Ptr, 2)
    T_NUMBER(gdImageCreateFromGd2Ptr, 0)     // int
    T_NUMBER(gdImageCreateFromGd2Ptr, 1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromGd2Ptr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2Part, 5)
    T_NUMBER(gdImageCreateFromGd2Part, 0)     // FILE*
    T_NUMBER(gdImageCreateFromGd2Part, 1)     // int
    T_NUMBER(gdImageCreateFromGd2Part, 2)     // int
    T_NUMBER(gdImageCreateFromGd2Part, 3)     // int
    T_NUMBER(gdImageCreateFromGd2Part, 4)     // int

    RETURN_NUMBER((long)gdImageCreateFromGd2Part((FILE *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2PartCtx, 5)
    T_NUMBER(gdImageCreateFromGd2PartCtx, 0)     // gdIOCtxPtr
    T_NUMBER(gdImageCreateFromGd2PartCtx, 1)     // int
    T_NUMBER(gdImageCreateFromGd2PartCtx, 2)     // int
    T_NUMBER(gdImageCreateFromGd2PartCtx, 3)     // int
    T_NUMBER(gdImageCreateFromGd2PartCtx, 4)     // int

    RETURN_NUMBER((long)gdImageCreateFromGd2PartCtx((gdIOCtxPtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2PartPtr, 6)
    T_NUMBER(gdImageCreateFromGd2PartPtr, 0)     // int
    T_NUMBER(gdImageCreateFromGd2PartPtr, 1)     // void*
    T_NUMBER(gdImageCreateFromGd2PartPtr, 2)     // int
    T_NUMBER(gdImageCreateFromGd2PartPtr, 3)     // int
    T_NUMBER(gdImageCreateFromGd2PartPtr, 4)     // int
    T_NUMBER(gdImageCreateFromGd2PartPtr, 5)     // int

    RETURN_NUMBER((long)gdImageCreateFromGd2PartPtr((int)PARAM(0), (void *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromXbm, 1)
    T_NUMBER(gdImageCreateFromXbm, 0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromXbm((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromXpm, 1)
    T_STRING(gdImageCreateFromXpm, 0)     // char*

    RETURN_NUMBER((long)gdImageCreateFromXpm(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageDestroy, 1)
    T_NUMBER(gdImageDestroy, 0)     // gdImagePtr

    gdImageDestroy((gdImagePtr)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetPixel, 4)
    T_NUMBER(gdImageSetPixel, 0)     // gdImagePtr
    T_NUMBER(gdImageSetPixel, 1)     // int
    T_NUMBER(gdImageSetPixel, 2)     // int
    T_NUMBER(gdImageSetPixel, 3)     // int

    gdImageSetPixel((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetPixel, 3)
    T_NUMBER(gdImageGetPixel, 0)     // gdImagePtr
    T_NUMBER(gdImageGetPixel, 1)     // int
    T_NUMBER(gdImageGetPixel, 2)     // int

    RETURN_NUMBER(gdImageGetPixel((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetTrueColorPixel, 3)
    T_NUMBER(gdImageGetTrueColorPixel, 0)     // gdImagePtr
    T_NUMBER(gdImageGetTrueColorPixel, 1)     // int
    T_NUMBER(gdImageGetTrueColorPixel, 2)     // int

    RETURN_NUMBER(gdImageGetTrueColorPixel((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageAABlend, 1)
    T_NUMBER(gdImageAABlend, 0)     // gdImagePtr

    gdImageAABlend((gdImagePtr)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageLine, 6)
    T_NUMBER(gdImageLine, 0)     // gdImagePtr
    T_NUMBER(gdImageLine, 1)     // int
    T_NUMBER(gdImageLine, 2)     // int
    T_NUMBER(gdImageLine, 3)     // int
    T_NUMBER(gdImageLine, 4)     // int
    T_NUMBER(gdImageLine, 5)     // int

    gdImageLine((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageDashedLine, 6)
    T_NUMBER(gdImageDashedLine, 0)     // gdImagePtr
    T_NUMBER(gdImageDashedLine, 1)     // int
    T_NUMBER(gdImageDashedLine, 2)     // int
    T_NUMBER(gdImageDashedLine, 3)     // int
    T_NUMBER(gdImageDashedLine, 4)     // int
    T_NUMBER(gdImageDashedLine, 5)     // int

    gdImageDashedLine((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageRectangle, 6)
    T_NUMBER(gdImageRectangle, 0)     // gdImagePtr
    T_NUMBER(gdImageRectangle, 1)     // int
    T_NUMBER(gdImageRectangle, 2)     // int
    T_NUMBER(gdImageRectangle, 3)     // int
    T_NUMBER(gdImageRectangle, 4)     // int
    T_NUMBER(gdImageRectangle, 5)     // int

    gdImageRectangle((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFilledRectangle, 6)
    T_NUMBER(gdImageFilledRectangle, 0)     // gdImagePtr
    T_NUMBER(gdImageFilledRectangle, 1)     // int
    T_NUMBER(gdImageFilledRectangle, 2)     // int
    T_NUMBER(gdImageFilledRectangle, 3)     // int
    T_NUMBER(gdImageFilledRectangle, 4)     // int
    T_NUMBER(gdImageFilledRectangle, 5)     // int

    gdImageFilledRectangle((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetClip, 5)
    T_NUMBER(gdImageSetClip, 0)     // gdImagePtr
    T_NUMBER(gdImageSetClip, 1)     // int
    T_NUMBER(gdImageSetClip, 2)     // int
    T_NUMBER(gdImageSetClip, 3)     // int
    T_NUMBER(gdImageSetClip, 4)     // int

    gdImageSetClip((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetClip, 5)
    T_NUMBER(gdImageGetClip, 0)     // gdImagePtr

// ... parameter 1 is by reference (int*)
    int local_parameter_1;
// ... parameter 2 is by reference (int*)
    int local_parameter_2;
// ... parameter 3 is by reference (int*)
    int local_parameter_3;
// ... parameter 4 is by reference (int*)
    int local_parameter_4;

    gdImageGetClip((gdImagePtr)(long)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3, &local_parameter_4);
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageBoundsSafe, 3)
    T_NUMBER(gdImageBoundsSafe, 0)     // gdImagePtr
    T_NUMBER(gdImageBoundsSafe, 1)     // int
    T_NUMBER(gdImageBoundsSafe, 2)     // int

    RETURN_NUMBER(gdImageBoundsSafe((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageChar, 6)
    T_NUMBER(gdImageChar, 0)     // gdImagePtr
    T_NUMBER(gdImageChar, 1)     // gdFontPtr
    T_NUMBER(gdImageChar, 2)     // int
    T_NUMBER(gdImageChar, 3)     // int
    T_NUMBER(gdImageChar, 4)     // int
    T_NUMBER(gdImageChar, 5)     // int

    gdImageChar((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCharUp, 6)
    T_NUMBER(gdImageCharUp, 0)     // gdImagePtr
    T_NUMBER(gdImageCharUp, 1)     // gdFontPtr
    T_NUMBER(gdImageCharUp, 2)     // int
    T_NUMBER(gdImageCharUp, 3)     // int
    T_NUMBER(gdImageCharUp, 4)     // int
    T_NUMBER(gdImageCharUp, 5)     // int

    gdImageCharUp((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageString, 6)
    T_NUMBER(gdImageString, 0)     // gdImagePtr
    T_NUMBER(gdImageString, 1)     // gdFontPtr
    T_NUMBER(gdImageString, 2)     // int
    T_NUMBER(gdImageString, 3)     // int
    T_STRING(gdImageString, 4)     // unsigned char
    T_NUMBER(gdImageString, 5)     // int

    gdImageString((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (unsigned char *)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringUp, 6)
    T_NUMBER(gdImageStringUp, 0)     // gdImagePtr
    T_NUMBER(gdImageStringUp, 1)     // gdFontPtr
    T_NUMBER(gdImageStringUp, 2)     // int
    T_NUMBER(gdImageStringUp, 3)     // int
    T_STRING(gdImageStringUp, 4)     // unsigned string
    T_NUMBER(gdImageStringUp, 5)     // int

    gdImageStringUp((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (unsigned char *)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageString16, 6)
    T_NUMBER(gdImageString16, 0)     // gdImagePtr
    T_NUMBER(gdImageString16, 1)     // gdFontPtr
    T_NUMBER(gdImageString16, 2)     // int
    T_NUMBER(gdImageString16, 3)     // int
    T_STRING(gdImageString16, 4)     // unsigned short
    T_NUMBER(gdImageString16, 5)     // int

    gdImageString16((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (unsigned short *)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringUp16, 6)
    T_NUMBER(gdImageStringUp16, 0)     // gdImagePtr
    T_NUMBER(gdImageStringUp16, 1)     // gdFontPtr
    T_NUMBER(gdImageStringUp16, 2)     // int
    T_NUMBER(gdImageStringUp16, 3)     // int
    T_STRING(gdImageStringUp16, 4)     // unsigned short
    T_NUMBER(gdImageStringUp16, 5)     // int

    gdImageStringUp16((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (unsigned short *)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdFontCacheSetup, 0)

    RETURN_NUMBER(gdFontCacheSetup())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdFontCacheShutdown, 0)

    gdFontCacheShutdown();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdFreeFontCache, 0)

    gdFreeFontCache();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringTTF, 9)
    T_NUMBER(gdImageStringTTF, 0) // gdImage*
    T_ARRAY(gdImageStringTTF, 1)  // int*
    T_NUMBER(gdImageStringTTF, 2) // int
    T_STRING(gdImageStringTTF, 3) // char*
    T_NUMBER(gdImageStringTTF, 4) // double
    T_NUMBER(gdImageStringTTF, 5) // double
    T_NUMBER(gdImageStringTTF, 6) // int
    T_NUMBER(gdImageStringTTF, 7) // int
    T_STRING(gdImageStringTTF, 8) // char*

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (count != 8) {
        return (void *)"gdImageStringTTF: parameter 2 should be an array with 8 elements";
    }

    int *arr_i = GetIntList(PARAMETER(1), Invoke);

    RETURN_STRING((char *)gdImageStringTTF((gdImage *)(long)PARAM(0), arr_i, (int)PARAM(2), PARAM(3), (double)PARAM(4), (double)PARAM(5), (int)PARAM(6), (int)PARAM(7), PARAM(8)))
    delete[] arr_i;
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringFT, 9)
    T_NUMBER(gdImageStringFT, 0) // gdImage*
    T_ARRAY(gdImageStringFT, 1)  // int*
    T_NUMBER(gdImageStringFT, 2) // int
    T_STRING(gdImageStringFT, 3) // char*
    T_NUMBER(gdImageStringFT, 4) // double
    T_NUMBER(gdImageStringFT, 5) // double
    T_NUMBER(gdImageStringFT, 6) // int
    T_NUMBER(gdImageStringFT, 7) // int
    T_STRING(gdImageStringFT, 8) // char*

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (count != 8) {
        return (void *)"gdImageStringFT: parameter 2 should be an array with 8 elements";
    }

    int *arr_i = GetIntList(PARAMETER(1), Invoke);

    RETURN_STRING((char *)gdImageStringFT((gdImage *)(long)PARAM(0), arr_i, (int)PARAM(2), PARAM(3), (double)PARAM(4), (double)PARAM(5), (int)PARAM(6), (int)PARAM(7), PARAM(8)))
    delete[] arr_i;

END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdFTUseFontConfig, 1)
    T_NUMBER(gdFTUseFontConfig, 0)     // int

    RETURN_NUMBER(gdFTUseFontConfig((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringFTEx, 10)
    T_NUMBER(gdImageStringFTEx, 0) // gdImage*
    T_ARRAY(gdImageStringFTEx, 1)  // int *
    T_NUMBER(gdImageStringFTEx, 2) // int
    T_STRING(gdImageStringFTEx, 3) // char*
    T_NUMBER(gdImageStringFTEx, 4) // double
    T_NUMBER(gdImageStringFTEx, 5) // double
    T_NUMBER(gdImageStringFTEx, 6) // int
    T_NUMBER(gdImageStringFTEx, 7) // int
    T_STRING(gdImageStringFTEx, 8) // char*
    T_NUMBER(gdImageStringFTEx, 9) // gdFTStringExtraPtr

// ... parameter 1 is by reference (int*)
//int local_parameter_1;
    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (count != 8) {
        return (void *)"gdImageStringFTEx: parameter 2 should be an array with 8 elements";
    }

    int *arr_i = GetIntList(PARAMETER(1), Invoke);

    RETURN_STRING((char *)gdImageStringFTEx((gdImage *)(long)PARAM(0), arr_i, (int)PARAM(2), PARAM(3), (double)PARAM(4), (double)PARAM(5), (int)PARAM(6), (int)PARAM(7), PARAM(8), (gdFTStringExtraPtr)(long)PARAM(9)))
    delete[] arr_i;

END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePolygon, 4)
    T_NUMBER(gdImagePolygon, 0)     // gdImagePtr
    T_ARRAY(gdImagePolygon, 1)      // gdPointPtr
    T_NUMBER(gdImagePolygon, 2)     // int
    T_NUMBER(gdImagePolygon, 3)     // int

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (count != PARAM_INT(2) * 2) {
        return (void *)"gdImagePolygon: Actual gdPoint array elements count is different from parameter 3";
    }

    int     *arr_i  = GetIntList(PARAMETER(1), Invoke);
    gdPoint *points = new gdPoint[PARAM_INT(2) + 1];

    int index = 0;

    for (int i = 0; i < count; i += 2) {
        points[index].x   = arr_i[i];
        points[index++].y = arr_i[i + 1];
    }
    delete[] arr_i;


    gdImagePolygon((gdImagePtr)(long)PARAM(0), points, (int)PARAM(2), (int)PARAM(3));
    delete[] points;

    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageOpenPolygon, 4)
    T_NUMBER(gdImageOpenPolygon, 0)     // gdImagePtr
    T_ARRAY(gdImageOpenPolygon, 1)      // gdPointPtr
    T_NUMBER(gdImageOpenPolygon, 2)     // int
    T_NUMBER(gdImageOpenPolygon, 3)     // int

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (count != PARAM_INT(2) * 2) {
        return (void *)"gdImageOpenPolygon: Actual gdPoint array elements count is different from parameter 3";
    }

    int     *arr_i  = GetIntList(PARAMETER(1), Invoke);
    gdPoint *points = new gdPoint[PARAM_INT(2) + 1];

    int index = 0;

    for (int i = 0; i < count; i += 2) {
        points[index].x   = arr_i[i];
        points[index++].y = arr_i[i + 1];
    }
    delete[] arr_i;

    gdImageOpenPolygon((gdImagePtr)(long)PARAM(0), points, (int)PARAM(2), (int)PARAM(3));
    delete[] points;

    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFilledPolygon, 4)
    T_NUMBER(gdImageFilledPolygon, 0)     // gdImagePtr
    T_ARRAY(gdImageFilledPolygon, 1)      // gdPointPtr
    T_NUMBER(gdImageFilledPolygon, 2)     // int
    T_NUMBER(gdImageFilledPolygon, 3)     // int

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (count != PARAM_INT(2) * 2) {
        return (void *)"gdImageFilledPolygon: Actual gdPoint array elements count is different from parameter 3";
    }

    int     *arr_i  = GetIntList(PARAMETER(1), Invoke);
    gdPoint *points = new gdPoint[PARAM_INT(2) + 1];

    int index = 0;

    for (int i = 0; i < count; i += 2) {
        points[index].x   = arr_i[i];
        points[index++].y = arr_i[i + 1];
    }
    delete[] arr_i;

    gdImageFilledPolygon((gdImagePtr)(long)PARAM(0), points, (int)PARAM(2), (int)PARAM(3));
    delete[] points;

    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorAllocate, 4)
    T_NUMBER(gdImageColorAllocate, 0)     // gdImagePtr
    T_NUMBER(gdImageColorAllocate, 1)     // int
    T_NUMBER(gdImageColorAllocate, 2)     // int
    T_NUMBER(gdImageColorAllocate, 3)     // int

    RETURN_NUMBER(gdImageColorAllocate((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorAllocateAlpha, 5)
    T_NUMBER(gdImageColorAllocateAlpha, 0)     // gdImagePtr
    T_NUMBER(gdImageColorAllocateAlpha, 1)     // int
    T_NUMBER(gdImageColorAllocateAlpha, 2)     // int
    T_NUMBER(gdImageColorAllocateAlpha, 3)     // int
    T_NUMBER(gdImageColorAllocateAlpha, 4)     // int

    RETURN_NUMBER(gdImageColorAllocateAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorClosest, 4)
    T_NUMBER(gdImageColorClosest, 0)     // gdImagePtr
    T_NUMBER(gdImageColorClosest, 1)     // int
    T_NUMBER(gdImageColorClosest, 2)     // int
    T_NUMBER(gdImageColorClosest, 3)     // int

    RETURN_NUMBER(gdImageColorClosest((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorClosestAlpha, 5)
    T_NUMBER(gdImageColorClosestAlpha, 0)     // gdImagePtr
    T_NUMBER(gdImageColorClosestAlpha, 1)     // int
    T_NUMBER(gdImageColorClosestAlpha, 2)     // int
    T_NUMBER(gdImageColorClosestAlpha, 3)     // int
    T_NUMBER(gdImageColorClosestAlpha, 4)     // int

    RETURN_NUMBER(gdImageColorClosestAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorClosestHWB, 4)
    T_NUMBER(gdImageColorClosestHWB, 0)     // gdImagePtr
    T_NUMBER(gdImageColorClosestHWB, 1)     // int
    T_NUMBER(gdImageColorClosestHWB, 2)     // int
    T_NUMBER(gdImageColorClosestHWB, 3)     // int

    RETURN_NUMBER(gdImageColorClosestHWB((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorExact, 4)
    T_NUMBER(gdImageColorExact, 0)     // gdImagePtr
    T_NUMBER(gdImageColorExact, 1)     // int
    T_NUMBER(gdImageColorExact, 2)     // int
    T_NUMBER(gdImageColorExact, 3)     // int

    RETURN_NUMBER(gdImageColorExact((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorExactAlpha, 5)
    T_NUMBER(gdImageColorExactAlpha, 0)     // gdImagePtr
    T_NUMBER(gdImageColorExactAlpha, 1)     // int
    T_NUMBER(gdImageColorExactAlpha, 2)     // int
    T_NUMBER(gdImageColorExactAlpha, 3)     // int
    T_NUMBER(gdImageColorExactAlpha, 4)     // int

    RETURN_NUMBER(gdImageColorExactAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorResolve, 4)
    T_NUMBER(gdImageColorResolve, 0)     // gdImagePtr
    T_NUMBER(gdImageColorResolve, 1)     // int
    T_NUMBER(gdImageColorResolve, 2)     // int
    T_NUMBER(gdImageColorResolve, 3)     // int

    RETURN_NUMBER(gdImageColorResolve((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorResolveAlpha, 5)
    T_NUMBER(gdImageColorResolveAlpha, 0)     // gdImagePtr
    T_NUMBER(gdImageColorResolveAlpha, 1)     // int
    T_NUMBER(gdImageColorResolveAlpha, 2)     // int
    T_NUMBER(gdImageColorResolveAlpha, 3)     // int
    T_NUMBER(gdImageColorResolveAlpha, 4)     // int

    RETURN_NUMBER(gdImageColorResolveAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorDeallocate, 2)
    T_NUMBER(gdImageColorDeallocate, 0)     // gdImagePtr
    T_NUMBER(gdImageColorDeallocate, 1)     // int

    gdImageColorDeallocate((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreatePaletteFromTrueColor, 3)
    T_NUMBER(gdImageCreatePaletteFromTrueColor, 0)     // gdImagePtr
    T_NUMBER(gdImageCreatePaletteFromTrueColor, 1)     // int
    T_NUMBER(gdImageCreatePaletteFromTrueColor, 2)     // int

    RETURN_NUMBER((long)gdImageCreatePaletteFromTrueColor((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageTrueColorToPalette, 3)
    T_NUMBER(gdImageTrueColorToPalette, 0)     // gdImagePtr
    T_NUMBER(gdImageTrueColorToPalette, 1)     // int
    T_NUMBER(gdImageTrueColorToPalette, 2)     // int

    gdImageTrueColorToPalette((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorTransparent, 2)
    T_NUMBER(gdImageColorTransparent, 0)     // gdImagePtr
    T_NUMBER(gdImageColorTransparent, 1)     // int

    gdImageColorTransparent((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePaletteCopy, 2)
    T_NUMBER(gdImagePaletteCopy, 0)     // gdImagePtr
    T_NUMBER(gdImagePaletteCopy, 1)     // gdImagePtr

    gdImagePaletteCopy((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGif, 2)
    T_NUMBER(gdImageGif, 0)     // gdImagePtr
    T_NUMBER(gdImageGif, 1)     // FILE*

    gdImageGif((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePng, 2)
    T_NUMBER(gdImagePng, 0)     // gdImagePtr
    T_NUMBER(gdImagePng, 1)     // FILE*

    gdImagePng((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngCtx, 2)
    T_NUMBER(gdImagePngCtx, 0)     // gdImagePtr
    T_NUMBER(gdImagePngCtx, 1)     // gdIOCtx*

    gdImagePngCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifCtx, 2)
    T_NUMBER(gdImageGifCtx, 0)     // gdImagePtr
    T_NUMBER(gdImageGifCtx, 1)     // gdIOCtx*

    gdImageGifCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngEx, 3)
    T_NUMBER(gdImagePngEx, 0)     // gdImagePtr
    T_NUMBER(gdImagePngEx, 1)     // FILE*
    T_NUMBER(gdImagePngEx, 2)     // int

    gdImagePngEx((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngCtxEx, 3)
    T_NUMBER(gdImagePngCtxEx, 0)     // gdImagePtr
    T_NUMBER(gdImagePngCtxEx, 1)     // gdIOCtx*
    T_NUMBER(gdImagePngCtxEx, 2)     // int

    gdImagePngCtxEx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageWBMP, 3)
    T_NUMBER(gdImageWBMP, 0)     // gdImagePtr
    T_NUMBER(gdImageWBMP, 1)     // int
    T_NUMBER(gdImageWBMP, 2)     // FILE*

    gdImageWBMP((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (FILE *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageWBMPCtx, 3)
    T_NUMBER(gdImageWBMPCtx, 0)     // gdImagePtr
    T_NUMBER(gdImageWBMPCtx, 1)     // int
    T_NUMBER(gdImageWBMPCtx, 2)     // gdIOCtx*

    gdImageWBMPCtx((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (gdIOCtx *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdFree, 1)
    T_NUMBER(gdFree, 0)     // void*

    gdFree((void *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageWBMPPtr, 3)
    T_NUMBER(gdImageWBMPPtr, 0)     // gdImagePtr
    T_NUMBER(gdImageWBMPPtr, 2)     // int

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageWBMPPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1, (int)PARAM(2)))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageJpeg, 3)
    T_NUMBER(gdImageJpeg, 0)     // gdImagePtr
    T_NUMBER(gdImageJpeg, 1)     // FILE*
    T_NUMBER(gdImageJpeg, 2)     // int

    gdImageJpeg((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageJpegCtx, 3)
    T_NUMBER(gdImageJpegCtx, 0)     // gdImagePtr
    T_NUMBER(gdImageJpegCtx, 1)     // gdIOCtx*
    T_NUMBER(gdImageJpegCtx, 2)     // int

    gdImageJpegCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageJpegPtr, 3)
    T_NUMBER(gdImageJpegPtr, 0)     // gdImagePtr
    T_NUMBER(gdImageJpegPtr, 2)     // int

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageJpegPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1, (int)PARAM(2)))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimBegin, 4)
    T_NUMBER(gdImageGifAnimBegin, 0)     // gdImagePtr
    T_NUMBER(gdImageGifAnimBegin, 1)     // FILE*
    T_NUMBER(gdImageGifAnimBegin, 2)     // int
    T_NUMBER(gdImageGifAnimBegin, 3)     // int

    gdImageGifAnimBegin((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimAdd, 8)
    T_NUMBER(gdImageGifAnimAdd, 0)     // gdImagePtr
    T_NUMBER(gdImageGifAnimAdd, 1)     // FILE*
    T_NUMBER(gdImageGifAnimAdd, 2)     // int
    T_NUMBER(gdImageGifAnimAdd, 3)     // int
    T_NUMBER(gdImageGifAnimAdd, 4)     // int
    T_NUMBER(gdImageGifAnimAdd, 5)     // int
    T_NUMBER(gdImageGifAnimAdd, 6)     // int
    T_NUMBER(gdImageGifAnimAdd, 7)     // gdImagePtr

    gdImageGifAnimAdd((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (gdImagePtr)(long)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimEnd, 1)
    T_NUMBER(gdImageGifAnimEnd, 0)     // FILE*

    gdImageGifAnimEnd((FILE *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimBeginCtx, 4)
    T_NUMBER(gdImageGifAnimBeginCtx, 0)     // gdImagePtr
    T_NUMBER(gdImageGifAnimBeginCtx, 1)     // gdIOCtx*
    T_NUMBER(gdImageGifAnimBeginCtx, 2)     // int
    T_NUMBER(gdImageGifAnimBeginCtx, 3)     // int

    gdImageGifAnimBeginCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimAddCtx, 8)
    T_NUMBER(gdImageGifAnimAddCtx, 0)     // gdImagePtr
    T_NUMBER(gdImageGifAnimAddCtx, 1)     // gdIOCtx*
    T_NUMBER(gdImageGifAnimAddCtx, 2)     // int
    T_NUMBER(gdImageGifAnimAddCtx, 3)     // int
    T_NUMBER(gdImageGifAnimAddCtx, 4)     // int
    T_NUMBER(gdImageGifAnimAddCtx, 5)     // int
    T_NUMBER(gdImageGifAnimAddCtx, 6)     // int
    T_NUMBER(gdImageGifAnimAddCtx, 7)     // gdImagePtr

    gdImageGifAnimAddCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (gdImagePtr)(long)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimEndCtx, 1)
    T_NUMBER(gdImageGifAnimEndCtx, 0)     // gdIOCtx*

    gdImageGifAnimEndCtx((gdIOCtx *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimBeginPtr, 4)
    T_NUMBER(gdImageGifAnimBeginPtr, 0)     // gdImagePtr
    T_NUMBER(gdImageGifAnimBeginPtr, 2)     // int
    T_NUMBER(gdImageGifAnimBeginPtr, 3)     // int

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageGifAnimBeginPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1, (int)PARAM(2), (int)PARAM(3)))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimAddPtr, 8)
    T_NUMBER(gdImageGifAnimAddPtr, 0)     // gdImagePtr
    T_NUMBER(gdImageGifAnimAddPtr, 2)     // int
    T_NUMBER(gdImageGifAnimAddPtr, 3)     // int
    T_NUMBER(gdImageGifAnimAddPtr, 4)     // int
    T_NUMBER(gdImageGifAnimAddPtr, 5)     // int
    T_NUMBER(gdImageGifAnimAddPtr, 6)     // int
    T_NUMBER(gdImageGifAnimAddPtr, 7)     // gdImagePtr

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageGifAnimAddPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1, (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (gdImagePtr)(long)PARAM(7)))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimEndPtr, 1)

// ... parameter 0 is by reference (int*)
    int local_parameter_0;

    RETURN_NUMBER((long)gdImageGifAnimEndPtr(&local_parameter_0))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngToSink, 2)
    T_NUMBER(gdImagePngToSink, 0)     // gdImagePtr
    T_NUMBER(gdImagePngToSink, 1)     // gdSinkPtr

    gdImagePngToSink((gdImagePtr)(long)PARAM(0), (gdSinkPtr)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGd, 2)
    T_NUMBER(gdImageGd, 0)     // gdImagePtr
    T_NUMBER(gdImageGd, 1)     // FILE*

    gdImageGd((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGd2, 4)
    T_NUMBER(gdImageGd2, 0)     // gdImagePtr
    T_NUMBER(gdImageGd2, 1)     // FILE*
    T_NUMBER(gdImageGd2, 2)     // int
    T_NUMBER(gdImageGd2, 3)     // int

    gdImageGd2((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifPtr, 2)
    T_NUMBER(gdImageGifPtr, 0)     // gdImagePtr

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageGifPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngPtr, 2)
    T_NUMBER(gdImagePngPtr, 0)     // gdImagePtr

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImagePngPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngPtrEx, 3)
    T_NUMBER(gdImagePngPtrEx, 0)     // gdImagePtr
    T_NUMBER(gdImagePngPtrEx, 2)     // int

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImagePngPtrEx((gdImagePtr)(long)PARAM(0), &local_parameter_1, (int)PARAM(2)))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGdPtr, 2)
    T_NUMBER(gdImageGdPtr, 0)     // gdImagePtr

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageGdPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGd2Ptr, 4)
    T_NUMBER(gdImageGd2Ptr, 0)     // gdImagePtr
    T_NUMBER(gdImageGd2Ptr, 1)     // int
    T_NUMBER(gdImageGd2Ptr, 2)     // int

// ... parameter 3 is by reference (int*)
    int local_parameter_3;

    RETURN_NUMBER((long)gdImageGd2Ptr((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFilledArc, 9)
    T_NUMBER(gdImageFilledArc, 0)     // gdImagePtr
    T_NUMBER(gdImageFilledArc, 1)     // int
    T_NUMBER(gdImageFilledArc, 2)     // int
    T_NUMBER(gdImageFilledArc, 3)     // int
    T_NUMBER(gdImageFilledArc, 4)     // int
    T_NUMBER(gdImageFilledArc, 5)     // int
    T_NUMBER(gdImageFilledArc, 6)     // int
    T_NUMBER(gdImageFilledArc, 7)     // int
    T_NUMBER(gdImageFilledArc, 8)     // int

    gdImageFilledArc((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageArc, 8)
    T_NUMBER(gdImageArc, 0)     // gdImagePtr
    T_NUMBER(gdImageArc, 1)     // int
    T_NUMBER(gdImageArc, 2)     // int
    T_NUMBER(gdImageArc, 3)     // int
    T_NUMBER(gdImageArc, 4)     // int
    T_NUMBER(gdImageArc, 5)     // int
    T_NUMBER(gdImageArc, 6)     // int
    T_NUMBER(gdImageArc, 7)     // int

    gdImageArc((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFilledEllipse, 6)
    T_NUMBER(gdImageFilledEllipse, 0)     // gdImagePtr
    T_NUMBER(gdImageFilledEllipse, 1)     // int
    T_NUMBER(gdImageFilledEllipse, 2)     // int
    T_NUMBER(gdImageFilledEllipse, 3)     // int
    T_NUMBER(gdImageFilledEllipse, 4)     // int
    T_NUMBER(gdImageFilledEllipse, 5)     // int

    gdImageFilledEllipse((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFillToBorder, 5)
    T_NUMBER(gdImageFillToBorder, 0)     // gdImagePtr
    T_NUMBER(gdImageFillToBorder, 1)     // int
    T_NUMBER(gdImageFillToBorder, 2)     // int
    T_NUMBER(gdImageFillToBorder, 3)     // int
    T_NUMBER(gdImageFillToBorder, 4)     // int

    gdImageFillToBorder((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFill, 4)
    T_NUMBER(gdImageFill, 0)     // gdImagePtr
    T_NUMBER(gdImageFill, 1)     // int
    T_NUMBER(gdImageFill, 2)     // int
    T_NUMBER(gdImageFill, 3)     // int

    gdImageFill((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopy, 8)
    T_NUMBER(gdImageCopy, 0)     // gdImagePtr
    T_NUMBER(gdImageCopy, 1)     // gdImagePtr
    T_NUMBER(gdImageCopy, 2)     // int
    T_NUMBER(gdImageCopy, 3)     // int
    T_NUMBER(gdImageCopy, 4)     // int
    T_NUMBER(gdImageCopy, 5)     // int
    T_NUMBER(gdImageCopy, 6)     // int
    T_NUMBER(gdImageCopy, 7)     // int

    gdImageCopy((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyMerge, 9)
    T_NUMBER(gdImageCopyMerge, 0)     // gdImagePtr
    T_NUMBER(gdImageCopyMerge, 1)     // gdImagePtr
    T_NUMBER(gdImageCopyMerge, 2)     // int
    T_NUMBER(gdImageCopyMerge, 3)     // int
    T_NUMBER(gdImageCopyMerge, 4)     // int
    T_NUMBER(gdImageCopyMerge, 5)     // int
    T_NUMBER(gdImageCopyMerge, 6)     // int
    T_NUMBER(gdImageCopyMerge, 7)     // int
    T_NUMBER(gdImageCopyMerge, 8)     // int

    gdImageCopyMerge((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyMergeGray, 9)
    T_NUMBER(gdImageCopyMergeGray, 0)     // gdImagePtr
    T_NUMBER(gdImageCopyMergeGray, 1)     // gdImagePtr
    T_NUMBER(gdImageCopyMergeGray, 2)     // int
    T_NUMBER(gdImageCopyMergeGray, 3)     // int
    T_NUMBER(gdImageCopyMergeGray, 4)     // int
    T_NUMBER(gdImageCopyMergeGray, 5)     // int
    T_NUMBER(gdImageCopyMergeGray, 6)     // int
    T_NUMBER(gdImageCopyMergeGray, 7)     // int
    T_NUMBER(gdImageCopyMergeGray, 8)     // int

    gdImageCopyMergeGray((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyResized, 10)
    T_NUMBER(gdImageCopyResized, 0)     // gdImagePtr
    T_NUMBER(gdImageCopyResized, 1)     // gdImagePtr
    T_NUMBER(gdImageCopyResized, 2)     // int
    T_NUMBER(gdImageCopyResized, 3)     // int
    T_NUMBER(gdImageCopyResized, 4)     // int
    T_NUMBER(gdImageCopyResized, 5)     // int
    T_NUMBER(gdImageCopyResized, 6)     // int
    T_NUMBER(gdImageCopyResized, 7)     // int
    T_NUMBER(gdImageCopyResized, 8)     // int
    T_NUMBER(gdImageCopyResized, 9)     // int

    gdImageCopyResized((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8), (int)PARAM(9));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyResampled, 10)
    T_NUMBER(gdImageCopyResampled, 0)     // gdImagePtr
    T_NUMBER(gdImageCopyResampled, 1)     // gdImagePtr
    T_NUMBER(gdImageCopyResampled, 2)     // int
    T_NUMBER(gdImageCopyResampled, 3)     // int
    T_NUMBER(gdImageCopyResampled, 4)     // int
    T_NUMBER(gdImageCopyResampled, 5)     // int
    T_NUMBER(gdImageCopyResampled, 6)     // int
    T_NUMBER(gdImageCopyResampled, 7)     // int
    T_NUMBER(gdImageCopyResampled, 8)     // int
    T_NUMBER(gdImageCopyResampled, 9)     // int

    gdImageCopyResampled((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8), (int)PARAM(9));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyRotated, 9)
    T_NUMBER(gdImageCopyRotated, 0)     // gdImagePtr
    T_NUMBER(gdImageCopyRotated, 1)     // gdImagePtr
    T_NUMBER(gdImageCopyRotated, 2)     // double
    T_NUMBER(gdImageCopyRotated, 3)     // double
    T_NUMBER(gdImageCopyRotated, 4)     // int
    T_NUMBER(gdImageCopyRotated, 5)     // int
    T_NUMBER(gdImageCopyRotated, 6)     // int
    T_NUMBER(gdImageCopyRotated, 7)     // int
    T_NUMBER(gdImageCopyRotated, 8)     // int

    gdImageCopyRotated((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (double)PARAM(2), (double)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetBrush, 2)
    T_NUMBER(gdImageSetBrush, 0)     // gdImagePtr
    T_NUMBER(gdImageSetBrush, 1)     // gdImagePtr

    gdImageSetBrush((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetTile, 2)
    T_NUMBER(gdImageSetTile, 0)     // gdImagePtr
    T_NUMBER(gdImageSetTile, 1)     // gdImagePtr

    gdImageSetTile((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetAntiAliased, 2)
    T_NUMBER(gdImageSetAntiAliased, 0)     // gdImagePtr
    T_NUMBER(gdImageSetAntiAliased, 1)     // int

    gdImageSetAntiAliased((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetAntiAliasedDontBlend, 3)
    T_NUMBER(gdImageSetAntiAliasedDontBlend, 0)     // gdImagePtr
    T_NUMBER(gdImageSetAntiAliasedDontBlend, 1)     // int
    T_NUMBER(gdImageSetAntiAliasedDontBlend, 2)     // int

    gdImageSetAntiAliasedDontBlend((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetStyle, 3)
    T_NUMBER(gdImageSetStyle, 0) // gdImagePtr
    T_ARRAY(gdImageSetStyle, 1)  // int*
    T_NUMBER(gdImageSetStyle, 2) // int

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (count != PARAM_INT(2)) {
        return (void *)"gdImageSetStyle: given style array length differs from the length parameter";
    }

    int *arr_i = GetIntList(PARAMETER(1), Invoke);


    gdImageSetStyle((gdImagePtr)(long)PARAM(0), arr_i, (int)PARAM(2));
    delete[] arr_i;
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetThickness, 2)
    T_NUMBER(gdImageSetThickness, 0)     // gdImagePtr
    T_NUMBER(gdImageSetThickness, 1)     // int

    gdImageSetThickness((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageInterlace, 2)
    T_NUMBER(gdImageInterlace, 0)     // gdImagePtr
    T_NUMBER(gdImageInterlace, 1)     // int

    gdImageInterlace((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageAlphaBlending, 2)
    T_NUMBER(gdImageAlphaBlending, 0)     // gdImagePtr
    T_NUMBER(gdImageAlphaBlending, 1)     // int

    gdImageAlphaBlending((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSaveAlpha, 2)
    T_NUMBER(gdImageSaveAlpha, 0)     // gdImagePtr
    T_NUMBER(gdImageSaveAlpha, 1)     // int

    gdImageSaveAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdNewFileCtx, 1)
    T_NUMBER(gdNewFileCtx, 0)     // FILE*

    RETURN_NUMBER((long)gdNewFileCtx((FILE *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdNewDynamicCtx, 2)
    T_NUMBER(gdNewDynamicCtx, 0)     // int
    T_NUMBER(gdNewDynamicCtx, 1)     // void*

    RETURN_NUMBER((long)gdNewDynamicCtx((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdNewDynamicCtxEx, 3)
    T_NUMBER(gdNewDynamicCtxEx, 0)     // int
    T_NUMBER(gdNewDynamicCtxEx, 1)     // void*
    T_NUMBER(gdNewDynamicCtxEx, 2)     // int

    RETURN_NUMBER((long)gdNewDynamicCtxEx((int)PARAM(0), (void *)(long)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdNewSSCtx, 2)
    T_NUMBER(gdNewSSCtx, 0)     // gdSourcePtr
    T_NUMBER(gdNewSSCtx, 1)     // gdSinkPtr

    RETURN_NUMBER((long)gdNewSSCtx((gdSourcePtr)(long)PARAM(0), (gdSinkPtr)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSquareToCircle, 2)
    T_NUMBER(gdImageSquareToCircle, 0)     // gdImagePtr
    T_NUMBER(gdImageSquareToCircle, 1)     // int

    RETURN_NUMBER((long)gdImageSquareToCircle((gdImagePtr)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringFTCircle, 11)
    T_NUMBER(gdImageStringFTCircle, 0)     // gdImagePtr
    T_NUMBER(gdImageStringFTCircle, 1)     // int
    T_NUMBER(gdImageStringFTCircle, 2)     // int
    T_NUMBER(gdImageStringFTCircle, 3)     // double
    T_NUMBER(gdImageStringFTCircle, 4)     // double
    T_NUMBER(gdImageStringFTCircle, 5)     // double
    T_STRING(gdImageStringFTCircle, 6)     // char*
    T_NUMBER(gdImageStringFTCircle, 7)     // double
    T_STRING(gdImageStringFTCircle, 8)     // char*
    T_STRING(gdImageStringFTCircle, 9)     // char*
    T_NUMBER(gdImageStringFTCircle, 10)    // int

    RETURN_STRING((char *)gdImageStringFTCircle((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5), PARAM(6), (double)PARAM(7), PARAM(8), PARAM(9), (int)PARAM(10)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSharpen, 2)
    T_NUMBER(gdImageSharpen, 0)     // gdImagePtr
    T_NUMBER(gdImageSharpen, 1)     // int

    gdImageSharpen((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorGetAlpha, 1)
    T_NUMBER(gdTrueColorGetAlpha, 0)

    RETURN_NUMBER(gdTrueColorGetAlpha((PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorGetRed, 1)
    T_NUMBER(gdTrueColorGetRed, 0)

    RETURN_NUMBER(gdTrueColorGetRed((PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorGetGreen, 1)
    T_NUMBER(gdTrueColorGetGreen, 0)

    RETURN_NUMBER(gdTrueColorGetGreen((PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorGetBlue, 1)
    T_NUMBER(gdTrueColorGetBlue, 0)

    RETURN_NUMBER(gdTrueColorGetBlue((PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColor, 3)
    T_NUMBER(gdTrueColor, 0)
    T_NUMBER(gdTrueColor, 1)
    T_NUMBER(gdTrueColor, 2)

    RETURN_NUMBER(gdTrueColor((PARAM_INT(0)), (PARAM_INT(1)), (PARAM_INT(2))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorAlpha, 4)
    T_NUMBER(gdTrueColorAlpha, 0)
    T_NUMBER(gdTrueColorAlpha, 1)
    T_NUMBER(gdTrueColorAlpha, 2)
    T_NUMBER(gdTrueColorAlpha, 3)

    RETURN_NUMBER(gdTrueColorAlpha((PARAM_INT(0)), (PARAM_INT(1)), (PARAM_INT(2)), (PARAM_INT(3))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageTrueColor, 1)
    T_NUMBER(gdImageTrueColor, 0)

    RETURN_NUMBER(gdImageTrueColor(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSX, 1)
    T_NUMBER(gdImageSX, 0)

    RETURN_NUMBER(gdImageSX(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSY, 1)
    T_NUMBER(gdImageSY, 0)

    RETURN_NUMBER(gdImageSY(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorsTotal, 1)
    T_NUMBER(gdImageColorsTotal, 0)

    RETURN_NUMBER(gdImageColorsTotal(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageRed, 2)
    T_NUMBER(gdImageRed, 0)
    T_NUMBER(gdImageRed, 1)

    RETURN_NUMBER(gdImageRed(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGreen, 2)
    T_NUMBER(gdImageGreen, 0)
    T_NUMBER(gdImageGreen, 1)

    RETURN_NUMBER(gdImageGreen(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageBlue, 2)
    T_NUMBER(gdImageBlue, 0)
    T_NUMBER(gdImageBlue, 1)

    RETURN_NUMBER(gdImageBlue(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageAlpha, 2)
    T_NUMBER(gdImageAlpha, 0)
    T_NUMBER(gdImageAlpha, 1)

    RETURN_NUMBER(gdImageAlpha(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetTransparent, 1)
    T_NUMBER(gdImageGetTransparent, 0)

    RETURN_NUMBER(gdImageGetTransparent(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetInterlaced, 1)
    T_NUMBER(gdImageGetInterlaced, 0)

    RETURN_NUMBER(gdImageGetInterlaced(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePalettePixel, 3)
    T_NUMBER(gdImagePalettePixel, 0)
    T_NUMBER(gdImagePalettePixel, 1)
    T_NUMBER(gdImagePalettePixel, 2)

    RETURN_NUMBER(gdImagePalettePixel(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1)), (PARAM_INT(2))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageTrueColorPixel, 3)
    T_NUMBER(gdImageTrueColorPixel, 0)
    T_NUMBER(gdImageTrueColorPixel, 1)
    T_NUMBER(gdImageTrueColorPixel, 2)

    RETURN_NUMBER(gdImageTrueColorPixel(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1)), (PARAM_INT(2))))
END_IMPL
//------------------------------------------------------------------------

