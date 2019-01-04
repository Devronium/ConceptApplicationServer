#include "ClassMember.h"
#include "Optimizer.h"
#include "ConceptInterpreter.h"
#include "SHManager.h"

//#define EMPIRIC_STACK_CHECK
POOLED_IMPLEMENTATION(ExecutableMember)
POOLED_IMPLEMENTATION(ClassMember)

ClassMember::ClassMember(void *DefinedIn, const char *name, char is_data_only, char _not_binary, char is_unserialize) {
    NAME   = name;
    ACCESS = ACCESS_PUBLIC;

    IS_FUNCTION           = 0;
    IS_STATIC             = 0;
    IS_OPERATOR           = 0;
    MEMBER_SET            = 0;
    MEMBER_GET            = 0;
    OPTIMIZER             = 0;
    PARAMETERS_COUNT      = 0;
    MUST_PARAMETERS_COUNT = 0;

    Defined_In       = DefinedIn;
    _DEBUG_STARTLINE = 0;

    VD = 0;
    if (is_data_only) {
        if (!is_unserialize) {
            VD         = new SmallVariableDESCRIPTOR;
            VD->TYPE   = VARIABLE_NUMBER;
            VD->value  = "0";
            VD->nValue = 0;
        }
        this->CDATA           = 0;
        //VariableDescriptors = 0;
        //PIF_DATA            = 0;
    } else {
        if (_not_binary) {
            CDATA               = new CompilerData();
        } else {
            this->CDATA         = 0;
        }
    }
}

void ClassMember::EnsureVD() {
    if (VD) {
        return;
    }
    VD         = new SmallVariableDESCRIPTOR;
    VD->TYPE   = VARIABLE_NUMBER;
    VD->value  = "0";
    VD->nValue = 0;
}

int ClassMember::Serialize(void *PIF, FILE *out, bool is_lib, int version) {
    concept_fwrite_int(&ACCESS, sizeof(ACCESS), 1, out);
    char is_funtion_or_operator = IS_FUNCTION;
    if (IS_OPERATOR) {
        is_funtion_or_operator = (signed char)-1;
    }

    concept_fwrite_int(&is_funtion_or_operator, sizeof(is_funtion_or_operator), 1, out);
    if (IS_FUNCTION == 2) {
        concept_fwrite_int(&MEMBER_GET, sizeof(MEMBER_GET), 1, out);
    } else
    if (IS_FUNCTION == 3) {
        concept_fwrite_int(&MEMBER_SET, sizeof(MEMBER_SET), 1, out);
        concept_fwrite_int(&MEMBER_GET, sizeof(MEMBER_GET), 1, out);
    } else
    if (IS_FUNCTION == 1) {
        concept_fwrite_int(&PARAMETERS_COUNT, sizeof(PARAMETERS_COUNT), 1, out);
        concept_fwrite_int(&MUST_PARAMETERS_COUNT, sizeof(PARAMETERS_COUNT), 1, out);
        concept_fwrite_int(&IS_STATIC, sizeof(IS_STATIC), 1, out);
    }

    SmallVariableDESCRIPTOR DUMMY;
    DUMMY.nValue = 0;
    DUMMY.TYPE   = VARIABLE_NUMBER;

    SmallVariableDESCRIPTOR *refVD = &DUMMY;
    if (VD) {
        refVD = VD;
    }

    SERIALIZE_SMALL_VAR_DESCRIPTOR(refVD, out);
    if (IS_FUNCTION == 1) {
        Optimizer_Serialize((struct Optimizer *)OPTIMIZER, (PIFAlizator *)PIF, out, (INTEGER)is_lib, version);
    }
    return 0;
}

int ClassMember::ComputeSharedSize(concept_FILE *in, signed char& IS_FUNCTION) {
    int         size = 0;
    signed char ACCESS;
    signed char IS_OPERATOR;

    FREAD_INT_FAIL(&ACCESS, sizeof(ACCESS), 1, in);
    FREAD_INT_FAIL(&IS_FUNCTION, sizeof(IS_FUNCTION), 1, in);
    if (IS_FUNCTION < (signed char)0) {
        IS_OPERATOR = 1;
        IS_FUNCTION = 1;
    }
    if (IS_FUNCTION == 2) {
        SKIP(sizeof(INTEGER), in);
    } else
    if (IS_FUNCTION == 3) {
        // MEMBER_GET
        SKIP(sizeof(INTEGER), in);
        // MEMBER_SET
        SKIP(sizeof(INTEGER), in);
    } else
    if (IS_FUNCTION == 1) {
        //PARAMETERS_COUNT
        SKIP(sizeof(unsigned short), in)
        //MUST_PARAMETERS_COUNT
        SKIP(sizeof(unsigned short), in);
        SKIP(1, in);
        size += sizeof(AnsiList);
    }
    VariableDESCRIPTOR VD;
    UNSERIALIZE_VAR_DESCRIPTOR_SIZE((&VD), in);
    int vd_size = VD.value.Size();
    if (vd_size) {
        size += vd_size + 1;
    }
#ifdef ARM_ADJUST_SIZE
    return ARM_ADJUST_SIZE(size);
#else
    return size;
#endif
}

