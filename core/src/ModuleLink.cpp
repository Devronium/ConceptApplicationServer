#define CACHE_MEMBERS

#include "ModuleLink.h"
#include "AnsiException.h"
#include "Array.h"
#include "ConceptInterpreter.h"
#include "library.h"
#include <string.h>
#include "Codes.h"
#include "PIFAlizator.h"
#include "BuiltIns.h"

#ifdef _WIN32
 #include <windows.h>       // LoadLibrary
 #include <io.h>
#else
 #include <unistd.h>
 #include <stdarg.h>
 #include <dlfcn.h>         // dlopen
 #include <poll.h>

int eof(int fd) {
    struct pollfd ufds[1];

    ufds[0].fd     = fd;
    ufds[0].events = POLLIN;

    int res = poll(ufds, 1, 0);

    return !res;
}

ssize_t sock_fd_read(int sock, int *fd) {
    ssize_t size;
    char    buf[16];

    if (fd) {
        *fd = -1;
        struct msghdr msg;
        struct iovec  iov;
        union {
            struct cmsghdr cmsghdr;
            char           control[CMSG_SPACE(sizeof(int))];
        } cmsgu;
        struct cmsghdr *cmsg;

        iov.iov_base = buf;
        iov.iov_len  = sizeof(buf);

        msg.msg_name       = NULL;
        msg.msg_namelen    = 0;
        msg.msg_iov        = &iov;
        msg.msg_iovlen     = 1;
        msg.msg_control    = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        size = recvmsg(sock, &msg, 0);
        if (size < 0)
            return size;

        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && (cmsg->cmsg_len == CMSG_LEN(sizeof(int)))) {
            if (cmsg->cmsg_level != SOL_SOCKET)
                return -2;

            if (cmsg->cmsg_type != SCM_RIGHTS)
                return -3;

            *fd = *((int *)CMSG_DATA(cmsg));
        } else
            *fd = -1;
    } else {
        size = read(sock, buf, sizeof(buf));
        if (size < 0)
            return size;
    }
    return size;
}
#endif

#ifdef CACHE_MEMBERS
    #include "HashTable.h"
static HashTable FunctionMap;
#endif

INTEGER ImportModule(AnsiString& MODULE_MASK, AnsiList *Errors, INTEGER line, AnsiString FILENAME, AnsiList *TARGET, PIFAlizator *Sender, INTEGER _NO_ERROR_REPORT) {
#ifdef _WIN32
    // windows module
    HANDLE hLIBRARY = LoadLibrary(MODULE_MASK + MODULE_EXTENSION);
#else
    // linux module
    HANDLE hLIBRARY = dlopen(MODULE_MASK + MODULE_EXTENSION, RTLD_LAZY);
#endif
    if (!hLIBRARY) {
        if (!_NO_ERROR_REPORT)
#ifdef _WIN32
        {
            Errors->Add(new AnsiException(ERR670, line, 670, MODULE_MASK, FILENAME), DATA_EXCEPTION);
        }

#else
        {
            Errors->Add(new AnsiException(AnsiString(ERR670) + AnsiString(": ") + (char *)dlerror(), line, 670, MODULE_MASK, FILENAME), DATA_EXCEPTION);
        }
#endif

        else {
            return 0;
        }
    } else {
        TARGET->Add(hLIBRARY, DATA_HANDLER);
        // --------------- Module Management functions -----------------------------------------//
        AnsiString PREFIX = "_";
#ifdef _WIN32
        CONCEPT_MANAGEMENT_CALL _PROC_ADR = (CONCEPT_MANAGEMENT_CALL)GetProcAddress((HMODULE)hLIBRARY, ON_CREATE);
#else
        CONCEPT_MANAGEMENT_CALL _PROC_ADR = (CONCEPT_MANAGEMENT_CALL)dlsym((HMODULE)hLIBRARY, ON_CREATE);
#endif

        if (!_PROC_ADR) {
#ifdef _WIN32
            _PROC_ADR = (CONCEPT_MANAGEMENT_CALL)GetProcAddress((HMODULE)hLIBRARY, PREFIX + ON_CREATE);
#else
            _PROC_ADR = (CONCEPT_MANAGEMENT_CALL)dlsym((HMODULE)hLIBRARY, PREFIX + ON_CREATE);
#endif
        }
        if (_PROC_ADR) {
            char *LOAD_RESULT = (char *)_PROC_ADR((SYS_INT)hLIBRARY, Sender, Invoke);
            if (LOAD_RESULT) {
                Errors->Add(new AnsiException(ERR880, line, 880, LOAD_RESULT, FILENAME), DATA_EXCEPTION);
            }
        }
        // -------------------------------------------------------------------------------------//
    }
    return 1;
}

INTEGER UnImportModule(HMODULE hMODULE, PIFAlizator *Sender) {
    // --------------- Module Management functions -----------------------------------------//
    AnsiString PREFIX = "_";

#ifdef _WIN32
    CONCEPT_MANAGEMENT_CALL _PROC_ADR = (CONCEPT_MANAGEMENT_CALL)GetProcAddress((HMODULE)hMODULE, ON_DESTROY);
#else
    CONCEPT_MANAGEMENT_CALL _PROC_ADR = (CONCEPT_MANAGEMENT_CALL)dlsym((HMODULE)hMODULE, ON_DESTROY);
#endif

    PREFIX += ON_DESTROY;
    if (!_PROC_ADR) {
#ifdef _WIN32
        _PROC_ADR = (CONCEPT_MANAGEMENT_CALL)GetProcAddress((HMODULE)hMODULE, PREFIX.c_str() );
#else
        _PROC_ADR = (CONCEPT_MANAGEMENT_CALL)dlsym((HMODULE)hMODULE, PREFIX.c_str() );
#endif
    }

    char *LOAD_RESULT = 0;
    if (_PROC_ADR) {
        LOAD_RESULT = (char *)_PROC_ADR((SYS_INT)hMODULE, Sender, Invoke);
    }
    // -------------------------------------------------------------------------------------//

#ifdef _WIN32
    // windows module
    if (Sender)
        return 0;
    else {
        return 0;
    }
#else
    // linux module
    if ((Sender) || (LOAD_RESULT))
        return 0;
    else
        return dlclose((HMODULE)hMODULE);
#endif
    return 0;
}

INTEGER SetVariable(VariableDATA *VD, INTEGER TYPE, char *STRING_VALUE, NUMBER NUMBER_VALUE) {
    if (!VD)
        return -1;
    // is constant ?
    if (VD->IS_PROPERTY_RESULT == -1)
        return -2;
    //---------------//
    CLASS_CHECK(VD);
    VD->CLASS_DATA = NULL;
    //---------------//
    char copy_buffer = 0;
    if (TYPE == _LINK__STRING_DATA) {
        TYPE        = VARIABLE_STRING;
        copy_buffer = 1;
        if (!(INTEGER)NUMBER_VALUE) {
            STRING_VALUE = 0;
        }
    }
    VD->TYPE = TYPE;
    if (VD->TYPE == VARIABLE_STRING) {
        if (!NUMBER_VALUE) {
            CONCEPT_STRING(VD) = STRING_VALUE;
        } else {
            if (copy_buffer) {
                CONCEPT_STRING(VD).LinkBuffer(STRING_VALUE, (INTEGER)NUMBER_VALUE);
            } else {
                CONCEPT_STRING(VD).LoadBuffer(STRING_VALUE, (INTEGER)NUMBER_VALUE);
            }
        }
    } else
    if ((VD->TYPE == VARIABLE_CLASS) || (VD->TYPE == VARIABLE_ARRAY) || (VD->TYPE == VARIABLE_DELEGATE)) {
        VD->CLASS_DATA = (void *)STRING_VALUE;
        if ((TYPE == VARIABLE_CLASS) || (TYPE == VARIABLE_DELEGATE)) {
            ((CompiledClass *)VD->CLASS_DATA)->LINKS++;
            if (TYPE == VARIABLE_DELEGATE)
                VD->DELEGATE_DATA = (int)NUMBER_VALUE;
            ((CompiledClass *)VD->CLASS_DATA)->reachable = 0x1C;
        } else {
            ((Array *)VD->CLASS_DATA)->LINKS++;
            ((Array *)VD->CLASS_DATA)->reachable = 0x1C;
        }
    } else {
        VD->NUMBER_DATA = NUMBER_VALUE;
    }
    return 0;
}

INTEGER GetVariable(VariableDATA *VD, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE) {
    if (!VD)
        return -1;
    *TYPE         = VD->TYPE;
    *STRING_VALUE = 0;

    if (VD->TYPE == VARIABLE_STRING) {
        *STRING_VALUE = (char *)CONCEPT_C_STRING(VD);
        *NUMBER_VALUE = CONCEPT_C_LENGTH(VD);
    } else
    if ((VD->TYPE == VARIABLE_CLASS) || (VD->TYPE == VARIABLE_ARRAY)) {
        *STRING_VALUE = (char *)VD->CLASS_DATA;
        *NUMBER_VALUE = 0;
    } else
    if (VD->TYPE == VARIABLE_DELEGATE) {
        *STRING_VALUE = (char *)VD->CLASS_DATA;
        *NUMBER_VALUE = VD->DELEGATE_DATA;
    } else {
        *NUMBER_VALUE = VD->NUMBER_DATA;
    }
    return 0;
}

INTEGER GetClassMember(void *CLASS_PTR, const char *class_member_name, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE) {
    if (CLASS_PTR) {
        ClassCode    *CCode    = ((CompiledClass *)CLASS_PTR)->GetClass();
        VariableDATA **CONTEXT = ((CompiledClass *)CLASS_PTR)->GetContext();
        if (CCode) {
            INTEGER m_type = 0;
            INTEGER index  = CCode->HasMemberInCompiled(class_member_name, &m_type);
            if (index) {
                index--;

                ClassMember *CM = index < CCode->pMEMBERS_COUNT ? CCode->pMEMBERS[index] : 0;

                if ((CM) && (CM->IS_FUNCTION)) {
                    index = CCode->GetAbsoluteMemberID(index + 1) - 1;
                    ParamList PLIST;
                    PLIST.COUNT = 0;

                    RuntimeOptimizedElement OE;
                    OE.Operator_DEBUG_INFO_LINE = 0;
                    OE.Operator_ID = 0;
                    OE.OperandRight_ID = 0;

                    PIFAlizator  *PIF   = GET_PIF(((CompiledClass *)CLASS_PTR));
                    VariableDATA *Owner = (VariableDATA *)VAR_ALLOC(PIF);
                    Owner->CLASS_DATA         = CLASS_PTR;
                    Owner->IS_PROPERTY_RESULT = 0;
                    Owner->LINKS = 1;
                    Owner->TYPE  = VARIABLE_CLASS;
                    ((CompiledClass *)CLASS_PTR)->LINKS++;
                    SCStack *STACK_TRACE = NULL;
#ifndef SIMPLE_MULTI_THREADING
                    if (PIF) {
                        GCRoot *root = PIF->GCROOT;
                        if (root) {
                            STACK_TRACE = (SCStack *)root->STACK_TRACE;
                            if (STACK_TRACE)
                                STACK_TRACE = (SCStack *)STACK_TRACE->TOP;
                        }
                    }
#endif
                    try {
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
                                                                     STACK_TRACE
                                                                     );
                        int result = -1;
                        if (VarDATA) {
                            if (!VarDATA->LINKS)
                                VarDATA->LINKS = 1;
                            result = GetVariable(VarDATA, TYPE, STRING_VALUE, NUMBER_VALUE);
                            FREE_VARIABLE(VarDATA);
                        }
                        FREE_VARIABLE(Owner);
                        return result;
                    } catch (VariableDATA *THROW_DATA) {
                        FREE_VARIABLE(THROW_DATA);
                        FREE_VARIABLE(Owner);
                        return -1;
                    }
                }
                index = CCode->RELOCATIONS2 [index] - 1;
                if (index >= 0) {
                    if ((CONTEXT) && (!m_type) && (!CONTEXT [index]) && (CM))
                        ((CompiledClass *)CLASS_PTR)->CreateVariable(index, CM);

                    if (CONTEXT [index]) {
                        return GetVariable(CONTEXT [index], TYPE, STRING_VALUE, NUMBER_VALUE);
                    }
                } else {
                    return -1;
                }
            }
        }
    }
    return -1;
}

