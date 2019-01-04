#ifndef __CODES_H
#define __CODES_H

#include "AnsiString.h"
#include <stdint.h>

#define OPERATOR_UNARY          0x01
#define OPERATOR_BINARY         0x02
#define OPERATOR_SELECTOR       0x03
#define OPERATOR_UNARY_LEFT     0x04
#define OPERATOR_RESOLUTION     0x05

#define KEY_BEGIN               0xA01
#define KEY_END                 0xA02
#define KEY_IF                  0xA03
#define KEY_ELSE                0xA04
#define KEY_FOR                 0xA05
#define KEY_FOREACH             0xA06
#define KEY_WHILE               0xA07
#define KEY_DO                  0xA08
#define KEY_ECHO                0xA09
#define KEY_RETURN              0xA0A
#define KEY_TRY                 0xA0B
#define KEY_CATCH               0xA0C
#define KEY_THROW               0xA0D
#define KEY_BREAK               0xA0E
#define KEY_CONTINUE            0xA0F
#define KEY_SEP                 0xA11
#define KEY_CLASS               0xA12
#define KEY_FUNCTION            0xA13
#define KEY_PRIVATE             0xA14
#define KEY_PUBLIC              0xA15
#ifndef DISABLE_DEPRECATED_FEATURES
#define KEY_CEVENT              0xA16
#endif
#define KEY_PROPERTY            0xA17
#define KEY_SET                 0xA18
#define KEY_GET                 0xA19
#define KEY_EXTENDS             0xA1A
#define KEY_INCLUDE             0xA1B
#define KEY_IMPORT              0xA1C
#define KEY_IN                  0xA1D
#ifndef DISABLE_DEPRECATED_FEATURES
#define KEY_TRIGERS             0xA1E
#endif
#define KEY_VAR                 0xA1F
#define KEY_QUOTE1              0xA20
#define KEY_QUOTE2              0xA21
#define KEY_OPERATOR            0xA22
#define KEY_DEFINE              0xA23
#define KEY_OVERRIDE            0xA24
#define KEY_PROTECTED           0xA25
#define KEY_PRAGMA              0xA26
#define KEY_STATIC              0xA27
#define KEY_SUPER               0xA28
#define KEY_CASE                0xA29
#define KEY_SWITCH              0xA2A
#define KEY_CASE_SEP            0xA2B
#define KEY_DEFAULT             0xA2C

#define __START_OPERATORS       0x00

#define KEY_SELECTOR_P          0x11
#define KEY_SELECTOR_I          0x12

#define KEY_P_OPEN              0x13
#define KEY_P_CLOSE             0x14

#define KEY_SEL                 0x15 // selector ->
#define KEY_INDEX_OPEN          0x16
#define KEY_INDEX_CLOSE         0x17

#define KEY_DLL_CALL            0x18 // operator virtual, nu are echivalent !

#define KEY_INC_LEFT            0x19 // ++
#define KEY_DEC_LEFT            0x1A // --

#define OPERATOR_LEVEL_SET_0    0x1A

// unary operators
#define KEY_NEW                 0x1B
#define KEY_DELETE              0x1C

#define KEY_TYPE_OF             0x1D
#define KEY_CLASS_NAME          0x1E
#define KEY_LENGTH              0x1F

#define KEY_VALUE               0x80

#define KEY_NOT                 0x81 // !
#define KEY_COM                 0x82 // ~ complement
#define KEY_INC                 0x83 // ++
#define KEY_DEC                 0x84 // --
#define KEY_POZ                 0x85 // + // positive
#define KEY_NEG                 0x86 // - // negative

#define OPERATOR_LEVEL_1        0x86

// binary operators : multiplicative
#define KEY_DIV                 0x87 // /
#define KEY_REM                 0x88 // %
#define KEY_MUL                 0x89 // *

#define OPERATOR_LEVEL_2        0x89

// binary operators : additive
#define KEY_SUM                 0x8A // +
#define KEY_DIF                 0x8B // -

#define OPERATOR_LEVEL_3        0x8B

// binary operators : bitwise
#define KEY_SHL                 0x8C // <<
#define KEY_SHR                 0x8D // >>

#define OPERATOR_LEVEL_4        0x8D

// binary operators : comparision
#define KEY_LES                 0x8E // <
#define KEY_LEQ                 0x8F // <=
#define KEY_GRE                 0x90 // >
#define KEY_GEQ                 0x91 // >=

#define OPERATOR_LEVEL_5        0x91

#define KEY_EQU                 0x92 // ==
#define KEY_NEQ                 0x93 // !=

#define OPERATOR_LEVEL_6        0x93

// binary operators : bitwise
#define KEY_AND                 0x94 // &
#define KEY_XOR                 0x95 // ^
#define KEY_OR                  0x96 // |

// binary operators : boolean
#define KEY_BAN                 0x97 // &&
#define KEY_BOR                 0x98 // ||
#define KEY_CND_1               0x99 // ?
#define KEY_CND_NULL            0x99 // ??

