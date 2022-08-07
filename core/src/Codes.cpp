#include "Codes.h"
#include <string.h>
#include <time.h>
#include "dtoa.h"

INTEGER GetOperatorType(INTEGER ID) {
    switch (ID) {
        // selectors
        case KEY_INDEX_OPEN:
        case KEY_INDEX_CLOSE:
        case KEY_P_OPEN:
        case KEY_SELECTOR_P:
        case KEY_SELECTOR_I:
        case KEY_P_CLOSE:
            return OPERATOR_SELECTOR;

        // operators
        case KEY_NEW:
        case KEY_DELETE:
        case KEY_TYPE_OF:
        case KEY_CLASS_NAME:
        case KEY_LENGTH:
        case KEY_VALUE:
        case KEY_NOT:
        case KEY_COM:
        case KEY_INC:
        case KEY_DEC:
        case KEY_POZ:
        case KEY_NEG:
        case KEY_AWAIT:
            return OPERATOR_UNARY;

        case KEY_INC_LEFT:
        case KEY_DEC_LEFT:
            return OPERATOR_UNARY_LEFT;

        case KEY_DLL_CALL:
        case KEY_SEL:

        case KEY_MUL:
        case KEY_DIV:
        case KEY_REM:
        case KEY_SUM:
        case KEY_DIF:
        case KEY_SHL:
        case KEY_SHR:
        case KEY_LES:
        case KEY_LEQ:
        case KEY_GRE:
        case KEY_GEQ:
        case KEY_EQU:
        case KEY_NEQ:
        case KEY_AND:
        case KEY_XOR:
        case KEY_OR:
        case KEY_BAN:
        case KEY_BOR:
        case KEY_ASG:
        case KEY_BY_REF:
        case KEY_ASU:
        case KEY_ADI:
        case KEY_AMU:
        case KEY_ADV:
        case KEY_ARE:
        case KEY_AAN:
        case KEY_AXO:
        case KEY_AOR:
        case KEY_ASL:
        case KEY_CND_NULL:
        case KEY_ASR:
            return OPERATOR_BINARY;

        case KEY_CND_2:
            return OPERATOR_RESOLUTION;

        case KEY_COMMA:
            return KEY_COMMA;

        case KEY_ARR_KEY:
            return KEY_ARR_KEY;
    }
    return 0;
}

