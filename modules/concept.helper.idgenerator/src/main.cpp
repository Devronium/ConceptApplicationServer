//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
//-----------------------------------------------------------------------------------
//static INTEGER		_UNIQUE_ID=0;

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT____CONCEPT_INTERFACE_HELPER_GENERATE_UNIQUE_ID CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)": '__CONCEPT_INTERFACE_HELPER_GENERATE_UNIQUE_ID' parameters error. This fuction takes  no parameters.";

    /*// General variables
       NUMBER       NUMBER_DUMMY;
       char *       STRING_DUMMY;
       INTEGER      TYPE;

       // Result
       long	_C_call_result;
       // Specific variables

       // Variable type check

       // function call
       _C_call_result=_UNIQUE_ID++;*/
    unsigned int _C_call_result = Invoke(INVOKE_GETID, PARAMETERS->HANDLER);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
