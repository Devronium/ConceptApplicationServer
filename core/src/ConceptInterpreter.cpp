//#define FAST_EXIT_NO_GC_CALL
#include "ConceptInterpreter.h"
#include "AnsiException.h"
#include "ModuleLink.h"
#include "Debugger.h"
#include <time.h>
#include <math.h>
#ifndef _WIN32
    #include <unistd.h>
#endif

POOLED_IMPLEMENTATION(tsSCStack)
POOLED_IMPLEMENTATION(ConceptInterpreter)

static TinyString DLL_CLASS = "STATIC/LIBRARY";
static TinyString DLL_MEMBER = "STATIC_FUNCTION";

#define IS_SIMPLE(id)   ((id == KEY_TYPE_OF) || (id == KEY_CLASS_NAME) || (id == KEY_BY_REF) || (id == KEY_DELETE))

#define PROPERTY_CODE(THISREF, PROPERTIES)                                                                                                                                                                                                                                                                      \
    if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT)) {                                                                                                                                                                                                                             \
        CCTEMP = (struct CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET))->CLASS_DATA;                                                                                                                                                                                          \
        WRITE_UNLOCK                                                                                                                                                                                                                                                                                            \
        CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET), OE, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID , LOCAL_CONTEXT, ClassID, THISREF->LocalClassID, &THROW_DATA, STACK_TRACE);            \
        if (THROW_DATA) {                                                                                                                                                                                                                                                                                       \
            if (ConceptInterpreter_Catch(THISREF, THROW_DATA, LOCAL_CONTEXT, OE, PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {                                                                                                                                   \
                FAST_FREE(PIF, PROPERTIES);                                                                                                                                                                                                                                                                     \
                PROPERTIES = 0;                                                                                                                                                                                                                                                                                 \
                WRITE_UNLOCK                                                                                                                                                                                                                                                                                    \
                return 0;                                                                                                                                                                                                                                                                                       \
            }                                                                                                                                                                                                                                                                                                   \
        }                                                                                                                                                                                                                                                                                                       \
    }

#define PROPERTY_CODE_IGNORE_RESULT(THISREF, PROPERTIES)                                                                                                                                                                                                                                                        \
    if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT)) {                                                                                                                                                                                                                             \
        CCTEMP = (struct CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET))->CLASS_DATA;                                                                                                                                                                                          \
        WRITE_UNLOCK                                                                                                                                                                                                                                                                                            \
        CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET), OE, CCTEMP->_Class->CLSID == ClassID, OE->OperandLeft_ID , LOCAL_CONTEXT, ClassID, THISREF->LocalClassID, &THROW_DATA, STACK_TRACE);       \
        if (THROW_DATA) {                                                                                                                                                                                                                                                                                       \
            if (ConceptInterpreter_Catch(THISREF, THROW_DATA, LOCAL_CONTEXT, OE, PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {                                                                                                                                   \
                FAST_FREE(PIF, PROPERTIES);                                                                                                                                                                                                                                                                     \
                PROPERTIES = 0;                                                                                                                                                                                                                                                                                 \
                WRITE_UNLOCK                                                                                                                                                                                                                                                                                    \
                return 0;                                                                                                                                                                                                                                                                                       \
            }                                                                                                                                                                                                                                                                                                   \
        }                                                                                                                                                                                                                                                                                                       \
    }

#define PROPERTY_CODE_LEFT(THISREF, PROPERTIES)                                                                                                                                                                                                                                                                 \
    if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT)) {                                                                                                                                                                                                                             \
        CCTEMP = (struct CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET))->CLASS_DATA;                                                                                                                                                                                          \
        WRITE_UNLOCK                                                                                                                                                                                                                                                                                            \
        CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET), OE, CCTEMP->_Class->CLSID == ClassID, OE->OperandLeft_ID , LOCAL_CONTEXT, ClassID, THISREF->LocalClassID, &THROW_DATA, STACK_TRACE);       \
        if (THROW_DATA) {                                                                                                                                                                                                                                                                                       \
            if (ConceptInterpreter_Catch(THISREF, THROW_DATA, LOCAL_CONTEXT, OE, PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {                                                                                                                                   \
                FAST_FREE(PIF, PROPERTIES);                                                                                                                                                                                                                                                                     \
                PROPERTIES = 0;                                                                                                                                                                                                                                                                                 \
                WRITE_UNLOCK                                                                                                                                                                                                                                                                                    \
                return 0;                                                                                                                                                                                                                                                                                       \
            }                                                                                                                                                                                                                                                                                                   \
        }                                                                                                                                                                                                                                                                                                       \
    }
//---------------------------------------------------------
#define TEMP_PROPERTY_CODE(THISREF, PROPERTIES)                                                                                                                                                                                                                                                                 \
    if ((PROPERTIES) && (PROPERTIES [tempOE->OperandLeft_ID - 1].IS_PROPERTY_RESULT)) {                                                                                                                                                                                                                         \
        CCTEMP = (struct CompiledClass *)((VariableDATA *)(PROPERTIES [tempOE->OperandLeft_ID - 1].CALL_SET))->CLASS_DATA;                                                                                                                                                                                      \
        WRITE_UNLOCK                                                                                                                                                                                                                                                                                            \
        CCTEMP->_Class->SetProperty(PIF, PROPERTIES [tempOE->OperandLeft_ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [tempOE->OperandLeft_ID - 1].CALL_SET), tempOE, CCTEMP->_Class->CLSID == ClassID, tempOE->OperandLeft_ID , LOCAL_CONTEXT, ClassID, THISREF->LocalClassID, &THROW_DATA, STACK_TRACE); \
        if (THROW_DATA) {                                                                                                                                                                                                                                                                                       \
            if (ConceptInterpreter_Catch(THISREF, THROW_DATA, LOCAL_CONTEXT, OE, PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {                                                                                                                                   \
                FAST_FREE(PIF, PROPERTIES);                                                                                                                                                                                                                                                                     \
                PROPERTIES = 0;                                                                                                                                                                                                                                                                                 \
                WRITE_UNLOCK                                                                                                                                                                                                                                                                                    \
                return 0;                                                                                                                                                                                                                                                                                       \
            }                                                                                                                                                                                                                                                                                                   \
        }                                                                                                                                                                                                                                                                                                       \
    }
//---------------------------------------------------------
#define UPDATE_STRING_VARIABLE_THAT_HAS_INDEX(THISREF, VARIABLE, PROPERTIES)                                                        \
    if ((VARIABLE->CLASS_DATA) && (VARIABLE->TYPE == VARIABLE_STRING) && (((struct plainstring *)VARIABLE->CLASS_DATA)->EXTRA_DATA)) { \
        RuntimeOptimizedElement *tempOE = &OPT->CODE [((struct plainstring *)VARIABLE->CLASS_DATA)->EXTRA_DATA - 1];                \
        ((struct plainstring *)VARIABLE->CLASS_DATA)->EXTRA_DATA = 0;                                                               \
        D_LONG_TYPE _def_index = -1;                                                                                                \
        if (LOCAL_CONTEXT [tempOE->OperandLeft_ID - 1]->TYPE == VARIABLE_STRING) {                                                  \
            switch (LOCAL_CONTEXT [tempOE->OperandRight_ID - 1]->TYPE) {                                                            \
                case VARIABLE_NUMBER:                                                                                               \
                    _def_index = (D_LONG_TYPE)LOCAL_CONTEXT [tempOE->OperandRight_ID - 1]->NUMBER_DATA;                             \
                    break;                                                                                                          \
                case VARIABLE_STRING:                                                                                               \
                    _def_index = CONCEPT_STRING_INT(LOCAL_CONTEXT [tempOE->OperandRight_ID - 1]);                                   \
                    break;                                                                                                          \
            }                                                                                                                       \
            CONCEPT_STRING_REPLACE(LOCAL_CONTEXT [tempOE->OperandLeft_ID - 1], VARIABLE, _def_index);                               \
            TEMP_PROPERTY_CODE(THISREF, PROPERTIES)                                                                                 \
        }                                                                                                                           \
    }                                                                                                                               \
//---------------------------------------------------------
#define CHECK_IF_STRING_INDEX_VAR(THISREF, VARIABLE)                                                                                \
    if ((asg_type != VARIABLE_STRING) && (VARIABLE->TYPE == VARIABLE_STRING) && (VARIABLE->CLASS_DATA) && (((struct plainstring *)VARIABLE->CLASS_DATA)->EXTRA_DATA)) {      \
        PIF->RunTimeError(1330, ERR1330, OE, THISREF->OWNER, STACK_TRACE);                                                          \
        DECLARE_PATH(0x20);                                                                                                         \
        continue;                                                                                                                   \
    }
//---------------------------------------------------------

#define CLASS_CHECK_KEEP_EXTRA(VARIABLE)                                                   \
    if ((VARIABLE->CLASS_DATA) && (VARIABLE->TYPE != VARIABLE_STRING)) {                   \
        if (VARIABLE->TYPE == VARIABLE_CLASS) {                                            \
            if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {                \
                delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA); }      \
            VARIABLE->TYPE       = VARIABLE_NUMBER;                                        \
            VARIABLE->CLASS_DATA = NULL;                                                   \
        } else                                                                             \
        if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
            if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS) {                        \
                delete_Array((struct Array *)VARIABLE->CLASS_DATA); }                      \
            VARIABLE->TYPE       = VARIABLE_NUMBER;                                        \
            VARIABLE->CLASS_DATA = NULL;                                                   \
        } else                                                                             \
        if (VARIABLE->TYPE == VARIABLE_NUMBER) {                                           \
            VARIABLE->CLASS_DATA = NULL;                                                   \
        } else                                                                             \
        if (VARIABLE->TYPE == VARIABLE_DELEGATE) {                                         \
            delete_Delegate(VARIABLE->CLASS_DATA);                                         \
            VARIABLE->TYPE       = VARIABLE_NUMBER;                                        \
            VARIABLE->CLASS_DATA = NULL;                                                   \
        }                                                                                  \
    }
//---------------------------------------------------------
#ifdef SIMPLE_MULTI_THREADING
#define CLASS_CHECK_TS(VARIABLE, STACK_TRACE)                                                  \
        if ((VARIABLE->TYPE != VARIABLE_NUMBER) && (VARIABLE->CLASS_DATA)) {                   \
            if (VARIABLE->TYPE == VARIABLE_STRING) {                                           \
                plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA);                \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_CLASS) {                                            \
                if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {                \
                    if (PIF->WriteLock.MasterLock) {                                           \
                        WRITE_UNLOCK                                                           \
                        delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA, STACK_TRACE); \
                        WRITE_LOCK                                                             \
                    } else                                                                     \
                        delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA, STACK_TRACE); \
                }                                                                              \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {                                            \
                if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS) {                        \
                    if (PIF->WriteLock.MasterLock) {                                           \
                        WRITE_UNLOCK                                                           \
                        delete_Array((struct Array *)VARIABLE->CLASS_DATA);                    \
                        WRITE_LOCK                                                             \
                    } else                                                                     \
                        delete_Array((struct Array *)VARIABLE->CLASS_DATA);                    \
                }                                                                              \
            } else                                                                             \
            if (VARIABLE->TYPE == VARIABLE_DELEGATE) {                                         \
                if (PIF->WriteLock.MasterLock) {                                               \
                    WRITE_UNLOCK                                                               \
                    delete_Delegate(VARIABLE->CLASS_DATA, STACK_TRACE);                        \
                    WRITE_LOCK                                                                 \
                } else                                                                         \
                    delete_Delegate(VARIABLE->CLASS_DATA, STACK_TRACE);                        \
            }                                                                                  \
            VARIABLE->CLASS_DATA = NULL;                                                       \
        }  
#else
#define CLASS_CHECK_TS              CLASS_CHECK
#endif

#define INTERNAL_CLASS_CHECK_RESULT(PIF, VARIABLE) { \
    if (VARIABLE->LINKS > 1) { \
        VARIABLE->LINKS--; \
        VariableDATA *RETURN_DATA = (VariableDATA *)VAR_ALLOC(PIF); \
        RETURN_DATA->LINKS              = 1; \
        RETURN_DATA->NUMBER_DATA        = 0; \
        RETURN_DATA->TYPE               = VARIABLE_NUMBER; \
        RETURN_DATA->IS_PROPERTY_RESULT = 0; \
        VARIABLE = RETURN_DATA; \
    } else { \
        CLASS_CHECK_TS(VARIABLE, STACK_TRACE); \
    }\
}

#define CLASS_CHECK_RESULT(VARIABLE)    INTERNAL_CLASS_CHECK_RESULT(PIF, VARIABLE);
#define RESET_PROPERTIES(PROPERTIES, ID){if (PROPERTIES) PROPERTIES[ID - 1].IS_PROPERTY_RESULT = 0;}

#ifndef INLINE_COMMON_CALLS
void FREE_VARIABLE(VariableDATA *VARIABLE, SCStack *STACK_TRACE) {
    VARIABLE->LINKS--;
    if (!VARIABLE->LINKS) {
        if (VARIABLE->CLASS_DATA) {
            if (VARIABLE->TYPE == VARIABLE_STRING) {
                plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA);
            } else
            if (VARIABLE->TYPE == VARIABLE_CLASS) {
                if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)
                    delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA, STACK_TRACE);
            } else
            if (VARIABLE->TYPE == VARIABLE_ARRAY) {
                if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS)
                    delete_Array((struct Array *)VARIABLE->CLASS_DATA);
            } else
            if (VARIABLE->TYPE == VARIABLE_DELEGATE)
                delete_Delegate(VARIABLE->CLASS_DATA, STACK_TRACE);
        }
        VAR_FREE(VARIABLE);
    }
}
#endif

void COPY_VARIABLE(VariableDATA *DEST, VariableDATA *SRC, SCStack *STACK_TRACE) {
    CLASS_CHECK(DEST, STACK_TRACE);
    DEST->TYPE = SRC->TYPE;
    DEST->IS_PROPERTY_RESULT = 0;
    switch (SRC->TYPE) {
        case VARIABLE_NUMBER:
            DEST->NUMBER_DATA = SRC->NUMBER_DATA;
            return;
        case VARIABLE_STRING:
            if (SRC->CLASS_DATA) {
                NEW_CONCEPT_STRING2(DEST, SRC);
            } else {
                DEST->CLASS_DATA = NULL;
            }
            return;
        case VARIABLE_CLASS:
            DEST->CLASS_DATA = SRC->CLASS_DATA;
            ((struct CompiledClass *)DEST->CLASS_DATA)->LINKS++;
            return;
        case VARIABLE_ARRAY:
            DEST->CLASS_DATA = SRC->CLASS_DATA;
            ((struct Array *)DEST->CLASS_DATA)->LINKS++;
            return;
        case VARIABLE_DELEGATE:
            DEST->CLASS_DATA = copy_Delegate(SRC->CLASS_DATA);
            return;
    }
}
//---------------------------------------------------------
#define RESTORE_TRY_DATA(THISREF)                                                    \
    if (PREVIOUS_TRY) {                                                              \
        CATCH_INSTRUCTION_POINTER = OPT->CODE [PREVIOUS_TRY - 1].OperandReserved_ID; \
        CATCH_VARIABLE            = OPT->CODE [PREVIOUS_TRY - 1].Result_ID;          \
    }
//---------------------------------------------------------

