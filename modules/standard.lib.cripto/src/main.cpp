//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
#ifdef __WITH_TOMCRYPT
 #include "tomcrypt/tomcrypt.h"
#endif
extern "C" {
#define PROTO_LIST(PROTOTYPE)    PROTOTYPE
#define UINT4    unsigned int

#include "md5/md5.h"
#include "sha1/sha1.h"
#include "crc32/crc32.h"
#ifdef WITH_OPENSSL
 #include <openssl/rsa.h>
 #include <openssl/err.h>
#else
 #include "rsa/rsa.h"
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "hmac/hmac.h"
}
#include "INTERFACE_TO_AES.h"
#ifdef _USE_SECONDARY_AES
 #include "EfAes/EfAes.h"
#endif

#ifdef __WITH_INTEL_LIB
extern "C" {
 #include "AES-NI/iaesni.h"
}
static int hardware_aes = 0;
#endif
#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#endif

const char *default_dhe_p = "87A8E61DB4B6663CFFBBD19C651959998CEEF608660DD0F25D2CEED4435E3B00E00DF8F1D61957D4FAF7DF4561B2AA3016C3D91134096FAA3BF4296D830E9A7C209E0C6497517ABD5A8A9D306BCF67ED91F9E6725B4758C022E0B1EF4275BF7B6C5BFC11D45F9088B941F54EB1E59BB8BC39A0BF12307F5C4FDB70C581B23F76B63ACAE1CAA6B7902D52526735488A0EF13C6D9A51BFA4AB3AD8347796524D8EF6A167B5A41825D967E144E5140564251CCACB83E6B486F6B3CA3F7971506026C0B857F689962856DED4010ABD0BE621C3A3960A54E710C375F26375D7014103A4B54330C198AF126116D2276E11715F693877FAD7EF09CADB094AE91E1A1597";
const char *default_dhe_g = "3FB32C9B73134D0B2E77506660EDBD484CA7B18F21EF205407F4793A1A0BA12510DBC15077BE463FFF4FED4AAC0BB555BE3A6C1B0C6B47B1BC3773BF7E8C6F62901228F8C28CBB18A55AE31341000A650196F931C77A57F2DDF463E5E9EC144B777DE62AAAB8A8628AC376D282D6ED3864E67982428EBC831D14348F6F2F9193B5045AF2767164E1DFC967C1FB3F2E55A4BD1BFFE83B9C80D052B985D182EA0ADB2A3B7313D3FE14C8484B1E052588B9B7D2BBD2DF016199ECD06E1557CD0915B3353BBB64E0EC377FD028370DF92B52C7891428CDC67EB6184B523D1DB246C32F63078490F00EF8D647D148D47954515E2327CFEF98C582664B4C0F6CC41659";
static const ltc_ecc_set_type ecc256 = {
    32,
    "ECC-256",
    "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF",
    "5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B",
    "FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551",
    "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296",
    "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5",
};
//---------------------------------------------------------------------------
#define BLOCKMODE_CTR    BLOCKMODE_CRT
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(BLOCKMODE_ECB)
    DEFINE_ECONSTANT(BLOCKMODE_CBC)
    DEFINE_ECONSTANT(BLOCKMODE_PCBC)
    DEFINE_ECONSTANT(BLOCKMODE_OFB)
    DEFINE_ECONSTANT(BLOCKMODE_CFB)
    DEFINE_ECONSTANT(BLOCKMODE_CRT)
    DEFINE_ECONSTANT(BLOCKMODE_CTR)
    DEFINE_SCONSTANT("standard.net.crypto", "standard.net.cripto")
#ifdef __WITH_INTEL_LIB
    hardware_aes = check_for_aes_instructions();
#endif
#ifdef __WITH_TOMCRYPT
 #ifdef USE_LTM
    ltc_mp = ltm_desc;
 #elif defined(USE_TFM)
    ltc_mp = tfm_desc;
 #elif defined(USE_GMP)
    ltc_mp = gmp_desc;
 #else
    extern ltc_math_descriptor EXT_MATH_LIB;
    ltc_mp = EXT_MATH_LIB;
 #endif
    register_prng(&sprng_desc);
    register_hash(&sha1_desc);
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
#define LITTLE_ENDIAN

/******************************************************************************
 * SHA-256.
 */

typedef struct {
    uint8_t  hash[32];          // Changed by RKW, unsigned char becomes uint8_t
    uint32_t buffer[16];        // Changed by RKW, unsigned long becomes uint32_t
    uint32_t state[8];          // Changed by RKW, unsinged long becomes uint32_t
    uint8_t  length[8];         // Changed by RKW, unsigned char becomes uint8_t
} sha256;

void sha256_initialize(sha256 *sha) {
    int i;

    for (i = 0; i < 17; ++i) sha->buffer[i] = 0;
    sha->state[0] = 0x6a09e667;
    sha->state[1] = 0xbb67ae85;
    sha->state[2] = 0x3c6ef372;
    sha->state[3] = 0xa54ff53a;
    sha->state[4] = 0x510e527f;
    sha->state[5] = 0x9b05688c;
    sha->state[6] = 0x1f83d9ab;
    sha->state[7] = 0x5be0cd19;
    for (i = 0; i < 8; ++i) sha->length[i] = 0;
}

//  Changed by RKW, formal args are now const uint8_t, uint_32
//    from const unsigned char, unsigned long respectively
void sha256_update(sha256        *sha,
                   const uint8_t *message,
                   uint32_t      length) {
    int i, j;

    /* Add the length of the received message, counted in
     * bytes, to the total length of the messages hashed to
     * date, counted in bits and stored in 8 separate bytes. */
    for (i = 7; i >= 0; --i) {
        int bits;
        if (i == 7)
            bits = length << 3;
        else if ((i == 0) || (i == 1) || (i == 2))
            bits = 0;
        else
            bits = length >> (53 - 8 * i);
        bits &= 0xff;
        if (sha->length[i] + bits > 0xff) {
            for (j = i - 1; j >= 0 && sha->length[j]++ == 0xff; --j);
        }
        sha->length[i] += bits;
    }

    /* Add the received message to the SHA buffer, updating the
     * hash at each block (each time the buffer is filled). */
    while (length > 0) {
        /* Find the index in the SHA buffer at which to
         * append what's left of the received message. */
        int index = sha->length[6] % 2 * 32 + sha->length[7] / 8;
        index = (index + 64 - length % 64) % 64;

        /* Append the received message bytes to the SHA buffer until
         * we run out of message bytes or until the buffer is filled. */
        for ( ; length > 0 && index < 64; ++message, ++index, --length) {
            sha->buffer[index / 4] |= *message << (24 - index % 4 * 8);
        }
        /* Update the hash with the buffer contents if the buffer is full. */
        if (index == 64) {
            /* Update the hash with a block of message content. See FIPS 180-2
             * (<csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf>)
             * for a description of and details on the algorithm used here. */
            // Changed by RKW, const unsigned long becomes const uint32_t
            const uint32_t k[64] = {
                0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
                0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
                0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
                0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
                0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
                0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
                0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
                0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
                0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
            };
            // Changed by RKW, unsigned long becomes uint32_t
            uint32_t w[64], a, b, c, d, e, f, g, h;
            int      t;
            for (t = 0; t < 16; ++t) {
                w[t]           = sha->buffer[t];
                sha->buffer[t] = 0;
            }
            for (t = 16; t < 64; ++t) {
                // Changed by RKW, unsigned long becomes uint32_t
                uint32_t s0, s1;
                s0   = (w[t - 15] >> 7 | w[t - 15] << 25);
                s0  ^= (w[t - 15] >> 18 | w[t - 15] << 14);
                s0  ^= (w[t - 15] >> 3);
                s1   = (w[t - 2] >> 17 | w[t - 2] << 15);
                s1  ^= (w[t - 2] >> 19 | w[t - 2] << 13);
                s1  ^= (w[t - 2] >> 10);
                w[t] = (s1 + w[t - 7] + s0 + w[t - 16]) & 0xffffffffU;
            }
            a = sha->state[0];
            b = sha->state[1];
            c = sha->state[2];
            d = sha->state[3];
            e = sha->state[4];
            f = sha->state[5];
            g = sha->state[6];
            h = sha->state[7];
            for (t = 0; t < 64; ++t) {
                // Changed by RKW, unsigned long becomes uint32_t
                uint32_t e0, e1, t1, t2;
                e0  = (a >> 2 | a << 30);
                e0 ^= (a >> 13 | a << 19);
                e0 ^= (a >> 22 | a << 10);
                e1  = (e >> 6 | e << 26);
                e1 ^= (e >> 11 | e << 21);
                e1 ^= (e >> 25 | e << 7);
                t1  = h + e1 + ((e & f) ^ (~e & g)) + k[t] + w[t];
                t2  = e0 + ((a & b) ^ (a & c) ^ (b & c));
                h   = g;
                g   = f;
                f   = e;
                e   = d + t1;
                d   = c;
                c   = b;
                b   = a;
                a   = t1 + t2;
            }
            sha->state[0] = (sha->state[0] + a) & 0xffffffffU;
            sha->state[1] = (sha->state[1] + b) & 0xffffffffU;
            sha->state[2] = (sha->state[2] + c) & 0xffffffffU;
            sha->state[3] = (sha->state[3] + d) & 0xffffffffU;
            sha->state[4] = (sha->state[4] + e) & 0xffffffffU;
            sha->state[5] = (sha->state[5] + f) & 0xffffffffU;
            sha->state[6] = (sha->state[6] + g) & 0xffffffffU;
            sha->state[7] = (sha->state[7] + h) & 0xffffffffU;
        }
    }
}

