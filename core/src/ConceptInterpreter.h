#ifndef __CONCEPTINTERPRETER_H
#define __CONCEPTINTERPRETER_H

#define USE_JIT_TRACE

#include "Optimizer.h"
#include "Debugger.h"
#include "Array.h"
#include "ConceptPools.h"

#define CONCEPT_STRING(VARIABLE)    (VARIABLE->CLASS_DATA ? (*((AnsiString *)VARIABLE->CLASS_DATA)) : *((AnsiString *)(VARIABLE->CLASS_DATA = new AnsiString())))
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
    int EvalNumberExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked, char pushed_type);
#else
    int EvalNumberExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char pushed_type);    
#endif
public:
    POOLED(ConceptInterpreter)
    ConceptInterpreter(Optimizer *O, INTEGER LocalClsID, ClassMember *Owner);
    VariableDATA **CreateEnviroment(PIFAlizator *PIF, VariableDATA *Sender, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, SCStack *STACK_TRACE, bool& can_run);
    void DestroyEnviroment(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, SCStack *STACK_TRACE, char static_call_main = 0);
    VariableDATA *Interpret(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE);
    static int StacklessInterpret(PIFAlizator *PIF, GreenThreadCycle *GREEN);

    ~ConceptInterpreter(void);
};
#endif // __CONCEPTINTERPRETER_H

