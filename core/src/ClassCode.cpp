#include "ClassCode.h"
#include "ConceptInterpreter.h"
#include <string.h>
#include "SHManager.h"

POOLED_IMPLEMENTATION(ClassCode)

//#ifndef SINGLE_PROCESS_DLL
//#endif

#ifdef STATIC_FLAG
ParamList * ClassCode::EMPTY_PARAM_LIST = 0;
#endif

ClassCode::ClassCode(char *name, PIFAlizator *P, char binary_form) {
    if (binary_form) {
        Members = 0;
    } else {
        NAME    = name;
        Members = new AnsiList();
    }
    CONSTRUCTOR        = 0;
    DESTRUCTOR         = 0;
    CONSTRUCTOR_MEMBER = 0;
    DESTRUCTOR_MEMBER  = 0;
    pMEMBERS           = 0;
    pMEMBERS_COUNT     = 0;
    CLSID         = 0;
    DEFINED_LEVEL = 0;

#ifndef STATIC_FLAG
    EMPTY_PARAM_LIST = 0;
#endif

    _DEBUG_INFO_FILENAME = P->FileName;
    NEEDED       = 0;
    first_parent = 0;

    RELOCATIONS      = 0;
    RELOCATIONS2     = 0;
    DataMembersCount = 0;
}

void ClassCode::Hibernate(void *cc) {
    CompiledClass *CCLASS = (CompiledClass *)cc;
    INTEGER       Count   = CCLASS->_Class->DataMembersCount;

    if (!CCLASS->_CONTEXT) {
        return;
    }

    for (register INTEGER i = 0; i < Count; i++) {
        VariableDATA *_CONTEXT_i = CCLASS->_CONTEXT [i];
        if (_CONTEXT_i) {
            switch (_CONTEXT_i->TYPE) {
                case VARIABLE_CLASS:
                    break;

                case VARIABLE_ARRAY:
                    break;

                case VARIABLE_STRING:
                    break;
            }
        }
    }
}

ClassMember *ClassCode::AddLinked(PIFAlizator *PIF, ClassMember *CM_i) {
    INTEGER Count = Members->Count();

    for (INTEGER i = 0; i < Count; i++) {
        ClassMember *CM = (ClassMember *)Members->Item(i);
        if (!strcmp(CM_i->NAME, CM->NAME)) {
            AnsiException *Exc = new AnsiException(ERR060, CM->_DEBUG_STARTLINE, 60, CM_i->NAME, ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME, CM_i->NAME);
            PIF->Errors.Add(Exc, DATA_EXCEPTION);
            return CM;
        }
    }
    // is inherited, do not delete !
    Members->Add(CM_i, DATA_CLASS_MEMBER, true);
    return CM_i;
}

ClassMember *ClassCode::AddMember(PIFAlizator *PIF, char *name, INTEGER line, char *FileName, INTEGER ACCESS, char data_only) {
    INTEGER Count = Members->Count();

    for (INTEGER i = 0; i < Count; i++) {
        ClassMember *CM = (ClassMember *)Members->Item(i);
        if (!strcmp(CM->NAME, name)) {
            AnsiException *Exc = new AnsiException(ERR060, line, 60, name, FileName, NAME);
            PIF->Errors.Add(Exc, DATA_EXCEPTION);
            break;
        }
    }
    ClassMember *CM = new ClassMember(this, name,  data_only);
    ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME = FileName;
    CM->_DEBUG_STARTLINE = line;
    Members->Add(CM, DATA_CLASS_MEMBER);
    CM->ACCESS = ACCESS;
    return CM;
}

#define COPY_VD(DEST, SRC)      \
    DEST->BY_REF = SRC->BY_REF; \
    DEST->name   = SRC->name;   \
    DEST->nValue = SRC->nValue; \
    DEST->TYPE   = SRC->TYPE;   \
    DEST->USED   = SRC->USED;   \
    DEST->value  = SRC->value;

#define COPY_SMALL_VD(DEST, SRC) \
    DEST->nValue = SRC->nValue;  \
    DEST->TYPE   = SRC->TYPE;    \
    DEST->value  = SRC->value;

int ClassCode::BoundMember(PIFAlizator *PIF, ClassMember *CM, ClassCode *Sender) {
    AnsiString Mname = ((ClassCode *)CM->Defined_In)->NAME;

    Mname += (char *)"@";
    Mname += CM->NAME;

    int has_it = Sender->HasMember(Mname.c_str());
    if (has_it) {
        return has_it;
    }

    int  ref_id    = PIF->GeneralMembers->ContainsString(Mname.c_str(), Mname.Length());
    char *ref_name = 0;
    if (!ref_id) {
        PIF->GeneralMembers->Add(Mname);
        ref_id = PIF->GeneralMembers->Count();
    }
    ref_name = PIF->GeneralMembers->Item(ref_id - 1);

    ClassMember *CM2 = new ClassMember(CM->Defined_In, ref_name, false);
    CM2->_DEBUG_STARTLINE      = CM->_DEBUG_STARTLINE;
    CM2->ACCESS                = CM->ACCESS;
    CM2->Defined_In            = Sender;
    CM2->IS_OPERATOR           = CM->IS_OPERATOR;
    CM2->IS_FUNCTION           = CM->IS_FUNCTION;
    CM2->IS_STATIC             = CM->IS_STATIC;
    CM2->MEMBER_GET            = CM->MEMBER_GET;
    CM2->MEMBER_SET            = CM->MEMBER_SET;
    CM2->MUST_PARAMETERS_COUNT = CM->MUST_PARAMETERS_COUNT;
    CM2->PARAMETERS_COUNT      = CM->PARAMETERS_COUNT;

    if (!CM2->IS_FUNCTION) {
        CM2->EnsureVD();
        CM->EnsureVD();
        COPY_SMALL_VD(CM2->VD, CM->VD);
    }

    if (CM->VariableDescriptors) {
        int len = CM->VariableDescriptors->Count();
        for (int i = 0; i < len; i++) {
            VariableDESCRIPTOR *VD  = (VariableDESCRIPTOR *)CM->VariableDescriptors->Item(i);
            VariableDESCRIPTOR *VD2 = new VariableDESCRIPTOR;
            COPY_VD(VD2, VD);
            CM2->VariableDescriptors->Add(VD2, DATA_VAR_DESCRIPTOR);
        }
    }

    if (CM->PIF_DATA) {
        int len = CM->PIF_DATA->Count();
        for (int i = 0; i < len; i++) {
            AnalizerElement *AE  = (AnalizerElement *)CM->PIF_DATA->Item(i);
            AnalizerElement *AE2 = new AnalizerElement;
            AE2->_DEBUG_INFO_LINE = AE->_DEBUG_INFO_LINE;
            AE2->_HASH_DATA       = AE->_HASH_DATA;
            AE2->_INFO_OPTIMIZED  = AE->_INFO_OPTIMIZED;
            AE2->_PARSE_DATA      = AE->_PARSE_DATA;
            AE2->ID   = AE->ID;
            AE2->TYPE = AE->TYPE;

            CM2->PIF_DATA->Add(AE2, DATA_ANALIZER_ELEMENT);
        }
    } else if ((CM->IS_FUNCTION == 1) && (CM->OPTIMIZER)) {
        CM2->OPTIMIZER  = CM->OPTIMIZER;
        CM2->MEMBER_GET = -1;
    }
    Sender->Members->Add(CM2, DATA_CLASS_MEMBER);

    return 0;
}