//  Changed by RKW, formal args are now const uint8_t, uint_32
//    from const unsigned char, unsigned long respectively
void sha256_finalize(sha256        *sha,
                     const uint8_t *message,
                     uint32_t      length) {
    int i;
    // Changed by RKW, unsigned char becomes uint8_t
    uint8_t terminator[64 + 8] = { 0x80 };

    /* Hash the final message bytes if necessary. */
    if (length > 0) sha256_update(sha, message, length);

    /* Create a terminator that includes a stop bit, padding, and
     * the the total message length. See FIPS 180-2 for details. */
    length = 64 - sha->length[6] % 2 * 32 - sha->length[7] / 8;
    if (length < 9) length += 64;
    for (i = 0; i < 8; ++i) terminator[length - 8 + i] = sha->length[i];
    /* Hash the terminator to finalize the message digest. */
    sha256_update(sha, terminator, length);
    /* Extract the message digest. */
    for (i = 0; i < 32; ++i) {
        sha->hash[i] = (sha->state[i / 4] >> (24 - 8 * (i % 4))) & 0xff;
    }
}

//  Changed by RKW, formal args are now uint8_t, const uint_8
//    from unsigned char, const unsigned char respectively
void sha256_get(uint8_t       hash[32],
                const uint8_t *message,
                int           length) {
    int    i;
    sha256 sha;

    sha256_initialize(&sha);
    sha256_finalize(&sha, message, length);
    for (i = 0; i < 32; ++i) hash[i] = sha.hash[i];
}

/******************************************************************************
 * HMAC-SHA256.
 */

typedef struct _hmac_sha256 {
    uint8_t digest[32];         // Changed by RKW, unsigned char becomes uint_8
    uint8_t key[64];            // Changed by RKW, unsigned char becomes uint_8
    sha256  sha;
} hmac_sha256;

//  Changed by RKW, formal arg is now const uint8_t
//    from const unsigned char
void hmac_sha256_initialize(hmac_sha256 *hmac,
                            const uint8_t *key, int length) {
    int i;

    /* Prepare the inner hash key block, hashing the key if it's too long. */
    if (length <= 64) {
        for (i = 0; i < length; ++i) hmac->key[i] = key[i] ^ 0x36;
        for ( ; i < 64; ++i) hmac->key[i] = 0x36;
    } else {
        sha256_initialize(&(hmac->sha));
        sha256_finalize(&(hmac->sha), key, length);
        for (i = 0; i < 32; ++i) hmac->key[i] = hmac->sha.hash[i] ^ 0x36;
        for ( ; i < 64; ++i) hmac->key[i] = 0x36;
    }
    /* Initialize the inner hash with the key block. */
    sha256_initialize(&(hmac->sha));
    sha256_update(&(hmac->sha), hmac->key, 64);
}

//  Changed by RKW, formal arg is now const uint8_t
//    from const unsigned char
void hmac_sha256_update(hmac_sha256 *hmac,
                        const uint8_t *message, int length) {
    /* Update the inner hash. */
    sha256_update(&(hmac->sha), message, length);
}

//  Changed by RKW, formal arg is now const uint8_t
//    from const unsigned char
void hmac_sha256_finalize(hmac_sha256 *hmac,
                          const uint8_t *message, int length) {
    int i;

    /* Finalize the inner hash and store its value in the digest array. */
    sha256_finalize(&(hmac->sha), message, length);
    for (i = 0; i < 32; ++i) hmac->digest[i] = hmac->sha.hash[i];
    /* Convert the inner hash key block to the outer hash key block. */
    for (i = 0; i < 64; ++i) hmac->key[i] ^= (0x36 ^ 0x5c);
    /* Calculate the outer hash. */
    sha256_initialize(&(hmac->sha));
    sha256_update(&(hmac->sha), hmac->key, 64);
    sha256_finalize(&(hmac->sha), hmac->digest, 32);
    /* Use the outer hash value as the HMAC digest. */
    for (i = 0; i < 32; ++i) hmac->digest[i] = hmac->sha.hash[i];
}

//  Changed by RKW, formal args are now uint8_t, const uint8_t
//    from unsinged char, const unsigned char respectively
void hmac_sha256_get(uint8_t digest[32],
                     const uint8_t *message, int message_length,
                     const uint8_t *key, int key_length) {
    int         i;
    hmac_sha256 hmac;

    hmac_sha256_initialize(&hmac, key, key_length);
    hmac_sha256_finalize(&hmac, message, message_length);
    for (i = 0; i < 32; ++i) digest[i] = hmac.digest[i];
}

/******************************************************************************
 * Input/output.
 */
int do_hmac_sha256(char *key_gc, int key_len, const uint8_t *data, int data_len, char *out) {
    hmac_sha256   hmac;
    sha256        sha;
    unsigned char key[64];
    unsigned char block[1024];
    int           i, c, d;

    /* Parse and verify arguments. */
    //int hexadecimals = (argc == 2 && strcmp(argv[1], "-x") == 0);
    /* Read the key, hashing it if necessary. */
    sha256_initialize(&sha);
    for (i = 0; i < key_len; i++) {
        if ((i > 0) && (i % 64 == 0))
            sha256_update(&sha, key, 64);
        key[i % 64] = c;
    }
    /* Display an error and exit if the key is invalid. */
    if ((i == 0) || (c == '?')) {
        return -1;
    }
    /* Initialize the HMAC-SHA256 digest with the key or its hash. */
    if (i <= 64) {
        hmac_sha256_initialize(&hmac, key, i);
    } else {
        sha256_finalize(&sha, key, i % 64);
        hmac_sha256_initialize(&hmac, sha.hash, 64);
    }
    i = 0;
    int len = 1024;
    do {
        if (data_len < len)
            len = data_len;
        if (len > 0) {
            hmac_sha256_update(&hmac, data + i, len);
            i   += 1024;
            len -= 1024;
        }
    } while (len > 0);
    /* Finalize the HMAC-SHA256 digest and output its value. */
    hmac_sha256_finalize(&hmac, NULL, 0);
    memcpy(out, hmac.digest, 32);
    return 0;
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MD5Init, 0)
    MD5_CTX * CTX = (MD5_CTX *)malloc(sizeof(MD5_CTX));
    MD5Init(CTX);
    RETURN_NUMBER((SYS_INT)CTX)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MD5Update, 2)
    T_HANDLE(MD5Update, 0)
    T_STRING(MD5Update, 1)
    MD5_CTX * CTX = (MD5_CTX *)PARAM_INT(0);
    MD5Update(CTX, (unsigned char *)PARAM(1), (long)PARAM_LEN(1));
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MD5Final, 1)
    T_HANDLE(MD5Final, 0)
    MD5_CTX * CTX = (MD5_CTX *)PARAM_INT(0);
    unsigned char md5_sum[16];
    MD5Final(md5_sum, CTX);

    unsigned char result[32];
    for (int i = 0; i < 16; i++) {
        unsigned char first = md5_sum[i] / 0x10;
        unsigned char sec   = md5_sum[i] % 0x10;

        if (first < 10)
            result[i * 2] = '0' + first;
        else
            result[i * 2] = 'a' + (first - 10);

        if (sec < 10)
            result[i * 2 + 1] = '0' + sec;
        else
            result[i * 2 + 1] = 'a' + (sec - 10);
    }


    SET_NUMBER(0, 0)
    RETURN_BUFFER((char *)result, 32);
    free(CTX);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_md5 CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "md5 takes one parameter");
    LOCAL_INIT;

    char   *szParam0;
    NUMBER length;

    GET_CHECK_BUFFER(0, szParam0, length, "md5: parameter should be of static string type");

    MD5_CTX       CTX;
    unsigned char md5_sum[16];

    MD5Init(&CTX);
    MD5Update(&CTX, (unsigned char *)szParam0, (long)length);
    MD5Final(md5_sum, &CTX);

    unsigned char result[32];
    for (int i = 0; i < 16; i++) {
        unsigned char first = md5_sum[i] / 0x10;
        unsigned char sec   = md5_sum[i] % 0x10;

        if (first < 10)
            result[i * 2] = '0' + first;
        else
            result[i * 2] = 'a' + (first - 10);

        if (sec < 10)
            result[i * 2 + 1] = '0' + sec;
        else
            result[i * 2 + 1] = 'a' + (sec - 10);
    }


    RETURN_BUFFER((char *)result, 32);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SHA1Init, 0)
    SHA1Context * CTX = (SHA1Context *)malloc(sizeof(SHA1Context));
    SHA1Reset(CTX);
    RETURN_NUMBER((SYS_INT)CTX)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SHA1Update, 2)
    T_HANDLE(SHA1Update, 0)
    T_STRING(SHA1Update, 1)
    SHA1Context * CTX = (SHA1Context *)PARAM_INT(0);
    SHA1Input(CTX, (unsigned char *)PARAM(1), (long)PARAM_LEN(1));
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SHA1Final, 1)
    T_HANDLE(SHA1Final, 0)
    SHA1Context * CTX = (SHA1Context *)PARAM_INT(0);

    if (!SHA1Result(CTX)) {
        RETURN_STRING("");
        free(CTX);
        return 0;
    }

    unsigned char result[40];
    unsigned char sha1_sum[20];

    unsigned char *sha1_ptr = (unsigned char *)CTX->Message_Digest;
    for (int j = 0; j < 5; j++) {
#ifdef LITTLE_ENDIAN
        for (int k = 3; k >= 0; k--) {
            sha1_sum[j * 4 + 3 - k] = sha1_ptr[j * 4 + k];
        }
#else
        for (int k = 0; k < 4; k++)
            sha1_sum[j * 4 + k] = sha1_ptr[j * 4 + k];
#endif
    }

    for (int i = 0; i < 20; i++) {
        unsigned char first = sha1_sum[i] / 0x10;
        unsigned char sec   = sha1_sum[i] % 0x10;

        if (first < 10)
            result[i * 2] = '0' + first;
        else
            result[i * 2] = 'a' + (first - 10);

        if (sec < 10)
            result[i * 2 + 1] = '0' + sec;
        else
            result[i * 2 + 1] = 'a' + (sec - 10);
    }

    SET_NUMBER(0, 0)
    RETURN_BUFFER((char *)result, 40);
    free(CTX);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_sha1 CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "sha1 takes one parameter");
    LOCAL_INIT;

    char   *szParam0;
    NUMBER length;

    GET_CHECK_BUFFER(0, szParam0, length, "sha1: parameter should be of static string type");

    SHA1Context CTX;

    SHA1Reset(&CTX);
    SHA1Input(&CTX, (unsigned char *)szParam0, (long)length);

    if (!SHA1Result(&CTX)) {
        RETURN_STRING("");
        return 0;
    }

    unsigned char result[40];
    unsigned char sha1_sum[20];

    unsigned char *sha1_ptr = (unsigned char *)CTX.Message_Digest;
    for (int j = 0; j < 5; j++) {
#ifdef LITTLE_ENDIAN
        for (int k = 3; k >= 0; k--) {
            sha1_sum[j * 4 + 3 - k] = sha1_ptr[j * 4 + k];
        }
#else
        for (int k = 0; k < 4; k++)
            sha1_sum[j * 4 + k] = sha1_ptr[j * 4 + k];
#endif
    }

    for (int i = 0; i < 20; i++) {
        unsigned char first = sha1_sum[i] / 0x10;
        unsigned char sec   = sha1_sum[i] % 0x10;

        if (first < 10)
            result[i * 2] = '0' + first;
        else
            result[i * 2] = 'a' + (first - 10);

        if (sec < 10)
            result[i * 2 + 1] = '0' + sec;
        else
            result[i * 2 + 1] = 'a' + (sec - 10);
    }

    RETURN_BUFFER((char *)result, 40);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_crc32 CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "crc32 takes one parameter");
    LOCAL_INIT;

    char   *szParam0;
    NUMBER length;

    GET_CHECK_BUFFER(0, szParam0, length, "crc32: parameter should be of static string type");
    RETURN_NUMBER(_crc32(CRC32_INITIAL, szParam0, (long)length));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_sha256 CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "sha256 takes one parameter");
    LOCAL_INIT;

    char   *szParam0;
    NUMBER length;

    GET_CHECK_BUFFER(0, szParam0, length, "sha256: parameter should be of static string type");

    unsigned char result[64];
    unsigned char hash[32];

    sha256_get(hash, (unsigned char *)szParam0, (int)length);

    for (int i = 0; i < 32; i++) {
        unsigned char first = hash[i] / 0x10;
        unsigned char sec   = hash[i] % 0x10;

        if (first < 10)
            result[i * 2] = '0' + first;
        else
            result[i * 2] = 'a' + (first - 10);

        if (sec < 10)
            result[i * 2 + 1] = '0' + sec;
        else
            result[i * 2 + 1] = 'a' + (sec - 10);
    }

    RETURN_BUFFER((char *)result, 64);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SHA256Init, 0)
    sha256 * CTX = (sha256 *)malloc(sizeof(sha256));
    sha256_initialize(CTX);
    RETURN_NUMBER((SYS_INT)CTX)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SHA256Update, 2)
    T_HANDLE(SHA256Update, 0)
    T_STRING(SHA256Update, 1)
    sha256 * CTX = (sha256 *)PARAM_INT(0);
    sha256_update(CTX, (unsigned char *)PARAM(1), (long)PARAM_LEN(1));
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SHA256Final, 1)
    T_HANDLE(SHA256Final, 0)
    sha256 * CTX = (sha256 *)PARAM_INT(0);

    sha256_finalize(CTX, 0, 0);

    unsigned char result[64];
    unsigned char sha1_sum[32];

    unsigned char *sha_ptr = (unsigned char *)CTX->hash;

    for (int i = 0; i < 64; i++) {
        unsigned char first = sha_ptr[i] / 0x10;
        unsigned char sec   = sha_ptr[i] % 0x10;

        if (first < 10)
            result[i * 2] = '0' + first;
        else
            result[i * 2] = 'a' + (first - 10);

        if (sec < 10)
            result[i * 2 + 1] = '0' + sec;
        else
            result[i * 2 + 1] = 'a' + (sec - 10);
    }

    SET_NUMBER(0, 0)
    RETURN_BUFFER((char *)result, 64);
    free(CTX);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AESEncryptInit, 1)
    T_STRING(AESEncryptInit, 0)
    unsigned long length2 = PARAM_LEN(0);
    if ((length2 != 16) && (length2 != 24) && (length2 != 32))
        return (void *)"AESEncryptInit: invalid key size. Key size should have 128, 192 or 256 bits (16, 24 or 32 bytes in length)";

