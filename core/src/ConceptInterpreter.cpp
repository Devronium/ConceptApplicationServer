//#define FAST_EXIT_NO_GC_CALL

#include "ConceptInterpreter.h"
#include "AnsiException.h"
#include "ModuleLink.h"
#include "Debugger.h"
#include <time.h>
#include <map>
#include <math.h>
#ifndef _WIN32
    #include <unistd.h>
#endif

#if defined(USE_JIT) || defined(USE_JIT_TRACE)
//#define ARM_PATCH
extern "C" {
 #include "JIT/sljitLir.h"

union executable_code {
    void *code;
    sljit_sw(SLJIT_CALL * func0)(void);
    sljit_sw(SLJIT_CALL * func1)(sljit_sw a);
    sljit_sw(SLJIT_CALL * func2)(sljit_sw a, sljit_sw b);
    sljit_sw(SLJIT_CALL * func3)(sljit_sw a, sljit_sw b, sljit_sw c);
};
typedef union executable_code   executable_code;

 #define IS_REAL(n)       ((n) - (double)(INTEGER)(n)) != 0.0
 #define IS_INTEGER(n)    ((n) - (double)(INTEGER)(n)) == 0.0
}
#endif

POOLED_IMPLEMENTATION(tsSCStack)
POOLED_IMPLEMENTATION(ConceptInterpreter)

static TinyString DLL_CLASS = "STATIC/LIBRARY";
static TinyString DLL_MEMBER = "STATIC_FUNCTION";

// CODUL CE VERIFICA DE PROPRIETATI ...
// To avoid another function call in time-critical function
//---------------------------------------------------------

#define PROPERTY_CODE(THISREF, PROPERTIES)                                                                                                                                                                                                                                                                      \
    if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT)) {                                                                                                                                                                                                                             \
        try {                                                                                                                                                                                                                                                                                                   \
            CCTEMP = (CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET))->CLASS_DATA;                                                                                                                                                                                             \
            WRITE_UNLOCK                                                                                                                                                                                                                                                                                        \
            CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET), &OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID , LOCAL_CONTEXT, ClassID, THISREF->LocalClassID, STACK_TRACE); \
        } catch (VariableDATA *LAST_THROW) {                                                                                                                                                                                                                                                                    \
            THROW_DATA = LAST_THROW;                                                                                                                                                                                                                                                                            \
            if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {                                                                                                                                                                                                                                              \
                LOCAL_CONTEXT [CATCH_VARIABLE - 1]->LINKS--;                                                                                                                                                                                                                                                    \
                if (!LOCAL_CONTEXT [CATCH_VARIABLE - 1]->LINKS) {                                                                                                                                                                                                                                               \
                    VAR_FREE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]); }                                                                                                                                                                                                                                             \
                LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;                                                                                                                                                                                                                                                \
                INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;                                                                                                                                                                                                                                          \
                THROW_DATA                = 0;                                                                                                                                                                                                                                                                  \
                CATCH_INSTRUCTION_POINTER = 0;                                                                                                                                                                                                                                                                  \
                CATCH_VARIABLE            = 0;                                                                                                                                                                                                                                                                  \
                RESTORE_TRY_DATA(THISREF);                                                                                                                                                                                                                                                                      \
            } else {                                                                                                                                                                                                                                                                                            \
                FAST_FREE(PROPERTIES);                                                                                                                                                                                                                                                                          \
                PROPERTIES = 0;                                                                                                                                                                                                                                                                                 \
                WRITE_UNLOCK                                                                                                                                                                                                                                                                                    \
                return 0; }                                                                                                                                                                                                                                                                                     \
        }                                                                                                                                                                                                                                                                                                       \
    }

#define PROPERTY_CODE_LEFT(THISREF, PROPERTIES)                                                                                                                                                                                                                                                                      \
    if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT)) {                                                                                                                                                                                                                                  \
        try {                                                                                                                                                                                                                                                                                                        \
            CCTEMP = (CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET))->CLASS_DATA;                                                                                                                                                                                                  \
            WRITE_UNLOCK                                                                                                                                                                                                                                                                                             \
            CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET), &OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, OE->OperandLeft.ID , LOCAL_CONTEXT, ClassID, THISREF->LocalClassID, STACK_TRACE); \
        } catch (VariableDATA *LAST_THROW) {                                                                                                                                                                                                                                                                         \
            THROW_DATA = LAST_THROW;                                                                                                                                                                                                                                                                                 \
            if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {                                                                                                                                                                                                                                                   \
                LOCAL_CONTEXT [CATCH_VARIABLE - 1]->LINKS--;                                                                                                                                                                                                                                                         \
                if (!LOCAL_CONTEXT [CATCH_VARIABLE - 1]->LINKS) {                                                                                                                                                                                                                                                    \
                    VAR_FREE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]); }                                                                                                                                                                                                                                                  \
                LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;                                                                                                                                                                                                                                                     \
                INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;                                                                                                                                                                                                                                               \
                THROW_DATA                = 0;                                                                                                                                                                                                                                                                       \
                CATCH_INSTRUCTION_POINTER = 0;                                                                                                                                                                                                                                                                       \
                CATCH_VARIABLE            = 0;                                                                                                                                                                                                                                                                       \
                RESTORE_TRY_DATA(THISREF);                                                                                                                                                                                                                                                                           \
            } else {                                                                                                                                                                                                                                                                                                 \
                FAST_FREE(PROPERTIES);                                                                                                                                                                                                                                                                               \
                PROPERTIES = 0;                                                                                                                                                                                                                                                                                      \
                WRITE_UNLOCK                                                                                                                                                                                                                                                                                         \
                return 0; }                                                                                                                                                                                                                                                                                          \
        }                                                                                                                                                                                                                                                                                                            \
    }
//---------------------------------------------------------
#define TEMP_PROPERTY_CODE(THISREF, PROPERTIES)                                                                                                                                                                                                                                                                                      \
    if ((PROPERTIES) && (PROPERTIES [tempOE->OperandLeft.ID - 1].IS_PROPERTY_RESULT)) {                                                                                                                                                                                                                                              \
        try {                                                                                                                                                                                                                                                                                                                        \
            CCTEMP = (CompiledClass *)((VariableDATA *)(PROPERTIES [tempOE->OperandLeft.ID - 1].CALL_SET))->CLASS_DATA;                                                                                                                                                                                                              \
            WRITE_UNLOCK                                                                                                                                                                                                                                                                                                             \
            CCTEMP->_Class->SetProperty(PIF, PROPERTIES [tempOE->OperandLeft.ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [tempOE->OperandLeft.ID - 1].CALL_SET), &tempOE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, tempOE->OperandLeft.ID , LOCAL_CONTEXT, ClassID, THISREF->LocalClassID, STACK_TRACE); \
        } catch (VariableDATA *LAST_THROW) {                                                                                                                                                                                                                                                                                         \
            THROW_DATA = LAST_THROW;                                                                                                                                                                                                                                                                                                 \
            if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {                                                                                                                                                                                                                                                                   \
                LOCAL_CONTEXT [CATCH_VARIABLE - 1]->LINKS--;                                                                                                                                                                                                                                                                         \
                if (!LOCAL_CONTEXT [CATCH_VARIABLE - 1]->LINKS) {                                                                                                                                                                                                                                                                    \
                    VAR_FREE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]); }                                                                                                                                                                                                                                                                  \
                LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;                                                                                                                                                                                                                                                                     \
                INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;                                                                                                                                                                                                                                                               \
                THROW_DATA                = 0;                                                                                                                                                                                                                                                                                       \
                CATCH_INSTRUCTION_POINTER = 0;                                                                                                                                                                                                                                                                                       \
                CATCH_VARIABLE            = 0;                                                                                                                                                                                                                                                                                       \
                RESTORE_TRY_DATA(THISREF);                                                                                                                                                                                                                                                                                           \
            } else {                                                                                                                                                                                                                                                                                                                 \
                FAST_FREE(PROPERTIES);                                                                                                                                                                                                                                                                                               \
                PROPERTIES = 0;                                                                                                                                                                                                                                                                                                      \
                WRITE_UNLOCK                                                                                                                                                                                                                                                                                                         \
                return 0; }                                                                                                                                                                                                                                                                                                          \
        }                                                                                                                                                                                                                                                                                                                            \
    }
//---------------------------------------------------------
#define UPDATE_STRING_VARIABLE_THAT_HAS_INDEX(THISREF, VARIABLE, PROPERTIES)                                                        \
    if ((VARIABLE->CLASS_DATA) && (VARIABLE->TYPE == VARIABLE_STRING) && (((AnsiString *)VARIABLE->CLASS_DATA)->EXTRA_DATA)) {      \
        RuntimeOptimizedElement *tempOE = &OPT->CODE [((AnsiString *)VARIABLE->CLASS_DATA)->EXTRA_DATA - 1];                        \
        ((AnsiString *)VARIABLE->CLASS_DATA)->EXTRA_DATA = 0;                                                                       \
        INTEGER _def_index = -1;                                                                                                    \
        if (LOCAL_CONTEXT [tempOE->OperandLeft.ID - 1]->TYPE == VARIABLE_STRING) {                                                  \
            switch (LOCAL_CONTEXT [tempOE->OperandRight.ID - 1]->TYPE) {                                                            \
                case VARIABLE_NUMBER:                                                                                               \
                    _def_index = (INTEGER)LOCAL_CONTEXT [tempOE->OperandRight.ID - 1]->NUMBER_DATA;                                 \
                    break;                                                                                                          \
                case VARIABLE_STRING:                                                                                               \
                    _def_index = CONCEPT_STRING(LOCAL_CONTEXT [tempOE->OperandRight.ID - 1]).ToInt();                               \
                    break;                                                                                                          \
            }                                                                                                                       \
            CONCEPT_STRING(LOCAL_CONTEXT [tempOE->OperandLeft.ID - 1]).ReplaceCharWithString(CONCEPT_STRING(VARIABLE), _def_index); \
            TEMP_PROPERTY_CODE(THISREF, PROPERTIES)                                                                                 \
        }                                                                                                                           \
    }                                                                                                                               \
//---------------------------------------------------------
#define CLASS_CHECK_KEEP_EXTRA(VARIABLE)                                                   \
    if ((VARIABLE->CLASS_DATA) && (VARIABLE->TYPE != VARIABLE_STRING)) {                   \
        if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) { \
            if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {                       \
                delete (CompiledClass *)VARIABLE->CLASS_DATA; }                            \
            VARIABLE->TYPE       = VARIABLE_NUMBER;                                        \
            VARIABLE->CLASS_DATA = NULL;                                                   \
        } else                                                                             \
        if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
            if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS) {                               \
                delete (Array *)VARIABLE->CLASS_DATA; }                                    \
            VARIABLE->TYPE       = VARIABLE_NUMBER;                                        \
            VARIABLE->CLASS_DATA = NULL;                                                   \
        } else                                                                             \
        if (VARIABLE->TYPE == VARIABLE_NUMBER) {                                           \
            VARIABLE->CLASS_DATA = NULL;                                                   \
        }                                                                                  \
    }
//---------------------------------------------------------
#ifdef SIMPLE_MULTI_THREADING
#define CLASS_CHECK_TS(VARIABLE)                                                               \
        if ((VARIABLE->TYPE != VARIABLE_NUMBER) && (VARIABLE->CLASS_DATA)) {                   \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                delete (AnsiString *)VARIABLE->CLASS_DATA;                                     \
            } else                                                                             \
            if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) { \
                if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {                       \
                    if (PIF->WriteLock.MasterLock) {                                           \
                        WRITE_UNLOCK                                                           \
                        delete (CompiledClass *)VARIABLE->CLASS_DATA;                          \
                        WRITE_LOCK                                                             \
                    } else                                                                     \
                        delete (CompiledClass *)VARIABLE->CLASS_DATA;                          \
                }                                                                              \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS) {                               \
                    if (PIF->WriteLock.MasterLock) {                                           \
                        WRITE_UNLOCK                                                           \
                        delete (Array *)VARIABLE->CLASS_DATA;                                  \
                        WRITE_LOCK                                                             \
                    } else                                                                     \
                        delete (Array *)VARIABLE->CLASS_DATA;                                  \
                }                                                                              \
            }                                                                                  \
            VARIABLE->CLASS_DATA = NULL;                                                       \
        }  
#else
#define CLASS_CHECK_TS              CLASS_CHECK
#endif

#ifdef SIMPLE_MULTI_THREADING
static void INTERNAL_CLASS_CHECK_RESULT(PIFAlizator *PIF, VariableDATA *&VARIABLE, char &IsWriteLocked) {
#else
static void INTERNAL_CLASS_CHECK_RESULT(PIFAlizator *PIF, VariableDATA *&VARIABLE) {
#endif
    if (VARIABLE->LINKS > 1) {
        VARIABLE->LINKS--;
        VariableDATA *RETURN_DATA = (VariableDATA *)VAR_ALLOC(PIF);
        RETURN_DATA->LINKS              = 1;
        RETURN_DATA->NUMBER_DATA        = 0;
        RETURN_DATA->TYPE               = VARIABLE_NUMBER;
        RETURN_DATA->IS_PROPERTY_RESULT = 0;
        VARIABLE = RETURN_DATA;
    } else {
        CLASS_CHECK_TS(VARIABLE);
    }
}

#ifdef SIMPLE_MULTI_THREADING
#define CLASS_CHECK_RESULT(VARIABLE)    INTERNAL_CLASS_CHECK_RESULT(PIF, VARIABLE, IsWriteLocked);
#else
#define CLASS_CHECK_RESULT(VARIABLE)    INTERNAL_CLASS_CHECK_RESULT(PIF, VARIABLE);
#endif

#ifndef INLINE_COMMON_CALLS
void FREE_VARIABLE(VariableDATA *VARIABLE) {
    VARIABLE->LINKS--;
    if (VARIABLE->LINKS < 1) {
        if (VARIABLE->CLASS_DATA) {
            if (VARIABLE->TYPE == VARIABLE_STRING) {
                delete (AnsiString *)VARIABLE->CLASS_DATA;
            } else
            if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) {
                if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)
                    delete (CompiledClass *)VARIABLE->CLASS_DATA;
            } else
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {
                if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS)
                    delete (Array *)VARIABLE->CLASS_DATA;
            }
        }
        VAR_FREE(VARIABLE);
    }
}
#endif
//---------------------------------------------------------
#define RESTORE_TRY_DATA(THISREF)                                                    \
    if (PREVIOUS_TRY) {                                                              \
        CATCH_INSTRUCTION_POINTER = OPT->CODE [PREVIOUS_TRY - 1].OperandReserved.ID; \
        CATCH_VARIABLE            = OPT->CODE [PREVIOUS_TRY - 1].Result_ID;          \
    }
//---------------------------------------------------------

#define EVAL_NUMBER_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                                    \
    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {                                                                                                                                                                                                                                         \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;                                                                                                                      \
            break;                                                                                                                                                                                                                                                                                   \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat();                                                                                                         \
            break;                                                                                                                                                                                                                                                                                   \
        default:                                                                                                                                                                                                                                                                                     \
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME)); \
    }                                                                                                                                                                                                                                                                                                \

#define EVAL_INTEGER_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                                   \
    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {                                                                                                                                                                                                                                         \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR(unsigned INTEGER) LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;                                                                                  \
            break;                                                                                                                                                                                                                                                                                   \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR(unsigned INTEGER)(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat());                                                                    \
            break;                                                                                                                                                                                                                                                                                   \
        default:                                                                                                                                                                                                                                                                                     \
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME)); \
    }                                                                                                                                                                                                                                                                                                \

#define EVAL_ASG_INTEGER_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                               \
    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {                                                                                                                                                                                                                                         \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = (unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR(unsigned INTEGER) LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;                            \
            break;                                                                                                                                                                                                                                                                                   \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = (unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR(unsigned INTEGER)(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat());              \
            break;                                                                                                                                                                                                                                                                                   \
        default:                                                                                                                                                                                                                                                                                     \
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME)); \
    }                                                                                                                                                                                                                                                                                                \

#define EVAL_DIVIDE_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                                          \
    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {                                                                                                                                                                                                                                               \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                              \
            if (!LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA)                                                                                                                                                                                                                                     \
                PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR260, OE->OperandRight._DEBUG_INFO_LINE, 260, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME));     \
            else                                                                                                                                                                                                                                                                                           \
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;                                                                                                                        \
            break;                                                                                                                                                                                                                                                                                         \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                              \
            {                                                                                                                                                                                                                                                                                              \
                double dt = (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat());                                                                                                                                                                                                           \
                if (!dt)                                                                                                                                                                                                                                                                                   \
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR260, OE->OperandRight._DEBUG_INFO_LINE, 260, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME)); \
                else                                                                                                                                                                                                                                                                                       \
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR dt;                                                                                                                                                                      \
            }                                                                                                                                                                                                                                                                                              \
            break;                                                                                                                                                                                                                                                                                         \
        default:                                                                                                                                                                                                                                                                                           \
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME));       \
    }

#define EVAL_DIVIDE_INTEGER_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                                  \
    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {                                                                                                                                                                                                                                               \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                              \
            if (!LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA)                                                                                                                                                                                                                                     \
                PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR260, OE->OperandRight._DEBUG_INFO_LINE, 260, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME));     \
            else                                                                                                                                                                                                                                                                                           \
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR(INTEGER) LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;                                                                                                      \
            break;                                                                                                                                                                                                                                                                                         \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                              \
            {                                                                                                                                                                                                                                                                                              \
                INTEGER dt = (INTEGER)(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat());                                                                                                                                                                                                 \
                if (!dt)                                                                                                                                                                                                                                                                                   \
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR260, OE->OperandRight._DEBUG_INFO_LINE, 260, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME)); \
                else                                                                                                                                                                                                                                                                                       \
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR dt;                                                                                                                                                             \
            }                                                                                                                                                                                                                                                                                              \
            break;                                                                                                                                                                                                                                                                                         \
        default:                                                                                                                                                                                                                                                                                           \
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME));       \
    }

#define EVAL_ASG_INTEGER_DIVIDE_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                              \
    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {                                                                                                                                                                                                                                               \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                              \
            if (!LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA)                                                                                                                                                                                                                                     \
                PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR260, OE->OperandRight._DEBUG_INFO_LINE, 260, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME));     \
            else                                                                                                                                                                                                                                                                                           \
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = (INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR(INTEGER) LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;                                                \
            break;                                                                                                                                                                                                                                                                                         \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                              \
            {                                                                                                                                                                                                                                                                                              \
                INTEGER dt = (INTEGER)(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat());                                                                                                                                                                                                 \
                if (!dt)                                                                                                                                                                                                                                                                                   \
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR260, OE->OperandRight._DEBUG_INFO_LINE, 260, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME)); \
                else                                                                                                                                                                                                                                                                                       \
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = (INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA OPERATOR dt;                                                                                                       \
            }                                                                                                                                                                                                                                                                                              \
            break;                                                                                                                                                                                                                                                                                         \
        default:                                                                                                                                                                                                                                                                                           \
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THISREF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THISREF->OWNER->Defined_In))->NAME, THISREF->OWNER->NAME));       \
    }

//---------------------------------------------------------
//#ifndef SINGLE_PROCESS_DLL
//#endif

