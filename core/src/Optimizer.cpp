#include "Optimizer.h"
#include "AnsiException.h"
#include "SHManager.h"
#include "ConceptInterpreter.h"

POOLED_IMPLEMENTATION(Optimizer)

#define MY_ABS2(X)        ((X < 0) ? -X : X)
#define MY_STATIC(X)      ((X != VARIABLE_STRING) && (X != VARIABLE_NUMBER)) ? VARIABLE_NUMBER : X

#define MY_ABS(X)         MY_STATIC(MY_ABS2(X))

#define PUSH_CHAR(VAR)    char VAR_PUSHED = VAR;
#define POP_CHAR(VAR)     VAR             = VAR_PUSHED;

#define MAKE_NULL(AE)    AE.TYPE = 0; AE.ID = 0;

#ifndef SINGLE_PROCESS_DLL
THREADED_FLAG INTEGER Optimizer::LAST_DEBUG_TRAP;

PIFAlizator *Optimizer::PIFOwner;

THREADED_FLAG AnsiList *Optimizer::BREAK_Elements;
THREADED_FLAG AnsiList *Optimizer::CONTINUE_Elements;

THREADED_FLAG ClassCode *Optimizer::_CLASS;
THREADED_FLAG const char*Optimizer::_MEMBER;

THREADED_FLAG DoubleList *Optimizer::PIFList;
THREADED_FLAG DoubleList *Optimizer::VDList;
THREADED_FLAG INTEGER    Optimizer::PIF_POSITION;
THREADED_FLAG const char *Optimizer::_DEBUG_INFO_FILENAME;
THREADED_FLAG char       Optimizer::NO_WARNING_EMPTY;
THREADED_FLAG char       Optimizer::NO_WARNING_ATTR;
THREADED_FLAG char       Optimizer::_clean_condition;
THREADED_FLAG INTEGER    Optimizer::CATCH_ELEMENT;
THREADED_FLAG AnsiList   *Optimizer::OptimizedPIF = NULL;
THREADED_FLAG AnsiList   *Optimizer::ParameterList = NULL;
#endif

#define END_WHILE_CYCLE(TEMP_ST_POS, NOT_TRUE_POS)                                                       \
    {                                                                                                    \
        int __d_i, __l_i;                                                                                \
        __l_i = CONTINUE_Elements->Count();                                                              \
        if (__l_i) {                                                                                     \
            for (__d_i = 0; __d_i < __l_i; __d_i++) {                                                    \
                OptimizedElement *CONTINUE_Element = (OptimizedElement *)CONTINUE_Elements->Item(__d_i); \
                CONTINUE_Element->OperandReserved.ID = TEMP_ST_POS;                                      \
            }                                                                                            \
        }                                                                                                \
        delete CONTINUE_Elements;                                                                        \
        CONTINUE_Elements = PUSHED_CONTINUE_Elements;                                                    \
        __l_i             = BREAK_Elements->Count();                                                     \
        if (__l_i) {                                                                                     \
            for (__d_i = 0; __d_i < __l_i; __d_i++) {                                                    \
                OptimizedElement *BREAK_Element = (OptimizedElement *)BREAK_Elements->Item(__d_i);       \
                BREAK_Element->OperandReserved.ID = NOT_TRUE_POS;                                        \
            }                                                                                            \
        }                                                                                                \
        delete BREAK_Elements;                                                                           \
        BREAK_Elements = PUSHED_BREAK_Elements;                                                          \
    }

#define END_SWITCH(NOT_TRUE_POS)                                                                   \
    {                                                                                              \
        int __d_i, __l_i;                                                                          \
        __l_i = BREAK_Elements->Count();                                                           \
        if (__l_i) {                                                                               \
            for (__d_i = 0; __d_i < __l_i; __d_i++) {                                              \
                OptimizedElement *BREAK_Element = (OptimizedElement *)BREAK_Elements->Item(__d_i); \
                BREAK_Element->OperandReserved.ID = NOT_TRUE_POS;                                  \
            }                                                                                      \
        }                                                                                          \
        delete BREAK_Elements;                                                                     \
        BREAK_Elements = PUSHED_BREAK_Elements;                                                    \
    }

class TempVariableManager {
    int        *variables;
    int        var_size;
    int        pos;
    DoubleList *VDList;
public:
    TempVariableManager(DoubleList *vlist) {
        VDList    = vlist;
        variables = NULL;
        var_size  = 0;
        pos       = 0;
    }

    void Reset() {
        pos = 0;
    }

    int GetVar() {
        if (pos >= var_size) {
            int orig = var_size;
            var_size += 16;

            variables = (int *)realloc(variables, sizeof(int) * var_size);
            for (int i = orig; i < var_size; i++)
                variables[i] = 0;
        }
        int res = variables[pos];
        if (!res) {
            VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
            VD->BY_REF = 0;
            VD->value  = NULL_STRING;
            VD->nValue = 0;
            VD->USED   = -2;

            VD->TYPE = VARIABLE_NUMBER;
            VDList->Add(VD, DATA_VAR_DESCRIPTOR);
            res            = VDList->Count();
            variables[pos] = res;
        }
        pos++;
        return res;
    }
    
    ~TempVariableManager() {
        if (variables)
            free(variables);
    }
};

Optimizer::Optimizer(PIFAlizator *P, DoubleList *_PIFList, DoubleList *_VDList, const char *Filename, ClassCode *cls, const char *member, bool is_unserialized) {
    PIFOwner = P;
    PIFList  = _PIFList;
    VDList   = _VDList;

    PIF_POSITION         = 0;
    CODE                 = 0;
    DATA                 = 0;
    PARAMS               = 0;
    codeCount            = 0;
    dataCount            = 0;
    paramCount           = 0;
    CATCH_ELEMENT        = 0;
    _DEBUG_INFO_FILENAME = Filename;
    NO_WARNING_EMPTY     = 0;
    NO_WARNING_ATTR      = 0;
    _clean_condition     = 0;
    _CLASS               = cls;
    _MEMBER              = member;
    INTERPRETER          = 0;

    Optimizer::BREAK_Elements    = 0;
    Optimizer::CONTINUE_Elements = 0;

    LAST_DEBUG_TRAP = -1;
}

bool Optimizer::TryCheckParameters(OptimizedElement *OE, int p_count, int *minp, int *maxp, ClassMember **target_CM, ClassCode *owner_CC) {
    if (target_CM) {
        *target_CM = 0;
    }
    if (!OE) {
        return true;
    }

    *minp = 0;
    *maxp = 0;

    if (OE->Operator.ID == KEY_NEW) {
        ClassCode *CC       = (ClassCode *)PIFOwner->ClassList->Item(OE->OperandLeft.ID - 1);
        int       param_min = 0;
        int       param_max = 0;
        if ((CC) && (CC->CONSTRUCTOR_MEMBER)) {
            param_min = CC->CONSTRUCTOR_MEMBER->MUST_PARAMETERS_COUNT;
            param_max = CC->CONSTRUCTOR_MEMBER->PARAMETERS_COUNT;
            if (target_CM) {
                *target_CM = CC->CONSTRUCTOR_MEMBER;
            }
        } else {
            return true;
        }

        if ((p_count >= param_min) && (p_count <= param_max)) {
            return true;
        } else {
            *minp = param_min;
            *maxp = param_max;
            return false;
        }
    }

    if ((OE->Operator.ID == KEY_SEL) && (OE->OperandLeft.TYPE == TYPE_VARIABLE) && (OE->OperandLeft.ID == 1)) {
        ClassCode *CC = owner_CC;
        if (!CC) {
            int len = PIFOwner->ClassList->Count();
            CC = (ClassCode *)PIFOwner->ClassList->Item(len - 1);
        }
        if ((CC) && (CC->Members)) {
            int index = CC->HasMember(OE->OperandRight._PARSE_DATA);
            if (index) {
                ClassMember *CM = (ClassMember *)CC->Members->Item(index - 1);
                if (CM) {
                    if (target_CM) {
                        *target_CM = CM;
                    }
                    if ((p_count >= CM->MUST_PARAMETERS_COUNT) && (p_count <= CM->PARAMETERS_COUNT)) {
                        return true;
                    } else {
                        if ((CM->IS_FUNCTION) && (CM->IS_FUNCTION < 2)) {
                            *minp = CM->MUST_PARAMETERS_COUNT;
                            *maxp = CM->PARAMETERS_COUNT;
                            return false;
                        } else {
                            return true;
                        }
                    }
                }
            }
        }
        return true;
    }
    return true;
}

