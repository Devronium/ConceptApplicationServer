/*	rsa_lib/rsa_encode.c
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
#include <memory.h>  // memcpy()
#ifdef _WIN32
 #include <malloc.h> // malloc()
#else
 #include <stdlib.h> // malloc()
#endif

#include "../bn_openssl097beta1/bn.h"
#include "rsa.h"

/*	result is an allocated buffer, the result_size is for comparison
 *      whether the buffer size is enough or not
 *      the return value is the REAL result size required */
unsigned long RSA_verify(char *source, unsigned long source_size,
                         char *result, unsigned long result_size,
                         char *pubkey_content, unsigned long pubkey_content_size) {
    unsigned long i;
    unsigned long total_block;
    BIGNUM        *sourcebn            = BN_new();
    BIGNUM        *target_oaep_decoded = BN_new();
    BIGNUM        *targetbn            = BN_new();
    BN_CTX        *ctx    = BN_CTX_new();
    char          *target = NULL;
    char          *original_target_pointer = NULL;
    unsigned long target_size, final_size;
    pub_key       *pubkey = RSA_public_certificate_new();
    int           chiper_block_length_byte, message_block_length_byte, chiper_BN_ULONG_length, message_BN_ULONG_length;

    // load public certificate from buffer
    if (!RSA_load_public_certificate_from_buffer(pubkey, pubkey_content, pubkey_content_size))
        goto err;

    //if(!load_public_certificate_structure_from_buffer(pubkey, pubkey_content, pubkey_content_size))
    //	goto err;

    // define the rest of the variable required
    chiper_block_length_byte  = (pubkey->modulus_length / 8);    // divide by 8 .. convert bit to byte
    chiper_BN_ULONG_length    = chiper_block_length_byte / 4;
    message_block_length_byte = chiper_block_length_byte - 16;   // chiper = 128-bit of random + message;
    // -16 = 8 * 16 = 128-bits of random
    message_BN_ULONG_length = message_block_length_byte / 4;     // convert to 32-bits group

    // source_size MUST BE multiply of chiper_block_length_byte
    total_block = source_size / chiper_block_length_byte;

    // reserve target buffer
    target_size = total_block * message_block_length_byte;
    target      = (char *)malloc(target_size);
    if (target == NULL)
        goto err;

    memset(target, 0, target_size);     // clear .. IMPORTANT
    original_target_pointer = target;

    //reserve a fix space for sourcebn
    bn_wexpand(sourcebn, chiper_BN_ULONG_length);
    sourcebn->top = chiper_BN_ULONG_length;

    // reserve a fix space for the decoded target
    bn_wexpand(target_oaep_decoded, message_BN_ULONG_length);
    target_oaep_decoded->top = message_BN_ULONG_length;

    // start to verify block by block
    for (i = 0; i < source_size; i += chiper_block_length_byte) {
        // convert char* to BIGNUM*
        memcpy(sourcebn->d, source + i, chiper_block_length_byte);

        // decrypt
        if (RSA_public_certificate_do_1_block(targetbn, sourcebn, pubkey, ctx))
            goto err;

        // decode it ... ignore return value .. because it SHOULD be no error return
        OAEP_decode(targetbn, target_oaep_decoded, message_block_length_byte * 8);

        // save it to real char* target
        memcpy(target, target_oaep_decoded->d, message_block_length_byte);

        target += message_block_length_byte;         // increase counter for next save pointer
    }

    target = original_target_pointer;     //rewind

    // remove padding
    final_size = RSA_remove_padding(target, target, target_size, target_size); //, pubkey->modulus_length / 8);

    if (result_size < final_size)                                              // check size
        goto err;

    memcpy(result, target, final_size);     // copy the buffer to where it supposed to be

    // free the allocated memory
    RSA_public_certificate_free(&pubkey);
    free(target);
    BN_free(sourcebn);
    BN_free(targetbn);
    BN_free(target_oaep_decoded);
    BN_CTX_free(ctx);

    return final_size;

err:
    target = original_target_pointer;     //rewind

    RSA_public_certificate_free(&pubkey);
    free(target);
    BN_free(sourcebn);
    BN_free(targetbn);
    BN_free(target_oaep_decoded);
    BN_CTX_free(ctx);

    return 0;      // error
}

/*	result is an allocated buffer, the result_size is for comparison
 *      whether the buffer size is enough or not
 *      the return value is the REAL result size required */