int ClassMember::Unserialize(concept_FILE *in, AnsiList *ClassList , bool is_lib, int *Relocation, int *ClassNames) {
    concept_fread_int(&ACCESS, sizeof(ACCESS), 1, in);
    concept_fread_int(&IS_FUNCTION, sizeof(IS_FUNCTION), 1, in);
    if (IS_FUNCTION < (signed char)0) {
        IS_OPERATOR = 1;
        IS_FUNCTION = 1;
    }

    if (IS_FUNCTION == 2) {
        concept_fread_int(&MEMBER_GET, sizeof(MEMBER_GET), 1, in);
        if (is_lib) {
            MEMBER_GET = Relocation [MEMBER_GET - 1] + 1;
        }
    } else
    if (IS_FUNCTION == 3) {
        concept_fread_int(&MEMBER_SET, sizeof(MEMBER_SET), 1, in);
        concept_fread_int(&MEMBER_GET, sizeof(MEMBER_GET), 1, in);
        if (is_lib) {
            if (MEMBER_SET) {
                MEMBER_SET = Relocation [MEMBER_SET - 1] + 1;
            }
            if (MEMBER_GET) {
                MEMBER_GET = Relocation [MEMBER_GET - 1] + 1;
            }
        }
    } else
    if (IS_FUNCTION == 1) {
        concept_fread_int(&PARAMETERS_COUNT, sizeof(PARAMETERS_COUNT), 1, in);
        concept_fread_int(&MUST_PARAMETERS_COUNT, sizeof(PARAMETERS_COUNT), 1, in);
        concept_fread_int(&IS_STATIC, sizeof(IS_STATIC), 1, in);
    }

    SmallVariableDESCRIPTOR DUMMY;
    DUMMY.nValue = 0;
    DUMMY.TYPE   = VARIABLE_NUMBER;

    SmallVariableDESCRIPTOR *refVD = &DUMMY;
    if (!IS_FUNCTION) {
        this->EnsureVD();
        refVD = VD;
    } else {
        VD = 0;
    }

    if (!IS_FUNCTION) {
        refVD->value = "";
    }

    UNSERIALIZE_SMALL_VAR_DESCRIPTOR(refVD, in, false);
    if ((refVD->TYPE == TYPE_NUMBER) && (!refVD->nValue) && (VD)) {
        delete VD;
        VD = 0;
    }
    return 0;
}

ClassMember::~ClassMember(void) {
    if (OPTIMIZER) {
        if (MEMBER_GET != -1) {
            delete_Optimizer((struct Optimizer *)OPTIMIZER);
        }
    }

    if (CDATA) {
        delete CDATA;
    }

    if (VD) {
        delete VD;
    }
}

#ifdef EMPIRIC_STACK_CHECK
static int STACK_HIT = 0;
 #define MAX_RECURSIVE_CALL    2100
#endif

