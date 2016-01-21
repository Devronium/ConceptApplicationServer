/*	Random number generator
 *      WARNING NOTE: this has not been thoroughtly tested using appropriate random number test.
 *                    everyone is welcome to test it and please do so, please tell me the result
 *                                if the result is not good, I will make another algorithm
 *                                because this random value generator is the MOST important part of this cryptography library.
 *      This random value is based on time, the reason I use this are:
 *      1. Any hacker should NOT known when exactly the program start.
 *      2. Each user machine is different, the speed is also different,
 *         therefore there is a different of time in a few seconds.
 *      3. This is fast.
 *      4. It is 32 bits, compare to rand() which is 16 bits.
 *      Last compiled Thursday, 11 October 2001 */

/* Written by Hariyanto Lim
 * hmaxf_urlcr@yahoo.com
 * www.geocities.com/hmaxf_urlcr
 * ====================================================================
 * Copyright (c) 2001.  HMaxF Project, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Copyright remains to Hariyanto Lim, and as such any Copyright notices in
 *	  the code are not to be removed. If this package is used in a product,
 *    Hariyanto Lim should be given attribution as the author of the parts of the library used.
 *    This can be in the form of a textual message at program startup or
 *    in documentation (online or textual) provided with the package.
 * ===================================================================
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL I, HARIYANTO LIM, THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bn.h"
#include "hmf_rand.h"

int BN_generate_random_bit(BIGNUM *target, int bits, int top, int bottom) {
    int      totalwords = (bits + 31) / 32;
    int      bitleft    = bits % 32;
    int      result;
    BN_ULONG *dtop;

    // reserve space
    if (bn_wexpand(target, totalwords) == NULL)
        return -1;          // program error
    // set the size ... why bn_wexpand DOES NOT do this ?
    target->top = totalwords;

    // get pointer to the last value .. because for small optimization
    dtop = &target->d[totalwords - 1];

    // fill with random value ... all blocks
    random32_init(time(NULL));     // initiate the random generator
    for (result = 0; result < totalwords; result++)
        target->d[result] = random32_bit(32);

    // get only the lower value of the uppest block
    if (bitleft) {
        result = ((1 << bitleft) - 1);

        *dtop &= result;
    }

    // set the top bit
    if (top) {
        if (bitleft)
            *dtop |= (1 << (bitleft - 1));
        else
            *dtop |= (1 << 31);
    }

    // set the bottom bit
    if (bottom)
        target->d[0] |= 1;

    return 0;      // finished and success
}
