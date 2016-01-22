#ifndef __ANSI_TYPES_H
#define __ANSI_TYPES_H
#include <stdint.h>

#define INTEGER                     int
#define SYS_INT                     intptr_t
#define POINTER                     void *
#define NUMBER                      double
#define SYSTEM_SOCKET               INTEGER

#define TYPE_INVALID                -1

#define TYPE_EMPTY                  0x00
#define TYPE_STRING                 0x01
#define TYPE_NUMBER                 0x02
#define TYPE_VARIABLE               0x03
#define TYPE_METHOD                 0x04
#define TYPE_ATOM                   0x05
#define TYPE_KEYWORD                0x06
#define TYPE_OPERATOR               0x07
#define TYPE_SEPARATOR              0x08
#define TYPE_INDEXER                0x09
#define TYPE_SELECTOR_START         0x0A
#define TYPE_SELECTOR_END           0x0B
#define TYPE_CLASS                  0x0C
#define TYPE_PARAM_LIST             0x0D
#define TYPE_HEX_NUMBER             0x0E

#define TYPE_OPTIMIZED_KEYWORD      0x20
#define TYPE_OPTIMIZED_JUMP_ADR     0x21

#define KEY_OPTIMIZED_IF            0xFF0
#define KEY_OPTIMIZED_GOTO          0xFF1
#define KEY_OPTIMIZED_ECHO          0xFF2
#define KEY_OPTIMIZED_RETURN        0xFF3
#define KEY_OPTIMIZED_THROW         0xFF4
#define KEY_OPTIMIZED_TRY_CATCH     0xFF5
#define KEY_OPTIMIZED_END_CATCH     0xFF6
#define KEY_OPTIMIZED_DEBUG_TRAP    0xFF7

#define KEY_BEGIN                   0x01
#define KEY_END                     0x02
#define KEY_IF                      0x03
#define KEY_ELSE                    0x04
#define KEY_FOR                     0x05
#define KEY_FOREACH                 0x06
#define KEY_WHILE                   0x07
#define KEY_DO                      0x08
#define KEY_ECHO                    0x09
#define KEY_RETURN                  0x0A
#define KEY_TRY                     0x0B
#define KEY_CATCH                   0x0C
#define KEY_THROW                   0x0D
#define KEY_BREAK                   0x0E
#define KEY_CONTINUE                0x0F
// new is an operator now !
//#define KEY_NEW             0x10
#define KEY_SEP                     0x11
#define KEY_CLASS                   0x12
#define KEY_FUNCTION                0x13
#define KEY_PRIVATE                 0x14
#define KEY_PUBLIC                  0x15
#define KEY_CEVENT                  0x16
#define KEY_PROPERTY                0x17
#define KEY_SET                     0x18
#define KEY_GET                     0x19
#define KEY_EXTENDS                 0x1A
#define KEY_INCLUDE                 0x1B
#define KEY_IMPORT                  0x1C
#define KEY_IN                      0x1D
#define KEY_TRIGERS                 0x1E
#define KEY_VAR                     0x1F
#define KEY_QUOTE1                  0x20
#define KEY_QUOTE2                  0x21
#define KEY_OPERATOR                0x22
#define KEY_DEFINE                  0x23
#define KEY_OVERRIDE                0x24
#define KEY_PROTECTED               0x25
#define KEY_PRAGMA                  0x26
#define KEY_STATIC                  0x27
#define KEY_SUPER                   0x28
#define KEY_CASE                    0x29
#define KEY_SWITCH                  0x2A
#define KEY_CASE_SEP                0x2B
#define KEY_DEFAULT                 0x2C

#define __START_OPERATORS           0x80

#define KEY_DELEGATE                0x90

#define KEY_SELECTOR_P              0x91
#define KEY_SELECTOR_I              0x92

#define KEY_P_OPEN                  0x93
#define KEY_P_CLOSE                 0x94

#define KEY_SEL                     0x95 // selector ->
#define KEY_INDEX_OPEN              0x96
#define KEY_INDEX_CLOSE             0x97

#define KEY_DLL_CALL                0x98 // operator virtual, nu are echivalent !

