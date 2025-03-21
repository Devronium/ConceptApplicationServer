#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

CONCEPT_FUNCTION(ROSConnect);
CONCEPT_FUNCTION(ROSDisconnect);

CONCEPT_FUNCTION(ROSQuery);
CONCEPT_FUNCTION(ROSResult);
}
#endif // __LIBRARY_H