const char *GetKeyWord(INTEGER ID) {
    switch (ID) {
        case KEY_BEGIN:
            return BEGIN;

        case KEY_END:
            return END;

        case KEY_IF:
            return IF;

        case KEY_ELSE:
            return ELSE;

        case KEY_FOR:
            return FOR;

        case KEY_WHILE:
            return WHILE;

        case KEY_DO:
            return DO;

        case KEY_ECHO:
            return ECHO;

        case KEY_RETURN:
            return RETURN;

        case KEY_TRY:
            return TRY;

        case KEY_CATCH:
            return CATCH;

        case KEY_THROW:
            return THROW;

        case KEY_BREAK:
            return BREAK;

        case KEY_CONTINUE:
            return CONTINUE;

        case KEY_NEW:
            return NEW;

        case KEY_DELETE:
            return M_DELETE;

        case KEY_SEP:
            return SEP;

        case KEY_CLASS:
            return CLASS;

        case KEY_FUNCTION:
            return FUNCTION;

        case KEY_PRIVATE:
            return PRIVATE;

        case KEY_PUBLIC:
            return PUBLIC;

#ifndef DISABLE_DEPRECATED_FEATURES
        case KEY_CEVENT:
            return CEVENT;
#endif

        case KEY_PROPERTY:
            return PROPERTY;

        case KEY_SET:
            return SET;

        case KEY_GET:
            return GET;

        case KEY_EXTENDS:
            return EXTENDS;

        case KEY_INCLUDE:
            return INCLUDE;

        case KEY_IMPORT:
            return IMPORT;

        case KEY_IN:
            return C_IN;
#ifndef DISABLE_DEPRECATED_FEATURES
        case KEY_TRIGERS:
            return TRIGERS;
#endif
        case KEY_VAR:
            return VAR;

        case KEY_QUOTE1:
            return QUOTE1;

        case KEY_QUOTE2:
            return QUOTE2;

        case KEY_OPERATOR:
            return M_OPERATOR;

        case KEY_P_OPEN:
            return P_OPEN;

        case KEY_P_CLOSE:
            return P_CLOSE;

        case KEY_INDEX_OPEN:
            return INDEX_OPEN;

        case KEY_INDEX_CLOSE:
            return INDEX_CLOSE;

        case KEY_COMMA:
            return CONCEPT_COMMA;

        case KEY_DEFINE:
            return C_DEFINE;

        case KEY_LENGTH:
            return C_LENGTH;

        case KEY_VALUE:
            return C_VALUE;

        case KEY_OVERRIDE:
            return OVERRIDE;

        case KEY_PROTECTED:
            return PROTECTED;

        case KEY_PRAGMA:
            return C_PRAGMA;

        case KEY_STATIC:
            return C_STATIC;

        case KEY_SUPER:
            return C_SUPER;

        // operators
        case KEY_SELECTOR_P:
            return SELECTOR_P;

        case KEY_SELECTOR_I:
            return SELECTOR_I;

        case KEY_SEL:
            return SEL;

        case KEY_NOT:
            return NOT;

        case KEY_COM:
            return COM;

        case KEY_INC_LEFT:
        case KEY_INC:
            return INC;

        case KEY_DEC_LEFT:
        case KEY_DEC:
            return DEC;

        case KEY_POZ:
            return POZ;

        case KEY_NEG:
            return NEG;

        case KEY_MUL:
            return MUL;

        case KEY_DIV:
            return DIV;

        case KEY_REM:
            return REM;

        case KEY_SUM:
            return SUM;

        case KEY_DIF:
            return DIF;

        case KEY_SHL:
            return SHL;

        case KEY_SHR:
            return SHR;

        case KEY_LES:
            return LES;

        case KEY_LEQ:
            return LEQ;

        case KEY_GRE:
            return GRE;

        case KEY_GEQ:
            return GEQ;

        case KEY_EQU:
            return EQU;

        case KEY_NEQ:
            return NEQ;

        case KEY_AND:
            return AND;

        case KEY_XOR:
            return XOR;

        case KEY_OR:
            return OR;

        case KEY_BAN:
            return BAN;

        case KEY_BOR:
            return BOR;

        case KEY_CND_2:
            return CND_2;

        case KEY_ARR_KEY:
            return ARR_KEY;

        case KEY_ASG:
            return ASG;

        case KEY_BY_REF:
            return ASG_BY_REF;

        case KEY_ASU:
            return ASU;

        case KEY_ADI:
            return ADI;

        case KEY_AMU:
            return AMU;

        case KEY_ADV:
            return ADV;

        case KEY_ARE:
            return ARE;

        case KEY_AAN:
            return AAN;

        case KEY_AXO:
            return AXO;

        case KEY_AOR:
            return AOR;

        case KEY_ASL:
            return ASL;

        case KEY_ASR:
            return ASR;

        case KEY_CLASS_NAME:
            return CLASS_NAME;

        case KEY_TYPE_OF:
            return TYPE_OF;

        case KEY_DLL_CALL:
            return DLL_CALL;

        case KEY_CASE:
            return C_CASE;

        case KEY_SWITCH:
            return C_SWITCH;

        case KEY_DEFAULT:
            return C_DEFAULT;

        case KEY_AWAIT:
            return AWAIT;

        case KEY_ASYNC:
            return C_ASYNC;

        case KEY_OPTIMIZED_IF:
            return "OPTIMIZED_IF";

        case KEY_OPTIMIZED_GOTO:
            return "OPTIMIZED_GOTO";

        case KEY_OPTIMIZED_ECHO:
            return "OPTIMIZED_ECHO";

        case KEY_OPTIMIZED_RETURN:
            return "OPTIMIZED_RETURN";

        case KEY_OPTIMIZED_THROW:
            return "OPTIMIZED_THROW";

        case KEY_OPTIMIZED_TRY_CATCH:
            return "OPTIMIZED_TRY_CATCH";

        case KEY_OPTIMIZED_END_CATCH:
            return "OPTIMIZED_END_CATCH";
    }
    return NULL_STRING;
}

