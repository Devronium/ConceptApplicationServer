#include "StaticString.h"
#include "ConceptTypes.h"
#include "SHManager.h"

POOLED_IMPLEMENTATION(StaticString)

#ifdef USE_DEPRECATED
 #include <string.h>
 #include <stdlib.h>
 #include <stdio.h>
#else
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
#endif

#define breakeven_point    12
#define fast_memcpy(d, s, n)                                                                 \
    { size_t nn = (size_t)(n);                                                               \
      if (nn >= breakeven_point) { memcpy((d), (s), nn); }                                   \
      else if (nn > 0) {                                                                     \
          char *dd; const char *ss;                                                          \
          for (ss = (s), dd = (d); nn > 0; nn--) { *dd++ = *ss++; } } }

#define MEMCPY             fast_memcpy

StaticString::StaticString() {
    DataOffset = 0;
    _LENGTH    = 0;
}

StaticString::StaticString(char *value) {
    DataOffset = 0;
    _LENGTH    = 0;
    operator=(value);
}

StaticString::StaticString(const char *value) {
    DataOffset = 0;
    _LENGTH    = 0;
    operator=((char *)value);
}

StaticString::StaticString(const StaticString& S) {
    DataOffset = 0;
    _LENGTH    = 0;
    if (S._LENGTH) {
        char *Data = 0;
        if (S.DataOffset) {
            Data = ((char *)(void *)&S) + S.DataOffset;
        }
        this->LoadBuffer(Data, S._LENGTH);
    }
}

void StaticString::operator=(const char *value) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    size_t len;
    if (Data) {
        free(Data);
        Data       = 0;
        DataOffset = 0;
    }
    _LENGTH = 0;
    if (value) {
        len     = strlen(value);
        _LENGTH = len;
        if (len) {
            Data       = (char *)realloc(Data, len + 1);
            Data [len] = 0;
            MEMCPY(Data, value, len)

            DataOffset = (intptr_t)Data - (intptr_t) this;
        }
    }
}

void StaticString::operator=(AnsiString& S) {
    this->LoadBuffer(S.c_str(), S.Length());
}

void StaticString::operator=(StaticString& S) {
    size_t len;

    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    if (Data) {
        DataOffset = 0;
        if (Data) {
            free(Data);
        }
    }
    _LENGTH = 0;

    char *other_data = S.c_str();
    len = S.Length();
    if (len) {
        _LENGTH    = len;
        Data       = (char *)malloc(len + 1);
        Data [len] = 0;
        MEMCPY(Data, other_data, len);

        DataOffset = (intptr_t)Data - (intptr_t) this;
    }
}

void StaticString::operator=(intptr_t i) {
    char buffer[16];
#if __SIZEOF_POINTER__ == 8
    sprintf(buffer, "%lli", (long long)i);
#else
    sprintf(buffer, "%i", i);
#endif
    operator=(buffer);
}

int StaticString::operator!=(StaticString& S) {
    if (_LENGTH != S._LENGTH)
        return 1;

    if (!_LENGTH)
        return 0;

    if (memcmp(this->c_str(), S.c_str(), _LENGTH))
        return 1;

    return 0;
}


StaticString::~StaticString(void) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
        free(Data);
        DataOffset = 0;
    }
}

StaticString::operator char *() {
    return c_str();
}

char *StaticString::c_str() {
    char *Data = 0;

    if (!_LENGTH)
        return (char *)"";

    if (DataOffset)
        Data = ((char *)(void *)this) + DataOffset;

    if (!Data)
        return (char *)"";

    return Data;
}

intptr_t StaticString::ToInt() {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    if (Data) {
        return atol(Data);
    }

    return 0;
}

double StaticString::ToFloat() {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    if (Data) {
        return atof(Data);
    }
    return 0;
}

intptr_t StaticString::Length() {
    if (!DataOffset) {
        return 0;
    }

    return _LENGTH;
}

void StaticString::LoadBuffer(const char *buffer, int size) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
        free(Data);
        Data       = 0;
        DataOffset = 0;
    }
    _LENGTH = size;

    if (size) {
        Data = (char *)malloc(size + 1);
        MEMCPY(Data, buffer, size);
        Data [size] = 0;

        DataOffset = (intptr_t)Data - (intptr_t) this;
    }
}

int StaticString::Serialize(FILE *out, int type) {
    unsigned char  uClen = (unsigned char)_LENGTH;
    unsigned short uSlen = (unsigned short)_LENGTH;
    char           *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    int len = _LENGTH; 
    switch (type) {
        case SERIALIZE_8BIT_LENGTH:
            concept_fwrite(&uClen, sizeof(uClen), 1, out);
            len = uClen;
            break;

        case SERIALIZE_16BIT_LENGTH:
            concept_fwrite_int(&uSlen, sizeof(uSlen), 1, out);
            len = uSlen;
            break;

        default:
            concept_fwrite_int(&_LENGTH, sizeof(_LENGTH), 1, out);
            break;
    }
    concept_fwrite_buffer(Data, len, 1, out);
    return 1;
}

int StaticString::ComputeSharedSize(concept_FILE *in, int type) {
    int            len;
    unsigned char  uClen = 0;
    unsigned short uSlen = 0;

    switch (type) {
        case SERIALIZE_8BIT_LENGTH:
            FREAD_FAIL(&uClen, sizeof(uClen), 1, in);
            len = uClen;
            break;

        case SERIALIZE_16BIT_LENGTH:
            FREAD_INT_FAIL(&uSlen, sizeof(uSlen), 1, in);
            len = uSlen;
            break;

        default:
            FREAD_INT_FAIL(&len, sizeof(len), 1, in);
            break;
    }
    SKIP(len, in);
#ifdef ARM_ADJUST_SIZE
    return ARM_ADJUST_SIZE(len + 1);
#else
    return len + 1;
#endif
}

int StaticString::Unserialize(concept_FILE *in, int type, signed char use_pool) {
    int            len;
    unsigned char  uClen = (unsigned char)_LENGTH;
    unsigned short uSlen = (unsigned short)_LENGTH;
    char           *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    switch (type) {
        case SERIALIZE_8BIT_LENGTH:
            if (!concept_fread(&uClen, sizeof(uClen), 1, in)) {
                return -1;
            }
            len = uClen;
            break;

        case SERIALIZE_16BIT_LENGTH:
            if (!concept_fread_int(&uSlen, sizeof(uSlen), 1, in)) {
                return -1;
            }
            len = uSlen;
            break;

        default:
            if (!concept_fread_int(&len, sizeof(len), 1, in)) {
                return -1;
            }
            break;
    }

    free(Data);
    Data = 0;
    if (len) {
        if (use_pool == 1) {
            _LENGTH = len;
            Data    = (char *)SHAlloc(len + 1);
            concept_fread_buffer(Data, len, 1, in);
            Data [len] = 0;
            DataOffset = (intptr_t)Data - (intptr_t) this;
        } else {
            _LENGTH = len;
            Data    = (char *)malloc(len + 1);
            concept_fread_buffer(Data, len, 1, in);
            Data [len] = 0;
            if (use_pool == -1) {
                SHAlloc(len + 1);
            }
            DataOffset = (intptr_t)Data - (intptr_t) this;
        }
    } else {
        DataOffset = 0;
        _LENGTH    = 0;
    }
    return 1;
}

