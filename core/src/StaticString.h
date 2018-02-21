#ifndef __STATICSTRING_H
#define __STATICSTRING_H

#include <stdio.h>
#include "ConceptPools.h"
#include "AnsiString.h"

#define SERIALIZE_8BIT_LENGTH     1
#define SERIALIZE_16BIT_LENGTH    2
#define SERIALIZE_32BIT_LENGTH    4

class StaticString {
private:
    intptr_t DataOffset;
    int      _LENGTH;
public:
    POOLED(StaticString);

    StaticString(void);
    StaticString(char *value);
    StaticString(const StaticString& S);
    StaticString(const char *value);

    void operator=(const char *value);
    void operator=(StaticString& S);
    void operator=(intptr_t i);
    void operator=(AnsiString& S);
    int operator!=(StaticString& S);

    operator char *();

    char *c_str();
    intptr_t ToInt();
    double ToFloat();

    intptr_t Length();
    void LoadBuffer(const char *buffer, int size);

    int Serialize(FILE *out, int type);
    static int ComputeSharedSize(concept_FILE *in, int type);
    int Unserialize(concept_FILE *out, int type, signed char use_pool = 0);

    ~StaticString();
};
#endif