#define EVAL_NUMBER_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                                    \
    switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {                                                                                                                                                                                                                                         \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;                                                                                                                      \
            break;                                                                                                                                                                                                                                                                                   \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR CONCEPT_STRING_FLOAT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);                                                                                                             \
            break;                                                                                                                                                                                                                                                                                   \
        default:                                                                                                                                                                                                                                                                                     \
            PIF->RunTimeError(1204, ERR1204, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                \

#define EVAL_NUMBER_EXPRESSION2(THISREF, OPERATOR)                                                                                                                                                                                                                                                   \
    switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {                                                                                                                                                                                                                                         \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;                                                                                                                                                                       \
            break;                                                                                                                                                                                                                                                                                   \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR CONCEPT_STRING_FLOAT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);                                                                                                                                                              \
            break;                                                                                                                                                                                                                                                                                   \
        default:                                                                                                                                                                                                                                                                                     \
            PIF->RunTimeError(1204, ERR1204, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                \

#define EVAL_INTEGER_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                                   \
    switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {                                                                                                                                                                                                                                         \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR(unsigned INTEGER) LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;                                                                                  \
            break;                                                                                                                                                                                                                                                                                   \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR(unsigned INTEGER) CONCEPT_STRING_FLOAT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);                                                                         \
            break;                                                                                                                                                                                                                                                                                   \
        default:                                                                                                                                                                                                                                                                                     \
            PIF->RunTimeError(1204, ERR1204, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                \

#define EVAL_ASG_INTEGER_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                               \
    switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {                                                                                                                                                                                                                                         \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = (unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR(unsigned INTEGER) LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;                            \
            break;                                                                                                                                                                                                                                                                                   \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                        \
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = (unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR(unsigned INTEGER) CONCEPT_STRING_FLOAT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);                   \
            break;                                                                                                                                                                                                                                                                                   \
        default:                                                                                                                                                                                                                                                                                     \
            PIF->RunTimeError(1204, ERR1204, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                \

#define EVAL_DIVIDE_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                                          \
    switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {                                                                                                                                                                                                                                               \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                              \
            if (!LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA)                                                                                                                                                                                                                                     \
                PIF->RunTimeError(260, ERR260, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                           \
            else                                                                                                                                                                                                                                                                                           \
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;                                                                                                                        \
            break;                                                                                                                                                                                                                                                                                         \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                              \
            {                                                                                                                                                                                                                                                                                              \
                double dt = CONCEPT_STRING_FLOAT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);                                                                                                                                                                                                                 \
                if (!dt)                                                                                                                                                                                                                                                                                   \
                    PIF->RunTimeError(260, ERR260, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                       \
                else                                                                                                                                                                                                                                                                                       \
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR dt;                                                                                                                                                                      \
            }                                                                                                                                                                                                                                                                                              \
            break;                                                                                                                                                                                                                                                                                         \
        default:                                                                                                                                                                                                                                                                                           \
            PIF->RunTimeError(1204, ERR1204, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                             \
    }

#define EVAL_DIVIDE_INTEGER_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                                  \
    switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {                                                                                                                                                                                                                                               \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                              \
            if (!LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA)                                                                                                                                                                                                                                     \
                PIF->RunTimeError(260, ERR260, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                           \
            else                                                                                                                                                                                                                                                                                           \
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (INTEGER)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR(INTEGER) LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;                                                                                                      \
            break;                                                                                                                                                                                                                                                                                         \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                              \
            {                                                                                                                                                                                                                                                                                              \
                INTEGER dt = (INTEGER) CONCEPT_STRING_INT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);                                                                                                                                                                                                        \
                if (!dt)                                                                                                                                                                                                                                                                                   \
                    PIF->RunTimeError(260, ERR260, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                       \
                else                                                                                                                                                                                                                                                                                       \
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (INTEGER)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR dt;                                                                                                                                                             \
            }                                                                                                                                                                                                                                                                                              \
            break;                                                                                                                                                                                                                                                                                         \
        default:                                                                                                                                                                                                                                                                                           \
            PIF->RunTimeError(1204, ERR1204, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                             \
    }

#define EVAL_ASG_INTEGER_DIVIDE_EXPRESSION(THISREF, OPERATOR)                                                                                                                                                                                                                                              \
    switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {                                                                                                                                                                                                                                               \
        case VARIABLE_NUMBER:                                                                                                                                                                                                                                                                              \
            if (!LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA)                                                                                                                                                                                                                                     \
                PIF->RunTimeError(260, ERR260, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                           \
            else                                                                                                                                                                                                                                                                                           \
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = (INTEGER)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR(INTEGER) LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;                                                \
            break;                                                                                                                                                                                                                                                                                         \
        case VARIABLE_STRING:                                                                                                                                                                                                                                                                              \
            {                                                                                                                                                                                                                                                                                              \
                INTEGER dt = (INTEGER)CONCEPT_STRING_INT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);                                                                                                                                                                                                         \
                if (!dt)                                                                                                                                                                                                                                                                                   \
                    PIF->RunTimeError(260, ERR260, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                       \
                else                                                                                                                                                                                                                                                                                       \
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = (INTEGER)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA OPERATOR dt;                                                                                                       \
            }                                                                                                                                                                                                                                                                                              \
            break;                                                                                                                                                                                                                                                                                         \
        default:                                                                                                                                                                                                                                                                                           \
            PIF->RunTimeError(1204, ERR1204, OE, THISREF->OWNER, STACK_TRACE);                                                                                                                                                                                                                             \
    }

//---------------------------------------------------------
//#ifndef SINGLE_PROCESS_DLL
//#endif

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

#if defined(USE_JIT) || defined (USE_JIT_TRACE)
extern "C" {
#ifdef USE_JIT_TRACE
 #ifdef ARM_PATCH
    #define JIT_CALL_FUNC(func)
                            if (usedflags[OE->Result_ID - 1]) { \
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(func)); \
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0); \
                            } else { \
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(func ## _v2)); \
                                sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_TEMPORARY_EREG1, 0); \
                            }
 #else
    #define JIT_CALL_FUNC(func) \
                            if (usedflags[OE->Result_ID - 1]) \
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(func)); \
                            else \
                                sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_IMM, SLJIT_FUNC_OFFSET(func ## _v2));
 #endif
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

#ifdef EXPERIMENTAL_FEATURES
static sljit_sw SLJIT_CALL ClassDataVD(Optimizer *OPT, RuntimeOptimizedElement *OE, void **data) {
    VariableDATA **LOCAL_CONTEXT = (VariableDATA **)data[0];
    SCStack *STACK_CONTEXT = (SCStack *)data[1];
    VariableDATAPROPERTY **PROPERTIES = (VariableDATAPROPERTY **)data[2];

    VariableDATA *arr = LOCAL_CONTEXT[OE->OperandLeft_ID - 1];
    if ((arr->TYPE == VARIABLE_CLASS) && (arr->CLASS_DATA)) {
        CompiledClass *CCTEMP = (CompiledClass *)arr->CLASS_DATA;
        signed char    next_is_asg;
        VariableDATA  *THROW_DATA = 0;
        VariableDATA  *data = 0;
        ParamList *FORMAL_PARAMETERS;
        struct ConceptInterpreter *THIS_REF = (struct ConceptInterpreter *)OPT->INTERPRETER;

        if (OE->OperandReserved_ID) {
            FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
            next_is_asg       = 0;
        } else {
            next_is_asg       = OE->OperandReserved_TYPE;
            FORMAL_PARAMETERS = 0;
        }
        // data = CCTEMP->_Class->ExecuteMember(OPT->PIFOwner, OE->OperandRight_ID - 1, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], OE, CCTEMP->_Class->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, -1, -1, &THROW_DATA, NULL, next_is_asg, NULL, OPT->dataCount, OE->Result_ID - 1);
        data = CCTEMP->_Class->ExecuteMember(OPT->PIFOwner, OE->OperandRight_ID - 1, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], OE, 1, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, -1, -1, &THROW_DATA, STACK_CONTEXT, next_is_asg, PROPERTIES, OPT->dataCount, OE->Result_ID - 1);
        if (data == LOCAL_CONTEXT [OE->Result_ID - 1])
            return 1;
        if (((data) && ((data->TYPE == VARIABLE_CLASS) || (data->TYPE == VARIABLE_DELEGATE)) && (!data->CLASS_DATA)) || (!data)) {
            CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_CONTEXT);
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
        }
        if (data) {
            FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_CONTEXT);
            LOCAL_CONTEXT [OE->Result_ID - 1] = data;
            data->LINKS++;
        } else {
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
        }
        if ((data) && (data->TYPE != VARIABLE_NUMBER))
            return 0;
        return 1;
    }
    return 0;
}
#endif

static sljit_sw SLJIT_CALL ArrayDataVD(VariableDATA *arr, VariableDATA *idx) {
    int index;

    double2int(index, idx->NUMBER_DATA);
    if ((arr->TYPE == VARIABLE_ARRAY) && (arr->CLASS_DATA) && (index >= 0)) {
  #ifdef OPTIMIZE_FAST_ARRAYS
        VariableDATA *data = FAST_CHECK((struct Array *)arr->CLASS_DATA, index);
        if (!data)
            data = Array_GetWithCreate((struct Array *)arr->CLASS_DATA, index);
  #else
        VariableDATA *data = Array_GetWithCreate((struct Array *)arr->CLASS_DATA, index);
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
        VariableDATA *data = FAST_CHECK((struct Array *)arr->CLASS_DATA, index);
        if (!data)
            data = Array_GetWithCreate((struct Array *)arr->CLASS_DATA, index);
  #else
        VariableDATA *data = Array_GetWithCreate((struct Array *)arr->CLASS_DATA, index);
  #endif
        if (data->TYPE == VARIABLE_NUMBER) {
            RESET_VARIABLE(result, NULL);
            result->NUMBER_DATA = data->NUMBER_DATA;
            return 1;
        } else {
            switch (data->TYPE) {
                case VARIABLE_STRING:
                    RESET_VARIABLE(result, NULL);
                    if ((data->CLASS_DATA) && (CONCEPT_C_LENGTH(data)))
                        result->NUMBER_DATA = 1;
                    else
                        result->NUMBER_DATA = 0;
                    return 1;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    RESET_VARIABLE(result, NULL);
                    result->NUMBER_DATA = 1;
                    return 1;

                case VARIABLE_ARRAY:
                    RESET_VARIABLE(result, NULL);
                    if ((data->CLASS_DATA) && (Array_Count((struct Array *)data->CLASS_DATA)))
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
    Array    *arrData = (struct Array *)arr->CLASS_DATA;
    if ((arr->TYPE == VARIABLE_ARRAY) && (arrData) && (index >= 0)) {
        res = 0;
  #ifdef OPTIMIZE_FAST_ARRAYS
        VariableDATA **cached_data = arrData->cached_data;
        VariableDATA *data         = ((index < STATIC_ARRAY_THRESHOLD) && (cached_data)) ? cached_data[index] : 0;
        if (!data)
            data = Array_GetWithCreate(arrData, index);
  #else
        VariableDATA *data = Array_GetWithCreate(arrData, index);
  #endif
        int type = data->TYPE;

        if (type == VARIABLE_NUMBER) {
            return data->NUMBER_DATA ? 1 : 0;
        } else
        if (type == VARIABLE_STRING) {
            if ((data->CLASS_DATA) && (CONCEPT_C_LENGTH(data)))
                res = 1;
            return res;
        } else
        if (type == VARIABLE_ARRAY) {
            if ((data->CLASS_DATA) && (Array_Count((struct Array *)data->CLASS_DATA)))
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
        VariableDATA *data = FAST_CHECK((struct Array *)arr->CLASS_DATA, index);
        if (!data)
            data = Array_GetWithCreate((struct Array *)arr->CLASS_DATA, index);
  #else
        VariableDATA *data = Array_GetWithCreate((struct Array *)arr->CLASS_DATA, index);
  #endif
        int type = data->TYPE;

        if (type == VARIABLE_NUMBER) {
            return data->NUMBER_DATA ? 1 : 0;
        } else
        if (type == VARIABLE_STRING) {
            if ((data->CLASS_DATA) && (CONCEPT_C_LENGTH(data)))
                res = 1;
            return res;
        } else
        if (type == VARIABLE_ARRAY) {
            if ((data->CLASS_DATA) && (Array_Count((struct Array *)data->CLASS_DATA)))
                res = 1;
            return res;
        } else
            res = 1;
    }
    return res;
}

static sljit_sw SLJIT_CALL ArraySETREGION(VariableDATA **data, RuntimeOptimizedElement *CODE, sljit_sw i) {
    int      target        = CODE[i].OperandLeft_ID - 1;
    int      limit         = CODE[i + 1].OperandRight_ID - 1;
    int      arr           = CODE[i + 3].OperandLeft_ID - 1;
    int      value         = CODE[i + 4].OperandRight_ID - 1;
    int      increment     = -1;
    intptr_t increment_val = 1;

    if (CODE[i + 5].Operator_ID == KEY_ASU)
        increment = CODE[i + 5].OperandRight_ID - 1;

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

    Array *arr_object = (struct Array *)arr_var->CLASS_DATA;

    VariableDATA **cached_data = arr_object->cached_data;
    if (iterator < 0)
        return 0;

    if (increment_val == 1) {
        for ( ; iterator < limit_val; ++iterator) {
  #ifdef OPTIMIZE_FAST_ARRAYS
            VariableDATA *data_var = ((iterator < STATIC_ARRAY_THRESHOLD) && (cached_data)) ? cached_data[iterator] : 0;
            if (!data_var)
                data_var = Array_GetWithCreate(arr_object, iterator, val_var->NUMBER_DATA);
  #else
            VariableDATA *data_var = Array_GetWithCreate(arr_object, iterator, val_var->NUMBER_DATA);
  #endif
            if (data_var->TYPE == VARIABLE_NUMBER) {
                data_var->NUMBER_DATA = val_var->NUMBER_DATA;
            } else
            if (data_var->TYPE != VARIABLE_CLASS) {
                RESET_VARIABLE(data_var, NULL);
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
                data_var = Array_GetWithCreate(arr_object, iterator, val_var->NUMBER_DATA);
  #else
            VariableDATA *data_var = Array_GetWithCreate(arr_object, iterator, val_var->NUMBER_DATA);
  #endif
            if (data_var->TYPE == VARIABLE_NUMBER) {
                data_var->NUMBER_DATA = val_var->NUMBER_DATA;
            } else
            if (data_var->TYPE != VARIABLE_CLASS) {
                RESET_VARIABLE(data_var, NULL);
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

    Array *arrData = (struct Array *)arr->CLASS_DATA;
    if ((arr->TYPE == VARIABLE_ARRAY) && (arrData) && (index >= 0)) {
  #ifdef OPTIMIZE_FAST_ARRAYS
        VariableDATA **cached_data = arrData->cached_data;
        VariableDATA *data         = ((index < STATIC_ARRAY_THRESHOLD) && (cached_data)) ? cached_data[index] : 0;
        if (!data)
            data = Array_GetWithCreate(arrData, index);
  #else
        VariableDATA *data = Array_GetWithCreate(arrData, index);
  #endif
        if (data->TYPE == VARIABLE_NUMBER) {
            data->NUMBER_DATA = newvalue->NUMBER_DATA;
            return 1;
        } else
        if (data->TYPE != VARIABLE_CLASS) {
            RESET_VARIABLE(data, NULL);
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
            VariableDATA *data = FAST_CHECK((struct Array *)arr->CLASS_DATA, index);
            if (!data)
                data = Array_GetWithCreate((struct Array *)arr->CLASS_DATA,  index);
  #else
            VariableDATA *data = Array_GetWithCreate((struct Array *)arr->CLASS_DATA, index);
  #endif
            if (data->TYPE == VARIABLE_NUMBER) {
                data->NUMBER_DATA = newvalue;
            } else
            if (data->TYPE != VARIABLE_CLASS) {
                RESET_VARIABLE(data, NULL);
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
                RESET_VARIABLE(result, NULL);
                result->NUMBER_DATA = Array_Count((struct Array *)target->CLASS_DATA);
                return 1;
            }
            break;

        case VARIABLE_STRING:
            if (target->CLASS_DATA) {
                RESET_VARIABLE(result, NULL);
                result->NUMBER_DATA = (CONCEPT_C_LENGTH(target));
                return 1;
            } else {
                RESET_VARIABLE(result, NULL)
                result->NUMBER_DATA = 0;
                return 1;
            }
            break;
    }
    return 0;
}

static sljit_sw SLJIT_CALL ReplaceVariable(VariableDATA **a, sljit_sw index1, VariableDATA *d) {
    if (a[index1] != d) {
        FREE_VARIABLE(a[index1], NULL);
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
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(err, 0, code, "", "JIT"));
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

// version A
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

// version B
static sljit_sw SLJIT_CALL c_modVD2_v2(VariableDATA *left, VariableDATA *right) {
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
    return 0;
}

static sljit_sw SLJIT_CALL c_AANVD_v2(VariableDATA *left, VariableDATA *right) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA & (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_AXOVD_v2(VariableDATA *left, VariableDATA *right) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA ^ (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_AORVD_v2(VariableDATA *left, VariableDATA *right) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA | (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_ASLVD_v2(VariableDATA *left, VariableDATA *right) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA << (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}

static sljit_sw SLJIT_CALL c_ASRVD_v2(VariableDATA *left, VariableDATA *right) {
    left->NUMBER_DATA   = (unsigned INTEGER)left->NUMBER_DATA >> (unsigned INTEGER)right->NUMBER_DATA;
    return 0;
}
// end version B

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

struct ConceptInterpreter *new_ConceptInterpreter(Optimizer *O, INTEGER LocalClsID, ClassMember *Owner) {
    struct ConceptInterpreter *self = (struct ConceptInterpreter *)malloc(sizeof(struct ConceptInterpreter));
    self->LocalClassID              = LocalClsID;
    self->OWNER                     = Owner;
#ifdef USE_JIT_TRACE
    self->callcount                 = 0;
    self->jittracecode              = 0;
    self->initcode.code             = 0;
#endif
    return self;
}

#define DECLARE_PATH(TYPE)

#ifdef USE_JIT_TRACE
 #define OFFSETOF(type, field)    ((uintptr_t)&(((type *)0)->field))

 #define OPERAND_LEFT       if (reg1 != OE->OperandLeft_ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandLeft_ID - 1)); reg1 = OE->OperandLeft_ID; }
 #define OPERAND_RIGHT      if (reg2 != OE->OperandRight_ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandRight_ID - 1)); reg2 = OE->OperandRight_ID; }
 #define OPERAND_RESULT     if (reg3 != OE->Result_ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->Result_ID - 1)); reg3 = OE->Result_ID; }
 #define OPERAND_RESULT2    if (reg2 != OE->Result_ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->Result_ID - 1)); reg2 = OE->Result_ID; }
 #define OPERAND_LEFT2      if (reg2 != OE->OperandLeft_ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandLeft_ID - 1)); reg2 = OE->OperandLeft_ID; }
 #define OPERAND_RIGHT2     if (reg1 != OE->OperandRight_ID) { sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandRight_ID - 1)); reg1 = OE->OperandRight_ID; }

static sljit_sw SLJIT_CALL CreateArrayVariable(VariableDATA *LOCAL_CONTEXT_i, PIFAlizator *PIF) {
    LOCAL_CONTEXT_i->CLASS_DATA = NULL;
    LOCAL_CONTEXT_i->CLASS_DATA = new_Array(PIF, true);
    return 0;
}

static sljit_sw SLJIT_CALL CreateStringVariable(VariableDATA *LOCAL_CONTEXT_i, RuntimeVariableDESCRIPTOR *TARGET, PIFAlizator *PIF) {
    NEW_CONCEPT_STRING_BUFFER(LOCAL_CONTEXT_i, TARGET->value.c_str(), TARGET->value.Length());
    return 0;
}

static sljit_sw SLJIT_CALL AllocVariable(PIFAlizator *PIF) {
    return (sljit_sw)VAR_ALLOC(PIF);
}

static sljit_sw SLJIT_CALL ReleaseVariable(VariableDATA *LOCAL_CONTEXT_i) {
    if (LOCAL_CONTEXT_i) {
        LOCAL_CONTEXT_i->LINKS--;
        if (LOCAL_CONTEXT_i->LINKS)
            return 0;

        CLASS_CHECK_NO_RESET(LOCAL_CONTEXT_i, NULL);
    }
    return (sljit_sw)LOCAL_CONTEXT_i;
}

void *ConceptInterpreter_ContextCreateJIT(struct ConceptInterpreter *self, Optimizer *OPT) {
    if ((!OPT) || (!self->OWNER))
        return NULL;

    INTEGER data_count = OPT->dataCount;
    INTEGER i = self->OWNER->PARAMETERS_COUNT + 1;

    if ((data_count - i) <= 2)
        return NULL;
    struct sljit_compiler *compiler = sljit_create_compiler(NULL);
#ifdef ARM_PATCH
    sljit_emit_enter(compiler, 0, 2, 4, 3, 0, 0, 0);
#else
    sljit_emit_enter(compiler, 0, 2, 3, 3, 0, 0, 0);
#endif
    while (i < data_count) {
        // variable descriptor
        RuntimeVariableDESCRIPTOR *TARGET = &OPT->DATA [i];
        // R0 = VariableDATA
#ifdef POOL_BLOCK_ALLOC
        sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * i);
        struct sljit_jump *jump = sljit_emit_cmp(compiler, SLJIT_NOT_ZERO, SLJIT_R0, 0, SLJIT_IMM, 0);
#endif
        sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_S1, 0);
#ifdef ARM_PATCH
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(AllocVariable));
        sljit_emit_ijump(compiler, SLJIT_CALL1, SLJIT_TEMPORARY_EREG1, 0);
#else
        sljit_emit_ijump(compiler, SLJIT_CALL1, SLJIT_IMM, SLJIT_FUNC_OFFSET(AllocVariable));
#endif
        sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_RETURN_REG, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * i, SLJIT_R0, 0);
#ifdef POOL_BLOCK_ALLOC
        sljit_set_label(jump, sljit_emit_label(compiler));
#endif
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, LINKS), SLJIT_IMM, (sljit_u16)1);
        if (TARGET->BY_REF == 2)
            sljit_emit_op1(compiler, SLJIT_MOV_S8, SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, IS_PROPERTY_RESULT), SLJIT_IMM, (sljit_s8)-1);
        else
            sljit_emit_op1(compiler, SLJIT_MOV_S8, SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, IS_PROPERTY_RESULT), SLJIT_IMM, (sljit_s8)0);
        
        signed char TYPE = TARGET->TYPE;
        if (TYPE < 0)
            TYPE = -TYPE;
        sljit_emit_op1(compiler, SLJIT_MOV_S8, SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, TYPE), SLJIT_IMM, (sljit_s8)TYPE);

        switch (TYPE) {
            case VARIABLE_NUMBER:
                sljit_emit_fop1(compiler, SLJIT_MOV_F64,    
                                          SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                          SLJIT_MEM0(), (sljit_sw) &TARGET->nValue);
                break;
            case VARIABLE_ARRAY:
                sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_S1, 0);
#ifdef ARM_PATCH
                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(CreateArrayVariable));
                sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_TEMPORARY_EREG1, 0);
#else
                sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_IMM, SLJIT_FUNC_OFFSET(CreateArrayVariable));
#endif
                break;
            case VARIABLE_STRING:
                if (TARGET->value.Length()) {
                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)TARGET);
                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_S1, 0);
#ifdef ARM_PATCH
                    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(CreateStringVariable));
                    sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
#else
                    sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(CreateStringVariable));
#endif
                    break;
                }
                // no break here
            default:
                sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, CLASS_DATA), SLJIT_IMM, (sljit_sw)0);
                break;
        }
        i++;
    }
    sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, data_count);

    void *code = sljit_generate_code(compiler);
    sljit_free_compiler(compiler);

    return code;
}

void *ConceptInterpreter_ContextDestroyJIT(struct ConceptInterpreter *self, Optimizer *OPT) {
    if ((!OPT) || (!self->OWNER))
        return NULL;
    struct sljit_compiler *compiler = sljit_create_compiler(NULL);
#ifdef ARM_PATCH
    sljit_emit_enter(compiler, 0, 1, 4, 3, 0, 0, 0);
#else
    sljit_emit_enter(compiler, 0, 1, 3, 3, 0, 0, 0);
#endif
    INTEGER data_count = OPT->dataCount;
    for (int i = 0; i < data_count; i++) {
        // R0 = VariableDATA
        sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * i);
        struct sljit_jump *jump = sljit_emit_cmp(compiler, SLJIT_ZERO, SLJIT_R0, 0, SLJIT_IMM, 0);
        // sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_S1, 0);
#ifdef ARM_PATCH
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(ReleaseVariable));
        sljit_emit_ijump(compiler, SLJIT_CALL1, SLJIT_TEMPORARY_EREG1, 0);
#else
        sljit_emit_ijump(compiler, SLJIT_CALL1, SLJIT_IMM, SLJIT_FUNC_OFFSET(ReleaseVariable));
#endif
        sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_RETURN_REG, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * i, SLJIT_R0, 0);
    }
    sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, data_count);

    void *code = sljit_generate_code(compiler);
    sljit_free_compiler(compiler);

    return code;
}

