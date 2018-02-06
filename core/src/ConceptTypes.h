#ifndef __CONCEPTTYPES_H
#define __CONCEPTTYPES_H

//#define SINGLE_PROCESS_DLL
//#define THREADED_FLAG __declspec(thread)
#define THREADED_FLAG

#ifdef SINGLE_PROCESS_DLL
 #define STATIC_FLAG
#else
 #define STATIC_FLAG    THREADED_FLAG static
#endif

#ifdef _WIN32
 #include <winsock.h>
#else
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
#endif

#ifdef _WIN64
 #include <winsock.h>
 #define __SIZEOF_POINTER__    8
#endif

#ifdef __x86_64
 #define __SIZEOF_POINTER__    8
#endif

#ifdef _M_IA64
 #define __SIZEOF_POINTER__    8
#endif

#ifdef _LP64
 #define __SIZEOF_POINTER__    8
#endif

#ifdef __LP64__
 #define __SIZEOF_POINTER__    8
#endif

#include <string.h>
#include "ConceptPools.h"
#include "TinyString.h"
#include "StaticString.h"
#include "Codes.h"
#include <stdint.h>

typedef void (*CHECK_POINT)(int status);
typedef void (*SECURE_MESSAGE)(const char *Sender, const char *Target, int message, const char *value, int len, int socket, void *handler);
typedef void (*DESTROY_PROTO_DATA)(void *data, void *handler);

