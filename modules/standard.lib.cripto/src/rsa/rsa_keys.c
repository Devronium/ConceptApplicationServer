/*	rsa_lib/rsa_keys.c
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
#include "rsa.h"
#ifdef _WIN32
 #include <malloc.h> // malloc()
#else
 #include <stdlib.h> // malloc()
#endif
#include <memory.h>  // memcpy()
#include <string.h>  // strncmp()

// this is function is simply what it said
// return ZERO is success .... else error
int RSA_generate_keys(char *privatekey,
                      char *publiccertificate,
                      int  *privatekey_length,
                      int  *publiccertificate_length,
                      int  primeBitSize) {
    pri_key *prikey = NULL;
    pub_key *pubkey = NULL;
    BIGNUM  *p1     = NULL;
    BIGNUM  *q1     = NULL;
    BIGNUM  *temp   = NULL;
    BN_CTX  *ctx    = NULL;
    int     result;

    //char tempBuffer[buffer_size]; // adjust when required ...

    // check the primeBitSize
    if (primeBitSize < 256)    // minimum
        return 0;

    if (primeBitSize > 2048)    // maximum
        return 0;

    if (primeBitSize % 256)    // primeBitSize must be multiply of 256
        return 0;


    prikey = RSA_private_key_new();
    if (prikey == NULL)
        goto err;

    pubkey = RSA_public_certificate_new();
    if (pubkey == NULL)
        goto err;

    p1   = BN_new();
    q1   = BN_new();
    temp = BN_new();
    ctx  = BN_CTX_new();

    // generate a fix e = 65537 = 0x010001
    if (!BN_set_word(prikey->e, 65537))
        goto err;

generate_key:

    // generate p .. DOES NOT use safe prime --> (p-1)/2 = prime
    // to enable safe prime change the value 0 (zero) to 1 (one)
    prikey->p = BN_generate_prime(NULL, primeBitSize, 0, NULL, NULL, NULL, NULL);

    // generate q
    prikey->q = BN_generate_prime(NULL, primeBitSize, 0, NULL, NULL, NULL, NULL);

    if (!BN_cmp(prikey->p, prikey->q))
        goto generate_key;         // p = q ... this is very rare happen .. the chance is very small

    // generate p1 = p - 1
    BN_sub(p1, prikey->p, BN_value_one());

    // generate q1 = q - 1
    BN_sub(q1, prikey->q, BN_value_one());

    // check whether p and q is valid for this fix e
    BN_gcd(temp, prikey->e, p1, ctx);
    if (!BN_is_word(temp, 1))
        goto generate_key;

    BN_gcd(prikey->qinv, prikey->e, q1, ctx);     // use qinv .. prevent double use
    if (!BN_is_word(prikey->qinv, 1))
        goto generate_key;

    // generate dp
    if (BN_mod_inverse(prikey->dp, prikey->e, p1, ctx) == NULL)
        goto err;

    // generate dq
    if (BN_mod_inverse(prikey->dq, prikey->e, q1, ctx) == NULL)
        goto err;

    // generate qinv
    if (BN_mod_inverse(prikey->qinv, prikey->q, prikey->p, ctx) == NULL)
        goto err;

    // store all the variable required in public certificate structure	=======================
    if (BN_copy(pubkey->e, prikey->e) == NULL)
        goto err;

    if (!BN_mul(pubkey->n, prikey->p, prikey->q, ctx))    // n = p * q
        goto err;

    //store the modulus size
    prikey->modulus_length = (unsigned short)(primeBitSize * 2);
    pubkey->modulus_length = prikey->modulus_length;

    result = RSA_save_public_certificate_to_buffer(pubkey, publiccertificate, *publiccertificate_length);
    if (result == 0)
        return 0;
    *publiccertificate_length = result;

    result = RSA_save_private_key_to_buffer(prikey, privatekey, *privatekey_length);
    if (result == 0)
        return 0;
    *privatekey_length = result;

    // all allocated variables must be free here before exit
    RSA_public_certificate_free(&pubkey);
    RSA_private_key_free(&prikey);
    BN_free(p1);
    BN_free(q1);
    BN_free(temp);
    BN_CTX_free(ctx);

    return 1;
err:
    RSA_public_certificate_free(&pubkey);
    RSA_private_key_free(&prikey);
    BN_free(p1);
    BN_free(q1);
    BN_free(temp);
    BN_CTX_free(ctx);

    return 0;      // error
}
