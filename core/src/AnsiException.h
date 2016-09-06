#ifndef __ANSIEXCEPTION_H
#define __ANSIEXCEPTION_H

#include "AnsiString.h"
#include "Errors.h"
#include "ConceptPools.h"

class AnsiException {
private:
    AnsiString _TEXT;
    AnsiString _EXTRA;
    AnsiString _FILE;
    AnsiString _MEMBER;
    AnsiString _CLASS;
    int        _line;
    intptr_t   _ID;
public:
    POOLED(AnsiException)

    AnsiException(const char *text, int line, intptr_t ID, const char *extra, const char *FileName, const char *class_name = "", const char *member_name = "");
    AnsiString GetText();
    AnsiString GetExtra();
    AnsiString GetFileName();
    AnsiString GetClass();
    AnsiString GetMember();
    intptr_t GetLine();
    intptr_t GetID();

    AnsiString ToString(bool short_version = false);

    ~AnsiException(void);
};
#endif //__ANSIEXCEPTION_H