#define INVOKE_SET_VARIABLE                   0x01
#define INVOKE_GET_VARIABLE                   0x02
#define INVOKE_SET_CLASS_MEMBER               0x03
#define INVOKE_GET_CLASS_MEMBER               0x04
#define INVOKE_FREE_VARIABLE                  0x05
#define INVOKE_CREATE_ARRAY                   0x06
#define INVOKE_GET_ARRAY_ELEMENT              0x07
#define INVOKE_SET_ARRAY_ELEMENT              0x08
#define INVOKE_GET_ARRAY_COUNT                0x09
#define INVOKE_GET_ARRAY_ELEMENT_BY_KEY       0x0A
#define INVOKE_SET_ARRAY_ELEMENT_BY_KEY       0x0B
#define INVOKE_CALL_DELEGATE                  0x0C
#define INVOKE_COUNT_DELEGATE_PARAMS          0x0D
#define INVOKE_LOCK_VARIABLE                  0x0E
#define INVOKE_GET_ARRAY_KEY                  0x0F
#define INVOKE_GET_ARRAY_INDEX                0x10
#define INVOKE_CREATE_VARIABLE                0x11
#define INVOKE_DEFINE_CONSTANT                0x12
#define INVOKE_ARRAY_VARIABLE                 0x13
#define INVOKE_ARRAY_VARIABLE_BY_KEY          0x14
#define INVOKE_GET_CLASS_VARIABLE             0x15
#define INVOKE_DEFINE_CLASS                   0x16
#define INVOKE_CREATE_OBJECT                  0x17
#define INVOKE_GET_APPLICATION_INFO           0x18
#define INVOKE_GET_SERIAL_CLASS               0x19
#define INVOKE_GET_SERIAL_CLASS_NO_DEFAULTS   0x1A
#define INVOKE_SET_SERIAL_CLASS               0x20
#define INVOKE_GET_MEMBER_FROM_ID             0x21
#define INVOKE_DYNAMIC_LOCK                   0x22
#define INVOKE_HAS_MEMBER                     0x23
#define INVOKE_OBJECT_LINKS                   0x24
#define INVOKE_VAR_LINKS                      0x25
#define INVOKE_CLI_ARGUMENTS                  0x26
#define INVOKE_SORT_ARRAY_BY_KEYS             0x27
#define INVOKE_CHECK_POINT                    0x28
#define INVOKE_ARRAY_ELEMENT_IS_SET           0x29
#define INVOKE_GET_DELEGATE_NAMES             0x2A
#define INVOKE_GET_USERDATA                   0x2B
#define INVOKE_GET_THREAD_DATA                0x2C
#define INVOKE_SET_THREAD_DATA                0x2D
#define INVOKE_HIBERNATE                      0x2E
#define INVOKE_NEW_BUFFER                     0x2F
#define INVOKE_DELETE_BUFFER                  0x30
#define INVOKE_PRINT                          0x31
#define INVOKE_CALL_DELEGATE_THREAD           0x32
#define INVOKE_CREATE_DELEGATE                0x33
#define INVOKE_FREE_VARIABLE_REFERENCE        0x34
#define INVOKE_CALL_DELEGATE_THREAD_SAFE      0x35
#define INVOKE_THREAD_LOCK                    0x36
#define INVOKE_EXTERNAL_THREADING             0x37
#define INVOKE_GET_PROTO_SOCKET               0x38
#define INVOKE_DELEGATE_BYTECODE              0x39
#define INVOKE_GET_RELOC_MEMBER_FROM_ID       0x3A
#define INVOKE_CREATE_OBJECT_NOCONSTRUCTOR    0x3B
#define INVOKE_DYNAMIC_UNLOCK                 0x3C
#define INVOKE_CALL_DELEGATE_THREAD_SPINLOCK  0x3D
#define INVOKE_GET_DIRECT_PIPE                0x3E
#define INVOKE_BYTECODE_FREE                  0x40
#define INVOKE_PROFILER                       0x41
#define INVOKE_MULTITHREADED                  0x42
#define INVOKE_GREENTHREAD                    0x43
#define INVOKE_FREEGREENTHREAD                0x44
#define INVOKE_GREENLOOP                      0x45
#define INVOKE_GREENLINK                      0x46
#define INVOKE_CURRENTGREENTHREAD             0x47
#define INVOKE_GREENINSERT                    0x48
#define INVOKE_SETSENDMESSAGEFUNCTION         0x49
#define INVOKE_CREATE_VARIABLE_2              0x4A
#define INVOKE_DYNAMIC_INCLUDE                0x4B
#define INVOKE_DYNAMIC_REMOVE_CLASS           0x4C
#define INVOKE_CREATE_OBJECT_3                0x4D
#define INVOKE_CHECK_REACHABILITY             0x4E
#define INVOKE_GET_CALLSTACK                  0x4F
#define INVOKE_GETGLOBALS                     0x50
#define INVOKE_GETID                          0x51
#define INVOKE_GETPROTODATA                   0x52
#define INVOKE_SETPROTODATA                   0x53
#define INVOKE_EXECUTECODE                    0x54
#define INVOKE_MEMBER_NAME                    0x55
#define INVOKE_CLASS_NAME                     0x56
#define INVOKE_COUNT_DELEGATE_PARAMS2         0x57
#define INVOKE_MEMBER_INFO                    0x58
#define INVOKE_GET_KEYS                       0x59
#define INVOKE_CREATE_INDEPENDENT_WORKER      0x5A
#define INVOKE_WORKER_ERRORS                  0x5B
#define INVOKE_PROFILE_MEMORY                 0x60
#define INVOKE_CREATE_WORKER                  0x61
#define INVOKE_FREE_WORKER                    0x62
#define INVOKE_CREATE_OBJECT_2                0x63
#define INVOKE_ARRAY_KEYS                     0x64

#define INVOKE_SUCCESS                        0
#define CANNOT_INVOKE_INTERFACE               -10
#define INVALID_INVOKE_PARAMETER              -20
#define CANNOT_INVOKE_IN_THIS_CASE            -30

#define     ACCESS_LOCAL                      0x00
#define     ACCESS_PUBLIC                     0x01
#define     ACCESS_PRIVATE                    0x02
#define     ACCESS_PROTECTED                  0x03

#define     VARIABLE_CONSTANT                 0x00
#define     VARIABLE_UNDEFINED                0x01
#define     VARIABLE_NUMBER                   0x02
#define     VARIABLE_STRING                   0x03
#define     VARIABLE_CLASS                    0x04
#define     VARIABLE_ARRAY                    0x05
#define     VARIABLE_DELEGATE                 0x06
#define     _LINK__STRING_DATA                -1