unsigned char GetShortID(unsigned short id) {
    switch (id) {
        case 0x198:
            return KEY_ARR_KEY;
        case 0x199:
            return KEY_CND_2;
        case 0xFF0:
            return KEY_OPTIMIZED_IF;
        case 0xFF1:
            return KEY_OPTIMIZED_GOTO;
        case 0xFF2:
            return KEY_OPTIMIZED_ECHO;
        case 0xFF3:
            return KEY_OPTIMIZED_RETURN;
        case 0xFF4:
            return KEY_OPTIMIZED_THROW;
        case 0xFF5:
            return KEY_OPTIMIZED_TRY_CATCH;
        case 0xFF6:
            return KEY_OPTIMIZED_END_CATCH;
        case 0xFF7:
            return KEY_OPTIMIZED_DEBUG_TRAP;
    }
    return id - 0x80;
}

unsigned short GetLongID(unsigned char id) {
    unsigned short new_id = id;
    switch (new_id) {
        case KEY_ARR_KEY:
            return 0x198;
        case KEY_CND_2:
            return 0x199;
        case KEY_OPTIMIZED_IF:
            return 0xFF0;
        case KEY_OPTIMIZED_GOTO:
            return 0xFF1;
        case KEY_OPTIMIZED_ECHO:
            return 0xFF2;
        case KEY_OPTIMIZED_RETURN:
            return 0xFF3;
        case KEY_OPTIMIZED_THROW:
            return 0xFF4;
        case KEY_OPTIMIZED_TRY_CATCH:
            return 0xFF5;
        case KEY_OPTIMIZED_END_CATCH:
            return 0xFF6;
        case KEY_OPTIMIZED_DEBUG_TRAP:
            return 0xFF7;
    }
    return new_id + 0x80;
}


