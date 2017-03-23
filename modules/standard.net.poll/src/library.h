#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
    CONCEPT_FUNCTION(PollOpen)
    CONCEPT_FUNCTION(PollAdd)
    CONCEPT_FUNCTION(PollRemove)
    CONCEPT_FUNCTION(PollWait)
    CONCEPT_FUNCTION(PollClose)
}
#endif // __LIBRARY_H