unsigned long RSA_sign(char *source, unsigned long source_size,
                       char *result, unsigned long result_size,
                       char *prikey_content, unsigned long prikey_content_size) {
    unsigned long i;
    unsigned long total_block;
    BIGNUM        *sourcebn            = BN_new();
    BIGNUM        *source_oaep_encoded = BN_new();
    BIGNUM        *targetbn            = BN_new();
    BN_CTX        *ctx                     = BN_CTX_new();
    char          *target                  = NULL;
    char          *source_padded           = NULL;
    char          *original_target_pointer = NULL;
    unsigned long source_padded_length, target_size;
    pri_key       *prikey = RSA_private_key_new();
    int           chiper_block_length_byte, message_block_length_byte, chiper_BN_ULONG_length, message_BN_ULONG_length;

    // load private key from buffer
    if (!RSA_load_private_key_from_buffer(prikey, prikey_content, prikey_content_size))
        goto err;

    // convert the private key raw data to rsa private key structure
    //if(!load_private_key_structure_from_buffer(prikey, prikey_content, prikey_content_size))
    //	goto err;

    // define the rest of the variable required
    chiper_block_length_byte  = (prikey->modulus_length / 8);    // divide by 8 .. convert bit to byte
    chiper_BN_ULONG_length    = chiper_block_length_byte / 4;
    message_block_length_byte = chiper_block_length_byte - 16;   // chiper = 128-bit of random + message;
    // -16 = 8 * 16 = 128-bits of random
    message_BN_ULONG_length = message_block_length_byte / 4;     // convert to 32-bits group

    // insert padding ...
    // check if there is left over or not
    if (source_size % message_block_length_byte)
        source_padded_length = ((source_size / message_block_length_byte) + 1) * message_block_length_byte;
    else
        source_padded_length = (source_size / message_block_length_byte) * message_block_length_byte;

    // allocated moemry
    source_padded = (char *)malloc(source_padded_length);
    if (source_padded == NULL)
        goto err;

    if (!RSA_insert_padding(source, source_padded, source_size, source_padded_length, source_padded_length - source_size))
        goto err;

    total_block = source_padded_length / message_block_length_byte;

    // reserve target buffer
    target_size = total_block * chiper_block_length_byte;
    target      = (char *)malloc(target_size);
    if (target == NULL)
        goto err;

    memset(target, 0, target_size);     // clear ... important
    original_target_pointer = target;

    // reserve a fix length sourcebn
    bn_wexpand(sourcebn, message_BN_ULONG_length);
    sourcebn->top = message_BN_ULONG_length;

    // start to sign block by block
    for (i = 0; i < source_padded_length; i += message_block_length_byte) {
        // convert char* to BIGNUM*
        memcpy(sourcebn->d, source_padded + i, message_block_length_byte);

        // apply OAEP encode
        if (OAEP_encode(sourcebn, source_oaep_encoded, message_block_length_byte * 8))
            goto err;

        // encrypt
        if (RSA_private_key_CRT_do_1_block(targetbn, source_oaep_encoded, prikey, ctx))
            goto err;

        // save the result to the target .. convert BIGNUM* to char*
        memcpy(target, targetbn->d, chiper_block_length_byte);

        target += chiper_block_length_byte;             // increase counter for next save pointer
    }

    // free the allocated memory
    target = original_target_pointer;     // rewind before return the value ...

    if (result_size < target_size)
        goto err;

    memcpy(result, target, target_size);

    free(target);
    free(source_padded);
    BN_free(sourcebn);
    BN_free(targetbn);
    BN_free(source_oaep_encoded);
    BN_CTX_free(ctx);
    RSA_private_key_free(&prikey);

    return target_size;
err:
    target = original_target_pointer;     // rewind before return the value ...

    free(target);
    free(source_padded);
    BN_free(sourcebn);
    BN_free(targetbn);
    BN_free(source_oaep_encoded);
    BN_CTX_free(ctx);
    RSA_private_key_free(&prikey);

    return 0;
}

/*	result is an allocated buffer, the result_size is for comparison
 *      whether the buffer size is enough or not
 *      the return value is the REAL result size required */
