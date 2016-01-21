#include "rsa.h"

// use constant because memcpy() will give warning if use #define
const unsigned long crlf_length = 2;
const unsigned char crlf[]      = { 0x0D, 0x0A };

int load_public_certificate_structure_from_buffer(pub_key *pubkey, char *buffer, int bufferLen) {
    unsigned short *bnsize;
    char           *pBuffer = buffer;

    int min_size = sizeof(pubkey->modulus_length) + 2 * bn_header_size;

    if (bufferLen < min_size)
        return(0);

    // load modulus size
    memcpy(&pubkey->modulus_length, pBuffer, sizeof(pubkey->modulus_length));
    pBuffer += sizeof(pubkey->modulus_length);

    bnsize   = (unsigned short *)pBuffer;
    pBuffer += bn_header_size;
    if (BN_bin2bn((const unsigned char *)pBuffer, *bnsize, pubkey->e) == NULL)
        return(0);
    pBuffer += *bnsize;

    bnsize   = (unsigned short *)pBuffer;
    pBuffer += bn_header_size;
    if (BN_bin2bn((const unsigned char *)pBuffer, *bnsize, pubkey->n) == NULL)
        return(0);
    pBuffer += *bnsize;

    if (pBuffer - buffer != bufferLen)
        return(0);

    return(1);
}

int load_private_key_structure_from_buffer(pri_key *prikey, char *buffer, int bufferLen) {
    unsigned short *bnsize;
    char           *pBuffer = buffer;

    int min_size = sizeof(prikey->modulus_length) + 5 * bn_header_size;

    if (bufferLen < min_size)
        return(0);

    // load modulus size
    memcpy(&prikey->modulus_length, pBuffer, sizeof(prikey->modulus_length));
    pBuffer += sizeof(prikey->modulus_length);

    // dp
    bnsize   = (unsigned short *)pBuffer;
    pBuffer += bn_header_size;
    if (BN_bin2bn((const unsigned char *)pBuffer, *bnsize, prikey->dp) == NULL)
        return(0);
    pBuffer += *bnsize;

    // dq
    bnsize   = (unsigned short *)pBuffer;
    pBuffer += bn_header_size;
    if (BN_bin2bn((const unsigned char *)pBuffer, *bnsize, prikey->dq) == NULL)
        return(0);
    pBuffer += *bnsize;

    // e
    bnsize   = (unsigned short *)pBuffer;
    pBuffer += bn_header_size;
    if (BN_bin2bn((const unsigned char *)pBuffer, *bnsize, prikey->e) == NULL)
        return(0);
    pBuffer += *bnsize;

    // p
    bnsize   = (unsigned short *)pBuffer;
    pBuffer += bn_header_size;
    if (BN_bin2bn((const unsigned char *)pBuffer, *bnsize, prikey->p) == NULL)
        return(0);
    pBuffer += *bnsize;

    // q
    bnsize   = (unsigned short *)pBuffer;
    pBuffer += bn_header_size;
    if (BN_bin2bn((const unsigned char *)pBuffer, *bnsize, prikey->q) == NULL)
        return(0);
    pBuffer += *bnsize;

    // qinv
    bnsize   = (unsigned short *)pBuffer;
    pBuffer += bn_header_size;
    if (BN_bin2bn((const unsigned char *)pBuffer, *bnsize, prikey->qinv) == NULL)
        return(0);
    pBuffer += *bnsize;

    if (pBuffer - buffer != bufferLen)
        return(0);

    return(1);
}

// bufferlen is the length of buffer .. to check whether the buffer is enough or not ...
// return the total length of buffer filled
// if return ZERO it means buffer is not enough
int save_public_certificate_structure_to_buffer(pub_key *pubkey, char *buffer, int bufferLen) {
    int            total;
    char           temp[2048]; // <==== change it necessary
    char           *pTemp = temp;
    unsigned short *result;

    total = 0;

    // store the modulus size first
    memcpy(pTemp, &pubkey->modulus_length, sizeof(pubkey->modulus_length));
    pTemp += sizeof(pubkey->modulus_length);

    //save e .. bn_header_size is required for BN_bin2bn()
    result  = (unsigned short *)pTemp;
    *result = (unsigned short)BN_bn2bin(pubkey->e, (unsigned char *)pTemp + bn_header_size);
    if (*result == 0)
        return(0);
    pTemp += (*result + bn_header_size);

    //save n .. bn_header_size is required for BN_bin2bn()
    result  = (unsigned short *)pTemp;
    *result = (unsigned short)BN_bn2bin(pubkey->n, (unsigned char *)pTemp + bn_header_size);
    if (*result == 0)
        return(0);
    pTemp += (*result + bn_header_size);

    total = pTemp - temp;
    if (total > bufferLen)
        return(0);

    memcpy(buffer, temp, total);

    return(total);
}