#define KEY_INC_LEFT                0x99 // ++
#define KEY_DEC_LEFT                0x9A // --

#define OPERATOR_LEVEL_SET_0        0x9A

// unary operators
#define KEY_NEW                     0x9B
#define KEY_DELETE                  0x9C

#define KEY_TYPE_OF                 0x9D
#define KEY_CLASS_NAME              0x9E
#define KEY_LENGTH                  0x9F
#define KEY_VALUE                   0x100

#define KEY_NOT                     0x101 // !
#define KEY_COM                     0x102 // ~ complement
#define KEY_INC                     0x103 // ++
#define KEY_DEC                     0x104 // --
#define KEY_POZ                     0x105 // + // positive
#define KEY_NEG                     0x106 // - // negative

#define OPERATOR_LEVEL_1            0x106

// binary operators : multiplicative
#define KEY_DIV                     0x107 // /
#define KEY_REM                     0x108 // %
#define KEY_MUL                     0x109 // *

#define OPERATOR_LEVEL_2            0x109

// binary operators : additive
#define KEY_SUM                     0x10A // +
#define KEY_DIF                     0x10B // -

#define OPERATOR_LEVEL_3            0x10B

// binary operators : bitwise
#define KEY_SHL                     0x10C // <<
#define KEY_SHR                     0x10D // >>

#define OPERATOR_LEVEL_4            0x10B

// binary operators : comparision
#define KEY_LES                     0x10E // <
#define KEY_LEQ                     0x10F // <=
#define KEY_GRE                     0x110 // >
#define KEY_GEQ                     0x111 // >=

#define OPERATOR_LEVEL_5            0x111

#define KEY_EQU                     0x112 // ==
#define KEY_NEQ                     0x113 // !=

#define OPERATOR_LEVEL_6            0x113

// binary operators : bitwise
#define KEY_AND                     0x114 // &
#define KEY_XOR                     0x115 // ^
#define KEY_OR                      0x116 // |

// binary operators : boolean
#define KEY_BAN                     0x117 // &&
#define KEY_BOR                     0x118 // ||
#define KEY_CND_1                   0x119 // ?
#define KEY_CND_NULL                0x119 // ??
#define KEY_ARR_KEY                 0x198 // =>
#define KEY_CND_2                   0x199 // :

#define OPERATOR_LEVEL_SET_7        0x119

// binary operators : assigment
#define KEY_ASG                     0x11A // =
#define KEY_BY_REF                  0x11B // =&

#define KEY_ASU                     0x11C // +=
#define KEY_ADI                     0x11D // -=
#define KEY_ADV                     0x11E // /=
#define KEY_ARE                     0x11F // %=
#define KEY_AMU                     0x120 // *=
#define KEY_AAN                     0x121 // &=
#define KEY_AXO                     0x122 // ^=
#define KEY_AOR                     0x123 // |=
#define KEY_ASL                     0x124 // <<=
#define KEY_ASR                     0x125 // >>=

#define OPERATOR_LEVEL_8            0x125

#define KEY_COMMA                   0x126 // ,

#define OPERATOR_LEVEL_9            0x126

#define __END_OPERATORS             0x200 // !!! KEY_CND_2 is 0x199 and KEY_ARR_KEY is 0x198

typedef void   VariableDATA;

typedef struct {
    INTEGER *PARAM_INDEX;
    INTEGER COUNT;
} ParamList;


typedef struct tsTreeContainer {
    int     Operator_ID;
    char    Operator_TYPE;
    int     OperandLeft_ID;
    char    OperandLeft_TYPE;
    int     OperandRight_ID;
    char    OperandRight_TYPE;
    int     OperandReserved_ID;
    char    OperandReserved_TYPE;
    int     Result_ID;
    char    *Function;
    INTEGER *Parameters;
    INTEGER ParametersCount;
} TreeContainer;

typedef struct tsTreeVD {
    char   TYPE;
    NUMBER nValue;
    char   *Value;
    int    Length;
    char   IsRef;
} TreeVD;

typedef INTEGER (*EXTERNAL_CALL)(ParamList *PARAMETERS, VariableDATA **LOCAL_CONTEXT, VariableDATA *RESULT);
#endif // __ANSI_TYPES_H