#define OPERATOR_LEVEL_SET_7    0x99

// binary operators : assigment
#define KEY_ASG                 0x9A // =
#define KEY_BY_REF              0x9B // =&

#define KEY_ASU                 0x9C // +=
#define KEY_ADI                 0x9D // -=
#define KEY_ADV                 0x9E // /=
#define KEY_ARE                 0x9F // %=
#define KEY_AMU                 0xA0 // *=
#define KEY_AAN                 0xA1 // &=
#define KEY_AXO                 0xA2 // ^=
#define KEY_AOR                 0xA3 // |=
#define KEY_ASL                 0xA4 // <<=
#define KEY_ASR                 0xA5 // >>=

#define OPERATOR_LEVEL_8        0xA5

#define KEY_COMMA               0xA6 // ,

#define OPERATOR_LEVEL_9        0xA6

#define KEY_ARR_KEY             0xE0 // =>
#define KEY_CND_2               0xE1 // :

#define __END_OPERATORS         0xE2 // !!! KEY_CND_2 is 0xE1 and KEY_ARR_KEY is 0xE0


// reverse
// selector
#define SEL           "->"
#define SEL_2         "."
// unary operators
#define NOT           "!"
#define COM           "~"
#define INC           "++"
#define DEC           "--"
#define POZ           "+"
#define NEG           "-"

// binary operators : multiplicative
#define MUL           "*"
#define DIV           "/"
#define REM           "%"

// binary operators : additive
#define SUM           "+"
#define DIF           "-"

// binary operators : bitwise
#define SHL           "<<"
#define SHR           ">>"

// binary operators : comparision
#define LES           "<"
#define LEQ           "<="
#define GRE           ">"
#define GEQ           ">="
#define EQU           "=="
#define NEQ           "!="

// binary operators : bitwise
#define AND           "&"
#define XOR           "^"
#define OR            "|"

// binary operators : boolean
#define BAN           "&&"
#define BOR           "||"
#define CND_1         "?"
#define CND_NULL      "??"
#define CND_2         ":"
#define ARR_KEY       "=>"

// binary operators : assigment
#define ASG           "="
#define ASG_BY_REF    "=&"
#define ASU           "+="
#define ADI           "-="
#define AMU           "*="
#define ADV           "/="
#define ARE           "%="
#define AAN           "&="
#define AXO           "^="
#define AOR           "|="
#define ASL           "<<="
#define ASR           ">>="

#define SELECTOR_P    "()"
#define SELECTOR_I    "[]"
#define DLL_CALL      "::"

#define C_SPECIAL     '@'

// reverse !

#define BEGIN            "{"
#define END              "}"
#define IF               "if"
#define ELSE             "else"
#define FOR              "for"
#define FOREACH          "foreach"
#define WHILE            "while"
#define DO               "do"
#define ECHO             "echo"
#define RETURN           "return"
#define TRY              "try"
#define CATCH            "catch"
#define THROW            "throw"
#define BREAK            "break"
#define CONTINUE         "continue"
#define NEW              "new"
#define M_DELETE         "delete"
#define SEP              ";"
#define CLASS            "class"
#define FUNCTION         "function"
#define PRIVATE          "private"
#define PUBLIC           "public"
#define PROTECTED        "protected"
#define CEVENT           "event"
#define PROPERTY         "property"
#define SET              "set"
#define GET              "get"
#define EXTENDS          "extends"
#define INCLUDE          "include"
#define IMPORT           "import"
#define C_IN             "in"
#define TRIGERS          "triggers"
#define VAR              "var"
#define M_OPERATOR       "operator"
#define QUOTE1           "'"
#define QUOTE2           "\""
#define P_OPEN           "("
#define P_CLOSE          ")"
#define INDEX_OPEN       "["
#define INDEX_CLOSE      "]"
#define CONCEPT_COMMA    ","
#define TYPE_OF          "typeof"
#define CLASS_NAME       "classof"
#define C_DEFINE         "define"
#define C_LENGTH         "length"
#define OVERRIDE         "override"
#define C_VALUE          "value"
#define C_PRAGMA         "pragma"
#define C_STATIC         "static"
#define C_SUPER          "super"
#define C_FINALIZE       "finalize"
#define C_CASE           "case"
#define C_SWITCH         "switch"
#define C_CASE_SEP       ":"
#define C_DEFAULT        "default"

#define NULL_STRING                ""

#define VARIABLE_PREFIX            '$'

#define TYPE_INVALID               -1

#define TYPE_EMPTY                 0x00
#define TYPE_STRING                0x01
#define TYPE_NUMBER                0x02
#define TYPE_VARIABLE              0x03
#define TYPE_METHOD                0x04
#define TYPE_ATOM                  0x05
#define TYPE_KEYWORD               0x06
#define TYPE_OPERATOR              0x07
#define TYPE_SEPARATOR             0x08
#define TYPE_INDEXER               0x09
#define TYPE_SELECTOR_START        0x0A
#define TYPE_SELECTOR_END          0x0B
#define TYPE_CLASS                 0x0C
#define TYPE_PARAM_LIST            0x0D
#define TYPE_HEX_NUMBER            0x0E
#define TYPE_REGEX                 0x0F

