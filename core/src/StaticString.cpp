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
}

StaticString::StaticString(char *value) {
    DataOffset = 0;
    operator=(value);
}

StaticString::StaticString(const char *value) {
    DataOffset = 0;
    operator=((char *)value);
}

StaticString::StaticString(const StaticString& S) {
    DataOffset = 0;
    if (S.Length())
        this->LoadBuffer(S.c_str(), S.Length());
}

void StaticString::operator=(const char *value) {
    char *Data;
    size_t len;
    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
        free(Data);
        DataOffset = 0;
    }
    if (value) {
        len     = strlen(value);
        if (len) {
            Data = (char *)malloc(sizeof(LENGTH_DATA_TYPE) + len + 1);
            *(LENGTH_DATA_TYPE *)Data = len;
            Data += sizeof(LENGTH_DATA_TYPE);
            MEMCPY(Data, value, len)
            Data [len] = 0;
            DataOffset = (intptr_t)Data - (intptr_t) this - sizeof(LENGTH_DATA_TYPE);
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
        free(Data);
        DataOffset = 0;
    }

    const char *other_data = S.c_str();
    len = S.Length();
    if (len) {
        Data = (char *)malloc(sizeof(LENGTH_DATA_TYPE) + len + 1);
        *(LENGTH_DATA_TYPE *)Data = len;
        Data += sizeof(LENGTH_DATA_TYPE);
        MEMCPY(Data, other_data, len);
        Data [len] = 0;

        DataOffset = (intptr_t)Data - (intptr_t) this - sizeof(LENGTH_DATA_TYPE);
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
    if (Length() != S.Length())
        return 1;

    if (!Length())
        return 0;

    if (memcmp(this->c_str(), S.c_str(), Length()))
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

StaticString::operator const char *() {
    return c_str();
}

const char *StaticString::c_str() const {
    if (DataOffset)
        return ((char *)(void *)this) + DataOffset + sizeof(LENGTH_DATA_TYPE);

    return "";
}

intptr_t StaticString::ToInt() {
    if (DataOffset)
        return atol(((char *)(void *)this) + DataOffset + sizeof(LENGTH_DATA_TYPE));
    return 0;
}

double StaticString::ToFloat() {
    char *Data = 0;

    if (DataOffset)
        return atof(((char *)(void *)this) + DataOffset + sizeof(LENGTH_DATA_TYPE));
    return 0;
}

LENGTH_DATA_TYPE StaticString::Length() const {
    if (!DataOffset)
        return 0;

    char *Data = ((char *)(void *)this) + DataOffset;
    return *((LENGTH_DATA_TYPE *)Data);
}

LENGTH_DATA_TYPE StaticString::Size() const {
    if (!DataOffset)
        return 0;

    char *Data = ((char *)(void *)this) + DataOffset;
    return *((LENGTH_DATA_TYPE *)Data) + sizeof(LENGTH_DATA_TYPE);
}

void StaticString::LoadBuffer(const char *buffer, int size) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
        free(Data);
        DataOffset = 0;
    }
    if (size) {
        Data = (char *)malloc(sizeof(LENGTH_DATA_TYPE) + size + 1);
        *(LENGTH_DATA_TYPE *)Data = size;
        Data += sizeof(LENGTH_DATA_TYPE);
        MEMCPY(Data, buffer, size);
        Data [size] = 0;
        DataOffset = (intptr_t)Data - (intptr_t) this - sizeof(LENGTH_DATA_TYPE);
    }
}

int StaticString::Serialize(FILE *out, int type) {
    LENGTH_DATA_TYPE _LENGTH = 0;
    char           *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset + sizeof(LENGTH_DATA_TYPE);
        _LENGTH = this->Length();
    }
    unsigned char  uClen = (unsigned char)_LENGTH;
    unsigned short uSlen = (unsigned short)_LENGTH;

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
    if (len) {
        len += sizeof(LENGTH_DATA_TYPE);
    } else
        return 0;
#ifdef ARM_ADJUST_SIZE
    return ARM_ADJUST_SIZE(len + 1);
#else
    return len + 1;
#endif
}

int StaticString::Unserialize(concept_FILE *in, int type, signed char use_pool) {
    int            len;
    unsigned char  uClen;
    unsigned short uSlen;
    char           *Data = 0;

    if (DataOffset)
        Data = ((char *)(void *)this) + DataOffset;

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
            Data    = (char *)SHAlloc(sizeof(LENGTH_DATA_TYPE) + len + 1);
            *(LENGTH_DATA_TYPE *)Data = len;
            Data += sizeof(LENGTH_DATA_TYPE);
            concept_fread_buffer(Data, len, 1, in);
            Data [len] = 0;
            DataOffset = (intptr_t)Data - (intptr_t) this - sizeof(LENGTH_DATA_TYPE);
        } else {
            Data    = (char *)malloc(sizeof(LENGTH_DATA_TYPE) + len + 1);
            *(LENGTH_DATA_TYPE *)Data = len;
            Data += sizeof(LENGTH_DATA_TYPE);
            concept_fread_buffer(Data, len, 1, in);
            Data [len] = 0;
            if (use_pool == -1)
                SHAlloc(sizeof(LENGTH_DATA_TYPE) + len + 1);

            DataOffset = (intptr_t)Data - (intptr_t) this - sizeof(LENGTH_DATA_TYPE);
        }
    } else {
        DataOffset = 0;
    }
    return 1;
}