INTEGER GetClassMemberVariable(void *CLASS_PTR, const char *class_member_name, void **ptr) {
    if (CLASS_PTR) {
        ClassCode    *CCode    = ((CompiledClass *)CLASS_PTR)->GetClass();
        VariableDATA **CONTEXT = ((CompiledClass *)CLASS_PTR)->GetContext();
        if ((CCode) && (CONTEXT)) {
            INTEGER m_type = 0;
            INTEGER index  = CCode->HasMemberInCompiled(class_member_name, &m_type);
            if (index) {
                index--;
                ClassMember *CM = index < CCode->pMEMBERS_COUNT ? CCode->pMEMBERS[index] : 0;
                index = CCode->RELOCATIONS2 [index] - 1;
                if ((CONTEXT) && (!m_type) && (!CONTEXT [index]) && (CM))
                    ((CompiledClass *)CLASS_PTR)->CreateVariable(index, CM);

                if (CONTEXT [index]) {
                    *ptr = CONTEXT [index];
                }
            }
            return 0;
        }
    }
    return -1;
}

INTEGER SetClassMember(void *CLASS_PTR, const char *class_member_name, INTEGER TYPE, const char *STRING_VALUE, NUMBER NUMBER_VALUE) {
    if (CLASS_PTR) {
        ClassCode    *CCode    = ((CompiledClass *)CLASS_PTR)->GetClass();
        VariableDATA **CONTEXT = ((CompiledClass *)CLASS_PTR)->GetContext();
        if (CCode) {
            INTEGER     m_type = 0;
            INTEGER     index  = CCode->HasMemberInCompiled(class_member_name, &m_type);
            ClassMember *CM    = 0;
            if (index) {
                index--;
                CM = index < CCode->pMEMBERS_COUNT ? CCode->pMEMBERS[index] : 0;

                if ((CM) && (CM->IS_FUNCTION == 3) && (CM->MEMBER_SET)) {
                    index = CCode->GetAbsoluteMemberID(index + 1) - 1;

                    PIFAlizator  *PIF       = GET_PIF(((CompiledClass *)CLASS_PTR));
                    VariableDATA *Parameter = (VariableDATA *)VAR_ALLOC(PIF);
                    Parameter->IS_PROPERTY_RESULT = 0;
                    Parameter->LINKS = 1;
                    Parameter->TYPE  = TYPE;

                    VariableDATA *CONTAINER [1];
                    CONTAINER [0] = Parameter;

                    if (TYPE == VARIABLE_CLASS) {
                        Parameter->CLASS_DATA = (void *)STRING_VALUE;
                        ((CompiledClass *)CLASS_PTR)->LINKS++;
                    } else
                    if (TYPE == VARIABLE_DELEGATE) {
                        Parameter->CLASS_DATA = (void *)STRING_VALUE;
                        ((CompiledClass *)CLASS_PTR)->LINKS++;
                        Parameter->DELEGATE_DATA = (int)NUMBER_VALUE;
                    } else
                    if (TYPE == VARIABLE_ARRAY) {
                        Parameter->CLASS_DATA = (void *)STRING_VALUE;
                        ((Array *)CLASS_PTR)->LINKS++;
                    } else
                    if (TYPE == VARIABLE_STRING) {
                        Parameter->CLASS_DATA     = 0;
                        CONCEPT_STRING(Parameter) = STRING_VALUE;
                    } else
                        Parameter->NUMBER_DATA = NUMBER_VALUE;

                    VariableDATA *Owner = (VariableDATA *)VAR_ALLOC(PIF);
                    Owner->CLASS_DATA         = CLASS_PTR;
                    Owner->IS_PROPERTY_RESULT = 0;
                    Owner->LINKS = 1;
                    Owner->TYPE  = VARIABLE_CLASS;
                    ((CompiledClass *)CLASS_PTR)->LINKS++;

                    RuntimeOptimizedElement OE;
                    OE.Operator_DEBUG_INFO_LINE = 0;
                    OE.Operator_ID = 0;
                    OE.OperandRight_ID = 0;

                    try {
                        CCode->SetProperty(PIF,
                                           index,
                                           Owner,
                                           &OE,
                                           true,
                                           1,
                                           CONTAINER,
                                           CCode->CLSID,
                                           CCode->CLSID,
                                           NULL);

                        FREE_VARIABLE(Owner);
                        FREE_VARIABLE(Parameter);
                        return 0;
                    } catch (VariableDATA *THROW_DATA) {
                        FREE_VARIABLE(THROW_DATA);
                        FREE_VARIABLE(Owner);
                        FREE_VARIABLE(Parameter);
                        return -1;
                    }
                }
            }

            index = CCode->RELOCATIONS2 [index] - 1;
            if (index >= 0) {
                if ((CONTEXT) && (!m_type) && (!CONTEXT [index]) && (CM))
                    ((CompiledClass *)CLASS_PTR)->CreateVariable(index, CM);

                if (CONTEXT [index]) {
                    return SetVariable(CONTEXT [index], TYPE, (char *)STRING_VALUE, NUMBER_VALUE);
                }
            } else {
                return -1;
            }
        }
    }
    return -1;
}

