#include "AnsiString.h"

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

static const char *ansi_string_empty = "";

#define fast_memcpy(d, s, n)                                                     \
    { register size_t nn = (size_t)(n);                                          \
      if (nn >= breakeven_point) memcpy((d), (s), nn);                           \
      else if (nn > 0) { /* proc call overhead is worth only for large strings*/ \
          register char *dd; register const char *ss;                            \
          for (ss = (s), dd = (d); nn > 0; nn--) *dd++ = *ss++; } }

#define MEMCPY    fast_memcpy

void memcpy32(void *dest, void *src, int bcount) {
    if (bcount < 4) {
        memcpy(dest, src, bcount);
        return;
    }
    int blocks = bcount / 4;
    bcount %= 4;

    long *ldest = (long *)dest;
    long *lsrc  = (long *)src;

    void *end = lsrc + blocks;

label1:
    *ldest++ = *lsrc++;
    if (lsrc != end)
        goto label1;

    if (bcount)
        memcpy(ldest, lsrc, bcount);
}

AnsiString::AnsiString() {
    Data       = NULL;
    _DATA_SIZE = 0;
    _LENGTH    = 0;
}

AnsiString::AnsiString(const char *value) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    operator=(value);
}

AnsiString::AnsiString(long i) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    operator=(i);
}

AnsiString::AnsiString(int i) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    operator=((long)i);
}

AnsiString::AnsiString(char c) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    operator=(c);
}

AnsiString::AnsiString(double d) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    operator=(d);
}

AnsiString::AnsiString(const AnsiString& S) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    if (S._LENGTH)
        this->LoadBuffer(S.Data, S._LENGTH);
}

void AnsiString::operator=(const char *value) {
    ssize_t len;

    if (Data)
        Data[0] = 0;
    _LENGTH = 0;
    if (value) {
        len     = strlen(value);
        _LENGTH = len;
        if (len) {
            if (len + 1 >= _DATA_SIZE) {
                _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
                Data = (char *)realloc(Data, _DATA_SIZE);
            }
            MEMCPY(Data, value, len + 1); // include null-terminator
        }
    }
}

void AnsiString::operator=(AnsiString S) {
    ssize_t len;

    if (Data)
        Data[0] = 0;
    _LENGTH = 0;

    const char *other_data = S.c_str();
    len = S.Length();
    if (len) {
        _LENGTH = len;
        if (len + 1 >= _DATA_SIZE) {
            free(Data);
            _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            Data = (char *)malloc(_DATA_SIZE);
        }
        MEMCPY(Data, other_data, len + 1);
    }
}

void AnsiString::operator=(char c) {
    char buffer[2];

    buffer[0] = c;
    buffer[1] = 0;
    operator=(buffer);
}

void AnsiString::operator=(long i) {
    char buffer[MAX_DECIMALS];

    sprintf(buffer, "%li", i);
    operator=(buffer);
}

void AnsiString::operator=(double d) {
    char buffer[MAX_DECIMALS];

    sprintf(buffer, "%.30g", d);
    int len = strlen(buffer);
    if (((len > 1) && ((buffer[len - 1] == '.') || (buffer[len - 1] == ','))))
        buffer[len - 1] = 0;
    operator=(buffer);
}

void AnsiString::operator=(int i) {
    operator=((long)i);
}

int AnsiString::operator <(const char *str) {
    if ((Data) && (str)) {
        if (strcmp(Data, str) == -1)
            return 1;
        else
            return 0;
    }
    return 0;
}

int AnsiString::operator >(const char *str) {
    if ((Data) && (str)) {
        if (strcmp(Data, str) == 1)
            return 1;
        else
            return 0;
    }
    return 0;
}

int AnsiString::operator >=(const char *str) {
    if ((Data) && (str)) {
        if (strcmp(Data, str) >= 0)
            return 1;
        else
            return 0;
    }
    return 0;
}

int AnsiString::operator <=(const char *str) {
    if ((Data) && (str)) {
        if (strcmp(Data, str) <= 0)
            return 1;
        else
            return 0;
    }
    return 0;
}

int AnsiString::operator==(const char *str) {
    if ((Data) && (str)) {
        if (strcmp(Data, str))
            return 0;
        else
            return 1;
    }
    if ((!Data) && (str) && (!str[0]))
        return 1;

    if ((!str) && (Data) && (!Data[0]))
        return 1;

    if ((!Data) && (!str))
        return 1;
    return 0;
}

int AnsiString::operator !=(const char *str) {
    return !operator==(str);
}

int AnsiString::operator==(AnsiString S) {
    return operator==(S.c_str());
}

int AnsiString::operator!=(AnsiString S) {
    return operator!=(S.c_str());
}

int AnsiString::operator >(AnsiString S) {
    return operator >(S.c_str());
}

int AnsiString::operator <(AnsiString S) {
    return operator <(S.c_str());
}

int AnsiString::operator >=(AnsiString S) {
    return operator >=(S.c_str());
}

int AnsiString::operator <=(AnsiString S) {
    return operator <=(S.c_str());
}

AnsiString::~AnsiString(void) {
    free(Data);
}

AnsiString::operator const char *() {
    return c_str();
}

const char *AnsiString::c_str() {
    if (!Data)
        return ansi_string_empty;
    return Data;
}