void ConceptInterpreter_AnalizeInstructionPath(struct ConceptInterpreter *self, Optimizer *OPT) {
    int count = OPT->codeCount;

    if (!count)
        return;

    if (!self->initcode.code)
        self->initcode.code = ConceptInterpreter_ContextCreateJIT(self, OPT);

    struct sljit_compiler *compiler = 0;
    int dataCount           = OPT->dataCount;
    int instruction_pointer = 1;
    RuntimeOptimizedElement *OE;
    void **jittracecode = (void **)malloc(sizeof(void *) * count);
    memset(jittracecode, 0, sizeof(void *) * count);
    static sljit_f64 d_true     = 1;
    static sljit_f64 d_false    = 0;
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

    INTEGER ParamCount = self->OWNER->PARAMETERS_COUNT;
    for (int i = 1; i <= ParamCount; i++) {
        RuntimeVariableDESCRIPTOR *TARGET = &OPT->DATA [i];
        if (TARGET->BY_REF)
            usedflags[i] = 2;
    }
    for (int i = 0; i < count; i++) {
        OE = &OPT->CODE[i];
        if (IS_OPERATOR(OE)) {
            switch (OE->Operator_ID) {
                case KEY_SEL:
                    if (OE->OperandReserved_ID) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
                        if (FORMAL_PARAMETERS) {
                            FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                            FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);
                            if (FORMAL_PARAMETERS2.PARAM_INDEX) {
                                for (int j = 0; j < FORMAL_PARAMETERS2.COUNT; j++) {
                                    int idx = FORMAL_PARAMETERS2.PARAM_INDEX[j] - 1;
                                    if (usedflags[idx] != 3) {
                                        if (usedflags[idx])
                                            usedflags[idx] = 2;
                                        else
                                            usedflags[idx] = 1;
                                    }
                                }
                            }
                        }
                    }

                case KEY_DELETE:
                case KEY_TYPE_OF:
                case KEY_CLASS_NAME:
                case KEY_LENGTH:
                    if (usedflags[OE->OperandLeft_ID - 1] != 3) {
                        if (usedflags[OE->OperandLeft_ID - 1])
                            usedflags[OE->OperandLeft_ID - 1] = 2;
                        else
                            usedflags[OE->OperandLeft_ID - 1] = 1;
                    }
                    break;

                case KEY_NEW:
                    if ((OE->OperandReserved_ID > 0) && (OE->OperandLeft_ID != STATIC_CLASS_ARRAY)) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
                        if (FORMAL_PARAMETERS) {
                            FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                            FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);

                            if (FORMAL_PARAMETERS2.PARAM_INDEX) {
                                for (int j = 0; j < FORMAL_PARAMETERS2.COUNT; j++) {
                                    int idx = FORMAL_PARAMETERS2.PARAM_INDEX[j] - 1;
                                    if (usedflags[idx] != 3) {
                                        if (usedflags[idx])
                                            usedflags[idx] = 2;
                                        else
                                            usedflags[idx] = 1;
                                    }
                                }
                            }
                        }
                    }
                    break;

                case KEY_DLL_CALL:
                    if (OE->OperandReserved_ID) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
                        if (FORMAL_PARAMETERS) {
                            FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                            FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);

                            if (FORMAL_PARAMETERS2.PARAM_INDEX) {
                                for (int j = 0; j < FORMAL_PARAMETERS2.COUNT; j++) {
                                    int idx = FORMAL_PARAMETERS2.PARAM_INDEX[j] - 1;
                                    if (usedflags[idx] != 3) {
                                        if (usedflags[idx])
                                            usedflags[idx] = 2;
                                        else
                                            usedflags[idx] = 1;
                                    }
                                }
                            }
                        }
                    }
                    break;
                case KEY_CND_NULL:
                    usedflags[OE->OperandRight_ID - 1] = 3;
                    usedflags[OE->OperandLeft_ID - 1] = 3;
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
                    if (usedflags[OE->OperandRight_ID - 1] != 3) {
                        if (usedflags[OE->OperandRight_ID - 1])
                            usedflags[OE->OperandRight_ID - 1] = 2;
                        else
                            usedflags[OE->OperandRight_ID - 1] = 1;
                    }
                    if (usedflags[OE->OperandLeft_ID - 1] != 3) {
                        if (usedflags[OE->OperandLeft_ID - 1])
                            usedflags[OE->OperandLeft_ID - 1] = 2;
                        else
                            usedflags[OE->OperandLeft_ID - 1] = 1;
                    }
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
                    if (usedflags[OE->OperandLeft_ID - 1] != 3) {
                        if (usedflags[OE->OperandLeft_ID - 1])
                            usedflags[OE->OperandLeft_ID - 1] = 2;
                        else
                            usedflags[OE->OperandLeft_ID - 1] = 1;
                    }
                    break;
            }
        } else
        if (IS_KEYWORD(OE)) {
            switch (OE->Operator_ID) {
                case KEY_OPTIMIZED_THROW:
                case KEY_OPTIMIZED_RETURN:
                case KEY_OPTIMIZED_ECHO:
                case KEY_OPTIMIZED_IF:
                    if (usedflags[OE->OperandRight_ID - 1] != 3) {
                        if (usedflags[OE->OperandRight_ID - 1])
                            usedflags[OE->OperandRight_ID - 1] = 2;
                        else
                            usedflags[OE->OperandRight_ID - 1] = 1;
                    }
                    // optimize IF .. GOTO to GOTO
                    if ((OE->Operator_ID == KEY_OPTIMIZED_IF) && (OE->OperandReserved_ID < count)) {
                        RuntimeOptimizedElement *OE2 = &OPT->CODE[OE->OperandReserved_ID];
                        while (OE2->Operator_ID == KEY_OPTIMIZED_GOTO) {
                            OE->OperandReserved_ID = OE2->OperandReserved_ID;
                            if (OE2->OperandReserved_ID >= count)
                                break;
                            OE2 = &OPT->CODE[OE2->OperandReserved_ID];
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
            if (IS_OPERATOR(OE)) {
                switch (OE->Operator_ID) {
                    case KEY_NEW:
                        prec_is_new = OE->Result_ID;
                        optimizable = false;
                        break;

                    case KEY_DELETE:
#ifndef EXPERIMENTAL_FEATURES
                    case KEY_SEL:
#endif
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
                        if ((OPT->DATA[OE->OperandRight_ID - 1].TYPE != VARIABLE_NUMBER) && (OPT->DATA[OE->OperandRight_ID - 1].TYPE != -VARIABLE_NUMBER)) {
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
                        if ((OPT->DATA[OE->OperandLeft_ID - 1].TYPE != VARIABLE_NUMBER) && (OPT->DATA[OE->OperandLeft_ID - 1].TYPE != -VARIABLE_NUMBER)) {
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
                        if ((OPT->DATA[OE->OperandRight_ID - 1].TYPE != VARIABLE_NUMBER) && (OPT->DATA[OE->OperandRight_ID - 1].TYPE != -VARIABLE_NUMBER)) {
                            optimizable = false;
                            prec_is_new = false;
                            break;
                        }
                        break;
                }
            } else
            if (IS_KEYWORD(OE)) {
                switch (OE->Operator_ID) {
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
                        if ((OPT->DATA[OE->OperandRight_ID - 1].TYPE != VARIABLE_NUMBER) && (OPT->DATA[OE->OperandRight_ID - 1].TYPE != -VARIABLE_NUMBER)) {
                            optimizable = false;
                            prec_is_new = false;
                            break;
                        }

                    // don't break here
                    case KEY_OPTIMIZED_GOTO:
                        if (OE->OperandReserved_ID < start)
                            outsidejump[OE->OperandReserved_ID] = 1;
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
            //int  *lrptr       = (int *)malloc(sizeof(int) * delta2);
            for (int i = 0; i < delta2; i++) {
                labels[i]    = 0;
                jumps[i]     = 0;
                labelsptr[i] = 0;
                posj[i]      = 0;
                //lrptr[i]     = 0;
            }

            for (int i = start; i < end; i++) {
                RuntimeOptimizedElement *OE = &OPT->CODE[i];
                if (OE) {
                    if (IS_KEYWORD(OE)) {
                        switch (OE->Operator_ID) {
                            case KEY_OPTIMIZED_GOTO:
                            case KEY_OPTIMIZED_IF:
                                if ((OE->OperandReserved_ID >= start) && (OE->OperandReserved_ID < end)) {
                                    labels[OE->OperandReserved_ID - start]++;
                                    //if ((OE->Operator_ID == KEY_OPTIMIZED_GOTO) && (i > start)) {
                                        //OENext = &OPT->CODE[i - 1];
                                        //if ((OENext->Operator_TYPE == TYPE_OPERATOR) && (OENext->OperandLeft.TYPE == TYPE_VARIABLE))
                                        //    lrptr[OE->OperandReserved_ID - start] = OENext->OperandLeft_ID;
                                    //}
                                }
                                break;
                        }
                    }
                }
            }

            compiler = sljit_create_compiler(NULL);
#ifdef EXPERIMENTAL_FEATURES
 #ifdef ARM_PATCH
            sljit_emit_enter(compiler, 0, 2, 4, 3, 0, 0, 0);
 #else
            sljit_emit_enter(compiler, 0, 2, 3, 3, 0, 0, 0);
 #endif
#else
 #ifdef ARM_PATCH
            sljit_emit_enter(compiler, 0, 1, 4, 3, 0, 0, 0);
 #else
            sljit_emit_enter(compiler, 0, 1, 3, 3, 0, 0, 0);
 #endif
#endif
            if (dataflags) {
                memset(dataflags, 0, sizeof(char) * dataCount);
                for (int i = start; i < end; i++) {
                    OE = &OPT->CODE[i];
                    if (IS_OPERATOR(OE)) {
                        switch (OE->Operator_ID) {
                            case KEY_BY_REF:
                                if (dataflags[OE->OperandLeft_ID - 1])
                                    dataflags[OE->OperandLeft_ID - 1] = 2;
                                else
                                    dataflags[OE->OperandLeft_ID - 1] = 1;
                                dataflags[OE->OperandRight_ID - 1] = 1;
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
                                dataflags[OE->OperandLeft_ID - 1]  = 1;
                                dataflags[OE->OperandRight_ID - 1] = 1;
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
                                dataflags[OE->OperandLeft_ID - 1] = 1;
                                if (usedflags[OE->Result_ID - 1])
                                    dataflags[OE->Result_ID - 1] = 1;
                                break;

                            case KEY_INDEX_OPEN:
                                //left will be checked at runtime
                                dataflags[OE->OperandRight_ID - 1] = 1;
                                if (usedflags[OE->Result_ID - 1])
                                    dataflags[OE->Result_ID - 1] = 1;
                                break;
                        }
                    } else
                    if ((IS_KEYWORD(OE)) && (OE->Operator_ID == KEY_OPTIMIZED_IF))
                        dataflags[OE->OperandRight_ID - 1] = 1;
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
                    /*if (cnt == 1) {
                        if (reg1 != lrptr[idx])
                            reg1 = 0;
                    } else*/
                    reg1 = 0;
                    reg2 = 0;
                    reg3 = 0;
                }

                if (IS_OPERATOR(OE)) {
                    switch (OE->Operator_ID) {
                        case KEY_BY_REF:
                        case KEY_ASG:
                            OPERAND_LEFT
                            if (reg3 == OE->OperandRight_ID) {
                                if (usedflags[OE->Result_ID - 1])
                                    OPERAND_RESULT2

                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA));

                                if (usedflags[OE->Result_ID - 1])
                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA));
                            } else {
                                OPERAND_RIGHT
                                if (usedflags[OE->Result_ID - 1])
                                    OPERAND_RESULT

                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                if (usedflags[OE->Result_ID - 1])
                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));
                            }
                            if (end - i >= 7) {
                                // detect array init
                                if ((OPT->CODE[i + 1].Operator_ID == KEY_LES) && (OPT->CODE[i + 1].OperandLeft_ID == OE->OperandLeft_ID) &&
                                    (OPT->CODE[i + 2].Operator_ID == KEY_OPTIMIZED_IF) && (OPT->CODE[i + 1].Result_ID == OPT->CODE[i + 2].OperandRight_ID) &&
                                    (OPT->CODE[i + 3].Operator_ID == KEY_INDEX_OPEN) && (OPT->CODE[i + 3].OperandRight_ID == OE->OperandLeft_ID) &&
                                    ((OPT->CODE[i + 4].Operator_ID == KEY_ASG) || (OPT->CODE[i + 4].Operator_ID == KEY_BY_REF)) && (OPT->CODE[i + 3].Result_ID == OPT->CODE[i + 4].OperandLeft_ID) && (OE->OperandLeft_ID != OPT->CODE[i + 4].OperandRight_ID) &&
                                    ((OPT->CODE[i + 5].Operator_ID == KEY_INC) || (OPT->CODE[i + 5].Operator_ID == KEY_INC_LEFT) || ((OPT->CODE[i + 5].Operator_ID == KEY_ASU) && (OPT->CODE[i + 5].OperandLeft_ID != OPT->CODE[i + 5].OperandRight_ID))) && (OPT->CODE[i + 5].OperandLeft_ID == OE->OperandLeft_ID) &&
                                    (OPT->CODE[i + 6].Operator_ID == KEY_OPTIMIZED_GOTO) && (OPT->CODE[i + 6].OperandReserved_ID == i + 1)) {
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

                            sljit_emit_fop2(compiler, SLJIT_ADD_F64,
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                            break;

                        case KEY_DEC:
                            OPERAND_LEFT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            sljit_emit_fop2(compiler, SLJIT_SUB_F64,
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            break;

                        case KEY_INC_LEFT:
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT
                            OPERAND_LEFT

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            sljit_emit_fop2(compiler, SLJIT_ADD_F64,
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);
                            break;

                        case KEY_DEC_LEFT:
                            OPERAND_LEFT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            sljit_emit_fop2(compiler, SLJIT_SUB_F64,
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);

                            break;

                        case KEY_ASU:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                                sljit_emit_fop2(compiler, SLJIT_ADD_F64,
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            break;

                        case KEY_ADI:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            sljit_emit_fop2(compiler, SLJIT_SUB_F64,
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            break;

                        case KEY_AMU:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            sljit_emit_fop2(compiler, SLJIT_MUL_F64,
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            break;

                        case KEY_ADV:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            sljit_emit_fop2(compiler, SLJIT_DIV_F64,
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            if (usedflags[OE->Result_ID - 1])
                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
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
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                            OPERAND_LEFT

                            sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                            break;

                        case KEY_MUL:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                                OPERAND_LEFT
                            if (OE->OperandLeft_ID != OE->OperandRight_ID) {
                                OPERAND_RIGHT

                                sljit_emit_fop2(compiler, SLJIT_MUL_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));
                            } else {
                                sljit_emit_fop2(compiler, SLJIT_MUL_F64,
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

                                sljit_emit_fop2(compiler, SLJIT_DIV_F64,
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
                            if (OE->OperandLeft_ID != OE->OperandRight_ID) {
                                OPERAND_RIGHT

                                sljit_emit_fop2(compiler, SLJIT_ADD_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));
                            } else {
                                sljit_emit_fop2(compiler, SLJIT_ADD_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                            }
                            break;

                        case KEY_DIF:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                            if (OE->OperandLeft_ID != OE->OperandRight_ID) {
                                OPERAND_LEFT
                                OPERAND_RIGHT

                                    sljit_emit_fop2(compiler, SLJIT_SUB_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));
                            } else {
                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);
                            }
                            break;

                        case KEY_LES:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved_ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator_ID == KEY_OPTIMIZED_IF) &&
                                (OENext->OperandRight_ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_GREATER_EQUAL_F64,
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

                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_LESS_F64,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_LEQ:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved_ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator_ID == KEY_OPTIMIZED_IF) &&
                                (OENext->OperandRight_ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_GREATER_F64,
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

                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_LESS_EQUAL,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_GRE:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved_ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator_ID == KEY_OPTIMIZED_IF) &&
                                (OENext->OperandRight_ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_LESS_EQUAL_F64,
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

                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_GREATER_F64,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_GEQ:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved_ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator_ID == KEY_OPTIMIZED_IF) &&
                                (OENext->OperandRight_ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_LESS_F64,
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

                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_GREATER_EQUAL_F64,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_EQU:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved_ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator_ID == KEY_OPTIMIZED_IF) &&
                                (OENext->OperandRight_ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_NOT_EQUAL_F64,
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

                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_EQUAL_F64,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_NEQ:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved_ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator_ID == KEY_OPTIMIZED_IF) &&
                                (OENext->OperandRight_ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                OPERAND_RIGHT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_EQUAL_F64,
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

                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_NOT_EQUAL_F64,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_NOT:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            if (i + 1 < end) {
                                OENext = &OPT->CODE[i + 1];
                                cnt    = labels[i - start + 1];
                                icode  = OENext->OperandReserved_ID;
                            } else
                                OENext = 0;

                            if ((OENext) && (OENext->Operator_ID == KEY_OPTIMIZED_IF) &&
                                (OENext->OperandRight_ID == OE->Result_ID) && (usedflags[OE->Result_ID - 1] < 2) &&
                                (icode >= start) && (icode < end) && (!cnt)) {
                                OPERAND_LEFT
                                    jump = sljit_emit_fcmp(compiler, SLJIT_NOT_EQUAL_F64,
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

                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_true);

                                jump = sljit_emit_fcmp(compiler, SLJIT_EQUAL_F64,
                                                       SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM0(), (sljit_sw) & d_false);

                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;

                        case KEY_NEG:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                            OPERAND_LEFT

                            sljit_emit_fop1(compiler, SLJIT_NEG_F64,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                            break;

                        case KEY_CND_NULL:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                            OPERAND_LEFT
                            OPERAND_RIGHT

                            sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));

                            jump = sljit_emit_fcmp(compiler, SLJIT_NOT_EQUAL_F64,
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_false);

                            sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            sljit_set_label(jump, sljit_emit_label(compiler));
                            break;

                        case KEY_REM:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

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
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            JIT_CALL_FUNC(c_modVD2)
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_BOR:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                            OPERAND_LEFT
                            OPERAND_RIGHT

                            sljit_emit_fop2(compiler, SLJIT_ADD_F64,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            jump = sljit_emit_fcmp(compiler, SLJIT_NOT_EQUAL_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_false);

                            jump2 = sljit_emit_jump(compiler, SLJIT_JUMP);
                            sljit_set_label(jump, sljit_emit_label(compiler));

                            sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);

                            sljit_set_label(jump2, sljit_emit_label(compiler));
                            break;

                        case KEY_BAN:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            OPERAND_RESULT
                            OPERAND_LEFT
                            OPERAND_RIGHT

                                sljit_emit_fop2(compiler, SLJIT_MUL_F64,
                                                SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA));

                            jump = sljit_emit_fcmp(compiler, SLJIT_NOT_EQUAL_F64,
                                                    SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM0(), (sljit_sw) & d_false);

                            jump2 = sljit_emit_jump(compiler, SLJIT_JUMP);
                            sljit_set_label(jump, sljit_emit_label(compiler));

                            sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                            SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                            SLJIT_MEM0(), (sljit_sw) & d_true);

                            sljit_set_label(jump2, sljit_emit_label(compiler));
                            break;

                        case KEY_AAN:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            JIT_CALL_FUNC(c_AANVD)
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_AND:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

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
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            JIT_CALL_FUNC(c_AXOVD)
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_AOR:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            JIT_CALL_FUNC(c_AORVD)

                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_ASL:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            JIT_CALL_FUNC(c_ASLVD)
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_ASR:
                            OPERAND_LEFT
                            OPERAND_RIGHT
                            if (usedflags[OE->Result_ID - 1])
                                OPERAND_RESULT

                            JIT_CALL_FUNC(c_ASRVD)
                            reg1 = 0;
                            reg2 = 0;
                            reg3 = 0;
                            break;

                        case KEY_COM:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

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
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

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
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

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
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

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
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

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
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            if ((i + 1 < end) && (usedflags[OE->Result_ID - 1] != 3)) {
                                cnt    = labels[i - start + 1];
                                OENext = &OPT->CODE[i + 1];
                            } else
                                OENext = 0;

                            if ((OENext) && ((OENext->Operator_ID == KEY_ASG) || (OENext->Operator_ID == KEY_BY_REF)) && (OENext->OperandLeft_ID == OE->Result_ID) && (dataflags[OE->Result_ID - 1] != 2) && (!cnt)) {
                                OPERAND_LEFT
                                    OPERAND_RIGHT
                                // result from OENext
                                if (reg3 != OENext->OperandRight_ID) {
                                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OENext->OperandRight_ID - 1));
                                    reg3 = OENext->OperandRight_ID;
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
                                if ((usedflags[OENext->Result_ID - 1]) || (usedflags[OENext->OperandLeft_ID - 1] > 1)) {
                                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OENext->OperandRight_ID - 1));
                                    sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OENext->OperandLeft_ID - 1));
                                    if (usedflags[OENext->Result_ID - 1]) {
                                        sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OENext->Result_ID - 1));

                                        sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                        SLJIT_MEM1(SLJIT_R2), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                        SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                                    }

                                    sljit_emit_fop1(compiler, SLJIT_MOV_F64,
                                                    SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                    SLJIT_MEM1(SLJIT_R0), OFFSETOF(VariableDATA, NUMBER_DATA));
                                }
                            } else
                            if ((OENext) && (OENext->Operator_ID == KEY_OPTIMIZED_IF) && (OENext->OperandRight_ID == OE->Result_ID)) {
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

                                    icode = OENext->OperandReserved_ID;
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
                                sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_TEMPORARY_EREG1, 0);
 #else
                                sljit_emit_ijump(compiler, SLJIT_CALL2, SLJIT_IMM, SLJIT_FUNC_OFFSET(ArrayDataVD));
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

#ifdef EXPERIMENTAL_FEATURES
                        case KEY_SEL:
                            {
                                reg1 = 0;
                                reg2 = 0;
                                reg3 = 0;

                                sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)OPT);
                                sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)OE);
                                sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R2, 0, SLJIT_S1, 0);
#ifdef ARM_PATCH
                                sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_TEMPORARY_EREG1, 0, SLJIT_IMM, SLJIT_FUNC_OFFSET(ClassDataVD));
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_TEMPORARY_EREG1, 0);
#else
                                sljit_emit_ijump(compiler, SLJIT_CALL3, SLJIT_IMM, SLJIT_FUNC_OFFSET(ClassDataVD));
#endif
                                struct sljit_jump *jump = sljit_emit_cmp(compiler, SLJIT_NOT_EQUAL,
                                                                            SLJIT_RETURN_REG, 0,
                                                                            SLJIT_IMM, 0);

                                sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, i + 1);
                                sljit_set_label(jump, sljit_emit_label(compiler));
                            }
                            break;
