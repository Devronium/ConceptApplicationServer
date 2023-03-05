#include "BuiltIns.h"
#include "PIFAlizator.h"
#include "ModuleLink.h"
#include "BuiltInsHelpers.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>

    #define localtime_r(a,b)    localtime(a)
    #define gmtime_r(a,b)       gmtime(a)
    #define ctime_r(a,b)        ctime(a)
    #define asctime_r(a,b)      asctime(a)

    #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
        #define DELTA_EPOCH_IN_MICROSECS  116444736000000000Ui64
    #else
        #define DELTA_EPOCH_IN_MICROSECS  116444736000000000ULL
    #endif
#ifndef HAVE_TIMEZONE
    struct timezone {
        int tz_minuteswest;
        int tz_dsttime;
    };
#endif
    int gettimeofday(struct timeval *tv, struct timezone *tz) {
        FILETIME         ft;
        unsigned __int64 tmpres = 0;

        if (NULL != tv) {
            GetSystemTimeAsFileTime(&ft);

            tmpres  |= ft.dwHighDateTime;
            tmpres <<= 32;
            tmpres  |= ft.dwLowDateTime;

            tmpres     -= DELTA_EPOCH_IN_MICROSECS;
            tmpres     /= 10; 

            tv->tv_sec  = (long)(tmpres / 1000000UL);
            tv->tv_usec = (long)(tmpres % 1000000UL);
        }

        if (NULL != tz) {
            tz->tz_minuteswest = _timezone / 60;
            tz->tz_dsttime     = _daylight;
        }
        return 0;
    }
#else
    #include <sys/time.h>
    extern long timezone;
#endif

extern "C" {
    #include "builtin/regexp.h"
}

// ==================================================== //
// BUILT-IN functions                                   //
// ==================================================== //
CONCEPT_FUNCTION_IMPL(___CONCEPT_INTERFACE_HELPER_GENERATE_UNIQUE_ID, 0)
    unsigned int ref_id = Invoke(INVOKE_GETID, PARAMETERS->PIF);
    RETURN_NUMBER(ref_id);
END_IMPL

CONCEPT_FUNCTION_IMPL(CLArg, 0)
    long argc   = 0;
    char **argv = 0;
    Invoke(INVOKE_CLI_ARGUMENTS, &argc, &argv);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);
    if (CLIENT_SOCKET <= 0) {
        for (int i = 0; i < argc; i++) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, argv[i], (double)0);
        }
    } else {
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)0, (INTEGER)VARIABLE_STRING, getenv("CONCEPT_ARGUMENTS"), (double)0);
    }
END_IMPL

CONCEPT_FUNCTION_IMPL(GLOBALS, 0)
    void *GOARRAY = NULL;
    Invoke(INVOKE_GETGLOBALS, PARAMETERS->PIF, &GOARRAY);
    RETURN_ARRAY(GOARRAY);
END_IMPL

CONCEPT_FUNCTION_IMPL(CheckReachability, 0) 
    int res = Invoke(INVOKE_CHECK_REACHABILITY, PARAMETERS->PIF);
    RETURN_NUMBER(res); 
END_IMPL

CONCEPT_FUNCTION_IMPL(MemoryInfo, 0) 
    CREATE_ARRAY(RESULT);
    GetMemoryStatistics(PARAMETERS->PIF, RESULT->CLASS_DATA);
END_IMPL

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(RE_create, 2, 3)
    T_STRING(RE_create, 0)
    T_NUMBER(RE_create, 1)

    const char *errorp = NULL;
    Reprog *reg = JS_regcomp(PARAM(0), PARAM_INT(1), &errorp);
    if (PARAMETERS->COUNT > 2) {
        if (errorp) {
            SET_STRING(2, errorp);
        } else {
            SET_STRING(2, "");
        }
    }
    RETURN_NUMBER((SYS_INT)reg);
END_IMPL

CONCEPT_FUNCTION_IMPL(RE_exec, 2) 
    T_HANDLE(RE_exec, 0)
    T_STRING(RE_exec, 1)

    Reprog *reg = (Reprog *)(SYS_INT)PARAM(0);
    int offset = JS_reglastindex(reg);
    const char *_string = PARAM(1);
    if (offset >= 0) {
        if (offset >= PARAM_LEN(1)) {
            RETURN_NUMBER(0);
            return 0;
        }
        _string += offset;
    }
    Resub sub;
    int res = JS_regexec(reg, _string, &sub, 0);
    if (!res) {
        const char *sp = sub.sub[0].sp;
        const char *ep = sub.sub[0].ep;
        if ((sp) && (ep) && (sp != ep)) {
            const char *maxep = ep;
            uintptr_t delta = ep - sp;
            if ((sub.sub[1].sp) && (sub.sub[1].ep)) {
                CREATE_ARRAY(RESULT);

                INTEGER index = 0;
                do {
                    delta = ep - sp;
                    if (delta) {
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, (INTEGER)VARIABLE_STRING, sp, (double)delta);
                    } else {
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, (INTEGER)VARIABLE_STRING, "", (double)0);
                    }
                    if (ep > maxep)
                        maxep = ep;
                    index++;
                    if (index >= REG_MAXSUB)
                        break;
                    sp = sub.sub[index].sp;
                    ep = sub.sub[index].ep;
                } while (index < sub.nsub);
            } else {
                RETURN_BUFFER(sp, delta);
            }
            if (offset >= 0) {
                intptr_t relative_offset = (uintptr_t)maxep - (uintptr_t)PARAM(1);
                if (relative_offset < 0)
                    relative_offset = PARAM_LEN(1);
                JS_setreglastindex(reg, relative_offset);
            }
        } else {
            RETURN_STRING("")
        }
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL

CONCEPT_FUNCTION_IMPL(RE_test, 2) 
    T_HANDLE(RE_test, 0)
    T_STRING(RE_test, 1)

    Reprog *reg = (Reprog *)(SYS_INT)PARAM(0);
    Resub sub;

    int offset = JS_reglastindex(reg);
    const char *_string = PARAM(1);
    if (offset >= 0) {
        if (offset >= PARAM_LEN(1)) {
            RETURN_NUMBER(0);
            return 0;
        }
        _string += offset;
    }

    int res = JS_regexec(reg, _string, &sub, 0);
    if (offset >= 0) {
        const char *sp = sub.sub[0].sp;
        const char *ep = sub.sub[0].ep;
        if ((sp) && (ep) && (sp != ep)) {
            intptr_t relative_offset = (uintptr_t)ep - (uintptr_t)PARAM(1);
            if (relative_offset < 0)
                relative_offset = PARAM_LEN(1);
            JS_setreglastindex(reg, relative_offset);
        }
    }
    RETURN_NUMBER(!res);
END_IMPL

CONCEPT_FUNCTION_IMPL(RE_lastindex, 1)
    T_HANDLE(RE_test, 0)
    Reprog *reg = (Reprog *)(SYS_INT)PARAM(0);
    int lastIndex = JS_reglastindex(reg);
    if (lastIndex < 0)
        lastIndex = 0;
    RETURN_NUMBER(lastIndex);
END_IMPL

CONCEPT_FUNCTION_IMPL(RE_done, 1)
    T_NUMBER(RE_done, 0)
    Reprog *reg = (Reprog *)(SYS_INT)PARAM(0);
    if (reg) {
        JS_regfree(reg);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL

CONCEPT_FUNCTION_IMPL(milliseconds, 0)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long ms = (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
    RETURN_NUMBER((NUMBER)ms);
END_IMPL

CONCEPT_FUNCTION_IMPL(timezone, 0)
#ifdef _WIN32
    RETURN_NUMBER(_timezone/60);
#else
    RETURN_NUMBER(timezone/60);
#endif
END_IMPL

CONCEPT_FUNCTION_IMPL(time, 0)
    RETURN_NUMBER((NUMBER)time(NULL));
END_IMPL

CONCEPT_FUNCTION_IMPL(localtime, 1)
    T_NUMBER(localtime, 0)
    CREATE_ARRAY(RESULT);

    time_t tempp = PARAM_INT(0);
    struct tm tmbuf;
    struct tm *timeinfo = localtime_r(&tempp, &tmbuf);
    if (timeinfo) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_hour", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)timeinfo->tm_hour);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_isdst", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)timeinfo->tm_isdst);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_mday", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)timeinfo->tm_mday);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_min", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)timeinfo->tm_min);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_mon", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)timeinfo->tm_mon);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_sec", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)timeinfo->tm_sec);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_wday", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)timeinfo->tm_wday);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_yday", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)timeinfo->tm_yday);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_year", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)timeinfo->tm_year);
    }
END_IMPL

CONCEPT_FUNCTION_IMPL(gmtime, 1)
    T_NUMBER(gmtime, 0)
    CREATE_ARRAY(RESULT);

    time_t tempp = PARAM_INT(0);
    struct tm tmbuf;
    struct tm *timeinfo = gmtime_r(&tempp, &tmbuf);
    if (timeinfo) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_hour", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tm_hour);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_isdst", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tm_isdst);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_mday", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tm_mday);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_min", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tm_min);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_mon", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tm_mon);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_sec", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tm_sec);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_wday", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tm_wday);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_yday", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tm_yday);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tm_year", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tm_year);
    }
END_IMPL

CONCEPT_FUNCTION_IMPL(__epoch, 8)
    T_NUMBER(__epoch, 0);
    T_NUMBER(__epoch, 1);
    T_NUMBER(__epoch, 2);
    T_NUMBER(__epoch, 3);
    T_NUMBER(__epoch, 4);
    T_NUMBER(__epoch, 5);
    T_NUMBER(__epoch, 6);
    T_NUMBER(__epoch, 7);

    struct tm tmbuf;
    time_t tempp = 0;
    struct tm *timeinfo;
    int is_localtime = PARAM_INT(7);
    if (is_localtime) {
        timeinfo = localtime_r(&tempp, &tmbuf);
    } else {
        timeinfo = gmtime_r(&tempp, &tmbuf);
        timeinfo->tm_isdst = -1;
    }

    timeinfo->tm_year = PARAM_INT(0) - 1900;
    timeinfo->tm_mon = PARAM_INT(1);
    timeinfo->tm_mday = PARAM_INT(2);
    timeinfo->tm_hour = PARAM_INT(3);
    timeinfo->tm_min = PARAM_INT(4);
    timeinfo->tm_sec = PARAM_INT(5);

    uint64_t temp = (uint64_t)mktime(timeinfo) * 1000 + PARAM_INT(6);
    if (is_localtime) {
#ifdef _WIN32
        temp -= _timezone * 1000;
#else
        temp -= timezone  * 1000;
#endif
    }
    RETURN_NUMBER((NUMBER)temp);
END_IMPL

CONCEPT_FUNCTION_IMPL(formatdate, 2)
    T_NUMBER(formatdate, 0)
    T_STRING(formatdate, 1)

    char buffer [80];
    time_t tempp = (time_t)(PARAM(0)/1000);
    struct tm tmbuf;
    struct tm *timeinfo = gmtime_r(&tempp, &tmbuf);
    int size = strftime(buffer, 80, PARAM(1), timeinfo);
    if (size > 0) {
        RETURN_BUFFER(buffer, size);
    } else {
        RETURN_STRING(buffer);
    }
END_IMPL