void ClassMember::EndMainCall(void *PIF, VariableDATA *&RESULT, VariableDATA *&THROW_DATA, SCStack *PREV, SCStack *STACK_TRACE) {
    if (RESULT) {
        if (RESULT->TYPE == VARIABLE_NUMBER)
            ((PIFAlizator *)PIF)->last_result = (int)RESULT->NUMBER_DATA;

        if (((PIFAlizator *)PIF)->static_result) {
            CLASS_CHECK(((VariableDATA *)((PIFAlizator *)PIF)->static_result), PREV);
            switch (RESULT->TYPE) {
                case VARIABLE_NUMBER:
                    ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE        = VARIABLE_NUMBER;
                    ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->NUMBER_DATA = RESULT->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE           = VARIABLE_STRING;
                    CONCEPT_STRING(((VariableDATA *)((PIFAlizator *)PIF)->static_result), RESULT);
                    break;

                case VARIABLE_ARRAY:
                    ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE           = VARIABLE_STRING;
                    CONCEPT_STRING_SET_CSTR(((VariableDATA *)((PIFAlizator *)PIF)->static_result), "array");
                    break;

                case VARIABLE_DELEGATE:
                    ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE = VARIABLE_STRING;
                    if (RESULT->CLASS_DATA) {
                        CONCEPT_STRING_SET_CSTR(((VariableDATA *)((PIFAlizator *)PIF)->static_result), ((struct CompiledClass *)delegate_Class(RESULT->CLASS_DATA))->_Class->NAME.c_str());
                        CONCEPT_STRING_ADD_CSTR(((VariableDATA *)((PIFAlizator *)PIF)->static_result), "::");
                        CONCEPT_STRING_ADD_CSTR(((VariableDATA *)((PIFAlizator *)PIF)->static_result), ((struct CompiledClass *)delegate_Class(RESULT->CLASS_DATA))->_Class->pMEMBERS [(INTEGER)delegate_Member(RESULT->CLASS_DATA) - 1]->NAME);
                    } else {
                        CONCEPT_STRING_SET_CSTR(((VariableDATA *)((PIFAlizator *)PIF)->static_result), "delegate");
                    }
                    break;

                case VARIABLE_CLASS:
                    ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE = VARIABLE_STRING;
                    if (RESULT->CLASS_DATA) {
                        CONCEPT_STRING_SET_CSTR(((VariableDATA *)((PIFAlizator *)PIF)->static_result), ((struct CompiledClass *)RESULT->CLASS_DATA)->_Class->NAME.c_str());
                    } else {
                        CONCEPT_STRING_SET_CSTR(((VariableDATA *)((PIFAlizator *)PIF)->static_result), "object");
                    }
                    break;
            }
        }
        FREE_VARIABLE(RESULT, PREV);
        RESULT = NULL;
    }
    if (THROW_DATA) {
        const char *data = NULL;
        char *temp = NULL;
        // check if is not the static call to Main. In this case the THROW_DATA is/may be deleted
        if (THROW_DATA->TYPE == VARIABLE_STRING) {
            data = CONCEPT_C_STRING(THROW_DATA);
        } else
        if (THROW_DATA->TYPE == VARIABLE_NUMBER) {
            temp = (char *)FAST_MALLOC(PIF, 0xFF);
            cstr_loaddouble(temp, THROW_DATA->NUMBER_DATA);
            data = temp;
        } else
        if (THROW_DATA->TYPE == VARIABLE_CLASS) {
            data = CompiledClass_GetClassName((struct CompiledClass *)THROW_DATA->CLASS_DATA);
        } else
        if (THROW_DATA->TYPE == VARIABLE_ARRAY) {
            data = "Array";
        } else
        if (THROW_DATA->TYPE == VARIABLE_DELEGATE) {
            const char *class_name = CompiledClass_GetClassName((struct CompiledClass *)delegate_Class(THROW_DATA->CLASS_DATA));
            int len = strlen(class_name);
            temp = (char *)FAST_MALLOC(PIF, len + 13);
            temp[len + 12] = 0;
            memcpy(temp, "Delegate of ", 12);
            memcpy(temp + 12, class_name, len);
            data = temp;
        }

        AnsiException *Exc = new AnsiException(ERR630, 0, 630, data, ((ClassCode *)(this->Defined_In))->_DEBUG_INFO_FILENAME, NAME);
        ((PIFAlizator *)PIF)->AcknoledgeRunTimeError(PREV, Exc);

        FREE_VARIABLE(THROW_DATA, PREV);
        THROW_DATA = NULL;

        FAST_FREE(PIF, temp);
    }
#ifdef POOL_STACK
    if ((!PREV) && (STACK_TRACE->STACK_CONTEXT)) {
        // variables 0 to stack_pos will be cleared by DestroyEnvironment
        for (int i = ((struct Optimizer *)OPTIMIZER)->dataCount; i < BLOCK_STACK_SIZE; i++) {
            VariableDATA *VD = (VariableDATA *)STACK_TRACE->STACK_CONTEXT[i];
            if (VD)
                VAR_FREE(VD);
        }
    }
#endif
}