#endif
                        case KEY_LENGTH:
                            if (!usedflags[OE->Result_ID - 1])
                                continue;

                            sljit_emit_op1(compiler, SLJIT_MOV_P, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(VariableDATA *) * (OE->OperandLeft_ID - 1));
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
                } else {
                    // if (IS_KEYWORD(OE)) {
                    switch (OE->Operator_ID) {
                        case KEY_OPTIMIZED_IF:
                            // recicling result ... it is set to zero
                            OPERAND_RIGHT

                                icode = OE->OperandReserved_ID;
                            if ((icode < start) || (icode >= end)) {
                                jump = sljit_emit_fcmp(compiler, SLJIT_NOT_EQUAL_F64,
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM0(), (sljit_sw) & d_false);
                                sljit_emit_return(compiler, SLJIT_MOV, SLJIT_IMM, icode);
                                sljit_set_label(jump, sljit_emit_label(compiler));
                                outsidejump[icode] = 1;
                            } else {
                                jump = sljit_emit_fcmp(compiler, SLJIT_EQUAL_F64,
                                                       SLJIT_MEM1(SLJIT_R1), OFFSETOF(VariableDATA, NUMBER_DATA),
                                                       SLJIT_MEM0(), (sljit_sw) & d_false);

                                icode_idx = icode - start;
                                if (!jumps[icode_idx])
                                    jumps[icode_idx] = (void **)malloc(sizeof(void *) * labels[icode_idx]);
                                jumps[icode_idx][posj[icode_idx]++] = jump;
                            }
                            break;

                        case KEY_OPTIMIZED_GOTO:
                            icode = OE->OperandReserved_ID;
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
                //free(lrptr);
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
    self->jittracecode = jittracecode;
}
#endif

int ConceptInterpreter_StacklessInterpret(PIFAlizator *PIF, GreenThreadCycle *GREEN) {
    char          *STATIC_ERROR = 0;
    ParamList     *FORMAL_PARAMETERS;
    VariableDATA  *RESULT;
    CompiledClass *CCTEMP;
    ClassCode     *CC;
    void          *instancePTR;
    VariableDATA  *RETURN_DATA;
    signed char    next_is_asg;
    VariableDATA  *THROW_DATA  = 0;
    SCStack       *STACK_TRACE = NULL;

    GreenThreadCycle *TARGET_THREAD = GREEN;
    GreenThreadCycle *PREC_THREAD   = (GreenThreadCycle *)GREEN->LAST_THREAD;

    ParamListExtra FORMAL_PARAMETERS2;

    FORMAL_PARAMETERS2.PIF = PIF;

#ifdef SIMPLE_MULTI_THREADING
    char IsWriteLocked = 0;
#else
    ClassMember *pMEMBER_i;
    int relocation;
    bool not_executed;
#endif

    while (TARGET_THREAD) {
        struct ConceptInterpreter *THIS_REF = (struct ConceptInterpreter *)TARGET_THREAD->INTERPRETER;
        Optimizer          *OPT      = (struct Optimizer *)TARGET_THREAD->OPT;
        int          ClassID         = THIS_REF->LocalClassID;
        VariableDATA **LOCAL_CONTEXT = TARGET_THREAD->LOCAL_CONTEXT;

        INTEGER INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_POINTER;
        INTEGER INSTRUCTION_COUNT   = TARGET_THREAD->INSTRUCTION_COUNT;

        RuntimeOptimizedElement *CODE = OPT->CODE;
        INTEGER CATCH_INSTRUCTION_POINTER      = TARGET_THREAD->CATCH_INSTRUCTION_POINTER;
        INTEGER CATCH_VARIABLE = TARGET_THREAD->CATCH_VARIABLE;
        INTEGER PREVIOUS_TRY   = TARGET_THREAD->PREVIOUS_TRY;
        STACK_TRACE = &TARGET_THREAD->STACK_TRACE;
#ifdef USE_JIT_TRACE
        WRITE_LOCK
        if (!THIS_REF->jittracecode)
            ConceptInterpreter_AnalizeInstructionPath(THIS_REF, OPT);
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
#ifdef EXPERIMENTAL_FEATURES
                        res       = code.func2((sljit_sw)LOCAL_CONTEXT, (sljit_sw)JITDATA);
#else
                        res       = code.func1((sljit_sw)LOCAL_CONTEXT);
#endif
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
            RuntimeOptimizedElement *OE = &CODE [INSTRUCTION_POINTER++];
            OPERATOR_ID_TYPE OE_Operator_ID      = OE->Operator_ID;
            if (IS_OPERATOR(OE)) {
                //WRITE_LOCK
                switch (OE_Operator_ID) {
                    case KEY_ASG:
                        {
                            WRITE_LOCK
                            if ((OE->Operator_FLAGS == MAY_IGNORE_RESULT) && (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE == VARIABLE_NUMBER) && (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE == VARIABLE_NUMBER)) {
                                LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                                PROPERTY_CODE_IGNORE_RESULT(THIS_REF, TARGET_THREAD->PROPERTIES)
                                continue;
                            }                                // ------------------- //
                            if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE != VARIABLE_CLASS) {
                                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                                // ------------------- //
                                // pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                                char asg_type = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                                CHECK_IF_STRING_INDEX_VAR(THIS_REF, LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
                                if ((asg_type == VARIABLE_CLASS) || (asg_type == VARIABLE_ARRAY)) {
                                    int delta = 0;
                                    if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA) {
                                        //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
                                        CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE);
                                    } else {
                                        delta = -1;
                                    }

                                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                                    if (asg_type == VARIABLE_CLASS) {
                                        ((struct CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2 + delta;
                                    } else {
                                        ((struct Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2 + delta;
                                    }
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                                    if ((TARGET_THREAD->PROPERTIES) && (TARGET_THREAD->PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT)) {
                                        CCTEMP = (struct CompiledClass *)((VariableDATA *)(TARGET_THREAD->PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET))->CLASS_DATA;
                                        WRITE_UNLOCK
                                        CCTEMP->_Class->SetProperty(PIF, TARGET_THREAD->PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(TARGET_THREAD->PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET), OE, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, THIS_REF->LocalClassID, &THROW_DATA, STACK_TRACE);
                                        if (THROW_DATA) {
                                            if (ConceptInterpreter_Catch(THIS_REF, THROW_DATA, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {
                                                FREE_VARIABLE(THROW_DATA, STACK_TRACE);
                                                // uncaught exception
                                                PIF->RunTimeError(1300, ERR1300, OE, THIS_REF->OWNER, STACK_TRACE);
                                                INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                            }
                                        }
                                    }
                                    DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                                    continue;
                                }
                            } else {
                                //---------------------------------------------------
                                CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
                                ////////////////////////////////////////////////////////////
                                // pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                                ////////////////////////////////////////////////////////////
                                CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                                if (CCTEMP->_Class->Relocation(DEF_ASG)) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_CLASS;
                                } else {
                                    // ------------------- //
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                                    OE_Operator_ID = KEY_BY_REF;
                                }
                            }
                            if (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_NUMBER) {
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                                LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE   = VARIABLE_NUMBER;
                                PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                continue;
                            }
                        }
                        break;

                    case KEY_SEL:
                        // execute member !!!
                        WRITE_LOCK
                        if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE != VARIABLE_CLASS) {
                            PIF->RunTimeError(240, ERR240, OE, THIS_REF->OWNER, STACK_TRACE);
                            DECLARE_PATH(0x20);
                            continue;
                        }
                        RESET_PROPERTIES(TARGET_THREAD->PROPERTIES, OE->Result_ID);
                        CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;

                        if (OE->OperandReserved_ID) {
                            FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
                            next_is_asg       = 0;
                        } else {
                            next_is_asg       = OE->OperandReserved_TYPE;
                            FORMAL_PARAMETERS = 0;
                        }
                        //-------------------------------------------//
#ifndef SIMPLE_MULTI_THREADING
                        not_executed = true;
                        relocation = CCTEMP->_Class->Relocation(OE->OperandRight_ID - 1);
                        pMEMBER_i = relocation ? CCTEMP->_Class->pMEMBERS [relocation - 1] : 0;
                        if ((pMEMBER_i) && ((CCTEMP->_Class == THIS_REF->OWNER->Defined_In) || (pMEMBER_i->ACCESS == 1))) {
                            if (FORMAL_PARAMETERS) {
                                if (pMEMBER_i->IS_FUNCTION == 1) {
                                    if ((FORMAL_PARAMETERS->COUNT == pMEMBER_i->MUST_PARAMETERS_COUNT)) {
                                        not_executed = false;
                                        WRITE_UNLOCK
                                        RESULT = pMEMBER_i->Execute(PIF, CCTEMP->_Class->CLSID, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], FORMAL_PARAMETERS, LOCAL_CONTEXT, THROW_DATA, STACK_TRACE, LOCAL_CONTEXT [OE->Result_ID - 1], OE->Operator_FLAGS);
                                        WRITE_LOCK
                                    }
                                }
                            } else {
                                if (!pMEMBER_i->IS_FUNCTION) {
                                    // not_executed = false;
                                    relocation = CCTEMP->_Class->RELOCATIONS2 [relocation - 1];
                                    RESULT = CCTEMP->_CONTEXT [relocation - 1];
                                    if (!RESULT)
                                        RESULT = CompiledClass_CreateVariable(CCTEMP, relocation - 1, pMEMBER_i);
                                    goto nothrow;
                                }
                            }
                        }
                        if (not_executed) {
                            WRITE_UNLOCK
                            RESULT = CCTEMP->_Class->ExecuteMember(PIF, OE->OperandRight_ID - 1, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], OE, CCTEMP->_Class->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, THIS_REF->LocalClassID, &THROW_DATA, STACK_TRACE, next_is_asg, &TARGET_THREAD->PROPERTIES, OPT->dataCount, OE->Result_ID - 1, relocation);
                            WRITE_LOCK
                        }
#else
                        WRITE_UNLOCK
                        RESULT = CCTEMP->_Class->ExecuteMember(PIF, OE->OperandRight_ID - 1, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], OE, CCTEMP->_Class->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, THIS_REF->LocalClassID, &THROW_DATA, STACK_TRACE, next_is_asg, &TARGET_THREAD->PROPERTIES, OPT->dataCount, OE->Result_ID - 1);
                        WRITE_LOCK
#endif
                        if (THROW_DATA) {
                            DECLARE_PATH(LAST_THROW->TYPE);
                            if (ConceptInterpreter_Catch(THIS_REF, THROW_DATA, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {
                                WRITE_UNLOCK
                                FREE_VARIABLE(THROW_DATA, STACK_TRACE);
                                // uncaught exception
                                PIF->RunTimeError(1300, ERR1300, OE, THIS_REF->OWNER, STACK_TRACE);
                                INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                break;
                            }
                            WRITE_UNLOCK
                            continue;
                        }
nothrow:
                        //-------------------------------------------//
                        if (RESULT == LOCAL_CONTEXT [OE->Result_ID - 1]) {
                            DECLARE_PATH(RESULT->TYPE);
                            continue;
                        }
                        if (OE->Operator_FLAGS == MAY_IGNORE_RESULT) {
                            if ((RESULT) && (!RESULT->LINKS)) {
                                RESULT->LINKS = 1;
                                FREE_VARIABLE(RESULT, STACK_TRACE);
                            }
                            continue;
                        } else {
                            if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                                WRITE_UNLOCK
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            } else
                            if (RESULT) {
                                FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                                RESULT->LINKS++;
                            }
                        }
                        DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                        continue;

                    case KEY_NEW:
                        //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        CLASS_CHECK(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                        if (OE->OperandLeft_ID == STATIC_CLASS_ARRAY) {
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_NUMBER;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = new_Array(PIF);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                            DECLARE_PATH(VARIABLE_ARRAY);
                        } else {
                            CC = PIF->StaticClassList[OE->OperandLeft_ID - 1];
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_CLASS;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = NULL;
                            FORMAL_PARAMETERS = 0;
                            if (OE->OperandReserved_ID) {
                                FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
                            }
                            WRITE_UNLOCK;
                            instancePTR = CC->CreateInstance(PIF, LOCAL_CONTEXT [OE->Result_ID - 1], OE,FORMAL_PARAMETERS, LOCAL_CONTEXT, STACK_TRACE);

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
                        if (OE->OperandReserved_ID) {
                            FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
                        }
                        if (OE->OperandLeft_ID == STATIC_CLASS_DELEGATE) {
                            // call to delegate function
                            VariableDATA *lOwner = 0;
                            //-------------------------------------------//
                            if (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE != VARIABLE_DELEGATE) {
                                PIF->RunTimeError(970, ERR970, OE, THIS_REF->OWNER, STACK_TRACE);
                                DECLARE_PATH(0x20);
                                continue;
                            }
                            CCTEMP                     = (struct CompiledClass *)delegate_Class(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                            lOwner                     = (VariableDATA *)VAR_ALLOC(PIF);
                            lOwner->CLASS_DATA         = CCTEMP;
                            lOwner->IS_PROPERTY_RESULT = 0;
                            lOwner->LINKS              = 1;
                            lOwner->TYPE               = VARIABLE_CLASS;
                            CCTEMP->LINKS++;

                            WRITE_UNLOCK
                            RESULT = CCTEMP->_Class->ExecuteDelegate(PIF,
                                                                    (INTEGER)delegate_Member(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA),
                                                                    lOwner,
                                                                    OE,
                                                                    FORMAL_PARAMETERS,
                                                                    LOCAL_CONTEXT,
                                                                    ClassID,
                                                                    THIS_REF->LocalClassID,
                                                                    &THROW_DATA,
                                                                    STACK_TRACE);
                            WRITE_LOCK
                            if (THROW_DATA) {
                                FREE_VARIABLE(lOwner, STACK_TRACE);
                                DECLARE_PATH(LAST_THROW->TYPE);
                                if (ConceptInterpreter_Catch(THIS_REF, THROW_DATA, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {
                                    WRITE_UNLOCK
                                    FREE_VARIABLE(THROW_DATA, STACK_TRACE);
                                    // uncaught exception
                                    PIF->RunTimeError(1300, ERR1300, OE, THIS_REF->OWNER, STACK_TRACE);
                                    INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                    break;
                                }
                                WRITE_UNLOCK
                                continue;
                            }
                            FREE_VARIABLE(lOwner, STACK_TRACE);

                            if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                            if (RESULT == LOCAL_CONTEXT [OE->Result_ID - 1]) {
                                DECLARE_PATH(RESULT->TYPE);
                                continue;
                            }
                            if (RESULT) {
                                FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                                RESULT->LINKS++;
                            }
                            DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                            continue;
                        } else
                        if (OE->OperandLeft_ID == STATIC_CLASS_DLL) {
                            if ((FORMAL_PARAMETERS) && (FORMAL_PARAMETERS->COUNT)) {
                                FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                                FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);
                            } else {
                                FORMAL_PARAMETERS2.COUNT       = 0;
                                FORMAL_PARAMETERS2.PARAM_INDEX = 0;
                            }
                            STACK(STACK_TRACE, OE->Operator_DEBUG_INFO_LINE)
                            WRITE_UNLOCK
                            try {
                                STATIC_ERROR = (char *)((EXTERNAL_CALL)(PIF->StaticLinks [OE->OperandRight_ID - 1]))(&FORMAL_PARAMETERS2, LOCAL_CONTEXT, LOCAL_CONTEXT [OE->Result_ID - 1], SetVariable, GetVariable, PIF->out->sock, PIF->SERVER_PUBLIC_KEY, PIF->SERVER_PRIVATE_KEY, PIF->CLIENT_PUBLIC_KEY, SetClassMember, GetClassMember, Invoke);
                            } catch (...) {
                                PIF->RunTimeError(690, ERR690, OE, THIS_REF->OWNER, STACK_TRACE);
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

                                        CONCEPT_STRING_SET_CSTR(THROW_DATA, STATIC_ERROR);

                                        FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1], STACK_TRACE);
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
                                        PIF->RunTimeError(1300, ERR1300, OE, THIS_REF->OWNER, STACK_TRACE);
                                        INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                        break;
                                    }
                                } else {
                                    DECLARE_PATH(0x20);
                                    PIF->RunTimeError(700, ERR700, OE, THIS_REF->OWNER, STACK_TRACE, STATIC_ERROR);
                                    RESET_VARIABLE(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                                }
                            }
                        } else {
                            CC = PIF->StaticClassList[OE->OperandLeft_ID - 1];
                            WRITE_UNLOCK
                            RESULT = CC->ExecuteMember(PIF, OE->OperandRight_ID - 1, LOCAL_CONTEXT [0], OE, CC->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, THIS_REF->LocalClassID, &THROW_DATA, STACK_TRACE);
                            WRITE_LOCK
                            //-------------------------------------------//
                            if (THROW_DATA) {
                                DECLARE_PATH(LAST_THROW->TYPE);
                                if (ConceptInterpreter_Catch(THIS_REF, THROW_DATA, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {
                                    WRITE_UNLOCK
                                    FREE_VARIABLE(THROW_DATA, STACK_TRACE);
                                    // uncaught exception
                                    PIF->RunTimeError(1300, ERR1300, OE, THIS_REF->OWNER, STACK_TRACE);
                                    INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                                    break;
                                }
                                WRITE_UNLOCK;
                                continue;
                            }
                            if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                            if (RESULT) {
                                FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                                RESULT->LINKS++;
                            } else {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                        }
                        DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                        continue;

                    default:
                        SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
                        SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                        if (OE->OperandLeft_ID) {
                            if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE == VARIABLE_NUMBER) {
                                // optimize frequent used operators
                                CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1]);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                if (OE_Operator_ID == KEY_LES) {
                                    EVAL_NUMBER_EXPRESSION(THIS_REF, <)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_INC_LEFT) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA)++;
                                    PROPERTY_CODE_LEFT(THIS_REF, TARGET_THREAD->PROPERTIES)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_SUM) {
                                    EVAL_NUMBER_EXPRESSION(THIS_REF, +)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_ASU) {
                                    EVAL_NUMBER_EXPRESSION(THIS_REF, += )
                                    PROPERTY_CODE(THIS_REF, TARGET_THREAD->PROPERTIES)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_DIF) {
                                    EVAL_NUMBER_EXPRESSION(THIS_REF, -)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_DEC) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = --(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA);
                                    PROPERTY_CODE_LEFT(THIS_REF, TARGET_THREAD->PROPERTIES)
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_NOT) {
                                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = !LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA;
                                    continue;
                                } else
                                if (OE_Operator_ID == KEY_MUL) {
                                    EVAL_NUMBER_EXPRESSION(THIS_REF, *)
                                    PROPERTY_CODE_LEFT(THIS_REF, TARGET_THREAD->PROPERTIES)
                                    continue;
                                }
                                if (!IS_SIMPLE(OE_Operator_ID))
                                    goto numbereval;
                            } else
                            if ((OE_Operator_ID == KEY_INDEX_OPEN) && (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE == VARIABLE_ARRAY)) {
                                WRITE_LOCK
                                switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                                    case VARIABLE_NUMBER:
                                    case VARIABLE_STRING:
                                        // LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                        RETURN_DATA = Array_Get((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA, LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                                        if (!RETURN_DATA) {
                                            DECLARE_PATH(0x20);
                                            PIF->RunTimeError(1110, ERR1110, OE, THIS_REF->OWNER, STACK_TRACE);
                                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                            continue;
                                        }
                                        if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                            FREE_VARIABLE(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                            LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                            RETURN_DATA->LINKS++;
                                        }
                                        DECLARE_PATH(RETURN_DATA->TYPE);
                                        continue;

                                    default:
                                        DECLARE_PATH(0x20);
                                        // LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                        PIF->RunTimeError(900, ERR900, OE, THIS_REF->OWNER, STACK_TRACE);
                                        continue;
                                }
                                continue;
                            }
                            if (IS_SIMPLE(OE_Operator_ID)) {
#ifdef SIMPLE_MULTI_THREADING
                                int simple_eval = ConceptInterpreter_EvalSimpleExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                                int simple_eval = ConceptInterpreter_EvalSimpleExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                                if (simple_eval == 1)
                                    continue;
                                else
                                if (simple_eval == 2)
                                    break;
                            }

                            CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1]);
                            ////////////////////////////////////////////////////////////
                            //pushed_type = VARIABLE_NUMBER;//LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                            ////////////////////////////////////////////////////////////
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE;
                        } else {
                            CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        }
                        // -------------------- pana aici --------------------------------//
                }

                switch (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE) {
                    case VARIABLE_NUMBER:
numbereval:
#ifdef SIMPLE_MULTI_THREADING
                        if (ConceptInterpreter_EvalNumberExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked))
#else
                        if (ConceptInterpreter_EvalNumberExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY))
#endif
                            continue;
                        INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                        break;

                    case VARIABLE_STRING:
#ifdef SIMPLE_MULTI_THREADING
                        ConceptInterpreter_EvalStringExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                        ConceptInterpreter_EvalStringExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                        continue;

                    case VARIABLE_CLASS:
#ifdef SIMPLE_MULTI_THREADING
                        ConceptInterpreter_EvalClassExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, OE_Operator_ID, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                        ConceptInterpreter_EvalClassExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, OE_Operator_ID, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                        continue;

                    case VARIABLE_ARRAY:
#ifdef SIMPLE_MULTI_THREADING
                        ConceptInterpreter_EvalArrayExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                        ConceptInterpreter_EvalArrayExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                        continue;

                    case VARIABLE_DELEGATE:
#ifdef SIMPLE_MULTI_THREADING
                        ConceptInterpreter_EvalDelegateExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                        ConceptInterpreter_EvalDelegateExpression(THIS_REF, PIF, LOCAL_CONTEXT, OE, TARGET_THREAD->PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                        continue;
                }
            } else {
                switch (OE_Operator_ID) {
                    case KEY_OPTIMIZED_IF:

                        DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE);
                        switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                            case VARIABLE_NUMBER:
                                if (!LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA) {
                                    if ((OE->OperandReserved_ID < INSTRUCTION_POINTER) && (TARGET_THREAD != TARGET_THREAD->NEXT)) {
                                        DO_EXECUTE  = 0;
                                        PREC_THREAD = TARGET_THREAD;
                                        TARGET_THREAD->CATCH_INSTRUCTION_POINTER = CATCH_INSTRUCTION_POINTER;
                                        TARGET_THREAD->CATCH_VARIABLE            = CATCH_VARIABLE;
                                        TARGET_THREAD->PREVIOUS_TRY        = PREVIOUS_TRY;
                                        TARGET_THREAD->INSTRUCTION_POINTER = OE->OperandReserved_ID;
                                        TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;
                                    }
                                    INSTRUCTION_POINTER = OE->OperandReserved_ID;
                                }
                                continue;

                            case VARIABLE_STRING:
                                if (!CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1])) {
                                    if ((OE->OperandReserved_ID < INSTRUCTION_POINTER) && (TARGET_THREAD != TARGET_THREAD->NEXT)) {
                                        DO_EXECUTE  = 0;
                                        PREC_THREAD = TARGET_THREAD;
                                        TARGET_THREAD->CATCH_INSTRUCTION_POINTER = CATCH_INSTRUCTION_POINTER;
                                        TARGET_THREAD->CATCH_VARIABLE            = CATCH_VARIABLE;
                                        TARGET_THREAD->PREVIOUS_TRY        = PREVIOUS_TRY;
                                        TARGET_THREAD->INSTRUCTION_POINTER = OE->OperandReserved_ID;
                                        TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;
                                    }
                                    INSTRUCTION_POINTER = OE->OperandReserved_ID;
                                }
                                continue;

                            case VARIABLE_ARRAY:
                                if (!Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA)) {
                                    if ((OE->OperandReserved_ID < INSTRUCTION_POINTER) && (TARGET_THREAD != TARGET_THREAD->NEXT)) {
                                        DO_EXECUTE  = 0;
                                        PREC_THREAD = TARGET_THREAD;
                                        TARGET_THREAD->CATCH_INSTRUCTION_POINTER = CATCH_INSTRUCTION_POINTER;
                                        TARGET_THREAD->CATCH_VARIABLE            = CATCH_VARIABLE;
                                        TARGET_THREAD->PREVIOUS_TRY        = PREVIOUS_TRY;
                                        TARGET_THREAD->INSTRUCTION_POINTER = OE->OperandReserved_ID;
                                        TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;
                                    }
                                    INSTRUCTION_POINTER = OE->OperandReserved_ID;
                                }
                                continue;
                        }
                        break;

                    case KEY_OPTIMIZED_GOTO:
                        DECLARE_PATH(VARIABLE_NUMBER);
                        if ((OE->OperandReserved_ID < INSTRUCTION_POINTER) && (TARGET_THREAD != TARGET_THREAD->NEXT)) {
                            DO_EXECUTE  = 0;
                            PREC_THREAD = TARGET_THREAD;
                            TARGET_THREAD->CATCH_INSTRUCTION_POINTER = CATCH_INSTRUCTION_POINTER;
                            TARGET_THREAD->CATCH_VARIABLE            = CATCH_VARIABLE;
                            TARGET_THREAD->PREVIOUS_TRY        = PREVIOUS_TRY;
                            TARGET_THREAD->INSTRUCTION_POINTER = OE->OperandReserved_ID;
                            TARGET_THREAD = (GreenThreadCycle *)TARGET_THREAD->NEXT;
                        }
                        INSTRUCTION_POINTER = OE->OperandReserved_ID;
                        continue;

                    case KEY_OPTIMIZED_ECHO:
                        DECLARE_PATH(1);
                        if (PIF->out) {
                            WRITE_LOCK
                            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                                case VARIABLE_STRING:
                                    PIF->out->Print(CONCEPT_C_STRING(LOCAL_CONTEXT [OE->OperandRight_ID - 1]), CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]));
                                    break;

                                case VARIABLE_NUMBER:
                                    PIF->out->Print(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                                    break;

                                case VARIABLE_CLASS:
                                    CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                                    PIF->out->Print(CCTEMP->_Class->NAME);
                                    break;

                                case VARIABLE_ARRAY:
                                    {
                                        struct plainstring *temp = Array_ToString((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                                        if (temp) {
                                            PIF->out->Print(plainstring_c_str(temp), plainstring_len(temp));
                                            plainstring_delete(temp);
                                        }
                                    }
                                    break;

                                case VARIABLE_DELEGATE:
                                    CCTEMP = (struct CompiledClass *)delegate_Class(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                                    PIF->out->Print(CCTEMP->_Class->NAME);
                                    PIF->out->Print("::");
                                    PIF->out->Print(CCTEMP->_Class->pMEMBERS [(INTEGER)delegate_Member(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA) - 1]->NAME);
                                    break;
                            }
                        }
                        continue;

                    case KEY_OPTIMIZED_RETURN:
                        DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE);
                        WRITE_UNLOCK
                        INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                        break;

                    case KEY_OPTIMIZED_THROW:
                        DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE);

                        if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                            LOCAL_CONTEXT [OE->OperandRight_ID - 1]->LINKS++;
                            THROW_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1];

                            FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1], STACK_TRACE);
                            LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                            INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                            THROW_DATA                = 0;
                            CATCH_INSTRUCTION_POINTER = 0;
                            CATCH_VARIABLE            = 0;
                            // check for recursive try catch bug April 16, 2012 //
                            if (PREVIOUS_TRY)
                                PREVIOUS_TRY = CODE [PREVIOUS_TRY - 1].OperandLeft_ID;
                            // end of check for recursive try catch bug         //
                            //------------------//
                            RESTORE_TRY_DATA(THIS_REF);
                            //------------------//
                        } else {
                            WRITE_UNLOCK
                            PIF->RunTimeError(1300, ERR1300, OE, THIS_REF->OWNER, STACK_TRACE);
                            INSTRUCTION_POINTER = TARGET_THREAD->INSTRUCTION_COUNT;
                        }
                        continue;

                    case KEY_OPTIMIZED_TRY_CATCH:
                        DECLARE_PATH(1);
                        CATCH_INSTRUCTION_POINTER = OE->OperandReserved_ID;
                        CATCH_VARIABLE            = OE->Result_ID;
                        PREVIOUS_TRY = OE->OperandLeft_ID;
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
                            if (PIF->DEBUGGER_TRAP(OPT->DATA, LOCAL_CONTEXT, OPT->dataCount, OE->Operator_DEBUG_INFO_LINE, ((ClassCode *)(THIS_REF->OWNER->Defined_In))->_DEBUG_INFO_FILENAME.c_str(), (GET_VARIABLE_PROC)GetVariableByName, PIF->DEBUGGER_RESERVED, PIF, STACK_TRACE)) {
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
                ConceptInterpreter_DestroyEnviroment((struct ConceptInterpreter *)TARGET_THREAD->INTERPRETER, PIF, TARGET_THREAD->LOCAL_CONTEXT, &TARGET_THREAD->STACK_TRACE);
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

int ConceptInterpreter_Catch(struct ConceptInterpreter *self, VariableDATA *&THROW_DATA, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY) {
    if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
        struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
        FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1], NULL);
        LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
        INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
        THROW_DATA                = 0;
        CATCH_INSTRUCTION_POINTER = 0;
        CATCH_VARIABLE            = 0;
        RESTORE_TRY_DATA(self);
        return 0;
    }
    return 1;
}

#ifdef SIMPLE_MULTI_THREADING
int ConceptInterpreter_EvalClassExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, OPERATOR_ID_TYPE OE_Operator_ID, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked) {
#else
int ConceptInterpreter_EvalClassExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, OPERATOR_ID_TYPE OE_Operator_ID, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY) {
#endif
    CompiledClass  *CCTEMP;
    DECLARE_PATH(VARIABLE_NUMBER);
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
    INTEGER OPERATOR_ID;
    ParamList OPERATOR_PARAM;
    INTEGER   PARAM_INDEX[1];
    VariableDATA *RESULT;

    switch (OE_Operator_ID) {
        case KEY_BY_REF:
            //---------------------------//
            if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA) {
                CLASS_CHECK_TS(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            } else {
                if (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA) {
                    ((struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA)->LINKS -= 2;
                } else {
                    // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                    ((struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA)->LINKS -= 1;
                }
            }
            //---------------------------//

            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
            ((struct CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2;

            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_CLASS;
            if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT)) {
                CCTEMP = (struct CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET))->CLASS_DATA;
                WRITE_UNLOCK
                CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET), OE, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, self->LocalClassID, &THROW_DATA, STACK_TRACE);
                if (THROW_DATA)
                    return 0;
            }
            DECLARE_PATH(VARIABLE_CLASS);
            return 1;

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
            CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
            if (!CCTEMP->_Class->Relocation(DEF_EQU)) {
                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)

                switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                    case VARIABLE_CLASS:
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA ? 1 : 0;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                        break;

                    default:
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                }
                DECLARE_PATH(VARIABLE_NUMBER);
                return 1;
            }
            OPERATOR_ID = DEF_EQU;
            break;

        case KEY_NEQ:
            CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
            if (!CCTEMP->_Class->Relocation(DEF_NEQ)) {
                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)

                switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                    case VARIABLE_CLASS:
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA ? 1 : 0;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                        break;

                    default:
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                }
                DECLARE_PATH(VARIABLE_NUMBER);
                return 1;
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
            CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
            if (!CCTEMP->_Class->Relocation(DEF_BAN)) {
                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                    case VARIABLE_NUMBER:
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA != 0;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                        break;

                    case VARIABLE_STRING:
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]) != 0;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                        break;

                    case VARIABLE_CLASS:
                    case VARIABLE_DELEGATE:
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                        break;

                    case VARIABLE_ARRAY:
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA) != 0;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                        break;
                }
                DECLARE_PATH(VARIABLE_NUMBER);
                return 1;
            }

            OPERATOR_ID = DEF_BAN;
            break;

        case KEY_BOR:
            CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
            if (!CCTEMP->_Class->Relocation(DEF_BOR)) {
                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                DECLARE_PATH(VARIABLE_NUMBER);
                return 1;
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
            CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
            if (!CCTEMP->_Class->Relocation(DEF_NOT)) {
                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                DECLARE_PATH(VARIABLE_NUMBER);
                return 1;
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
            if ((LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_CLASS) && (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA)) {
                DECLARE_PATH(VARIABLE_CLASS);
                return 1;
            }
            //---------------------------//
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
            //---------------------------//
            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
            ((struct CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
            DECLARE_PATH(VARIABLE_CLASS);
            return 1;

        default:
            DECLARE_PATH(0x20);
            PIF->RunTimeError(920, ERR920, OE, self->OWNER, STACK_TRACE);
            break;
    }
    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////
    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
    //////////////////////////////////////////////////////
    if (!OPERATOR_ID) {
        DECLARE_PATH(0x20);
        return 1;
    }
    if (OE->OperandRight_ID) {
        OPERATOR_PARAM.COUNT       = 1;
        PARAM_INDEX[0]             = OE->OperandRight_ID;
        OPERATOR_PARAM.PARAM_INDEX = DELTA_REF(&OPERATOR_PARAM, PARAM_INDEX);

    } else {
        OPERATOR_PARAM.COUNT = 0;
    }
    CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
    WRITE_UNLOCK
    RESULT = CCTEMP->_Class->ExecuteMember(PIF, OPERATOR_ID, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], OE, CCTEMP->_Class->CLSID == ClassID, &OPERATOR_PARAM, LOCAL_CONTEXT, 0, ClassID, self->LocalClassID, &THROW_DATA, STACK_TRACE);
    WRITE_LOCK
    if (THROW_DATA) {
        DECLARE_PATH(VARIABLE_NUMBER);
        DECLARE_PATH(LAST_THROW->TYPE);
        if (ConceptInterpreter_Catch(self, THROW_DATA, LOCAL_CONTEXT, OE, PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {
            FAST_FREE(PIF, PROPERTIES);
            PROPERTIES = 0;
            WRITE_UNLOCK
            return 0;
        }
        return 1;
    }

    if (RESULT == LOCAL_CONTEXT [OE->Result_ID - 1]) {
        DECLARE_PATH(RESULT->TYPE);
        return 1;
    }
    if (OE->Operator_FLAGS == MAY_IGNORE_RESULT) {
        if (RESULT) {
            if (!RESULT->LINKS)
                RESULT->LINKS = 1;
            FREE_VARIABLE(RESULT, STACK_TRACE);
        }
        return 1;
    }
    if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
        CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
    }
    if (RESULT) {
        FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
        LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
        RESULT->LINKS++;
    } else {
        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
        DECLARE_PATH(0x20);
    }
    return 1;
}

#ifdef SIMPLE_MULTI_THREADING
int ConceptInterpreter_EvalArrayExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked) {
#else
int ConceptInterpreter_EvalArrayExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY) {
#endif
    CompiledClass  *CCTEMP;
    DECLARE_PATH(VARIABLE_NUMBER);
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
    VariableDATA *RETURN_DATA;

    switch (OE->Operator_ID) {
        case KEY_ASG:
        case KEY_BY_REF:
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            // next line was commented (?)
            //---------------------------//
            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
            ((struct Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2;
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_ARRAY;
            if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT)) {
                CCTEMP = (struct CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET))->CLASS_DATA;
                WRITE_UNLOCK
                CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET), OE, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, self->LocalClassID, &THROW_DATA, STACK_TRACE);
                if (THROW_DATA)
                    return 0;
            }
            DECLARE_PATH(VARIABLE_ARRAY);
            return 1;

        case KEY_SUM:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                case VARIABLE_NUMBER:
                case VARIABLE_CLASS:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                    RETURN_DATA = Array_NewArray((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA);
                    Array_AddCopy((struct Array *)RETURN_DATA->CLASS_DATA, LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                        FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                        RETURN_DATA->LINKS++;
                    }
                    DECLARE_PATH(RETURN_DATA->TYPE);
                    return 1;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                    RETURN_DATA = Array_NewArray((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA);
                    Array_Concat((struct Array *)RETURN_DATA->CLASS_DATA, (struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                        FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                        RETURN_DATA->LINKS++;
                    }
                    DECLARE_PATH(RETURN_DATA->TYPE);
                    return 1;
            }
            break;

        case KEY_ASU:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                case VARIABLE_NUMBER:
                case VARIABLE_CLASS:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                    RETURN_DATA = Array_AddCopy((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA, LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                        FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                        RETURN_DATA->LINKS++;
                    }
                    DECLARE_PATH(RETURN_DATA->TYPE);
                    return 1;

                case VARIABLE_ARRAY:
                    //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                    // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = Array_Concat((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA, (struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    if (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA) {
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_ARRAY;
                        ((struct Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                        DECLARE_PATH(VARIABLE_ARRAY);
                    } else {
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                        DECLARE_PATH(VARIABLE_NUMBER);
                    }
                    return 1;
            }
            break;

        case KEY_LENGTH:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA);
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_EQU:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            if (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE != VARIABLE_ARRAY) {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
            } else {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
            }
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_NEQ:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            if (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE != VARIABLE_ARRAY) {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
            } else {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
            }
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_BAN:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) && LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) && CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) != 0);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) && Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;
            }
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_BOR:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) || LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) || CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) || Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;
            }
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_NOT:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) == 0;
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_CND_NULL:
            if ((LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_ARRAY) && (LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA == LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) && (Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA))) {
                DECLARE_PATH(VARIABLE_ARRAY);
                return 1;
            }
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
            // ------------------- //
            if (Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA)) {
                LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
                ((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA)->LINKS++;
                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_ARRAY;
                DECLARE_PATH(VARIABLE_ARRAY);
                return 1;
            }
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                case VARIABLE_STRING:
                    NEW_CONCEPT_STRING2(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                    DECLARE_PATH(VARIABLE_STRING);
                    break;

                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = copy_Delegate(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_DELEGATE;
                    DECLARE_PATH(VARIABLE_DELEGATE);
                    break;

                case VARIABLE_CLASS:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                    ((struct CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                    DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                    ((struct Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                    DECLARE_PATH(VARIABLE_ARRAY);
                    break;
            }
            return 1;

        default:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            DECLARE_PATH(0x20);
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
            PIF->RunTimeError(910, ERR910, OE, self->OWNER, STACK_TRACE);
            break;
    }
    return 1;
}

#ifdef SIMPLE_MULTI_THREADING
int ConceptInterpreter_EvalDelegateExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked) {
#else
int ConceptInterpreter_EvalDelegateExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY) {
#endif
    CompiledClass  *CCTEMP;
    DECLARE_PATH(VARIABLE_NUMBER);
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;

    switch (OE->Operator_ID) {

        case KEY_ASG:
        case KEY_BY_REF:
            // ------------------- //
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            // ------------------- //
            //LOCAL_CONTEXT [OE->Result_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->DELEGATE_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->DELEGATE_DATA;
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA = copy_Delegate(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_DELEGATE;
            if (OE->Operator_FLAGS == MAY_IGNORE_RESULT) {
                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                //----------------//
                PROPERTY_CODE_IGNORE_RESULT(self, PROPERTIES)
                //----------------//
            } else {
                LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA    = copy_Delegate(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA);
                //----------------//
                PROPERTY_CODE(self, PROPERTIES)
                //----------------//
            }
            DECLARE_PATH(VARIABLE_DELEGATE);
            return 1;

        case KEY_EQU:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            if (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE != VARIABLE_DELEGATE) {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
            } else {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((delegate_Class(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) == delegate_Class(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA)) && (delegate_Member(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) == delegate_Member(LOCAL_CONTEXT [OE->OperandRight_ID - 1])));
            }
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_NEQ:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            if (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE != VARIABLE_DELEGATE) {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
            } else {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ((delegate_Class(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA) != delegate_Class(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA)) || (delegate_Member(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA)!= delegate_Member(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA)));
            }
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_BAN:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA != 0);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]) != 0);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA) != 0);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;
            }
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_BOR:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
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
            return 1;

        case KEY_NOT:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_CND_NULL:
            if ((LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_DELEGATE) && (delegate_Class(LOCAL_CONTEXT [OE->Result_ID - 1]) == delegate_Class(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])) && (delegate_Member(LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA) == delegate_Member(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA))) {
                DECLARE_PATH(VARIABLE_DELEGATE);
                return 1;
            }
            //---------------------------//
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
            //---------------------------//
            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA  = copy_Delegate(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA);
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        default:
            DECLARE_PATH(0x20);
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type);
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
            PIF->RunTimeError(960, ERR960, OE, self->OWNER, STACK_TRACE);
            break;
    }
    return 1;
}

