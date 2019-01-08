#ifndef __ANSI_TYPES_H
#define __ANSI_TYPES_H
#include <stdint.h>

#define INTEGER          int
#define SYS_INT          intptr_t
#define POINTER          void *
#define NUMBER           double
#define SYSTEM_SOCKET    INTEGER

typedef void   VariableDATA;

typedef struct {
    INTEGER *PARAM_INDEX;
    INTEGER COUNT;
    void    *HANDLER;
} ParamList;

typedef INTEGER (*EXTERNAL_CALL)(ParamList *PARAMETERS, VariableDATA **LOCAL_CONTEXT, VariableDATA *RESULT);
#endif // __ANSI_TYPES_H
