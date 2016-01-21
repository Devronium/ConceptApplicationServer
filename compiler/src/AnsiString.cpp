#include "AnsiString.h"

#ifdef USE_DEPRECATED
 #include <iostream.h>
 #include <string.h>
 #include <stdlib.h>
 #include <stdio.h>
#else
 #include <iostream>
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
#endif

AnsiString::AnsiString() {
    Data    = NULL;
    _LENGTH = 0;
}

AnsiString::AnsiString(char *value) {
    Data    = NULL;
    _LENGTH = 0;
    operator=(value);
}

AnsiString::AnsiString(long i) {
    Data    = NULL;
    _LENGTH = 0;
    operator=(i);
}

AnsiString::AnsiString(int i) {
    Data    = NULL;
    _LENGTH = 0;
    operator=((long)i);
}

AnsiString::AnsiString(char c) {
    Data    = NULL;
    _LENGTH = 0;
    operator=(c);
}

AnsiString::AnsiString(double d) {
    Data    = NULL;
    _LENGTH = 0;
    operator=(d);
}

AnsiString::AnsiString(const AnsiString& S) {
    Data    = NULL;
    _LENGTH = 0;
    if (S.Data)
        operator=(S.Data);
}

void AnsiString::operator=(char *value) {
    size_t len;

    if (Data)
        delete[] Data;
    Data = NULL;
    if (value) {
        len     = strlen(value);
        _LENGTH = len;
        if (len) {
            Data = new char[len + 1];
            strcpy(Data, value);
        }
    }
}

void AnsiString::operator=(AnsiString S) {
    size_t len;

    if (Data)
        delete[] Data;
    Data    = NULL;
    _LENGTH = 0;
    if (S.Data) {
        len     = S._LENGTH;
        _LENGTH = len;
        if (len) {
            Data = new char[len + 1];
            strcpy(Data, S.Data);
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

    sprintf(buffer, "%g", d);
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
        newdata   = new char[len + len_value + 1];
        strcpy(newdata, Data);
        strcat(newdata, value);
        delete[] Data;
        Data = newdata;
    } else
        operator=(value);
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
        newdata   = new char[len + len_value + 1];
        strcpy(newdata, Data);
        strcat(newdata, S.Data);
        delete[] Data;
        Data = newdata;
    } else
        operator=(S);
    //operator+=(S.Data);
}

void AnsiString::operator +=(char c) {
    /*char buffer[2];
     * buffer[0]=c;
     * buffer[1]=0;
     * operator+=(buffer);*/
    char *newdata = new char[++_LENGTH + 1];

    newdata[_LENGTH - 1] = c;
    newdata[_LENGTH]     = 0;
    if (Data) {
        memcpy(newdata, Data, _LENGTH - 1);
        delete[] Data;
    }
    Data = newdata;
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
     *  sscanf(Data, "%li", &i);
     *  return i;*/
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
        Data       = new char[size + 1];
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
    //return (long)strlen(Data);
    return _LENGTH;
}

long AnsiString::Pos(AnsiString substr) {
    long result = -1;
    char *ptr   = strstr(Data, substr.Data);

    if (ptr)
        result = ptr - Data + 1;
    return result;
}
