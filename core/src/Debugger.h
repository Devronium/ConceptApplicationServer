#ifndef __DEBUGGER_H
#define __DEBUGGER_H

typedef int (*GET_VARIABLE_PROC)(int operation, void **VDESC, void **CONTEXT, int Depth, char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);
typedef int (*DEBUGGER_CALLBACK)(void *VDESC, void *CONTEXT, int Depth, int line, char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE);

int GetVariableByName(int operation, void **VDESC, void **CONTEXT, int Depth, char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);
#endif

