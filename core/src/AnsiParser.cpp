#include "AnsiParser.h"
#include <string.h>

POOLED_IMPLEMENTATION(AnsiParser)

#define IS_QUOTE(c)    (c == '"') || (c == '\'')

AnsiParser::AnsiParser(AnsiString *TRG, ConstantMapType *Constants) {
    position           = 0;
    rollback_position  = -1;
    rollback_line      = 0;
    Target             = TRG;
    linia_curenta      = 1;
    ConstantList       = Constants;
    has_virtual_result = 0;
    put_a_quote        = 0;
    in_formatted       = 0;
    in_expr            = 0;
    is_end             = 0;
    put_prefix         = 0;
    regexp_flags       = 0;
    int len = Target->Length();
    if (len >= 3) {
        unsigned char *s = (unsigned char *)Target->c_str();
        if ((s[0] == 0xEF) && (s[1] == 0xBB) && (s[2] == 0xBF))
            position = 3;
    }
}

AnsiParser::~AnsiParser(void) {
}

void AnsiParser::Atomize() {
    position      = 0;
    linia_curenta = 1;
    int len = Target->Length();
    if (len >= 3) {
        unsigned char *s = (unsigned char *)Target->c_str();
        if ((s[0] == 0xEF) && (s[1] == 0xBB) && (s[2] == 0xBF))
            position = 3;
    }
}

AnsiString AnsiParser::GetConstant() {
    AnsiString result = NULL_STRING;
    intptr_t   len    = Target->Length();
    int        i;
    int        empty = 1;

    for (i = position; i < len; i++) {
        char c = (*Target) [i];

        if (c == '\r') {
            continue;
        }
        if (c == '\n') {
            break;
        } else
        if (empty) {
            if ((c != ' ') && (c != '\t') && (c != '\b') && (c != '\r')) {
                result += c;
                empty   = 0;
            }
        } else {
            result += c;
        }
    }
    position = i;
    return result;
}

bool AnsiParser::Done() {
    if (!Target) {
        return true;
    }
    if (position >= Target->Length()) {
        return true;
    } else {
        return false;
    }
}

#define PRED_EQU(a, b)    (((a)[0] == (b)[0]) && ((a)[1] == (b)[1]) ? 1 : 0)

