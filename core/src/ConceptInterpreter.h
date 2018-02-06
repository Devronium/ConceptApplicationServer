#ifndef __CONCEPTINTERPRETER_H
#define __CONCEPTINTERPRETER_H

#define USE_JIT_TRACE

#include "Optimizer.h"
#include "Debugger.h"
#include "Array.h"
#include "ConceptPools.h"

#define CONCEPT_STRING(VARIABLE)            (VARIABLE->CLASS_DATA ? (*((AnsiString *)VARIABLE->CLASS_DATA)) : *((AnsiString *)(VARIABLE->CLASS_DATA = new AnsiString())))
#define NEW_CONCEPT_STRING(VARIABLE)        (*((AnsiString *)(VARIABLE->CLASS_DATA = new AnsiString())))
#define CONCEPT_C_STRING(VARIABLE)          (VARIABLE->CLASS_DATA ? ((AnsiString *)VARIABLE->CLASS_DATA)->c_str() : "")
#define CONCEPT_C_LENGTH(VARIABLE)          (VARIABLE->CLASS_DATA ? ((AnsiString *)VARIABLE->CLASS_DATA)->Length() : 0)
//---------------------------------------------------------
#ifdef INLINE_COMMON_CALLS
#define FREE_VARIABLE(VARIABLE)                                                                \
    VARIABLE->LINKS--;                                                                         \
    if (VARIABLE->LINKS < 1) {                                                                 \
        if (VARIABLE->CLASS_DATA) {                                                            \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                delete (AnsiString *)VARIABLE->CLASS_DATA;                                     \
            } else                                                                             \
            if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) { \
                if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)                         \
                    delete (CompiledClass *)VARIABLE->CLASS_DATA;                              \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS)                                 \
                    delete (Array *)VARIABLE->CLASS_DATA;                                      \
            }                                                                                  \
        }                                                                                      \
        VAR_FREE(VARIABLE);                                                                    \
    }
#else
void FREE_VARIABLE(VariableDATA *VARIABLE);
#endif

#ifdef SIMPLE_MULTI_THREADING
#define FREE_VARIABLE_TS(VARIABLE)                                                              \
    VARIABLE->LINKS--;                                                                         \
    if (VARIABLE->LINKS < 1) {                                                                 \
        if (VARIABLE->CLASS_DATA) {                                                            \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                delete (AnsiString *)VARIABLE->CLASS_DATA;                                     \
            } else                                                                             \
            if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) { \
                if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {                       \
                    WRITE_UNLOCK                                                               \
                    delete (CompiledClass *)VARIABLE->CLASS_DATA;                              \
                }                                                                              \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS) {                               \
                    WRITE_UNLOCK                                                               \
                    delete (Array *)VARIABLE->CLASS_DATA;                                      \
                }                                                                              \
            }                                                                                  \
        }                                                                                      \
        VAR_FREE(VARIABLE);                                                                    \
    }
#else
#define FREE_VARIABLE_TS    FREE_VARIABLE
#endif

#define FREE_VARIABLE_RESET(VARIABLE, pushed_type)                                       \
    VARIABLE->LINKS--;                                                                   \
    if (VARIABLE->LINKS < 1) {                                                           \
        if (VARIABLE->CLASS_DATA) {                                                      \
            if (pushed_type == VARIABLE_STRING) {                                        \
                delete (AnsiString *)VARIABLE->CLASS_DATA;                               \
            } else                                                                       \
            if ((pushed_type == VARIABLE_CLASS) || (pushed_type == VARIABLE_DELEGATE)) { \
                if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)                   \
                    delete (CompiledClass *)VARIABLE->CLASS_DATA;                        \
            } else                                                                       \
            if (pushed_type == VARIABLE_ARRAY) {                                         \
                if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS)                           \
                    delete (Array *)VARIABLE->CLASS_DATA;                                \
            }                                                                            \
        }                                                                                \
        VAR_FREE(VARIABLE);                                                              \
    }

#define FAST_FREE_VARIABLE(VARIABLE)                                                           \
    VARIABLE->LINKS--;                                                                         \
    if (VARIABLE->LINKS < 1) {                                                                 \
        if (VARIABLE->CLASS_DATA) {                                                            \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                delete (AnsiString *)VARIABLE->CLASS_DATA;                                     \
            } else                                                                             \
            if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) { \
                if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)                         \
                    delete (CompiledClass *)VARIABLE->CLASS_DATA;                              \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS)                                 \
                    delete (Array *)VARIABLE->CLASS_DATA;                                      \
            }                                                                                  \
        }                                                                                      \
    }