int ClassCode::RemoveMember(PIFAlizator *PIF, char *name, INTEGER line, char *FileName) {
    INTEGER Count  = Members->Count();
    int     has_it = 0;

    for (INTEGER i = 0; i < Count; i++) {
        ClassMember *CM = (ClassMember *)Members->Item(i);
        if ((CM->NAME) && (!strcmp(CM->NAME, name))) {
            if (CM->ACCESS == ACCESS_PRIVATE) {
                AnsiException *Exc = new AnsiException(ERR1190, line, 1190, name, FileName, NAME);
                PIF->Errors.Add(Exc, DATA_EXCEPTION);
                return 0;
            }
            has_it = ((ClassCode *)CM->Defined_In)->BoundMember(PIF, CM, this);
            Members->Remove(i);

            if (i + 1 == CONSTRUCTOR) {
                CONSTRUCTOR        = 0;
                CONSTRUCTOR_MEMBER = 0;
            } else
            if ((CONSTRUCTOR) && (CONSTRUCTOR > i)) {
                CONSTRUCTOR--;
            }

            if ((DESTRUCTOR) && (DESTRUCTOR > i)) {
                DESTRUCTOR--;
            }
            return i;
        }
    }
    AnsiException *Exc = new AnsiException(ERR120, line, 120, name, FileName, NAME);
    PIF->Errors.Add(Exc, DATA_EXCEPTION);
    return 0;
}

int ClassCode::HasMember(char *name) {
    if (!Members)
        return this->HasMemberInCompiled(name);
    INTEGER Count = Members->Count();

    for (INTEGER i = 0; i < Count; i++) {
        ClassMember *CM = (ClassMember *)Members->Item(i);
        if ((CM->NAME) && (!strcmp(CM->NAME, name))) {
            return i + 1;
        }
    }
    return 0;
}

int ClassCode::CanBeRunStatic(char *name) {
    INTEGER Count = Members->Count();

    for (INTEGER i = 0; i < Count; i++) {
        ClassMember *CM = (ClassMember *)Members->Item(i);
        if ((CM->NAME) && (!strcmp(CM->NAME, name))) {
            return CM->IS_STATIC;
        }
    }
    return 0;
}

int ClassCode::HasMemberInCompiled(char *name, INTEGER *m_type) {
    INTEGER Count = pMEMBERS_COUNT;

    if (!pMEMBERS) {
        return 0;
    }

    for (INTEGER i = 0; i < Count; i++) {
        ClassMember *CM = pMEMBERS [i];
        if ((CM) && (CM->NAME) && (!strcmp(CM->NAME, name))) {
            if (m_type) {
                *m_type = CM->IS_FUNCTION;
            }
            return i + 1;
        }
    }
    return 0;
}

ClassMember *ClassCode::MemberID(int mid) {
    int j = 0;

    if ((!pMEMBERS_COUNT) || (!pMEMBERS)) {
        return 0;
    }

    int count = pMEMBERS_COUNT;

    do {
        int reloc = RELOCATIONS [j++];
        if (j == mid) {
            return pMEMBERS [reloc - 1];
        }
        if (reloc) {
            count--;
        }

        if (!count) {
            break;
        }
    } while (true);

    return 0;
}

int ClassCode::GetAbsoluteMemberID(int mid) {
    int j = 0;

    while (RELOCATIONS [j++] != mid) {
    }

    return j;
}

int ClassCode::GetSerialMembers(CompiledClass *CC, int max_members,
                                char **pmembers, unsigned char *flags, char *access,
                                char *types, char **szValue,
                                double *n_data, void **class_data, void **variable_data) {
    if (!pMEMBERS) {
        return 0;
    }

    int index = 0;
    for (INTEGER i = 0; i < pMEMBERS_COUNT; i++) {
        if (index >= max_members) {
            return  pMEMBERS_COUNT - index;
        }
        ClassMember *CM = pMEMBERS [i];
        if (CM) {
            pmembers [index] = CM->NAME;
            flags [index]    = CM->IS_FUNCTION;
            if (CM->IS_OPERATOR) {
                flags [index] = 0x80;
            }
            access [index] = CM->ACCESS;
            if (CM->IS_STATIC) {
                access [index] |= 0x80;
            }
            if (!CM->IS_FUNCTION) {
                szValue [index]       = 0;
                n_data [index]        = 0;
                class_data [index]    = 0;
                variable_data [index] = 0;
                int reloc = this->RELOCATIONS2 [i] - 1;

                if ((CC->_CONTEXT) && (!CC->_CONTEXT [reloc])) {
                    if ((CM->VD) && ((CM->VD->TYPE != VARIABLE_NUMBER) || (CM->VD->nValue)))
                        CC->CreateVariable(reloc, CM);
                }

                if ((CC->_CONTEXT) && (CC->_CONTEXT [reloc])) {
                    types [index]         = CC->_CONTEXT [reloc]->TYPE;
                    variable_data [index] = CC->_CONTEXT [reloc];
                    switch (CC->_CONTEXT [reloc]->TYPE) {
                        case VARIABLE_STRING:
                            n_data [index]  = CONCEPT_STRING(CC->_CONTEXT [reloc]).Length();
                            szValue [index] = CONCEPT_STRING(CC->_CONTEXT [reloc]).c_str();
                            break;

                        case VARIABLE_CLASS:
                            szValue [index]    = ((CompiledClass *)CC->_CONTEXT [reloc]->CLASS_DATA)->_Class->NAME.c_str();
                            class_data [index] = CC->_CONTEXT [reloc]->CLASS_DATA;
                            break;

                        case VARIABLE_ARRAY:
                            class_data [index] = CC->_CONTEXT [reloc]->CLASS_DATA;
                            break;

                        case VARIABLE_NUMBER:
                            n_data [index] = CC->_CONTEXT [reloc]->NUMBER_DATA;
                            break;

                        case VARIABLE_DELEGATE:
                            {
                                class_data [index] = CC->_CONTEXT [reloc]->CLASS_DATA;
                                int         relocation = CC->_CONTEXT [reloc]->DELEGATE_DATA;
                                ClassMember *CMD       = relocation ? ((CompiledClass *)CC->_CONTEXT [reloc]->CLASS_DATA)->_Class->pMEMBERS [relocation - 1] : 0;
                                if (CMD) {
                                    n_data [index] = (intptr_t)CMD->NAME;
                                } else {
                                    n_data [index] = 0;
                                }
                                szValue [index] = ((CompiledClass *)CC->_CONTEXT [reloc]->CLASS_DATA)->_Class->NAME.c_str();
                            }
                            break;
                    }
                } else {
                    flags [index] = 0xff;
                }
            } else {
                types [index] = -1;
            }
            index++;
        }
    }
    return 0;
}

