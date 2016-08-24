//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include <sablot.h>


INVOKE_CALL InvokePtr = 0;

char **GetCharList(void *arr) {
    INTEGER type      = 0;
    char    *str      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;
    char    **ret     = 0;

    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    ret        = new char * [count + 1];
    ret[count] = 0;

    for (int i = 0; i < count; i++) {
        InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i] = szData;
            } else
                ret[i] = 0;
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(SAB_NO_ERROR_REPORTING)
    DEFINE_ECONSTANT(SAB_PARSE_PUBLIC_ENTITIES)
    DEFINE_ECONSTANT(SAB_DISABLE_ADDING_META)
    DEFINE_ECONSTANT(SAB_DISABLE_STRIPPING)
    DEFINE_ECONSTANT(SAB_IGNORE_DOC_NOT_FOUND)
    DEFINE_ECONSTANT(SAB_FILES_TO_HANDLER)
    DEFINE_ECONSTANT(SAB_DUMP_SHEET_STRUCTURE)
    DEFINE_ECONSTANT(SAB_NO_EXTERNAL_ENTITIES)

    DEFINE_ECONSTANT(HLR_MESSAGE)
    DEFINE_ECONSTANT(HLR_SCHEME)
    DEFINE_ECONSTANT(HLR_SAX)
    DEFINE_ECONSTANT(HLR_MISC)
    DEFINE_ECONSTANT(HLR_ENC)

    DEFINE_ECONSTANT(SH_ERR_OK)
    DEFINE_ECONSTANT(SH_ERR_NOT_OK)
    DEFINE_ECONSTANT(SH_ERR_UNSUPPORTED_SCHEME)

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotCreateDocument, 2)
    T_NUMBER(_SablotCreateDocument, 0)     // SablotSituation