#if defined(USE_JIT) || defined (USE_JIT_TRACE)
extern "C" {
 #if (__BIG_ENDIAN__ == 1) || (BIG_ENDIAN == 1) || (defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN))
union i_cast {
    double dn;
    int    bits[2];
};
// 6755399441055744 = 1.5 x 2^52 (round)
  #define double2int(i, d)    { volatile union i_cast u; u.dn = (d) + 6755399441055744.0; i = (int)u.bits[1]; }
 #else
  #ifdef _WIN32
// some weird gcc optimizations break this
// however seems fine on most Linux distros
// but just ot be sure, limit this method to windows
   #define double2int(i, d)    { double t = ((d) + 6755399441055744.0); i = *((int *)(&t)); }
  #else
union i_cast {
    double dn;
    int    bits[2];
};
// 6755399441055744 = 1.5 x 2^52 (round)
   #define double2int(i, d)    { volatile union i_cast u; u.dn = (d) + 6755399441055744.0; i = (int)u.bits[0]; }
  #endif
 #endif

 #ifdef USE_JIT_TRACE
static sljit_sw SLJIT_CALL c_CHECKTYPES3(VariableDATA *a, VariableDATA *b, VariableDATA *c) {
    if ((a->TYPE != VARIABLE_NUMBER) || (a->IS_PROPERTY_RESULT == 1))
        return 0;
    if ((b->TYPE != VARIABLE_NUMBER) || (b->IS_PROPERTY_RESULT == 1))
        return 0;
    if ((c->TYPE != VARIABLE_NUMBER) || (c->IS_PROPERTY_RESULT == 1))
        return 0;
  #ifdef SIMPLE_MULTI_THREADING
    if ((a->LINKS > 1) || (b->LINKS > 1) || (c->LINKS > 1))
        return 0;
  #endif
    return 1;
}

static sljit_sw SLJIT_CALL c_CHECKTYPES2(VariableDATA *a, VariableDATA *b) {
    if ((a->TYPE != VARIABLE_NUMBER) || (a->IS_PROPERTY_RESULT == 1))
        return 0;
    if ((b->TYPE != VARIABLE_NUMBER) || (b->IS_PROPERTY_RESULT == 1))
        return 0;
  #ifdef SIMPLE_MULTI_THREADING
    if ((a->LINKS > 1) || (b->LINKS > 1))
        return 0;
  #endif
    return 1;
}

static sljit_sw SLJIT_CALL c_CHECKTYPES1(VariableDATA *a) {
    if ((a->TYPE != VARIABLE_NUMBER) || (a->IS_PROPERTY_RESULT == 1))
        return 0;
  #ifdef SIMPLE_MULTI_THREADING
    if (a->LINKS > 1)
        return 0;
  #endif
    return 1;
}

static sljit_sw SLJIT_CALL ArrayDataVD(VariableDATA *arr, VariableDATA *idx, VariableDATA *result) {
    int index;

    double2int(index, idx->NUMBER_DATA);
    if ((arr->TYPE == VARIABLE_ARRAY) && (arr->CLASS_DATA) && (index >= 0)) {
  #ifdef OPTIMIZE_FAST_ARRAYS
        VariableDATA *data = FAST_CHECK((Array *)arr->CLASS_DATA, index);
        if (!data)
            data = ((Array *)arr->CLASS_DATA)->GetWithCreate(index);
  #else
        VariableDATA *data = ((Array *)arr->CLASS_DATA)->GetWithCreate(index);
  #endif
        if ((data) && (data->TYPE != VARIABLE_NUMBER))
            return 0;
        return (SYS_INT)data;
    }
    return 0;
}

static sljit_sw SLJIT_CALL ArrayDataCopyVD(VariableDATA *idx, VariableDATA *arr, VariableDATA *result) {
    int index;

    double2int(index, (idx->NUMBER_DATA));

    if ((arr->TYPE == VARIABLE_ARRAY) && (arr->CLASS_DATA) && (index >= 0)) {
  #ifdef OPTIMIZE_FAST_ARRAYS
        VariableDATA *data = FAST_CHECK((Array *)arr->CLASS_DATA, index);
        if (!data)
            data = ((Array *)arr->CLASS_DATA)->GetWithCreate(index);
  #else
        VariableDATA *data = ((Array *)arr->CLASS_DATA)->GetWithCreate(index);
  #endif
        if (data->TYPE == VARIABLE_NUMBER) {
            RESET_VARIABLE(result);
            result->NUMBER_DATA = data->NUMBER_DATA;
            return 1;
        } else {
            switch (data->TYPE) {
                case VARIABLE_STRING:
                    RESET_VARIABLE(result);
                    if ((data->CLASS_DATA) && (((AnsiString *)data->CLASS_DATA)->Length()))
                        result->NUMBER_DATA = 1;
                    else
                        result->NUMBER_DATA = 0;
                    return 1;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    RESET_VARIABLE(result);
                    result->NUMBER_DATA = 1;
                    return 1;

                case VARIABLE_ARRAY:
                    RESET_VARIABLE(result);
                    if ((data->CLASS_DATA) && (((Array *)data->CLASS_DATA)->Count()))
                        result->NUMBER_DATA = 1;
                    else
                        result->NUMBER_DATA = 0;
                    return 1;
            }
        }
    }
    return 0;
}

static sljit_sw SLJIT_CALL ConceptVarToInt(VariableDATA *idx) {
    sljit_sw res = (sljit_sw)idx->NUMBER_DATA;

    return (sljit_sw)res;
}

static sljit_sw SLJIT_CALL ConceptVarFromInt(VariableDATA *idx, sljit_sw val) {
    idx->NUMBER_DATA = val;
    return 0;
}

static sljit_sw SLJIT_CALL ArrayDataVDIF(VariableDATA *idx, VariableDATA *arr) {
    int index;

    double2int(index, (idx->NUMBER_DATA));
    sljit_sw res      = 2;
    Array    *arrData = (Array *)arr->CLASS_DATA;
    if ((arr->TYPE == VARIABLE_ARRAY) && (arrData) && (index >= 0)) {
        res = 0;
  #ifdef OPTIMIZE_FAST_ARRAYS
        VariableDATA **cached_data = arrData->cached_data;
        VariableDATA *data         = ((index < STATIC_ARRAY_THRESHOLD) && (cached_data)) ? cached_data[index] : 0;
        if (!data)
            data = arrData->GetWithCreate(index);
  #else
        VariableDATA *data = arrData->GetWithCreate(index);
  #endif
        int type = data->TYPE;

        if (type == VARIABLE_NUMBER) {
            return data->NUMBER_DATA ? 1 : 0;
        } else
        if (type == VARIABLE_STRING) {
            if ((data->CLASS_DATA) && (((AnsiString *)data->CLASS_DATA)->Length()))
                res = 1;
            return res;
        } else
        if (type == VARIABLE_ARRAY) {
            if ((data->CLASS_DATA) && (((Array *)data->CLASS_DATA)->Count()))
                res = 1;
            return res;
        } else
            res = 1;
    }
    return res;
}

static sljit_sw SLJIT_CALL iArrayDataVDIF(sljit_sw index, VariableDATA *arr) {
    sljit_sw res = 2;

    if ((arr->TYPE == VARIABLE_ARRAY) && (arr->CLASS_DATA) && (index >= 0)) {
        res = 0;
  #ifdef OPTIMIZE_FAST_ARRAYS
        VariableDATA *data = FAST_CHECK((Array *)arr->CLASS_DATA, index);
        if (!data)
            data = ((Array *)arr->CLASS_DATA)->GetWithCreate(index);
  #else
        VariableDATA *data = ((Array *)arr->CLASS_DATA)->GetWithCreate(index);
  #endif
        int type = data->TYPE;

        if (type == VARIABLE_NUMBER) {
            return data->NUMBER_DATA ? 1 : 0;
        } else
        if (type == VARIABLE_STRING) {
            if ((data->CLASS_DATA) && (((AnsiString *)data->CLASS_DATA)->Length()))
                res = 1;
            return res;
        } else
        if (type == VARIABLE_ARRAY) {
            if ((data->CLASS_DATA) && (((Array *)data->CLASS_DATA)->Count()))
                res = 1;
            return res;
        } else
            res = 1;
    }
    return res;
}

static sljit_sw SLJIT_CALL ArraySETREGION(VariableDATA **data, RuntimeOptimizedElement *CODE, sljit_sw i) {
    int      target        = CODE[i].OperandLeft.ID - 1;
    int      limit         = CODE[i + 1].OperandRight.ID - 1;
    int      arr           = CODE[i + 3].OperandLeft.ID - 1;
    int      value         = CODE[i + 4].OperandRight.ID - 1;
    int      increment     = -1;
    intptr_t increment_val = 1;

    if (CODE[i + 5].Operator.ID == KEY_ASU)
        increment = CODE[i + 5].OperandRight.ID - 1;

    VariableDATA *arr_var = data[arr];
    VariableDATA *val_var = data[value];

    if ((!data[target]) || (!data[limit]) || (!arr_var) || (!val_var))
        return 0;

    if ((arr_var->TYPE != VARIABLE_ARRAY) || (!arr_var->CLASS_DATA))
        return 0;

    if (increment > 0) {
        if (!data[increment])
            return 0;
        double2int(increment_val, data[increment]->NUMBER_DATA);
    }
    if (!increment_val)
        return 0;

    intptr_t iterator;
    double2int(iterator, data[target]->NUMBER_DATA);
    NUMBER limit_val = data[limit]->NUMBER_DATA;

    Array *arr_object = (Array *)arr_var->CLASS_DATA;

    VariableDATA **cached_data = arr_object->cached_data;
    if (iterator < 0)
        return 0;

    if (increment_val == 1) {
        for ( ; iterator < limit_val; ++iterator) {
  #ifdef OPTIMIZE_FAST_ARRAYS
            VariableDATA *data_var = ((iterator < STATIC_ARRAY_THRESHOLD) && (cached_data)) ? cached_data[iterator] : 0;
            if (!data_var)
                data_var = arr_object->GetWithCreate(iterator, val_var->NUMBER_DATA);
  #else
            VariableDATA *data_var = arr_object->GetWithCreate(iterator, val_var->NUMBER_DATA);
  #endif
            if (data_var->TYPE == VARIABLE_NUMBER) {
                data_var->NUMBER_DATA = val_var->NUMBER_DATA;
            } else
            if (data_var->TYPE != VARIABLE_CLASS) {
                RESET_VARIABLE(data_var);
                data_var->NUMBER_DATA = val_var->NUMBER_DATA;
            } else {
                data[target]->NUMBER_DATA = iterator;
                return 0;
            }
        }
    } else {
        for ( ; iterator < limit_val; iterator += increment_val) {
            // to do
            if (iterator < 0) {
                data[target]->NUMBER_DATA = iterator;
                return 0;
            }

  #ifdef OPTIMIZE_FAST_ARRAYS
            VariableDATA *data_var = ((iterator < STATIC_ARRAY_THRESHOLD) && (cached_data)) ? cached_data[iterator] : 0;
            if (!data_var)
                data_var = arr_object->GetWithCreate(iterator, val_var->NUMBER_DATA);
  #else
            VariableDATA *data_var = arr_object->GetWithCreate(iterator, val_var->NUMBER_DATA);
  #endif
            if (data_var->TYPE == VARIABLE_NUMBER) {
                data_var->NUMBER_DATA = val_var->NUMBER_DATA;
            } else
            if (data_var->TYPE != VARIABLE_CLASS) {
                RESET_VARIABLE(data_var);
                data_var->NUMBER_DATA = val_var->NUMBER_DATA;
            } else {
                data[target]->NUMBER_DATA = iterator;
                return 0;
            }
        }
    }
    data[target]->NUMBER_DATA = iterator;
    return 1;
}

static sljit_sw SLJIT_CALL ArrayDataASGVD(VariableDATA *arr, VariableDATA *idx, VariableDATA *newvalue) {
    int index;

    double2int(index, (idx->NUMBER_DATA));

    Array *arrData = (Array *)arr->CLASS_DATA;
    if ((arr->TYPE == VARIABLE_ARRAY) && (arrData) && (index >= 0)) {
  #ifdef OPTIMIZE_FAST_ARRAYS
        VariableDATA **cached_data = arrData->cached_data;
        VariableDATA *data         = ((index < STATIC_ARRAY_THRESHOLD) && (cached_data)) ? cached_data[index] : 0;
        if (!data)
            data = arrData->GetWithCreate(index);
  #else
        VariableDATA *data = arrData->GetWithCreate(index);
  #endif
        if (data->TYPE == VARIABLE_NUMBER) {
            data->NUMBER_DATA = newvalue->NUMBER_DATA;
            return 1;
        } else
        if (data->TYPE != VARIABLE_CLASS) {
            RESET_VARIABLE(data);
            data->NUMBER_DATA = newvalue->NUMBER_DATA;
            return 1;
        }
    }
    return 0;
}

static sljit_sw SLJIT_CALL c_ARRAYELEMENTINIT(VariableDATA *start, VariableDATA *limit, VariableDATA *arr) {
    INTEGER from;
    INTEGER to;

    double2int(from, (start->NUMBER_DATA));
    double2int(to, (limit->NUMBER_DATA));
    NUMBER newvalue = 1;
    if ((arr->TYPE == VARIABLE_ARRAY) && (arr->CLASS_DATA) && (from >= 0)) {
        for (int index = from; index < to; index++) {
  #ifdef OPTIMIZE_FAST_ARRAYS
            VariableDATA *data = FAST_CHECK((Array *)arr->CLASS_DATA, index);
            if (!data)
                data = ((Array *)arr->CLASS_DATA)->GetWithCreate(index);
  #else
            VariableDATA *data = ((Array *)arr->CLASS_DATA)->GetWithCreate(index);
  #endif
            if (data->TYPE == VARIABLE_NUMBER) {
                data->NUMBER_DATA = newvalue;
            } else
            if (data->TYPE != VARIABLE_CLASS) {
                RESET_VARIABLE(data);
                data->NUMBER_DATA = newvalue;
            } else
                return 0;
        }
        return 1;
    }
    return 0;
}

static sljit_sw SLJIT_CALL LengthVD(VariableDATA *target, VariableDATA *result) {
    switch (target->TYPE) {
        case VARIABLE_ARRAY:
            if (target->CLASS_DATA) {
                RESET_VARIABLE(result);
                result->NUMBER_DATA = ((Array *)target->CLASS_DATA)->Count();
                return 1;
            }
            break;

        case VARIABLE_STRING:
            if (target->CLASS_DATA) {
                RESET_VARIABLE(result);
                result->NUMBER_DATA = ((AnsiString *)target->CLASS_DATA)->Length();
                return 1;
            } else {
                RESET_VARIABLE(result)
                result->NUMBER_DATA = 0;
                return 1;
            }
            break;
    }
    return 0;
}

static sljit_sw SLJIT_CALL ReplaceVariable(VariableDATA **a, sljit_sw index1, VariableDATA *d) {
    if (a[index1] != d) {
        FREE_VARIABLE(a[index1]);
        a[index1] = d;
        d->LINKS++;
    }
    return 0;
}

  #ifdef JIT_RUNTIME_CHECKS
static int JITRuntimeError(VariableDATA *var, const char *err, int code) {
    PIFAlizator *PIF = (PIFAlizator *)GetPOOLContext(var);

    if (PIF) {
        GCRoot  *root        = ((PIFAlizator *)PIF)->GCROOT;
        SCStack *STACK_TRACE = NULL;
        if (root)
            STACK_TRACE = (SCStack *)root->STACK_TRACE;

        if (STACK_TRACE) {
            ClassMember *CM = NULL;
            if (STACK_TRACE->TOP)
                CM = (ClassMember *)((SCStack *)(STACK_TRACE->TOP))->CM;
            TinyString def("JIT");
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(err, STACK_TRACE->line, code, "", CM ? ((ClassCode *)CM->GetClass())->GetFilename(PIF, ((ClassCode *)CM->GetClass())->GetCLSID(), &def)->c_str() : "", CM ? ((ClassCode *)CM->GetClass())->GetName() : "", CM ? CM->GetName() : ""));
        } else
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(err, 0, code, 0, "JIT"));
        return 1;
    }
    return 0;
}

static sljit_sw SLJIT_CALL c_divCHECK(VariableDATA *left, VariableDATA *right) {
    if (right->NUMBER_DATA == 0)
        JITRuntimeError(right, ERR260, 260);
    return 0;
}
  #endif

static sljit_sw SLJIT_CALL c_modVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    sljit_sw o2 = (sljit_sw)right->NUMBER_DATA;

    if (o2)
        result->NUMBER_DATA = (sljit_sw)left->NUMBER_DATA % o2;
    else {
  #ifdef JIT_RUNTIME_CHECKS
        JITRuntimeError(right, ERR260, 260);
  #endif
        if (left->NUMBER_DATA < 0)
            result->NUMBER_DATA = -HUGE_VAL;
        else
            result->NUMBER_DATA = HUGE_VAL;
    }
    return 0;
}

