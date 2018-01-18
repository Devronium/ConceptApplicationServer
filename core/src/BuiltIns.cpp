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
            uintptr_t delta = ep - sp;
            if (offset >= 0) {
                intptr_t relative_offset = (uintptr_t)ep - (uintptr_t)PARAM(1);
                if (relative_offset < 0)
                    relative_offset = PARAM_LEN(1);
                JS_setreglastindex(reg, relative_offset);
            }
            RETURN_BUFFER(sp, delta);
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
    RETURN_NUMBER(ms);
END_IMPL

CONCEPT_FUNCTION_IMPL(timezone, 0)
#ifdef _WIN32
    RETURN_NUMBER(_timezone/60);
#else
    RETURN_NUMBER(timezone/60);
#endif
END_IMPL

CONCEPT_FUNCTION_IMPL(time, 0)
    RETURN_NUMBER(time(NULL));
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

CONCEPT_FUNCTION_IMPL(__epoch, 7)
    T_NUMBER(__epoch, 0);
    T_NUMBER(__epoch, 1);
    T_NUMBER(__epoch, 2);
    T_NUMBER(__epoch, 3);
    T_NUMBER(__epoch, 4);
    T_NUMBER(__epoch, 5);
    T_NUMBER(__epoch, 6);

    struct tm tmbuf;
    time_t tempp = 0;
    struct tm *timeinfo = localtime_r(&tempp, &tmbuf);

    timeinfo->tm_year = PARAM_INT(0) - 1900;
    timeinfo->tm_mon = PARAM_INT(1);
    timeinfo->tm_mday = PARAM_INT(2);
    timeinfo->tm_hour = PARAM_INT(3);
    timeinfo->tm_min = PARAM_INT(4);
    timeinfo->tm_sec = PARAM_INT(5);
    uint64_t temp = (uint64_t)mktime(timeinfo) * 1000 + PARAM_INT(6);
    RETURN_NUMBER(temp);
END_IMPL

CONCEPT_FUNCTION_IMPL(formatdate, 2)
    T_NUMBER(formatdate, 0)
    T_STRING(formatdate, 1)

    char buffer [80];
    time_t tempp = (time_t)(PARAM(0)/1000);
    struct tm tmbuf;
    struct tm *timeinfo = localtime_r(&tempp, &tmbuf);
    int size = strftime(buffer, 80, PARAM(1), timeinfo);
    if (size > 0) {
        RETURN_BUFFER(buffer, size);
    } else {
        RETURN_STRING(buffer);
    }
END_IMPL

WRAP_FUNCTION(Math, abs)
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

    srand((unsigned)time(NULL));
    tzset();
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
		        "return __epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms);"
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
		        "var arr = localtime(__epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms) / 1000);"
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
		        "var utc = __epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms) + timezone() * 60000;"
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
		        "return formatdate(__epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms), \"%a %b %d %Y\");"
	        "}"

	        "toTimeString() {"
		        "var tz = -timezone();"
		        "return formatdate(__epoch(this.year, this.month, this.day, this.hours, this.minutes, this.seconds, this.ms), \"%H:%M:%S GMT+\") + this.L0(floor(tz / 60)) + this.L0(tz % 60);"
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
    BUILTIN(CheckReachability);
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