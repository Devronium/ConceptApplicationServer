// (c) 2008 RadGs Software
// Author : Suica Eduard

#ifndef __TinySTRING_H
#define __TinySTRING_H

#include <stdio.h>
#include "AnsiString.h"
#include "ConceptPools.h"
//#define USE_DEPRECATED

class TinyString {
private:
    intptr_t DataOffset;
public:
    POOLED(TinyString)
    TinyString(void);
    TinyString(char *value);
    TinyString(const char *value);
    TinyString(char c);
    TinyString(int i);
    TinyString(long i);
    TinyString(double d);
    TinyString(const TinyString& S);

    char operator[](uintptr_t index);
    char operator[](int index);

    int operator==(AnsiString& S);
    int operator!=(AnsiString& S);
    void operator=(AnsiString& S);

    int operator==(char *str);
    int operator!=(char *str);

    int operator==(TinyString& S);
    int operator!=(TinyString& S);

    int operator <(char *str);
    int operator <=(char *str);
    int operator >(char *str);
    int operator >=(char *str);

    int operator <(TinyString& S);
    int operator <=(TinyString& S);
    int operator >(TinyString& S);
    int operator >=(TinyString& S);

    void operator=(char *value);
    void operator=(TinyString& S);
    void operator=(long i);
    void operator=(int i);
    void operator=(char c);
    void operator=(double d);

    void operator+=(char *value);
    void operator+=(TinyString& S);
    void operator+=(char c);

    operator char *();
    operator AnsiString();

    TinyString operator+(TinyString& S);

    char *c_str();
    intptr_t ToInt();
    double ToFloat();

    intptr_t Length();

    intptr_t Pos(TinyString substr);

    void LoadBuffer(char *buffer, int size);

    int Serialize(FILE *out, int type);
    int Unserialize(concept_FILE *out, int type, signed char use_pool = 0);

    ~TinyString();
};
#endif