static sljit_sw SLJIT_CALL c_modVD2(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    sljit_sw o2 = (sljit_sw)right->NUMBER_DATA;

    if (o2)
        left->NUMBER_DATA = (sljit_sw)left->NUMBER_DATA % o2;
    else {
  #ifdef JIT_RUNTIME_CHECKS
        JITRuntimeError(right, ERR260, 260);
  #endif
        if (left->NUMBER_DATA < 0)
            left->NUMBER_DATA = -HUGE_VAL;
        else
            left->NUMBER_DATA = HUGE_VAL;
    }
    result->NUMBER_DATA = left->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_AANVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA & (unsigned INTEGER)right->NUMBER_DATA;
    result->NUMBER_DATA = left->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_AXOVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA ^ (unsigned INTEGER)right->NUMBER_DATA;
    result->NUMBER_DATA = left->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_AORVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA | (unsigned INTEGER)right->NUMBER_DATA;
    result->NUMBER_DATA = left->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_ASLVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA << (unsigned INTEGER)right->NUMBER_DATA;
    result->NUMBER_DATA = left->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_ASRVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA >> (unsigned INTEGER)right->NUMBER_DATA;
    result->NUMBER_DATA = left->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_COMVD(VariableDATA *left, VariableDATA *result) {
    result->NUMBER_DATA = ~(unsigned INTEGER)left->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_SHLVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    result->NUMBER_DATA = (unsigned INTEGER)left->NUMBER_DATA << (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_SHRVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    result->NUMBER_DATA = (unsigned INTEGER)left->NUMBER_DATA >> (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_ANDVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    result->NUMBER_DATA = (unsigned INTEGER)left->NUMBER_DATA & (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_XORVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    result->NUMBER_DATA = (unsigned INTEGER)left->NUMBER_DATA ^ (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_ORVD(VariableDATA *left, VariableDATA *right, VariableDATA *result) {
    result->NUMBER_DATA = (unsigned INTEGER)left->NUMBER_DATA | (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}
 #endif
}
#endif

ConceptInterpreter::ConceptInterpreter(Optimizer *O, INTEGER LocalClsID, ClassMember *Owner) {
    LocalClassID = LocalClsID;
    OWNER        = Owner;
#ifdef USE_JIT_TRACE
    callcount    = 0;
    jittracecode = 0;
#endif
}

#define DECLARE_PATH(TYPE)

#ifdef USE_JIT_TRACE
 #define OFFSETOF(type, field)    ((uintptr_t)&(((type *)0)->field))

 #define OPERAND_LEFT       if (reg1 != OE->OperandLeft.ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandLeft.ID - 1)); reg1 = OE->OperandLeft.ID; }
 #define OPERAND_RIGHT      if (reg2 != OE->OperandRight.ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandRight.ID - 1)); reg2 = OE->OperandRight.ID; }
 #define OPERAND_RESULT     if (reg3 != OE->Result_ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->Result_ID - 1)); reg3 = OE->Result_ID; }
 #define OPERAND_RESULT2    if (reg2 != OE->Result_ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->Result_ID - 1)); reg2 = OE->Result_ID; }
 #define OPERAND_LEFT2      if (reg2 != OE->OperandLeft.ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandLeft.ID - 1)); reg2 = OE->OperandLeft.ID; }
 #define OPERAND_RIGHT2     if (reg1 != OE->OperandRight.ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandRight.ID - 1)); reg1 = OE->OperandRight.ID; }

void ConceptInterpreter::AnalizeInstructionPath(Optimizer *OPT) {
    int count = OPT->codeCount;

    if (!count)
        return;
    struct sljit_compiler *compiler = 0;
    int dataCount           = OPT->dataCount;
    int instruction_pointer = 1;
    RuntimeOptimizedElement *OE;
    jittracecode = (void **)malloc(sizeof(void *) * count);
    memset(jittracecode, 0, sizeof(void *) * count);
    static sljit_d d_true     = 1;
    static sljit_d d_false    = 0;
    char           *dataflags = dataCount ? (char *)malloc(dataCount) : 0;
    char           *usedflags = dataCount ? (char *)malloc(dataCount) : 0;
    memset(usedflags, 0, sizeof(char) * dataCount);
    int  delta1       = count + 2;
    char *outsidejump = (char *)malloc(delta1);
    memset(outsidejump, 0, sizeof(char) * delta1);
    int       is_outsidejump = 0;
    int       prec_is_new    = 0;
    ParamList *FORMAL_PARAMETERS;
    ParamList FORMAL_PARAMETERS2;
    RuntimeOptimizedElement *OENext = 0;
    executable_code         code;
    struct sljit_jump       *jump;
    struct sljit_jump       *jump2;
    int icode;
    int icode_idx;
    int reg1;
    int reg2;
    int reg3;

    INTEGER ParamCount = this->OWNER->PARAMETERS_COUNT;
    for (int i = 1; i <= ParamCount; i++) {
        RuntimeVariableDESCRIPTOR *TARGET = &OPT->DATA [i];
        if (TARGET->BY_REF)
            usedflags[i] = 2;
    }
    for (int i = 0; i < count; i++) {
        OE = &OPT->CODE[i];
        if (OE->Operator.TYPE == TYPE_OPERATOR) {
            switch (OE->Operator.ID) {
                case KEY_SEL:
                    if (OE->OperandReserved.ID) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved.ID - 1];
                        if (FORMAL_PARAMETERS) {
                            FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                            FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);
                            if (FORMAL_PARAMETERS2.PARAM_INDEX) {
                                for (int j = 0; j < FORMAL_PARAMETERS2.COUNT; j++) {
                                    int idx = FORMAL_PARAMETERS2.PARAM_INDEX[j] - 1;
                                    if (usedflags[idx])
                                        usedflags[idx] = 2;
                                    else
                                        usedflags[idx] = 1;
                                }
                            }
                        }
                    }

                case KEY_DELETE:
                case KEY_TYPE_OF:
                case KEY_CLASS_NAME:
                case KEY_LENGTH:
                    if (usedflags[OE->OperandLeft.ID - 1])
                        usedflags[OE->OperandLeft.ID - 1] = 2;
                    else
                        usedflags[OE->OperandLeft.ID - 1] = 1;
                    break;

                case KEY_NEW:
                    if ((OE->OperandReserved.ID > 0) && (OE->OperandLeft.ID != STATIC_CLASS_ARRAY)) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved.ID - 1];
                        if (FORMAL_PARAMETERS) {
                            FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                            FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);

                            if (FORMAL_PARAMETERS2.PARAM_INDEX) {
                                for (int j = 0; j < FORMAL_PARAMETERS2.COUNT; j++) {
                                    int idx = FORMAL_PARAMETERS2.PARAM_INDEX[j] - 1;
                                    if (usedflags[idx])
                                        usedflags[idx] = 2;
                                    else
                                        usedflags[idx] = 1;
                                }
                            }
                        }
                    }
                    break;

                case KEY_DLL_CALL:
                    if (OE->OperandReserved.ID) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved.ID - 1];
                        if (FORMAL_PARAMETERS) {
                            FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                            FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);

                            if (FORMAL_PARAMETERS2.PARAM_INDEX) {
                                for (int j = 0; j < FORMAL_PARAMETERS2.COUNT; j++) {
                                    int idx = FORMAL_PARAMETERS2.PARAM_INDEX[j] - 1;
                                    if (usedflags[idx])
                                        usedflags[idx] = 2;
                                    else
                                        usedflags[idx] = 1;
                                }
                            }
                        }
                    }
                    break;

                case KEY_ASG:
                case KEY_BY_REF:
                case KEY_ASU:
                case KEY_ADI:
                case KEY_AMU:
                case KEY_MUL:
                case KEY_ADV:
                case KEY_DIV:
                case KEY_SUM:
                case KEY_DIF:
                case KEY_LES:
                case KEY_LEQ:
                case KEY_GRE:
                case KEY_GEQ:
                case KEY_EQU:
                case KEY_NEQ:
                case KEY_CND_NULL:
                case KEY_REM:
                case KEY_ARE:
                case KEY_BOR:
                case KEY_BAN:
                case KEY_AAN:
                case KEY_AND:
                case KEY_AXO:
                case KEY_AOR:
                case KEY_ASL:
                case KEY_ASR:
                case KEY_SHL:
                case KEY_SHR:
                case KEY_XOR:
                case KEY_OR:
                case KEY_INDEX_OPEN:
                    if (usedflags[OE->OperandRight.ID - 1])
                        usedflags[OE->OperandRight.ID - 1] = 2;
                    else
                        usedflags[OE->OperandRight.ID - 1] = 1;
                    if (usedflags[OE->OperandLeft.ID - 1])
                        usedflags[OE->OperandLeft.ID - 1] = 2;
                    else
                        usedflags[OE->OperandLeft.ID - 1] = 1;
                    break;

                case KEY_INC:
                case KEY_DEC:
                case KEY_INC_LEFT:
                case KEY_DEC_LEFT:
                case KEY_POZ:
                case KEY_VALUE:
                case KEY_NOT:
                case KEY_NEG:
                case KEY_COM:
                    if (usedflags[OE->OperandLeft.ID - 1])
                        usedflags[OE->OperandLeft.ID - 1] = 2;
                    else
                        usedflags[OE->OperandLeft.ID - 1] = 1;
                    break;
            }
        } else
        if (OE->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) {
            switch (OE->Operator.ID) {
                case KEY_OPTIMIZED_THROW:
                case KEY_OPTIMIZED_RETURN:
                case KEY_OPTIMIZED_ECHO:
                case KEY_OPTIMIZED_IF:
                    if (usedflags[OE->OperandRight.ID - 1])
                        usedflags[OE->OperandRight.ID - 1] = 2;
                    else
                        usedflags[OE->OperandRight.ID - 1] = 1;
                    // optimize IF .. GOTO to GOTO
                    if ((OE->Operator.ID == KEY_OPTIMIZED_IF) && (OE->OperandReserved.ID < count)) {
                        RuntimeOptimizedElement *OE2 = &OPT->CODE[OE->OperandReserved.ID];
                        while ((OE2->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) && (OE2->Operator.ID == KEY_OPTIMIZED_GOTO)) {
                            OE->OperandReserved.ID = OE2->OperandReserved.ID;
                            if (OE2->OperandReserved.ID >= count)
                                break;
                            OE2 = &OPT->CODE[OE2->OperandReserved.ID];
                        }
                    }
                    break;
            }
        }
    }
    while (instruction_pointer <= count) {
        int start = instruction_pointer - 1;
        // already optimized
        if (jittracecode[start])
            break;

        bool optimizable = true;
        while (instruction_pointer <= count) {
            OE = &OPT->CODE[instruction_pointer - 1];
            if (OE->Operator.TYPE == TYPE_OPERATOR) {
                switch (OE->Operator.ID) {
                    case KEY_NEW:
                        prec_is_new = OE->Result_ID;
                        optimizable = false;
                        break;

                    case KEY_DELETE:
                    case KEY_SEL:
                    case KEY_TYPE_OF:
                    case KEY_CLASS_NAME:
                    case KEY_DLL_CALL:
                        optimizable = false;
                        prec_is_new = false;
                        break;

                    case KEY_ASG:
                    case KEY_BY_REF:
                        if (prec_is_new) {
                            optimizable = false;
                            prec_is_new = 0;
                        }

                    case KEY_ASU:
                    case KEY_ADI:
                    case KEY_AMU:
                    case KEY_MUL:
                    case KEY_ADV:
                    case KEY_DIV:
                    case KEY_SUM:
                    case KEY_DIF:
                    case KEY_LES:
                    case KEY_LEQ:
                    case KEY_GRE:
                    case KEY_GEQ:
                    case KEY_EQU:
                    case KEY_NEQ:
                    case KEY_CND_NULL:
                    case KEY_REM:
                    case KEY_ARE:
                    case KEY_BOR:
                    case KEY_BAN:
                    case KEY_AAN:
                    case KEY_AND:
                    case KEY_AXO:
                    case KEY_AOR:
                    case KEY_ASL:
                    case KEY_ASR:
                    case KEY_SHL:
                    case KEY_SHR:
                    case KEY_XOR:
                    case KEY_OR:
                        if ((OPT->DATA[OE->OperandRight.ID - 1].TYPE != VARIABLE_NUMBER) && (OPT->DATA[OE->OperandRight.ID - 1].TYPE != -VARIABLE_NUMBER)) {
                            optimizable = false;
                            prec_is_new = false;
                            break;
                        }

                    case KEY_INC:
                    case KEY_DEC:
                    case KEY_INC_LEFT:
                    case KEY_DEC_LEFT:
                    case KEY_POZ:
                    case KEY_VALUE:
                    case KEY_NOT:
                    case KEY_NEG:
                    case KEY_COM:
                        if ((OPT->DATA[OE->OperandLeft.ID - 1].TYPE != VARIABLE_NUMBER) && (OPT->DATA[OE->OperandLeft.ID - 1].TYPE != -VARIABLE_NUMBER)) {
                            optimizable = false;
                            prec_is_new = false;
                            break;
                        }
                        if ((OPT->DATA[OE->Result_ID - 1].TYPE != VARIABLE_NUMBER) && (OPT->DATA[OE->Result_ID - 1].TYPE != -VARIABLE_NUMBER)) {
                            optimizable = false;
                            prec_is_new = false;
                            break;
                        }
                        break;

                    case KEY_INDEX_OPEN:
                        if ((OPT->DATA[OE->OperandRight.ID - 1].TYPE != VARIABLE_NUMBER) && (OPT->DATA[OE->OperandRight.ID - 1].TYPE != -VARIABLE_NUMBER)) {
                            optimizable = false;
                            prec_is_new = false;
                            break;
                        }
                        break;
                }
            } else
            if (OE->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) {
                switch (OE->Operator.ID) {
                    case KEY_OPTIMIZED_ECHO:
                    case KEY_OPTIMIZED_TRY_CATCH:
                    case KEY_OPTIMIZED_END_CATCH:
                    case KEY_OPTIMIZED_DEBUG_TRAP:
                    case KEY_OPTIMIZED_THROW:
                    case KEY_OPTIMIZED_RETURN:
                        optimizable = false;
                        prec_is_new = false;
                        break;

                    case KEY_OPTIMIZED_IF:
                        if ((OPT->DATA[OE->OperandRight.ID - 1].TYPE != VARIABLE_NUMBER) && (OPT->DATA[OE->OperandRight.ID - 1].TYPE != -VARIABLE_NUMBER)) {
                            optimizable = false;
                            prec_is_new = false;
                            break;
                        }

                    // don't break here
                    case KEY_OPTIMIZED_GOTO:
                        if (OE->OperandReserved.ID < start)
                            outsidejump[OE->OperandReserved.ID] = 1;
                        break;
                }
            }
            instruction_pointer++;

            if (!optimizable)
                break;
            prec_is_new = 0;
        }

        // at least 2 instructions for optimizer
        int end = instruction_pointer - 1;
        if (!optimizable)
            end--;
        int delta = end - start;
        if (delta > 1) {
            int  delta2       = delta + 1;
            int  *labels      = (int *)malloc(sizeof(int) * delta2);
            int  *posj        = (int *)malloc(sizeof(int) * delta2);
            void ***jumps     = (void ***)malloc(sizeof(void **) * delta2);
            void ***labelsptr = (void ***)malloc(sizeof(void **) * delta2);
            int  *lrptr       = (int *)malloc(sizeof(int) * delta2);
            for (int i = 0; i < delta2; i++) {
                labels[i]    = 0;
                jumps[i]     = 0;
                labelsptr[i] = 0;
                posj[i]      = 0;
                lrptr[i]     = 0;
            }

            for (int i = start; i < end; i++) {
                RuntimeOptimizedElement *OE = &OPT->CODE[i];
                if (OE) {
                    if (OE->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) {
                        switch (OE->Operator.ID) {
                            case KEY_OPTIMIZED_GOTO:
                            case KEY_OPTIMIZED_IF:
                                if ((OE->OperandReserved.ID >= start) && (OE->OperandReserved.ID < end)) {
                                    labels[OE->OperandReserved.ID - start]++;
                                    if ((OE->Operator.ID == KEY_OPTIMIZED_GOTO) && (i > start)) {
                                        OENext = &OPT->CODE[i - 1];
                                        if ((OENext->Operator.TYPE == TYPE_OPERATOR) && (OENext->OperandLeft.TYPE == TYPE_VARIABLE))
                                            lrptr[OE->OperandReserved.ID - start] = OENext->OperandLeft.ID;
                                    }
                                }
                                break;
                        }
                    }
                }
            }

            compiler = sljit_create_compiler();
 #ifdef ARM_PATCH
            sljit_emit_enter(compiler, 0, 1, 4, 3, 0, 0, 0);
 #else
            sljit_emit_enter(compiler, 0, 1, 3, 3, 0, 0, 0);
 #endif
            if (dataflags) {
                memset(dataflags, 0, sizeof(char) * dataCount);
                for (int i = start; i < end; i++) {
                    OE = &OPT->CODE[i];
                    if (OE->Operator.TYPE == TYPE_OPERATOR) {
                        switch (OE->Operator.ID) {
                            case KEY_BY_REF:
                                if (dataflags[OE->OperandLeft.ID - 1])
                                    dataflags[OE->OperandLeft.ID - 1] = 2;
                                else
                                    dataflags[OE->OperandLeft.ID - 1] = 1;
                                dataflags[OE->OperandRight.ID - 1] = 1;
                                if (usedflags[OE->Result_ID - 1])
                                    dataflags[OE->Result_ID - 1] = 1;
                                break;

                            case KEY_ASG:
                            case KEY_ASU:
                            case KEY_ADI:
                            case KEY_AMU:
                            case KEY_ADV:
                            case KEY_MUL:
                            case KEY_DIV:
                            case KEY_SUM:
                            case KEY_DIF:
                            case KEY_LES:
                            case KEY_LEQ:
                            case KEY_GRE:
                            case KEY_GEQ:
                            case KEY_EQU:
                            case KEY_NEQ:
                            case KEY_CND_NULL:
                            case KEY_REM:
                            case KEY_ARE:
                            case KEY_BOR:
                            case KEY_BAN:
                            case KEY_AAN:
                            case KEY_AND:
                            case KEY_AXO:
                            case KEY_AOR:
                            case KEY_ASL:
                            case KEY_ASR:
                            case KEY_SHL:
                            case KEY_SHR:
                            case KEY_XOR:
                            case KEY_OR:
                                dataflags[OE->OperandLeft.ID - 1]  = 1;
                                dataflags[OE->OperandRight.ID - 1] = 1;
                                if (usedflags[OE->Result_ID - 1])
                                    dataflags[OE->Result_ID - 1] = 1;
                                break;

                            case KEY_INC:
                            case KEY_DEC:
                            case KEY_INC_LEFT:
                            case KEY_DEC_LEFT:
                            case KEY_POZ:
                            case KEY_VALUE:
                            case KEY_NOT:
                            case KEY_NEG:
                            case KEY_COM:
                                dataflags[OE->OperandLeft.ID - 1] = 1;
                                if (usedflags[OE->Result_ID - 1])
                                    dataflags[OE->Result_ID - 1] = 1;
                                break;

                            case KEY_INDEX_OPEN:
                                //left will be checked at runtime
                                dataflags[OE->OperandRight.ID - 1] = 1;
                                if (usedflags[OE->Result_ID - 1])
                                    dataflags[OE->Result_ID - 1] = 1;
                                break;
                        }
                    } else
                    if ((OE->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) && (OE->Operator.ID == KEY_OPTIMIZED_IF))
                        dataflags[OE->OperandRight.ID - 1] = 1;
                }
                int buf[2];
                int buf_idx = 0;
                for (int i = 0; i < dataCount; i++) {
                    if (dataflags[i]) {
                        if (buf_idx == 2) {
                            buf_idx = 0;

                            sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * buf[0]);
                            sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * buf[1]);
                            sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * i);
 #ifdef ARM_PATCH
                            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_CHECKTYPES3));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_CHECKTYPES3));
 #endif
                            jump = sljit_emit_cmp(compiler, SLJIT_EQUAL,
                                                  SLJIT_RETURN_REG, 0,
                                                  SLJIT_IMM, 1);

                            sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, start);
                            sljit_set_label(jump, sljit_emit_label(compiler));
                        } else
                            buf[buf_idx++] = i;
                    }
                }
                if (buf_idx == 2) {
                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * buf[0]);
                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * buf[1]);
 #ifdef ARM_PATCH
                    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_CHECKTYPES2));
                    sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_TEMPORARY_EREG1, 0);
 #else
                    sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_CHECKTYPES2));
 #endif
                    jump = sljit_emit_cmp(compiler, SLJIT_EQUAL,
                                          SLJIT_RETURN_REG, 0,
                                          SLJIT_IMM, 1);

                    sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, start);
                    sljit_set_label(jump, sljit_emit_label(compiler));
                } else
                if (buf_idx == 1) {
                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * buf[0]);
 #ifdef ARM_PATCH
                    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_CHECKTYPES1));
                    sljit_emit_ijump(compiler, SLJIT_CALL1, SLJIT_TEMPORARY_EREG1, 0);
 #else
                    sljit_emit_ijump(compiler, SLJIT_CALL1, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_CHECKTYPES1));
 #endif
                    jump = sljit_emit_cmp(compiler, SLJIT_EQUAL,
                                          SLJIT_RETURN_REG, 0,
                                          SLJIT_IMM, 1);

                    sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, start);
                    sljit_set_label(jump, sljit_emit_label(compiler));
                }
            }

            reg1 = 0;
            reg2 = 0;
            reg3 = 0;

            for (int i = start; i < end; i++) {
                OE = &OPT->CODE[i];
                int idx = i - start;
                int cnt = labels[idx];
                if (cnt) {
                    labelsptr[idx] = (void **)malloc(sizeof(void *) * cnt);
                    for (int j = 0; j < cnt; j++)
                        labelsptr[idx][j] = (void *)sljit_emit_label(compiler);
                    if (cnt == 1) {
                        if (reg1 != lrptr[idx])
                            reg1 = 0;
                    } else
                        reg1 = 0;
                    reg2 = 0;
                    reg3 = 0;
                }

                if (OE->Operator.TYPE == TYPE_OPERATOR) {
                    switch (OE->Operator.ID) {
                        case KEY_BY_REF:
                        case KEY_ASG:
                            OPERAND_LEFT
                            if (reg3 == OE->OperandRight.ID) {
                                if (usedflags[OE->Result_ID - 1])
                                    OPERAND_RESULT2

                                        sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                        SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                        SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA));

                                if (usedflags[OE->Result_ID - 1])
                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA));
                            } else {
                                OPERAND_RIGHT
                                if (usedflags[OE->Result_ID - 1])
                                    OPERAND_RESULT

                                        sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                        SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                        SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                if (usedflags[OE->Result_ID - 1])
                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));
                            }
                            if (end - i >= 7) {
                                // detect array init
                                if ((OPT->CODE[i + 1].Operator.TYPE == TYPE_OPERATOR) && (OPT->CODE[i + 1].Operator.ID == KEY_LES) && (OPT->CODE[i + 1].OperandLeft.ID == OE->OperandLeft.ID) &&
                                    (OPT->CODE[i + 2].Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) && (OPT->CODE[i + 2].Operator.ID == KEY_OPTIMIZED_IF) && (OPT->CODE[i + 1].Result_ID == OPT->CODE[i + 2].OperandRight.ID) &&
                                    (OPT->CODE[i + 3].Operator.TYPE == TYPE_OPERATOR) && (OPT->CODE[i + 3].Operator.ID == KEY_INDEX_OPEN) && (OPT->CODE[i + 3].OperandRight.ID == OE->OperandLeft.ID) &&
                                    (OPT->CODE[i + 4].Operator.TYPE == TYPE_OPERATOR) && ((OPT->CODE[i + 4].Operator.ID == KEY_ASG) || (OPT->CODE[i + 4].Operator.ID == KEY_BY_REF)) && (OPT->CODE[i + 3].Result_ID == OPT->CODE[i + 4].OperandLeft.ID) && (OE->OperandLeft.ID != OPT->CODE[i + 4].OperandRight.ID) &&
                                    (OPT->CODE[i + 5].Operator.TYPE == TYPE_OPERATOR) && ((OPT->CODE[i + 5].Operator.ID == KEY_INC) || (OPT->CODE[i + 5].Operator.ID == KEY_INC_LEFT) || ((OPT->CODE[i + 5].Operator.ID == KEY_ASU) && (OPT->CODE[i + 5].OperandLeft.ID != OPT->CODE[i + 5].OperandRight.ID))) && (OPT->CODE[i + 5].OperandLeft.ID == OE->OperandLeft.ID) &&
                                    (OPT->CODE[i + 6].Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) && (OPT->CODE[i + 6].Operator.ID == KEY_OPTIMIZED_GOTO) && (OPT->CODE[i + 6].OperandReserved.ID == i + 1)) {
                                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_S0, 0);
                                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)OPT->CODE);
                                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_IMM, (sljit_sw)i);
 #ifdef ARM_PATCH
                                    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArraySETREGION));
                                    sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                                    sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArraySETREGION));
 #endif
                                    struct sljit_jump *jump = sljit_emit_cmp(compiler, SLJIT_NOT_EQUAL, SLJIT_RETURN_REG, 0, SLJIT_IMM, 0);
                                    sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, i + 1);
                                    sljit_set_label(jump, sljit_emit_label(compiler));

                                    reg1 = 0;
                                    reg2 = 0;
                                    reg3 = 0;
                                    i   += 6;
                                }
                            }
                            break;

                        case KEY_INC:
                            OPERAND_LEFT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                                    sljit_emit_fop2(compiler, SLJIT_DADD,
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                            break;

                        case KEY_DEC:
                            OPERAND_LEFT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                                    sljit_emit_fop2(compiler, SLJIT_DSUB,
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            break;

                        case KEY_INC_LEFT:
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT
                                    OPERAND_LEFT

                                if (usedflags[OE->Result_ID - 1])
                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            sljit_emit_fop2(compiler, SLJIT_DADD,
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);
                            break;

                        case KEY_DEC_LEFT:
                            OPERAND_LEFT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                                if (usedflags[OE->Result_ID - 1])
                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            sljit_emit_fop2(compiler, SLJIT_DSUB,
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);

                            break;

                        case KEY_ASU:
                            OPERAND_LEFT
                                OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                                sljit_emit_fop2(compiler, SLJIT_DADD,
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            break;

                        case KEY_ADI:
                            OPERAND_LEFT
                                OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                                sljit_emit_fop2(compiler, SLJIT_DSUB,
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            break;

                        case KEY_AMU:
                            OPERAND_LEFT
                                OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                                sljit_emit_fop2(compiler, SLJIT_DMUL,
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            break;

                        case KEY_ADV:
                            OPERAND_LEFT
                                OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                                sljit_emit_fop2(compiler, SLJIT_DDIV,
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

 #ifdef JIT_RUNTIME_CHECKS
  #ifdef ARM_PATCH
                            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_divCHECK));
                            sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_TEMPORARY_EREG1, 0);
  #else
                            sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_divCHECK));
  #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
 #endif
                            break;

                        case KEY_POZ:
                        case KEY_VALUE:
                            OPERAND_RESULT
                            OPERAND_LEFT

                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                            break;

                        case KEY_MUL:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                                OPERAND_LEFT
                            if (OE->OperandLeft.ID != OE->OperandRight.ID) {
                                OPERAND_RIGHT

                                sljit_emit_fop2(compiler, SLJIT_DMUL,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));
                            } else {
                                sljit_emit_fop2(compiler, SLJIT_DMUL,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                            }
                            break;

                        case KEY_DIV:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                            OPERAND_LEFT
                            OPERAND_RIGHT

                                sljit_emit_fop2(compiler, SLJIT_DDIV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));
 #ifdef JIT_RUNTIME_CHECKS
  #ifdef ARM_PATCH
                            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_divCHECK));
                            sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_TEMPORARY_EREG1, 0);
  #else
                            sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_divCHECK));
  #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
 #endif
                            break;

                        case KEY_SUM:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                                OPERAND_LEFT
                            if (OE->OperandLeft.ID != OE->OperandRight.ID) {
                                OPERAND_RIGHT

                                sljit_emit_fop2(compiler, SLJIT_DADD,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));
                            } else {
                                sljit_emit_fop2(compiler, SLJIT_DADD,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                            }
                            break;

                        case KEY_DIF:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                            if (OE->OperandLeft.ID != OE->OperandRight.ID) {
                                OPERAND_LEFT
                                OPERAND_RIGHT

                                    sljit_emit_fop2(compiler, SLJIT_DSUB,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));
                            } else {
                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);
                            }
                            break;

                        case KEY_LES:
                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved.ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator.ID == KEY_OPTIMIZED_IF) && (OENext->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) &&
                                (OENext->OperandRight.ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_D_GREATER_EQUAL,
                                                           SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                           SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                                i++;
                            } else {
                                OPERAND_RESULT
                                OPERAND_LEFT
                                OPERAND_RIGHT

                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_D_LESS,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_LEQ:
                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved.ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator.ID == KEY_OPTIMIZED_IF) && (OENext->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) &&
                                (OENext->OperandRight.ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_D_GREATER,
                                                           SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                           SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                                i++;
                            } else {
                                OPERAND_RESULT
                                OPERAND_LEFT
                                OPERAND_RIGHT

                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_D_LESS_EQUAL,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_GRE:
                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved.ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator.ID == KEY_OPTIMIZED_IF) && (OENext->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) &&
                                (OENext->OperandRight.ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_D_LESS_EQUAL,
                                                           SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                           SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                                i++;
                            } else {
                                OPERAND_RESULT
                                OPERAND_LEFT
                                OPERAND_RIGHT

                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_D_GREATER,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_GEQ:
                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved.ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator.ID == KEY_OPTIMIZED_IF) && (OENext->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) &&
                                (OENext->OperandRight.ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_D_LESS,
                                                           SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                           SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                                i++;
                            } else {
                                OPERAND_RESULT
                                OPERAND_LEFT
                                OPERAND_RIGHT

                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_D_GREATER_EQUAL,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_EQU:
                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved.ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator.ID == KEY_OPTIMIZED_IF) && (OENext->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) &&
                                (OENext->OperandRight.ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_D_NOT_EQUAL,
                                                           SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                           SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                                i++;
                            } else {
                                OPERAND_RESULT
                                OPERAND_LEFT
                                OPERAND_RIGHT

                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_D_EQUAL,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_NEQ:
                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved.ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator.ID == KEY_OPTIMIZED_IF) && (OENext->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) &&
                                (OENext->OperandRight.ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_D_EQUAL,
                                                           SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                           SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                                i++;
                            } else {
                                OPERAND_RESULT
                                OPERAND_LEFT
                                OPERAND_RIGHT

                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_D_NOT_EQUAL,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_NOT:
                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved.ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator.ID == KEY_OPTIMIZED_IF) && (OENext->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) &&
                                (OENext->OperandRight.ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_D_NOT_EQUAL,
                                                           SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                           SLJIT_MEM0(), (sljit_sw) & d_false);

                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                                i++;
                            } else {
                                OPERAND_RESULT
                                OPERAND_LEFT

                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_D_EQUAL,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_NEG:
                            OPERAND_RESULT
                            OPERAND_LEFT

                                sljit_emit_fop1(compiler, SLJIT_DNEG,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                            break;

                        case KEY_CND_NULL:
                            OPERAND_RESULT
                            OPERAND_LEFT
                            OPERAND_RIGHT

                                sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            jump = sljit_emit_fcmp(compiler, SLJIT_D_NOT_EQUAL,
                                                   SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                   SLJIT_MEM0(), (sljit_sw) & d_false);

                            sljit_emit_fop1(compiler, SLJIT_DMOV,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            sljit_set_label(jump, sljit_emit_label(compiler));
                            break;

                        case KEY_REM:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_modVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_modVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_ARE:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_modVD2));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_modVD2));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_BOR:
                            OPERAND_RESULT
                            OPERAND_LEFT
                            OPERAND_RIGHT

                                sljit_emit_fop2(compiler, SLJIT_DADD,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            jump = sljit_emit_fcmp(compiler, SLJIT_D_NOT_EQUAL,
                                                   SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                   SLJIT_MEM0(), (sljit_sw) & d_false);

                            jump2 = sljit_emit_jump(compiler, SLJIT_JUMP);
                            sljit_set_label(jump, sljit_emit_label(compiler));

                            sljit_emit_fop1(compiler, SLJIT_DMOV,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);

                            sljit_set_label(jump2, sljit_emit_label(compiler));
                            break;

                        case KEY_BAN:
                            OPERAND_RESULT
                            OPERAND_LEFT
                            OPERAND_RIGHT

                                sljit_emit_fop2(compiler, SLJIT_DMUL,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            jump = sljit_emit_fcmp(compiler, SLJIT_D_NOT_EQUAL,
                                                   SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                   SLJIT_MEM0(), (sljit_sw) & d_false);

                            jump2 = sljit_emit_jump(compiler, SLJIT_JUMP);
                            sljit_set_label(jump, sljit_emit_label(compiler));

                            sljit_emit_fop1(compiler, SLJIT_DMOV,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);

                            sljit_set_label(jump2, sljit_emit_label(compiler));
                            break;

                        case KEY_AAN:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_AANVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_AANVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_AND:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_ANDVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_ANDVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_AXO:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_AXOVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_AXOVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_AOR:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_AORVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_AORVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_ASL:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_ASLVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_ASLVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_ASR:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_ASRVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_ASRVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_COM:
                            OPERAND_LEFT
                            OPERAND_RESULT2

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_COMVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_COMVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_SHL:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_SHLVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_SHLVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_SHR:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_SHRVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_SHRVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_XOR:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_XORVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_XORVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_OR:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            OPERAND_RESULT

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_ORVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(c_ORVD));
 #endif
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_INDEX_OPEN:
                            if (i + 1 < end) {
                                cnt    = labels[i - start + 1];
                                OENext = &OPT->CODE[i + 1];
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator.TYPE == TYPE_OPERATOR) && ((OENext->Operator.ID == KEY_ASG) || (OENext->Operator.ID == KEY_BY_REF)) && (OENext->OperandLeft.ID == OE->Result_ID) && (dataflags[OE->Result_ID - 1] != 2) && (!cnt)) {
                                OPERAND_LEFT
                                    OPERAND_RIGHT
                                // result from OENext
                                if (reg3 != OENext->OperandRight.ID) {
                                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OENext->OperandRight.ID - 1));
                                    reg3 = OENext->OperandRight.ID;
                                }

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArrayDataASGVD));
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArrayDataASGVD));
 #endif
                                struct sljit_jump *jump = sljit_emit_cmp(compiler, SLJIT_NOT_EQUAL,
                                                                         SLJIT_RETURN_REG, 0,
                                                                         SLJIT_IMM, 0);

                                sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, i);
                                sljit_set_label(jump, sljit_emit_label(compiler));
                                i++;
                                if ((usedflags[OENext->Result_ID - 1]) || (usedflags[OENext->OperandLeft.ID - 1] > 1)) {
                                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OENext->OperandRight.ID - 1));
                                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OENext->OperandLeft.ID - 1));
                                    if (usedflags[OENext->Result_ID - 1]) {
                                        sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OENext->Result_ID - 1));

                                        sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                        SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                        SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                                    }

                                    sljit_emit_fop1(compiler, SLJIT_DMOV,
                                                    SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                                }
                            } else
                            if ((OENext) && (OENext->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) && (OENext->Operator.ID == KEY_OPTIMIZED_IF) && (OENext->OperandRight.ID == OE->Result_ID)) {
                                if (usedflags[OE->Result_ID - 1] != 1) {
                                    OPERAND_LEFT2
                                    OPERAND_RIGHT2
                                    OPERAND_RESULT
 #ifdef ARM_PATCH
                                        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArrayDataCopyVD));
                                    sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                                    sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArrayDataCopyVD));
 #endif
                                    struct sljit_jump *jump = sljit_emit_cmp(compiler, SLJIT_NOT_EQUAL,
                                                                             SLJIT_RETURN_REG, 0,
                                                                             SLJIT_IMM, 0);

                                    sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, i);
                                    sljit_set_label(jump, sljit_emit_label(compiler));
                                } else {
                                    OPERAND_RIGHT2
                                    OPERAND_LEFT2
 #ifdef ARM_PATCH
                                        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArrayDataVDIF));
                                    sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_TEMPORARY_EREG1, 0);
 #else
                                    sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArrayDataVDIF));
 #endif
                                    struct sljit_jump *jump = sljit_emit_cmp(compiler, SLJIT_NOT_EQUAL, SLJIT_RETURN_REG, 0, SLJIT_IMM, 2);
                                    sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, i);
                                    sljit_set_label(jump, sljit_emit_label(compiler));

                                    icode = OENext->OperandReserved.ID;
                                    if ((icode < start) || (icode >= end)) {
                                        jump = sljit_emit_cmp(compiler, SLJIT_NOT_EQUAL, SLJIT_RETURN_REG, 0, SLJIT_IMM, 0);
                                        sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, icode);
                                        sljit_set_label(jump, sljit_emit_label(compiler));
                                        outsidejump[icode] = 1;
                                    } else {
                                        jump = sljit_emit_cmp(compiler, SLJIT_EQUAL, SLJIT_RETURN_REG, 0, SLJIT_IMM, 0);

                                        icode_idx = icode - start;
                                        if (!jumps[icode_idx])
                                            jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                        jumps[icode_idx][posj[icode_idx]++] = jump;
                                    }
                                    i++;
                                }
                            } else {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                OPERAND_RESULT
 #ifdef ARM_PATCH
                                    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArrayDataVD));
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArrayDataVD));
 #endif
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_RETURN_REG, 0);
                                struct sljit_jump *jump = sljit_emit_cmp(compiler, SLJIT_NOT_EQUAL,
                                                                         SLJIT_RETURN_REG, 0,
                                                                         SLJIT_IMM, 0);

                                sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, i);
                                sljit_set_label(jump, sljit_emit_label(compiler));

                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, OE->Result_ID - 1);

 #ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(ReplaceVariable));
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
 #else
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(ReplaceVariable));
 #endif
                            }
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_LENGTH:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandLeft.ID - 1));
                            sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->Result_ID - 1));

 #ifdef ARM_PATCH
                            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(LengthVD));
                            sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_TEMPORARY_EREG1, 0);
 #else
                            sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_IMM, SLJIT_FUNC_OFFSET(LengthVD));
 #endif
                            jump = sljit_emit_cmp(compiler, SLJIT_NOT_EQUAL,
                                                  SLJIT_RETURN_REG, 0,
                                                  SLJIT_IMM, 0);

                            sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, i);
                            sljit_set_label(jump, sljit_emit_label(compiler));
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        default:
                            break;
                    }
                } else
                if (OE->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) {
                    switch (OE->Operator.ID) {
                        case KEY_OPTIMIZED_IF:
                            // recicling result ... it is set to zero
                            OPERAND_RIGHT

                                icode = OE->OperandReserved.ID;
                            if ((icode < start) || (icode >= end)) {
                                jump = sljit_emit_fcmp(compiler, SLJIT_D_NOT_EQUAL,
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM0(), (sljit_sw) & d_false);
                                sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, icode);
                                sljit_set_label(jump, sljit_emit_label(compiler));
                                outsidejump[icode] = 1;
                            } else {
                                jump = sljit_emit_fcmp(compiler, SLJIT_D_EQUAL,
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM0(), (sljit_sw) & d_false);

                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                            }
                            break;

                        case KEY_OPTIMIZED_GOTO:
                            icode = OE->OperandReserved.ID;
                            if ((icode < start) || (icode >= end)) {
                                sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, icode);
                                outsidejump[icode] = 1;
                            } else {
                                jump      = sljit_emit_jump(compiler, SLJIT_JUMP);
                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                            }
                            break;

                        default:
                            break;
                    }
                }
            }
            int idx = end - start;
            int cnt = labels[idx];
            if (cnt) {
                if (!labelsptr[idx]) {
                    labelsptr[idx] = (void **)malloc(sizeof(void *) * cnt);

                    for (int i = 0; i < cnt; i++)
                        labelsptr[idx][i] = (void *)sljit_emit_label(compiler);
                }
            }

            for (int i = start; i <= end; i++) {
                idx = i - start;
                struct sljit_jump **jumplist = (struct sljit_jump **)jumps[idx];
                if (jumplist) {
                    int cnt2 = posj[idx];
                    for (int j = 0; j < cnt2; j++) {
                        struct sljit_jump *jump = jumplist[j];
                        if (jump)
                            sljit_set_label(jump, (struct sljit_label *)labelsptr[idx][j]);
                    }
                }
            }
            sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, end);
            code.code = sljit_generate_code(compiler);
            sljit_free_compiler(compiler);
            jittracecode[start] = code.code;

            if (labelsptr) {
                for (int i = 0; i < delta2; i++) {
                    void *ptr = (void *)labelsptr[i];
                    if (ptr)
                        free(ptr);
                    ptr = (void *)jumps[i];
                    if (ptr)
                        free(ptr);
                }
                free(labelsptr);
                free(lrptr);
            }
            if (jumps)
                free(jumps);

            if (labels)
                free((void *)labels);

            if (posj)
                free(posj);
        }
        if ((instruction_pointer > count) || (is_outsidejump)) {
            bool has_jumps = false;
            for (int i = is_outsidejump; i < count; i++) {
                if ((outsidejump[i]) && (!jittracecode[i])) {
                    is_outsidejump      = i + 1;
                    instruction_pointer = is_outsidejump;
                    has_jumps           = true;
                    break;
                }
            }
            if (!has_jumps)
                break;
        }
    }
    if (outsidejump)
        free(outsidejump);
    if (dataflags)
        free(dataflags);
    if (usedflags)
        free(usedflags);
}
#endif