INTEGER GetID(AnsiString& KeyWord) {
    char c = KeyWord[0];
    if ((c >= 'A') && (c <= 'Z'))
        return 0;
    switch (KeyWord.Length()) {
        case 1:
            {
                if (c == SEP[0]) {
                    return KEY_SEP;
                }
                if (c == QUOTE1[0]) {
                    return KEY_QUOTE1;
                }
                if (c == QUOTE2[0]) {
                    return KEY_QUOTE2;
                }
                if (c == P_OPEN[0]) {
                    return KEY_P_OPEN;
                }
                if (c == P_CLOSE[0]) {
                    return KEY_P_CLOSE;
                }
                if (c == INDEX_OPEN[0]) {
                    return KEY_INDEX_OPEN;
                }
                if (c == INDEX_CLOSE[0]) {
                    return KEY_INDEX_CLOSE;
                }
                if (c == CONCEPT_COMMA[0]) {
                    return KEY_COMMA;
                }
                if (c == SEL_2[0]) {
                    return KEY_SEL;
                }
                if (c == ASG[0]) {
                    return KEY_ASG;
                }
                if (c == BEGIN[0]) {
                    return KEY_BEGIN;
                }
                if (c == END[0]) {
                    return KEY_END;
                }
                if (c == NOT[0]) {
                    return KEY_NOT;
                }
                if (c == COM[0]) {
                    return KEY_COM;
                }
                if (c == MUL[0]) {
                    return KEY_MUL;
                }
                if (c == DIV[0]) {
                    return KEY_DIV;
                }
                if (c == REM[0]) {
                    return KEY_REM;
                }
                if (c == SUM[0]) {
                    return KEY_SUM;
                }
                if (c == DIF[0]) {
                    return KEY_DIF;
                }
                if (c == LES[0]) {
                    return KEY_LES;
                }
                if (c == GRE[0]) {
                    return KEY_GRE;
                }
                if (c == AND[0]) {
                    return KEY_AND;
                }
                if (c == XOR[0]) {
                    return KEY_XOR;
                }
                if (c == OR[0]) {
                    return KEY_OR;
                }
                if (c == CND_2[0]) {
                    return KEY_CND_2;
                }
            }
            break;

        case 2:
            if (KeyWord == SELECTOR_I) {
                return KEY_SELECTOR_I;
            }
            if (KeyWord == SEL) {
                return KEY_SEL;
            }
            if (KeyWord == INC) {
                return KEY_INC;
            }
            if (KeyWord == DEC) {
                return KEY_DEC;
            }
            if (KeyWord == SHL) {
                return KEY_SHL;
            }
            if (KeyWord == SHR) {
                return KEY_SHR;
            }
            if (KeyWord == LEQ) {
                return KEY_LEQ;
            }
            if (KeyWord == GEQ) {
                return KEY_GEQ;
            }
            if (KeyWord == EQU) {
                return KEY_EQU;
            }
            if (KeyWord == NEQ) {
                return KEY_NEQ;
            }
            if (KeyWord == BAN) {
                return KEY_BAN;
            }
            if (KeyWord == BOR) {
                return KEY_BOR;
            }
            if (KeyWord == ASG_BY_REF) {
                return KEY_BY_REF;
            }
            if (KeyWord == ASU) {
                return KEY_ASU;
            }
            if (KeyWord == ADI) {
                return KEY_ADI;
            }
            if (KeyWord == AMU) {
                return KEY_AMU;
            }
            if (KeyWord == ADV) {
                return KEY_ADV;
            }
            if (KeyWord == ARE) {
                return KEY_ARE;
            }
            if (KeyWord == AAN) {
                return KEY_AAN;
            }
            if (KeyWord == AXO) {
                return KEY_AXO;
            }
            if (KeyWord == AOR) {
                return KEY_AOR;
            }
            if (KeyWord == DLL_CALL) {
                return KEY_DLL_CALL;
            }
            if (KeyWord == IF) {
                return KEY_IF;
            }
            if (KeyWord == DO) {
                return KEY_DO;
            }
            if (KeyWord == CND_NULL) {
                return KEY_CND_NULL;
            }
            if (KeyWord == ARR_KEY) {
                return KEY_ARR_KEY;
            }
            break;

        case 3:
            if (KeyWord == VAR) {
                return KEY_VAR;
            }
            if (KeyWord == ASL) {
                return KEY_ASL;
            }
            if (KeyWord == ASR) {
                return KEY_ASR;
            }
            if (KeyWord == FOR) {
                return KEY_FOR;
            }
            if (KeyWord == TRY) {
                return KEY_TRY;
            }
            if (KeyWord == NEW) {
                return KEY_NEW;
            }
            if (KeyWord == SET) {
                return KEY_SET;
            }
            if (KeyWord == GET) {
                return KEY_GET;
            }
            break;

        case 4:
            if (KeyWord == ELSE) {
                return KEY_ELSE;
            }
            if (KeyWord == ECHO) {
                return KEY_ECHO;
            }
            if (KeyWord == C_CASE) {
                return KEY_CASE;
            }
            break;

        case 5:
            if (KeyWord == WHILE) {
                return KEY_WHILE;
            }
            if (KeyWord == THROW) {
                return KEY_THROW;
            }
            if (KeyWord == CATCH) {
                return KEY_CATCH;
            }
            if (KeyWord == BREAK) {
                return KEY_BREAK;
            }
            if (KeyWord == CLASS) {
                return KEY_CLASS;
            }
#ifndef DISABLE_DEPRECATED_FEATURES
            if (KeyWord == CEVENT) {
                return KEY_CEVENT;
            }
#endif
            if (KeyWord == C_VALUE) {
                return KEY_VALUE;
            }
            if (KeyWord == C_SUPER) {
                return KEY_SUPER;
            }
            if (KeyWord == C_ASYNC) {
                return KEY_ASYNC;
            }
            if (KeyWord == AWAIT) {
                return KEY_AWAIT;
            }
            break;

        case 6:
            if (KeyWord == RETURN) {
                return KEY_RETURN;
            }
            if (KeyWord == M_DELETE) {
                return KEY_DELETE;
            }
            if (KeyWord == PUBLIC) {
                return KEY_PUBLIC;
            }
            if (KeyWord == C_SWITCH) {
                return KEY_SWITCH;
            }
            if (KeyWord == C_LENGTH) {
                return KEY_LENGTH;
            }
            if (KeyWord == C_STATIC) {
                return KEY_STATIC;
            }
            if (KeyWord == C_DEFINE) {
                return KEY_DEFINE;
            }
            if (KeyWord == TYPE_OF) {
                return KEY_TYPE_OF;
            }
            if (KeyWord == IMPORT) {
                return KEY_IMPORT;
            }
            if (KeyWord == C_PRAGMA) {
                return KEY_PRAGMA;
            }
            break;

        case 7:
            if (KeyWord == PRIVATE) {
                return KEY_PRIVATE;
            }
            if (KeyWord == EXTENDS) {
                return KEY_EXTENDS;
            }
            if (KeyWord == CLASS_NAME) {
                return KEY_CLASS_NAME;
            }
            if (KeyWord == INCLUDE) {
                return KEY_INCLUDE;
            }
            if (KeyWord == C_DEFAULT) {
                return KEY_DEFAULT;
            }
            break;

        case 8:
            if (KeyWord == FUNCTION) {
                return KEY_FUNCTION;
            }
            if (KeyWord == CONTINUE) {
                return KEY_CONTINUE;
            }
            if (KeyWord == PROPERTY) {
                return KEY_PROPERTY;
            }
            if (KeyWord == M_OPERATOR) {
                return KEY_OPERATOR;
            }
            if (KeyWord == OVERRIDE) {
                return KEY_OVERRIDE;
            }
#ifndef DISABLE_DEPRECATED_FEATURES
            if (KeyWord == TRIGERS) {
                return KEY_TRIGERS;
            }
#endif
            break;

        case 9:
            if (KeyWord == PROTECTED) {
                return KEY_PROTECTED;
            }
            break;
    }
    return 0;
}

