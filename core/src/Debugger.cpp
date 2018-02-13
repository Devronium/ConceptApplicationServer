#include "Codes.h"
#include "ConceptTypes.h"
#include "CompiledClass.h"
#include "Debugger.h"
#include "ConceptInterpreter.h"
#include <string.h>
#include "ModuleLink.h"

#define MIN(a, b)    (((a) < (b)) ? (a) : (b))

VariableDATA *VarToClean [0xFF];
int          VarCnt = 0;

#define CLEAN_VARS    { for (int i = 0; i < VarCnt; i ++) { FREE_VARIABLE(VarToClean [i]); } VarCnt = 0; }
#define ADD_VAR(VAR)    { if (VAR) VarToClean [VarCnt ++] = VAR; }

VariableDATA *GetClassMember(void *CLASS_PTR, const char *class_member_name) {
    if (CLASS_PTR) {
        const ClassCode *CCode = CompiledClass_GetClass((struct CompiledClass *)CLASS_PTR);
        VariableDATA **CONTEXT = CompiledClass_GetContext((struct CompiledClass *)CLASS_PTR);
        if ((CCode) && (CONTEXT)) {
            INTEGER index = CCode->HasMemberInCompiled(class_member_name);
            if (index) {
                index--;
                ClassMember *CM = index < CCode->pMEMBERS_COUNT ? CCode->pMEMBERS[index] : 0;

                if ((CM) && (CM->IS_FUNCTION)) {
                    //VariableDATA
                    index = CCode->GetAbsoluteMemberID(index + 1) - 1;

                    ParamList PLIST;
                    PLIST.COUNT = 0;

                    PIFAlizator  *PIF   = GET_PIF(((struct CompiledClass *)CLASS_PTR));
                    VariableDATA *Owner = (VariableDATA *)VAR_ALLOC(PIF);
                    Owner->CLASS_DATA         = CLASS_PTR;
                    Owner->IS_PROPERTY_RESULT = 0;
                    Owner->LINKS = 1;
                    Owner->TYPE = VARIABLE_CLASS;
                    ((struct CompiledClass *)CLASS_PTR)->LINKS++;

                    RuntimeOptimizedElement OE;
                    OE.Operator_DEBUG_INFO_LINE = 0;
                    OE.Operator_ID = 0;
                    OE.OperandRight_ID = 0;

                    VariableDATA *THROW_DATA;
                    VariableDATA *VarDATA = CCode->ExecuteMember(PIF,
                                                                    index,
                                                                    Owner,
                                                                    &OE,
                                                                    true,
                                                                    CM->IS_FUNCTION == 3 ? 0 : &PLIST,
                                                                    (VariableDATA **)-1,
                                                                    0,
                                                                    CCode->CLSID,
                                                                    CCode->CLSID,
                                                                    &THROW_DATA,
                                                                    NULL
                                                                    );
                    FREE_VARIABLE(Owner);
                    ADD_VAR(VarDATA);
                    if (THROW_DATA) {
                        FREE_VARIABLE(THROW_DATA);
                    }
                    return VarDATA;
                } else {
                    if (CCode->RELOCATIONS2) {
                        index = CCode->RELOCATIONS2 [index] - 1;
                        if (index >= 0) {
                            if ((CONTEXT) && (!CONTEXT [index]))
                                CompiledClass_CreateVariable((struct CompiledClass *)CLASS_PTR, index, CM);
                            return CONTEXT [index];
                        }
                    }
                }
            }
        }
    }
    return 0;
}

VariableDATA *GetClassMembers(void *CLASS_PTR, char *members_string) {
    const char *ParsedVariableName = members_string;
    void *CLASS_PTR_LOCAL = CLASS_PTR;
    char *pch = strchr((char *)members_string, '.');
    while (pch) {
        pch[0] = 0;
        pch++;
        ParsedVariableName = pch;
        VariableDATA *Temp = GetClassMember(CLASS_PTR_LOCAL, members_string);
        if ((Temp) && (Temp->TYPE == VARIABLE_CLASS)) {
            CLASS_PTR_LOCAL = Temp->CLASS_DATA;
        } else {
            return 0;
        }
        if (!CLASS_PTR_LOCAL) {
            return 0;
        }
        members_string = pch;
        pch = strchr(pch, '.');
    }
    return CLASS_PTR_LOCAL ? GetClassMember(CLASS_PTR_LOCAL, ParsedVariableName) : 0;
}

