#include "AnsiString.h"
#include "ConceptTypes.h"
#include "SHManager.h"
#define WITH_DTOA
#ifdef WITH_DTOA
    #include "dtoa.h"
#endif

POOLED_IMPLEMENTATION(AnsiString)

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
    { register size_t nn = (size_t)(n);                                                      \
      if (nn >= breakeven_point) { memcpy((d), (s), nn); }                                   \
      else if (nn > 0) {                                                                     \
          register char *dd; register const char *ss;                                        \
          for (ss = (s), dd = (d); nn > 0; nn--) { *dd++ = *ss++; } } }

//#define MEMCPY      memcpy
#define MEMCPY    fast_memcpy

void memcpy32(void *dest, void *src, int bcount) {
    if (bcount < 4) {
        memcpy(dest, src, bcount);
        return;
    }
    int blocks = bcount / 4;
    bcount %= 4;

    intptr_t *ldest = (intptr_t *)dest;
    intptr_t *lsrc  = (intptr_t *)src;

    void *end = lsrc + blocks;

label1:
    *ldest++ = *lsrc++;
    if (lsrc != end) {
        goto label1;
    }

    if (bcount) {
        memcpy(ldest, lsrc, bcount);
    }
}

AnsiString::AnsiString() {
    Data       = NULL;
    _DATA_SIZE = 0;
    _LENGTH    = 0;
    EXTRA_DATA = 0;
}

AnsiString::AnsiString(char *value) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    EXTRA_DATA = 0;
    operator=(value);
}

AnsiString::AnsiString(const char *value) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    EXTRA_DATA = 0;
    operator=((char *)value);
}

AnsiString::AnsiString(D_LONG_TYPE i) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    EXTRA_DATA = 0;
    operator=(i);
}

AnsiString::AnsiString(int i) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    EXTRA_DATA = 0;
    operator=((D_LONG_TYPE)i);
}

AnsiString::AnsiString(char c) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    EXTRA_DATA = 0;
    operator=(c);
}

AnsiString::AnsiString(double d) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    EXTRA_DATA = 0;
    operator=(d);
}

AnsiString::AnsiString(const AnsiString& S) {
    Data       = NULL;
    _LENGTH    = 0;
    _DATA_SIZE = 0;
    EXTRA_DATA = 0;
    if (S._LENGTH) {
        this->LoadBuffer(S.Data, S._LENGTH);
    }
}

void AnsiString::operator=(char *value) {
    size_t len;

    if (Data == value)
        return;
    if (Data) {
        Data [0] = 0;
    }
    _LENGTH = 0;
    if (value) {
        len     = strlen(value);
        _LENGTH = len;
        if (len) {
            if (len + 1 >= _DATA_SIZE) {
                _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
                Data       = (char *)realloc(Data, _DATA_SIZE);
            }
            if (Data) {
                MEMCPY(Data, value, len + 1);
            }
        }
    }
}

void AnsiString::operator=(const char *value) {
    size_t len;

    if (Data == value)
        return;
    if (Data) {
        Data[0] = 0;
    }
    _LENGTH = 0;
    if (value) {
        len     = strlen(value);
        _LENGTH = len;
        if (len) {
            if (len + 1 >= _DATA_SIZE) {
                _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
                Data       = (char *)realloc(Data, _DATA_SIZE);
            }
            if (Data) {
                MEMCPY(Data, value, len + 1);
            }
        }
    }
}

void AnsiString::operator=(const AnsiString& S) {
    size_t len;
    char   *other_data = S.c_str();

    // same buffer!!!
    if (other_data == Data)
        return;

    len = S._LENGTH;
    if (Data) {
        if (_DATA_SIZE - len > BLOCK_SIZE) {
            free(Data);
            Data       = 0;
            _DATA_SIZE = 0;
        } else
            Data [0] = 0;
    }
    _LENGTH = 0;

    if (len) {
        _LENGTH = len;
        if (len + 1 >= _DATA_SIZE) {

            free(Data);
            _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            Data = (char *)malloc(_DATA_SIZE);
        }
        MEMCPY(Data, other_data, len);
        Data[len] = 0;
    }
}

