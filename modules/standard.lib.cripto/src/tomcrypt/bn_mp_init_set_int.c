#include <tommath_private.h>
#ifdef BN_MP_INIT_SET_INT_C

/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tstdenis82@gmail.com, http://libtom.org
 */

/* initialize and set a digit */
int mp_init_set_int(mp_int *a, unsigned long b) {
    int err;

    if ((err = mp_init(a)) != MP_OKAY) {
        return err;
    }
    return mp_set_int(a, b);
}
#endif

/* $Source$ */
/* $Revision$ */
/* $Date$ */