#ifdef __WITH_INTEL_LIB
    if (hardware_aes) {
        AnsiString *k = new AnsiString();
        k->LoadBuffer(PARAM(0), (unsigned long)length2);
        RETURN_NUMBER((SYS_INT)k);
        return 0;
    }
#endif
    AES *a = encrypt_init2(PARAM(0), (unsigned long)length2);
    RETURN_NUMBER((long)a)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AESDecryptInit, 1)
    T_STRING(AESDecryptInit, 0)
    unsigned long length2 = PARAM_LEN(0);
    if ((length2 != 16) && (length2 != 24) && (length2 != 32))
        return (void *)"AESDecryptInit: invalid key size. Key size should have 128, 192 or 256 bits (16, 24 or 32 bytes in length)";

#ifdef __WITH_INTEL_LIB
    if (hardware_aes) {
        AnsiString *k = new AnsiString();
        k->LoadBuffer(PARAM(0), (unsigned long)length2);
        RETURN_NUMBER((SYS_INT)k);
        return 0;
    }
#endif
    AES *a = decrypt_init2(PARAM(0), (unsigned long)length2);
    RETURN_NUMBER((long)a)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AESEncrypt, 2, 4)
    T_HANDLE(AESEncrypt, 0)
    T_STRING(AESEncrypt, 1)

    AES::BlockMode mode = AES::ECB;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AESEncrypt, 2)
        mode = (AES::BlockMode)PARAM_INT(2);
    }

    AES *a = NULL;
    int keySize;
#ifdef __WITH_INTEL_LIB
    AnsiString *key = NULL;
    if (hardware_aes) {
        key     = (AnsiString *)PARAM_INT(0);
        keySize = key->Length();
    } else {
        a       = (AES *)PARAM_INT(0);
        keySize = a->bsize;
    }
#else
    a       = (AES *)PARAM_INT(0);
    keySize = a->bsize;
#endif

    int iSize      = PARAM_LEN(1);
    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

// force PCKS7 padding for complete streams
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(AESEncrypt, 3)
        if ((iRoundSize == iSize) && (PARAM_INT(3)))
            iRoundSize += 16;
    } else
    if (iRoundSize == iSize)
        iRoundSize += 16;

    char *filled = 0;
    char is_new  = 0;
    if (iSize != iRoundSize) {
        is_new = 1;
        filled = (char *)malloc(iRoundSize);
        memcpy(filled, PARAM(1), iSize);
        unsigned char padding = iRoundSize - iSize;
        memset(filled + iSize, padding, (int)padding);
    } else
        filled = PARAM(1);

    long num_blocks = iRoundSize / 16;

    char *out;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AESEncrypt: not enough memory";
    out[iRoundSize] = 0;

#ifdef __WITH_INTEL_LIB
    UCHAR iv[32];
    memset(iv, 0, 32);
    if (hardware_aes) {
        switch (keySize) {
            case 16:
                if (mode == BLOCKMODE_CBC)
                    intel_AES_enc128_CBC((unsigned char *)filled, (unsigned char *)out, (unsigned char *)key->c_str(), num_blocks, iv);
                else
                    intel_AES_enc128((unsigned char *)filled, (unsigned char *)out, (unsigned char *)key->c_str(), num_blocks);
                break;

            case 24:
                if (mode == BLOCKMODE_CBC)
                    intel_AES_enc192_CBC((unsigned char *)filled, (unsigned char *)out, (unsigned char *)key->c_str(), num_blocks, iv);
                else
                    intel_AES_enc192((unsigned char *)filled, (unsigned char *)out, (unsigned char *)key->c_str(), num_blocks);
                break;

            case 32:
                if (mode == BLOCKMODE_CBC)
                    intel_AES_enc256_CBC((unsigned char *)filled, (unsigned char *)out, (unsigned char *)key->c_str(), num_blocks, iv);
                else
                    intel_AES_enc256((unsigned char *)filled, (unsigned char *)out, (unsigned char *)key->c_str(), num_blocks);
                break;
        }
    } else
        a->Encrypt((const unsigned char *)filled, (unsigned char *)out, num_blocks, mode);
#else
    a->Encrypt((const unsigned char *)filled, (unsigned char *)out, num_blocks, mode);
#endif
    if (is_new)
        free(filled);

    SetVariable(RESULT, -1, out, iRoundSize);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AESDecrypt, 2, 4)
    T_HANDLE(AESDecrypt, 0)
    T_STRING(AESDecrypt, 1)

    AES::BlockMode mode = AES::ECB;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AESDecrypt, 2)
        mode = (AES::BlockMode)PARAM_INT(2);
    }

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    AES *context = NULL;
    int keySize;
#ifdef __WITH_INTEL_LIB
    AnsiString *key = NULL;
    if (hardware_aes) {
        key     = (AnsiString *)PARAM_INT(0);
        keySize = key->Length();
    } else {
        context = (AES *)PARAM_INT(0);
        keySize = context->bsize;
    }