CompiledClass *ClassCode::CreateInstance(PIFAlizator *PIF, VariableDATA *Owner, RuntimeElement *AE, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, SCStack *PREV, char is_static) {
    static ParamList dummy = { 0, 0 };
    CompiledClass    *res  = new(AllocClassObject(PIF))CompiledClass(this);

    Owner->CLASS_DATA = res;

    if (CONSTRUCTOR) {
        ClassMember *CM = this->CONSTRUCTOR_MEMBER;

        if (SenderCTX) {
            //==========================//
            if (!FORMAL_PARAM) {
                FORMAL_PARAM = &dummy;
            }
            //==========================//

            if ((!FORMAL_PARAM) || !(ENOUGH_PARAMETERS(CM, FORMAL_PARAM))) {
                AnsiException *Exc = new AnsiException(ERR220, AE ? AE->_DEBUG_INFO_LINE : 0, 220, AnsiString(CM->NAME) + AnsiString(" requires ") + AnsiString(CM->PARAMETERS_COUNT), ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME, NAME);
                PIF->AcknoledgeRunTimeError(PREV, Exc);
                return 0;
            }
#ifndef SIMPLE_MULTI_THREADING
            if ((FORMAL_PARAM->COUNT == 1) && (CM->FastOptimizedExecute(PIF, res, FORMAL_PARAM, SenderCTX)))
                return res;
#endif
        } else if (CM->PARAMETERS_COUNT) {
            AnsiException *Exc = new AnsiException(ERR230, 0, 230, NULL_STRING, _DEBUG_INFO_FILENAME, NAME);
            PIF->AcknoledgeRunTimeError(PREV, Exc);
            return 0;
        }

        if (is_static)
            PIF->RootInstance = res;
        VariableDATA *THROW_DATA = 0;
        STACK(PREV, AE ? AE->_DEBUG_INFO_LINE : 0)
        VariableDATA * RESULT = CM->Execute(PIF, this->CLSID, Owner, FORMAL_PARAM, SenderCTX, THROW_DATA, PREV, is_static);
        UNSTACK;
        if (RESULT) {
            if (is_static) {
                if (RESULT->TYPE == VARIABLE_NUMBER)
                    PIF->last_result = (int)RESULT->NUMBER_DATA;
            }
            FREE_VARIABLE(RESULT);
        } else
        if (THROW_DATA) {
            AnsiString *data = new AnsiString();

            if (AE) {
                *data  = AE->_PARSE_DATA.c_str();
                *data += ", data:";
            }

            // check if is not the static call to Main. In this case the THROW_DATA is/may be deleted
            if (SenderCTX) {
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
            }

            AnsiException *Exc = new AnsiException(ERR630, AE ? AE->_DEBUG_INFO_LINE : 0, 630,  data->c_str(), ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME, NAME);
            PIF->AcknoledgeRunTimeError(PREV, Exc);

            delete data;
        }
    } else if ((FORMAL_PARAM) && (FORMAL_PARAM->COUNT)) {
        AnsiException *Exc = new AnsiException(ERR280, AE ? AE->_DEBUG_INFO_LINE : 0, 280, NAME, _DEBUG_INFO_FILENAME, NAME);
        PIF->AcknoledgeRunTimeError(PREV, Exc);
        return 0;
    }
    return res;
}