INTEGER GetType(AnsiString& KeyWord) {
    INTEGER length = KeyWord.Length();

    if (length == 0) {
        return TYPE_EMPTY;
    }

    if (KeyWord == SEP) {
        return TYPE_SEPARATOR;
    }

    if ((KeyWord == BEGIN) || (KeyWord == END)) {
        return TYPE_KEYWORD;
    }

    if (LexicalCheck(KeyWord, TYPE_OPERATOR)) {
        return TYPE_OPERATOR;
    }

    if (LexicalCheck(KeyWord, TYPE_METHOD)) {
        return TYPE_METHOD;
    }

    if (length > 1) {
        char *kptr = KeyWord.c_str();
        if ((kptr [0] == kptr [length - 1]) && ((kptr [0] == QUOTE1 [0]) || (kptr [0] == QUOTE2 [0]))) {
            return TYPE_STRING;
        }
        if ((kptr [0] == P_OPEN [0]) && (kptr [length - 1] == P_CLOSE [0])) {
            return TYPE_ATOM;
        }

        if ((kptr [0] == INDEX_OPEN [0]) && (kptr [length - 1] == INDEX_CLOSE [0])) {
            return TYPE_INDEXER;
        }
#ifndef NO_BUILTIN_REGEX
        if ((length > 2) && ((kptr [0] == '/') && (kptr [length - 1] == '/'))) {
            return TYPE_REGEX;
        }
#endif
    }

    if (LexicalCheck(KeyWord, TYPE_NUMBER)) {
        return TYPE_NUMBER;
    }

    if (LexicalCheck(KeyWord, TYPE_HEX_NUMBER)) {
        return TYPE_HEX_NUMBER;
    }
    return TYPE_INVALID;
}