#define     STATIC_CLASS_ARRAY                -1
#define     STATIC_CLASS_DLL                  -2
#define     STATIC_CLASS_DELEGATE             -3

#define     CLASS_MEMBERS_DOMAIN              unsigned short
typedef struct tsTreeContainer {
    int         Operator_ID;
    signed char Operator_TYPE;
    int         OperandLeft_ID;
    signed char OperandLeft_TYPE;
    int         OperandRight_ID;
    signed char OperandRight_TYPE;
    int         OperandReserved_ID;
    signed char OperandReserved_TYPE;
    int         Result_ID;
    char        *Function;
    INTEGER     *Parameters;
    INTEGER     ParametersCount;
} TreeContainer;

typedef struct tsTreeVD {
    signed char TYPE;
    NUMBER      nValue;
    char        *Value;
    int         Length;
    signed char IsRef;
} TreeVD;

typedef struct tsVariableDESCRIPTOR {
    NUMBER       nValue;
    StaticString value;
    TinyString   name;
    signed char  TYPE;
    signed char  USED;
    signed char  BY_REF;

    // value is for constants only !
    POOLED(tsVariableDESCRIPTOR);
} VariableDESCRIPTOR;

//#define RuntimeVariableDESCRIPTOR   VariableDESCRIPTOR
typedef struct tsRuntimeVariableDESCRIPTOR {
    NUMBER       nValue;
    StaticString value;
    signed char  TYPE;
    signed char  BY_REF;
    signed char  USED;

    // value is for constants only !
    POOLED(tsRuntimeVariableDESCRIPTOR);
} RuntimeVariableDESCRIPTOR;

typedef struct tsSmallVariableDESCRIPTOR {
    StaticString value;
    NUMBER       nValue;
    signed char  TYPE;

    POOLED(tsSmallVariableDESCRIPTOR);
} SmallVariableDESCRIPTOR;

typedef struct tsVariableDATAPROPERTY {
    POINTER              CALL_SET;
    CLASS_MEMBERS_DOMAIN IS_PROPERTY_RESULT;
} VariableDATAPROPERTY;

typedef struct tsVariableDATA {
    union {
        NUMBER NUMBER_DATA;
        struct {
            POINTER              CLASS_DATA;
            CLASS_MEMBERS_DOMAIN DELEGATE_DATA;
        };
    };
    unsigned short LINKS;
    signed char    TYPE;
    signed char    IS_PROPERTY_RESULT;
    signed char    flags;
    POOLED(tsVariableDATA);
} VariableDATA;

typedef struct tsUndefinedMember {
    AnsiString     name;
    unsigned short line;
    AnsiString     filename;
    TinyString     _CLASS;
    AnsiString     _MEMBER;

    POOLED(tsUndefinedMember);
} UndefinedMember;

typedef struct tsAnalizerElement {
    void           *_HASH_DATA;
    SYS_INT        ID;
    TinyString     _PARSE_DATA;

    unsigned short _DEBUG_INFO_LINE;
    signed char    TYPE;
    signed char    _INFO_OPTIMIZED;

    POOLED(tsAnalizerElement)
} AnalizerElement;

typedef struct tsSHORT_AnalizerElement {
    INTEGER     ID;
    signed char TYPE;

    POOLED(tsSHORT_AnalizerElement)
} SHORT_AnalizerElement;

#define OPERATOR_ID_TYPE unsigned short
typedef struct tsPartialAnalizerElement {
    OPERATOR_ID_TYPE ID;
    unsigned short _DEBUG_INFO_LINE;
    signed char    TYPE;
    signed char    FLAGS;
    //signed char    _INFO_OPTIMIZED;

    POOLED(tsPartialAnalizerElement)
} PartialAnalizerElement;

typedef struct tsOptimizedElement {
    AnalizerElement        OperandLeft;
    AnalizerElement        OperandRight;
    PartialAnalizerElement Operator;
    SHORT_AnalizerElement  OperandReserved;
    INTEGER                Result_ID;

    POOLED(tsOptimizedElement)
} OptimizedElement;