// ... parameter 1 is by reference (SDOM_Document*)
    SDOM_Document local_parameter_1;

    RETURN_NUMBER(SablotCreateDocument((SablotSituation)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotParse, 3)
    T_NUMBER(_SablotParse, 0)     // SablotSituation
    T_STRING(_SablotParse, 1)     // char*

// ... parameter 2 is by reference (SDOM_Document*)
    SDOM_Document local_parameter_2;

    RETURN_NUMBER(SablotParse((SablotSituation)(long)PARAM(0), PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotParseBuffer, 3)
    T_NUMBER(_SablotParseBuffer, 0)     // SablotSituation
    T_STRING(_SablotParseBuffer, 1)     // char*

// ... parameter 2 is by reference (SDOM_Document*)
    SDOM_Document local_parameter_2;

    RETURN_NUMBER(SablotParseBuffer((SablotSituation)(long)PARAM(0), PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotParseStylesheet, 3)
    T_NUMBER(_SablotParseStylesheet, 0)     // SablotSituation
    T_STRING(_SablotParseStylesheet, 1)     // char*

// ... parameter 2 is by reference (SDOM_Document*)
    SDOM_Document local_parameter_2;

    RETURN_NUMBER(SablotParseStylesheet((SablotSituation)(long)PARAM(0), PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotParseStylesheetBuffer, 3)
    T_NUMBER(_SablotParseStylesheetBuffer, 0)     // SablotSituation
    T_STRING(_SablotParseStylesheetBuffer, 1)     // char*

// ... parameter 2 is by reference (SDOM_Document*)
    SDOM_Document local_parameter_2;

    RETURN_NUMBER(SablotParseStylesheetBuffer((SablotSituation)(long)PARAM(0), PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotLockDocument, 2)
    T_NUMBER(_SablotLockDocument, 0)     // SablotSituation
    T_NUMBER(_SablotLockDocument, 1)     // SDOM_Document

    RETURN_NUMBER(SablotLockDocument((SablotSituation)(long)PARAM(0), (SDOM_Document)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotDestroyDocument, 2)
    T_NUMBER(_SablotDestroyDocument, 0)     // SablotSituation
    T_NUMBER(_SablotDestroyDocument, 1)     // SDOM_Document

    RETURN_NUMBER(SablotDestroyDocument((SablotSituation)(long)PARAM(0), (SDOM_Document)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotAddParam, 4)
    T_NUMBER(_SablotAddParam, 0)     // SablotSituation
    T_NUMBER(_SablotAddParam, 1)     // void*
    T_STRING(_SablotAddParam, 2)     // char*
    T_STRING(_SablotAddParam, 3)     // char*

    RETURN_NUMBER(SablotAddParam((SablotSituation)(long)PARAM(0), (void *)(long)PARAM(1), PARAM(2), PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotAddArgBuffer, 4)
    T_NUMBER(_SablotAddArgBuffer, 0)     // SablotSituation
    T_NUMBER(_SablotAddArgBuffer, 1)     // void*
    T_STRING(_SablotAddArgBuffer, 2)     // char*
    T_STRING(_SablotAddArgBuffer, 3)     // char*

    RETURN_NUMBER(SablotAddArgBuffer((SablotSituation)(long)PARAM(0), (void *)(long)PARAM(1), PARAM(2), PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotAddArgTree, 4)
    T_NUMBER(_SablotAddArgTree, 0)     // SablotSituation
    T_NUMBER(_SablotAddArgTree, 1)     // void*
    T_STRING(_SablotAddArgTree, 2)     // char*
    T_NUMBER(_SablotAddArgTree, 3)     // SDOM_Document

    RETURN_NUMBER(SablotAddArgTree((SablotSituation)(long)PARAM(0), (void *)(long)PARAM(1), PARAM(2), (SDOM_Document)(long)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotRunProcessorGen, 5)
    T_NUMBER(_SablotRunProcessorGen, 0)     // SablotSituation
    T_NUMBER(_SablotRunProcessorGen, 1)     // void*
    T_STRING(_SablotRunProcessorGen, 2)     // char*
    T_STRING(_SablotRunProcessorGen, 3)     // char*
    T_STRING(_SablotRunProcessorGen, 4)     // char*

    RETURN_NUMBER(SablotRunProcessorGen((SablotSituation)(long)PARAM(0), (void *)(long)PARAM(1), PARAM(2), PARAM(3), PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotRunProcessorExt, 5)
    T_NUMBER(_SablotRunProcessorExt, 0)     // SablotSituation
    T_NUMBER(_SablotRunProcessorExt, 1)     // void*
    T_STRING(_SablotRunProcessorExt, 2)     // char*
    T_STRING(_SablotRunProcessorExt, 3)     // char*
    T_NUMBER(_SablotRunProcessorExt, 4)     // NodeHandle

    RETURN_NUMBER(SablotRunProcessorExt((SablotSituation)(long)PARAM(0), (void *)(long)PARAM(1), PARAM(2), PARAM(3), (NodeHandle)(long)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotCreateSituation, 1)

// ... parameter 0 is by reference (SablotSituation*)
    SablotSituation local_parameter_0;

    RETURN_NUMBER(SablotCreateSituation(&local_parameter_0))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotSetOptions, 2)
    T_NUMBER(_SablotSetOptions, 0)     // SablotSituation
    T_NUMBER(_SablotSetOptions, 1)     // int

    RETURN_NUMBER(SablotSetOptions((SablotSituation)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotGetOptions, 1)
    T_NUMBER(_SablotGetOptions, 0)     // SablotSituation

    RETURN_NUMBER(SablotGetOptions((SablotSituation)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotClearSituation, 1)
    T_NUMBER(_SablotClearSituation, 0)     // SablotSituation

    RETURN_NUMBER(SablotClearSituation((SablotSituation)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(_SablotGetErrorURI,1)
        T_NUMBER(_SablotClearSituation, 0) // SablotSituation

        RETURN_STRING((char*)SablotGetErrorURI((SablotSituation)(long)PARAM(0)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(_SablotGetErrorLine,1)
        T_NUMBER(_SablotGetErrorLine, 0) // SablotSituation

        RETURN_NUMBER(SablotGetErrorLine((SablotSituation)(long)PARAM(0)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(_SablotGetErrorMsg,1)
        T_NUMBER(_SablotGetErrorMsg, 0) // SablotSituation

        RETURN_STRING((char*)SablotGetErrorMsg((SablotSituation)(long)PARAM(0)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotDestroySituation, 1)
    T_NUMBER(_SablotDestroySituation, 0)     // SablotSituation

    RETURN_NUMBER(SablotDestroySituation((SablotSituation)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotCreateProcessor, 1)

// ... parameter 0 is by reference (SablotHandle*)
    SablotHandle local_parameter_0;

    RETURN_NUMBER(SablotCreateProcessor(&local_parameter_0))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotCreateProcessorForSituation, 2)
    T_NUMBER(_SablotCreateProcessorForSituation, 0)     // SablotSituation

// ... parameter 1 is by reference (void**)
    void *local_parameter_1 = 0;

    RETURN_NUMBER(SablotCreateProcessorForSituation((SablotSituation)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotDestroyProcessor, 1)
    T_NUMBER(_SablotDestroyProcessor, 0)     // SablotHandle

    RETURN_NUMBER(SablotDestroyProcessor((SablotHandle)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotRunProcessor, 6)
    T_NUMBER(_SablotRunProcessor, 0)     // SablotHandle
    T_STRING(_SablotRunProcessor, 1)     // char*
    T_STRING(_SablotRunProcessor, 2)     // char*
    T_STRING(_SablotRunProcessor, 3)     // char*

    char *data4 = 0;
    char *data5 = 0;
    GET_CHECK_ARRAY(4, data4, "Parameter 4 should be an array");
    GET_CHECK_ARRAY(5, data5, "Parameter 5 should be an array");

    char **arr4 = GetCharList(PARAMETER(4));
    char **arr5 = GetCharList(PARAMETER(5));

// ... parameter 4 is by reference (char**)
//char* local_parameter_4;
// ... parameter 5 is by reference (char**)
//char* local_parameter_5;

    RETURN_NUMBER(SablotRunProcessor((SablotHandle)(long)PARAM(0), PARAM(1), PARAM(2), PARAM(3), (const char **)arr4, (const char **)arr5))

    if (arr4)
        delete[] arr4;
    if (arr5)
        delete[] arr5;
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotGetResultArg, 3)
    T_NUMBER(_SablotGetResultArg, 0)     // SablotHandle
    T_STRING(_SablotGetResultArg, 1)     // char*

// ... parameter 2 is by reference (char**)
    char *local_parameter_2 = 0;

    RETURN_NUMBER(SablotGetResultArg((SablotHandle)(long)PARAM(0), PARAM(1), &local_parameter_2))
//SET_NUMBER(2, (long)local_parameter_2)
    if (local_parameter_2) {
        SET_STRING(2, local_parameter_2)
        SablotFree(local_parameter_2);
    } else {
        SET_STRING(2, "")
    }

END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotFreeResultArgs, 1)
    T_NUMBER(_SablotFreeResultArgs, 0)     // SablotHandle

    RETURN_NUMBER(SablotFreeResultArgs((SablotHandle)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(_SablotRegHandler,4)
        T_NUMBER(_SablotFreeResultArgs, 0) // SablotHandle
        T_NUMBER(_SablotFreeResultArgs, 1) // HandlerType
        T_NUMBER(_SablotFreeResultArgs, 2) // void*
        T_NUMBER(_SablotFreeResultArgs, 3) // void*

        RETURN_NUMBER(SablotRegHandler((SablotHandle)(long)PARAM(0), (HandlerType)PARAM(1), (void *)(long)PARAM(2), (void *)(long)PARAM(3)))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(_SablotUnregHandler,4)
        T_NUMBER(_SablotUnregHandler, 0) // SablotHandle
        T_NUMBER(_SablotUnregHandler, 1) // HandlerType
        T_NUMBER(_SablotUnregHandler, 2) // void*
        T_NUMBER(_SablotUnregHandler, 3) // void*

        RETURN_NUMBER(SablotUnregHandler((SablotHandle)(long)PARAM(0), (HandlerType)PARAM(1), (void *)(long)PARAM(2), (void *)(long)PARAM(3)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotSetBase, 2)
    T_NUMBER(_SablotSetBase, 0)     // SablotHandle
    T_STRING(_SablotSetBase, 1)     // char*

    RETURN_NUMBER(SablotSetBase((SablotHandle)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotSetBaseForScheme, 3)
    T_NUMBER(_SablotSetBaseForScheme, 0)     // void*
    T_STRING(_SablotSetBaseForScheme, 1)     // char*
    T_STRING(_SablotSetBaseForScheme, 2)     // char*

    RETURN_NUMBER(SablotSetBaseForScheme((void *)(long)PARAM(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotSetLog, 3)
    T_NUMBER(_SablotSetLog, 0)     // SablotHandle
    T_STRING(_SablotSetLog, 1)     // char*
    T_NUMBER(_SablotSetLog, 2)     // int

    RETURN_NUMBER(SablotSetLog((SablotHandle)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
#define EE(statement)       { int code___ = (statement); if (code___) return code___; }
#define EE_D(CODE, PROC)    { int code___ = (CODE); if (code___) { SablotDestroyProcessor(PROC); return code___; } }

static MH_ERROR sablot_make_code(void *userData, SablotHandle p, int severity, unsigned short facility, unsigned short code) {
    return code;
}

static MH_ERROR sablot_log(void *userData, SablotHandle p, MH_ERROR code, MH_LEVEL level, char **fields) {
    return 0;
}

static AnsiString sablot_err;
static MH_ERROR sablot_error(void *userData, SablotHandle p, MH_ERROR code, MH_LEVEL level, char **fields) {
    char **cc;
    int  has_err = 0;

    if (sablot_err.Length()) {
        sablot_err += "\n\n";
        has_err     = 1;
    }
    for (cc = fields; *cc != NULL; cc++) {
        if (has_err) {
            sablot_err += ",\n";
        } else
            has_err = 1;
        sablot_err += *cc;
    }
    return 1;
}

static MessageHandler mh = {
    sablot_make_code,
    sablot_log,
    sablot_error
};

int SablotProcess2(const char *sheetURI, const char *inputURI,
                   const char *resultURI,
                   const char **params, const char **arguments,
                   char **resultArg) {
    sablot_err.LoadBuffer(0, 0);
    void *theproc;
    EE(SablotCreateProcessor(&theproc));
    EE(SablotRegHandler(theproc, HLR_MESSAGE, (void *)&mh, (void *)NULL));
    EE_D(SablotRunProcessor(theproc,
                            sheetURI, inputURI, resultURI,
                            params, arguments), theproc);
    EE_D(SablotGetResultArg(theproc, resultURI, resultArg), theproc);
    EE(SablotDestroyProcessor(theproc));
    return 0;
}

CONCEPT_FUNCTION_IMPL(_SablotProcess, 6)
    T_STRING(_SablotProcess, 0)     // char*
    T_STRING(_SablotProcess, 1)     // char*
    T_STRING(_SablotProcess, 2)     // char*

    char *data3 = 0;
    char *data4 = 0;
    GET_CHECK_ARRAY(3, data3, "Parameter 3 should be an array");
    GET_CHECK_ARRAY(4, data4, "Parameter 4 should be an array");

    char **arr3 = GetCharList(PARAMETER(3));
    char **arr4 = GetCharList(PARAMETER(4));

// ... parameter 3 is by reference (char**)
//char* local_parameter_3;
// ... parameter 4 is by reference (char**)
//char* local_parameter_4;
// ... parameter 5 is by reference (char**)
    char *local_parameter_5 = 0;

    RETURN_NUMBER(SablotProcess2(PARAM(0), PARAM(1), PARAM(2), (const char **)arr3, (const char **)arr4, &local_parameter_5))

    if (local_parameter_5) {
        SET_STRING(5, local_parameter_5)
        SablotFree(local_parameter_5);
    } else {
        SET_STRING(5, "")
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotProcessFiles, 3)
    T_STRING(_SablotProcessFiles, 0)     // char*
    T_STRING(_SablotProcessFiles, 1)     // char*
    T_STRING(_SablotProcessFiles, 2)     // char*

    RETURN_NUMBER(SablotProcessFiles(PARAM(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotProcessStrings, 3)
    T_STRING(_SablotProcessStrings, 0)     // char*
    T_STRING(_SablotProcessStrings, 1)     // char*

// ... parameter 2 is by reference (char**)
    char *local_parameter_2 = 0;

    RETURN_NUMBER(SablotProcessStrings(PARAM(0), PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotProcessStringsWithBase, 4)
    T_STRING(_SablotProcessStringsWithBase, 0)     // char*
    T_STRING(_SablotProcessStringsWithBase, 1)     // char*
    T_STRING(_SablotProcessStringsWithBase, 3)     // char*

// ... parameter 2 is by reference (char**)
    char *local_parameter_2 = 0;

    RETURN_NUMBER(SablotProcessStringsWithBase(PARAM(0), PARAM(1), &local_parameter_2, PARAM(3)))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotClearError, 1)
    T_NUMBER(_SablotClearError, 0)     // SablotHandle

    RETURN_NUMBER(SablotClearError((SablotHandle)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotGetMsgText, 1)
    T_NUMBER(_SablotGetMsgText, 0)     // int

    if (sablot_err.Length()) {
        AnsiString res = "<pre>\n";

        res += sablot_err;
        res += "\n\n";
        res += (char *)SablotGetMsgText((int)PARAM(0));
        res += "\n</pre>";
        RETURN_STRING(res.c_str());
    } else {
        RETURN_STRING((char *)SablotGetMsgText((int)PARAM(0)))
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotSetInstanceData, 2)
    T_NUMBER(_SablotSetInstanceData, 0)     // SablotHandle
    T_NUMBER(_SablotSetInstanceData, 1)     // void*

    SablotSetInstanceData((SablotHandle)(long)PARAM(0), (void *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotGetInstanceData, 1)
    T_NUMBER(_SablotGetInstanceData, 0)     // SablotHandle

    RETURN_NUMBER((long)SablotGetInstanceData((SablotHandle)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_SablotSetEncoding, 2)
    T_NUMBER(_SablotSetEncoding, 0)     // SablotHandle
    T_STRING(_SablotSetEncoding, 1)     // char*

    SablotSetEncoding((SablotHandle)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL

