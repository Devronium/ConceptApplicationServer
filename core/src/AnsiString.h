// (c) 2005 RadGs Software
// Author : Suica Eduard

#ifndef __ANSISTRING_H
#define __ANSISTRING_H

#include <stdio.h>
#include <stdint.h>
//#define USE_DEPRECATED
#include "ConceptPools.h"

#define MAX_DECIMALS              0xFF
#define BLOCK_SIZE                16 //16

#define SERIALIZE_8BIT_LENGTH     1
#define SERIALIZE_16BIT_LENGTH    2
#define SERIALIZE_32BIT_LENGTH    4

#if __SIZEOF_POINTER__ == 8
 #define D_LONG_TYPE              intptr_t
#else
 #define D_LONG_TYPE              long
#endif

class AnsiString {
private:
    char     *Data;
    intptr_t _DATA_SIZE;

    intptr_t _LENGTH;
public:
    uintptr_t EXTRA_DATA;
    POOLED(AnsiString);

    AnsiString(void);
    AnsiString(char *value);
    AnsiString(const char *value);
    AnsiString(char c);
    AnsiString(int i);
    AnsiString(D_LONG_TYPE i);
    AnsiString(double d);
    AnsiString(const AnsiString& S);

    char operator[](D_LONG_TYPE index);
    char operator[](uintptr_t index);
    char operator[](int index);

    int operator==(char *str);
    int operator!=(char *str);

    int operator==(double d);
    int operator!=(double d);

    int operator==(const AnsiString& S);
    int operator!=(const AnsiString& S);

    int operator <(char *str);
    int operator <=(char *str);
    int operator >(char *str);
    int operator >=(char *str);

    int operator <(const AnsiString& S);
    int operator <=(const AnsiString& S);
    int operator >(const AnsiString& S);
    int operator >=(const AnsiString& S);

    void operator=(const char *value);
    void operator=(char *value);
    void operator=(const AnsiString& S);
    void operator=(D_LONG_TYPE i);
    void operator=(int i);
    void operator=(char c);
    void operator=(double d);

    void operator+=(const char *value);
    void operator+=(char *value);
    void operator+=(const AnsiString& S);
    void operator+=(char c);

    operator char *();

    AnsiString operator+(const AnsiString& S);
    AnsiString operator+(char *value);
    AnsiString operator+(const char *value);

    char *c_str() const;
    intptr_t ToInt() const;
    double ToFloat() const;

    int LoadFile(char *filename);
    int SaveFile(char *filename);
    void LoadBuffer(char *buffer, int size);
    void AddBuffer(char *S_Data, int S_Len);
    void LinkBuffer(char *buffer, int size);
    void IncreaseBuffer(int size);
    void Sum(AnsiString& S1, AnsiString& S2);
    void Asg(AnsiString& S);
    void ReplaceCharWithString(AnsiString& s, intptr_t position);

    intptr_t Length() const;

    intptr_t Pos(const AnsiString& substr) const;

    int Serialize(FILE *out, int type);
    static int ComputeSharedSize(concept_FILE *in, int type);

    // 0 not to use pool, 1 to use pool and -1 not using pool, but incrementing
    int Unserialize(concept_FILE *out, int type, signed char use_pool = 0);

    ~AnsiString();
};
#endif