void ClassCode::SetProperty(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, RuntimeElement *AE, INTEGER local, INTEGER VALUE, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, SCStack *PREV) {
    int relocation = this->RELOCATIONS [i];

    register ClassMember *pMEMBER_i = relocation ? pMEMBERS [relocation - 1] : 0;

    if (pMEMBER_i) {
        if (((ClassCode *)pMEMBER_i->Defined_In)->CLSID != LOCAL_CLSID) {
            if (pMEMBER_i->ACCESS == ACCESS_PRIVATE) {
                char          *mname = PIF->GeneralMembers->Item(i);
                AnsiException *Exc   = new AnsiException(ERR190, AE ? AE->_DEBUG_INFO_LINE : 0, 190, mname ? mname : (AE ? AE->_PARSE_DATA.c_str() : pMEMBER_i->NAME), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
                PIF->AcknoledgeRunTimeError(PREV, Exc);
                return;
            }
            if ((pMEMBER_i->ACCESS == ACCESS_PROTECTED) && (!local)) {
                ClassCode *caller = (ClassCode *)PIF->StaticClassList[LOCAL_CLSID];
                if ((!caller) || (!caller->Inherits(this->CLSID))) {
                    char          *mname = PIF->GeneralMembers->Item(i);
                    AnsiException *Exc   = new AnsiException(ERR850, AE ? AE->_DEBUG_INFO_LINE : 0, 850, mname ? mname : (AE ? AE->_PARSE_DATA.c_str() : pMEMBER_i->NAME), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
                    PIF->AcknoledgeRunTimeError(PREV, Exc);
                    return;
                }
            }
        }
        ParamList baseONE_PARAM_LIST;
        INTEGER   PLIST[1];
        ParamList *ONE_PARAM_LIST = &baseONE_PARAM_LIST;
        ONE_PARAM_LIST->COUNT       = 1;
        ONE_PARAM_LIST->PARAM_INDEX = DELTA_REF(ONE_PARAM_LIST, PLIST);
        //}
        DELTA_UNREF(ONE_PARAM_LIST, ONE_PARAM_LIST->PARAM_INDEX) [0] = VALUE;

        if (!pMEMBER_i->MEMBER_SET) {
            char          *mname = PIF->GeneralMembers->Item(i);
            AnsiException *Exc   = new AnsiException(ERR410, AE->_DEBUG_INFO_LINE, 410, mname ? AnsiString(mname) : AE->_PARSE_DATA, *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
            PIF->AcknoledgeRunTimeError(PREV, Exc);
            return;
        }
        try {
            VariableDATA *RESULT = ExecuteMember(PIF, pMEMBER_i->MEMBER_SET - 1, Owner, AE, 1, ONE_PARAM_LIST, SenderCTX, 1, ((ClassCode *)pMEMBER_i->Defined_In)->CLSID, ((ClassCode *)pMEMBER_i->Defined_In)->CLSID, PREV);

            if (RESULT) {
                // new zone added //
                CLASS_CHECK(RESULT);

                VariableDATA *NEW_VALUE = SenderCTX [VALUE - 1];
                RESULT->TYPE = NEW_VALUE->TYPE;
                switch (RESULT->TYPE) {
                    case VARIABLE_NUMBER:
                        RESULT->NUMBER_DATA = NEW_VALUE->NUMBER_DATA;
                        break;

                    case VARIABLE_STRING:
                        RESULT->CLASS_DATA     = NULL;
                        CONCEPT_STRING(RESULT) = CONCEPT_STRING(NEW_VALUE);
                        break;

                    case VARIABLE_CLASS:
                        RESULT->CLASS_DATA = NEW_VALUE->CLASS_DATA;
                        ((CompiledClass *)RESULT->CLASS_DATA)->LINKS++;
                        break;

                    case VARIABLE_ARRAY:
                        RESULT->CLASS_DATA = NEW_VALUE->CLASS_DATA;
                        ((Array *)RESULT->CLASS_DATA)->LINKS++;
                        break;

                    case VARIABLE_DELEGATE:
                        RESULT->CLASS_DATA = NEW_VALUE->CLASS_DATA;
                        ((CompiledClass *)RESULT->CLASS_DATA)->LINKS++;
                        RESULT->DELEGATE_DATA = NEW_VALUE->DELEGATE_DATA;
                        break;
                }
                //----------------------------------------------
                // end zone added //
                RESULT->LINKS++;
                FREE_VARIABLE(RESULT);
            }
            return;
        } catch (VariableDATA *LAST_THROW) {
            throw LAST_THROW;
            return;
        }
    }

    char          *mname = PIF->GeneralMembers->Item(i);
    AnsiException *Exc   = new AnsiException(ERR150, AE->_DEBUG_INFO_LINE, 150, mname ? AnsiString(mname) : AE->_PARSE_DATA, *GetFilename(PIF, CLSID, &_DEBUG_INFO_FILENAME), NAME);
    PIF->AcknoledgeRunTimeError(PREV, Exc);
    return;
}

TinyString *ClassCode::GetFilename(PIFAlizator *PIF, INTEGER LOCAL_CLSID, TinyString *default_Value) {
    if (LOCAL_CLSID < 0)
        return default_Value;
    ClassCode *CC;
    if (PIF->StaticClassList)
        CC = PIF->StaticClassList[LOCAL_CLSID];
    else
        CC = (ClassCode *)PIF->ClassList->Item(LOCAL_CLSID);
    if (CC)
        return &CC->_DEBUG_INFO_FILENAME;
}

VariableDATA *ClassCode::ExecuteDelegate(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, RuntimeElement *AE, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, SCStack *PREV) {
    VariableDATA         *RESULT;
    register ClassMember *pMEMBER_i = i ? pMEMBERS [i - 1] : 0;

    if (pMEMBER_i) {
        if (!(ENOUGH_PARAMETERS(pMEMBER_i, FORMAL_PARAM))) {
            AnsiString szparameters = (char *)"none";
            if (FORMAL_PARAM) {
                szparameters = AnsiString(FORMAL_PARAM->COUNT);
            }
            AnsiException *Exc = new AnsiException(ERR210, AE ? AE->_DEBUG_INFO_LINE : 0, 210, AnsiString("required ") + AnsiString(pMEMBER_i->PARAMETERS_COUNT) + AnsiString(", with ") + AnsiString(pMEMBER_i->MUST_PARAMETERS_COUNT) + AnsiString(" non default parameters, received ") + szparameters, *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME),  pMEMBER_i->NAME);
            PIF->AcknoledgeRunTimeError(PREV, Exc);
            return 0;
        }
        VariableDATA *THROW_DATA = 0;

        STACK(PREV, AE ? AE->_DEBUG_INFO_LINE : 0)
        RESULT = pMEMBER_i->Execute(PIF, this->CLSID, Owner, FORMAL_PARAM, SenderCTX, THROW_DATA, PREV);
        UNSTACK;

        if (THROW_DATA) {
            throw THROW_DATA;
        }

        return RESULT;
    }
    return 0;
}

VariableDATA *ClassCode::ExecuteMember(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, RuntimeElement *AE, INTEGER local, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, char property, INTEGER CLSID, INTEGER LOCAL_CLSID, SCStack *PREV, char next_is_asg, VariableDATAPROPERTY **PROPERTIES, int dataLen, int result_id) {
    INTEGER      IS_PROPERTY = 0;
    VariableDATA *RESULT;
    int          relocation = this->RELOCATIONS [i];

    register ClassMember *pMEMBER_i = relocation ? pMEMBERS [relocation - 1] : 0;

    if (pMEMBER_i) {
        if (((ClassCode *)pMEMBER_i->Defined_In)->CLSID != LOCAL_CLSID) {
            if (pMEMBER_i->ACCESS == ACCESS_PRIVATE) {
                char          *mname = PIF->GeneralMembers->Item(i);
                AnsiException *Exc   = new AnsiException(ERR190, AE ? AE->_DEBUG_INFO_LINE : 0, 190, mname ? mname : (AE ? AE->_PARSE_DATA.c_str() : pMEMBER_i->NAME), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), ((ClassCode *)(pMEMBER_i->Defined_In))->NAME, NAME);
                PIF->AcknoledgeRunTimeError(PREV, Exc);
                return 0;
            }
            if ((pMEMBER_i->ACCESS == ACCESS_PROTECTED) && (!local)) {
                ClassCode *caller = (ClassCode *)PIF->StaticClassList[LOCAL_CLSID];
                if ((!caller) || (!caller->Inherits(this->CLSID))) {
                    char          *mname = PIF->GeneralMembers->Item(i);
                    AnsiException *Exc   = new AnsiException(ERR850, AE ? AE->_DEBUG_INFO_LINE : 0, 850, mname ? mname : (AE ? AE->_PARSE_DATA.c_str() : pMEMBER_i->NAME), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), ((ClassCode *)(pMEMBER_i->Defined_In))->NAME, NAME);
                    PIF->AcknoledgeRunTimeError(PREV, Exc);
                    return 0;
                }
            }
        }

        while (pMEMBER_i->IS_FUNCTION == 2) {
            i          = pMEMBER_i->MEMBER_GET - 1;
            relocation = this->RELOCATIONS [i];
            pMEMBER_i  = relocation ? pMEMBERS [relocation - 1] : 0;
            if (!pMEMBER_i) {
                return 0;
            }
        }

        int relocation2 = this->RELOCATIONS2 [relocation - 1];

        switch (pMEMBER_i->IS_FUNCTION) {
            case 1:
                if (!FORMAL_PARAM) {
                    VariableDATA *deleg = (VariableDATA *)VAR_ALLOC(PIF);
                    deleg->CLASS_DATA = (CompiledClass *)Owner->CLASS_DATA;
                    CC_WRITE_LOCK(PIF)
                        ((CompiledClass *)Owner->CLASS_DATA)->LINKS++;
                    CC_WRITE_UNLOCK(PIF)
                    deleg->IS_PROPERTY_RESULT = 0;
                    deleg->LINKS         = 0;
                    deleg->DELEGATE_DATA = relocation;
                    deleg->TYPE          = VARIABLE_DELEGATE;
                    return deleg;
                } else
                if (!(ENOUGH_PARAMETERS(pMEMBER_i, FORMAL_PARAM))) {
                    AnsiString szparameters = (char *)"none";
                    if (FORMAL_PARAM) {
                        szparameters = AnsiString(FORMAL_PARAM->COUNT);
                    }
                    AnsiException *Exc = new AnsiException(ERR210, AE ? AE->_DEBUG_INFO_LINE : 0, 210, AnsiString("required ") + AnsiString(pMEMBER_i->PARAMETERS_COUNT) + AnsiString(", with ") + AnsiString(pMEMBER_i->MUST_PARAMETERS_COUNT) + AnsiString(" non default parameters, received ") + szparameters, *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), ((ClassCode *)(pMEMBER_i->Defined_In))->NAME, pMEMBER_i->NAME);
                    PIF->AcknoledgeRunTimeError(PREV, Exc);
                    return 0;
                }
                {
                    VariableDATA *THROW_DATA = 0;

                    STACK(PREV, AE ? AE->_DEBUG_INFO_LINE : 0)
                    RESULT = pMEMBER_i->Execute(PIF, this->CLSID, Owner, FORMAL_PARAM, SenderCTX, THROW_DATA, PREV);
                    UNSTACK;

                    if (THROW_DATA) {
                        throw THROW_DATA;
                    }
                }
                return RESULT;
                break;

            case 3:
                IS_PROPERTY = i + 1;
                if ((next_is_asg) && (PROPERTIES)) {
                    VariableDATA *IMAGE = (VariableDATA *)VAR_ALLOC(PIF);

                    if (!(*PROPERTIES)) {
                        *PROPERTIES = (VariableDATAPROPERTY *)FAST_MALLOC(sizeof(VariableDATAPROPERTY) * dataLen);
                        memset(*PROPERTIES, 0, sizeof(VariableDATAPROPERTY) * dataLen);
                    }
                    (*PROPERTIES)[result_id].CALL_SET           = Owner;
                    (*PROPERTIES)[result_id].IS_PROPERTY_RESULT = i + 1;
                    IMAGE->IS_PROPERTY_RESULT = 1;

                    IMAGE->LINKS       = 0;
                    IMAGE->NUMBER_DATA = 0;
                    IMAGE->TYPE        = VARIABLE_NUMBER;
                    return IMAGE;
                }
                if (!pMEMBER_i->MEMBER_GET) {
                    char          *mname = PIF->GeneralMembers->Item(i);
                    AnsiException *Exc   = new AnsiException(ERR370, AE->_DEBUG_INFO_LINE, 370, mname ? AnsiString(mname) : AE->_PARSE_DATA, *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
                    PIF->AcknoledgeRunTimeError(PREV, Exc);
                    return 0;
                }

                try {
                    if (!EMPTY_PARAM_LIST) {
                        EMPTY_PARAM_LIST              = new ParamList;
                        EMPTY_PARAM_LIST->COUNT       = 0;
                        EMPTY_PARAM_LIST->PARAM_INDEX = 0;
                    }
                    RESULT = ExecuteMember(PIF, pMEMBER_i->MEMBER_GET - 1, Owner, AE, 1, EMPTY_PARAM_LIST, SenderCTX, 1, ((ClassCode *)pMEMBER_i->Defined_In)->CLSID, ((ClassCode *)pMEMBER_i->Defined_In)->CLSID, PREV);
                    if (!RESULT) {
                        RESULT                     = (VariableDATA *)VAR_ALLOC(PIF);
                        RESULT->CLASS_DATA         = 0;
                        RESULT->LINKS              = 0;
                        RESULT->NUMBER_DATA        = 0;
                        RESULT->TYPE               = VARIABLE_NUMBER;
                        RESULT->IS_PROPERTY_RESULT = 0;
                    }
                    if ((FORMAL_PARAM) && ((!property) || (FORMAL_PARAM->COUNT))) {
                        if (RESULT->TYPE == VARIABLE_DELEGATE) {
                            CC_WRITE_LOCK(PIF)
                                ((CompiledClass *)RESULT->CLASS_DATA)->LINKS++;
                            CC_WRITE_UNLOCK(PIF)
                            VariableDATA * lOwner = 0;
                            try {
                                lOwner = (VariableDATA *)VAR_ALLOC(PIF);
                                lOwner->CLASS_DATA         = RESULT->CLASS_DATA;
                                lOwner->IS_PROPERTY_RESULT = 0;
                                lOwner->LINKS = 1;
                                lOwner->TYPE = VARIABLE_CLASS;

                                RESULT = ((CompiledClass *)RESULT->CLASS_DATA)->_Class->ExecuteMember(PIF,
                                                                                                      (INTEGER)RESULT->DELEGATE_DATA,
                                                                                                      lOwner,
                                                                                                      AE,
                                                                                                      1,
                                                                                                      FORMAL_PARAM,
                                                                                                      SenderCTX,
                                                                                                      0,
                                                                                                      CLSID,
                                                                                                      LOCAL_CLSID,
                                                                                                      PREV
                                                                                                      );
                                FREE_VARIABLE(lOwner);
                                return RESULT;
                            } catch (VariableDATA *LAST_THROW) {
                                FREE_VARIABLE(lOwner);
                                throw LAST_THROW;
                                return 0;
                            }
                        }
                        AnsiException *Exc = new AnsiException(ERR390, AE->_DEBUG_INFO_LINE, 390, AE->_PARSE_DATA, *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
                        PIF->AcknoledgeRunTimeError(PREV, Exc);

                        RESULT                     = (VariableDATA *)VAR_ALLOC(PIF);
                        RESULT->CLASS_DATA         = 0;
                        RESULT->LINKS              = 0;
                        RESULT->NUMBER_DATA        = 0;
                        RESULT->TYPE               = VARIABLE_NUMBER;
                        RESULT->IS_PROPERTY_RESULT = 0;
                        return RESULT;
                    }

                    if (!RESULT->LINKS) {
                        if (PROPERTIES) {
                            if (!(*PROPERTIES)) {
                                *PROPERTIES = (VariableDATAPROPERTY *)FAST_MALLOC(sizeof(VariableDATAPROPERTY) * dataLen);
                                memset(*PROPERTIES, 0, sizeof(VariableDATAPROPERTY) * dataLen);
                            }
                            (*PROPERTIES)[result_id].CALL_SET           = Owner;
                            (*PROPERTIES)[result_id].IS_PROPERTY_RESULT = i + 1;
                            RESULT->IS_PROPERTY_RESULT = 1;
                        } else
                            RESULT->IS_PROPERTY_RESULT = 0;
                    } else {
                        VariableDATA *IMAGE = (VariableDATA *)VAR_ALLOC(PIF);

                        if (PROPERTIES) {
                            if (!(*PROPERTIES)) {
                                *PROPERTIES = (VariableDATAPROPERTY *)FAST_MALLOC(sizeof(VariableDATAPROPERTY) * dataLen);
                                memset(*PROPERTIES, 0, sizeof(VariableDATAPROPERTY) * dataLen);
                            }
                            (*PROPERTIES)[result_id].CALL_SET           = Owner;
                            (*PROPERTIES)[result_id].IS_PROPERTY_RESULT = i + 1;
                            IMAGE->IS_PROPERTY_RESULT = 1;
                        } else
                            IMAGE->IS_PROPERTY_RESULT = 0;

                        IMAGE->LINKS = 0;
                        IMAGE->TYPE  = RESULT->TYPE;
                        if (RESULT->TYPE == VARIABLE_STRING) {
                            IMAGE->CLASS_DATA     = 0;
                            CONCEPT_STRING(IMAGE) = CONCEPT_STRING(RESULT);
                        } else
                        if ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_ARRAY)) {
                            IMAGE->CLASS_DATA = RESULT->CLASS_DATA;
                        } else
                        if (RESULT->TYPE == VARIABLE_DELEGATE) {
                            IMAGE->CLASS_DATA    = RESULT->CLASS_DATA;
                            IMAGE->DELEGATE_DATA = RESULT->DELEGATE_DATA;
                        } else {
                            IMAGE->NUMBER_DATA = RESULT->NUMBER_DATA;
                        }
                        //----------------------------------------------
                        if ((IMAGE->TYPE == VARIABLE_CLASS) || (IMAGE->TYPE == VARIABLE_DELEGATE)) {
                            CC_WRITE_LOCK(PIF)
                                ((CompiledClass *)IMAGE->CLASS_DATA)->LINKS++;
                            CC_WRITE_UNLOCK(PIF)
                        } else
                        if (IMAGE->TYPE == VARIABLE_ARRAY) {
                            CC_WRITE_LOCK(PIF)
                                ((Array *)IMAGE->CLASS_DATA)->LINKS++;
                            CC_WRITE_UNLOCK(PIF)
                        }
                        //----------------------------------------------
                        RESULT = IMAGE;
                    }
                    return RESULT;
                } catch (VariableDATA *LAST_THROW) {
                    throw LAST_THROW;
                    return 0;
                }
                break;

            default:
                CC_WRITE_LOCK(PIF)
                RESULT = ((CompiledClass *)Owner->CLASS_DATA)->_CONTEXT [relocation2 - 1];
                if (!RESULT) {
                    RESULT = ((CompiledClass *)Owner->CLASS_DATA)->CreateVariable(relocation2 - 1, pMEMBER_i);
                } else
                if ((RESULT->TYPE == VARIABLE_DELEGATE) && (FORMAL_PARAM) && (!property)) {
                    ((CompiledClass *)RESULT->CLASS_DATA)->LINKS++;
                    CC_WRITE_UNLOCK(PIF)
                    VariableDATA * lOwner = 0;
                    try {
                        lOwner = (VariableDATA *)VAR_ALLOC(PIF);
                        lOwner->CLASS_DATA         = RESULT->CLASS_DATA;
                        lOwner->IS_PROPERTY_RESULT = 0;
                        lOwner->LINKS = 1;
                        lOwner->TYPE = VARIABLE_CLASS;

                        RESULT = ((CompiledClass *)RESULT->CLASS_DATA)->_Class->ExecuteDelegate(PIF,
                                                                                                (INTEGER)RESULT->DELEGATE_DATA,
                                                                                                lOwner,
                                                                                                AE,
                                                                                                FORMAL_PARAM,
                                                                                                SenderCTX,
                                                                                                CLSID,
                                                                                                LOCAL_CLSID,
                                                                                                PREV);
                        FREE_VARIABLE(lOwner);
                        return RESULT;
                    } catch (VariableDATA *LAST_THROW) {
                        FREE_VARIABLE(lOwner);
                        throw LAST_THROW;
                        return 0;
                    }
                }
                CC_WRITE_UNLOCK(PIF)
                if ((FORMAL_PARAM) && (!property)) {
                    char          *mname = PIF->GeneralMembers->Item(i);
                    AnsiException *Exc   = new AnsiException(ERR250, AE->_DEBUG_INFO_LINE, 250, mname ? AnsiString(mname) : AE->_PARSE_DATA, *GetFilename(PIF, CLSID, &_DEBUG_INFO_FILENAME), ((ClassCode *)(pMEMBER_i->Defined_In))->NAME, NAME);
                    PIF->AcknoledgeRunTimeError(PREV, Exc);

                    RESULT                     = (VariableDATA *)VAR_ALLOC(PIF);
                    RESULT->CLASS_DATA         = 0;
                    RESULT->LINKS              = 0;
                    RESULT->NUMBER_DATA        = 0;
                    RESULT->TYPE               = VARIABLE_NUMBER;
                    RESULT->IS_PROPERTY_RESULT = 0;
                }

                return RESULT;
        }
    } else {
        char          *mname = PIF->GeneralMembers->Item(i);
        AnsiException *Exc   = new AnsiException(ERR150, AE->_DEBUG_INFO_LINE, 150, mname ? AnsiString(mname) : AE->_PARSE_DATA, *GetFilename(PIF, CLSID, &_DEBUG_INFO_FILENAME), NAME);
        PIF->AcknoledgeRunTimeError(PREV, Exc);
    }
    return 0;
}