int ConceptInterpreter::StacklessInterpret(PIFAlizator *PIF, GreenThreadCycle *GREEN) {
    char          *STATIC_ERROR = 0;
    ParamList     *FORMAL_PARAMETERS;
    VariableDATA  *RESULT;
    CompiledClass *CCTEMP;
    INTEGER       OPERATOR_ID;
    ClassCode     *CC;
    void          *instancePTR;
    VariableDATA  *RETURN_DATA;
    AnsiException *Exc;
    char          pushed_type, next_is_asg;
    VariableDATA  *THROW_DATA  = 0;
    SCStack       *STACK_TRACE = NULL;

    GreenThreadCycle *TARGET_THREAD = GREEN;
    GreenThreadCycle *PREC_THREAD   = (GreenThreadCycle *)GREEN->LAST_THREAD;

    ParamListExtra FORMAL_PARAMETERS2;

    FORMAL_PARAMETERS2.PIF = PIF;

    ParamList OPERATOR_PARAM;
    INTEGER   PARAM_INDEX[1];

#ifdef SIMPLE_MULTI_THREADING
    char IsWriteLocked = 0;
#endif

    while (TARGET_THREAD) {
        ConceptInterpreter *THIS_REF = (ConceptInterpreter *)TARGET_THREAD->INTERPRETER;
        Optimizer          *OPT      = (Optimizer *)TARGET_THREAD->OPT;
        int          ClassID         = THIS_REF->LocalClassID;
        VariableDATA **LOCAL_CONTEXT = TARGET_THREAD->LOCAL_CONTEXT;

        register INTEGER INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_POINTER;
        register INTEGER INSTRUCTION_COUNT   = TARGET_THREAD->INSTRUCTION_COUNT;

        register RuntimeOptimizedElement *CODE = OPT->CODE;
        INTEGER CATCH_INSTRUCTION_POINTER      = TARGET_THREAD->CATCH_INSTRUCTION_POINTER;
        INTEGER CATCH_VARIABLE = TARGET_THREAD->CATCH_VARIABLE;
        INTEGER PREVIOUS_TRY   = TARGET_THREAD->PREVIOUS_TRY;
        STACK_TRACE = &TARGET_THREAD->STACK_TRACE;
#ifdef USE_JIT_TRACE
        WRITE_LOCK
        if (!THIS_REF->jittracecode)
            THIS_REF->AnalizeInstructionPath(OPT);
        void **jittrace = THIS_REF->jittracecode;
        WRITE_UNLOCK;
        executable_code code;
#endif

        char DO_EXECUTE = 1;
        while ((INSTRUCTION_POINTER < INSTRUCTION_COUNT) && (DO_EXECUTE)) {
            GREEN->CURRENT_THREAD = TARGET_THREAD;
            WRITE_UNLOCK
#ifdef USE_JIT_TRACE
            bool skip = false;
            if (jittrace) {
                if (jittrace[INSTRUCTION_POINTER]) {
                    int res;
                    do {
                        code.code = jittrace[INSTRUCTION_POINTER];
                        res       = code.func1((sljit_sw)LOCAL_CONTEXT);
                        if (res == INSTRUCTION_POINTER)
                            break;
                        INSTRUCTION_POINTER = res;
                        skip = true;
                    } while ((INSTRUCTION_POINTER < INSTRUCTION_COUNT) && (jittrace[INSTRUCTION_POINTER]));

                    if (INSTRUCTION_POINTER >= INSTRUCTION_COUNT)
                        break;
                }
            }
            if (skip) {
                DO_EXECUTE  = 0;
                PREC_THREAD = TARGET_THREAD;
                TARGET_THREAD->CATCH_INSTRUCTION_POINTER = CATCH_INSTRUCTION_POINTER;
                TARGET_THREAD->CATCH_VARIABLE            = CATCH_VARIABLE;
                TARGET_THREAD->PREVIOUS_TRY        = PREVIOUS_TRY;
                TARGET_THREAD->INSTRUCTION_POINTER = INSTRUCTION_POINTER;
                TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;
                continue;
            }
#endif
            register RuntimeOptimizedElement *OE = &CODE [INSTRUCTION_POINTER++];
            register int OE_Operator_ID          = OE->Operator.ID;
            if (OE->Operator.TYPE == TYPE_OPERATOR) {
                //WRITE_LOCK
                switch (OE_Operator_ID) {
                    case KEY_ASG:
                        {
                            WRITE_LOCK
                            if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE != VARIABLE_CLASS) {
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                                // ------------------- //
                                pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                char asg_type = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                if ((asg_type == VARIABLE_CLASS) || (asg_type == VARIABLE_ARRAY)) {
                                    int delta = 0;
                                    if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) {
                                        //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                        CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                    } else {
                                        delta = -1;
                                    }

                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                    if (asg_type == VARIABLE_CLASS) {
                                        ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2 + delta;
                                    } else {
                                        ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2 + delta;
                                    }
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                    if ((TARGET_THREAD->PROPERTIES) && (TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT)) {
                                        CCTEMP = (CompiledClass *)((VariableDATA *)(TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET))->CLASS_DATA;
                                        WRITE_UNLOCK
                                        CCTEMP->_Class->SetProperty(PIF, TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET), &OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, THIS_REF->LocalClassID, STACK_TRACE);
                                    }
                                    DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                                    continue;
                                }
                            } else {
                                //---------------------------------------------------
                                CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                ////////////////////////////////////////////////////////////
                                pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                                ////////////////////////////////////////////////////////////
                                if (CCTEMP->_Class->Relocation(DEF_ASG)) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_CLASS;
                                } else {
                                    // ------------------- //
                                    CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                                    // ------------------- //
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                    OE_Operator_ID = KEY_BY_REF;
                                }
                            }
                            if (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_NUMBER) {
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE   = VARIABLE_NUMBER;
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                continue;
                            }
                        }
                        break;
                    case KEY_NEW:
                        CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        if (OE->OperandLeft.ID == STATIC_CLASS_ARRAY) {
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = new(AllocArray(PIF))Array(PIF);
                            DECLARE_PATH(VARIABLE_ARRAY);
                        } else {
                            CC = PIF->StaticClassList[OE->OperandLeft.ID - 1];
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_CLASS;
                            FORMAL_PARAMETERS = 0;
                            if (OE->OperandReserved.ID) {
                                FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved.ID - 1];
                            }
                            WRITE_UNLOCK;
                            instancePTR = CC->CreateInstance(PIF, LOCAL_CONTEXT [OE->Result_ID - 1], &OE->OperandLeft, FORMAL_PARAMETERS, LOCAL_CONTEXT, NULL);

                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = instancePTR;

                            if (!instancePTR) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                            } else {
                                DECLARE_PATH(VARIABLE_CLASS);
                            }
                        }
                        continue;

                    case KEY_DELETE:
                        CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                        LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE        = VARIABLE_NUMBER;
                        LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = 0;
                        DECLARE_PATH(VARIABLE_NUMBER);
                        continue;

                    case KEY_SEL:
                        // execute member !!!
                        WRITE_LOCK
                        if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE != VARIABLE_CLASS) {
                            Exc = new AnsiException(ERR240, OE->Operator._DEBUG_INFO_LINE, 240, OE->OperandLeft._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                            DECLARE_PATH(0x20);
                            continue;
                        }
                        CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;

                        if (OE->OperandReserved.ID) {
                            FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved.ID - 1];
                            next_is_asg       = 0;
                        } else {
                            next_is_asg       = OE->OperandReserved.TYPE;
                            FORMAL_PARAMETERS = 0;
                        }
                        try {
                            //-------------------------------------------//
                            WRITE_UNLOCK
                            RESULT = CCTEMP->_Class->ExecuteMember(PIF, OE->OperandRight.ID - 1, LOCAL_CONTEXT [OE->OperandLeft.ID - 1], &OE->OperandRight, CCTEMP->_Class->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, THIS_REF->LocalClassID, STACK_TRACE, next_is_asg, &TARGET_THREAD->PROPERTIES, OPT->dataCount, OE->Result_ID - 1);
                            WRITE_LOCK
                            //-------------------------------------------//

                            if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                            if (RESULT) {
                                FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                                RESULT->LINKS++;
                            } else {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                        } catch (VariableDATA *LAST_THROW) {
                            DECLARE_PATH(LAST_THROW->TYPE);
                            if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                                FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                                LOCAL_CONTEXT [CATCH_VARIABLE - 1] = LAST_THROW;
                                INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                                THROW_DATA                = 0;
                                CATCH_INSTRUCTION_POINTER = 0;
                                CATCH_VARIABLE            = 0;
                                //-------------//
                                RESTORE_TRY_DATA(THIS_REF);
                                //-------------//
                            } else {
                                WRITE_UNLOCK
                                    FREE_VARIABLE(LAST_THROW);
                                // uncaught exception
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1300, OE->OperandRight._DEBUG_INFO_LINE, 1300, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME));
                                INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                break;
                            }
                        }
                        DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                        continue;

                    case KEY_TYPE_OF:
                        //==================//
                        CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        //==================//
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_STRING;
                        switch (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE) {
                            case VARIABLE_NUMBER:
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "numeric";
                                break;

                            case VARIABLE_STRING:
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "string";
                                break;

                            case VARIABLE_CLASS:
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "class";
                                break;

                            case VARIABLE_ARRAY:
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "array";
                                break;

                            case VARIABLE_DELEGATE:
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "delegate";
                                break;

                            default:
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "";
                        }
                        DECLARE_PATH(VARIABLE_STRING);
                        continue;

                    case KEY_CLASS_NAME:
                        //==================//
                        CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        //==================//
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_STRING;
                        if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE != VARIABLE_CLASS) {
                            CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "";
                        } else {
                            CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                            CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CCTEMP->_Class->NAME.c_str();
                        }
                        DECLARE_PATH(VARIABLE_STRING);
                        continue;

                    case KEY_DLL_CALL:
                        WRITE_LOCK
                        FORMAL_PARAMETERS = 0;
                        STATIC_ERROR = 0;
                        if (OE->OperandReserved.ID) {
                            FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved.ID - 1];
                        }
                        if (OE->OperandLeft.ID == STATIC_CLASS_DELEGATE) {
                            // call to delegate function
                            VariableDATA *lOwner = 0;
                            try {
                                //-------------------------------------------//
                                // LOCAL_CONTEXT devine 0 !!!
                                //-------------------------------------------//
                                if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_DELEGATE) {
                                    Exc = new AnsiException(ERR970, OE->OperandRight._DEBUG_INFO_LINE, 970, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    DECLARE_PATH(0x20);
                                    continue;
                                }
                                CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                lOwner = (VariableDATA *)VAR_ALLOC(PIF);
                                lOwner->CLASS_DATA         = CCTEMP;
                                lOwner->DELEGATE_DATA      = 0;
                                lOwner->IS_PROPERTY_RESULT = 0;
                                lOwner->LINKS = 1;
                                lOwner->TYPE = VARIABLE_CLASS;
                                CCTEMP->LINKS++;

                                WRITE_UNLOCK
                                RESULT = CCTEMP->_Class->ExecuteDelegate(PIF,
                                                                             (INTEGER)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA,
                                                                             lOwner,
                                                                             &OE->OperandRight,
                                                                             FORMAL_PARAMETERS,
                                                                             LOCAL_CONTEXT,
                                                                             ClassID,
                                                                             THIS_REF->LocalClassID,
                                                                             NULL
                                                                             );
                                WRITE_LOCK
                                FREE_VARIABLE(lOwner);

                                //-------------------------------------------//
                                //}
                                //-------------------------------------------//
                                if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                                    CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                }
                                if (RESULT == LOCAL_CONTEXT [OE->Result_ID - 1]) {
                                    DECLARE_PATH(RESULT->TYPE);
                                    continue;
                                }
                                if (RESULT) {
                                    FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                    LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                                    RESULT->LINKS++;
                                }
                            } catch (VariableDATA *LAST_THROW) {
                                FREE_VARIABLE(lOwner);
                                DECLARE_PATH(LAST_THROW->TYPE);
                                if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                                    FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                                    LOCAL_CONTEXT [CATCH_VARIABLE - 1] = LAST_THROW;
                                    INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                                    THROW_DATA                = 0;
                                    CATCH_INSTRUCTION_POINTER = 0;
                                    CATCH_VARIABLE            = 0;
                                    //-------------//
                                    RESTORE_TRY_DATA(THIS_REF);
                                    //-------------//
                                } else {
                                    WRITE_UNLOCK
                                    FREE_VARIABLE(LAST_THROW);
                                    // uncaught exception
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1300, OE->OperandRight._DEBUG_INFO_LINE, 1300, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME));
                                    INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                    break;
                                }
                            }
                            DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                            continue;
                        } else
                        if (OE->OperandLeft.ID == STATIC_CLASS_DLL) {
                            if ((FORMAL_PARAMETERS) && (FORMAL_PARAMETERS->COUNT)) {
                                FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                                FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);
                            } else {
                                FORMAL_PARAMETERS2.COUNT       = 0;
                                FORMAL_PARAMETERS2.PARAM_INDEX = 0;
                            }
                            STACK(STACK_TRACE, OE->OperandLeft._DEBUG_INFO_LINE)
                            WRITE_UNLOCK
                            try {
                                STATIC_ERROR = (char *)((EXTERNAL_CALL)(PIF->StaticLinks [OE->OperandRight.ID - 1]))(&FORMAL_PARAMETERS2, LOCAL_CONTEXT, LOCAL_CONTEXT [OE->Result_ID - 1], SetVariable, GetVariable, PIF->out->sock, PIF->SERVER_PUBLIC_KEY, PIF->SERVER_PRIVATE_KEY, PIF->CLIENT_PUBLIC_KEY, SetClassMember, GetClassMember, Invoke);
                            } catch (...) {
                                Exc = new AnsiException(ERR690, OE->OperandLeft._DEBUG_INFO_LINE, 690, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                            }
                            WRITE_LOCK
                            UNSTACK;
                            if (STATIC_ERROR) {
                                if (PIF->USE_EXC) {
                                    DECLARE_PATH(VARIABLE_STRING);
                                    if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                                        THROW_DATA = (VariableDATA *)VAR_ALLOC(PIF);
                                        THROW_DATA->CLASS_DATA         = 0;
                                        THROW_DATA->TYPE               = VARIABLE_STRING;
                                        THROW_DATA->LINKS              = 1;
                                        THROW_DATA->IS_PROPERTY_RESULT = 0;

                                        CONCEPT_STRING(THROW_DATA) = STATIC_ERROR;

                                        FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                                        LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                                        INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                                        THROW_DATA                = 0;
                                        CATCH_INSTRUCTION_POINTER = 0;
                                        CATCH_VARIABLE            = 0;
                                        //--------------//
                                        RESTORE_TRY_DATA(THIS_REF);
                                        //--------------//
                                    } else {
                                        WRITE_UNLOCK
                                        // uncaught exception
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1300, OE->OperandRight._DEBUG_INFO_LINE, 1300, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME));
                                        INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                        break;
                                    }
                                } else {
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR700, OE->OperandLeft._DEBUG_INFO_LINE, 700, AnsiString(OE->OperandLeft._PARSE_DATA.c_str()) + AnsiString(": ") + AnsiString(STATIC_ERROR), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    RESET_VARIABLE(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                }
                            }
                        } else {
                            try {
                                CC = PIF->StaticClassList[OE->OperandLeft.ID - 1];
                                //-------------------------------------------//
                                WRITE_UNLOCK
                                RESULT = CC->ExecuteMember(PIF, OE->OperandRight.ID - 1, LOCAL_CONTEXT [0], &OE->OperandRight, CC->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, THIS_REF->LocalClassID, STACK_TRACE);
                                WRITE_LOCK
                                //-------------------------------------------//

                                if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                                    CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                }
                                if (RESULT) {
                                    FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                    LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                                    RESULT->LINKS++;
                                } else {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                }
                            } catch (VariableDATA *LAST_THROW) {
                                DECLARE_PATH(LAST_THROW->TYPE);
                                if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                                    FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                                    LOCAL_CONTEXT [CATCH_VARIABLE - 1] = LAST_THROW;
                                    INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                                    THROW_DATA                = 0;
                                    CATCH_INSTRUCTION_POINTER = 0;
                                    CATCH_VARIABLE            = 0;
                                    //-------------//
                                    RESTORE_TRY_DATA(THIS_REF);
                                    //-------------//
                                } else {
                                    WRITE_UNLOCK
                                        FREE_VARIABLE(LAST_THROW);
                                    // uncaught exception
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1300, OE->OperandRight._DEBUG_INFO_LINE, 1300, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME));
                                    INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                    break;
                                }
                            }
                        }
                        DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                        continue;

                    // ---------------- PUS IF-urile de mai jos aici ---------------- //
                    case KEY_BY_REF:
                        WRITE_LOCK
                        CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                        if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE == VARIABLE_NUMBER) {
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE   = VARIABLE_NUMBER;
                            PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                            continue;
                        }

                        pushed_type = VARIABLE_NUMBER;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                        break;

                    default:
                        SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                        SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                        if (OE->OperandLeft.ID) {
                            ////////////////////////////////////////////////////////////
                            pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                            ////////////////////////////////////////////////////////////
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE;
                            if (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_NUMBER) {
                                if (OE_Operator_ID == KEY_LES) {
                                    EVAL_NUMBER_EXPRESSION(THIS_REF, <)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_INC_LEFT) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA)++;
                                    PROPERTY_CODE_LEFT(THIS_REF, TARGET_THREAD->PROPERTIES)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_SUM) {
                                    EVAL_NUMBER_EXPRESSION(THIS_REF, +)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_ASU) {
                                    EVAL_NUMBER_EXPRESSION(THIS_REF, += )
                                    LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                    PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_DIF) {
                                    EVAL_NUMBER_EXPRESSION(THIS_REF, -)
                                    continue;
                                }
                            } else
                            if ((OE_Operator_ID == KEY_INDEX_OPEN) && (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_ARRAY)) {
                                WRITE_LOCK
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                        RETURN_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Get(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                        if (!RETURN_DATA) {
                                            DECLARE_PATH(0x20);
                                            Exc = new AnsiException(ERR1110, OE->OperandLeft._DEBUG_INFO_LINE, 1110, AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                            PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                            continue;
                                        }
                                        if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                            FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                            LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                            RETURN_DATA->LINKS++;
                                        }
                                        DECLARE_PATH(RETURN_DATA->TYPE);
                                        continue;

                                    default:
                                        DECLARE_PATH(0x20);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                        Exc = new AnsiException(ERR900, OE->OperandLeft._DEBUG_INFO_LINE, 900, OE->OperandLeft._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        continue;
                                }
                                continue;
                            }
                        }
                        // -------------------- pana aici --------------------------------//
                }

                switch (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE) {
                    case VARIABLE_NUMBER:
#ifndef USE_INLINE_EVALUATION
#ifdef SIMPLE_MULTI_THREADING
                        if (THIS_REF->EvalNumberExpression(PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked, pushed_type))
#else
                        if (THIS_REF->EvalNumberExpression(PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, pushed_type))
#endif
                            continue;
                        return 0;
#else
                        DECLARE_PATH(VARIABLE_NUMBER);
                        switch (OE_Operator_ID) {
                            // assigment operators
                            case KEY_BY_REF:
                            case KEY_ASG:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_INC:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ++LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_DEC:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = --LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_INC_LEFT:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA)++;
                                //----------------//
                                PROPERTY_CODE_LEFT(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_DEC_LEFT:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA)--;
                                //----------------//
                                PROPERTY_CODE_LEFT(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_ASU:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_NUMBER_EXPRESSION(THIS_REF, += )
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_ADI:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_NUMBER_EXPRESSION(THIS_REF, -= )
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_AMU:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_NUMBER_EXPRESSION(THIS_REF, *= )
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_ADV:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_DIVIDE_EXPRESSION(THIS_REF, /= )
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_ARE:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_ASG_INTEGER_DIVIDE_EXPRESSION(THIS_REF, %)
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_AAN:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_ASG_INTEGER_EXPRESSION(THIS_REF, &)
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_AXO:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_ASG_INTEGER_EXPRESSION(THIS_REF, ^)
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_AOR:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_ASG_INTEGER_EXPRESSION(THIS_REF, |)
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_ASL:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_ASG_INTEGER_EXPRESSION(THIS_REF, <<)
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_ASR:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                EVAL_ASG_INTEGER_EXPRESSION(THIS_REF, >>)
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            // unary operators
                            case KEY_NOT:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = !LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                                continue;

                            case KEY_COM:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ~(unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                                continue;

                            case KEY_POZ:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                                continue;

                            case KEY_NEG:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = -LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                                continue;

                            case KEY_MUL:
                                EVAL_NUMBER_EXPRESSION(THIS_REF, *)
                                continue;

                            case KEY_DIV:
                                EVAL_DIVIDE_EXPRESSION(THIS_REF, /)
                                continue;

                            case KEY_REM:
                                EVAL_DIVIDE_INTEGER_EXPRESSION(THIS_REF, %)
                                continue;

                            case KEY_SUM:
                                EVAL_NUMBER_EXPRESSION(THIS_REF, +)
                                continue;

                            case KEY_DIF:
                                EVAL_NUMBER_EXPRESSION(THIS_REF, -)
                                continue;

                            case KEY_SHL:
                                EVAL_INTEGER_EXPRESSION(THIS_REF, <<)
                                continue;

                            case KEY_SHR:
                                EVAL_INTEGER_EXPRESSION(THIS_REF, >>)
                                continue;

                            case KEY_LES:
                                EVAL_NUMBER_EXPRESSION(THIS_REF, <)
                                continue;

                            case KEY_LEQ:
                                EVAL_NUMBER_EXPRESSION(THIS_REF, <=)
                                continue;

                            case KEY_GRE:
                                EVAL_NUMBER_EXPRESSION(THIS_REF, >)
                                continue;

                            case KEY_GEQ:
                                EVAL_NUMBER_EXPRESSION(THIS_REF, >=)
                                continue;

                            case KEY_EQU:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA == CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat();
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_ARRAY:
                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                        break;

                                    default:
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME));
                                }
                                continue;

                            case KEY_NEQ:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat();
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_ARRAY:
                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                        break;

                                    default:
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME));
                                }
                                continue;

                            case KEY_AND:
                                EVAL_INTEGER_EXPRESSION(THIS_REF, &)
                                continue;

                            case KEY_XOR:
                                EVAL_INTEGER_EXPRESSION(THIS_REF, ^)
                                continue;

                            case KEY_OR:
                                EVAL_INTEGER_EXPRESSION(THIS_REF, |)
                                continue;

                            case KEY_BAN:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA && LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA && CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != 0);
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA && ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                        break;
                                }
                                continue;

                            case KEY_BOR:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA || LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA || CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA || ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                        break;
                                }
                                continue;

                            case KEY_CND_NULL:
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                // ------------------- //
                                if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                                    continue;
                                }
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        break;

                                    case VARIABLE_STRING:

                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                                        break;

                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA;

                                    case VARIABLE_CLASS:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                        ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                                        ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                        break;
                                }
                                continue;

                            case KEY_VALUE:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                                continue;

                            default:
                                Exc = new AnsiException(ERR660, OE->Operator._DEBUG_INFO_LINE, 660,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                break;
                        }
                        break;