typedef struct tsRuntimeOptimizedElement {
    SYS_INT                OperandRight_ID;
    TinyString             OperandRight_PARSE_DATA;
    INTEGER                OperandLeft_ID;
    INTEGER                OperandReserved_ID;
    INTEGER                Result_ID;
    OPERATOR_ID_TYPE       Operator_ID;
    unsigned short         Operator_DEBUG_INFO_LINE;
    signed char            Operator_TYPE;
    signed char            Operator_FLAGS;
    signed char            OperandReserved_TYPE;
    POOLED(tsRuntimeOptimizedElement)
} RuntimeOptimizedElement;

typedef struct tsParamList {
    INTEGER *PARAM_INDEX;
    INTEGER COUNT;

    POOLED(tsParamList)
} ParamList;

typedef struct tsParamListExtra {
    INTEGER *PARAM_INDEX;
    INTEGER COUNT;
    void    *PIF;
} ParamListExtra;

struct GCRoot {
    void *STACK_TRACE;
    void *NEXT;
};

typedef struct tsSCStack {
    void           *PREV;
    void           *ROOT;
    void           *TOP;
    void           *CM;

    void           **STACK_CONTEXT;
    void           **LOCAL_CONTEXT;

    int            stack_pos;
    int            len;
    unsigned short line;
    signed char    alloc_from_stack;
} SCStack;

struct GreenThreadCycle {
    void                 *INTERPRETER;
    void                 *OPT;
    VariableDATA         **LOCAL_CONTEXT;
    VariableDATAPROPERTY *PROPERTIES;
    VariableDATA         *THROW_DATA;
    INTEGER              INSTRUCTION_POINTER;
    INTEGER              INSTRUCTION_COUNT;
    INTEGER              CATCH_INSTRUCTION_POINTER;
    INTEGER              CATCH_VARIABLE;
    INTEGER              PREVIOUS_TRY;
    VariableDATA         *OWNER;
    SCStack              STACK_TRACE;
    void                 *refobject;
    void                 *PIF;
    void                 *LAST_THREAD;
    void                 *CURRENT_THREAD;
    // interpreter next
    void                 *NEXT;
};

#define FREAD_FAIL(buffer, size, elems, fin)        \
    if (!concept_fread(buffer, size, elems, fin)) { \
        concept_fclose(in);                         \
        return 0;                                   \
    }

#ifdef __BIG_ENDIAN__
    if (!concept_fread_int(buffer, size, elems, fin)) { \
        concept_fclose(in);                             \
        return 0;                                       \
    }
#else
    #define FREAD_INT_FAIL  FREAD_FAIL
#endif

#define SKIP(bytes, fin)    concept_fseek(fin, bytes, SEEK_CUR);

#define SERIALIZE_PARAM_LIST(Param, out)                                   \
    concept_fwrite_int(&Param->COUNT, sizeof(short), 1, out);              \
    concept_fwrite_int(DELTA_UNREF(Param, Param->PARAM_INDEX), sizeof(INTEGER), Param->COUNT, out);

#if __SIZEOF_POINTER__ == 8
 #define SERIALIZE_AE(OE, out, put_parse)              \
    {                                                  \
        int _id = OE->OperandRight_ID;                 \
        concept_fwrite_int(&_id, sizeof(_id), 1, out); \
    }                                                  \
    if (put_parse) {                                   \
        OE->OperandRight_PARSE_DATA.Serialize(out, SERIALIZE_16BIT_LENGTH); }
#else
 #define SERIALIZE_AE(OE, out, put_parse)                                           \
    concept_fwrite_int(&OE->OperandRight_ID, sizeof(OE->OperandRight_ID), 1, out);  \
    if (put_parse) {                                                                \
        OE->OperandRight_PARSE_DATA.Serialize(out, SERIALIZE_16BIT_LENGTH); }
#endif

#define SERIALIZE_SHORT_AE(OE, out)                            \
    {                                                          \
        int tempID = OE->Operator_ID;                          \
        concept_fwrite_int(&OE->Operator_TYPE, sizeof(OE->Operator_TYPE), 1, out); \
        concept_fwrite_int(&tempID, sizeof(tempID), 1, out);   \
    }