VariableDATA *ClassMember::Execute(void *PIF, intptr_t CONCEPT_CLASS_ID, struct CompiledClass *Owner, const ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, VariableDATA *& THROW_DATA, SCStack *PREV, VariableDATA *USE_RESULT, INTEGER FLAGS, char is_main THREAD_CREATION_LOCKS) {
#ifdef EMPIRIC_STACK_CHECK
    if (++STACK_HIT > MAX_RECURSIVE_CALL) {
        ((PIFAlizator *)PIF)->Errors.Add(new AnsiException(ERR840, _DEBUG_STARTLINE, 840, NAME, _DEBUG_FILENAME), DATA_EXCEPTION);
        return 0;
    }
#endif

    struct ConceptInterpreter *INTERPRETER = (struct ConceptInterpreter *)((struct Optimizer *)this->OPTIMIZER)->INTERPRETER;
    if (!INTERPRETER) {
        INTERPRETER = new_ConceptInterpreter((struct Optimizer *)OPTIMIZER, ((ClassCode *)Defined_In)->CLSID, this);
        ((struct Optimizer *)this->OPTIMIZER)->INTERPRETER = INTERPRETER;
    }
    SCStack STACK_TRACE;
    STACK_TRACE.CM               = this;
    STACK_TRACE.PREV             = PREV;
    STACK_TRACE.stack_pos        = 0;
    STACK_TRACE.alloc_from_stack = 0;
    STACK_TRACE.len              = 0;
    VariableDATA *vdcontext      = NULL;
    void *PREV_TOP = NULL;
    if (PREV) {
        STACK_TRACE.STACK_CONTEXT    = NULL;
        PREV_TOP                     = ((SCStack *)PREV->ROOT)->TOP;
        STACK_TRACE.ROOT             = PREV->ROOT;
        ((SCStack *)PREV->ROOT)->TOP = &STACK_TRACE;
    } else {
        STACK_TRACE.STACK_CONTEXT = (void **)FAST_MALLOC(PIF, sizeof(VariableDATA *) * BLOCK_STACK_SIZE);
#ifdef POOL_STACK
        memset(STACK_TRACE.STACK_CONTEXT, 0, sizeof(VariableDATA *) * BLOCK_STACK_SIZE);
#endif
        STACK_TRACE.ROOT          = &STACK_TRACE;
    }
    STACK_TRACE.TOP = &STACK_TRACE;

    bool         can_run;
#ifdef NO_FORCED_INLINE_CODE
    VariableDATA **CONTEXT = ConceptInterpreter_CreateEnvironment((struct ConceptInterpreter *)INTERPRETER, (PIFAlizator *)PIF, Owner, FORMAL_PARAM, SenderCTX, &STACK_TRACE, can_run);
#else
    VariableDATA **CONTEXT;
    struct Optimizer *OPT = (struct Optimizer *)this->OPTIMIZER;

    can_run = true;
    INTEGER ParamCount = FORMAL_PARAM ? FORMAL_PARAM->COUNT : 0;
    INTEGER data_count = OPT->dataCount;

    SCStack *STACK_ROOT = (SCStack *)STACK_TRACE.ROOT;
    if ((STACK_ROOT) && (STACK_ROOT->STACK_CONTEXT) && (STACK_ROOT->stack_pos + data_count < BLOCK_STACK_SIZE)) {
        CONTEXT                 = ((VariableDATA **)STACK_ROOT->STACK_CONTEXT) + STACK_ROOT->stack_pos;
        STACK_ROOT->stack_pos        += data_count;
        STACK_TRACE.alloc_from_stack = 1;
    } else
        CONTEXT = (VariableDATA **)FAST_MALLOC(PIF, sizeof(VariableDATA *) * OPT->dataCount);
#ifdef POOL_BLOCK_ALLOC
 #ifdef POOL_STACK
    if (STACK_TRACE.alloc_from_stack) {
        if (!CONTEXT[0])
            CONTEXT[0] = (VariableDATA *)VAR_ALLOC(PIF);
    } else
 #endif
    BLOCK_VAR_ALLOC(CONTEXT, PIF, data_count, 0);
    VariableDATA *this_ref = CONTEXT[0];
#else
    VariableDATA *this_ref = (VariableDATA *)VAR_ALLOC(PIF);
    CONTEXT [0] = this_ref;
#endif
    this_ref->TYPE               = VARIABLE_CLASS;
    this_ref->CLASS_DATA         = Owner;
    this_ref->LINKS              = 1;
    this_ref->IS_PROPERTY_RESULT = -1;

    CC_WRITE_LOCK(((PIFAlizator *)PIF))
    if (this_ref->CLASS_DATA)
        ((struct CompiledClass *)this_ref->CLASS_DATA)->LINKS++;

    INTEGER i;
    RuntimeVariableDESCRIPTOR *DATA = OPT->DATA;
    for (i = 1; i <= ParamCount; i++) {
 #ifdef POOL_STACK
        if ((STACK_TRACE.alloc_from_stack) && (!CONTEXT[i]))
            CONTEXT[i] = (VariableDATA *)VAR_ALLOC(PIF);
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
                    ((PIFAlizator *)PIF)->AcknoledgeRunTimeError(&STACK_TRACE, new AnsiException(1205, ERR1205, this->_DEBUG_STARTLINE, "parameter ", i, cc->_Class->_DEBUG_INFO_FILENAME.c_str(), ((ClassCode *)(this->Defined_In))->NAME.c_str(), this->NAME));
                } else {
                    ((PIFAlizator *)PIF)->AcknoledgeRunTimeError(&STACK_TRACE, new AnsiException(1205, ERR1205, this->_DEBUG_STARTLINE, "parameter ", i, ((ClassCode *)(this->Defined_In))->_DEBUG_INFO_FILENAME.c_str(), ((ClassCode *)(this->Defined_In))->NAME.c_str(), this->NAME));
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
                            ((PIFAlizator *)PIF)->AcknoledgeRunTimeError(&STACK_TRACE, new AnsiException(1206, ERR1206, this->_DEBUG_STARTLINE, "parameter ", i, cc->_Class->_DEBUG_INFO_FILENAME, ((ClassCode *)(this->Defined_In))->NAME, this->NAME));
                        } else {
                            ((PIFAlizator *)PIF)->AcknoledgeRunTimeError(&STACK_TRACE, new AnsiException(1206, ERR1206, this->_DEBUG_STARTLINE, "parameter ", i, ((ClassCode *)(this->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(this->Defined_In))->NAME, this->NAME));
                        }
                        can_run = false;
                    }
                }
            }