void AnsiString::operator=(char c) {
    size_t len;

    _LENGTH = 0;
    len     = 1;
    _LENGTH = len;
    if (_DATA_SIZE < 2) {
        _DATA_SIZE = BLOCK_SIZE;
        Data       = (char *)realloc(Data, _DATA_SIZE);
    }
    if (Data) {
        Data[0] = c;
        Data[1] = 0;
    }
}

void AnsiString::operator=(D_LONG_TYPE i) {
    char buffer [MAX_DECIMALS];

    sprintf(buffer, "%li", i);
    operator=(buffer);
}

void AnsiString::operator=(double d) {
    char buffer [MAX_DECIMALS];
#ifdef WITH_DTOA
    // 3 times faster
    dtoa_milo(d, buffer);
#else
    sprintf(buffer, "%.15g", d);
    size_t len = strlen(buffer);
    if (((len > 1) && ((buffer [len - 1] == '.') || (buffer [len - 1] == ',')))) {
        buffer [len - 1] = 0;
    }
#endif
    operator=(buffer);
}

void AnsiString::operator=(int i) {
    operator=((D_LONG_TYPE)i);
}

int AnsiString::operator <(const char *str) {
    if ((Data) && (str))
        return strcmp(Data, str) < 0;

    const char *ref_Data = Data;
    if (!str)
        str = "";
    if (!ref_Data)
        ref_Data = "";

    return strcmp(ref_Data, str) < 0;
}

int AnsiString::operator >(const char *str) {
    if ((Data) && (str))
        return strcmp(Data, str) > 0;

    const char *ref_Data = Data;
    if (!str)
        str = "";
    if (!ref_Data)
        ref_Data = "";

    return strcmp(ref_Data, str) > 0;
}

int AnsiString::operator >=(const char *str) {
    if ((Data) && (str))
        return strcmp(Data, str) >= 0;

    const char *ref_Data = Data;
    if (!str)
        str = "";
    if (!ref_Data)
        ref_Data = "";

    return strcmp(ref_Data, str) >= 0;
}

int AnsiString::operator <=(const char *str) {
    if ((Data) && (str))
        return strcmp(Data, str) <= 0;

    const char *ref_Data = Data;
    if (!str)
        str = "";
    if (!ref_Data)
        ref_Data = "";

    return strcmp(ref_Data, str) <= 0;
}

