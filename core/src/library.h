#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...
#ifdef _WIN32
 #include <windows.h>
#else
 #define SOCKET    int
#endif

#include "ConceptTypes.h"

void *GetStaticData();
void SetStaticData(void *data);

#ifdef _WIN32
 #define CONCEPT_DLL_API            __declspec(dllexport) int _cdecl
 #define CONCEPT_DLL_API_HANDLER    __declspec(dllexport) void *_cdecl
#else
 #define CONCEPT_DLL_API            int
 #define CONCEPT_DLL_API_HANDLER    void *
#endif

extern "C" {
typedef int (*NOTIFY_PARENT)(int pipe_out, int apid, int msg_id, int len, char *data);

// other functions declared here
CONCEPT_DLL_API SetArguments(int argc, char **argv);
CONCEPT_DLL_API LastResult();
CONCEPT_DLL_API Concept_Execute(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, void *userdata);
CONCEPT_DLL_API Concept_Compile(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent);
CONCEPT_DLL_API Concept_Execute2(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, CHECK_POINT cp, void *userdata);

CONCEPT_DLL_API Concept_ExecuteBuffer(char *buffer, int len, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, CHECK_POINT cp, void *result, void *module_list, void *userdata);
CONCEPT_DLL_API Concept_GetSocket();

CONCEPT_DLL_API Concept_CachedInit(char *filename);
CONCEPT_DLL_API Concept_CachedDone(char *filename);

CONCEPT_DLL_API SetNotifyParent(NOTIFY_PARENT np);
CONCEPT_DLL_API SetDirectPipe(int direct_pipe);
CONCEPT_DLL_API SetCheckpointFunction(CHECK_POINT cp);

CONCEPT_DLL_API_HANDLER Concept_Execute3_Init(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, void *userdata);
CONCEPT_DLL_API Concept_Execute3_Run(void *PTR, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent);
CONCEPT_DLL_API Concept_Execute3_RunClone(void *PTR, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, int direct_pipe);
CONCEPT_DLL_API Concept_Execute3_Done(void *PTR);
}
#endif // __LIBRARY_H

