//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT____CONCEPT_INTERFACE_HELPER_GENERATE_UNIQUE_ID CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)": '__CONCEPT_INTERFACE_HELPER_GENERATE_UNIQUE_ID' parameters error. This fuction takes  no parameters.";

    unsigned int _C_call_result = Invoke(INVOKE_GETID, PARAMETERS->HANDLER);
    SetVariable(RESULT, VARIABLE_NUMBER, empty_string, _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
