//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include <math.h>
#include <stdlib.h>
#include "library.h"
#include <time.h>
#include <float.h>
#include <ctype.h>
#include "AnsiString.h"

#define smbM_PI    3.14159265358979323846
#define MAX(A, B)    A > B ? A : B
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    AnsiString value = M_E;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_E", value.c_str());
    value = M_LOG2E;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_LOG2E", value.c_str());
    value = M_LOG10E;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_LOG10E", value.c_str());
    value = M_LN2;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_LN2", value.c_str());
    value = M_LN10;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_LN10", value.c_str());
    value = M_PI;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_PI", value.c_str());
    value = M_PI_2;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_PI_2", value.c_str());
    value = M_PI_4;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_PI_4", value.c_str());
    value = M_1_PI;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_1_PI", value.c_str());
    value = M_2_PI;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_2_PI", value.c_str());
    value = M_2_SQRTPI;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_2_SQRTPI", value.c_str());
    value = M_SQRT2;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_SQRT2", value.c_str());
    value = M_SQRT1_2;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "M_SQRT1_2", value.c_str());
    DEFINE_ECONSTANT(DBL_EPSILON);
    DEFINE_ECONSTANT(DBL_MIN);
    DEFINE_ECONSTANT(DBL_MIN_EXP);
    DEFINE_ECONSTANT(DBL_MIN_10_EXP);
    DEFINE_ECONSTANT(DBL_MAX);
    DEFINE_ECONSTANT(DBL_MAX_EXP);
    DEFINE_ECONSTANT(DBL_MAX_10_EXP);
    DEFINE_ECONSTANT(DBL_DIG);
    DEFINE_ECONSTANT(DBL_MANT_DIG);
    srand((unsigned)time(NULL));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__abs CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'abs' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    int _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"abs: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (int)abs((int)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__acos CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'acos' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"acos: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)acos((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__asin CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'asin' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"asin: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)asin((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__atan CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'atan' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"atan: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)atan((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__ceil CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'ceil' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"ceil: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)ceil((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__cos CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'cos' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"cos: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)cos((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__exp CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'exp' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"exp: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)exp((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fabs CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'fabs' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fabs: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)fabs((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__floor CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'floor' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"floor: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)floor((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fmod CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)": 'fmod' parameters error. This fuction takes 2 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fmod: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fmod: parameter 2 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)fmod((double)nParam0, (double)nParam1);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__labs CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'labs' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    long _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"labs: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (long)labs((long)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__ldexp CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)": 'ldexp' parameters error. This fuction takes 2 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"ldexp: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"ldexp: parameter 2 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)ldexp((double)nParam0, (int)nParam1);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__log CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'log' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"log: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)log((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__log10 CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'log10' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    float _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"log10: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (float)log10((float)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__pow CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)": 'pow' parameters error. This fuction takes 2 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"pow: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"pow: parameter 2 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)pow((double)nParam0, (double)nParam1);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__rand CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)": 'rand' parameters error. This fuction takes  no parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    int _C_call_result;
    // Specific variables

    // Variable type check

    // function call
    _C_call_result = (int)rand();

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__sin CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'sin' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"sin: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)sin((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__sqrt CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'sqrt' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"sqrt: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)sqrt((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__srand CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'srand' parameters error. This fuction takes one parameter.";

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
        return (void *)"srand: parameter 1 should be of STATIC NUMBER type";

    // function call
    srand((unsigned int)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__tan CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'tan' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    double _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"tan: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)tan((double)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
double RoundDouble(double doValue, int nPrecision) {
    static const double doBase = 10.0;
    double doComplete5, doComplete5i;

    doComplete5 = doValue * pow(doBase, (double)(nPrecision + 1));

    if (doValue < 0.0)
        doComplete5 -= 5.0;
    else
        doComplete5 += 5.0;

    doComplete5 /= doBase;
    modf(doComplete5, &doComplete5i);

    return doComplete5i / pow(doBase, (double)nPrecision);
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__round CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(1, 2, "round takes one or two parameters: number, digits=0");

    LOCAL_INIT;

    double number;
    int    digits = 0;

    GET_CHECK_NUMBER(0, number, "round: parameter 1 should be a static number");

    if (PARAMETERS_COUNT == 2) {
        double dig;
        GET_CHECK_NUMBER(1, dig, "round: parameter 2 should be a static number");
        digits = (int)dig;
    }


    /*long mul = (long)pow((double)10,(double)digits);
       number*=mul;

       if(number>0)
        number+=(double)1/2;
       else
        number-=(double)1/2;

       number=(long)number;
       //number/=mul;

       //sprintf(buffer,"%.30g",d);

       RETURN_NUMBER(number);*/
    RETURN_NUMBER(RoundDouble(number, digits));
    return 0;
}
//---------------------------------------------------------------------------
AnsiString math_number_format(double d, int dec, char dec_point, char thousand_sep) {
    AnsiString result;

    char *tmpbuf, *resbuf;
    char *s, *t;     /* source, target */
    int  tmplen, reslen = 0;
    int  count       = 0;
    int  is_negative = 0;

    if (d < 0) {
        is_negative = 1;
        d           = -d;
    }
    dec    = MAX(0, dec);
    tmpbuf = (char *)malloc(1 + DBL_MAX_10_EXP + 1 + dec + 1);

    tmplen = sprintf(tmpbuf, "%.*f", dec, d);

    if (!isdigit((int)tmpbuf[0])) {
        result = tmpbuf;
        free(tmpbuf);
        return result;
    }

    if (dec) {
        reslen = dec + 1 + (tmplen - dec - 1) + ((thousand_sep) ? (tmplen - 1 - dec - 1) / 3 : 0);
    } else {
        reslen = tmplen + ((thousand_sep) ? (tmplen - 1) / 3 : 0);
    }
    if (is_negative) {
        reslen++;
    }
    if (!dec_point) {
        reslen--;
    }
    resbuf = (char *)malloc(reslen + 2);

    s    = tmpbuf + tmplen - 1;
    t    = resbuf + reslen;
    *t-- = 0;

    if (dec) {
        while (isdigit((int)*s)) {
            *t-- = *s--;
        }
        if (dec_point)
            *t-- = dec_point;
        s--;
    }

    while (s >= tmpbuf) {
        *t-- = *s--;
        if (thousand_sep && ((++count % 3) == 0) && (s >= tmpbuf)) {
            *t-- = thousand_sep;
        }
    }
    if (is_negative) {
        *t-- = '-';
    }
    free(tmpbuf);
    result = resbuf;
    free(resbuf);
    return /*resbuf*/ result;
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_number_format, 4)
    T_NUMBER(_number_format, 0)
    T_NUMBER(_number_format, 1)
    T_STRING(_number_format, 2)
    T_STRING(_number_format, 3)

    char dec_point = 0;
    char thousand_sep = 0;

    if (PARAM(2))
        dec_point = PARAM(2)[0];

    if (PARAM(3))
        thousand_sep = PARAM(3)[0];

    RETURN_STRING(math_number_format(PARAM(0), PARAM_INT(1), dec_point, thousand_sep))
END_IMPL
//---------------------------------------------------------------------------
void smbFft(float *fftBuffer, long fftFrameSize, long sign) {
/*

   FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)

   Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the time
   domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes and returns
   the cosine and sine parts in an interleaved manner, ie.
   fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize must
   be a power of 2. It expects a complex input signal (see footnote 2), ie. when
   working with 'common' audio signals our input signal has to be passed as
   {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform of the
   frequencies of interest is in fftBuffer[0...fftFrameSize].

 */
    float wr, wi, arg, *p1, *p2, temp;
    float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
    long  i, bitm, j, le, le2, k, logN;

    logN = (long)(log(fftFrameSize) / log(2.) + .5);

    for (i = 2; i < 2 * fftFrameSize - 2; i += 2) {
        for (bitm = 2, j = 0; bitm < 2 * fftFrameSize; bitm <<= 1) {
            if (i & bitm) j++;
            j <<= 1;
        }
        if (i < j) {
            p1      = fftBuffer + i;
            p2      = fftBuffer + j;
            temp    = *p1;
            *(p1++) = *p2;
            *(p2++) = temp;
            temp    = *p1;
            *p1     = *p2;
            *p2     = temp;
        }
    }

    for (k = 0, le = 2; k < logN; k++) {
        le <<= 1;
        le2  = le >> 1;
        ur   = 1.0;
        ui   = 0.0;
        arg  = smbM_PI / (le2 >> 1);
        wr   = cos(arg);
        wi   = sign * sin(arg);
        for (j = 0; j < le2; j += 2) {
            p1r = fftBuffer + j;
            p1i = p1r + 1;
            p2r = p1r + le2;
            p2i = p2r + 1;
            for (i = j; i < 2 * fftFrameSize; i += le) {
                tr    = *p2r * ur - *p2i * ui;
                ti    = *p2r * ui + *p2i * ur;
                *p2r  = *p1r - tr;
                *p2i  = *p1i - ti;
                *p1r += tr;
                *p1i += ti;
                p1r  += le;
                p1i  += le;
                p2r  += le;
                p2i  += le;
            }
            tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
        }
    }
}

float *GetFloatList2(void *arr, INVOKE_CALL _Invoke) {
    INTEGER type      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;
    float   *ret      = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    ret            = new float[count * 2 + 1];
    ret[count * 2] = 0;

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i] = 0;
            } else
                ret[i] = nData;
        }
    }
    return ret;
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FFT, 1, 2)
    T_ARRAY(FFT, 0)

    int sign = -1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(FFT, 1);
        if (PARAM_INT(1))
            sign = 1;
    }
    int   count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));
    float *arr  = GetFloatList2(PARAMETER(0), Invoke);
    CREATE_ARRAY(RESULT);
    if ((arr) && (count > 0)) {
        smbFft(arr, (count + 1) / 2, sign);
        for (int i = 0; i < count; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)arr[i]);
    }
    if (arr)
        delete[] arr;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(Mix, 1, 2)
    T_ARRAY(Mix, 0)
    int is_char = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(Mix, 1)
        is_char = PARAM_INT(1);
    }
    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));

    if (count > 0) {
        void    *newpData = 0;
        char    **in      = (char **)malloc(count * sizeof(char *));
        INTEGER *lens     = (int *)malloc(count * sizeof(INTEGER));
        int     idx       = 0;
        int     max_len   = 0;
        for (INTEGER i = 0; i < count; i++) {
            newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(0), i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING) {
                    in[idx]   = szData;
                    lens[idx] = (INTEGER)nData;
                    if (max_len < lens[idx])
                        max_len = lens[idx];
                    idx++;
                }
            }
        }
        if (max_len) {
            char *out = NULL;
            CORE_NEW(max_len + 1, out);
            out[max_len] = 0;
            if (is_char) {
                for (int i = 0; i < max_len; i++) {
                    int b            = 0;
                    int signal_count = 0;
                    for (int j = 0; j < idx; j++) {
                        if (i < lens[j]) {
                            int x = in[j][i];
                            b += x;
                            b -= (int)(double)b * x / 127;//256;
                        }
                    }
                    out[i] = b;
                }
            } else {
                short *out_int      = (short *)out;
                int   effective_len = max_len / 2;
                for (int i = 0; i < effective_len; i++) {
                    int b = 0;
                    for (int j = 0; j < idx; j++) {
                        if (i < lens[j] / 2) {
                            int x = ((short **)in)[j][i];
                            b += x;
                            b -= (int)(double)b * x / 32767;//65535;

                            /*for (int k = j + 1; k < idx; k++) {
                                if (i < lens[k]/2) {
                                    int y = ((short **)in)[k][i];
                                    b -= (int)(double)x*y/32767;//65535;
                                }
                               }*/
                        }
                    }
                    if (b > 32767)
                        b = 32767;
                    if (b < -32767)
                        b = -32767;
                    out_int[i] = b;
                }
                max_len = effective_len * 2;
            }
            SetVariable(RESULT, -1, out, max_len);
        } else {
            RETURN_STRING("");
        }
        free(in);
        free(lens);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PCMAvg, 1, 2)
    T_STRING(PCMAvg, 0)
    int is_char = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(PCMAvg, 1)
        is_char = PARAM_INT(1);
    }
    double res = 0;
    double sample;
    if (is_char) {
        int  len  = PARAM_LEN(0);
        char *buf = PARAM(0);
        for (int i = 0; i < len; i++) {
            char a = buf[i];
            if (a > 0)
                sample = (double)a / 127;
            else
                sample = (double)a / -127;
            res += sample / len;
        }
    } else {
        int   len  = PARAM_LEN(0) / 2;
        short *buf = (short *)PARAM(0);
        for (int i = 0; i < len; i++) {
            short a = buf[i];
            if (a > 0)
                sample = (double)a / 32767;
            else
                sample = (double)a / -32767;
            res += sample / len;
        }
    }
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PCMAmplify, 2, 3)
    T_STRING(PCMAmplify, 0)
    T_NUMBER(PCMAmplify, 1)

    int is_char = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(PCMAmplify, 2)
        is_char = PARAM_INT(2);
    }

    int    len;
    double sample;
    if (is_char) {
        len = PARAM_LEN(0);
        char *buf = PARAM(0);
        for (int i = 0; i < len; i++) {
            sample = buf[i] * PARAM(1);
            if (sample > 127)
                sample = 127;
            else
            if (sample < -127)
                sample = -127;
            buf[i] = (char)sample;
        }
    } else {
        len = PARAM_LEN(0) / 2;
        short *buf = (short *)PARAM(0);
        for (int i = 0; i < len; i++) {
            sample = buf[i] * PARAM(1);
            if (sample > 32767)
                sample = 32767;
            else
            if (sample < -32767)
                sample = -32767;
            buf[i] = (short)sample;
        }
    }
    RETURN_NUMBER(len)
END_IMPL
//---------------------------------------------------------------------------