CONCEPT_FUNCTION_IMPL(log_log, 4)
    T_NUMBER(log_log, 0)
    T_STRING(log_log, 1)
    T_NUMBER(log_log, 2)
    T_STRING(log_log, 3)
    PIFAlizator *PIF = (PIFAlizator *)PARAMETERS->PIF;
    ConceptLogContext *context = get_log_context(PIF);
    if (!context) {
        RETURN_NUMBER(0);
        return 0;
    }
    log_log(PIF, PARAM_INT(0), PARAM(1), PARAM_INT(2), PARAM(3));
    RETURN_NUMBER(1);
END_IMPL

CONCEPT_FUNCTION_IMPL(log_level, 1)
    T_NUMBER(log_level, 0)
    PIFAlizator *PIF = (PIFAlizator *)PARAMETERS->PIF;
    ConceptLogContext *context = get_log_context(PIF);
    if (!context) {
        RETURN_NUMBER(0);
        return 0;
    }
    context->loglevel = PARAM_INT(0);
    RETURN_NUMBER(1);
END_IMPL

CONCEPT_FUNCTION_IMPL(log_quiet, 1)
    T_NUMBER(log_quiet, 0)
    PIFAlizator *PIF = (PIFAlizator *)PARAMETERS->PIF;
    ConceptLogContext *context = get_log_context(PIF);
    if (!context) {
        RETURN_NUMBER(0);
        return 0;
    }
    context->quiet = PARAM_INT(0) ? 1 : 0;
    RETURN_NUMBER(1);
END_IMPL

CONCEPT_FUNCTION_IMPL(log_colors, 1)
    T_NUMBER(log_quiet, 0)
    PIFAlizator *PIF = (PIFAlizator *)PARAMETERS->PIF;
    ConceptLogContext *context = get_log_context(PIF);
    if (!context) {
        RETURN_NUMBER(0);
        return 0;
    }
    context->colors = PARAM_INT(0) ? 1 : 0;
    RETURN_NUMBER(1);
END_IMPL

CONCEPT_FUNCTION_IMPL(log_file, 1)
    T_STRING(log_quiet, 0)
    int is_set = log_use_file((PIFAlizator *)PARAMETERS->PIF, PARAM(0));
    RETURN_NUMBER(is_set);
END_IMPL

CONCEPT_FUNCTION_IMPL(allocinfo, 0)
    CREATE_ARRAY(RESULT);
#ifdef USE_DLMALLOC
    struct dl_mallinfo info = FAST_MALLINFO(PARAMETERS->PIF);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "arena", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)info.arena);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ordblks", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)info.ordblks);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "smblks", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)info.smblks);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "hblks", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)info.hblks);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "hblkhd", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)info.hblkhd);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "usmblks", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)info.usmblks);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "fsmblks", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)info.fsmblks);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "uordblks", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)info.uordblks);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "keepcost", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)info.keepcost);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "footprint", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)FAST_FOOTPRINT(PARAMETERS->PIF));
#endif
END_IMPL


CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(__resolve, 2, 3)
    T_OBJECT(__resolve, 0)

    if (PARAMETERS->COUNT == 2)
        Invoke(INVOKE_RESOLVE, PARAMETERS->PIF, (void *)PARAM(0), PARAMETER(1), (void *)NULL);
    else
        Invoke(INVOKE_RESOLVE, PARAMETERS->PIF, (void *)PARAM(0), PARAMETER(1), PARAMETER(2));
    RETURN_NUMBER(0);
END_IMPL

