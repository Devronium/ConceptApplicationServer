//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <string.h>
#include <stdio.h>

extern "C" {
void captcha(unsigned char im[70 * 200], unsigned char *l);
void makegif(unsigned char im[70 * 200], unsigned char *gif);
}
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(Captcha, 1)
    unsigned char im[70 * 200];
    unsigned char l[6];
    unsigned char gif[17646];

    captcha(im, l);
    makegif(im, gif);

    SET_BUFFER(0, (char *)gif, sizeof(gif))

    RETURN_STRING((char *)l);
END_IMPL
//-----------------------------------------------------//