#else
            ConceptInterpreter_CheckParameters(INTERPRETER, (PIFAlizator *)PIF, SenderCTX, TARGET, sndr, &STACK_TRACE, i, can_run);
#endif
        }
        // if IS_PROPERTY_RESULT is -1 => is constant !
        if ((TARGET->BY_REF) && (sndr->IS_PROPERTY_RESULT != -1)) {
#ifdef POOL_BLOCK_ALLOC
            VAR_FREE(CONTEXT [i]);
#endif
            CONTEXT [i] = sndr;
            CONTEXT [i]->LINKS++;
        } else {
            VariableDATA *PARAM = sndr;
#ifdef POOL_BLOCK_ALLOC
            VariableDATA *CONTEXT_i = CONTEXT [i];
#else
            VariableDATA *CONTEXT_i = (VariableDATA *)VAR_ALLOC(PIF);
            CONTEXT [i] = CONTEXT_i;
#endif

            CONTEXT_i->TYPE  = PARAM->TYPE;
            CONTEXT_i->LINKS = 1;
            if (PARAM->TYPE == VARIABLE_STRING) {
                CONTEXT_i->CLASS_DATA     = 0;
                CONCEPT_STRING(CONTEXT_i, PARAM);
            } else
            if (PARAM->TYPE == VARIABLE_CLASS) {
                CONTEXT_i->CLASS_DATA = PARAM->CLASS_DATA;
                ((struct CompiledClass *)PARAM->CLASS_DATA)->LINKS++;
            } else
            if (PARAM->TYPE == VARIABLE_DELEGATE) {
                CONTEXT_i->CLASS_DATA = copy_Delegate(PARAM->CLASS_DATA);
            } else
            if (PARAM->TYPE == VARIABLE_ARRAY) {
                CONTEXT_i->CLASS_DATA = PARAM->CLASS_DATA;
                ((struct Array *)PARAM->CLASS_DATA)->LINKS++;
            } else
                CONTEXT_i->NUMBER_DATA = PARAM->NUMBER_DATA;
            CONTEXT_i->IS_PROPERTY_RESULT = 0;
        }
    }
    while (i < data_count) {
#ifdef USE_JIT_TRACE
        if ((INTERPRETER->initcode.code) && (i > this->PARAMETERS_COUNT)) {
            INTERPRETER->initcode.func2((sljit_sw)CONTEXT, (sljit_sw)PIF);
            break;
        }
#endif
        // variable descriptor
        RuntimeVariableDESCRIPTOR *TARGET = &DATA [i];
#ifdef POOL_STACK
        if ((STACK_TRACE.alloc_from_stack) && (!CONTEXT[i]))
            CONTEXT[i] = (VariableDATA *)VAR_ALLOC(PIF);
#endif

#ifdef POOL_BLOCK_ALLOC
        VariableDATA *CONTEXT_i = CONTEXT[i];
#else
        VariableDATA *CONTEXT_i = (VariableDATA *)VAR_ALLOC(PIF);
        CONTEXT [i] = CONTEXT_i;
#endif

        i++;
        CONTEXT_i->TYPE = (TARGET->TYPE < 0) ? -TARGET->TYPE : TARGET->TYPE;
        CONTEXT_i->IS_PROPERTY_RESULT = (TARGET->BY_REF == 2) ? -1 : 0;
        CONTEXT_i->LINKS = 1;
        if (CONTEXT_i->TYPE == VARIABLE_NUMBER)
            CONTEXT_i->NUMBER_DATA = TARGET->nValue;
        else
        if (CONTEXT_i->TYPE == VARIABLE_STRING) {
            if (TARGET->value.Length()) {
                NEW_CONCEPT_STRING_BUFFER(CONTEXT_i, TARGET->value.c_str(), TARGET->value.Length());
            } else
                CONTEXT_i->CLASS_DATA = 0;
        } else
        if (CONTEXT_i->TYPE == VARIABLE_ARRAY) {
            CONTEXT_i->CLASS_DATA = NULL;
            // extremly important: AllocArray is called with "skip top" parameter set to true
            // this is in case garbage collector is called, and current stack variables are not yet initialized
            // not to do the same if class objects are allocated here
            CONTEXT_i->CLASS_DATA = new_Array(PIF, true);
        } else
            CONTEXT_i->CLASS_DATA = NULL;
    }
    CC_WRITE_UNLOCK(((PIFAlizator *)PIF))
