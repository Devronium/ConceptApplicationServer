#ifndef __INTERFACE_TO_AES_H
#define __INTERFACE_TO_AES_H

#include "AES/AES.h"
#include <string.h>

AES  EncryptAes;
AES  DecryptAes;
char En_inited  = 0;
char Dec_inited = 0;
//char tables_created=0;

#ifndef AES_uint32
 #define AES_uint32    unsigned int
#endif

#ifndef AES_int32
 #define AES_int32    int
#endif

AES_uint32 encrypt_init(char *key, AES_uint32 key_size) {
    //if (!En_inited) {

    /*if (!tables_created) {
            CreateAESTables(true);
            tables_created=1;
       }*/
    EncryptAes.SetParameters(key_size * 8, 128);
    EncryptAes.StartEncryption((unsigned char *)key);
    En_inited = 1;
    return 1;
    //}
    return 0;
}

AES_uint32 decrypt_init(char *key, AES_uint32 key_size) {
    //if (!Dec_inited) {

    /*if (!tables_created) {
            CreateAESTables(true);
            tables_created=1;
       }*/
    DecryptAes.SetParameters(key_size * 8, 128);
    DecryptAes.StartDecryption((unsigned char *)key);
    Dec_inited = 1;
    return 1;
    //}
    return 0;
}

AES *encrypt_init2(char *key, AES_uint32 key_size) {
    AES *EncryptAes2 = new AES();

    EncryptAes2->SetParameters(key_size * 8, 128);
    EncryptAes2->StartEncryption((unsigned char *)key);
    return EncryptAes2;
}

AES *decrypt_init2(char *key, AES_uint32 key_size) {
    AES *Aes2 = new AES();

    Aes2->SetParameters(key_size * 8, 128);
    Aes2->StartDecryption((unsigned char *)key);
    return Aes2;
}

AES_uint32 decrypt(char *in_buffer, AES_int32 in_buffer_size, char *out_buffer, AES_uint32 out_buffer_size, AES::BlockMode mode = AES::CBC) {
    //if (*in_buffer!='x') {
    //	return 0;
    //}
    //in_buffer++;
    //AES_int32 size=*(AES_int32 *)in_buffer;
    //in_buffer+=sizeof(int);

    /*int size=0;
        while (in_buffer_size>=16) {
                DecryptAes.DecryptBlock((const unsigned char *)in_buffer, (unsigned char *)out_buffer);
                in_buffer+=16;
                out_buffer+=16;
                in_buffer_size-=16;
        size+=16;
        }*/
    long num_blocks = in_buffer_size / 16;

    DecryptAes.Decrypt((const unsigned char *)in_buffer, (unsigned char *)out_buffer, num_blocks, mode);
    return num_blocks * 16;
}

AES_uint32 encrypt(char *in_buffer, AES_int32 in_buffer_size, char *out_buffer, AES_uint32 out_buffer_size, AES::BlockMode mode = AES::CBC) {
    //*out_buffer='x';
    //out_buffer++;
    //*(AES_int32 *)out_buffer=in_buffer_size;
    //out_buffer+=sizeof(int);

    char      *in_buffer2;
    char      *original_buffer = 0;
    AES_int32 in_buffer2_size  = in_buffer_size;

    if (in_buffer_size % 16) {
        in_buffer2_size = (in_buffer_size / 16 + 1) * 16;
        in_buffer2      = new char[in_buffer2_size];
        //memset(in_buffer2,0,(in_buffer_size/16+1)*16);
        original_buffer = in_buffer2;
        memcpy(in_buffer2, in_buffer, in_buffer_size);
    } else {
        //in_buffer2=new char[in_buffer_size];
        in_buffer2 = in_buffer;
    }

    /*AES_int32 size=0;
       while (in_buffer2_size>0) {
            EncryptAes.EncryptBlock((const unsigned char *)in_buffer2, (unsigned char *)out_buffer);

            in_buffer2_size-=16;
       size+=16;
            in_buffer2+=16;
            out_buffer+=16;
       }*/
    long num_blocks = in_buffer2_size / 16;
    EncryptAes.Encrypt((const unsigned char *)in_buffer2, (unsigned char *)out_buffer, num_blocks, mode);
    if (original_buffer)
        delete[] original_buffer;
    return num_blocks * 16;
}
#endif //__INTERFACE_TO_AES_H
