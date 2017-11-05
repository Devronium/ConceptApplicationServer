//#define GO_DEBUG
//#define WITH_LIBXML2

//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CFISH_USE_SHORT_NAMES
#define LUCY_USE_SHORT_NAMES

#include "Clownfish/String.h"
#include "Clownfish/Vector.h"
#include "Lucy/Simple.h"
#include "Lucy/Document/Doc.h"
#include "Lucy/Document/HitDoc.h" 
#include "Lucy/Analysis/EasyAnalyzer.h"
#include "Lucy/Index/Indexer.h"
#include "Lucy/Plan/FullTextType.h"
#include "Lucy/Plan/StringType.h"
#include "Lucy/Plan/BlobType.h"
#include "Lucy/Plan/Schema.h" 
#include "Lucy/Search/QueryParser.h"
#include "Lucy/Search/Query.h"

#define LUCY_TEXT       0
#define LUCY_STRING     4
#define LUCY_BLOB       8

#define LUCY_NO_INDEX   1
#define LUCY_NO_STORE   2

struct caller_context {
    void *input;
    void *input2;
    void *input3;
    void *output;
};

void stemmer_trap(void *context) {
    struct caller_context *ctx = (struct caller_context *)context;
    ctx->output  = EasyAnalyzer_new((String *)ctx->input);
}

void indexer_add_trap(void *context) {
    struct caller_context *ctx = (struct caller_context *)context;
    Indexer_Add_Doc((Indexer *)ctx->input, (Doc *)ctx->input2, *(NUMBER *)ctx->input3);
}

void indexer_new_trap(void *context) {
    struct caller_context *ctx = (struct caller_context *)context;
    ctx->output = Indexer_new((Schema *)ctx->input, (Obj *)ctx->input2, NULL, Indexer_CREATE);
}

void indexer_commit_trap(void *context) {
    Indexer_Commit((Indexer *)context);
}

void indexer_optimize_trap(void *context) {
    Indexer_Optimize((Indexer *)context);
}

void indexer_remove_trap(void *context) {
    struct caller_context *ctx = (struct caller_context *)context;
    Indexer_Delete_By_Doc_ID((Indexer *)ctx->input, (SYS_INT)ctx->input2);
}

void string_trap(void *context) {
    struct caller_context *ctx = (struct caller_context *)context;
    ctx->output = Str_newf("%s", (char *)ctx->input);
}

void utf8_trap(void *context) {
    struct caller_context *ctx = (struct caller_context *)context;
    ctx->output = Str_new_from_utf8((char *)ctx->input, (int)(uintptr_t)ctx->input2);
}

void simple_search_trap(void *context) {
    struct caller_context *ctx = (struct caller_context *)context;
    Simple_Search((Simple *)ctx->input, (String *)ctx->input2, (int)(uintptr_t)ctx->input3, (int)(uintptr_t)ctx->output, NULL);
}

void simple_new_trap(void *context) {
    struct caller_context *ctx = (struct caller_context *)context;
    ctx->output = Simple_new((Obj *)ctx->input, (String *)ctx->input2);
}

void show_and_release_error(Err *err) {
    String *message = Err_Get_Mess(err);
    char *utf8 = Str_To_Utf8(message);
    fprintf(stderr, "%s", utf8);
    free(utf8);
    DECREF(err);
}

String *safe_string(const char *str) {
    struct caller_context ctx = {(void *)str, NULL, NULL, NULL};
    Err *err = Err_trap(string_trap, &ctx);
    if (err) {
        show_and_release_error(err);
        return Str_newf("");
    }
    return (String *)ctx.output;
}

String *utf8_string(const char *str, int len) {
    struct caller_context ctx = {(void *)str, (void *)(uintptr_t)len, NULL, NULL};
    Err *err = Err_trap(utf8_trap, &ctx);
    if (err) {
        show_and_release_error(err);
        return Str_newf("");
    }
    return (String *)ctx.output;
}

void indexer_remove_query_trap(void *context) {
    struct caller_context *ctx = (struct caller_context *)context;
    Schema *schema = Indexer_Get_Schema((Indexer *)ctx->input);
    QueryParser *qparser  = QParser_new(schema, NULL, NULL, NULL);
    String *query_str = safe_string((char *)ctx->input2);
    Query *query = QParser_Parse(qparser, query_str); 
    Indexer_Delete_By_Query((Indexer *)ctx->input, query);
    DECREF(query);
    DECREF(qparser);
    DECREF(query_str);
    // do not decrement link count for schema (it crashes)
    // DECREF(schema);
}



CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    lucy_bootstrap_parcel();
    DEFINE_SCONSTANT("LUCY_TEXT",       "0")
    DEFINE_SCONSTANT("LUCY_STRING",     "4")
    DEFINE_SCONSTANT("LUCY_BLOB",       "8")
    DEFINE_SCONSTANT("LUCY_NO_INDEX",   "1")
    DEFINE_SCONSTANT("LUCY_NO_STORE",   "2")
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(lucy_new, 1, 2)
    T_STRING(lucy_new, 0)
    const char *language = "en";
    if (PARAMETERS_COUNT > 1) {
        T_STRING(lucy_new, 1)
        if (PARAM_LEN(1))
            language = PARAM(1);
    }
    String *use_folder   = safe_string(PARAM(0));
    String *use_language = safe_string(language);

    struct caller_context ctx = {use_folder, use_language, NULL, NULL};
    Err *err = Err_trap(simple_new_trap, &ctx);
    Simple *lucy = NULL;
    if (err)
        show_and_release_error(err);
    else
        lucy = (Simple *)ctx.output;

    DECREF(use_language);
    DECREF(use_folder);
    RETURN_NUMBER((SYS_INT)lucy);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(lucy_indexer_new, 2, 3)
    T_ARRAY(lucy_indexer_new, 0)
    T_STRING(lucy_indexer_new, 1)
    const char *language = "en";
    if (PARAMETERS_COUNT > 2) {
        T_STRING(lucy_new, 2)
        if (PARAM_LEN(2))
            language = PARAM(2);
    }
    String *use_language    = safe_string(language);
    struct caller_context ctx = {use_language, NULL, NULL, NULL};
    Err *err = Err_trap(stemmer_trap, &ctx);
    if (err) {
        DECREF(use_language);
        show_and_release_error(err);
        RETURN_NUMBER(0);
        return 0;
    }
    EasyAnalyzer *analyzer = (EasyAnalyzer *)ctx.output;
    String *use_folder      = safe_string(PARAM(1));
    Schema *use_schema      = Schema_new();

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));
    if (count > 0) {
        char **keys = (char **)malloc(count * sizeof(char *));
        Invoke(INVOKE_ARRAY_KEYS, PARAMETER(0), keys, (INTEGER)count);

        Doc *doc = Doc_new(NULL, 0);
        int elements = 0;
        char buffer[0xFF];
        int len;

        for (INTEGER i = 0; i < count; i++) {
            char *key = keys[i];
            if (key) {
                void *var = NULL;
                Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(0), i, &var);
                if (!var)
                    continue;

                String *field_str = safe_string(key);
                char    *szData;
                INTEGER type;
                NUMBER  nData;
                int flags = 0;
                Invoke(INVOKE_GET_VARIABLE, var, &type, &szData, &nData);
                FieldType *field_type = NULL;
                if (type == VARIABLE_NUMBER) {
                    flags = (int)nData;
                    int lucy_type = (flags >> 2) & 3;
                    int boost = flags >> 5;
                    switch (lucy_type) {
                        case 1:
                            field_type = (FieldType *)StringType_new();
                            if (flags & LUCY_NO_INDEX)
                                StringType_Set_Indexed((StringType *)field_type, false);

                            if (flags & LUCY_NO_STORE)
                                StringType_Set_Stored((StringType *)field_type, false);
                            if (boost)
                                StringType_Set_Boost((StringType *)field_type, (float)boost / 1000);
                            break;
                        case 2:
                            field_type = (FieldType *)BlobType_new(flags && 0x02);
                            break;
                        default:
                            field_type = (FieldType *)FullTextType_new((Analyzer*)analyzer);
                            if (flags & LUCY_NO_INDEX)
                                FullTextType_Set_Indexed((FullTextType *)field_type, false);

                            if (flags & LUCY_NO_STORE)
                                FullTextType_Set_Stored((FullTextType *)field_type, false);
                            if (boost)
                                FullTextType_Set_Boost((FullTextType *)field_type, (float)boost / 1000);
                    }
                    if (field_type) {
                        Schema_Spec_Field(use_schema, field_str, field_type);
                    }
                }
                if (field_type)
                    DECREF(field_type);
                DECREF(field_str);
            }
        }
    }
    struct caller_context context = {use_schema, use_folder, NULL, NULL};
    err = Err_trap(indexer_new_trap, &context);
    Indexer *lucy = NULL;
    if (err)
        show_and_release_error(err);
    else
        lucy = (Indexer *)context.output;

    DECREF(use_language);
    DECREF(use_folder);
    DECREF(use_schema);
    DECREF(analyzer);
    RETURN_NUMBER((SYS_INT)lucy);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lucy_indexer_done, 1)
    T_NUMBER(lucy_indexer_done, 0);
    Indexer *lucy = (Indexer *)(SYS_INT)PARAM(0);
    if (lucy) {
        DECREF(lucy);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(lucy_indexer_add, 2, 3)
    T_HANDLE(lucy_indexer_add, 0);
    T_ARRAY(lucy_indexer_add, 1);
    NUMBER rank = 1.0;
    Indexer *lucy = (Indexer *)(SYS_INT)PARAM(0);
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(lucy_indexer_add, 2);
        rank = PARAM(2);
    }
    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (count <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }
    char **keys = (char **)malloc(count * sizeof(char *));
    if (!keys) {
        RETURN_NUMBER(0);
        return 0;
    }
    Invoke(INVOKE_ARRAY_KEYS, PARAMETER(1), keys, (INTEGER)count);

    Doc *doc = Doc_new(NULL, 0);
    int elements = 0;
    char buffer[0xFF];
    int len;

    for (INTEGER i = 0; i < count; i++) {
        char *key = keys[i];
        if (key) {
            void *var = NULL;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &var);
            if (var) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, var, &type, &szData, &nData);
                String *field = NULL;
                String *value = NULL;
                switch (type) {
                    case VARIABLE_STRING:
                        field = safe_string(key);
                        value = utf8_string(szData, (int)nData);
                        Doc_Store(doc, field, (Obj*)value);
                        elements++;
                        break;
                    case VARIABLE_NUMBER:
                        sprintf(buffer, "%.30g", nData);
                        len = strlen(buffer);
                        if (((len > 1) && ((buffer[len - 1] == '.') || (buffer[len - 1] == ',')))) {
                            len--;
                            buffer[len] = 0; 
                        }
                        field = safe_string(key);
                        value = utf8_string(buffer, len);
                        Doc_Store(doc, field, (Obj*)value);
                        elements++;
                        break;
                }
                if (field)
                    DECREF(field);
                if (value)
                    DECREF(value);
            }
        }
    }
    free(keys);
    if (elements) {
        struct caller_context ctx = {lucy, doc, &rank, NULL};
        Err *err = Err_trap(indexer_add_trap, &ctx);
        if (err) {
            show_and_release_error(err);
            elements = 0;
        }
    }
    DECREF(doc);
    RETURN_NUMBER(elements);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lucy_indexer_remove, 2)
    T_HANDLE(lucy_indexer_remove, 0);
    T_NUMBER(lucy_indexer_remove, 1);
    Indexer *lucy = (Indexer *)(SYS_INT)PARAM(0);

    struct caller_context ctx = {lucy, (void *)(SYS_INT)PARAM(1), NULL, NULL};
    Err *err = Err_trap(indexer_remove_trap, &ctx);
    if (err) {
        show_and_release_error(err);
        RETURN_NUMBER(-1);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lucy_indexer_remove_query, 2)
    T_HANDLE(lucy_indexer_remove_query, 0);
    T_STRING(lucy_indexer_remove_query, 1);
    Indexer *lucy = (Indexer *)(SYS_INT)PARAM(0);

    if (PARAM_LEN(1) <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }
    struct caller_context ctx = {lucy, (void *)PARAM(1), NULL, NULL};
    Err *err = Err_trap(indexer_remove_query_trap, &ctx);
    if (err) {
        show_and_release_error(err);
        RETURN_NUMBER(-1);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lucy_indexer_commit, 1)
    T_HANDLE(lucy_indexer_commit, 0);
    Indexer *lucy = (Indexer *)(SYS_INT)PARAM(0);
    Err *err = Err_trap(indexer_commit_trap, lucy);
    if (err) {
        show_and_release_error(err);
        RETURN_NUMBER(-1);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lucy_indexer_optimize, 1)
    T_HANDLE(lucy_indexer_optimize, 0);
    Indexer *lucy = (Indexer *)(SYS_INT)PARAM(0);
    Err *err = Err_trap(indexer_optimize_trap, lucy);
    if (err) {
        show_and_release_error(err);
        RETURN_NUMBER(-1);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lucy_done, 1)
    T_NUMBER(lucy_done, 0);
    Simple *lucy = (Simple *)(SYS_INT)PARAM(0);
    if (lucy) {
        DECREF(lucy);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lucy_add, 2)
    T_HANDLE(lucy_add, 0);
    T_ARRAY(lucy_add, 1);
    Simple *lucy = (Simple *)(SYS_INT)PARAM(0);

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (count <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }
    char **keys = (char **)malloc(count * sizeof(char *));
    if (!keys) {
        RETURN_NUMBER(0);
        return 0;
    }
    Invoke(INVOKE_ARRAY_KEYS, PARAMETER(1), keys, (INTEGER)count);

    Doc *doc = Doc_new(NULL, 0);
    int elements = 0;
    char buffer[0xFF];
    int len;

    for (INTEGER i = 0; i < count; i++) {
        char *key = keys[i];
        if (key) {
            void *var = NULL;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &var);
            if (var) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, var, &type, &szData, &nData);
                String *field = NULL;
                String *value = NULL;
                switch (type) {
                    case VARIABLE_STRING:
                        field = safe_string(key);
                        value = utf8_string(szData, (int)nData);
                        Doc_Store(doc, field, (Obj*)value);
                        elements++;
                        break;
                    case VARIABLE_NUMBER:
                        sprintf(buffer, "%.30g", nData);
                        len = strlen(buffer);
                        if (((len > 1) && ((buffer[len - 1] == '.') || (buffer[len - 1] == ',')))) {
                            len--;
                            buffer[len] = 0; 
                        }
                        field = safe_string(key);
                        value = utf8_string(buffer, len);
                        Doc_Store(doc, field, (Obj*)value);
                        elements++;
                        break;
                }
                if (field)
                    DECREF(field);
                if (value)
                    DECREF(value);
            }
        }
    }
    free(keys);
    if (elements)
        Simple_Add_Doc(lucy, doc);
    DECREF(doc);
    RETURN_NUMBER(elements);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(lucy_search, 2, 4)
    T_HANDLE(lucy_search, 0);
    T_STRING(lucy_search, 1);
    Simple *lucy = (Simple *)(SYS_INT)PARAM(0);
    uint32_t offset = 0;
    uint32_t num_wanted = 10;
    CREATE_ARRAY(RESULT);
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(lucy_search, 2)
        offset = PARAM_INT(2);
        if (offset < 0)
            offset = 0;
    }
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(lucy_search, 3)
        num_wanted = PARAM_INT(3);
        if (num_wanted <= 0)
            num_wanted = 10;
    }
    String *query_str = safe_string(PARAM(1));
    struct caller_context ctx = {lucy, query_str, (void *)(uintptr_t)offset, (void *)(uintptr_t)num_wanted};
    Err *err = Err_trap(simple_search_trap, &ctx);
    if (err) {
        show_and_release_error(err);
        DECREF(query_str);
        return 0;
    }
    // Simple_Search(lucy, query_str, offset, num_wanted, NULL);

    HitDoc *hit;
    INTEGER index = 0;
    while (NULL != (hit = Simple_Next(lucy))) {
            void *elem_data;
            Invoke(INVOKE_ARRAY_VARIABLE, RESULT, index, &elem_data);
            if (elem_data) {
                CREATE_ARRAY(elem_data);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem_data, "$doc_id", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)HitDoc_Get_Doc_ID(hit));
                uint32_t size = HitDoc_Get_Size(hit);
                if (size) {
                    cfish_Vector *names = HitDoc_Field_Names(hit);
                    for (INTEGER i = 0; i < size; i++) {
                        String *key = (String *)Vec_Fetch(names, i);
                        if (key) {
                            String *val = (String*)HitDoc_Extract(hit, key);
                            if (val) {
                                char *key_utf = Str_To_Utf8(key);
                                char *val_utf = Str_To_Utf8(val);
                                if (val_utf) {
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem_data, key_utf, (INTEGER)VARIABLE_STRING, (char *)val_utf, (NUMBER)0);
                                    free(val_utf);
                                }
                                free(key_utf);
                                DECREF(val);
                            }
                            DECREF(key);
                        }
                    }
                    DECREF(names);
                }
            }
            index++;
            DECREF(hit);
    }
    DECREF(query_str);
END_IMPL
//------------------------------------------------------------------------
