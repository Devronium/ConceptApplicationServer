//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"

static char *profile_callback = "__profile";

#define PROFILER_CALL_START    0
#define PROFILER_CALL_END      1
#define PROFILER_CALL_THROW    2

int MT_core = 0;

class MetaContainer {
public:
    void *DELEGATE;
    char *deleg_class;
    char *deleg_member;

    MetaContainer() {
        DELEGATE     = 0;
        deleg_class  = 0;
        deleg_member = 0;
    }
};

#define GET_METACONTAINER                                                               \
    MetaContainer * mc = NULL;                                                          \
    Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)1, &mc);                      \
    if (!mc) {                                                                          \
        mc = new MetaContainer();                                                       \
        Invoke(INVOKE_SETPROTODATA, PARAMETERS->HANDLER, (int)1, mc, destroy_metadata); \
    }

void destroy_metadata(void *data, void *handler) {
    if (data)
        delete (MetaContainer *)data;
}

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    Invoke(INVOKE_PROFILER, (void *)HANDLER, profile_callback);

    DEFINE_ECONSTANT(PROFILER_CALL_START);
    DEFINE_ECONSTANT(PROFILER_CALL_END);
    DEFINE_ECONSTANT(PROFILER_CALL_THROW);

    if (Invoke(INVOKE_MULTITHREADED) == 1)
        MT_core = 1;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__profile, 3)
    GET_METACONTAINER

    if (mc->DELEGATE) {
        T_NUMBER(__profile, 0)
        T_STRING(__profile, 1)
        T_STRING(__profile, 2)

        // avoid recursive call
        if ((strcmp(PARAM(2), mc->deleg_member) || (strcmp(PARAM(1), mc->deleg_class)))) {
            void *RES       = 0;
            void *EXCEPTION = 0;

            if (MT_core)
                Invoke(INVOKE_CALL_DELEGATE_THREAD_SAFE, mc->DELEGATE, &RES, &EXCEPTION, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, (char *)"", PARAM(0), (INTEGER)VARIABLE_STRING, (char *)PARAM(1), (NUMBER)PARAM_LEN(1), (INTEGER)VARIABLE_STRING, (char *)PARAM(2), (NUMBER)PARAM_LEN(2));
            else
                Invoke(INVOKE_CALL_DELEGATE, mc->DELEGATE, &RES, &EXCEPTION, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, (char *)"", PARAM(0), (INTEGER)VARIABLE_STRING, (char *)PARAM(1), (NUMBER)PARAM_LEN(1), (INTEGER)VARIABLE_STRING, (char *)PARAM(2), (NUMBER)PARAM_LEN(2));

            if (EXCEPTION)
                Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);
            Invoke(INVOKE_FREE_VARIABLE, RES);
        }
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ProfilerDelegate, 1)
    T_DELEGATE(ProfilerDelegate, 0)

    GET_METACONTAINER

    if (mc->DELEGATE)
        Invoke(INVOKE_FREE_VARIABLE, mc->DELEGATE);

    Invoke(INVOKE_CREATE_VARIABLE_2, PARAMETERS->HANDLER, &mc->DELEGATE);
    Invoke(INVOKE_SET_VARIABLE, mc->DELEGATE, (INTEGER)VARIABLE_DELEGATE, (void *)PARAM(0), (NUMBER)PARAM_LEN(0));
    Invoke(INVOKE_GET_DELEGATE_NAMES, mc->DELEGATE, &mc->deleg_class, &mc->deleg_member);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ProfilerReset, 0)
    GET_METACONTAINER

    if (mc->DELEGATE)
        Invoke(INVOKE_FREE_VARIABLE, mc->DELEGATE);

    mc->DELEGATE = NULL;
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(CoreMemoryInfo, 0)
    CREATE_ARRAY(RESULT);

    Invoke(INVOKE_PROFILE_MEMORY, PARAMETERS->HANDLER, RESULT);
END_IMPL
//------------------------------------------------------------------------

