#include "TinyString.h"
#include "SHManager.h"

#ifdef USE_DEPRECATED
 #include <string.h>
 #include <stdlib.h>
 #include <stdio.h>
#else
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
#endif

POOLED_IMPLEMENTATION(TinyString)

#define breakeven_point    12

#define fast_memcpy(d, s, n)                                                                 \
    { register size_t nn = (size_t)(n);                                                      \
      if (nn >= breakeven_point) { memcpy((d), (s), nn); }                                   \
      else if (nn > 0) {              \
          register char *dd; register const char *ss;                                        \
          for (ss = (s), dd = (d); nn > 0; nn--) { *dd++ = *ss++; } } }

#define MEMCPY    fast_memcpy

TinyString::TinyString() {
    DataOffset = 0;
}

TinyString::TinyString(char *value) {
    DataOffset = 0;
    operator=(value);
}

TinyString::TinyString(const char *value) {
    DataOffset = 0;
    operator=((char *)value);
}

TinyString::TinyString(long i) {
    DataOffset = 0;
    operator=(i);
}

TinyString::TinyString(int i) {
    DataOffset = 0;
    operator=((long)i);
}

TinyString::TinyString(char c) {
    DataOffset = 0;
    operator=(c);
}

TinyString::TinyString(double d) {
    DataOffset = 0;
    operator=(d);
}

TinyString::TinyString(const TinyString& S) {
    DataOffset = 0;
    if (S.DataOffset) {
        operator=(((char *)&S) + (S.DataOffset));
    } else {
        operator=((char *)0);
    }
    //}
}

void TinyString::operator=(char *value) {
    size_t len   = 0;
    char   *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + (DataOffset);
    }
    if (Data) {
        free(Data);
    }
    DataOffset = 0;
    Data       = 0;
    if (value) {
        len  = strlen(value);
        Data = (char *)malloc(len + 1);
        MEMCPY(Data, value, len);
        Data [len] = 0;
        DataOffset = (intptr_t)Data - (intptr_t) this;
    }
}

void TinyString::operator=(TinyString& S) {
    operator=(S.c_str());
}

void TinyString::operator=(AnsiString& S) {
    operator=(S.c_str());
}

void TinyString::operator=(char c) {
    char buffer [2];

    buffer [0] = c;
    buffer [1] = 0;
    operator=(buffer);
}

void TinyString::operator=(long i) {
    char buffer [MAX_DECIMALS];

    sprintf(buffer, "%li", i);
    operator=(buffer);
}

void TinyString::operator=(double d) {
    char buffer [MAX_DECIMALS];

    sprintf(buffer, "%.30g", d);
    int len = strlen(buffer);
    if (((len > 1) && ((buffer [len - 1] == '.') || (buffer [len - 1] == ',')))) {
        buffer [len - 1] = 0;
    }
    operator=(buffer);
}

void TinyString::operator=(int i) {
    operator=((long)i);
}

int TinyString::operator <(char *str) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }
    if ((Data) && (str)) {
        return strcmp(Data, str) < 0;
    }
    return 0;
}

int TinyString::operator >(char *str) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }
    if ((Data) && (str)) {
        return strcmp(Data, str) > 0;
    }
    return 0;
}

int TinyString::operator >=(char *str) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }
    if ((Data) && (str)) {
        return strcmp(Data, str) >= 0;
    }
    return 0;
}

int TinyString::operator <=(char *str) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    if ((Data) && (str)) {
        return strcmp(Data, str) <= 0;
    }
    return 0;
}

int TinyString::operator==(char *str) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }
    if ((Data) && (str)) {
        if (strcmp(Data, str)) {
            return 0;
        } else {
            return 1;
        }
    }
    if ((!Data) && (str) && (!str [0])) {
        return 1;
    }

    if ((!str) && (Data) && (!Data [0])) {
        return 1;
    }

    if ((!Data) && (!str)) {
        return 1;
    }
    return 0;
}

int TinyString::operator !=(char *str) {
    return !operator==(str);
}

int TinyString::operator==(TinyString& S) {
    return operator==(S.c_str());
}

int TinyString::operator==(AnsiString& S) {
    return operator==(S.c_str());
}

int TinyString::operator!=(TinyString& S) {
    return operator!=(S.c_str());
}

int TinyString::operator!=(AnsiString& S) {
    return operator!=(S.c_str());
}

int TinyString::operator >(TinyString& S) {
    return operator >(S.c_str());
}

int TinyString::operator <(TinyString& S) {
    return operator <(S.c_str());
}

int TinyString::operator >=(TinyString& S) {
    return operator >=(S.c_str());
}

