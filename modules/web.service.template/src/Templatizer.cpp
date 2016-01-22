#include "Templatizer.h"
#include <string.h>
#include <time.h>
#include <stdint.h>

CTemplatizer::CTemplatizer(INVOKE_CALL INV) {
    Invoke = INV;
}

int CTemplatizer::FindVariable(AnsiString name) {
    int cnt = Variables.Count();

    for (int i = 0; i < cnt; i++) {
        TemplateVariable *TV = (TemplateVariable *)Variables.Item(i);
        if (TV->name == name)
            return i;
    }
    return -1;
}

int CTemplatizer::FindVariable(void *data) {
    int cnt = Variables.Count();

    for (int i = 0; i < cnt; i++) {
        TemplateVariable *TV = (TemplateVariable *)Variables.Item(i);
        if (TV->data == data)
            return i;
    }
    return -1;
}

int CTemplatizer::FindVariable(AnsiString name, void *data) {
    int cnt = Variables.Count();

    for (int i = 0; i < cnt; i++) {
        TemplateVariable *TV = (TemplateVariable *)Variables.Item(i);
        if ((TV->data == data) || (TV->name == name))
            return i;
    }
    return -1;
}

int CTemplatizer::BindVariable(AnsiString name, void *data) {
    if (FindVariable(name, data) > -1)
        return 0;

    TemplateVariable *TV = new TemplateVariable(name, data, Invoke, line, filename);
    Variables.Add(TV, DATA_VARIABLE);

    return 1;
}

int CTemplatizer::UnbindVariable(AnsiString name) {
    int index = FindVariable(name);

    if (index == -1)
        return 0;

    Variables.Delete(index);
    return 1;
}

int CTemplatizer::UnbindVariable(void *data) {
    int index = FindVariable(data);

    if (index == -1)
        return 0;

    Variables.Delete(index);
    return 1;
}

#define IS_MODIFIER(c)      ((c == '.') || (c == ':') || (c == '['))
//#define IS_IN_GRAMMAR(c) (((c>='a') && (c<='z')) || ((c>='A') && (c<='Z')) || ((c>='0') && (c<='9')) || (c=='_')) ? 1 : 0
#define IS_IN_GRAMMAR(c)    (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_')) ? 1 : 0
#define IS_SEPARATOR(c)     ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n')) ? 1 : 0

char *CTemplatizer::Literal(char *data, AnsiList *Owner) {
    AnsiString       element;
    AnsiString       parse;
    signed char      in_brackets = 1;
    CTemplateElement *te         = new CTemplateElement(HTML, line, filename);

    Owner->Add(te, DATA_ELEMENT);
    signed char wait_end = 0;
    while (char c = *data) {
        if (c == '{') {
            in_brackets = 1;
            parse      += c;
            data++;
            continue;
        }

        if (in_brackets) {
            if (IS_IN_GRAMMAR(c)) {
                element += c;
                parse   += c;
            } else {
                parse += c;
                int len = element.Length();
                if (wait_end) {
                    errors += "Extra data after 'endliteral' encountered";
                    ADD_INFO_ERROR(errors);
                }
                if ((len) && (element == (char *)"endliteral")) {
                    wait_end = 1;
                } else {
                    if ((wait_end) && (len)) {
                        errors += "Extra data after 'endliteral' encountered";
                        ADD_INFO_ERROR(errors);
                    }
                    te->HTML_DATA += parse;
                }
                parse   = (char *)"";
                element = (char *)"";
            }
        } else
            te->HTML_DATA += c;

        if (c == '}') {
            in_brackets = 0;
            if (wait_end) {
                data++;
                break;
            }
        }
        data++;
    }
    return data;
}