#endif
    STACK_TRACE.LOCAL_CONTEXT = (void **)CONTEXT;

    if (CONTEXT)
        STACK_TRACE.len = ((struct Optimizer *)OPTIMIZER)->dataCount;

    if (!PREV)
        AddGCRoot(PIF, &STACK_TRACE);

#ifdef SIMPLE_MULTI_THREADING
    if (thread_lock)
        *thread_lock = 0;
#endif
    VariableDATA *RESULT = NULL;
    if (can_run)
        RESULT = ConceptInterpreter_Interpret((struct ConceptInterpreter *)INTERPRETER, (PIFAlizator *)PIF, CONTEXT, CONCEPT_CLASS_ID, THROW_DATA, &STACK_TRACE);
    if (PREV)
        ((SCStack *)PREV->ROOT)->TOP = PREV_TOP;
    STACK_TRACE.len = -1;
    if (CONTEXT) {
        if (is_main) 
            this->EndMainCall(PIF, RESULT, THROW_DATA, PREV, &STACK_TRACE);
        else
        if ((USE_RESULT) && (RESULT) && (!THROW_DATA) && (FLAGS != MAY_IGNORE_RESULT)) {
            COPY_VARIABLE(USE_RESULT, RESULT, PREV);
            FREE_VARIABLE(RESULT, PREV);
            RESULT = USE_RESULT;
            RESULT->LINKS++;
        }
#if defined(POOL_BLOCK_ALLOC) && defined(POOL_STACK) && (!defined(SIMPLE_MULTI_THREADING))
        // faster inline context cleaning
        if ((PREV) && (STACK_TRACE.alloc_from_stack)) {
            INTEGER data_count = ((struct Optimizer *)this->OPTIMIZER)->dataCount;
            for (INTEGER i = 0; i < data_count; i++) {
                VariableDATA *LOCAL_CONTEXT_i = CONTEXT[i];
                if (LOCAL_CONTEXT_i) {
                    LOCAL_CONTEXT_i->LINKS--;
                    if (LOCAL_CONTEXT_i->LINKS) {
                        CONTEXT[i] = NULL;
                        continue;
                    }
                    CLASS_CHECK_NO_RESET(LOCAL_CONTEXT_i, &STACK_TRACE);
                }
            }
            // }
            SCStack *STACK_ROOT = (SCStack *)STACK_TRACE.ROOT;
            if ((STACK_ROOT) && (STACK_ROOT->STACK_CONTEXT)) {
                STACK_ROOT->stack_pos        -= data_count;
            }
        } else
#endif
        ConceptInterpreter_DestroyEnviroment((struct ConceptInterpreter *)INTERPRETER, (PIFAlizator *)PIF, CONTEXT, &STACK_TRACE, is_main);
    }
    if (!PREV) {
        STACK_TRACE.len = 0;
#ifdef POOL_STACK
        // clear pre-alocated stack
        if ((!is_main) && (STACK_TRACE.STACK_CONTEXT)) {
#ifdef POOL_BLOCK_ALLOC
            BLOCK_VAR_FREE(STACK_TRACE.STACK_CONTEXT, BLOCK_STACK_SIZE);
#else
            for (int i = 0; i < BLOCK_STACK_SIZE; i++) {
                VariableDATA *VD = (VariableDATA *)STACK_TRACE.STACK_CONTEXT[i];
                if (VD) {
                    VAR_FREE(VD);
                }
            }
#endif
        }
#endif
        FAST_FREE(PIF, STACK_TRACE.STACK_CONTEXT);
        RemoveGCRoot(PIF, &STACK_TRACE);
    }
