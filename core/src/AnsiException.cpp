#include "AnsiException.h"

POOLED_IMPLEMENTATION(AnsiException)

AnsiException::AnsiException(const char *text, int line, intptr_t ID, const char *extra, const char *FileName, const char *class_name, const char *member_name) {
    _TEXT   = text;
    _line   = line;
    _FILE   = FileName;
    _ID     = ID;
    _EXTRA  = extra;
    _MEMBER = member_name;
    _CLASS  = class_name;
}

AnsiString AnsiException::GetText() {
    return _TEXT;
}

AnsiString AnsiException::GetExtra() {
    return _EXTRA;
}

intptr_t AnsiException::GetLine() {
    return _line;
}

intptr_t AnsiException::GetID() {
    return _ID;
}

AnsiString AnsiException::GetFileName() {
    return _FILE;
}

AnsiString AnsiException::GetClass() {
    return _CLASS;
}

AnsiString AnsiException::GetMember() {
    return _MEMBER;
}

AnsiString AnsiException::ToString(bool short_version) {
    AnsiString result;

    if (short_version) {
        result += "E";
        result += AnsiString(_ID);
        result += " in ";
        if (_CLASS != "") {
            result += _CLASS;
            if (_MEMBER != "") {
                if (_CLASS.Length()) {
                    result += ".";
                }
                result += _MEMBER;
            }
            result += ":";
            result += AnsiString(_line);
            result += "\t";
        } else {
            result += _FILE;
            result += ":";
            result += AnsiString(_line);
            result += "\t";
        }
        result += _TEXT;
        result += " (";
        result += _EXTRA;
        result += ")\n";
    } else {
        result = "Error Code : \t";

        result += AnsiString(_ID);
        result += "\nFile name : \t";
        result += _FILE;
        result += "\nLine number : \t";
        result += AnsiString(_line);
        if (_CLASS != "") {
            result += "\nIn : \t";
            result += _CLASS;
            if (_MEMBER != "") {
                if (_CLASS.Length()) {
                    result += ".";
                }
                result += _MEMBER;
            }
        }
        result += "\nError text : \t";
        result += _TEXT;
        result += "\nError extra : \t";
        result += _EXTRA;
        result += "\n";
    }
    return result;
}

AnsiException::~AnsiException(void) {
}