#endif
                    case VARIABLE_STRING:
                        switch (OE_Operator_ID) {
                            // unary operators
                            case KEY_NOT:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() == 0);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_LENGTH:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length();
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_VALUE:
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).ToFloat();
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_INC:
                                if (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length()) {
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]).LoadBuffer(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).c_str() + 1, CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() - 1);
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                    //----------------//
                                    PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                    //----------------//
                                }
                                DECLARE_PATH(VARIABLE_STRING);
                                continue;

                            case KEY_INC_LEFT:
                                if (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length()) {
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).LoadBuffer(CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]).c_str() + 1, CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() - 1);
                                    //----------------//
                                    PROPERTY_CODE_LEFT(THIS_REF, TARGET_THREAD->PROPERTIES)
                                    //----------------//
                                }
                                DECLARE_PATH(VARIABLE_STRING);
                                continue;

                            // binary ...
                            case KEY_SUM:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]).Sum(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]), CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]));
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    case VARIABLE_NUMBER:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) + AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA);
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                continue;

                            case KEY_EQU:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) == CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                continue;

                            case KEY_GRE:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) > CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) > AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                continue;

                            case KEY_LES:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) < CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) < AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                continue;

                            case KEY_GEQ:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) >= CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) >= AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                continue;

                            case KEY_LEQ:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) <= CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) <= AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                continue;

                            case KEY_NEQ:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) != CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                continue;

                            case KEY_BY_REF:
                            case KEY_ASG:
                                // ------------------- //
                                ////SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                WRITE_LOCK
                                CLASS_CHECK_KEEP_EXTRA(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_STRING;
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                        if (LOCAL_CONTEXT [OE->Result_ID - 1] != LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) {
                                            FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                            LOCAL_CONTEXT [OE->Result_ID - 1] = LOCAL_CONTEXT [OE->OperandLeft.ID - 1];
                                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->LINKS++;
                                        }
                                        //----------------------------------------------------------------------------------------//
                                        UPDATE_STRING_VARIABLE_THAT_HAS_INDEX(THIS_REF, LOCAL_CONTEXT [OE->OperandLeft.ID - 1], TARGET_THREAD->PROPERTIES);
                                        //----------------------------------------------------------------------------------------//
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    case VARIABLE_NUMBER:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1])      = AnsiString(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA);
                                        //----------------------------------------------------------------------------------------//
                                        UPDATE_STRING_VARIABLE_THAT_HAS_INDEX(THIS_REF, LOCAL_CONTEXT [OE->OperandLeft.ID - 1], TARGET_THREAD->PROPERTIES);
                                        //----------------------------------------------------------------------------------------//
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_ASU:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).IncreaseBuffer(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length());
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) += CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                        if (LOCAL_CONTEXT [OE->Result_ID - 1] != LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) {
                                            FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                            LOCAL_CONTEXT [OE->Result_ID - 1] = LOCAL_CONTEXT [OE->OperandLeft.ID - 1];
                                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->LINKS++;
                                        }
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    case VARIABLE_NUMBER:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) += AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA);
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1])       = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                continue;

                            case KEY_INDEX_OPEN:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) [(INTEGER)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA];
                                        ((AnsiString *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->EXTRA_DATA = INSTRUCTION_POINTER;
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    case VARIABLE_STRING:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) [(INTEGER)CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToInt()];
                                        ((AnsiString *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->EXTRA_DATA = INSTRUCTION_POINTER;
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    default:
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR640, OE->OperandLeft._DEBUG_INFO_LINE, 640, OE->OperandLeft._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                break;

                            case KEY_BAN:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() && LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() && CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() != 0);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() && ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;
                                }
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_BOR:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() || LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() || CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_DELEGATE:
                                        // always true for class and delegates
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() || ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;
                                }
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_CND_NULL:
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                // ------------------- //
                                if (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length()) {
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                                    DECLARE_PATH(VARIABLE_STRING);
                                    continue;
                                }
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    case VARIABLE_STRING:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA;

                                    case VARIABLE_CLASS:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                        ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                        DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                                        ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                        DECLARE_PATH(VARIABLE_ARRAY);
                                        break;
                                }
                                continue;

                            default:
                                DECLARE_PATH(0x20);
                                Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                break;
                        }
                        continue;

                    case VARIABLE_CLASS:
                        OPERATOR_ID = 0;
                        switch (OE_Operator_ID) {
                            case KEY_BY_REF:
                                //---------------------------//
                                if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) {
                                    CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                } else {
                                    if (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) {
                                        ((CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->LINKS -= 2;
                                    } else {
                                        CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                        ((CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->LINKS -= 1;
                                    }
                                }
                                //---------------------------//

                                LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2;

                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_CLASS;
                                if ((TARGET_THREAD->PROPERTIES) && (TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT)) {
                                    CCTEMP = (CompiledClass *)((VariableDATA *)TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET)->CLASS_DATA;
                                    WRITE_UNLOCK
                                    CCTEMP->_Class->SetProperty(PIF, TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET), &OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, THIS_REF->LocalClassID, STACK_TRACE);
                                }
                                DECLARE_PATH(VARIABLE_CLASS);
                                continue;
                                break;

                            case KEY_ASG:
                                OPERATOR_ID = DEF_ASG;
                                break;

                            case KEY_INDEX_OPEN:
                                OPERATOR_ID = DEF_SELECTOR_I;
                                break;

                            case KEY_SUM:
                                OPERATOR_ID = DEF_SUM;
                                break;

                            case KEY_MUL:
                                OPERATOR_ID = DEF_MUL;
                                break;

                            case KEY_DIV:
                                OPERATOR_ID = DEF_DIV;
                                break;

                            case KEY_REM:
                                OPERATOR_ID = DEF_REM;
                                break;

                            case KEY_DIF:
                                OPERATOR_ID = DEF_DIF;
                                break;

                            case KEY_SHL:
                                OPERATOR_ID = DEF_SHL;
                                break;

                            case KEY_SHR:
                                OPERATOR_ID = DEF_SHR;
                                break;

                            case KEY_LES:
                                OPERATOR_ID = DEF_LES;
                                break;

                            case KEY_LEQ:
                                OPERATOR_ID = DEF_LEQ;
                                break;

                            case KEY_GRE:
                                OPERATOR_ID = DEF_GRE;
                                break;

                            case KEY_GEQ:
                                OPERATOR_ID = DEF_GEQ;
                                break;

                            case KEY_EQU:
                                CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                if (!CCTEMP->_Class->Relocation(DEF_EQU)) {
                                    CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)

                                    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                        case VARIABLE_CLASS:
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA ? 1 : 0;
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                            break;

                                        default:
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    }
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    continue;
                                }
                                OPERATOR_ID = DEF_EQU;
                                break;

                            case KEY_NEQ:
                                CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                if (!CCTEMP->_Class->Relocation(DEF_NEQ)) {
                                    //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                    CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)

                                    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                        case VARIABLE_CLASS:
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA ? 1 : 0;
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                            break;

                                        default:
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    }
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    continue;
                                }
                                OPERATOR_ID = DEF_NEQ;
                                break;

                            case KEY_AND:
                                OPERATOR_ID = DEF_AND;
                                break;

                            case KEY_XOR:
                                OPERATOR_ID = DEF_XOR;
                                break;

                            case KEY_OR:
                                OPERATOR_ID = DEF_OR;
                                break;

                            case KEY_BAN:
                                CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                if (!CCTEMP->_Class->Relocation(DEF_BAN)) {
                                    //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                    CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                        case VARIABLE_NUMBER:
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA != 0;
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                            break;

                                        case VARIABLE_STRING:
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length() != 0;
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                            break;

                                        case VARIABLE_CLASS:
                                        case VARIABLE_DELEGATE:
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                            break;

                                        case VARIABLE_ARRAY:
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count() != 0;
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                            break;
                                    }
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    continue;
                                }

                                OPERATOR_ID = DEF_BAN;
                                break;

                            case KEY_BOR:
                                CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                if (!CCTEMP->_Class->Relocation(DEF_BOR)) {
                                    //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                    CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    continue;
                                }
                                OPERATOR_ID = DEF_BOR;
                                break;

                            case KEY_ASU:
                                OPERATOR_ID = DEF_ASU;
                                break;

                            case KEY_ADI:
                                OPERATOR_ID = DEF_ADI;
                                break;

                            case KEY_AMU:
                                OPERATOR_ID = DEF_AMU;
                                break;

                            case KEY_ADV:
                                OPERATOR_ID = DEF_ADV;
                                break;

                            case KEY_ARE:
                                OPERATOR_ID = DEF_ARE;
                                break;

                            case KEY_AAN:
                                OPERATOR_ID = DEF_AAN;
                                break;

                            case KEY_AXO:
                                OPERATOR_ID = DEF_AXO;
                                break;

                            case KEY_AOR:
                                OPERATOR_ID = DEF_AOR;
                                break;

                            case KEY_ASL:
                                OPERATOR_ID = DEF_ASL;
                                break;

                            case KEY_ASR:
                                OPERATOR_ID = DEF_ASR;
                                break;

                            // unari !
                            case KEY_NOT:
                                CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                if (!CCTEMP->_Class->Relocation(DEF_NOT)) {
                                    //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                    CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    continue;
                                }
                                OPERATOR_ID = DEF_NOT;
                                break;

                            case KEY_COM:
                                OPERATOR_ID = DEF_COM;
                                break;

                            case KEY_INC_LEFT:
                            case KEY_INC:
                                OPERATOR_ID = DEF_INC;
                                break;

                            case KEY_DEC_LEFT:
                            case KEY_DEC:
                                OPERATOR_ID = DEF_DEC;
                                break;

                            case KEY_CND_NULL:
                                if ((LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_CLASS) && (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)) {
                                    DECLARE_PATH(VARIABLE_CLASS);
                                    continue;
                                }
                                //---------------------------//
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                //---------------------------//
                                LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                DECLARE_PATH(VARIABLE_CLASS);
                                continue;

                            default:
                                DECLARE_PATH(0x20);
                                Exc = new AnsiException(ERR920, OE->Operator._DEBUG_INFO_LINE, 920,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                break;
                        }
                        //////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                        //////////////////////////////////////////////////////
                        if (!OPERATOR_ID) {
                            DECLARE_PATH(0x20);
                            continue;
                        }
                        if (OE->OperandRight.ID) {
                            OPERATOR_PARAM.COUNT       = 1;
                            PARAM_INDEX[0]             = OE->OperandRight.ID;
                            OPERATOR_PARAM.PARAM_INDEX = DELTA_REF(&OPERATOR_PARAM, PARAM_INDEX);

                        } else {
                            OPERATOR_PARAM.COUNT = 0;
                        }
                        CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                        try {
                            WRITE_UNLOCK
                            RESULT = CCTEMP->_Class->ExecuteMember(PIF, OPERATOR_ID, LOCAL_CONTEXT [OE->OperandLeft.ID - 1], &OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, &OPERATOR_PARAM, LOCAL_CONTEXT, 0, ClassID, THIS_REF->LocalClassID, STACK_TRACE);
                            WRITE_LOCK

                            if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                            if (RESULT) {
                                FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                                RESULT->LINKS++;
                            } else {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                DECLARE_PATH(0x20);
                            }
                        } catch (VariableDATA *LAST_THROW) {
                            DECLARE_PATH(VARIABLE_NUMBER);
                            DECLARE_PATH(LAST_THROW->TYPE);
                            if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                                FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                                LOCAL_CONTEXT [CATCH_VARIABLE - 1] = LAST_THROW;
                                INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                                THROW_DATA                = 0;
                                CATCH_INSTRUCTION_POINTER = 0;
                                CATCH_VARIABLE            = 0;
                                //-------------//
                                RESTORE_TRY_DATA(THIS_REF);
                                //-------------//
                            } else {
                                WRITE_UNLOCK
                                    FREE_VARIABLE(LAST_THROW);
                                // uncaught exception
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1300, OE->OperandRight._DEBUG_INFO_LINE, 1300, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME));
                                INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                break;
                            }
                        }
                        break;

                    case VARIABLE_ARRAY:
                        switch (OE_Operator_ID) {
                            case KEY_ASG:
                            case KEY_BY_REF:
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                //---------------------------//
                                LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2;
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_ARRAY;
                                if ((TARGET_THREAD->PROPERTIES) && (TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT)) {
                                    CCTEMP = (CompiledClass *)((VariableDATA *)(TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET))->CLASS_DATA;
                                    WRITE_UNLOCK
                                    CCTEMP->_Class->SetProperty(PIF, TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(TARGET_THREAD->PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET), &OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, THIS_REF->LocalClassID, STACK_TRACE);
                                }
                                DECLARE_PATH(VARIABLE_ARRAY);
                                continue;

                            case KEY_INDEX_OPEN:
                                WRITE_LOCK
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                        RETURN_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Get(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                        if (!RETURN_DATA) {
                                            DECLARE_PATH(0x20);
                                            Exc = new AnsiException(ERR1110, OE->OperandLeft._DEBUG_INFO_LINE, 1110, AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                            PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                            break;
                                        }
                                        if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                            FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                            LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                            RETURN_DATA->LINKS++;
                                        }
                                        DECLARE_PATH(RETURN_DATA->TYPE);
                                        continue;

                                    default:
                                        DECLARE_PATH(0x20);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                        Exc = new AnsiException(ERR900, OE->OperandLeft._DEBUG_INFO_LINE, 900, OE->OperandLeft._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        break;
                                }
                                continue;

                            case KEY_SUM:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                    case VARIABLE_NUMBER:
                                    case VARIABLE_CLASS:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                        RETURN_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->NewArray();
                                        ((Array *)RETURN_DATA->CLASS_DATA)->AddCopy(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                        if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                            FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                            LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                            RETURN_DATA->LINKS++;
                                        }
                                        DECLARE_PATH(RETURN_DATA->TYPE);
                                        continue;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                        RETURN_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->NewArray();
                                        ((Array *)RETURN_DATA->CLASS_DATA)->Concat((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA);
                                        if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                            FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                            LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                            RETURN_DATA->LINKS++;
                                        }
                                        DECLARE_PATH(RETURN_DATA->TYPE);
                                        continue;
                                }
                                break;

                            case KEY_ASU:
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_STRING:
                                    case VARIABLE_NUMBER:
                                    case VARIABLE_CLASS:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                        RETURN_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->AddCopy(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                        if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                            FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                            LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                            RETURN_DATA->LINKS++;
                                        }
                                        DECLARE_PATH(RETURN_DATA->TYPE);
                                        continue;

                                    case VARIABLE_ARRAY:
                                        //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                        CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Concat((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA);
                                        if (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA) {
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_ARRAY;
                                            ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                            DECLARE_PATH(VARIABLE_ARRAY);
                                        } else {
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                            DECLARE_PATH(VARIABLE_NUMBER);
                                        }
                                        continue;
                                }
                                break;

                            case KEY_LENGTH:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count();
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_EQU:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_ARRAY) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                } else {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA);
                                }
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_NEQ:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_ARRAY) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                } else {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA);
                                }
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_BAN:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() && LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() && CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() != 0);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() && ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;
                                }
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_BOR:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() || LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() || CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() || ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;
                                }
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_NOT:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() == 0;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_CND_NULL:
                                if ((LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_ARRAY) && (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA) && (((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count())) {
                                    DECLARE_PATH(VARIABLE_ARRAY);
                                    continue;
                                }
                                // ------------------- //
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                // ------------------- //
                                if (((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count()) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                    ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->LINKS++;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_ARRAY;
                                    DECLARE_PATH(VARIABLE_ARRAY);
                                    continue;
                                }
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                        break;

                                    case VARIABLE_STRING:
                                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                                        DECLARE_PATH(VARIABLE_STRING);
                                        break;

                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA;

                                    case VARIABLE_CLASS:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                        ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                        DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                                        ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                        DECLARE_PATH(VARIABLE_ARRAY);
                                        break;
                                }
                                continue;

                            default:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                DECLARE_PATH(0x20);
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                Exc = new AnsiException(ERR910, OE->Operator._DEBUG_INFO_LINE, 910,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                break;
                        }
                        break;

                    case VARIABLE_DELEGATE:
                        switch (OE_Operator_ID) {
                            case KEY_ASG:
                            case KEY_BY_REF:
                                // ------------------- //
                                if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) {
                                    //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                    CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                } else {
                                    ((CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->LINKS -= 1;
                                }

                                // ------------------- //
                                LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA    = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                ((CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->LINKS += 2;
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_DELEGATE;
                                //----------------//
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                //----------------//
                                DECLARE_PATH(VARIABLE_DELEGATE);
                                continue;

                            case KEY_EQU:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_DELEGATE) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                } else {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) && (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->DELEGATE_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA));
                                }
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_NEQ:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_DELEGATE) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                } else {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) || (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->DELEGATE_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA));
                                }
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_BAN:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA != 0);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length() != 0);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count() != 0);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;
                                }
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_BOR:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                    case VARIABLE_STRING:
                                    case VARIABLE_CLASS:
                                    case VARIABLE_DELEGATE:
                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;
                                }
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_NOT:
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            case KEY_CND_NULL:
                                if ((LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_DELEGATE) && (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA) && (LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA == LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->DELEGATE_DATA)) {
                                    DECLARE_PATH(VARIABLE_DELEGATE);
                                    continue;
                                }
                                //---------------------------//
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                //---------------------------//
                                LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA    = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->DELEGATE_DATA;
                                ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;

                            default:
                                DECLARE_PATH(0x20);
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                Exc = new AnsiException(ERR960, OE->Operator._DEBUG_INFO_LINE, 960,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME);
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                break;
                        }
                        break;
                }
            } else
            if (OE->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) {
                switch (OE_Operator_ID) {
                    case KEY_OPTIMIZED_IF:

                        DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE);
                        switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                            case VARIABLE_STRING:
                                if (!CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length()) {
                                    if ((OE->OperandReserved.ID < INSTRUCTION_POINTER) && (TARGET_THREAD != TARGET_THREAD->NEXT)) {
                                        DO_EXECUTE  = 0;
                                        PREC_THREAD = TARGET_THREAD;
                                        TARGET_THREAD->CATCH_INSTRUCTION_POINTER = CATCH_INSTRUCTION_POINTER;
                                        TARGET_THREAD->CATCH_VARIABLE            = CATCH_VARIABLE;
                                        TARGET_THREAD->PREVIOUS_TRY        = PREVIOUS_TRY;
                                        TARGET_THREAD->INSTRUCTION_POINTER = OE->OperandReserved.ID;
                                        TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;
                                    }
                                    INSTRUCTION_POINTER = OE->OperandReserved.ID;
                                }
                                continue;

                            case VARIABLE_NUMBER:
                                if (!LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) {
                                    if ((OE->OperandReserved.ID < INSTRUCTION_POINTER) && (TARGET_THREAD != TARGET_THREAD->NEXT)) {
                                        DO_EXECUTE  = 0;
                                        PREC_THREAD = TARGET_THREAD;
                                        TARGET_THREAD->CATCH_INSTRUCTION_POINTER = CATCH_INSTRUCTION_POINTER;
                                        TARGET_THREAD->CATCH_VARIABLE            = CATCH_VARIABLE;
                                        TARGET_THREAD->PREVIOUS_TRY        = PREVIOUS_TRY;
                                        TARGET_THREAD->INSTRUCTION_POINTER = OE->OperandReserved.ID;
                                        TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;
                                    }
                                    INSTRUCTION_POINTER = OE->OperandReserved.ID;
                                }
                                continue;

                            case VARIABLE_ARRAY:
                                if (!((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count()) {
                                    if ((OE->OperandReserved.ID < INSTRUCTION_POINTER) && (TARGET_THREAD != TARGET_THREAD->NEXT)) {
                                        DO_EXECUTE  = 0;
                                        PREC_THREAD = TARGET_THREAD;
                                        TARGET_THREAD->CATCH_INSTRUCTION_POINTER = CATCH_INSTRUCTION_POINTER;
                                        TARGET_THREAD->CATCH_VARIABLE            = CATCH_VARIABLE;
                                        TARGET_THREAD->PREVIOUS_TRY        = PREVIOUS_TRY;
                                        TARGET_THREAD->INSTRUCTION_POINTER = OE->OperandReserved.ID;
                                        TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;
                                    }
                                    INSTRUCTION_POINTER = OE->OperandReserved.ID;
                                }
                                continue;
                        }
                        break;

                    case KEY_OPTIMIZED_GOTO:
                        DECLARE_PATH(VARIABLE_NUMBER);
                        if ((OE->OperandReserved.ID < INSTRUCTION_POINTER) && (TARGET_THREAD != TARGET_THREAD->NEXT)) {
                            DO_EXECUTE  = 0;
                            PREC_THREAD = TARGET_THREAD;
                            TARGET_THREAD->CATCH_INSTRUCTION_POINTER = CATCH_INSTRUCTION_POINTER;
                            TARGET_THREAD->CATCH_VARIABLE            = CATCH_VARIABLE;
                            TARGET_THREAD->PREVIOUS_TRY        = PREVIOUS_TRY;
                            TARGET_THREAD->INSTRUCTION_POINTER = OE->OperandReserved.ID;
                            TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;
                        }
                        INSTRUCTION_POINTER = OE->OperandReserved.ID;
                        continue;

                    case KEY_OPTIMIZED_ECHO:
                        DECLARE_PATH(1);
                        if (PIF->out) {
                            WRITE_LOCK
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    PIF->out->Print(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]), CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length());
                                    break;

                                case VARIABLE_NUMBER:
                                    PIF->out->Print(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA);
                                    break;

                                case VARIABLE_CLASS:
                                    CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                    PIF->out->Print(CCTEMP->_Class->NAME);
                                    break;

                                case VARIABLE_ARRAY:
                                    {
                                        AnsiString temp(((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->ToString());
                                        PIF->out->Print2(temp);
                                    }
                                    break;

                                case VARIABLE_DELEGATE:
                                    CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                    PIF->out->Print(CCTEMP->_Class->NAME);
                                    PIF->out->Print("::");
                                    PIF->out->Print(CCTEMP->_Class->pMEMBERS [(INTEGER)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA - 1]->NAME);
                                    break;
                            }
                        }
                        continue;

                    case KEY_OPTIMIZED_RETURN:
                        DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE);
                        WRITE_UNLOCK
                            INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                        break;

                    case KEY_OPTIMIZED_THROW:
                        DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE);

                        if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                            LOCAL_CONTEXT [OE->OperandRight.ID - 1]->LINKS++;
                            THROW_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1];

                            FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                            LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                            INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                            THROW_DATA                = 0;
                            CATCH_INSTRUCTION_POINTER = 0;
                            CATCH_VARIABLE            = 0;
                            // check for recursive try catch bug April 16, 2012 //
                            if (PREVIOUS_TRY)
                                PREVIOUS_TRY = OPT->CODE [PREVIOUS_TRY - 1].OperandLeft.ID;
                            // end of check for recursive try catch bug         //
                            //------------------//
                            RESTORE_TRY_DATA(THIS_REF);
                            //------------------//
                        } else {
                            WRITE_UNLOCK
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1300, OE->OperandRight._DEBUG_INFO_LINE, 1300, OE->OperandRight._PARSE_DATA, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->NAME, THIS_REF->OWNER->NAME));
                            INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                        }
                        continue;

                    case KEY_OPTIMIZED_TRY_CATCH:
                        DECLARE_PATH(1);
                        CATCH_INSTRUCTION_POINTER = OE->OperandReserved.ID;
                        CATCH_VARIABLE            = OE->Result_ID;
                        PREVIOUS_TRY = OE->OperandLeft.ID;
                        continue;

                    case KEY_OPTIMIZED_END_CATCH:
                        DECLARE_PATH(1);
                        CATCH_INSTRUCTION_POINTER = 0;
                        CATCH_VARIABLE            = 0;
                        THROW_DATA = 0;
                        //------------------//
                        RESTORE_TRY_DATA(THIS_REF);
                        //------------------//
                        continue;

                    case KEY_OPTIMIZED_DEBUG_TRAP:
                        DECLARE_PATH(1);
                        if (PIF->DEBUGGER_TRAP) {
                            WRITE_UNLOCK
                            if (PIF->DEBUGGER_TRAP(OPT->DATA, LOCAL_CONTEXT, OPT->dataCount, OE->Operator._DEBUG_INFO_LINE, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME.c_str(), (GET_VARIABLE_PROC)GetVariableByName, PIF->DEBUGGER_RESERVED, PIF, STACK_TRACE)) {
                                INSTRUCTION_POINTER = INSTRUCTION_COUNT;
                            }
                        }
                        continue;
                }
            }
        }
        WRITE_UNLOCK
        if (DO_EXECUTE) {
            if (TARGET_THREAD->LOCAL_CONTEXT) {
                TARGET_THREAD->STACK_TRACE.len = -1;
                ((ConceptInterpreter *)TARGET_THREAD->INTERPRETER)->DestroyEnviroment(PIF, TARGET_THREAD->LOCAL_CONTEXT, &TARGET_THREAD->STACK_TRACE);
                TARGET_THREAD->STACK_TRACE.LOCAL_CONTEXT = 0;
                TARGET_THREAD->STACK_TRACE.len           = 0;
                TARGET_THREAD->LOCAL_CONTEXT             = 0;
            }

            bool ended = false;
            if (TARGET_THREAD == TARGET_THREAD->NEXT)
                ended = true;

            GreenThreadCycle *TOFREE = 0;
            if (TARGET_THREAD != GREEN)
                TOFREE = TARGET_THREAD;
            else
                TARGET_THREAD->INSTRUCTION_POINTER = INSTRUCTION_POINTER;

            if (PREC_THREAD) {
                PREC_THREAD->NEXT = TARGET_THREAD->NEXT;

                if (PREC_THREAD == TARGET_THREAD) {
                    PREC_THREAD = (GreenThreadCycle *)PREC_THREAD->NEXT;
                    GreenThreadCycle *ORIG = TARGET_THREAD;
                    TARGET_THREAD = (GreenThreadCycle *)PREC_THREAD;

                    if (TARGET_THREAD->NEXT == ORIG)
                        PREC_THREAD->NEXT = PREC_THREAD;
                }
            }
            TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;

            if ((TOFREE) && ((TOFREE != TARGET_THREAD) || (ended)))
                ClassMember::DoneThread(TOFREE);

            if (ended)
                break;
        }
    }
    if (GREEN)
        ClassMember::DoneThread(GREEN);
    return 0;
}