void AnsiParser::NextAtom(AnsiString& result, int no_constants, int TYPE, int ID) {
    uintptr_t len = Target->Length();

    result = NULL_STRING;
    int      separator    = 0;
    int      escape       = 0;
    int      oper         = 0;
    char     quote        = 0;
    char     next_close   = 0;
    int      line_comment = 0;
    int      comment      = 0;
    int      only_numbers = 1;
#ifndef NO_BUILTIN_REGEX
    char     can_regex    = 0;
    int      in_regex     = 0;
#endif
    uintptr_t i;
    char     in_var = 0;
    char     c      = 0;
    regexp_flags    = 0;
    if (!Target) {
        return;
    }
    if (has_virtual_result) {
        if (put_prefix) {
            put_prefix = 0;
            if (is_end) {
                result = ")";
                return;
            } else {
                result         = virtual_result;
                virtual_result = "(";
                return;
            }
        }
        has_virtual_result = 0;
        result             = virtual_result;
        return;
    }
    
    rollback_position = position;
    rollback_line = linia_curenta;

    char *str_ptr = Target->c_str();
    if (put_a_quote) {
        quote       = '"';
        result      = '"';
        put_a_quote = 0;
    } else {
#ifndef NO_BUILTIN_REGEX
        switch (TYPE) {
            case TYPE_OPERATOR:
                switch (ID) {
                    case KEY_INDEX_CLOSE:
                    case KEY_P_CLOSE:
                    case KEY_END:
                        break;
                    default:
                        can_regex = 1;
                }
                break;
            case TYPE_KEYWORD:
            case TYPE_SEPARATOR:
                can_regex = 1;
                break;
        }
#endif
    }
    char prediction[2];
    for (i = position; i < len; i++) {
        separator = 0;
        c         = str_ptr [i];

        if (c == NEW_LINE) {
            linia_curenta++;
            line_comment = 0;
        }
        if (((c == '\n') || (c == '\r')) && (quote) && (!comment)) {
            i++;
            break;
        }

        unsigned int seq_len = 1;
        if ((!quote) && (!comment) && (!line_comment)) {
            unsigned char utf8_char = (unsigned char)c;
            if (utf8_char & 0x80) {
                // utf8 sequence
                if ((utf8_char & 0xF0) == 0xF0)
                    seq_len = 4;
                else
                if ((utf8_char & 0xE0) == 0xE0)
                    seq_len = 3;
                else
                if ((utf8_char & 0xC0) == 0xC0)
                    seq_len = 2;

                if (seq_len > 1) {
                    // invalid utf8
                    if (i + seq_len > len) {
                        seq_len = 1;
                    } else
                    if ((!comment) && (!quote)) {
                        for (unsigned int j = 1; j < seq_len; j++) {
                            utf8_char = (unsigned char)str_ptr [i + j];
                            if ((utf8_char >> 6) != 0x02) {
                                // invalid utf8
                                seq_len = 1;
                                break;
                            }
                        }
                    }
                }
            }
            if (c == '#') {
                if (!comment) {
                    line_comment = 1;
                    result       = "";
                    oper         = 0;
                    separator    = 0;
                }
            }

            if ((c == '$') && (in_formatted)) {
                virtual_result     = "+";
                has_virtual_result = 1;
                is_end             = 1;
                put_a_quote        = 1;
                in_formatted       = 0;
                in_var             = 1;
                is_end             = 1;
                put_prefix         = 1;
                continue;
            } else
            if ((c == '{') && (in_var)) {
                in_expr            = 1;
                in_var             = 1;
                has_virtual_result = 0;
                put_a_quote        = 0;
                in_formatted       = 0;
                in_var             = 0;
                is_end             = 0;
                put_prefix         = 1;
                continue;
            } else
            if ((c == '}') && (in_expr)) {
                in_expr = 0;
                i++;
                virtual_result     = "+";
                has_virtual_result = 1;
                put_a_quote        = 1;
                in_formatted       = 0;
                in_var             = 1;
                is_end             = 1;
                if (!result.Length()) {
                    position = i;
                    NextAtom(result);
                    return;
                }
                break;
            }

            if (c == '.') {
                if (!only_numbers) {
                    break;
                } else
                if (i == position) {
                    result = c;
                    i++;
                    break;
                }
            }

            if ((only_numbers) && ((c < '0') || (c > '9'))) {
                only_numbers = 0;
            }
        }

        if (result.Length() == 1) {
            prediction[0] = result[0];
            prediction[1] = c;
        } else
            prediction[0] = 0;

        if (!quote) {
            if (prediction[0]) {
                if PRED_EQU(prediction, LINE_COMMENT) {
                    if (!comment) {
                        line_comment = 1;
                        result       = "";
                        oper         = 0;
                        separator    = 0;
                    }
                } else
                if PRED_EQU(prediction, COMMENT_START) {
                    comment   = 1;
                    result    = "";
                    oper      = 0;
                    separator = 0;
                } else
                if PRED_EQU(prediction, COMMENT_END) {
                    if (!comment) {
                        result    = (char *)COMMENT_END;
                        position += 2;
                        return;
                    }
                    comment   = 0;
                    result    = "";
                    oper      = 0;
                    separator = 0;
                } else
                if (comment) {
                    if (c == COMMENT_END [0]) {
                        result = c;
                    } else {
                        result = "";
                    }
                } 
#ifndef NO_BUILTIN_REGEX
                else
                if ((can_regex) && (prediction[0] == '/') && (prediction[1] != '*') && (prediction[1] != '*')) {
                    in_regex = 1;
                    quote = '/';
                    oper = 0;
                    no_constants = 1;
                }
#endif
            } else
            if (comment) {
                if (c == COMMENT_END [0]) {
                    result = c;
                } else {
                    result = "";
                }
            }
        }

        if ((!line_comment) && (!comment) && ((!prediction[0]) || (!PRED_EQU(prediction, COMMENT_END)))) {
            if ((!quote) && ((c == ST_BEGIN) || (c == ST_END))) {
                if (!result.Length()) {
                    result = c;
                    i++;
                }
                break;
            }

            if ((!quote) && (Contains(OPERATORS, c))) {
                if ((oper) || ((!result.Length()) && (!oper))) {
                    oper = 1;
                } else {
                    break;
                }
            } else if (oper) {
                no_constants = 1;
                break;
            }

            if ((!quote) && ((c == STATAMENT_SEPARATOR))) {
                if (!result.Length()) {
                    no_constants = 1;
                    result = c;
                    i++;
                }
                break;
            }
            if ((c == ESCAPE_CHARACTER) && (quote) && (!escape)) {
                escape  = 1;
                result += c;
            } else {
                if ((!oper) && (!escape) && (IS_QUOTE(c))) {
                    if ((!quote) && (result.Length())) {
                        break;
                    }
                    result += c;
                    if (!quote) {
                        quote = c;
                        no_constants = 1;
                    } else if (quote == c) {
                        quote = 0;
                        i++;
                        break;
                    }
                } else
#ifndef NO_BUILTIN_REGEX
                if ((in_regex) && (c == '/')) {
                    result += c;
                    no_constants = 1;
                    quote = 0;
                    i++;
                    for (unsigned int j = i; j < len; j++) {
                        char modifier = str_ptr [j];
                        if ((modifier != 'i') && (modifier != 'm') && (modifier != 'g'))
                            break;
                        switch (modifier) {
                            case 'i':
                                regexp_flags |= 1;
                                i++;
                                break;
                            case 'm':
                                regexp_flags |= 2;
                                i++;
                                break;
                            case 'g':
                                regexp_flags |= 8;
                                i++;
                                break;
                        }
                    }
                    break;
                } else
#endif
                if (!quote) {
                    if ((!oper) && (Contains(SEPARATORS, c))) {
                        if (result.Length()) {
                            break;
                        } else {
                            only_numbers = 1;
                            separator    = 1;
                        }
                    }
                    if (!separator) {
                        if ((!oper) && ((seq_len > 1) || (Contains(ATOM_MEMBER, c)))) {
                            if (!oper) {
                                result += c;
                                for (unsigned int j = 1; j < seq_len; j++)
                                    result += str_ptr [i + j];
                                i += seq_len - 1;
                            } else {
                                break;
                            }
                        } else {
                            if (!oper) {
                                break;
                            } else {
                                if (oper) {
                                    if ((!result.Length()) && (c == C_SPECIAL)) {
                                        i++;
                                        result = C_SPECIAL;
                                        break;
                                    }
                                    AnsiString tmp(result);
                                    tmp += c;

                                    INTEGER id = GetID(tmp);
                                    if ((id <= __START_OPERATORS) || (id >= __END_OPERATORS)) {
                                        if ((c == '?') && (!result.Length()) && (i + 1 < len) && (str_ptr [i + 1] == '?')) {
                                            result += c;
                                            continue;
                                        }
                                        break;
                                    } else {
                                        result += c;
                                    }
                                } else {
                                    result += c;
                                }
                            }
                        }
                    }
                } else {
                    if ((quote == '"') && (c == '$') && (!escape)) {
                        virtual_result     = "+";
                        has_virtual_result = 1;
                        is_end             = 0;
                        put_prefix         = 1;
                        result            += quote;
                        in_formatted       = 1;
                        break;
                    }

                    result += c;
                    if (escape) {
                        escape = 0;
                    }
                }
            }
        }
    }
    intptr_t old_position = position;
    position = i + (c == NEW_LINE);

    if ((!result.Length()) && (position < (intptr_t)len)) {
        result += c;
        position++;
        return;
    }
    if ((!no_constants) && (result.Length() >= 1)) {
        char first_char = result[0];
        if ((first_char == '{') || (first_char == '{') || (first_char == '-') || (first_char == '.') || 
            (first_char == '(') || (first_char == ')') || (first_char == '[') || (first_char == ']') || 
            ((first_char >= '0') && (first_char <= '9')))
             no_constants = 1;
    }
    if ((ConstantList) && (!no_constants)) {
#ifdef STDMAP_CONSTANTS
        VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)ConstantList->Find(result.c_str());
        if ((VD) && (VD->name == result)) {
            int tmp_len = VD->value.Length();

            if (position > tmp_len) {
                // not pretty, but much faster for large files
                position -= tmp_len;
                position--;
                memcpy(str_ptr + position, VD->value.c_str(), tmp_len);
                str_ptr[position + tmp_len] = ' ';
            } else {
                AnsiString tmp_value;
                tmp_value.LoadBuffer(VD->value.c_str(), tmp_len);
                (*Target) = tmp_value + " " + (Target->c_str() + position);
                position  = 0;
            }
            NextAtom(result);
            return;
        }

#else
        INTEGER Count = ConstantList->Count();
        INTEGER POS;
        do {
            POS = 0;
            for (INTEGER i = 0; i < Count; i++) {
                VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)(*ConstantList) [i];
                if (VD->name == result) {
                    int tmp_len = VD->value.Length();

                    if (position > tmp_len) {
                        // not pretty, but much faster for large files
                        position -= tmp_len;
                        position--;
                        memcpy(str_ptr + position, VD->value.c_str(), tmp_len);
                        str_ptr[position + tmp_len] = ' ';
                    } else {
                        AnsiString tmp_value;
                        tmp_value.LoadBuffer(VD->value.c_str(), tmp_len);
                        (*Target) = tmp_value + " " + (Target->c_str() + position);
                        position  = 0;
                    }
                    NextAtom(result);
                    return;
                }
            }
        } while (POS);
#endif
    }
}

bool AnsiParser::Rollback() {
    if (has_virtual_result)
        return false;

    if (rollback_position >= 0) {
        linia_curenta = rollback_line;
        position = rollback_position;
        return true;
    }
    return false;
}

intptr_t AnsiParser::LastLine() {
    return linia_curenta;
}