void ClassCode::GenerateCode(StaticList *GeneralMembers) {
    INTEGER GeneralMembersCount = GeneralMembers->Count();
    INTEGER LocalMembersCount   = Members->Count();

    RELOCATIONS  = new INTEGER [GeneralMembersCount];
    RELOCATIONS2 = new CLASS_MEMBERS_DOMAIN[LocalMembersCount];
    memset(RELOCATIONS2, 0, sizeof(CLASS_MEMBERS_DOMAIN) * LocalMembersCount);
    memset(RELOCATIONS, 0, sizeof(INTEGER) * GeneralMembersCount);

    pMEMBERS = new ClassMember * [LocalMembersCount];

    INTEGER var_index = 0;
    for (INTEGER i = 0; i < LocalMembersCount; i++) {
        ClassMember *LocalMember = (ClassMember *)Members->Item(i);
        pMEMBERS [i] = LocalMember;

        //===================================================//
        if ((LocalMember) && (!LocalMember->IS_FUNCTION)) {
            RELOCATIONS2 [i] = ++var_index;
        }
        //===================================================//

        for (INTEGER j = 0; j < GeneralMembersCount; j++) {
            char *GeneralMember = GeneralMembers->Item(j);

            if (!strcmp(LocalMember->NAME, GeneralMember)) {
                RELOCATIONS [j] = i + 1;
                break;
            }
        }
    }

    DataMembersCount = var_index;
    pMEMBERS_COUNT   = LocalMembersCount;
}

