//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <time.h>
#include <locale.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h> //I've ommited this line.

    #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
        #define DELTA_EPOCH_IN_MICROSECS  116444736000000000Ui64
    #else
        #define DELTA_EPOCH_IN_MICROSECS  116444736000000000ULL
    #endif

    #define localtime_r(a,b)    localtime(a)
    #define gmtime_r(a,b)       gmtime(a)
    #define ctime_r(a,b)        ctime(a)
    #define asctime_r(a,b)      asctime(a)

    struct timezone {
        int tz_minuteswest;
        int tz_dsttime;
    };

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
#endif
//---------------------------------------------------------------------------
INVOKE_CALL InvokePtr = 0;
//---------------------------------------------------------------------------
#define TM_YEAR_BASE    1900
#define ALT_E           0x01
#define ALT_O           0x02
#define LEGAL_ALT(x)    { if (alt_format & ~(x)) return (0); }

static const char *day[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
    "Friday", "Saturday"
};
static const char *abday[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char *mon[12] = {
    "January", "February",  "March",   "April",    "May", "June", "July",
    "August",  "September", "October", "November", "December"
};
static const char *abmon[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char *am_pm[2] = {
    "AM", "PM"
};


static int conv_num(const char **buf, int *dest, int llim, int ulim) {
    int result = 0;

    /* The limit also determines the number of valid digits. */
    int rulim = ulim;

    if ((**buf < '0') || (**buf > '9'))
        return 0;

    do {
        result *= 10;
        result += *(*buf)++ - '0';
        rulim  /= 10;
    } while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

    if ((result < llim) || (result > ulim))
        return 0;

    *dest = result;
    return 1;
}

char *ci_strptime(const char *buf, const char *fmt, struct tm *tm) {
    char       c;
    const char *bp;
    size_t     len = 0;
    int        alt_format, i, split_year = 0;

    bp = buf;

    while ((c = *fmt) != '\0') {
        /* Clear `alternate' modifier prior to new conversion. */
        alt_format = 0;

        /* Eat up white-space. */
        if (isspace(c)) {
            while (isspace(*bp))
                bp++;

            fmt++;
            continue;
        }

        if ((c = *fmt++) != '%')
            goto literal;


again:          switch (c = *fmt++) {
            case '%':           /* "%%" is converted to "%". */
literal:
                if (c != *bp++)
                    return 0;
                break;

            /*
             * "Alternative" modifiers. Just set the appropriate flag
             * and start over again.
             */
            case 'E':           /* "%E?" alternative conversion modifier. */
                LEGAL_ALT(0);
                alt_format |= ALT_E;
                goto again;

            case 'O':           /* "%O?" alternative conversion modifier. */
                LEGAL_ALT(0);
                alt_format |= ALT_O;
                goto again;

            /*
             * "Complex" conversion rules, implemented through recursion.
             */
            case 'c':           /* Date and time, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = ci_strptime(bp, "%x %X", tm)))
                    return 0;
                break;

            case 'D':           /* The date as "%m/%d/%y". */
                LEGAL_ALT(0);
                if (!(bp = ci_strptime(bp, "%m/%d/%y", tm)))
                    return 0;
                break;

            case 'R':           /* The time as "%H:%M". */
                LEGAL_ALT(0);
                if (!(bp = ci_strptime(bp, "%H:%M", tm)))
                    return 0;
                break;

            case 'r':           /* The time in 12-hour clock representation. */
                LEGAL_ALT(0);
                if (!(bp = ci_strptime(bp, "%I:%M:%S %p", tm)))
                    return 0;
                break;

            case 'T':           /* The time as "%H:%M:%S". */
                LEGAL_ALT(0);
                if (!(bp = ci_strptime(bp, "%H:%M:%S", tm)))
                    return 0;
                break;

            case 'X':           /* The time, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = ci_strptime(bp, "%H:%M:%S", tm)))
                    return 0;
                break;

            case 'x':           /* The date, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = ci_strptime(bp, "%m/%d/%y", tm)))
                    return 0;
                break;

            /*
             * "Elementary" conversion rules.
             */
            case 'A':           /* The day of week, using the locale's form. */
            case 'a':
                LEGAL_ALT(0);
                for (i = 0; i < 7; i++) {
                    /* Full name. */
                    len = strlen(day[i]);
                    if (strncasecmp(day[i], bp, len) == 0)
                        break;

                    /* Abbreviated name. */
                    len = strlen(abday[i]);
                    if (strncasecmp(abday[i], bp, len) == 0)
                        break;
                }

                /* Nothing matched. */
                if (i == 7)
                    return 0;

                tm->tm_wday = i;
                bp         += len;
                break;

            case 'B':           /* The month, using the locale's form. */
            case 'b':
            case 'h':
                LEGAL_ALT(0);
                for (i = 0; i < 12; i++) {
                    /* Full name. */
                    len = strlen(mon[i]);
                    if (strncasecmp(mon[i], bp, len) == 0)
                        break;

                    /* Abbreviated name. */
                    len = strlen(abmon[i]);
                    if (strncasecmp(abmon[i], bp, len) == 0)
                        break;
                }

                /* Nothing matched. */
                if (i == 12)
                    return 0;

                tm->tm_mon = i;
                bp        += len;
                break;

            case 'C':           /* The century number. */
                LEGAL_ALT(ALT_E);
                if (!(conv_num(&bp, &i, 0, 99)))
                    return 0;

                if (split_year) {
                    tm->tm_year = (tm->tm_year % 100) + (i * 100);
                } else {
                    tm->tm_year = i * 100;
                    split_year  = 1;
                }
                break;

            case 'd':           /* The day of month. */
            case 'e':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
                    return 0;
                break;

            case 'k':           /* The hour (24-hour clock representation). */
                LEGAL_ALT(0);

            /* FALLTHROUGH */
            case 'H':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
                    return 0;
                break;

            case 'l':           /* The hour (12-hour clock representation). */
                LEGAL_ALT(0);

            /* FALLTHROUGH */
            case 'I':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
                    return 0;
                if (tm->tm_hour == 12)
                    tm->tm_hour = 0;
                break;

            case 'j':           /* The day of year. */
                LEGAL_ALT(0);
                if (!(conv_num(&bp, &i, 1, 366)))
                    return 0;
                tm->tm_yday = i - 1;
                break;

            case 'M':           /* The minute. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
                    return 0;
                break;

            case 'm':           /* The month. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &i, 1, 12)))
                    return 0;
                tm->tm_mon = i - 1;
                break;

            case 'p':           /* The locale's equivalent of AM/PM. */
                LEGAL_ALT(0);
                /* AM? */
                if (strcasecmp(am_pm[0], bp) == 0) {
                    if (tm->tm_hour > 11)
                        return 0;

                    bp += strlen(am_pm[0]);
                    break;
                }
                /* PM? */
                else if (strcasecmp(am_pm[1], bp) == 0) {
                    if (tm->tm_hour > 11)
                        return 0;

                    tm->tm_hour += 12;
                    bp          += strlen(am_pm[1]);
                    break;
                }

                /* Nothing matched. */
                return 0;

            case 'S':           /* The seconds. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
                    return 0;
                break;

            case 'U':           /* The week of year, beginning on sunday. */
            case 'W':           /* The week of year, beginning on monday. */
                LEGAL_ALT(ALT_O);

                /*
                 * XXX This is bogus, as we can not assume any valid
                 * information present in the tm structure at this
                 * point to calculate a real value, so just check the
                 * range for now.
                 */
                if (!(conv_num(&bp, &i, 0, 53)))
                    return 0;
                break;

            case 'w':           /* The day of week, beginning on sunday. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
                    return 0;
                break;

            case 'Y':           /* The year. */
                LEGAL_ALT(ALT_E);
                if (!(conv_num(&bp, &i, 0, 9999)))
                    return 0;

                tm->tm_year = i - TM_YEAR_BASE;
                break;

            case 'y':           /* The year within 100 years of the epoch. */
                LEGAL_ALT(ALT_E | ALT_O);
                if (!(conv_num(&bp, &i, 0, 99)))
                    return 0;

                if (split_year) {
                    tm->tm_year = ((tm->tm_year / 100) * 100) + i;
                    break;
                }
                split_year = 1;
                if (i <= 68)
                    tm->tm_year = i + 2000 - TM_YEAR_BASE;
                else
                    tm->tm_year = i + 1900 - TM_YEAR_BASE;
                break;

            /*
             * Miscellaneous conversions.
             */
            case 'n':           /* Any kind of white-space. */
            case 't':
                LEGAL_ALT(0);
                while (isspace(*bp))
                    bp++;
                break;


            default:            /* Unknown/unsupported conversion. */
                return 0;
        }
    }

    /* LINTED functional specification */
    return (char *)bp;
}

void Get_TM(void *arr, struct tm *mytime) {
    INTEGER type = 0;
    char    *str = 0;
    NUMBER  nr   = 0;

    if (!InvokePtr)
        return;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "tm_hour", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        mytime->tm_hour = nr;
    else
        mytime->tm_hour = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "tm_isdst", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        mytime->tm_isdst = nr;
    else
        mytime->tm_isdst = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "tm_mday", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        mytime->tm_mday = nr;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "tm_min", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        mytime->tm_min = nr;
    else
        mytime->tm_min = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "tm_mon", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        mytime->tm_mon = nr;
    else
        mytime->tm_mon = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "tm_sec", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        mytime->tm_sec = nr;
    else
        mytime->tm_sec = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "tm_wday", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        mytime->tm_wday = nr;
    else
        mytime->tm_wday = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "tm_yday", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        mytime->tm_yday = nr;
    else
        mytime->tm_yday = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "tm_year", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        mytime->tm_year = nr;
    else
        mytime->tm_year = 0;
}

//---------------------------------------------------------------------------
static time_t base_time = 0;
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    base_time = tv.tv_sec;

    //DEFINE_ECONSTANT(CLOCKS_PER_SEC)
    DEFINE_SCONSTANT("CLOCKS_PER_SEC", "1000")
    DEFINE_ECONSTANT(LC_ALL)
    DEFINE_ECONSTANT(LC_COLLATE)
    DEFINE_ECONSTANT(LC_CTYPE)
    DEFINE_ECONSTANT(LC_MONETARY)
    DEFINE_ECONSTANT(LC_NUMERIC)
    DEFINE_ECONSTANT(LC_TIME)

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__clock CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)"clock() takes no parameters";

    INTEGER c = clock();
    c = ((double)c / CLOCKS_PER_SEC) * 1000;
    SetVariable(RESULT, VARIABLE_NUMBER, "", (INTEGER)c);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__microseconds CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)"microseconds() takes no parameters";

    struct timeval tv;
    gettimeofday(&tv, NULL);
    tv.tv_sec -= base_time;
    SetVariable(RESULT, VARIABLE_NUMBER, "", (uint64_t)((uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec));
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__asctime CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'asctime' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    char *_C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"asctime: parameter 1 should be of STATIC NUMBER type";

    // function call
    time_t    timevar = (time_t)nParam0;
    struct tm tmbuf;
    struct tm *ptm    = gmtime_r(&timevar, &tmbuf);
    char buffer[28];
    if (ptm) {
        _C_call_result = (char *)asctime_r(ptm, buffer);
    } else
        _C_call_result = "";

    SetVariable(RESULT, VARIABLE_STRING, _C_call_result, 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__ctime CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'ctime' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    char *_C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"ctime: parameter 1 should be of STATIC NUMBER type";

    // function call
    char buffer[28];
    time_t tempp = (time_t)nParam0;
    _C_call_result = (char *)ctime_r(&tempp, buffer);

    SetVariable(RESULT, VARIABLE_STRING, _C_call_result, 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__difftime CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)": 'difftime' parameters error. This fuction takes 2 parameters.";

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
        return (void *)"difftime: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"difftime: parameter 2 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (double)difftime((time_t)nParam0, (time_t)nParam1);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__localtime CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'localtime' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    time_t _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"localtime: parameter 1 should be of STATIC NUMBER type";

    // function call
    time_t    tempp = (time_t)nParam0;
    struct tm *timeinfo;
    struct tm tmbuf;
    timeinfo = localtime_r(&tempp, &tmbuf);

    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, RESULT)))
        return (void *)"Failed to INVOKE_CREATE_ARRAY";

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
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__gmtime CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'gmtime' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    time_t _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"gmtime: parameter 1 should be of STATIC NUMBER type";

    // function call
    time_t    tempp = (time_t)nParam0;
    struct tm *timeinfo;
    struct tm tmbuf;
    timeinfo = gmtime_r(&tempp, &tmbuf);

    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, RESULT)))
        return (void *)"Failed to INVOKE_CREATE_ARRAY";

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
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__time CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)": 'time' parameters error. This fuction takes  no parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    time_t _C_call_result;
    // Specific variables

    // Variable type check

    // function call
    _C_call_result = (time_t)time(NULL);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__mktime CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'mktime' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    time_t _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    VariableDATA *param = LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1];
    GetVariable(param, &TYPE, &STRING_DUMMY, &nParam0);

    if (TYPE != VARIABLE_ARRAY)
        return (void *)"mktime: parameter 1 should be of ARRAY type";

    // function call
    struct tm timeinfo;

    Get_TM(param, &timeinfo);

    time_t tempp = mktime(&timeinfo);

    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, param)))
        return (void *)"Failed to INVOKE_CREATE_ARRAY";

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, param, "tm_hour", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo.tm_hour);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, param, "tm_isdst", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo.tm_isdst);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, param, "tm_mday", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo.tm_mday);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, param, "tm_min", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo.tm_min);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, param, "tm_mon", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo.tm_mon);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, param, "tm_sec", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo.tm_sec);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, param, "tm_wday", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo.tm_wday);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, param, "tm_yday", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo.tm_yday);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, param, "tm_year", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo.tm_year);

    RETURN_NUMBER(tempp);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(setlocale, 2)
    T_NUMBER(setlocale, 0)
    T_STRING(setlocale, 1)

    RETURN_STRING(setlocale(PARAM_INT(0), PARAM(1)))
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(localeconv, 0)

    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, RESULT)))
        return (void *)"Failed to INVOKE_CREATE_ARRAY";

    struct lconv *lc = localeconv();
    if (!lc)
        return 0;

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "decimal_point", (INTEGER)VARIABLE_STRING, lc->decimal_point, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "thousands_sep", (INTEGER)VARIABLE_STRING, lc->thousands_sep, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "thousands_sep", (INTEGER)VARIABLE_STRING, lc->thousands_sep, (NUMBER)0);

    AnsiString grouping;
    for (int i = 0; i < 0xFF; i++) {
        char c = lc->grouping[i];
        if (!c)
            break;

        if (i)
            grouping += ",";

        grouping += AnsiString((long)c);
    }
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "grouping", (INTEGER)VARIABLE_STRING, grouping.c_str(), (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "int_curr_symbol", (INTEGER)VARIABLE_STRING, lc->int_curr_symbol, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "currency_symbol", (INTEGER)VARIABLE_STRING, lc->currency_symbol, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "mon_decimal_point", (INTEGER)VARIABLE_STRING, lc->mon_decimal_point, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "mon_thousands_sep", (INTEGER)VARIABLE_STRING, lc->mon_thousands_sep, (NUMBER)0);

    grouping = (char *)"";
    for (int i = 0; i < 0xFF; i++) {
        char c = lc->mon_grouping[i];
        if (!c)
            break;

        if (i)
            grouping += ",";

        grouping += AnsiString((long)c);
    }

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "mon_grouping", (INTEGER)VARIABLE_STRING, grouping.c_str(), (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "positive_sign", (INTEGER)VARIABLE_STRING, lc->positive_sign, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "negative_sign", (INTEGER)VARIABLE_STRING, lc->negative_sign, (NUMBER)0);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "int_frac_digits", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)lc->int_frac_digits);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "frac_digits", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)lc->frac_digits);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "p_cs_precedes", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)lc->p_cs_precedes);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "p_sep_by_space", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)lc->p_sep_by_space);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "n_cs_precedes", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)lc->n_cs_precedes);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "n_sep_by_space", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)lc->n_sep_by_space);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "p_sign_posn", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)lc->p_sign_posn);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "n_sign_posn", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)lc->n_sign_posn);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__slocaltime CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'localtime' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    time_t _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"localtime: parameter 1 should be of STATIC NUMBER type";

    // function call
    time_t    tempp = (time_t)nParam0;
    struct tm *timeinfo;
    struct tm tmbuf;

    timeinfo = localtime_r(&tempp, &tmbuf);

    char buffer [80];
    if (timeinfo)
        strftime(buffer, 80, "%c", timeinfo);
    else
        buffer[0] = 0;

    SetVariable(RESULT, VARIABLE_STRING, buffer, 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(strftime, 2)
    T_ARRAY(strftime, 0)
    T_STRING(strftime, 1)
    char res[0xFFF];
    res[0xFFE] = 0;

    struct tm timeinfo;

    Get_TM(PARAMETER(0), &timeinfo);
    strftime(res, 0xFFE, PARAM(1), &timeinfo);

    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(strftime2, 2)
    T_NUMBER(strftime2, 0)
    T_STRING(strftime2, 1)
    char res[0xFFF];
    res[0xFFE] = 0;

    time_t          t         = time(0);
    struct tm tmbuf;
    const struct tm *timeinfo = localtime_r(&t, &tmbuf);
    if (timeinfo)
        strftime(res, 0xFFE, PARAM(1), timeinfo);
    else
        res[0] = 0;

    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(strptime, 2, 3)
    T_STRING(strptime, 0)
    T_STRING(strptime, 1)

    struct tm timeinfo2;
    memset(&timeinfo2, 0, sizeof(timeinfo2));
    struct tm *timeinfo;
    timeinfo2.tm_hour = -1;

    if (!ci_strptime(PARAM(0), PARAM(1), &timeinfo2)) {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, 0);
        }
        RETURN_NUMBER(0);
        return 0;
    }
    int has_time = 1;
    if (timeinfo2.tm_hour == -1) {
        timeinfo2.tm_hour = 0;
        has_time          = 0;
    }
    if (PARAMETERS_COUNT > 2) {
        SET_NUMBER(2, has_time);
    }

    time_t t = mktime(&timeinfo2);
    timeinfo = &timeinfo2;

    CREATE_ARRAY(RESULT);
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
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(strptime2, 2, 3)
    T_STRING(strptime2, 0)
    T_STRING(strptime2, 1)

    struct tm timeinfo2;
    memset(&timeinfo2, 0, sizeof(timeinfo2));
    struct tm *timeinfo;
    time_t    res = 0;
    timeinfo2.tm_hour = -1;

    if (!ci_strptime(PARAM(0), PARAM(1), &timeinfo2)) {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, 0);
        }
        RETURN_NUMBER(0);
        return 0;
    }
    int has_time = 1;
    if (timeinfo2.tm_hour == -1) {
        timeinfo2.tm_hour = 0;
        has_time          = 0;
    }
    if (PARAMETERS_COUNT > 2) {
        SET_NUMBER(2, has_time);
    }

    res = mktime(&timeinfo2);

    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------------------------------------