#ifndef DISABLE_INTROSPECTION
CONCEPT_FUNCTION_IMPL(bytecode, 1)
    T_DELEGATE(bytecode, 0)
    CREATE_ARRAY(RESULT);
    VariableDATA *VD = PARAMETER(0);
    const ClassCode *CC = ((struct CompiledClass *)delegate_Class(VD->CLASS_DATA))->_Class;
    int         relocation = delegate_Member(VD->CLASS_DATA);
    ClassMember *pMEMBER_i = relocation ? CC->pMEMBERS [relocation - 1] : 0;
    Array *arr = (Array *)(RESULT->CLASS_DATA);
    if ((pMEMBER_i) && (pMEMBER_i->OPTIMIZER)) {
        RuntimeOptimizedElement *OElist = ((struct Optimizer *)pMEMBER_i->OPTIMIZER)->CODE;
        ParamList *Parameters           = ((struct Optimizer *)pMEMBER_i->OPTIMIZER)->PARAMS;
        int       count = ((struct Optimizer *)pMEMBER_i->OPTIMIZER)->codeCount;
        TreeContainer *tc = (TreeContainer *)malloc(sizeof(TreeContainer) * count);
        for (INTEGER it = 0; it < count; it++) {
            RuntimeOptimizedElement *OE    = &OElist[it];
            VariableDATA *VD = Array_ModuleGet(arr, it);
            if (VD) {
                CREATE_ARRAY(VD);
                VariableDATA *VD2 = Array_ModuleGet((Array *)VD->CLASS_DATA, "op");
                VD2->CLASS_DATA = plainstring_new_str(GetKeyWord(OE->Operator_ID));
                VD2->TYPE = VARIABLE_STRING;

                if (OE->OperandLeft_ID > 0) {
                    if (OE->Operator_ID == KEY_NEW) {
                        VD2 = Array_ModuleGet((Array *)VD->CLASS_DATA, "class");
                        ClassCode *CC = (ClassCode *)((PIFAlizator *)PARAMETERS->PIF)->ClassList->Item(OE->OperandLeft_ID - 1);
                        if (CC) {
                            VD2->CLASS_DATA = plainstring_new_str(CC->NAME.c_str());
                            VD2->TYPE = VARIABLE_STRING;
                        } else {
                            VD2->NUMBER_DATA = OE->OperandReserved_ID - 1;
                        }
                    } else
                    if ((OE->Operator_ID == KEY_DLL_CALL) && (OE->OperandLeft_ID > 0)) {
                        VD2 = Array_ModuleGet((Array *)VD->CLASS_DATA, "class");
                        ClassCode *CC = (ClassCode *)((PIFAlizator *)PARAMETERS->PIF)->ClassList->Item(OE->OperandLeft_ID - 1);
                        if (CC) {
                            VD2->CLASS_DATA = plainstring_new_str(CC->NAME.c_str());
                            plainstring_add((struct plainstring *)VD2->CLASS_DATA, " (");
                            plainstring_add_int((struct plainstring *)VD2->CLASS_DATA, OE->OperandLeft_ID - 1);
                            plainstring_add((struct plainstring *)VD2->CLASS_DATA, ")");
                            VD2->TYPE = VARIABLE_STRING;
                        } else {
                            VD2->NUMBER_DATA = OE->OperandLeft_ID - 1;
                        }
                    } else {
                        VD2 = Array_ModuleGet((Array *)VD->CLASS_DATA, "left");
                        VD2->NUMBER_DATA = OE->OperandLeft_ID - 1;
                    }
                } else {
                    if (OE->Operator_ID == KEY_NEW) {
                        VD2 = Array_ModuleGet((Array *)VD->CLASS_DATA, "class");
                        VD2->CLASS_DATA = plainstring_new_str("[]");
                        VD2->TYPE = VARIABLE_STRING;
                    }
                }

                if (OE->OperandRight_ID > 0) {
                    VD2 = Array_ModuleGet((Array *)VD->CLASS_DATA, "right");
                    if (OE->OperandRight_PARSE_DATA.Length()) {
                        VD2->CLASS_DATA = plainstring_new_str(OE->OperandRight_PARSE_DATA.c_str());
                        plainstring_add((struct plainstring *)VD2->CLASS_DATA, " (");
                        plainstring_add_int((struct plainstring *)VD2->CLASS_DATA, OE->OperandRight_ID - 1);
                        plainstring_add((struct plainstring *)VD2->CLASS_DATA, ")");
                        VD2->TYPE = VARIABLE_STRING;
                    } else
                    if (((OE->Operator_ID == KEY_SEL) || (OE->Operator_ID == KEY_DLL_CALL)) && (((PIFAlizator *)PARAMETERS->PIF)->GeneralMembers)) {
                        const char *member = ((PIFAlizator *)PARAMETERS->PIF)->GeneralMembers->Item(OE->OperandRight_ID - 1);
                        if (member) {
                            VD2->CLASS_DATA = plainstring_new_str(member);
                            plainstring_add((struct plainstring *)VD2->CLASS_DATA, " (");
                            plainstring_add_int((struct plainstring *)VD2->CLASS_DATA, OE->OperandRight_ID - 1);
                            plainstring_add((struct plainstring *)VD2->CLASS_DATA, ")");
                            VD2->TYPE = VARIABLE_STRING;
                        } else
                            VD2->NUMBER_DATA = OE->OperandRight_ID - 1;
                    } else
                        VD2->NUMBER_DATA = OE->OperandRight_ID - 1;
                }

                if (OE->OperandReserved_ID > 0) {
                    const char *key = "reserved";
                    if ((OE->Operator_ID == KEY_OPTIMIZED_IF) || (OE->Operator_ID == KEY_OPTIMIZED_GOTO) || ((OE->Operator_ID == KEY_OPTIMIZED_TRY_CATCH)))
                        key = "jump";
                    else
                    if ((OE->Operator_ID == KEY_NEW) || (OE->Operator_ID == KEY_SEL) || (OE->Operator_ID == KEY_DLL_CALL))
                        key = "parameter_list";
                    VD2 = Array_ModuleGet((Array *)VD->CLASS_DATA, key);
                    VD2->NUMBER_DATA = OE->OperandReserved_ID;
                }

                if (OE->Result_ID > 0) {
                    VD2 = Array_ModuleGet((Array *)VD->CLASS_DATA, "result");
                    VD2->NUMBER_DATA = OE->Result_ID - 1;
                }

                if (OE->Operator_DEBUG_INFO_LINE) {
                    VD2 = Array_ModuleGet((Array *)VD->CLASS_DATA, "line");
                    VD2->NUMBER_DATA = OE->Operator_DEBUG_INFO_LINE;
                }
            }
        }
    }
END_IMPL

CONCEPT_FUNCTION_IMPL(bytedata, 1)
    T_DELEGATE(bytedata, 0)
    CREATE_ARRAY(RESULT);
    VariableDATA *VD = PARAMETER(0);
    const ClassCode *CC = ((struct CompiledClass *)delegate_Class(VD->CLASS_DATA))->_Class;
    int         relocation = delegate_Member(VD->CLASS_DATA);
    ClassMember *pMEMBER_i = relocation ? CC->pMEMBERS [relocation - 1] : 0;
    Array *arr = (Array *)(RESULT->CLASS_DATA);
    if ((pMEMBER_i) && (pMEMBER_i->OPTIMIZER)) {
        RuntimeVariableDESCRIPTOR *datalist = ((struct Optimizer *)pMEMBER_i->OPTIMIZER)->DATA;
        int count = ((struct Optimizer *)pMEMBER_i->OPTIMIZER)->dataCount;
        for (INTEGER it = 0; it < count; it++) {
            RuntimeVariableDESCRIPTOR *data = &datalist[it];
            VariableDATA *VD = Array_ModuleGet(arr, it);
            if (VD) {
                int type = data->TYPE;
                if (type < 0)
                    type = -type;

                switch (type) {
                    case VARIABLE_STRING:
                        if (data->BY_REF == 2)
                            VD->CLASS_DATA = plainstring_new_str("const \"");
                        else
                            VD->CLASS_DATA = plainstring_new_str("\"");
                        plainstring_addbuffer((struct plainstring *)VD->CLASS_DATA, data->value.c_str(), data->value.Length());
                        plainstring_add_char((struct plainstring *)VD->CLASS_DATA, '"');
                        VD->TYPE = VARIABLE_STRING;
                        break;
                    case VARIABLE_NUMBER:
                        VD->NUMBER_DATA = data->nValue;
                        break;
                    case VARIABLE_ARRAY:
                        VD->CLASS_DATA = plainstring_new_str("[array]");
                        VD->TYPE = VARIABLE_STRING;
                        break;
                    case VARIABLE_CLASS:
                        VD->CLASS_DATA = plainstring_new_str("[object]");
                        VD->TYPE = VARIABLE_STRING;
                        break;
                    case VARIABLE_DELEGATE:
                        VD->CLASS_DATA = plainstring_new_str("[delegate]");
                        VD->TYPE = VARIABLE_STRING;
                        break;
                    default:
                        VD->CLASS_DATA = plainstring_new_str("[unknown]");
                        VD->TYPE = VARIABLE_STRING;
                        break;
                }
            }
        }
    }
