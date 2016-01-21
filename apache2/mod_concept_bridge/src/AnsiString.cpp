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

#define fast_memcpy(d, s, n)                                                     \
    { register size_t nn = (size_t)(n);                                          \
      if (nn >= breakeven_point) memcpy((d), (s), nn);                           \
      else if (nn > 0) { /* proc call overhead is worth only for large strings*/ \
          register char *dd; register const char *ss;                            \
          for (ss = (s), dd = (d); nn > 0; nn--) *dd++ = *ss++; } }

//#define MEMCPY      memcpy
#define MEMCPY    fast_memcpy



/*#define HAVE_MMX
 * #include "fastmemcpy.h"
 *
 #define MEMCPY      fast_memcpy*/

/*#include "dmemcopy.h"
 *
 #define MEMCPY      dmemcpy*/

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

AnsiString::AnsiString(char *value) {
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
    //this->LoadBuffer(S.Data,S._LENGTH);
}

void AnsiString::operator=(char *value) {
    size_t len;

    if (Data)
        Data[0] = 0;
    _LENGTH = 0;
    if (value) {
        len     = strlen(value);
        _LENGTH = len;
        if (len) {
            if (len + 1 >= _DATA_SIZE) {
                /*if (Data)
                 *  delete[] Data;*/
                _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
                //Data=new char[_DATA_SIZE];
                Data = (char *)realloc(Data, _DATA_SIZE);
            }
            // varianta 2:
            if (Data)
                MEMCPY(Data, value, len + 1);  // copiez si terminatorul
            // varianta 1:
            // strcpy(Data,value);
        }
    }
}

void AnsiString::operator=(const AnsiString& S) {
    size_t len;

    if (Data)
        Data[0] = 0;
    _LENGTH = 0;

    char *other_data = S.c_str();
    len = S._LENGTH;
    if (len) {
        _LENGTH = len;
        if (len + 1 >= _DATA_SIZE) {
            /*if (Data)
             *  delete[] Data;*/
            free(Data);
            _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            //Data=new char[_DATA_SIZE];
            Data = (char *)malloc(_DATA_SIZE);
        }
        MEMCPY(Data, other_data, len + 1);
        //Data[len]=0;
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

    //operator=(ltoa(i,buffer,10));

    // pt linux !
    sprintf(buffer, "%li", i);
    operator=(buffer);
}

void AnsiString::operator=(double d) {
    char buffer[MAX_DECIMALS];

    sprintf(buffer, "%.30g", d);
    //_gcvt(d,MAX_DECIMALS-10,buffer);
    int len = strlen(buffer);
    if (((len > 1) && ((buffer[len - 1] == '.') || (buffer[len - 1] == ','))))
        buffer[len - 1] = 0;
    operator=(buffer);
}

void AnsiString::operator=(int i) {
    operator=((long)i);
}

int AnsiString::operator <(char *str) {
    if ((Data) && (str))
        return strcmp(Data, str) < 0;
    return 0;
}

int AnsiString::operator >(char *str) {
    if ((Data) && (str))
        return strcmp(Data, str) > 0;
    return 0;
}

int AnsiString::operator >=(char *str) {
    if ((Data) && (str))
        return strcmp(Data, str) >= 0;
    return 0;
}

int AnsiString::operator <=(char *str) {
    if ((Data) && (str))
        return strcmp(Data, str) <= 0;
    return 0;
}

int AnsiString::operator==(char *str) {
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

int AnsiString::operator !=(char *str) {
    return !operator==(str);
}

int AnsiString::operator==(const AnsiString& S) {
    if (_LENGTH != S._LENGTH)
        return 0;
    if (!_LENGTH)
        return 1;
    if (memcmp(Data, S.Data, _LENGTH))
        return 0;
    else
        return 1;
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
    long len = S._LENGTH < this->_LENGTH ? S._LENGTH : this->_LENGTH;

    if (len)
        return memcmp(Data, S.Data, len) > 0;
    return 0;
    //return operator > (S.c_str());
}

int AnsiString::operator <(const AnsiString& S) {
    long len = S._LENGTH < this->_LENGTH ? S._LENGTH : this->_LENGTH;

    if (len)
        return memcmp(Data, S.Data, len) < 0;
    return 0;
    //return operator < (S.c_str());
}

int AnsiString::operator >=(const AnsiString& S) {
    long len = S._LENGTH < this->_LENGTH ? S._LENGTH : this->_LENGTH;

    if (len)
        return memcmp(Data, S.Data, len) >= 0;
    return 0;
    //return operator >= (S.c_str());
}

int AnsiString::operator <=(const AnsiString& S) {
    long len = S._LENGTH < this->_LENGTH ? S._LENGTH : this->_LENGTH;

    if (len)
        return memcmp(Data, S.Data, len) <= 0;
    return 0;
    //return operator <= (S.c_str());
}

AnsiString::~AnsiString(void) {
    /*if (Data)
     *  delete[] Data;*/
    free(Data);
    Data = 0;
}

AnsiString::operator char *() {
    return c_str();
}

char *AnsiString::c_str() const {
    if (!Data)
        return "";
    return Data;
}

void AnsiString::operator +=(char *value) {
    size_t len;
    size_t len_value;

    if ((Data) && (value)) {
        //len=strlen(Data);
        len       = _LENGTH;
        len_value = strlen(value);
        _LENGTH   = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;

            /*newdata=new char[_DATA_SIZE];
             * // varianta 1:
             * //strcpy(newdata,Data);
             * //strcat(newdata,value);
             * // varianta 2
             * MEMCPY(newdata,Data,len);
             * MEMCPY(newdata+len,value,len_value+1);
             * delete[] Data;
             * Data=newdata;*/
            Data = (char *)realloc(Data, _DATA_SIZE);
            MEMCPY(Data + len, value, len_value + 1);
        } else
            //strcpy(Data+len,value);
            MEMCPY(Data + len, value, len_value + 1);
    } else
        operator=(value);
}

void AnsiString::operator +=(const AnsiString& S) {
    size_t len;
    size_t len_value = S._LENGTH;

    if (!len_value)
        return;

    if ((Data) && (len_value)) {
        len     = _LENGTH;
        _LENGTH = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;

            Data = (char *)realloc(Data, _DATA_SIZE);
        }
        MEMCPY(Data + len, S.c_str(), len_value + 1);
    } else
        operator=(S);
}

