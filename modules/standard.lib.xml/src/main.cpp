//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"

#include <memory.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlmemory.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xpath.h>
#include <libxml/uri.h>

INVOKE_CALL InvokePtr = 0;

void *GenericErrorDelegate = 0;
void *StructErrorDelegate  = 0;
//-----------------------------------------------------------------------------------
void MyxmlErrorFunc(void *arg, const char *msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator) {
    if ((arg) && (InvokePtr)) {
        void *RES       = 0;
        void *EXCEPTION = 0;

        InvokePtr(INVOKE_CALL_DELEGATE, arg, &RES, &EXCEPTION, (INTEGER)4,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)arg,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)severity,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)locator
                  );

        if (RES)
            InvokePtr(INVOKE_FREE_VARIABLE, RES);
        if (EXCEPTION)
            InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
    }
}

//-----------------------------------------------------------------------------------
void MyStructuredErrorFunc(void *ctx, xmlErrorPtr error) {
    if ((StructErrorDelegate) && (InvokePtr)) {
        void *RES       = 0;
        void *EXCEPTION = 0;

        void *var = 0;

        InvokePtr(INVOKE_CREATE_VARIABLE, &var);
        InvokePtr(INVOKE_CREATE_ARRAY, var);

        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "domain", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)error->domain);

        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "code", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)error->code);
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "message", (INTEGER)VARIABLE_STRING, (char *)error->message, (NUMBER)0);
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "level", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(INTEGER)error->level);
        if (error->file)
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "file", (INTEGER)VARIABLE_STRING, (char *)error->file, (NUMBER)0);
        else
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "file", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "line", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)error->line);
        if (error->str1)
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "str1", (INTEGER)VARIABLE_STRING, (char *)error->str1, (NUMBER)0);
        else
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "str1", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
        if (error->str2)
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "str2", (INTEGER)VARIABLE_STRING, (char *)error->str2, (NUMBER)0);
        else
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "str2", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
        if (error->str3)
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "str3", (INTEGER)VARIABLE_STRING, (char *)error->str3, (NUMBER)0);
        else
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "str3", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "int1", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)error->int1);
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "int2", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)error->int2);

        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "ctx", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)error->ctxt);
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "node", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)error->node);


        InvokePtr(INVOKE_CALL_DELEGATE, StructErrorDelegate, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)ctx,
                  (INTEGER)VARIABLE_UNDEFINED, var
                  );

        InvokePtr(INVOKE_FREE_VARIABLE, var);
        if (RES)
            InvokePtr(INVOKE_FREE_VARIABLE, RES);
        if (EXCEPTION)
            InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
    }
}

//-----------------------------------------------------------------------------------
void MyGenericErrorFunc(void *ctx, const char *msg, ...) {
    if ((GenericErrorDelegate) && (InvokePtr)) {
        void *RES       = 0;
        void *EXCEPTION = 0;

        InvokePtr(INVOKE_CALL_DELEGATE, GenericErrorDelegate, &RES, &EXCEPTION, (INTEGER)2,
                  (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)ctx,
                  (INTEGER)VARIABLE_STRING, (char *)msg, (double)0
                  );

        if (RES)
            InvokePtr(INVOKE_FREE_VARIABLE, RES);
        if (EXCEPTION)
            InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
    }
}

//-----------------------------------------------------------------------------------
#ifdef _WIN32
xmlFreeFunc xmlFree = 0;
#endif
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    xmlMallocFunc  xmlMalloc  = 0;
    xmlReallocFunc xmlRealloc = 0;
    if (!xmlFree)
        xmlMemGet(&xmlFree, &xmlMalloc, &xmlRealloc, NULL);
