/*	rsa_lib/rsa_lib.c
 *      This RSA library is using BN module from OpenSSL Project (www.openssl.org)
 *      NOTE:
 *      1. This RSA library is stricted to only 2048 bits modulo value (=N).
 *      2. Only use two primes, P and Q (each 1024 bits), DOES NOT use multi prime.
 *      3. This RSA encryption library is DESIGNED for private individual or group use only.
 *      4. It follows the rules in PKCS#1 v2.1 (as per 5th January 2001).
 *      5. Use a slightly different OAEP algorithm, yet it possesses a strong cryptography.
 *      6. The strong/weak point is in BN_RAND_HL.C (random number generator based on time)
 *      7. No hash function.
 *      8. The certificate (public key) is NOT compatible for X509.
 *      Last compiled Wednesday, 26th June 2001 */

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
#include "../bn_openssl097beta1/bn.h"
#include "../bn_openssl097beta1/bn_lcl.h"
#include "rsa.h"
#ifdef _WIN32
 #include <malloc.h>        // malloc()
#else
 #include <stdlib.h>        // malloc()
#endif

pub_key *RSA_public_certificate_new() {
    pub_key *pubkey;

    pubkey = (pub_key *)malloc(sizeof(pub_key));
    if (pubkey == NULL)
        return NULL;

    pubkey->e = BN_new();
    pubkey->n = BN_new();

    return pubkey;
}

void RSA_public_certificate_free(pub_key **pubkey) {
    if (pubkey == NULL)
        return;        // nothing to free

    // check for each sub key ... it should be fine
    BN_free((*pubkey)->e);
    BN_free((*pubkey)->n);

    free(*pubkey);
    *pubkey = NULL;     // last one ... the main

    return;
}

pri_key *RSA_private_key_new() {
    pri_key *prikey = NULL;

    prikey = (pri_key *)malloc(sizeof(pri_key));
    if (prikey == NULL)
        return NULL;

    prikey->dp   = BN_new();
    prikey->dq   = BN_new();
    prikey->e    = BN_new();
    prikey->p    = BN_new();
    prikey->q    = BN_new();
    prikey->qinv = BN_new();

    return prikey;
}

void RSA_private_key_free(pri_key **prikey) {
    if (prikey == NULL)
        return;        // nothing to free

    BN_free((*prikey)->dp);
    BN_free((*prikey)->dq);
    BN_free((*prikey)->e);
    BN_free((*prikey)->p);
    BN_free((*prikey)->q);
    BN_free((*prikey)->qinv);

    free(*prikey);
    prikey = NULL;     // last one ... the main

    return;
}
