//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"

#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxml/xmlreader.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/uri.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/extensions.h>
#include <libxslt/xsltInternals.h>
#include <stdint.h>

#include <map>
#include <string>

#ifdef _WIN32
xmlFreeFunc xmlFree = 0;
#endif

#define MAX_OBJECTS    0xFFFF

std::map<std::string, void *> CLASS_OBJECTS;

void *BACK_REFERENCES[MAX_OBJECTS];
void *BACK_REFERENCES2[MAX_OBJECTS];
char BACK_TYPES[MAX_OBJECTS];
int  BACK_REF_COUNT;

void Serialize(void *pData, xmlNodePtr parent, char is_simple = 0, char is_simple_array_element = 0);
void SerializeVariable(char *member, int type, char *szData, NUMBER nData, void *class_data, void *variable_data, xmlNodePtr parent, char is_simple = 0);
void SerializeArray(void *pData, void *arr_id, xmlNodePtr parent, char is_simple = 0);
void DoObject(void *pData, void *parent);
void DoArray(void *pData, void *parent);


#ifdef GO_DEBUG
 #include <iostream>
 #define DEBUG(data)                   std::cout << (data) << "\n"; std::cout.flush();
 #define DEBUG2(data, data2)           std::cout << (data) << ": " << (data2) << "\n"; std::cout.flush();
 #define DEBUG3(data, data2, data3)    std::cout << (data) << ": " << (data2) << ", " << (data3) << "\n"; std::cout.flush();
#else
 #define DEBUG(data)
 #define DEBUG2(data, data1)
 #define DEBUG3(data, data1, data2)
#endif
#define RESET_ERROR

xmlNodePtr             root           = 0;
INVOKE_CALL            InvokePtr      = 0;
CALL_BACK_VARIABLE_SET SetVariablePtr = 0;
CALL_BACK_VARIABLE_GET GetVariablePtr = 0;
xmlDocPtr ref_doc = 0;

//---------------------------------------------------------------------------
int CheckBack(void *pData) {
    // check if a reference is already serialized (anti-cyclic reference)
    for (int i = 0; i < BACK_REF_COUNT; i++) {
        if (BACK_REFERENCES[i] == pData)
            return i + 1;
    }
    if (BACK_REF_COUNT < MAX_OBJECTS)
        BACK_REFERENCES[BACK_REF_COUNT++] = pData;
    return 0;
}

//---------------------------------------------------------------------------
void SerializeArray(void *pData, void *arr_id, xmlNodePtr parent, char is_simple) {
    void       *newpData;
    char       *key;
    xmlNodePtr node;
    xmlNodePtr node2;

    if (!is_simple) {
        int ref_ptr = CheckBack(arr_id);
        if (ref_ptr) {
            DEBUG("...backreferenced array");
            node = xmlNewNode(NULL, BAD_CAST "cyclic_reference");
            xmlNewProp(node, BAD_CAST "refID", BAD_CAST AnsiString((long)ref_ptr).c_str());

            xmlAddChild(parent, node);
            return;
        }
    }


    DEBUG2("           array", arr_id);
    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, pData);

    if (is_simple) {
        node = parent;
        if (!node) {
            node = xmlNewNode(NULL, BAD_CAST "array");
            root = node;
        }
    } else {
        node = xmlNewNode(NULL, BAD_CAST "array");
        xmlNewProp(node, BAD_CAST "cycid", BAD_CAST AnsiString((long)BACK_REF_COUNT).c_str());
        xmlAddChild(parent, node);
    }

    for (int i = 0; i < count; i++) {
        newpData = 0;
        key      = 0;
        InvokePtr(INVOKE_ARRAY_VARIABLE, pData, i, &newpData);
        InvokePtr(INVOKE_GET_ARRAY_KEY, pData, i, &key);
        if (newpData) {
            node2 = xmlNewNode(NULL, BAD_CAST "element");
            xmlAddChild(node, node2);

            if (key)
                xmlNewProp(node2, BAD_CAST "key", BAD_CAST key);
            xmlNewProp(node2, BAD_CAST "index", BAD_CAST AnsiString((long)i).c_str());

            char    *szData;
            INTEGER type;
            NUMBER  nData;

            InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);

            if (type == VARIABLE_DELEGATE) {
                char *deleg_name = 0;
                //xmlNewProp(node2, BAD_CAST "type", BAD_CAST "delegate");
                InvokePtr(INVOKE_GET_MEMBER_FROM_ID, (void *)szData, (long)nData, &deleg_name);
                nData = (intptr_t)deleg_name;
                char *class_name = 0;
                InvokePtr(INVOKE_GET_SERIAL_CLASS, (void *)szData, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);
                SerializeVariable(/*key*/ 0, type, class_name, nData, /*0*/ (void *)szData, newpData, node2, is_simple);
            } else
            if (type != VARIABLE_CLASS) {
                SerializeVariable(/*key*/ 0, type, szData, nData, /*0*/ (void *)szData, newpData, node2, is_simple);
            } else {
                if (!is_simple)
                    xmlNewProp(node2, BAD_CAST "type", BAD_CAST "object");
                Serialize(szData, node2, is_simple, 1);
            }
        }
    }
}

