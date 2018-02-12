#ifndef __ANSIEXCEPTION_H
#define __ANSIEXCEPTION_H

#include "Errors.h"
#include "ConceptPools.h"

class AnsiException {
private:
    char *_TEXT;
    char *_EXTRA;
    char *_FILE;
    char *_MEMBER;
    char *_CLASS;
    int        _line;
    intptr_t   _ID;
    static void set_string(char **var, const char *text);
    static void set_string2(char **var, const char *text1, const char *text2, const char *text3);
public:
    POOLED(AnsiException)

    AnsiException(const char *text, int line, intptr_t ID, const char *extra, const char *FileName, const char *class_name = "", const char *member_name = "");
    AnsiException(intptr_t ID, const char *text, int line, const char *extra1, const char *extra2, const char *extra3, const char *FileName, const char *class_name = "", const char *member_name = "");
    AnsiException(intptr_t ID, const char *text, int line, const char *extra1, int extra2, const char *FileName, const char *class_name = "", const char *member_name = "");
    AnsiException(intptr_t ID, const char *text, int line, int extra1, const char *extra2, const char *FileName, const char *class_name = "", const char *member_name = "");
    AnsiException(intptr_t ID, const char *text, int line, const char *extra1, const char *extra2, int extra3, const char *FileName, const char *class_name = "", const char *member_name = "");
    AnsiException(const char *text, int line, intptr_t ID, double extra, const char *FileName, const char *class_name = "", const char *member_name = "");
    const char *GetText();
    const char *GetExtra();
    const char *GetFileName();
    const char *GetClass();
    const char *GetMember();
    int GetLine();
    intptr_t GetID();

    const char *ToString(char *buffer, int *size, bool short_version = false);

    ~AnsiException(void);
};
#endif //__ANSIEXCEPTION_H