#ifdef SIMPLE_MULTI_THREADING
int ConceptInterpreter_EvalStringExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked) {
#else
int ConceptInterpreter_EvalStringExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY) {
#endif
    CompiledClass *CCTEMP;
    DECLARE_PATH(VARIABLE_NUMBER);
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
    switch (OE->Operator_ID) {
        // unary operators
        case KEY_NOT:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) == 0);
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_LENGTH:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_VALUE:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING_FLOAT(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_EQU:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING_EQU(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_STRING_EQU_FLOAT(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
                    break;
            }
            return 1;

        case KEY_NEQ:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = !CONCEPT_STRING_EQU(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = !CONCEPT_STRING_EQU_FLOAT(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
                    break;
            }
            return 1;

        case KEY_INC:
            if (CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])) {
                NEW_CONCEPT_STRING_BUFFER(LOCAL_CONTEXT [OE->Result_ID - 1], CONCEPT_C_STRING(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) + 1, CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) - 1);
                CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], LOCAL_CONTEXT [OE->Result_ID - 1]);
                //----------------//
                PROPERTY_CODE(self, PROPERTIES)
                //----------------//
            }
            DECLARE_PATH(VARIABLE_STRING);
            return 1;

        case KEY_INC_LEFT:
            if (CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])) {
                NEW_CONCEPT_STRING2(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
                CONCEPT_STRING_BUFFER(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], CONCEPT_C_STRING(LOCAL_CONTEXT [OE->Result_ID - 1]) + 1, CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) - 1);
                //----------------//
                PROPERTY_CODE_LEFT(self, PROPERTIES)
                //----------------//
            }
            DECLARE_PATH(VARIABLE_STRING);
            return 1;

        // binary ...
        case KEY_SUM:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                    NEW_CONCEPT_SUM(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandLeft_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    DECLARE_PATH(VARIABLE_STRING);
                    break;

                case VARIABLE_NUMBER:
                    NEW_CONCEPT_STRING2(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
                    CONCEPT_STRING_ADD_DOUBLE(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                    DECLARE_PATH(VARIABLE_STRING);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
                    break;
            }
            return 1;

        case KEY_GRE:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                    CONCEPT_STRING_COMPARE(LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], greater, LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                case VARIABLE_NUMBER:
                    CONCEPT_STRING_COMPARE_FLOAT(LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], greater, LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
                    break;
            }
            return 1;

        case KEY_LES:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                    CONCEPT_STRING_COMPARE(LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], less, LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                case VARIABLE_NUMBER:
                    CONCEPT_STRING_COMPARE_FLOAT(LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], less, LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
                    break;
            }
            return 1;

        case KEY_GEQ:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                    CONCEPT_STRING_COMPARE(LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], greaterequal, LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                case VARIABLE_NUMBER:
                    CONCEPT_STRING_COMPARE_FLOAT(LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], greaterequal, LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
                    break;
            }
            return 1;

        case KEY_LEQ:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                    CONCEPT_STRING_COMPARE(LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], lessequal, LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                case VARIABLE_NUMBER:
                    CONCEPT_STRING_COMPARE_FLOAT(LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], lessequal, LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
                    break;
            }
            return 1;

        case KEY_BY_REF:
        case KEY_ASG:
            // ------------------- //
            ////SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            WRITE_LOCK
            CLASS_CHECK_KEEP_EXTRA(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_STRING;
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                    CONCEPT_STRING(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) {
                        if (OE->Operator_FLAGS == MAY_IGNORE_RESULT) {
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                        } else {
                            FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1]);
                            LOCAL_CONTEXT [OE->Result_ID - 1] = LOCAL_CONTEXT [OE->OperandLeft_ID - 1];
                            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->LINKS++;
                        }
                    }
                    //----------------------------------------------------------------------------------------//
                    UPDATE_STRING_VARIABLE_THAT_HAS_INDEX(self, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], PROPERTIES);
                    //----------------------------------------------------------------------------------------//
                    DECLARE_PATH(VARIABLE_STRING);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
                    return 1;
            }
            //----------------//
            PROPERTY_CODE_LEFT(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_ASU:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_STRING:
                    CONCEPT_STRING_ASU(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) {
                        if (OE->Operator_FLAGS == MAY_IGNORE_RESULT) {
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                        } else {
                            FREE_VARIABLE_RESET(LOCAL_CONTEXT [OE->Result_ID - 1]);
                            LOCAL_CONTEXT [OE->Result_ID - 1] = LOCAL_CONTEXT [OE->OperandLeft_ID - 1];
                            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->LINKS++;
                        }
                    }
                    DECLARE_PATH(VARIABLE_STRING);
                    break;

                case VARIABLE_NUMBER:
                    CONCEPT_STRING_ADD_DOUBLE(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                    NEW_CONCEPT_STRING2(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
                    DECLARE_PATH(VARIABLE_STRING);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
                    break;
            }
            //----------------//
            PROPERTY_CODE_LEFT(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_INDEX_OPEN:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    NEW_CONCEPT_STRING_INDEX(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandLeft_ID - 1], (D_LONG_TYPE)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                    ((struct plainstring *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->EXTRA_DATA = INSTRUCTION_POINTER;
                    DECLARE_PATH(VARIABLE_STRING);
                    break;

                case VARIABLE_STRING:
                    NEW_CONCEPT_STRING_INDEX(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandLeft_ID - 1], CONCEPT_STRING_INT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]));
                    ((struct plainstring *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->EXTRA_DATA = INSTRUCTION_POINTER;
                    DECLARE_PATH(VARIABLE_STRING);
                    break;

                default:
                    DECLARE_PATH(0x20);
                    PIF->RunTimeError(640, ERR640, OE, self->OWNER, STACK_TRACE);
                    break;
            }
            break;

        case KEY_BAN:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) && LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) && CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) != 0);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) && Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;
            }
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_BOR:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) || LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) || CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    // always true for class and delegates
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]) || Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    break;
            }
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;

        case KEY_CND_NULL:
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE)
            // ------------------- //
            if (CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])) {
                NEW_CONCEPT_STRING2(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                DECLARE_PATH(VARIABLE_STRING);
                return 1;
            }
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                    DECLARE_PATH(VARIABLE_NUMBER);
                    break;

                case VARIABLE_STRING:
                    NEW_CONCEPT_STRING2(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                    DECLARE_PATH(VARIABLE_STRING);
                    break;

                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = copy_Delegate(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_DELEGATE;
                    DECLARE_PATH(VARIABLE_DELEGATE);
                    break;

                case VARIABLE_CLASS:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                    ((struct CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                    DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                    ((struct Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                    DECLARE_PATH(VARIABLE_ARRAY);
                    break;
            }
            return 1;

        default:
            DECLARE_PATH(0x20);
            PIF->RunTimeError(650, ERR650, OE, self->OWNER, STACK_TRACE);
            break;
    }
    return 1;
}

#ifdef SIMPLE_MULTI_THREADING
int ConceptInterpreter_EvalNumberExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked) {
#else
int ConceptInterpreter_EvalNumberExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY) {
#endif
    CompiledClass  *CCTEMP;
    DECLARE_PATH(VARIABLE_NUMBER);
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
    switch (OE->Operator_ID) {
        // assigment operators
        case KEY_INC:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ++LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_DEC_LEFT:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA)--;
            //----------------//
            PROPERTY_CODE_LEFT(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_EQU:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA == LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA == CONCEPT_STRING_FLOAT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_ARRAY:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                    break;

                default:
                    PIF->RunTimeError(1024, ERR1204, OE, self->OWNER, STACK_TRACE);
            }
            return 1;

        case KEY_NEQ:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA != LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA != CONCEPT_STRING_FLOAT(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_ARRAY:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                    break;

                default:
                    PIF->RunTimeError(1024, ERR1204, OE, self->OWNER, STACK_TRACE);
            }
            return 1;

        case KEY_DIV:
            EVAL_DIVIDE_EXPRESSION(self, /)
            return 1;

        case KEY_REM:
            EVAL_DIVIDE_INTEGER_EXPRESSION(self, %)
            return 1;

        case KEY_ADI:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            EVAL_NUMBER_EXPRESSION(self, -= )
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_AMU:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            EVAL_NUMBER_EXPRESSION(self, *= )
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_ADV:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            EVAL_DIVIDE_EXPRESSION(self, /= )
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_LEQ:
            EVAL_NUMBER_EXPRESSION(self, <=)
            return 1;

        case KEY_GRE:
            EVAL_NUMBER_EXPRESSION(self, >)
            return 1;

        case KEY_GEQ:
            EVAL_NUMBER_EXPRESSION(self, >=)
            return 1;

        case KEY_AND:
            EVAL_INTEGER_EXPRESSION(self, &)
            return 1;

        case KEY_XOR:
            EVAL_INTEGER_EXPRESSION(self, ^)
            return 1;

        case KEY_OR:
            EVAL_INTEGER_EXPRESSION(self, |)
            return 1;

        case KEY_BAN:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA && LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA && CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA != 0);
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA && Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    break;
            }
            return 1;

        case KEY_BOR:
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA || LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA || CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 1;
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA || Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    break;
            }
            return 1;

        case KEY_SHL:
            EVAL_INTEGER_EXPRESSION(self, <<)
            return 1;

        case KEY_SHR:
            EVAL_INTEGER_EXPRESSION(self, >>)
            return 1;

        case KEY_ARE:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            EVAL_ASG_INTEGER_DIVIDE_EXPRESSION(self, %)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_AAN:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            EVAL_ASG_INTEGER_EXPRESSION(self, &)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_AXO:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            EVAL_ASG_INTEGER_EXPRESSION(self, ^)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_AOR:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            EVAL_ASG_INTEGER_EXPRESSION(self, |)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_ASL:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            EVAL_ASG_INTEGER_EXPRESSION(self, <<)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        case KEY_ASR:
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
            EVAL_ASG_INTEGER_EXPRESSION(self, >>)
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
            // ------------------- //
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = VARIABLE_NUMBER;
            //----------------//
            PROPERTY_CODE(self, PROPERTIES)
            //----------------//
            return 1;

        // unary operators
        case KEY_COM:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = ~(unsigned INTEGER)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA;
            return 1;

        case KEY_POZ:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA;
            return 1;

        case KEY_NEG:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = -LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA;
            return 1;

        case KEY_CND_NULL:
            // ------------------- //
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            // CLASS_CHECK_RESET(LOCAL_CONTEXT [OE->Result_ID - 1], pushed_type)
            // ------------------- //
            if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA) {
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA;
                return 1;
            }
            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    NEW_CONCEPT_STRING2(LOCAL_CONTEXT [OE->Result_ID - 1], LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE           = VARIABLE_STRING;
                    break;

                case VARIABLE_DELEGATE:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = copy_Delegate(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_DELEGATE;
                    break;

                case VARIABLE_CLASS:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                    ((struct CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                    break;

                case VARIABLE_ARRAY:
                    LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                    LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE       = VARIABLE_ARRAY;
                    ((struct Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS++;
                    break;
            }
            return 1;

        case KEY_VALUE:
            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA;
            return 1;

        default:
            PIF->RunTimeError(660, ERR660, OE, self->OWNER, STACK_TRACE);
            break;
    }
    return 1;
}


#ifdef SIMPLE_MULTI_THREADING
int ConceptInterpreter_EvalSimpleExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY, char &IsWriteLocked) {
#else
int ConceptInterpreter_EvalSimpleExpression(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, const RuntimeOptimizedElement *OE, VariableDATAPROPERTY * &PROPERTIES, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE, INTEGER &INSTRUCTION_POINTER, INTEGER &CATCH_INSTRUCTION_POINTER, INTEGER &CATCH_VARIABLE, INTEGER &PREVIOUS_TRY) {
#endif
    CompiledClass  *CCTEMP;
    DECLARE_PATH(VARIABLE_NUMBER);
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
    switch (OE->Operator_ID) {
        case KEY_BY_REF:
            WRITE_LOCK
            CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
            if (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE == VARIABLE_NUMBER) {
                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
                LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE   = VARIABLE_NUMBER;
                PROPERTY_CODE(self, PROPERTIES)
                return 1;
            }

            // pushed_type = VARIABLE_NUMBER;
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
            return 2;

        case KEY_TYPE_OF:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            //==================//
            CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1]);
            //==================//
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_STRING;
            switch (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
                    NEW_CONCEPT_STRING_CSTR(LOCAL_CONTEXT [OE->Result_ID - 1], "numeric");
                    break;

                case VARIABLE_STRING:
                    NEW_CONCEPT_STRING_CSTR(LOCAL_CONTEXT [OE->Result_ID - 1], "string");
                    break;

                case VARIABLE_CLASS:
                    NEW_CONCEPT_STRING_CSTR(LOCAL_CONTEXT [OE->Result_ID - 1], "class");
                    break;

                case VARIABLE_ARRAY:
                    NEW_CONCEPT_STRING_CSTR(LOCAL_CONTEXT [OE->Result_ID - 1], "array");
                    break;

                case VARIABLE_DELEGATE:
                    NEW_CONCEPT_STRING_CSTR(LOCAL_CONTEXT [OE->Result_ID - 1], "delegate");
                    break;

                default:
                    NEW_CONCEPT_STRING_CSTR(LOCAL_CONTEXT [OE->Result_ID - 1], "");
            }
            DECLARE_PATH(VARIABLE_STRING);
            return 1;

        case KEY_CLASS_NAME:
            //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
            //==================//
            CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1]);
            //==================//
            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_STRING;
            if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE != VARIABLE_CLASS) {
                NEW_CONCEPT_STRING_CSTR(LOCAL_CONTEXT [OE->Result_ID - 1], "");
            } else {
                CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
                NEW_CONCEPT_STRING_CSTR(LOCAL_CONTEXT [OE->Result_ID - 1], CCTEMP->_Class->NAME.c_str());
            }
            DECLARE_PATH(VARIABLE_STRING);
            return 1;

        case KEY_DELETE:
            //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
            CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE);
            // LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA  = 0;
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE        = VARIABLE_NUMBER;
            LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = 0;
            DECLARE_PATH(VARIABLE_NUMBER);
            return 1;
    }
    return 0;
}

#ifdef USE_JIT_TRACE
int ConceptInterpreter_JIT(INTEGER &INSTRUCTION_POINTER, INTEGER INSTRUCTION_COUNT, void **jittrace, VariableDATA **LOCAL_CONTEXT) {
    if (jittrace[INSTRUCTION_POINTER]) {
        executable_code code;
        int res;
        do {
            code.code = jittrace[INSTRUCTION_POINTER];
            res       = code.func1((sljit_sw)LOCAL_CONTEXT);
            if (res == INSTRUCTION_POINTER)
                break;
            INSTRUCTION_POINTER = res;
        } while ((INSTRUCTION_POINTER < INSTRUCTION_COUNT) && (jittrace[INSTRUCTION_POINTER]));

        if (INSTRUCTION_POINTER >= INSTRUCTION_COUNT)
            return 1;
    }
    return 0;
}
#endif

VariableDATA *ConceptInterpreter_Interpret(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, intptr_t ClassID, VariableDATA *& THROW_DATA, SCStack *STACK_TRACE) {
    Optimizer        *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
    INTEGER INSTRUCTION_POINTER = 0;
    INTEGER INSTRUCTION_COUNT   = OPT->codeCount;
    char             *STATIC_ERROR       = 0;
    ParamList        *FORMAL_PARAMETERS;
    VariableDATA     *RESULT;
    CompiledClass    *CCTEMP;
    ClassCode        *CC;
    void             *instancePTR;
    VariableDATA     *RETURN_DATA;
    signed char      next_is_asg;
    RuntimeOptimizedElement *CODE = OPT->CODE;

#ifdef SIMPLE_MULTI_THREADING
    char IsWriteLocked = 0;
#else
    ClassMember      *pMEMBER_i;
    int              relocation;
    bool             not_executed;
#endif
    INTEGER CATCH_INSTRUCTION_POINTER = 0;
    INTEGER CATCH_VARIABLE            = 0;
    INTEGER PREVIOUS_TRY = 0;

    ParamListExtra FORMAL_PARAMETERS2;
    FORMAL_PARAMETERS2.PIF = PIF;

#ifdef USE_JIT_TRACE
    WRITE_LOCK
    if (self->callcount) {
        if (self->callcount == 1) {
            ConceptInterpreter_AnalizeInstructionPath(self, OPT);
            self->callcount = 2;
        }
    } else
        self->callcount++;
    void **jittrace = self->jittracecode;
    WRITE_UNLOCK;
#ifdef JIT_INLINE
    int res;
    executable_code code;
#endif
#endif
    VariableDATAPROPERTY *PROPERTIES = NULL;
#ifdef PROFILE_HIT_COUNT
    static INTEGER hits[0x1000];
    static INTEGER iterations = 0;
#endif
#ifdef EXPERIMENTAL_FEATURES
    void *JITDATA[4];
    JITDATA[0] = (void *)LOCAL_CONTEXT;
    JITDATA[1] = (void *)STACK_TRACE;
    JITDATA[2] = (void *)&PROPERTIES;
    JITDATA[3] = (void *)ClassID;
#endif
    while (INSTRUCTION_POINTER < INSTRUCTION_COUNT) {
        WRITE_UNLOCK
#ifdef USE_JIT_TRACE
#ifdef JIT_INLINE
        // faster with some compilers/architectures
        if ((jittrace) && (jittrace[INSTRUCTION_POINTER])) {
            do {
                code.code = jittrace[INSTRUCTION_POINTER];
#ifdef EXPERIMENTAL_FEATURES
                res       = code.func2((sljit_sw)LOCAL_CONTEXT, (sljit_sw)JITDATA);
#else
                res       = code.func1((sljit_sw)LOCAL_CONTEXT);
#endif
                if (res == INSTRUCTION_POINTER)
                    break;
                INSTRUCTION_POINTER = res;
            } while ((INSTRUCTION_POINTER < INSTRUCTION_COUNT) && (jittrace[INSTRUCTION_POINTER]));

            if (INSTRUCTION_POINTER >= INSTRUCTION_COUNT)
                break;
        }
#else
        if ((jittrace) && (jittrace[INSTRUCTION_POINTER]) && (JIT(INSTRUCTION_POINTER, INSTRUCTION_COUNT, jittrace, LOCAL_CONTEXT)))
            break;
#endif
#endif
        RuntimeOptimizedElement *OE = &CODE [INSTRUCTION_POINTER++];
        OPERATOR_ID_TYPE OE_Operator_ID      = OE->Operator_ID;
#ifdef PROFILE_HIT_COUNT
        hits[OE_Operator_ID]++;
        iterations++;
        if (iterations % 1000000 == 0) {
            for (int i = 0; i <= 0xFF7; i++) {
                if (hits[i]) {
                    fprintf(stderr, "%03x => %16ik (%s)\n", i, hits[i]/1000, GetKeyWord(i));
                }
            }
            fprintf(stderr, "========================\n");
        }
#endif
        switch (OE_Operator_ID) {
            case KEY_OPTIMIZED_IF:

                DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE);
                switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                    case VARIABLE_NUMBER:
                        if (!LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA) {
                            INSTRUCTION_POINTER = OE->OperandReserved_ID;
                        }
                        continue;

                    case VARIABLE_STRING:
                        if (!CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1])) {
                            INSTRUCTION_POINTER = OE->OperandReserved_ID;
                        }
                        continue;

                    case VARIABLE_ARRAY:
                        if (!Array_Count((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA)) {
                            INSTRUCTION_POINTER = OE->OperandReserved_ID;
                        }
                        continue;
                }
                continue;

            case KEY_OPTIMIZED_GOTO:
                DECLARE_PATH(VARIABLE_NUMBER);
                INSTRUCTION_POINTER = OE->OperandReserved_ID;
                continue;

            case KEY_ASG:
                {
                    WRITE_LOCK
                    if ((OE->Operator_FLAGS == MAY_IGNORE_RESULT) && (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE == VARIABLE_NUMBER) && (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE == VARIABLE_NUMBER)) {
                        LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                        PROPERTY_CODE_IGNORE_RESULT(self, PROPERTIES)
                        continue;
                    }
                    if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE != VARIABLE_CLASS) {
                        // ------------------- //
                        //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                        // ------------------- //
                        // pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                        char asg_type = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                        CHECK_IF_STRING_INDEX_VAR(self, LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
                        if ((asg_type == VARIABLE_CLASS) || (asg_type == VARIABLE_ARRAY)) {
                            int delta = 0;
                            if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA != LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA) {
                                //SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]);
                                CLASS_CHECK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE);
                            } else {
                                delta = -1;
                            }

                            LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                            if (asg_type == VARIABLE_CLASS) {
                                ((struct CompiledClass *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2 + delta;
                            } else {
                                ((struct Array *)LOCAL_CONTEXT [OE->Result_ID - 1]->CLASS_DATA)->LINKS += 2 + delta;
                            }
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                            if ((PROPERTIES) && (PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT)) {
                                CCTEMP = (struct CompiledClass *)((VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET))->CLASS_DATA;
                                WRITE_UNLOCK
                                CCTEMP->_Class->SetProperty(PIF, PROPERTIES [OE->OperandLeft_ID - 1].IS_PROPERTY_RESULT - 1, (VariableDATA *)(PROPERTIES [OE->OperandLeft_ID - 1].CALL_SET), OE, CCTEMP->_Class->CLSID == ClassID, OE->Result_ID - 1, LOCAL_CONTEXT, ClassID, self->LocalClassID, &THROW_DATA, STACK_TRACE);
                                if (THROW_DATA) {
                                    FAST_FREE(PIF, PROPERTIES);
                                    PROPERTIES = 0;
                                    WRITE_UNLOCK
                                    return 0;
                                }
                            }
                            DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                            continue;
                        }
                    } else {
                        //---------------------------------------------------
                        CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;
                        ////////////////////////////////////////////////////////////
                        // pushed_type = LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                        ////////////////////////////////////////////////////////////
                        CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1])
                        if (CCTEMP->_Class->Relocation(DEF_ASG)) {
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_CLASS;
                        } else {
                            // ------------------- //
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                            OE_Operator_ID = KEY_BY_REF;
                        }
                    }
                    if (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE == VARIABLE_NUMBER) {
                        CLASS_CHECK_TS(LOCAL_CONTEXT [OE->OperandLeft_ID - 1], STACK_TRACE)
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                        LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE   = VARIABLE_NUMBER;
                        PROPERTY_CODE(self, PROPERTIES)
                        continue;
                    }
                }
                goto fallbacklabel;

            case KEY_SEL:
                // execute member !!!
                WRITE_LOCK
                if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE != VARIABLE_CLASS) {
                    PIF->RunTimeError(240, ERR240, OE, self->OWNER, STACK_TRACE);
                    DECLARE_PATH(0x20);
                    continue;
                }
                RESET_PROPERTIES(PROPERTIES, OE->Result_ID);
                CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA;

                if (OE->OperandReserved_ID) {
                    FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
                    next_is_asg       = 0;
                } else {
                    next_is_asg       = OE->OperandReserved_TYPE;
                    FORMAL_PARAMETERS = 0;
                }
                //-------------------------------------------//
#ifndef SIMPLE_MULTI_THREADING
                not_executed = true;
                relocation = CCTEMP->_Class->Relocation(OE->OperandRight_ID - 1);
                pMEMBER_i = relocation ? CCTEMP->_Class->pMEMBERS [relocation - 1] : 0;
                if ((pMEMBER_i) && ((CCTEMP->_Class == self->OWNER->Defined_In) || (pMEMBER_i->ACCESS == 1))) {
                    if (FORMAL_PARAMETERS) {
                        if (pMEMBER_i->IS_FUNCTION == 1) {
                            if ((FORMAL_PARAMETERS->COUNT == pMEMBER_i->MUST_PARAMETERS_COUNT)) {
                                not_executed = false;
                                WRITE_UNLOCK
                                RESULT = pMEMBER_i->Execute(PIF, CCTEMP->_Class->CLSID, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], FORMAL_PARAMETERS, LOCAL_CONTEXT, THROW_DATA, STACK_TRACE, LOCAL_CONTEXT [OE->Result_ID - 1], OE->Operator_FLAGS);
                                WRITE_LOCK
                            }
                        }
                    } else {
                        if (!pMEMBER_i->IS_FUNCTION) {
                            // not_executed = false;
                            relocation = CCTEMP->_Class->RELOCATIONS2 [relocation - 1];
                            RESULT = CCTEMP->_CONTEXT [relocation - 1];
                            if (!RESULT)
                                RESULT = CompiledClass_CreateVariable(CCTEMP, relocation - 1, pMEMBER_i);
                            goto nothrow;
                        }
                    }
                }
                if (not_executed) {
                    WRITE_UNLOCK
                    RESULT = CCTEMP->_Class->ExecuteMember(PIF, OE->OperandRight_ID - 1, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], OE, CCTEMP->_Class->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, self->LocalClassID, &THROW_DATA, STACK_TRACE, next_is_asg, &PROPERTIES, OPT->dataCount, OE->Result_ID - 1, relocation);
                    WRITE_LOCK
                }
#else
                WRITE_UNLOCK
                RESULT = CCTEMP->_Class->ExecuteMember(PIF, OE->OperandRight_ID - 1, LOCAL_CONTEXT [OE->OperandLeft_ID - 1], OE, CCTEMP->_Class->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, self->LocalClassID, &THROW_DATA, STACK_TRACE, next_is_asg, &PROPERTIES, OPT->dataCount, OE->Result_ID - 1);
                WRITE_LOCK