//---------------------------------------------------------------------------

void SerializeVariable(char *member, int type, char *szData, NUMBER nData, void *class_data, void *variable_data, xmlNodePtr parent, char is_simple) {
    DEBUG2("      -> variable", member ? member : "array element");
    xmlNodePtr node;
    xmlNodePtr node2;

    if (member) {
        if (is_simple) {
            node = xmlNewNode(NULL, BAD_CAST(member ? member : "%NO"));
            xmlAddChild(parent, node);
        } else {
            node = xmlNewNode(NULL, BAD_CAST "member");
            xmlAddChild(parent, node);
            xmlNewProp(node, BAD_CAST "name", BAD_CAST(member ? member : "%NO"));
        }
    } else {
        node = parent;
    }

    switch (type) {
        case VARIABLE_STRING:
            DEBUG2("           string", (void *)szData);
            DEBUG2("           length", (long)nData);
            if (nData > 0)
                DEBUG2("           content[0]", szData[0]);
            xmlNewProp(node, BAD_CAST "type", BAD_CAST "string");
            if ((szData) && (nData > 0)) {
                node2 = xmlNewTextLen(BAD_CAST szData, (int)nData);
                xmlAddChild(node, node2);
            }
            break;

        case VARIABLE_NUMBER:
            DEBUG("           number");
            xmlNewProp(node, BAD_CAST "type", BAD_CAST "number");
            node2 = xmlNewText(BAD_CAST AnsiString(nData).c_str());
            xmlAddChild(node, node2);
            break;

        case VARIABLE_CLASS:
            DEBUG("           class");
            if (!is_simple)
                xmlNewProp(node, BAD_CAST "type", BAD_CAST "class");
            Serialize(class_data, node, is_simple, 1);
            break;

        case VARIABLE_ARRAY:
            DEBUG("           array");
            xmlNewProp(node, BAD_CAST "type", BAD_CAST "array");
            SerializeArray(variable_data, class_data, node, is_simple);
            break;

        case VARIABLE_DELEGATE:
            DEBUG("           delegate");
            xmlNewProp(node, BAD_CAST "type", BAD_CAST "delegate");
            DEBUG2("              class", (void *)szData);
            xmlNewProp(node, BAD_CAST "class", BAD_CAST(szData ? szData : "%ERROR"));
            DEBUG2("              member", (void *)(long)nData);
            xmlNewProp(node, BAD_CAST "member", BAD_CAST(nData ? (char *)(long)nData : "%ERROR"));
            break;
    }
    DEBUG("           serialized");
}