#ifdef EMPIRIC_STACK_CHECK
    --STACK_HIT;
#endif
    if (RESULT) {
        RESULT->LINKS--;
    }
    return RESULT;
}

GreenThreadCycle *ClassMember::CreateThread(void *PIF, intptr_t CONCEPT_CLASS_ID, VariableDATA *Owner) {
    // cannot query anthing else except functions
    if (this->IS_FUNCTION != 1)
        return 0;
    struct ConceptInterpreter *INTERPRETER = (struct ConceptInterpreter *)((struct Optimizer *)this->OPTIMIZER)->INTERPRETER;
    if (!INTERPRETER) {
        INTERPRETER = new_ConceptInterpreter((struct Optimizer *)OPTIMIZER, ((ClassCode *)Defined_In)->CLSID, this);
        ((struct Optimizer *)this->OPTIMIZER)->INTERPRETER = INTERPRETER;
    }

    ParamList FORMAL_PARAM;
    FORMAL_PARAM.COUNT       = 0;
    FORMAL_PARAM.PARAM_INDEX = 0;

    GreenThreadCycle *gtc = (GreenThreadCycle *)malloc(sizeof(GreenThreadCycle));
    if (!gtc)
        return 0;

    gtc->CATCH_INSTRUCTION_POINTER = 0;
    gtc->CATCH_VARIABLE            = 0;
    gtc->INSTRUCTION_COUNT         = ((struct Optimizer *)this->OPTIMIZER)->codeCount;
    gtc->INSTRUCTION_POINTER       = 0;
    gtc->INTERPRETER    = INTERPRETER;
    gtc->NEXT           = 0;
    gtc->PREVIOUS_TRY   = 0;
    gtc->THROW_DATA     = 0;
    gtc->LAST_THREAD    = 0;
    gtc->CURRENT_THREAD = 0;
    gtc->PROPERTIES     = 0;
    gtc->OWNER          = Owner;
    gtc->PIF            = PIF;
    gtc->OPT            = this->OPTIMIZER;
    gtc->refobject      = NULL;
    if ((Owner) && (Owner->TYPE == VARIABLE_DELEGATE)) {
        gtc->refobject  = delegate_Class(Owner->CLASS_DATA);
        ((struct CompiledClass *)gtc->refobject)->LINKS++;
    }

    gtc->STACK_TRACE.CM               = this;
    gtc->STACK_TRACE.PREV             = NULL;
    gtc->STACK_TRACE.ROOT             = &gtc->STACK_TRACE;
    gtc->STACK_TRACE.TOP              = &gtc->STACK_TRACE;
    gtc->STACK_TRACE.STACK_CONTEXT    = NULL;
    gtc->STACK_TRACE.stack_pos        = 0;
    gtc->STACK_TRACE.alloc_from_stack = 0;
    gtc->STACK_TRACE.line             = 0;
    gtc->STACK_TRACE.LOCAL_CONTEXT    = 0;
    gtc->STACK_TRACE.len              = 0;
    bool         can_be_run;
    VariableDATA **CONTEXT = ConceptInterpreter_CreateEnvironment((struct ConceptInterpreter *)INTERPRETER, (PIFAlizator *)PIF, (struct CompiledClass *)Owner->CLASS_DATA, &FORMAL_PARAM, 0, &gtc->STACK_TRACE, can_be_run);
    gtc->LOCAL_CONTEXT             = CONTEXT;
    gtc->STACK_TRACE.LOCAL_CONTEXT = (void **)CONTEXT;
    if (CONTEXT)
        gtc->STACK_TRACE.len = ((struct Optimizer *)this->OPTIMIZER)->dataCount;
    else
        gtc->STACK_TRACE.len = 0;
    AddGCRoot(PIF, &gtc->STACK_TRACE);

    return gtc;
}