END_IMPL

CONCEPT_FUNCTION_IMPL(callstack, 0)
    CREATE_ARRAY(RESULT);
    PIFAlizator *PIF = (PIFAlizator *)PARAMETERS->PIF;
    GCRoot  *root        = PIF->GCROOT;
    SCStack *STACK_TRACE = NULL;
    if (root) {
        STACK_TRACE = (SCStack *)root->STACK_TRACE;
        if (STACK_TRACE)
            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;
    }
    INTEGER i = 0;
    while (STACK_TRACE) {
        ClassMember *CM = (ClassMember *)STACK_TRACE->CM;
        VariableDATA *VD = Array_ModuleGet((Array *)RESULT->CLASS_DATA, i++);
        VD->CLASS_DATA = plainstring_new();
        VD->TYPE = VARIABLE_STRING;
        if (CM) {
            if (CM->Defined_In) {
                plainstring_add((struct plainstring *)VD->CLASS_DATA, ((ClassCode *)CM->Defined_In)->NAME.c_str());
                plainstring_add((struct plainstring *)VD->CLASS_DATA, ".");
            } else {
                plainstring_add((struct plainstring *)VD->CLASS_DATA, "::");
            }
            plainstring_add((struct plainstring *)VD->CLASS_DATA, CM->NAME);
        } else {
            plainstring_add((struct plainstring *)VD->CLASS_DATA, "STATIC/LIBRARY.STATIC_FUNCTION");
        }
        plainstring_add_char((struct plainstring *)VD->CLASS_DATA, ':');
        plainstring_add_int((struct plainstring *)VD->CLASS_DATA, STACK_TRACE->line);
        STACK_TRACE = (SCStack *)STACK_TRACE->PREV;
    }
END_IMPL
#endif

WRAP_INT_FUNCTION(Math, abs)
WRAP_FUNCTION(Math, acos)
WRAP_FUNCTION(Math, acosh)
WRAP_FUNCTION(Math, asin)
WRAP_FUNCTION(Math, asinh)
WRAP_FUNCTION(Math, atan)
WRAP_FUNCTION(Math, atanh)
WRAP_FUNCTION2(Math, atan2)
WRAP_FUNCTION(Math, cbrt)
WRAP_FUNCTION(Math, ceil)
WRAP_FUNCTION(Math, cos)
WRAP_FUNCTION(Math, cosh)
WRAP_FUNCTION(Math, exp)
WRAP_FUNCTION(Math, expm1)
WRAP_FUNCTION(Math, floor)
WRAP_FUNCTION2(Math, hypot)
WRAP_FUNCTION(Math, log)
WRAP_FUNCTION(Math, log1p)
WRAP_FUNCTION(Math, log10)
WRAP_FUNCTION(Math, log2)
WRAP_FUNCTION2(Math, pow)
WRAP_FUNCTION(Math, round)
WRAP_FUNCTION(Math, sin)
WRAP_FUNCTION(Math, sinh)
WRAP_FUNCTION(Math, sqrt)
WRAP_FUNCTION(Math, tan)
WRAP_FUNCTION(Math, tanh)
WRAP_FUNCTION(Math, trunc)
WRAP_FUNCTION0(Math, rand)
WRAP_VOID_INT_FUNCTION(Math, srand)
// ==================================================== //
// BUILT-IN INITIALIZATION (CONSTANTS, etc.)            //
// ==================================================== //