#endif
                if (THROW_DATA) {
                    DECLARE_PATH(THROW_DATA->TYPE);
                    if (ConceptInterpreter_Catch(self, THROW_DATA, LOCAL_CONTEXT, OE, PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {
                        FAST_FREE(PIF, PROPERTIES);
                        PROPERTIES = 0;
                        WRITE_UNLOCK
                        return 0;
                    }
                    continue;
                }
                //-------------------------------------------//
nothrow:
                if (RESULT == LOCAL_CONTEXT [OE->Result_ID - 1]) {
                    DECLARE_PATH(RESULT->TYPE);
                    continue;
                }
                if (OE->Operator_FLAGS == MAY_IGNORE_RESULT) {
                    if ((RESULT) && (!RESULT->LINKS)) {
                        RESULT->LINKS = 1;
                        FREE_VARIABLE(RESULT, STACK_TRACE);
                    }
                    continue;
                } else {
                    if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                        WRITE_UNLOCK
                        CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                        LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                    } else
                    if (RESULT) {
                        FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                        LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                        RESULT->LINKS++;
                    }
                }
                DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                continue;

            case KEY_NEW:
                //SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1]);
                if ((OE->OperandRight_ID > 0) && (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE != VARIABLE_CLASS) && (!PROPERTIES)) {
                    // skip next instruction (is asg)
                    INSTRUCTION_POINTER++;
                    RESULT = LOCAL_CONTEXT [OE->OperandRight_ID - 1];
                } else
                    RESULT = LOCAL_CONTEXT [OE->Result_ID - 1];
                CLASS_CHECK(RESULT, STACK_TRACE);
                if (OE->OperandLeft_ID == STATIC_CLASS_ARRAY) {
                    RESULT->TYPE       = VARIABLE_NUMBER;
                    RESULT->CLASS_DATA = new_Array(PIF);
                    RESULT->TYPE       = VARIABLE_ARRAY;
                    DECLARE_PATH(VARIABLE_ARRAY);
                } else {
                    CC = PIF->StaticClassList[OE->OperandLeft_ID - 1];
                    RESULT->TYPE = VARIABLE_CLASS;
                    RESULT->CLASS_DATA = NULL;
                    FORMAL_PARAMETERS = 0;
                    if (OE->OperandReserved_ID) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
                    }
                    WRITE_UNLOCK;
                    instancePTR = CC->CreateInstance(PIF, RESULT, OE, FORMAL_PARAMETERS, LOCAL_CONTEXT, STACK_TRACE);

                    RESULT->CLASS_DATA = instancePTR;

                    if (!instancePTR) {
                        RESULT->TYPE = VARIABLE_NUMBER;
                        DECLARE_PATH(VARIABLE_NUMBER);
                    } else {
                        DECLARE_PATH(VARIABLE_CLASS);
                    }
                }
                continue;
        }

        if (IS_OPERATOR(OE)) {
            //WRITE_LOCK
            switch (OE_Operator_ID) {
                case KEY_DLL_CALL:
                    WRITE_LOCK
                    FORMAL_PARAMETERS = 0;
                    STATIC_ERROR = 0;
                    if (OE->OperandReserved_ID) {
                        FORMAL_PARAMETERS = &OPT->PARAMS [OE->OperandReserved_ID - 1];
                    }
                    if (OE->OperandLeft_ID == STATIC_CLASS_DELEGATE) {
                        // call to delegate function
                        VariableDATA *lOwner = 0;
                        //-------------------------------------------//
                        if (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE != VARIABLE_DELEGATE) {
                            PIF->RunTimeError(970, ERR970, OE, self->OWNER, STACK_TRACE);
                            DECLARE_PATH(0x20);
                            continue;
                        }
                        CCTEMP                     = (struct CompiledClass *)delegate_Class(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                        lOwner                     = (VariableDATA *)VAR_ALLOC(PIF);
                        lOwner->CLASS_DATA         = CCTEMP;
                        lOwner->IS_PROPERTY_RESULT = 0;
                        lOwner->LINKS              = 1;
                        lOwner->TYPE               = VARIABLE_CLASS;
                        CCTEMP->LINKS++;

                        WRITE_UNLOCK
                        RESULT = CCTEMP->_Class->ExecuteDelegate(PIF,
                                                                (INTEGER)delegate_Member(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA),
                                                                lOwner,
                                                                OE,
                                                                FORMAL_PARAMETERS,
                                                                LOCAL_CONTEXT,
                                                                ClassID,
                                                                self->LocalClassID,
                                                                &THROW_DATA,
                                                                STACK_TRACE);
                        WRITE_LOCK
                        FREE_VARIABLE(lOwner, STACK_TRACE);
                        if (THROW_DATA) {
                            DECLARE_PATH(THROW_DATA->TYPE);
                            if (ConceptInterpreter_Catch(self, THROW_DATA, LOCAL_CONTEXT, OE, PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {
                                FAST_FREE(PIF, PROPERTIES);
                                PROPERTIES = 0;
                                WRITE_UNLOCK
                                return 0;
                            }
                            continue;
                        }
                        if (OE->Operator_FLAGS == MAY_IGNORE_RESULT) {
                            if ((RESULT) && (RESULT != LOCAL_CONTEXT [OE->Result_ID - 1])) {
                                if (!RESULT->LINKS)
                                    RESULT->LINKS = 1;
                                FREE_VARIABLE(RESULT, STACK_TRACE);
                            }
                            continue;
                        }
                        if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                            CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                        }
                        if (RESULT == LOCAL_CONTEXT [OE->Result_ID - 1]) {
                            DECLARE_PATH(RESULT->TYPE);
                            continue;
                        }
                        if (RESULT) {
                            FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                            LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                            RESULT->LINKS++;
                        }
                        DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                        continue;
                    } else
                    if (OE->OperandLeft_ID == STATIC_CLASS_DLL) {
                        if ((FORMAL_PARAMETERS) && (FORMAL_PARAMETERS->COUNT)) {
                            FORMAL_PARAMETERS2.COUNT       = FORMAL_PARAMETERS->COUNT;
                            FORMAL_PARAMETERS2.PARAM_INDEX = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);
                        } else {
                            FORMAL_PARAMETERS2.COUNT       = 0;
                            FORMAL_PARAMETERS2.PARAM_INDEX = 0;
                        }
                        STACK(STACK_TRACE, OE->Operator_DEBUG_INFO_LINE)
                        WRITE_UNLOCK
                        try {
                            STATIC_ERROR = (char *)((EXTERNAL_CALL)(PIF->StaticLinks [OE->OperandRight_ID - 1]))(&FORMAL_PARAMETERS2, LOCAL_CONTEXT, LOCAL_CONTEXT [OE->Result_ID - 1], SetVariable, GetVariable, PIF->out->sock, PIF->SERVER_PUBLIC_KEY, PIF->SERVER_PRIVATE_KEY, PIF->CLIENT_PUBLIC_KEY, SetClassMember, GetClassMember, Invoke);
                        } catch (...) {
                            PIF->RunTimeError(690, ERR690, OE, self->OWNER, STACK_TRACE);
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

                                CONCEPT_STRING_SET_CSTR(THROW_DATA, STATIC_ERROR);

                                DECLARE_PATH(VARIABLE_STRING);
                                if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                                    FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1], STACK_TRACE);
                                    LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                                    INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                                    THROW_DATA                = 0;
                                    CATCH_INSTRUCTION_POINTER = 0;
                                    CATCH_VARIABLE            = 0;
                                    //--------------//
                                    RESTORE_TRY_DATA(self);
                                    //--------------//
                                } else {
                                    WRITE_UNLOCK
                                    FAST_FREE(PIF, PROPERTIES);
                                    return 0;
                                }
                            } else {
                                DECLARE_PATH(0x20);
                                PIF->RunTimeError(700, ERR700, OE, self->OWNER, STACK_TRACE, STATIC_ERROR);
                                RESET_VARIABLE(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                            }
                        }
                    } else {
                        CC = PIF->StaticClassList[OE->OperandLeft_ID - 1];
                        WRITE_UNLOCK
                        RESULT = CC->ExecuteMember(PIF, OE->OperandRight_ID - 1, LOCAL_CONTEXT [0], OE, CC->CLSID == ClassID, FORMAL_PARAMETERS, LOCAL_CONTEXT, 0, ClassID, self->LocalClassID, &THROW_DATA, STACK_TRACE);
                        WRITE_LOCK
                        //-------------------------------------------//
                        if (THROW_DATA) {
                            DECLARE_PATH(THROW_DATA->TYPE);
                            if (ConceptInterpreter_Catch(self, THROW_DATA, LOCAL_CONTEXT, OE, PROPERTIES, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY)) {
                                FAST_FREE(PIF, PROPERTIES);
                                PROPERTIES = 0;
                                WRITE_UNLOCK
                                return 0;
                            }
                            WRITE_UNLOCK
                            continue;
                        }
                        if (OE->Operator_FLAGS == MAY_IGNORE_RESULT) {
                            if ((RESULT) && (RESULT != LOCAL_CONTEXT [OE->Result_ID - 1])) {
                                if (!RESULT->LINKS)
                                    RESULT->LINKS = 1;
                                FREE_VARIABLE(RESULT, STACK_TRACE);
                            }
                            WRITE_UNLOCK
                            continue;
                        }
                        if (((RESULT) && ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_DELEGATE)) && (!RESULT->CLASS_DATA)) || (!RESULT)) {
                            CLASS_CHECK_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE        = VARIABLE_NUMBER;
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                        }
                        if (RESULT) {
                            FREE_VARIABLE_TS(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                            LOCAL_CONTEXT [OE->Result_ID - 1] = RESULT;
                            RESULT->LINKS++;
                        } else {
                            LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = 0;
                        }
                    }
                    DECLARE_PATH(LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE);
                    continue;

                default:
                    SMART_LOCK(LOCAL_CONTEXT [OE->OperandLeft_ID - 1])
                    SMART_LOCK(LOCAL_CONTEXT [OE->Result_ID - 1])
                    if (OE->OperandLeft_ID) {
                        if (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE == VARIABLE_NUMBER) {
                            // optimize frequent used operators
#ifndef _WIN32
                            // optimize only on 64 bit systems
                            if (OE->Operator_FLAGS == MAY_IGNORE_RESULT) {
                                switch (OE_Operator_ID) {
                                    case KEY_INC_LEFT:
                                    case KEY_INC:
                                        LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA++;
                                        PROPERTY_CODE_LEFT(self, PROPERTIES)
                                        continue;
                                    case KEY_ASU:
                                        EVAL_NUMBER_EXPRESSION2(self, += )
                                        PROPERTY_CODE_IGNORE_RESULT(self, PROPERTIES)
                                        continue;
                                    case KEY_DEC_LEFT:
                                    case KEY_DEC:
                                        LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA--;
                                        PROPERTY_CODE_LEFT(self, PROPERTIES)
                                        continue;
                                }
                            }
#endif
                            CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1]);
                            LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                            if (OE_Operator_ID == KEY_LES) {
                                EVAL_NUMBER_EXPRESSION(self, <)
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_INC_LEFT) {
                                WRITE_LOCK
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA)++;
                                PROPERTY_CODE_LEFT(self, PROPERTIES)
                                WRITE_UNLOCK
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_SUM) {
                                EVAL_NUMBER_EXPRESSION(self, +)
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_ASU) {
                                WRITE_LOCK
                                EVAL_NUMBER_EXPRESSION(self, += )
                                PROPERTY_CODE(self, PROPERTIES)
                                WRITE_UNLOCK
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_DIF) {
                                EVAL_NUMBER_EXPRESSION(self, -)
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_DEC) {
                                WRITE_LOCK
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = --(LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA);
                                PROPERTY_CODE_LEFT(self, PROPERTIES)
                                WRITE_UNLOCK
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_NOT) {
                                LOCAL_CONTEXT [OE->Result_ID - 1]->NUMBER_DATA = !LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->NUMBER_DATA;
                                continue;
                            } else
                            if (OE_Operator_ID == KEY_MUL) {
                                EVAL_NUMBER_EXPRESSION(self, *)
                                PROPERTY_CODE_LEFT(self, PROPERTIES)
                                continue;
                            }
                            if (!IS_SIMPLE(OE_Operator_ID))
                                goto numbereval;
                        } else
                        if ((OE_Operator_ID == KEY_INDEX_OPEN) && (LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE == VARIABLE_ARRAY)) {
                            WRITE_LOCK
                            switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                                case VARIABLE_NUMBER:
#ifdef OPTIMIZE_FAST_ARRAYS
                                    {
                                        int index;
                                        double2int(index, LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                                        if (index >= 0) {
                                            RETURN_DATA = FAST_CHECK((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA, index);
                                            if (!RETURN_DATA)
                                                RETURN_DATA = Array_GetWithCreate((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA, index);
                                            if (RETURN_DATA) {
                                                if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                                    FREE_VARIABLE(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                                    LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                                    RETURN_DATA->LINKS++;
                                                }
                                                continue;
                                            }
                                        }
                                    }
#endif

                                case VARIABLE_STRING:
                                    // LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                    RETURN_DATA = Array_Get((struct Array *)LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->CLASS_DATA, LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                                    if (!RETURN_DATA) {
                                        DECLARE_PATH(0x20);
                                        PIF->RunTimeError(1110, ERR1110, OE, self->OWNER, STACK_TRACE);
                                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = VARIABLE_NUMBER;
                                        continue;
                                    }
                                    if (LOCAL_CONTEXT [OE->Result_ID - 1] != RETURN_DATA) {
                                        FREE_VARIABLE(LOCAL_CONTEXT [OE->Result_ID - 1], STACK_TRACE);
                                        LOCAL_CONTEXT [OE->Result_ID - 1] = RETURN_DATA;
                                        RETURN_DATA->LINKS++;
                                    }
                                    DECLARE_PATH(RETURN_DATA->TYPE);
                                    continue;

                                default:
                                    DECLARE_PATH(0x20);
                                    // LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = pushed_type;
                                    PIF->RunTimeError(900, ERR900, OE, self->OWNER, STACK_TRACE);
                                    continue;
                            }
                            continue;
                        }
                        if (IS_SIMPLE(OE_Operator_ID)) {
#ifdef SIMPLE_MULTI_THREADING
                            int simple_eval = ConceptInterpreter_EvalSimpleExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                            int simple_eval = ConceptInterpreter_EvalSimpleExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                            if (simple_eval == 1)
                                continue;
                            else
                            if (simple_eval == 2)
                                break;
                        }
                        CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1]);
                        ////////////////////////////////////////////////////////////
                        //pushed_type = VARIABLE_NUMBER;//LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE;
                        ////////////////////////////////////////////////////////////
                        LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE = LOCAL_CONTEXT [OE->OperandLeft_ID - 1]->TYPE;
                    } else {
                        CLASS_CHECK_RESULT(LOCAL_CONTEXT [OE->Result_ID - 1]);
                    }
                    // -------------------- pana aici --------------------------------//
            }

fallbacklabel:
            switch (LOCAL_CONTEXT [OE->Result_ID - 1]->TYPE) {
                case VARIABLE_NUMBER:
numbereval:
#ifdef SIMPLE_MULTI_THREADING
                    if (ConceptInterpreter_EvalNumberExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked))
#else
                    if (ConceptInterpreter_EvalNumberExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY))
#endif
                        continue;
                    FAST_FREE(PIF, PROPERTIES);
                    return 0;

                case VARIABLE_STRING:
#ifdef SIMPLE_MULTI_THREADING
                    ConceptInterpreter_EvalStringExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                    ConceptInterpreter_EvalStringExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                    continue;

                case VARIABLE_CLASS:
#ifdef SIMPLE_MULTI_THREADING
                    ConceptInterpreter_EvalClassExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, OE_Operator_ID, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                    ConceptInterpreter_EvalClassExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, OE_Operator_ID, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                    continue;

                case VARIABLE_ARRAY:
#ifdef SIMPLE_MULTI_THREADING
                    ConceptInterpreter_EvalArrayExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                    ConceptInterpreter_EvalArrayExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                    continue;

                case VARIABLE_DELEGATE:
#ifdef SIMPLE_MULTI_THREADING
                    ConceptInterpreter_EvalDelegateExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY, IsWriteLocked);
#else
                    ConceptInterpreter_EvalDelegateExpression(self, PIF, LOCAL_CONTEXT, OE, PROPERTIES, ClassID, THROW_DATA, STACK_TRACE, INSTRUCTION_POINTER, CATCH_INSTRUCTION_POINTER, CATCH_VARIABLE, PREVIOUS_TRY);
#endif
                    continue;
            }
        } else {
            switch (OE_Operator_ID) {
                case KEY_OPTIMIZED_RETURN:
                    DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE);
                    if (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->LINKS == 1) {
                        LOCAL_CONTEXT [OE->OperandRight_ID - 1]->LINKS = 2;
                        LOCAL_CONTEXT [OE->OperandRight_ID - 1]->IS_PROPERTY_RESULT = 0;

                        WRITE_UNLOCK
                        if (PROPERTIES)
                            FAST_FREE(PIF, PROPERTIES);
                        return LOCAL_CONTEXT [OE->OperandRight_ID - 1];
                    } else {
                        if (self->OWNER->IS_OPERATOR) {
                            LOCAL_CONTEXT [OE->OperandRight_ID - 1]->LINKS++;
                            LOCAL_CONTEXT [OE->OperandRight_ID - 1]->IS_PROPERTY_RESULT = 0;
                            WRITE_UNLOCK
                            if (PROPERTIES)
                                FAST_FREE(PIF, PROPERTIES);
                            return LOCAL_CONTEXT [OE->OperandRight_ID - 1];
                        }
                        RETURN_DATA              = (VariableDATA *)VAR_ALLOC(PIF);
                        RETURN_DATA->LINKS       = 1;
                        RETURN_DATA->IS_PROPERTY_RESULT = 0;
                        RETURN_DATA->TYPE        = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE;
                        if (RETURN_DATA->TYPE == VARIABLE_NUMBER) {
                            RETURN_DATA->NUMBER_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA;
                        } else
                        if (RETURN_DATA->TYPE == VARIABLE_STRING) {
                            RETURN_DATA->CLASS_DATA     = 0;
                            CONCEPT_STRING(RETURN_DATA, LOCAL_CONTEXT [OE->OperandRight_ID - 1]);
                        } else
                        if (RETURN_DATA->TYPE == VARIABLE_DELEGATE) {
                            RETURN_DATA->CLASS_DATA = copy_Delegate(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                        } else {
                            RETURN_DATA->CLASS_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                            if (RETURN_DATA->CLASS_DATA) {
                                if (RETURN_DATA->TYPE == VARIABLE_CLASS) {
                                    ((struct CompiledClass *)RETURN_DATA->CLASS_DATA)->LINKS++;
                                } else
                                if (RETURN_DATA->TYPE == VARIABLE_ARRAY) {
                                    ((struct Array *)RETURN_DATA->CLASS_DATA)->LINKS++;
                                }
                            }
                        }
                        WRITE_UNLOCK
                        if (PROPERTIES)
                            FAST_FREE(PIF, PROPERTIES);
                        return RETURN_DATA;
                    }
                    break;

                case KEY_OPTIMIZED_ECHO:
                    DECLARE_PATH(1);
                    if (PIF->out) {
                        WRITE_LOCK
                        switch (LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE) {
                            case VARIABLE_STRING:
                                PIF->out->Print(CONCEPT_C_STRING(LOCAL_CONTEXT [OE->OperandRight_ID - 1]), CONCEPT_C_LENGTH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]));
                                break;

                            case VARIABLE_NUMBER:
                                PIF->out->Print(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->NUMBER_DATA);
                                break;

                            case VARIABLE_CLASS:
                                CCTEMP = (struct CompiledClass *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA;
                                PIF->out->Print(CCTEMP->_Class->NAME);
                                break;

                            case VARIABLE_ARRAY:
                                {
                                    struct plainstring *temp = Array_ToString((struct Array *)LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                                    if (temp) {
                                        PIF->out->Print(plainstring_c_str(temp), plainstring_len(temp));
                                        plainstring_delete(temp);
                                    }
                                }
                                break;

                            case VARIABLE_DELEGATE:
                                CCTEMP = (struct CompiledClass *)delegate_Class(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA);
                                PIF->out->Print(CCTEMP->_Class->NAME);
                                PIF->out->Print("::");
                                PIF->out->Print(CCTEMP->_Class->pMEMBERS [(INTEGER)delegate_Member(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->CLASS_DATA) - 1]->NAME);
                                break;
                        }
                    }
                    continue;

                case KEY_OPTIMIZED_THROW:
                    DECLARE_PATH(LOCAL_CONTEXT [OE->OperandRight_ID - 1]->TYPE);
                    LOCAL_CONTEXT [OE->OperandRight_ID - 1]->LINKS++;
                    THROW_DATA = LOCAL_CONTEXT [OE->OperandRight_ID - 1];
                    THROW_DATA->IS_PROPERTY_RESULT = 0;
                    if ((CATCH_INSTRUCTION_POINTER) && (CATCH_VARIABLE)) {
                        FREE_VARIABLE(LOCAL_CONTEXT [CATCH_VARIABLE - 1], STACK_TRACE);
                        LOCAL_CONTEXT [CATCH_VARIABLE - 1] = THROW_DATA;
                        INSTRUCTION_POINTER       = CATCH_INSTRUCTION_POINTER;
                        THROW_DATA                = 0;
                        CATCH_INSTRUCTION_POINTER = 0;
                        CATCH_VARIABLE            = 0;
                        // check for recursive try catch bug April 16, 2012 //
                        if (PREVIOUS_TRY)
                            PREVIOUS_TRY = CODE [PREVIOUS_TRY - 1].OperandLeft_ID;
                        // end of check for recursive try catch bug         //
                        //------------------//
                        RESTORE_TRY_DATA(self);
                        //------------------//
                    } else {
                        WRITE_UNLOCK
                        if (PROPERTIES)
                            FAST_FREE(PIF, PROPERTIES);
                        return 0;
                    }
                    continue;

                case KEY_OPTIMIZED_TRY_CATCH:
                    DECLARE_PATH(1);
                    CATCH_INSTRUCTION_POINTER = OE->OperandReserved_ID;
                    CATCH_VARIABLE            = OE->Result_ID;
                    PREVIOUS_TRY = OE->OperandLeft_ID;
                    continue;

                case KEY_OPTIMIZED_END_CATCH:
                    DECLARE_PATH(1);
                    CATCH_INSTRUCTION_POINTER = 0;
                    CATCH_VARIABLE            = 0;
                    THROW_DATA = 0;
                    //------------------//
                    RESTORE_TRY_DATA(self);
                    //------------------//
                    continue;

                case KEY_OPTIMIZED_DEBUG_TRAP:
                    DECLARE_PATH(1);
                    if (PIF->DEBUGGER_TRAP) {
                        WRITE_UNLOCK
                        if (PIF->DEBUGGER_TRAP(OPT->DATA, LOCAL_CONTEXT, OPT->dataCount, OE->Operator_DEBUG_INFO_LINE, ((ClassCode *)(self->OWNER->Defined_In))->_DEBUG_INFO_FILENAME.c_str(), (GET_VARIABLE_PROC)GetVariableByName, PIF->DEBUGGER_RESERVED, PIF, STACK_TRACE)) {
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
        FAST_FREE(PIF, PROPERTIES);
    return RETURN_DATA;
}

#ifndef INLINE_PARAMETER_CHECK
void ConceptInterpreter_CheckParameters(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **SenderCTX, const RuntimeVariableDESCRIPTOR *TARGET, const VariableDATA *sndr, SCStack *STACK_TRACE, INTEGER i, bool& can_run) {
    if (sndr->TYPE != -TARGET->TYPE) {
        const VariableDATA *sender = SenderCTX ? SenderCTX [0] : 0;
        if ((sender) && (sender->TYPE == VARIABLE_CLASS) && (sender->CLASS_DATA)) {
            CompiledClass *cc = (struct CompiledClass *)sender->CLASS_DATA;
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(1205, ERR1205, self->OWNER->_DEBUG_STARTLINE, "parameter ", i, cc->_Class->_DEBUG_INFO_FILENAME.c_str(), ((ClassCode *)(self->OWNER->Defined_In))->NAME.c_str(), self->OWNER->NAME));
        } else {
            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(1205, ERR1205, self->OWNER->_DEBUG_STARTLINE, "parameter ", i, ((ClassCode *)(self->OWNER->Defined_In))->_DEBUG_INFO_FILENAME.c_str(), ((ClassCode *)(self->OWNER->Defined_In))->NAME.c_str(), self->OWNER->NAME));
        }
        can_run = false;
    } else
    if (sndr->TYPE == VARIABLE_CLASS) {
        INTEGER CLS_ID = (INTEGER)TARGET->nValue - 1;
        if (CLS_ID >= 0) {
            if ((!sndr->CLASS_DATA) || (!((struct CompiledClass *)sndr->CLASS_DATA)->_Class->Inherits(CLS_ID))) {
                const VariableDATA *sender = SenderCTX [0];
                if ((sender) && (sender->TYPE == VARIABLE_CLASS) && (sender->CLASS_DATA)) {
                    CompiledClass *cc = (struct CompiledClass *)sender->CLASS_DATA;
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(1206, ERR1206, self->OWNER->_DEBUG_STARTLINE, "parameter ", i, cc->_Class->_DEBUG_INFO_FILENAME, ((ClassCode *)(self->OWNER->Defined_In))->NAME, self->OWNER->NAME));
                } else {
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(1206, ERR1206, self->OWNER->_DEBUG_STARTLINE, "parameter ", i, ((ClassCode *)(self->OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(self->OWNER->Defined_In))->NAME, self->OWNER->NAME));
                }
                can_run = false;
            }
        }
    }
}
#endif

