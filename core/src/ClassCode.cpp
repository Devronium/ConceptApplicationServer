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

#define FIND_RELOCATION(mid, i, result, limit) \
        result = NULL; \
        if (limit > 5) { \
            INTEGER start = 0; \
            INTEGER end = limit - 1; \
            INTEGER middle = end / 2;\
            MemberLink *member; \
            while (start <= end) { \
                member = &RELOCATIONS[middle]; \
                i = middle; \
                if (member->mid < mid) \
                    start = middle + 1;    \
                else \
                if (member->mid == mid) { \
                    result = member; \
                    break; \
                } else\
                    end = middle - 1; \
                middle = (start + end) / 2; \
            } \
        } else \
        if (limit) { \
            for (i = 0; i < limit; i++) { \
                if (mid == RELOCATIONS[i].mid) { \
                    result = &RELOCATIONS[i]; \
                    break; \
                } \
                if (mid < RELOCATIONS[i].mid) \
                    break; \
            } \
        }

struct ClassDelegate *new_Delegate(void *CLASS_DATA, CLASS_MEMBERS_DOMAIN DELEGATE_DATA) {
    struct ClassDelegate *self = (struct ClassDelegate *)malloc(sizeof(struct ClassDelegate));
    self->CLASS_DATA = CLASS_DATA;
    self->DELEGATE_DATA = DELEGATE_DATA;
    if (CLASS_DATA)
        ((CompiledClass *)CLASS_DATA)->LINKS ++;
    return self;
}

void *delegate_Class(const void *self) {
    if (!self)
        return 0;
    return ((struct ClassDelegate *)self)->CLASS_DATA;
}

CLASS_MEMBERS_DOMAIN delegate_Member(const void *self) {
    if (!self)
        return 0;
    return ((struct ClassDelegate *)self)->DELEGATE_DATA;
}

void delete_Delegate(void *self, SCStack *STACK_TRACE) {
    if (!self)
        return;
    struct ClassDelegate *deleg = (struct ClassDelegate *)self;
    if (deleg->CLASS_DATA) {
        if (!(--((struct CompiledClass *)deleg->CLASS_DATA)->LINKS))
            delete_CompiledClass((struct CompiledClass *)deleg->CLASS_DATA, STACK_TRACE);
    }
    free(deleg);
}

void free_Delegate(void *self) {
    if (!self)
        return;
    struct ClassDelegate *deleg = (struct ClassDelegate *)self;
    free(deleg);
}

struct ClassDelegate *copy_Delegate(const void *deleg) {
    if (!deleg)
        return 0;
    return new_Delegate(((struct ClassDelegate *)deleg)->CLASS_DATA, ((struct ClassDelegate *)deleg)->DELEGATE_DATA);
}