#else
    context = (AES *)PARAM_INT(0);
    keySize = context->bsize;
#endif

    if (iSize % 16)
        return (void *)"AESDecrypt: string is not a multiple of AES block size";

    int iRoundSize = iSize;

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AESDecrypt: not enough memory";

    out[iRoundSize] = 0;

#ifdef __WITH_INTEL_LIB
    if (hardware_aes) {
        UCHAR iv[32];
        memset(iv, 0, 32);
        switch (keySize) {
            case 16:
                if (mode == BLOCKMODE_CBC)
                    intel_AES_dec128_CBC((unsigned char *)PARAM(1), (unsigned char *)out, (unsigned char *)key->c_str(), iSize / keySize, iv);
                else
                    intel_AES_dec128((unsigned char *)PARAM(1), (unsigned char *)out, (unsigned char *)key->c_str(), iSize / keySize);
                break;

            case 24:
                if (mode == BLOCKMODE_CBC)
                    intel_AES_dec192_CBC((unsigned char *)PARAM(1), (unsigned char *)out, (unsigned char *)key->c_str(), iSize / keySize, iv);
                else
                    intel_AES_dec192((unsigned char *)PARAM(1), (unsigned char *)out, (unsigned char *)key->c_str(), iSize / keySize);
                break;

            case 32:
                if (mode == BLOCKMODE_CBC)
                    intel_AES_dec256_CBC((unsigned char *)PARAM(1), (unsigned char *)out, (unsigned char *)key->c_str(), iSize / keySize, iv);
                else
                    intel_AES_dec256((unsigned char *)PARAM(1), (unsigned char *)out, (unsigned char *)key->c_str(), iSize / keySize);
                break;
        }
    } else
        context->Decrypt((const unsigned char *)PARAM(1), (unsigned char *)out, iSize / 16, mode);
#else
    context->Decrypt((const unsigned char *)PARAM(1), (unsigned char *)out, iSize / 16, mode);
#endif
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(AESDecrypt, 3)

        if (PARAM_INT(3)) {
            // padding ?
            int padding = out[iRoundSize - 1];
            if ((padding > 0) && (padding <= 16))
                iRoundSize -= padding;
        }
    }
    SetVariable(RESULT, -1, out, iRoundSize);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AESRelease, 1)
    T_NUMBER(AESRelease, 0);
    void *a = (void *)PARAM_INT(0);
    if (a) {
        SET_NUMBER(0, 0);
#ifdef __WITH_INTEL_LIB
        if (hardware_aes)
            delete (AnsiString *)a;
        else
#endif
        delete (AES *)a;
    }
    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(aes_encrypt, 2, 3)
    T_STRING(aes_encrypt, 0)
    T_STRING(aes_encrypt, 1)
    AES::BlockMode mode = AES::ECB;

    unsigned long length2 = PARAM_LEN(1);
    unsigned long length  = PARAM_LEN(0);

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(aes_encrypt, 2)
        mode = (AES::BlockMode)PARAM_INT(2);
    }

    if ((length2 != 16) && (length2 != 24) && (length2 != 32))
        return (void *)"aes_encrypt: invalid key size. Key size should have 128, 192 or 256 bits (16, 24 or 32 bytes in length)";

    if (length == 0) {
        RETURN_STRING((char *)"");
        return 0;
    }

//char *output=new char[(long)length+0xFF];
    char *output = 0;
    CORE_NEW(length + 0x1FF, output);
    output[length] = 0;

    if (!output)
        return (void *)"aes_encrypt: not enough memory";

    encrypt_init(PARAM(1), (unsigned long)length2);
    int size = encrypt(PARAM(0), (unsigned long)length, output, (unsigned long)length + 0xFF, mode);
    if (size < 0)
        size = 0;

    SetVariable(RESULT, -1, output, size);
//RETURN_BUFFER(output, size);
//delete[] output;

END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(aes_decrypt, 2, 3)
    T_STRING(aes_decrypt, 0)
    T_STRING(aes_decrypt, 1)
    AES::BlockMode mode = AES::ECB;

    unsigned long length2 = PARAM_LEN(1);
    unsigned long length  = PARAM_LEN(0);

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(aes_decrypt, 2)
        mode = (AES::BlockMode)PARAM_INT(2);
    }

    if ((length2 != 16) && (length2 != 24) && (length2 != 32))
        return (void *)"aes_decrypt: invalid key size. Key size should have 128, 192 or 256 bits (16, 24 or 32 bytes in length)";

    if (length == 0) {
        RETURN_STRING((char *)"");
        return 0;
    }

//char *output=new char[(long)length+0xFF];
    char *output = 0;
    CORE_NEW(length + 0x1FF, output);
    output[length] = 0;

    if (!output)
        return (void *)"aes_decrypt: not enough memory";

    decrypt_init(PARAM(1), (unsigned long)length2);
    int size = decrypt(PARAM(0), (unsigned long)length, output, (unsigned long)length + 0xFF, mode);
    if (size < 0)
        size = 0;

    SetVariable(RESULT, -1, output, size);