int ClassCode::Serialize(PIFAlizator *PIF, FILE *out, bool is_lib) {
    char temp_needed = NEEDED;

    if (is_lib) {
        temp_needed = 1;
    }

    concept_fwrite(&temp_needed, sizeof(temp_needed), 1, out);
    if (!temp_needed) {
        return 0;
    }
    concept_fwrite_int(&CLSID, sizeof(CLSID), 1, out);
    NAME.Serialize(out, SERIALIZE_16BIT_LENGTH);

    INTEGER GeneralMembersCount = PIF->GeneralMembers->Count();
    INTEGER members_count       = Members->Count();
    concept_fwrite_int(&members_count, sizeof(members_count), 1, out);
    concept_fwrite_int(&CONSTRUCTOR, sizeof(CONSTRUCTOR), 1, out);
    concept_fwrite_int(&DESTRUCTOR, sizeof(DESTRUCTOR), 1, out);

    for (INTEGER ii = 0; ii < members_count; ii++) {
        ClassMember *CM_TARGET = (ClassMember *)Members->Item(ii);
        for (INTEGER jj = 0; jj < GeneralMembersCount; jj++) {
            int         reloc = RELOCATIONS [jj];
            ClassMember *CM   = reloc ? pMEMBERS [reloc - 1] : 0;
            if ((CM) && (CM == CM_TARGET)) {
                concept_fwrite_int(&jj, sizeof(jj), 1, out);
                char is_defined_in_this_class = ((void *)CM->Defined_In == (void *)this);
                concept_fwrite(&is_defined_in_this_class, sizeof(is_defined_in_this_class), 1, out);
                if (is_defined_in_this_class) {
                    CM->Serialize(out, is_lib);
                } else {
                    int clsid = ((ClassCode *)CM->Defined_In)->CLSID;
                    concept_fwrite_int(&clsid, sizeof(clsid), 1, out);
                }
                break;
            }
        }
    }
    return 0;
}