ClassCode::ClassCode(const char *name, PIFAlizator *P, char binary_form) {
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
    CompiledClass *CCLASS = (struct CompiledClass *)cc;
    INTEGER       Count   = CCLASS->_Class->DataMembersCount;

    if (!CCLASS->_CONTEXT) {
        return;
    }

    for (INTEGER i = 0; i < Count; i++) {
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

ClassMember *ClassCode::AddMember(PIFAlizator *PIF, const char *name, INTEGER line, const char *FileName, INTEGER ACCESS, char data_only) {
    INTEGER Count = Members->Count();

    for (INTEGER i = 0; i < Count; i++) {
        ClassMember *CM = (ClassMember *)Members->Item(i);
        if (!strcmp(CM->NAME, name)) {
            AnsiException *Exc = new AnsiException(ERR060, line, 60, name, FileName, NAME);
            PIF->Errors.Add(Exc, DATA_EXCEPTION);
            break;
        }
    }
    ClassMember *CM = new ClassMember(this, name, data_only);
    ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME = FileName;
    CM->_DEBUG_STARTLINE = line;
    Members->Add(CM, DATA_CLASS_MEMBER);
    CM->ACCESS = ACCESS;

    if ((!PIF->ASG_OVERLOADED) && (name) && (!strcmp(name, ASG)))
        PIF->ASG_OVERLOADED = 1;
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
    struct plainstring *Mname = plainstring_new_str(((ClassCode *)CM->Defined_In)->NAME.c_str());

    plainstring_add(Mname, "@");
    plainstring_add(Mname, CM->NAME);

    int has_it = Sender->HasMember(plainstring_c_str(Mname));
    if (has_it) {
        return has_it;
    }

    int  ref_id    = PIF->GeneralMembers->ContainsString(plainstring_c_str(Mname));
    const char *ref_name = 0;
    if (!ref_id) {
        PIF->GeneralMembers->Add(plainstring_c_str(Mname), plainstring_len(Mname));
        ref_id = PIF->GeneralMembers->Count();
    }
    plainstring_delete(Mname);

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

    if (CM->CDATA) {
        int len = CM->CDATA->VariableDescriptors->Count();
        for (int i = 0; i < len; i++) {
            VariableDESCRIPTOR *VD  = (VariableDESCRIPTOR *)CM->CDATA->VariableDescriptors->Item(i);
            VariableDESCRIPTOR *VD2 = new VariableDESCRIPTOR;
            COPY_VD(VD2, VD);
            CM2->CDATA->VariableDescriptors->Add(VD2, DATA_VAR_DESCRIPTOR);
        }

        len = CM->CDATA->PIF_DATA->Count();
        for (int i = 0; i < len; i++) {
            AnalizerElement *AE  = (AnalizerElement *)CM->CDATA->PIF_DATA->Item(i);
            AnalizerElement *AE2 = new AnalizerElement;
            AE2->_DEBUG_INFO_LINE = AE->_DEBUG_INFO_LINE;
            AE2->_HASH_DATA       = AE->_HASH_DATA;
            AE2->_INFO_OPTIMIZED  = AE->_INFO_OPTIMIZED;
            AE2->_PARSE_DATA      = AE->_PARSE_DATA;
            AE2->ID   = AE->ID;
            AE2->TYPE = AE->TYPE;

            CM2->CDATA->PIF_DATA->Add(AE2, DATA_ANALIZER_ELEMENT);
        }
    } else if ((CM->IS_FUNCTION == 1) && (CM->OPTIMIZER)) {
        CM2->OPTIMIZER  = CM->OPTIMIZER;
        CM2->MEMBER_GET = -1;
    }
    Sender->Members->Add(CM2, DATA_CLASS_MEMBER);

    return 0;
}

int ClassCode::RemoveMember(PIFAlizator *PIF, const char *name, INTEGER line, const char *FileName) {
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
            Members->Delete(i);

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

int ClassCode::HasMember(const char *name) const {
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

int ClassCode::CanBeRunStatic(const char *name, ClassMember **member) {
    INTEGER Count = Members->Count();

    for (INTEGER i = 0; i < Count; i++) {
        ClassMember *CM = (ClassMember *)Members->Item(i);
        if ((CM->NAME) && (!strcmp(CM->NAME, name))) {
            if (member)
                *member = CM;
            return CM->IS_STATIC;
        }
    }
    return 0;
}

int ClassCode::HasMemberInCompiled(const char *name, INTEGER *m_type) const {
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
        int reloc = Relocation(j++);
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

int ClassCode::GetAbsoluteMemberID(int mid) const {
    int j = 0;
    while (Relocation(j++) != mid);
    return j;
}

int ClassCode::GetSerialMembers(CompiledClass *CC, int max_members,
                                char **pmembers, unsigned char *flags, char *access,
                                char *types, char **szValue,
                                double *n_data, void **class_data, void **variable_data, int all_members) const {
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
            pmembers [index] = (char *)CM->NAME;
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
                if ((CC->_CONTEXT) && (!CC->_CONTEXT [reloc]) && (all_members)) {
                    if ((CM->VD) && ((CM->VD->TYPE != VARIABLE_NUMBER) || (CM->VD->nValue)))
                        CompiledClass_CreateVariable(CC, reloc, CM);
                }

                if ((CC->_CONTEXT) && (CC->_CONTEXT [reloc])) {
                    types [index]         = CC->_CONTEXT [reloc]->TYPE;
                    variable_data [index] = CC->_CONTEXT [reloc];
                    switch (CC->_CONTEXT [reloc]->TYPE) {
                        case VARIABLE_STRING:
                            n_data [index]  = CONCEPT_C_LENGTH(CC->_CONTEXT [reloc]);
                            szValue [index] = (char *)CONCEPT_C_STRING(CC->_CONTEXT [reloc]);
                            break;

                        case VARIABLE_CLASS:
                            szValue [index]    = (char *)((struct CompiledClass *)CC->_CONTEXT [reloc]->CLASS_DATA)->_Class->NAME.c_str();
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
                                class_data [index] = delegate_Class(CC->_CONTEXT [reloc]->CLASS_DATA);
                                int         relocation = delegate_Member(CC->_CONTEXT [reloc]->CLASS_DATA);
                                ClassMember *CMD       = relocation ? ((struct CompiledClass *)delegate_Class(CC->_CONTEXT [reloc]->CLASS_DATA))->_Class->pMEMBERS [relocation - 1] : 0;
                                if (CMD) {
                                    n_data [index] = (intptr_t)CMD->NAME;
                                } else {
                                    n_data [index] = 0;
                                }
                                szValue [index] = (char *)((struct CompiledClass *)delegate_Class(CC->_CONTEXT [reloc]->CLASS_DATA))->_Class->NAME.c_str();
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

INTEGER ClassCode::Relocation(INTEGER mid) const {
    if (RELOCATIONS) {
#ifdef HASH_RELOCATION
        khiter_t k = kh_get(inthashtable, RELOCATIONS, mid);
        if ((k != kh_end(RELOCATIONS)) && (kh_exist(RELOCATIONS, k)))
            return kh_val(RELOCATIONS, k);
#else
        INTEGER index;
        MemberLink *link;
        FIND_RELOCATION(mid, index, link, pMEMBERS_COUNT);
        if (link)
            return link->lid + 1;
#endif
    }
    return 0;
}

#ifndef HASH_RELOCATION
void ClassCode::FindRelocation(INTEGER mid, INTEGER &i, MemberLink *&result, INTEGER limit) {
    FIND_RELOCATION(mid, i, result, limit);
}
#endif

void ClassCode::SetRelocation(INTEGER mid, INTEGER index) {
    if (RELOCATIONS) {
#ifdef HASH_RELOCATION
        khiter_t k = kh_get(inthashtable, RELOCATIONS, mid);
        if ((k != kh_end(RELOCATIONS)) && (kh_exist(RELOCATIONS, k))) {
            kh_value(RELOCATIONS, k) = index;
            return;
        }
        int ret;
        k = kh_put(inthashtable, RELOCATIONS, mid, &ret);
        kh_value(RELOCATIONS, k) = index;
#else
        INTEGER i = 0;
        MemberLink *link;
        index--;
        FindRelocation(mid, i, link, index);
        if ((i < index) && (RELOCATIONS[i].mid < mid))
            i++;

        if (i < index) {
            for (int k = index - 1; k >= i; k--) {
                RELOCATIONS[k + 1].mid = RELOCATIONS[k].mid;
                RELOCATIONS[k + 1].lid = RELOCATIONS[k].lid;
            }
        }
        RELOCATIONS[i].mid = mid;
        RELOCATIONS[i].lid = index;
#endif
    }
}

CompiledClass *ClassCode::CreateInstance(PIFAlizator *PIF, VariableDATA *Owner, const RuntimeOptimizedElement *OE, const ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, SCStack *PREV, char is_static) const {
    static const ParamList dummy = { 0, 0 };
    CompiledClass *res  = new_CompiledClass(PIF, this);

    Owner->CLASS_DATA = res;

    if (CONSTRUCTOR) {
        ClassMember *CM = this->CONSTRUCTOR_MEMBER;

        if (SenderCTX) {
            //==========================//
            if (!FORMAL_PARAM) {
                FORMAL_PARAM = &dummy;
            }
            //==========================//

            if (!(ENOUGH_PARAMETERS(CM, FORMAL_PARAM))) {
                AnsiException *Exc = new AnsiException(220, ERR220, OE ? OE->Operator_DEBUG_INFO_LINE : 0, CM->NAME, " requires ", CM->PARAMETERS_COUNT, ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME, NAME);
                PIF->AcknoledgeRunTimeError(PREV, Exc);
                return 0;
            }
#ifndef SIMPLE_MULTI_THREADING
            if ((FORMAL_PARAM->COUNT == 1) && (CM->FastOptimizedExecute(PIF, res, FORMAL_PARAM, SenderCTX)))
                return res;
#endif
        } else if (CM->MUST_PARAMETERS_COUNT) {
            AnsiException *Exc = new AnsiException(ERR230, 0, 230, NULL_STRING, _DEBUG_INFO_FILENAME, NAME);
            PIF->AcknoledgeRunTimeError(PREV, Exc);
            return 0;
        }

        if (!PIF->RootInstance) // if (is_static)
            PIF->RootInstance = res;
        VariableDATA *THROW_DATA = 0;
        STACK(PREV, OE ? OE->Operator_DEBUG_INFO_LINE : 0)
        VariableDATA * RESULT = CM->Execute(PIF, this->CLSID, (CompiledClass *)Owner->CLASS_DATA, FORMAL_PARAM, SenderCTX, THROW_DATA, PREV, NULL, 0, is_static);
        UNSTACK;
        if (RESULT) {
            if (is_static) {
                if (RESULT->TYPE == VARIABLE_NUMBER)
                    PIF->last_result = (int)RESULT->NUMBER_DATA;
            }
            FREE_VARIABLE(RESULT, PREV);
        } else
        if (THROW_DATA) {
            struct plainstring *data = plainstring_new();

            if (OE) {
                plainstring_set(data, ((TinyString)OE->OperandRight_PARSE_DATA).c_str());
                plainstring_add(data, ", data:");
            }

            // check if is not the static call to Main. In this case the THROW_DATA is/may be deleted
            if (SenderCTX) {
                if (THROW_DATA->TYPE == VARIABLE_STRING) {
                    plainstring_add(data, CONCEPT_C_STRING(THROW_DATA));
                } else
                if (THROW_DATA->TYPE == VARIABLE_NUMBER) {
                    plainstring_add_double(data, THROW_DATA->NUMBER_DATA);
                } else
                if (THROW_DATA->TYPE == VARIABLE_CLASS) {
                    plainstring_add(data, CompiledClass_GetClassName((struct CompiledClass *)THROW_DATA->CLASS_DATA));
                } else
                if (THROW_DATA->TYPE == VARIABLE_ARRAY) {
                    plainstring_add(data, "Array");
                } else
                if (THROW_DATA->TYPE == VARIABLE_DELEGATE) {
                    plainstring_add(data, "Delegate of ");
                    plainstring_add(data, CompiledClass_GetClassName((struct CompiledClass *)delegate_Class(THROW_DATA->CLASS_DATA)));
                }

                FREE_VARIABLE(THROW_DATA, PREV);
            }

            AnsiException *Exc = new AnsiException(ERR630, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 630, plainstring_c_str(data), ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME, NAME);
            PIF->AcknoledgeRunTimeError(PREV, Exc);

            plainstring_delete(data);
        }
    } else if ((FORMAL_PARAM) && (FORMAL_PARAM->COUNT)) {
        AnsiException *Exc = new AnsiException(ERR280, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 280, NAME, _DEBUG_INFO_FILENAME, NAME);
        PIF->AcknoledgeRunTimeError(PREV, Exc);
        return 0;
    }
    return res;
}

void ClassCode::SetProperty(PIFAlizator *PIF, INTEGER i, struct CompiledClass *Owner, const RuntimeOptimizedElement *OE, INTEGER local, INTEGER VALUE, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, VariableDATA **LOCAL_THROW, SCStack *PREV) const {
    int relocation = this->Relocation(i);
    *LOCAL_THROW = NULL;
    ClassMember *pMEMBER_i = relocation ? pMEMBERS [relocation - 1] : 0;

    if (pMEMBER_i) {
        if (((ClassCode *)pMEMBER_i->Defined_In)->CLSID != LOCAL_CLSID) {
            if (pMEMBER_i->ACCESS == ACCESS_PRIVATE) {
                const char *mname = pMEMBER_i->NAME;
                AnsiException *Exc   = new AnsiException(ERR190, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 190, mname ? mname : (OE ? ((TinyString)OE->OperandRight_PARSE_DATA).c_str() : pMEMBER_i->NAME), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
                PIF->AcknoledgeRunTimeError(PREV, Exc);
                return;
            }
            if ((pMEMBER_i->ACCESS == ACCESS_PROTECTED) && (!local)) {
                ClassCode *caller = (ClassCode *)PIF->StaticClassList[LOCAL_CLSID];
                if ((!caller) || (!caller->Inherits(this->CLSID))) {
                    const char *mname = pMEMBER_i->NAME;
                    AnsiException *Exc   = new AnsiException(ERR850, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 850, mname ? mname : (OE ? ((TinyString)OE->OperandRight_PARSE_DATA).c_str() : pMEMBER_i->NAME), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
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
            const char *mname = pMEMBER_i->NAME;
            AnsiException *Exc   = new AnsiException(ERR410, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 410, mname ? mname : (OE ? OE->OperandRight_PARSE_DATA.c_str() : ""), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
            PIF->AcknoledgeRunTimeError(PREV, Exc);
            return;
        }
        VariableDATA *RESULT = ExecuteMember(PIF, pMEMBER_i->MEMBER_SET - 1, Owner, OE, 1, ONE_PARAM_LIST, SenderCTX, 1, ((ClassCode *)pMEMBER_i->Defined_In)->CLSID, ((ClassCode *)pMEMBER_i->Defined_In)->CLSID, LOCAL_THROW, PREV);
        if (RESULT) {
            // new zone added //
            CLASS_CHECK(RESULT, PREV);

            VariableDATA *NEW_VALUE = SenderCTX [VALUE - 1];
            RESULT->TYPE = NEW_VALUE->TYPE;
            switch (RESULT->TYPE) {
                case VARIABLE_NUMBER:
                    RESULT->NUMBER_DATA = NEW_VALUE->NUMBER_DATA;
                    break;

                case VARIABLE_STRING:
                    RESULT->CLASS_DATA     = NULL;
                    CONCEPT_STRING(RESULT, NEW_VALUE);
                    break;

                case VARIABLE_CLASS:
                    RESULT->CLASS_DATA = NEW_VALUE->CLASS_DATA;
                    ((struct CompiledClass *)RESULT->CLASS_DATA)->LINKS++;
                    break;

                case VARIABLE_ARRAY:
                    RESULT->CLASS_DATA = NEW_VALUE->CLASS_DATA;
                    ((struct Array *)RESULT->CLASS_DATA)->LINKS++;
                    break;

                case VARIABLE_DELEGATE:
                    RESULT->CLASS_DATA = copy_Delegate(NEW_VALUE->CLASS_DATA);
                    break;
            }
            //----------------------------------------------
            // end zone added //
            RESULT->LINKS++;
            FREE_VARIABLE(RESULT, PREV);
        }
        return;
    }

    const char *mname = PIF->GeneralMembers->Item(i);
    AnsiException *Exc   = new AnsiException(ERR150, OE->Operator_DEBUG_INFO_LINE, 150, mname ? mname : (OE ? OE->OperandRight_PARSE_DATA.c_str() : ""), *GetFilename(PIF, CLSID, &_DEBUG_INFO_FILENAME), NAME);
    PIF->AcknoledgeRunTimeError(PREV, Exc);
    return;
}

const TinyString *ClassCode::GetFilename(PIFAlizator *PIF, INTEGER LOCAL_CLSID, const TinyString *default_Value) const {
    if (LOCAL_CLSID < 0)
        return default_Value;
    ClassCode *CC;
    if (PIF->StaticClassList)
        CC = PIF->StaticClassList[LOCAL_CLSID];
    else
        CC = (ClassCode *)PIF->ClassList->Item(LOCAL_CLSID);
    if (CC)
        return &CC->_DEBUG_INFO_FILENAME;
    return NULL;
}

VariableDATA *ClassCode::ExecuteDelegate(PIFAlizator *PIF, INTEGER i, struct CompiledClass *Owner, const RuntimeOptimizedElement *OE, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, VariableDATA **LOCAL_THROW, SCStack *PREV, int result_id THREAD_CREATION_LOCKS) const {
    VariableDATA         *RESULT;
    ClassMember *pMEMBER_i = i ? pMEMBERS [i - 1] : 0;
    *LOCAL_THROW = NULL;

    if (pMEMBER_i) {
        if (!(ENOUGH_PARAMETERS(pMEMBER_i, FORMAL_PARAM))) {
            this->BuildParameterError(PIF, OE ? OE->Operator_DEBUG_INFO_LINE : 0, FORMAL_PARAM ? FORMAL_PARAM->COUNT : 0, pMEMBER_i, PREV);
            return 0;
        }
        VariableDATA *THROW_DATA = 0;

        STACK(PREV, OE ? OE->Operator_DEBUG_INFO_LINE : 0)
#ifdef SIMPLE_MULTI_THREADING
        RESULT = pMEMBER_i->Execute(PIF, this->CLSID, Owner, FORMAL_PARAM, SenderCTX, *LOCAL_THROW, PREV, (result_id > 0) ? SenderCTX[result_id] : NULL, 0, 0, thread_lock);
#else
        RESULT = pMEMBER_i->Execute(PIF, this->CLSID, Owner, FORMAL_PARAM, SenderCTX, *LOCAL_THROW, PREV, (result_id > 0) ? SenderCTX[result_id] : NULL, 0, 0);
#endif
        UNSTACK;

        if (*LOCAL_THROW)
            RESULT = NULL;

        return RESULT;
    }
    return 0;
}

void ClassCode::BuildParameterError(PIFAlizator *PIF, int line, int FORMAL_PARAM_COUNT, const ClassMember *pMEMBER_i, SCStack *PREV, int ack_error) const {
    struct plainstring *err = plainstring_new_str("required ");
    if (pMEMBER_i)
        plainstring_add_int(err, pMEMBER_i->PARAMETERS_COUNT);
    plainstring_add(err, ", with ");
    if (pMEMBER_i)
        plainstring_add_int(err, pMEMBER_i->MUST_PARAMETERS_COUNT);
    plainstring_add(err, " non default parameters, received ");
    if (FORMAL_PARAM_COUNT)
        plainstring_add_int(err, FORMAL_PARAM_COUNT);
    else
        plainstring_add(err, "none");
    AnsiException *Exc = new AnsiException(ERR210, line, 210, plainstring_c_str(err), pMEMBER_i ? GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME)->c_str() : "", pMEMBER_i ? ((ClassCode *)(pMEMBER_i->Defined_In))->NAME.c_str() : "", pMEMBER_i ? pMEMBER_i->NAME : "");
    plainstring_delete(err);
    if (ack_error)
        PIF->AcknoledgeRunTimeError(PREV, Exc);
    else
        PIF->Errors.Add(Exc, DATA_EXCEPTION);
}

VariableDATA *ClassCode::ExecuteMember(PIFAlizator *PIF, INTEGER i, struct CompiledClass *Owner, const RuntimeOptimizedElement *OE, INTEGER local, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, char property, INTEGER CLSID, INTEGER LOCAL_CLSID, VariableDATA **LOCAL_THROW, SCStack *PREV, char next_is_asg, VariableDATAPROPERTY **PROPERTIES, int dataLen, int result_id, int relocation) const {
    INTEGER      IS_PROPERTY = 0;
    VariableDATA *RESULT;
    struct CompiledClass *CLASS_DATA;
    if (relocation <= 0)
        relocation = this->Relocation(i);
    *LOCAL_THROW = NULL;

    ClassMember *pMEMBER_i = relocation ? pMEMBERS [relocation - 1] : 0;
    if (pMEMBER_i) {
        if (((ClassCode *)pMEMBER_i->Defined_In)->CLSID != LOCAL_CLSID) {
            if (pMEMBER_i->ACCESS == ACCESS_PRIVATE) {
                const char *mname = pMEMBER_i->NAME;
                AnsiException *Exc   = new AnsiException(ERR190, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 190, mname ? mname : (OE ? ((TinyString)OE->OperandRight_PARSE_DATA).c_str() : pMEMBER_i->NAME), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), ((ClassCode *)(pMEMBER_i->Defined_In))->NAME, NAME);
                PIF->AcknoledgeRunTimeError(PREV, Exc);
                return 0;
            }
            if ((pMEMBER_i->ACCESS == ACCESS_PROTECTED) && (!local)) {
                ClassCode *caller = (ClassCode *)PIF->StaticClassList[LOCAL_CLSID];
                if ((!caller) || (!caller->Inherits(this->CLSID))) {
                    const char *mname = pMEMBER_i->NAME;
                    AnsiException *Exc   = new AnsiException(ERR850, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 850, mname ? mname : (OE ? ((TinyString)OE->OperandRight_PARSE_DATA).c_str() : pMEMBER_i->NAME), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), ((ClassCode *)(pMEMBER_i->Defined_In))->NAME, NAME);
                    PIF->AcknoledgeRunTimeError(PREV, Exc);
                    return 0;
                }
            }
        }
#ifndef DISABLE_DEPRECATED_FEATURES
        while (pMEMBER_i->IS_FUNCTION == 2) {
            i          = pMEMBER_i->MEMBER_GET - 1;
            relocation = this->Relocation(i);
            pMEMBER_i  = relocation ? pMEMBERS [relocation - 1] : 0;
            if (!pMEMBER_i) {
                return 0;
            }
        }
#endif

        int relocation2;
        switch (pMEMBER_i->IS_FUNCTION) {
            case 1:
                if (!FORMAL_PARAM) {
                    VariableDATA *deleg = (VariableDATA *)VAR_ALLOC(PIF);
                    deleg->IS_PROPERTY_RESULT = 0;
                    deleg->LINKS         = 0;
                    CC_WRITE_LOCK(PIF)
                    deleg->CLASS_DATA    = new_Delegate(Owner, relocation);
                    CC_WRITE_UNLOCK(PIF)
                    deleg->TYPE          = VARIABLE_DELEGATE;
                    return deleg;
                } else
                if (!(ENOUGH_PARAMETERS(pMEMBER_i, FORMAL_PARAM))) {
                    this->BuildParameterError(PIF, OE ? OE->Operator_DEBUG_INFO_LINE : 0, FORMAL_PARAM->COUNT, pMEMBER_i, PREV);
                    return 0;
                }
                {
                    STACK(PREV, OE ? OE->Operator_DEBUG_INFO_LINE : 0)
                    RESULT = pMEMBER_i->Execute(PIF, this->CLSID, Owner, FORMAL_PARAM, SenderCTX, *LOCAL_THROW, PREV, (result_id > 0) ? SenderCTX[result_id] : NULL, OE ? OE->Operator_FLAGS : 0);
                    UNSTACK;

                    if (*LOCAL_THROW) {
                        if (RESULT) {
                            FREE_VARIABLE(RESULT, PREV);
                        }
                        return NULL;
                    }
                }
                return RESULT;

            case 3:
                IS_PROPERTY = i + 1;
                if ((next_is_asg) && (PROPERTIES)) {
                    VariableDATA *IMAGE;
                    
                    if (result_id > 0)  {
                        IMAGE = SenderCTX[result_id];
                        CLASS_CHECK_NO_RESET(IMAGE, PREV);
                    } else {
                        IMAGE = (VariableDATA *)VAR_ALLOC(PIF);
                        IMAGE->LINKS = 0;
                    }

                    if (!(*PROPERTIES)) {
                        *PROPERTIES = (VariableDATAPROPERTY *)FAST_MALLOC(PIF, sizeof(VariableDATAPROPERTY) * dataLen);
                        memset(*PROPERTIES, 0, sizeof(VariableDATAPROPERTY) * dataLen);
                    }
                    (*PROPERTIES)[result_id].CALL_SET           = Owner;
                    (*PROPERTIES)[result_id].IS_PROPERTY_RESULT = i + 1;
                    IMAGE->IS_PROPERTY_RESULT = 1;

                    IMAGE->NUMBER_DATA = 0;
                    IMAGE->TYPE        = VARIABLE_NUMBER;
                    return IMAGE;
                }
                if (!pMEMBER_i->MEMBER_GET) {
                    const char *mname = pMEMBER_i->NAME;
                    AnsiException *Exc   = new AnsiException(ERR370, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 370, mname ? mname : (OE ? OE->OperandRight_PARSE_DATA.c_str() : ""), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
                    PIF->AcknoledgeRunTimeError(PREV, Exc);
                    return 0;
                }

                if (!EMPTY_PARAM_LIST) {
                    EMPTY_PARAM_LIST              = new ParamList;
                    EMPTY_PARAM_LIST->COUNT       = 0;
                    EMPTY_PARAM_LIST->PARAM_INDEX = 0;
                }
                RESULT = ExecuteMember(PIF, pMEMBER_i->MEMBER_GET - 1, Owner, OE, 1, EMPTY_PARAM_LIST, SenderCTX, 1, ((ClassCode *)pMEMBER_i->Defined_In)->CLSID, ((ClassCode *)pMEMBER_i->Defined_In)->CLSID, LOCAL_THROW, PREV);
                if (*LOCAL_THROW) {
                    if (RESULT) {
                        FREE_VARIABLE(RESULT, PREV);
                    }
                    return NULL;
                }
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
                        CLASS_DATA = (struct CompiledClass *)delegate_Class(RESULT->CLASS_DATA);
                        CC_WRITE_LOCK(PIF)
                        CLASS_DATA->LINKS++;
                        CC_WRITE_UNLOCK(PIF)

                        RESULT = CLASS_DATA->_Class->ExecuteDelegate(PIF,
                                                                    (INTEGER)delegate_Member(RESULT->CLASS_DATA),
                                                                    CLASS_DATA,
                                                                    OE,
                                                                    FORMAL_PARAM,
                                                                    SenderCTX,
                                                                    CLSID,
                                                                    LOCAL_CLSID,
                                                                    LOCAL_THROW,
                                                                    PREV,
                                                                    result_id);

                        CC_WRITE_LOCK(PIF)
                        CLASS_DATA->LINKS--;
                        CC_WRITE_UNLOCK(PIF)

                        if (*LOCAL_THROW) {
                            if (RESULT) {
                                FREE_VARIABLE(RESULT, PREV);
                            }
                            return NULL;
                        }
                        return RESULT;
                    }
                    AnsiException *Exc = new AnsiException(ERR390, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 390, (OE ? OE->OperandRight_PARSE_DATA.c_str() : ""), *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), NAME);
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
                            *PROPERTIES = (VariableDATAPROPERTY *)FAST_MALLOC(PIF, sizeof(VariableDATAPROPERTY) * dataLen);
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
                            *PROPERTIES = (VariableDATAPROPERTY *)FAST_MALLOC(PIF, sizeof(VariableDATAPROPERTY) * dataLen);
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
                        CONCEPT_STRING(IMAGE, RESULT);
                    } else
                    if ((RESULT->TYPE == VARIABLE_CLASS) || (RESULT->TYPE == VARIABLE_ARRAY)) {
                        IMAGE->CLASS_DATA = RESULT->CLASS_DATA;
                    } else
                    if (RESULT->TYPE == VARIABLE_DELEGATE) {
                        IMAGE->CLASS_DATA = copy_Delegate(RESULT->CLASS_DATA);
                    } else {
                        IMAGE->NUMBER_DATA = RESULT->NUMBER_DATA;
                    }
                    //----------------------------------------------
                    if (IMAGE->TYPE == VARIABLE_CLASS) {
                        CC_WRITE_LOCK(PIF)
                        ((struct CompiledClass *)IMAGE->CLASS_DATA)->LINKS++;
                        CC_WRITE_UNLOCK(PIF)
                    } else
                    if (IMAGE->TYPE == VARIABLE_ARRAY) {
                        CC_WRITE_LOCK(PIF)
                        ((struct Array *)IMAGE->CLASS_DATA)->LINKS++;
                        CC_WRITE_UNLOCK(PIF)
                    }
                    //----------------------------------------------
                    RESULT = IMAGE;
                }
                return RESULT;

            default:
                relocation2 = this->RELOCATIONS2 [relocation - 1];
                CC_WRITE_LOCK(PIF)
                RESULT = Owner->_CONTEXT [relocation2 - 1];
                if (!RESULT) {
                    RESULT = CompiledClass_CreateVariable(Owner, relocation2 - 1, pMEMBER_i);
                } else
                if ((RESULT->TYPE == VARIABLE_DELEGATE) && (FORMAL_PARAM) && (!property)) {
                    CLASS_DATA = (struct CompiledClass *)delegate_Class(RESULT->CLASS_DATA);
                    CLASS_DATA->LINKS ++;
                    CC_WRITE_UNLOCK(PIF)

                    RESULT = CLASS_DATA->_Class->ExecuteDelegate(PIF,
                                                                (INTEGER)delegate_Member(RESULT->CLASS_DATA),
                                                                CLASS_DATA,
                                                                OE,
                                                                FORMAL_PARAM,
                                                                SenderCTX,
                                                                CLSID,
                                                                LOCAL_CLSID,
                                                                LOCAL_THROW,
                                                                PREV,
                                                                result_id);
                    CC_WRITE_LOCK(PIF)
                    CLASS_DATA->LINKS --;
                    CC_WRITE_UNLOCK(PIF)
                    if (*LOCAL_THROW) {
                        if (RESULT) {
                            FREE_VARIABLE(RESULT, PREV);
                            RESULT = NULL;
                        }
                    }
                    return RESULT;
                }
                CC_WRITE_UNLOCK(PIF)
                if ((FORMAL_PARAM) && (!property)) {
                    const char *mname = pMEMBER_i->NAME;
                    AnsiException *Exc   = new AnsiException(ERR250, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 250, mname ? mname : (OE ? OE->OperandRight_PARSE_DATA.c_str() : ""), *GetFilename(PIF, CLSID, &_DEBUG_INFO_FILENAME), ((ClassCode *)(pMEMBER_i->Defined_In))->NAME, NAME);
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
        const char *mname = PIF->GeneralMembers->Item(i);
        AnsiException *Exc   = new AnsiException(ERR150, OE ? OE->Operator_DEBUG_INFO_LINE : 0, 150, mname ? mname : (OE ? OE->OperandRight_PARSE_DATA.c_str() : ""), *GetFilename(PIF, CLSID, &_DEBUG_INFO_FILENAME), NAME);
        PIF->AcknoledgeRunTimeError(PREV, Exc);
    }
    return 0;
}

void ClassCode::GenerateCode(StaticList *GeneralMembers) {
    INTEGER GeneralMembersCount = GeneralMembers->Count();
    INTEGER LocalMembersCount   = Members ? Members->Count() : 0;
    pMEMBERS_COUNT   = LocalMembersCount;

    if (!LocalMembersCount)
        return;

#ifdef HASH_RELOCATION
    RELOCATIONS = kh_init(inthashtable);
#else
    RELOCATIONS  = new MemberLink [LocalMembersCount];
    memset(RELOCATIONS, 0, sizeof(MemberLink) * LocalMembersCount);
#endif
    RELOCATIONS2 = new CLASS_MEMBERS_DOMAIN[LocalMembersCount];
    memset(RELOCATIONS2, 0, sizeof(CLASS_MEMBERS_DOMAIN) * LocalMembersCount);

    pMEMBERS = new ClassMember * [LocalMembersCount];

    INTEGER var_index = 0;
    for (INTEGER i = 0; i < LocalMembersCount; i++) {
        ClassMember *LocalMember = (ClassMember *)Members->Item(i);
        pMEMBERS [i] = LocalMember;

        //===================================================//
        if ((LocalMember) && (!LocalMember->IS_FUNCTION))
            RELOCATIONS2 [i] = ++var_index;
        //===================================================//

        for (INTEGER j = 0; j < GeneralMembersCount; j++) {
            char *GeneralMember = GeneralMembers->Item(j);

            if (!strcmp(LocalMember->NAME, GeneralMember)) {
                //RELOCATIONS [j] = i + 1;
                this->SetRelocation(j, i+1);
                break;
            }
        }
    }

    DataMembersCount = var_index;
}

int ClassCode::Serialize(PIFAlizator *PIF, FILE *out, bool is_lib, int version) {
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
    INTEGER jj;
    for (INTEGER ii = 0; ii < members_count; ii++) {
        ClassMember *CM_TARGET = (ClassMember *)Members->Item(ii);
#ifdef CACHED_LIST
        jj = PIF->GeneralMembers->ContainsString(CM_TARGET->NAME);
        if (jj > 0) {
            jj--;
            concept_fwrite_int(&jj, sizeof(jj), 1, out);
            char is_defined_in_this_class = ((void *)CM_TARGET->Defined_In == (void *)this);
            concept_fwrite(&is_defined_in_this_class, sizeof(is_defined_in_this_class), 1, out);
            if (is_defined_in_this_class) {
                CM_TARGET->Serialize(PIF, out, is_lib, version);
            } else {
                int clsid = ((ClassCode *)CM_TARGET->Defined_In)->CLSID;
                concept_fwrite_int(&clsid, sizeof(clsid), 1, out);
            }
        } else
#endif
        for (jj = 0; jj < GeneralMembersCount; jj++) {
            int         reloc = Relocation(jj);
            ClassMember *CM   = reloc ? pMEMBERS [reloc - 1] : 0;
            if ((CM) && (CM == CM_TARGET)) {
                concept_fwrite_int(&jj, sizeof(jj), 1, out);
                char is_defined_in_this_class = ((void *)CM->Defined_In == (void *)this);
                concept_fwrite(&is_defined_in_this_class, sizeof(is_defined_in_this_class), 1, out);
                if (is_defined_in_this_class) {
                    CM->Serialize(PIF, out, is_lib, version);
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

int ClassCode::ComputeSharedSize(concept_FILE *in, int general_members, int version) {
    int size = 0;

    size += sizeof(AnsiList);
    signed char MyNEEDED;
    INTEGER     MyCLSID;

    FREAD_FAIL(&MyNEEDED, sizeof(MyNEEDED), 1, in);
    if (!MyNEEDED) {
        return 0;
    }

    FREAD_INT_FAIL(&MyCLSID, sizeof(MyCLSID), 1, in);

    plainstring_computesharedsize(in, SERIALIZE_16BIT_LENGTH);

    INTEGER members_count;
    FREAD_INT_FAIL(&members_count, sizeof(members_count), 1, in);

    // CONSTRUCTOR + DESTRUCTOR!
    SKIP(sizeof(INTEGER) + sizeof(INTEGER), in);

    //RELOCATIONS
    size += sizeof(MemberLink) * members_count;
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
                size += Optimizer_ComputeSharedSize(in, version);
            }
        } else {
            int clsid;
            FREAD_INT_FAIL(&clsid, sizeof(clsid), 1, in);
        }
    }
    return size;
}

int ClassCode::Unserialize(PIFAlizator *PIF, concept_FILE *in, AnsiList *ClassList, bool is_lib, int *ClassNames, int *Relocation, int version) {
    bool is_created = PIF->is_buffer ? 0 : SHIsCreated();
    bool is_pooled  = PIF->is_buffer ? 0 : SHIsPooled();
    if ((!is_lib) && (Members)) {
        delete Members;
        Members = 0;
    }

    concept_fread_int(&NEEDED, sizeof(NEEDED), 1, in);
    if (!NEEDED)
        return 0;

    if (!concept_fread_int(&CLSID, sizeof(CLSID), 1, in))
        return -1;

    NAME.Unserialize(in, SERIALIZE_16BIT_LENGTH);

    if (is_lib)
        ClassNames [CLSID] = ClassList->Count() - 1;

    if (pMEMBERS)
        delete [] pMEMBERS;

    if (RELOCATIONS) {
#ifdef HASH_RELOCATION
        kh_destroy(inthashtable, RELOCATIONS);
#else
        delete [] RELOCATIONS;
#endif
    }

    if (RELOCATIONS2)
        delete [] RELOCATIONS2;

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
    if (!members_count) {
        RELOCATIONS = NULL;
        RELOCATIONS2 = NULL;
        pMEMBERS_COUNT = 0;
        pMEMBERS = NULL;
        this->DEFINED_LEVEL = PIF->INCLUDE_LEVEL;
        return 0;
    }
    if (!is_lib) {
        INTEGER GeneralMembersCount = PIF->GeneralMembers->Count();
        if (is_pooled) {
#ifdef HASH_RELOCATION
            RELOCATIONS = kh_init(inthashtable);
            RELOCATIONS2 = (CLASS_MEMBERS_DOMAIN *)SHAlloc(sizeof(CLASS_MEMBERS_DOMAIN) * members_count);
            if (is_created)
                memset(RELOCATIONS2, 0, sizeof(CLASS_MEMBERS_DOMAIN) * members_count);
#else
            RELOCATIONS  = (MemberLink *)SHAlloc(sizeof(MemberLink) * members_count);
            RELOCATIONS2 = (CLASS_MEMBERS_DOMAIN *)SHAlloc(sizeof(CLASS_MEMBERS_DOMAIN) * members_count);
            if (is_created) {
                memset(RELOCATIONS2, 0, sizeof(CLASS_MEMBERS_DOMAIN) * members_count);
                memset(RELOCATIONS, 0, sizeof(MemberLink) * members_count);
            }
#endif
        } else {
#ifdef HASH_RELOCATION
            RELOCATIONS = kh_init(inthashtable);
#else
            RELOCATIONS  = new MemberLink [members_count];
            memset(RELOCATIONS, 0, sizeof(MemberLink) * members_count);
#endif
            RELOCATIONS2 = new CLASS_MEMBERS_DOMAIN [members_count];
            memset(RELOCATIONS2, 0, sizeof(CLASS_MEMBERS_DOMAIN) * members_count);
        }

        pMEMBERS       = new ClassMember * [members_count];
        pMEMBERS_COUNT = members_count;
    }

    CLASS_MEMBERS_DOMAIN reloc_index = 0;
    for (INTEGER jj = 0; jj < members_count; jj++) {
        int index;
        if (!concept_fread_int(&index, sizeof(index), 1, in))
            return -1;

        if (is_lib) {
            index = Relocation [index];
        } else
        if ((!is_pooled) || (is_created)) {
            //RELOCATIONS [index] = jj + 1;
            this->SetRelocation(index, jj+1);
        }

        char is_defined_in_this_class;
        if (!concept_fread(&is_defined_in_this_class, sizeof(is_defined_in_this_class), 1, in)) {
            return -1;
        }
        if (is_defined_in_this_class) {
            ClassMember *CM = new ClassMember(this, PIF->GeneralMembers->Item(index), true, false, true);
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
                if ((CM->IS_OPERATOR) && (!PIF->ASG_OVERLOADED) && (CM->NAME) && (!strcmp(CM->NAME, ASG)))
                    PIF->ASG_OVERLOADED = 1;
                CM->OPTIMIZER = new_Optimizer(PIF, &PIF->PIF, &PIF->VariableDescriptors, ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME, this, CM, true);
                Optimizer_Unserialize((struct Optimizer *)CM->OPTIMIZER, PIF, in, PIF->ModuleList, is_lib, ClassNames, Relocation, version);
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

            const char *mem_name = PIF->GeneralMembers->Item(index);
            if (is_lib) {
                CM_inherited = (ClassMember *)CC_parent->Members->Item(CC_parent->HasMember(mem_name) - 1);
            } else {
                int reloc = CC_parent->Relocation(index);

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

int ClassCode::Inherits(INTEGER CLSID) const {
    if (this->CLSID == CLSID)
        return 1;

    ClassCode *target_cc = (ClassCode *)this->first_parent;
    while (target_cc) {
        if (target_cc->CLSID == CLSID)
            return 1;

        target_cc = (ClassCode *)target_cc->first_parent;
    }
    if (first_parent) {
        for (int i = 0; i < this->pMEMBERS_COUNT; i++) {
            ClassMember *CM = this->pMEMBERS [i];
            if (CM) {
                if (((ClassCode *)CM->Defined_In)->CLSID == CLSID)
                    return 1;
            }
        }
    }
    return 0;
}

void ClassCode::UpdateNeeded() {
    if (NEEDED)
        return;

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
#ifdef HASH_RELOCATION
            kh_destroy(inthashtable, RELOCATIONS);
#else
            delete [] RELOCATIONS;
#endif
        }
        if (RELOCATIONS2) {
            delete [] RELOCATIONS2;
        }
    }
}

GreenThreadCycle *ClassCode::CreateThread(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner) const {
    ClassMember *pMEMBER_i = i ? pMEMBERS [i - 1] : 0;

    if (pMEMBER_i) {
#ifndef DISABLE_DEPRECATED_FEATURES
        while (pMEMBER_i->IS_FUNCTION == 2) {
            i = pMEMBER_i->MEMBER_GET - 1;
            int relocation = this->Relocation(i);
            pMEMBER_i = relocation ? pMEMBERS [relocation - 1] : 0;
            if (!pMEMBER_i) {
                return 0;
            }
        }
#endif
        if (pMEMBER_i->IS_FUNCTION == 1) {
            if (pMEMBER_i->MUST_PARAMETERS_COUNT) {
                AnsiException *Exc = new AnsiException(1310, ERR1310, 0, "mandatory parameters: ", pMEMBER_i->MUST_PARAMETERS_COUNT, *GetFilename(PIF, CLSID, &((ClassCode *)(pMEMBER_i->Defined_In))->_DEBUG_INFO_FILENAME), ((ClassCode *)(pMEMBER_i->Defined_In))->NAME, pMEMBER_i->NAME);
                PIF->AcknoledgeRunTimeError(NULL, Exc);
                return 0;
            }
            return pMEMBER_i->CreateThread(PIF, this->CLSID, Owner);
        }
    }
    return 0;
}

void ClassCode::EnsureThreadSafe() {
    INTEGER Count = pMEMBERS_COUNT;

    if (!pMEMBERS)
        return;

    for (INTEGER i = 0; i < Count; i++) {
        ClassMember *CM = pMEMBERS [i];
        if ((CM) && (CM->IS_FUNCTION == 1))
            CM->EnsureThreadSafe();
    }
}