unsigned long RSA_decrypt(char *source, unsigned long source_size,
                          char *result, unsigned long result_size,
                          char *prikey_content, unsigned long prikey_content_size) {
    unsigned long i, total_block;
    BIGNUM        *sourcebn            = BN_new();
    BIGNUM        *targetbn            = BN_new();
    BIGNUM        *target_oaep_decoded = BN_new();
    BN_CTX        *ctx    = BN_CTX_new();
    char          *target = NULL;
    char          *original_target_pointer = NULL;
    unsigned long target_size, final_size;
    pri_key       *prikey = RSA_private_key_new();
    int           chiper_block_length_byte, message_block_length_byte, chiper_BN_ULONG_length, message_BN_ULONG_length;

    if (!RSA_load_private_key_from_buffer(prikey, prikey_content, prikey_content_size))
        goto err;

    // convert the private key raw data to rsa private key structure
    //if(!load_private_key_structure_from_buffer(prikey, prikey_content, prikey_content_size))
    //	goto err;

    // define the rest of the variable required
    chiper_block_length_byte  = (prikey->modulus_length / 8);    // divide by 8 .. convert bit to byte
    chiper_BN_ULONG_length    = chiper_block_length_byte / 4;
    message_block_length_byte = chiper_block_length_byte - 16;   // chiper = 128-bit of random + message;
    // -16 = 8 * 16 = 128-bits of random
    message_BN_ULONG_length = message_block_length_byte / 4;     // convert to 32-bits group

    // source_size MUST BE multiply of chiper_block_length_byte
    total_block = source_size / chiper_block_length_byte;

    // reserve target buffer
    target_size = total_block * message_block_length_byte;
    target      = (char *)malloc(target_size);
    if (target == NULL)
        goto err;

    memset(target, 0, target_size);     // clear .. IMPORTANT
    original_target_pointer = target;

    //reserve a fix space for sourcebn
    bn_wexpand(sourcebn, chiper_BN_ULONG_length);
    sourcebn->top = chiper_BN_ULONG_length;

    // reserve a fix space for the decoded target
    bn_wexpand(target_oaep_decoded, message_BN_ULONG_length);
    target_oaep_decoded->top = message_BN_ULONG_length;

    // start to decrypt block by block
    for (i = 0; i < source_size; i += chiper_block_length_byte) {
        // convert char* to BIGNUM*
        memcpy(sourcebn->d, source + i, chiper_block_length_byte);

        // decrypt
        if (RSA_private_key_CRT_do_1_block(targetbn, sourcebn, prikey, ctx))
            goto err;

        // decode it ... ignore return value .. because it SHOULD be no error return
        OAEP_decode(targetbn, target_oaep_decoded, message_block_length_byte * 8);

        // save it to real char* target
        memcpy(target, target_oaep_decoded->d, message_block_length_byte);

        target += message_block_length_byte;         // increase counter for next save pointer
    }

    target = original_target_pointer;     //rewind

    // remove padding
    // use target twice ... to save creating another buffer
    final_size = RSA_remove_padding(target, target, target_size, target_size);     //, prikey->modulus_length / 8);

    if (final_size == 0)
        goto err;

    if (result_size < final_size)
        goto err;

    memcpy(result, target, final_size);

    // free the allocated memory
    free(target);
    BN_free(sourcebn);
    BN_free(targetbn);
    BN_free(target_oaep_decoded);
    BN_CTX_free(ctx);
    RSA_private_key_free(&prikey);

    return final_size;
err:
    target = original_target_pointer;     //rewind

    free(target);
    BN_free(sourcebn);
    BN_free(targetbn);
    BN_free(target_oaep_decoded);
    BN_CTX_free(ctx);
    RSA_private_key_free(&prikey);

    return 0;
}

/*	result is an allocated buffer, the result_size is for comparison
 *      whether the buffer size is enough or not
 *      the return value is the REAL result size required */
