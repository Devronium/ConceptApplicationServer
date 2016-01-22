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
    T_NUMBER(0)     // int
    T_NUMBER(1)     // int

    RETURN_NUMBER(gdAlphaBlend((int)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreate, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // int

    RETURN_NUMBER((long)gdImageCreate((int)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateTrueColor, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // int

    RETURN_NUMBER((long)gdImageCreateTrueColor((int)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromPng, 1)
    T_NUMBER(0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromPng((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromPngCtx, 1)
    T_NUMBER(0)     // gdIOCtxPtr

    RETURN_NUMBER((long)gdImageCreateFromPngCtx((gdIOCtxPtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromPngPtr, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromPngPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGif, 1)
    T_NUMBER(0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromGif((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGifCtx, 1)
    T_NUMBER(0)     // gdIOCtxPtr

    RETURN_NUMBER((long)gdImageCreateFromGifCtx((gdIOCtxPtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGifPtr, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromGifPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromWBMP, 1)
    T_NUMBER(0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromWBMP((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromWBMPCtx, 1)
    T_NUMBER(0)     // gdIOCtx*

    RETURN_NUMBER((long)gdImageCreateFromWBMPCtx((gdIOCtx *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromWBMPPtr, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromWBMPPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromJpeg, 1)
    T_NUMBER(0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromJpeg((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromJpegCtx, 1)
    T_NUMBER(0)     // gdIOCtx*

    RETURN_NUMBER((long)gdImageCreateFromJpegCtx((gdIOCtx *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromJpegPtr, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromJpegPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromPngSource, 1)
    T_NUMBER(0)     // gdSourcePtr

    RETURN_NUMBER((long)gdImageCreateFromPngSource((gdSourcePtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd, 1)
    T_NUMBER(0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromGd((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGdCtx, 1)
    T_NUMBER(0)     // gdIOCtxPtr

    RETURN_NUMBER((long)gdImageCreateFromGdCtx((gdIOCtxPtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGdPtr, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromGdPtr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2, 1)
    T_NUMBER(0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromGd2((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2Ctx, 1)
    T_NUMBER(0)     // gdIOCtxPtr

    RETURN_NUMBER((long)gdImageCreateFromGd2Ctx((gdIOCtxPtr)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2Ptr, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*

    RETURN_NUMBER((long)gdImageCreateFromGd2Ptr((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2Part, 5)
    T_NUMBER(0)     // FILE*
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int

    RETURN_NUMBER((long)gdImageCreateFromGd2Part((FILE *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2PartCtx, 5)
    T_NUMBER(0)     // gdIOCtxPtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int

    RETURN_NUMBER((long)gdImageCreateFromGd2PartCtx((gdIOCtxPtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromGd2PartPtr, 6)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int

    RETURN_NUMBER((long)gdImageCreateFromGd2PartPtr((int)PARAM(0), (void *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromXbm, 1)
    T_NUMBER(0)     // FILE*

    RETURN_NUMBER((long)gdImageCreateFromXbm((FILE *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreateFromXpm, 1)
    T_STRING(0)     // char*

    RETURN_NUMBER((long)gdImageCreateFromXpm(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageDestroy, 1)
    T_NUMBER(0)     // gdImagePtr

    gdImageDestroy((gdImagePtr)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetPixel, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    gdImageSetPixel((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetPixel, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int

    RETURN_NUMBER(gdImageGetPixel((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetTrueColorPixel, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int

    RETURN_NUMBER(gdImageGetTrueColorPixel((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageAABlend, 1)
    T_NUMBER(0)     // gdImagePtr

    gdImageAABlend((gdImagePtr)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageLine, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int

    gdImageLine((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageDashedLine, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int

    gdImageDashedLine((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageRectangle, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int

    gdImageRectangle((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFilledRectangle, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int

    gdImageFilledRectangle((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetClip, 5)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int

    gdImageSetClip((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetClip, 5)
    T_NUMBER(0)     // gdImagePtr

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
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int

    RETURN_NUMBER(gdImageBoundsSafe((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageChar, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdFontPtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int

    gdImageChar((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCharUp, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdFontPtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int

    gdImageCharUp((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageString, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdFontPtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_STRING(4)     // unsigned char
    T_NUMBER(5)     // int

    gdImageString((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (unsigned char *)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringUp, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdFontPtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_STRING(4)     // unsigned string
    T_NUMBER(5)     // int

    gdImageStringUp((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (unsigned char *)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageString16, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdFontPtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_STRING(4)     // unsigned short
    T_NUMBER(5)     // int

    gdImageString16((gdImagePtr)(long)PARAM(0), (gdFontPtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (unsigned short *)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringUp16, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdFontPtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_STRING(4)     // unsigned short
    T_NUMBER(5)     // int

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
    T_NUMBER(0) // gdImage*
    T_ARRAY(1)  // int*
    T_NUMBER(2) // int
    T_STRING(3) // char*
    T_NUMBER(4) // double
    T_NUMBER(5) // double
    T_NUMBER(6) // int
    T_NUMBER(7) // int
    T_STRING(8) // char*

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
    T_NUMBER(0) // gdImage*
    T_ARRAY(1)  // int*
    T_NUMBER(2) // int
    T_STRING(3) // char*
    T_NUMBER(4) // double
    T_NUMBER(5) // double
    T_NUMBER(6) // int
    T_NUMBER(7) // int
    T_STRING(8) // char*

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
    T_NUMBER(0)     // int

    RETURN_NUMBER(gdFTUseFontConfig((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringFTEx, 10)
    T_NUMBER(0) // gdImage*
    T_ARRAY(1)  // int *
    T_NUMBER(2) // int
    T_STRING(3) // char*
    T_NUMBER(4) // double
    T_NUMBER(5) // double
    T_NUMBER(6) // int
    T_NUMBER(7) // int
    T_STRING(8) // char*
    T_NUMBER(9) // gdFTStringExtraPtr

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
    T_NUMBER(0)     // gdImagePtr
    T_ARRAY(1)      // gdPointPtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

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
    T_NUMBER(0)     // gdImagePtr
    T_ARRAY(1)      // gdPointPtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

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
    T_NUMBER(0)     // gdImagePtr
    T_ARRAY(1)      // gdPointPtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

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
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    RETURN_NUMBER(gdImageColorAllocate((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorAllocateAlpha, 5)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int

    RETURN_NUMBER(gdImageColorAllocateAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorClosest, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    RETURN_NUMBER(gdImageColorClosest((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorClosestAlpha, 5)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int

    RETURN_NUMBER(gdImageColorClosestAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorClosestHWB, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    RETURN_NUMBER(gdImageColorClosestHWB((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorExact, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    RETURN_NUMBER(gdImageColorExact((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorExactAlpha, 5)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int

    RETURN_NUMBER(gdImageColorExactAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorResolve, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    RETURN_NUMBER(gdImageColorResolve((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorResolveAlpha, 5)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int

    RETURN_NUMBER(gdImageColorResolveAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorDeallocate, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int

    gdImageColorDeallocate((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCreatePaletteFromTrueColor, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int

    RETURN_NUMBER((long)gdImageCreatePaletteFromTrueColor((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageTrueColorToPalette, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int

    gdImageTrueColorToPalette((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorTransparent, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int

    gdImageColorTransparent((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePaletteCopy, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdImagePtr

    gdImagePaletteCopy((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGif, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // FILE*

    gdImageGif((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePng, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // FILE*

    gdImagePng((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngCtx, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdIOCtx*

    gdImagePngCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifCtx, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdIOCtx*

    gdImageGifCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngEx, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // FILE*
    T_NUMBER(2)     // int

    gdImagePngEx((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngCtxEx, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdIOCtx*
    T_NUMBER(2)     // int

    gdImagePngCtxEx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageWBMP, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // FILE*

    gdImageWBMP((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (FILE *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageWBMPCtx, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // gdIOCtx*

    gdImageWBMPCtx((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (gdIOCtx *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdFree, 1)
    T_NUMBER(0)     // void*

    gdFree((void *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageWBMPPtr, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(2)     // int

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageWBMPPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1, (int)PARAM(2)))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageJpeg, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // FILE*
    T_NUMBER(2)     // int

    gdImageJpeg((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageJpegCtx, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdIOCtx*
    T_NUMBER(2)     // int

    gdImageJpegCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageJpegPtr, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(2)     // int

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageJpegPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1, (int)PARAM(2)))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimBegin, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // FILE*
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    gdImageGifAnimBegin((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimAdd, 8)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // FILE*
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // gdImagePtr

    gdImageGifAnimAdd((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (gdImagePtr)(long)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimEnd, 1)
    T_NUMBER(0)     // FILE*

    gdImageGifAnimEnd((FILE *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimBeginCtx, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdIOCtx*
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    gdImageGifAnimBeginCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimAddCtx, 8)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdIOCtx*
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // gdImagePtr

    gdImageGifAnimAddCtx((gdImagePtr)(long)PARAM(0), (gdIOCtx *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (gdImagePtr)(long)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimEndCtx, 1)
    T_NUMBER(0)     // gdIOCtx*

    gdImageGifAnimEndCtx((gdIOCtx *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimBeginPtr, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageGifAnimBeginPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1, (int)PARAM(2), (int)PARAM(3)))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifAnimAddPtr, 8)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // gdImagePtr

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
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdSinkPtr

    gdImagePngToSink((gdImagePtr)(long)PARAM(0), (gdSinkPtr)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGd, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // FILE*

    gdImageGd((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGd2, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // FILE*
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    gdImageGd2((gdImagePtr)(long)PARAM(0), (FILE *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGifPtr, 2)
    T_NUMBER(0)     // gdImagePtr

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageGifPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngPtr, 2)
    T_NUMBER(0)     // gdImagePtr

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImagePngPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePngPtrEx, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(2)     // int

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImagePngPtrEx((gdImagePtr)(long)PARAM(0), &local_parameter_1, (int)PARAM(2)))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGdPtr, 2)
    T_NUMBER(0)     // gdImagePtr

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER((long)gdImageGdPtr((gdImagePtr)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGd2Ptr, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int

// ... parameter 3 is by reference (int*)
    int local_parameter_3;

    RETURN_NUMBER((long)gdImageGd2Ptr((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFilledArc, 9)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // int
    T_NUMBER(8)     // int

    gdImageFilledArc((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageArc, 8)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // int

    gdImageArc((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFilledEllipse, 6)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int

    gdImageFilledEllipse((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFillToBorder, 5)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int

    gdImageFillToBorder((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageFill, 4)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    gdImageFill((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopy, 8)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdImagePtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // int

    gdImageCopy((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyMerge, 9)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdImagePtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // int
    T_NUMBER(8)     // int

    gdImageCopyMerge((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyMergeGray, 9)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdImagePtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // int
    T_NUMBER(8)     // int

    gdImageCopyMergeGray((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyResized, 10)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdImagePtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // int
    T_NUMBER(8)     // int
    T_NUMBER(9)     // int

    gdImageCopyResized((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8), (int)PARAM(9));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyResampled, 10)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdImagePtr
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // int
    T_NUMBER(8)     // int
    T_NUMBER(9)     // int

    gdImageCopyResampled((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8), (int)PARAM(9));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageCopyRotated, 9)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdImagePtr
    T_NUMBER(2)     // double
    T_NUMBER(3)     // double
    T_NUMBER(4)     // int
    T_NUMBER(5)     // int
    T_NUMBER(6)     // int
    T_NUMBER(7)     // int
    T_NUMBER(8)     // int

    gdImageCopyRotated((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1), (double)PARAM(2), (double)PARAM(3), (int)PARAM(4), (int)PARAM(5), (int)PARAM(6), (int)PARAM(7), (int)PARAM(8));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetBrush, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdImagePtr

    gdImageSetBrush((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetTile, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // gdImagePtr

    gdImageSetTile((gdImagePtr)(long)PARAM(0), (gdImagePtr)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetAntiAliased, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int

    gdImageSetAntiAliased((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetAntiAliasedDontBlend, 3)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int

    gdImageSetAntiAliasedDontBlend((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSetStyle, 3)
    T_NUMBER(0) // gdImagePtr
    T_ARRAY(1)  // int*
    T_NUMBER(2) // int

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
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int

    gdImageSetThickness((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageInterlace, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int

    gdImageInterlace((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageAlphaBlending, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int

    gdImageAlphaBlending((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSaveAlpha, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int

    gdImageSaveAlpha((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdNewFileCtx, 1)
    T_NUMBER(0)     // FILE*

    RETURN_NUMBER((long)gdNewFileCtx((FILE *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdNewDynamicCtx, 2)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*

    RETURN_NUMBER((long)gdNewDynamicCtx((int)PARAM(0), (void *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdNewDynamicCtxEx, 3)
    T_NUMBER(0)     // int
    T_NUMBER(1)     // void*
    T_NUMBER(2)     // int

    RETURN_NUMBER((long)gdNewDynamicCtxEx((int)PARAM(0), (void *)(long)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdNewSSCtx, 2)
    T_NUMBER(0)     // gdSourcePtr
    T_NUMBER(1)     // gdSinkPtr

    RETURN_NUMBER((long)gdNewSSCtx((gdSourcePtr)(long)PARAM(0), (gdSinkPtr)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSquareToCircle, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int

    RETURN_NUMBER((long)gdImageSquareToCircle((gdImagePtr)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageStringFTCircle, 11)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // double
    T_NUMBER(4)     // double
    T_NUMBER(5)     // double
    T_STRING(6)     // char*
    T_NUMBER(7)     // double
    T_STRING(8)     // char*
    T_STRING(9)     // char*
    T_NUMBER(10)    // int

    RETURN_STRING((char *)gdImageStringFTCircle((gdImagePtr)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5), PARAM(6), (double)PARAM(7), PARAM(8), PARAM(9), (int)PARAM(10)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSharpen, 2)
    T_NUMBER(0)     // gdImagePtr
    T_NUMBER(1)     // int

    gdImageSharpen((gdImagePtr)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorGetAlpha, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdTrueColorGetAlpha((PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorGetRed, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdTrueColorGetRed((PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorGetGreen, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdTrueColorGetGreen((PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorGetBlue, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdTrueColorGetBlue((PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColor, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)

    RETURN_NUMBER(gdTrueColor((PARAM_INT(0)), (PARAM_INT(1)), (PARAM_INT(2))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdTrueColorAlpha, 4)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)
    T_NUMBER(3)

    RETURN_NUMBER(gdTrueColorAlpha((PARAM_INT(0)), (PARAM_INT(1)), (PARAM_INT(2)), (PARAM_INT(3))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageTrueColor, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdImageTrueColor(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSX, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdImageSX(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageSY, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdImageSY(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageColorsTotal, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdImageColorsTotal(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageRed, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    RETURN_NUMBER(gdImageRed(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGreen, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    RETURN_NUMBER(gdImageGreen(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageBlue, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    RETURN_NUMBER(gdImageBlue(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageAlpha, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    RETURN_NUMBER(gdImageAlpha(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetTransparent, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdImageGetTransparent(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageGetInterlaced, 1)
    T_NUMBER(0)

    RETURN_NUMBER(gdImageGetInterlaced(((gdImagePtr)PARAM_INT(0))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImagePalettePixel, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)

    RETURN_NUMBER(gdImagePalettePixel(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1)), (PARAM_INT(2))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gdImageTrueColorPixel, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)

    RETURN_NUMBER(gdImageTrueColorPixel(((gdImagePtr)PARAM_INT(0)), (PARAM_INT(1)), (PARAM_INT(2))))
END_IMPL
//------------------------------------------------------------------------
