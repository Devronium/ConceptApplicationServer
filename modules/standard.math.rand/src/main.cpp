//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <time.h>
#include "randomc.h"

CRandomMersenne *RanGen = 0;

void InitRandomEngine() {
    if (!RanGen)
        RanGen = new CRandomMersenne((int)time(0));
}

//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InitRandomEngine();
    return 0;
}

CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        if (RanGen) {
            delete RanGen;
            RanGen = 0;
        }
    }
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RandomSeed, 1)
    T_NUMBER(RandomSeed, 0)
    InitRandomEngine();
    if (!RanGen)
        return (void *)"RandomSeed: Random number generator initialisation failed";
    RanGen->RandomInit(PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RandomInteger, 2)
    T_NUMBER(RandomInteger, 0)
    T_NUMBER(RandomInteger, 1)

    InitRandomEngine();
    if (!RanGen)
        return (void *)"RandomInteger: Random number generator initialisation failed";

    RETURN_NUMBER(RanGen->IRandom(PARAM_INT(0), PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RandomIntegerX, 2)
    T_NUMBER(RandomIntegerX, 0)
    T_NUMBER(RandomIntegerX, 1)

    InitRandomEngine();
    if (!RanGen)
        return (void *)"RandomIntegerX: Random number generator initialisation failed";

    RETURN_NUMBER(RanGen->IRandomX(PARAM_INT(0), PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RandomFloat, 0)
    InitRandomEngine();
    if (!RanGen)
        return (void *)"RandomFloat: Random number generator initialisation failed";

    RETURN_NUMBER(RanGen->Random())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RandomBit, 0)
    InitRandomEngine();
    if (!RanGen)
        return (void *)"RandomBit: Random number generator initialisation failed";

    RETURN_NUMBER(RanGen->BRandom())
END_IMPL
//------------------------------------------------------------------------