char *CTemplatizer::ParseElement(char *data, AnsiList *Owner, CTemplateElement *EO) {
    AnsiString element;

    data++;
    CTemplateElement *te             = 0;
    CTemplateElement *te2            = 0;
    char             expect_end      = 0;
    char             expect_modifier = 0;
    char             is_literal      = 0;
    char             expect_member   = 0;
    char             expect_key      = 0;
    int  index;
    char peek_data;

    while (char c = *data) {
        if (c == '\n')
            line++;
        peek_data = (char)(*(data + 1));

        if ((IS_IN_GRAMMAR(c)) || (((c == '\"') || (c == '\'')) && expect_key))
            element += c;
        else if (element.Length()) {
            if (expect_end) {
                errors += "End expected, but '";
                errors += c;
                errors += "' found";
                ADD_INFO_ERROR(errors);
                return data;
            }
            if (!te) {
                if (element == (char *)"foreach") {
                    te = new CTemplateElement(FOREACH, line, filename);
                    Owner->Add(te, DATA_ELEMENT);
                } else
                if (element == (char *)"if") {
                    te = new CTemplateElement(IF, line, filename);
                    Owner->Add(te, DATA_ELEMENT);
                } else
                if (element == (char *)"endfor") {
                    if ((EO) && (EO->Type == FOREACH)) {
                        go_back    = 1;
                        expect_end = 1;
                    } else {
                        errors += "Unexpected 'endfor'";
                        ADD_INFO_ERROR(errors);
                        return data;
                    }
                } else
                if (element == (char *)"endif") {
                    if ((EO) && (EO->Type == IF)) {
                        go_back    = 1;
                        expect_end = 1;
                    } else {
                        errors += "Unexpected 'endif'";
                        ADD_INFO_ERROR(errors);
                        return data;
                    }
                } else
                if (element == (char *)"endliteral") {
                    errors += "Unexpected 'endliteral'";
                    ADD_INFO_ERROR(errors);
                    return data;
                } else
                if (element == (char *)"else") {
                    if ((EO) && (EO->Type == IF)) {
                        go_back    = -1;
                        expect_end = 1;
                    } else {
                        errors += "Unexpected 'else'";
                        ADD_INFO_ERROR(errors);
                        return data;
                    }
                } else
                if (element == (char *)"ldelim") {
                    te = new CTemplateElement(HTML, line, filename);
                    Owner->Add(te, DATA_ELEMENT);
                    te->HTML_DATA = '{';
                    expect_end    = 1;
                    te            = 0;
                } else
                if (element == (char *)"literal") {
                    expect_end = 1;
                    is_literal = 1;
                    te         = 0;
                } else
                if (element == (char *)"rdelim") {
                    te = new CTemplateElement(HTML, line, filename);
                    Owner->Add(te, DATA_ELEMENT);
                    te->HTML_DATA = '}';
                    expect_end    = 1;
                    te            = 0;
                } else
                if (element == (char *)"include") {
                    FILE       *FIN;
                    AnsiString fname;
                    bool       started = false;

                    while (true) {
                        c = *data;
                        if (!c)
                            break;

                        if (c == '\n')
                            line++;

                        if (!c)
                            break;

                        if (!started) {
                            if (IS_IN_GRAMMAR(c))
                                started = true;
                        }
                        if (c == '}') {                             // BUG !!!
                            expect_end = 1;
                            data++;
                            te = 0;
                            break;
                        }

                        if (started) {
                            if ((c != ' ') || (c != '\t') || (c != '\r') || (c != '\n') || (c != '\b'))
                                fname += c;
                        }

                        data++;
                    }

                    FIN = fopen((char *)fname.c_str(), "rb");
                    if (FIN) {
                        fseek(FIN, 0, SEEK_END);
                        long size = (long)ftell(FIN);
                        fseek(FIN, 0, SEEK_SET);
                        if (size > 0) {
                            char *buffer = new char[size + 1];
                            buffer[size] = 0;
                            if (fread(buffer, 1, size, FIN) == size) {
                                AnsiString     t_filename = this->filename;
                                unsigned short t_line     = this->line;
                                filename = fname;
                                line     = 1;
                                Compile(buffer, Owner /*, EO*/);
                                this->line     = t_line;
                                this->filename = t_filename;
                            } else {
                                errors += "Cannot read from included file: ";
                                errors += fname;
                                ADD_INFO_ERROR(errors);
                            }
                            delete[] buffer;
                        }
                        fclose(FIN);
                    } else {
                        errors += "Cannot open included file: ";
                        errors += fname;
                        ADD_INFO_ERROR(errors);
                    }
                } else {
                    index = FindVariable(element);
                    if (index > -1) {
                        te            = new CTemplateElement(VARIABLE, line, filename);
                        te->VAR_INDEX = index;
                        Owner->Add(te, DATA_ELEMENT);
                        expect_end = 1;
                        te2        = te;
                        te         = 0;
                    } else {
                        errors += "Unbound variable: ";
                        errors += element;
                        ADD_INFO_ERROR(errors);
                        te         = 0;
                        expect_end = 1;
                    }
                }
                //break;
            } else {
                if (expect_member) {
                    AnsiString *S = new AnsiString((char *)".");
                    *S += element;
                    if (!te->members_chain)
                        te->members_chain = new AnsiList();
                    te->members_chain->Add(S, DATA_STRING);
                    te2           = te;
                    expect_member = 0;
                } else
                if (expect_key) {
                    AnsiString *S = new AnsiString((char *)"[");
                    *S += element;
                    if (!te->members_chain)
                        te->members_chain = new AnsiList();
                    te->members_chain->Add(S, DATA_STRING);
                    te2        = te;
                    expect_key = 0;
                    if (c != ']') {
                        errors += "Array missing ']'";
                        ADD_INFO_ERROR(errors);
                    }
                } else
                if (expect_modifier) {
                    if (element == (char *)"count") {
                        te->modifier = LENGTH;
                    } else
                    if (element == (char *)"key") {
                        te->modifier = KEY;
                    } else
                    if (element == (char *)"index") {
                        te->modifier = INDEX;
                    } else
                    if (element == (char *)"day") {
                        te->modifier = DAY;
                    } else
                    if (element == (char *)"month") {
                        te->modifier = MONTH;
                    } else
                    if (element == (char *)"year") {
                        te->modifier = YEAR;
                    } else
                    if (element == (char *)"hour") {
                        te->modifier = HOUR;
                    } else
                    if (element == (char *)"min") {
                        te->modifier = MINUTE;
                    } else
                    if (element == (char *)"sec") {
                        te->modifier = KEY;
                    } else
                    if (element == (char *)"wday") {
                        te->modifier = DAYOFWEEK;
                    } else
                    if (element == (char *)"dst") {
                        te->modifier = DST;
                    } else
                    if (element == (char *)"limit") {
                        te->modifier = LIMIT;
                    } else {
                        errors += "Unknown modifier '";
                        errors += element;
                        ADD_INFO_ERROR(errors);
                    }
                    te = 0;
                } else {
                    switch (te->Type) {
                        case FOREACH:
                        case IF:
                            index      = FindVariable(element);
                            expect_end = 1;
                            if (index > -1) {
                                te->VAR_INDEX = index;
                                //-------------------//
                                if (IS_MODIFIER(c)) {
                                    expect_end = 1;
                                    te2        = te;
                                    te         = 0;
                                }
                                //-------------------//
                            } else {
                                errors += "Unbound variable: ";
                                errors += element;
                                ADD_INFO_ERROR(errors);
                                te         = 0;
                                expect_end = 1;
                            }
                            break;

                        default:
                            // error !!!
                            break;
                    }
                }
            }
            element = (char *)"";
        }

        if (te2) {
            if (c == ':') {
                expect_modifier = 1;
                expect_end      = 0;
                te = te2;
            } else
            if (c == '.') {
                expect_member = 1;
                expect_end    = 0;
                te            = te2;
            } else
            if (c == '[') {
                expect_key = 1;
                expect_end = 0;
                te         = te2;
            } else
            if (c == '=') {
                if (peek_data == '=')
                    data++;

                te2->comparator = COMPARATOR_EQU;
                data            = GetComparator(data, te2);
            } else
            if (c == '>') {
                if (peek_data == '=') {
                    te2->comparator = COMPARATOR_GEQ;
                    data++;
                } else
                    te2->comparator = COMPARATOR_GRE;
                data = GetComparator(data, te2);
            } else
            if (c == '<') {
                if (peek_data == '=') {
                    te2->comparator = COMPARATOR_GEQ;
                    data++;
                } else
                if (peek_data == '>') {
                    data++;
                    te2->comparator = COMPARATOR_NEQ;
                    data            = GetComparator(data, te2);
                } else
                    te2->comparator = COMPARATOR_GRE;
                data = GetComparator(data, te2);
            } else
            if ((c == '!') && (peek_data == '=')) {
                if (peek_data)
                    data++;
                te2->comparator = COMPARATOR_NEQ;
                data            = GetComparator(data, te2);
            }
            te2 = 0;
        }
        if (te) {
            if (c == '=') {
                if (peek_data == '=')
                    data++;

                te->comparator = COMPARATOR_EQU;
                data           = GetComparator(data, te);
            } else
            if (c == '>') {
                if (peek_data == '=') {
                    te->comparator = COMPARATOR_GEQ;
                    data++;
                } else
                    te->comparator = COMPARATOR_GRE;
                data = GetComparator(data, te);
            } else
            if (c == '<') {
                if (peek_data == '=') {
                    te->comparator = COMPARATOR_GEQ;
                    data++;
                } else
                if (peek_data == '>') {
                    data++;
                    te->comparator = COMPARATOR_NEQ;
                    data           = GetComparator(data, te);
                } else
                    te->comparator = COMPARATOR_GRE;
                data = GetComparator(data, te);
            } else
            if ((c == '!') && (peek_data == '=')) {
                if (peek_data)
                    data++;
                te->comparator = COMPARATOR_NEQ;
                data           = GetComparator(data, te);
            }
        }
        if (c == '}')
            break;
        data++;
    }
    if (te) {
        switch (te->Type) {
            case FOREACH:
            case IF:
                data++;
                data    = (char *)Compile(data, te->BlockTrue, te);
                go_back = 0;
                break;
        }
    } else
    if (go_back == -1) {
        data++;
        go_back = 0;
        data    = (char *)Compile(data, EO->BlockFalse, EO);
        go_back = 1;
    } else
    if (is_literal) {
        data++;
        data = (char *)Literal(data, Owner);
    }

    return data;
}

