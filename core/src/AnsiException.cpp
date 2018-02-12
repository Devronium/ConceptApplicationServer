#include "AnsiException.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
    #define snprintf    _snprintf
#endif

POOLED_IMPLEMENTATION(AnsiException)

AnsiException::AnsiException(const char *text, int line, intptr_t ID, const char *extra, const char *FileName, const char *class_name, const char *member_name) {
    _line   = line;
    _ID     = ID;
    AnsiException::set_string(&_TEXT, text);
    AnsiException::set_string(&_FILE, FileName);
    AnsiException::set_string(&_EXTRA, extra);
    AnsiException::set_string(&_MEMBER, member_name);
    AnsiException::set_string(&_CLASS, class_name);
}

AnsiException::AnsiException(intptr_t ID, const char *text, int line, const char *extra1, const char *extra2, const char *extra3, const char *FileName, const char *class_name, const char *member_name) {
    _line   = line;
    _ID     = ID;
    AnsiException::set_string(&_TEXT, text);
    AnsiException::set_string(&_FILE, FileName);
    AnsiException::set_string2(&_EXTRA, extra1, extra2, extra3);
    AnsiException::set_string(&_MEMBER, member_name);
    AnsiException::set_string(&_CLASS, class_name);
}

AnsiException::AnsiException(const char *text, int line, intptr_t ID, double extra, const char *FileName, const char *class_name, const char *member_name) {
    char extra_str[0xFF];
    cstr_loaddouble(extra_str, extra);
    AnsiException(text, line, ID, extra_str, FileName, class_name, member_name);
}

AnsiException::AnsiException(intptr_t ID, const char *text, int line, const char *extra1, int extra2, const char *FileName, const char *class_name, const char *member_name) {
    char extra_str[21];
    sprintf(extra_str, "%i", extra2);
    AnsiException(ID, text, line, extra1, extra_str, "", FileName, class_name, member_name);
}

AnsiException::AnsiException(intptr_t ID, const char *text, int line, int extra1, const char *extra2, const char *FileName, const char *class_name, const char *member_name) {
    char extra_str[21];
    sprintf(extra_str, "%i", extra1);
    AnsiException(ID, text, line, extra_str, extra2, "", FileName, class_name, member_name);
}

AnsiException::AnsiException(intptr_t ID, const char *text, int line, const char *extra1, const char *extra2, int extra3, const char *FileName, const char *class_name, const char *member_name) {
    char extra_str[21];
    sprintf(extra_str, "%i", extra2);
    AnsiException(ID, text, line, extra1, extra2, extra_str, FileName, class_name, member_name);
}

const char *AnsiException::GetText() {
    return _TEXT;
}

const char *AnsiException::GetExtra() {
    return _EXTRA;
}

int AnsiException::GetLine() {
    return _line;
}

intptr_t AnsiException::GetID() {
    return _ID;
}

const char *AnsiException::GetFileName() {
    return _FILE;
}

const char *AnsiException::GetClass() {
    return _CLASS;
}

const char *AnsiException::GetMember() {
    return _MEMBER;
}

const char *AnsiException::ToString(char *buffer, int *size, bool short_version) {
    if (buffer)
        buffer[0] = 0;
    if ((!buffer) || (!size) || (!(*size))) {
        if (size)
            *size = 0;
        return NULL;
    }

    if (short_version) {
        if (_CLASS) {
            if (_MEMBER)
                *size = snprintf(buffer, *size, "E%i in %s.%s:%i %s (%s)\n", (int)_ID, _CLASS, _MEMBER, (int)_line, _TEXT, _EXTRA ? _EXTRA : "");
            else
                *size = snprintf(buffer, *size, "E%i in %s:%i %s (%s)\n", (int)_ID, _CLASS, (int)_line, _TEXT, _EXTRA ? _EXTRA : "");
        } else {
            *size = snprintf(buffer, *size, "E%i in %s:%i %s (%s)\n", (int)_ID, _FILE, (int)_line, _TEXT, _EXTRA ? _EXTRA : "");
        }
    } else {
        if (_CLASS) {
            if (_MEMBER)
                *size = snprintf(buffer, *size, "Error Code : \t%i\nFile name : \t%s\nLine number : \t%i\nIn : \t%s.%s\nError text : \t%s\nError extra : \t%s\n", (int)_ID, _FILE, (int)_line, _CLASS, _MEMBER, _TEXT, _EXTRA ? _EXTRA : "");
            else
                *size = snprintf(buffer, *size, "Error Code : \t%i\nFile name : \t%s\nLine number : \t%i\nIn : \t%s\nError text : \t%s\nError extra : \t%s\n", (int)_ID, _FILE, (int)_line, _CLASS, _TEXT, _EXTRA ? _EXTRA : "");
        } else
            *size = snprintf(buffer, *size, "Error Code : \t%i\nFile name : \t%s\nLine number : \t%i\nError text : \t%s\nError extra : \t%s\n", (int)_ID, _FILE, (int)_line, _TEXT, _EXTRA ? _EXTRA : "");
    }
    return buffer;
}

void AnsiException::set_string(char **var, const char *text) {
    int len = text ? strlen(text) : 0;
    if (len) {
        *var = (char *)malloc(len + 1);
        // max printable len
        if (len > 0xFFF)
            len = 0xFFF;
        if (*var) {
            memcpy(*var, text, len);
            (*var)[len] = 0;
        }
    } else
        *var = NULL;
}

void AnsiException::set_string2(char **var, const char *text1, const char *text2, const char *text3) {
    int len1 = text1 ? strlen(text1) : 0;
    int len2 = text2 ? strlen(text2) : 0;
    int len3 = text3 ? strlen(text3) : 0;
    if (len1 > 0xFFF)
        len1 = 0xFFF;
    if (len2 > 0xFFF)
        len2 = 0xFFF;
    if (len3 > 0xFFF)
        len3 = 0xFFF;
    int len = len1 + len2 + len3;
    if (len1) {
        *var = (char *)malloc(len + 1);
        // max printable len
        if (*var) {
            if (len1)
                memcpy(*var, text1, len1);
            if (len2)
                memcpy(*var + len1, text2, len2);
            if (len3)
                memcpy(*var + len1 + len2, text3, len3);
            (*var)[len] = 0;
        }
    } else
        *var = NULL;
}

AnsiException::~AnsiException(void) {
    free(_TEXT);
    free(_FILE);
    free(_MEMBER);
    free(_CLASS);
}
