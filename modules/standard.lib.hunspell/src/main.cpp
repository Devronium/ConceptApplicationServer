//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <ctype.h>
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <hunspell.h>
#include "semhh.h"

#ifndef NO_THREAD_CACHE
static HHSEM sem;
static int links = 0;
static AnsiString affpath;
static AnsiString dpath;
static Hunhandle *shared = NULL;
#endif
#define HUNSPELL_LOCK(h)   if (h == shared) semp(sem);
#define HUNSPELL_UNLOCK(h) if (h == shared) semv(sem);
//--------------------------------------------------------------------------- 
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifndef NO_THREAD_CACHE
    seminit(sem, 1);
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER)
        semdel(sem);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_create, 2)
    T_STRING(0)
    T_STRING(1)

#ifndef NO_THREAD_CACHE
    if (shared) {
        if ((affpath == PARAM(0)) && (dpath == PARAM(1))) {
            semp(sem);
            links++;
            semv(sem);
            RETURN_NUMBER((SYS_INT)shared);
            return 0;
        }
    }
#endif

    Hunhandle * hun = Hunspell_create(PARAM(0), PARAM(1));
    RETURN_NUMBER((SYS_INT)hun);

#ifndef NO_THREAD_CACHE
    if (!shared) {
        semp(sem);
        shared = hun;
        links = 1;
        affpath = PARAM(0);
        dpath = PARAM(1);
        semv(sem);
    }
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_create_key, 3)
    T_STRING(0)
    T_STRING(1)
    T_STRING(2)
    Hunhandle * hun = Hunspell_create_key(PARAM(0), PARAM(1), PARAM(2));
    RETURN_NUMBER((SYS_INT)hun);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_destroy, 1)
    T_HANDLE(0)
    Hunhandle * hun = (Hunhandle *)(SYS_INT)PARAM(0);
    if (hun) {
#ifndef NO_THREAD_CACHE
        if (hun == shared) {
            semp(sem);
            links--;
            if (!links) {
                Hunspell_destroy(hun);
                shared = NULL;
                affpath = (char *)"";
                dpath = (char *)"";
            }
            semv(sem);
        } else
#endif
        Hunspell_destroy(hun);
    }
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_spell, 2)
    T_HANDLE(0)
    T_STRING(1)
    Hunhandle * hun = (Hunhandle *)(SYS_INT)PARAM(0);
    HUNSPELL_LOCK(hun)
    int res = Hunspell_spell(hun, PARAM(1));
    HUNSPELL_UNLOCK(hun)
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_suggest, 2)
    T_HANDLE(0)
    T_STRING(1)
    CREATE_ARRAY(RESULT);
    Hunhandle *hun    = (Hunhandle *)(SYS_INT)PARAM(0);
    char      **slist = 0;
    HUNSPELL_LOCK(hun)
    int       res     = Hunspell_suggest(hun, &slist, PARAM(1));
    HUNSPELL_UNLOCK(hun)
    if (slist) {
        for (int i = 0; i < res; i++) {
            if (slist[i])
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, slist[i], (double)0);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, "", (double)0);
        }
        Hunspell_free_list(hun, &slist, res);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_get_dic_encoding, 1)
    T_HANDLE(0)
    Hunhandle * hun = (Hunhandle *)(SYS_INT)PARAM(0);
    HUNSPELL_LOCK(hun)
    char *res = Hunspell_get_dic_encoding(hun);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
    HUNSPELL_UNLOCK(hun)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_analyze, 2)
    T_HANDLE(0)
    T_STRING(1)
    CREATE_ARRAY(RESULT);
    Hunhandle *hun    = (Hunhandle *)(SYS_INT)PARAM(0);
    char      **slist = 0;
    HUNSPELL_LOCK(hun)
    int       res     = Hunspell_analyze(hun, &slist, PARAM(1));
    HUNSPELL_UNLOCK(hun)
    if (slist) {
        for (int i = 0; i < res; i++) {
            if (slist[i])
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, slist[i], (double)0);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, "", (double)0);
        }
        Hunspell_free_list(hun, &slist, res);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_stem, 2)
    T_HANDLE(0)
    T_STRING(1)
    CREATE_ARRAY(RESULT);
    Hunhandle *hun    = (Hunhandle *)(SYS_INT)PARAM(0);
    char      **slist = 0;
    HUNSPELL_LOCK(hun)
    int       res     = Hunspell_stem(hun, &slist, PARAM(1));
    HUNSPELL_UNLOCK(hun)
    if (slist) {
        for (int i = 0; i < res; i++) {
            if (slist[i])
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, slist[i], (double)0);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, "", (double)0);
        }
        Hunspell_free_list(hun, &slist, res);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_stem2, 2)
    T_HANDLE(0)
    T_ARRAY(1)
    CREATE_ARRAY(RESULT);
    Hunhandle *hun    = (Hunhandle *)(SYS_INT)PARAM(0);
    char      **slist = 0;
    char      **list  = GetCharList(PARAMETER(1), Invoke);
    int       count   = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    HUNSPELL_LOCK(hun)
    int       res     = Hunspell_stem2(hun, &slist, list, count);
    HUNSPELL_UNLOCK(hun)
    if (slist) {
        for (int i = 0; i < res; i++) {
            if (slist[i])
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, slist[i], (double)0);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, "", (double)0);
        }
        Hunspell_free_list(hun, &slist, res);
    }
    if (list)
        delete[] list;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_generate, 3)
    T_HANDLE(0)
    T_STRING(1)
    T_STRING(2)
    CREATE_ARRAY(RESULT);
    Hunhandle *hun    = (Hunhandle *)(SYS_INT)PARAM(0);
    char      **slist = 0;
    HUNSPELL_LOCK(hun)
    int       res     = Hunspell_generate(hun, &slist, PARAM(1), PARAM(2));
    HUNSPELL_UNLOCK(hun)
    if (slist) {
        for (int i = 0; i < res; i++) {
            if (slist[i])
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, slist[i], (double)0);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, "", (double)0);
        }
        Hunspell_free_list(hun, &slist, res);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_generate2, 3)
    T_HANDLE(0)
    T_STRING(1)
    T_ARRAY(2)
    char **list = GetCharList(PARAMETER(2), Invoke);
    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
    CREATE_ARRAY(RESULT);
    Hunhandle *hun    = (Hunhandle *)(SYS_INT)PARAM(0);
    char      **slist = 0;
    HUNSPELL_LOCK(hun)
    int       res     = Hunspell_generate2(hun, &slist, PARAM(1), list, count);
    HUNSPELL_UNLOCK(hun)
    if (slist) {
        for (int i = 0; i < res; i++) {
            if (slist[i])
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, slist[i], (double)0);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, "", (double)0);
        }
        Hunspell_free_list(hun, &slist, res);
    }
    if (list)
        delete[] list;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_add, 2)
    T_HANDLE(0)
    T_STRING(1)
    Hunhandle * hun = (Hunhandle *)(SYS_INT)PARAM(0);
    HUNSPELL_LOCK(hun)
    int res = Hunspell_add(hun, PARAM(1));
    HUNSPELL_UNLOCK(hun)
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_add_with_affix, 3)
    T_HANDLE(0)
    T_STRING(1)
    T_STRING(2)
    Hunhandle * hun = (Hunhandle *)(SYS_INT)PARAM(0);
    HUNSPELL_LOCK(hun)
    int res = Hunspell_add_with_affix(hun, PARAM(1), PARAM(2));
    HUNSPELL_UNLOCK(hun)
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Hunspell_remove, 2)
    T_HANDLE(0)
    T_STRING(1)
    Hunhandle * hun = (Hunhandle *)(SYS_INT)PARAM(0);
    HUNSPELL_LOCK(hun)
    int res = Hunspell_remove(hun, PARAM(1));
    HUNSPELL_UNLOCK(hun)
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------