void AnsiString::operator +=(char c) {
    if (_LENGTH + 2 < _DATA_SIZE) {
        Data[_LENGTH++] = c;
        Data[_LENGTH]   = 0;
    } else {
        _DATA_SIZE     += BLOCK_SIZE;
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

char AnsiString::operator [](long index) {
    size_t len;

    if (Data) {
        len = _LENGTH;
        if (index < len)
            return Data[index];
    }
    return 0;
}

char AnsiString::operator [](int index) {
    size_t len;

    if (Data) {
        len = _LENGTH;
        if ((index < len) && (index >= 0))
            return Data[index];
    }
    return 0;
}

long AnsiString::ToInt() const {
    if (Data)
        return atol(Data);

    return 0;
}

double AnsiString::ToFloat() const {
    if (Data)
        return atof(Data);
    return 0;
}

AnsiString AnsiString::operator +(const AnsiString& S) {
    //AnsiString ret(Data);
    // bug corectat de pe 15 ianuarie 2006
    // problema : Daca data arata de forma "abcde\0\0\0\0" si concatenam un sir ...
    // nu era bine, ca-l punea imediat dupa e, in loc sa-l puna dupa al 4-lea 0
    AnsiString ret = *this;

    if (S._LENGTH)
        ret += S;
    return ret;
}

AnsiString AnsiString::operator+(char *value) {
    if (_LENGTH) {
        AnsiString ret = *this;
        ret += value;
        return ret;
    } else
        return AnsiString(value);
}

int AnsiString::LoadFile(char *filename) {
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

int AnsiString::SaveFile(char *filename) {
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

long AnsiString::Length() const {
    if (!Data)
        return 0;
    return _LENGTH;
}

long AnsiString::Pos(const AnsiString& substr) const {
    long result = -1;
    char *ptr   = strstr(Data, substr.c_str());

    if (ptr)
        result = ptr - Data + 1;
    return result;
}

void AnsiString::LinkBuffer(char *buffer, int size) {
    free(Data);
    if (size < 0)
        size = 0;
    _LENGTH    = size;
    _DATA_SIZE = size;
    if (size)
        Data = buffer;
    else
        Data = 0;
}

void AnsiString::LoadBuffer(char *buffer, int size) {
    free(Data);
    if (size < 0)
        size = 0;
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
        Data[size] = 0;
    }
}

void AnsiString::AddBuffer(char *S_Data, int S_Len) {
    char   *newdata;
    size_t len;
    size_t len_value;

    if ((Data) && (S_Data)) {
        //len=strlen(Data);
        len       = _LENGTH;
        len_value = S_Len;
        _LENGTH   = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            newdata    = (char *)malloc(_DATA_SIZE); //new char[_DATA_SIZE];
            memcpy(newdata, Data, len);
            memcpy(newdata + len, S_Data, len_value + 1);
            //delete[] Data;
            free(Data);
            Data = newdata;
        } else
            memcpy(Data + len, S_Data, len_value + 1);
    } else
        LoadBuffer(S_Data, S_Len);
    //operator+=(S.Data);
}