INTEGER IsOperator(AnsiString& S) {
    INTEGER id = GetID(S);

    return ((id > __START_OPERATORS) && (id < __END_OPERATORS)) ? 1 : 0;
}

AnsiString Strip2(AnsiString String) {
    char *data = String.c_str();
    char *pos  = 0;

    switch (String [0]) {
        case '\'':
        case '"':
        case '(':
            {
                pos = strchr(data + 1, data [0]);
                if (pos) {
                    pos [0] = 0;
                    return AnsiString(data + 1);
                }
                return String;
            }

        default:
            for (int i = String.Length() - 1; i >= 0; i--) {
                if ((data [i] == ' ') || (data [i] == '\t')) {
                    data [i] = 0;
                } else {
                    break;
                }
            }
            return AnsiString(data);
    }
}

inline INTEGER ValidUTF8Chars(AnsiString& S) {
    INTEGER len = S.Length();

    if (!len)
        return 0;

    INTEGER utf8_chars = 0;
    unsigned char *str = (unsigned char *)S.c_str();
    for (INTEGER i = 0; i < len; i++) {
        unsigned char utf8_char = str[i];
        if ((((utf8_char >= 'a') && (utf8_char <= 'z')) ||
             ((utf8_char >= 'A') && (utf8_char <= 'Z')) || (utf8_char == '_')) ||
            ((utf8_char >= '0') && (utf8_char <= '9') && (i))) {
            continue;
        } else
        if (utf8_char & 0x80) {
            // utf8 sequence
            int seq_len = 1;
            if ((utf8_char & 0xF0) == 0xF0)
                seq_len = 4;
            else
            if ((utf8_char & 0xE0) == 0xE0)
                seq_len = 3;
            else
            if ((utf8_char & 0xC0) == 0xC0)
                seq_len = 2;
            else
                return 0;

            if (i + seq_len > len) {
                // invalid utf8
                return 0;
            } else
            for (int j = 1; j < seq_len; j++) {
                utf8_char = (unsigned char)str [i + j];
                if ((utf8_char >> 6) != 0x02)
                    return 0;
            }
            utf8_chars ++;
            i += seq_len - 1;
        } else
            return 0;
    }
    return utf8_chars;
}

inline INTEGER ValidName(AnsiString& S) {
    INTEGER len = S.Length();

    if (!len)
        return 0;

    for (INTEGER i = 0; i < len; i++) {
        if ((((S [i] >= 'a') && (S [i] <= 'z')) ||
             ((S [i] >= 'A') && (S [i] <= 'Z')) || (S [i] == '_')) ||
            ((S [i] >= '0') && (S [i] <= '9') && (i))) {
            continue;
        } else {
            return ValidUTF8Chars(S);
        }
    }
    return 1;
}

inline INTEGER ValidHexNumber(AnsiString& S) {
    INTEGER len = S.Length();

    if (len < 3) {
        return 0;
    }
    for (INTEGER i = 2; i < len; i++) {
        if (((S [i] >= '0') && (S [i] <= '9')) ||
            ((S [i] >= 'a') && (S [i] <= 'f')) ||
            ((S [i] >= 'A') && (S [i] <= 'F'))) {
            continue;
        } else {
            return 0;
        }
    }
    if ((S [0] != '0') || (S [1] != 'x'))
        return 0;
    return 1;
}

inline INTEGER ValidNumber(AnsiString& S) {
    INTEGER len        = S.Length();
    INTEGER done_point = 0;
    INTEGER EXP        = 0;
    INTEGER EXP_PREC   = 0;

    if (!len) {
        return 0;
    }

    for (INTEGER i = 0; i < len; i++) {
        if (EXP_PREC) {
            EXP_PREC = 0;
            if ((S [i] != '+') && (S [i] != '-')) {
                if ((S [i] >= '0') && (S [i] <= '9'))
                    continue;
                return 0;
            }
        } else
        if (((S [i] == 'e') || (S [i] == 'E')) && (!EXP)) {
            EXP      = 1;
            EXP_PREC = 1;
        } else
        if (S [i] == '.') {
            done_point = 1;
        } else
        if ((S [i] >= '0') && (S [i] <= '9')) {
            continue;
        } else {
            return 0;
        }
    }
    return !EXP_PREC;
}