int AnsiString::operator==(const char *str) {
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

int AnsiString::operator !=(const char *str) {
    return !operator==(str);
}

int AnsiString::operator==(const AnsiString& S) {
    if (_LENGTH != S._LENGTH) {
        return 0;
    }
    if (!_LENGTH) {
        return 1;
    }
    if (memcmp(Data, S.Data, _LENGTH)) {
        return 0;
    } else {
        return 1;
    }
    return 0;
}

int AnsiString::operator==(double d) {
    AnsiString S(d);

    return operator==(S);
}

int AnsiString::operator!=(double d) {
    AnsiString S(d);

    return operator!=(S);
}

int AnsiString::operator!=(const AnsiString& S) {
    return !operator==(S);
}

int AnsiString::operator >(const AnsiString& S) {
    intptr_t len = S._LENGTH < this->_LENGTH ? S._LENGTH : this->_LENGTH;

    if (len) {
        int res = memcmp(Data, S.Data, len);
        if (res > 0)
            return 1;
        if ((res == 0) && (this->_LENGTH > S._LENGTH))
            return 1;
    } else
    if (this->_LENGTH)
        return 1;
    return 0;
}

int AnsiString::operator <(const AnsiString& S) {
    intptr_t len = S._LENGTH < this->_LENGTH ? S._LENGTH : this->_LENGTH;

    if (len) {
        int res = memcmp(Data, S.Data, len);
        if (res < 0)
            return 1;
        if ((res == 0) && (this->_LENGTH < S._LENGTH))
            return 1;
    } else
    if (S._LENGTH)
        return 1;
    return 0;
}

int AnsiString::operator >=(const AnsiString& S) {
    intptr_t len = S._LENGTH < this->_LENGTH ? S._LENGTH : this->_LENGTH;

    if (len) {
        int res = memcmp(Data, S.Data, len);
        if (res > 0)
            return 1;
        if ((res == 0) && (this->_LENGTH >= S._LENGTH))
            return 1;
    } else
    if ((this->_LENGTH) || (this->_LENGTH == S._LENGTH))
        return 1;
    return 0;
}

int AnsiString::operator <=(const AnsiString& S) {
    intptr_t len = S._LENGTH < this->_LENGTH ? S._LENGTH : this->_LENGTH;

    if (len) {
        int res = memcmp(Data, S.Data, len);
        if (res < 0)
            return 1;
        if ((res == 0) && (this->_LENGTH <= S._LENGTH))
            return 1;
    } else
    if ((S._LENGTH) || (this->_LENGTH == S._LENGTH))
        return 1;
    return 0;
}

AnsiString::~AnsiString(void) {

    free(Data);
    Data = 0;
}

AnsiString::operator char *() {
    return c_str();
}

char *AnsiString::c_str() const {
    if (!Data)
        return (char *)"";

    return Data;
}

void AnsiString::operator +=(char *value) {
    size_t len;
    size_t len_value;

    if ((Data) && (value)) {
        len       = _LENGTH;
        len_value = strlen(value);
        _LENGTH   = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            Data       = (char *)realloc(Data, _DATA_SIZE);
            MEMCPY(Data + len, value, len_value + 1);
        } else {
            MEMCPY(Data + len, value, len_value + 1);
        }
    } else
    if (value) {
        operator=(value);
    }
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
            Data       = (char *)realloc(Data, _DATA_SIZE);
            MEMCPY(Data + len, value, len_value + 1);
        } else {
            MEMCPY(Data + len, value, len_value + 1);
        }
    } else
    if (value) {
        operator=((char *)value);
    }
}

void AnsiString::operator +=(const AnsiString& S) {
    size_t len;
    size_t len_value = S._LENGTH;

    if (!len_value)
        return;

    if (Data) {
        if (Data == S.Data) {
            // overlapping !
            AnsiString temp(S);
            len     = _LENGTH;
            _LENGTH = len + len_value;

            if (_DATA_SIZE < _LENGTH + 1) {
                _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;

                Data = (char *)realloc(Data, _DATA_SIZE);
            }
            MEMCPY(Data + len, temp.Data, len_value);
        } else {
            len     = _LENGTH;
            _LENGTH = len + len_value;

            if (_DATA_SIZE < _LENGTH + 1) {
                _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;

                Data = (char *)realloc(Data, _DATA_SIZE);
            }
            MEMCPY(Data + len, S.Data, len_value);
        }
        Data[_LENGTH] = 0;
    } else {
        operator=(S);
    }
}