int ClassCode::ComputeSharedSize(concept_FILE *in, int general_members) {
    int size = 0;

    size += sizeof(AnsiList);
    signed char MyNEEDED;
    INTEGER     MyCLSID;

    FREAD_FAIL(&MyNEEDED, sizeof(MyNEEDED), 1, in);
    if (!MyNEEDED) {
        return 0;
    }

    FREAD_INT_FAIL(&MyCLSID, sizeof(MyCLSID), 1, in);

    AnsiString::ComputeSharedSize(in, SERIALIZE_16BIT_LENGTH);

    INTEGER members_count;
    FREAD_INT_FAIL(&members_count, sizeof(members_count), 1, in);

    // CONSTRUCTOR + DESTRUCTOR!
    SKIP(sizeof(INTEGER) + sizeof(INTEGER), in);

    //RELOCATIONS
    size += sizeof(INTEGER) * general_members;
    //RELOCATIONS2
    size += sizeof(CLASS_MEMBERS_DOMAIN) * members_count;

    for (INTEGER jj = 0; jj < members_count; jj++) {
        int index;
        FREAD_INT_FAIL(&index, sizeof(index), 1, in);

        signed char is_defined_in_this_class;
        FREAD_INT_FAIL(&is_defined_in_this_class, sizeof(is_defined_in_this_class), 1, in);

        if (is_defined_in_this_class) {
            signed char IS_FUNCTION = 0;
            size += ClassMember::ComputeSharedSize(in, IS_FUNCTION);
            if (IS_FUNCTION == 1) {
                size += Optimizer::ComputeSharedSize(in);
            }
        } else {
            int clsid;
            FREAD_INT_FAIL(&clsid, sizeof(clsid), 1, in);
        }
    }
    return size;
}

