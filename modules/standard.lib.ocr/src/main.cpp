//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>

#include <unichar.h>
#ifdef OLD_TESSERACT
 #include "tessdll.h"
 #include "imgs.h"
#else
 #include <baseapi.h>
 #include <allheaders.h>
#endif

#define OCR_E_CANT_OPEN    -1
#define OCR_E_CANT_READ    -1

#ifdef OLD_TESSERACT
 #ifdef _WIN32
extern STRING_VAR_H(debug_file, "nul", "File to send tprintf output to");
const STRING _nl("nul");
 #else
extern STRING_VAR_H(debug_file, "/dev/null", "File to send tprintf output to");
const STRING _nl("/dev/null");
 #endif
#endif

AnsiString static_path;
//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(OCR_E_CANT_OPEN)
    DEFINE_ECONSTANT(OCR_E_CANT_READ)

#ifdef OLD_TESSERACT
    debug_file.set_value(_nl);
#endif
    return 0;
}
//------------------------------------------------------------------------
AnsiString GetDirectory() {
#ifdef _WIN32
    char buffer[4096];
    buffer[0] = 0;
    GetModuleFileName(NULL, buffer, 4096);
    for (int i = strlen(buffer) - 1; i >= 0; i--) {
        if ((buffer[i] == '/') || (buffer[i] == '\\')) {
            buffer[i + 1] = 0;
            break;
        }
    }
    return AnsiString(buffer);
#endif
}

//------------------------------------------------------------------------
#ifdef OLD_TESSERACT
char *run_tesseract2(const char *language,
                     const unsigned char *imagedata,
                     int bytes_per_pixel,
                     int bytes_per_line,
                     int width, int height, char *datapath, char *configfile, bool numeric_mode) {
    TessBaseAPI::InitWithLanguage(datapath, NULL, language, configfile, numeric_mode, 0, NULL);
    char *text = TessBaseAPI::TesseractRect(imagedata, bytes_per_pixel,
                                            bytes_per_line, 0, 0,
                                            width, height);
    TessBaseAPI::End();

    return text;                    //Normal exit
}

#else
char *run_tesseract2(const char *language, const char *imagedata /*, int size*/) {
    tesseract::TessBaseAPI *myOCR = new tesseract::TessBaseAPI();
    char *text = 0;

    if (myOCR->Init(NULL, language)) {
        delete myOCR;
        return 0;
    }
    Pix *pix = pixRead(imagedata);//pixReadMem(imagedata, size);
    if (pix) {
        myOCR->SetImage(pix);
        text = myOCR->GetUTF8Text();
    }
    myOCR->Clear();
    myOCR->End();
    pixDestroy(&pix);

    return text;
}
#endif
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(OCRDebugFile, 1)
    T_STRING(OCRDebugFile, 0)
#ifdef OLD_TESSERACT
    debug_file.set_value(PARAM(0));
#endif
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// OCR(image_filename, var out_text, language="eng", datapath="", configfile="")
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(OCR, 2, 5)
    T_STRING(OCR, 0)
    AnsiString lang = "eng";
#ifdef OLD_TESSERACT
    char *datapath   = NULL;
    char *configfile = NULL;

    if (PARAMETERS_COUNT > 2) {
        T_STRING(OCR, 2);
        lang = PARAM(2);
    }
    if (PARAMETERS_COUNT > 3) {
        T_STRING(OCR, 3);
        if (PARAM_LEN(3))
            datapath = PARAM(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_STRING(OCR, 4);
        if (PARAM_LEN(4))
            configfile = PARAM(4);
    }
    IMAGE image;
 #ifdef _WIN32
    if (!datapath) {
        static_path = GetDirectory() + (char *)"..\\share\\tessdata";
        datapath    = static_path.c_str();
    }
 #endif
    if (image.read_header(PARAM(0)) < 0) {
        RETURN_NUMBER(OCR_E_CANT_OPEN);
        return 0;
    }

    if (image.read(image.get_ysize()) < 0) {
        RETURN_NUMBER(OCR_E_CANT_READ);
        return 0;
    }

    int bytes_per_line = COMPUTE_IMAGE_XDIM(image.get_xsize(), image.get_bpp());

    char *res = run_tesseract2(lang.c_str(), image.get_buffer(), image.get_bpp() / 8, bytes_per_line, image.get_xsize(), image.get_ysize(), datapath, configfile, false);
    SET_STRING(1, res);
    if (res)
        delete[] res;
#else
    if (PARAMETERS_COUNT > 2) {
        T_STRING(OCR, 2);
        lang = PARAM(2);
    }

    char *res = run_tesseract2(lang.c_str(), PARAM(0));
    if (res) {
        SET_STRING(1, res);
        delete[] res;
    } else {
        SET_STRING(1, "");
    }
#endif
    RETURN_NUMBER(0)
END_IMPL