// bufferlen is the length of buffer .. to check whether the buffer is enough or not ...
// return the total length of buffer filled
// if return ZERO it means buffer is not enough
int save_private_key_structure_to_buffer(pri_key *prikey, char *buffer, int bufferLen) {
    int            total;
    char           temp[2048]; // <==== change it necessary
    char           *pTemp = temp;
    unsigned short *result;

    total = 0;

    // store the modulus size first
    memcpy(pTemp, &prikey->modulus_length, sizeof(prikey->modulus_length));
    pTemp += sizeof(prikey->modulus_length);

    //save dp .. bn_header_size is required for BN_bin2bn()
    result  = (unsigned short *)pTemp;
    *result = (unsigned short)BN_bn2bin(prikey->dp, (unsigned char *)pTemp + bn_header_size);
    if (*result == 0)
        return(0);
    pTemp += (*result + bn_header_size);

    // dq
    result  = (unsigned short *)pTemp;
    *result = (unsigned short)BN_bn2bin(prikey->dq, (unsigned char *)pTemp + bn_header_size);
    if (*result == 0)
        return(0);
    pTemp += (*result + bn_header_size);

    // e
    result  = (unsigned short *)pTemp;
    *result = (unsigned short)BN_bn2bin(prikey->e, (unsigned char *)pTemp + bn_header_size);
    if (*result == 0)
        return(0);
    pTemp += (*result + bn_header_size);

    // p
    result  = (unsigned short *)pTemp;
    *result = (unsigned short)BN_bn2bin(prikey->p, (unsigned char *)pTemp + bn_header_size);
    if (*result == 0)
        return(0);
    pTemp += (*result + bn_header_size);

    // q
    result  = (unsigned short *)pTemp;
    *result = (unsigned short)BN_bn2bin(prikey->q, (unsigned char *)pTemp + bn_header_size);
    if (*result == 0)
        return(0);
    pTemp += (*result + bn_header_size);

    // qinv
    result  = (unsigned short *)pTemp;
    *result = (unsigned short)BN_bn2bin(prikey->qinv, (unsigned char *)pTemp + bn_header_size);
    if (*result == 0)
        return(0);
    pTemp += (*result + bn_header_size);

    total = pTemp - temp;
    if (total > bufferLen)
        return(0);

    memcpy(buffer, temp, total);

    return(total);
}

// return the real size of target required
// return ZERO if failed
int RSA_save_public_certificate_to_buffer(pub_key *pubkey, char *target, int targetLen) {
    char *pTarget;
    char temp[buffer_size];
    int  result;

    // 1. save structure to buffer
    result = save_public_certificate_structure_to_buffer(pubkey, target, targetLen);
    if (result == 0)
        return(0);

    // 2. encode the buffer to PEM
    result = PEM_encode(target, temp, result, buffer_size);
    if (result == 0)
        return(0);

    // 3. input RSA public certificate header string
    pTarget = target;
    memcpy(pTarget, certificate_top, certificate_top_length);
    pTarget += certificate_top_length;
    memcpy(pTarget, crlf, crlf_length);
    pTarget += crlf_length;
    memcpy(pTarget, temp, result);
    pTarget += result;
    memcpy(pTarget, crlf, crlf_length);
    pTarget += crlf_length;
    memcpy(pTarget, certificate_bottom, certificate_bottom_length);
    pTarget += certificate_bottom_length;

    return(pTarget - target);
}

// return ZERO if error else success
int RSA_load_public_certificate_from_buffer(pub_key *pubkey, char *source, int sourceLen) {
    char temp[buffer_size];
    char temp2[buffer_size];
    int  result;

    //1. take out the header and the bottom certificate string
    result = sourceLen - (certificate_top_length + certificate_bottom_length + crlf_length + crlf_length);

    if ((result <= 0) || (result > buffer_size))
        return(0);
    if (certificate_top_length + crlf_length > sourceLen)
        return(0);

    memcpy(temp, source + certificate_top_length + crlf_length, result);


    //2. decode PEM
    result = PEM_decode(temp, temp2, result, buffer_size);
    if (result == 0)
        return(0);

    //3. convert buffer to structure
    result = load_public_certificate_structure_from_buffer(pubkey, temp2, result);
    if (result == 0)
        return(0);

    return(1);
}

int RSA_save_private_key_to_buffer(pri_key *prikey, char *target, int targetLen) {
    char *pTarget;
    char temp[buffer_size];
    int  result;

    // 1. save structure to buffer
    result = save_private_key_structure_to_buffer(prikey, target, targetLen);
    if (result == 0)
        return(0);

    // 2. encode the buffer to PEM
    result = PEM_encode(target, temp, result, buffer_size);
    if (result == 0)
        return(0);

    // 3. input RSA public certificate header string
    pTarget = target;

    // copy the top string + crlf
    memcpy(pTarget, key_top, key_top_length);
    pTarget += key_top_length;
    memcpy(pTarget, crlf, crlf_length);
    pTarget += crlf_length;

    // copy the real body content
    memcpy(pTarget, temp, result);
    pTarget += result;

    // copy crlf and bottom certificate string
    memcpy(pTarget, crlf, crlf_length);
    pTarget += crlf_length;
    memcpy(pTarget, key_bottom, key_bottom_length);
    pTarget += key_bottom_length;

    return(pTarget - target);
}

int RSA_load_private_key_from_buffer(pri_key *prikey, char *source, int sourceLen) {
    char temp[buffer_size];
    char temp2[buffer_size];
    int  result;

    //1. take out the header and the bottom certificate string
    result = sourceLen - (key_top_length + key_bottom_length + crlf_length + crlf_length);

    if ((result <= 0) || (result > buffer_size))
        return(0);
    if (key_top_length + crlf_length > sourceLen)
        return(0);

    memcpy(temp, source + key_top_length + crlf_length, result);

    //2. decode PEM
    result = PEM_decode(temp, temp2, result, buffer_size);
    if (result == 0)
        return(0);

    //3. convert buffer to structure
    result = load_private_key_structure_from_buffer(prikey, temp2, result);
    if (result == 0)
        return(0);

    return(1);
}
