#include "ClassMember.h"
#include "Optimizer.h"
#include "ConceptInterpreter.h"
#include "SHManager.h"

//#define EMPIRIC_STACK_CHECK
POOLED_IMPLEMENTATION(ExecutableMember)
POOLED_IMPLEMENTATION(ClassMember)

ClassMember::ClassMember(void *DefinedIn, char *name,  char is_data_only, char _not_binary, char is_unserialize) {
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
            VD->value  = (char *)"0";
            VD->nValue = 0;
        }
        VariableDescriptors = 0;
        PIF_DATA            = 0;
    } else {
        if (_not_binary) {
            PIF_DATA            = new DoubleList();
            VariableDescriptors = new DoubleList();
        } else {
            PIF_DATA            = 0;
            VariableDescriptors = 0;
        }
    }
}

void ClassMember::EnsureVD() {
    if (VD) {
        return;
    }
    VD         = new SmallVariableDESCRIPTOR;
    VD->TYPE   = VARIABLE_NUMBER;
    VD->value  = (char *)"0";
    VD->nValue = 0;
}

int ClassMember::Serialize(FILE *out, bool is_lib) {
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
        ((Optimizer *)OPTIMIZER)->Serialize(out, (INTEGER)is_lib);
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
    int vd_size = VD.value.Length();
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
        refVD->value = (char *)"";
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
            delete (Optimizer *)OPTIMIZER;
        }
    }

    if (VariableDescriptors) {
        delete VariableDescriptors;
    }

    if (PIF_DATA) {
        delete PIF_DATA;
    }

    if (VD) {
        delete VD;
    }
}

#ifdef EMPIRIC_STACK_CHECK
static int STACK_HIT = 0;
 #define MAX_RECURSIVE_CALL    2100
#endif