char *CTemplatizer::GetComparator(char *data, CTemplateElement *te) {
    AnsiString element;
    char       quote = 0;

    data++;
    while (char c = *data) {
        if ((c == '\"') || (c == '\'')) {
            data++;
            if (quote != c) {
                quote = c;
                continue;
            } else {
                //quote=0;
                data--;
                break;
            }
        }


        if ((quote) || (IS_IN_GRAMMAR(c)) || (c == '-') || (c == '+'))
            element += c;
        else
        if (c == '}') {
            data--;
            break;
        }
        data++;
    }
    if (quote) {
        te->CompareType = TPL_COMPARE_STRING;
        te->szCompareTo = element;
    } else {
        te->CompareType = TPL_COMPARE_NUMBER;
        te->nCompareTo  = element.ToFloat();
    }
    return data;
}

long CTemplatizer::Compile(char *data, AnsiList *Owner, CTemplateElement *EO) {
    if ((!data) || (!data[0]))
        return -1;

    /*int len=strlen(data);
       if (!len)
        return -1;*/

    if (!Owner) {
        go_back  = 0;
        errors   = (char *)"";
        Owner    = &Template;
        line     = 1;
        filename = (char *)"";
    }
    CTemplateElement *te = new CTemplateElement(HTML, line, filename);
    Owner->Add(te, DATA_ELEMENT);
    while (*data) {
        char c = *data;
        switch (c) {
            case '{':
                data = ParseElement(data, Owner, EO);
                if (go_back)
                    return (intptr_t)data;
                te = new CTemplateElement(HTML, line, filename);
                Owner->Add(te, DATA_ELEMENT);
                break;

            case '}':
                errors += "Unexpected '}'";
                ADD_INFO_ERROR(errors);
                break;

            case '\n':
                line++;

            //break;
            default:
                te->HTML_DATA += c;
        }
        if ((!data) || (!data[0]))
            break;
        data++;
    }
    if (EO) {
        switch (EO->Type) {
            case FOREACH:
                errors += "'endfor' missing";
                ADD_INFO_ERROR_2(errors, EO->line, EO->filename);
                break;

            case IF:
                errors += "'endif' missing";
                ADD_INFO_ERROR_2(errors, EO->line, EO->filename);
                break;
        }
    }
    return 0;
}