#ifdef SIMPLE_MULTI_THREADING
int ConceptInterpreter::EvalNumberExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked, char pushed_type) {
#else
int ConceptInterpreter::EvalNumberExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char pushed_type) {
#endif
    CompiledClass  *CCTEMP;
    DECLARE_PATH(VARIABLE_NUMBER);
    Optimizer *OPT = (Optimizer *)this->OWNER->OPTIMIZER;
    switch (OE->Operator.ID) {
        // assigment operators
        case KEY_INC:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ++LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_DEC:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = --LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_DEC_LEFT:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA)--;
            //----------------//
            PROPERTY_CODE_LEFT(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_MUL:
            EVAL_NUMBER_EXPRESSION(this, *)
            return 1;

        case KEY_DIV:
            EVAL_DIVIDE_EXPRESSION(this, /)
            return 1;

        case KEY_REM:
            EVAL_DIVIDE_INTEGER_EXPRESSION(this, %)
            return 1;

        case KEY_ADI:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            EVAL_NUMBER_EXPRESSION(this, -= )
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_AMU:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            EVAL_NUMBER_EXPRESSION(this, *= )
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_ADV:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            EVAL_DIVIDE_EXPRESSION(this, /= )
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_LEQ:
            EVAL_NUMBER_EXPRESSION(this, <=)
            return 1;

        case KEY_GRE:
            EVAL_NUMBER_EXPRESSION(this, >)
            return 1;

        case KEY_GEQ:
            EVAL_NUMBER_EXPRESSION(this, >=)
            return 1;

        case KEY_EQU:
            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA == CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat();
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_ARRAY:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                    break;

                default:
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
            }
            return 1;

        case KEY_NEQ:
            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat();
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_ARRAY:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                    break;

                default:
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
            }
            return 1;

        case KEY_AND:
            EVAL_INTEGER_EXPRESSION(this, &)
            return 1;

        case KEY_XOR:
            EVAL_INTEGER_EXPRESSION(this, ^)
            return 1;

        case KEY_OR:
            EVAL_INTEGER_EXPRESSION(this, |)
            return 1;

        case KEY_BAN:
            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA && LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA && CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != 0);
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA && ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                    break;
            }
            return 1;

        case KEY_BOR:
            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA || LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA || CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA || ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                    break;
            }
            return 1;

        case KEY_SHL:
            EVAL_INTEGER_EXPRESSION(this, <<)
            return 1;

        case KEY_SHR:
            EVAL_INTEGER_EXPRESSION(this, >>)
            return 1;

        case KEY_ARE:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            EVAL_ASG_INTEGER_DIVIDE_EXPRESSION(this, %)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_AAN:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            EVAL_ASG_INTEGER_EXPRESSION(this, &)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_AXO:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            EVAL_ASG_INTEGER_EXPRESSION(this, ^)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_AOR:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            EVAL_ASG_INTEGER_EXPRESSION(this, |)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_ASL:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            EVAL_ASG_INTEGER_EXPRESSION(this, <<)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        case KEY_ASR:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            EVAL_ASG_INTEGER_EXPRESSION(this, >>)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(this, PROPERTIES)
            //----------------//
            return 1;

        // unary operators
        case KEY_NOT:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = !LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
            return 1;

        case KEY_COM:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ~(unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
            return 1;

        case KEY_POZ:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
            return 1;

        case KEY_NEG:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = -LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
            return 1;

        case KEY_CND_NULL:
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
            // ------------------- //
            if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA) {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                return 1;
            }
            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:

                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                    break;

                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA;

                case VARIABLE_CLASS:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                    ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                    ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                    break;
            }
            return 1;

        case KEY_VALUE:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
            return 1;

        default:
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR660, OE->Operator._DEBUG_INFO_LINE, 660,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
            break;
    }
    return 1;
}

VariableDATA *ConceptInterpreter::Interpret(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE) {
    Optimizer        *OPT = (Optimizer *)this->OWNER->OPTIMIZER;
    register INTEGER INSTRUCTION_POINTER = 0;
    register INTEGER INSTRUCTION_COUNT   = OPT->codeCount;
    char             *STATIC_ERROR       = 0;
    ParamList        *FORMAL_PARAMETERS;
    VariableDATA     *RESULT;
    CompiledClass    *CCTEMP;
    INTEGER          OPERATOR_ID;
    ClassCode        *CC;
    void             *instancePTR;
    VariableDATA     *RETURN_DATA;
    AnsiException    *Exc;
    char             pushed_type, next_is_asg;
    register RuntimeOptimizedElement *CODE = OPT->CODE;

#ifdef SIMPLE_MULTI_THREADING
    char IsWriteLocked = 0;
#endif
    INTEGER CATCH_INSTRUCTION_POINTER = 0;
    INTEGER CATCH_VARIABLE            = 0;
    INTEGER PREVIOUS_TRY = 0;

    ParamListExtra FORMAL_PARAMETERS2;
    FORMAL_PARAMETERS2.PIF = PIF;

    ParamList OPERATOR_PARAM;
    INTEGER   PARAM_INDEX[1];

#ifdef USE_JIT_TRACE
    WRITE_LOCK
    if (callcount == 1) {
        if (!jittracecode)
            this->AnalizeInstructionPath(OPT);
    } else
        callcount++;
    void **jittrace = jittracecode;
    WRITE_UNLOCK;
    executable_code code;
#endif
    VariableDATAPROPERTY *PROPERTIES = NULL;
    while (INSTRUCTION_POINTER < INSTRUCTION_COUNT) {
        WRITE_UNLOCK
#ifdef USE_JIT_TRACE
        if (jittrace) {
            if (jittrace[INSTRUCTION_POINTER]) {
                int res;
                do {
                    code.code = jittrace[INSTRUCTION_POINTER];
                    res       = code.func1((sljit_sw)LOCAL_CONTEXT);
                    if (res == INSTRUCTION_POINTER)
                        break;
                    INSTRUCTION_POINTER = res;
                } while ((INSTRUCTION_POINTER < INSTRUCTION_COUNT) && (jittrace[INSTRUCTION_POINTER]));

                if (INSTRUCTION_POINTER >= INSTRUCTION_COUNT)
                    break;
            }
        }
#endif
        register RuntimeOptimizedElement *OE = &CODE [INSTRUCTION_POINTER++];
        register int OE_Operator_ID          = OE->Operator.ID;
        if (OE->Operator.TYPE == TYPE_OPERATOR) {
            //WRITE_LOCK
            switch (OE_Operator_ID) {
                case KEY_ASG:
                    {
                        WRITE_LOCK
                        if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE != VARIABLE_CLASS) {
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1]);
                            CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                            // ------------------- //
                            pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                            char asg_type = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                            if ((asg_type == VARIABLE_CLASS) || (asg_type == VARIABLE_ARRAY)) {
                                int delta = 0;
                                if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) {
                                    //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                    CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                } else {
                                    delta = -1;
                                }

                                LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                if (asg_type == VARIABLE_CLASS) {
                                    ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2 + delta;
                                } else {
                                    ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2 + delta;
                                }
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT)) {
                                    CCTEMP = (CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET))->CLASS_DATA;
                                    WRITE_UNLOCK
                                    CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET), &OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, LocalClassID, STACK_TRACE);
                                }
                                DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                                continue;
                            }
                        } else {
                            //---------------------------------------------------
                            CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                            ////////////////////////////////////////////////////////////
                            pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                            ////////////////////////////////////////////////////////////
                            if (CCTEMP->_Class->Relocation(DEF_ASG)) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_CLASS;
                            } else {
                                // ------------------- //
                                CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                                // ------------------- //
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                OE_Operator_ID = KEY_BY_REF;
                            }
                        }
                        if (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_NUMBER) {
                            CLASS_CHECK_TS(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE   = VARIABLE_NUMBER;
                            PROPERTY_CODE(this, PROPERTIES)
                            continue;
                        }
                    }
                    break;

                case KEY_SEL:
                    // execute member !!!
                    WRITE_LOCK
                    if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE != VARIABLE_CLASS) {
                        Exc = new AnsiException(ERR240, OE->Operator._DEBUG_INFO_LINE, 240, OE->OperandLeft._PARSE_DATA, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                        DECLARE_PATH(0x20);
                        continue;
                    }
                    CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;

                    if (OE->OperandReserved.ID) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved.ID - 1];
                        next_is_asg       = 0;
                    } else {
                        next_is_asg       = OE->OperandReserved.TYPE;
                        FORMAL_PARAMETERS = 0;
                    }
                    try {
                        //-------------------------------------------//
#ifndef SIMPLE_MULTI_THREADING
                        bool not_executed = true;
                        if ((FORMAL_PARAMETERS) && (CCTEMP->_Class == this->OWNER->Defined_In)) {
                            int         relocation = CCTEMP->_Class->Relocation(OE->OperandRight.ID - 1);
                            ClassMember *pMEMBER_i = relocation ? CCTEMP->_Class->pMEMBERS [relocation - 1] : 0;
                            if ((pMEMBER_i == this->OWNER) && (FORMAL_PARAMETERS->COUNT == pMEMBER_i->MUST_PARAMETERS_COUNT)) {
                                not_executed = false;
                                VariableDATA *LOCAL_THROW = NULL;
                                WRITE_UNLOCK
                                RESULT = pMEMBER_i->Execute(PIF, ClassID, LOCAL_CONTEXT [OE->OperandLeft.ID - 1], FORMAL_PARAMETERS, LOCAL_CONTEXT, LOCAL_THROW, STACK_TRACE);
                                if (LOCAL_THROW)
                                    throw LOCAL_THROW;
                                WRITE_LOCK
                            }
                        }
                        if (not_executed) {
#endif
                        WRITE_UNLOCK
                        RESULT = CCTEMP->_Class->ExecuteMember(PIF, OE->OperandRight.ID - 1, LOCAL_CONTEXT [OE->OperandLeft.ID - 1], &OE->OperandRight, CCTEMP->_Class->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, LocalClassID, STACK_TRACE, next_is_asg, &PROPERTIES, OPT->dataCount, OE->Result_ID - 1);
                        WRITE_LOCK
#ifndef SIMPLE_MULTI_THREADING
                        }
#endif
                        //-------------------------------------------//
                        if (RESULT == LOCAL_CONTEXT [OE->Result_ID - 1]) {
                            DECLARE_PATH(RESULT->TYPE);
                            continue;
                        }
                        if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                            WRITE_UNLOCK
                            CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                        }
                        if (RESULT) {
                            FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                            LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                            RESULT->LINKS++;
                        } else {
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                        }
                    } catch (VariableDATA *LAST_THROW) {
                        THROW_DATA = LAST_THROW;
                        DECLARE_PATH(LAST_THROW->TYPE);
                        if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                            FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                            LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                            INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                            THROW_DATA                = 0;
                            CATCH_INSTRUCTION_POINTER = 0;
                            CATCH_VARIABLE            = 0;
                            //-------------//
                            RESTORE_TRY_DATA(this);
                            //-------------//
                        } else {
                            WRITE_UNLOCK
                            if (PROPERTIES)
                                FAST_FREE(PROPERTIES);
                            return 0;
                        }
                    }
                    DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                    continue;

                case KEY_NEW:
                    //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1]);
                    CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                    if (OE->OperandLeft.ID == STATIC_CLASS_ARRAY) {
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = new(AllocArray(PIF))Array(PIF);
                        DECLARE_PATH(VARIABLE_ARRAY);
                    } else {
                        CC = PIF->StaticClassList[OE->OperandLeft.ID - 1];
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_CLASS;
                        FORMAL_PARAMETERS = 0;
                        if (OE->OperandReserved.ID) {
                            FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved.ID - 1];
                        }
                        WRITE_UNLOCK;
                        instancePTR = CC->CreateInstance(PIF, LOCAL_CONTEXT [OE->Result_ID - 1], &OE->OperandLeft, FORMAL_PARAMETERS, LOCAL_CONTEXT, STACK_TRACE);

                        LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = instancePTR;

                        if (!instancePTR) {
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                        } else {
                            DECLARE_PATH(VARIABLE_CLASS);
                        }
                    }
                    continue;

                case KEY_DLL_CALL:
                    WRITE_LOCK
                    FORMAL_PARAMETERS = 0;
                    STATIC_ERROR = 0;
                    if (OE->OperandReserved.ID) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved.ID - 1];
                    }
                    if (OE->OperandLeft.ID == STATIC_CLASS_DELEGATE) {
                        // call to delegate function
                        VariableDATA *lOwner = 0;
                        try {
                            //-------------------------------------------//
                            if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_DELEGATE) {
                                Exc = new AnsiException(ERR970, OE->OperandRight._DEBUG_INFO_LINE, 970, OE->OperandRight._PARSE_DATA, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                DECLARE_PATH(0x20);
                                continue;
                            }
                            CCTEMP                     = (CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                            lOwner                     = (VariableDATA *)VAR_ALLOC(PIF);
                            lOwner->CLASS_DATA         = CCTEMP;
                            lOwner->IS_PROPERTY_RESULT = 0;
                            lOwner->LINKS              = 1;
                            lOwner->TYPE               = VARIABLE_CLASS;
                            CCTEMP->LINKS++;

                            WRITE_UNLOCK
                            RESULT = CCTEMP->_Class->ExecuteDelegate(PIF,
                                                                         (INTEGER)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA,
                                                                         lOwner,
                                                                         &OE->OperandRight,
                                                                         FORMAL_PARAMETERS,
                                                                         LOCAL_CONTEXT,
                                                                         ClassID,
                                                                         LocalClassID,
                                                                         STACK_TRACE);
                            WRITE_LOCK
                            FREE_VARIABLE(lOwner);

                            if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                            if (RESULT == LOCAL_CONTEXT [OE->Result_ID - 1]) {
                                DECLARE_PATH(RESULT->TYPE);
                                continue;
                            }
                            if (RESULT) {
                                FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                                RESULT->LINKS++;
                            }
                        } catch (VariableDATA *LAST_THROW) {
                            FREE_VARIABLE(lOwner);
                            DECLARE_PATH(LAST_THROW->TYPE);
                            THROW_DATA = LAST_THROW;
                            if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                                FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                                LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                                INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                                THROW_DATA                = 0;
                                CATCH_INSTRUCTION_POINTER = 0;
                                CATCH_VARIABLE            = 0;
                                //-------------//
                                RESTORE_TRY_DATA(this);
                                //-------------//
                            } else {
                                WRITE_UNLOCK
                                if (PROPERTIES)
                                    FAST_FREE(PROPERTIES);
                                return 0;
                            }
                        }
                        DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                        continue;
                    } else
                    if (OE->OperandLeft.ID == STATIC_CLASS_DLL) {
                        if ((FORMAL_PARAMETERS) && (FORMAL_PARAMETERS->COUNT)) {
                            FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                            FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);
                        } else {
                            FORMAL_PARAMETERS2.COUNT       = 0;
                            FORMAL_PARAMETERS2.PARAM_INDEX = 0;
                        }
                        STACK(STACK_TRACE, OE->OperandLeft._DEBUG_INFO_LINE)
                        WRITE_UNLOCK
                        try {
                            STATIC_ERROR = (char *)((EXTERNAL_CALL)(PIF->StaticLinks [OE->OperandRight.ID - 1]))(&FORMAL_PARAMETERS2, LOCAL_CONTEXT, LOCAL_CONTEXT [OE->Result_ID - 1], SetVariable, GetVariable, PIF->out->sock, PIF->SERVER_PUBLIC_KEY, PIF->SERVER_PRIVATE_KEY, PIF->CLIENT_PUBLIC_KEY, SetClassMember, GetClassMember, Invoke);
                        } catch (...) {
                            Exc = new AnsiException(ERR690, OE->OperandLeft._DEBUG_INFO_LINE, 690, OE->OperandRight._PARSE_DATA, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                        }
                        WRITE_LOCK
                        UNSTACK;
                        if (STATIC_ERROR) {
                            if (PIF->USE_EXC) {
                                THROW_DATA = (VariableDATA *)VAR_ALLOC(PIF);
                                THROW_DATA->NUMBER_DATA        = 0;
                                THROW_DATA->CLASS_DATA         = 0;
                                THROW_DATA->LINKS              = 1;
                                THROW_DATA->IS_PROPERTY_RESULT = 0;

                                CONCEPT_STRING(THROW_DATA) = STATIC_ERROR;

                                DECLARE_PATH(VARIABLE_STRING);
                                if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                                    FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                                    LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                                    INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                                    THROW_DATA                = 0;
                                    CATCH_INSTRUCTION_POINTER = 0;
                                    CATCH_VARIABLE            = 0;
                                    //--------------//
                                    RESTORE_TRY_DATA(this);
                                    //--------------//
                                } else {
                                    WRITE_UNLOCK
                                    if (PROPERTIES)
                                        FAST_FREE(PROPERTIES);
                                    return 0;
                                }
                            } else {
                                DECLARE_PATH(0x20);
                                Exc = new AnsiException(ERR700, OE->OperandLeft._DEBUG_INFO_LINE, 700, AnsiString(OE->OperandLeft._PARSE_DATA.c_str()) + AnsiString(": ") + AnsiString(STATIC_ERROR), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                RESET_VARIABLE(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                        }
                    } else {
                        try {
                            CC = PIF->StaticClassList[OE->OperandLeft.ID - 1];
                            WRITE_UNLOCK
                            RESULT = CC->ExecuteMember(PIF, OE->OperandRight.ID - 1, LOCAL_CONTEXT [0], &OE->OperandRight, CC->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, LocalClassID, STACK_TRACE);
                            WRITE_LOCK
                            //-------------------------------------------//
                            if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                            if (RESULT) {
                                FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                                RESULT->LINKS++;
                            } else {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                        } catch (VariableDATA *LAST_THROW) {
                            THROW_DATA = LAST_THROW;
                            DECLARE_PATH(LAST_THROW->TYPE);
                            if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                                FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                                LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                                INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                                THROW_DATA                = 0;
                                CATCH_INSTRUCTION_POINTER = 0;
                                CATCH_VARIABLE            = 0;
                                //-------------//
                                RESTORE_TRY_DATA(this);
                                //-------------//
                            } else {
                                WRITE_UNLOCK
                                if (PROPERTIES)
                                    FAST_FREE(PROPERTIES);
                                return 0;
                            }
                        }
                    }
                    DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                    continue;

                case KEY_TYPE_OF:
                    //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                    //==================//
                    CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                    //==================//
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_STRING;
                    switch (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE) {
                        case VARIABLE_NUMBER:
                            CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "numeric";
                            break;

                        case VARIABLE_STRING:
                            CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "string";
                            break;

                        case VARIABLE_CLASS:
                            CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "class";
                            break;

                        case VARIABLE_ARRAY:
                            CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "array";
                            break;

                        case VARIABLE_DELEGATE:
                            CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "delegate";
                            break;

                        default:
                            CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "";
                    }
                    DECLARE_PATH(VARIABLE_STRING);
                    continue;

                case KEY_CLASS_NAME:
                    //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                    //==================//
                    CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                    //==================//
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_STRING;
                    if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE != VARIABLE_CLASS) {
                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = "";
                    } else {
                        CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                        CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CCTEMP->_Class->NAME.c_str();
                    }
                    DECLARE_PATH(VARIABLE_STRING);
                    continue;

                case KEY_BY_REF:
                    WRITE_LOCK
                    CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                    if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE == VARIABLE_NUMBER) {
                        CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                        LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE   = VARIABLE_NUMBER;
                        PROPERTY_CODE(this, PROPERTIES)
                        continue;
                    }

                    pushed_type = VARIABLE_NUMBER;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                    break;

                case KEY_DELETE:
                    //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                    CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                    LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA  = 0;
                    LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE        = VARIABLE_NUMBER;
                    LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA = 0;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    continue;

                default:
                    SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                    SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                    if (OE->OperandLeft.ID) {
                        ////////////////////////////////////////////////////////////
                        pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                        ////////////////////////////////////////////////////////////
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE;
                        if (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_NUMBER) {
                            if (OE_Operator_ID == KEY_LES) {
                                EVAL_NUMBER_EXPRESSION(this, <)
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_INC_LEFT) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA)++;
                                PROPERTY_CODE_LEFT(this, PROPERTIES)
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_SUM) {
                                EVAL_NUMBER_EXPRESSION(this, +)
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_ASU) {
                                EVAL_NUMBER_EXPRESSION(this, += )
                                LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                                PROPERTY_CODE(this, PROPERTIES)
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_DIF) {
                                EVAL_NUMBER_EXPRESSION(this, -)
                                continue;
                            }
                        } else
                        if ((OE_Operator_ID == KEY_INDEX_OPEN) && (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_ARRAY)) {
                            WRITE_LOCK
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                    RETURN_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Get(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                    if (!RETURN_DATA) {
                                        DECLARE_PATH(0x20);
                                        Exc = new AnsiException(ERR1110, OE->OperandLeft._DEBUG_INFO_LINE, 1110, AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                        continue;
                                    }
                                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                        FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                        LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                        RETURN_DATA->LINKS++;
                                    }
                                    DECLARE_PATH(RETURN_DATA->TYPE);
                                    continue;

                                default:
                                    DECLARE_PATH(0x20);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                    Exc = new AnsiException(ERR900, OE->OperandLeft._DEBUG_INFO_LINE, 900, OE->OperandLeft._PARSE_DATA, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    continue;
                            }
                            continue;
                        }
                    }
                    // -------------------- pana aici --------------------------------//
            }

            switch (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
#ifndef USE_INLINE_EVALUATION
#ifdef SIMPLE_MULTI_THREADING
                    if (EvalNumberExpression(PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked, pushed_type))
#else
                    if (EvalNumberExpression(PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, pushed_type))
#endif
                        continue;
                    return 0;
#else
                    DECLARE_PATH(VARIABLE_NUMBER);
                    switch (OE_Operator_ID) {
                        case KEY_INC:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ++LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_DEC:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = --LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_DEC_LEFT:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA)--;
                            //----------------//
                            PROPERTY_CODE_LEFT(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_MUL:
                            EVAL_NUMBER_EXPRESSION(this, *)
                            continue;

                        case KEY_DIV:
                            EVAL_DIVIDE_EXPRESSION(this, /)
                            continue;

                        case KEY_REM:
                            EVAL_DIVIDE_INTEGER_EXPRESSION(this, %)
                            continue;

                        case KEY_ADI:
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            EVAL_NUMBER_EXPRESSION(this, -= )
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_AMU:
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            EVAL_NUMBER_EXPRESSION(this, *= )
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_ADV:
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            EVAL_DIVIDE_EXPRESSION(this, /= )
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_LEQ:
                            EVAL_NUMBER_EXPRESSION(this, <=)
                            continue;

                        case KEY_GRE:
                            EVAL_NUMBER_EXPRESSION(this, >)
                            continue;

                        case KEY_GEQ:
                            EVAL_NUMBER_EXPRESSION(this, >=)
                            continue;

                        case KEY_EQU:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    break;

                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA == CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat();
                                    break;

                                case VARIABLE_CLASS:
                                case VARIABLE_ARRAY:
                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                    break;

                                default:
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
                            }
                            continue;

                        case KEY_NEQ:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    break;

                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToFloat();
                                    break;

                                case VARIABLE_CLASS:
                                case VARIABLE_ARRAY:
                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                    break;

                                default:
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1204, OE->OperandRight._DEBUG_INFO_LINE, 1204, OE->OperandRight._PARSE_DATA, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
                            }
                            continue;

                        case KEY_AND:
                            EVAL_INTEGER_EXPRESSION(this, &)
                            continue;

                        case KEY_XOR:
                            EVAL_INTEGER_EXPRESSION(this, ^)
                            continue;

                        case KEY_OR:
                            EVAL_INTEGER_EXPRESSION(this, |)
                            continue;

                        case KEY_BAN:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA && LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    break;

                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA && CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                    break;

                                case VARIABLE_CLASS:
                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != 0);
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA && ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                    break;
                            }
                            continue;

                        case KEY_BOR:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA || LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    break;

                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA || CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                    break;

                                case VARIABLE_CLASS:
                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA || ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                    break;
                            }
                            continue;

                        case KEY_SHL:
                            EVAL_INTEGER_EXPRESSION(this, <<)
                            continue;

                        case KEY_SHR:
                            EVAL_INTEGER_EXPRESSION(this, >>)
                            continue;

                        case KEY_ARE:
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            EVAL_ASG_INTEGER_DIVIDE_EXPRESSION(this, %)
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_AAN:
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            EVAL_ASG_INTEGER_EXPRESSION(this, &)
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_AXO:
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            EVAL_ASG_INTEGER_EXPRESSION(this, ^)
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_AOR:
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            EVAL_ASG_INTEGER_EXPRESSION(this, |)
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_ASL:
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            EVAL_ASG_INTEGER_EXPRESSION(this, <<)
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_ASR:
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            EVAL_ASG_INTEGER_EXPRESSION(this, >>)
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_NUMBER;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        // unary operators
                        case KEY_NOT:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = !LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                            continue;

                        case KEY_COM:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ~(unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                            continue;

                        case KEY_POZ:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                            continue;

                        case KEY_NEG:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = -LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                            continue;

                        case KEY_CND_NULL:
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                            // ------------------- //
                            if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                                continue;
                            }
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    break;

                                case VARIABLE_STRING:

                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                                    break;

                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA;

                                case VARIABLE_CLASS:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                    ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                                    ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                    break;
                            }
                            continue;

                        case KEY_VALUE:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                            continue;

                        default:
                            Exc = new AnsiException(ERR660, OE->Operator._DEBUG_INFO_LINE, 660,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                            break;
                    }
                    break;