#endif

    AnsiString value = XML_PARSE_DTDATTR;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "XML_PARSE_DTDATTR", value.c_str());
    InvokePtr = Invoke;
    value     = XML_PARSE_NOENT;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "XML_PARSE_NOENT", value.c_str());
    value = XML_PARSE_DTDVALID;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "XML_PARSE_DTDVALID", value.c_str());

    DEFINE_ECONSTANT(XML_PARSER_SEVERITY_VALIDITY_WARNING)
    DEFINE_ECONSTANT(XML_PARSER_SEVERITY_VALIDITY_ERROR)
    DEFINE_ECONSTANT(XML_PARSER_SEVERITY_WARNING)
    DEFINE_ECONSTANT(XML_PARSER_SEVERITY_ERROR)

    DEFINE_ECONSTANT(XML_TEXTREADER_MODE_INITIAL)
    DEFINE_ECONSTANT(XML_TEXTREADER_MODE_INTERACTIVE)
    DEFINE_ECONSTANT(XML_TEXTREADER_MODE_ERROR)
    DEFINE_ECONSTANT(XML_TEXTREADER_MODE_EOF)
    DEFINE_ECONSTANT(XML_TEXTREADER_MODE_CLOSED)
    DEFINE_ECONSTANT(XML_TEXTREADER_MODE_READING)

    DEFINE_ECONSTANT(XML_READER_TYPE_NONE)
    DEFINE_ECONSTANT(XML_READER_TYPE_ELEMENT)
    DEFINE_ECONSTANT(XML_READER_TYPE_ATTRIBUTE)
    DEFINE_ECONSTANT(XML_READER_TYPE_TEXT)
    DEFINE_ECONSTANT(XML_READER_TYPE_CDATA)
    DEFINE_ECONSTANT(XML_READER_TYPE_ENTITY_REFERENCE)
    DEFINE_ECONSTANT(XML_READER_TYPE_ENTITY)
    DEFINE_ECONSTANT(XML_READER_TYPE_PROCESSING_INSTRUCTION)
    DEFINE_ECONSTANT(XML_READER_TYPE_COMMENT)
    DEFINE_ECONSTANT(XML_READER_TYPE_DOCUMENT)
    DEFINE_ECONSTANT(XML_READER_TYPE_DOCUMENT_TYPE)
    DEFINE_ECONSTANT(XML_READER_TYPE_DOCUMENT_FRAGMENT)
    DEFINE_ECONSTANT(XML_READER_TYPE_NOTATION)
    DEFINE_ECONSTANT(XML_READER_TYPE_WHITESPACE)
    DEFINE_ECONSTANT(XML_READER_TYPE_SIGNIFICANT_WHITESPACE)
    DEFINE_ECONSTANT(XML_READER_TYPE_END_ELEMENT)
    DEFINE_ECONSTANT(XML_READER_TYPE_END_ENTITY)
    DEFINE_ECONSTANT(XML_READER_TYPE_XML_DECLARATION)

    DEFINE_ECONSTANT(XML_ERR_NONE)
    DEFINE_ECONSTANT(XML_ERR_WARNING)
    DEFINE_ECONSTANT(XML_ERR_ERROR)
    DEFINE_ECONSTANT(XML_ERR_FATAL)

    DEFINE_ECONSTANT(XML_FROM_NONE)
    DEFINE_ECONSTANT(XML_FROM_PARSER)
    DEFINE_ECONSTANT(XML_FROM_TREE)
    DEFINE_ECONSTANT(XML_FROM_NAMESPACE)
    DEFINE_ECONSTANT(XML_FROM_DTD)
    DEFINE_ECONSTANT(XML_FROM_HTML)
    DEFINE_ECONSTANT(XML_FROM_MEMORY)
    DEFINE_ECONSTANT(XML_FROM_OUTPUT)
    DEFINE_ECONSTANT(XML_FROM_IO)
    DEFINE_ECONSTANT(XML_FROM_FTP)
    DEFINE_ECONSTANT(XML_FROM_HTTP)
    DEFINE_ECONSTANT(XML_FROM_XINCLUDE)
    DEFINE_ECONSTANT(XML_FROM_XPATH)
    DEFINE_ECONSTANT(XML_FROM_XPOINTER)
    DEFINE_ECONSTANT(XML_FROM_REGEXP)
    DEFINE_ECONSTANT(XML_FROM_DATATYPE)
    DEFINE_ECONSTANT(XML_FROM_SCHEMASP)
    DEFINE_ECONSTANT(XML_FROM_SCHEMASV)
    DEFINE_ECONSTANT(XML_FROM_RELAXNGP)
    DEFINE_ECONSTANT(XML_FROM_RELAXNGV)
    DEFINE_ECONSTANT(XML_FROM_CATALOG)
    DEFINE_ECONSTANT(XML_FROM_C14N)
    DEFINE_ECONSTANT(XML_FROM_XSLT)
    DEFINE_ECONSTANT(XML_FROM_VALID)
    DEFINE_ECONSTANT(XML_FROM_CHECK)
    DEFINE_ECONSTANT(XML_FROM_WRITER)
    DEFINE_ECONSTANT(XML_FROM_MODULE)
    DEFINE_ECONSTANT(XML_FROM_I18N)
    //DEFINE_ECONSTANT(XML_FROM_SCHEMATRONV)


    LIBXML_TEST_VERSION

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    GenericErrorDelegate = 0;
    StructErrorDelegate  = 0;
    if (!HANDLER) {
        xmlCleanupParser();
        xmlMemoryDump();
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlReaderForFile CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlReaderForFile takes 3 parameters: filename, encoding, options");
    LOCAL_INIT;

    char   *string   = 0;
    char   *encoding = 0;
    NUMBER options;

    GET_CHECK_STRING(0, string, "xmlReaderFile : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, encoding, "xmlReaderFile : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(2, options, "xmlReaderFile : parameter 2 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = 0;

    string = SafePath(string, Invoke, PARAMETERS->HANDLER);
    reader = xmlReaderForFile(string, encoding, (int)options);
    free(string);

    RETURN_NUMBER((SYS_INT)reader);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlEncodeEntitiesReentrant CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlEncodeEntitiesReentrant takes 2 parameters: document handle, string to encode");
    LOCAL_INIT;

    NUMBER doc     = 0;
    char   *str    = 0;
    char   *result = 0;

    GET_CHECK_NUMBER(0, doc, "xmlEncodeEntitiesReentrant: parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, str, "xmlEncodeEntitiesReentrant : parameter 1 should be a string (STATIC STRING)");

    result = (char *)xmlEncodeEntitiesReentrant((xmlDocPtr)((SYS_INT)doc), BAD_CAST str);
    RETURN_STRING(result);

    if (result)
        xmlFree(result);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlReaderForMemory CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "xmlReaderForMemory takes 4 parameters: buffer, url, encoding, options");
    LOCAL_INIT;

    char   *string   = 0;
    char   *encoding = 0;
    char   *url;
    NUMBER len;
    NUMBER options;

    GET_CHECK_BUFFER(0, string, len, "xmlReaderForMemory : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, url, "xmlReaderForMemory : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, encoding, "xmlReaderForMemory : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(3, options, "xmlReaderForMemory : parameter 3 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = 0;

    reader = xmlReaderForMemory(string, (int)len, url, encoding, (int)options);

    RETURN_NUMBER((SYS_INT)reader);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderRead CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlTextReaderRead takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderRead : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_NUMBER(-100000);
        return 0;
    }

    RETURN_NUMBER(xmlTextReaderRead(reader));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlFreeTextReader CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlFreeTextReader takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlFreeTextReader : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlFreeTextReader(reader);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderConstName CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlTextReaderConstName takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderConstName : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_STRING("");
        return 0;
    }

    const xmlChar *value = xmlTextReaderConstName(reader);
    int           len    = xmlStrlen(value);

    RETURN_BUFFER((char *)value, len);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderConstValue CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlTextReaderConstValue takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderConstValue : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_STRING("");
        return 0;
    }

    const xmlChar *value = xmlTextReaderConstValue(reader);
    int           len    = xmlStrlen(value);

    RETURN_BUFFER((char *)value, len);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderDepth CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlTextReaderDepth takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderDepth : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(xmlTextReaderDepth(reader));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderNodeType CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlTextReaderNodeType takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderNodeType : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(xmlTextReaderNodeType(reader));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderIsEmptyElement CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlTextReaderIsEmptyElement takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderIsEmptyElement : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(xmlTextReaderIsEmptyElement(reader));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderHasValue CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlTextReaderHasValue takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderHasValue : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(xmlTextReaderHasValue(reader));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderIsValid CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlTextReaderIsValid takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderIsValid : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(xmlTextReaderIsValid(reader));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderPreservePattern CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlTextReaderPreservePattern takes 2 parameters: reader,pattern");
    LOCAL_INIT;

    NUMBER _reader;
    char   *pattern;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderPreservePattern : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, pattern, "xmlTextReaderPreservePattern : parameter 1 should be a string (STATIC STRING)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(xmlTextReaderPreservePattern(reader, (const xmlChar *)pattern, 0));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlCleanupParser CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "xmlCleanupParser takes no parameters");
    LOCAL_INIT;

    xmlCleanupParser();
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlMemoryDump CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "xmlMemoryDump takes no parameters");
    LOCAL_INIT;

    xmlMemoryDump();
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlTextReaderCurrentDoc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlTextReaderCurrentDoc takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _reader;

    GET_CHECK_NUMBER(0, _reader, "xmlTextReaderCurrentDoc : parameter 0 should be a number (STATIC NUMBER)");

    xmlTextReaderPtr reader = (xmlTextReaderPtr)(SYS_INT)_reader;
    if (!reader) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlTextReaderCurrentDoc(reader));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlParseFile CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlParseFile takes one parameter: filename");
    LOCAL_INIT;

    char *filename;

    GET_CHECK_STRING(0, filename, "xmlParseFile : parameter 0 should be a string (STATIC STRING)");

    filename = SafePath(filename, Invoke, PARAMETERS->HANDLER);
    RETURN_NUMBER((SYS_INT)xmlParseFile(filename));
    free(filename);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlParseMemory CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlParseMemory takes one parameter: buffer");
    LOCAL_INIT;

    char   *buf;
    NUMBER len;

    GET_CHECK_BUFFER(0, buf, len, "xmlParseMemory : parameter 0 should be a string (STATIC STRING)");

    RETURN_NUMBER((SYS_INT)xmlParseMemory(buf, (int)len));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlDocGetRootElement CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlDocGetRootElement takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _nptr;

    GET_CHECK_NUMBER(0, _nptr, "xmlDocGetRootElement : parameter 0 should be a number (STATIC NUMBER)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlDocGetRootElement(ptr));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlFreeDoc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlFreeDoc takes one parameter: reader");
    LOCAL_INIT;

    NUMBER _nptr;

    GET_CHECK_NUMBER(0, _nptr, "xmlFreeDoc : parameter 0 should be a number (STATIC NUMBER)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlFreeDoc(ptr);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_xmlGetNodeArray CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlGetNodeArray takes one parameter : nodeID");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "xmlGetNodeArray : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr node = (xmlNodePtr)(SYS_INT)nPtr;

    if (!node)
        return 0;


    /*void *array_var;
       void *array_data;

       Invoke(INVOKE_CREATE_VARIABLE,&array_var);
       Invoke(INVOKE_CREATE_ARRAY,array_var);

       GET_ARRAY_FROM_VARIABLE(array_var,array_data);*/

    Invoke(INVOKE_CREATE_ARRAY, RESULT);
    void *array_var = RESULT;

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "name", (INTEGER)VARIABLE_STRING, (char *)node->name, (NUMBER)0);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "children", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)node->children);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "last", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)node->last);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "parent", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)node->parent);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "next", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)node->next);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "prev", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)node->prev);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "doc", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)node->doc);

    void *var = 0;

    //InvokePtr(INVOKE_CREATE_VARIABLE, &var);
    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, array_var, "properties", &var);
    if (var) {
        InvokePtr(INVOKE_CREATE_ARRAY, var);

        int i = 0;
        for (xmlAttrPtr attr = node->properties; NULL != attr; attr = attr->next) {
            if (attr->name) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, var, i++, (INTEGER)VARIABLE_STRING, attr->name, (double)0);
            }
        }
    }

    //Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY,array_var,"properties",(INTEGER)VARIABLE_ARRAY,(char *),(NUMBER)0);

    /*RETURN_ARRAY(array_data);*/
    //Invoke(INVOKE_FREE_VARIABLE, array_var);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlReadFile CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlReadFile takes 3 parameters: filename, encoding, options");
    LOCAL_INIT;

    char   *string   = 0;
    char   *encoding = 0;
    NUMBER options;

    GET_CHECK_STRING(0, string, "xmlReadFile : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, encoding, "xmlReadFile : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(2, options, "xmlReadFile : parameter 2 should be a number (STATIC NUMBER)");

    if (!encoding[0])
        encoding = 0;

    RETURN_NUMBER((SYS_INT)xmlReadFile(string, encoding, (int)options));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlReadDoc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "xmlReadDoc takes 4 parameters: buffer, url, encoding, options");
    LOCAL_INIT;

    char   *string   = 0;
    char   *encoding = 0;
    char   *url;
    NUMBER len;
    NUMBER options;

    GET_CHECK_BUFFER(0, string, len, "xmlReadDoc : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, url, "xmlReadDoc : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, encoding, "xmlReadDoc : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(3, options, "xmlReadDoc : parameter 3 should be a number (STATIC NUMBER)");

    if (!encoding[0])
        encoding = 0;

    RETURN_NUMBER((SYS_INT)xmlReadDoc(BAD_CAST string, url, encoding, (int)options));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewDoc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlNewDoc takes one parameter: version");
    LOCAL_INIT;

    char *version;

    GET_CHECK_STRING(0, version, "xmlNewDoc : parameter 0 should be a string (STATIC STRING)");

    RETURN_NUMBER((SYS_INT)xmlNewDoc(BAD_CAST version));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNodeSetContent CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNodeSetContent takes 2 parameters: current, content");
    LOCAL_INIT;

    char   *content;
    NUMBER _nptr;
    NUMBER len;

    GET_CHECK_NUMBER(0, _nptr, "xmlNodeSetContent : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_BUFFER(1, content, len, "xmlNodeSetContent : parameter 1 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodeSetContentLen(ptr, BAD_CAST content, (int)len);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNodeSetName CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNodeSetName takes 2 parameters: current, name");
    LOCAL_INIT;

    char   *content;
    NUMBER _nptr;
    NUMBER len;

    GET_CHECK_NUMBER(0, _nptr, "xmlNodeSetName : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_BUFFER(1, content, len, "xmlNodeSetName : parameter 1 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodeSetName(ptr, BAD_CAST content);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNodeSetLang CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNodeSetLang takes 2 parameters: current, lang");
    LOCAL_INIT;

    char   *content;
    NUMBER _nptr;
    NUMBER len;

    GET_CHECK_NUMBER(0, _nptr, "xmlNodeSetLang : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_BUFFER(1, content, len, "xmlNodeSetLang : parameter 1 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodeSetLang(ptr, BAD_CAST content);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNodeSetBase CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNodeSetBase takes 2 parameters: current, lang");
    LOCAL_INIT;

    char   *content;
    NUMBER _nptr;
    NUMBER len;

    GET_CHECK_NUMBER(0, _nptr, "xmlNodeSetBase : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_BUFFER(1, content, len, "xmlNodeSetBase : parameter 1 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodeSetBase(ptr, BAD_CAST content);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlRemoveProp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlRemoveProp takes 1 parameters: node");
    LOCAL_INIT;

    NUMBER _nptr;

    GET_CHECK_NUMBER(0, _nptr, "xmlRemoveProp : parameter 0 should be a number (STATIC NUMBER)");

    xmlAttrPtr ptr = (xmlAttrPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(-1);
        return 0;
    }

    RETURN_NUMBER(xmlRemoveProp(ptr));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNodeGetContent CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlNodeGetContent takes one parameter: current");
    LOCAL_INIT;

    NUMBER _nptr;

    GET_CHECK_NUMBER(0, _nptr, "xmlNodeGetContent : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlChar *value = xmlNodeGetContent(ptr);
    int     len    = xmlStrlen(value);

    RETURN_BUFFER((char *)value, len);

    if (value)
        xmlFree(value);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNodeGetLang CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlNodeGetLang takes one parameter: current");
    LOCAL_INIT;

    NUMBER _nptr;

    GET_CHECK_NUMBER(0, _nptr, "xmlNodeGetLang : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlChar *value = xmlNodeGetLang(ptr);
    int     len    = xmlStrlen(value);

    RETURN_BUFFER((char *)value, len);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNodeGetBase CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNodeGetBase takes 2 parameters: doc, current");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    GET_CHECK_NUMBER(0, _nptr, "xmlNodeGetBase : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlNodeGetBase : parameter 1 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlChar *value = xmlNodeGetBase(ptr, ptr2);
    int     len    = xmlStrlen(value);

    RETURN_BUFFER((char *)value, len);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlGetPath CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlGetPath takes 2 parameters: doc, path");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *_path;

    GET_CHECK_NUMBER(0, _nptr, "xmlGetPath : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, _path, "xmlGetPath : parameter 1 should be a number (STATIC STRING)");


    xmlDocPtr doc = (xmlDocPtr)(SYS_INT)_nptr;
    if (!doc) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    xmlXPathObjectPtr  result  = xmlXPathEvalExpression(BAD_CAST _path, context);

    long node_handle = 0;
    if (result) {
        if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
            //if (result->nodesetval->nodeNr>0) {
            Invoke(INVOKE_CREATE_ARRAY, RESULT);
            for (int i = 0; i < result->nodesetval->nodeNr; i++) {
                node_handle = (SYS_INT)result->nodesetval->nodeTab[i];
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)node_handle);
                result->nodesetval->nodeTab[i] = NULL;
            }
            xmlXPathFreeObject(result);
            xmlXPathFreeContext(context);
            return 0;
            //}
        }
        xmlXPathFreeObject(result);
    }

    xmlXPathFreeContext(context);

    RETURN_NUMBER(node_handle);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewProp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlNewProp takes 3 parameters: node,name,value");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *name;
    char   *value;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewProp : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, name, "xmlNewProp : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, value, "xmlNewProp : parameter 2 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewProp(ptr, BAD_CAST name, BAD_CAST value));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewNode CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNewNode takes 2 parameters: node,name");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *name;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewNode : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, name, "xmlNewNode : parameter 1 should be a string (STATIC STRING)");

    xmlNsPtr ptr = (xmlNsPtr)(SYS_INT)_nptr;

    RETURN_NUMBER((SYS_INT)xmlNewNode(ptr, BAD_CAST name));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlDocDumpFormatMemory CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlDocDumpFormatMemory takes 2 parameters: doc, format");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER format;

    GET_CHECK_NUMBER(0, _nptr, "xmlDocDumpFormatMemory : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, format, "xmlDocDumpFormatMemory : parameter 1 should be a number (STATIC NUMBER)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_STRING("");
        return 0;
    }

    xmlChar *mem;
    int     size;

    xmlDocDumpFormatMemory(ptr, &mem, &size, (int)format);

    if (size < 0)
        size = 0;

    RETURN_BUFFER((char *)mem, size);

    xmlFree(mem);
    //free(mem);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlDocDumpFormatMemoryEnc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlDocDumpFormatMemoryEnc takes 3 parameters: doc, format, encoding");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER format;
    char *enc = (char *)"UTF-8";

    GET_CHECK_NUMBER(0, _nptr, "xmlDocDumpFormatMemoryEnc : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, format, "xmlDocDumpFormatMemoryEnc : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(2, enc, "xmlDocDumpFormatMemoryEnc : parameter 2 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_STRING("");
        return 0;
    }

    xmlChar *mem;
    int     size;

    xmlDocDumpFormatMemoryEnc(ptr, &mem, &size, enc, (int)format);

    if (size < 0)
        size = 0;

    RETURN_BUFFER((char *)mem, size);

    xmlFree(mem);
    //free(mem);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlDocSetRootElement CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlDocSetRootElement takes 2 parameters: doc, current");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    GET_CHECK_NUMBER(0, _nptr, "xmlDocSetRootElement : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlDocSetRootElement : parameter 1 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlDocSetRootElement(ptr, ptr2));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlAddChild CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlAddChild takes 2 parameters: parent, current");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    GET_CHECK_NUMBER(0, _nptr, "xmlAddChild : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlAddChild : parameter 1 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlAddChild(ptr, ptr2));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlAddChildList CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlAddChildList takes 2 parameters: parent, current");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    GET_CHECK_NUMBER(0, _nptr, "xmlAddChildList : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlAddChildList : parameter 1 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlAddChildList(ptr, ptr2));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlAddNextSibling CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlAddNextSibling takes 2 parameters: parent, current");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    GET_CHECK_NUMBER(0, _nptr, "xmlAddNextSibling : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlAddNextSibling : parameter 1 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlAddNextSibling(ptr, ptr2));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlAddPrevSibling CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlAddPrevSibling takes 2 parameters: parent, current");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    GET_CHECK_NUMBER(0, _nptr, "xmlAddPrevSibling : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlAddPrevSibling : parameter 1 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlAddPrevSibling(ptr, ptr2));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlAddSibling CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlAddSibling takes 2 parameters: parent, current");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    GET_CHECK_NUMBER(0, _nptr, "xmlAddSibling : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlAddSibling : parameter 1 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlAddSibling(ptr, ptr2));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlGetNodePath CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlGetNodePath takes 1 parameters: current");
    LOCAL_INIT;

    NUMBER _nptr;

    GET_CHECK_NUMBER(0, _nptr, "xmlGetNodePath : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }


    char *path = (char *)xmlGetNodePath(ptr);
    RETURN_STRING(path);
    xmlFree(path);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlGetNsList CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlGetNsList takes 2 parameters: doc, current");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    GET_CHECK_NUMBER(0, _nptr, "xmlGetNsList : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlGetNsList : parameter 1 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlGetNsList(ptr, ptr2));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlGetNsProp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlGetNsProp takes 3 parameters: current, name, namespace");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *name;
    char   *_namespace;

    GET_CHECK_NUMBER(0, _nptr, "xmlGetNsProp : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, name, "xmlGetNsProp : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, _namespace, "xmlGetNsProp : parameter 2 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    char *res = (char *)xmlGetNsProp(ptr, BAD_CAST name, BAD_CAST _namespace);

    RETURN_STRING(res);

    if (res)
        xmlFree(res);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlGetProp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlGetProp takes 2 parameters: current, name");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *name;

    GET_CHECK_NUMBER(0, _nptr, "xmlGetProp : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, name, "xmlGetProp : parameter 1 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    char *res = (char *)xmlGetProp(ptr, BAD_CAST name);

    RETURN_STRING(res);

    if (res)
        xmlFree(res);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlHasNsProp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlHasNsProp takes 3 parameters: current, name, namespace");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *name;
    char   *_namespace;

    GET_CHECK_NUMBER(0, _nptr, "xmlHasNsProp : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, name, "xmlHasNsProp : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, _namespace, "xmlHasNsProp : parameter 2 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlHasNsProp(ptr, BAD_CAST name, BAD_CAST _namespace));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlHasProp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlHasProp takes 2 parameters: current, name");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *name;

    GET_CHECK_NUMBER(0, _nptr, "xmlHasProp : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, name, "xmlHasProp : parameter 1 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlHasProp(ptr, BAD_CAST name));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlIsBlankNode CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlIsBlankNode takes 1 parameters: current");
    LOCAL_INIT;

    NUMBER _nptr;

    GET_CHECK_NUMBER(0, _nptr, "xmlIsBlankNode : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(xmlIsBlankNode(ptr));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewChild CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "xmlNewChild takes 4 parameters: parent, ns, name, content");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;
    char   *name;
    char   *content;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewChild : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlNewChild : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(2, name, "xmlNewChild : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, content, "xmlNewChild : parameter 3 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNsPtr ptr2 = (xmlNsPtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewChild(ptr, ptr2, BAD_CAST name, BAD_CAST content));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewComment CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlNewComment takes 1 parameters: content");
    LOCAL_INIT;

    char *content;

    GET_CHECK_STRING(0, content, "xmlNewComment : parameter 1 should be a string (STATIC STRING)");

    RETURN_NUMBER((SYS_INT)xmlNewComment(BAD_CAST content));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewDocComment CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNewDocComment takes 2 parameters: doc, content");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *content;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewDocComment : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, content, "xmlNewDocComment : parameter 1 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewDocComment(ptr, BAD_CAST content));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewDocNode CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "xmlNewDocNode takes 4 parameters: parent, ns, name, content");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;
    char   *name;
    char   *content;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewDocNode: parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlNewDocNode : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(2, name, "xmlNewDocNode : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, content, "xmlNewDocNode : parameter 3 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNsPtr ptr2 = (xmlNsPtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewDocNode(ptr, ptr2, BAD_CAST name, BAD_CAST content));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewDocRawNode CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "xmlNewDocRawNode takes 4 parameters: parent, ns, name, content");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;
    char   *name;
    char   *content;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewDocRawNode: parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlNewDocRawNode : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(2, name, "xmlNewDocRawNode : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, content, "xmlNewDocRawNode : parameter 3 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNsPtr ptr2 = (xmlNsPtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewDocRawNode(ptr, ptr2, BAD_CAST name, BAD_CAST content));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewDocText CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNewDocText takes 2 parameters: doc, content");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *content;
    NUMBER len;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewDocText : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_BUFFER(1, content, len, "xmlNewDocText : parameter 1 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewDocTextLen(ptr, BAD_CAST content, (INTEGER)len));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewGlobalNs CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlNewGlobalNs takes 3 parameters: doc, href, prefix");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *href;
    char   *prefix;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewGlobalNs : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, href, "xmlNewGlobalNs : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, prefix, "xmlNewGlobalNs : parameter 2 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewGlobalNs(ptr, BAD_CAST href, BAD_CAST prefix));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewNs CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlNewNs takes 3 parameters: node, href, prefix");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *href;
    char   *prefix;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewNs : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, href, "xmlNewNs : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, prefix, "xmlNewNs : parameter 2 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewNs(ptr, BAD_CAST href, BAD_CAST prefix));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewNsProp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "xmlNewNsProp takes 4 parameters: current, ns, name, value");
    LOCAL_INIT;

    NUMBER _nptr, _nptr2;
    char   *name;
    char   *_namespace;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewNsProp : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlNewNsProp : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(2, name, "xmlNewNsProp : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, _namespace, "xmlNewNsProp : parameter 3 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNsPtr ptr2 = (xmlNsPtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewNsProp(ptr, ptr2, BAD_CAST name, BAD_CAST _namespace));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewText CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlNewText takes one parameter: content");
    LOCAL_INIT;

    char   *content;
    NUMBER len;

    GET_CHECK_BUFFER(0, content, len, "xmlNewText : parameter 0 should be a string (STATIC STRING)");


    RETURN_NUMBER((SYS_INT)xmlNewTextLen(BAD_CAST content, (INTEGER)len));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewTextChild CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "xmlNewTextChild takes 4 parameters: current, ns, name, value");
    LOCAL_INIT;

    NUMBER _nptr, _nptr2;
    char   *name;
    char   *_namespace;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewTextChild : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlNewTextChild : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(2, name, "xmlNewTextChild : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, _namespace, "xmlNewTextChild : parameter 3 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNsPtr ptr2 = (xmlNsPtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewTextChild(ptr, ptr2, BAD_CAST name, BAD_CAST _namespace));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNodeAddContent CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNodeAddContent takes 2 parameters: current, text");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *name;
    NUMBER len;

    GET_CHECK_NUMBER(0, _nptr, "xmlNodeAddContent : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_BUFFER(1, name, len, "xmlNodeAddContent : parameter 1 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodeAddContentLen(ptr, BAD_CAST name, (int)len);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlSaveFile CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 3, "xmlSaveFile takes 2 or 3 parameters: filename, current [, encoding]");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *name;
    char   *encoding;
    NUMBER len;

    GET_CHECK_BUFFER(0, name, len, "xmlSaveFile : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, _nptr, "xmlSaveFile : parameter 1 should be a number (STATIC NUMBER)");


    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    name = SafePath(name, Invoke, PARAMETERS->HANDLER);
    if (PARAMETERS_COUNT == 3) {
        GET_CHECK_STRING(2, encoding, "xmlSaveFile : parameter 2 should be a string (STATIC STRING)");
        if (!encoding[0])
            encoding = 0;
        RETURN_NUMBER(xmlSaveFileEnc(name, ptr, encoding));
    } else {
        RETURN_NUMBER(xmlSaveFile(name, ptr));
    }
    free(name);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlSearchNs CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlSearchNs takes 3 parameters: doc, current, namespace");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    char *name;
    GET_CHECK_NUMBER(0, _nptr, "xmlSearchNs : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlSearchNs : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(2, name, "xmlSearchNs : parameter 2 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlSearchNs(ptr, ptr2, BAD_CAST name));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlSearchNsByHref CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlSearchNsByHref takes 3 parameters: doc, current, href");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    char *name;
    GET_CHECK_NUMBER(0, _nptr, "xmlSearchNs : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlSearchNs : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(2, name, "xmlSearchNs : parameter 2 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodePtr ptr2 = (xmlNodePtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlSearchNsByHref(ptr, ptr2, BAD_CAST name));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlSetNsProp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "xmlSetNsProp takes 4 parameters: current, ns, name, value");
    LOCAL_INIT;

    NUMBER _nptr, _nptr2;
    char   *name;
    char   *_namespace;

    GET_CHECK_NUMBER(0, _nptr, "xmlSetNsProp : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlSetNsProp : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(2, name, "xmlSetNsProp : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, _namespace, "xmlSetNsProp : parameter 3 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNsPtr ptr2 = (xmlNsPtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlSetNsProp(ptr, ptr2, BAD_CAST name, BAD_CAST _namespace));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlSetProp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlSetProp takes 3 parameters: node,name,value");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *name;
    char   *value;

    GET_CHECK_NUMBER(0, _nptr, "xmlSetProp : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, name, "xmlSetProp : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, value, "xmlSetProp : parameter 2 should be a string (STATIC STRING)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlSetProp(ptr, BAD_CAST name, BAD_CAST value));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlSetTreeDoc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlSetTreeDoc takes 2 parameters: current, doc");
    LOCAL_INIT;

    NUMBER _nptr;
    NUMBER _nptr2;

    GET_CHECK_NUMBER(0, _nptr, "xmlDocSetRootElement : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, _nptr2, "xmlDocSetRootElement : parameter 1 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlDocPtr ptr2 = (xmlDocPtr)(SYS_INT)_nptr2;
    if (!ptr2) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlSetTreeDoc(ptr, ptr2);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewPI CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNewPI takes 2 parameters: name,content");
    LOCAL_INIT;

    char   *name;
    char   *content;

    GET_CHECK_STRING(0, name, "xmlNewPI : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, content, "xmlNewPI : parameter 1 should be a string (STATIC STRING)");

    RETURN_NUMBER((SYS_INT)xmlNewPI(BAD_CAST name, BAD_CAST content));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewReference CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "xmlNewReference takes 2 parameters: doc, content");
    LOCAL_INIT;

    NUMBER _nptr;
    char   *content;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewReference : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, content, "xmlNewReference : parameter 1 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewReference(ptr, BAD_CAST content));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNewDocPI CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "xmlNewDocPI takes 3 parameters: doc, name, content");
    LOCAL_INIT;

    NUMBER _nptr;

    char *name;
    char *content;

    GET_CHECK_NUMBER(0, _nptr, "xmlNewDocPI : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, name, "xmlNewDocPI : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, content, "xmlNewDocPI : parameter 2 should be a string (STATIC STRING)");

    xmlDocPtr ptr = (xmlDocPtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)xmlNewDocPI(ptr, BAD_CAST name, BAD_CAST content));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlReadMemory CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "xmlReadMemory takes 4 parameters: buffer, url, encoding, options");
    LOCAL_INIT;

    char   *string   = 0;
    char   *encoding = 0;
    char   *url;
    NUMBER len;
    NUMBER options;

    GET_CHECK_BUFFER(0, string, len, "xmlReadMemory : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, url, "xmlReadMemory : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, encoding, "xmlReadMemory : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(3, options, "xmlReadMemory : parameter 3 should be a number (STATIC NUMBER)");

    xmlDocPtr reader = 0;

    reader = xmlReadMemory(string, (int)len, url, encoding, (int)options);

    RETURN_NUMBER((SYS_INT)reader);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__xmlNodeIsText CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "xmlNodeIsText takes 1 parameters: current");
    LOCAL_INIT;

    NUMBER _nptr;

    GET_CHECK_NUMBER(0, _nptr, "xmlNodeIsText : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(SYS_INT)_nptr;
    if (!ptr) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(xmlNodeIsText(ptr));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_xmlCopyNode, 2)
    T_NUMBER(_xmlCopyNode, 0)
    T_NUMBER(_xmlCopyNode, 1)

    RETURN_NUMBER((SYS_INT)xmlCopyNode((xmlNodePtr)PARAM_INT(0), PARAM_INT(1)))
END_IMPL
//---------------------------------------------------------------------------
// BEGIN OF HTML FUNCTIONS
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlCreateMemoryParserCtxt, 1)
    T_STRING(_htmlCreateMemoryParserCtxt, 0)

    RETURN_NUMBER((SYS_INT)htmlCreateMemoryParserCtxt(PARAM(0), PARAM_LEN(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlCtxtReadDoc, 5)
    T_NUMBER(_htmlCtxtReadDoc, 0)
    T_STRING(_htmlCtxtReadDoc, 1)
    T_STRING(_htmlCtxtReadDoc, 2)
    T_STRING(_htmlCtxtReadDoc, 3)
    T_NUMBER(_htmlCtxtReadDoc, 4)

    RETURN_NUMBER((SYS_INT)htmlCtxtReadDoc((htmlParserCtxtPtr)PARAM_INT(0), BAD_CAST PARAM(1), PARAM(2), PARAM(3), PARAM_INT(4)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlCtxtReadFile, 4)
    T_NUMBER(_htmlCtxtReadFile, 0)
    T_STRING(_htmlCtxtReadFile, 1)
    T_STRING(_htmlCtxtReadFile, 2)
    T_NUMBER(_htmlCtxtReadFile, 3)

    RETURN_NUMBER((SYS_INT)htmlCtxtReadFile((htmlParserCtxtPtr)PARAM_INT(0), PARAM(1), PARAM(2), PARAM_INT(3)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlCtxtReadMemory, 5)
    T_NUMBER(_htmlCtxtReadMemory, 0)
    T_STRING(_htmlCtxtReadMemory, 1)
    T_STRING(_htmlCtxtReadMemory, 2)
    T_STRING(_htmlCtxtReadMemory, 3)
    T_NUMBER(_htmlCtxtReadMemory, 4)

    RETURN_NUMBER((SYS_INT)htmlCtxtReadMemory((htmlParserCtxtPtr)PARAM_INT(0), PARAM(1), PARAM_LEN(1), PARAM(2), PARAM(3), PARAM_INT(4)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlCtxtReset, 1)
    T_NUMBER(_htmlCtxtReset, 0)

    htmlCtxtReset((htmlParserCtxtPtr)PARAM_INT(0));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlCtxtUseOptions, 2)
    T_NUMBER(_htmlCtxtUseOptions, 0)
    T_NUMBER(_htmlCtxtUseOptions, 1)

    RETURN_NUMBER(htmlCtxtUseOptions((htmlParserCtxtPtr)PARAM_INT(0), PARAM_INT(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlElementAllowedHere, 2)
    T_NUMBER(_htmlElementAllowedHere, 0)
    T_STRING(_htmlElementAllowedHere, 1)

    RETURN_NUMBER(htmlElementAllowedHere((htmlElemDesc *)PARAM_INT(0), BAD_CAST PARAM(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlElementStatusHere, 2)
    T_NUMBER(_htmlElementStatusHere, 0)
    T_NUMBER(_htmlElementStatusHere, 1)

    RETURN_NUMBER(htmlElementStatusHere((htmlElemDesc *)PARAM_INT(0), (htmlElemDesc *)PARAM_INT(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlFreeParserCtxt, 1)
    T_NUMBER(_htmlFreeParserCtxt, 0)

    htmlFreeParserCtxt((htmlParserCtxtPtr)PARAM_INT(0));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlHandleOmittedElem, 1)
    T_NUMBER(_htmlHandleOmittedElem, 0)

    RETURN_NUMBER(htmlHandleOmittedElem(PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlIsAutoClosed, 2)
    T_NUMBER(_htmlIsAutoClosed, 0)
    T_NUMBER(_htmlIsAutoClosed, 1)

    RETURN_NUMBER(htmlIsAutoClosed((htmlDocPtr)PARAM_INT(0), (htmlNodePtr)PARAM_INT(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlIsScriptAttribute, 1)
    T_STRING(_htmlIsScriptAttribute, 0)

    RETURN_NUMBER(htmlIsScriptAttribute(BAD_CAST PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(_htmlNewParserCtx,0)
        RETURN_NUMBER((SYS_INT)htmlNewParserCtxt())
   END_IMPL*/
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlNodeStatus, 2)
    T_NUMBER(_htmlNodeStatus, 0)
    T_NUMBER(_htmlNodeStatus, 1)

    RETURN_NUMBER(htmlNodeStatus((htmlNodePtr)PARAM_INT(0), PARAM_INT(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlParseCharRef, 1)
    T_NUMBER(_htmlParseCharRef, 0)

    RETURN_NUMBER(htmlParseCharRef((htmlParserCtxtPtr)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlParseChunk, 3)
    T_NUMBER(_htmlParseChunk, 0)
    T_STRING(_htmlParseChunk, 1)
    T_NUMBER(_htmlParseChunk, 2)

    RETURN_NUMBER(htmlParseChunk((htmlParserCtxtPtr)PARAM_INT(0), PARAM(1), PARAM_LEN(1), PARAM_INT(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlParseDoc, 2)
    T_STRING(_htmlParseDoc, 0)
    T_STRING(_htmlParseDoc, 1)

    RETURN_NUMBER((SYS_INT)htmlParseDoc(BAD_CAST PARAM(0), PARAM(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlParseDocument, 1)
    T_NUMBER(_htmlParseDocument, 0)

    RETURN_NUMBER(htmlParseDocument((htmlParserCtxtPtr)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlParseElement, 1)
    T_NUMBER(_htmlParseElement, 0)

    htmlParseElement((htmlParserCtxtPtr)PARAM_INT(0));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlParseFile, 2)
    T_STRING(_htmlParseFile, 0)
    T_STRING(_htmlParseFile, 1)

    RETURN_NUMBER((SYS_INT)htmlParseFile(PARAM(0), PARAM(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlReadDoc, 4)
    T_STRING(_htmlReadDoc, 0)
    T_STRING(_htmlReadDoc, 1)
    T_STRING(_htmlReadDoc, 2)
    T_NUMBER(_htmlReadDoc, 3)

    RETURN_NUMBER((SYS_INT)htmlReadDoc(BAD_CAST PARAM(0), PARAM(1), PARAM(2), PARAM_INT(3)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlReadFile, 3)
    T_STRING(_htmlReadFile, 0)
    T_STRING(_htmlReadFile, 1)
    T_NUMBER(_htmlReadFile, 2)

    RETURN_NUMBER((SYS_INT)htmlReadFile(PARAM(0), PARAM(1), PARAM_INT(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlReadMemory, 4)
    T_STRING(_htmlReadMemory, 0)
    T_STRING(_htmlReadMemory, 1)
    T_STRING(_htmlReadMemory, 2)
    T_NUMBER(_htmlReadMemory, 3)

    RETURN_NUMBER((SYS_INT)htmlReadMemory(PARAM(0), PARAM_LEN(0), PARAM(1), PARAM(2), PARAM_INT(3)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlDocDumpMemory, 1)
    T_NUMBER(_htmlDocDumpMemory, 0)

    xmlChar * buf = 0;
    int size = 0;
    htmlDocDumpMemory((xmlDocPtr)PARAM_INT(0), &buf, &size);

    RETURN_BUFFER(((char *)(BAD_CAST buf)), size);

    if (buf)
        xmlFree(buf);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlGetMetaEncoding, 1)
    T_NUMBER(_htmlGetMetaEncoding, 0)

    RETURN_STRING((char *)htmlGetMetaEncoding((htmlDocPtr)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlIsBooleanAttr, 1)
    T_STRING(_htmlIsBooleanAttr, 0)

    RETURN_NUMBER(htmlIsBooleanAttr(BAD_CAST PARAM(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlNewDoc, 2)
    T_STRING(_htmlNewDoc, 0)
    T_STRING(_htmlNewDoc, 1)

    RETURN_NUMBER((SYS_INT)htmlNewDoc(BAD_CAST PARAM(0), BAD_CAST PARAM(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlNewDocNoDtD, 2)
    T_STRING(_htmlNewDocNoDtD, 0)
    T_STRING(_htmlNewDocNoDtD, 1)

    RETURN_NUMBER((SYS_INT)htmlNewDocNoDtD(BAD_CAST PARAM(0), BAD_CAST PARAM(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlSaveFile, 2)
    T_STRING(_htmlSaveFile, 0)
    T_NUMBER(_htmlSaveFile, 1)

    RETURN_NUMBER(htmlSaveFile(PARAM(0), (xmlDocPtr)PARAM_INT(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlSaveFileEnc, 3)
    T_STRING(_htmlSaveFileEnc, 0)
    T_NUMBER(_htmlSaveFileEnc, 1)
    T_STRING(_htmlSaveFileEnc, 2)

    RETURN_NUMBER(htmlSaveFileEnc(PARAM(0), (xmlDocPtr)PARAM_INT(1), PARAM(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlSaveFileFormat, 4)
    T_STRING(_htmlSaveFileFormat, 0)
    T_NUMBER(_htmlSaveFileFormat, 1)
    T_STRING(_htmlSaveFileFormat, 2)
    T_NUMBER(_htmlSaveFileFormat, 3)

    RETURN_NUMBER(htmlSaveFileFormat(PARAM(0), (xmlDocPtr)PARAM_INT(1), PARAM(2), PARAM_INT(3)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlSetMetaEncoding, 2)
    T_NUMBER(_htmlSetMetaEncoding, 0)
    T_STRING(_htmlSetMetaEncoding, 1)

    RETURN_NUMBER(htmlSetMetaEncoding((htmlDocPtr)PARAM_INT(0), BAD_CAST PARAM(1)))
END_IMPL
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlElemDescArray, 1)
    T_NUMBER(_htmlElemDescArray, 0)
    htmlElemDesc * ent = (htmlElemDesc *)PARAM_INT(0);

    if (!ent) {
        RETURN_NUMBER(0)
        return 0;
    }

    Invoke(INVOKE_CREATE_ARRAY, RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "name", (INTEGER)VARIABLE_STRING, (char *)ent->name, (NUMBER)0);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "startTag", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ent->startTag);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "endTag", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ent->endTag);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "saveEndTag", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ent->saveEndTag);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "empty", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ent->empty);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "depr", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ent->depr);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "dtd", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ent->dtd);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "isinline", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ent->isinline);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "desc", (INTEGER)VARIABLE_STRING, (char *)ent->desc, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "defaultsubelt", (INTEGER)VARIABLE_STRING, (char *)ent->defaultsubelt, (NUMBER)0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_htmlEntityDescArray, 1)
    T_NUMBER(_htmlEntityDescArray, 0)

    htmlEntityDesc * ent = (htmlEntityDesc *)PARAM_INT(0);

    if (!ent) {
        RETURN_NUMBER(0)
        return 0;
    }

    Invoke(INVOKE_CREATE_ARRAY, RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "value", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ent->value);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "name", (INTEGER)VARIABLE_STRING, (char *)ent->name, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "desc", (INTEGER)VARIABLE_STRING, (char *)ent->desc, (NUMBER)0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xmlTextReaderSetErrorHandler, 2)
    T_NUMBER(xmlTextReaderSetErrorHandler, 0)
    T_DELEGATE(xmlTextReaderSetErrorHandler, 1)

    xmlTextReaderPtr reader = (xmlTextReaderPtr)PARAM_INT(0);
    if (!reader) {
        RETURN_NUMBER(0)
        return 0;
    }
    xmlTextReaderSetErrorHandler(reader, (xmlTextReaderErrorFunc)MyxmlErrorFunc, (void *)(PARAMETER(1)));

    RETURN_NUMBER(1)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xmlSetGenericErrorFunc, 2)
    T_NUMBER(xmlSetGenericErrorFunc, 0)

    char *dclass = 0;
    NUMBER dmember = 0;
    GET_DELEGATE(1, dclass, dmember)

    void *ctx = (void *)PARAM_INT(0);

    if ((TYPE != VARIABLE_DELEGATE) && (TYPE != VARIABLE_NUMBER)) {
        T_DELEGATE(xmlSetGenericErrorFunc, 1)
    }

    if (GenericErrorDelegate) {
        Invoke(INVOKE_FREE_VARIABLE, GenericErrorDelegate);
        GenericErrorDelegate = 0;
    }

    if (TYPE == VARIABLE_NUMBER) {
        xmlSetGenericErrorFunc(ctx, (xmlGenericErrorFunc)0);
    } else {
        DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(1), GenericErrorDelegate);
        xmlSetGenericErrorFunc(ctx, MyGenericErrorFunc);
    }

    RETURN_NUMBER(1)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xmlSetStructuredErrorFunc, 2)
    T_NUMBER(xmlSetStructuredErrorFunc, 0)

    char *dclass = 0;
    NUMBER dmember = 0;
    GET_DELEGATE(1, dclass, dmember)

    void *ctx = (void *)PARAM_INT(0);

    if ((TYPE != VARIABLE_DELEGATE) && (TYPE != VARIABLE_NUMBER)) {
        T_DELEGATE(xmlSetStructuredErrorFunc, 1)
    }

    if (StructErrorDelegate) {
        Invoke(INVOKE_FREE_VARIABLE, StructErrorDelegate);
        StructErrorDelegate = 0;
    }

    if (TYPE == VARIABLE_NUMBER) {
        xmlSetStructuredErrorFunc(ctx, (xmlStructuredErrorFunc)0);
    } else {
        DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(1), StructErrorDelegate);
        xmlSetStructuredErrorFunc(ctx, MyStructuredErrorFunc);
    }

    RETURN_NUMBER(1)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(xmlURIEscapeStr, 1, 2)
    T_STRING(xmlURIEscapeStr, 0)
    char *exceptions = (char *)"";
    if (PARAMETERS_COUNT > 1) {
        T_STRING(xmlURIEscapeStr, 1)
        exceptions = PARAM(0);
    }
    xmlChar *res = xmlURIEscapeStr((xmlChar *)PARAM(0), (xmlChar *)exceptions);
    RETURN_STRING((char *)res)
    xmlFree(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xmlURIUnescapeString, 1)
    T_STRING(xmlURIUnescapeString, 0)
    char *res = xmlURIUnescapeString(PARAM(0), PARAM_LEN(0), NULL);
    RETURN_STRING(res)
    xmlFree(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xmlFreeNode, 1)
    T_NUMBER(xmlFreeNode, 0)

    xmlNodePtr node = (xmlNodePtr)PARAM_INT(0);
    if (node) {
        xmlUnlinkNode(node);
        xmlFreeNode(node);
        SET_NUMBER(0, 0);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xmlNewCDataBlock, 2)
    T_NUMBER(xmlNewCDataBlock, 0)
    T_STRING(xmlNewCDataBlock, 1)

    xmlDocPtr doc = (xmlDocPtr)(SYS_INT)PARAM(0);
    if (!doc) {
        RETURN_NUMBER(0);
        return 0;
    }

    xmlNodePtr node = xmlNewCDataBlock(doc, BAD_CAST PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER((SYS_INT)node);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xmlParseInNodeContext, 4)
    T_HANDLE(xmlParseInNodeContext, 0)
    T_STRING(xmlParseInNodeContext, 1)
    T_NUMBER(xmlParseInNodeContext, 2)

    xmlNodePtr node = (xmlNodePtr)(SYS_INT)PARAM(0);

    xmlNodePtr      new_node = NULL;
    xmlParserErrors err      = xmlParseInNodeContext(node, PARAM(1), PARAM_LEN(1), PARAM_INT(2), &new_node);
    SET_NUMBER(3, (SYS_INT)new_node);
    RETURN_NUMBER((SYS_INT)err);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xmlNodeDump, 5)
    T_NUMBER(xmlNodeDump, 1)
    T_HANDLE(xmlNodeDump, 2)
    T_NUMBER(xmlNodeDump, 3)
    T_NUMBER(xmlNodeDump, 4)

    xmlBufferPtr buf = xmlBufferCreate();
    xmlDocPtr  doc  = (xmlDocPtr)(SYS_INT)PARAM(1);
    xmlNodePtr node = (xmlNodePtr)(SYS_INT)PARAM(2);

    size_t res   = xmlNodeDump(buf, doc, node, PARAM_INT(3), PARAM_INT(4));
    char   *rbuf = (char *)xmlBufferContent(buf);
    SET_STRING(0, rbuf);
    xmlBufferFree(buf);
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xmlChildElementCount, 1)
    T_HANDLE(xmlChildElementCount, 0)

    xmlNodePtr node = (xmlNodePtr)(SYS_INT)PARAM(0);

    unsigned long count = xmlChildElementCount(node);
    RETURN_NUMBER((SYS_INT)count);
END_IMPL
//---------------------------------------------------------------------------
