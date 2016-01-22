#ifndef __INTERFACE_TO_AES_H
#define __INTERFACE_TO_AES_H

#include "AES/AES.h"
#include <string.h>

AES  EncryptAes;
AES  DecryptAes;
char tables_created = 0;

AES_uint32 encrypt_init(AES *EncryptAes, char *key, AES_uint32 key_size) {
    if (!tables_created) {
        CreateAESTables(true);
        tables_created = 1;
    }
    EncryptAes->SetParameters(key_size * 8, key_size * 8);
    EncryptAes->StartEncryption((unsigned char *)key);
    return 1;
}

AES_uint32 decrypt_init(AES *DecryptAes, char *key, AES_uint32 key_size) {
    if (!tables_created) {
        CreateAESTables(true);
        tables_created = 1;
    }
    DecryptAes->SetParameters(key_size * 8, key_size * 8);
    DecryptAes->StartDecryption((unsigned char *)key);
    return 1;
}

AES_uint32 decrypt(AES *DecryptAes, char *in_buffer, AES_int32 in_buffer_size, char *out_buffer, AES_uint32 out_buffer_size) {
    if (*in_buffer != 'x') {
        return 0;
    }
    in_buffer++;
    AES_int32 size = *(AES_int32 *)in_buffer;
    in_buffer      += sizeof(AES_int32);
    in_buffer_size -= 5;
    while (in_buffer_size >= 16) {
        DecryptAes->Decrypt((AES_uint32 *)in_buffer, (AES_uint32 *)out_buffer);
        in_buffer      += 16;
        out_buffer     += 16;
        in_buffer_size -= 16;
    }
    return size;
}

AES_uint32 encrypt(AES *EncryptAes, char *in_buffer, AES_int32 in_buffer_size, char *out_buffer, AES_uint32 out_buffer_size) {
    *out_buffer = 'x';
    out_buffer++;
    *(AES_int32 *)out_buffer = in_buffer_size;
    out_buffer += sizeof(AES_int32);

    char last_buf[16];
    long in_buffer2_size = in_buffer_size;
    long padding         = 0;
    if (in_buffer_size % 16) {
        in_buffer2_size = (in_buffer_size / 16 + 1) * 16;
        padding         = 16 - (in_buffer2_size - in_buffer_size);
    }

    int size = 5;

    while (in_buffer2_size > 0) {
        if ((in_buffer2_size == 16) && (padding)) {
            memcpy(last_buf, in_buffer, padding);
            EncryptAes->Encrypt((AES_uint32 *)last_buf, (AES_uint32 *)out_buffer);
        } else
            EncryptAes->Encrypt((AES_uint32 *)in_buffer, (AES_uint32 *)out_buffer);

        in_buffer2_size -= 16;
        size            += 16;
        in_buffer       += 16;
        out_buffer      += 16;
    }
    return size;
}
#endif //__INTERFACE_TO_AES_H