#endif

                case VARIABLE_STRING:
                    switch (OE_Operator_ID) {
                        // unary operators
                        case KEY_NOT:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() == 0);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_LENGTH:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length();
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_VALUE:
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).ToFloat();
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_INC:
                            if (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length()) {
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]).LoadBuffer(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).c_str() + 1, CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() - 1);
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                //----------------//
                                PROPERTY_CODE(this, PROPERTIES)
                                //----------------//
                            }
                            DECLARE_PATH(VARIABLE_STRING);
                            continue;

                        case KEY_INC_LEFT:
                            if (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length()) {
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).LoadBuffer(CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]).c_str() + 1, CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() - 1);
                                //----------------//
                                PROPERTY_CODE_LEFT(this, PROPERTIES)
                                //----------------//
                            }
                            DECLARE_PATH(VARIABLE_STRING);
                            continue;

                        // binary ...
                        case KEY_SUM:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]).Sum(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]), CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]));
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                case VARIABLE_NUMBER:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) + AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA);
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            continue;

                        case KEY_EQU:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) == CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            continue;

                        case KEY_GRE:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) > CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) > AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            continue;

                        case KEY_LES:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) < CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) < AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            continue;

                        case KEY_GEQ:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) >= CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) >= AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            continue;

                        case KEY_LEQ:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) <= CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) <= AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            continue;

                        case KEY_NEQ:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) != CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]) ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA ? 1 : 0;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            continue;

                        case KEY_BY_REF:
                        case KEY_ASG:
                            // ------------------- //
                            ////SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            WRITE_LOCK
                            CLASS_CHECK_KEEP_EXTRA(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // ------------------- //
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_STRING;
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) {
                                        FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                        LOCAL_CONTEXT [OE->Result_ID - 1] = LOCAL_CONTEXT [OE->OperandLeft.ID - 1];
                                        LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->LINKS++;
                                    }
                                    //----------------------------------------------------------------------------------------//
                                    UPDATE_STRING_VARIABLE_THAT_HAS_INDEX(this, LOCAL_CONTEXT [OE->OperandLeft.ID - 1], PROPERTIES);
                                    //----------------------------------------------------------------------------------------//
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                case VARIABLE_NUMBER:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1])      = AnsiString(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA);
                                    //----------------------------------------------------------------------------------------//
                                    UPDATE_STRING_VARIABLE_THAT_HAS_INDEX(this, LOCAL_CONTEXT [OE->OperandLeft.ID - 1], PROPERTIES);
                                    //----------------------------------------------------------------------------------------//
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_ASU:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).IncreaseBuffer(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length());
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) += CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) {
                                        FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                        LOCAL_CONTEXT [OE->Result_ID - 1] = LOCAL_CONTEXT [OE->OperandLeft.ID - 1];
                                        LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->LINKS++;
                                    }
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                case VARIABLE_NUMBER:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) += AnsiString(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA);
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1])       = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650, GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            continue;

                        case KEY_INDEX_OPEN:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) [(INTEGER)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA];
                                    ((AnsiString *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->EXTRA_DATA = INSTRUCTION_POINTER;
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                case VARIABLE_STRING:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]) [(INTEGER)CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).ToInt()];
                                    ((AnsiString *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->EXTRA_DATA = INSTRUCTION_POINTER;
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                default:
                                    DECLARE_PATH(0x20);
                                    Exc = new AnsiException(ERR640, OE->OperandLeft._DEBUG_INFO_LINE, 640, OE->OperandLeft._PARSE_DATA, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                                    PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                                    break;
                            }
                            break;

                        case KEY_BAN:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() && LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() && CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_CLASS:
                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() != 0);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() && ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;
                            }
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_BOR:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() || LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() || CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_CLASS:
                                case VARIABLE_DELEGATE:
                                    // always true for class and delegates
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length() || ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;
                            }
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_CND_NULL:
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                            // ------------------- //
                            if (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]).Length()) {
                                CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft.ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                                DECLARE_PATH(VARIABLE_STRING);
                                continue;
                            }
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                case VARIABLE_STRING:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA;

                                case VARIABLE_CLASS:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                    ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                    DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                                    ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                    DECLARE_PATH(VARIABLE_ARRAY);
                                    break;
                            }
                            continue;

                        default:
                            DECLARE_PATH(0x20);
                            Exc = new AnsiException(ERR650, OE->Operator._DEBUG_INFO_LINE, 650,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                            break;
                    }
                    continue;

                case VARIABLE_CLASS:
                    OPERATOR_ID = 0;
                    switch (OE_Operator_ID) {
                        case KEY_BY_REF:
                            //---------------------------//
                            if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) {
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            } else {
                                if (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) {
                                    ((CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->LINKS -= 2;
                                } else {
                                    CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                    ((CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->LINKS -= 1;
                                }
                            }
                            //---------------------------//

                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                            ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2;

                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_CLASS;
                            if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT)) {
                                CCTEMP = (CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET))->CLASS_DATA;
                                WRITE_UNLOCK
                                CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET), &OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, LocalClassID, STACK_TRACE);
                            }
                            DECLARE_PATH(VARIABLE_CLASS);
                            continue;

                        case KEY_ASG:
                            OPERATOR_ID = DEF_ASG;
                            break;

                        case KEY_INDEX_OPEN:
                            OPERATOR_ID = DEF_SELECTOR_I;
                            break;

                        case KEY_SUM:
                            OPERATOR_ID = DEF_SUM;
                            break;

                        case KEY_MUL:
                            OPERATOR_ID = DEF_MUL;
                            break;

                        case KEY_DIV:
                            OPERATOR_ID = DEF_DIV;
                            break;

                        case KEY_REM:
                            OPERATOR_ID = DEF_REM;
                            break;

                        case KEY_DIF:
                            OPERATOR_ID = DEF_DIF;
                            break;

                        case KEY_SHL:
                            OPERATOR_ID = DEF_SHL;
                            break;

                        case KEY_SHR:
                            OPERATOR_ID = DEF_SHR;
                            break;

                        case KEY_LES:
                            OPERATOR_ID = DEF_LES;
                            break;

                        case KEY_LEQ:
                            OPERATOR_ID = DEF_LEQ;
                            break;

                        case KEY_GRE:
                            OPERATOR_ID = DEF_GRE;
                            break;

                        case KEY_GEQ:
                            OPERATOR_ID = DEF_GEQ;
                            break;

                        case KEY_EQU:
                            CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                            if (!CCTEMP->_Class->Relocation(DEF_EQU)) {
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)

                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_CLASS:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    default:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                }
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;
                            }
                            OPERATOR_ID = DEF_EQU;
                            break;

                        case KEY_NEQ:
                            CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                            if (!CCTEMP->_Class->Relocation(DEF_NEQ)) {
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)

                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_CLASS:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA ? 1 : 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    default:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                }
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;
                            }
                            OPERATOR_ID = DEF_NEQ;
                            break;

                        case KEY_AND:
                            OPERATOR_ID = DEF_AND;
                            break;

                        case KEY_XOR:
                            OPERATOR_ID = DEF_XOR;
                            break;

                        case KEY_OR:
                            OPERATOR_ID = DEF_OR;
                            break;

                        case KEY_BAN:
                            CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                            if (!CCTEMP->_Class->Relocation(DEF_BAN)) {
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA != 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_STRING:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length() != 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_CLASS:
                                    case VARIABLE_DELEGATE:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;

                                    case VARIABLE_ARRAY:
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count() != 0;
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                        break;
                                }
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;
                            }

                            OPERATOR_ID = DEF_BAN;
                            break;

                        case KEY_BOR:
                            CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                            if (!CCTEMP->_Class->Relocation(DEF_BOR)) {
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;
                            }
                            OPERATOR_ID = DEF_BOR;
                            break;

                        case KEY_ASU:
                            OPERATOR_ID = DEF_ASU;
                            break;

                        case KEY_ADI:
                            OPERATOR_ID = DEF_ADI;
                            break;

                        case KEY_AMU:
                            OPERATOR_ID = DEF_AMU;
                            break;

                        case KEY_ADV:
                            OPERATOR_ID = DEF_ADV;
                            break;

                        case KEY_ARE:
                            OPERATOR_ID = DEF_ARE;
                            break;

                        case KEY_AAN:
                            OPERATOR_ID = DEF_AAN;
                            break;

                        case KEY_AXO:
                            OPERATOR_ID = DEF_AXO;
                            break;

                        case KEY_AOR:
                            OPERATOR_ID = DEF_AOR;
                            break;

                        case KEY_ASL:
                            OPERATOR_ID = DEF_ASL;
                            break;

                        case KEY_ASR:
                            OPERATOR_ID = DEF_ASR;
                            break;

                        // unari !
                        case KEY_NOT:
                            CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                            if (!CCTEMP->_Class->Relocation(DEF_NOT)) {
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                DECLARE_PATH(VARIABLE_NUMBER);
                                continue;
                            }
                            OPERATOR_ID = DEF_NOT;
                            break;

                        case KEY_COM:
                            OPERATOR_ID = DEF_COM;
                            break;

                        case KEY_INC_LEFT:
                        case KEY_INC:
                            OPERATOR_ID = DEF_INC;
                            break;

                        case KEY_DEC_LEFT:
                        case KEY_DEC:
                            OPERATOR_ID = DEF_DEC;
                            break;

                        case KEY_CND_NULL:
                            if ((LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_CLASS) && (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)) {
                                DECLARE_PATH(VARIABLE_CLASS);
                                continue;
                            }
                            //---------------------------//
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                            //---------------------------//
                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                            ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                            DECLARE_PATH(VARIABLE_CLASS);
                            continue;

                        default:
                            DECLARE_PATH(0x20);
                            Exc = new AnsiException(ERR920, OE->Operator._DEBUG_INFO_LINE, 920,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                            break;
                    }
                    //////////////////////////////////////////////////////
                    //////////////////////////////////////////////////////
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                    //////////////////////////////////////////////////////
                    if (!OPERATOR_ID) {
                        DECLARE_PATH(0x20);
                        continue;
                    }
                    if (OE->OperandRight.ID) {
                        OPERATOR_PARAM.COUNT       = 1;
                        PARAM_INDEX[0]             = OE->OperandRight.ID;
                        OPERATOR_PARAM.PARAM_INDEX = DELTA_REF(&OPERATOR_PARAM, PARAM_INDEX);

                    } else {
                        OPERATOR_PARAM.COUNT = 0;
                    }
                    CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                    try {
                        WRITE_UNLOCK
                        RESULT = CCTEMP->_Class->ExecuteMember(PIF, OPERATOR_ID, LOCAL_CONTEXT [OE->OperandLeft.ID - 1], & OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, &OPERATOR_PARAM, LOCAL_CONTEXT, 0, ClassID, LocalClassID, STACK_TRACE);
                        WRITE_LOCK
                        if (RESULT == LOCAL_CONTEXT [OE->Result_ID - 1]) {
                            DECLARE_PATH(RESULT->TYPE);
                            continue;
                        }
                        if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                            CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                        }
                        if (RESULT) {
                            FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1]);
                            LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                            RESULT->LINKS++;
                        } else {
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            DECLARE_PATH(0x20);
                        }
                    } catch (VariableDATA *LAST_THROW) {
                        DECLARE_PATH(VARIABLE_NUMBER);
                        THROW_DATA = LAST_THROW;
                        DECLARE_PATH(LAST_THROW->TYPE);
                        if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                            FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                            LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                            INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                            THROW_DATA                = 0;
                            CATCH_INSTRUCTION_POINTER = 0;
                            CATCH_VARIABLE            = 0;
                            //-------------//
                            RESTORE_TRY_DATA(this);
                            //-------------//
                        } else {
                            WRITE_UNLOCK
                            if (PROPERTIES)
                                FAST_FREE(PROPERTIES);
                            return 0;
                        }
                    }
                    break;

                case VARIABLE_ARRAY:
                    switch (OE_Operator_ID) {
                        case KEY_ASG:
                        case KEY_BY_REF:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            // next line was commented (?)
                            //---------------------------//
                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                            ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2;
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_ARRAY;
                            if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT)) {
                                CCTEMP = (CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET))->CLASS_DATA;
                                WRITE_UNLOCK
                                CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft.ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft.ID - 1].CALL_SET), &OE->OperandLeft, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, LocalClassID, STACK_TRACE);
                            }
                            DECLARE_PATH(VARIABLE_ARRAY);
                            continue;

                        case KEY_SUM:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                case VARIABLE_NUMBER:
                                case VARIABLE_CLASS:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                    RETURN_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->NewArray();
                                    ((Array *)RETURN_DATA->CLASS_DATA)->AddCopy(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                        FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                        LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                        RETURN_DATA->LINKS++;
                                    }
                                    DECLARE_PATH(RETURN_DATA->TYPE);
                                    continue;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                    RETURN_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->NewArray();
                                    ((Array *)RETURN_DATA->CLASS_DATA)->Concat((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA);
                                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                        FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                        LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                        RETURN_DATA->LINKS++;
                                    }
                                    DECLARE_PATH(RETURN_DATA->TYPE);
                                    continue;
                            }
                            break;

                        case KEY_ASU:
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                case VARIABLE_NUMBER:
                                case VARIABLE_CLASS:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                    RETURN_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->AddCopy(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                        FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                        LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                        RETURN_DATA->LINKS++;
                                    }
                                    DECLARE_PATH(RETURN_DATA->TYPE);
                                    continue;

                                case VARIABLE_ARRAY:
                                    //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                                    CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Concat((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA);
                                    if (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA) {
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_ARRAY;
                                        ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                        DECLARE_PATH(VARIABLE_ARRAY);
                                    } else {
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                        DECLARE_PATH(VARIABLE_NUMBER);
                                    }
                                    continue;
                            }
                            break;

                        case KEY_LENGTH:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count();
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_EQU:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_ARRAY) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            } else {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA);
                            }
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_NEQ:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_ARRAY) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                            } else {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA);
                            }
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_BAN:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() && LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() && CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_CLASS:
                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() != 0);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() && ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;
                            }
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_BOR:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() || LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() || CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length();
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_CLASS:
                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() || ((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count();
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;
                            }
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_NOT:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count() == 0;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_CND_NULL:
                            if ((LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_ARRAY) && (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA) && (((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count())) {
                                DECLARE_PATH(VARIABLE_ARRAY);
                                continue;
                            }
                            // ------------------- //
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                            // ------------------- //
                            if (((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->Count()) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                                ((Array *)LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA)->LINKS++;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_ARRAY;
                                DECLARE_PATH(VARIABLE_ARRAY);
                                continue;
                            }
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    DECLARE_PATH(VARIABLE_NUMBER);
                                    break;

                                case VARIABLE_STRING:
                                    CONCEPT_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                                    DECLARE_PATH(VARIABLE_STRING);
                                    break;

                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA;

                                case VARIABLE_CLASS:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                                    ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                    DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                                    ((Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                                    DECLARE_PATH(VARIABLE_ARRAY);
                                    break;
                            }
                            continue;

                        default:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            DECLARE_PATH(0x20);
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                            Exc = new AnsiException(ERR910, OE->Operator._DEBUG_INFO_LINE, 910,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                            break;
                    }
                    break;

                case VARIABLE_DELEGATE:
                    switch (OE_Operator_ID) {
                        case KEY_ASG:
                        case KEY_BY_REF:
                            // ------------------- //
                            if (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) {
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft.ID - 1])
                            } else {
                                ((CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->LINKS -= 1;
                            }

                            // ------------------- //
                            LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA    = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                            ((CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->LINKS += 2;
                            LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->TYPE = VARIABLE_DELEGATE;
                            //----------------//
                            PROPERTY_CODE(this, PROPERTIES)
                            //----------------//
                            DECLARE_PATH(VARIABLE_DELEGATE);
                            continue;

                        case KEY_EQU:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_DELEGATE) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            } else {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) && (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA == LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA));
                            }
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_NEQ:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE != VARIABLE_DELEGATE) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            } else {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA) || (LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA != LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA));
                            }
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_BAN:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA != 0);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_STRING:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length() != 0);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_CLASS:
                                case VARIABLE_DELEGATE:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;

                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count() != 0);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;
                            }
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_BOR:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
                                case VARIABLE_STRING:
                                case VARIABLE_CLASS:
                                case VARIABLE_DELEGATE:
                                case VARIABLE_ARRAY:
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                    break;
                            }
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_NOT:
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        case KEY_CND_NULL:
                            if ((LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_DELEGATE) && (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA) && (LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA == LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA)) {
                                DECLARE_PATH(VARIABLE_DELEGATE);
                                continue;
                            }
                            //---------------------------//
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                            //---------------------------//
                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA  = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->CLASS_DATA;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft.ID - 1]->NUMBER_DATA;
                            ((CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                            DECLARE_PATH(VARIABLE_NUMBER);
                            continue;

                        default:
                            DECLARE_PATH(0x20);
                            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                            CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                            Exc = new AnsiException(ERR960, OE->Operator._DEBUG_INFO_LINE, 960,  GetKeyWord(OE->Operator.ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME);
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
                            break;
                    }
                    break;
            }
        } else
        if (OE->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD) {
            switch (OE_Operator_ID) {
                case KEY_OPTIMIZED_IF:

                    DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE);
                    switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                        case VARIABLE_STRING:
                            if (!CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length()) {
                                INSTRUCTION_POINTER = OE->OperandReserved.ID;
                            }
                            continue;

                        case VARIABLE_NUMBER:
                            if (!LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA) {
                                INSTRUCTION_POINTER = OE->OperandReserved.ID;
                            }
                            continue;

                        case VARIABLE_ARRAY:
                            if (!((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->Count()) {
                                INSTRUCTION_POINTER = OE->OperandReserved.ID;
                            }
                            continue;
                    }
                    break;

                case KEY_OPTIMIZED_GOTO:
                    DECLARE_PATH(VARIABLE_NUMBER);
                    INSTRUCTION_POINTER = OE->OperandReserved.ID;
                    continue;

                case KEY_OPTIMIZED_ECHO:
                    DECLARE_PATH(1);
                    if (PIF->out) {
                        WRITE_LOCK
                        switch (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE) {
                            case VARIABLE_STRING:
                                PIF->out->Print(CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]), CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]).Length());
                                break;

                            case VARIABLE_NUMBER:
                                PIF->out->Print(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA);
                                break;

                            case VARIABLE_CLASS:
                                CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                PIF->out->Print(CCTEMP->_Class->NAME);
                                break;

                            case VARIABLE_ARRAY:
                                {
                                    AnsiString temp(((Array *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA)->ToString());
                                    PIF->out->Print2(temp);
                                }
                                break;

                            case VARIABLE_DELEGATE:
                                CCTEMP = (CompiledClass *)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                                PIF->out->Print(CCTEMP->_Class->NAME);
                                PIF->out->Print("::");
                                PIF->out->Print(CCTEMP->_Class->pMEMBERS [(INTEGER)LOCAL_CONTEXT [OE->OperandRight.ID - 1]->DELEGATE_DATA - 1]->NAME);
                                break;
                        }
                    }
                    continue;

                case KEY_OPTIMIZED_RETURN:
                    DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE);
                    if (LOCAL_CONTEXT [OE->OperandRight.ID - 1]->LINKS == 1) {
                        LOCAL_CONTEXT [OE->OperandRight.ID - 1]->LINKS = 2;
                        LOCAL_CONTEXT [OE->OperandRight.ID - 1]->IS_PROPERTY_RESULT = 0;

                        WRITE_UNLOCK
                        if (PROPERTIES)
                            FAST_FREE(PROPERTIES);
                        return LOCAL_CONTEXT [OE->OperandRight.ID - 1];
                    } else {
                        if (OWNER->IS_OPERATOR) {
                            LOCAL_CONTEXT [OE->OperandRight.ID - 1]->LINKS++;
                            LOCAL_CONTEXT [OE->OperandRight.ID - 1]->IS_PROPERTY_RESULT = 0;
                            WRITE_UNLOCK
                            if (PROPERTIES)
                                FAST_FREE(PROPERTIES);
                            return LOCAL_CONTEXT [OE->OperandRight.ID - 1];
                        }
                        RETURN_DATA              = (VariableDATA *)VAR_ALLOC(PIF);
                        RETURN_DATA->LINKS       = 1;
                        RETURN_DATA->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->NUMBER_DATA;
                        RETURN_DATA->TYPE        = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE;
                        RETURN_DATA->IS_PROPERTY_RESULT = 0;
                        if (RETURN_DATA->TYPE == VARIABLE_STRING) {
                            RETURN_DATA->CLASS_DATA     = 0;
                            CONCEPT_STRING(RETURN_DATA) = CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandRight.ID - 1]);
                        } else {
                            RETURN_DATA->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1]->CLASS_DATA;
                            if (RETURN_DATA->CLASS_DATA) {
                                if ((RETURN_DATA->TYPE == VARIABLE_CLASS) || (RETURN_DATA->TYPE == VARIABLE_DELEGATE)) {
                                    ((CompiledClass *)RETURN_DATA->CLASS_DATA)->LINKS++;
                                } else
                                if (RETURN_DATA->TYPE == VARIABLE_ARRAY) {
                                    ((Array *)RETURN_DATA->CLASS_DATA)->LINKS++;
                                }
                            }
                        }
                        WRITE_UNLOCK
                        if (PROPERTIES)
                            FAST_FREE(PROPERTIES);
                        return RETURN_DATA;
                    }
                    break;

                case KEY_OPTIMIZED_THROW:
                    DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight.ID - 1]->TYPE);
                    LOCAL_CONTEXT [OE->OperandRight.ID - 1]->LINKS++;
                    THROW_DATA = LOCAL_CONTEXT [OE->OperandRight.ID - 1];
                    THROW_DATA->IS_PROPERTY_RESULT = 0;
                    if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                        FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1]);
                        LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                        INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                        THROW_DATA                = 0;
                        CATCH_INSTRUCTION_POINTER = 0;
                        CATCH_VARIABLE            = 0;
                        // check for recursive try catch bug April 16, 2012 //
                        if (PREVIOUS_TRY)
                            PREVIOUS_TRY = OPT->CODE [PREVIOUS_TRY - 1].OperandLeft.ID;
                        // end of check for recursive try catch bug         //
                        //------------------//
                        RESTORE_TRY_DATA(this);
                        //------------------//
                    } else {
                        WRITE_UNLOCK
                        if (PROPERTIES)
                            FAST_FREE(PROPERTIES);
                        return 0;
                    }
                    continue;

                case KEY_OPTIMIZED_TRY_CATCH:
                    DECLARE_PATH(1);
                    CATCH_INSTRUCTION_POINTER = OE->OperandReserved.ID;
                    CATCH_VARIABLE            = OE->Result_ID;
                    PREVIOUS_TRY = OE->OperandLeft.ID;
                    continue;

                case KEY_OPTIMIZED_END_CATCH:
                    DECLARE_PATH(1);
                    CATCH_INSTRUCTION_POINTER = 0;
                    CATCH_VARIABLE            = 0;
                    THROW_DATA = 0;
                    //------------------//
                    RESTORE_TRY_DATA(this);
                    //------------------//
                    continue;

                case KEY_OPTIMIZED_DEBUG_TRAP:
                    DECLARE_PATH(1);
                    if (PIF->DEBUGGER_TRAP) {
                        WRITE_UNLOCK
                        if (PIF->DEBUGGER_TRAP(OPT->DATA, LOCAL_CONTEXT, OPT->dataCount, OE->Operator._DEBUG_INFO_LINE, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME.c_str(), (GET_VARIABLE_PROC)GetVariableByName, PIF->DEBUGGER_RESERVED, PIF, STACK_TRACE)) {
                            INSTRUCTION_POINTER = INSTRUCTION_COUNT;
                        }
                    }
                    continue;
            }
        }
    }
    WRITE_UNLOCK
    THROW_DATA = 0;
    RETURN_DATA = 0;
    if (PROPERTIES)
        FAST_FREE(PROPERTIES);
    return RETURN_DATA;
}

