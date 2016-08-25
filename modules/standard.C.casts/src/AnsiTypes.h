#ifndef __ANSI_TYPES_H
#define __ANSI_TYPES_H

#define INTEGER          int
#define SYS_INT          long
#define POINTER          void *
#define NUMBER           double
#define SYSTEM_SOCKET    INTEGER

#include <stdint.h>

typedef void   VariableDATA;

typedef struct {
    INTEGER *PARAM_INDEX;
    INTEGER COUNT;
} ParamList;

typedef INTEGER (*EXTERNAL_CALL)(ParamList *PARAMETERS, VariableDATA **LOCAL_CONTEXT, VariableDATA *RESULT);
#endif // __ANSI_TYPES_H
