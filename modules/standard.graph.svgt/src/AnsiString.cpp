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
    if (S.Data)
        this->LoadBuffer(S.Data, S._LENGTH);
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
                if (Data)
                    delete[] Data;
                _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
                Data       = new char[_DATA_SIZE];
            }
            strcpy(Data, value);
        }
    }
}

void AnsiString::operator=(AnsiString S) {
    size_t len;

    if (Data)
        Data[0] = 0;
    _LENGTH = 0;
    if (S.Data) {
        len     = S._LENGTH;
        _LENGTH = len;
        if (len) {
            if (len + 1 >= _DATA_SIZE) {
                if (Data)
                    delete[] Data;
                _DATA_SIZE = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
                Data       = new char[_DATA_SIZE];
            }
            memcpy(Data, S.Data, len + 1);
        }
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
    if ((Data) && (str)) {
        if (strcmp(Data, str) == -1)
            return 1;
        else
            return 0;
    }
    return 0;
}

int AnsiString::operator >(char *str) {
    if ((Data) && (str)) {
        if (strcmp(Data, str) == 1)
            return 1;
        else
            return 0;
    }
    return 0;
}

int AnsiString::operator >=(char *str) {
    if ((Data) && (str)) {
        if (strcmp(Data, str) >= 0)
            return 1;
        else
            return 0;
    }
    return 0;
}

int AnsiString::operator <=(char *str) {
    if ((Data) && (str)) {
        if (strcmp(Data, str) <= 0)
            return 1;
        else
            return 0;
    }
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
    return 0;
}

int AnsiString::operator !=(char *str) {
    return !operator==(str);
}

int AnsiString::operator==(AnsiString S) {
    return operator==(S.Data);
}

int AnsiString::operator!=(AnsiString S) {
    return operator!=(S.Data);
}

int AnsiString::operator >(AnsiString S) {
    return operator >(S.Data);
}

int AnsiString::operator <(AnsiString S) {
    return operator <(S.Data);
}

int AnsiString::operator >=(AnsiString S) {
    return operator >=(S.Data);
}

int AnsiString::operator <=(AnsiString S) {
    return operator <=(S.Data);
}

AnsiString::~AnsiString(void) {
    if (Data)
        delete[] Data;
}

AnsiString::operator char *() {
    return c_str();
}

char *AnsiString::c_str() {
    if (!Data)
        return "";
    return Data;
}

void AnsiString::operator +=(char *value) {
    char   *newdata;
    size_t len;
    size_t len_value;

    if ((Data) && (value)) {
        //len=strlen(Data);
        len       = _LENGTH;
        len_value = strlen(value);
        _LENGTH   = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            newdata    = new char[_DATA_SIZE];
            strcpy(newdata, Data);
            strcat(newdata, value);
            delete[] Data;
            Data = newdata;
        } else
            strcpy(Data + len, value);
    } else {
        operator=(value);
    }
}

void AnsiString::operator +=(AnsiString S) {
    char   *newdata;
    size_t len;
    size_t len_value;

    if ((Data) && (S.Data)) {
        //len=strlen(Data);
        len       = _LENGTH;
        len_value = S._LENGTH;
        _LENGTH   = len + len_value;
        if (_DATA_SIZE < _LENGTH + 1) {
            _DATA_SIZE = ((_LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            newdata    = new char[_DATA_SIZE];
            memcpy(newdata, Data, len);
            memcpy(newdata + len, S.Data, len_value + 1);
            delete[] Data;
            Data = newdata;
        } else
            memcpy(Data + len, S.Data, len_value + 1);
    } else {
        operator=(S);
    }
    //operator+=(S.Data);
}

void AnsiString::operator +=(char c) {
    /*char buffer[2];
       buffer[0]=c;
       buffer[1]=0;
       operator+=(buffer);*/

    if (_LENGTH + 2 < _DATA_SIZE) {
        Data[_LENGTH++] = c;
        Data[_LENGTH]   = 0;
    } else {
        _DATA_SIZE += BLOCK_SIZE;
        char *newdata = new char[_DATA_SIZE];
        newdata[_LENGTH++] = c;
        newdata[_LENGTH]   = 0;
        if (Data) {
            memcpy(newdata, Data, _LENGTH - 1);
            delete[] Data;
        }
        Data = newdata;
    }
}

char AnsiString::operator [](unsigned long index) {
    size_t len;

    if (Data) {
        //len=strlen(Data);
        len = _LENGTH;
        if (index < len)
            return Data[index];
    }
    return 0;
}

char AnsiString::operator [](int index) {
    size_t len;

    if (Data) {
        //len=strlen(Data);
        len = _LENGTH;
        if ((index < len) && (index >= 0))
            return Data[index];
    }
    return 0;
}

long AnsiString::ToInt() {
    if (Data)
        return atol(Data);

    /*long i=0;
       sscanf(Data, "%li", &i);
       return i;*/
    return 0;
}

double AnsiString::ToFloat() {
    if (Data)
        return atof(Data);
    return 0;
}

AnsiString AnsiString::operator +(AnsiString S) {
    AnsiString ret(Data);

    if (S != (char *)"")
        ret += S;
    return ret;
}

int AnsiString::LoadFile(char *filename) {
    FILE *in;
    long size;

    in = fopen(filename, "rb");
    if (in) {
        fseek(in, 0, SEEK_END);
        size = ftell(in);
        fseek(in, 0, SEEK_SET);
        if (Data)
            delete[] Data;
        _DATA_SIZE = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
        Data       = new char[_DATA_SIZE];
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
            //len=strlen(Data);
            len = _LENGTH;
            if (fwrite(Data, 1, len, in) == len)
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
    //return (long)strlen(Data);
    return _LENGTH;
}

long AnsiString::Pos(AnsiString substr, int after) {
    long result = -1;
    int  offset = 0;

    if ((after > 0) && (after < _LENGTH))
        offset = after;
    else
    if (after >= _LENGTH)
        return result;
    char *ptr = strstr(Data + offset, substr.Data);
    if (ptr)
        result = ptr - Data + 1;
    return result;
}

void AnsiString::LoadBuffer(char *buffer, int size) {
    if (Data)
        delete[] Data;
    _LENGTH    = size;
    _DATA_SIZE = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
    Data       = new char[_DATA_SIZE];
    memcpy(Data, buffer, size);
    Data[size] = 0;
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
            newdata    = new char[_DATA_SIZE];
            memcpy(newdata, Data, len);
            memcpy(newdata + len, S_Data, len_value + 1);
            delete[] Data;
            Data = newdata;
        } else
            memcpy(Data + len, S_Data, len_value + 1);
    } else {
        LoadBuffer(S_Data, S_Len);
    }
    //operator+=(S.Data);
}