void AnsiString::operator +=(const char *value) {
    size_t len;
    size_t len_value;

    if ((Data) && (value)) {
        len       = _LENGTH;
        len_value = strlen(value);
        _LENGTH   = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            Data = (char *)realloc(Data, _DATA_SIZE);
            MEMCPY(Data + len, value, len_value + 1);
        } else {
            MEMCPY(Data + len, value, len_value + 1);
        }
    } else {
        operator=(value);
    }
}

void AnsiString::operator +=(AnsiString S) {
    size_t len;
    size_t len_value = S.Length();

    if (!len_value)
        return;

    if ((Data) && (len_value)) {
        len = _LENGTH;
        _LENGTH = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            Data = (char *)realloc(Data, _DATA_SIZE);
        }
        MEMCPY(Data + len, S.c_str(), len_value + 1);
    } else {
        operator=(S);
    }
}

void AnsiString::operator +=(char c) {
    if (_LENGTH + 2 < _DATA_SIZE) {
        Data[_LENGTH++] = c;
        Data[_LENGTH]   = 0;
    } else {
        _DATA_SIZE      += BLOCK_SIZE;
        Data            = (char *)realloc(Data, _DATA_SIZE);
        Data[_LENGTH++] = c;
        Data[_LENGTH]   = 0;
    }
}

char AnsiString::operator [](unsigned long index) {
    size_t len;

    if (Data) {
        len = _LENGTH;
        if (index < len)
            return Data[index];
    }
    return 0;
}

char AnsiString::operator [](int index) {
    ssize_t len;

    if (Data) {
        len = _LENGTH;
        if ((index < len) && (index >= 0))
            return Data[index];
    }
    return 0;
}

long AnsiString::ToInt() {
    if (Data)
        return atol(Data);
    return 0;
}

double AnsiString::ToFloat() {
    if (Data)
        return atof(Data);
    return 0;
}

AnsiString AnsiString::operator +(AnsiString S) {
    AnsiString ret = *this;

    if (S._LENGTH)
        ret += S;
    return ret;
}

int AnsiString::LoadFile(const char *filename) {
    FILE *in;
    long size;

    in = fopen(filename, "rb");
    if (in) {
        fseek(in, 0, SEEK_END);
        size = ftell(in);
        fseek(in, 0, SEEK_SET);
        free(Data);

        _DATA_SIZE = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;

        Data       = (char *)malloc(_DATA_SIZE);
        Data[size] = 0;
        fread(Data, size, 1, in);
        fclose(in);
        _LENGTH = size;
        return 0;
    }
    return -1;
}

int AnsiString::SaveFile(const char *filename) {
    FILE   *in;
    size_t len;
    int    ret = -1;

    in = fopen(filename, "wb");
    if (in) {
        if (Data) {
            len = _LENGTH;
            if (fwrite(Data, len, 1, in) == len)
                ret = 0;
        } else
            ret = 0;
        fclose(in);
        return ret;
    }
    return ret;
}

long AnsiString::Length() {
    if (!Data)
        return 0;
    return _LENGTH;
}

long AnsiString::Pos(AnsiString substr) {
    long result = -1;
    char *ptr   = strstr(Data, substr.c_str());

    if (ptr)
        result = ptr - Data + 1;
    return result;
}

void AnsiString::LoadBuffer(const char *buffer, int size) {
    free(Data);

    _LENGTH    = size;
    _DATA_SIZE = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
    Data = (char *)malloc(_DATA_SIZE);
    MEMCPY(Data, buffer, size);
    Data[size] = 0;
}

void AnsiString::AddBuffer(const char *S_Data, int S_Len, int blocks) {
    size_t len;
    size_t len_value;

    if ((Data) && (S_Data)) {
        //len=strlen(Data);
        len       = _LENGTH;
        len_value = S_Len;
        _LENGTH   = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE * blocks + BLOCK_SIZE;
            Data       = (char *)realloc(Data, _DATA_SIZE);
        }
        memcpy(Data + len, S_Data, len_value + 1);
    } else {
        LoadBuffer(S_Data, S_Len);
    }
}

int AnsiString::Serialize(FILE *out, int type) {
    unsigned char  uClen = (unsigned char)_LENGTH;
    unsigned short uSlen = (unsigned short)_LENGTH;

    switch (type) {
        case SERIALIZE_8BIT_LENGTH:
            fwrite(&uClen, sizeof(uClen), 1, out);
            break;

        case SERIALIZE_16BIT_LENGTH:
            fwrite(&uSlen, sizeof(uSlen), 1, out);
            break;

        default:
            fwrite(&_LENGTH, sizeof(_LENGTH), 1, out);
            break;
    }
    fwrite(Data, _LENGTH, 1, out);
    return 1;
}

int AnsiString::Unserialize(FILE *in, int type) {
    long           len;
    unsigned char  uClen = (unsigned char)_LENGTH;
    unsigned short uSlen = (unsigned short)_LENGTH;

    switch (type) {
        case SERIALIZE_8BIT_LENGTH:
            if (!fread(&uClen, sizeof(uClen), 1, in))
                return -1;
            len = uSlen;
            break;

        case SERIALIZE_16BIT_LENGTH:
            if (!fread(&uSlen, sizeof(uSlen), 1, in))
                return -1;
            len = uSlen;
            break;

        default:
            if (!fread(&len, sizeof(len), 1, in))
                return -1;
            break;
    }

    if (len) {
        free(Data);
        _LENGTH    = len;
        _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
        Data       = (char *)malloc(_DATA_SIZE);
        fread(Data, len, 1, in);
        Data[len] = 0;
    }
    return 1;
}
