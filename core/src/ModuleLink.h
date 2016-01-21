#ifndef __MODULE_LINK_H
#define __MODULE_LINK_H

#include "AnsiString.h"
#include "AnsiList.h"
#include "Codes.h"
#include "ConceptTypes.h"
#include "CompiledClass.h"
#include "PIFAlizator.h"

class PIFAlizator;

#ifdef _WIN32
 #define MODULE_EXTENSION    ".dll"
#else
 #define MODULE_EXTENSION    ".so"
 #define HANDLE              void *
 #define HMODULE             HANDLE
#endif

#define ON_CREATE            "__CONCEPT_MEMORY_MANAGEMENT_CreateContext"
#define ON_DESTROY           "__CONCEPT_MEMORY_MANAGEMENT_DestroyContext"

#define SYSTEM_SOCKET        INTEGER

//
//      function Main() {
//      }
//      }
// }
//

typedef INTEGER (*CALL_BACK_VARIABLE_SET)(VariableDATA *VD, INTEGER TYPE, char *STRING_VALUE, NUMBER NUMBER_VALUE);
typedef INTEGER (*CALL_BACK_VARIABLE_GET)(VariableDATA *VD, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE);

typedef INTEGER (*CALL_BACK_CLASS_MEMBER_GET)(void *CLASS_PTR, char *class_member_name, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE);
typedef INTEGER (*CALL_BACK_CLASS_MEMBER_SET)(void *CLASS_PTR, char *class_member_name, INTEGER TYPE, char *STRING_VALUE, NUMBER NUMBER_VALUE);

typedef INTEGER (*INVOKE_CALL)(INTEGER INVOKE_TYPE, ...);

typedef POINTER (*CONCEPT_MANAGEMENT_CALL)(INTEGER _MANAGEMENT_UNIQUE_ID, void *HANDLER, INVOKE_CALL Invoker);

typedef POINTER (*EXTERNAL_CALL)(ParamListExtra *PARAMETERS, VariableDATA **LOCAL_CONTEXT, VariableDATA *RESULT, CALL_BACK_VARIABLE_SET CallBACKSet, CALL_BACK_VARIABLE_GET CallBACKGet, SYSTEM_SOCKET CLIENT_SOCKET, char *LOCAL_PUBLIC_KEY, char *LOCAL_PRIVATE_KEY, char *REMOTE_PUBLIC_KEY, CALL_BACK_CLASS_MEMBER_SET CallBACKClassSet, CALL_BACK_CLASS_MEMBER_GET CallBACKClassGet, INVOKE_CALL Invoke);

INTEGER ImportModule(AnsiString& MODULE_MASK, AnsiList *Errors, INTEGER line, AnsiString FILENAME, AnsiList *TARGET, PIFAlizator *Sender, INTEGER _NO_ERROR_REPORT = 0);
INTEGER UnImportModule(HMODULE hMODULE, PIFAlizator *Sender = 0);
SYS_INT LinkFunction(char *FUNCTION_NAME, AnsiList *TARGET, void **CACHED_hDLL);
void DoneLinking();

INTEGER SetVariable(VariableDATA *VD, INTEGER TYPE, char *STRING_VALUE, NUMBER NUMBER_VALUE);
INTEGER GetVariable(VariableDATA *VD, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE);

INTEGER GetClassMember(void *CLASS_PTR, char *class_member_name, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE);
INTEGER SetClassMember(void *CLASS_PTR, char *class_member_name, INTEGER TYPE, char *STRING_VALUE, NUMBER NUMBER_VALUE);

INTEGER Invoke(INTEGER INVOKE_TYPE, ...);
#endif // __MODULE_LINK_H