#define GET_VARIABLE(result, v_index, modifier, number, members_chain, te)       \
    {                                                                            \
        number = false;                                                          \
        TemplateVariable *TV = (TemplateVariable *)Variables.Item(v_index);      \
        switch (modifier) {                                                      \
            case LENGTH:                                                         \
                if (TV != TV_FOR)                                                \
                    result += AnsiString((long)arr_count);                       \
                else                                                             \
                    result += AnsiString((long)TV->GetArrayCount());             \
                number = true;                                                   \
                break;                                                           \
            case INDEX:                                                          \
                result += AnsiString((long)TV->index);                           \
                number  = true;                                                  \
                break;                                                           \
            case KEY:                                                            \
                result += TV->GetArrayKey().c_str();                             \
                break;                                                           \
            case NONE:                                                           \
                {                                                                \
                    switch (TV->QueryType(members_chain, &errors)) {             \
                        case VARIABLE_STRING:                                    \
                            result += TV->GetString();                           \
                            break;                                               \
                        case VARIABLE_NUMBER:                                    \
                            result += AnsiString(TV->GetNumber());               \
                            number  = true;                                      \
                            break;                                               \
                        case VARIABLE_ARRAY:                                     \
                            result += AnsiString(TV->GetArrayCount());           \
                            number  = true;                                      \
                            break;                                               \
                        case VARIABLE_CLASS:                                     \
                            result += (char *)"1";                               \
                            number  = true;                                      \
                            break;                                               \
                        case 0:                                                  \
                            break;                                               \
                        default:                                                 \
                            errors += "Unsupported data type for variable '";    \
                            errors += TV->name;                                  \
                            errors += "'";                                       \
                            ADD_INFO_ERROR(errors);                              \
                            break;                                               \
                    }                                                            \
                }                                                                \
                break;                                                           \
            case DAY:                                                            \
                if (TV->QueryType(members_chain, &errors) == VARIABLE_NUMBER) {  \
                    time_t    tt       = (time_t)TV->GetNumber();                \
                    struct tm *newtime = localtime(&tt);                         \
                    result += AnsiString((long)newtime->tm_mday);                \
                } else {                                                         \
                    errors += "Unsupported modifier (DAY) for this data type";   \
                    ADD_INFO_ERROR(errors);                                      \
                }                                                                \
                break;                                                           \
            case MONTH:                                                          \
                if (TV->QueryType(members_chain, &errors) == VARIABLE_NUMBER) {  \
                    time_t    tt       = (time_t)TV->GetNumber();                \
                    struct tm *newtime = localtime(&tt);                         \
                    result += AnsiString((long)newtime->tm_mon + 1);             \
                } else {                                                         \
                    errors += "Unsupported modifier (MONTH) for this data type"; \
                    ADD_INFO_ERROR(errors);                                      \
                }                                                                \
                break;                                                           \
            case YEAR:                                                           \
                if (TV->QueryType(members_chain, &errors) == VARIABLE_NUMBER) {  \
                    time_t    tt       = (time_t)TV->GetNumber();                \
                    struct tm *newtime = localtime(&tt);                         \
                    result += AnsiString((long)newtime->tm_year);                \
                } else {                                                         \
                    errors += "Unsupported modifier (YEAR) for this data type";  \
                    ADD_INFO_ERROR(errors);                                      \
                }                                                                \
                break;                                                           \
            case HOUR:                                                           \
                if (TV->QueryType(members_chain, &errors) == VARIABLE_NUMBER) {  \
                    time_t    tt       = (time_t)TV->GetNumber();                \
                    struct tm *newtime = localtime(&tt);                         \
                    result += AnsiString((long)newtime->tm_hour);                \
                } else {                                                         \
                    errors += "Unsupported modifier (HOUR) for this data type";  \
                    ADD_INFO_ERROR(errors);                                      \
                }                                                                \
                break;                                                           \
            case MINUTE:                                                         \
                if (TV->QueryType(members_chain, &errors) == VARIABLE_NUMBER) {  \
                    time_t    tt       = (time_t)TV->GetNumber();                \
                    struct tm *newtime = localtime(&tt);                         \
                    result += AnsiString((long)newtime->tm_min);                 \
                } else {                                                         \
                    errors += "Unsupported modifier (MIN) for this data type";   \
                    ADD_INFO_ERROR(errors);                                      \
                }                                                                \
                break;                                                           \
            case SECOND:                                                         \
                if (TV->QueryType(members_chain, &errors) == VARIABLE_NUMBER) {  \
                    time_t    tt       = (time_t)TV->GetNumber();                \
                    struct tm *newtime = localtime(&tt);                         \
                    result += AnsiString((long)newtime->tm_sec);                 \
                } else {                                                         \
                    errors += "Unsupported modifier (SEC) for this data type";   \
                    ADD_INFO_ERROR(errors);                                      \
                }                                                                \
                break;                                                           \
            case DAYOFWEEK:                                                      \
                if (TV->QueryType(members_chain, &errors) == VARIABLE_NUMBER) {  \
                    time_t    tt       = (time_t)TV->GetNumber();                \
                    struct tm *newtime = localtime(&tt);                         \
                    result += AnsiString((long)newtime->tm_wday);                \
                } else {                                                         \
                    errors += "Unsupported modifier (WDAY) for this data type";  \
                    ADD_INFO_ERROR(errors);                                      \
                }                                                                \
                break;                                                           \
            case DST:                                                            \
                if (TV->QueryType(members_chain, &errors) == VARIABLE_NUMBER) {  \
                    time_t    tt       = (time_t)TV->GetNumber();                \
                    struct tm *newtime = localtime(&tt);                         \
                    result += AnsiString((long)newtime->tm_isdst);               \
                } else {                                                         \
                    errors += "Unsupported modifier (DST) for this data type";   \
                    ADD_INFO_ERROR(errors);                                      \
                }                                                                \
                break;                                                           \
        }                                                                        \
    }                                                                            \
    switch (te->CompareType) {                                                   \
        case TPL_COMPARE_STRING:                                                 \
            switch (te->comparator) {                                            \
                case COMPARATOR_EQU:                                             \
                    result = (result == te->szCompareTo);                        \
                    break;                                                       \
                case COMPARATOR_LES:                                             \
                    result = (result < te->szCompareTo);                         \
                    break;                                                       \
                case COMPARATOR_GRE:                                             \
                    result = (result > te->szCompareTo);                         \
                    break;                                                       \
                case COMPARATOR_LEQ:                                             \
                    result = (result <= te->szCompareTo);                        \
                    break;                                                       \
                case COMPARATOR_GEQ:                                             \
                    result = (result >= te->szCompareTo);                        \
                    break;                                                       \
                case COMPARATOR_NEQ:                                             \
                    result = (result != te->szCompareTo);                        \
                    break;                                                       \
            }                                                                    \
            is_number = 1;                                                       \
            break;                                                               \
        case TPL_COMPARE_NUMBER:                                                 \
            switch (te->comparator) {                                            \
                case COMPARATOR_EQU:                                             \
                    result = (result.ToFloat() == te->nCompareTo);               \
                    break;                                                       \
                case COMPARATOR_LES:                                             \
                    result = (result.ToFloat() < te->nCompareTo);                \
                    break;                                                       \
                case COMPARATOR_GRE:                                             \
                    result = (result.ToFloat() > te->nCompareTo);                \
                    break;                                                       \
                case COMPARATOR_LEQ:                                             \
                    result = (result.ToFloat() <= te->nCompareTo);               \
                    break;                                                       \
                case COMPARATOR_GEQ:                                             \
                    result = (result.ToFloat() >= te->nCompareTo);               \
                    break;                                                       \
                case COMPARATOR_NEQ:                                             \
                    result = (result.ToFloat() != te->nCompareTo);               \
                    break;                                                       \
            }                                                                    \
            is_number = 1;                                                       \
            break;                                                               \
        case TPL_COMPARE_CONSTANT:                                               \
            is_number = 1;                                                       \
            break;                                                               \
    }