INTEGER Invoke(INTEGER INVOKE_TYPE, ...) {
    va_list ap;

    va_start(ap, INVOKE_TYPE);
    INTEGER result = INVOKE_SUCCESS;
    switch (INVOKE_TYPE) {
        case INVOKE_NEW_BUFFER:
            {
                INTEGER size  = va_arg(ap, INTEGER);
                char    **str = va_arg(ap, char **);
                *str = 0;
                if (size) {
                    *str = (char *)malloc(size);
                    if (!(*str)) {
                        result = CANNOT_INVOKE_IN_THIS_CASE;
                    }
                }
            }
            break;

        case INVOKE_DELETE_BUFFER:
            {
                char *str = va_arg(ap, char *);
                if (str) {
                    free(str);
                } else {
                    result = CANNOT_INVOKE_IN_THIS_CASE;
                }
            }
            break;

        case INVOKE_SET_VARIABLE:
            {
                VariableDATA *vdata = va_arg(ap, VariableDATA *);
                INTEGER      type   = va_arg(ap, INTEGER);
                char         *str   = va_arg(ap, char *);
                NUMBER       nvalue = va_arg(ap, NUMBER);
                result = SetVariable(vdata, type, str, nvalue);
            }
            break;

        case INVOKE_GET_VARIABLE:
            {
                VariableDATA *vdata  = va_arg(ap, VariableDATA *);
                INTEGER      *type   = va_arg(ap, INTEGER *);
                char         **str   = va_arg(ap, char **);
                NUMBER       *nvalue = va_arg(ap, NUMBER *);
                result = GetVariable(vdata, type, str, nvalue);
            }
            break;

        case INVOKE_SET_CLASS_MEMBER:
            {
                void    *handle  = va_arg(ap, void *);
                char    *mname   = va_arg(ap, char *);
                INTEGER type     = va_arg(ap, INTEGER);
                char    *szvalue = va_arg(ap, char *);
                NUMBER  nvalue   = va_arg(ap, NUMBER);
                result = SetClassMember(handle, mname, type, szvalue, nvalue);
            }
            break;

        case INVOKE_GET_CLASS_MEMBER:
            {
                void    *handle   = va_arg(ap, void *);
                char    *mname    = va_arg(ap, char *);
                INTEGER *type     = va_arg(ap, INTEGER *);
                char    **szvalue = va_arg(ap, char **);
                NUMBER  *nvalue   = va_arg(ap, NUMBER *);
                result = GetClassMember(handle, mname, type, szvalue, nvalue);
                break;
            }
            break;

        case INVOKE_GET_CLASS_VARIABLE:
            {
                VariableDATA *handle = va_arg(ap, VariableDATA *);
                if ((handle) && (handle->TYPE == VARIABLE_CLASS)) {
                    char *mname = va_arg(ap, char *);
                    void **ptr  = va_arg(ap, void **);
                    result = GetClassMemberVariable(handle->CLASS_DATA, mname, ptr);
                } else
                    result = INVALID_INVOKE_PARAMETER;
                break;
            }
            break;

        case INVOKE_LOCK_VARIABLE:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (target) {
                    target->LINKS++;
                    // skipped by GC
                    if ((target->TYPE == VARIABLE_CLASS) || (target->TYPE == VARIABLE_DELEGATE))
                        ((CompiledClass *)target->CLASS_DATA)->reachable = 0x1C;
                    else
                    if (target->TYPE == VARIABLE_ARRAY)
                        ((Array *)target->CLASS_DATA)->reachable = 0x1C;
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_DYNAMIC_LOCK:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (target) {
                    if ((target->TYPE == VARIABLE_CLASS) || (target->TYPE == VARIABLE_DELEGATE)) {
                        ((CompiledClass *)target->CLASS_DATA)->LINKS++;
                        ((CompiledClass *)target->CLASS_DATA)->reachable = 0x1C;
                    } else
                    if (target->TYPE == VARIABLE_ARRAY) {
                        ((Array *)target->CLASS_DATA)->LINKS++;
                        ((Array *)target->CLASS_DATA)->reachable = 0x1C;
                    } else {
                        result = INVALID_INVOKE_PARAMETER;
                    }
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

#ifdef ALLOW_UNSAFE_APIS
        case INVOKE_DYNAMIC_UNLOCK:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (target) {
                    if ((target->TYPE == VARIABLE_CLASS) || (target->TYPE == VARIABLE_DELEGATE)) {
                        ((CompiledClass *)target->CLASS_DATA)->LINKS--;
                    } else
                    if (target->TYPE == VARIABLE_ARRAY) {
                        ((Array *)target->CLASS_DATA)->LINKS--;
                    } else {
                        result = INVALID_INVOKE_PARAMETER;
                    }
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;
#endif

        case INVOKE_CREATE_VARIABLE:
            {
                VariableDATA **target = va_arg(ap, VariableDATA * *);
                *target = (VariableDATA *)VAR_ALLOC(0);
                (*target)->IS_PROPERTY_RESULT = 0;
                (*target)->LINKS       = 1;
                (*target)->NUMBER_DATA = 0;
                (*target)->TYPE        = VARIABLE_NUMBER;
            }
            break;

        case INVOKE_FREE_VARIABLE:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (target) {
                    VariableDATA *VARIABLE = target;
                    VARIABLE->LINKS--;
                    if (VARIABLE->LINKS < 1) {
                        if (VARIABLE->CLASS_DATA) {
                            if (VARIABLE->TYPE == VARIABLE_STRING) {
                                delete (AnsiString *)VARIABLE->CLASS_DATA;
                            } else
                            if ((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) {
                                if (!--((CompiledClass *)VARIABLE->CLASS_DATA)->LINKS)
                                    delete (CompiledClass *)VARIABLE->CLASS_DATA;
                            } else
                            if (VARIABLE->TYPE == VARIABLE_ARRAY) {
                                if (!--((Array *)VARIABLE->CLASS_DATA)->LINKS)
                                    delete (Array *)VARIABLE->CLASS_DATA;
                            }
                        }
                        VAR_FREE(VARIABLE);
                    }
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_FREE_VARIABLE_REFERENCE:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (target) {
                    VAR_FREE(target);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_CREATE_ARRAY:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (target) {
                    CLASS_CHECK(target);
                    target->TYPE = VARIABLE_NUMBER;
                    PIFAlizator *PIF = (PIFAlizator *)POOL_CONTEXT(target);
                    target->CLASS_DATA = new(AllocArray(PIF))Array(PIF);
                    target->TYPE = VARIABLE_ARRAY;
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_CREATE_OBJECT:
            {
                PIFAlizator  *pif        = va_arg(ap, PIFAlizator *);
                VariableDATA *target     = va_arg(ap, VariableDATA *);
                char         *class_name = va_arg(ap, char *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }

                CLASS_CHECK(target);
                target->TYPE = VARIABLE_NUMBER;

                int       class_count = pif->ClassList->Count();
                int       clsid       = -1;
                ClassCode *CC         = 0;
                ClassCode **StaticClassList = ((pif->parentPIF) && ((PIFAlizator *)pif->parentPIF)->StaticClassList) ? ((PIFAlizator *)pif->parentPIF)->StaticClassList : pif->StaticClassList;
                for (int i = class_count - 1; i >= 0; i--) {
                    CC = StaticClassList[i];
                    if (CC->NAME == class_name) {
                        clsid = i;
                        break;
                    }
                }
                if (clsid == -1) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }

                ParamList FORMAL_PARAM;
                FORMAL_PARAM.COUNT       = 0;
                FORMAL_PARAM.PARAM_INDEX = 0;

                VariableDATA dummyVD [1];
                SCStack *STACK_TRACE = NULL;
#ifndef SIMPLE_MULTI_THREADING
                if (pif) {
                    GCRoot *root = pif->GCROOT;
                    if (root) {
                        STACK_TRACE = (SCStack *)root->STACK_TRACE;
                        if (STACK_TRACE)
                            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;
                    }
                }
#endif
                target->TYPE       = VARIABLE_CLASS;
                target->CLASS_DATA = NULL;
                target->CLASS_DATA = CC->CreateInstance(pif, target, 0, &FORMAL_PARAM, (VariableDATA **)&dummyVD, STACK_TRACE);
                if (!target->CLASS_DATA) {
                    target->TYPE = VARIABLE_NUMBER;
                    result       = INVALID_INVOKE_PARAMETER;
                    break;
                }
            }
            break;

        case INVOKE_CREATE_OBJECT_NOCONSTRUCTOR:
            {
                PIFAlizator  *pif        = va_arg(ap, PIFAlizator *);
                VariableDATA *target     = va_arg(ap, VariableDATA *);
                char         *class_name = va_arg(ap, char *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }

                CLASS_CHECK(target);
                target->TYPE = VARIABLE_NUMBER;

                int       class_count = pif->ClassList->Count();
                int       clsid       = -1;
                ClassCode *CC         = 0;
                ClassCode **StaticClassList = ((pif->parentPIF) && ((PIFAlizator *)pif->parentPIF)->StaticClassList) ? ((PIFAlizator *)pif->parentPIF)->StaticClassList : pif->StaticClassList;
                for (int i = class_count - 1; i >= 0; i--) {
                    CC = StaticClassList[i];
                    if (CC->NAME == class_name) {
                        clsid = i;
                        break;
                    }
                }
                if (clsid == -1) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                target->TYPE       = VARIABLE_CLASS;
                target->CLASS_DATA = NULL;
                target->CLASS_DATA = new(AllocClassObject(pif))CompiledClass(CC);
                if (!target->CLASS_DATA) {
                    target->TYPE        = VARIABLE_NUMBER;
                    target->NUMBER_DATA = 0;
                }
            }
            break;

        case INVOKE_ARRAY_VARIABLE:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                INTEGER      index   = va_arg(ap, INTEGER);
                VariableDATA **res   = va_arg(ap, VariableDATA * *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    // TO DO !
                    *res = ((Array *)target->CLASS_DATA)->ModuleGet(index);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_ARRAY_VARIABLE_BY_KEY:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                char         *key    = va_arg(ap, char *);
                VariableDATA **res   = va_arg(ap, VariableDATA * *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    *res = ((Array *)target->CLASS_DATA)->ModuleGet(key);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_GET_ARRAY_ELEMENT:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                INTEGER      index   = va_arg(ap, INTEGER);
                void         *res    = 0;
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    // TO DO !
                    res = ((Array *)target->CLASS_DATA)->ModuleGet(index);
                    INTEGER *type = va_arg(ap, INTEGER *);
                    char    **str = va_arg(ap, char **);
                    NUMBER  *nr   = va_arg(ap, double *);
                    GetVariable((VariableDATA *)res, type, str, nr);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_GET_ARRAY_ELEMENT_BY_KEY:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                char         *key    = va_arg(ap, char *);
                void         *res    = 0;
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    res = ((Array *)target->CLASS_DATA)->ModuleGet(key);
                    INTEGER *type = va_arg(ap, INTEGER *);
                    char    **str = va_arg(ap, char **);
                    NUMBER  *nr   = va_arg(ap, double *);
                    GetVariable((VariableDATA *)res, type, str, nr);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_SET_ARRAY_ELEMENT:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                INTEGER      index   = va_arg(ap, INTEGER);
                void         *res    = 0;
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    res = ((Array *)target->CLASS_DATA)->ModuleGet(index);

                    INTEGER type = va_arg(ap, INTEGER);
                    char    *str = va_arg(ap, char *);
                    NUMBER  nr   = va_arg(ap, double);
                    SetVariable((VariableDATA *)res, type, str, nr);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_SET_ARRAY_ELEMENT_BY_KEY:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                char         *key    = va_arg(ap, char *);
                void         *res    = 0;
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    res = ((Array *)target->CLASS_DATA)->ModuleGet(key);

                    INTEGER type = va_arg(ap, INTEGER);
                    char    *str = va_arg(ap, char *);
                    NUMBER  nr   = va_arg(ap, double);
                    SetVariable((VariableDATA *)res, type, str, nr);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_GET_ARRAY_KEY:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                INTEGER      index   = va_arg(ap, INTEGER);
                char         **key   = va_arg(ap, char **);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    *key = ((Array *)target->CLASS_DATA)->GetKey(index);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_ARRAY_ELEMENT_IS_SET:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                INTEGER      index   = va_arg(ap, INTEGER);
                char         *key    = va_arg(ap, char *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    if (index >= 0) {
                        result = ((Array *)target->CLASS_DATA)->FindIndex(index) != -1;
                    } else {
                        result = ((Array *)target->CLASS_DATA)->FindKey(key) != -1;
                    }
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_GET_ARRAY_INDEX:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                char         *key    = va_arg(ap, char *);
                INTEGER      *index  = va_arg(ap, INTEGER *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    *index = ((Array *)target->CLASS_DATA)->GetIndex(key);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_GET_ARRAY_COUNT:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    result = ((Array *)target->CLASS_DATA)->Count();
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_ARRAY_KEYS:
            {
                VariableDATA *target   = va_arg(ap, VariableDATA *);
                char         **keys    = va_arg(ap, char **);
                INTEGER      keys_size = va_arg(ap, INTEGER);
                if ((!target) || (!keys) || (keys_size <= 0)) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_ARRAY) && (target->CLASS_DATA)) {
                    ((Array *)target->CLASS_DATA)->GetKeys(keys, keys_size);
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_CREATE_DELEGATE:
            {
                VariableDATA *target            = va_arg(ap, VariableDATA *);
                VariableDATA *deleg             = va_arg(ap, VariableDATA *);
                char         *class_member_name = va_arg(ap, char *);

                if (((target->TYPE == VARIABLE_CLASS) || (target->TYPE == VARIABLE_DELEGATE)) && (target->CLASS_DATA) && (deleg) && (class_member_name)) {
                    CLASS_CHECK(deleg);
                    int     m_type = 0;
                    INTEGER index  = ((CompiledClass *)target->CLASS_DATA)->_Class->HasMemberInCompiled(class_member_name, &m_type);
                    if (index) {
                        index--;
                        ClassMember *CM = index < ((CompiledClass *)target->CLASS_DATA)->_Class->pMEMBERS_COUNT ? ((CompiledClass *)target->CLASS_DATA)->_Class->pMEMBERS[index] : 0;

                        if ((CM) && (CM->IS_FUNCTION)) {
                            deleg->TYPE          = VARIABLE_DELEGATE;
                            deleg->DELEGATE_DATA = index + 1;
                            deleg->CLASS_DATA    = target->CLASS_DATA;
                            ((CompiledClass *)target->CLASS_DATA)->LINKS++;
                            ((CompiledClass *)target->CLASS_DATA)->reachable = 0x1C;
                        } else
                            result = INVALID_INVOKE_PARAMETER;
                    } else
                        result = INVALID_INVOKE_PARAMETER;
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_CALL_DELEGATE:
        case INVOKE_CALL_DELEGATE_THREAD:
#ifdef SIMPLE_MULTI_THREADING
        case INVOKE_CALL_DELEGATE_THREAD_SAFE:
        case INVOKE_CALL_DELEGATE_THREAD_SPINLOCK:
#endif
            {
                VariableDATA *target            = va_arg(ap, VariableDATA *);
                VariableDATA **SENDER_RESULT    = va_arg(ap, VariableDATA * *);
                VariableDATA **SENDER_EXCEPTION = va_arg(ap, VariableDATA * *);
#ifdef SIMPLE_MULTI_THREADING
                INTEGER *spin_lock = NULL;
                if (INVOKE_TYPE == INVOKE_CALL_DELEGATE_THREAD_SPINLOCK)
                    spin_lock  = va_arg(ap, INTEGER *);
#endif

                if ((!target) || (target->TYPE != VARIABLE_DELEGATE) || (!target->CLASS_DATA)) {
#ifdef SIMPLE_MULTI_THREADING
                    if (spin_lock)
                        *spin_lock = 0;
#endif
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                PIFAlizator *PIF = GET_PIF(((CompiledClass *)target->CLASS_DATA));
#ifdef SIMPLE_MULTI_THREADING
                char thread_created = 0;
                if (((INVOKE_TYPE == INVOKE_CALL_DELEGATE_THREAD) || (INVOKE_TYPE == INVOKE_CALL_DELEGATE_THREAD_SAFE) || (INVOKE_TYPE == INVOKE_CALL_DELEGATE_THREAD_SPINLOCK)) && (target->TYPE == VARIABLE_DELEGATE) && (target->CLASS_DATA)) {
                    NEW_THREAD
                    thread_created = 1;

                    if (PIF)
                        PIF->EnsureThreadSafe();
                }
#endif

                if ((PIF) && (INVOKE_TYPE == INVOKE_CALL_DELEGATE_THREAD_SAFE)) {
                    semp(PIF->DelegateLock);
                }

                if ((target->TYPE == VARIABLE_DELEGATE) && (target->CLASS_DATA)) {
                    INTEGER      parameters = va_arg(ap, INTEGER);
                    VariableDATA **CTX      = 0;
                    bool         is_list    = false;
                    if (parameters == -1) {
                        CTX = va_arg(ap, VariableDATA * *);
                        int i = 0;
                        while (CTX [i++]) {
                        }
                        parameters = i - 1;
                        is_list    = true;
                    } else {
                        CTX = parameters ? (VariableDATA **)FAST_MALLOC(sizeof(VariableDATA *) * parameters) : (VariableDATA **)0 ;
                    }
                    ParamList FORMAL_PARAM;
                    FORMAL_PARAM.COUNT       = parameters;
                    FORMAL_PARAM.PARAM_INDEX = parameters ? DELTA_REF(&FORMAL_PARAM, new INTEGER [parameters]) : 0;

                    if (is_list) {
                        for (int i = 0; i < parameters; i++) {
                            DELTA_UNREF(&FORMAL_PARAM, FORMAL_PARAM.PARAM_INDEX) [i] = i + 1;
                            CTX [i]->LINKS++;
                        }
                    } else {
                        for (int i = 0; i < parameters; i++) {
                            DELTA_UNREF(&FORMAL_PARAM, FORMAL_PARAM.PARAM_INDEX) [i] = i + 1;
                            INTEGER type = va_arg(ap, INTEGER);
                            if (type == VARIABLE_UNDEFINED) {
                                CTX [i] = va_arg(ap, VariableDATA *);
                                CTX [i]->LINKS++;
                            } else {
                                char   *str = va_arg(ap, char *);
                                NUMBER nr   = va_arg(ap, double);

                                CTX [i] = (VariableDATA *)VAR_ALLOC(PIF);
                                CTX [i]->TYPE = VARIABLE_NUMBER;
                                CTX [i]->IS_PROPERTY_RESULT = 0;
                                CTX [i]->LINKS       = 1;
                                CTX [i]->NUMBER_DATA = 0;
                                SetVariable(CTX [i], type, str, nr);
                            }
                        }
                    }
                    VariableDATA *RESULT = target;
                    VariableDATA *lOwner = 0;

                    try {
                        lOwner = (VariableDATA *)VAR_ALLOC(PIF);
                        lOwner->CLASS_DATA = target->CLASS_DATA;
                        lOwner->IS_PROPERTY_RESULT = 0;
                        lOwner->LINKS = 1;
                        lOwner->TYPE = VARIABLE_CLASS;

                        ((CompiledClass *)RESULT->CLASS_DATA)->LINKS++;
#ifndef SIMPLE_MULTI_THREADING
                        SCStack *STACK_TRACE = NULL;
                        if ((PIF) && (INVOKE_TYPE == INVOKE_CALL_DELEGATE)) {
                            GCRoot *root = PIF->GCROOT;
                            if (root) {
                                STACK_TRACE = (SCStack *)root->STACK_TRACE;
                                if (STACK_TRACE)
                                    STACK_TRACE = (SCStack *)STACK_TRACE->TOP;
                            }
                        }
#endif
                        *SENDER_RESULT = ((CompiledClass *)RESULT->CLASS_DATA)->_Class->ExecuteDelegate(PIF,
                                                                                                        (INTEGER)RESULT->DELEGATE_DATA,
                                                                                                        lOwner,
                                                                                                        0,
                                                                                                        &FORMAL_PARAM,
                                                                                                        CTX,
                                                                                                        ((CompiledClass *)RESULT->CLASS_DATA)->_Class->CLSID,
                                                                                                        ((CompiledClass *)RESULT->CLASS_DATA)->_Class->CLSID,
#ifdef SIMPLE_MULTI_THREADING
                                                                                                        NULL, spin_lock);
#else
                                                                                                        STACK_TRACE);
#endif
                        FREE_VARIABLE(lOwner);
                        if (*SENDER_RESULT)
                            (*SENDER_RESULT)->LINKS++;
                    } catch (VariableDATA *LAST_THROW) {
                        FREE_VARIABLE(lOwner);
                        *SENDER_EXCEPTION = LAST_THROW;
                    }
#ifdef SIMPLE_MULTI_THREADING
                    if (spin_lock)
                        *spin_lock = 0;
#endif
                    if ((CTX) && ((intptr_t)CTX != -1)) {
                        for (int i = 0; i < parameters; i++) {
                            FREE_VARIABLE(CTX [i]);
                        }
                        if (!is_list) {
                            FAST_FREE(CTX);
                        }
                    }
                    if (FORMAL_PARAM.PARAM_INDEX) {
                        delete[] DELTA_UNREF(&FORMAL_PARAM, FORMAL_PARAM.PARAM_INDEX);
                    }
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
#ifdef SIMPLE_MULTI_THREADING
                if (thread_created) {
                    DONE_THREAD
                }
#endif
                if ((PIF) && (INVOKE_TYPE == INVOKE_CALL_DELEGATE_THREAD_SAFE)) {
                    semv(PIF->DelegateLock);
                }
            }
            break;

        case INVOKE_EXTERNAL_THREADING:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                INTEGER      i       = va_arg(ap, INTEGER);
                if ((!target) || ((target->TYPE != VARIABLE_DELEGATE) && (target->TYPE != VARIABLE_CLASS)) || (!target->CLASS_DATA)) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                PIFAlizator *PIF = GET_PIF(((CompiledClass *)target->CLASS_DATA));
                NEW_THREAD
            }
            break;

        case INVOKE_THREAD_LOCK:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                INTEGER      i       = va_arg(ap, INTEGER);
                if ((!target) || ((target->TYPE != VARIABLE_DELEGATE) && (target->TYPE != VARIABLE_CLASS)) || (!target->CLASS_DATA)) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }

                PIFAlizator *PIF = GET_PIF(((CompiledClass *)target->CLASS_DATA));
                if (!PIF) {
                    result = CANNOT_INVOKE_IN_THIS_CASE;
                    break;
                }
                if (i) {
                    semp(PIF->DelegateLock);
                } else {
                    semv(PIF->DelegateLock);
                }
            }
            break;

        case INVOKE_COUNT_DELEGATE_PARAMS:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_DELEGATE) && (target->CLASS_DATA)) {
                    int         mid = target->DELEGATE_DATA;
                    ClassMember *CM = mid ? ((CompiledClass *)target->CLASS_DATA)->_Class->pMEMBERS [mid - 1] : 0;
                    if (CM) {
                        result = CM->PARAMETERS_COUNT;
                    } else {
                        result = CANNOT_INVOKE_IN_THIS_CASE;
                    }
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_COUNT_DELEGATE_PARAMS2:
            {
                VariableDATA *target     = va_arg(ap, VariableDATA *);
                INTEGER      *params     = va_arg(ap, INTEGER *);
                INTEGER      *min_params = va_arg(ap, INTEGER *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_DELEGATE) && (target->CLASS_DATA)) {
                    int         mid = target->DELEGATE_DATA;
                    ClassMember *CM = mid ? ((CompiledClass *)target->CLASS_DATA)->_Class->pMEMBERS [mid - 1] : 0;
                    if (CM) {
                        *params     = CM->PARAMETERS_COUNT;
                        *min_params = CM->MUST_PARAMETERS_COUNT;
                    } else {
                        result = CANNOT_INVOKE_IN_THIS_CASE;
                    }
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_GET_DELEGATE_NAMES:
            {
                VariableDATA *target       = va_arg(ap, VariableDATA *);
                char         **class_name  = va_arg(ap, char **);
                char         **member_name = va_arg(ap, char **);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if ((target->TYPE == VARIABLE_DELEGATE) && (target->CLASS_DATA)) {
                    int         mid = target->DELEGATE_DATA;
                    ClassMember *CM = mid ? ((CompiledClass *)target->CLASS_DATA)->_Class->pMEMBERS [mid - 1] : 0;
                    if (CM) {
                        *class_name  = (char *)((CompiledClass *)target->CLASS_DATA)->_Class->NAME.c_str();
                        *member_name = (char *)CM->NAME;
                    } else {
                        result = CANNOT_INVOKE_IN_THIS_CASE;
                    }
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_DEFINE_CLASS:
            {
                PIFAlizator *pif = va_arg(ap, PIFAlizator *);

                char *classname = va_arg(ap, char *);

                if (pif->ClassExists(classname)) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }

                ClassCode *CC = new ClassCode(classname, pif);

                char *member_name = va_arg(ap, char *);
                char is_array     = 0;
                while (member_name) {
                    size_t     len = strlen(member_name);
                    AnsiString name;
                    if ((member_name [len - 2] == '[') && (member_name [len - 1] == ']')) {
                        is_array = 1;
                        name.LoadBuffer(member_name, len - 2);
                    } else {
                        name     = member_name;
                        is_array = 0;
                    }

                    char        *ref_mname = pif->CheckMember(name);
                    ClassMember *CM        = CC->AddMember(pif, ref_mname, 0, "STATIC_MODULE", ACCESS_PUBLIC, true);
                    if (is_array) {
                        CM->VD->TYPE = VARIABLE_ARRAY;
                    }
                    member_name = va_arg(ap, char *);
                }
                CC->NEEDED        = 0;
                CC->DEFINED_LEVEL = -1;
                semp(PIFAlizator::WorkerLock);
                pif->ClassList->Add(CC, DATA_CLASS_CODE);
#ifdef CACHED_CLASSES
                HASH_TYPE key = hash_func(CC->NAME.c_str(), CC->NAME.Length());
                pif->CachedClasses[key] = pif->ClassList.Count();
#endif
                pif->SyncClassList();
                if (pif->parentPIF)
                    ((PIFAlizator *)pif->parentPIF)->SyncClassList();
                semv(PIFAlizator::WorkerLock);
            }
            break;

        case INVOKE_DEFINE_CONSTANT:
            {
                PIFAlizator *pif      = va_arg(ap, PIFAlizator *);
                char        *CONSTANT = va_arg(ap, char *);
                char        *value    = va_arg(ap, char *);

                if ((pif) && (CONSTANT) && (value)) {
                    VariableDESCRIPTOR *VD = 0;
#ifdef STDMAP_CONSTANTS
                    VD = (VariableDESCRIPTOR *)pif->ConstantList->Find(CONSTANT);
#else
                    int ct_count           = pif->ConstantList->Count();
                    for (int i = 0; i < ct_count; i++) {
                        VariableDESCRIPTOR *VTEMP = (VariableDESCRIPTOR *)pif->ConstantList->Item(i);
                        if (VTEMP->name == CONSTANT) {
                            VD = VTEMP;
                            break;
                        }
                    }
#endif
                    if (!VD) {
                        VD         = new VariableDESCRIPTOR;
#ifdef STDMAP_CONSTANTS
                        VD->name  = CONSTANT;
#endif
                        VD->BY_REF = 0;
                        pif->ConstantList->Add(VD, DATA_VAR_DESCRIPTOR);
                    }
#ifndef STDMAP_CONSTANTS
                    VD->name  = CONSTANT;
#endif
                    VD->value = value;
                    VD->USED  = -1;
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_GET_APPLICATION_INFO:
            {
                PIFAlizator *pif         = va_arg(ap, PIFAlizator *);
                int         *pipein      = va_arg(ap, int *);
                int         *pipeout     = va_arg(ap, int *);
                int         *apid        = va_arg(ap, int *);
                int         *parent_apid = va_arg(ap, int *);

                if ((pif) && (apid) && (pipein) && (pipeout) && (parent_apid)) {
                    *pipein      = pif->pipe_read;
                    *pipeout     = pif->pipe_write;
                    *apid        = pif->apid;
                    *parent_apid = pif->parent_apid;
                }
            }
            break;

        case INVOKE_GET_SERIAL_CLASS_NO_DEFAULTS:
        case INVOKE_GET_SERIAL_CLASS:
            {
                CompiledClass *CC             = va_arg(ap, CompiledClass *);
                int           max_members     = va_arg(ap, int);
                char          **class_name    = va_arg(ap, char **);
                char          **pmembers      = va_arg(ap, char **);
                unsigned char *flags          = va_arg(ap, unsigned char *);
                char          *access         = va_arg(ap, char *);
                char          *types          = va_arg(ap, char *);
                char          **szValue       = va_arg(ap, char **);
                double        *n_data         = va_arg(ap, double *);
                void          **class_data    = va_arg(ap, void **);
                void          **variable_data = va_arg(ap, void **);

                if (!CC) {
                    result = CANNOT_INVOKE_INTERFACE;
                    break;
                }
                *class_name = (char *)CC->_Class->NAME.c_str();
                result      = CC->_Class->GetSerialMembers(CC, max_members, pmembers, flags, access, types, szValue, n_data, class_data, variable_data, (int)(INVOKE_TYPE == INVOKE_GET_SERIAL_CLASS));
            }
            break;

        case INVOKE_GET_MEMBER_FROM_ID:
            {
                CompiledClass *CC       = va_arg(ap, CompiledClass *);
                intptr_t      member_id = va_arg(ap, intptr_t);
                char          **member  = va_arg(ap, char **);
                if ((!CC) || (member_id < 0)) {
                    result = CANNOT_INVOKE_INTERFACE;
                    break;
                }
                ClassMember *CM = NULL;
                if ((member_id > 0) && (member_id <= CC->_Class->pMEMBERS_COUNT))
                    CM = CC->_Class->pMEMBERS [member_id - 1];
                if (CM) {
                    *member = (char *)CM->NAME;
                } else {
                    result = CANNOT_INVOKE_IN_THIS_CASE;
                }
            }
            break;

        case INVOKE_GET_RELOC_MEMBER_FROM_ID:
            {
                CompiledClass *CC       = va_arg(ap, CompiledClass *);
                intptr_t      member_id = va_arg(ap, intptr_t);
                char          **member  = va_arg(ap, char **);
                if ((!CC) || (member_id < 0)) {
                    result = CANNOT_INVOKE_INTERFACE;
                    break;
                }
                int         mid = CC->_Class->Relocation(member_id);
                ClassMember *CM = mid ? CC->_Class->pMEMBERS [mid - 1] : 0;
                if (CM) {
                    *member = (char *)CM->NAME;
                } else {
                    result = CANNOT_INVOKE_IN_THIS_CASE;
                }
            }
            break;

        case INVOKE_HAS_MEMBER:
            {
                CompiledClass *CC     = va_arg(ap, CompiledClass *);
                char          *member = va_arg(ap, char *);
                if (!CC) {
                    result = CANNOT_INVOKE_INTERFACE;
                    break;
                }

                if (CC->_Class->HasMember(member)) {
                    result = 1;
                } else {
                    result = 0;
                }
            }
            break;

        case INVOKE_OBJECT_LINKS:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                if (target->CLASS_DATA) {
                    if ((target->TYPE == VARIABLE_CLASS) || (target->TYPE == VARIABLE_DELEGATE)) {
                        result = ((CompiledClass *)target->CLASS_DATA)->LINKS;
                    } else
                    if (target->TYPE == VARIABLE_ARRAY) {
                        result = ((Array *)target->CLASS_DATA)->LINKS;
                    }
                }
            }
            break;

        case INVOKE_VAR_LINKS:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                if (target) {
                    result = target->LINKS;
                }
            }
            break;

        case INVOKE_CLI_ARGUMENTS:
            {
                intptr_t *argc   = va_arg(ap, intptr_t *);
                char     ***argv = va_arg(ap, char ***);
                *argv = PIFAlizator::argv;
                *argc = PIFAlizator::argc;
            }
            break;

        case INVOKE_SORT_ARRAY_BY_KEYS:
            {
                Array *target = va_arg(ap, Array *);
                Array **res   = va_arg(ap, Array * *);

                if (target) {
                    *res = target->SortArrayElementsByKey();
                    if (*res)
                        (*res)->LINKS = 0;
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_CHECK_POINT:
            {
                int status = va_arg(ap, int);
                if (PIFAlizator::CheckPoint) {
                    PIFAlizator::CheckPoint(status);
                } else {
                    result = CANNOT_INVOKE_INTERFACE;
                }
            }
            break;

        case INVOKE_GET_USERDATA:
            {
                PIFAlizator *pif   = va_arg(ap, PIFAlizator *);
                void        **data = va_arg(ap, void **);
                if (!pif) {
                    result = INVALID_INVOKE_PARAMETER;
                } else {
                    *data = pif->out->userdata;
                }
            }
            break;

        case INVOKE_GET_THREAD_DATA:
            {
                void **data = va_arg(ap, void **);
                *data = GetStaticData();
            }
            break;

        case INVOKE_SET_THREAD_DATA:
            {
                void *data = va_arg(ap, void *);
                SetStaticData(data);
            }
            break;

        case INVOKE_PRINT:
            {
                PIFAlizator *PIF  = va_arg(ap, PIFAlizator *);
                char        *what = va_arg(ap, char *);
                INTEGER     len   = va_arg(ap, INTEGER);
                PIF->out->Print(what, len);
            }
            break;

        case INVOKE_GET_PROTO_SOCKET:
            {
                PIFAlizator *PIF = va_arg(ap, PIFAlizator *);
                if ((PIF) && (PIF->direct_pipe > 0) && (eof(PIF->direct_pipe) == 0)) {
#ifdef _WIN32
                    long new_sock = 0;
                    if (read(PIF->direct_pipe, &new_sock, sizeof(new_sock)) == sizeof(new_sock)) {
#else
                    int new_sock = 0;
                    if (sock_fd_read(PIF->direct_pipe, &new_sock) >= 0) {
#endif
                        if (new_sock > 0) {
                            PIF->out->sock         = new_sock;
                            PIFAlizator::refSOCKET = new_sock;
                        }
                    }
                }
                result = PIF->out->sock;
            }
            break;

        case INVOKE_GET_DIRECT_PIPE:
            {
                PIFAlizator *PIF = va_arg(ap, PIFAlizator *);
                if (PIF)
                    result = PIF->direct_pipe;
                else
                    result = -1;
            }
            break;

        case INVOKE_HIBERNATE:
            {
                PIFAlizator *pif  = va_arg(ap, PIFAlizator *);
                char        *flag = va_arg(ap, char *);
                if (!pif) {
                    result = INVALID_INVOKE_PARAMETER;
                } else {
                    if (*flag) {
                        pif->Hibernate();
                    } else {
                    }
                }
            }
            break;

        case INVOKE_DELEGATE_BYTECODE:
            {
                VariableDATA *target = va_arg(ap, VariableDATA *);
                void         **CODE  = va_arg(ap, void **);
                INTEGER      *len    = va_arg(ap, INTEGER *);

                void    **DATA = va_arg(ap, void **);
                INTEGER *dlen  = va_arg(ap, INTEGER *);

                if ((!target) || (target->TYPE != VARIABLE_DELEGATE) || (!target->CLASS_DATA)) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                ClassCode *CC = ((CompiledClass *)target->CLASS_DATA)->_Class;
                int         relocation = target->DELEGATE_DATA;
                ClassMember *pMEMBER_i = relocation ? CC->pMEMBERS [relocation - 1] : 0;
                if ((pMEMBER_i) && (pMEMBER_i->OPTIMIZER)) {
                    RuntimeOptimizedElement *OElist = ((Optimizer *)pMEMBER_i->OPTIMIZER)->CODE;
                    ParamList *Parameters           = ((Optimizer *)pMEMBER_i->OPTIMIZER)->PARAMS;
                    int       count = ((Optimizer *)pMEMBER_i->OPTIMIZER)->codeCount;
                    *len = count;
                    TreeContainer *tc = (TreeContainer *)malloc(sizeof(TreeContainer) * count);
                    for (int it = 0; it < count; it++) {
                        RuntimeOptimizedElement *OE    = &OElist[it];
                        TreeContainer           *reftc = &tc[it];

                        reftc->Operator_ID       = OE->Operator_ID;
                        reftc->Operator_TYPE     = OE->Operator_TYPE;
                        reftc->OperandLeft_ID    = OE->OperandLeft_ID;
                        reftc->OperandRight_ID   = OE->OperandRight_ID;
                        if ((OE->Operator_TYPE == TYPE_OPERATOR) && (OE->Operator_ID == KEY_DLL_CALL) && (OE->OperandLeft_ID == STATIC_CLASS_DLL)) {
                            reftc->Function = (char *)OE->OperandRight_PARSE_DATA.c_str();
                        } else {
                            reftc->Function = 0;
                        }

                        if ((OE->Operator_TYPE == TYPE_OPERATOR) && ((OE->Operator_ID == KEY_DLL_CALL) || (OE->Operator_ID == KEY_SEL) || (OE->Operator_ID == KEY_NEW))) {
                            if ((OE->Operator_ID == KEY_SEL) && (OE->OperandLeft_ID == 1)) {
                                int         i2          = (int)OE->OperandRight_ID - 1;
                                int         relocation2 = CC->Relocation(i2);
                                ClassMember *pMEMBER_i2 = relocation2 ? CC->pMEMBERS [relocation2 - 1] : 0;
                                if (pMEMBER_i2)
                                    reftc->Function = (char *)pMEMBER_i2->NAME;
                            }
                            if (((OE->Operator_ID != KEY_NEW) || (OE->OperandLeft_ID != -1)) && (OE->OperandReserved_ID > 0)) {
                                ParamList *FORMAL_PARAMETERS = &Parameters[OE->OperandReserved_ID - 1];
                                reftc->ParametersCount = FORMAL_PARAMETERS->COUNT;
                                reftc->Parameters      = DELTA_UNREF(FORMAL_PARAMETERS, FORMAL_PARAMETERS->PARAM_INDEX);
                            } else {
                                reftc->ParametersCount = -1;
                                reftc->Parameters      = 0;
                            }
                        } else {
                            reftc->ParametersCount = -1;
                            reftc->Parameters      = 0;
                        }
                        reftc->OperandReserved_ID   = OE->OperandReserved_ID;
                        reftc->OperandReserved_TYPE = OE->OperandReserved_TYPE;
                        reftc->Result_ID            = OE->Result_ID;
                    }
                    *CODE = tc;
                    count = ((Optimizer *)pMEMBER_i->OPTIMIZER)->dataCount;
                    RuntimeVariableDESCRIPTOR *VDS = ((Optimizer *)pMEMBER_i->OPTIMIZER)->DATA;
                    *dlen = count;
                    TreeVD *tvd = (TreeVD *)malloc(sizeof(TreeVD) * count);
                    for (int it = 0; it < count; it++) {
                        RuntimeVariableDESCRIPTOR *VD = &VDS[it];
                        tvd[it].TYPE   = VD->TYPE;
                        tvd[it].nValue = VD->nValue;
                        tvd[it].Value  = VD->value.c_str();
                        tvd[it].Length = VD->value.Length();
                        tvd[it].IsRef  = VD->BY_REF;
                    }
                    *DATA = tvd;
                } else {
                    *CODE  = 0;
                    *len   = 0;
                    *DATA  = 0;
                    *dlen  = 0;
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_BYTECODE_FREE:
            {
                void *CODE = va_arg(ap, void *);
                void *DATA = va_arg(ap, void *);

                if (CODE)
                    free(CODE);
                if (DATA)
                    free(DATA);
            }
            break;

        case INVOKE_PROFILER:
            {
                PIFAlizator *pif = va_arg(ap, PIFAlizator *);
                pif->PROFILE_DRIVEN = va_arg(ap, char *);
            }
            break;

        case INVOKE_MULTITHREADED:
#ifdef SIMPLE_MULTI_THREADING
            result = 1;
#else
            result = 0;
#endif
            break;

        case INVOKE_GREENTHREAD:
            {
                VariableDATA     *target = va_arg(ap, VariableDATA *);
                GreenThreadCycle **cycle = va_arg(ap, GreenThreadCycle * *);
                *cycle = 0;
                if ((!target) || (target->TYPE != VARIABLE_DELEGATE) || (!target->CLASS_DATA)) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                PIFAlizator *PIF = GET_PIF(((CompiledClass *)target->CLASS_DATA));
                if ((target->TYPE == VARIABLE_DELEGATE) && (target->CLASS_DATA)) {
                    try {
                        ((CompiledClass *)target->CLASS_DATA)->reachable = 0x1C;
                        VariableDATA *var = (VariableDATA *)VAR_ALLOC(PIF);
                        var->IS_PROPERTY_RESULT = 0;
                        var->LINKS          = 1;
                        var->DELEGATE_DATA  = target->DELEGATE_DATA;
                        var->CLASS_DATA     = target->CLASS_DATA;
                        ((CompiledClass *)var->CLASS_DATA)->LINKS++;
                        var->TYPE        = target->TYPE;
                        *cycle = ((CompiledClass *)target->CLASS_DATA)->_Class->CreateThread(PIF, (INTEGER)target->DELEGATE_DATA, var);
                    } catch (VariableDATA *LAST_THROW) {
                        FREE_VARIABLE(LAST_THROW);
                    }
                } else {
                    result = INVALID_INVOKE_PARAMETER;
                }
            }
            break;

        case INVOKE_FREEGREENTHREAD:
            {
                GreenThreadCycle *cycle = va_arg(ap, GreenThreadCycle *);
                if (cycle)
                    ClassMember::DoneThread(cycle);
            }
            break;

        case INVOKE_GREENLOOP:
            {
                GreenThreadCycle *cycle = va_arg(ap, GreenThreadCycle *);
                GreenThreadCycle *last  = va_arg(ap, GreenThreadCycle *);
                va_end(ap);
                if (cycle) {
                    cycle->LAST_THREAD = last;
                    return ConceptInterpreter::StacklessInterpret((PIFAlizator *)cycle->PIF, cycle);
                } else
                    return CANNOT_INVOKE_INTERFACE;
            }
            break;

        case INVOKE_GREENLINK:
            {
                GreenThreadCycle *prev    = va_arg(ap, GreenThreadCycle *);
                GreenThreadCycle *current = va_arg(ap, GreenThreadCycle *);
                if (prev) {
                    prev->NEXT = current;
                } else {
                    result = CANNOT_INVOKE_INTERFACE;
                }
            }
            break;

        case INVOKE_CURRENTGREENTHREAD:
            {
                GreenThreadCycle *cycle    = va_arg(ap, GreenThreadCycle *);
                GreenThreadCycle **current = va_arg(ap, GreenThreadCycle * *);
                if (cycle)
                    *current = (GreenThreadCycle *)cycle->CURRENT_THREAD;
                else
                    *current = 0;
            }
            break;

        case INVOKE_GREENINSERT:
            {
                GreenThreadCycle *cycle   = va_arg(ap, GreenThreadCycle *);
                GreenThreadCycle *current = va_arg(ap, GreenThreadCycle *);
                if ((cycle) && (cycle->CURRENT_THREAD)) {
#ifdef SIMPLE_MULTI_THREADING
                    PIFAlizator *PIF = GET_PIF(((CompiledClass *)((VariableDATA *)cycle->OWNER)->CLASS_DATA));
                    semp(PIF->DelegateLock);
#endif

                    GreenThreadCycle *target = (GreenThreadCycle *)cycle->CURRENT_THREAD;
                    current->NEXT = target->NEXT;
                    target->NEXT  = current;

#ifdef SIMPLE_MULTI_THREADING
                    semv(PIF->DelegateLock);
#endif
                } else
                    result = CANNOT_INVOKE_INTERFACE;
            }
            break;

        case INVOKE_SETSENDMESSAGEFUNCTION:
            {
                PIFAlizator    *pif = va_arg(ap, PIFAlizator *);
                SECURE_MESSAGE sm   = va_arg(ap, SECURE_MESSAGE);
                if (pif) {
                    if (pif->out)
                        pif->out->send_secure_message = sm;
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_GETGLOBALS:
            {
                PIFAlizator *pif  = va_arg(ap, PIFAlizator *);
                Array       **arr = va_arg(ap, Array * *);
                if ((pif) && (arr)) {
                    if (!pif->var_globals) {
                        pif->var_globals = new(AllocArray(pif))Array(pif);
                        // never to be deleted
                        pif->var_globals->LINKS = 2;
                    }
                    *arr = pif->var_globals;
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_GETID:
            {
                PIFAlizator *pif = va_arg(ap, PIFAlizator *);
                if (pif) {
                    va_end(ap);
                    return ++pif->IDGenerator;
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_GETPROTODATA:
            {
                PIFAlizator *pif     = va_arg(ap, PIFAlizator *);
                int         position = va_arg(ap, int);
                void        **data   = va_arg(ap, void **);
                if ((pif) && (data) && (position >= 0) && (position < PDATA_ITEMS))
                    *data = pif->PDATA[position].data;
                else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_SETPROTODATA:
            {
                PIFAlizator        *pif      = va_arg(ap, PIFAlizator *);
                int                position  = va_arg(ap, int);
                void               *data     = va_arg(ap, void *);
                DESTROY_PROTO_DATA destroy_f = va_arg(ap, DESTROY_PROTO_DATA);
                if ((pif) && (data) && (position >= 0) && (position < PDATA_ITEMS)) {
                    pif->PDATA[position].data         = data;
                    pif->PDATA[position].destroy_func = destroy_f;
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_EXECUTECODE:
            {
                PIFAlizator  *pif  = va_arg(ap, PIFAlizator *);
                char         *code = va_arg(ap, char *);
                int          len   = va_arg(ap, int);
                VariableDATA *res  = va_arg(ap, VariableDATA *);
                if ((code) && (len > 0)) {
                    result = Concept_ExecuteBuffer(code, len, pif->INCLUDE_DIR.c_str(), pif->IMPORT_DIR.c_str(), pif->out->fprint, pif->out->sock, 0, NULL, NULL, pif->SERVER_PUBLIC_KEY, pif->SERVER_PRIVATE_KEY, pif->CLIENT_PUBLIC_KEY, pif->pipe_read, pif->pipe_write, pif->apid, pif->parent_apid, PIFAlizator::CheckPoint, res,  NULL, NULL);
                    // Optimizer::PIFOwner will be reset by the ExecuteBuffer function
                    // NOT THREAD SAFE !
                    Optimizer::PIFOwner = pif;
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_MEMBER_NAME:
            {
                PIFAlizator *pif   = va_arg(ap, PIFAlizator *);
                INTEGER     index  = va_arg(ap, INTEGER);
                char        **name = va_arg(ap, char **);
                if (name)
                    *name = 0;

                if ((name) && (index > 0)) {
                    if (pif->parentPIF)
                        *name = ((PIFAlizator *)pif->parentPIF)->GeneralMembers->Item(index - 1);
                    else
                        *name = pif->GeneralMembers->Item(index - 1);
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_CLASS_NAME:
            {
                PIFAlizator *pif   = va_arg(ap, PIFAlizator *);
                INTEGER     index  = va_arg(ap, INTEGER);
                char        **name = va_arg(ap, char **);
                if (name)
                    *name = 0;

                ClassCode **StaticClassList = ((pif->parentPIF) && ((PIFAlizator *)pif->parentPIF)->StaticClassList) ? ((PIFAlizator *)pif->parentPIF)->StaticClassList : pif->StaticClassList;
                if ((name) && (index > 0) && (pif->ClassList) && (index <= pif->ClassList->Count())) {
                    ClassCode *CC = StaticClassList[index - 1];
                    if (CC)
                        *name = (char *)CC->NAME.c_str();
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_MEMBER_INFO:
            {
                VariableDATA *target            = va_arg(ap, VariableDATA *);
                char         *class_member_name = va_arg(ap, char *);
                char         **mdef             = va_arg(ap, char **);
                INTEGER      *type = va_arg(ap, INTEGER *);
                INTEGER      *mset = va_arg(ap, INTEGER *);
                INTEGER      *mget = va_arg(ap, INTEGER *);
                INTEGER      *macc = va_arg(ap, INTEGER *);

                if (mdef)
                    *mdef = 0;
                if (type)
                    *type = -1;
                if (mset)
                    *mset = -1;
                if (mget)
                    *mget = -1;
                if (macc)
                    *macc = -1;

                if (((target->TYPE == VARIABLE_CLASS) || (target->TYPE == VARIABLE_DELEGATE)) && (target->CLASS_DATA) && (class_member_name)) {
                    int     m_type = 0;
                    INTEGER index  = ((CompiledClass *)target->CLASS_DATA)->_Class->HasMemberInCompiled(class_member_name, &m_type);
                    if (index) {
                        index--;
                        ClassMember *CM = index < ((CompiledClass *)target->CLASS_DATA)->_Class->pMEMBERS_COUNT ? ((CompiledClass *)target->CLASS_DATA)->_Class->pMEMBERS[index] : 0;

                        if (CM) {
                            if (mdef) {
                                if (CM->Defined_In)
                                    *mdef = (char *)((ClassCode *)CM->Defined_In)->NAME.c_str();
                            }
                            if (type) {
                                *type = CM->IS_FUNCTION;
                                if (CM->IS_OPERATOR)
                                    *type = 0x11;
                            }
                            if (mget)
                                *mget = CM->MEMBER_GET;
                            if (mset)
                                *mset = CM->MEMBER_SET;
                            if (macc)
                                *macc = CM->ACCESS;
                        } else
                            result = INVALID_INVOKE_PARAMETER;
                    } else
                        result = INVALID_INVOKE_PARAMETER;
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_CREATE_VARIABLE_2:
            {
                PIFAlizator  *pif     = va_arg(ap, PIFAlizator *);
                VariableDATA **target = va_arg(ap, VariableDATA * *);
                *target = (VariableDATA *)VAR_ALLOC(pif);
                (*target)->IS_PROPERTY_RESULT = 0;
                (*target)->LINKS       = 1;
                (*target)->NUMBER_DATA = 0;
                (*target)->TYPE        = VARIABLE_NUMBER;
            }
            break;

        case INVOKE_GET_KEYS:
            {
                PIFAlizator *pif = va_arg(ap, PIFAlizator *);
                if (pif) {
                    char **SERVER_PUBLIC_KEY  = va_arg(ap, char **);
                    char **SERVER_PRIVATE_KEY = va_arg(ap, char **);
                    char **CLIENT_PUBLIC_KEY  = va_arg(ap, char **);
                    if (SERVER_PUBLIC_KEY)
                        *SERVER_PUBLIC_KEY = pif->SERVER_PUBLIC_KEY;
                    if (SERVER_PRIVATE_KEY)
                        *SERVER_PRIVATE_KEY = pif->SERVER_PRIVATE_KEY;
                    if (CLIENT_PUBLIC_KEY)
                        *CLIENT_PUBLIC_KEY = pif->CLIENT_PUBLIC_KEY;
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_PROFILE_MEMORY:
            {
                PIFAlizator  *PIF    = va_arg(ap, PIFAlizator *);
                VariableDATA *RESULT = va_arg(ap, VariableDATA *);
                va_end(ap);
                CLASS_CHECK(RESULT);
                RESULT->TYPE       = VARIABLE_NUMBER;
                RESULT->CLASS_DATA = new(AllocArray(PIF))Array(PIF);
                RESULT->TYPE       = VARIABLE_ARRAY;
                return GetMemoryStatistics(PIF, RESULT->CLASS_DATA);
            }
            break;

        case INVOKE_CREATE_WORKER:
            {
                PIFAlizator *PIF     = va_arg(ap, PIFAlizator *);
                PIFAlizator **worker = va_arg(ap, PIFAlizator * *);

                if ((PIF) && (worker)) {
                    PIFAlizator *ref_pif = PIF;
                    while (ref_pif->parentPIF)
                        ref_pif = (PIFAlizator *)ref_pif->parentPIF;
                    semp(ref_pif->DelegateLock);
                    ref_pif->Workers++;
                    ref_pif->EnsureThreadSafe();
                    AnsiString S;
                    if (PIF->parentPIF)
                        PIF = (PIFAlizator *)PIF->parentPIF;
                    *worker                = new PIFAlizator("", "", &S, PIF->out, "", PIF->DebugOn, PIF->DEBUGGER_TRAP, PIF->DEBUGGER_RESERVED, PIF->SERVER_PUBLIC_KEY, PIF->SERVER_PRIVATE_KEY, PIF->CLIENT_PUBLIC_KEY, PIF);
                    (*worker)->apid        = PIF->apid;
                    (*worker)->parent_apid = PIF->parent_apid;
                    (*worker)->pipe_read   = -1;
                    (*worker)->pipe_write  = -1;
                    (*worker)->direct_pipe = -1;
                    (*worker)->Workers     = ref_pif->Workers;
                    semv(ref_pif->DelegateLock);
                } else
                    result = INVALID_INVOKE_PARAMETER;
                // to do
            }
            break;

        case INVOKE_FREE_WORKER:
            {
                PIFAlizator *PIF = va_arg(ap, PIFAlizator *);
                if (PIF) {
                    PIFAlizator *ref_pif = PIF;
                    while (ref_pif->parentPIF)
                        ref_pif = (PIFAlizator *)ref_pif->parentPIF;
                    semp(ref_pif->DelegateLock);
                    if (ref_pif)
                        ref_pif->Workers--;
                    semv(ref_pif->DelegateLock);
                    delete PIF;
                } else
                    result = INVALID_INVOKE_PARAMETER;
                // to do
            }
            break;

        case INVOKE_CREATE_INDEPENDENT_WORKER:
            {
                PIFAlizator *PIF     = va_arg(ap, PIFAlizator *);
                PIFAlizator **worker = va_arg(ap, PIFAlizator * *);
                char *filename = va_arg(ap, char *);
                char *error_buffer = va_arg(ap, char *);
                INTEGER error_buffer_size = va_arg(ap, INTEGER);
                if ((PIF) && (worker) && (filename)) {
                    AnsiString S;
                    S.LoadFile(filename);
                    PIFAlizator *ref_pif = PIF;
                    while (ref_pif->parentPIF)
                        ref_pif = (PIFAlizator *)ref_pif->parentPIF;

                    semp(ref_pif->DelegateLock);
                    ref_pif->Workers++;
                    ref_pif->EnsureThreadSafe();
                    *worker                = new PIFAlizator(ref_pif->INCLUDE_DIR, ref_pif->IMPORT_DIR, &S, ref_pif->out, filename, ref_pif->DebugOn, ref_pif->DEBUGGER_TRAP, ref_pif->DEBUGGER_RESERVED, ref_pif->SERVER_PUBLIC_KEY, ref_pif->SERVER_PRIVATE_KEY, ref_pif->CLIENT_PUBLIC_KEY);
                    (*worker)->UpdatePath();
                    if ((!ref_pif->DebugOn) && (!(*worker)->Unserialize(AnsiString(filename) + DEFAULT_BIN_EXTENSION))) {
                        (*worker)->Optimize(0, true);
                    } else {
                        (*worker)->Execute();
                        if (!(*worker)->ErrorCount()) {
                            (*worker)->Optimize();
                        }
                    }
                    AnsiString errors;
                    if ((*worker)->ErrorCount()) {
                        errors = (*worker)->PRINT_ERRORS(0);
                        if (PIF->out) {
                            if ((!error_buffer) || (error_buffer_size <= 0))
                                PIF->out->Print(errors);
                        }
                        delete *worker;
                        *worker = NULL;
                    } else {
                        if (((*worker)->WarningCount()) && (PIF->out))
                            PIF->out->Print((*worker)->PRINT_WARNINGS(0));

                        (*worker)->apid        = ref_pif->apid;
                        (*worker)->parent_apid = ref_pif->parent_apid;
                        (*worker)->pipe_read   = -1;
                        (*worker)->pipe_write  = -1;
                        (*worker)->direct_pipe = -1;
                        (*worker)->Workers     = ref_pif->Workers;
                    }
                    semv(ref_pif->DelegateLock);
                    if ((error_buffer_size > 0) && (error_buffer)) {
                        if (*worker) {
                            error_buffer[0] = 0;
                        } else {
                            int max_len = error_buffer_size - 1;
                            if (errors.Length() < max_len)
                                max_len = errors.Length();
                            memcpy(error_buffer, errors.c_str(), max_len);
                            error_buffer[max_len] = 0;
                        }
                    }
                } else
                    result = INVALID_INVOKE_PARAMETER;
                // to do
            }
            break;

        case INVOKE_WORKER_ERRORS:
            {
                PIFAlizator *PIF        = va_arg(ap, PIFAlizator *);
                INTEGER print_warnings  = va_arg(ap, INTEGER);
                char *error_buffer      = va_arg(ap, char *);
                INTEGER error_buffer_size = va_arg(ap, INTEGER);
                if (PIF) {
                    AnsiString errors;
                    semp(PIFAlizator::WorkerLock);
                    if (print_warnings) {
                        if (PIF->WarningCount())
                            errors = PIF->PRINT_WARNINGS(0);
                    } else
                    if (PIF->ErrorCount())
                        errors = PIF->PRINT_ERRORS(0);
                    semv(PIFAlizator::WorkerLock);
                    if ((error_buffer) && (error_buffer_size > 0)) {
                        int max_len = error_buffer_size - 1;
                        if (errors.Length() < max_len)
                            max_len = errors.Length();
                        memcpy(error_buffer, errors.c_str(), max_len);
                        error_buffer[max_len] = 0;
                    }
                    result = errors.Length();
                } else
                    result = INVALID_INVOKE_PARAMETER;
            }
            break;

        case INVOKE_CREATE_OBJECT_2:
            {
                PIFAlizator  *pif        = va_arg(ap, PIFAlizator *);
                VariableDATA *target     = va_arg(ap, VariableDATA *);
                char         *class_name = va_arg(ap, char *);
                VariableDATA **CONTEXT   = va_arg(ap, VariableDATA * *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }

                CLASS_CHECK(target);
                target->TYPE = VARIABLE_NUMBER;

                int       class_count = pif->ClassList->Count();
                int       clsid       = -1;
                ClassCode *CC         = 0;
                ClassCode **StaticClassList = ((pif->parentPIF) && ((PIFAlizator *)pif->parentPIF)->StaticClassList) ? ((PIFAlizator *)pif->parentPIF)->StaticClassList : pif->StaticClassList;
                for (int i = 0; i < class_count; i++) {
                    CC = StaticClassList[i];
                    if (CC->NAME == class_name) {
                        clsid = i;
                        break;
                    }
                }
                if (clsid == -1) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }

                ParamList FORMAL_PARAM;
                INTEGER   index = 0;
                FORMAL_PARAM.COUNT       = 0;
                FORMAL_PARAM.PARAM_INDEX = 0;
                void *ref = 0;
                do {
                    index = va_arg(ap, INTEGER);
                    if (index >= 0) {
                        FORMAL_PARAM.COUNT++;
                        ref = realloc(ref, FORMAL_PARAM.COUNT * sizeof(INTEGER));
                        FORMAL_PARAM.PARAM_INDEX = (INTEGER *)DELTA_REF(&FORMAL_PARAM, ref);
                        ((INTEGER *)ref)[FORMAL_PARAM.COUNT - 1] = index;
                    }
                } while (index >= 0);
                SCStack *STACK_TRACE = NULL;
#ifndef SIMPLE_MULTI_THREADING
                if (pif) {
                    GCRoot *root = pif->GCROOT;
                    if (root) {
                        STACK_TRACE = (SCStack *)root->STACK_TRACE;
                        if (STACK_TRACE)
                            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;
                    }
                }
#endif
                target->TYPE       = VARIABLE_CLASS;
                target->CLASS_DATA = NULL;
                target->CLASS_DATA = CC->CreateInstance(pif, target, 0, &FORMAL_PARAM, (VariableDATA **)CONTEXT, STACK_TRACE);
                if (ref)
                    free(ref);
                if (!target->CLASS_DATA) {
                    target->TYPE = VARIABLE_NUMBER;
                    result       = INVALID_INVOKE_PARAMETER;
                    break;
                }

                target->TYPE = VARIABLE_CLASS;
            }
            break;

        case INVOKE_CREATE_OBJECT_3:
            {
                PIFAlizator  *pif         = va_arg(ap, PIFAlizator *);
                VariableDATA *target      = va_arg(ap, VariableDATA *);
                char         *class_name  = va_arg(ap, char *);
                VariableDATA **CONTEXT    = va_arg(ap, VariableDATA * *);
                INTEGER      *PARAM_INDEX = va_arg(ap, INTEGER *);
                if (!target) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }

                CLASS_CHECK(target);
                target->TYPE = VARIABLE_NUMBER;

                int       class_count = pif->ClassList->Count();
                int       clsid       = -1;
                ClassCode *CC         = 0;
                ClassCode **StaticClassList = ((pif->parentPIF) && ((PIFAlizator *)pif->parentPIF)->StaticClassList) ? ((PIFAlizator *)pif->parentPIF)->StaticClassList : pif->StaticClassList;
                for (int i = 0; i < class_count; i++) {
                    CC = StaticClassList[i];
                    if (CC->NAME == class_name) {
                        clsid = i;
                        break;
                    }
                }
                if (clsid == -1) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }

                ParamList FORMAL_PARAM;
                INTEGER   index = 0;
                FORMAL_PARAM.COUNT       = 0;
                FORMAL_PARAM.PARAM_INDEX = 0;
                void *ref = 0;
                do {
                    index = PARAM_INDEX[FORMAL_PARAM.COUNT];
                    if (index >= 0) {
                        FORMAL_PARAM.COUNT++;
                        ref = realloc(ref, FORMAL_PARAM.COUNT * sizeof(INTEGER));
                        FORMAL_PARAM.PARAM_INDEX = (INTEGER *)DELTA_REF(&FORMAL_PARAM, ref);
                        ((INTEGER *)ref)[FORMAL_PARAM.COUNT - 1] = index;
                    }
                } while (index >= 0);
                SCStack *STACK_TRACE = NULL;
#ifndef SIMPLE_MULTI_THREADING
                if (pif) {
                    GCRoot *root = pif->GCROOT;
                    if (root) {
                        STACK_TRACE = (SCStack *)root->STACK_TRACE;
                        if (STACK_TRACE)
                            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;
                    }
                }
#endif
                target->TYPE       = VARIABLE_CLASS;
                target->CLASS_DATA = NULL;
                target->CLASS_DATA = CC->CreateInstance(pif, target, 0, &FORMAL_PARAM, (VariableDATA **)CONTEXT, STACK_TRACE);
                if (ref)
                    free(ref);
                if (!target->CLASS_DATA) {
                    target->TYPE = VARIABLE_NUMBER;
                    result       = INVALID_INVOKE_PARAMETER;
                    break;
                }

                target->TYPE = VARIABLE_CLASS;
            }
            break;

        case INVOKE_DYNAMIC_INCLUDE:
            {
                PIFAlizator *pif           = va_arg(ap, PIFAlizator *);
                char        *code          = va_arg(ap, char *);
                char        *error_buf     = va_arg(ap, char *);
                INTEGER     error_buf_size = va_arg(ap, INTEGER);
                if (!code) {
                    result = INVALID_INVOKE_PARAMETER;
                    break;
                }
                semp(PIFAlizator::WorkerLock);
                if (pif->Errors.Count())
                    pif->Errors.Clear();
                int start_from = pif->ClassList->Count();
                if (!pif->fixed_class_count)
                    pif->fixed_class_count = start_from;
                PIFAlizator *ref_pif = pif;
                while (ref_pif->parentPIF)
                    ref_pif = (PIFAlizator *)ref_pif->parentPIF;
                semp(ref_pif->DelegateLock);
                // try clean
                if (pif->fixed_class_count < start_from) {
                    int not_used = 0;
                    for (int i = pif->fixed_class_count; i < start_from; i++) {
                        ClassCode *CC = (ClassCode *)pif->ClassList->Item(i);
                        if ((CC) && (CC->NAME == "*")) {
                            not_used++;
                        }
                    }
                    if (not_used == start_from - pif->fixed_class_count) {
                        for (int i = pif->fixed_class_count; i < start_from; i++) {
                            ClassCode *CC = (ClassCode *)pif->ClassList->Remove(pif->fixed_class_count);
                            if (CC)
                                delete CC;
                        }
                        start_from = pif->ClassList->Count();
                        pif->SyncClassList();
                    }
                }
                pif->RuntimeIncludeCode(code);
                if ((error_buf) && (error_buf_size > 0))
                    error_buf[0] = 0;
                if (pif->ClassList->Count() != start_from) {
                    if (!pif->Errors.Count())
                        pif->Optimize(start_from, 0, 0);
                    else {
                        int count = pif->ClassList->Count();
                        for (int i = start_from; i < count; i++) {
                            ClassCode *CC = (ClassCode *)pif->ClassList->Remove(start_from);
                            if (CC)
                                delete CC;
                        }
                    }
                }
                semv(ref_pif->DelegateLock);

                if (pif->Errors.Count()) {
                    int len = pif->Errors.Count();
                    if ((error_buf) && (error_buf_size > 0)) {
                        for (int i = 0; i < len; i++) {
                            AnsiException *E = (AnsiException *)pif->Errors.Item(i);
                            if (E) {
                                AnsiString tmp     = E->ToString(true);
                                int        tmp_len = tmp.Length();
                                if (tmp_len + 1 < error_buf_size) {
                                    memcpy(error_buf, tmp.c_str(), tmp_len);
                                    error_buf[tmp_len] = 0;
                                    error_buf         += tmp_len;
                                    error_buf_size    -= tmp_len;
                                } else
                                    break;
                            }
                        }
                    }
                    pif->Errors.Clear();
                }
                pif->SyncClassList();
                if (pif->parentPIF)
                    ((PIFAlizator *)pif->parentPIF)->SyncClassList();
                if (ref_pif->Workers) {
                    ref_pif->SyncClassList();
                    ref_pif->EnsureThreadSafe();
                }
                semv(PIFAlizator::WorkerLock);
            }
            break;

        case INVOKE_DYNAMIC_REMOVE_CLASS:
            {
                PIFAlizator *pif        = va_arg(ap, PIFAlizator *);
                char        *class_name = va_arg(ap, char *);
                result = INVALID_INVOKE_PARAMETER;
                if ((!class_name) || (!class_name[0]) || (class_name[0] == '*') || (!pif->fixed_class_count))
                    break;

                semp(PIFAlizator::WorkerLock);
                int count = pif->ClassList->Count();
                for (int i = pif->fixed_class_count; i < count; i++) {
                    ClassCode *CC = (ClassCode *)pif->ClassList->Item(i);
                    if ((CC) && (CC->NAME == class_name)) {
                        if (ClearVariablesByCLSID(pif, CC->CLSID)) {
                            pif->ClassList->Remove(i);
                            pif->SyncClassList();
                            delete CC;
                        } else
                            CC->NAME = "*";
                        result   = INVOKE_SUCCESS;
                        break;
                    }
                }
                semv(PIFAlizator::WorkerLock);
            }
            break;

        case INVOKE_CHECK_REACHABILITY:
            {
                PIFAlizator *pif = va_arg(ap, PIFAlizator *);
                result = INVALID_INVOKE_PARAMETER;
                if (pif)
                    result = ModuleCheckReachability(pif);
            }
            break;

        case INVOKE_GET_CALLSTACK:
            {
                PIFAlizator *pif    = va_arg(ap, PIFAlizator *);
                char        **buf   = va_arg(ap, char **);
                INTEGER     max_len = va_arg(ap, INTEGER);

                result = INVALID_INVOKE_PARAMETER;
                if ((pif) && (buf) && (max_len > 2)) {
                    // room for null character
                    max_len--;
                    GCRoot  *root        = pif->GCROOT;
                    SCStack *STACK_TRACE = NULL;
                    if (root) {
                        STACK_TRACE = (SCStack *)root->STACK_TRACE;
                        if (STACK_TRACE)
                            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;
                    }
                    int        first = 1;
                    AnsiString cstack;
                    while (STACK_TRACE) {
                        ClassMember *CM = (ClassMember *)STACK_TRACE->CM;
                        if (CM) {
                            if (CM->Defined_In) {
                                cstack += ((ClassCode *)CM->Defined_In)->NAME.c_str();
                                cstack += ".";
                            } else
                                cstack += "::";
                            cstack += CM->NAME;
                        } else {
                            cstack += "STATIC/LIBRARY.STATIC_FUNCTION";
                        }
                        cstack += ":";
                        if (first) {
                            cstack += AnsiString((D_LONG_TYPE)0);
                            first   = 0;
                        } else
                            cstack += AnsiString((D_LONG_TYPE)STACK_TRACE->line);
                        cstack     += "\n";
                        STACK_TRACE = (SCStack *)STACK_TRACE->PREV;
                    }
                    int len = cstack.Length();
                    if (len < max_len)
                        max_len = len;

                    memcpy(*buf, cstack.c_str(), max_len);
                    (*buf)[max_len] = 0;
                    return len;
                }
            }
            break;

        default:
            va_end(ap);
            return CANNOT_INVOKE_INTERFACE;
    }
    va_end(ap);
    return result;
}

SYS_INT LinkFunction(void *PIF, const char *FUNCTION_NAME, AnsiList *TARGET, void **CACHED_hDLL) {
#ifdef CACHE_MEMBERS
    SYS_INT val = FunctionMap[FUNCTION_NAME];
    if (val)
        return val;
#endif

    AnsiString PREFIX  = "CONCEPT__";
    AnsiString PREFIX2 = "CONCEPT_";
    int        count   = TARGET->Count();

    if (CACHED_hDLL) {
#ifdef _WIN32
        EXTERNAL_CALL _PROC_ADR = (EXTERNAL_CALL)GetProcAddress((HMODULE)*CACHED_hDLL, PREFIX2 + FUNCTION_NAME);
#else
        EXTERNAL_CALL _PROC_ADR = (EXTERNAL_CALL)dlsym((HMODULE)*CACHED_hDLL, PREFIX2 + FUNCTION_NAME);
#endif
        if (!_PROC_ADR) {
#ifdef _WIN32
            _PROC_ADR = (EXTERNAL_CALL)GetProcAddress((HMODULE)*CACHED_hDLL, PREFIX + FUNCTION_NAME);
#else
            _PROC_ADR = (EXTERNAL_CALL)dlsym((HMODULE)*CACHED_hDLL, PREFIX + FUNCTION_NAME);
#endif
        }

        if (_PROC_ADR) {
            FunctionMap.add(FUNCTION_NAME, (SYS_INT)_PROC_ADR);
            return (SYS_INT)_PROC_ADR;
        }
    }
    for (INTEGER i = 0; i < count; i++) {
        HMODULE hDLL = (HMODULE)TARGET->Item(i);
#ifdef _WIN32
        EXTERNAL_CALL _PROC_ADR = (EXTERNAL_CALL)GetProcAddress(hDLL, PREFIX2 + FUNCTION_NAME);
#else
        EXTERNAL_CALL _PROC_ADR = (EXTERNAL_CALL)dlsym(hDLL, PREFIX2 + FUNCTION_NAME);
#endif
        if (!_PROC_ADR) {
#ifdef _WIN32
            _PROC_ADR = (EXTERNAL_CALL)GetProcAddress(hDLL, PREFIX + FUNCTION_NAME);
#else
            _PROC_ADR = (EXTERNAL_CALL)dlsym(hDLL, PREFIX + FUNCTION_NAME);
#endif
        }

        if (_PROC_ADR) {
            *CACHED_hDLL = hDLL;
#ifdef CACHE_MEMBERS
            FunctionMap.add(FUNCTION_NAME, (SYS_INT)_PROC_ADR);
#endif
            return (SYS_INT)_PROC_ADR;
        }
    }
    unsigned char is_private = 0;
    SYS_INT BUILTIN_ID = (SYS_INT)BUILTINADDR(PIF, FUNCTION_NAME, &is_private);
#ifdef CACHE_MEMBERS
    if ((BUILTIN_ID) && (!is_private))
         FunctionMap.add(FUNCTION_NAME, BUILTIN_ID);
#endif
    return BUILTIN_ID;
}

void DoneLinking() {
#ifdef CACHE_MEMBERS
    FunctionMap.clear();
#endif
}