//RETURN_BUFFER(output, size);
//delete[] output;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__rsa_encrypt CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "rsa_encrypt takes 2 parameters : input text, key");
    LOCAL_INIT;

    char   *szParam0;
    NUMBER length;

    char   *szParam1;
    NUMBER length2;

    GET_CHECK_BUFFER(0, szParam0, length, "rsa_encrypt: parameter should be of static string type");
    GET_CHECK_BUFFER(1, szParam1, length2, "rsa_encrypt: parameter should be of static string type");

    if (length == 0) {
        RETURN_STRING((char *)"");
        return 0;
    }

    char *output = (char *)malloc((long)length + 0x500);

    int size = RSA_encrypt(szParam0, (long)length, output, (long)length + 0x100, szParam1, (long)length2);

    if (size < 0)
        size = 0;
    if (!size)
        output[size] = 0;
    RETURN_BUFFER(output, size);
    delete[] output;

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__rsa_decrypt CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "rsa_decrypt takes 2 parameters : input text, key");
    LOCAL_INIT;

    char   *szParam0;
    NUMBER length;

    char   *szParam1;
    NUMBER length2;

    GET_CHECK_BUFFER(0, szParam0, length, "rsa_decrypt: parameter should be of static string type");
    GET_CHECK_BUFFER(1, szParam1, length2, "rsa_decrypt: parameter should be of static string type");

    if (length == 0) {
        RETURN_STRING((char *)"");
        return 0;
    }

    int  buf_len = (long)length + 1;
    char *output = 0;//=(char *)malloc(buf_len);
    CORE_NEW(buf_len, output);
    int size = RSA_decrypt(szParam0, (long)length, output, buf_len, szParam1, (long)length2);

    if (size < 0)
        size = 0;

    if (size) {
        output[size] = 0;
        SetVariable(RESULT, -1, output, size);
        //RETURN_BUFFER(output, size);
    } else {
        CORE_DELETE(output);
        RETURN_STRING("");
    }

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__rsa_generate_keys CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "rsa_generate_keys takes 2 parameters : bits, private_key, public_key");
    LOCAL_INIT;

    NUMBER nParam0;

    GET_CHECK_NUMBER(0, nParam0, "rsa_generate_keys: parameter should be of static number type");

    char public_key[8192];
    char private_key[8192];
    public_key[0]  = 0;
    private_key[0] = 0;
    int public_len  = 8192;
    int private_len = 8192;


    int res = RSA_generate_keys(private_key, public_key, &private_len, &public_len, (int)nParam0);
    if (res) {
        SET_BUFFER(1, private_key, private_len);
        SET_BUFFER(2, public_key, public_len);
    } else {
        SET_STRING(1, "");
        SET_STRING(2, "");
    }


    RETURN_NUMBER(res);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__rsa_sign CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "rsa_sign takes 2 parameters : input text, private_key");
    LOCAL_INIT;

    char   *szParam0;
    NUMBER length;

    char   *szParam1;
    NUMBER length2;

    GET_CHECK_BUFFER(0, szParam0, length, "rsa_sign: parameter should be of static string type");
    GET_CHECK_BUFFER(1, szParam1, length2, "rsa_sign: parameter should be of static string type");

    if (length == 0) {
        RETURN_STRING((char *)"");
        return 0;
    }

    char *output = new char[(long)length + 0x500];

    int size = RSA_sign(szParam0, (long)length, output, (long)length + 0x100, szParam1, (long)length2);

    if (size < 0)
        size = 0;
    if (!size)
        output[0] = 0;
    RETURN_BUFFER(output, size);
    delete[] output;

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__rsa_verify CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "rsa_verify takes 2 parameters : input text, public_key");
    LOCAL_INIT;

    char   *szParam0;
    NUMBER length;

    char   *szParam1;
    NUMBER length2;

    GET_CHECK_BUFFER(0, szParam0, length, "rsa_verify: parameter should be of static string type");
    GET_CHECK_BUFFER(1, szParam1, length2, "rsa_verify: parameter should be of static string type");

    if (length == 0) {
        RETURN_STRING((char *)"");
        return 0;
    }

    char *output = new char[(long)length + 0x500];

    int size = RSA_verify(szParam0, (long)length, output, (long)length + 0x100, szParam1, (long)length2);

    if (size < 0)
        size = 0;
    if (!size)
        output[size] = 0;
    RETURN_BUFFER(output, size);
    delete[] output;

    return 0;
}
//---------------------------------------------------------------------------
unsigned get_value(unsigned char c1) {
    switch (c1) {
        case 'a':
        case 'A':
            return 10;

        case 'b':
        case 'B':
            return 11;

        case 'c':
        case 'C':
            return 12;

        case 'd':
        case 'D':
            return 13;

        case 'e':
        case 'E':
            return 14;

        case 'f':
        case 'F':
            return 15;

        default:
            return c1 - '0';
    }
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(hmac_md5, 3)
    T_STRING(hmac_md5, 0)
    T_STRING(hmac_md5, 1)

    unsigned char buf[16];

    if (PARAM_LEN(0) % 2)
        FAIL_ERROR("hmac_sha1: key length must be multiple of 2");

    char key_len = PARAM_LEN(0) / 2;
    char *key    = new char[key_len + 1];
    key[0] = 0;

    char *k = PARAM(0);
    for (int i = 0; i < key_len; i++) {
        unsigned char c1 = get_value(k[i * 2]);
        unsigned char c2 = get_value(k[i * 2 + 1]);
        unsigned char c  = c1 * 0x10 + c2;

        key[i] = c;
    }

    RETURN_NUMBER(hmac_md5(key, key_len, PARAM(1), PARAM_LEN(1), buf));

    delete[] key;

    unsigned char result[32];
    for (int i = 0; i < 16; i++) {
        unsigned char first = buf[i] / 0x10;
        unsigned char sec   = buf[i] % 0x10;

        if (first < 10)
            result[i * 2] = '0' + first;
        else
            result[i * 2] = 'a' + (first - 10);

        if (sec < 10)
            result[i * 2 + 1] = '0' + sec;
        else
            result[i * 2 + 1] = 'a' + (sec - 10);
    }


    SET_BUFFER(2, (char *)result, 32);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(hmac_sha1, 3)
    T_STRING(hmac_sha1, 0)
    T_STRING(hmac_sha1, 1)

    unsigned char buf[20];


    if (PARAM_LEN(0) % 2)
        FAIL_ERROR("hmac_sha1: key length must be multiple of 2");

    char key_len = PARAM_LEN(0) / 2;
    char *key    = new char[key_len + 1];
    key[0] = 0;

    char *k = PARAM(0);
    for (int i = 0; i < key_len; i++) {
        unsigned char c1 = get_value(k[i * 2]);
        unsigned char c2 = get_value(k[i * 2 + 1]);
        unsigned char c  = c1 * 0x10 + c2;

        key[i] = c;
    }

    RETURN_NUMBER(hmac_sha1(key, key_len, PARAM(1), PARAM_LEN(1), buf));
    delete[] key;

    unsigned char result[40];
    for (int i = 0; i < 20; i++) {
        unsigned char first = buf[i] / 0x10;
        unsigned char sec   = buf[i] % 0x10;

        if (first < 10)
            result[i * 2] = '0' + first;
        else
            result[i * 2] = 'a' + (first - 10);

        if (sec < 10)
            result[i * 2 + 1] = '0' + sec;
        else
            result[i * 2 + 1] = 'a' + (sec - 10);
    }


    SET_BUFFER(2, (char *)result, 40);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(hmac_sha256, 3, 4)
    T_STRING(hmac_sha256, 0)
    T_STRING(hmac_sha256, 1)
    int raw = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(hmac_sha256, 3)
        raw = PARAM_INT(3);
    }

    unsigned char buf[32];
    memset(buf, 0, 32);


    if (PARAM_LEN(0) % 2)
        FAIL_ERROR("hmac_sha256: key length must be multiple of 2");

    char key_len = PARAM_LEN(0) / 2;
    char *key    = new char[key_len + 1];
    key[0] = 0;

    char *k = PARAM(0);
    for (int i = 0; i < key_len; i++) {
        unsigned char c1 = get_value(k[i * 2]);
        unsigned char c2 = get_value(k[i * 2 + 1]);
        unsigned char c  = c1 * 0x10 + c2;

        key[i]     = c;
        key[i + 1] = 0;
    }

    RETURN_NUMBER(do_hmac_sha256(key, key_len, (const uint8_t *)PARAM(1), PARAM_LEN(1), (char *)buf));
    delete[] key;

    if (!raw) {
        unsigned char result[64];
        for (int i = 0; i < 32; i++) {
            unsigned char first = buf[i] / 0x10;
            unsigned char sec   = buf[i] % 0x10;

            if (first < 10)
                result[i * 2] = '0' + first;
            else
                result[i * 2] = 'a' + (first - 10);

            if (sec < 10)
                result[i * 2 + 1] = '0' + sec;
            else
                result[i * 2 + 1] = 'a' + (sec - 10);
        }

        SET_BUFFER(2, (char *)result, 64);
    } else {
        SET_BUFFER(2, (char *)buf, 32);
    }
END_IMPL
//---------------------------------------------------------------------------
unsigned int murmur_hash(const void *key, long len) {
    if (!key)
        return 0;
    const unsigned int  m     = 0x5bd1e995;
    const int           r     = 24;
    unsigned int        seed  = 0x7870AAFF;
    const unsigned char *data = (const unsigned char *)key;
    //int len = strlen((const char *)data);
    if (!len)
        return 0;

    unsigned int h = seed ^ len;

    while (len >= 4) {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len  -= 4;
    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;

        case 2:
            h ^= data[1] << 8;

        case 1:
            h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

CONCEPT_FUNCTION_IMPL(Murmur, 1)
    T_STRING(Murmur, 0)

    unsigned int res = murmur_hash(PARAM(0), PARAM_LEN(0));
    RETURN_NUMBER(res)
END_IMPL
//---------------------------------------------------------------------------
#define ROT32(x, y)    ((x << y) | (x >> (32 - y))) // avoid effor
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed) {
    static const uint32_t c1 = 0xcc9e2d51;
    static const uint32_t c2 = 0x1b873593;
    static const uint32_t r1 = 15;
    static const uint32_t r2 = 13;
    static const uint32_t m  = 5;
    static const uint32_t n  = 0xe6546b64;

    uint32_t hash = seed;

    const int      nblocks = len / 4;
    const uint32_t *blocks = (const uint32_t *)key;
    int            i;
    uint32_t       k;

    for (i = 0; i < nblocks; i++) {
        k  = blocks[i];
        k *= c1;
        k  = ROT32(k, r1);
        k *= c2;

        hash ^= k;
        hash  = ROT32(hash, r2) * m + n;
    }

    const uint8_t *tail = (const uint8_t *)(key + nblocks * 4);
    uint32_t      k1    = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;

        case 2:
            k1 ^= tail[1] << 8;

        case 1:
            k1 ^= tail[0];

            k1   *= c1;
            k1    = ROT32(k1, r1);
            k1   *= c2;
            hash ^= k1;
    }

    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(Murmur3, 1, 2)
    T_STRING(Murmur3, 0)
    uint32_t seed = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(Murmur3, 1)
        seed = PARAM(1);
    }

    NUMBER res = murmur3_32(PARAM(0), PARAM_LEN(0), seed);
    RETURN_NUMBER(res)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DRMKey, 0)
    char *res = LOCAL_PUBLIC_KEY;
//if (!res)
//    res=getenv("DRM_KEY");

    if (res) {
        int len = strlen(res);
        if (len < 16)
            len = 16;

        RETURN_BUFFER(res, len)
    } else {
        RETURN_STRING("")
    }