VariableDATA *ClassMember::Execute(void *PIF, intptr_t CONCEPT_CLASS_ID, VariableDATA *Owner, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, VariableDATA *& THROW_DATA, SCStack *PREV, char is_main) {
#ifdef EMPIRIC_STACK_CHECK
    if (++STACK_HIT > MAX_RECURSIVE_CALL) {
        ((PIFAlizator *)PIF)->Errors.Add(new AnsiException(ERR840, _DEBUG_STARTLINE, 840, NAME, _DEBUG_FILENAME), DATA_EXCEPTION);
        return 0;
    }
#endif

    ConceptInterpreter *INTERPRETER = (ConceptInterpreter *)((Optimizer *)this->OPTIMIZER)->INTERPRETER;
    if (!INTERPRETER) {
        INTERPRETER = new ConceptInterpreter((Optimizer *)OPTIMIZER, ((ClassCode *)Defined_In)->CLSID, this);
        ((Optimizer *)this->OPTIMIZER)->INTERPRETER = INTERPRETER;
    }
    SCStack STACK_TRACE;
    STACK_TRACE.CM               = this;
    STACK_TRACE.PREV             = PREV;
    STACK_TRACE.stack_pos        = 0;
    STACK_TRACE.alloc_from_stack = 0;

    void *PREV_TOP = NULL;
    if (PREV) {
        STACK_TRACE.STACK_CONTEXT = NULL;
        PREV_TOP                     = ((SCStack *)PREV->ROOT)->TOP;
        STACK_TRACE.ROOT             = PREV->ROOT;
        ((SCStack *)PREV->ROOT)->TOP = &STACK_TRACE;
    } else {
        STACK_TRACE.STACK_CONTEXT = (void **)FAST_MALLOC(sizeof(VariableDATA *) * BLOCK_STACK_SIZE);
        STACK_TRACE.ROOT          = &STACK_TRACE;
    }
    STACK_TRACE.TOP = &STACK_TRACE;

    bool         can_run;
    VariableDATA **CONTEXT = ((ConceptInterpreter *)INTERPRETER)->CreateEnviroment((PIFAlizator *)PIF, Owner, FORMAL_PARAM, SenderCTX, &STACK_TRACE, can_run);
    STACK_TRACE.LOCAL_CONTEXT = (void **)CONTEXT;

    if (CONTEXT)
        STACK_TRACE.len = ((Optimizer *)OPTIMIZER)->dataCount;
    else
        STACK_TRACE.len = 0;

    if (!PREV)
        AddGCRoot(PIF, &STACK_TRACE);

    VariableDATA *RESULT = NULL;
    if (can_run)
        RESULT = ((ConceptInterpreter *)INTERPRETER)->Interpret((PIFAlizator *)PIF, CONTEXT, CONCEPT_CLASS_ID, THROW_DATA, &STACK_TRACE);
    if (PREV)
        ((SCStack *)PREV->ROOT)->TOP = PREV_TOP;
    STACK_TRACE.len = -1;
    if (CONTEXT) {
        if (is_main) {
            if (RESULT) {
                if (RESULT->TYPE == VARIABLE_NUMBER)
                    ((PIFAlizator *)PIF)->last_result = (int)RESULT->NUMBER_DATA;

                if (((PIFAlizator *)PIF)->static_result) {
                    CLASS_CHECK(((VariableDATA *)((PIFAlizator *)PIF)->static_result));
                    switch (RESULT->TYPE) {
                        case VARIABLE_NUMBER:
                            ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE        = VARIABLE_NUMBER;
                            ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->NUMBER_DATA = RESULT->NUMBER_DATA;
                            break;

                        case VARIABLE_STRING:
                            ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE           = VARIABLE_STRING;
                            CONCEPT_STRING(((VariableDATA *)((PIFAlizator *)PIF)->static_result)) = CONCEPT_STRING(RESULT);
                            break;

                        case VARIABLE_ARRAY:
                            ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE           = VARIABLE_STRING;
                            CONCEPT_STRING(((VariableDATA *)((PIFAlizator *)PIF)->static_result)) = "array";
                            break;

                        case VARIABLE_DELEGATE:
                            ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE = VARIABLE_STRING;
                            if (RESULT->CLASS_DATA) {
                                CONCEPT_STRING(((VariableDATA *)((PIFAlizator *)PIF)->static_result))  = ((CompiledClass *)RESULT->CLASS_DATA)->_Class->NAME.c_str();
                                CONCEPT_STRING(((VariableDATA *)((PIFAlizator *)PIF)->static_result)) += (char *)"::";
                                CONCEPT_STRING(((VariableDATA *)((PIFAlizator *)PIF)->static_result)) += ((CompiledClass *)RESULT->CLASS_DATA)->_Class->pMEMBERS [(INTEGER)RESULT->DELEGATE_DATA - 1]->NAME;
                            } else {
                                CONCEPT_STRING(((VariableDATA *)((PIFAlizator *)PIF)->static_result)) = "delegate";
                            }
                            break;

                        case VARIABLE_CLASS:
                            ((VariableDATA *)((PIFAlizator *)PIF)->static_result)->TYPE = VARIABLE_STRING;
                            if (RESULT->CLASS_DATA)
                                CONCEPT_STRING(((VariableDATA *)((PIFAlizator *)PIF)->static_result)) = ((CompiledClass *)RESULT->CLASS_DATA)->_Class->NAME.c_str();
                            else
                                CONCEPT_STRING(((VariableDATA *)((PIFAlizator *)PIF)->static_result)) = "object";
                            break;
                    }
                }
                FREE_VARIABLE(RESULT);
                RESULT = NULL;
            }
            if (THROW_DATA) {
                AnsiString *data = new AnsiString();

                // check if is not the static call to Main. In this case the THROW_DATA is/may be deleted
                if (THROW_DATA->TYPE == VARIABLE_STRING) {
                    *data += CONCEPT_STRING(THROW_DATA);
                } else
                if (THROW_DATA->TYPE == VARIABLE_NUMBER) {
                    *data += AnsiString(THROW_DATA->NUMBER_DATA);
                } else
                if (THROW_DATA->TYPE == VARIABLE_CLASS) {
                    *data += ((CompiledClass *)THROW_DATA->CLASS_DATA)->GetClassName();
                } else
                if (THROW_DATA->TYPE == VARIABLE_ARRAY) {
                    *data += "Array";
                } else
                if (THROW_DATA->TYPE == VARIABLE_DELEGATE) {
                    *data += "Delegate of ";
                    *data += ((CompiledClass *)THROW_DATA->CLASS_DATA)->GetClassName();
                }

                FREE_VARIABLE(THROW_DATA);
                THROW_DATA = NULL;

                AnsiException *Exc = new AnsiException(ERR630, 0, 630, data->c_str(), ((ClassCode *)(this->Defined_In))->_DEBUG_INFO_FILENAME, NAME);
                ((PIFAlizator *)PIF)->AcknoledgeRunTimeError(PREV, Exc);

                delete data;
            }
        }
        ((ConceptInterpreter *)INTERPRETER)->DestroyEnviroment((PIFAlizator *)PIF, CONTEXT, &STACK_TRACE, is_main);
    }
    if (!PREV) {
        STACK_TRACE.len = 0;
        FAST_FREE(STACK_TRACE.STACK_CONTEXT);
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
    ConceptInterpreter *INTERPRETER = (ConceptInterpreter *)((Optimizer *)this->OPTIMIZER)->INTERPRETER;
    if (!INTERPRETER) {
        INTERPRETER = new ConceptInterpreter((Optimizer *)OPTIMIZER, ((ClassCode *)Defined_In)->CLSID, this);
        ((Optimizer *)this->OPTIMIZER)->INTERPRETER = INTERPRETER;
    }

    ParamList FORMAL_PARAM;
    FORMAL_PARAM.COUNT       = 0;
    FORMAL_PARAM.PARAM_INDEX = 0;

    GreenThreadCycle *gtc = (GreenThreadCycle *)malloc(sizeof(GreenThreadCycle));
    if (!gtc)
        return 0;

    gtc->CATCH_INSTRUCTION_POINTER = 0;
    gtc->CATCH_VARIABLE            = 0;
    gtc->INSTRUCTION_COUNT         = ((Optimizer *)this->OPTIMIZER)->codeCount;
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

    gtc->STACK_TRACE.CM               = this;
    gtc->STACK_TRACE.PREV             = NULL;
    gtc->STACK_TRACE.ROOT             = &gtc->STACK_TRACE;
    gtc->STACK_TRACE.TOP              = &gtc->STACK_TRACE;
    gtc->STACK_TRACE.STACK_CONTEXT    = NULL;
    gtc->STACK_TRACE.stack_pos        = 0;
    gtc->STACK_TRACE.alloc_from_stack = 0;
    bool         can_be_run;
    VariableDATA **CONTEXT = ((ConceptInterpreter *)INTERPRETER)->CreateEnviroment((PIFAlizator *)PIF, Owner, &FORMAL_PARAM, 0, &gtc->STACK_TRACE, can_be_run);
    gtc->LOCAL_CONTEXT             = CONTEXT;
    gtc->STACK_TRACE.LOCAL_CONTEXT = (void **)CONTEXT;
    if (CONTEXT)
        gtc->STACK_TRACE.len = ((Optimizer *)this->OPTIMIZER)->dataCount;
    else
        gtc->STACK_TRACE.len = 0;
    AddGCRoot(PIF, &gtc->STACK_TRACE);

    return gtc;
}

void ClassMember::DoneThread(GreenThreadCycle *gtc) {
    if (gtc) {
        gtc->STACK_TRACE.len = -1;
        if (gtc->THROW_DATA) {
            FREE_VARIABLE(gtc->THROW_DATA);
        }

        if (gtc->LOCAL_CONTEXT)
            ((ConceptInterpreter *)gtc->INTERPRETER)->DestroyEnviroment((PIFAlizator *)gtc->PIF, gtc->LOCAL_CONTEXT, &gtc->STACK_TRACE);

        RemoveGCRoot(gtc->PIF, &gtc->STACK_TRACE);

        if (gtc->OWNER) {
            FREE_VARIABLE(gtc->OWNER);
        }

        if (gtc->PROPERTIES)
            FAST_FREE(gtc->PROPERTIES);
        free(gtc);
    }
}

bool ClassMember::FastOptimizedExecute(void *PIF, void *ref, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX) {
    if ((FORMAL_PARAM) && (FORMAL_PARAM->COUNT == 1) && (this->IS_FUNCTION == 1) && (this->OPTIMIZER) && (((Optimizer *)this->OPTIMIZER)->codeCount == 2)) {
        RuntimeOptimizedElement *OE1 = &((Optimizer *)this->OPTIMIZER)->CODE[0];
        RuntimeOptimizedElement *OE2 = &((Optimizer *)this->OPTIMIZER)->CODE[1];

        if ((OE1->Operator.TYPE == TYPE_OPERATOR) && (OE1->Operator.ID == KEY_SEL) && (OE1->OperandLeft.ID == 1) &&
            (OE2->Operator.TYPE == TYPE_OPERATOR) && ((OE2->Operator.ID == KEY_ASG) || (OE2->Operator.ID == KEY_BY_REF)) && (OE1->Result_ID == OE2->OperandLeft.ID) && (OE2->OperandRight.ID == 2)) {
            VariableDATA *sndr = SenderCTX [DELTA_UNREF(FORMAL_PARAM, FORMAL_PARAM->PARAM_INDEX) [0] - 1];
            if (sndr) {
                int         reloc      = ((CompiledClass *)ref)->_Class->Relocation(OE1->OperandRight.ID - 1);
                ClassMember *pMEMBER_i = reloc ? ((CompiledClass *)ref)->_Class->pMEMBERS [reloc - 1] : 0;
                if ((pMEMBER_i) && (!pMEMBER_i->IS_FUNCTION) && (pMEMBER_i->Defined_In == this->Defined_In)) {
                    int          relocation2 = ((CompiledClass *)ref)->_Class->RELOCATIONS2 [reloc - 1] - 1;
                    VariableDATA *val        = ((CompiledClass *)ref)->_CONTEXT[relocation2];
                    if (val) {
                        CLASS_CHECK(val);
                    } else {
                        val        = (VariableDATA *)VAR_ALLOC(PIF);
                        val->LINKS = 1;
                        ((CompiledClass *)ref)->_CONTEXT[relocation2] = val;
                        val->IS_PROPERTY_RESULT = 0;
                    }
                    val->TYPE = sndr->TYPE;
                    switch (sndr->TYPE) {
                        case VARIABLE_NUMBER:
                            val->NUMBER_DATA = sndr->NUMBER_DATA;
                            break;

                        case VARIABLE_STRING:
                            val->CLASS_DATA     = NULL;
                            CONCEPT_STRING(val) = CONCEPT_STRING(sndr);
                            break;

                        case VARIABLE_DELEGATE:
                            val->DELEGATE_DATA = sndr->DELEGATE_DATA;

                        case VARIABLE_CLASS:
                            val->CLASS_DATA = sndr->CLASS_DATA;
                            ((CompiledClass *)val->CLASS_DATA)->LINKS++;
                            break;

                        case VARIABLE_ARRAY:
                            val->CLASS_DATA = sndr->CLASS_DATA;
                            ((Array *)val->CLASS_DATA)->LINKS++;
                            break;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

