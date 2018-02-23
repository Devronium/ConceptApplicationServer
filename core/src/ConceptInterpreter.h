#ifndef __CONCEPTINTERPRETER_H
#define __CONCEPTINTERPRETER_H

#define USE_JIT_TRACE
#define JIT_INLINE

#include "Optimizer.h"
#include "Debugger.h"
#include "Array.h"
#include "ConceptPools.h"
extern "C" {
    #include "simple/plainstring.h"
}

#define CONCEPT_STRING(VARIABLE, VARIABLE2)     { if (VARIABLE->CLASS_DATA) { plainstring_set_plainstring((struct plainstring *)VARIABLE->CLASS_DATA, (struct plainstring *)VARIABLE2->CLASS_DATA); } else { VARIABLE->CLASS_DATA = plainstring_new_plainstring((struct plainstring *)VARIABLE2->CLASS_DATA); } }
#define CONCEPT_STRING_EQU(VARIABLE, VARIABLE2) plainstring_equals_plainstring((struct plainstring *)VARIABLE->CLASS_DATA, (struct plainstring *)VARIABLE2->CLASS_DATA)
#define CONCEPT_STRING_EQU_FLOAT(VARIABLE, d)   plainstring_equals_double((struct plainstring *)VARIABLE->CLASS_DATA, d)
#define CONCEPT_STRING_ADD_CSTR(VARIABLE, cstr) { if (VARIABLE->CLASS_DATA) { plainstring_add((struct plainstring *)VARIABLE->CLASS_DATA, cstr); } else { VARIABLE->CLASS_DATA = plainstring_new_str(cstr); } }
#define CONCEPT_STRING_ADD_DOUBLE(VARIABLE, d)  { if (VARIABLE->CLASS_DATA) { plainstring_add_double((struct plainstring *)VARIABLE->CLASS_DATA, d); } else { VARIABLE->CLASS_DATA = plainstring_new_double(d); } }
#define CONCEPT_STRING_BUFFER(VARIABLE, b, l)   { if (!VARIABLE->CLASS_DATA) { VARIABLE->CLASS_DATA = plainstring_new(); } plainstring_loadbuffer((struct plainstring *)VARIABLE->CLASS_DATA, b, l); }
#define CONCEPT_STRING_LINK(VARIABLE, b, l)     { if (!VARIABLE->CLASS_DATA) { VARIABLE->CLASS_DATA = plainstring_new(); } plainstring_linkbuffer((struct plainstring *)VARIABLE->CLASS_DATA, b, l); }
#define CONCEPT_STRING_FLOAT(VARIABLE)          plainstring_float((struct plainstring *)VARIABLE->CLASS_DATA)
#define CONCEPT_STRING_INT(VARIABLE)            (VARIABLE->CLASS_DATA ? plainstring_int((struct plainstring *)VARIABLE->CLASS_DATA) : 0)
#define CONCEPT_STRING_SET_CSTR(VARIABLE, cstr) { if (VARIABLE->CLASS_DATA) { plainstring_set((struct plainstring *)VARIABLE->CLASS_DATA, cstr); } else { VARIABLE->CLASS_DATA = plainstring_new_str(cstr); } }
#define CONCEPT_STRING_REPLACE(VARIABLE, VARIABLE2, index) plainstring_replace_char_with_string((struct plainstring *)VARIABLE->CLASS_DATA, (struct plainstring *)VARIABLE2->CLASS_DATA, index)
#define NEW_CONCEPT_STRING(VARIABLE)            (VARIABLE->CLASS_DATA = plainstring_new())
#define NEW_CONCEPT_SUM(VARIABLE, A, B)         {VARIABLE->CLASS_DATA = plainstring_new(); plainstring_sum_of_2((struct plainstring *)VARIABLE->CLASS_DATA, (struct plainstring *)A->CLASS_DATA, (struct plainstring *)B->CLASS_DATA); }
#define NEW_CONCEPT_STRING2(VARIABLE, VARIABLE2)(VARIABLE->CLASS_DATA = plainstring_new_plainstring((struct plainstring *)VARIABLE2->CLASS_DATA))
#define NEW_CONCEPT_STRING_CSTR(VARIABLE, cstr) (VARIABLE->CLASS_DATA = plainstring_new_str(cstr))
#define NEW_CONCEPT_STRING_BUFFER(VARIABLE,b,l) { VARIABLE->CLASS_DATA = plainstring_new();  plainstring_loadbuffer((struct plainstring *)VARIABLE->CLASS_DATA, b, l);}

#define NEW_CONCEPT_STRING_INDEX(VARIABLE, VARIABLE2, index) { VARIABLE->CLASS_DATA = plainstring_new(); plainstring_char_plainstring((struct plainstring *)VARIABLE2->CLASS_DATA, index, (struct plainstring *)VARIABLE->CLASS_DATA); }