AnsiString CTemplatizer::Execute(AnsiList *Target, CTemplateElement *Owner) {
    if (!Target)
        Target = &Template;
    int              count = Target->Count();
    AnsiString       result;
    int              base_ptr  = 0;
    int              arr_count = 0;
    TemplateVariable *TV_FOR   = 0;
    signed char      _no_end   = 0;
    bool             is_number;
    int              old_index   = 0;
    void             *data       = 0;
    void             *work_data  = 0;
    void             *popped_for = 0;
    if (Owner) {
        if (Owner->Type == FOREACH) {
            TV_FOR = (TemplateVariable *)Variables.Item(Owner->VAR_INDEX);
            if (TV_FOR->QueryType(Owner->members_chain, &errors) == VARIABLE_ARRAY) {
                if (TV_FOR->in_for) {
                    old_index  = TV_FOR->index;
                    data       = TV_FOR->data;
                    work_data  = TV_FOR->work_variable;
                    popped_for = TV_FOR->popped_for;
                    TV_FOR->PopData(Owner->members_chain);
                    arr_count = TV_FOR->GetArrayCount();
                    if (arr_count <= 0) {
                        TV_FOR->data  = data;
                        TV_FOR->index = old_index;
                        return result;
                    }
                } else {
                    arr_count = TV_FOR->GetArrayCount();
                    if (arr_count <= 0)
                        return result;
                }
                TV_FOR->BeginFor();
            } else
                _no_end = 1;
        }
    }

    for (int i = 0; i < count; i++) {
        CTemplateElement *te = (CTemplateElement *)Target->Item(i);
        switch (te->Type) {
            case HTML:
                result += te->HTML_DATA;
                break;

            case VARIABLE:
                GET_VARIABLE(result, te->VAR_INDEX, te->modifier, is_number, te->members_chain, te);
                break;

            case FOREACH:
                result += Execute(te->BlockTrue, te);
                break;

            case IF:
                {
                    AnsiString value;
                    GET_VARIABLE(value, te->VAR_INDEX, te->modifier, is_number, te->members_chain, te);
                    if (((is_number) && (value.ToInt())) || ((!is_number) && (value.Length()))) {
                        result += Execute(te->BlockTrue, te);
                    } else if (te->BlockFalse) {
                        result += Execute(te->BlockFalse, te);
                    }
                }
                break;

            default:
                break;
        }
        if ((Owner) && (i == count - 1)) {
            if (Owner->Type == FOREACH) {
                if ((++base_ptr < arr_count)) {
                    i = -1;
                    TV_FOR->Next();
                } else if (!_no_end) {
                    TV_FOR->EndFor();
                }
            }
        }
    }
    if (data) {
        TV_FOR->data          = data;
        TV_FOR->index         = old_index;
        TV_FOR->in_for        = true;
        TV_FOR->work_variable = work_data;
        TV_FOR->popped_for    = popped_for;
    }
    return result;
}

AnsiString CTemplatizer::GetErrors() {
    return errors;
}
