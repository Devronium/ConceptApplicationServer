//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>

#include <libfo/fo-libfo.h>
#include <libfo/libfo-compat.h>
#include <libfo/fo-doc-cairo.h>

INVOKE_CALL InvokePtr = 0;
AnsiString  last_error;

//-----------------------------------------------------------------------------------
static int IsError(GError *error) {
    if (error != NULL) {
        g_critical("%s:: %s",
                   g_quark_to_string(error->domain),
                   error->message);
        last_error  = (char *)g_quark_to_string(error->domain);
        last_error += (char *)":";
        last_error += (char *)error->message;

        g_error_free(error);
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------------
static FoDoc *init_fo_doc_cairo(const gchar *out_file, FoLibfoContext *libfo_context) {
    FoDoc  *fo_doc = NULL;
    GError *error  = NULL;

    fo_doc = fo_doc_cairo_new();
    fo_doc_open_file(fo_doc, out_file, libfo_context, &error);

    if (IsError(error)) {
        if (fo_doc)
            g_object_unref(fo_doc);
        return 0;
    }
    return fo_doc;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    fo_libfo_init();
    DEFINE_ECONSTANT(FO_FLAG_FORMAT_UNKNOWN);
    DEFINE_ECONSTANT(FO_FLAG_FORMAT_AUTO);
    DEFINE_ECONSTANT(FO_FLAG_FORMAT_PDF);
    DEFINE_ECONSTANT(FO_FLAG_FORMAT_POSTSCRIPT);
    DEFINE_ECONSTANT(FO_FLAG_FORMAT_SVG);

    /*DEFINE_ECONSTANT(FO_ENUM_FORMAT_AUTO);
       DEFINE_ECONSTANT(FO_ENUM_FORMAT_UNKNOWN);
       DEFINE_ECONSTANT(FO_ENUM_FORMAT_PDF);
       DEFINE_ECONSTANT(FO_ENUM_FORMAT_POSTSCRIPT);
       DEFINE_ECONSTANT(FO_ENUM_FORMAT_SVG);*/
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    fo_libfo_shutdown();
    return 0;
}
//------------------------------------------------------------------------
// xmlfile, outfile, [format_mode, xsltfile, compat, validation, continue_after_error]
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FOTransform, 2, 7)
    T_STRING(0)
    T_STRING(1)

    FoFlagsFormat format_mode = FO_FLAG_FORMAT_AUTO; //FO_ENUM_FORMAT_UNKNOWN;
    FoWarningFlag warning_mode = FO_WARNING_NONE;    //FO_WARNING_FO | FO_WARNING_PROPERTY;

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        format_mode = (FoFlagsFormat)PARAM_INT(2);
    }

    char validation = 0;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(5)
        validation = (char)PARAM_INT(5);
    }
    char continue_after_error = true;
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(6)
        continue_after_error = (char)PARAM_INT(6);
    }
    char compat = false;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4)
        compat = (char)PARAM_INT(4);
    }

    gchar *out_file  = PARAM(1); //"layout.pdf";
    gchar *xslt_file = NULL;
    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        xslt_file = PARAM(3);
        if (!PARAM_LEN(3))
            xslt_file = 0;
    }
    gchar          *xml_file    = PARAM(0);
    FoXmlDoc       *xml_doc     = NULL;
    FoXmlDoc       *result_tree = NULL;
    FoXslFormatter *fo_xsl_formatter;
    GError         *gerror = NULL;
    FoDoc          *fo_doc = NULL;

    FoLibfoContext *libfo_context = fo_libfo_context_new();

    fo_libfo_context_set_warning_mode(libfo_context, warning_mode);

    fo_libfo_context_set_validation(libfo_context, validation);
    fo_libfo_context_set_continue_after_error(libfo_context, continue_after_error);
    fo_libfo_context_set_format(libfo_context, format_mode);
    fo_doc = init_fo_doc_cairo(out_file, libfo_context);

    if (!fo_doc) {
        RETURN_NUMBER(-1);
        return 0;
    }

    if (xslt_file != NULL) {
        xml_doc = fo_xml_doc_new_from_filename(xml_file, libfo_context, &gerror);

        if (IsError(gerror)) {
            RETURN_NUMBER(-2);
            return 0;
        }

        FoXmlDoc *stylesheet_doc = fo_xml_doc_new_from_filename(xslt_file, libfo_context, &gerror);

        if (IsError(gerror)) {
            RETURN_NUMBER(-3);
            return 0;
        }

        result_tree = fo_xslt_transformer_do_transform(xml_doc, stylesheet_doc, &gerror);
        fo_xml_doc_unref(xml_doc);

        if (IsError(gerror)) {
            RETURN_NUMBER(-4);
            return 0;
        }
    } else {
        result_tree = fo_xml_doc_new_from_filename(xml_file, libfo_context, &gerror);
        if (IsError(gerror)) {
            RETURN_NUMBER(-2);
            return 0;
        }
    }

    if (compat) {
        FoXmlDoc *old_result_tree = result_tree;
        // Remove or rewrite what libfo can't yet handle.
        result_tree = libfo_compat_make_compatible(old_result_tree, libfo_context, &gerror);
        fo_xml_doc_unref(old_result_tree);
        if (IsError(gerror)) {
            RETURN_NUMBER(-5);
            return 0;
        }
    }

    fo_xsl_formatter = fo_xsl_formatter_new();
    fo_xsl_formatter_set_result_tree(fo_xsl_formatter, result_tree);
    fo_xsl_formatter_set_fo_doc(fo_xsl_formatter, fo_doc);
    fo_xsl_formatter_format(fo_xsl_formatter, libfo_context, &gerror);

    if (IsError(gerror)) {
        g_object_unref(fo_xsl_formatter);
        g_object_unref(fo_doc);
        RETURN_NUMBER(-6);
        return 0;
    }

    fo_xsl_formatter_draw(fo_xsl_formatter, libfo_context, &gerror);

    if (IsError(gerror)) {
        g_object_unref(fo_xsl_formatter);
        g_object_unref(fo_doc);
        RETURN_NUMBER(-7);
        return 0;
    }

    g_object_unref(fo_xsl_formatter);
    g_object_unref(fo_doc);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FOError, 0)
    RETURN_STRING(last_error.c_str())
