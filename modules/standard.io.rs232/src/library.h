#include "stdlibrary.h"
#include <stdlib.h>
#include <stdio.h>

extern "C" {
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS;

CONCEPT_FUNCTION(OpenComport)
CONCEPT_FUNCTION(PollComport)
CONCEPT_FUNCTION(SendComport)
CONCEPT_FUNCTION(CloseComport)
}