inline INTEGER LexicalCheck(AnsiString& S, INTEGER TYPE) {
    if (TYPE == TYPE_METHOD) {
        return ValidName(S);
    } else
    if (TYPE == TYPE_NUMBER) {
        return ValidNumber(S);
    } else
    if (TYPE == TYPE_HEX_NUMBER) {
        return ValidHexNumber(S);
    } else
    if (TYPE == TYPE_OPERATOR) {
        if (S.Length() > 7)
            return 0;

        return GetOperatorType(GetID(S));
    }
    return 0;
}

void StripString(AnsiString *S, AnsiString *result) {
    intptr_t len     = S->Length();
    int      start   = 0;
    int      escape  = 0;
    int      set_esc = 0;

    char first = 0;
    char last  = 0;

    if (len >= 2) {
        first = (*S) [(uintptr_t)0];
        last  = (*S) [(uintptr_t)len - 1];
    }

    if ((first == last) && ((first == '\"') || (first == '\''))) {
        len--;
        start++;
    }
    char *ptr = S->c_str();
    result->LoadBuffer("", 0);

    for (uintptr_t i = start; i < (uintptr_t)len; i++) {
        if ((ptr [i] == '\\') && (!escape)) {
            set_esc = 1;
        } else {
            set_esc = 0;
        }

        if (!set_esc) {
            if (!escape) {
                *result += ptr [i];
            } else {
                switch (ptr [i]) {
                    case 'n':
                        *result += '\n';
                        break;

                    case 'r':
                        *result += '\r';
                        break;

                    case 't':
                        *result += '\t';
                        break;

                    case 'b':
                        *result += '\b';
                        break;

                    case 'a':
                        *result += '\a';
                        break;

                    case 'f':
                        *result += '\f';
                        break;

                    case 'v':
                        *result += '\v';
                        break;

                    case 'x':
                        {
                            char hex [3] = { 0, 0, 0 };
                            int  dif;
                            for (dif = 1; dif < 3; dif++) {
                                int pos = i + dif;
                                if (pos >= len) {
                                    break;
                                }
                                if ((((ptr [pos] >= '0') && (ptr [pos] <= '9')) ||
                                     ((ptr [pos] >= 'a') && (ptr [pos] <= 'f')) ||
                                     ((ptr [pos] >= 'A') && (ptr [pos] <= 'F')))) {
                                    hex [dif - 1] = ptr [pos];
                                } else {
                                    break;
                                }
                            }
                            i += dif - 1;
                            if (hex [0]) {
                                unsigned INTEGER TEMP_NR = 0;
                                sscanf(hex, "%x", &TEMP_NR);
                                unsigned char t = (unsigned char)TEMP_NR;
                                result->AddBuffer((char *)&t, 1);
                            }
                        }
                        break;

                    default:
                        {
                            // check if octal ...
                            char oct [4] = { 0, 0, 0, 0 };
                            int  dif;
                            for (dif = 0; dif < 3; dif++) {
                                int pos = i + dif;
                                if (pos >= len) {
                                    break;
                                }
                                if ((ptr [pos] >= '0') && (ptr [pos] <= '7')) {
                                    oct [dif] = ptr [pos];
                                } else {
                                    break;
                                }
                            }
                            i += dif;
                            if (oct [0]) {
                                unsigned INTEGER TEMP_NR = 0;
                                sscanf(oct, "%o", &TEMP_NR);
                                char t = (signed char)TEMP_NR;
                                result->AddBuffer((char *)&t, 1);
                                i--;
                            } else {
                                // it something wrong ... is not octal ... i just put it
                                *result += ptr [i];
                            }
                        }
                }
            }
        }
        escape  = set_esc;
        set_esc = 0;
    }
}