#define SERIALIZE_VIRTUAL_AE(TYPE, ID, out)          \
    concept_fwrite_int(&TYPE, sizeof(TYPE), 1, out); \
    concept_fwrite_int(&ID, sizeof(ID), 1, out);

#define SERIALIZE_SMALL_VAR_DESCRIPTOR(VD, out)                  \
    /* is a parameter validator ? */                             \
    signed char VD_TYPE = VD->TYPE;                              \
    signed char VD_DUMMY;                                        \
    if (VD->TYPE < (signed char)0) {                             \
        char flag = 0x40;                                        \
        concept_fwrite_int(&flag, sizeof(flag), 1, out);         \
        INTEGER cls_object = (INTEGER)VD->nValue;                \
        concept_fwrite_int(&cls_object, sizeof(cls_object), 1, out); \
        VD_TYPE = -VD_TYPE;                                      \
    }                                                            \
    concept_fwrite(&VD_DUMMY, sizeof(VD_DUMMY), 1, out);         \
    if ((VD_TYPE == VARIABLE_NUMBER) && (VD->nValue)) {          \
        VD->value = AnsiString(VD->nValue); }                    \
    if (VD->value.Length()) {                                    \
        concept_fwrite_int(&VD_TYPE, sizeof(VD_TYPE), 1, out);   \
        VD->value.Serialize(out, SERIALIZE_32BIT_LENGTH);        \
    } else {                                                     \
        signed char temp = -VD_TYPE;                             \
        concept_fwrite(&temp, sizeof(temp), 1, out);             \
    }

#define SERIALIZE_VAR_DESCRIPTOR(VD, out)                        \
    /* is a parameter validator ? */                             \
    signed char VD_TYPE = VD->TYPE;                              \
    if (VD->TYPE < (signed char)0) {                             \
        signed char flag = 0x40;                                 \
        concept_fwrite(&flag, sizeof(flag), 1, out);             \
        INTEGER cls_object = (INTEGER)VD->nValue;                \
        concept_fwrite_int(&cls_object, sizeof(cls_object), 1, out); \
        VD_TYPE = -VD_TYPE;                                      \
    }                                                            \
    concept_fwrite_int(&VD->BY_REF, sizeof(VD->BY_REF), 1, out); \
    if ((VD_TYPE == VARIABLE_NUMBER) && (VD->nValue)) {          \
        VD->value = AnsiString(VD->nValue); }                    \
    if (VD->value.Length()) {                                    \
        concept_fwrite_int(&VD_TYPE, sizeof(VD_TYPE), 1, out);   \
        VD->value.Serialize(out, SERIALIZE_32BIT_LENGTH);        \
    } else {                                                     \
        signed char temp = -VD_TYPE;                             \
        concept_fwrite_int(&temp, sizeof(temp), 1, out);         \
    }

#define SERIALIZE_OPTIMIZED(OE, out)                                                                                             \
    SERIALIZE_SHORT_AE(OE, out);                                                                                                 \
    SERIALIZE_FLAGS(OE->Operator_FLAGS, out);                                                                                    \
    concept_fwrite_int(&OE->OperandLeft_ID, sizeof(OE->OperandLeft_ID), 1, out);                                                 \
    SERIALIZE_FLAGS(OE->Operator_FLAGS, out);                                                                                    \
    if ((OE->Operator_TYPE == TYPE_OPERATOR) && (OE->Operator_ID == KEY_DLL_CALL) && (OE->OperandLeft_ID == STATIC_CLASS_DLL)) { \
        SERIALIZE_AE(OE, out, 1);                                                                                                \
    } else {                                                                                                                     \
        SERIALIZE_AE(OE, out, 0);                                                                                                \
    }                                                                                                                            \
    SERIALIZE_VIRTUAL_AE(OE->OperandReserved_TYPE, OE->OperandReserved_ID, out);                                                 \
    concept_fwrite_int(&OE->Result_ID, sizeof(OE->Result_ID), 1, out);