void Optimizer::BuildParameterList(INTEGER START_POS, AnalizerElement *METHOD) {
    AnalizerElement *AE = 0;

    AnsiList *LocalParamList = new AnsiList(false);

    ParameterList->Add(LocalParamList, DATA_LIST);
    bool prec_is_comma      = false;
    bool parameter_expected = true;
    bool at_least_one       = false;
    int  last = PIF_POSITION - 1;

    int delta = PIF_POSITION - START_POS;

    // not really necessary, but just to be sure ...
    if (METHOD->_HASH_DATA) {
        delta = 0;
    }
    // end here

    int index = 0;
    if ((METHOD) && (delta)) {
        METHOD->_INFO_OPTIMIZED = -12;
        METHOD->_HASH_DATA      = new void * [delta + 1];
    }

    for (INTEGER i = START_POS; i < PIF_POSITION; i++) {
        AE = (AnalizerElement *)PIFList->Item(i);
        bool no_delete = true;
        if ((AE->TYPE == TYPE_OPERATOR) && (AE->ID == KEY_COMMA) && (!prec_is_comma) && (at_least_one) && (i != last)) {
            prec_is_comma      = true;
            parameter_expected = true;
            no_delete          = false;
        } else
        if ((AE->TYPE == TYPE_VARIABLE) && (parameter_expected)) {
            prec_is_comma      = false;
            parameter_expected = false;
            //============================================//
            // replaced Require(AE) with this because a bug
            // which caused the a++ expresion to be evaluated
            // (foo was not called)
            //============================================//
            if ((METHOD) && (delta)) {
                ((void **)METHOD->_HASH_DATA) [index++] = AE;
            } else {
                Require(AE);
            }
            //============================================//
            LocalParamList->Add((void *)AE->ID, DATA_32_BIT);
        } else {
            PIFOwner->Errors.Add(new AnsiException(ERR200, AE->_DEBUG_INFO_LINE, 200, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
        }

        if (index) {
            ((void **)METHOD->_HASH_DATA) [index] = 0;
        }

        if ((delta) && (no_delete)) {
            PIFList->Remove(i);
        } else {
            PIFList->Delete(i);
        }
        i--;
        AE = 0;
        PIF_POSITION--;
        at_least_one = true;
        last         = PIF_POSITION - 1;
    }
    // inserez lista de parametri ...
    AnalizerElement *newAE = new AnalizerElement;
    newAE->ID               = ParameterList->Count();
    newAE->TYPE             = TYPE_PARAM_LIST;
    newAE->_DEBUG_INFO_LINE = AE ? AE->_DEBUG_INFO_LINE             : 0;
    newAE->_INFO_OPTIMIZED  = 1;
    newAE->_HASH_DATA       = 0;
    PIFList->Insert(newAE, PIF_POSITION++, DATA_ANALIZER_ELEMENT);
}

void Optimizer::CopyElement(AnalizerElement *SRC, AnalizerElement *DEST) {
    DEST->_DEBUG_INFO_LINE = SRC ? SRC->_DEBUG_INFO_LINE : 0;
    DEST->_INFO_OPTIMIZED  = SRC ? SRC->_INFO_OPTIMIZED : 0;
    DEST->_PARSE_DATA      = SRC ? SRC->_PARSE_DATA.c_str() : NULL_STRING;
    DEST->ID         = SRC ? SRC->ID : 0;
    DEST->TYPE       = SRC ? SRC->TYPE : 0;
    DEST->_HASH_DATA = SRC ? SRC->_HASH_DATA : 0;
}

void Optimizer::CopyElement(AnalizerElement *SRC, PartialAnalizerElement *DEST) {
    DEST->_DEBUG_INFO_LINE = SRC ? SRC->_DEBUG_INFO_LINE : 0;
    DEST->ID   = SRC ? SRC->ID : 0;
    DEST->TYPE = SRC ? SRC->TYPE : 0;
}

void Optimizer::CopyElement(AnalizerElement *SRC, SHORT_AnalizerElement *DEST) {
    DEST->ID   = SRC ? SRC->ID : 0;
    DEST->TYPE = SRC ? SRC->TYPE : 0;
}

INTEGER Optimizer::Check(AnalizerElement *Target, AnalizerElement *AE) {
    if (!Target) {
        PIFOwner->Errors.Add(new AnsiException(ERR030, AE->_DEBUG_INFO_LINE, 30, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
        return 1;
    }
    if ((AE->TYPE == TYPE_OPERATOR) && (AE->ID == KEY_NEW)) {
        if (Target->TYPE != TYPE_CLASS) {
            PIFOwner->Errors.Add(new AnsiException(ERR110, AE->_DEBUG_INFO_LINE, 110, Target->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            return 1;
        }
        return 0;
    }

    if ((AE->TYPE == TYPE_OPERATOR) && (AE->ID == KEY_SEL)) {
        if ((Target->TYPE != TYPE_METHOD) && (Target->TYPE != TYPE_VARIABLE)) {
            PIFOwner->Errors.Add(new AnsiException(ERR130, AE->_DEBUG_INFO_LINE, 130, Target->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            return 1;
        }
        return 0;
    }

    // a se reveni !
    if ((AE->TYPE == TYPE_OPERATOR) && (AE->ID == KEY_DLL_CALL)) {
        if ((Target->TYPE != TYPE_METHOD) && (Target->TYPE != TYPE_CLASS) && (Target->TYPE != TYPE_VARIABLE)) {
            PIFOwner->Errors.Add(new AnsiException(ERR680, AE->_DEBUG_INFO_LINE, 680, Target->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            return 1;
        }
        return 0;
    }

    if ((AE->TYPE == TYPE_OPERATOR) && (AE->ID == KEY_INDEX_OPEN)) {
        if ((Target->TYPE != TYPE_PARAM_LIST) && (Target->TYPE != TYPE_VARIABLE)) {
            PIFOwner->Errors.Add(new AnsiException(ERR290, AE->_DEBUG_INFO_LINE, 290, Target->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            return 1;
        }
        return 0;
    }

    if ((Target->TYPE != TYPE_VARIABLE) &&
        (Target->TYPE != VARIABLE_UNDEFINED) &&
        (Target->TYPE != VARIABLE_NUMBER) &&
        (Target->TYPE != VARIABLE_STRING) &&
        (Target->TYPE != VARIABLE_CONSTANT)) {
        PIFOwner->Errors.Add(new AnsiException(ERR040, AE->_DEBUG_INFO_LINE, 40, Target->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
        return 1;
    }
    return 0;
}

INTEGER Optimizer::GetFirstOperator(DoubleList *LST, INTEGER start, INTEGER end) {
    INTEGER FIRST_OPERATOR = 0xFFFF;
    INTEGER FIRST_POSITION = -1;
    INTEGER PREV_OP;
    // --------------------------------------------------
    // --------------------------------------------------

    INTEGER FIRST_PRIORITY = 0xFFFF;

    for (INTEGER i = start; i < end; i++) {
        AnalizerElement *AE = (AnalizerElement *)LST->Item(i);
        if ((AE->TYPE == TYPE_OPERATOR) && (!AE->_INFO_OPTIMIZED)) {
            INTEGER PRIORITY = GetPriority(AE->ID);
            if ((PRIORITY < FIRST_PRIORITY) ||
                ((PRIORITY == FIRST_PRIORITY) && ((PRIORITY == OPERATOR_LEVEL_8) || ((PRIORITY == KEY_SEL) && (PREV_OP != KEY_SEL))))) {
                FIRST_OPERATOR = AE->ID;
                FIRST_POSITION = i;
                FIRST_PRIORITY = PRIORITY;
            }
            PREV_OP = AE->ID;
        }
    }
    return FIRST_POSITION;
}

INTEGER Optimizer::Require(AnalizerElement *Element) {
    INTEGER          dislocation = 0;
    OptimizedElement *OE         = 0;

    if (Element) {
        //===============================================================//
        if ((Element->_INFO_OPTIMIZED == (signed char)-12) && (Element->_HASH_DATA)) {
            int index           = 0;
            AnalizerElement *AE = 0;
            do {
                AE = (AnalizerElement *)((void **)Element->_HASH_DATA) [index++];
                if (AE) {
                    dislocation += Require(AE);
                    delete AE;
                } else {
                    break;
                }
            } while (true);
            delete[] ((void **)Element->_HASH_DATA);
            Element->_HASH_DATA = 0;
        }
        OE = (OptimizedElement *)Element->_HASH_DATA;
        //===============================================================//
        if (OE) {
            INTEGER          OP_ID   = -1;
            OptimizedElement *ifOE   = 0;
            OptimizedElement *gotoOE = 0;

            if (OE->Operator.TYPE == TYPE_OPERATOR) {
                OP_ID = OE->Operator.ID;
            }

            Element->_HASH_DATA = 0;

            dislocation++;
            dislocation += Require(&OE->OperandLeft);

            if (OP_ID == KEY_BAN) {
                ifOE = new OptimizedElement;
                ifOE->Operator._DEBUG_INFO_LINE = OE->Operator._DEBUG_INFO_LINE;
                ifOE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
                ifOE->Operator.ID   = KEY_OPTIMIZED_IF;
                MAKE_NULL(ifOE->OperandLeft);
                ifOE->OperandReserved.ID   = 0;
                ifOE->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;

                CopyElement(&OE->OperandLeft, &ifOE->OperandRight);
                ifOE->Result_ID = 0;
                dislocation++;
                OptimizedPIF->Add(ifOE, DATA_OPTIMIZED_ELEMENT);
            } else
            if ((OP_ID == KEY_BOR) || (OP_ID == KEY_CND_NULL)) {
                ifOE = new OptimizedElement;
                ifOE->Operator._DEBUG_INFO_LINE = OE->Operator._DEBUG_INFO_LINE;
                ifOE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
                ifOE->Operator.ID   = KEY_OPTIMIZED_IF;
                MAKE_NULL(ifOE->OperandLeft);
                ifOE->OperandReserved.ID   = OptimizedPIF->Count() + 2;
                ifOE->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;

                CopyElement(&OE->OperandLeft, &ifOE->OperandRight);
                ifOE->Result_ID = 0;

                // setez GOTO-ul
                gotoOE = new OptimizedElement;
                gotoOE->Operator._DEBUG_INFO_LINE = OE->Operator._DEBUG_INFO_LINE;
                gotoOE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
                gotoOE->Operator.ID   = KEY_OPTIMIZED_GOTO;
                MAKE_NULL(gotoOE->OperandLeft);
                MAKE_NULL(gotoOE->OperandRight);
                gotoOE->OperandReserved.ID   = 0;
                gotoOE->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;

                gotoOE->Result_ID = 0;

                OptimizedPIF->Add(ifOE, DATA_OPTIMIZED_ELEMENT);
                OptimizedPIF->Add(gotoOE, DATA_OPTIMIZED_ELEMENT);
            }

            dislocation += Require(&OE->OperandRight);

            if (OP_ID == KEY_BAN) {
                ifOE->OperandReserved.ID = OptimizedPIF->Count() ;
            } else
            if ((OP_ID == KEY_BOR) || (OP_ID == KEY_CND_NULL)) {
                gotoOE->OperandReserved.ID = OptimizedPIF->Count();
            }

            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);
        }
    }
    return dislocation;
}

INTEGER Optimizer::OptimizeArray(TempVariableManager *TVM) {
    AnalizerElement *newAE = new AnalizerElement;

    AnalizerElement *BASE_AE = (AnalizerElement *)PIFList->Remove(--PIF_POSITION);

    BASE_AE->_INFO_OPTIMIZED = 1;
    INTEGER            result = 0;
    VariableDESCRIPTOR *VD;

    VD         = new VariableDESCRIPTOR;
    VD->USED   = -1;
    VD->nValue = 0;
    VD->BY_REF = 0;
    VD->TYPE   = VARIABLE_NUMBER;
    VDList->Add(VD, DATA_VAR_DESCRIPTOR);
    INTEGER result_id = VDList->Count();

    VD         = new VariableDESCRIPTOR;
    VD->USED   = -1;
    VD->nValue = 0;
    VD->BY_REF = 0;
    VD->TYPE   = VARIABLE_NUMBER;
    VDList->Add(VD, DATA_VAR_DESCRIPTOR);
    INTEGER array_id = VDList->Count();

    newAE->ID               = VDList->Count();
    newAE->TYPE             = TYPE_VARIABLE;
    newAE->_DEBUG_INFO_LINE = BASE_AE->_DEBUG_INFO_LINE;
    newAE->_INFO_OPTIMIZED  = 1;
    newAE->_HASH_DATA       = 0;

    INTEGER index = 0;
    _clean_condition = 1;

    OptimizedElement *OE = new OptimizedElement;
    CopyElement(BASE_AE, &OE->Operator);
    OE->Operator.ID   = KEY_NEW;
    OE->Operator.TYPE = TYPE_OPERATOR;

    OE->OperandLeft.ID               = STATIC_CLASS_ARRAY;
    OE->OperandLeft.TYPE             = TYPE_CLASS;
    OE->OperandLeft._DEBUG_INFO_LINE = BASE_AE->_DEBUG_INFO_LINE;
    OE->OperandLeft._HASH_DATA       = 0;
    OE->OperandLeft._INFO_OPTIMIZED  = 1;
    MAKE_NULL(OE->OperandReserved);

    OE->Result_ID = array_id;

    CopyElement(0, &OE->OperandRight);
    OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);

    int iteratii = 0;
    while (1) {
        AnalizerElement *AE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
        if (!AE) {
            break;
        }
        iteratii++;

        if (AE->TYPE != TYPE_KEYWORD) {
            if ((AE->TYPE != TYPE_OPERATOR) || (AE->ID != KEY_INDEX_CLOSE)) {
                OptimizeExpression(TVM, KEY_COMMA, TYPE_OPERATOR, 0, 1, KEY_INDEX_CLOSE);
                AnalizerElement *RES     = (AnalizerElement *)PIFList->Item(PIF_POSITION - 1);
                AnalizerElement *RES_KEY = (AnalizerElement *)PIFList->Item(PIF_POSITION);
                if ((RES_KEY) && (RES_KEY->TYPE == TYPE_OPERATOR) && ((RES_KEY->ID == KEY_ARR_KEY) || (RES_KEY->ID == KEY_CND_2))) {
                    PIFList->Delete(PIF_POSITION);
                    OptimizeExpression(TVM, KEY_COMMA, TYPE_OPERATOR, 0, 1, KEY_INDEX_CLOSE);
                    RES_KEY = RES;
                    RES     = (AnalizerElement *)PIFList->Item(PIF_POSITION - 1);
                } else
                    RES_KEY = 0;
                INTEGER _id     = 0;
                INTEGER _KEY_id = 0;

                if ((RES_KEY) && (RES_KEY->TYPE != TYPE_VARIABLE))
                    PIFOwner->Errors.Add(new AnsiException(ERR1260, AE ? AE->_DEBUG_INFO_LINE : 0, 1260, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);

                if ((!RES) || (RES->TYPE != TYPE_VARIABLE)) {
                    PIFOwner->Errors.Add(new AnsiException(ERR1100, AE ? AE->_DEBUG_INFO_LINE : 0, 1100, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                } else {
                    _id = RES->ID;
                    PIFList->Delete(PIF_POSITION - 1);
                    PIF_POSITION--;

                    if (RES_KEY) {
                        _KEY_id = RES_KEY->ID;
                        PIFList->Delete(PIF_POSITION - 1);
                        PIF_POSITION--;
                    }
                }

                OptimizedElement *OE = new OptimizedElement;
                CopyElement(BASE_AE, &OE->Operator);
                MAKE_NULL(OE->OperandReserved);

                OE->OperandLeft.ID               = array_id;
                OE->OperandLeft.TYPE             = TYPE_VARIABLE;
                OE->OperandLeft._DEBUG_INFO_LINE = BASE_AE->_DEBUG_INFO_LINE;
                OE->OperandLeft._HASH_DATA       = 0;
                OE->OperandLeft._INFO_OPTIMIZED  = 1;

                result = _id;

                VariableDESCRIPTOR *VD = 0;
                INTEGER            result_index;

                if (_KEY_id) {
                    result_index = _KEY_id;
                } else {
                    VD         = new VariableDESCRIPTOR;
                    VD->USED   = -1;
                    VD->nValue = index;
                    VD->value  = AnsiString(index);
                    VD->BY_REF = 0;
                    VD->TYPE   = VARIABLE_NUMBER;
                    VDList->Add(VD, DATA_VAR_DESCRIPTOR);
                    result_index = VDList->Count();
                }
                index++;

                OE->OperandRight.ID               = result_index;
                OE->OperandRight.TYPE             = TYPE_VARIABLE;
                OE->OperandRight._DEBUG_INFO_LINE = BASE_AE->_DEBUG_INFO_LINE;
                OE->OperandRight._HASH_DATA       = 0;
                OE->OperandRight._INFO_OPTIMIZED  = 1;

                OE->Result_ID = result_id;
                OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);

                OE = new OptimizedElement;
                CopyElement(BASE_AE, &OE->Operator);
                MAKE_NULL(OE->OperandReserved);
                OE->Operator.ID = KEY_BY_REF;

                OE->OperandLeft.ID               = result_id;
                OE->OperandLeft.TYPE             = TYPE_VARIABLE;
                OE->OperandLeft._DEBUG_INFO_LINE = BASE_AE->_DEBUG_INFO_LINE;
                OE->OperandLeft._HASH_DATA       = 0;
                OE->OperandLeft._INFO_OPTIMIZED  = 1;

                OE->OperandRight.ID               = result;
                OE->OperandRight.TYPE             = TYPE_VARIABLE;
                OE->OperandRight._DEBUG_INFO_LINE = BASE_AE->_DEBUG_INFO_LINE;
                OE->OperandRight._HASH_DATA       = 0;
                OE->OperandRight._INFO_OPTIMIZED  = 1;

                VD         = new VariableDESCRIPTOR;
                VD->USED   = -1;
                VD->nValue = 0;
                VD->BY_REF = 0;
                VD->TYPE   = VARIABLE_NUMBER;
                VDList->Add(VD, DATA_VAR_DESCRIPTOR);
                INTEGER container_id = VDList->Count();

                OE->Result_ID = container_id;
                OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);
            } else {
                PIFList->Delete(PIF_POSITION--);
                break;
            }
        } else {
            PIFOwner->Errors.Add(new AnsiException(ERR1100, AE ? AE->_DEBUG_INFO_LINE : 0, 1100, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            break;
        }
    }
    if ((_clean_condition) && (iteratii != 1)) {
        PIFOwner->Errors.Add(new AnsiException(ERR1100, BASE_AE ? BASE_AE->_DEBUG_INFO_LINE : 0, 1100, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
    }
    PIFList->Insert(newAE, ++PIF_POSITION, DATA_ANALIZER_ELEMENT);
    if (BASE_AE) {
        delete BASE_AE;
    }
    return 0;
}

INTEGER Optimizer::OptimizeExpression(TempVariableManager *TVM, INTEGER ID, INTEGER TYPE, INTEGER IS_PARAM_LIST, INTEGER FIRST_CALL, INTEGER FLAGS, AnalizerElement *PREC_METHOD) {
    INTEGER         result         = 0;
    INTEGER         START_POSITION = PIF_POSITION;
    INTEGER         PREC_TYPE      = -1;
    INTEGER         PREC_ID        = -1;
    INTEGER         VALID          = 0;
    AnalizerElement *AE            = 0;
    AnalizerElement *PREC_AE       = 0;
    INTEGER         ITERATII       = 0;
    INTEGER         LAST_LINE      = -1;
    INTEGER         PREC_PREC_TYPE = -1;
    INTEGER         PREC_PREC_ID   = -1;
    AnalizerElement *MARKER        = 0;
#ifdef OPTIONAL_SEPARATOR
    INTEGER         VALID_EXPR     = 1;
#endif
    _clean_condition = 0;

    char is_array   = (FLAGS == KEY_INDEX_CLOSE) ? 1 : 0;
    char is_for_exp = (FLAGS == KEY_COMMA) ? 1 : 0;

    while (PIF_POSITION < PIFList->Count()) {
        AE = (AnalizerElement *)PIFList->Item(PIF_POSITION++);
        if (AE) {
#ifdef OPTIONAL_SEPARATOR
            if (!PIFOwner->STRICT_MODE) {
                if ((!FLAGS) && (TYPE == TYPE_SEPARATOR) && (ID == KEY_SEP)) {
                    if ((PREC_AE) && (AE->TYPE != TYPE_SEPARATOR)) {
                        if ((LAST_LINE >= 0) && (LAST_LINE != AE->_DEBUG_INFO_LINE)) {
                            // line changed
                            if (PREC_AE->TYPE == TYPE_OPERATOR) {
                                int loop_exit = 0;
                                switch (GetOperatorType(PREC_AE->ID)) {
                                    case OPERATOR_BINARY:
                                        if (AE->TYPE == TYPE_OPERATOR) {
                                            int type = GetOperatorType(AE->ID);
                                            if (type != OPERATOR_UNARY) {
                                                VALID_EXPR = 0;
                                                loop_exit = 1;
                                            }
                                        }
                                        break;
                                    case OPERATOR_UNARY:
                                    case OPERATOR_UNARY_LEFT:
                                        loop_exit = 1;
                                        break;
                                    case OPERATOR_RESOLUTION:
                                        break;
                                }
                                if (loop_exit)
                                    break;
                            } else
                            if (AE->TYPE != TYPE_OPERATOR)
                                break;
                        }
                    }
                }
            }
#endif
            LAST_LINE = AE->_DEBUG_INFO_LINE;
        }

        if (is_array) {
            if (AE->TYPE == TYPE_OPERATOR) {
                if (AE->ID == KEY_INDEX_CLOSE) {
                    AE = 0;
                    if (ITERATII) {
                        VALID = 1;
                    }
                    PIF_POSITION--;
                    break;
                } else
                if ((AE->ID == KEY_ARR_KEY) || (AE->ID == KEY_CND_2)) {
                    _clean_condition = 1;
                    PIF_POSITION--;
                    MARKER = (AnalizerElement *)PIFList->Item(PIF_POSITION);
                    AE     = 0;
                    VALID  = 1;
                    break;
                }
            }
        }

        if ((AE->ID == ID) && (AE->TYPE == TYPE)) {
            // sa se termine ...
            _clean_condition = 1;
            PIFList->Delete(--PIF_POSITION);
            MARKER = (AnalizerElement *)PIFList->Item(PIF_POSITION);
            AE     = 0;
            VALID  = 1;
            break;
        }
        ITERATII++;

        if (((AE->TYPE == TYPE_OPERATOR) && (AE->ID == KEY_ASG)) && ((TYPE == TYPE_OPERATOR) && ((ID == KEY_P_CLOSE) || (ID == KEY_INDEX_CLOSE)))) {
            if (!NO_WARNING_ATTR) {
                PIFOwner->Warning(WRN10001, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 10001, "", _DEBUG_INFO_FILENAME);
            }
        }

        if (((AE->TYPE == TYPE_SEPARATOR) && (AE->ID == KEY_SEP)) || (AE->TYPE == TYPE_KEYWORD)) {
            if (!VALID) {
                if (is_for_exp) {
                    _clean_condition = 1;
                    PIFList->Delete(--PIF_POSITION);
                    AE     = 0;
                    VALID  = 1;
                    result = -KEY_COMMA;
                }
            }
            break;
        }

        if (AE->TYPE == TYPE_OPERATOR) {
            if (AE->ID == KEY_P_OPEN) {
                PUSH_CHAR(_clean_condition);
                PIFList->Delete(--PIF_POSITION);
                int is_param_list = OptimizeExpression(TVM, KEY_P_CLOSE,
                                                       TYPE_OPERATOR,
                                                       ((PREC_TYPE == TYPE_METHOD) || (PREC_TYPE == TYPE_CLASS)) ||
                                                       ((PREC_TYPE == TYPE_VARIABLE) && (PREC_PREC_ID == KEY_DLL_CALL)),
                                                       0,
                                                       //===========//
                                                       0, PREC_AE
                                                       //===========//
                                                       );

                POP_CHAR(_clean_condition);
                continue;
            } else
            if ((AE->ID == KEY_P_CLOSE) && (is_for_exp)) {
                _clean_condition = 1;
                PIFList->Delete(--PIF_POSITION);
                AE     = 0;
                VALID  = 1;
                result = -KEY_COMMA;
                break;
            } else
            if (AE->ID == KEY_INDEX_OPEN) {
                if ((PREC_TYPE == TYPE_KEYWORD) ||
                    (PREC_TYPE == TYPE_OPERATOR) ||
                    (PREC_TYPE == TYPE_SEPARATOR) ||
                    (PREC_TYPE == -1)) {
                    OptimizeArray(TVM);
                    if (is_array) {
                        _clean_condition = 0;
                    }
                } else {
                    OptimizeExpression(TVM, KEY_INDEX_CLOSE, TYPE_OPERATOR, 0, 0);
                    if (is_array) {
                        _clean_condition = 0;
                    }
                }
                continue;
            }
        }
        PREC_PREC_TYPE = PREC_TYPE;
        PREC_PREC_ID   = PREC_ID;
        PREC_TYPE      = AE->TYPE;
        PREC_ID        = AE->ID;

        PREC_AE = AE;
    }

#ifdef OPTIONAL_SEPARATOR
    if ((!PIFOwner->STRICT_MODE) && (!VALID) && (VALID_EXPR)) {
        if ((AE) && (TYPE == TYPE_SEPARATOR) && (ID == KEY_SEP) && (!MARKER)) {
            _clean_condition = 1;
            PIF_POSITION--;
            MARKER = (AnalizerElement *)PIFList->Item(PIF_POSITION);
            AE     = 0;
            VALID  = 1;
        }
    }
#endif
    if (!VALID) {
        AE = 0;
        if ((TYPE == TYPE_OPERATOR) && (ID == KEY_P_CLOSE)) {
            PIFOwner->Errors.Add(new AnsiException(ERR500, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 500, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
        } else
        if ((TYPE == TYPE_OPERATOR) && (ID == KEY_INDEX_CLOSE)) {
            PIFOwner->Errors.Add(new AnsiException(ERR510, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 510, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
        } else
        if ((TYPE == TYPE_SEPARATOR) && (ID == KEY_SEP)) {
            PIFOwner->Errors.Add(new AnsiException(ERR520, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 520, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
        } else {
            PIFOwner->Errors.Add(new AnsiException(ERR490, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 490, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
        }
        return -1;
    } else {
        AE = 0;
        if ((FLAGS == KEY_FOR) && (!ITERATII)) {
            VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
            VD->value  = NULL_STRING;
            VD->USED   = 1;
            VD->nValue = 1;
            VD->TYPE   = VARIABLE_NUMBER;
            VD->BY_REF = 0;
            VDList->Add(VD, DATA_VAR_DESCRIPTOR);
            return VDList->Count();
        }

        if ((!ITERATII) && (!IS_PARAM_LIST) && ((TYPE == TYPE_OPERATOR) && (ID == KEY_P_CLOSE))) {
            PIFOwner->Errors.Add(new AnsiException(ERR610, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 610, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            return -1;
            //}
        }
    }
    FLAGS = 0;

    INTEGER FIRST_OPERATOR;
    do {
        FIRST_OPERATOR = GetFirstOperator(PIFList, START_POSITION, PIF_POSITION);
        if (FIRST_OPERATOR == -1) {
            break;
        }

        AnalizerElement *AE = (AnalizerElement *)PIFList->Item(FIRST_OPERATOR);
        INTEGER OP_TYPE = GetOperatorType(AE->ID);
        AE->_INFO_OPTIMIZED = 1;
        INTEGER AE_ID = AE->ID;

        if ((OP_TYPE == OPERATOR_BINARY) || (OP_TYPE == OPERATOR_SELECTOR)) {
            AnalizerElement *Left  = (AnalizerElement *)PIFList->Item(FIRST_OPERATOR - 1);
            AnalizerElement *Right = (AnalizerElement *)PIFList->Item(FIRST_OPERATOR + 1);
            AnalizerElement *Parameter = (AnalizerElement *)PIFList->Item(FIRST_OPERATOR + 2);

            if ((AE_ID == KEY_P_OPEN) || (AE_ID == KEY_P_CLOSE) || (Right == MARKER) || (FIRST_OPERATOR <= START_POSITION)) {
                this->PIFOwner->Errors.Add(new AnsiException(ERR490, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 490, AE ? AE->_PARSE_DATA.c_str() : "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            }
            if ((Check(Left, AE)) || (Check(Right, AE))) {
                continue;
            }

            int tmp_index;
            if ((!IS_PARAM_LIST) && (((AE_ID == KEY_SEL) || (AE_ID == KEY_DLL_CALL)) && (Parameter) && (Parameter->TYPE == TYPE_PARAM_LIST))) {
                tmp_index = TVM->GetVar();
            } else {
                VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
                VD->BY_REF = 0;
                VD->value  = NULL_STRING;
                VD->nValue = 0;
                VD->USED   = -2;

                VD->TYPE = VARIABLE_NUMBER;
                //}
                VDList->Add(VD, DATA_VAR_DESCRIPTOR);
                tmp_index = VDList->Count();
            }

            AnalizerElement *newAE = new AnalizerElement;
            newAE->ID               = tmp_index;
            newAE->TYPE             = TYPE_VARIABLE;
            newAE->_DEBUG_INFO_LINE = AE->_DEBUG_INFO_LINE;
            newAE->_INFO_OPTIMIZED  = 1;
            newAE->_HASH_DATA       = 0;

            OptimizedElement *OE = new OptimizedElement;
            newAE->_HASH_DATA = OE;

            OE->Result_ID = newAE->ID;

            // remove LEFT operator RIGHT
            int idx             = FIRST_OPERATOR - 1;
            AnalizerElement *a1 = (AnalizerElement *)PIFList->Remove(idx);
            AnalizerElement *a2 = (AnalizerElement *)PIFList->Remove(idx);
            AnalizerElement *a3 = (AnalizerElement *)PIFList->Remove(idx);

            CopyElement((AnalizerElement *)a1, &OE->OperandLeft);
            CopyElement((AnalizerElement *)a2, &OE->Operator);
            CopyElement((AnalizerElement *)a3, &OE->OperandRight);
            delete a1;
            delete a2;
            delete a3;
            PIFList->Insert(newAE, idx, DATA_ANALIZER_ELEMENT);

            PIF_POSITION -= 2;

            if (AE_ID == KEY_DLL_CALL) {
                if ((Parameter) && (Parameter->TYPE == TYPE_PARAM_LIST)) {
                    if ((OE->OperandLeft.ID != STATIC_CLASS_DLL) &&
                        (OE->OperandLeft.ID != STATIC_CLASS_DELEGATE)) {
                        if (OE->OperandLeft.TYPE == TYPE_CLASS) {
                            char *static_name = PIFOwner->GeneralMembers->Item(OE->OperandRight.ID - 1);
                            if (!((ClassCode *)PIFOwner->ClassList->Item(OE->OperandLeft.ID - 1))->CanBeRunStatic(static_name)) {
                                AE = 0;
                                PIFOwner->Errors.Add(new AnsiException(ERR950, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 950, AnsiString(static_name), _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                            }
                        } else {
                            AE = 0;
                            PIFOwner->Errors.Add(new AnsiException(ERR1220, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 1220, AnsiString(""), _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                        }
                    }
                    CopyElement(Parameter, &OE->OperandReserved);
                    PIF_POSITION--;
                    PIFList->Delete(FIRST_OPERATOR);
                } else {
                    AE = 0;
                    PIFOwner->Errors.Add(new AnsiException(ERR680, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 680, (AE ? AE->_PARSE_DATA : AnsiString()), _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                }
            } else
            if ((AE_ID == KEY_SEL) && (Parameter)) {
                if (Parameter->TYPE == TYPE_PARAM_LIST) {
                    AnsiList    *pc = (AnsiList *)this->ParameterList->Item(Parameter->ID - 1);
                    int         minp, maxp;
                    ClassMember *targetCM = 0;
                    if (!TryCheckParameters(OE, pc ? pc->Count() : 0, &minp, &maxp, &targetCM, this->_CLASS)) {
                        AnsiString szparameters = "none";
                        AnsiString target_name;
                        if (targetCM) {
                            target_name = targetCM->NAME;
                        } else {
                            target_name = "(none)";
                        }
                        if (pc) {
                            szparameters = AnsiString((intptr_t)pc->Count());
                        }

                        PIFOwner->Errors.Add(new AnsiException(ERR210, OE->Operator._DEBUG_INFO_LINE, 210, target_name + AnsiString(" requires ") + AnsiString(maxp) + AnsiString(", with ") + AnsiString(minp) + AnsiString(" non default parameters, received ") + szparameters, _DEBUG_INFO_FILENAME, this->_CLASS->NAME, this->_MEMBER), DATA_EXCEPTION);
                    }
                    CopyElement(Parameter, &OE->OperandReserved);
                    PIF_POSITION--;
                    PIFList->Delete(FIRST_OPERATOR);
                } else
                if ((Parameter->TYPE == TYPE_OPERATOR) && ((Parameter->ID == KEY_ASG) || (Parameter->ID == KEY_BY_REF))) {
                    MAKE_NULL(OE->OperandReserved);
                    if (FIRST_OPERATOR < PIF_POSITION) {
                        OE->OperandReserved.TYPE = -1;
                    }
                } else {
                    MAKE_NULL(OE->OperandReserved);
                }
            } else {
               MAKE_NULL(OE->OperandReserved);
            }
        }
        else if ((OP_TYPE == OPERATOR_UNARY) || (OP_TYPE == OPERATOR_UNARY_LEFT)) {
            AnalizerElement *Target = (AnalizerElement *)PIFList->Item(FIRST_OPERATOR + (OP_TYPE == OPERATOR_UNARY ? 1 : -1));
            AnalizerElement *Parameter = (OP_TYPE == OPERATOR_UNARY) ? (AnalizerElement *)PIFList->Item(FIRST_OPERATOR + 2) : 0;

            if (((OP_TYPE == OPERATOR_UNARY) && (Target == MARKER)) || ((OP_TYPE == OPERATOR_UNARY_LEFT) && (FIRST_OPERATOR <= START_POSITION)))
                this->PIFOwner->Errors.Add(new AnsiException(ERR490, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 490, AE ? AE->_PARSE_DATA.c_str() : "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);

            if (Check(Target, AE)) {
                return -1;
            }

            int tmp_index;

            VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
            VD->BY_REF = 0;
            VD->value  = NULL_STRING;
            VD->nValue = 0;
            VD->USED   = -2;

            VD->TYPE = VARIABLE_NUMBER;

            VDList->Add(VD, DATA_VAR_DESCRIPTOR);
            tmp_index = VDList->Count();
            //}

            AnalizerElement *newAE = new AnalizerElement;
            newAE->ID               = tmp_index;
            newAE->TYPE             = TYPE_VARIABLE;
            newAE->_DEBUG_INFO_LINE = Target->_DEBUG_INFO_LINE;
            newAE->_INFO_OPTIMIZED  = 1;

            OptimizedElement *OE = new OptimizedElement;
            newAE->_HASH_DATA = OE;

            OE->Result_ID = newAE->ID;

            CopyElement((AnalizerElement *)PIFList->Item(FIRST_OPERATOR), &OE->Operator);
            PIFList->Delete(FIRST_OPERATOR);

            CopyElement((AnalizerElement *)PIFList->Item(FIRST_OPERATOR - (OP_TYPE == OPERATOR_UNARY ? 0 : 1)), &OE->OperandLeft);
            PIFList->Delete(FIRST_OPERATOR - (OP_TYPE == OPERATOR_UNARY ? 0 : 1));

            PIFList->Insert(newAE, FIRST_OPERATOR - (OP_TYPE == OPERATOR_UNARY ? 0 : 1), DATA_ANALIZER_ELEMENT);
            PIF_POSITION--;
            if ((AE_ID == KEY_NEW) && (Parameter) && (Parameter->TYPE == TYPE_PARAM_LIST)) {
                CopyElement(Parameter, &OE->OperandReserved);
                PIF_POSITION--;
                PIFList->Delete(FIRST_OPERATOR + 1);

                AnsiList *pc = (AnsiList *)this->ParameterList->Item(OE->OperandReserved.ID - 1);
                int      minp, maxp;
                if (!TryCheckParameters(OE, pc ? pc->Count() : 0, &minp, &maxp)) {
                    AnsiString szparameters = "none";
                    if (pc) {
                        szparameters = AnsiString((intptr_t)pc->Count());
                    }

                    ClassCode *CC = (ClassCode *)PIFOwner->ClassList->Item(OE->OperandLeft.ID - 1);
                    PIFOwner->Errors.Add(new AnsiException(ERR220, OE->Operator._DEBUG_INFO_LINE, 220, (CC ? CC->NAME : AnsiString()) + AnsiString(" requires ") + AnsiString(maxp) + AnsiString(", with ") + AnsiString(minp) + AnsiString(" non default parameters, received ") + szparameters, _DEBUG_INFO_FILENAME, this->_CLASS->NAME, this->_MEMBER), DATA_EXCEPTION);
                }
            } else {
                MAKE_NULL(OE->OperandReserved);
            }

            CopyElement(0, &OE->OperandRight);
        } else
        if ((OP_TYPE == KEY_COMMA) && (IS_PARAM_LIST)) {
            // am lista de parametri ...
        }
    } while (FIRST_OPERATOR > -1);

    // codintie pusa "pe observatie" ... atentie !!
    if ((PIF_POSITION > START_POSITION + 1) && (!IS_PARAM_LIST) ) {
        PIFOwner->Errors.Add(new AnsiException(ERR490, AE ? AE->_DEBUG_INFO_LINE : LAST_LINE, 490, "", _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
    }

    if (FIRST_CALL) {
        AnalizerElement *AE = (AnalizerElement *)PIFList->Item(PIF_POSITION - 1);
        Require(AE);
    }

    if (IS_PARAM_LIST) {
        BuildParameterList(START_POSITION, PREC_METHOD);
        return TYPE_PARAM_LIST;
    }
    return result;
}

INTEGER Optimizer::OptimizeKeyWord(TempVariableManager *TVM, INTEGER ID, INTEGER TYPE) {
    INTEGER            STATAMENT_POS;
    INTEGER            ITER_POS;
    AnalizerElement    *AE = (AnalizerElement *)PIFList->Item(PIF_POSITION++);
    AnalizerElement    *tempAE;
    AnsiString         FileName           = _DEBUG_INFO_FILENAME;
    INTEGER            Line               = AE->_DEBUG_INFO_LINE;
    INTEGER            result             = 0;
    OptimizedElement   *OE                = 0;
    OptimizedElement   *OEgoto            = 0;
    INTEGER            NOT_TRUE_POSITION  = 0;
    INTEGER            TEMP_STATAMENT_POS = 0;
    char               temp_result;
    AnsiList           *PUSHED_BREAK_Elements    = BREAK_Elements;
    AnsiList           *PUSHED_CONTINUE_Elements = CONTINUE_Elements;
    AnsiList           *PUSHED_OptimizedPIF      = 0;
    AnsiList           *TEMP_OptimizedPIF        = 0;
    VariableDESCRIPTOR *VDTemp;

    switch (AE->ID) {
        case KEY_CONTINUE:
            if (!CONTINUE_Elements) {
                PIFOwner->Errors.Add(new AnsiException(ERR600, AE->_DEBUG_INFO_LINE, 600, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                return 0;
            }
            OEgoto = new OptimizedElement;
            OEgoto->Operator._DEBUG_INFO_LINE = Line;
            OEgoto->Operator.TYPE        = TYPE_OPTIMIZED_KEYWORD;
            OEgoto->Operator.ID          = KEY_OPTIMIZED_GOTO;
            OEgoto->Result_ID            = 0;
            MAKE_NULL(OEgoto->OperandLeft);
            MAKE_NULL(OEgoto->OperandRight);
            OEgoto->OperandReserved.ID   = OptimizedPIF->Count() + 1;
            OEgoto->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;
            OptimizedPIF->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);
            CONTINUE_Elements->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);

#ifdef OPTIONAL_SEPARATOR
            if (PIFOwner->STRICT_MODE) {
                if ((((AnalizerElement *)PIFList->Item(PIF_POSITION))->TYPE != TYPE_SEPARATOR) ||
                    (((AnalizerElement *)PIFList->Item(PIF_POSITION))->ID != KEY_SEP)) {
                    PIFOwner->Errors.Add(new AnsiException(ERR580, AE->_DEBUG_INFO_LINE, 580, ((AnalizerElement *)PIFList->Item(PIF_POSITION))->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                    return 0;
                }
                PIF_POSITION++;
            } else
            if ((((AnalizerElement *)PIFList->Item(PIF_POSITION))->TYPE == TYPE_SEPARATOR) && (((AnalizerElement *)PIFList->Item(PIF_POSITION))->ID == KEY_SEP))
                PIF_POSITION++;
#else
            if ((((AnalizerElement *)PIFList->Item(PIF_POSITION))->TYPE != TYPE_SEPARATOR) ||
                (((AnalizerElement *)PIFList->Item(PIF_POSITION))->ID != KEY_SEP)) {
                PIFOwner->Errors.Add(new AnsiException(ERR580, AE->_DEBUG_INFO_LINE, 580, ((AnalizerElement *)PIFList->Item(PIF_POSITION))->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                return 0;
            }
            PIF_POSITION++;
#endif
            break;

        case KEY_BREAK:
            if (!BREAK_Elements) {
                PIFOwner->Errors.Add(new AnsiException(ERR590, AE->_DEBUG_INFO_LINE, 590, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                return 0;
            }
            OEgoto = new OptimizedElement;
            OEgoto->Operator._DEBUG_INFO_LINE = Line;
            OEgoto->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OEgoto->Operator.ID   = KEY_OPTIMIZED_GOTO;
            OEgoto->Result_ID     = 0;
            MAKE_NULL(OEgoto->OperandLeft);
            MAKE_NULL(OEgoto->OperandRight);
            OEgoto->OperandReserved.ID   = OptimizedPIF->Count() + 1;
            OEgoto->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;
            OptimizedPIF->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);
            BREAK_Elements->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);

            tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
#ifdef OPTIONAL_SEPARATOR
            if (PIFOwner->STRICT_MODE) {
                if ((tempAE->TYPE != TYPE_SEPARATOR) || (tempAE->ID != KEY_SEP)) {
                    PIFOwner->Errors.Add(new AnsiException(ERR580, AE->_DEBUG_INFO_LINE, 580, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                    return 0;
                }
                PIF_POSITION++;
            } else
            if ((tempAE->TYPE == TYPE_SEPARATOR) && (tempAE->ID == KEY_SEP))
                PIF_POSITION++;
#else
            if ((tempAE->TYPE != TYPE_SEPARATOR) || (tempAE->ID != KEY_SEP)) {
                PIFOwner->Errors.Add(new AnsiException(ERR580, AE->_DEBUG_INFO_LINE, 580, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                return 0;
            }
            PIF_POSITION++;
#endif
            break;

        case KEY_END:
            return KEY_END;

        case KEY_BEGIN:
            while ((result != -1) && (result != KEY_END)) {
                result = OptimizeAny(TVM);
            }
            break;

        case KEY_IF:
            tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);

            if ((!tempAE) || (tempAE->TYPE != TYPE_OPERATOR) || (tempAE->ID != KEY_P_OPEN)) {
                PIFOwner->Errors.Add(new AnsiException(ERR1201, AE->_DEBUG_INFO_LINE, 1201, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            }

            PIFList->Delete(PIF_POSITION);

            STATAMENT_POS = OptimizedPIF->Count();
            OptimizeExpression(TVM, KEY_P_CLOSE, TYPE_OPERATOR);

            OE = new OptimizedElement;
            OE->Operator._DEBUG_INFO_LINE = Line;
            OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OE->Operator.ID   = KEY_OPTIMIZED_IF;
            MAKE_NULL(OE->OperandLeft);
            CopyElement((AnalizerElement *)PIFList->Item(PIF_POSITION - 1), &OE->OperandRight);
            OE->Result_ID = 0;
            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);

            OptimizeAny(TVM);

            OE->OperandReserved.ID   = OptimizedPIF->Count();
            OE->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;

            AE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
            if ((AE) && (AE->TYPE == TYPE_KEYWORD) && (AE->ID == KEY_ELSE)) {
                PIF_POSITION++;
                OE->OperandReserved.ID++;

                OEgoto = new OptimizedElement;
                OEgoto->Operator._DEBUG_INFO_LINE = Line;
                OEgoto->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
                OEgoto->Operator.ID   = KEY_OPTIMIZED_GOTO;
                OEgoto->Result_ID     = 0;
                MAKE_NULL(OEgoto->OperandLeft);
                MAKE_NULL(OEgoto->OperandRight);

                OEgoto->OperandReserved.ID   = 0;
                OEgoto->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;
                OptimizedPIF->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);
                
                AE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
                if ((!AE) || ((AE->TYPE == TYPE_KEYWORD) && (AE->ID == KEY_END)))
                    PIFOwner->Errors.Add(new AnsiException(ERR531, AE->_DEBUG_INFO_LINE, 531, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);

                OptimizeAny(TVM);
                OEgoto->OperandReserved.ID = OptimizedPIF->Count();
            }
            break;

        case KEY_ELSE:
            PIFOwner->Errors.Add(new AnsiException(ERR530, AE->_DEBUG_INFO_LINE, 530, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            return 0;

        case KEY_CATCH:
            PIFOwner->Errors.Add(new AnsiException(ERR560, AE->_DEBUG_INFO_LINE, 560, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            return 0;

        case KEY_ECHO:
            STATAMENT_POS = OptimizedPIF->Count();
            OptimizeExpression(TVM, ID, TYPE);

            OE = new OptimizedElement;
            OE->Operator._DEBUG_INFO_LINE = Line;
            OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OE->Operator.ID   = KEY_OPTIMIZED_ECHO;

            //--------------------------------//
            CopyElement(0, &OE->OperandLeft);
            //--------------------------------//
            CopyElement((AnalizerElement *)PIFList->Item(PIF_POSITION - 1), &OE->OperandRight);
            OE->Result_ID = 0;
            MAKE_NULL(OE->OperandReserved);
            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);
            break;

        case KEY_RETURN:
            if (PIFOwner->PROFILE_DRIVEN)
                AddProfilerCode(1);

            STATAMENT_POS = OptimizedPIF->Count();
            tempAE        = (AnalizerElement *)PIFList->Item(PIF_POSITION);

#ifdef OPTIONAL_SEPARATOR
            if ((tempAE) && (((tempAE->TYPE == TYPE) && (tempAE->ID == ID)) || ((!PIFOwner->STRICT_MODE) && (tempAE->_DEBUG_INFO_LINE != AE->_DEBUG_INFO_LINE)))) {
#else
            if ((tempAE) && (tempAE->TYPE == TYPE) && (tempAE->ID == ID)) {
#endif
                VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
                VD->USED   = 1;
                VD->nValue = 0;
                VD->BY_REF = 0;
                VD->TYPE   = VARIABLE_NUMBER;
                VDList->Add(VD, DATA_VAR_DESCRIPTOR);

                AnalizerElement *newAE = new AnalizerElement;
                newAE->ID               = VDList->Count();
                newAE->TYPE             = TYPE_VARIABLE;
                newAE->_DEBUG_INFO_LINE = tempAE->_DEBUG_INFO_LINE;
                newAE->_INFO_OPTIMIZED  = 1;
                newAE->_HASH_DATA       = 0;

                PIFList->Delete(PIF_POSITION);

                PIFList->Insert(newAE, PIF_POSITION, DATA_ANALIZER_ELEMENT);
                PIF_POSITION++;
            } else {
                OptimizeExpression(TVM, ID, TYPE);
            }

            OE = new OptimizedElement;
            OE->Operator._DEBUG_INFO_LINE = Line;
            OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OE->Operator.ID   = KEY_OPTIMIZED_RETURN;
            //--------------------------------//
            CopyElement(0, &OE->OperandLeft);
            //--------------------------------//

            CopyElement((AnalizerElement *)PIFList->Item(PIF_POSITION - 1), &OE->OperandRight);
            OE->Result_ID = 0;
            MAKE_NULL(OE->OperandReserved);
            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);
            break;

        case KEY_TRY:
            TEMP_STATAMENT_POS = this->CATCH_ELEMENT;

            STATAMENT_POS = OptimizedPIF->Count();

            OE = new OptimizedElement;
            OE->Operator._DEBUG_INFO_LINE = Line;
            OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OE->Operator.ID   = KEY_OPTIMIZED_TRY_CATCH;
            OE->Result_ID     = 0;

            //----------------------------------------------

            //----------------------------------------------
            CopyElement(0, &OE->OperandRight);
            //----------------------------------------------

            CopyElement(0, &OE->OperandLeft);
            OE->OperandLeft.ID = this->CATCH_ELEMENT;
            //----------------------------------------------
            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);

            this->CATCH_ELEMENT = OptimizedPIF->Count();

            OptimizeAny(TVM);

            tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
            if ((tempAE->TYPE != TYPE_KEYWORD) || (tempAE->ID != KEY_CATCH)) {
                PIFOwner->Errors.Add(new AnsiException(ERR540, AE->_DEBUG_INFO_LINE, 540, ((AnalizerElement *)PIFList->Item(PIF_POSITION))->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                return 0;
            }
            PIF_POSITION++;
            PIFList->Delete(PIF_POSITION);
            OptimizeExpression(TVM, KEY_P_CLOSE, TYPE_OPERATOR);

            tempAE        = ((AnalizerElement *)PIFList->Item(PIF_POSITION - 1));
            OE->Result_ID = ((AnalizerElement *)PIFList->Item(PIF_POSITION - 1))->ID;
            VDTemp        = (VariableDESCRIPTOR *)VDList->Item(OE->Result_ID - 1);
            if (VDTemp->BY_REF == 2)
                PIFOwner->Errors.Add(new AnsiException(ERR541, AE->_DEBUG_INFO_LINE, 541, tempAE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);

            STATAMENT_POS = OptimizedPIF->Count() + 1;

            // setez GO-TO-ul
            OEgoto = new OptimizedElement;
            OEgoto->Operator._DEBUG_INFO_LINE = Line;
            OEgoto->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OEgoto->Operator.ID   = KEY_OPTIMIZED_GOTO;
            OEgoto->Result_ID     = 0;
            MAKE_NULL(OEgoto->OperandLeft);
            MAKE_NULL(OEgoto->OperandRight);

            OEgoto->OperandReserved.ID   = 0;
            OEgoto->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;

            OptimizedPIF->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);

            OE->OperandReserved.ID   = STATAMENT_POS;
            OE->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;

            OptimizeAny(TVM);

            OE = new OptimizedElement;
            OE->Operator._DEBUG_INFO_LINE = Line;
            OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OE->Operator.ID   = KEY_OPTIMIZED_END_CATCH;

            this->CATCH_ELEMENT = TEMP_STATAMENT_POS;

            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);

            OEgoto->OperandReserved.ID = OptimizedPIF->Count() - 1;
            break;

        case KEY_THROW:
            if (PIFOwner->PROFILE_DRIVEN)
                AddProfilerCode(2);
            STATAMENT_POS = OptimizedPIF->Count();
            OptimizeExpression(TVM, ID, TYPE);

            OE = new OptimizedElement;
            OE->Operator._DEBUG_INFO_LINE = Line;
            OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OE->Operator.ID   = KEY_OPTIMIZED_THROW;
            MAKE_NULL(OE->OperandLeft);
            //--------------------------------//
            //--------------------------------//

            CopyElement((AnalizerElement *)PIFList->Item(PIF_POSITION - 1), &OE->OperandRight);
            OE->Result_ID = 0;
            MAKE_NULL(OE->OperandReserved);
            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);
            break;

        case KEY_DO:
            STATAMENT_POS     = OptimizedPIF->Count();
            CONTINUE_Elements = new AnsiList(0);
            BREAK_Elements    = new AnsiList(0);
            OptimizeAny(TVM);

            OE = new OptimizedElement;
            OE->Operator._DEBUG_INFO_LINE = Line;
            OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OE->Operator.ID   = KEY_OPTIMIZED_IF;
            MAKE_NULL(OE->OperandLeft);
            //--------------------------------//
            //--------------------------------//

            OE->Result_ID = 0;

            tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
            if ((!tempAE) || (tempAE->TYPE != TYPE_KEYWORD) ||
                (tempAE->ID != KEY_WHILE)) {
                PIFOwner->Errors.Add(new AnsiException(ERR550, AE->_DEBUG_INFO_LINE, 550, tempAE ? tempAE->_PARSE_DATA : AE->_PARSE_DATA, _DEBUG_INFO_FILENAME), DATA_EXCEPTION);
                delete OE;
                return 0;
            }
            PIF_POSITION++;

            tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);

            if ((!tempAE) || (tempAE->TYPE != TYPE_OPERATOR) || (tempAE->ID != KEY_P_OPEN)) {
                PIFOwner->Errors.Add(new AnsiException(ERR1201, AE->_DEBUG_INFO_LINE, 1201, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            }

            PIFList->Delete(PIF_POSITION);
            TEMP_STATAMENT_POS = OptimizedPIF->Count();
            OptimizeExpression(TVM, KEY_P_CLOSE, TYPE_OPERATOR);

            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);

            CopyElement((AnalizerElement *)PIFList->Item(PIF_POSITION - 1), &OE->OperandRight);

            NOT_TRUE_POSITION        = OptimizedPIF->Count() + 1;
            OE->OperandReserved.ID   = NOT_TRUE_POSITION;
            OE->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;

            // setez GO-TO-ul
            OEgoto = new OptimizedElement;
            OEgoto->Operator._DEBUG_INFO_LINE = Line;
            OEgoto->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OEgoto->Operator.ID   = KEY_OPTIMIZED_GOTO;
            OEgoto->Result_ID     = 0;
            MAKE_NULL(OEgoto->OperandLeft);
            MAKE_NULL(OEgoto->OperandRight);

            OEgoto->OperandReserved.ID   = STATAMENT_POS;
            OEgoto->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;
            OptimizedPIF->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);

            END_WHILE_CYCLE(TEMP_STATAMENT_POS, NOT_TRUE_POSITION);
            NO_WARNING_EMPTY = 1;
            tempAE           = (AnalizerElement *)PIFList->Item(PIF_POSITION);
#ifdef OPTIONAL_SEPARATOR
            if (PIFOwner->STRICT_MODE) {
                if ((!tempAE) || (tempAE->TYPE != TYPE_SEPARATOR) || (tempAE->ID != KEY_SEP))
                    PIFOwner->Errors.Add(new AnsiException(ERR340, tempAE->_DEBUG_INFO_LINE, 340, tempAE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            } else
            if ((tempAE->TYPE != TYPE_SEPARATOR) || (tempAE->ID != KEY_SEP))
                PIF_POSITION--;
#else
            if ((!tempAE) || (tempAE->TYPE != TYPE_SEPARATOR) || (tempAE->ID != KEY_SEP)) {
                PIFOwner->Errors.Add(new AnsiException(ERR340, tempAE->_DEBUG_INFO_LINE, 340, tempAE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            }
#endif
            break;

        case KEY_WHILE:
            tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);

            if ((!tempAE) || (tempAE->TYPE != TYPE_OPERATOR) || (tempAE->ID != KEY_P_OPEN)) {
                PIFOwner->Errors.Add(new AnsiException(ERR1201, AE->_DEBUG_INFO_LINE, 1201, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            }

            PIFList->Delete(PIF_POSITION);

            STATAMENT_POS = OptimizedPIF->Count();
            OptimizeExpression(TVM, KEY_P_CLOSE, TYPE_OPERATOR);

            OE = new OptimizedElement;
            OE->Operator._DEBUG_INFO_LINE = Line;
            OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OE->Operator.ID   = KEY_OPTIMIZED_IF;
            MAKE_NULL(OE->OperandLeft);
            //--------------------------------//
            //--------------------------------//
            CopyElement((AnalizerElement *)PIFList->Item(PIF_POSITION - 1), &OE->OperandRight);
            OE->Result_ID = 0;

            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);

            CONTINUE_Elements = new AnsiList(0);
            BREAK_Elements    = new AnsiList(0);
            OptimizeAny(TVM);

            NOT_TRUE_POSITION        = OptimizedPIF->Count() + 1;
            OE->OperandReserved.ID   = NOT_TRUE_POSITION;
            OE->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;

            // setez GO-TO-ul
            OEgoto = new OptimizedElement;
            OEgoto->Operator._DEBUG_INFO_LINE = Line;
            OEgoto->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OEgoto->Operator.ID   = KEY_OPTIMIZED_GOTO;
            OEgoto->Result_ID     = 0;
            MAKE_NULL(OEgoto->OperandLeft);
            MAKE_NULL(OEgoto->OperandRight);

            OEgoto->OperandReserved.ID   = STATAMENT_POS;
            OEgoto->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;
            OptimizedPIF->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);

            END_WHILE_CYCLE(STATAMENT_POS, NOT_TRUE_POSITION);
            break;

        case KEY_FOR:
            tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);

            if ((!tempAE) || (tempAE->TYPE != TYPE_OPERATOR) || (tempAE->ID != KEY_P_OPEN)) {
                PIFOwner->Errors.Add(new AnsiException(ERR1201, AE->_DEBUG_INFO_LINE, 1201, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            }

            PIFList->Delete(PIF_POSITION);
            OptimizeForExpression(TVM);

            STATAMENT_POS = OptimizedPIF->Count();
            temp_result   = OptimizeExpression(TVM, KEY_SEP, TYPE_SEPARATOR, 0, 1, KEY_FOR);

            OE = new OptimizedElement;
            OE->Operator._DEBUG_INFO_LINE = Line;
            OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OE->Operator.ID   = KEY_OPTIMIZED_IF;
            MAKE_NULL(OE->OperandLeft);
            //--------------------------------//
            //--------------------------------//

            if (temp_result <= 0) {
                CopyElement((AnalizerElement *)PIFList->Item(PIF_POSITION - 1), &OE->OperandRight);
            } else {
                OE->OperandRight._DEBUG_INFO_LINE = 0;
                OE->OperandRight._HASH_DATA       = 0;
                OE->OperandRight._INFO_OPTIMIZED  = 0;
                OE->OperandRight.ID   = VDList->Count();
                OE->OperandRight.TYPE = TYPE_VARIABLE;
            }
            OE->Result_ID = 0;

            OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);

            TEMP_STATAMENT_POS = OptimizedPIF->Count() + 1;
            // ============ v2 ================== //
            PUSHED_OptimizedPIF = OptimizedPIF;
            OptimizedPIF        = new AnsiList(0);
            OptimizeForExpression(TVM, true);
            TEMP_OptimizedPIF   = OptimizedPIF;
            OptimizedPIF        = PUSHED_OptimizedPIF;
            PUSHED_OptimizedPIF = 0;
            // ========== end v2 ================ //

            CONTINUE_Elements = new AnsiList(0);
            BREAK_Elements    = new AnsiList(0);
            OptimizeAny(TVM);
            // =================== v2 ====================//
            ITER_POS = OptimizedPIF->Count();
            if (TEMP_OptimizedPIF) {
                int count       = TEMP_OptimizedPIF->Count();
                int delta_count = OptimizedPIF->Count();
                for (int i2 = 0; i2 < count; i2++) {
                    OptimizedElement *OE_P = (OptimizedElement *)TEMP_OptimizedPIF->Item(i2);
                    if ((OE_P) && (OE_P->Operator.TYPE == TYPE_OPTIMIZED_KEYWORD)) {
                        if ((OE_P->Operator.ID == KEY_OPTIMIZED_GOTO) || (OE_P->Operator.ID == KEY_OPTIMIZED_IF))
                            OE_P->OperandReserved.ID += delta_count;
                    }
                }
                OptimizedPIF->GetFromList(TEMP_OptimizedPIF);
                delete TEMP_OptimizedPIF;
            }
            // ================= end v2 ==================//
            NOT_TRUE_POSITION        = OptimizedPIF->Count() + 1;
            OE->OperandReserved.ID   = NOT_TRUE_POSITION;
            OE->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;

            // setez GO-TO-ul
            OEgoto = new OptimizedElement;
            OEgoto->Operator._DEBUG_INFO_LINE = Line;
            OEgoto->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
            OEgoto->Operator.ID   = KEY_OPTIMIZED_GOTO;
            OEgoto->Result_ID     = 0;
            MAKE_NULL(OEgoto->OperandLeft);
            MAKE_NULL(OEgoto->OperandRight);

            OEgoto->OperandReserved.ID   = STATAMENT_POS;
            OEgoto->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;
            OptimizedPIF->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);

            END_WHILE_CYCLE(ITER_POS, NOT_TRUE_POSITION);
            break;

        case KEY_SWITCH:
            PIF_POSITION--;
            PIFList->Delete(PIF_POSITION);
            OptimizeSwitch(TVM);
            break;

        default:
            PIFOwner->Errors.Add(new AnsiException(ERR570, AE->_DEBUG_INFO_LINE, 570, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            return 0;
    }
    return 0;
}

INTEGER Optimizer::OptimizeSwitch(TempVariableManager *TVM) {
    AnalizerElement *tempAE    = (AnalizerElement *)PIFList->Item(PIF_POSITION);
    AnalizerElement *newAE     = 0;
    AnalizerElement *EXPR_ELEM = 0;
    unsigned short _DEBUG_INFO_LINE = 0;
    if ((!tempAE) || (tempAE->TYPE != TYPE_OPERATOR) || (tempAE->ID != KEY_P_OPEN)) {
        PIFOwner->Errors.Add(new AnsiException(ERR1201, tempAE ? tempAE->_DEBUG_INFO_LINE : 0, 1201, (tempAE ? tempAE->_PARSE_DATA : AnsiString("")), _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
        if (!tempAE) {
            return -1;
        }
    }

    PIFList->Delete(PIF_POSITION);
    OptimizeExpression(TVM, KEY_P_CLOSE, TYPE_OPERATOR);

    AnalizerElement *AE_SWITCH_EXPR = (AnalizerElement *)PIFList->Item(PIF_POSITION - 1);

    tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);

    if ((!tempAE) || (tempAE->TYPE != TYPE_KEYWORD) || (tempAE->ID != KEY_BEGIN)) {
        PIFOwner->Errors.Add(new AnsiException(ERR1200, tempAE ? tempAE->_DEBUG_INFO_LINE : 0, 1200, (tempAE ? tempAE->_PARSE_DATA : AnsiString("")), _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
        if (!tempAE) {
            return -1;
        }
    }
    PIFList->Delete(PIF_POSITION);

    bool have_cases = true;

    AnsiList *PUSHED_BREAK_Elements = Optimizer::BREAK_Elements;
    Optimizer::BREAK_Elements = new AnsiList(0);
    OptimizedElement *OE;
    OptimizedElement *OEgoto;

    bool prec_is_case = false;
    while (have_cases) {
        AnalizerElement *AE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
        if ((!AE) || (AE->TYPE != TYPE_KEYWORD)) {
            PIFOwner->Errors.Add(new AnsiException(ERR1202, AE ? AE->_DEBUG_INFO_LINE : 0, 1202, (AE ? AE->_PARSE_DATA : AnsiString("")), _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
            break;
        }
        _DEBUG_INFO_LINE = AE->_DEBUG_INFO_LINE;
        switch (AE->ID) {
            case KEY_CASE:
                if (prec_is_case) {
                    OEgoto = new OptimizedElement;
                    OEgoto->Operator._DEBUG_INFO_LINE = _DEBUG_INFO_LINE;
                    OEgoto->Operator.TYPE        = TYPE_OPTIMIZED_KEYWORD;
                    OEgoto->Operator.ID          = KEY_OPTIMIZED_GOTO;
                    OEgoto->Result_ID            = 0;
                    MAKE_NULL(OEgoto->OperandLeft);
                    MAKE_NULL(OEgoto->OperandRight);
                    OEgoto->OperandReserved.ID   = 0;
                    OEgoto->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;
                    OptimizedPIF->Add(OEgoto, DATA_OPTIMIZED_ELEMENT);
                    OE->OperandReserved.ID++;
                }

                PIFList->Delete(PIF_POSITION);
                newAE = new AnalizerElement;

                CopyElement(AE_SWITCH_EXPR, newAE);
                PIFList->Insert(newAE, PIF_POSITION, DATA_ANALIZER_ELEMENT);

                newAE                   = new AnalizerElement;
                newAE->ID               = KEY_EQU;
                newAE->TYPE             = TYPE_OPERATOR;
                newAE->_PARSE_DATA      = (char *)EQU;
                newAE->_HASH_DATA       = 0;
                newAE->_DEBUG_INFO_LINE = _DEBUG_INFO_LINE;
                newAE->_INFO_OPTIMIZED  = 0;

                PIFList->Insert(newAE, PIF_POSITION + 1, DATA_ANALIZER_ELEMENT);

                OptimizeExpression(TVM, KEY_CND_2, TYPE_OPERATOR);

                EXPR_ELEM = (AnalizerElement *)PIFList->Item(PIF_POSITION - 1);

                OE = new OptimizedElement;
                OE->Operator._DEBUG_INFO_LINE = _DEBUG_INFO_LINE;
                OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
                OE->Operator.ID   = KEY_OPTIMIZED_IF;
                MAKE_NULL(OE->OperandLeft);
                CopyElement(EXPR_ELEM, &OE->OperandRight);
                OE->Result_ID = 0;
                OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);

                if (prec_is_case) {
                    OEgoto->OperandReserved.ID = OptimizedPIF->Count();
                }

                while (true) {
                    tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
                    if (!tempAE) {
                        break;
                    }

                    if ((tempAE->TYPE == TYPE_KEYWORD) &&
                        ((tempAE->ID == KEY_CASE) ||
                         (tempAE->ID == KEY_DEFAULT) ||
                         (tempAE->ID == KEY_END))) {
                        break;
                    }
                    OptimizeAny(TVM);
                }
                OE->OperandReserved.ID   = OptimizedPIF->Count();
                OE->OperandReserved.TYPE = TYPE_OPTIMIZED_JUMP_ADR;
                prec_is_case             = true;
                break;

            case KEY_DEFAULT:
                PIFList->Delete(PIF_POSITION);
                tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
                if ((!tempAE) || (tempAE->TYPE != TYPE_OPERATOR) || (tempAE->ID != KEY_CND_2)) {
                    PIFOwner->Errors.Add(new AnsiException(ERR1203, tempAE ? tempAE->_DEBUG_INFO_LINE : 0, 1203, (tempAE ? tempAE->_PARSE_DATA : AnsiString("")), _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                    END_SWITCH(0);
                    return -1;
                }
                PIFList->Delete(PIF_POSITION);

                while (true) {
                    tempAE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
                    if (!tempAE) {
                        break;
                    }

                    if ((tempAE->TYPE == TYPE_KEYWORD) &&
                        ((tempAE->ID == KEY_CASE) ||
                         (tempAE->ID == KEY_DEFAULT) ||
                         (tempAE->ID == KEY_END))) {
                        break;
                    }
                    OptimizeAny(TVM);
                }
                prec_is_case = false;
                break;

            case KEY_END:
                // end of cases ...
                PIFList->Delete(PIF_POSITION);
                have_cases = false;
                break;

            default:
                PIFOwner->Errors.Add(new AnsiException(ERR1202, AE->_DEBUG_INFO_LINE, 1202, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                END_SWITCH(0);
                return -1;
        }
    }
    END_SWITCH(OptimizedPIF->Count());
    return 0;
}

INTEGER Optimizer::OptimizeForExpression(TempVariableManager *TVM, bool is_increment) {
    while (1) {
        AnalizerElement *AE = (AnalizerElement *)PIFList->Item(PIF_POSITION);
        if (!AE) {
            return -1;
        }

        if (AE->TYPE == TYPE_SEPARATOR) {
            return 0;
        }

        NO_WARNING_EMPTY = 1;
        NO_WARNING_ATTR  = 1;
        int result = OptimizeExpression(TVM, KEY_COMMA, TYPE_OPERATOR, 0, 1, KEY_COMMA);
        NO_WARNING_EMPTY = 0;
        NO_WARNING_ATTR  = 0;
        if (result == -KEY_COMMA) {
            return 0;
        }
    }
    return 0;
}

INTEGER Optimizer::OptimizeAny(TempVariableManager *TVM, INTEGER ID, INTEGER TYPE, char FLAGS) {
    AnalizerElement *AE = (AnalizerElement *)PIFList->Item(PIF_POSITION);

    TVM->Reset();

    if (!AE) {
        return -1;
    }
    if (AE->TYPE == TYPE_SEPARATOR) {
        if (NO_WARNING_EMPTY) {
            NO_WARNING_EMPTY = 0;
        } else {
            PIFOwner->Warning(WRN10002, AE->_DEBUG_INFO_LINE, 10002, AE->_PARSE_DATA, _DEBUG_INFO_FILENAME);
        }
    } else if ((PIFOwner->DebugOn) && (AE->_DEBUG_INFO_LINE != LAST_DEBUG_TRAP)) {
        LAST_DEBUG_TRAP = AE->_DEBUG_INFO_LINE;
        OptimizedElement *OE = new OptimizedElement;
        OE->Operator._DEBUG_INFO_LINE = AE->_DEBUG_INFO_LINE;
        OE->Operator.TYPE = TYPE_OPTIMIZED_KEYWORD;
        OE->Operator.ID   = KEY_OPTIMIZED_DEBUG_TRAP;
        OptimizedPIF->Add(OE, DATA_OPTIMIZED_ELEMENT);
    }
    if (AE->TYPE != TYPE_KEYWORD) {
        return OptimizeExpression(TVM, ID, TYPE, 0, 1, FLAGS);
    } else {
        return OptimizeKeyWord(TVM, ID, TYPE);
    }
    return 0;
}

int Optimizer::Optimize() {
    TempVariableManager TVM(this->VDList);

    if (PIFList->Count() == 1) {
        NO_WARNING_EMPTY = 1;
    }
    int start_ref = -1;
    if (PIFOwner->PROFILE_DRIVEN)
        AddProfilerCode(0);

    while (PIF_POSITION < PIFList->Count())
        OptimizeAny(&TVM);

    if (PIFOwner->PROFILE_DRIVEN)
        AddProfilerCode(1);

    return 0;
}

int Optimizer::OptimizeMemoryUsage() {
    if ((!DATA) || (!CODE)) {
        return -1;
    }

    RuntimeOptimizedElement *CUR = 0;
    int index;
    RuntimeVariableDESCRIPTOR *VDDATA;
    char *USAGE_COUNT = 0;

    int optimisation_count = 0;
    if (dataCount) {
        USAGE_COUNT = new char [dataCount];
        for (INTEGER i = 0; i < dataCount; i++) {
            VDDATA          = &DATA [i];
            USAGE_COUNT [i] = (VDDATA->USED == (signed char)-2) ? 0 : 2;

            if (!USAGE_COUNT [i]) {
                optimisation_count++;
            }
        }
    }

    for (INTEGER i = 0; i < codeCount; i++) {
        CUR = &CODE [i];

        if (CUR->Operator.ID != KEY_OPTIMIZED_GOTO) {
            if (CUR->OperandLeft.TYPE == TYPE_VARIABLE) {
                index = CUR->OperandLeft.ID - 1;
                if ((index >= 0) && (index < dataCount) && (!USAGE_COUNT [index])) {
                    USAGE_COUNT [index] = 2;
                    optimisation_count--;
                }
            }
            if (CUR->OperandRight.TYPE == TYPE_VARIABLE) {
                index = CUR->OperandRight.ID - 1;
                if ((index >= 0) && (index < dataCount) && (!USAGE_COUNT [index])) {
                    USAGE_COUNT [index] = 2;
                    optimisation_count--;
                }
            }
        }
    }

    for (INTEGER k = 0; k < paramCount; k++) {
        ParamList *pl = &PARAMS [k];
        for (INTEGER j = 0; j < pl->COUNT; j++) {
            INTEGER index = DELTA_UNREF(pl, pl->PARAM_INDEX) [j];
            if (!USAGE_COUNT [index]) {
                USAGE_COUNT [index - 1] = 2;
                optimisation_count--;
            }
        }
    }

    int first_temp = -1;
    int delta      = 0;
    if (optimisation_count > 0) {
        for (INTEGER i = 0; i < codeCount; i++) {
            CUR = &CODE [i];
            if (CUR->Operator.TYPE == TYPE_OPERATOR) {
                index = CUR->Result_ID - 1;
                if ((index >= 0) && (index < dataCount) && (!USAGE_COUNT [index])) {
                    if ((CUR->Operator.ID != KEY_ASG) && (CUR->Operator.ID != KEY_BY_REF) &&
                        (CUR->Operator.ID != KEY_SEL) &&
                        (CUR->Operator.ID != KEY_DLL_CALL) && (CUR->Operator.ID != KEY_INDEX_OPEN)) {
                        VDDATA = &DATA [index];
                        if (first_temp == -1) {
                            first_temp = CUR->Result_ID;
                        }
                        delta++;
                        CUR->Result_ID = first_temp;
                    } else {
                        USAGE_COUNT [index] = 2;
                        optimisation_count--;
                    }
                }
            }
        }
    }

    //-------------------------------------------------------------//
    for (int ii = 0; ii < codeCount; ii++) {
        CUR = &CODE [ii];
    }
    //-------------------------------------------------------------//

    if (USAGE_COUNT) {
        delete[] USAGE_COUNT;
    }
    return optimisation_count;
}

Optimizer::~Optimizer() {
    if ((this->PIFOwner->is_buffer) || (!SHIsPooled())) {
        if (DATA) {
            delete[] DATA;
        }

        if (CODE) {
            delete[] CODE;
        }
        if (PARAMS) {
            for (INTEGER i = 0; i < paramCount; i++) {
                if (PARAMS [i].COUNT) {
                    delete[] DELTA_UNREF(&PARAMS [i], PARAMS [i].PARAM_INDEX);
                }
            }
            delete[] PARAMS;
        }
    }
    if (INTERPRETER) {
        delete (ConceptInterpreter *)INTERPRETER;
    }
}

void Optimizer::GenerateIntermediateCode() {
    codeCount  = OptimizedPIF->Count();
    dataCount  = VDList->Count();
    paramCount = ParameterList->Count();

    if (codeCount) {
        CODE = new RuntimeOptimizedElement [codeCount];
    }
    if (dataCount) {
        DATA = new RuntimeVariableDESCRIPTOR [dataCount];
    }

    if (paramCount) {
        PARAMS = new ParamList [paramCount];
    } else {
        PARAMS = 0;
    }

    RuntimeOptimizedElement *CUR2 = 0;
    OptimizedElement        *CUR  = 0;
    for (INTEGER i = 0; i < codeCount; i++) {
        CUR  = (OptimizedElement *)OptimizedPIF->Item(i);
        CUR2 = &CODE [i];

        CUR2->Result_ID = CUR->Result_ID;

        CUR2->OperandReserved.ID   = CUR->OperandReserved.ID;
        CUR2->OperandReserved.TYPE = CUR->OperandReserved.TYPE;

        CUR2->Operator.ID               = CUR->Operator.ID;
        CUR2->Operator.TYPE             = CUR->Operator.TYPE;
        CUR2->Operator._DEBUG_INFO_LINE = CUR->Operator._DEBUG_INFO_LINE;

        CUR2->OperandLeft.ID               = CUR->OperandLeft.ID;
        CUR2->OperandLeft.TYPE             = CUR->OperandLeft.TYPE;
        CUR2->OperandLeft._DEBUG_INFO_LINE = CUR->OperandLeft._DEBUG_INFO_LINE;
        if (CUR->OperandLeft._PARSE_DATA.Length()) {
            CUR2->OperandLeft._PARSE_DATA = CUR->OperandLeft._PARSE_DATA;
        }

        CUR2->OperandRight.ID               = CUR->OperandRight.ID;
        CUR2->OperandRight.TYPE             = CUR->OperandRight.TYPE;
        CUR2->OperandRight._DEBUG_INFO_LINE = CUR->OperandRight._DEBUG_INFO_LINE;
        if (CUR->OperandRight._PARSE_DATA.Length()) {
            CUR2->OperandRight._PARSE_DATA = CUR->OperandRight._PARSE_DATA;
        }
    }
    OptimizedPIF->Clear();
    // end optimization
    for (INTEGER j = 0; j < dataCount; j++) {
        VariableDESCRIPTOR        *VD  = (VariableDESCRIPTOR *)VDList->Item(j);
        RuntimeVariableDESCRIPTOR *VD2 = &DATA [j];
        VD2->BY_REF = VD->BY_REF;
        if (VD->name.Length())
            PIFOwner->RegisterVariableName(DATA, VD->name.c_str(), j);

        VD2->nValue = VD->nValue;
        VD2->TYPE   = VD->TYPE;
        VD2->USED   = VD->USED;
        int len = VD->value.Length();
        if ((VD->TYPE == VARIABLE_NUMBER) && (len == 1) && (VD->value.c_str() [0] == '0')) {
            len = 0;
        }
        if (len) {
            VD2->value = VD->value;
        }
    }
    VDList->Clear();
    for (INTEGER k = 0; k < paramCount; k++) {
        AnsiList *LocalList     = (AnsiList *)ParameterList->Item(k);
        INTEGER  LocalParaCount = LocalList->Count();
        PARAMS [k].COUNT = LocalParaCount;
        if (LocalParaCount) {
            PARAMS [k].PARAM_INDEX = (INTEGER *)DELTA_REF(&PARAMS [k], new INTEGER [LocalParaCount]);
            for (INTEGER l = 0; l < LocalParaCount; l++) {
                DELTA_UNREF(&PARAMS [k], PARAMS [k].PARAM_INDEX) [l] = (INTEGER)(uintptr_t)LocalList->Item(l);
            }
        } else {
            PARAMS [k].PARAM_INDEX = 0;
        }
    }

    ParameterList->Clear();
    PIFList->Clear();
}

AnsiString Optimizer::DEBUG_INFO() {
    AnsiString res;

    res += "------------ Optimized expressions ----------------------------------------\n";
    int count = OptimizedPIF->Count();
    if (!count)
        count = this->codeCount;

    for (INTEGER i = 0; i < count; i++) {
        if (CODE) {
            RuntimeOptimizedElement *OE = &CODE[i];
            res += AnsiString(i) + AnsiString(". ") +
                   AnsiString("(ID: ") + AnsiString(OE->Result_ID) +
                   AnsiString(")\t=> ") +
                   AnsiString("(ID: ") + AnsiString(OE->Operator.ID) +
                   AnsiString(" TYPE:") + AnsiString((int)OE->Operator.TYPE) +
                   AnsiString(") '") + GetKeyWord(OE->Operator.ID) +

                   AnsiString("' (LP: ") + OE->OperandLeft._PARSE_DATA.c_str() + "/" + AnsiString(OE->OperandLeft.ID) +
                   AnsiString(" RP: ") + OE->OperandRight._PARSE_DATA.c_str() + "/" + AnsiString(OE->OperandRight.ID) +

                   AnsiString(") GO:") + AnsiString(OE->OperandReserved.ID) +
                   AnsiString("\n");
        } else {
            OptimizedElement *OE = (OptimizedElement *)OptimizedPIF->Item(i);
            res += AnsiString(i) + AnsiString(". ") +
                   AnsiString("(ID: ") + AnsiString(OE->Result_ID) +
                   AnsiString(")\t=> ") +
                   AnsiString("(ID: ") + AnsiString(OE->Operator.ID) +
                   AnsiString(" TYPE:") + AnsiString((int)OE->Operator.TYPE) +
                   AnsiString(") '") + GetKeyWord(OE->Operator.ID) +

                   AnsiString("' (LP: ") + OE->OperandLeft._PARSE_DATA.c_str() + "/" + AnsiString(OE->OperandLeft.ID) +
                   AnsiString(" RP: ") + OE->OperandRight._PARSE_DATA.c_str() + "/" + AnsiString(OE->OperandRight.ID) +

                   AnsiString(") GO:") + AnsiString(OE->OperandReserved.ID) +
                   AnsiString("\n");
        }
    }
    return res;
}

int Optimizer::Serialize(FILE *out, bool is_lib) {
    int i;

    concept_fwrite_int(&dataCount, sizeof(dataCount), 1, out);
    int ccount = this->PIFOwner->ClassList->Count();

    if (DATA[0].USED == -1)
        DATA[0].nValue = 0xBAD;

    SERIALIZE_VAR_DESCRIPTOR((&DATA [0]), out);

    for (i = 1; i < dataCount; i++) {
        if ((DATA [i].TYPE < 0) && ((DATA [i].nValue < 0) || (DATA [i].nValue > ccount))) {
            if (((DATA [i].TYPE != -VARIABLE_ARRAY) || ((((unsigned short)DATA [i].nValue) & 0xFF) != VARIABLE_NUMBER)) &&
                (DATA [i].TYPE != -VARIABLE_NUMBER) && (DATA [i].TYPE != -VARIABLE_STRING)) {
                DATA [i].nValue = 0;
                DATA [i].TYPE  *= -1;
            }
        }

        SERIALIZE_VAR_DESCRIPTOR((&DATA [i]), out);
    }

    concept_fwrite_int(&paramCount, sizeof(paramCount), 1, out);
    for (i = 0; i < paramCount; i++) {
        SERIALIZE_PARAM_LIST((&PARAMS [i]), out);
    }

    concept_fwrite_int(&codeCount, sizeof(codeCount), 1, out);
    for (i = 0; i < codeCount; i++) {
        SERIALIZE_OPTIMIZED((&CODE [i]), out);
    }

    return 0;
}

int Optimizer::ComputeSharedSize(concept_FILE *in) {
    int     size = 0;
    INTEGER dataCount;
    INTEGER codeCount;

    FREAD_INT_FAIL(&dataCount, sizeof(dataCount), 1, in);

    // both pointers and actual structures
    size += dataCount * sizeof(RuntimeVariableDESCRIPTOR);

#ifdef ARM_ADJUST_SIZE
    size = ARM_ADJUST_SIZE(size);
#endif
    for (int i = 0; i < dataCount; i++) {
        RuntimeVariableDESCRIPTOR VD;
        UNSERIALIZE_VAR_DESCRIPTOR_SIZE((&VD), in);

        int vd_size = VD.value.Length();
        if (vd_size) {
#ifdef ARM_ADJUST_SIZE
            size += ARM_ADJUST_SIZE(vd_size + 1);
#else
            size += vd_size + 1;
#endif
        }
    }

    INTEGER paramCount;
    FREAD_INT_FAIL(&paramCount, sizeof(paramCount), 1, in);

    // already multiple of 4
    size += paramCount * sizeof(ParamList);

    for (int i = 0; i < paramCount; i++) {
        ParamList PL;
        PL.COUNT       = 0;
        PL.PARAM_INDEX = 0;
        UNSERIALIZE_PARAM_LIST_SIZE((&PL), in);
        size += PL.COUNT * sizeof(INTEGER);
    }

    FREAD_INT_FAIL(&codeCount, sizeof(codeCount), 1, in);

#ifdef ARM_ADJUST_SIZE
    size += ARM_ADJUST_SIZE(codeCount * sizeof(RuntimeOptimizedElement));
#else
    size += codeCount * sizeof(RuntimeOptimizedElement);
#endif

    for (int i = 0; i < codeCount; i++) {
        // to do !
        RuntimeOptimizedElement OE;
        UNSERIALIZE_OPTIMIZED((&OE), in, false);
        int len = OE.OperandLeft._PARSE_DATA.Length();
        if (len) {
#ifdef ARM_ADJUST_SIZE
            size += ARM_ADJUST_SIZE(len + 1);
#else
            size += len + 1;
#endif
        }
        len = OE.OperandRight._PARSE_DATA.Length();
        if (len) {
#ifdef ARM_ADJUST_SIZE
            size += ARM_ADJUST_SIZE(len + 1);
#else
            size += len + 1;
#endif
        }
    }
    return size;
}

int Optimizer::Unserialize(concept_FILE *in, AnsiList *ModuleList, bool is_lib, int *ClassNames, int *Relocation) {
    int         i;
    signed char is_pooled  = this->PIFOwner->is_buffer ? 0 : (signed char)SHIsPooled();
    bool        is_created = this->PIFOwner->is_buffer ? 0 : SHIsCreated();

    if (!concept_fread_int(&dataCount, sizeof(dataCount), 1, in)) {
        return -1;
    }
    if (dataCount) {
        if (is_pooled) {
            DATA = (RuntimeVariableDESCRIPTOR *)SHAlloc(sizeof(RuntimeVariableDESCRIPTOR) * dataCount);
        } else {
            DATA = new RuntimeVariableDESCRIPTOR [dataCount];
        }
    }
    if ((is_pooled) && (!is_created)) {
        RuntimeVariableDESCRIPTOR VD2;
        RuntimeVariableDESCRIPTOR *VD = &VD2;
        for (i = 0; i < dataCount; i++) {
            // dummy !
            UNSERIALIZE_VAR_DESCRIPTOR(VD, in, -1);
        }
    } else {
        RuntimeVariableDESCRIPTOR *VD;
#ifdef USE_JIT
        RuntimeVariableDESCRIPTOR *VD = &DATA [0];
        UNSERIALIZE_VAR_DESCRIPTOR(VD, in, is_pooled);
        if (VD->nValue == 0xBAD)
            VD->USED = -1;
        else
            VD->USED = 1;
#endif

#ifdef USE_JIT
        for (i = 1; i < dataCount; i++) {
#else
        for (i = 0; i < dataCount; i++) {
#endif
            VD       = &DATA [i];
            VD->USED = 1;
            UNSERIALIZE_VAR_DESCRIPTOR(VD, in, is_pooled);

            //}

            //REMOVED RECENTLY !===//
            //====================//
        }
    }

    if (!concept_fread_int(&paramCount, sizeof(paramCount), 1, in)) {
        return -1;
    }

    if (paramCount) {
        if (is_pooled) {
            PARAMS = (ParamList *)SHAlloc(sizeof(ParamList) * paramCount);
        } else {
            PARAMS = new ParamList [paramCount];
        }
    } else {
        PARAMS = 0;
    }

    if ((is_pooled) && (!is_created)) {
        for (i = 0; i < paramCount; i++) {
            ParamList PL;
            PL.COUNT       = 0;
            PL.PARAM_INDEX = 0;
            UNSERIALIZE_PARAM_LIST_SIZE((&PL), in);
            if (PL.COUNT) {
                SHAlloc(sizeof(INTEGER) * PL.COUNT);
            }
        }
    } else {
        for (i = 0; i < paramCount; i++) {
            ParamList *PL = &PARAMS [i];
            PL->COUNT       = 0;
            PL->PARAM_INDEX = 0;
            UNSERIALIZE_PARAM_LIST(PL, in, is_pooled);
        }
    }

    if (!concept_fread_int(&codeCount, sizeof(codeCount), 1, in)) {
        return -1;
    }

    if (codeCount) {
        if (is_pooled) {
            CODE = (RuntimeOptimizedElement *)SHAlloc(sizeof(RuntimeOptimizedElement) * codeCount);
        } else {
            CODE = new RuntimeOptimizedElement [codeCount];
        }
    }

    //ADDED RECENTLY !===//
    if (OptimizedPIF) {
        delete OptimizedPIF;
        OptimizedPIF = 0;
    }
    if (ParameterList) {
        delete ParameterList;
        ParameterList = 0;
    }
    //=====================//
    RuntimeOptimizedElement *OE;
    if ((is_pooled) && (!is_created)) {
        RuntimeOptimizedElement OE2;
        OE = &OE2;
        for (i = 0; i < codeCount; i++) {
            UNSERIALIZE_OPTIMIZED(OE, in, -1);
            if (OE->Operator.TYPE == TYPE_OPERATOR) {
                if (OE->Operator.ID == KEY_DLL_CALL) {
                    if (OE->OperandLeft.ID == STATIC_CLASS_DLL) {
                        const char *funname = CODE [i].OperandRight._PARSE_DATA.c_str();

                        int l_res = PIFOwner->LinkStatic(funname);
                        if (!l_res)
                            PIFOwner->Errors.Add(new AnsiException(ERR870, 0, 870, OE->OperandRight._PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                    }
                }
            }
        }
    } else {
        for (i = 0; i < codeCount; i++) {
            //=======================//
            OE = &CODE [i];

            UNSERIALIZE_OPTIMIZED(OE, in, is_pooled);
            //=====================//
            if (OE->Operator.TYPE == TYPE_OPERATOR) {
                if (OE->Operator.ID == KEY_DLL_CALL) {
                    if (OE->OperandLeft.ID == STATIC_CLASS_DLL) {
                        OE->OperandRight.ID = PIFOwner->LinkStatic(OE->OperandRight._PARSE_DATA.c_str());
                        if (!OE->OperandRight.ID) {
                            PIFOwner->Errors.Add(new AnsiException(ERR870, 0, 870, OE->OperandRight._PARSE_DATA, _DEBUG_INFO_FILENAME, _CLASS->NAME, _MEMBER), DATA_EXCEPTION);
                        }
                    } else if ((is_lib) && (OE->OperandLeft.ID > 0)) {
                        int newID = ClassNames [OE->OperandLeft.ID - 1] + 1;
                        if (!newID) {
                            int delta = 1;
                            for (int k = OE->OperandLeft.ID - 2; k >= 0; k--) {
                                delta++;
                                if (ClassNames [k] > -1) {
                                    newID = ClassNames [k] + delta;
                                    break;
                                }
                            }
                        }
                        OE->OperandLeft.ID = newID;

                        OE->OperandRight.ID = Relocation [OE->OperandRight.ID - 1] + 1;
                    }
                } else
                if (is_lib) {
                    if (OE->Operator.ID == KEY_SEL) {
                        OE->OperandRight.ID = Relocation [OE->OperandRight.ID - 1] + 1;
                    } else
                    if ((OE->Operator.ID == KEY_NEW) && (OE->OperandLeft.ID >= 0)) {
                        int newID = ClassNames [OE->OperandLeft.ID - 1] + 1;
                        if (!newID) {
                            int delta = 1;
                            for (int k = OE->OperandLeft.ID - 2; k >= 0; k--) {
                                delta++;
                                if (ClassNames [k] > -1) {
                                    newID = ClassNames [k] + delta;
                                    break;
                                }
                            }
                        }
                        OE->OperandLeft.ID = newID;
                    }
                }
            }
        }
    }
    return 0;
}

void Optimizer::AddProfilerCode(int code) {
    if (!PIFOwner->PROFILE_DRIVEN_ID)
        PIFOwner->PROFILE_DRIVEN_ID = PIFOwner->LinkStatic(PIFOwner->PROFILE_DRIVEN);
    if (PIFOwner->PROFILE_DRIVEN_ID) {
        VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
        VD->BY_REF = 0;
        VD->value  = NULL_STRING;
        VD->nValue = 0;
        VD->USED   = -2;
        VD->TYPE   = VARIABLE_NUMBER;
        VDList->Add(VD, DATA_VAR_DESCRIPTOR);

        OptimizedElement *OEProfiler = new OptimizedElement;

        OEProfiler->Operator.ID               = KEY_DLL_CALL;
        OEProfiler->Operator.TYPE             = TYPE_OPERATOR;
        OEProfiler->Operator._DEBUG_INFO_LINE = 0;

        OEProfiler->OperandLeft.ID               = STATIC_CLASS_DLL;
        OEProfiler->OperandLeft.TYPE             = TYPE_CLASS;
        OEProfiler->OperandLeft._DEBUG_INFO_LINE = 0;
        OEProfiler->OperandLeft._HASH_DATA       = 0;
        OEProfiler->OperandLeft._PARSE_DATA      = "LIBRARY";

        OEProfiler->OperandRight.ID               = PIFOwner->PROFILE_DRIVEN_ID;
        OEProfiler->OperandRight.TYPE             = TYPE_METHOD;
        OEProfiler->OperandRight._DEBUG_INFO_LINE = 0;
        OEProfiler->OperandRight._HASH_DATA       = 0;
        OEProfiler->OperandRight._PARSE_DATA      = PIFOwner->PROFILE_DRIVEN;

        OEProfiler->Result_ID = VDList->Count();

        int start_param = VDList->Count();

        VD         = new VariableDESCRIPTOR;
        VD->BY_REF = 0;
        VD->value  = AnsiString((long)code);
        VD->nValue = code;
        VD->USED   = 1;
        VD->TYPE   = VARIABLE_NUMBER;
        VDList->Add(VD, DATA_VAR_DESCRIPTOR);

        VD         = new VariableDESCRIPTOR;
        VD->BY_REF = 0;
        VD->value  = this->_CLASS->NAME.c_str();
        VD->nValue = 0;
        VD->USED   = 1;
        VD->TYPE   = VARIABLE_STRING;
        VDList->Add(VD, DATA_VAR_DESCRIPTOR);

        VD         = new VariableDESCRIPTOR;
        VD->BY_REF = 0;
        VD->value  = this->_MEMBER;
        VD->nValue = 0;
        VD->USED   = 1;
        VD->TYPE   = VARIABLE_STRING;
        VDList->Add(VD, DATA_VAR_DESCRIPTOR);

        AnsiList *PL = new AnsiList(false);

        start_param++;
        PL->Add((void *)(intptr_t)start_param, DATA_32_BIT);

        start_param++;
        PL->Add((void *)(intptr_t)start_param, DATA_32_BIT);

        start_param++;
        PL->Add((void *)(intptr_t)start_param, DATA_32_BIT);

        ParameterList->Add(PL, DATA_LIST);

        OEProfiler->OperandReserved.ID   = ParameterList->Count();
        OEProfiler->OperandReserved.TYPE = TYPE_PARAM_LIST;

        OptimizedPIF->Add(OEProfiler, DATA_OPTIMIZED_ELEMENT);
    }
}