#define CONCEPT_C_STRING(VARIABLE)              (VARIABLE->CLASS_DATA ? plainstring_c_str((struct plainstring *)VARIABLE->CLASS_DATA) : "")
#define CONCEPT_C_LENGTH(VARIABLE)              (VARIABLE->CLASS_DATA ? plainstring_len((struct plainstring *)VARIABLE->CLASS_DATA) : 0)

#define CONCEPT_STRING_COMPARE(val, VARIABLE, type, VARIABLE2) { if (!VARIABLE->CLASS_DATA) { VARIABLE->CLASS_DATA = plainstring_new(); } if (!VARIABLE2->CLASS_DATA) { VARIABLE2->CLASS_DATA = plainstring_new(); } val = plainstring_ ## type ## _plainstring((struct plainstring *)VARIABLE->CLASS_DATA, (struct plainstring *)VARIABLE2->CLASS_DATA); }
#define CONCEPT_STRING_COMPARE_FLOAT(val, VARIABLE, type, d) { if (!VARIABLE->CLASS_DATA) { VARIABLE->CLASS_DATA = plainstring_new(); } val = plainstring_ ## type ## _double((struct plainstring *)VARIABLE->CLASS_DATA, d); }
#define CONCEPT_STRING_ASU(VARIABLE, VARIABLE2) { if (!VARIABLE->CLASS_DATA) { VARIABLE->CLASS_DATA = plainstring_new_plainstring((struct plainstring *)VARIABLE2->CLASS_DATA); } else { plainstring_increasebuffer((struct plainstring *)VARIABLE->CLASS_DATA, CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1])); plainstring_add_plainstring((struct plainstring *)VARIABLE->CLASS_DATA, (struct plainstring *)VARIABLE2->CLASS_DATA); } }

//---------------------------------------------------------
#ifdef INLINE_COMMON_CALLS
#define FREE_VARIABLE(VARIABLE, SCStack *STACK_TRACE)                                          \
    VARIABLE->LINKS--;                                                                         \
    if (VARIABLE->LINKS < 1) {                                                                 \
        if (VARIABLE->CLASS_DATA) {                                                            \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA);                \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_CLASS) {                                            \
                if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)                  \
                    delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA, STACK_TRACE); \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS)                          \
                    delete_Array((struct Array *)VARIABLE->CLASS_DATA);                        \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_DELEGATE)                                           \
                delete_Delegate(VARIABLE->CLASS_DATA, STACK_TRACE);                            \
        }                                                                                      \
        VAR_FREE(VARIABLE);                                                                    \
    }
#else
void FREE_VARIABLE(VariableDATA *VARIABLE, SCStack *STACK_TRACE);
#endif

#ifdef SIMPLE_MULTI_THREADING
#define FREE_VARIABLE_TS(VARIABLE, STACK_TRACE)                                                \
    VARIABLE->LINKS--;                                                                         \
    if (VARIABLE->LINKS < 1) {                                                                 \
        if (VARIABLE->CLASS_DATA) {                                                            \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA);                \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_CLASS) {                                            \
                if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {                \
                    WRITE_UNLOCK                                                               \
                    delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA, STACK_TRACE); \
                }                                                                              \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS) {                        \
                    WRITE_UNLOCK                                                               \
                    delete_Array((struct Array *)VARIABLE->CLASS_DATA);                        \
                }                                                                              \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_DELEGATE) {                                         \
                WRITE_UNLOCK                                                                   \
                delete_Delegate(VARIABLE->CLASS_DATA, STACK_TRACE);                            \
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
                plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA);          \
            } else                                                                       \
            if (pushed_type == VARIABLE_CLASS) {                                         \
                if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)            \
                    delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA);  \
            } else                                                                       \
            if (pushed_type == VARIABLE_ARRAY) {                                         \
                if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS)                    \
                    delete_Array((struct Array *)VARIABLE->CLASS_DATA);                  \
            } else                                                                       \
            if (pushed_type == VARIABLE_DELEGATE)                                        \
                delete_Delegate(VARIABLE->CLASS_DATA);                                   \
        }                                                                                \
        VAR_FREE(VARIABLE);                                                              \
    }

#define FAST_FREE_VARIABLE(VARIABLE)                                                           \
    VARIABLE->LINKS--;                                                                         \
    if (VARIABLE->LINKS < 1) {                                                                 \
        if (VARIABLE->CLASS_DATA) {                                                            \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA);                \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_CLASS) {                                            \
                if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)                  \
                    delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA);        \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS)                          \
                    delete_Array((struct Array *)VARIABLE->CLASS_DATA);                        \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_DELEGATE)                                           \
                delegate_Delegate(VARIABLE->CLASS_DATA);                                       \
        }                                                                                      \
    }

