//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <wkhtmltox/pdf.h>
#include <wkhtmltox/image.h>
//-------------------------//
// Local variables         //
//-------------------------//
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    wkhtmltopdf_init(false);
    wkhtmltoimage_init(false);
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    wkhtmltopdf_deinit();
    wkhtmltoimage_deinit();
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WKPDF, 1, 3)
    T_STRING(0)
    wkhtmltopdf_global_settings * gs;
    wkhtmltopdf_object_settings *os;
    wkhtmltopdf_converter       *c;

    gs = wkhtmltopdf_create_global_settings();
    if (PARAMETERS_COUNT > 1) {
        T_ARRAY(1)

        int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
        AnsiString val;
        for (int i = 0; i < count; i++) {
            void *newpData = NULL;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING)
                    val = AnsiString(szData);
                else
                    val = AnsiString(nData);

                char *key = NULL;
                Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(1), i, &key);
                if ((key) && (key[0]))
                    wkhtmltopdf_set_global_setting(gs, key, val.c_str());
            }
        }
    }
//wkhtmltopdf_set_global_setting(gs, "load.cookieJar", "myjar.jar");
    os = wkhtmltopdf_create_object_settings();
    wkhtmltopdf_set_object_setting(os, "page", PARAM(0));
    if (PARAMETERS_COUNT > 2) {
        T_ARRAY(2)

        int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
        AnsiString val;
        for (int i = 0; i < count; i++) {
            void *newpData = NULL;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING)
                    val = AnsiString(szData);
                else
                    val = AnsiString(nData);

                char *key = NULL;
                Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(2), i, &key);
                if ((key) && (key[0]))
                    wkhtmltopdf_set_object_setting(os, key, val.c_str());
            }
        }
    }
    c = wkhtmltopdf_create_converter(gs);
    wkhtmltopdf_add_object(c, os, NULL);
    int res = wkhtmltopdf_convert(c);
    if (res) {
        const unsigned char *data = NULL;
        unsigned long       len   = wkhtmltopdf_get_output(c, &data);
        if (len > 0) {
            RETURN_BUFFER((char *)data, len);
        } else {
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
    wkhtmltopdf_destroy_object_settings(os);
    wkhtmltopdf_destroy_converter(c);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WKImg, 1, 2)
    T_STRING(0)
    wkhtmltoimage_global_settings * gs;
    wkhtmltoimage_converter *c;

    gs = wkhtmltoimage_create_global_settings();
    wkhtmltoimage_set_global_setting(gs, "in", PARAM(0));
    wkhtmltoimage_set_global_setting(gs, "fmt", "png");
    if (PARAMETERS_COUNT > 1) {
        T_ARRAY(1)

        int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
        AnsiString val;
        for (int i = 0; i < count; i++) {
            void *newpData = NULL;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING)
                    val = AnsiString(szData);
                else
                    val = AnsiString(nData);

                char *key = NULL;
                Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(1), i, &key);
                if ((key) && (key[0]))
                    wkhtmltoimage_set_global_setting(gs, key, val.c_str());
            }
        }
    }
    c = wkhtmltoimage_create_converter(gs, NULL);
    int res = wkhtmltoimage_convert(c);
    if (res) {
        const unsigned char *data = NULL;
        unsigned long       len   = wkhtmltoimage_get_output(c, &data);
        if (len > 0) {
            RETURN_BUFFER((char *)data, len);
        } else {
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
    wkhtmltoimage_destroy_converter(c);
END_IMPL
//-----------------------------------------------------//