void BUILTININIT(void *pif) {
    PIFAlizator *PIF = (PIFAlizator *)pif;
    if (!PIF)
        return;
    PIF->DefineConstant("_GB", "GLOBALS()", 0);
    PIF->DefineConstant("_argv", "CLArg()", 0);

    DEFINE2(M_PI, "3.14159265358979323846");
    DEFINE2(M_E, "2.71828182845904523536");
    DEFINE2(M_LOG2E, "1.44269504088896340736");
    DEFINE2(M_LOG10E, "0.434294481903251827651");
    DEFINE2(M_LN2, "0.693147180559945309417");
    DEFINE2(M_LN10, "2.30258509299404568402");
    DEFINE2(M_PI_2, "1.57079632679489661923");
    DEFINE2(M_PI_4, " 0.785398163397448309616");
    DEFINE2(M_1_PI, " 0.318309886183790671538");
    DEFINE2(M_2_PI, "0.636619772367581343076");
    DEFINE2(M_2_SQRTPI, "1.12837916709551257390");
    DEFINE2(M_SQRT2, "1.41421356237309504880");
    DEFINE2(M_SQRT1_2, "0.707106781186547524401");
    DEFINE2(DBL_EPSILON, "2.2204460492503131e-016");
    DEFINE2(DBL_MIN, "2.2250738585072014e-308");
    DEFINE2(DBL_MIN_EXP, "(-1021)");
    DEFINE2(DBL_MIN_10_EXP, "(-307)");
    DEFINE2(DBL_MAX, "1.7976931348623158e+308");
    DEFINE2(DBL_MAX_EXP, "1024");
    DEFINE2(DBL_MAX_10_EXP, "308");
    DEFINE2(DBL_DIG, "15");
    DEFINE2(DBL_MANT_DIG, "53");

    DEFINE2(LOG_TRACE, "0");
    DEFINE2(LOG_DEBUG, "1");
    DEFINE2(LOG_INFO, "2");
    DEFINE2(LOG_WARN, "3");
    DEFINE2(LOG_ERROR, "4");
    DEFINE2(LOG_FATAL, "5");
    DEFINE2(LOG_THIS, "@filename, @line");

    srand((unsigned)time(NULL));
    tzset();
}

