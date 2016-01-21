/*	rsa_lib/rsa.h
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

#ifndef _INCLUDED_RSA_H_
#define _INCLUDED_RSA_H_

#include "../bn_openssl097beta1/bn.h"
#include "../bn_openssl097beta1/bn_lcl.h"

/*
 * #ifdef  __cplusplus
 * extern "C" {
 * #endif
 */

#define seed_length_bit              126            // for OAEP ... 4 block of 32-bit = 128-bits ... it is strong enough
// 128-bit - 2-bits = 126 ... to avoid bigger than value
#define buffer_size                  2048           // expand when necessary

#define bn_header_size               2
#define certificate_top              "-----BEGIN RSA CERTIFICATE-----"
#define certificate_bottom           "-----END RSA CERTIFICATE-----"
#define key_top                      "-----BEGIN RSA PRIVATE KEY-----"
#define key_bottom                   "-----END RSA PRIVATE KEY-----"
#define certificate_top_length       31
#define certificate_bottom_length    29
#define key_top_length               31
#define key_bottom_length            29


// define max block size (= 2048 bits) ... because of the padding limitation (rule)

//#define BN_ULONG_length				4
//#define buffer_size					2048 // expand when necessary

/*
 * #define prime_bit_size				1024 // p and q each is 1024 bits ... n = 2048 bits
 * #define max_block_size				256 // bytes
 * #define chiper_block_length_byte	256	// bytes
 * #define seed_length_bit				190 // for OAEP ... 190 bits = 24 bytes - 2 bits
 * #define seed_BN_ULONG_length		6 //
 * #define message_length_bit			1856
 * #define chiper_BN_ULONG_length		64
 * #define message_BN_ULONG_length		58 // 58 blocks of BN_ULONG
 * #define message_block_length_byte	232 // message_BN_ULONG_length * BN_ULONG_length
 */

typedef struct private_key {
    // e is only for re-generate public certificate ... in case public certificate is lost
    // e also use encryption using private key (= for private use)
    unsigned short modulus_length;      // to tell how big it is ...
    // so we know how big the message size should be before encrypt
    // also we know how big is the padding
    BIGNUM         *p;
    BIGNUM         *q;
    BIGNUM         *dp;
    BIGNUM         *dq;
    BIGNUM         *e;
    BIGNUM         *qinv;
} pri_key;

typedef struct public_key {
    unsigned short modulus_length;
    BIGNUM         *e;
    BIGNUM         *n;
} pub_key;

// rsa_encode.c
long RSA_public_certificate_do_1_block(BIGNUM *target, BIGNUM *msg, pub_key *key, BN_CTX *ctx);
long RSA_private_key_CRT_do_1_block(BIGNUM *target, BIGNUM *msg, pri_key *key, BN_CTX *ctx);
int RSA_insert_padding(char *source, char *target, int sourceLen, int targetLen, int blockSize);
int RSA_remove_padding(char *source, char *target, int sourceLen, int targetLen);

long OAEP_encode(BIGNUM *source, BIGNUM *target, int message_length_bit);
long OAEP_decode(BIGNUM *source, BIGNUM *target, int message_length_bit);

unsigned long RSA_encrypt(char *source, unsigned long source_size,
                          char *result, unsigned long result_size,
                          char *pubkey_content, unsigned long pubkey_content_size);
unsigned long RSA_decrypt(char *source, unsigned long source_size,
                          char *result, unsigned long result_size,
                          char *prikey_content, unsigned long prikey_content_size);
unsigned long RSA_sign(char *source, unsigned long source_size,
                       char *result, unsigned long result_size,
                       char *prikey_content, unsigned long prikey_content_size);
unsigned long RSA_verify(char *source, unsigned long source_size,
                         char *result, unsigned long result_size,
                         char *pubkey_content, unsigned long pubkey_content_size);

// rsa_keys.c
int RSA_generate_keys(char *privatekey,
                      char *publiccertificate,
                      int  *privatekey_length,
                      int  *publiccertificate_length,
                      int  primeBitSize);

int save_private_key_structure_to_buffer(pri_key *prikey, char *buffer, int bufferLen);
int save_public_certificate_structure_to_buffer(pub_key *pubkey, char *buffer, int bufferLen);
int load_public_certificate_structure_from_buffer(pub_key *pubkey, char *buffer, int bufferLen);
int load_private_key_structure_from_buffer(pri_key *prikey, char *buffer, int bufferLen);

//pri_key *RSA_read_private_key(char *source, unsigned long source_length);
//pub_key *RSA_read_public_certificate(char *source, unsigned long source_length);

int RSA_save_public_certificate_to_buffer(pub_key *pubkey, char *target, int targetLen);
int RSA_load_public_certificate_from_buffer(pub_key *pubkey, char *source, int sourceLen);

int RSA_save_private_key_to_buffer(pri_key *prikey, char *target, int targetLen);
int RSA_load_private_key_from_buffer(pri_key *prikey, char *source, int sourceLen);

// rsa_lib.c
pri_key *RSA_private_key_new();
void RSA_private_key_free(pri_key **prikey);
pub_key *RSA_public_certificate_new();
void RSA_public_certificate_free(pub_key **pubkey);

// rsa_pem.c
int PEM_encode(char *source, char *target, int sourceLen, int targetLen);
int PEM_decode(char *source, char *target, int sourceLen, int targetLen);

/*
 * #ifdef  __cplusplus
 * }
 * #endif
 */
#endif