#define UNSERIALIZE_PARAM_LIST_SIZE(Param, in)                           \
    concept_fread_int(&Param->COUNT, sizeof(/*Param->COUNT*/ short), 1, in); \
    if (Param->COUNT) {                                                  \
        SKIP(sizeof(INTEGER) * Param->COUNT, in); }                      \

#define UNSERIALIZE_PARAM_LIST(Param, in, is_pooled)                                                     \
    concept_fread_int(&Param->COUNT, sizeof(short), 1, in);                                              \
    if (Param->COUNT) {                                                                                  \
        if (is_pooled) {                                                                                 \
            Param->PARAM_INDEX = (INTEGER *)DELTA_REF(Param, SHAlloc(sizeof(INTEGER) * Param->COUNT)); } \
        else {                                                                                           \
            Param->PARAM_INDEX = (INTEGER *)DELTA_REF(Param, new INTEGER [Param->COUNT]); }              \
        concept_fread_int(DELTA_UNREF(Param, Param->PARAM_INDEX), sizeof(INTEGER), Param->COUNT, in);    \
    }

#define UNSERIALIZE_FLAGS(flag, out)  concept_fread_int(&flag, sizeof(flag), 1, out);
#define SERIALIZE_FLAGS(flag, out)    concept_fwrite_int(&flag, sizeof(flag), 1, out);

#if __SIZEOF_POINTER__ == 8
 #define UNSERIALIZE_AE(OE, out, get_parse, is_pooled) \
    {                                                  \
        int _id;                                       \
        concept_fread_int(&_id, sizeof(_id), 1, out);  \
        OE->OperandRight_ID = _id;                     \
    }                                                  \
    if (get_parse) {                                   \
        OE->OperandRight_PARSE_DATA.Unserialize(out, SERIALIZE_16BIT_LENGTH, is_pooled); }
#else
 #define UNSERIALIZE_AE(OE, out, get_parse, is_pooled)                              \
    concept_fread_int(&OE->OperandRight_ID, sizeof(OE->OperandRight_ID), 1, out);   \
    if (get_parse) {                                                                \
        OE->OperandRight_PARSE_DATA.Unserialize(out, SERIALIZE_16BIT_LENGTH, is_pooled); }
#endif

#define UNSERIALIZE_SHORT_AE(OE, out)                                             \
    {                                                                             \
        int tempID;                                                               \
        concept_fread_int(&OE->Operator_TYPE, sizeof(OE->Operator_TYPE), 1, out); \
        concept_fread_int(&tempID, sizeof(tempID), 1, out);                       \
        OE->Operator_ID = tempID;                                                 \
    }

#define UNSERIALIZE_VIRTUAL_AE(TYPE, ID, out)       \
    concept_fread_int(&TYPE, sizeof(TYPE), 1, out); \
    concept_fread_int(&ID, sizeof(ID), 1, out);     \

#define UNSERIALIZE_VAR_DESCRIPTOR_SIZE(VD, in)                \
    concept_fread_int(&VD->BY_REF, sizeof(VD->BY_REF), 1, in); \
    VD->nValue = 0;                                            \
    bool is_validator = false;                                 \
    if (VD->BY_REF == 0x40) {                                  \
        INTEGER cls_object = 0;                                \
        concept_fread_int(&cls_object, sizeof(cls_object), 1, in); \
        is_validator = true;                                   \
        concept_fread_int(&VD->BY_REF, sizeof(VD->BY_REF), 1, in); \
    }                                                          \
    concept_fread_int(&VD->TYPE, sizeof(VD->TYPE), 1, in);     \
    if (VD->TYPE < (signed char)0) {                           \
        VD->TYPE *= (signed char)-1;                           \
    } else {                                                   \
        VD->value.Unserialize(in, SERIALIZE_32BIT_LENGTH);     \
    }