int BUILTINOBJECTS(void *pif, const char *classname) {
    PIFAlizator *PIF = (PIFAlizator *)pif;
    // check if privates are enabled (recursive)
    if ((!PIF) || (!classname) || (PIF->enable_private))
        return 0;

    BUILTINCLASS("RegExp", "class RegExp{private var h;property lastIndex{get getLastIndex}RegExp(str,f=0,var e=null){this.h=RE_create(str,f,e);}test(str){return RE_test(this.h,str);}exec(str){return RE_exec(this.h,str);}getLastIndex(){return RE_lastindex(this.h);}finalize(){RE_done(this.h);}}");
    BUILTINCLASS("Math", "class Math{"
        DECLARE_WRAPPER(Math, abs)
        DECLARE_WRAPPER(Math, acos)
        DECLARE_WRAPPER(Math, acosh)
        DECLARE_WRAPPER(Math, asin)
        DECLARE_WRAPPER(Math, asinh)
        DECLARE_WRAPPER(Math, atan)
        DECLARE_WRAPPER(Math, atanh)
        DECLARE_WRAPPER2(Math, atan2)
        DECLARE_WRAPPER(Math, cbrt)
        DECLARE_WRAPPER(Math, ceil)
        DECLARE_WRAPPER(Math, cos)
        DECLARE_WRAPPER(Math, cosh)
        DECLARE_WRAPPER(Math, exp)
        DECLARE_WRAPPER(Math, expm1)
        DECLARE_WRAPPER(Math, floor)
        DECLARE_WRAPPER2(Math, hypot)
        DECLARE_WRAPPER(Math, log)
        DECLARE_WRAPPER(Math, log1p)
        DECLARE_WRAPPER(Math, log10)
        DECLARE_WRAPPER(Math, log2)
        DECLARE_WRAPPER(Math, pow)
        DECLARE_WRAPPER(Math, round)
        DECLARE_WRAPPER(Math, sin)
        DECLARE_WRAPPER(Math, sqrt)
        DECLARE_WRAPPER(Math, tan)
        DECLARE_WRAPPER(Math, tanh)
        DECLARE_WRAPPER(Math, trunc)

        DECLARE_WRAPPER0(Math, rand)
        DECLARE_WRAPPER_VOID(Math, srand)
    "}");

    BUILTINCLASS("Date", ""
        "class Date {"
            "private var year;"
            "private var month;"
            "private var day;"
            "private var hours;"
            "private var minutes;"
            "private var seconds;"
            "private var ms;"

            "Date(year = -1, month = 0, day = 0, hours = 0, minutes = 0, seconds = 0, ms = 0) {"
                "if ((typeof year == \"class\") && (classof year == \"Date\")) {"
                    "this.year = year.year;"
                    "this.month = year.month;"
                    "this.day = year.day;"
                    "this.hours = year.hours;"
                    "this.minutes = year.minutes;"
                    "this.seconds = year.seconds;"
                    "this.ms = year.ms;"
                    "return;"
                "} else "
                "if (typeof year == \"string\")"
                    "year = value year;"
                "else "
                "if (typeof year == \"array\") {"
                    "this.year = year[\"year\"];"
                    "this.month = year[\"month\"];"
                    "this.day = year[\"day\"];"
                    "this.hours = year[\"hours\"];"
                    "this.minutes = year[\"minutes\"];"
                    "this.seconds = year[\"seconds\"];"
                    "this.ms = year[\"milliseconds\"];"
                    "return;"
                "} "
                "if (typeof year == \"numeric\") {"
                    "if (year < 0)"
                        "year = milliseconds();"
                    "if ((!month) && (!day) && (!hours) && (!minutes) && (!seconds) && (!ms)) {"
                        "var timestamp = floor(year / 1000);"
                        "var arr = localtime(timestamp);"
                        "this.year = 1900 + arr[\"tm_year\"];"
                        "this.month = arr[\"tm_mon\"];"
                        "this.day = arr[\"tm_mday\"];"
                        "this.hours = arr[\"tm_hour\"];"
                        "this.minutes = arr[\"tm_min\"];"
                        "this.seconds = arr[\"tm_sec\"];"
                        "this.ms = year - timestamp * 1000;"
                    "} else {"
                        "while (ms >= 1000) {"
                            "seconds ++;"
                            "ms -= 1000;"
                        "}"
                        "while (seconds >= 60) {"
                            "minutes ++;"
                            "seconds -= 60;"
                        "}"
                        "while (minutes >= 60) {"
                            "hours ++;"
                            "minutes -= 60;"
                        "}"
                        "while (hours >= 24) {"
                            "day++;"
                            "hours -= 24;"
                        "}"
                        "while (month >= 12) {"
                            "year++;"
                            "month -= 12;"
                        "}"
                        "this.year = year;"
                        "this.month = month;"
                        "this.day = day;"
                        "this.hours = hours;"
                        "this.minutes = minutes;"
                        "this.seconds = seconds;"
                        "this.ms = ms;"
                    "}"
                "}"
            "}"

            "static now() {"
                "return milliseconds();"
            "}"

            "static getTimezoneOffset() {"
                "return timezone();"
            "}"

            "getTime() {"
                "return __epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms, true);"
            "}"

            "getDate() {"
                "return this.day;"
            "}"

            "getFullYear() {"
                "return this.year;"
            "}"

            "getMonth() {"
                "return this.month;"
            "}"

            "getHours() {"
                "return this.hours;"
            "}"

            "getMilliseconds() {"
                "return this.ms;"
            "}"

            "getMinutes() {"
                "return this.minutes;"
            "}"

            "getSeconds() {"
                "return this.seconds;"
            "}"

            "getDay() {"
                "var arr = localtime(__epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms, true) / 1000);"
                "return arr[\"tm_wday\"];"
            "}"

            "setFullYear(number year, number month, number day) {"
                "this.year = year;"
                "if (month >= 0)"
                    "this.month = month;"
                "if (day >= 0)"
                    "this.day = day;"
            "}"

            "setDate(number date) {"
                "if (year < 0)"
                    "year = 0;"
                "this.day = date;"
            "}"

            "setMinutes(number minutes, number sec = -1, number ms = -1) {"
                "if (minutes < 0)"
                    "minutes = 0;"
                "this.minutes = minutes;"
                "if (sec >= 0)"
                    "this.seconds = sec;"
                "if (ms >= 0)"
                    "this.ms = ms;"
            "}"

            "setMonth(number month, number day = -1) {"
                "if (month < 0)"
                    "month = 0;"
                "this.month = month;"
                "if (day >= 0)"
                    "this.day = day;"
            "}"

            "setSeconds(number seconds, number ms = -1) {"
                "if (seconds < 0)"
                    "seconds = 0;"
                "this.seconds = seconds;"
                "if (ms >= 0)"
                    "this.ms = ms;"
            "}"

            "UTC() {"
                "var utc = __epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms, false);"
                "if (utc < 0)"
                    "utc = 0;"
                "return utc;"
            "}"

            "private U(k) {"
                "var timestamp = floor(this.UTC() / 1000);"
                "var arr = localtime(timestamp);"
                "return arr[k];"
            "}"

            "getUTCDate() {"
                "return this.U(\"tm_mday\");"
            "}"

            "getUTCDay() {"
                "return this.U(\"tm_wday\");"
            "}"

            "getUTCFullYear() {"
                "return this.U(\"tm_year\");"
            "}"

            "getUTCHours() {"
                "return this.U(\"tm_hour\");"
            "}"

            "getUTCMinutes() {"
                "return this.U(\"tm_min\");"
            "}"
            
            "getUTCMonth() {"
                "return this.U(\"tm_mon\");"
            "}"

            "getUTCMilliseconds() {"
                "return this.ms;"
            "}"

            "getUTCSeconds() {"
                "return this.seconds;"
            "}"

            "private L0(n, z = 2) {"
                "n = \"\" + n;"
                "while (length n < z)"
                    "n = \"0\" + n;"
                "return n;"
            "}"

            "toISOString() {"
                "return formatdate(this.UTC(), \"%Y-%m-%dT%H:%M:%S.\") + this.L0(this.ms) + \"Z\";"
            "}"

            "toUTCString() {"
                "return formatdate(this.UTC(), \"%a, %d %b %Y %H:%M:%S\") + \" GMT\";"
            "}"

            "toDateString() {"
                "return formatdate(__epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms, true), \"%a %b %d %Y\");"
            "}"

            "toTimeString() {"
                "var tz = -timezone();"
                "return formatdate(__epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms, true), \"%H:%M:%S GMT+\") + this.L0(floor(tz / 60)) + this.L0(tz % 60);"
            "}"

            "operator-(x) {"
                "var stamp = this.getTime();"
                "if (typeof x == \"class\") stamp -= x.getTime(); else stamp -= x;\n"
                "return stamp;"
            "}"
        "}"
    );
    BUILTINCLASS("Error", ""
        "class Error {"
            "var stack;"
            "var message;"
            "var code;"
            "var previous;"
            "var filename;"
            "var line;"

            "Error(message = \"\", code = 0, filename = \"\", line = 0, previous = null) {"
                "this.message = message;"
                "this.code = code;"
                "this.previous = previous;"
                "this.stack = callstack();"
                "this.filename = filename;"
                "this.line = line;"
            "}"

            "protected __tostr(v, max_level = 3) {"
                "if (max_level <= 0)"
                    "return \"\";"
                "switch (typeof v) {"
                    "case \"string\":"
                        "return v;"
                    "case \"numeric\":"
                        "return \"\" + v;"
                    "case \"class\":"
                        "return classof v;"
                    "case \"array\":"
                        "var a = \"\";"
                        "for (var i = 0; i < length v; i++)"
                            "a += __tostr(v[i], max_level - 1) + \"\\n\";"
                        "return a;"
                    "default:"
                        "return typeof v;"
                "}"
            "}"

            "ToString() {"
                "var filename_str = \"\";"
                "if (filename)"
                    "filename_str = \"File: \" + filename + \":\" + line + \"\\n\";"
                "if (code)"
                    "return this.__tostr(this.message) + \"\\nCode: \" + this.__tostr(this.code) + \"\\n${filename_str}Call stack:\\n\" + this.__tostr(this.stack);"
                "return this.__tostr(this.message) + \"\\n${filename_str}Call stack:\\n\" + this.__tostr(this.stack);"
            "}"
        "}"
    );
    BUILTINCLASS("console", ""
        "class console {"
            "static arraytostring(array) {"
                "var str = \"\";"
                "for (var i = 0; i < length array; i++) {"
                    "var e = array[i];"
                     "if (str)"
                         "str += \", \";"
                    "switch (typeof e) {"
                        "case \"string\":"
                        "case \"numeric\":"
                            "str += e;"
                            "break;"
                        "case \"class\":"
                            "str += classof e;"
                            "break;"
                        "default:"
                            "str += typeof e;"
                    "}"
                "}"
                "return str;"
            "}"
            "static tostring(what) {"
                "switch (typeof what) {"
                    "case \"string\":"
                        "return what;"
                    "case \"numeric\":"
                        "return \"\" + what;"
                    "case \"class\":"
                        "return classof what;"
                    "case \"array\":"
                        "return \"[\" + console::arraytostring(what) + \"]\";"
                    "default:"
                        "return typeof what;"
                "}"
            "}"
            "static trace(filename, line = 0, what = \"\") {"
                "if (!what) {"
                "   what = filename;"
                "   filename = \"builtin\";"
                "   line = 1;"
                "}"
                "log_log(LOG_TRACE, filename, line, console.tostring(what));"
            "}"
            "static info(filename, line = 0, what = \"\") {"
                "if (!what) {"
                "   what = filename;"
                "   filename = \"builtin\";"
                "   line = 1;"
                "}"
                "log_log(LOG_INFO, filename, line, console.tostring(what));"
            "}"
            "static log(filename, line = 0, what = \"\") {"
                "if (!what) {"
                "   what = filename;"
                "   filename = \"builtin\";"
                "   line = 1;"
                "}"
                "log_log(LOG_DEBUG, filename, line, console.tostring(what));"
            "}"
            "static debug(filename, line = 0, what = \"\") {"
                "console::log(filename, line, what);"
            "}"
            "static warn(filename, line = 0, what = \"\") {"
                "if (!what) {"
                "   what = filename;"
                "   filename = \"builtin\";"
                "   line = 1;"
                "}"
                "log_log(LOG_WARN, filename, line, console.tostring(what));"
            "}"
            "static error(filename, line = 0, what = \"\") {"
                "if (!what) {"
                "   what = filename;"
                "   filename = \"builtin\";"
                "   line = 1;"
                "}"
                "log_log(LOG_ERROR, filename, line, console.tostring(what));"
            "}"
            "static critical(filename, line = 0, what = \"\") {"
                "if (!what) {"
                "   what = filename;"
                "   filename = \"builtin\";"
                "   line = 1;"
                "}"
                "log_log(LOG_FATAL, filename, line, console.tostring(what));"
            "}"
            "static colors(enabled = 1) {"
                "log_colors(enabled);"
            "}"
            "static level(level) {"
                "log_level(level);"
            "}"
            "static use_file(string path) {"
                "return log_file(path);"
            "}"
            "static quiet(quiet = 1) {"
                "log_quiet(quiet);"
            "}"
        "}"
    );

    return 0;
}