#define FAST_FREE_VARIABLE2(VARIABLE, RESET_VAR)                                               \
    VARIABLE->LINKS--;                                                                         \
    if (VARIABLE->LINKS < 1) {                                                                 \
        if (VARIABLE->CLASS_DATA) {                                                            \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                delete (AnsiString *)VARIABLE->CLASS_DATA;                                     \
            } else                                                                             \
            if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) { \
                if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {                       \
                    delete (CompiledClass *)VARIABLE->CLASS_DATA; }                            \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS) {                               \
                    delete (Array *)VARIABLE->CLASS_DATA; }                                    \
            }                                                                                  \
        }                                                                                      \
    } else                                                                                     \
        RESET_VAR = NULL;

#define FREE_VARIABLE_NO_OBJECTS(VARIABLE)                                 \
    VARIABLE->LINKS--;                                                     \
    if (VARIABLE->LINKS < 1) {                                             \
        if ((VARIABLE->TYPE == VARIABLE_STRING) && (VARIABLE->CLASS_DATA)) \
            delete (AnsiString *)VARIABLE->CLASS_DATA;                     \
        VAR_FREE(VARIABLE);                                                \
    }

#define CLASS_CHECK(VARIABLE)                                                              \
    if ((VARIABLE->TYPE != VARIABLE_NUMBER) && (VARIABLE->CLASS_DATA)) {                   \
        if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
            delete (AnsiString *)VARIABLE->CLASS_DATA; }                                   \
        else                                                                               \
        if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) { \
            if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)                         \
                delete (CompiledClass *)VARIABLE->CLASS_DATA;                              \
        } else                                                                             \
        if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
            if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS)                                 \
                delete (Array *)VARIABLE->CLASS_DATA;                                      \
        }                                                                                  \
        VARIABLE->CLASS_DATA = NULL;                                                       \
    }                                                                                      \

#define CLASS_CHECK_RESET(VARIABLE, pushed_type)                                     \
    if ((pushed_type != VARIABLE_NUMBER) && (VARIABLE->CLASS_DATA)) {                \
        if (pushed_type == VARIABLE_STRING) {                                        \
            delete (AnsiString *)VARIABLE->CLASS_DATA; }                             \
        else                                                                         \
        if ((pushed_type == VARIABLE_CLASS) || (pushed_type == VARIABLE_DELEGATE)) { \
            if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)                   \
                delete (CompiledClass *)VARIABLE->CLASS_DATA;                        \
        } else                                                                       \
        if (pushed_type == VARIABLE_ARRAY) {                                         \
            if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS)                           \
                delete (Array *)VARIABLE->CLASS_DATA;                                \
        }                                                                            \
    }                                                                                \
    VARIABLE->CLASS_DATA = NULL;                                                     \

#define RESET_VARIABLE(VARIABLE)             \
    if (VARIABLE->TYPE != VARIABLE_NUMBER) { \
        CLASS_CHECK(VARIABLE)                \
        VARIABLE->TYPE = VARIABLE_NUMBER;    \
    }
//---------------------------------------------------------
#define STACK(STACK_TRACE, _caller_line)    if (STACK_TRACE) ((SCStack *)STACK_TRACE)->line = _caller_line;
#define UNSTACK

class ConceptInterpreter {
    friend class ClassMember;
    friend class ClassCode;
    friend void *GetStaticData();

    friend void SetStaticData(void *);

private:
    INTEGER     LocalClassID;
    ClassMember *OWNER;

#ifdef USE_JIT_TRACE
    char callcount;
    void **jittracecode;
    void AnalizeInstructionPath(Optimizer *OPT);
#endif
#ifdef SIMPLE_MULTI_THREADING
    int EvalNumberExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalStringExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalClassExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, OPERATOR_ID_TYPE OE_Operator_ID, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalArrayExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalDelegateExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalSimpleExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
#else
    int EvalNumberExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalStringExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalClassExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, OPERATOR_ID_TYPE OE_Operator_ID, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalArrayExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalDelegateExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalSimpleExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
#endif
public:
    POOLED(ConceptInterpreter)
    ConceptInterpreter(Optimizer *O, INTEGER LocalClsID, ClassMember *Owner);
    VariableDATA **CreateEnvironment(PIFAlizator *PIF, VariableDATA *Sender, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, SCStack *STACK_TRACE, bool& can_run);
    void DestroyEnviroment(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, SCStack *STACK_TRACE, char static_call_main = 0);
    VariableDATA *Interpret(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE);
    static int StacklessInterpret(PIFAlizator *PIF, GreenThreadCycle *GREEN);

    ~ConceptInterpreter(void);
};
#endif // __CONCEPTINTERPRETER_H