#define UNSERIALIZE_SMALL_VAR_DESCRIPTOR(VD, in, is_pooled)           \
    SKIP(1, in);                                                      \
    VD->nValue = 0;                                                   \
    concept_fread_int(&VD->TYPE, sizeof(VD->TYPE), 1, in);            \
    if (VD->TYPE < (signed char)0) {                                  \
        VD->TYPE *= (signed char)-1;                                  \
    } else {                                                          \
        VD->value.Unserialize(in, SERIALIZE_32BIT_LENGTH, is_pooled); \
        if (VD->TYPE == VARIABLE_NUMBER) {                            \
            VD->nValue = VD->value.ToFloat(); }                       \
    }

#define UNSERIALIZE_VAR_DESCRIPTOR(VD, in, is_pooled)                     \
    concept_fread_int(&VD->BY_REF, sizeof(VD->BY_REF), 1, in);            \
    VD->nValue = 0;                                                       \
    INTEGER cls_object   = 0;                                             \
    bool    is_validator = false;                                         \
    /* is a parameter validator ? */                                      \
    if (VD->BY_REF == 0x40) {                                             \
        concept_fread_int(&cls_object, sizeof(cls_object), 1, in);        \
        is_validator = true;                                              \
        concept_fread_int(&VD->BY_REF, sizeof(VD->BY_REF), 1, in);        \
    }                                                                     \
    concept_fread_int(&VD->TYPE, sizeof(VD->TYPE), 1, in);                \
    if (VD->TYPE < (signed char)0) {                                      \
        VD->TYPE *= (signed char)-1;                                      \
    } else {                                                              \
        VD->value.Unserialize(in, SERIALIZE_32BIT_LENGTH, is_pooled);     \
        if (VD->TYPE == VARIABLE_NUMBER) {                                \
            VD->nValue = VD->value.ToFloat(); }                           \
    }                                                                     \
    if (is_validator) {                                                   \
        if (is_lib) {                                                     \
            if (VD->TYPE == VARIABLE_CLASS) {                             \
                VD->nValue = cls_object > 0 ? ClassNames [cls_object - 1] + 1 : 0; }           \
            else                                                          \
            if (VD->TYPE == VARIABLE_ARRAY)                               \
                VD->nValue = cls_object;                                  \
        } else                                                            \
        if ((VD->TYPE == VARIABLE_CLASS) || (VD->TYPE == VARIABLE_ARRAY)) \
            VD->nValue = cls_object;                                      \
        VD->TYPE = -VD->TYPE;                                             \
    }

#define UNSERIALIZE_OPTIMIZED(OE, out, is_pooled)                                                                                \
    UNSERIALIZE_SHORT_AE(OE, out);                                                                                               \
    UNSERIALIZE_FLAGS(OE->Operator_FLAGS, out);                                                                                  \
    concept_fread_int(&OE->OperandLeft_ID, sizeof(OE->OperandLeft_ID), 1, out);                                                  \
    UNSERIALIZE_FLAGS(OE->Operator_FLAGS, out);                                                                                  \
    if ((OE->Operator_TYPE == TYPE_OPERATOR) && (OE->Operator_ID == KEY_DLL_CALL) && (OE->OperandLeft_ID == STATIC_CLASS_DLL)) { \
        UNSERIALIZE_AE(OE, out, 1, is_pooled);                                                                                   \
    } else {                                                                                                                     \
        UNSERIALIZE_AE(OE, out, 0, is_pooled);                                                                                   \
    }                                                                                                                            \
    UNSERIALIZE_VIRTUAL_AE(OE->OperandReserved_TYPE, OE->OperandReserved_ID, out);                                               \
    concept_fread_int(&OE->Result_ID, sizeof(OE->Result_ID), 1, out);

typedef void (*ForeignPrint)(char *str, int length, void *userdata);
typedef void (*ForeignNotify)(void *PIF, char *str, int length, void *userdata);


class SimpleStream {
    friend class PIFAlizator;
    friend INTEGER Invoke(INTEGER INVOKE_TYPE, ...);

private:
    ForeignNotify fprint_notify;
    char          start_print;
    void          *PIF;
public:
    void         *userdata;
    ForeignPrint fprint;
    int          sock;