void *BUILTINADDR(void *pif, const char *name, unsigned char *is_private) {
    if ((!name) || (!name[0]))
        return NULL;

    PIFAlizator *PIF = (PIFAlizator *)pif;
    if (is_private)
        *is_private = 0;

    BUILTIN(GLOBALS);
    BUILTIN(CLArg);
    BUILTIN(___CONCEPT_INTERFACE_HELPER_GENERATE_UNIQUE_ID);

    // math
    BUILTIN(abs)
    BUILTIN(acos)
    BUILTIN(acosh)
    BUILTIN(asin)
    BUILTIN(asinh)
    BUILTIN(atan)
    BUILTIN(atanh)
    BUILTIN(atan2)
    BUILTIN(cbrt)
    BUILTIN(ceil)
    BUILTIN(cos)
    BUILTIN(cosh)
    BUILTIN(exp)
    BUILTIN(expm1)
    BUILTIN(floor)
    BUILTIN(hypot)
    BUILTIN(log)
    BUILTIN(log1p)
    BUILTIN(log10)
    BUILTIN(log2)
    BUILTIN(pow)
    BUILTIN(round)
    BUILTIN(sin)
    BUILTIN(sinh)
    BUILTIN(sqrt)
    BUILTIN(tan)
    BUILTIN(tanh)
    BUILTIN(trunc)
    BUILTIN(rand)
    BUILTIN(srand)

    // time
    BUILTIN(milliseconds)
    BUILTIN(localtime)
    BUILTIN(gmtime)
    BUILTIN(timezone)
    BUILTIN(time)
    BUILTIN(__epoch)
    BUILTIN(formatdate)

    BUILTIN(CheckReachability);
    BUILTIN(MemoryInfo);
    BUILTIN(allocinfo);

    BUILTIN(log_log);
    BUILTIN(log_level);
    BUILTIN(log_quiet);
    BUILTIN(log_colors);
    BUILTIN(log_file);
#ifndef DISABLE_INTROSPECTION
    BUILTIN(bytecode)
    BUILTIN(bytedata)
    BUILTIN(callstack)
#endif
    BUILTIN(__resolve)

    if ((!PIF) || (PIF->enable_private)) {
        if (is_private)
            *is_private = 1;

        // regex
        BUILTIN(RE_create);
        BUILTIN(RE_exec);
        BUILTIN(RE_test);
        BUILTIN(RE_lastindex);
        BUILTIN(RE_done);

        if (is_private)
            *is_private = 0;
    }

    return NULL;
}

void BUILTINDONE() {
    // nothing
}