VariableDATA **ConceptInterpreter_CreateEnvironment(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA *Sender, const ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, SCStack *STACK_TRACE, bool& can_run) {
    VariableDATA **LOCAL_CONTEXT;
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;

    can_run = true;
    INTEGER ParamCount = FORMAL_PARAM ? FORMAL_PARAM->COUNT : 0;
    INTEGER data_count = OPT->dataCount;

    SCStack *STACK_ROOT = (SCStack *)(STACK_TRACE ? STACK_TRACE->ROOT : NULL);
    if ((STACK_ROOT) && (STACK_ROOT->STACK_CONTEXT) && (STACK_ROOT->stack_pos + data_count < BLOCK_STACK_SIZE)) {
        LOCAL_CONTEXT                 = ((VariableDATA **)STACK_ROOT->STACK_CONTEXT) + STACK_ROOT->stack_pos;
        STACK_ROOT->stack_pos        += data_count;
        STACK_TRACE->alloc_from_stack = 1;
    } else
        LOCAL_CONTEXT = (VariableDATA **)FAST_MALLOC(PIF, sizeof(VariableDATA *) * OPT->dataCount);
#ifdef POOL_BLOCK_ALLOC
 #ifdef POOL_STACK
    if (STACK_TRACE->alloc_from_stack) {
        if (!LOCAL_CONTEXT[0])
            LOCAL_CONTEXT[0] = (VariableDATA *)VAR_ALLOC(PIF);
    } else
 #endif
    BLOCK_VAR_ALLOC(LOCAL_CONTEXT, PIF, data_count, 0);
    VariableDATA *this_ref = LOCAL_CONTEXT[0];
#else
    VariableDATA *this_ref = (VariableDATA *)VAR_ALLOC(PIF);
    LOCAL_CONTEXT [0] = this_ref;
#endif
    this_ref->TYPE               = VARIABLE_CLASS;
    this_ref->CLASS_DATA         = Sender->CLASS_DATA;
    this_ref->LINKS              = 1;
    this_ref->IS_PROPERTY_RESULT = -1;

    CC_WRITE_LOCK(PIF)
    if (this_ref->CLASS_DATA)
        ((struct CompiledClass *)this_ref->CLASS_DATA)->LINKS++;

    INTEGER i;
    RuntimeVariableDESCRIPTOR *DATA = OPT->DATA;
    for (i = 1; i <= ParamCount; i++) {
 #ifdef POOL_STACK
        if ((STACK_TRACE->alloc_from_stack) && (!LOCAL_CONTEXT[i]))
            LOCAL_CONTEXT[i] = (VariableDATA *)VAR_ALLOC(PIF);
 #endif
        RuntimeVariableDESCRIPTOR *TARGET = &DATA [i];
        VariableDATA *sndr = SenderCTX [DELTA_UNREF(FORMAL_PARAM, FORMAL_PARAM->PARAM_INDEX) [i - 1] - 1];
        if (TARGET->TYPE < 0) {
#ifdef INLINE_PARAMETER_CHECK
            // validator !
            if (sndr->TYPE != -TARGET->TYPE) {
                VariableDATA *sender = SenderCTX ? SenderCTX [0] : 0;
                if ((sender) && (sender->TYPE == VARIABLE_CLASS) && (sender->CLASS_DATA)) {
                    CompiledClass *cc = (struct CompiledClass *)sender->CLASS_DATA;
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(1205, ERR1205, self->OWNER->_DEBUG_STARTLINE, "parameter ", i, cc->_Class->_DEBUG_INFO_FILENAME.c_str(), ((ClassCode *)(self->OWNER->Defined_In))->NAME.c_str(), self->OWNER->NAME));
                } else {
                    PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(1205, ERR1205, self->OWNER->_DEBUG_STARTLINE, "parameter ", i, ((ClassCode *)(self->OWNER->Defined_In))->_DEBUG_INFO_FILENAME.c_str(), ((ClassCode *)(self->OWNER->Defined_In))->NAME.c_str(), self->OWNER->NAME));
                }
                can_run = false;
            } else
            if (sndr->TYPE == VARIABLE_CLASS) {
                INTEGER CLS_ID = (INTEGER)TARGET->nValue - 1;
                if (CLS_ID >= 0) {
                    if ((!sndr->CLASS_DATA) || (!((struct CompiledClass *)sndr->CLASS_DATA)->_Class->Inherits(CLS_ID))) {
                        VariableDATA *sender = SenderCTX [0];
                        if ((sender) && (sender->TYPE == VARIABLE_CLASS) && (sender->CLASS_DATA)) {
                            CompiledClass *cc = (struct CompiledClass *)sender->CLASS_DATA;
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(1206, ERR1206, self->OWNER->_DEBUG_STARTLINE, "parameter ", i, cc->_Class->_DEBUG_INFO_FILENAME, ((ClassCode *)(self->OWNER->Defined_In))->NAME, self->OWNER->NAME));
                        } else {
                            PIF->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(1206, ERR1206, self->OWNER->_DEBUG_STARTLINE, "parameter ", i, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(self->OWNER->Defined_In))->NAME, self->OWNER->NAME));
                        }
                        can_run = false;
                    }
                }
            }
#else
            ConceptInterpreter_CheckParameters(self, PIF, SenderCTX, TARGET, sndr, STACK_TRACE, i, can_run);
#endif
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
                CONCEPT_STRING(LOCAL_CONTEXT_i, PARAM);
            } else
            if (PARAM->TYPE == VARIABLE_CLASS) {
                LOCAL_CONTEXT_i->CLASS_DATA = PARAM->CLASS_DATA;
                ((struct CompiledClass *)PARAM->CLASS_DATA)->LINKS++;
            } else
            if (PARAM->TYPE == VARIABLE_DELEGATE) {
                LOCAL_CONTEXT_i->CLASS_DATA = copy_Delegate(PARAM->CLASS_DATA);
            } else
            if (PARAM->TYPE == VARIABLE_ARRAY) {
                LOCAL_CONTEXT_i->CLASS_DATA = PARAM->CLASS_DATA;
                ((struct Array *)PARAM->CLASS_DATA)->LINKS++;
            } else
                LOCAL_CONTEXT_i->NUMBER_DATA = PARAM->NUMBER_DATA;
            LOCAL_CONTEXT_i->IS_PROPERTY_RESULT = 0;
        }
    }
    while (i < data_count) {
#ifdef USE_JIT_TRACE
        if ((self->initcode.code) && (i > self->OWNER->PARAMETERS_COUNT)) {
            self->initcode.func2((sljit_sw)LOCAL_CONTEXT, (sljit_sw)PIF);
            CC_WRITE_UNLOCK(PIF)
            return LOCAL_CONTEXT;
        }
#endif
        // variable descriptor
        RuntimeVariableDESCRIPTOR *TARGET = &DATA [i];
#ifdef POOL_STACK
        if ((STACK_TRACE->alloc_from_stack) && (!LOCAL_CONTEXT[i]))
            LOCAL_CONTEXT[i] = (VariableDATA *)VAR_ALLOC(PIF);
#endif

#ifdef POOL_BLOCK_ALLOC
        VariableDATA *LOCAL_CONTEXT_i = LOCAL_CONTEXT[i];
#else
        VariableDATA *LOCAL_CONTEXT_i = (VariableDATA *)VAR_ALLOC(PIF);
        LOCAL_CONTEXT [i] = LOCAL_CONTEXT_i;
#endif

        i++;
        LOCAL_CONTEXT_i->TYPE = (TARGET->TYPE < 0) ? -TARGET->TYPE : TARGET->TYPE;
        LOCAL_CONTEXT_i->IS_PROPERTY_RESULT = (TARGET->BY_REF == 2) ? -1 : 0;
        LOCAL_CONTEXT_i->LINKS = 1;
        if (LOCAL_CONTEXT_i->TYPE == VARIABLE_NUMBER)
            LOCAL_CONTEXT_i->NUMBER_DATA = TARGET->nValue;
        else
        if (LOCAL_CONTEXT_i->TYPE == VARIABLE_STRING) {
            if (TARGET->value.Length()) {
                NEW_CONCEPT_STRING_BUFFER(LOCAL_CONTEXT_i, TARGET->value.c_str(), TARGET->value.Length());
            } else
                LOCAL_CONTEXT_i->CLASS_DATA = 0;
        } else
        if (LOCAL_CONTEXT_i->TYPE == VARIABLE_ARRAY) {
            LOCAL_CONTEXT_i->CLASS_DATA = NULL;
            // extremly important: AllocArray is called with "skip top" parameter set to true
            // this is in case garbage collector is called, and current stack variables are not yet initialized
            // not to do the same if class objects are allocated here
            LOCAL_CONTEXT_i->CLASS_DATA = new_Array(PIF, true);
        } else
            LOCAL_CONTEXT_i->CLASS_DATA = NULL;
    }
    CC_WRITE_UNLOCK(PIF)
    return LOCAL_CONTEXT;
}

void ConceptInterpreter_DestroyEnviroment(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, SCStack *STACK_TRACE, char static_call_main) {
    if (PIF->DEBUGGER_RESERVED) {
        if (*(intptr_t *)PIF->DEBUGGER_RESERVED == (intptr_t)LOCAL_CONTEXT) {
            *(intptr_t *)PIF->DEBUGGER_RESERVED = -1;
        }
    }
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
    INTEGER i;
    INTEGER data_count = OPT->dataCount;
#ifndef FAST_EXIT_NO_GC_CALL
    if (static_call_main) {
        ConceptInterpreter_DestroyGC(self, PIF, LOCAL_CONTEXT, STACK_TRACE);
    } else
#endif
    {
        CC_WRITE_LOCK(PIF)
        for (i = 0; i < data_count; i++) {
#ifdef SIMPLE_MULTI_THREADING
            VariableDATA *VARIABLE = LOCAL_CONTEXT [i];
            VARIABLE->LINKS--;
            if (!VARIABLE->LINKS) {
                if (VARIABLE->CLASS_DATA) {
                    if (VARIABLE->TYPE == VARIABLE_STRING) {
                        plainstring_delete((struct plainstring *)VARIABLE->CLASS_DATA);
                    } else
                    if (VARIABLE->TYPE == VARIABLE_CLASS) {
                        if (!--((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS) {
                            VARIABLE->TYPE = VARIABLE_NUMBER;
                            CC_WRITE_UNLOCK(PIF)
                            delete_CompiledClass((struct CompiledClass *)VARIABLE->CLASS_DATA);
                            CC_WRITE_LOCK2(PIF)
                        }
                    } else
                    if (VARIABLE->TYPE == VARIABLE_ARRAY) {
                        if (!--((struct Array *)VARIABLE->CLASS_DATA)->LINKS) {
                            VARIABLE->TYPE = VARIABLE_NUMBER;
                            CC_WRITE_UNLOCK(PIF)
                            delete_Array((struct Array *)VARIABLE->CLASS_DATA);
                            CC_WRITE_LOCK2(PIF)
                        }
                    } else
                    if (VARIABLE->TYPE == VARIABLE_DELEGATE) {
                        CC_WRITE_UNLOCK(PIF)
                        delete_Delegate(VARIABLE->CLASS_DATA);
                        CC_WRITE_LOCK2(PIF)
                    }
                }
                VAR_FREE(VARIABLE);
            }
#else
            VariableDATA *LOCAL_CONTEXT_i = LOCAL_CONTEXT [i];
            SCStack *STACK_ROOT = (SCStack *)(STACK_TRACE ? STACK_TRACE->ROOT : NULL);
            if (LOCAL_CONTEXT_i) {
 #ifdef POOL_BLOCK_ALLOC
    #ifdef POOL_STACK
                if (STACK_TRACE->alloc_from_stack) {
                    LOCAL_CONTEXT_i->LINKS--;
                    if (!LOCAL_CONTEXT_i->LINKS) {
                        CLASS_CHECK_TS(LOCAL_CONTEXT_i, STACK_TRACE);
                    } else {
                        LOCAL_CONTEXT[i] = NULL;
                    }
                } else 
    #endif
                {
                    FAST_FREE_VARIABLE2(LOCAL_CONTEXT_i, LOCAL_CONTEXT [i]);
                }
 #else
                FREE_VARIABLE(LOCAL_CONTEXT_i, STACK_TRACE);
 #endif
            }
#endif
        }
        CC_WRITE_UNLOCK(PIF)
#ifdef POOL_BLOCK_ALLOC
    #ifdef POOL_STACK
        if (!STACK_TRACE->alloc_from_stack)
    #endif
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
        FAST_FREE(PIF, LOCAL_CONTEXT);
    }
}

void ConceptInterpreter_DestroyGC(struct ConceptInterpreter *self, PIFAlizator *PIF, VariableDATA **LOCAL_CONTEXT, SCStack *STACK_TRACE) {
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
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
    INTEGER i;
    INTEGER data_count = OPT->dataCount;
    GarbageCollector __gc_obj;
    GarbageCollector __gc_array;
    GarbageCollector __gc_vars;
    VariableDATA *LOCAL_CONTEXT_i;
    char fast_array_clean = 0;
    for (i = 0; i < data_count; i++) {
        LOCAL_CONTEXT_i = LOCAL_CONTEXT [i];
        if (LOCAL_CONTEXT_i) {
            if (LOCAL_CONTEXT_i->CLASS_DATA) {
                if ((LOCAL_CONTEXT_i->TYPE == VARIABLE_CLASS) || (LOCAL_CONTEXT_i->TYPE == VARIABLE_DELEGATE)) {
                    void *CLASS_DATA = LOCAL_CONTEXT_i->CLASS_DATA;
                    if (LOCAL_CONTEXT_i->TYPE == VARIABLE_DELEGATE) {
                        CLASS_DATA = delegate_Class(CLASS_DATA);
                        free_Delegate(LOCAL_CONTEXT_i->CLASS_DATA);
                        LOCAL_CONTEXT_i->TYPE = VARIABLE_NUMBER;
                        LOCAL_CONTEXT_i->NUMBER_DATA = 0;
                    }
                    __gc_obj.Reference(CLASS_DATA);
                    if (!i) {
                        const ClassCode *CC = ((struct CompiledClass *)CLASS_DATA)->_Class;
                        if ((CC) && (!CC->DESTRUCTOR_MEMBER))
                            fast_array_clean = 1;
                    }
                    CompiledClass__GO_GARBAGE((struct CompiledClass *)CLASS_DATA, PIF, &__gc_obj, &__gc_array, &__gc_vars);
                } else
                if (LOCAL_CONTEXT_i->TYPE == VARIABLE_ARRAY) {
                    __gc_array.Reference(LOCAL_CONTEXT_i->CLASS_DATA);
                    Array_GO_GARBAGE((struct Array *)LOCAL_CONTEXT_i->CLASS_DATA, PIF, &__gc_obj, &__gc_array, &__gc_vars, -1, fast_array_clean);
                }
            }
            __gc_vars.Reference(LOCAL_CONTEXT_i);
        }
    }
    if (PIF->var_globals) {
        __gc_array.Reference(PIF->var_globals);
        Array_GO_GARBAGE(PIF->var_globals, PIF, &__gc_obj, &__gc_array, &__gc_vars);
    }
    VARPool *POOL = PIF->POOL;
    while (POOL) {
        for (int i = 0; i < POOL_BLOCK_SIZE; i++) {
            LOCAL_CONTEXT_i = &POOL->POOL[i];
            if ((LOCAL_CONTEXT_i) && (LOCAL_CONTEXT_i->flags >= 0) && (LOCAL_CONTEXT_i->CLASS_DATA)) {
                if ((LOCAL_CONTEXT_i->TYPE == VARIABLE_CLASS) || (LOCAL_CONTEXT_i->TYPE == VARIABLE_DELEGATE)) {
                    void *CLASS_DATA = LOCAL_CONTEXT_i->CLASS_DATA;
                    if (LOCAL_CONTEXT_i->TYPE == VARIABLE_DELEGATE) {
                        CLASS_DATA = delegate_Class(CLASS_DATA);
                        free_Delegate(LOCAL_CONTEXT_i->CLASS_DATA);
                        LOCAL_CONTEXT_i->TYPE = VARIABLE_NUMBER;
                        LOCAL_CONTEXT_i->NUMBER_DATA = 0;
                    }
                    __gc_obj.Reference(CLASS_DATA);
                    CompiledClass__GO_GARBAGE((struct CompiledClass *)CLASS_DATA, PIF, &__gc_obj, &__gc_array, &__gc_vars);
                } else
                if (LOCAL_CONTEXT_i->TYPE == VARIABLE_ARRAY) {
                    __gc_array.Reference(LOCAL_CONTEXT_i->CLASS_DATA);
                    Array_GO_GARBAGE((struct Array *)LOCAL_CONTEXT_i->CLASS_DATA, PIF, &__gc_obj, &__gc_array, &__gc_vars);
                }
            }
        }
        POOL = (VARPool *)POOL->NEXT;
    }
    //======================================================//
    __gc_obj.EndOfExecution_SayBye_Objects();
    __gc_array.EndOfExecution_SayBye_Arrays();
    __gc_vars.EndOfExecution_SayBye_Variables();
}

void delete_ConceptInterpreter(struct ConceptInterpreter *self) {
#ifdef USE_JIT_TRACE
    struct Optimizer *OPT = (struct Optimizer *)self->OWNER->OPTIMIZER;
    if (self->jittracecode) {
        int count = OPT->codeCount;
        for (int i = 0; i < count; i++) {
            if (self->jittracecode[i])
                sljit_free_code(self->jittracecode[i]);
        }
        free(self->jittracecode);
        self->jittracecode = 0;
    }
    if (self->initcode.code) {
        sljit_free_code(self->initcode.code);
        self->initcode.code = 0;
    }
#endif
    free(self);
}
