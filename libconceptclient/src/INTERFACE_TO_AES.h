#ifndef __INTERFACE_TO_AES_H
#define __INTERFACE_TO_AES_H

#include "AES/aes.h"
#include <string.h>

#define AES_uint32 unsigned int
#define AES_int32 int

AES_uint32 encrypt_init(CConceptClient *OWNER, char *key, AES_uint32 key_size) {
    if (!OWNER->LinkContainer.En_inited) {
        OWNER->LinkContainer.EncryptAes.SetParameters(key_size * 8, key_size * 8);
        OWNER->LinkContainer.EncryptAes.StartEncryption((unsigned char *)key);
        OWNER->LinkContainer.En_inited = 1;
        return 1;
    }
    return 0;
}

AES_uint32 decrypt_init(CConceptClient *OWNER, char *key, AES_uint32 key_size) {
    if (!OWNER->LinkContainer.Dec_inited) {
        OWNER->LinkContainer.DecryptAes.SetParameters(key_size * 8, key_size * 8);
        OWNER->LinkContainer.DecryptAes.StartDecryption((unsigned char *)key);
        OWNER->LinkContainer.Dec_inited = 1;
        return 1;
    }
    return 0;
}

AES_uint32 decrypt(CConceptClient *OWNER, char *in_buffer, AES_int32 in_buffer_size, char *out_buffer, AES_uint32 out_buffer_size, char ignore_size = 0, bool cbc = false) {
    int size = 0;

    if (ignore_size)
        size = in_buffer_size;
    else {
        if (*in_buffer != 'x')
            return 0;
        in_buffer++;
        size            = ntohl(*(AES_int32 *)in_buffer);
        in_buffer      += sizeof(AES_int32);
        in_buffer_size -= 5;
    }
    OWNER->LinkContainer.DecryptAes.Decrypt((unsigned char *)in_buffer, (unsigned char *)out_buffer, in_buffer_size / OWNER->LinkContainer.DecryptAes.bsize, cbc ? AES::CBC : AES::ECB);
    return size;
}

AES_uint32 encrypt(CConceptClient *OWNER, char *in_buffer, AES_int32 in_buffer_size, char *out_buffer, AES_uint32 out_buffer_size, bool cbc = false) {
    *out_buffer = 'x';
    out_buffer++;
    *(AES_int32 *)out_buffer = htonl(in_buffer_size);
    out_buffer += sizeof(AES_int32);
    char      last_buf[64];
    AES_int32 in_buffer2_size = in_buffer_size;
    AES_int32 padding         = 0;
    if (in_buffer_size % OWNER->LinkContainer.EncryptAes.bsize) {
        in_buffer2_size = (in_buffer_size / OWNER->LinkContainer.EncryptAes.bsize + 1) * OWNER->LinkContainer.EncryptAes.bsize;
        padding         = OWNER->LinkContainer.EncryptAes.bsize - (in_buffer2_size - in_buffer_size);
    }

    int size = 5;
    while (in_buffer2_size > 0) {
        if ((in_buffer2_size == OWNER->LinkContainer.EncryptAes.bsize) && (padding)) {
            memcpy(last_buf, in_buffer, padding);
            OWNER->LinkContainer.EncryptAes.Encrypt((unsigned char *)last_buf, (unsigned char *)out_buffer, 1, cbc ? AES::CBC : AES::ECB);
        } else
            OWNER->LinkContainer.EncryptAes.Encrypt((unsigned char *)in_buffer, (unsigned char *)out_buffer, 1, cbc ? AES::CBC : AES::ECB);

        in_buffer2_size -= OWNER->LinkContainer.EncryptAes.bsize;
        size            += OWNER->LinkContainer.EncryptAes.bsize;
        in_buffer       += OWNER->LinkContainer.EncryptAes.bsize;
        out_buffer      += OWNER->LinkContainer.EncryptAes.bsize;
    }
    return size;
}
#endif //__INTERFACE_TO_AES_H
