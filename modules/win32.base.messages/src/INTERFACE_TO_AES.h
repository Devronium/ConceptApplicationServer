#ifndef __INTERFACE_TO_AES_H
#define __INTERFACE_TO_AES_H

#include "AES/AES.h"
#include <string.h>

AES_uint32 encrypt_init(AES *EncryptAes, char *key, AES_uint32 key_size) {
    EncryptAes->SetParameters(key_size * 8, key_size * 8);
    EncryptAes->StartEncryption((unsigned char *)key);
    return 1;
}

AES_uint32 decrypt_init(AES *DecryptAes, char *key, AES_uint32 key_size) {
    DecryptAes->SetParameters(key_size * 8, key_size * 8);
    DecryptAes->StartDecryption((unsigned char *)key);
    return 1;
}

AES_uint32 decrypt(AES *DecryptAes, char *in_buffer, AES_int32 in_buffer_size, char *out_buffer, AES_uint32 out_buffer_size, AES::BlockMode mode = AES::ECB) {
    if (*in_buffer != 'x')
        return 0;

    in_buffer++;
    AES_int32 size = ntohl(*(AES_int32 *)in_buffer);
    in_buffer      += sizeof(AES_int32);
    in_buffer_size -= 5;
    //while (in_buffer_size >= 16) {
    DecryptAes->Decrypt((unsigned char *)in_buffer, (unsigned char *)out_buffer, in_buffer_size / DecryptAes->bsize, mode);
    //    in_buffer      += 16;
    //    out_buffer     += 16;
    //    in_buffer_size -= 16;
    //}
    return size;
}

AES_uint32 encrypt(AES *EncryptAes, char *in_buffer, AES_int32 in_buffer_size, char *out_buffer, AES_uint32 out_buffer_size, AES::BlockMode mode = AES::ECB) {
    *out_buffer = 'x';
    out_buffer++;
    *(AES_int32 *)out_buffer = htonl(in_buffer_size);
    out_buffer += sizeof(AES_int32);

    // up to 256 bits
    char last_buf[32];
    long in_buffer2_size = in_buffer_size;
    long padding         = 0;
    if (in_buffer_size % EncryptAes->bsize) {
        in_buffer2_size = (in_buffer_size / EncryptAes->bsize + 1) * EncryptAes->bsize;
        padding         = EncryptAes->bsize - (in_buffer2_size - in_buffer_size);
    }

    int size = 5;

    while (in_buffer2_size >= EncryptAes->bsize) {
        if ((in_buffer2_size == EncryptAes->bsize) && (padding)) {
            memcpy(last_buf, in_buffer, padding);
            EncryptAes->Encrypt((unsigned char *)last_buf, (unsigned char *)out_buffer, 1, mode);
        } else
            EncryptAes->Encrypt((unsigned char *)in_buffer, (unsigned char *)out_buffer, 1, mode);

        in_buffer2_size -= EncryptAes->bsize;
        size            += EncryptAes->bsize;
        in_buffer       += EncryptAes->bsize;
        out_buffer      += EncryptAes->bsize;
    }
    return size;
}
#endif //__INTERFACE_TO_AES_H