//---------------------------------------------------------------------------
void Serialize(void *pData, xmlNodePtr parent, char is_simple, char is_simple_array_element) {
    xmlNodePtr node;
    void       *err_ser    = 0;
    char       *class_name = 0;

    if (!is_simple) {
        int ref_ptr = CheckBack(pData);
        if (ref_ptr) {
            DEBUG("...backreference");
            node = xmlNewNode(NULL, BAD_CAST "cyclic_reference");
            xmlNewProp(node, BAD_CAST "refID", BAD_CAST AnsiString((long)ref_ptr).c_str());

            xmlAddChild(parent, node);
            return;
        }
    }

    DEBUG("Getting members count, classname");
    int members_count = InvokePtr(INVOKE_GET_SERIAL_CLASS, pData, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);
    DEBUG("... success");

    if (is_simple) {
        node = parent;
        if (!node) {
            node = xmlNewNode(NULL, BAD_CAST class_name);
            root = node;
        }
        if ((parent) && (is_simple_array_element))
            xmlNewProp(node, BAD_CAST "type", BAD_CAST class_name);
    } else {
        node = xmlNewNode(NULL, BAD_CAST "object");

        if (!parent) {
            //xmlDocSetRootElement(doc, node);
            root = node;
        } else
            xmlAddChild(parent, node);

        xmlNewProp(node, BAD_CAST "cycid", BAD_CAST AnsiString((long)BACK_REF_COUNT).c_str());

        DEBUG2(class_name, "start");
        xmlNewProp(node, BAD_CAST "class", BAD_CAST(class_name ? class_name : "%ERROR%"));
    }

    if (members_count > 0) {
        char   **members       = new char * [members_count];
        char   *flags          = new char[members_count];
        char   *access         = new char[members_count];
        char   *types          = new char[members_count];
        char   **szValues      = new char * [members_count];
        NUMBER *nValues        = new NUMBER[members_count];
        void   **class_data    = new void * [members_count];
        void   **variable_data = new void * [members_count];

        //memset(szValues,0,members_count * sizeof(char *));
        //memset(nValues,0,members_count * sizeof(NUMBER));

        DEBUG2(class_name, "invoke");
        int result = InvokePtr(INVOKE_GET_SERIAL_CLASS, pData, members_count, &class_name, members, flags, access, types, (const char **)szValues, nValues, class_data, variable_data);
        DEBUG2(class_name, "done invoke");

        if (result != 0) {
            err_ser = (void *)"Error in serialization (bug ?)";
            return;
        }
        DEBUG2(class_name, "members loop");

#ifdef GO_DEBUG
        for (int i = 0; i < members_count; i++) {
            if ((flags[i] == 0) && (types[i] == VARIABLE_STRING)) {
                DEBUG3(class_name, "string member", i);
                DEBUG3(class_name, "string member", (void *)szValues[i]);
            }
        }
#endif

        for (int i = 0; i < members_count; i++) {
            if (flags[i] == 0) {
                DEBUG3(class_name, "serialize", members[i]);
                SerializeVariable(members[i], types[i], szValues[i], nValues[i], class_data[i], variable_data[i], node, is_simple);
                if (err_ser)
                    return;
            }
        }
        DEBUG2(class_name, "done members loop");

        delete[] members;
        delete[] flags;
        delete[] access;
        delete[] types;
        delete[] szValues;
        delete[] nValues;
        delete[] class_data;
        delete[] variable_data;

        DEBUG2(class_name, "delete");
    }
}

//---------------------------------------------------------------------------
char **GetCharList(void *arr) {
    INTEGER type      = 0;
    char    *str      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;
    char    **ret     = 0;

    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    ret        = new char * [count + 1];
    ret[count] = 0;

    for (int i = 0; i < count; i++) {
        InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i] = szData;
            } else
                ret[i] = 0;
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------------
static AnsiString xslt2_error;
void MyGenericErrorFunc(void *ctx, const char *msg, ...) {
    va_list args;

    va_start(args, msg);
    char buffer[0xFFF];
    buffer[0] = 0;
    vsprintf(buffer, msg, args);
    va_end(args);
    xslt2_error += buffer;
}