END_IMPL
//---------------------------------------------------------------------------
int pkcs5_pbkdf2(const char *pass, size_t pass_len, const char *salt, size_t salt_len, unsigned char *key, int key_len, unsigned int rounds) {
    unsigned char *asalt, obuf[20];
    unsigned char d1[20], d2[20];
    unsigned int  i, j;
    unsigned int  count;
    int           r;

    if ((rounds < 1) || (key_len == 0))
        return -1;
    if ((salt_len == 0) || (salt_len > 0xFFFF - 1))
        return -1;
    if ((asalt = (unsigned char *)malloc(salt_len + 4)) == NULL)
        return -1;

    memcpy(asalt, salt, salt_len);

    for (count = 1; key_len > 0; count++) {
        asalt[salt_len + 0] = (count >> 24) & 0xff;
        asalt[salt_len + 1] = (count >> 16) & 0xff;
        asalt[salt_len + 2] = (count >> 8) & 0xff;
        asalt[salt_len + 3] = count & 0xff;
        hmac_sha1(pass, pass_len, asalt, salt_len + 4, d1);
        memcpy(obuf, d1, sizeof(obuf));

        for (i = 1; i < rounds; i++) {
            hmac_sha1(pass, pass_len, d1, sizeof(d1), d2);
            memcpy(d1, d2, sizeof(d1));
            for (j = 0; j < sizeof(obuf); j++)
                obuf[j] ^= d1[j];
        }

        r = (key_len < 20) ? 20 : key_len;
        memcpy(key, obuf, r);
        key     += r;
        key_len -= r;
    }
    free(asalt);
    return 0;
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PBKDF2, 2, 4)
    T_STRING(PBKDF2, 0)
    T_STRING(PBKDF2, 1)
    int n_rounds = 1000;
    int key_size = 16;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(PBKDF2, 2)
        key_size = PARAM_INT(2);
        if (PARAMETERS_COUNT > 3) {
            T_NUMBER(PBKDF2, 3)
            n_rounds = PARAM_INT(3);
        }
    }
    if (n_rounds < 1)
        n_rounds = 1;

    if (key_size <= 0) {
        RETURN_STRING("");
        return 0;
    }
    char *key = (char *)malloc(key_size + 20);
    if (key) {
        int res = pkcs5_pbkdf2(PARAM(0), PARAM_LEN(0), PARAM(1), PARAM_LEN(1), (unsigned char *)key, key_size, n_rounds);
        if (res < 0) {
            RETURN_STRING("");
        } else {
            RETURN_BUFFER(key, key_size);
        }
        free(key);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
#ifdef _USE_SECONDARY_AES
CONCEPT_FUNCTION_IMPL(AesDone, 1)
    T_NUMBER(AesDone, 0)
    AesCtx * context = (AesCtx *)PARAM_INT(0);
    if (context) {
        delete context;
        SET_NUMBER(0, 0)
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesSetKey, 2, 3)
    T_STRING(AesSetKey, 1)
    T_NUMBER(AesSetKey, 0)

    AesCtx * context = new AesCtx;
    char             *init_vector = 0;
    _AES_BLOCK_MODE_ mode         = (_AES_BLOCK_MODE_)PARAM_INT(0);
    if (PARAM_LEN(1) != 16) {
        return (void *)"AesSetKey: key must be exactly 128 bits long (16 bytes)";
    }
    char *key = PARAM(1);
    if (PARAMETERS_COUNT > 2) {
        T_STRING(AesSetKey, 2);
        int vlen = PARAM_LEN(2);
        if (vlen) {
            if (vlen != 16)
                return (void *)"AesSetKey: initialization vector must be exactly 128 bits long (16 bytes)";
            init_vector = PARAM(2);
        }
    }

    AesSetKey(context, mode, key, init_vector);
    RETURN_NUMBER((SYS_INT)context);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AesSetInitVector, 2)
    T_HANDLE(AesSetInitVector, 0)
    T_STRING(AesSetInitVector, 1);

    AesCtx *context     = (AesCtx *)PARAM_INT(0);
    char   *init_vector = 0;
    int    vlen         = PARAM_LEN(1);
    if (vlen) {
        if (vlen != 16)
            return (void *)"AesSetInitVector: initialization vector must be exactly 128 bits long (16 bytes)";
        init_vector = PARAM(1);
    }
    AesSetInitVector(context, init_vector);
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AesSetFeedbackSize, 2)
    T_HANDLE(AesSetFeedbackSize, 0)
    T_NUMBER(AesSetFeedbackSize, 1)
    AesCtx * context = (AesCtx *)PARAM_INT(0);
    AesSetFeedbackSize(context, PARAM_INT(1));
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesEncryptECB, 2, 3)
    T_HANDLE(AesEncryptECB, 0)
    T_STRING(AesEncryptECB, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

// force PCKS7 padding for complete streams
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesEncryptECB, 2)
        if ((iRoundSize == iSize) && (PARAM_INT(2)))
            iRoundSize += 16;
    } else
    if (iRoundSize == iSize)
        iRoundSize += 16;

    char *filled = 0;
    char is_new  = 0;
    if (iSize != iRoundSize) {
        is_new = 1;
        filled = new char[iRoundSize];
        memcpy(filled, PARAM(1), iSize);
        unsigned char padding = iRoundSize - iSize;
        memset(filled + iSize, padding, (int)padding);
    } else
        filled = PARAM(1);

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesEncryptECB: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesEncryptECB(context, out, filled, iRoundSize);
    SetVariable(RESULT, -1, out, iRoundSize);

    if (is_new)
        delete[] filled;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesEncryptCBC, 2, 3)
    T_HANDLE(AesEncryptCBC, 0)
    T_STRING(AesEncryptCBC, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

// force PCKS7 padding for complete streams
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesEncryptCBC, 2)
        if ((iRoundSize == iSize) && (PARAM_INT(2)))
            iRoundSize += 16;
    } else
    if (iRoundSize == iSize)
        iRoundSize += 16;

    char *filled = 0;
    char is_new  = 0;
    if (iSize != iRoundSize) {
        is_new = 1;
        filled = new char[iRoundSize];
        memcpy(filled, PARAM(1), iSize);
        unsigned char padding = iRoundSize - iSize;
        memset(filled + iSize, padding, (int)padding);
    } else
        filled = PARAM(1);

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesEncryptCBC: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesEncryptCBC(context, out, filled, iRoundSize);
    SetVariable(RESULT, -1, out, iRoundSize);

    if (is_new)
        delete[] filled;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesEncryptPCBC, 2, 3)
    T_HANDLE(AesEncryptPCBC, 0)
    T_STRING(AesEncryptPCBC, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

// force PCKS7 padding for complete streams
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesEncryptPCBC, 2)
        if ((iRoundSize == iSize) && (PARAM_INT(2)))
            iRoundSize += 16;
    } else
    if (iRoundSize == iSize)
        iRoundSize += 16;

    char *filled = 0;
    char is_new  = 0;
    if (iSize != iRoundSize) {
        is_new = 1;
        filled = new char[iRoundSize];
        memcpy(filled, PARAM(1), iSize);
        unsigned char padding = iRoundSize - iSize;
        memset(filled + iSize, padding, (int)padding);
    } else
        filled = PARAM(1);

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesEncryptPCBC: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesEncryptPCBC(context, out, filled, iRoundSize);
    SetVariable(RESULT, -1, out, iRoundSize);

    if (is_new)
        delete[] filled;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesEncryptCRT, 2, 3)
    T_HANDLE(AesEncryptCRT, 0)
    T_STRING(AesEncryptCRT, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

// force PCKS7 padding for complete streams
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesEncryptCRT, 2)
        if ((iRoundSize == iSize) && (PARAM_INT(2)))
            iRoundSize += 16;
    } else
    if (iRoundSize == iSize)
        iRoundSize += 16;

    char *filled = 0;
    char is_new  = 0;
    if (iSize != iRoundSize) {
        is_new = 1;
        filled = new char[iRoundSize];
        memcpy(filled, PARAM(1), iSize);
        unsigned char padding = iRoundSize - iSize;
        memset(filled + iSize, padding, (int)padding);
    } else
        filled = PARAM(1);

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesEncryptCRT: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesEncryptCRT(context, out, filled, iRoundSize);
    SetVariable(RESULT, -1, out, iRoundSize);

    if (is_new)
        delete[] filled;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesEncryptOFB, 2, 3)
    T_HANDLE(AesEncryptOFB, 0)
    T_STRING(AesEncryptOFB, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

// force PCKS7 padding for complete streams
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesEncryptOFB, 2)
        if ((iRoundSize == iSize) && (PARAM_INT(2)))
            iRoundSize += 16;
    } else
    if (iRoundSize == iSize)
        iRoundSize += 16;

    char *filled = 0;
    char is_new  = 0;
    if (iSize != iRoundSize) {
        is_new = 1;
        filled = new char[iRoundSize];
        memcpy(filled, PARAM(1), iSize);
        unsigned char padding = iRoundSize - iSize;
        memset(filled + iSize, padding, (int)padding);
    } else
        filled = PARAM(1);

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesEncryptOFB: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesEncryptOFB(context, out, filled, iRoundSize);
    SetVariable(RESULT, -1, out, iRoundSize);

    if (is_new)
        delete[] filled;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesEncryptCFB, 2, 3)
    T_HANDLE(AesEncryptCFB, 0)
    T_STRING(AesEncryptCFB, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

// force PCKS7 padding for complete streams
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesEncryptCFB, 2)
        if ((iRoundSize == iSize) && (PARAM_INT(2)))
            iRoundSize += 16;
    } else
    if (iRoundSize == iSize)
        iRoundSize += 16;

    char *filled = 0;
    char is_new  = 0;
    if (iSize != iRoundSize) {
        is_new = 1;
        filled = new char[iRoundSize];
        memcpy(filled, PARAM(1), iSize);
        unsigned char padding = iRoundSize - iSize;
        memset(filled + iSize, padding, (int)padding);
    } else
        filled = PARAM(1);

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesEncryptCFB: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesEncryptCFB(context, out, filled, iRoundSize);
    SetVariable(RESULT, -1, out, iRoundSize);

    if (is_new)
        delete[] filled;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesDecryptECB, 2, 3)
    T_HANDLE(AesDecryptECB, 0)
    T_STRING(AesDecryptECB, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    if (iSize % 16)
        return (void *)"AesDecryptECB: string is not a multiple of 16 bytes";

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesDecryptECB: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesDecryptECB(context, out, PARAM(1), iRoundSize);
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesDecryptECB, 2)

        if (PARAM_INT(2)) {
            // padding ?
            int padding = out[iRoundSize - 1];
            if ((padding > 0) && (padding <= 16))
                iRoundSize -= padding;
        }
    }
    SetVariable(RESULT, -1, out, iRoundSize);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesDecryptCBC, 2, 3)
    T_HANDLE(AesDecryptCBC, 0)
    T_STRING(AesDecryptCBC, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    if (iSize % 16)
        return (void *)"AesDecryptCBC: string is not a multiple of 16 bytes";

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesDecryptCBC: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesDecryptCBC(context, out, PARAM(1), iRoundSize);
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesDecryptCBC, 2)

        if (PARAM_INT(2)) {
            // padding ?
            int padding = out[iRoundSize - 1];
            if ((padding > 0) && (padding <= 16))
                iRoundSize -= padding;
        }
    }
    SetVariable(RESULT, -1, out, iRoundSize);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesDecryptPCBC, 2, 3)
    T_HANDLE(AesDecryptPCBC, 0)
    T_STRING(AesDecryptPCBC, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    if (iSize % 16)
        return (void *)"AesDecryptPCBC: string is not a multiple of 16 bytes";

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesDecryptPCBC: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesDecryptPCBC(context, out, PARAM(1), iRoundSize);
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesDecryptPCBC, 2)

        if (PARAM_INT(2)) {
            // padding ?
            int padding = out[iRoundSize - 1];
            if ((padding > 0) && (padding <= 16))
                iRoundSize -= padding;
        }
    }
    SetVariable(RESULT, -1, out, iRoundSize);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesDecryptCRT, 2, 3)
    T_HANDLE(AesDecryptCRT, 0)
    T_STRING(AesDecryptCRT, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    if (iSize % 16)
        return (void *)"AesDecryptCRT: string is not a multiple of 16 bytes";

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesDecryptCRT: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesDecryptCRT(context, out, PARAM(1), iRoundSize);
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesDecryptCRT, 2)

        if (PARAM_INT(2)) {
            // padding ?
            int padding = out[iRoundSize - 1];
            if ((padding > 0) && (padding <= 16))
                iRoundSize -= padding;
        }
    }
    SetVariable(RESULT, -1, out, iRoundSize);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesDecryptOFB, 2, 3)
    T_HANDLE(AesDecryptOFB, 0)
    T_STRING(AesDecryptOFB, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    if (iSize % 16)
        return (void *)"AesDecryptOFB: string is not a multiple of 16 bytes";

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesDecryptOFB: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesDecryptOFB(context, out, PARAM(1), iRoundSize);
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesDecryptOFB, 2)

        if (PARAM_INT(2)) {
            // padding ?
            int padding = out[iRoundSize - 1];
            if ((padding > 0) && (padding <= 16))
                iRoundSize -= padding;
        }
    }
    SetVariable(RESULT, -1, out, iRoundSize);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AesDecryptCFB, 2, 3)
    T_HANDLE(AesDecryptCFB, 0)
    T_STRING(AesDecryptCFB, 1)

    int iSize = PARAM_LEN(1);
    if (!iSize) {
        RETURN_STRING("");
        return 0;
    }

    if (iSize % 16)
        return (void *)"AesDecryptCFB: string is not a multiple of 16 bytes";

    int iRoundSize = AesRoundSize(iSize, 16);
    if (!iRoundSize) {
        RETURN_STRING("");
        return 0;
    }

    char *out = 0;
    CORE_NEW(iRoundSize + 1, out);
    if (!out)
        return (void *)"AesDecryptCFB: not enough memory";

    out[iRoundSize] = 0;
    AesCtx *context = (AesCtx *)PARAM_INT(0);
    AesDecryptCFB(context, out, PARAM(1), iRoundSize);
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AesDecryptCFB, 2)

        if (PARAM_INT(2)) {
            // padding ?
            int padding = out[iRoundSize - 1];
            if ((padding > 0) && (padding <= 16))
                iRoundSize -= padding;
        }
    }
    SetVariable(RESULT, -1, out, iRoundSize);