END_IMPL
//------------------------------------------------------------------------
// xmlfile, outfile, [format_mode, xsltfile, compat, validation, continue_after_error]
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FOTransformString, 2, 7)
    T_STRING(0)
    T_STRING(1)

    FoFlagsFormat format_mode = FO_FLAG_FORMAT_AUTO; //FO_ENUM_FORMAT_UNKNOWN;
    FoWarningFlag warning_mode = FO_WARNING_NONE;    //FO_WARNING_FO | FO_WARNING_PROPERTY;

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        format_mode = (FoFlagsFormat)PARAM_INT(2);
    }

    char validation = 0;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(5)
        validation = (char)PARAM_INT(5);
    }
    char continue_after_error = true;
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(6)
        continue_after_error = (char)PARAM_INT(6);
    }
    char compat = false;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4)
        compat = (char)PARAM_INT(4);
    }

    gchar *out_file  = PARAM(1); //"layout.pdf";
    gchar *xslt_file = NULL;
    int   xslt_len   = 0;
    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        xslt_file = PARAM(3);
        xslt_len  = PARAM_LEN(3);
        if (!xslt_len)
            xslt_file = 0;
    }
//gchar *xml_file = PARAM(0);
    FoXmlDoc       *xml_doc     = NULL;
    FoXmlDoc       *result_tree = NULL;
    FoXslFormatter *fo_xsl_formatter;
    GError         *gerror = NULL;
    FoDoc          *fo_doc = NULL;

    FoLibfoContext *libfo_context = fo_libfo_context_new();

    fo_libfo_context_set_warning_mode(libfo_context, warning_mode);

    fo_libfo_context_set_validation(libfo_context, validation);
    fo_libfo_context_set_continue_after_error(libfo_context, continue_after_error);
    fo_libfo_context_set_format(libfo_context, format_mode);
    fo_doc = init_fo_doc_cairo(out_file, libfo_context);

    if (!fo_doc) {
        RETURN_NUMBER(-1);
        return 0;
    }

    if (xslt_file != NULL) {
        //xml_doc = fo_xml_doc_new_from_filename (xml_file, libfo_context, &gerror);
        xml_doc = fo_xml_doc_new_from_memory(PARAM(0), PARAM_LEN(0), "input.fo", "UTF-8", libfo_context, &gerror);

        if (IsError(gerror)) {
            RETURN_NUMBER(-2);
            return 0;
        }

        //FoXmlDoc *stylesheet_doc = fo_xml_doc_new_from_filename (xslt_file, libfo_context, &gerror);
        FoXmlDoc *stylesheet_doc = fo_xml_doc_new_from_memory(xslt_file, xslt_len, "input.xslt", "UTF-8", libfo_context, &gerror);

        if (IsError(gerror)) {
            RETURN_NUMBER(-3);
            return 0;
        }

        result_tree = fo_xslt_transformer_do_transform(xml_doc, stylesheet_doc, &gerror);
        fo_xml_doc_unref(xml_doc);

        if (IsError(gerror)) {
            RETURN_NUMBER(-4);
            return 0;
        }
    } else {
        //result_tree = fo_xml_doc_new_from_filename (xml_file, libfo_context, &gerror);
        result_tree = fo_xml_doc_new_from_memory(PARAM(0), PARAM_LEN(0), "input.fo", "UTF-8", libfo_context, &gerror);

        if (IsError(gerror)) {
            RETURN_NUMBER(-2);
            return 0;
        }
    }

    if (compat) {
        FoXmlDoc *old_result_tree = result_tree;
        // Remove or rewrite what libfo can't yet handle.
        result_tree = libfo_compat_make_compatible(old_result_tree, libfo_context, &gerror);
        fo_xml_doc_unref(old_result_tree);
        if (IsError(gerror)) {
            RETURN_NUMBER(-5);
            return 0;
        }
    }

    fo_xsl_formatter = fo_xsl_formatter_new();
    fo_xsl_formatter_set_result_tree(fo_xsl_formatter, result_tree);
    fo_xsl_formatter_set_fo_doc(fo_xsl_formatter, fo_doc);
    fo_xsl_formatter_format(fo_xsl_formatter, libfo_context, &gerror);

    if (IsError(gerror)) {
        g_object_unref(fo_xsl_formatter);
        g_object_unref(fo_doc);
        RETURN_NUMBER(-6);
        return 0;
    }

    fo_xsl_formatter_draw(fo_xsl_formatter, libfo_context, &gerror);

    if (IsError(gerror)) {
        g_object_unref(fo_xsl_formatter);
        g_object_unref(fo_doc);
        RETURN_NUMBER(-7);
        return 0;
    }

    g_object_unref(fo_xsl_formatter);
    g_object_unref(fo_doc);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