unsigned long RSA_encrypt(char *source, unsigned long source_size,
                          char *result, unsigned long result_size,
                          char *pubkey_content, unsigned long pubkey_content_size) { // this function has been ON purpose designed this way ...
    unsigned long i;
    unsigned long total_block;
    BIGNUM        *sourcebn            = BN_new();
    BIGNUM        *source_oaep_encoded = BN_new();
    BIGNUM        *targetbn            = BN_new();
    BN_CTX        *ctx                     = BN_CTX_new();
    char          *target                  = NULL;
    char          *source_padded           = NULL;
    char          *original_target_pointer = NULL;
    unsigned long source_padded_length, target_size;
    pub_key       *pubkey = RSA_public_certificate_new();
    int           chiper_block_length_byte, message_block_length_byte, chiper_BN_ULONG_length, message_BN_ULONG_length;

    if (!RSA_load_public_certificate_from_buffer(pubkey, pubkey_content, pubkey_content_size))
        goto err;

    // load public certificate from buffer
    //if(!load_public_certificate_structure_from_buffer(pubkey, pubkey_content, pubkey_content_size))
    //	goto err;

    // define the rest of the variable required
    chiper_block_length_byte  = (pubkey->modulus_length / 8);    // divide by 8 .. convert bit to byte
    chiper_BN_ULONG_length    = chiper_block_length_byte / 4;
    message_block_length_byte = chiper_block_length_byte - 16;   // chiper = 128-bit of random + message;
    // -16 = 8 * 16 = 128-bits of random
    message_BN_ULONG_length = message_block_length_byte / 4;     // convert to 32-bits group

    // insert padding ...
    // check if there is left over or not
    if (source_size % message_block_length_byte)
        source_padded_length = ((source_size / message_block_length_byte) + 1) * message_block_length_byte;
    else
        source_padded_length = (source_size / message_block_length_byte) * message_block_length_byte;

    // allocated moemry
    source_padded = (char *)malloc(source_padded_length);
    if (source_padded == NULL)
        goto err;

    if (!RSA_insert_padding(source, source_padded, source_size, source_padded_length, source_padded_length - source_size))
        goto err;

    total_block = source_padded_length / message_block_length_byte;

    // reserve target buffer
    target_size = total_block * chiper_block_length_byte;
    target      = (char *)malloc(target_size);
    if (target == NULL)
        goto err;

    memset(target, 0, target_size);     // clear ... important
    original_target_pointer = target;

    // reserve a fix length sourcebn
    bn_wexpand(sourcebn, message_BN_ULONG_length);
    sourcebn->top = message_BN_ULONG_length;

    // start to encrypt block by block
    for (i = 0; i < source_padded_length; i += message_block_length_byte) {
        // convert char* to BIGNUM*
        memcpy(sourcebn->d, source_padded + i, message_block_length_byte);

        // apply OAEP encode
        if (OAEP_encode(sourcebn, source_oaep_encoded, message_block_length_byte * 8))
            goto err;

        // encrypt
        if (RSA_public_certificate_do_1_block(targetbn, source_oaep_encoded, pubkey, ctx))
            goto err;

        // save the result to the target .. convert BIGNUM* to char*
        memcpy(target, targetbn->d, chiper_block_length_byte);

        target += chiper_block_length_byte;             // increase counter for next save pointer
    }

    // rewind pointer
    target = original_target_pointer;

    if (result_size < target_size)
        goto err;

    memcpy(result, target, target_size);

    // free the allocated memory
    free(target);
    free(source_padded);
    BN_free(sourcebn);
    BN_free(targetbn);
    BN_free(source_oaep_encoded);
    BN_CTX_free(ctx);
    RSA_public_certificate_free(&pubkey);

    return target_size;
err:
    // rewind pointer
    target = original_target_pointer;
    free(target);
    free(source_padded);
    BN_free(sourcebn);
    BN_free(targetbn);
    BN_free(source_oaep_encoded);
    BN_CTX_free(ctx);
    RSA_public_certificate_free(&pubkey);

    return 0;
}

// return 1 if success else ZERO
int RSA_insert_padding(char *source, char *target, int sourceLen, int targetLen, int padding) {
    if (sourceLen + padding > targetLen)
        return 0;

    memcpy(target, source, sourceLen);

    memset(target + sourceLen, padding, padding);

    return 1;
}

// return the length of target without padding byte
// return ZERO if error
int RSA_remove_padding(char *source, char *target, int sourceLen, int targetLen) { //, int blockSize)
    char          *compare;
    unsigned char padding = *(source + (sourceLen - 1));                           // use unsigned char to avoid NEGATIVE VALUE

    compare = (char *)malloc(padding);

    memset(compare, padding, padding);

    // compare 'padding' to the 'compare' string
    if (memcmp(source + (sourceLen - padding), compare, padding) != 0)
        return 0;          // NOT a valid padding .. nothing to remove
    if (targetLen < sourceLen - padding)
        return 0;

    memcpy(target, source, sourceLen - padding);

    return sourceLen - padding;
}