int GetVariableByName(int operation, void **VDESC, void **CONTEXT, int Depth, char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE) {
    char floatbuf[0xFF];
    if (operation == -1) {
        struct plainstring *cstack = plainstring_new();;
        INTERNAL_LOCK(((PIFAlizator *)PIF))
        int first = 1;
        while (STACK_TRACE) {
            ClassMember *CM = (ClassMember *)((SCStack *)STACK_TRACE)->CM;
            if (CM) {
                if (CM->Defined_In) {
                    plainstring_add(cstack, ((ClassCode *)CM->Defined_In)->NAME.c_str());
                    plainstring_add_char(cstack, '.');
                } else
                    plainstring_add(cstack, "::");
                plainstring_add(cstack, CM->NAME);
            } else {
                plainstring_add(cstack, "STATIC/LIBRARY.STATIC_FUNCTION");
            }
            plainstring_add(cstack, ":");
            if (first) {
                plainstring_add_int(cstack, CM->_DEBUG_STARTLINE);
                first   = 0;
            } else
                plainstring_add_int(cstack, ((SCStack *)STACK_TRACE)->line);
            plainstring_add_char(cstack, '\n');
            STACK_TRACE = (SCStack *)((SCStack *)STACK_TRACE)->PREV;
        }
        INTERNAL_UNLOCK(((PIFAlizator *)PIF))
        buffer [buf_size - 1] = 0;
        int len  = plainstring_len(cstack);
        if (len > buf_size)
            len = buf_size;
        strncpy(buffer, plainstring_c_str(cstack), buf_size - 1);
        buffer [len] = 0;
        return -1;
    }

    if (!CONTEXT) {
        return 0;
    }
    CLEAN_VARS;

    
    char *temp_varname = strdup(VariableName);
    char *ParsedVariableName = temp_varname;
    char *pch = strchr(temp_varname, '.');
    if (pch) {
        pch [0] = 0;
        pch ++;
        ParsedVariableName = pch;
    }
    INTEGER i = ((PIFAlizator *)PIF)->FindVariableByName(VDESC, temp_varname);
    if ((i >= 0) && (i < Depth)) {
        if (((VariableDATA *)CONTEXT [i])->TYPE == VARIABLE_STRING) {
            if (operation == 0) {
                strncpy(buffer, CONCEPT_C_STRING(((VariableDATA *)CONTEXT [i])), buf_size);
            } else
            if (operation == 1) {
                CONCEPT_STRING_SET_CSTR(((VariableDATA *)CONTEXT [i]), buffer);
            }

            CLEAN_VARS;
            free(temp_varname);
            return pch ? 0 : VARIABLE_STRING;
        } else
        if (((VariableDATA *)CONTEXT [i])->TYPE == VARIABLE_NUMBER) {
            if (operation == 0) {
                cstr_loaddouble(floatbuf, ((VariableDATA *)CONTEXT [i])->NUMBER_DATA);
                strncpy(buffer, floatbuf, buf_size);
            } else
            if (operation == 1) {
                ((VariableDATA *)CONTEXT [i])->NUMBER_DATA = atof(buffer);
            }

            CLEAN_VARS;
            free(temp_varname);
            return pch ? 0 : VARIABLE_NUMBER;
        } else
        if (((VariableDATA *)CONTEXT [i])->TYPE == VARIABLE_CLASS) {
            if (operation == 0) {
                strncpy(buffer, CompiledClass_GetClassName((struct CompiledClass *)((VariableDATA *)CONTEXT [i])->CLASS_DATA), buf_size);
            }
            if (!pch) {
                CLEAN_VARS;
                free(temp_varname);
                return VARIABLE_CLASS;
            } else {
                VariableDATA *VD = GetClassMembers(((VariableDATA *)CONTEXT [i])->CLASS_DATA, ParsedVariableName);
                if (!VD) {
                    CLEAN_VARS;
                    free(temp_varname);
                    return 0;
                }
                switch (VD->TYPE) {
                    case VARIABLE_STRING:
                        if (operation == 0) {
                            strncpy(buffer, CONCEPT_C_STRING(VD), buf_size);
                        } else
                        if (operation == 1) {
                            CONCEPT_STRING_SET_CSTR(VD, buffer);
                            if (VD->IS_PROPERTY_RESULT) {
                                SetClassMember(((VariableDATA *)CONTEXT [i])->CLASS_DATA, ParsedVariableName, VARIABLE_STRING, buffer, 0);
                            }
                        }
                        CLEAN_VARS;
                        free(temp_varname);
                        return VD->TYPE;

                    case VARIABLE_NUMBER:
                        if (operation == 0) {
                            cstr_loaddouble(floatbuf, VD->NUMBER_DATA);
                            strncpy(buffer, floatbuf, buf_size);
                        } else
                        if (operation == 1) {
                            VD->NUMBER_DATA = atof(buffer);
                            if (VD->IS_PROPERTY_RESULT) {
                                SetClassMember(((VariableDATA *)CONTEXT [i])->CLASS_DATA, ParsedVariableName, VARIABLE_NUMBER, "", VD->NUMBER_DATA);
                            }
                        }
                        CLEAN_VARS;
                        free(temp_varname);
                        return VD->TYPE;

                    case VARIABLE_CLASS:
                        if (operation == 0) {
                            strncpy(buffer, CompiledClass_GetClassName((struct CompiledClass *)VD->CLASS_DATA), buf_size);
                        }
                        CLEAN_VARS;
                        free(temp_varname);
                        return VD->TYPE;

                    case VARIABLE_ARRAY:
                        if (operation == 0) {
                            struct plainstring *temp = Array_ToString((struct Array *)VD->CLASS_DATA);
                            if (temp) {
                                strncpy(buffer, plainstring_c_str(temp), buf_size);
                                plainstring_delete(temp);
                            }
                        }
                        CLEAN_VARS;
                        free(temp_varname);
                        return VD->TYPE;

                    case VARIABLE_DELEGATE:
                        if (operation == 0) {
                            struct plainstring *str = plainstring_new_str(CompiledClass_GetClassName((struct CompiledClass *)delegate_Class(VD->CLASS_DATA)));
                            plainstring_add_char(str, '.');
                            plainstring_add(str, ((struct CompiledClass *)delegate_Class(VD->CLASS_DATA))->_Class->pMEMBERS [(INTEGER)delegate_Member(VD->CLASS_DATA)]->NAME);
                            strncpy(buffer, plainstring_c_str(str), buf_size);
                            plainstring_delete(str);
                        }
                        CLEAN_VARS;
                        free(temp_varname);
                        return VD->TYPE;

                    default:
                        CLEAN_VARS;
                        free(temp_varname);
                        return 0;
                }
            }
        }
        CLEAN_VARS;
        free(temp_varname);
        return 0;
    }
    CLEAN_VARS;
    free(temp_varname);
    return 0;
}