INTEGER GetPriority(INTEGER OP_ID) {
    if (OP_ID <= OPERATOR_LEVEL_SET_0) {
        return OPERATOR_LEVEL_SET_0;
    }
    if (OP_ID <= OPERATOR_LEVEL_1) {
        return OPERATOR_LEVEL_1;
    }
    if (OP_ID <= OPERATOR_LEVEL_2) {
        return OPERATOR_LEVEL_2;
    }
    if (OP_ID <= OPERATOR_LEVEL_3) {
        return OPERATOR_LEVEL_3;
    }
    if (OP_ID <= OPERATOR_LEVEL_4) {
        return OPERATOR_LEVEL_4;
    }
    if (OP_ID <= OPERATOR_LEVEL_5) {
        return OPERATOR_LEVEL_5;
    }
    if (OP_ID <= OPERATOR_LEVEL_6) {
        return OPERATOR_LEVEL_6;
    }
    if (OP_ID <= OPERATOR_LEVEL_SET_7) {
        return OP_ID;
    }
    if (OP_ID <= OPERATOR_LEVEL_8) {
        return OPERATOR_LEVEL_8;
    }
    if (OP_ID <= OPERATOR_LEVEL_9) {
        return OPERATOR_LEVEL_9;
    }
    return 0;
}

void dtoa_milo_c_wrapper(double d, char *buf) {
    dtoa_milo(d, buf);
}

#ifdef WITH_MURMURHASHv1
HASH_TYPE hash_func(const void *key, int len) {
    if (!key)
        return 0;
    if (len < 0)
        len = strlen((char *)key);
    if (!len)
        return 0;

    const unsigned int  m     = 0x5bd1e995;
    const int           r     = 24;
    unsigned int        seed  = 0x7870AAFF;
    const unsigned char *data = (const unsigned char *)key;
    if (!len)
        return 0;

    unsigned int h = seed ^ len;

    while (len >= 4) {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len  -= 4;
    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;

        case 2:
            h ^= data[1] << 8;

        case 1:
            h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

#else
 #define ROT32(x, y)    ((x << y) | (x >> (32 - y)))// avoid effort
static unsigned int seed = 0;
unsigned int MurmurHash3(const char *key, int len) {
    static const uint32_t c1 = 0xcc9e2d51;
    static const uint32_t c2 = 0x1b873593;
    static const uint32_t r1 = 15;
    static const uint32_t r2 = 13;
    static const uint32_t m  = 5;
    static const uint32_t n  = 0xe6546b64;

    if (!seed) {
        srand((int)time(NULL) | ((unsigned long long)(void *)key));
        while (!seed)
            seed = rand();
    }
    uint32_t hash = seed;

    const int      nblocks = len / 4;
    const uint32_t *blocks = (const uint32_t *)key;
    int            i;
    uint32_t       k;
    for (i = 0; i < nblocks; i++) {
        k  = blocks[i];
        k *= c1;
        k  = ROT32(k, r1);
        k *= c2;

        hash ^= k;
        hash  = ROT32(hash, r2) * m + n;
    }

    const uint8_t *tail = (const uint8_t *)(key + nblocks * 4);
    uint32_t      k1    = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;

        case 2:
            k1 ^= tail[1] << 8;

        case 1:
            k1 ^= tail[0];

            k1   *= c1;
            k1    = ROT32(k1, r1);
            k1   *= c2;
            hash ^= k1;
    }

    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}

HASH_TYPE hash_func(const void *key_ref, int len) {
    const char *key = (const char *)key_ref;

    if (!key)
        return 0;
    if (len < 0)
        len = strlen((char *)key);
    if (!len)
        return 0;

    unsigned int hash = MurmurHash3(key, len);
    if (len >= 3) {

        unsigned long long final_hash = MurmurHash3(key + 2, len - 2);
        final_hash <<= 32;
        return final_hash | (unsigned long long)hash;
    }
    return hash;
}
#endif