long RSA_private_key_CRT_do_1_block(BIGNUM *target, BIGNUM *msg, pri_key *prikey, BN_CTX *ctx) { // all the BIGNUM function's return value are ignored ... if error occur ... check here ...
    BIGNUM *m1 = BN_new();
    BIGNUM *m2 = BN_new();
    BIGNUM *m  = BN_new();
    BIGNUM *h  = BN_new();

    // step 1: m1 = c^dp mod p
    BN_mod_exp_mont(m1, msg, prikey->dp, prikey->p, ctx, NULL);
    // step 2: m2 = c^dq mod q
    BN_mod_exp_mont(m2, msg, prikey->dq, prikey->q, ctx, NULL);

    // step 3: h = ((m1 - m2) * invq) mod p
    BN_sub(h, m1, m2);                          // h = m1 - m2
    if (h->neg) {                               // if h is negative ... turn it to positive
        // mod it by p .. IF ERROR CHECK HERE .. borrow the target variable
        if (!BN_mod(target, h, prikey->p, ctx)) // temp2 must be still negative
            goto err;

        // convert to positive
        if (!BN_add(h, prikey->p, target))         // temp3 now must be positive
            goto err;
    }

    BN_mul(target, h, prikey->qinv, ctx);  // h = h * invq
    BN_mod(h, target, prikey->p, ctx);     // h = h % p

    // step 4: m = m2 + (q * h)
    BN_mul(m, h, prikey->q, ctx);
    BN_add(target, m, m2);                      // use target ... direct store and finish

    BN_free(m1);
    BN_free(m2);
    BN_free(m);
    BN_free(h);

    return 0;
err:
    BN_free(m1);
    BN_free(m2);
    BN_free(m);
    BN_free(h);

    return -1;
}

long RSA_public_certificate_do_1_block(BIGNUM *target, BIGNUM *msg, pub_key *key, BN_CTX *ctx) {
    // this function is the fastest in BN module
    // a simple one line command to encrypt or verify data ...
    if (!BN_mod_exp_mont(target, msg, key->e, key->n, ctx, NULL))
        return -1;

    return 0;
}

// Optimal Asymmetric Encryption Padding ... including OS2IP (Octet String to Integer Primitive)
// NOTE: source and target MUST be different
long OAEP_encode(BIGNUM *source, BIGNUM *target, int message_length_bit) { /*	source = 232 bytes = 1856 bits (max size)
                                                                            *      target = 256 bytes = 2048 bits (max block size)
                                                                            *      OAEP requires seed(random value), message, and hash value ...
                                                                            *      this implementation does not use any hash function ...
                                                                            *      therefore only seed(random value) and message */
                                                                           // define seed
    BIGNUM   *seed = BN_new();
    BN_ULONG i;
    BN_ULONG seed_xor_crc = 0;
    BN_ULONG *d_target, *d_source;
    BN_ULONG message_BN_ULONG_length;

    // 24 bytes - 2 bits = 190 bits ..
    // IMPORTANT NOTE: two zero (00) binary at the top
    if (BN_generate_random_bit(seed, seed_length_bit, 0, 0)) {  // take out three bit ..
        BN_free(seed);
        return -1;
    }

    // get the first block of the seed for the XOR value
    seed_xor_crc = seed->d[0];

    // set the seed the highest value ... rightmost binary format
    if (!BN_lshift(target, seed, message_length_bit)) {
        BN_free(seed);
        return -1;          // error
    }

    // XOR the message with the firstr block of seed .. to mask(hide) it ...
    // use only the first block because it is faster ... and it offer the same security
    d_target = target->d;     // these two variables just for optimization
    d_source = source->d;
    message_BN_ULONG_length = message_length_bit / 32;
    for (i = 0; i < message_BN_ULONG_length; i++)
        d_target[i] = d_source[i] ^ seed_xor_crc;

    BN_free(seed);

    return 0;
}

// Decode Optimal Asymmetric Encryption Padding ...
// the target is a fix length ....... this function has no error return value, no need to check it
long OAEP_decode(BIGNUM *source, BIGNUM *target, int message_length_bit) {
    /*	source = 256 bytes = 2048 bits (max size)
     *      target = 232 bytes = 1856 bits (message block length byte) = 58 BN_ULONG
     *      OAEP decode only use AND operation for up to 1856 bits of source
     *      ALWAYS REMEMBER that this implementation is different from PKCS#1 v2.1
     *      This custom OAEP_decode function simply use AND bitwise operand on the decrypted buffer (BIGNUM)*/

    BN_ULONG source_xor_crc, i;
    BN_ULONG *target_d, *source_d;
    BN_ULONG message_BN_ULONG_length;

    message_BN_ULONG_length = message_length_bit / 32;

    // get the crc xor key
    source_xor_crc = source->d[message_BN_ULONG_length];     // the 59th .. NOTE: array is start from zero

    // simply XOR operation
    target_d = target->d;
    source_d = source->d;
    for (i = 0; i < message_BN_ULONG_length; i++)
        target_d[i] = source_xor_crc ^ source_d[i];

    return 0;
}