void ClassMember::DoneThread(GreenThreadCycle *gtc) {
    if (gtc) {
        gtc->STACK_TRACE.len = -1;
        if (gtc->THROW_DATA) {
            FREE_VARIABLE(gtc->THROW_DATA, NULL);
        }

        if (gtc->LOCAL_CONTEXT)
            ConceptInterpreter_DestroyEnviroment((struct ConceptInterpreter *)gtc->INTERPRETER, (PIFAlizator *)gtc->PIF, gtc->LOCAL_CONTEXT, &gtc->STACK_TRACE);

        RemoveGCRoot(gtc->PIF, &gtc->STACK_TRACE);

        if ((gtc->refobject) && (((struct CompiledClass *)gtc->refobject)->LINKS > 1)) {
            ((struct CompiledClass *)gtc->refobject)->LINKS--;
        }

        if (gtc->OWNER) {
            FREE_VARIABLE(gtc->OWNER, NULL);
        }

        if (gtc->PROPERTIES)
            FAST_FREE(gtc->PIF, gtc->PROPERTIES);
        free(gtc);
    }
}

bool ClassMember::FastOptimizedExecute(void *PIF, void *ref, const ParamList *FORMAL_PARAM, VariableDATA **SenderCTX) {
    if ((FORMAL_PARAM) && (FORMAL_PARAM->COUNT == 1) && (this->IS_FUNCTION == 1) && (this->OPTIMIZER) && (((struct Optimizer *)this->OPTIMIZER)->codeCount == 2)) {
        RuntimeOptimizedElement *OE1 = &((struct Optimizer *)this->OPTIMIZER)->CODE[0];
        RuntimeOptimizedElement *OE2 = &((struct Optimizer *)this->OPTIMIZER)->CODE[1];

        if ((OE1->Operator_ID == KEY_SEL) && (OE1->OperandLeft_ID == 1) &&
            ((OE2->Operator_ID == KEY_ASG) || (OE2->Operator_ID == KEY_BY_REF)) && (OE1->Result_ID == OE2->OperandLeft_ID) && (OE2->OperandRight_ID == 2)) {
            VariableDATA *sndr = SenderCTX [DELTA_UNREF(FORMAL_PARAM, FORMAL_PARAM->PARAM_INDEX) [0] - 1];
            if (sndr) {
                int         reloc      = ((struct CompiledClass *)ref)->_Class->Relocation(OE1->OperandRight_ID - 1);
                ClassMember *pMEMBER_i = reloc ? ((struct CompiledClass *)ref)->_Class->pMEMBERS [reloc - 1] : 0;
                if ((pMEMBER_i) && (!pMEMBER_i->IS_FUNCTION) && (pMEMBER_i->Defined_In == this->Defined_In)) {
                    int          relocation2 = ((struct CompiledClass *)ref)->_Class->RELOCATIONS2 [reloc - 1] - 1;
                    VariableDATA *val        = ((struct CompiledClass *)ref)->_CONTEXT[relocation2];
                    if (val) {
                        CLASS_CHECK(val, NULL);
                    } else {
                        val        = (VariableDATA *)VAR_ALLOC(PIF);
                        val->LINKS = 1;
                        ((struct CompiledClass *)ref)->_CONTEXT[relocation2] = val;
                        val->IS_PROPERTY_RESULT = 0;
                    }
                    val->TYPE = sndr->TYPE;
                    switch (sndr->TYPE) {
                        case VARIABLE_NUMBER:
                            val->NUMBER_DATA = sndr->NUMBER_DATA;
                            break;

                        case VARIABLE_STRING:
                            val->CLASS_DATA     = NULL;
                            CONCEPT_STRING(val, sndr);
                            break;

                        case VARIABLE_DELEGATE:
                            val->CLASS_DATA = copy_Delegate(sndr->CLASS_DATA);
                            break;

                        case VARIABLE_CLASS:
                            val->CLASS_DATA = sndr->CLASS_DATA;
                            ((struct CompiledClass *)val->CLASS_DATA)->LINKS++;
                            break;

                        case VARIABLE_ARRAY:
                            val->CLASS_DATA = sndr->CLASS_DATA;
                            ((struct Array *)val->CLASS_DATA)->LINKS++;
                            break;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

void ClassMember::EnsureThreadSafe() {
    if (this->IS_FUNCTION == 1) {
        struct ConceptInterpreter *INTERPRETER = (struct ConceptInterpreter *)((struct Optimizer *)this->OPTIMIZER)->INTERPRETER;
        if (!INTERPRETER) {
            INTERPRETER = new_ConceptInterpreter((struct Optimizer *)OPTIMIZER, ((ClassCode *)Defined_In)->CLSID, this);
            ((struct Optimizer *)this->OPTIMIZER)->INTERPRETER = INTERPRETER;
        }
#ifdef USE_JIT_TRACE
        if (!INTERPRETER->jittracecode) {
            INTERPRETER->callcount = 1;
            ConceptInterpreter_AnalizeInstructionPath(INTERPRETER, (struct Optimizer *)this->OPTIMIZER);
        }
#endif
    }
}