#define FAST_FREE_VARIABLE2(VARIABLE, RESET_VAR)                                               \
    VARIABLE->LINKS--;                                                                         \
    if (VARIABLE->LINKS < 1) {                                                                 \
        if (VARIABLE->CLASS_DATA) {                                                            \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA);                \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_CLASS) {                                            \
                if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {                \
                    delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA); }      \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS) {                        \
                    delete_Array((struct Array *)VARIABLE->CLASS_DATA); }                      \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_DELEGATE)                                           \
                delete_Delegate(VARIABLE->CLASS_DATA);                                         \
        }                                                                                      \
    } else                                                                                     \
        RESET_VAR = NULL;

#define FREE_VARIABLE_NO_OBJECTS(VARIABLE)                                 \
    VARIABLE->LINKS--;                                                     \
    if (VARIABLE->LINKS < 1) {                                             \
        if ((VARIABLE->TYPE == VARIABLE_STRING) && (VARIABLE->CLASS_DATA)) \
            plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA);\
        VAR_FREE(VARIABLE);                                                \
    }

#define CLASS_CHECK(VARIABLE, STACK_TRACE)                                                 \
    if ((VARIABLE->TYPE != VARIABLE_NUMBER) && (VARIABLE->CLASS_DATA)) {                   \
        if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
            plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA); }              \
        else                                                                               \
        if (VARIABLE->TYPE == VARIABLE_CLASS) {                                            \
            if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)                  \
                delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA, STACK_TRACE); \
        } else                                                                             \
        if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
            if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS)                          \
                delete_Array((struct Array *)VARIABLE->CLASS_DATA);                        \
        } else                                                                             \
        if (VARIABLE->TYPE == VARIABLE_DELEGATE)                                           \
            delete_Delegate(VARIABLE->CLASS_DATA, STACK_TRACE);                            \
        VARIABLE->CLASS_DATA = NULL;                                                       \
    }                                                                                      \

#define CLASS_CHECK_RESET(VARIABLE, pushed_type)                                     \
    if ((pushed_type != VARIABLE_NUMBER) && (VARIABLE->CLASS_DATA)) {                \
        if (pushed_type == VARIABLE_STRING) {                                        \
            plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA); }        \
        else                                                                         \
        if (pushed_type == VARIABLE_CLASS) { \
            if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)            \
                delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA);  \
        } else                                                                       \
        if (pushed_type == VARIABLE_ARRAY) {                                         \
            if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS)                    \
                delete_Array((struct Array *)VARIABLE->CLASS_DATA);                  \
        } else                                                                       \
        if (pushed_type == VARIABLE_DELEGATE)                                        \
            delete_Delegate(VARIABLE->CLASS_DATA);                                   \
    }                                                                                \
    VARIABLE->CLASS_DATA = NULL;                                                     \

#define RESET_VARIABLE(VARIABLE, STACK_TRACE)\
    if (VARIABLE->TYPE != VARIABLE_NUMBER) { \
        CLASS_CHECK(VARIABLE, STACK_TRACE);  \
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
    int JIT(INTEGER &INSTRUCTION_POINTER, INTEGER INSTRUCTION_COUNT, void **jittrace, VariableDATA **LOCAL_CONTEXT);
#endif
#ifdef SIMPLE_MULTI_THREADING
    int EvalNumberExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalStringExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalClassExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, OPERATOR_ID_TYPE OE_Operator_ID, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalArrayExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalDelegateExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalSimpleExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
    int EvalCall(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked);
#else
    int EvalNumberExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalStringExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalClassExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, OPERATOR_ID_TYPE OE_Operator_ID, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalArrayExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalDelegateExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalSimpleExpression(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
    int EvalCall(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
#endif
    int Catch(VariableDATA *&THROW_DATA, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY);
public:
    POOLED(ConceptInterpreter)
    ConceptInterpreter(Optimizer *O, INTEGER LocalClsID, ClassMember *Owner);
    VariableDATA **CreateEnvironment(PIFAlizator *PIF, VariableDATA *Sender, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, SCStack *STACK_TRACE, bool& can_run);
    void DestroyEnviroment(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, SCStack *STACK_TRACE, char static_call_main = 0);
    VariableDATA *Interpret(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE);
    static int StacklessInterpret(PIFAlizator *PIF, GreenThreadCycle *GREEN);

    void DestroyGC(PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, SCStack *STACK_TRACE);
    ~ConceptInterpreter(void);
};
#endif // __CONCEPTINTERPRETER_H

