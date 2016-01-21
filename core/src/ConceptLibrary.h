#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...
#ifdef _WIN32
 #include <windows.h>
#else
 #define SOCKET    int
#endif

#include "ConceptTypes.h"

#ifdef _WIN32
 #define CONCEPT_DLL_API    __declspec(dllimport) int _cdecl
#else
 #define CONCEPT_DLL_API    int
#endif

extern "C" {
typedef int (*NOTIFY_PARENT)(int pipe_out, int apid, int msg_id, int len, char *data);

// other functions declared here
CONCEPT_DLL_API SetArguments(int argc, char **argv);
CONCEPT_DLL_API Concept_Execute(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, void *userdata);
CONCEPT_DLL_API Concept_Compile(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent);
CONCEPT_DLL_API Concept_Execute2(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, CHECK_POINT cp, void *userdata);

CONCEPT_DLL_API SetNotifyParent(NOTIFY_PARENT np);
}
#endif // __LIBRARY_H