    static SECURE_MESSAGE send_secure_message;

    char AlreadyPrinted() {
        return start_print;
    }

    SimpleStream(ForeignPrint fp, int _sock = 0, ForeignNotify fp2 = 0, void *ud = 0) {
        sock          = _sock;
        fprint        = fp;
        fprint_notify = fp2;
        userdata      = ud;
        start_print   = 0;
        //send_secure_message = 0;
        PIF = NULL;
    }

    void Reinit(int _sock = 0, void *ud = 0) {
        sock        = _sock;
        userdata    = ud;
        start_print = 0;
    }

    void Print(const char *str, int length = -1, int message_id = -1) {
        if ((!length) || (!str))
            return;

        if (sock) {
            if (send_secure_message) {
                if ((length < 0) && (str))
                    length = strlen(str);
                send_secure_message("%SYSTEM", "%SCREEN", message_id, (char *)str, length, sock, PIF);
            }
            if (fprint_notify) {
                fprint_notify(PIF, (char *)str, length, userdata);
            }
        } else {
            fprint((char *)str, length, userdata);
            start_print = 1;
        }
    }

    void Print2(AnsiString& s, int message_id = -1) {
        this->Print(s.c_str(), s.Length(), message_id);
    }

    void Print(double val) {
        AnsiString value(val);

        Print(value.c_str());
    }

    void ClientError(const char *error_text, int err_id = -11) {
        if (sock) {
            if (send_secure_message)
                send_secure_message("%SYSTEM", "%SCREEN", err_id, (char *)error_text, strlen(error_text), sock, PIF);
        } else {
            if (userdata) {
                Print("<br /><pre>");
            }
            Print("\n---- RUN TIME ERROR --------------------------------------------\n");
            Print(error_text);
            Print("----------------------------------------------------------------\n");
            if (userdata) {
                Print("</pre>");
            }
        }
    }
};

#define STDMAP_CONSTANTS
#ifdef STDMAP_CONSTANTS
#include "HashTable.h"

class ConstantsListEmulation {
private:
    HashTable map;
public:
    ConstantsListEmulation() {
    }

    int Count() {
        return map.size();
    }

    INTEGER Serialize(FILE *out) {
        INTEGER constant_list = map.size();
        INTEGER temp = 0;
        HashTableIterator a = map.begin();
        for (HashTableIterator it = map.begin(); it != map.end(); ++it) {
            VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)map.val(it);
            if ((VD) && (!VD->USED) && (!VD->BY_REF))
                temp++;
        }
        concept_fwrite_int(&temp, sizeof(temp), 1, out);
        if (temp) {
            for (HashTableIterator it = map.begin(); it != map.end(); ++it) {
                VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)map.val(it);
                if ((VD) && (!VD->USED) && (!VD->BY_REF)) {
                    VD->name.Serialize(out, SERIALIZE_8BIT_LENGTH);
                    VD->value.Serialize(out, SERIALIZE_16BIT_LENGTH);
                }
            }
        }
        return temp;
    }

    void Add(void *data, int type) {
        VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)data;
        if (VD) {
            const char *name = VD->name.c_str();
            int len = VD->name.Length();
            VariableDESCRIPTOR *VD2 = (VariableDESCRIPTOR *)map[name];
            if (VD2)
                delete VD2;
            map.add(name, (intptr_t)VD, len);
        }
    }

    void Delete(const char *varname) {
        if ((!varname) || (!varname[0]))
            return;
        VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)map.erase(varname);
        if (VD)
            delete VD;
    }

    void *Find(const char *varname) {
        if ((!varname) || (!varname[0]))
            return NULL;

        return (void *)map.find(varname);
    }

    ~ConstantsListEmulation() {
        HashTableIterator a = map.begin();
        for(HashTableIterator it = map.begin(); it != map.end(); ++it) {
            VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)map.val(it);
            if (VD)
                delete VD;
        }
        map.clear();
    }
};

#define ConstantMapType ConstantsListEmulation
#else
#define ConstantMapType AnsiList
#endif

#endif //__CONCEPTTYPES_H