VariableDATA **ConceptInterpreter::CreateEnviroment(PIFAlizator *PIF, VariableDATA *Sender, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, SCStack *STACK_TRACE, bool& can_run) {
    VariableDATA **LOCAL_CONTEXT;
    Optimizer *OPT = (Optimizer *)this->OWNER->OPTIMIZER;

    can_run = true;
    INTEGER ParamCount = FORMAL_PARAM ? FORMAL_PARAM->COUNT : 0;
    INTEGER data_count = OPT->dataCount;

    SCStack *STACK_ROOT = (SCStack *)(STACK_TRACE ? STACK_TRACE->ROOT : NULL);
    if ((STACK_ROOT) && (STACK_ROOT->STACK_CONTEXT) && (STACK_ROOT->stack_pos + data_count < BLOCK_STACK_SIZE)) {
        LOCAL_CONTEXT                 = ((VariableDATA **)STACK_ROOT->STACK_CONTEXT) + STACK_ROOT->stack_pos;
        STACK_ROOT->stack_pos        += data_count;
        STACK_TRACE->alloc_from_stack = 1;
    } else
        LOCAL_CONTEXT = (VariableDATA **)FAST_MALLOC(sizeof(VariableDATA *) * OPT->dataCount);
#ifdef POOL_BLOCK_ALLOC
    BLOCK_VAR_ALLOC(LOCAL_CONTEXT, PIF, data_count, 0);
    VariableDATA *this_ref = LOCAL_CONTEXT[0];
#else
    VariableDATA *this_ref = (VariableDATA *)VAR_ALLOC(PIF);
    LOCAL_CONTEXT [0] = this_ref;
#endif
    this_ref->TYPE               = VARIABLE_CLASS;
    this_ref->CLASS_DATA         = Sender->CLASS_DATA;
    this_ref->LINKS              = 1;
    this_ref->IS_PROPERTY_RESULT = 0;

    CC_WRITE_LOCK(PIF)
    if ((this_ref->TYPE == VARIABLE_CLASS) && (this_ref->CLASS_DATA))
        ((CompiledClass *)this_ref->CLASS_DATA)->LINKS++;

    register INTEGER i;
    for (i = 1; i <= ParamCount; i++) {
        RuntimeVariableDESCRIPTOR *TARGET = &OPT->DATA [i];
        VariableDATA *sndr = SenderCTX [DELTA_UNREF(FORMAL_PARAM, FORMAL_PARAM->PARAM_INDEX) [i - 1] - 1];
        if (TARGET->TYPE < 0) {
            // validator !
            if (sndr->TYPE != -TARGET->TYPE) {
                VariableDATA *sender = SenderCTX ? SenderCTX [0] : 0;
                if ((sender) && (sender->TYPE == VARIABLE_CLASS) && (sender->CLASS_DATA)) {
                    CompiledClass *cc = (CompiledClass *)sender->CLASS_DATA;
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1205, OWNER->_DEBUG_STARTLINE, 1205, AnsiString("parameter ") + AnsiString((intptr_t)i), cc->_Class->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
                } else {
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1205, OWNER->_DEBUG_STARTLINE, 1205, AnsiString("parameter ") + AnsiString((intptr_t)i), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
                }
                can_run = false;
            } else
            if (sndr->TYPE == VARIABLE_CLASS) {
                INTEGER CLS_ID = (INTEGER)TARGET->nValue - 1;
                if (CLS_ID >= 0) {
                    if ((!sndr->CLASS_DATA) || (!((CompiledClass *)sndr->CLASS_DATA)->_Class->Inherits(CLS_ID))) {
                        VariableDATA *sender = SenderCTX [0];
                        if ((sender) && (sender->TYPE == VARIABLE_CLASS) && (sender->CLASS_DATA)) {
                            CompiledClass *cc = (CompiledClass *)sender->CLASS_DATA;
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1206, OWNER->_DEBUG_STARTLINE, 1206, AnsiString(OWNER->NAME) + AnsiString(", parameter ") + AnsiString((intptr_t)i), cc->_Class->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
                        } else {
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(ERR1206, OWNER->_DEBUG_STARTLINE, 1206, AnsiString(OWNER->NAME) + AnsiString(", parameter ") + AnsiString((intptr_t)i), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
                        }
                        can_run = false;
                    }
                }
            }
        }
        // if IS_PROPERTY_RESULT is -1 => is constant !
        if ((TARGET->BY_REF) && (sndr->IS_PROPERTY_RESULT != -1)) {
#ifdef POOL_BLOCK_ALLOC
            VAR_FREE(LOCAL_CONTEXT [i]);
#endif
            LOCAL_CONTEXT [i] = sndr;
            LOCAL_CONTEXT [i]->LINKS++;
        } else {
            VariableDATA *PARAM = sndr;
#ifdef POOL_BLOCK_ALLOC
            VariableDATA *LOCAL_CONTEXT_i = LOCAL_CONTEXT [i];
#else
            VariableDATA *LOCAL_CONTEXT_i = (VariableDATA *)VAR_ALLOC(PIF);
            LOCAL_CONTEXT [i] = LOCAL_CONTEXT_i;
#endif

            LOCAL_CONTEXT_i->TYPE  = PARAM->TYPE;
            LOCAL_CONTEXT_i->LINKS = 1;
            if (PARAM->TYPE == VARIABLE_STRING) {
                LOCAL_CONTEXT_i->CLASS_DATA     = 0;
                CONCEPT_STRING(LOCAL_CONTEXT_i) = CONCEPT_STRING(PARAM);
            } else
            if (PARAM->TYPE == VARIABLE_CLASS) {
                LOCAL_CONTEXT_i->CLASS_DATA = PARAM->CLASS_DATA;
                ((CompiledClass *)PARAM->CLASS_DATA)->LINKS++;
            } else
            if (PARAM->TYPE == VARIABLE_DELEGATE) {
                LOCAL_CONTEXT_i->CLASS_DATA    = PARAM->CLASS_DATA;
                LOCAL_CONTEXT_i->DELEGATE_DATA = PARAM->DELEGATE_DATA;
                ((CompiledClass *)PARAM->CLASS_DATA)->LINKS++;
            } else
            if (PARAM->TYPE == VARIABLE_ARRAY) {
                LOCAL_CONTEXT_i->CLASS_DATA = PARAM->CLASS_DATA;
                ((Array *)PARAM->CLASS_DATA)->LINKS++;
            } else
                LOCAL_CONTEXT_i->NUMBER_DATA = PARAM->NUMBER_DATA;
            LOCAL_CONTEXT_i->IS_PROPERTY_RESULT = 0;
        }
    }

    while (i < data_count) {
        // variable descriptor
        RuntimeVariableDESCRIPTOR *TARGET = &OPT->DATA [i];
#ifdef POOL_BLOCK_ALLOC
        VariableDATA *LOCAL_CONTEXT_i = LOCAL_CONTEXT[i];
#else
        VariableDATA *LOCAL_CONTEXT_i = (VariableDATA *)VAR_ALLOC(PIF);
        LOCAL_CONTEXT [i] = LOCAL_CONTEXT_i;
#endif
        i++;
        if (TARGET->TYPE < 0)
            LOCAL_CONTEXT_i->TYPE = -TARGET->TYPE;
        else
            LOCAL_CONTEXT_i->TYPE = TARGET->TYPE;

        if (TARGET->BY_REF == 2)
            LOCAL_CONTEXT_i->IS_PROPERTY_RESULT = -1;
        else
            LOCAL_CONTEXT_i->IS_PROPERTY_RESULT = 0;
        LOCAL_CONTEXT_i->LINKS = 1;
        if (LOCAL_CONTEXT_i->TYPE == VARIABLE_NUMBER)
            LOCAL_CONTEXT_i->NUMBER_DATA = TARGET->nValue;
        else
        if (LOCAL_CONTEXT_i->TYPE == VARIABLE_STRING) {
            if (TARGET->value.Length()) {
                LOCAL_CONTEXT_i->CLASS_DATA = new AnsiString();
                ((AnsiString *)(LOCAL_CONTEXT_i->CLASS_DATA))->LoadBuffer(TARGET->value.c_str(), TARGET->value.Length());
            } else
                LOCAL_CONTEXT_i->CLASS_DATA = 0;
        } else
        if (LOCAL_CONTEXT_i->TYPE == VARIABLE_ARRAY)
            LOCAL_CONTEXT_i->CLASS_DATA = new(AllocArray(PIF))Array(PIF);
        else
            LOCAL_CONTEXT_i->CLASS_DATA = NULL;
    }
    CC_WRITE_UNLOCK(PIF)
    return LOCAL_CONTEXT;
}

void ConceptInterpreter::DestroyEnviroment(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, SCStack *STACK_TRACE, char static_call_main) {
    if (PIF->DEBUGGER_RESERVED) {
        if (*(intptr_t *)PIF->DEBUGGER_RESERVED == (intptr_t)LOCAL_CONTEXT) {
            *(intptr_t *)PIF->DEBUGGER_RESERVED = -1;
        }
    }
    Optimizer *OPT = (Optimizer *)this->OWNER->OPTIMIZER;
    register INTEGER i;
    register INTEGER data_count = OPT->dataCount;
#ifndef FAST_EXIT_NO_GC_CALL
    if (static_call_main) {
#ifdef SIMPLE_MULTI_THREADING
        // wait for the threads to end
        while (PIF->ThreadsCount > 1) {
#ifdef _WIN32
            Sleep(100);
#else
            usleep(100000);
#endif
        }
#endif
        GarbageCollector __gc_obj;
        GarbageCollector __gc_array;
        GarbageCollector __gc_vars;
        VariableDATA *LOCAL_CONTEXT_i;
        for (i = 0; i < data_count; i++) {
            LOCAL_CONTEXT_i = LOCAL_CONTEXT [i];
            if (LOCAL_CONTEXT_i) {
                if (LOCAL_CONTEXT_i->CLASS_DATA) {
                    if ((LOCAL_CONTEXT_i->TYPE == VARIABLE_CLASS) || (LOCAL_CONTEXT_i->TYPE == VARIABLE_DELEGATE)) {
                        __gc_obj.Reference(LOCAL_CONTEXT_i->CLASS_DATA);
                        ((CompiledClass *)LOCAL_CONTEXT_i->CLASS_DATA)->__GO_GARBAGE(PIF, &__gc_obj, &__gc_array, &__gc_vars);
                    } else
                    if (LOCAL_CONTEXT_i->TYPE == VARIABLE_ARRAY) {
                        __gc_array.Reference(LOCAL_CONTEXT_i->CLASS_DATA);
                        ((Array *)LOCAL_CONTEXT_i->CLASS_DATA)->__GO_GARBAGE(PIF, &__gc_obj, &__gc_array, &__gc_vars);
                    }
                }
                __gc_vars.Reference(LOCAL_CONTEXT_i);
            }
        }
        if (PIF->var_globals) {
            __gc_array.Reference(PIF->var_globals);
            PIF->var_globals->__GO_GARBAGE(PIF, &__gc_obj, &__gc_array, &__gc_vars);
        }
        VARPool *POOL = PIF->POOL;
        while (POOL) {
            for (int i = 0; i < POOL_BLOCK_SIZE; i++) {
                LOCAL_CONTEXT_i = &POOL->POOL[i];
                if ((LOCAL_CONTEXT_i) && (LOCAL_CONTEXT_i->flags >= 0) && (LOCAL_CONTEXT_i->CLASS_DATA)) {
                    if ((LOCAL_CONTEXT_i->TYPE == VARIABLE_CLASS) || (LOCAL_CONTEXT_i->TYPE == VARIABLE_DELEGATE)) {
                        __gc_obj.Reference(LOCAL_CONTEXT_i->CLASS_DATA);
                        ((CompiledClass *)LOCAL_CONTEXT_i->CLASS_DATA)->__GO_GARBAGE(PIF, &__gc_obj, &__gc_array, &__gc_vars);
                    } else
                    if (LOCAL_CONTEXT_i->TYPE == VARIABLE_ARRAY) {
                        __gc_array.Reference(LOCAL_CONTEXT_i->CLASS_DATA);
                        ((Array *)LOCAL_CONTEXT_i->CLASS_DATA)->__GO_GARBAGE(PIF, &__gc_obj, &__gc_array, &__gc_vars);
                    }
                }
            }
            POOL = (VARPool *)POOL->NEXT;
        }
        //======================================================//
        __gc_obj.EndOfExecution_SayBye_Objects();
        __gc_array.EndOfExecution_SayBye_Arrays();
        __gc_vars.EndOfExecution_SayBye_Variables();
    } else
#endif
    {
        CC_WRITE_LOCK(PIF)

        for (i = 0; i < data_count; i++) {
#ifdef SIMPLE_MULTI_THREADING
            VariableDATA *VARIABLE = LOCAL_CONTEXT [i];
            VARIABLE->LINKS--;
            if (VARIABLE->LINKS < 1) {
                if (VARIABLE->TYPE == VARIABLE_STRING) {
                    delete (AnsiString *)VARIABLE->CLASS_DATA;
                } else
                if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) {
                    if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {
 #ifdef SIMPLE_MULTI_THREADING
                        VARIABLE->TYPE = VARIABLE_NUMBER;
                        CC_WRITE_UNLOCK(PIF)
 #endif
                        delete (CompiledClass *)VARIABLE->CLASS_DATA;
 #ifdef SIMPLE_MULTI_THREADING
                        CC_WRITE_LOCK2(PIF)
 #endif
                    }
                } else
                if (VARIABLE->TYPE == VARIABLE_ARRAY) {
                    if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS) {
 #ifdef SIMPLE_MULTI_THREADING
                        VARIABLE->TYPE = VARIABLE_NUMBER;
                        CC_WRITE_UNLOCK(PIF)
 #endif
                        delete (Array *)VARIABLE->CLASS_DATA;
 #ifdef SIMPLE_MULTI_THREADING
                        CC_WRITE_LOCK2(PIF)
 #endif
                    }
                }
                VAR_FREE(VARIABLE);
            }
#else
            VariableDATA *LOCAL_CONTEXT_i = LOCAL_CONTEXT [i];
            if (LOCAL_CONTEXT_i) {
 #ifdef POOL_BLOCK_ALLOC
                FAST_FREE_VARIABLE2(LOCAL_CONTEXT_i, LOCAL_CONTEXT [i]);
 #else
                FREE_VARIABLE(LOCAL_CONTEXT_i);
 #endif
            }
#endif
        }
        CC_WRITE_UNLOCK(PIF)
#ifdef POOL_BLOCK_ALLOC
        BLOCK_VAR_FREE(LOCAL_CONTEXT, data_count);
#endif
    }

    if ((STACK_TRACE) && (STACK_TRACE->alloc_from_stack)) {
        SCStack *STACK_ROOT = (SCStack *)(STACK_TRACE ? STACK_TRACE->ROOT : NULL);
        if ((STACK_ROOT) && (STACK_ROOT->STACK_CONTEXT)) {
            STACK_ROOT->stack_pos        -= data_count;
            STACK_TRACE->alloc_from_stack = 0;
        }
    } else {
        FAST_FREE(LOCAL_CONTEXT);
    }
}

ConceptInterpreter::~ConceptInterpreter(void) {
#ifdef USE_JIT_TRACE
    Optimizer *OPT = (Optimizer *)this->OWNER->OPTIMIZER;
    if (jittracecode) {
        int count = OPT->codeCount;
        for (int i = 0; i < count; i++) {
            if (jittracecode[i])
                sljit_free_code(jittracecode[i]);
        }
        free(jittracecode);
        jittracecode = 0;
    }
#endif
}