//---------------------------------------------------------------------------
void xslt_module_call(xmlXPathParserContextPtr ctxt, int argc) {
    if ((!ctxt->context) || (!ctxt->context->function))
        return;
    if (ref_doc) {
        xmlFreeDoc(ref_doc);
        ref_doc = 0;
    }
    void **PARAMETERS = new void * [argc + 1];
    PARAMETERS[argc] = 0;
    CALL_BACK_VARIABLE_SET SetVariable = SetVariablePtr;
    CALL_BACK_VARIABLE_GET GetVariable = GetVariablePtr;
    INVOKE_CALL            Invoke      = InvokePtr;
    for (int i = argc - 1; i >= 0; i--) {
        CREATE_VARIABLE(PARAMETERS[i]);

        xmlXPathObjectPtr name = valuePop(ctxt);
        if (name) {
            switch (name->type) {
                case XPATH_NUMBER:
                    SET_NUMBER_VARIABLE(PARAMETERS[i], name->floatval);
                    break;

                case XPATH_BOOLEAN:
                    SET_NUMBER_VARIABLE(PARAMETERS[i], name->boolval);
                    break;

                case XPATH_NODESET:
                    if (name->nodesetval) {
                        if (name->nodesetval->nodeNr == 0) {
                            SET_NUMBER_VARIABLE(PARAMETERS[i], 0);
                        } else
                        if (name->nodesetval->nodeNr == 1) {
                            if ((name->nodesetval->nodeTab) && (name->nodesetval->nodeTab[0]->type == XML_TEXT_NODE) && (name->nodesetval->nodeTab[0]->content)) {
                                SET_STRING_VARIABLE(PARAMETERS[i], (char *)name->nodesetval->nodeTab[0]->content);
                            } else
                            if ((name->nodesetval->nodeTab) && (name->nodesetval->nodeTab[0]->type != XML_TEXT_NODE) && (name->nodesetval->nodeTab[0]->children)) {
                                if ((name->nodesetval->nodeTab[0]->children->type == XML_TEXT_NODE) && (name->nodesetval->nodeTab[0]->children->content)) {
                                    SET_STRING_VARIABLE(PARAMETERS[i], (char *)name->nodesetval->nodeTab[0]->children->content);
                                } else {
                                    SET_STRING_VARIABLE(PARAMETERS[i], (char *)"");
                                }
                            } else {
                                SET_STRING_VARIABLE(PARAMETERS[i], (char *)"");
                            }
                        } else {
                            CREATE_ARRAY(PARAMETERS[i]);

                            for (int j = 0; j < name->nodesetval->nodeNr; j++)
                                Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETERS[i], j, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)&name->nodesetval[j]);
                        }
                    } else {
                        SET_NUMBER_VARIABLE(PARAMETERS[i], (NUMBER)0);
                    }
                    break;

                default:
                    if (name->stringval) {
                        SET_STRING_VARIABLE(PARAMETERS[i], (char *)name->stringval);
                    } else {
                        SET_STRING_VARIABLE(PARAMETERS[i], "");
                    }
                    break;
            }
            xmlXPathFreeObject(name);
        } else {
            SET_NUMBER_VARIABLE(PARAMETERS[i], (NUMBER)0);
        }
    }

    void *DELEGATE = CLASS_OBJECTS[(char *)ctxt->context->function];
    if (DELEGATE) {
        void *EXCEPTION = 0;
        void *RES       = 0;

        InvokePtr(INVOKE_CALL_DELEGATE, DELEGATE, &RES, &EXCEPTION, (INTEGER)-1, PARAMETERS);

        if (RES) {
            NUMBER  NUMBER_DUMMY;
            char    *STRING_DUMMY;
            INTEGER TYPE;

            GetVariable(RES, &TYPE, &STRING_DUMMY, &NUMBER_DUMMY);
            switch (TYPE) {
                case VARIABLE_STRING:
                    if (STRING_DUMMY)
                        valuePush(ctxt, xmlXPathNewString((const xmlChar *)STRING_DUMMY));
                    else
                        valuePush(ctxt, xmlXPathNewString((const xmlChar *)""));
                    break;

                case VARIABLE_NUMBER:
                    valuePush(ctxt, xmlXPathNewFloat(NUMBER_DUMMY));
                    break;

                case VARIABLE_ARRAY:
                    SerializeArray(RES, (void *)STRING_DUMMY, 0, 1);
                    if (root) {
                        ref_doc = xmlNewDoc(BAD_CAST "1.0");
                        xmlDocSetRootElement(ref_doc, root);
                        valuePush(ctxt, xmlXPathNewNodeSet((xmlNodePtr)ref_doc));
                        //xmlDebugDumpNode(stdout, root, 10);
                    }
                    root = 0;
                    break;

                case VARIABLE_CLASS:
                    Serialize((void *)STRING_DUMMY, 0, 1);
                    if (root) {
                        ref_doc = xmlNewDoc(BAD_CAST "1.0");
                        xmlDocSetRootElement(ref_doc, root);
                        valuePush(ctxt, xmlXPathNewNodeSet((xmlNodePtr)ref_doc));
                    }
                    root = 0;
                    break;
            }
            Invoke(INVOKE_FREE_VARIABLE, RES);
        }
        if (EXCEPTION)
            Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);
    }
    for (int i = 0; i < argc; i++) {
        if (PARAMETERS[i])
            Invoke(INVOKE_FREE_VARIABLE, PARAMETERS[i]);
    }
    delete[] PARAMETERS;
}

