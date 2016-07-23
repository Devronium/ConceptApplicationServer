#ifndef __PARAMETERS_H
#define __PARAMETERS_H

#include "AnsiString.h"

typedef struct Parameters {
    AnsiString     Sender;
    int            ID;
    AnsiString     Target;
    AnsiString     Value;
    void           *Owner;
    signed char    Flags;
    void           *UserData;
} TParameters;

#endif