#define TYPE_OPTIMIZED_KEYWORD     0x20
#define TYPE_OPTIMIZED_JUMP_ADR    0x21

#define MAY_LOOP                   0x7E
#define MAY_IGNORE_RESULT          0x7F


#define INTEGER                    int
#define SYS_INT                    intptr_t
#define C_BYTE                     char
#define POINTER                    void *
#define NUMBER                     double

// PARSING constants

#define ESCAPE_CHARACTER       '\\'
#define NEW_LINE               '\n'

#define LINE_COMMENT           "//"
#define COMMENT_START          "/*"
#define COMMENT_END            "*/"

#define ATOM_MEMBER            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_." // am scos $
#define IDENTIFIER             "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_()"
#define FIRSTCHAR              "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"
#define STATAMENT_SEPARATOR    ';'
#define CASE_SEPARATOR         ':'
#define SEPARATORS             " \t\r\n"
#define ATOMIZER_OPEN          '('
#define ATOMIZER_CLOSE         ')'
#define QUOTES                 "'\""
#define OPERATORS              "+-=*/%&|<>!^()[],~:@?"
#define BLANK                  ' '
#define LEVEL_1                "*/%"
#define LEVEL_2                "+-"
#define ST_BEGIN               '{'
#define ST_END                 '}'
#define MAX_OPERATOR_LEN       2
#define INDEXER_OPEN           '['
#define INDEXER_CLOSE          ']'

// end of parsing constants ...

#define KEY_OPTIMIZED_IF            0xE2
#define KEY_OPTIMIZED_GOTO          0xE3
#define KEY_OPTIMIZED_ECHO          0xE4
#define KEY_OPTIMIZED_RETURN        0xE5
#define KEY_OPTIMIZED_THROW         0xE6
#define KEY_OPTIMIZED_TRY_CATCH     0xE7
#define KEY_OPTIMIZED_END_CATCH     0xE8
#define KEY_OPTIMIZED_DEBUG_TRAP    0xE9

#define KEYWORDS_START              KEY_OPTIMIZED_IF

// operatori definibili

// binary operators : multiplicative
#define DEF_MUL           0x00 // *
#define DEF_DIV           0x01 // /
#define DEF_REM           0x02 // %

// binary operators : additive
#define DEF_SUM           0x03 // +
#define DEF_DIF           0x04 // -

// binary operators : bitwise
#define DEF_SHL           0x05 // <<
#define DEF_SHR           0x06 // >>

// binary operators : comparision
#define DEF_LES           0x07 // <
#define DEF_LEQ           0x08 // <=
#define DEF_GRE           0x09 // >
#define DEF_GEQ           0x0A // >=
#define DEF_EQU           0x0B // ==
#define DEF_NEQ           0x0C // !=

// binary operators : bitwise
#define DEF_AND           0x0D // &
#define DEF_XOR           0x0E // ^
#define DEF_OR            0x0F // |

// binary operators : boolean
#define DEF_BAN           0x10 // &&
#define DEF_BOR           0x11 // ||

// binary operators : assigment
#define DEF_ASG           0x12 // =
#define DEF_ASU           0x13 // +=
#define DEF_ADI           0x14 // -=
#define DEF_AMU           0x15 // *=
#define DEF_ADV           0x16 // /=
#define DEF_ARE           0x17 // %=
#define DEF_AAN           0x18 // &=
#define DEF_AXO           0x19 // ^=
#define DEF_AOR           0x1A // |=
#define DEF_ASL           0x1B // <<=
#define DEF_ASR           0x1C // >>=

// selector []
#define DEF_SELECTOR_I    0x1D // []

#define DEF_NOT           0x1E //"!"
#define DEF_COM           0x1F //"~"
#define DEF_INC           0x20 //"++"
#define DEF_DEC           0x21 //"--"



#define Contains(str, c)    strchr(str, c)
#ifdef WITH_MURMURHASHv1
 #define HASH_TYPE    unsigned int
#else
 #define HASH_TYPE    unsigned long long
#endif

const char *GetKeyWord(INTEGER ID);
INTEGER GetID(AnsiString& KeyWord);
INTEGER GetType(AnsiString& KeyWord);
unsigned char GetShortID(unsigned short id);
unsigned short GetLongID(unsigned char id);

AnsiString Strip2(AnsiString String);
INTEGER IsOperator(AnsiString& S);
INTEGER GetOperatorType(INTEGER ID);
INTEGER LexicalCheck(AnsiString& S, INTEGER TYPE);
void StripString(AnsiString *S, AnsiString* result);
INTEGER GetPriority(INTEGER OP_ID);
HASH_TYPE hash_func(const void *key_ref, int len = -1);
void __DEBUG_PRINT(char *str);

#endif // __CODES_H
