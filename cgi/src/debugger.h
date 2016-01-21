#ifndef __DEBUGGER_H
#define __DEBUGGER_H

#define MAX_BREAKPOINTS    0xFF

#include "AnsiString.h"
#ifdef _WIN32
 #include <io.h>
//#include <windows.h>
 #include <winsock2.h>
 #include <sys/utime.h>
#else
 #include <unistd.h>

 #define HANDLE     void *
 #define HMODULE    HANDLE

 #define SOCKET     int
#endif

typedef struct {
    long       CLIENT_NOTICE; // VARIABILA SETATA DE INTERPRETOR !!!

    int        last_line;
    int        debug_type;
    int        run_to_line;
    AnsiString FileName;
    AnsiString run_to_filename;
    void       *context_id;
    SOCKET     sock;
    SOCKET     DEBUG_SOCKET;
} ClientDebugInfo;


typedef struct {
    AnsiString filename;
    int        line;
} BREAKPOINT;


typedef int (*GET_VARIABLE_PROC)(int operation, void *VDESC, void *CONTEXT, int Depth, char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);
typedef int (*DEBUGGER_CALLBACK)(void *VDESC, void *CONTEXT, int Depth, int line, char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE);

int RemoteNotify(int pipe_out, int apid, int msg_id, int len, char *data);
ClientDebugInfo *InitDebuger(AnsiString filename, int pipe_out, int pipe_in, int papid);
int CONCEPT_DEBUGGER_TRAP(void *VDESC, void *CONTEXT, int Depth, int line, char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE);
#endif // __DEBUGGER_H