int TinyString::operator <=(TinyString& S) {
    return operator <=(S.c_str());
}

TinyString::~TinyString(void) {

    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }
    if (Data) {
        free(Data);
    }
    DataOffset = 0;
}

TinyString::operator char *() {
    return c_str();
}

TinyString::operator AnsiString() {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }
    return AnsiString(Data);
}

char *TinyString::c_str() {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    if (!Data) {
        return (char *)"";
    }
    return Data;
}

void TinyString::operator +=(char *value) {
    size_t len;
    size_t len_value;

    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    if ((Data) && (value)) {
        len       = strlen(Data);
        len_value = strlen(value);
        size_t new_length = len + len_value;

        Data = (char *)realloc(Data, new_length + 1);
        MEMCPY(Data + len, value, len_value);

        Data [new_length] = 0;

        DataOffset = (intptr_t)Data - (intptr_t) this;
    } else {
        operator=(value);
    }
}

void TinyString::operator +=(TinyString& S) {
    operator+=(S.c_str());
}

void TinyString::operator +=(char c) {
    char buffer [2];

    buffer [0] = c;
    buffer [1] = 0;
    operator+=(buffer);
}

char TinyString::operator [](uintptr_t index) {
    size_t len;
    char   *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }
    if (Data) {
        len = strlen(Data);
        if (index < len) {
            return Data [index];
        }
    }
    return 0;
}

char TinyString::operator [](int index) {
    size_t len;
    char   *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }
    if (Data) {
        len = strlen(Data);
        if ((index < len) && (index >= 0)) {
            return Data [index];
        }
    }
    return 0;
}

intptr_t TinyString::ToInt() {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    if (Data) {
        return atol(Data);
    }

    return 0;
}

double TinyString::ToFloat() {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }
    if (Data) {
        return atof(Data);
    }
    return 0;
}

TinyString TinyString::operator +(TinyString& S) {
    TinyString ret     = *this;
    char       *S_Data = 0;

    if (S.DataOffset) {
        S_Data = ((char *)(void *)&S) + S.DataOffset;
    }
    if (S_Data) {
        ret += S;
    }
    return ret;
}

intptr_t TinyString::Length() {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    if (!Data) {
        return 0;
    }
    return strlen(Data);
}

intptr_t TinyString::Pos(TinyString substr) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    intptr_t result = -1;
    char     *ptr   = strstr(Data, substr.c_str());
    if (ptr) {
        result = ptr - Data + 1;
    }
    return result;
}

void TinyString::LoadBuffer(char *buffer, int size) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
        free(Data);
    }

    if (size > 0) {
        Data = (char *)malloc(size + 1);
        MEMCPY(Data, buffer, size);
        Data [size] = 0;

        DataOffset = (intptr_t)Data - (intptr_t) this;
    } else
        DataOffset = 0;
}

int TinyString::Serialize(FILE *out, int type) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    size_t         _LENGTH = Length();
    unsigned char  uClen   = (unsigned char)_LENGTH;
    unsigned short uSlen   = (unsigned short)_LENGTH;
    switch (type) {
        case SERIALIZE_8BIT_LENGTH:
            concept_fwrite(&uClen, sizeof(uClen), 1, out);
            break;

        case SERIALIZE_16BIT_LENGTH:
            concept_fwrite_int(&uSlen, sizeof(uSlen), 1, out);
            break;

        default:
            concept_fwrite_int(&_LENGTH, sizeof(_LENGTH), 1, out);
            break;
    }
    concept_fwrite_buffer(Data, _LENGTH, 1, out);
    return 1;
}

int TinyString::Unserialize(concept_FILE *in, int type, signed char use_pool) {
    char *Data = 0;

    if (DataOffset) {
        Data = ((char *)(void *)this) + DataOffset;
    }

    intptr_t       len;
    unsigned char  uClen = (unsigned char)0;
    unsigned short uSlen = (unsigned short)0;

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

    if ((len) && (use_pool == -1)) {
        concept_fseek(in, len, SEEK_CUR);
        SHAlloc(len + 1);
        // just move the stack pointer and return (already in shared memory)
        return 1;
    }

    if (Data) {
        free(Data);
    }
    Data       = 0;
    DataOffset = 0;

    if (len) {
        if (use_pool == 1) {
            Data = (char *)SHAlloc(len + 1);
            concept_fread_buffer(Data, len, 1, in);
            Data [len] = 0;
        } else {
            Data = (char *)malloc(len + 1);
            concept_fread_buffer(Data, len, 1, in);
            Data [len] = 0;
        }
        DataOffset = (intptr_t)Data - (intptr_t) this;
    }
    return len + 1;
}