void AnsiString::IncreaseBuffer(int size) {
    if (size <= 0)
        return;

    int new_len = _LENGTH + size + 1;

    if (_DATA_SIZE < new_len) {
        int mod_size = new_len / BLOCK_SIZE;

        if ((mod_size >= 0x2000) && (_DATA_SIZE > 0x20000))
            new_len = (new_len / BLOCK_SIZE + mod_size / 3) * BLOCK_SIZE;

        _DATA_SIZE = (new_len / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;

        Data = (char *)realloc(Data, _DATA_SIZE);
    }
}

void AnsiString::operator +=(char c) {
    if (_LENGTH + 2 < _DATA_SIZE) {
        Data [_LENGTH++] = c;
        Data [_LENGTH]   = 0;
    } else {
        _DATA_SIZE      += BLOCK_SIZE;
        Data             = (char *)realloc(Data, _DATA_SIZE);
        Data [_LENGTH++] = c;
        Data [_LENGTH]   = 0;
    }
}

char AnsiString::operator [](uintptr_t index) {
    size_t len;

    if (Data) {
        len = _LENGTH;
        if (index < len)
            return Data [index];
    }
    return 0;
}

char AnsiString::operator [](D_LONG_TYPE index) {
    size_t len;

    if (Data) {
        len = _LENGTH;
        if ((index < len) && (index >= 0)) {
            return Data [index];
        }
    }
    return 0;
}

char AnsiString::operator [](int index) {
    size_t len;

    if (Data) {
        len = _LENGTH;
        if ((index < len) && (index >= 0)) {
            return Data [index];
        }
    }
    return 0;
}

intptr_t AnsiString::ToInt() const {
    if (Data) {
        return atol(Data);
    }

    return 0;
}

double AnsiString::ToFloat() const {
    if (Data) {
        return atof(Data);
    }
    return 0;
}

AnsiString AnsiString::operator +(const AnsiString& S) {
    AnsiString ret = *this;

    if (S._LENGTH) {
        ret += S;
    }
    return ret;
}

AnsiString AnsiString::operator+(const char *value) {
    if (_LENGTH) {
        AnsiString ret = *this;
        ret += value;
        return ret;
    } else {
        return AnsiString(value);
    }
}

AnsiString AnsiString::operator+(char *value) {
    if (_LENGTH) {
        AnsiString ret = *this;
        ret += value;
        return ret;
    } else   {
        return AnsiString(value);
    }
}

int AnsiString::LoadFile(char *filename) {
    FILE     *in;
    intptr_t size;

    in = fopen(filename, "rb");
    if (in) {
        fseek(in, 0, SEEK_END);
        size = ftell(in);
        fseek(in, 0, SEEK_SET);
        free(Data);

        _DATA_SIZE  = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
        Data        = (char *)malloc(_DATA_SIZE);
        Data [size] = 0;
        fread(Data, size, 1, in);
        fclose(in);
        _LENGTH = size;
        return 0;
    }
    return -1;
}

int AnsiString::SaveFile(char *filename) {
    FILE   *in;
    size_t len;
    int    ret = -1;

    in = fopen(filename, "wb");
    if (in) {
        if (Data) {
            len = _LENGTH;
            if (fwrite(Data, 1, len, in) == len) {
                ret = 0;
            }
        } else {
            ret = 0;
        }
        fclose(in);
        return ret;
    }
    return ret;
}

intptr_t AnsiString::Length() const {
    if (!Data) {
        return 0;
    }
    return _LENGTH;
}

intptr_t AnsiString::Pos(const AnsiString& substr) const {
    intptr_t result = -1;
    char     *ptr   = strstr(Data, substr.c_str());

    if (ptr) {
        result = ptr - Data + 1;
    }
    return result;
}

void AnsiString::LinkBuffer(char *buffer, int size) {
    free(Data);
    if (size < 0) {
        size = 0;
    }
    _LENGTH    = size;
    _DATA_SIZE = size;
    if (size) {
        Data = buffer;
    } else {
        Data = 0;
    }
}

void AnsiString::LoadBuffer(const char *buffer, int size) {
    void *free_after = NULL;
    if ((Data) && (buffer == Data))
        free_after = Data;
    else
        free(Data);
    if (size < 0) {
        size = 0;
    }
    Data    = 0;
    _LENGTH = size;
    if (!size) {
        _DATA_SIZE = 0;
        return;
    }
    _DATA_SIZE = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
    Data       = (char *)malloc(_DATA_SIZE);
    if (Data) {
        MEMCPY(Data, buffer, size);
        Data [size] = 0;
    }   
    free(free_after);
}

void AnsiString::Sum(AnsiString& S1, AnsiString& S2) {
    if ((Data) && ((S1.Data == Data) || (S2.Data == Data))) {
        this->LoadBuffer(S1.Data, S1._LENGTH);
        *this += S2;
        return;
    }

    int len1 = S1._LENGTH;
    int len2 = S2._LENGTH;
    int size = len1 + len2;

    if ((size < _LENGTH) || (!Data)) {
        if (Data)
            free(Data);

        Data    = 0;
        _LENGTH = 0;
        if (!size) {
            _DATA_SIZE = 0;
            return;
        }
        _DATA_SIZE = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
        Data       = (char *)malloc(_DATA_SIZE);
    } else
        this->IncreaseBuffer(size - _LENGTH);

    if (len1) {
        MEMCPY(Data, S1.Data, len1);
    }
    if (len2)
        MEMCPY(Data + len1, S2.Data, len2);
    _LENGTH     = size;
    Data [size] = 0;
}

void AnsiString::Asg(AnsiString& S) {
    int size = S._LENGTH;

    if ((size < _LENGTH) || (!Data)) {
        if (Data)
            free(Data);

        Data    = 0;
        _LENGTH = 0;
        if (!size) {
            _DATA_SIZE = 0;
            return;
        }
        _DATA_SIZE = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
        Data       = (char *)malloc(_DATA_SIZE);
    } else
        this->IncreaseBuffer(size - _LENGTH);

    if (size)
        MEMCPY(Data, S.Data, size);
    _LENGTH     = size;
    Data [size] = 0;
}

int AnsiString::Serialize(FILE *out, int type) {
    unsigned char  uClen = (unsigned char)_LENGTH;
    unsigned short uSlen = (unsigned short)_LENGTH;

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

int AnsiString::ComputeSharedSize(concept_FILE *in, int type) {
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

int AnsiString::Unserialize(concept_FILE *in, int type, signed char use_pool) {
    int            len;
    unsigned char  uClen = (unsigned char)_LENGTH;
    unsigned short uSlen = (unsigned short)_LENGTH;

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

    if (len) {
        free(Data);
        if (use_pool == 1) {
            _LENGTH    = len;
            _DATA_SIZE = len + 1;
            Data       = (char *)SHAlloc(_DATA_SIZE);
            concept_fread_buffer(Data, len, 1, in);
            Data [len] = 0;
        } else {
            _LENGTH    = len;
            _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            Data       = (char *)malloc(_DATA_SIZE);
            concept_fread_buffer(Data, len, 1, in);
            Data [len] = 0;
            if (use_pool == -1) {
                SHAlloc(len + 1);
            }
        }
    } else {
        _LENGTH = 0;
    }
    return 1;
}

void AnsiString::AddBuffer(const char *S_Data, int S_Len) {
    size_t len;
    size_t len_value;

    if ((!S_Data) || (!S_Len))
        return;

    if (Data) {
        len       = _LENGTH;
        len_value = S_Len;
        _LENGTH   = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;

            Data = (char *)realloc(Data, _DATA_SIZE);
            if (Data) {
                memcpy(Data + len, S_Data, len_value);
                Data[_LENGTH] = 0;
            } else {
                _LENGTH    = 0;
                _DATA_SIZE = 0;
            }
        } else {
            memcpy(Data + len, S_Data, len_value);
            Data[_LENGTH] = 0;
        }
    } else {
        LoadBuffer(S_Data, S_Len);
    }
}

void AnsiString::ReplaceCharWithString(AnsiString& s, intptr_t position) {
    if ((position < 0) || (position >= _LENGTH))
        return;

    int slen = s._LENGTH;
    if (slen == 1) {
        Data[position] = s.Data[0];
    } else {
        uintptr_t len = _LENGTH + s._LENGTH;
        const char *ptr = s.Data;
        uintptr_t ptr_len = s._LENGTH;
        AnsiString temp;
        if (s.Data == Data) {
            temp = s;
            ptr = temp.c_str();
        }
        if (len > _DATA_SIZE) {
            _DATA_SIZE = (len / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            Data       = (char *)realloc(Data, _DATA_SIZE);
        }
        if (Data) {
            memmove(Data + position + ptr_len, Data + position + 1, _LENGTH - position - 1);
            memmove(Data + position, ptr, ptr_len);
            _LENGTH       = len - 1;
            Data[_LENGTH] = 0;
        }
    }
}
