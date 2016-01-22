//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "Templatizer.h"
#include "TemplateElement.h"
#include "AnsiString.h"
#include "AnsiList.h"

//AnsiList Templatizers;

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_NewTemplateContext CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "NewTemplateContext takes no parameters");

    LOCAL_INIT;

    CTemplatizer *T = new CTemplatizer(Invoke);
    //Templatizers.Add(new CTemplatizer(Invoke),DATA_TEMPLATIZER);
    RETURN_NUMBER((SYS_INT)T);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_DestroyTemplateContext CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "DestroyTemplateContext takes one parameter");

    LOCAL_INIT;

    NUMBER index = 0;

    //if (PARAMETERS_COUNT) {
    GET_CHECK_NUMBER(0, index, "DestroyTemplateContext: parameter 0 should be a connection idetifier");
    //}

    //index--;
    //if ((index<0) || (index>Templatizers.Count()))
    //    return (void *)"DestroyTemplateContext: invalid template context id";
    if (!index)
        return (void *)"DestroyTemplateContext: invalid template context id";

    delete (CTemplatizer *)(SYS_INT)index;
    RETURN_NUMBER(0)
    //RETURN_NUMBER(Templatizers.Delete((int)index));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_TBind CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "TBind(template_var_name, concept_variable, template_handle)");

    LOCAL_INIT;

    NUMBER index = 0;
    char   *var_name;

    GET_CHECK_STRING(0, var_name, "TBind: parameter 0 should be a name for the template variable");


    //if (PARAMETERS_COUNT==3) {
    GET_CHECK_NUMBER(2, index, "TBind: parameter 2 should be a connection idetifier");
    //}

    /*index--;
       if ((index<0) || (index>Templatizers.Count()))
        return (void *)"TBind: invalid template context id";*/
    if (!index)
        return (void *)"TBind: invalid template context id";

    CTemplatizer *T = (CTemplatizer *)(SYS_INT)index;

    RETURN_NUMBER(T->BindVariable(var_name, LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1]));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_TUnBind CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "TUnBind(concept_variable , template_handle)");

    LOCAL_INIT;

    NUMBER index = 0;
    char   *var_name;

    //if (PARAMETERS_COUNT==2) {
    GET_CHECK_NUMBER(1, index, "TUnBind: parameter 1 should be a connection idetifier");
    //}

    /*index--;
       if ((index<0) || (index>Templatizers.Count()))
        return (void *)"TUnBind: invalid template context id";

       CTemplatizer *T=(CTemplatizer *)Templatizers.Item((int)index);*/
    if (!index)
        return (void *)"TUnBind: invalid template context id";

    CTemplatizer *T = (CTemplatizer *)(SYS_INT)index;

    RETURN_NUMBER(T->UnbindVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1]));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_TCompile CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "TCompile(template_content , template_handle)");

    LOCAL_INIT;

    NUMBER index = 0;
    char   *tpl;

    GET_CHECK_STRING(0, tpl, "TCompile: parameter 0 should be a string (static string)");


    //if (PARAMETERS_COUNT==2) {
    GET_CHECK_NUMBER(1, index, "TCompile: parameter 1 should be a connection idetifier");
    //}

    /*index--;
       if ((index<0) || (index>Templatizers.Count()))
        return (void *)"TCompile: invalid template context id";

       CTemplatizer *T=(CTemplatizer *)Templatizers.Item((int)index);*/
    if (!index)
        return (void *)"CONCEPT_TCompile: invalid template context id";

    CTemplatizer *T = (CTemplatizer *)(SYS_INT)index;

    RETURN_NUMBER(T->Compile(tpl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_TExecute CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "TExecute(template_handle)");

    LOCAL_INIT;

    NUMBER index = 0;

    //if (PARAMETERS_COUNT==1) {
    GET_CHECK_NUMBER(0, index, "TExecute: parameter 0 should be a connection idetifier");
    //}

    /*index--;
       if ((index<0) || (index>Templatizers.Count()))
        return (void *)"TExecute: invalid template context id";

       CTemplatizer *T=(CTemplatizer *)Templatizers.Item((int)index);*/
    if (!index)
        return (void *)"TExecute: invalid template context id";

    CTemplatizer *T = (CTemplatizer *)(SYS_INT)index;

    RETURN_STRING(T->Execute());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_TErrors CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "TErrors(template_handle)");

    LOCAL_INIT;

    NUMBER index = 0;

    //if (PARAMETERS_COUNT==1) {
    GET_CHECK_NUMBER(0, index, "TErrors: parameter 0 should be a connection idetifier");
    //}

    /*index--;
       if ((index<0) || (index>Templatizers.Count()))
        return (void *)"TErrors: invalid template context id";

       CTemplatizer *T=(CTemplatizer *)Templatizers.Item((int)index);*/
    if (!index)
        return (void *)"TErrors: invalid template context id";

    CTemplatizer *T = (CTemplatizer *)(SYS_INT)index;

    RETURN_STRING(T->GetErrors());
    return 0;
}
//---------------------------------------------------------------------------