END_IMPL
//---------------------------------------------------------------------------
#endif
#ifdef __WITH_TOMCRYPT
CONCEPT_FUNCTION_IMPL(ECCKeys, 3)
    T_NUMBER(ECCKeys, 0)
    ecc_key key;
    SET_STRING(1, "");
    SET_STRING(2, "");
    int err;
    if ((err = ecc_make_key(NULL, find_prng("sprng"), PARAM_INT(0) / 8, &key)) != CRYPT_OK) {
        RETURN_NUMBER(err);
        return 0;
    }
    unsigned char out[8192];
    unsigned long len = 8192;
    if (!ecc_export(out, &len, PK_PUBLIC, &key)) {
        SET_BUFFER(1, (char *)out, len);
    }
    len = 8192;
    if (!ecc_export(out, &len, PK_PRIVATE, &key)) {
        SET_BUFFER(2, (char *)out, len);
    }
    ecc_free(&key);
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ECCEncrypt, 2, 3)
    T_STRING(ECCEncrypt, 0)
    T_STRING(ECCEncrypt, 1)
    unsigned long size = PARAM_LEN(0) + 8192;
    char    *out = 0;
    int     err;
    ecc_key key;
    err = ecc_import((unsigned char *)PARAM(1), PARAM_LEN(1), &key);
    if (err) {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, err);
        }
        RETURN_STRING("");
        return 0;
    }
    CORE_NEW(size, out);
    if (out) {
        int hash_idx = find_hash("sha1");
        int prng_idx = find_prng("sprng");
        err = ecc_encrypt_key((unsigned char *)PARAM(0), PARAM_LEN(0), (unsigned char *)out, &size, NULL, prng_idx, hash_idx, &key);
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, err);
        }
        if ((err == CRYPT_OK) && (size > 0)) {
            SetVariable(RESULT, -1, out, size);
        } else {
            CORE_DELETE(out);
            RETURN_STRING("");
        }
    } else {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, 0);
        }

        RETURN_STRING("");
    }
    ecc_free(&key);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ECCDecrypt, 2, 3)
    T_STRING(ECCDecrypt, 0)
    T_STRING(ECCDecrypt, 1)
    unsigned long size = PARAM_LEN(0) + 1;
    char    *out = 0;
    int     err;
    ecc_key key;
    err = ecc_import((unsigned char *)PARAM(1), PARAM_LEN(1), &key);
    if (err) {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, err);
        }
        RETURN_STRING("");
        return 0;
    }
    CORE_NEW(size, out);
    if (out) {
        err = ecc_decrypt_key((unsigned char *)PARAM(0), PARAM_LEN(0), (unsigned char *)out, &size, &key);
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, err);
        }
        if ((err == CRYPT_OK) && (size > 0)) {
            SetVariable(RESULT, -1, out, size);
        } else {
            CORE_DELETE(out);
            RETURN_STRING("");
        }
    } else {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, 0);
        }

        RETURN_STRING("");
    }
    ecc_free(&key);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(RSAKeys, 3)
    T_NUMBER(RSAKeys, 0)
    rsa_key key;
    SET_STRING(1, "");
    SET_STRING(2, "");
    int err;
    if ((err = rsa_make_key(NULL, find_prng("sprng"), PARAM_INT(0) / 8, 65537, &key)) != CRYPT_OK) {
        RETURN_NUMBER(err);
        return 0;
    }
    unsigned char out[8192];
    unsigned long len = 8192;
    if (!rsa_export(out, &len, PK_PUBLIC, &key)) {
        SET_BUFFER(1, (char *)out, len);
    }
    len = 8192;
    if (!rsa_export(out, &len, PK_PRIVATE, &key)) {
        SET_BUFFER(2, (char *)out, len);
    }
    rsa_free(&key);
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(RSAEncrypt, 2, 3)
    T_STRING(RSAEncrypt, 0)
    T_STRING(RSAEncrypt, 1)
    unsigned long size = PARAM_LEN(0) + 8192;
    char    *out = 0;
    int     err;
    rsa_key key;
    err = rsa_import((unsigned char *)PARAM(1), PARAM_LEN(1), &key);
    if (err) {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, err);
        }
        RETURN_STRING("");
        return 0;
    }
    CORE_NEW(size, out);
    if (out) {
        int hash_idx = find_hash("sha1");
        int prng_idx = find_prng("sprng");
        err = rsa_encrypt_key((unsigned char *)PARAM(0), PARAM_LEN(0), (unsigned char *)out, &size, (unsigned char *)"Concept", 7, NULL, prng_idx, hash_idx, &key);
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, err);
        }
        if ((err == CRYPT_OK) && (size > 0)) {
            SetVariable(RESULT, -1, out, size);
        } else {
            CORE_DELETE(out);
            RETURN_STRING("");
        }
    } else {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, 0);
        }

        RETURN_STRING("");
    }
    rsa_free(&key);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(RSADecrypt, 2, 4)
    T_STRING(RSADecrypt, 0)
    T_STRING(RSADecrypt, 1)
    unsigned long size = PARAM_LEN(0) + 1;
    char    *out = 0;
    int     err;
    rsa_key key;
    if (PARAMETERS_COUNT > 3) {
        SET_NUMBER(3, 0);
    }
    err = rsa_import((unsigned char *)PARAM(1), PARAM_LEN(1), &key);
    if (err) {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, err);
        }
        RETURN_STRING("");
        return 0;
    }
    CORE_NEW(size, out);
    if (out) {
        int hash_idx = find_hash("sha1");
        int res      = 0;
        err = rsa_decrypt_key((unsigned char *)PARAM(0), PARAM_LEN(0), (unsigned char *)out, &size, (unsigned char *)"Concept", 7, hash_idx, &res, &key);
        if (PARAMETERS_COUNT > 3) {
            SET_NUMBER(3, res);
        }
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, err);
        }
        if ((err == CRYPT_OK) && (size > 0)) {
            SetVariable(RESULT, -1, out, size);
        } else {
            CORE_DELETE(out);
            RETURN_STRING("");
        }
    } else {
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, 0);
        }

        RETURN_STRING("");
    }
    rsa_free(&key);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ECCRSAErrorExplain, 1)
    T_NUMBER(ECCRSAErrorExplain, 0)
    char *err = (char *)error_to_string(PARAM_INT(0));
    if (err) {
        RETURN_STRING(err);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(CryptoRandom, 1)
    T_NUMBER(CryptoRandom, 0)

    int len = PARAM_INT(0);
    if (len <= 0) {
        RETURN_STRING("");
        return 0;
    }
    char *key = NULL;
    CORE_NEW(len + 1, key);
    if (!key) {
        RETURN_STRING("");
        return 0;
    }
    key[len] = 0;
#ifdef __APPLE__
    for (int i = 0; i < len; i++) {
        unsigned int v = arc4random() % 0x100;
        key[i] = (char)v;
    }
    SetVariable(RESULT, -1, key, len);
#else
#ifdef _WIN32
    HCRYPTPROV hProvider = 0;
    if (CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
        if (CryptGenRandom(hProvider, len, (BYTE *)key)) {
            CryptReleaseContext(hProvider, 0);
            SetVariable(RESULT, -1, key, len);
            return 0;
        }
        CryptReleaseContext(hProvider, 0);
    }
#else
    FILE *fp = fopen("/dev/urandom", "r");
    if (fp) {
        int key_len = fread(key, 1, len, fp);
        fclose(fp);
        if (key_len == len) {
            SetVariable(RESULT, -1, key, len);
            return 0;
        }
    }
#endif
    if (key) {
        CORE_DELETE(key);
    }
    RETURN_STRING("");
#endif
END_IMPL
//---------------------------------------------------------------------------
typedef struct {
    void *x;
    void *y;
    void *p;
    void *g;
} DHKey;

void __private_dh_clear_key(DHKey *key) {
    mp_clear_multi(key->g, key->p, key->x, key->y, NULL);
    key->g = NULL;
    key->p = NULL;
    key->x = NULL;
    key->y = NULL;
}

int __private_dh_make_key(int keysize, DHKey *key, const char *pbuf, const char *gbuf, int pbuf_len, int gbuf_len) {
    unsigned char *buf;
    int err;
    
    static prng_state prng;
    int wprng = find_prng("sprng");
    if ((err = prng_is_valid(wprng)) != CRYPT_OK)
        return err;

    buf = (unsigned char *)malloc(keysize);
    if (!buf)
        return -1;
    
    if (rng_make_prng(keysize, wprng, &prng, NULL) != CRYPT_OK) {
        free(buf);
        return -1;
    }
    
    if (prng_descriptor[wprng].read(buf, keysize, &prng) != (unsigned long)keysize) {
        free(buf);
        return -1;
    }
    
    if ((err = mp_init_multi(&key->g, &key->p, &key->x, &key->y, NULL)) != CRYPT_OK) {
        free(buf);
        return -1;
    }
    
    if (gbuf_len <= 0) {
        if ((err = mp_read_radix(key->g, gbuf, 16)) != CRYPT_OK) {
            free(buf);
            __private_dh_clear_key(key);
            return -1;
        }
    } else {
        if ((err = mp_read_unsigned_bin(key->g, (unsigned char *)gbuf, gbuf_len)) != CRYPT_OK) {
            free(buf);
            __private_dh_clear_key(key);
            return -1;
        }
    }
    
    if (pbuf_len <= 0) {
        if ((err = mp_read_radix(key->p, pbuf, 16)) != CRYPT_OK) {
            free(buf);
            __private_dh_clear_key(key);
            return -1;
        }
    } else {
        if ((err = mp_read_unsigned_bin(key->p, (unsigned char *)pbuf, pbuf_len)) != CRYPT_OK) {
            free(buf);
            __private_dh_clear_key(key);
            return -1;
        }
    }
    
    if ((err = mp_read_unsigned_bin(key->x, buf, keysize)) != CRYPT_OK) {
        free(buf);
        __private_dh_clear_key(key);
        return -1;
    }
    
    if ((err = mp_exptmod(key->g, key->x, key->p, key->y)) != CRYPT_OK) {
        free(buf);
        __private_dh_clear_key(key);
        return -1;
    }
    
    free(buf);
    return 0;
}

int __private_dh_shared_secret(const char *private_x, const char *private_p, const char *public_y, unsigned char *out, unsigned long *outlen) {
    void *tmp;
    void *px, *pp, *pub_y;
    unsigned long x;
    int err;
    
    
    /* compute y^x mod p */
    if ((err = mp_init_multi(&tmp, &px, &pp, &pub_y, NULL)) != CRYPT_OK)
        return err;
    
    if ((err = mp_read_radix(px, private_x, 16)) != CRYPT_OK) {
        mp_clear_multi(tmp, px, pp, pub_y, NULL);
        return -1;
    }
    if ((err = mp_read_radix(pp, private_p, 16)) != CRYPT_OK) {
        mp_clear_multi(tmp, px, pp, pub_y, NULL);
        return -1;
    }
    if ((err = mp_read_radix(pub_y, public_y, 16)) != CRYPT_OK) {
        mp_clear_multi(tmp, px, pp, pub_y, NULL);
        return -1;
    }
    if ((err = mp_exptmod(pub_y, px, pp, tmp)) != CRYPT_OK) {
        mp_clear_multi(tmp, px, pp, pub_y, NULL);
        return err;
    }
    
    x = (unsigned long)mp_unsigned_bin_size(tmp);
    if (*outlen < x) {
        err = CRYPT_BUFFER_OVERFLOW;
        mp_clear_multi(tmp, px, pp, pub_y, NULL);
        return err;
    }
    
    if ((err = mp_to_unsigned_bin(tmp, out)) != CRYPT_OK) {
        mp_clear_multi(tmp, px, pp, pub_y, NULL);
        return err;
    }
    *outlen = x;
    mp_clear_multi(tmp, px, pp, pub_y, NULL);
    return 0;
}
//---------------------------------------------------------------------------
int __private_eccdh_shared_secret(const ltc_ecc_set_type *dp, const char *pub_key_x, const char *pub_key_y, const char *pub_key_z, const char *priv_key, unsigned char *out, unsigned long *outlen) {
    int err;
 
    ecc_key public_key;
    ecc_key private_key;

    memset(&public_key, 0, sizeof(ecc_key));
    memset(&private_key, 0, sizeof(ecc_key));

    if (mp_init_multi(&public_key.pubkey.x, &public_key.pubkey.y, &public_key.pubkey.z, &private_key.k, NULL))
        return -1;
    
    if ((err = mp_read_radix(public_key.pubkey.x, pub_key_x, 16)) != CRYPT_OK) {
        mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
        return -1;
    }
    if ((err = mp_read_radix(public_key.pubkey.y, pub_key_y, 16)) != CRYPT_OK) {
        mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
        return -1;
    }
    if ((err = mp_read_radix(public_key.pubkey.z, pub_key_z, 16)) != CRYPT_OK) {
        mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
        return -1;
    }
    if ((err = mp_read_radix(private_key.k, priv_key, 16)) != CRYPT_OK) {
        mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
        return -1;
    }

    public_key.idx = -1;
    public_key.dp = dp;
    public_key.type = PK_PUBLIC;

    private_key.idx = -1;
    private_key.dp = dp;
    private_key.type = PK_PRIVATE;

    err = ecc_shared_secret(&private_key, &public_key, out, outlen);
    mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
    return err;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(DHMakeKey, 0, 3)
    int key_size = 2048;
    const char *dhe_p = default_dhe_p;
    const char *dhe_g = default_dhe_g;

    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(DHMakeKey, 0)
        key_size = PARAM_INT(0);
    }
    if (PARAMETERS_COUNT > 1) {
        T_STRING(DHMakeKey, 1)
        dhe_p = PARAM(1);
    }
    if (PARAMETERS_COUNT > 1) {
        T_STRING(DHMakeKey, 2)
        dhe_g = PARAM(2);
    }
    DHKey dhe;
    memset(&dhe, 0, sizeof(dhe));
    CREATE_ARRAY(RESULT);
    if (!__private_dh_make_key(key_size/8, &dhe, dhe_p, dhe_g, 0, 0)) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "p", (INTEGER)VARIABLE_STRING, (char *)dhe_p, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "g", (INTEGER)VARIABLE_STRING, (char *)dhe_g, (NUMBER)0);
        char str[0xFFF];
        str[0] = 0;
        mp_toradix(dhe.y, str, 16);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "y", (INTEGER)VARIABLE_STRING, (char *)str, (NUMBER)0);
        str[0] = 0;
        mp_toradix(dhe.x, str, 16);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "key", (INTEGER)VARIABLE_STRING, (char *)str, (NUMBER)0);
        __private_dh_clear_key(&dhe);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ECDHMakeKey, 0)
    ecc_key ecc_dhe;

    CREATE_ARRAY(RESULT);
    if (!ecc_make_key_ex(NULL, find_prng("sprng"), &ecc_dhe, &ecc256)) {
        char str[0xFFF];
        str[0] = 0;
        mp_toradix(ecc_dhe.pubkey.x, str, 16);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "x", (INTEGER)VARIABLE_STRING, (char *)str, (NUMBER)0);
        str[0] = 0;
        mp_toradix(ecc_dhe.pubkey.y, str, 16);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "y", (INTEGER)VARIABLE_STRING, (char *)str, (NUMBER)0);
        str[0] = 0;
        mp_toradix(ecc_dhe.pubkey.z, str, 16);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "z", (INTEGER)VARIABLE_STRING, (char *)str, (NUMBER)0);
        str[0] = 0;
        mp_toradix(ecc_dhe.k, str, 16);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "key", (INTEGER)VARIABLE_STRING, (char *)str, (NUMBER)0);
        ecc_free(&ecc_dhe);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(DHSecret, 2, 3)
    // y
    T_STRING(DHSecret, 0)
    // x
    T_STRING(DHSecret, 1)

    const char *dhe_p = default_dhe_p;

    if (PARAMETERS_COUNT > 2) {
        T_STRING(DHSecret, 2)
        dhe_p = PARAM(2);
    }

    unsigned char out[0xFFF];
    unsigned long out_len = sizeof(out);
    if (!__private_dh_shared_secret(PARAM(1), dhe_p, PARAM(0), out, &out_len)) {
        RETURN_BUFFER((const char *)out, out_len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ECDHSecret, 4)
    // pubkey.x
    T_STRING(ECDHSecret, 0)
    // pubkey.y
    T_STRING(ECDHSecret, 1)
    // pubkey.z
    T_STRING(ECDHSecret, 2)
    // privkey.y
    T_STRING(ECDHSecret, 3)

    unsigned char out[0xFFF];
    unsigned long out_len = sizeof(out);
    if (!__private_eccdh_shared_secret(&ecc256, PARAM(0), PARAM(1), PARAM(2), PARAM(3), out, &out_len)) {
        RETURN_BUFFER((const char *)out, out_len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
#endif

