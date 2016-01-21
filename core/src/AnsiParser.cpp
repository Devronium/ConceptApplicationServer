#include "AnsiParser.h"
#include <string.h>

POOLED_IMPLEMENTATION(AnsiParser)

#define IS_QUOTE(c)    (c == '"') || (c == '\'')

AnsiParser::AnsiParser(AnsiString *TRG, AnsiList *Constants) {
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

void AnsiParser::NextAtom(AnsiString& result, int no_constants) {
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
    intptr_t i;
    char     in_var = 0;
    char     c      = 0;

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

        if ((!quote) && (!comment) && (!line_comment)) {
            if (c == '#') {
                if (!comment) {
                    line_comment = 1;
                    result       = (char *)"";
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
                        result       = (char *)"";
                        oper         = 0;
                        separator    = 0;
                    }
                } else
                if PRED_EQU(prediction, COMMENT_START) {
                    comment   = 1;
                    result    = (char *)"";
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
                    result    = (char *)"";
                    oper      = 0;
                    separator = 0;
                } else
                if (comment) {
                    if (c == COMMENT_END [0]) {
                        result = c;
                    } else {
                        result = (char *)"";
                    }
                }
            } else
            if (comment) {
                if (c == COMMENT_END [0]) {
                    result = c;
                } else {
                    result = (char *)"";
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
                break;
            }

            if ((!quote) && ((c == STATAMENT_SEPARATOR))) {
                if (!result.Length()) {
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
                    } else if (quote == c) {
                        quote = 0;
                        i++;
                        break;
                    }
                } else
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
                        if ((!oper) && (Contains(ATOM_MEMBER, c))) {
                            if (!oper) {
                                result += c;
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

    if ((!result.Length()) && (position < len)) {
        result += c;
        position++;
        return;
    }

    if ((ConstantList) && (!no_constants)) {
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

