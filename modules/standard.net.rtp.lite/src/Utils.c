/*******
**
** FILE INFO:
** project:	RTP_lib
** file:	Utils.c
** started on:	05/01/03
** started by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
**
**
** TODO:
**
** BUGS:
**
** UPDATE INFO:
** updated on:	05/13/03
** updated by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
**
********/


#include                <stdlib.h>
#include                <stdio.h>
#include                <time.h>
#include                "Types.h"


/**
** Random functions
**/
int                     Random_init(void) {
    srand((unsigned)time(NULL));
    return 0;
}

u_int32                 Random32(int type) {
    return rand();
}                               /* random32 */