//---------------------------------------------------------------------------

/*void xslt_module_shutdown(xsltTransformContextPtr ctxt, const xmlChar *uri, void *data) {
    // nothing
   }
   //---------------------------------------------------------------------------
   void  *xslt_module_init(xsltTransformContextPtr ctxt, const xmlChar *uri) {
    xsltRegisterExtFunction (ctxt, (const xmlChar *)"foo", uri, xslt_module_call);
        return NULL;
   }*/
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    xsltSetGenericErrorFunc(0, MyGenericErrorFunc);
#ifdef _WIN32
    xmlMallocFunc  xmlMalloc  = 0;
    xmlReallocFunc xmlRealloc = 0;
    if (!xmlFree)
        xmlMemGet(&xmlFree, &xmlMalloc, &xmlRealloc, NULL);
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (HANDLER)
        xslt2_error = (char *)"";
    else
        xsltCleanupGlobals();

    // LET THE GARBAGE COLLECTOR HANDLE THIS !!!
    if (CLASS_OBJECTS.size()) {
        std::map<std::string, void *>::iterator end = CLASS_OBJECTS.end();
        for (std::map<std::string, void *>::iterator iter = CLASS_OBJECTS.begin(); iter != end; ++iter)
            if (iter->second)
                FREE_VARIABLE_REFERENCE(iter->second);
    }
    CLASS_OBJECTS.clear();
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XSLTError, 0)
    INTEGER len = xslt2_error.Length();
    if (len > 0) {
        AnsiString res(xslt2_error);
        RETURN_BUFFER(res.c_str(), res.Length())
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(XSLTRegister, 1, 3)
    SetVariablePtr = SetVariable;
    GetVariablePtr = GetVariable;
    xmlChar *ref = (xmlChar *)"http://www.devronium.com/csp";
    if (PARAMETERS_COUNT > 2) {
        T_STRING(XSLTRegister, 2)
        ref = (xmlChar *)PARAM(2);
    }
    T_OBJECT(XSLTRegister, 0);
    char *orig_name = 0;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(XSLTRegister, 1);
        orig_name = PARAM(1);
    }
    int res = -1;

    char *class_name   = 0;
    int  members_count = InvokePtr(INVOKE_GET_SERIAL_CLASS, PARAM(0), (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);
    if (members_count > 0) {
        char   **members       = new char * [members_count];
        char   *flags          = new char[members_count];
        char   *access         = new char[members_count];
        char   *types          = new char[members_count];
        char   **szValues      = new char * [members_count];
        NUMBER *nValues        = new NUMBER[members_count];
        void   **class_data    = new void * [members_count];
        void   **variable_data = new void * [members_count];

        int result = InvokePtr(INVOKE_GET_SERIAL_CLASS, PARAM(0), members_count, &class_name, members, flags, access, types, (const char **)szValues, nValues, class_data, variable_data);

        if (result != 0) {
            delete[] members;
            delete[] flags;
            delete[] access;
            delete[] types;
            delete[] szValues;
            delete[] nValues;
            delete[] class_data;
            delete[] variable_data;
            RETURN_NUMBER(result);
            return 0;
        }

        void *EXCEPTION = 0;
        void *RES       = 0;
        char is_func    = 1;

        if (!orig_name)
            orig_name = class_name;

        for (int i = 0; i < members_count; i++) {
            if (flags[i] == 1) {
                AnsiString fname(orig_name);
                fname += ".";
                fname += members[i];
                void *var = CLASS_OBJECTS[fname.c_str()];
                if (!var) {
                    Invoke(INVOKE_CREATE_VARIABLE_2, PARAMETERS->HANDLER, &var);
                }
                if (var) {
                    Invoke(INVOKE_CREATE_DELEGATE, PARAMETER(0), var, members[i]);
                    CLASS_OBJECTS[fname.c_str()] = var;
                    res = xsltRegisterExtModuleFunction((const xmlChar *)fname.c_str(), (const xmlChar *)ref, xslt_module_call);
                }
            }
        }

        delete[] members;
        delete[] flags;
        delete[] access;
        delete[] types;
        delete[] szValues;
        delete[] nValues;
        delete[] class_data;
        delete[] variable_data;
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(XSLTProcess, 2, 3)
    T_STRING(XSLTProcess, 0)
    T_STRING(XSLTProcess, 1)

    xsltStylesheetPtr cur = NULL;
    xmlDocPtr doc, res, docstyle;

    int        i;
    const char *params[1];
    params[0] = NULL;

    const char **params2 = 0;
    if (PARAMETERS_COUNT > 2) {
        T_ARRAY(XSLTProcess, 2);
        params2 = (const char **)GetCharList(PARAMETER(2));
    }

    if (xslt2_error.Length())
        xslt2_error.LoadBuffer(0, 0);

    void *old_ctx = xmlGenericErrorContext;
    xmlSetGenericErrorFunc(0, MyGenericErrorFunc);

    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;

    doc      = xmlReadMemory(PARAM(0), PARAM_LEN(0), "in.xml", 0, XML_PARSE_RECOVER);
    docstyle = xmlReadMemory(PARAM(1), PARAM_LEN(1), "in.xslt", 0, XML_PARSE_RECOVER);

    if ((doc) && (docstyle)) {
        cur = xsltParseStylesheetDoc(docstyle);

        if (cur) {
            res = xsltApplyStylesheet(cur, doc, params2 ? params2 : params);

            xmlChar *doc_txt_ptr = 0;
            int     doc_txt_len  = 0;
            xsltSaveResultToString(&doc_txt_ptr, &doc_txt_len, res, cur);
            if (doc_txt_len > 0) {
                RETURN_BUFFER((char *)doc_txt_ptr, doc_txt_len);

                if (doc_txt_ptr)
                    xmlFree(doc_txt_ptr);
            } else {
                RETURN_STRING("")
            }
            xsltFreeStylesheet(cur);
            xmlFreeDoc(res);
            xmlFreeDoc(doc);

            //xsltCleanupGlobals();
            //xmlCleanupParser();
        } else {
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
    if (params2)
        delete[] params2;
    xmlSetGenericErrorFunc(old_ctx, xmlGenericError);
    if (ref_doc) {
        xmlFreeDoc(ref_doc);
        ref_doc = 0;
    }
END_IMPL
//------------------------------------------------------------------------