int ClassCode::Unserialize(PIFAlizator *PIF, concept_FILE *in, AnsiList *ClassList, bool is_lib, int *ClassNames, int *Relocation) {
    bool is_created = PIF->is_buffer ? 0 : SHIsCreated();
    bool is_pooled  = PIF->is_buffer ? 0 : SHIsPooled();

    if ((!is_lib) && (Members)) {
        delete Members;
        Members = 0;
    }

    concept_fread_int(&NEEDED, sizeof(NEEDED), 1, in);
    if (!NEEDED) {
        return 0;
    }

    if (!concept_fread_int(&CLSID, sizeof(CLSID), 1, in)) {
        return -1;
    }
    NAME.Unserialize(in, SERIALIZE_16BIT_LENGTH);

    if (is_lib) {
        ClassNames [CLSID] = ClassList->Count() - 1;
    }

    if (pMEMBERS) {
        delete [] pMEMBERS;
    }

    if (RELOCATIONS) {
        delete [] RELOCATIONS;
    }

    if (RELOCATIONS2) {
        delete [] RELOCATIONS2;
    }

    INTEGER members_count;
    if (!concept_fread_int(&members_count, sizeof(members_count), 1, in)) {
        return -1;
    }

    if (!concept_fread_int(&CONSTRUCTOR, sizeof(CONSTRUCTOR), 1, in)) {
        CONSTRUCTOR = -1;
        return -1;
    }

    if (!concept_fread_int(&DESTRUCTOR, sizeof(DESTRUCTOR), 1, in)) {
        DESTRUCTOR = -1;
        return -1;
    }

    if (!is_lib) {
        INTEGER GeneralMembersCount = PIF->GeneralMembers->Count();
        if (is_pooled) {
            RELOCATIONS  = (INTEGER *)SHAlloc(sizeof(INTEGER) * GeneralMembersCount);
            RELOCATIONS2 = (CLASS_MEMBERS_DOMAIN *)SHAlloc(sizeof(CLASS_MEMBERS_DOMAIN) * members_count);
            if (is_created) {
                memset(RELOCATIONS2, 0, sizeof(CLASS_MEMBERS_DOMAIN) * members_count);
                memset(RELOCATIONS, 0, sizeof(INTEGER) * GeneralMembersCount);
            }
        } else {
            RELOCATIONS  = new INTEGER [GeneralMembersCount];
            RELOCATIONS2 = new CLASS_MEMBERS_DOMAIN [members_count];
            memset(RELOCATIONS2, 0, sizeof(CLASS_MEMBERS_DOMAIN) * members_count);
            memset(RELOCATIONS, 0, sizeof(INTEGER) * GeneralMembersCount);
        }

        pMEMBERS       = new ClassMember * [members_count];
        pMEMBERS_COUNT = members_count;
    }

    CLASS_MEMBERS_DOMAIN reloc_index = 0;
    for (INTEGER jj = 0; jj < members_count; jj++) {
        int index;
        if (!concept_fread_int(&index, sizeof(index), 1, in)) {
            return -1;
        }

        if (is_lib) {
            index = Relocation [index];
        } else
        if ((!is_pooled) || (is_created)) {
            RELOCATIONS [index] = jj + 1;
        }

        char is_defined_in_this_class;
        if (!concept_fread(&is_defined_in_this_class, sizeof(is_defined_in_this_class), 1, in)) {
            return -1;
        }
        if (is_defined_in_this_class) {
            ClassMember *CM = new ClassMember(this, PIF->GeneralMembers->Item(index),  true, false, true);
            if (is_lib)
                Members->Add(CM, DATA_CLASS_MEMBER);
            if (!is_lib) {
                pMEMBERS [jj] = CM;
            }
            CM->Unserialize(in, ClassList, is_lib, Relocation, ClassNames);
            CM->Defined_In = this;

            if ((!is_lib) && (!CM->IS_FUNCTION)) {
                reloc_index++;
                if ((!is_pooled) || (is_created)) {
                    RELOCATIONS2 [jj] = reloc_index;
                }
            }

            if (CM->IS_FUNCTION == 1) {
                CM->OPTIMIZER = new Optimizer(PIF, &PIF->PIF, &PIF->VariableDescriptors, ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME, this, CM->NAME, true);
                ((Optimizer *)CM->OPTIMIZER)->Unserialize(in, PIF->ModuleList, is_lib, ClassNames, Relocation);
            } else {
                CM->OPTIMIZER = 0;
            }
        } else {
            int clsid;
            if (!concept_fread_int(&clsid, sizeof(clsid), 1, in)) {
                return -1;
            }

            if (is_lib) {
                clsid = ClassNames [clsid];
            }

            ClassCode *CC_parent = (ClassCode *)ClassList->Item(clsid);

            if (!CC_parent) {
                return -1;
            }

            if (!first_parent) {
                first_parent = CC_parent;
            }

            ClassMember *CM_inherited = 0;

            AnsiString mem_name(PIF->GeneralMembers->Item(index));
            if (is_lib) {
                CM_inherited = (ClassMember *)CC_parent->Members->Item(CC_parent->HasMember(mem_name) - 1);
            } else {
                int reloc = CC_parent->RELOCATIONS [index];

                CM_inherited = reloc ? CC_parent->pMEMBERS [reloc - 1] : 0;
            }
            if (!CM_inherited) {
                return -1;
            }

            if (is_lib)
                Members->Add(CM_inherited, DATA_CLASS_MEMBER, true);
            else {
                pMEMBERS [jj] = CM_inherited;
                if (!CM_inherited->IS_FUNCTION) {
                    reloc_index++;
                    if ((!is_pooled) || (is_created)) {
                        RELOCATIONS2 [jj] = reloc_index;
                    }
                }
            }
        }
    }
    if (CONSTRUCTOR) {
        if (is_lib)
            this->CONSTRUCTOR_MEMBER = (ClassMember *)Members->Item(CONSTRUCTOR - 1);
        else
            this->CONSTRUCTOR_MEMBER = pMEMBERS [CONSTRUCTOR - 1];
    }
    if (DESTRUCTOR) {
        if (is_lib)
            this->DESTRUCTOR_MEMBER = (ClassMember *)Members->Item(DESTRUCTOR - 1);
        else
            this->DESTRUCTOR_MEMBER = pMEMBERS [DESTRUCTOR - 1];
    }

    if (!is_lib) {
        this->DataMembersCount = reloc_index;
    }

    this->DEFINED_LEVEL = PIF->INCLUDE_LEVEL;

    return 0;
}

int ClassCode::Inherits(INTEGER CLSID) {
    if (this->CLSID == CLSID) {
        return 1;
    }

    ClassCode *target_cc = (ClassCode *)this->first_parent;
    while (target_cc) {
        if (target_cc->CLSID == CLSID) {
            return 1;
        }

        target_cc = (ClassCode *)target_cc->first_parent;
    }
    if (first_parent) {
        for (int i = 0; i < this->pMEMBERS_COUNT; i++) {
            ClassMember *CM = this->pMEMBERS [i];
            if (CM) {
                if (((ClassCode *)CM->Defined_In)->CLSID == CLSID) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void ClassCode::UpdateNeeded() {
    if (NEEDED) {
        return;
    }
    for (int i = 0; i < this->pMEMBERS_COUNT; i++) {
        ClassMember *CM = this->pMEMBERS [i];
        if (CM) {
            if ((((ClassCode *)CM->Defined_In) != this) && (!((ClassCode *)CM->Defined_In)->NEEDED)) {
                ((ClassCode *)CM->Defined_In)->NEEDED = 1;
            }
        }
    }
}

void ClassCode::BeforeDestructor(PIFAlizator *PIF) {
    if ((pMEMBERS) && (!Members)) {
        for (int i = 0; i < pMEMBERS_COUNT; i++) {
            ClassMember *CM = this->pMEMBERS [i];
            if ((CM) && (CM->Defined_In == this))
                PIF->DeletedMembers.Add(CM, DATA_CLASS_MEMBER);
        }
    }
}

ClassCode::~ClassCode(void) {
    char is_pooled = SHIsPooled();

    if (pMEMBERS) {
        delete[] pMEMBERS;
        pMEMBERS = 0;
    }
    if (EMPTY_PARAM_LIST) {
        delete EMPTY_PARAM_LIST;
        EMPTY_PARAM_LIST = 0;
    }

    if (Members) {
        delete Members;
        Members = 0;
    }
    if (!is_pooled) {
        if (RELOCATIONS) {
            delete [] RELOCATIONS;
        }
        if (RELOCATIONS2) {
            delete [] RELOCATIONS2;
        }
    }
}

GreenThreadCycle *ClassCode::CreateThread(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner) {
    register ClassMember *pMEMBER_i = i ? pMEMBERS [i - 1] : 0;

    if (pMEMBER_i) {
        while (pMEMBER_i->IS_FUNCTION == 2) {
            i = pMEMBER_i->MEMBER_GET - 1;
            int relocation = this->RELOCATIONS [i];
            pMEMBER_i = relocation ? pMEMBERS [relocation - 1] : 0;
            if (!pMEMBER_i) {
                return 0;
            }
        }

        if (pMEMBER_i->IS_FUNCTION == 1) {
            if (pMEMBER_i->MUST_PARAMETERS_COUNT) {
                AnsiException *Exc = new AnsiException(ERR1310, 0, 1310, AnsiString("required ") + AnsiString(pMEMBER_i->PARAMETERS_COUNT) + AnsiString(", with ") + AnsiString(pMEMBER_i->MUST_PARAMETERS_COUNT) + AnsiString(" non default parameters, must 0"), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), ((ClassCode *)(pMEMBER_i->Defined_In))->NAME, pMEMBER_i->NAME);
                PIF->AcknoledgeRunTimeError(NULL, Exc);
                return 0;
            }
            return pMEMBER_i->CreateThread(PIF, this->CLSID, Owner);
        }
    }
    return 0;
}

