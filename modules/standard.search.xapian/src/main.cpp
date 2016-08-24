//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>

#include <xapian.h>

INVOKE_CALL InvokePtr = 0;

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    //DEFINE_ECONSTANT(FO_ENUM_FORMAT_SVG);
    DEFINE_ICONSTANT("OP_AND", Xapian::Query::OP_AND)
    DEFINE_ICONSTANT("OP_OR", Xapian::Query::OP_OR)
    DEFINE_ICONSTANT("OP_AND_NOT", Xapian::Query::OP_AND_NOT)
    DEFINE_ICONSTANT("OP_XOR", Xapian::Query::OP_XOR)
    DEFINE_ICONSTANT("OP_AND_MAYBE", Xapian::Query::OP_AND_MAYBE)
    DEFINE_ICONSTANT("OP_FILTER", Xapian::Query::OP_FILTER)
    DEFINE_ICONSTANT("OP_NEAR", Xapian::Query::OP_NEAR)
    DEFINE_ICONSTANT("OP_PHRASE", Xapian::Query::OP_PHRASE)
    DEFINE_ICONSTANT("OP_VALUE_RANGE", Xapian::Query::OP_VALUE_RANGE)
    DEFINE_ICONSTANT("OP_SCALE_WEIGHT", Xapian::Query::OP_SCALE_WEIGHT)
    DEFINE_ICONSTANT("OP_ELITE_SET", Xapian::Query::OP_ELITE_SET)
    DEFINE_ICONSTANT("OP_VALUE_GET", Xapian::Query::OP_VALUE_GE)
    DEFINE_ICONSTANT("OP_VALUE_LE", Xapian::Query::OP_VALUE_LE)

    DEFINE_ICONSTANT("FLAG_BOOLEAN", Xapian::QueryParser::FLAG_BOOLEAN)
    DEFINE_ICONSTANT("FLAG_PHRASE", Xapian::QueryParser::FLAG_PHRASE)
    DEFINE_ICONSTANT("FLAG_LOVEHATE", Xapian::QueryParser::FLAG_LOVEHATE)
    DEFINE_ICONSTANT("FLAG_BOOLEAN_ANY_CASE", Xapian::QueryParser::FLAG_BOOLEAN_ANY_CASE)
    DEFINE_ICONSTANT("FLAG_WILDCARD", Xapian::QueryParser::FLAG_WILDCARD)
    DEFINE_ICONSTANT("FLAG_PURE_NOT", Xapian::QueryParser::FLAG_PURE_NOT)
    DEFINE_ICONSTANT("FLAG_PARTIAL", Xapian::QueryParser::FLAG_PARTIAL)
    DEFINE_ICONSTANT("FLAG_SPELLING_CORRECTION", Xapian::QueryParser::FLAG_SPELLING_CORRECTION)
    DEFINE_ICONSTANT("FLAG_SYNONYM", Xapian::QueryParser::FLAG_SYNONYM)
    DEFINE_ICONSTANT("FLAG_AUTO_SYNONYMS", Xapian::QueryParser::FLAG_AUTO_SYNONYMS)
    DEFINE_ICONSTANT("FLAG_AUTO_MULTIWORD_SYNONYMS", Xapian::QueryParser::FLAG_AUTO_MULTIWORD_SYNONYMS)
    int val = Xapian::QueryParser::FLAG_PHRASE | Xapian::QueryParser::FLAG_BOOLEAN | Xapian::QueryParser::FLAG_LOVEHATE;
    DEFINE_ICONSTANT("FLAG_DEFAULT", val)

    DEFINE_ICONSTANT("STEM_NONE", Xapian::QueryParser::STEM_NONE)
    DEFINE_ICONSTANT("STEM_SOME", Xapian::QueryParser::STEM_SOME)
    DEFINE_ICONSTANT("STEM_ALL", Xapian::QueryParser::STEM_ALL)

    DEFINE_ICONSTANT("ASCENDING", Xapian::Enquire::ASCENDING)
    DEFINE_ICONSTANT("DESCENDING", Xapian::Enquire::DESCENDING)
    DEFINE_ICONSTANT("DONT_CARE", Xapian::Enquire::DONT_CARE)
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocumentCreate, 1)
    SET_STRING(0, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Document())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocumentDestroy, 2)
    T_NUMBER(XapianDocumentDestroy, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__get_value, 3)
    T_NUMBER(XapianDocument__get_value, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__get_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDocument__get_value, 2)
    Xapian::valueno valueno = (Xapian::valueno)PARAM(2);
    try {
        //RETURN_STRING((char *)CONCEPT_THIS->get_value(valueno).c_str())
        std::string data = CONCEPT_THIS->get_value(valueno);
        if (data.size()) {
            RETURN_BUFFER(data.c_str(), data.size())
        } else {
            RETURN_STRING("")
        }
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__add_value, 4)
    T_NUMBER(XapianDocument__add_value, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__add_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDocument__add_value, 2)
    Xapian::valueno valueno = (Xapian::valueno)PARAM_INT(2);

    CHECK_STATIC_PARAM(3, "XapianDocument__add_value: value should be a string or a number")
//T_STRING(3)
    std::string value;
    if (TYPE == VARIABLE_NUMBER) {
        T_NUMBER(XapianDocument__add_value, 3)
        value = Xapian::sortable_serialise(PARAM(3));
    } else {
        T_STRING(XapianDocument__add_value, 3)
        value.append((const char *)PARAM(3), (size_t)PARAM_LEN(3));
    }
//std::string value=(std::string)PARAM(3);
    try {
        CONCEPT_THIS->add_value(valueno, value);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__remove_value, 3)
    T_NUMBER(XapianDocument__remove_value, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__remove_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDocument__remove_value, 2)
    Xapian::valueno valueno = (Xapian::valueno)PARAM(2);
    try {
        CONCEPT_THIS->remove_value(valueno);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__clear_values, 2)
    T_NUMBER(XapianDocument__clear_values, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__clear_values object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        CONCEPT_THIS->clear_values();
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__get_data, 2)
    T_NUMBER(XapianDocument__get_data, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__get_data object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        std::string data = CONCEPT_THIS->get_data();
        if (data.size()) {
            RETURN_BUFFER(data.c_str(), data.size())
        } else {
            RETURN_STRING("")
        }
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__set_data, 3)
    T_NUMBER(XapianDocument__set_data, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__set_data object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDocument__set_data, 2)
    std::string data;
    data.append((const char *)PARAM(2), (size_t)PARAM_LEN(2));
    try {
        CONCEPT_THIS->set_data(data);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__add_posting, 5)
    T_NUMBER(XapianDocument__add_posting, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__add_posting object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDocument__add_posting, 2)
    std::string tname = (std::string)PARAM(2);
    T_NUMBER(XapianDocument__add_posting, 3)
    Xapian::termpos tpos = (Xapian::termpos)PARAM(3);
    T_NUMBER(XapianDocument__add_posting, 4)
    Xapian::termcount wdfinc = (Xapian::termcount)PARAM(4);
    try {
        CONCEPT_THIS->add_posting(tname, tpos, wdfinc);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__add_term, 4)
    T_NUMBER(XapianDocument__add_term, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__add_term object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDocument__add_term, 2)
    std::string tname = (std::string)PARAM(2);
    T_NUMBER(XapianDocument__add_term, 3)
    Xapian::termcount wdfinc = (Xapian::termcount)PARAM(3);
    try {
        CONCEPT_THIS->add_term(tname, wdfinc);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__remove_posting, 5)
    T_NUMBER(XapianDocument__remove_posting, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__remove_posting object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDocument__remove_posting, 2)
    std::string tname = (std::string)PARAM(2);
    T_NUMBER(XapianDocument__remove_posting, 3)
    Xapian::termpos tpos = (Xapian::termpos)PARAM(3);
    T_NUMBER(XapianDocument__remove_posting, 4)
    Xapian::termcount wdfdec = (Xapian::termcount)PARAM(4);
    try {
        CONCEPT_THIS->remove_posting(tname, tpos, wdfdec);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__remove_term, 3)
    T_NUMBER(XapianDocument__remove_term, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__remove_term object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDocument__remove_term, 2)
    std::string tname = (std::string)PARAM(2);
    try {
        CONCEPT_THIS->remove_term(tname);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__clear_terms, 2)
    T_NUMBER(XapianDocument__clear_terms, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__clear_terms object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        CONCEPT_THIS->clear_terms();
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__termlist_count, 2)
    T_NUMBER(XapianDocument__termlist_count, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__termlist_count object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->termlist_count())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__values_count, 2)
    T_NUMBER(XapianDocument__values_count, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__values_count object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->values_count())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__get_docid, 2)
    T_NUMBER(XapianDocument__get_docid, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__get_docid object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_docid())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDocument__get_description, 2)
    T_NUMBER(XapianDocument__get_description, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::ValueIterator temp_begin;
CONCEPT_FUNCTION_IMPL(XapianDocument__values_begin, 2)
    T_NUMBER(XapianDocument__values_begin, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__values_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        temp_begin = CONCEPT_THIS->values_begin();
        RETURN_NUMBER((SYS_INT)&temp_begin)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::ValueIterator temp_end;
CONCEPT_FUNCTION_IMPL(XapianDocument__values_end, 2)
    T_NUMBER(XapianDocument__values_end, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__values_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        temp_end = CONCEPT_THIS->values_end();
        RETURN_NUMBER((SYS_INT)&temp_end)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator term_temp_begin;
CONCEPT_FUNCTION_IMPL(XapianDocument__termlist_begin, 2)
    T_NUMBER(XapianDocument__termlist_begin, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__termlist_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        term_temp_begin = CONCEPT_THIS->termlist_begin();
        RETURN_NUMBER((SYS_INT)&term_temp_begin)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator term_temp_end;
CONCEPT_FUNCTION_IMPL(XapianDocument__termlist_end, 2)
    T_NUMBER(XapianDocument__termlist_end, 0)
    Xapian::Document * CONCEPT_THIS = (Xapian::Document *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDocument__termlist_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        term_temp_end = CONCEPT_THIS->termlist_end();
        RETURN_NUMBER((SYS_INT)&term_temp_end)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabaseCreate, 1)
    SET_STRING(0, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::WritableDatabase())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabaseCreate2, 3)
    SET_STRING(0, (char *)"");
    T_STRING(XapianWritableDatabaseCreate2, 1)
    T_NUMBER(XapianWritableDatabaseCreate2, 2)

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::WritableDatabase(PARAM(1), PARAM_INT(2)))
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabaseDestroy, 2)
    T_NUMBER(XapianWritableDatabaseDestroy, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__flush, 2)
    T_NUMBER(XapianWritableDatabase__flush, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__flush object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        CONCEPT_THIS->flush();
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__begin_transaction, 3)
    T_NUMBER(XapianWritableDatabase__begin_transaction, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__begin_transaction object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianWritableDatabase__begin_transaction, 2)
    bool flushed = (bool)PARAM(2);
    try {
        CONCEPT_THIS->begin_transaction(flushed);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__commit_transaction, 2)
    T_NUMBER(XapianWritableDatabase__commit_transaction, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__commit_transaction object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        CONCEPT_THIS->commit_transaction();
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__cancel_transaction, 2)
    T_NUMBER(XapianWritableDatabase__cancel_transaction, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__cancel_transaction object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        CONCEPT_THIS->cancel_transaction();
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__add_document, 3)
    T_NUMBER(XapianWritableDatabase__add_document, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__add_document object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianWritableDatabase__add_document, 2)
    Xapian::Document * document = (Xapian::Document *)PARAM_INT(2);
    if (!document) {
        return (void *)"XapianWritableDatabase__add_document: document is null";
    }
    try {
        RETURN_NUMBER(CONCEPT_THIS->add_document(*document))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__delete_document, 3)
    T_NUMBER(XapianWritableDatabase__delete_document, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__delete_document object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianWritableDatabase__delete_document, 2)
    Xapian::docid docid = (Xapian::docid)PARAM(2);
    try {
        CONCEPT_THIS->delete_document(docid);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__delete_document2, 3)
    T_NUMBER(XapianWritableDatabase__delete_document2, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__delete_document2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianWritableDatabase__delete_document2, 2)
    std::string unique_term = (std::string)PARAM(2);
    try {
        CONCEPT_THIS->delete_document(unique_term);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__replace_document, 4)
    T_NUMBER(XapianWritableDatabase__replace_document, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__replace_document object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianWritableDatabase__replace_document, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    T_NUMBER(XapianWritableDatabase__replace_document, 3)
    Xapian::Document * document = (Xapian::Document *)PARAM_INT(3);
    if (!document) {
        return (void *)"XapianWritableDatabase__replace_document: document is null";
    }
    try {
        CONCEPT_THIS->replace_document(did, *document);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__replace_document2, 4)
    T_NUMBER(XapianWritableDatabase__replace_document2, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__replace_document2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianWritableDatabase__replace_document2, 2)
    std::string unique_term = (std::string)PARAM(2);
    T_NUMBER(XapianWritableDatabase__replace_document2, 3)
    Xapian::Document * document = (Xapian::Document *)PARAM_INT(3);
    if (!document) {
        return (void *)"XapianWritableDatabase__replace_document2: document is null";
    }
    try {
        RETURN_NUMBER(CONCEPT_THIS->replace_document(unique_term, *document))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__add_spelling, 4)
    T_NUMBER(XapianWritableDatabase__add_spelling, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__add_spelling object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianWritableDatabase__add_spelling, 2)
    std::string word = (std::string)PARAM(2);
    T_NUMBER(XapianWritableDatabase__add_spelling, 3)
    Xapian::termcount freqinc = (Xapian::termcount)PARAM(3);
    try {
        CONCEPT_THIS->add_spelling(word, freqinc);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__remove_spelling, 4)
    T_NUMBER(XapianWritableDatabase__remove_spelling, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__remove_spelling object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianWritableDatabase__remove_spelling, 2)
    std::string word = (std::string)PARAM(2);
    T_NUMBER(XapianWritableDatabase__remove_spelling, 3)
    Xapian::termcount freqdec = (Xapian::termcount)PARAM(3);
    try {
        CONCEPT_THIS->remove_spelling(word, freqdec);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__add_synonym, 4)
    T_NUMBER(XapianWritableDatabase__add_synonym, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__add_synonym object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianWritableDatabase__add_synonym, 2)
    std::string term = (std::string)PARAM(2);
    T_STRING(XapianWritableDatabase__add_synonym, 3)
    std::string synonym = (std::string)PARAM(3);
    try {
        CONCEPT_THIS->add_synonym(term, synonym);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__remove_synonym, 4)
    T_NUMBER(XapianWritableDatabase__remove_synonym, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__remove_synonym object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianWritableDatabase__remove_synonym, 2)
    std::string term = (std::string)PARAM(2);
    T_STRING(XapianWritableDatabase__remove_synonym, 3)
    std::string synonym = (std::string)PARAM(3);
    try {
        CONCEPT_THIS->remove_synonym(term, synonym);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__clear_synonyms, 3)
    T_NUMBER(XapianWritableDatabase__clear_synonyms, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__clear_synonyms object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianWritableDatabase__clear_synonyms, 2)
    std::string term = (std::string)PARAM(2);
    try {
        CONCEPT_THIS->clear_synonyms(term);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__set_metadata, 4)
    T_NUMBER(XapianWritableDatabase__set_metadata, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__set_metadata object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianWritableDatabase__set_metadata, 2)
    std::string key = (std::string)PARAM(2);
    T_STRING(XapianWritableDatabase__set_metadata, 3)
    std::string _value = (std::string)PARAM(3);
    try {
        CONCEPT_THIS->set_metadata(key, _value);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianWritableDatabase__get_description, 2)
    T_NUMBER(XapianWritableDatabase__get_description, 0)
    Xapian::WritableDatabase * CONCEPT_THIS = (Xapian::WritableDatabase *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianWritableDatabase__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGeneratorCreate, 1)
    SET_STRING(0, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::TermGenerator())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGeneratorDestroy, 2)
    T_NUMBER(XapianTermGeneratorDestroy, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__set_stemmer, 3)
    T_NUMBER(XapianTermGenerator__set_stemmer, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__set_stemmer object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianTermGenerator__set_stemmer, 2)
    Xapian::Stem * stemmer = (Xapian::Stem *)PARAM_INT(2);
    if (!stemmer) {
        return (void *)"XapianTermGenerator__set_stemmer: no stemmer given";
    }
    try {
        CONCEPT_THIS->set_stemmer(*stemmer);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__set_stopper, 3)
    T_NUMBER(XapianTermGenerator__set_stopper, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__set_stopper object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianTermGenerator__set_stopper, 2)
    Xapian::Stopper * stop = (Xapian::Stopper *)PARAM_INT(2);
    try {
        CONCEPT_THIS->set_stopper(stop);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__set_document, 3)
    T_NUMBER(XapianTermGenerator__set_document, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__set_document object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianTermGenerator__set_document, 2)
    Xapian::Document * document = (Xapian::Document *)PARAM_INT(2);
    if (!document) {
        return (void *)"XapianTermGenerator__set_document: document is null";
    }
    try {
        CONCEPT_THIS->set_document(*document);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__get_document, 2)
    T_NUMBER(XapianTermGenerator__get_document, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__get_document object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Document(CONCEPT_THIS->get_document()))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__set_database, 3)
    T_NUMBER(XapianTermGenerator__set_database, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__set_database object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianTermGenerator__set_database, 2)
    Xapian::WritableDatabase * db = (Xapian::WritableDatabase *)PARAM_INT(2);
    if (!db) {
        return (void *)"XapianTermGenerator__set_database: database is null";
    }
    try {
        CONCEPT_THIS->set_database(*db);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__set_flags, 4)
    T_NUMBER(XapianTermGenerator__set_flags, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__set_flags object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianTermGenerator__set_flags, 2)
    Xapian::TermGenerator::flags toggle = (Xapian::TermGenerator::flags)PARAM_INT(2);
    T_NUMBER(XapianTermGenerator__set_flags, 3)
    Xapian::TermGenerator::flags mask = (Xapian::TermGenerator::flags)PARAM_INT(3);
    try {
        RETURN_NUMBER(CONCEPT_THIS->set_flags(toggle, mask))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__index_text, 5)
    T_NUMBER(XapianTermGenerator__index_text, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__index_text object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianTermGenerator__index_text, 2)
    std::string text = (std::string)PARAM(2);
    T_NUMBER(XapianTermGenerator__index_text, 3)
    Xapian::termcount weight = (Xapian::termcount)PARAM(3);
    T_STRING(XapianTermGenerator__index_text, 4)
    std::string prefix = (std::string)PARAM(4);
    try {
        CONCEPT_THIS->index_text(text, weight, prefix);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__index_text_without_positions, 5)
    T_NUMBER(XapianTermGenerator__index_text_without_positions, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__index_text_without_positions object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianTermGenerator__index_text_without_positions, 2)
    std::string text = (std::string)PARAM(2);
    T_NUMBER(XapianTermGenerator__index_text_without_positions, 3)
    Xapian::termcount weight = (Xapian::termcount)PARAM(3);
    T_STRING(XapianTermGenerator__index_text_without_positions, 4)
    std::string prefix = (std::string)PARAM(4);

    try {
        CONCEPT_THIS->index_text_without_positions(text, weight, prefix);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__increase_termpos, 3)
    T_NUMBER(XapianTermGenerator__increase_termpos, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__increase_termpos object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianTermGenerator__increase_termpos, 2)
    Xapian::termcount delta = (Xapian::termcount)PARAM(2);
    try {
        CONCEPT_THIS->increase_termpos(delta);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__get_termpos, 2)
    T_NUMBER(XapianTermGenerator__get_termpos, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__get_termpos object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_termpos())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__set_termpos, 3)
    T_NUMBER(XapianTermGenerator__set_termpos, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__set_termpos object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianTermGenerator__set_termpos, 2)
    Xapian::termcount termpos = (Xapian::termcount)PARAM(2);
    try {
        CONCEPT_THIS->set_termpos(termpos);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermGenerator__get_description, 2)
    T_NUMBER(XapianTermGenerator__get_description, 0)
    Xapian::TermGenerator * CONCEPT_THIS = (Xapian::TermGenerator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermGenerator__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianStemCreate, 1)
    SET_STRING(0, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Stem())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianStemCreate2, 2)
    SET_STRING(0, (char *)"");
    T_STRING(XapianStemCreate2, 1)

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Stem((std::string)PARAM(1)))
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianStemDestroy, 2)
    T_NUMBER(XapianStemDestroy, 0)
    Xapian::Stem * CONCEPT_THIS = (Xapian::Stem *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianStem__get_description, 2)
    T_NUMBER(XapianStem__get_description, 0)
    Xapian::Stem * CONCEPT_THIS = (Xapian::Stem *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianStem__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianStem__stem_word, 3)
    T_NUMBER(XapianStem__stem_word, 0)
    Xapian::Stem * CONCEPT_THIS = (Xapian::Stem *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianStem__stem_word object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianStem__stem_word, 2)
    std::string word = (std::string)PARAM(2);
    try {
        RETURN_STRING((char *)CONCEPT_THIS->operator()(word).c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianValueIterator__next, 2)
    T_NUMBER(XapianValueIterator__next, 0)
    Xapian::ValueIterator * CONCEPT_THIS = (Xapian::ValueIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianValueIterator__next object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)&CONCEPT_THIS->operator++())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianValueIterator__get_value, 2)
    T_NUMBER(XapianValueIterator__get_value, 0)
    Xapian::ValueIterator * CONCEPT_THIS = (Xapian::ValueIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianValueIterator__get_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->operator*().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianValueIterator__get_valueno, 2)
    T_NUMBER(XapianValueIterator__get_valueno, 0)
    Xapian::ValueIterator * CONCEPT_THIS = (Xapian::ValueIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianValueIterator__get_valueno object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_valueno())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianValueIterator__get_description, 2)
    T_NUMBER(XapianValueIterator__get_description, 0)
    Xapian::ValueIterator * CONCEPT_THIS = (Xapian::ValueIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianValueIterator__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianValueIterator__equal, 3)
    T_NUMBER(XapianValueIterator__equal, 0)
    Xapian::ValueIterator * CONCEPT_THIS = (Xapian::ValueIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianValueIterator__equal object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianValueIterator__equal, 2)
    Xapian::ValueIterator * iter = (Xapian::ValueIterator *)PARAM_INT(2);
    if (!iter) {
        RETURN_NUMBER(0);
        return 0;
    }
    try {
        RETURN_NUMBER(*CONCEPT_THIS == *iter)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPositionIterator__next, 2)
    T_NUMBER(XapianPositionIterator__next, 0)
    Xapian::PositionIterator * CONCEPT_THIS = (Xapian::PositionIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPositionIterator__next object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)&CONCEPT_THIS->operator++())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPositionIterator__get_value, 2)
    T_NUMBER(XapianPositionIterator__get_value, 0)
    Xapian::PositionIterator * CONCEPT_THIS = (Xapian::PositionIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPositionIterator__get_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)CONCEPT_THIS->operator*())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPositionIterator__skip_to, 3)
    T_NUMBER(XapianPositionIterator__skip_to, 0)
    Xapian::PositionIterator * CONCEPT_THIS = (Xapian::PositionIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPositionIterator__skip_to object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianPositionIterator__skip_to, 2)
    Xapian::termpos pos = (Xapian::termpos)PARAM(2);
    try {
        CONCEPT_THIS->skip_to(pos);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPositionIterator__get_description, 2)
    T_NUMBER(XapianPositionIterator__get_description, 0)
    Xapian::PositionIterator * CONCEPT_THIS = (Xapian::PositionIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPositionIterator__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPositionIterator__equal, 3)
    T_NUMBER(XapianPositionIterator__equal, 0)
    Xapian::PositionIterator * CONCEPT_THIS = (Xapian::PositionIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPositionIterator__equal object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianPositionIterator__equal, 2)
    Xapian::PositionIterator * iter = (Xapian::PositionIterator *)PARAM_INT(2);
    if (!iter) {
        RETURN_NUMBER(0);
        return 0;
    }
    try {
        RETURN_NUMBER(*CONCEPT_THIS == *iter)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermIterator__next, 2)
    T_NUMBER(XapianTermIterator__next, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__next object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)&CONCEPT_THIS->operator++())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermIterator__get_value, 2)
    T_NUMBER(XapianTermIterator__get_value, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__get_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->operator*().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermIterator__skip_to, 3)
    T_NUMBER(XapianTermIterator__skip_to, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__skip_to object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianTermIterator__skip_to, 2)
    std::string tname = (std::string)PARAM(2);
    try {
        CONCEPT_THIS->skip_to(tname);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermIterator__get_description, 2)
    T_NUMBER(XapianTermIterator__get_description, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermIterator__equal, 3)
    T_NUMBER(XapianTermIterator__equal, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__equal object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianTermIterator__equal, 2)
    Xapian::TermIterator * iter = (Xapian::TermIterator *)PARAM_INT(2);
    if (!iter) {
        RETURN_NUMBER(0);
        return 0;
    }
    try {
        RETURN_NUMBER(*CONCEPT_THIS == *iter)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermIterator__get_wdf, 2)
    T_NUMBER(XapianTermIterator__get_wdf, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__get_wdf object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_wdf())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermIterator__get_termfreq, 2)
    T_NUMBER(XapianTermIterator__get_termfreq, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__get_termfreq object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_termfreq())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianTermIterator__positionlist_count, 2)
    T_NUMBER(XapianTermIterator__positionlist_count, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__positionlist_count object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->positionlist_count())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::PositionIterator temp_pos_begin;
CONCEPT_FUNCTION_IMPL(XapianTermIterator__positionlist_begin, 2)
    T_NUMBER(XapianTermIterator__positionlist_begin, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__positionlist_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        temp_pos_begin = CONCEPT_THIS->positionlist_begin();
        RETURN_NUMBER((SYS_INT)&temp_pos_begin)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::PositionIterator temp_pos_end;
CONCEPT_FUNCTION_IMPL(XapianTermIterator__positionlist_end, 2)
    T_NUMBER(XapianTermIterator__positionlist_end, 0)
    Xapian::TermIterator * CONCEPT_THIS = (Xapian::TermIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianTermIterator__positionlist_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        temp_pos_end = CONCEPT_THIS->positionlist_end();
        RETURN_NUMBER((SYS_INT)&temp_pos_end)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__next, 2)
    T_NUMBER(XapianMSetIterator__next, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__next object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)&CONCEPT_THIS->operator++())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__prev, 2)
    T_NUMBER(XapianMSetIterator__prev, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__prev object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)&CONCEPT_THIS->operator--())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__get_value, 2)
    T_NUMBER(XapianMSetIterator__get_value, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__get_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->operator*())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__get_description, 2)
    T_NUMBER(XapianMSetIterator__get_description, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__equal, 3)
    T_NUMBER(XapianMSetIterator__equal, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__equal object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianMSetIterator__equal, 2)
    Xapian::MSetIterator * iter = (Xapian::MSetIterator *)PARAM_INT(2);
    if (!iter) {
        RETURN_NUMBER(0);
        return 0;
    }
    try {
        RETURN_NUMBER((*CONCEPT_THIS) == (*iter))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__get_document, 2)
    T_NUMBER(XapianMSetIterator__get_document, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__get_document object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Document(CONCEPT_THIS->get_document()))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__get_rank, 2)
    T_NUMBER(XapianMSetIterator__get_rank, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__get_rank object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_rank())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__get_weight, 2)
    T_NUMBER(XapianMSetIterator__get_weight, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__get_weight object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_weight())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__get_collapse_key, 2)
    T_NUMBER(XapianMSetIterator__get_collapse_key, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__get_collapse_key object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_collapse_key().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__get_collapse_count, 2)
    T_NUMBER(XapianMSetIterator__get_collapse_count, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__get_collapse_count object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_collapse_count())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetIterator__get_percent, 2)
    T_NUMBER(XapianMSetIterator__get_percent, 0)
    Xapian::MSetIterator * CONCEPT_THIS = (Xapian::MSetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSetIterator__get_percent object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_percent())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESetIterator__next, 2)
    T_NUMBER(XapianESetIterator__next, 0)
    Xapian::ESetIterator * CONCEPT_THIS = (Xapian::ESetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESetIterator__next object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)&CONCEPT_THIS->operator++())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESetIterator__prev, 2)
    T_NUMBER(XapianESetIterator__prev, 0)
    Xapian::ESetIterator * CONCEPT_THIS = (Xapian::ESetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESetIterator__prev object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)&CONCEPT_THIS->operator--())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESetIterator__get_value, 2)
    T_NUMBER(XapianESetIterator__get_value, 0)
    Xapian::ESetIterator * CONCEPT_THIS = (Xapian::ESetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESetIterator__get_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->operator*().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESetIterator__get_description, 2)
    T_NUMBER(XapianESetIterator__get_description, 0)
    Xapian::ESetIterator * CONCEPT_THIS = (Xapian::ESetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESetIterator__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESetIterator__equal, 3)
    T_NUMBER(XapianESetIterator__equal, 0)
    Xapian::ESetIterator * CONCEPT_THIS = (Xapian::ESetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESetIterator__equal object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianESetIterator__equal, 2)
    Xapian::ESetIterator * iter = (Xapian::ESetIterator *)PARAM_INT(2);
    if (!iter) {
        RETURN_NUMBER(0);
        return 0;
    }
    try {
        RETURN_NUMBER(*CONCEPT_THIS == *iter)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESetIterator__get_weight, 2)
    T_NUMBER(XapianESetIterator__get_weight, 0)
    Xapian::ESetIterator * CONCEPT_THIS = (Xapian::ESetIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESetIterator__get_weight object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_weight())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPostingIterator__next, 2)
    T_NUMBER(XapianPostingIterator__next, 0)
    Xapian::PostingIterator * CONCEPT_THIS = (Xapian::PostingIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPostingIterator__next object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)&CONCEPT_THIS->operator++())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPostingIterator__get_value, 2)
    T_NUMBER(XapianPostingIterator__get_value, 0)
    Xapian::PostingIterator * CONCEPT_THIS = (Xapian::PostingIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPostingIterator__get_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT)CONCEPT_THIS->operator*())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPostingIterator__get_description, 2)
    T_NUMBER(XapianPostingIterator__get_description, 0)
    Xapian::PostingIterator * CONCEPT_THIS = (Xapian::PostingIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPostingIterator__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPostingIterator__equal, 3)
    T_NUMBER(XapianPostingIterator__equal, 0)
    Xapian::PostingIterator * CONCEPT_THIS = (Xapian::PostingIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPostingIterator__equal object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianPostingIterator__equal, 2)
    Xapian::PostingIterator * iter = (Xapian::PostingIterator *)PARAM_INT(2);
    if (!iter) {
        RETURN_NUMBER(0);
        return 0;
    }
    try {
        RETURN_NUMBER(*CONCEPT_THIS == *iter)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPostingIterator__get_wdf, 2)
    T_NUMBER(XapianPostingIterator__get_wdf, 0)
    Xapian::PostingIterator * CONCEPT_THIS = (Xapian::PostingIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPostingIterator__get_wdf object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_wdf())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianPostingIterator__get_doclength, 2)
    T_NUMBER(XapianPostingIterator__get_doclength, 0)
    Xapian::PostingIterator * CONCEPT_THIS = (Xapian::PostingIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPostingIterator__get_doclength object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_doclength())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::PositionIterator temp_post_begin;
CONCEPT_FUNCTION_IMPL(XapianPostingIterator__positionlist_begin, 2)
    T_NUMBER(XapianPostingIterator__positionlist_begin, 0)
    Xapian::PostingIterator * CONCEPT_THIS = (Xapian::PostingIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPostingIterator__positionlist_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        temp_post_begin = CONCEPT_THIS->positionlist_begin();
        RETURN_NUMBER((SYS_INT)&temp_post_begin)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::PositionIterator temp_post_end;
CONCEPT_FUNCTION_IMPL(XapianPostingIterator__positionlist_end, 2)
    T_NUMBER(XapianPostingIterator__positionlist_end, 0)
    Xapian::PostingIterator * CONCEPT_THIS = (Xapian::PostingIterator *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianPostingIterator__positionlist_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        temp_post_end = CONCEPT_THIS->positionlist_end();
        RETURN_NUMBER((SYS_INT)&temp_post_end)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabaseCreate, 1)
    SET_STRING(0, (char *)"");
    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Database())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabaseCreate2, 2)
    SET_STRING(0, (char *)"");
    T_STRING(XapianDatabaseCreate2, 1)

    try {
        Xapian::Database *handle = new Xapian::Database(PARAM(1));
        RETURN_NUMBER((SYS_INT)handle);
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabaseDestroy, 2)
    T_NUMBER(XapianDatabaseDestroy, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__add_database, 3)
    T_NUMBER(XapianDatabase__add_database, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__add_database object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDatabase__add_database, 2)
    Xapian::Database * database = (Xapian::Database *)PARAM_INT(2);
    if (!database) {
        return (void *)"XapianDatabase__add_database: database is null";
    }
    try {
        CONCEPT_THIS->add_database(*database);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__reopen, 2)
    T_NUMBER(XapianDatabase__reopen, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__reopen object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        CONCEPT_THIS->reopen();
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_description, 2)
    T_NUMBER(XapianDatabase__get_description, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::PostingIterator XapianDatabase__postlist_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__postlist_begin, 3)
    T_NUMBER(XapianDatabase__postlist_begin, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__postlist_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__postlist_begin, 2)
    std::string tname = (std::string)PARAM(2);
    try {
        XapianDatabase__postlist_beginIterator = CONCEPT_THIS->postlist_begin(tname);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__postlist_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::PostingIterator XapianDatabase__postlist_endIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__postlist_end, 3)
    T_NUMBER(XapianDatabase__postlist_end, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__postlist_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__postlist_end, 2)
    std::string tname = (std::string)PARAM(2);
    try {
        XapianDatabase__postlist_endIterator = CONCEPT_THIS->postlist_end(tname);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__postlist_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__termlist_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__termlist_begin, 3)
    T_NUMBER(XapianDatabase__termlist_begin, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__termlist_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDatabase__termlist_begin, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    try {
        XapianDatabase__termlist_beginIterator = CONCEPT_THIS->termlist_begin(did);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__termlist_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__termlist_endIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__termlist_end, 3)
    T_NUMBER(XapianDatabase__termlist_end, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__termlist_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDatabase__termlist_end, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    try {
        XapianDatabase__termlist_endIterator = CONCEPT_THIS->termlist_end(did);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__termlist_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__has_positions, 2)
    T_NUMBER(XapianDatabase__has_positions, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__has_positions object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->has_positions())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::PositionIterator XapianDatabase__positionlist_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__positionlist_begin, 4)
    T_NUMBER(XapianDatabase__positionlist_begin, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__positionlist_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDatabase__positionlist_begin, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    T_STRING(XapianDatabase__positionlist_begin, 3)
    std::string tname = (std::string)PARAM(3);
    try {
        XapianDatabase__positionlist_beginIterator = CONCEPT_THIS->positionlist_begin(did, tname);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__positionlist_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::PositionIterator XapianDatabase__positionlist_endIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__positionlist_end, 4)
    T_NUMBER(XapianDatabase__positionlist_end, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__positionlist_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDatabase__positionlist_end, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    T_STRING(XapianDatabase__positionlist_end, 3)
    std::string tname = (std::string)PARAM(3);
    try {
        XapianDatabase__positionlist_endIterator = CONCEPT_THIS->positionlist_end(did, tname);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__positionlist_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__allterms_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__allterms_begin, 2)
    T_NUMBER(XapianDatabase__allterms_begin, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__allterms_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianDatabase__allterms_beginIterator = CONCEPT_THIS->allterms_begin();
        RETURN_NUMBER((SYS_INT)&XapianDatabase__allterms_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__allterms_endIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__allterms_end, 2)
    T_NUMBER(XapianDatabase__allterms_end, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__allterms_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianDatabase__allterms_endIterator = CONCEPT_THIS->allterms_end();
        RETURN_NUMBER((SYS_INT)&XapianDatabase__allterms_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__allterms_begin2Iterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__allterms_begin2, 3)
    T_NUMBER(XapianDatabase__allterms_begin2, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__allterms_begin2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__allterms_begin2, 2)
    std::string prefix = (std::string)PARAM(2);
    try {
        XapianDatabase__allterms_begin2Iterator = CONCEPT_THIS->allterms_begin(prefix);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__allterms_begin2Iterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__allterms_end2Iterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__allterms_end2, 3)
    T_NUMBER(XapianDatabase__allterms_end2, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__allterms_end2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__allterms_end2, 2)
    std::string prefix = (std::string)PARAM(2);
    try {
        XapianDatabase__allterms_end2Iterator = CONCEPT_THIS->allterms_end(prefix);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__allterms_end2Iterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_doccount, 2)
    T_NUMBER(XapianDatabase__get_doccount, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_doccount object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_doccount())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_lastdocid, 2)
    T_NUMBER(XapianDatabase__get_lastdocid, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_lastdocid object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_lastdocid())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_avlength, 2)
    T_NUMBER(XapianDatabase__get_avlength, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_avlength object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_avlength())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_termfreq, 3)
    T_NUMBER(XapianDatabase__get_termfreq, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_termfreq object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__get_termfreq, 2)
    std::string tname = (std::string)PARAM(2);
    try {
        RETURN_NUMBER(CONCEPT_THIS->get_termfreq(tname))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__term_exists, 3)
    T_NUMBER(XapianDatabase__term_exists, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__term_exists object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__term_exists, 2)
    std::string tname = (std::string)PARAM(2);
    try {
        RETURN_NUMBER(CONCEPT_THIS->term_exists(tname))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_collection_freq, 3)
    T_NUMBER(XapianDatabase__get_collection_freq, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_collection_freq object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__get_collection_freq, 2)
    std::string tname = (std::string)PARAM(2);
    try {
        RETURN_NUMBER(CONCEPT_THIS->get_collection_freq(tname))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_doclength, 3)
    T_NUMBER(XapianDatabase__get_doclength, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_doclength object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDatabase__get_doclength, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    try {
        RETURN_NUMBER(CONCEPT_THIS->get_doclength(did))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__keep_alive, 2)
    T_NUMBER(XapianDatabase__keep_alive, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__keep_alive object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        CONCEPT_THIS->keep_alive();
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_document, 3)
    T_NUMBER(XapianDatabase__get_document, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_document object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianDatabase__get_document, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Document(CONCEPT_THIS->get_document(did)))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_spelling_suggestion, 4)
    T_NUMBER(XapianDatabase__get_spelling_suggestion, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_spelling_suggestion object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__get_spelling_suggestion, 2)
    std::string word = (std::string)PARAM(2);
    T_NUMBER(XapianDatabase__get_spelling_suggestion, 3)
    unsigned max_edit_distance = (unsigned)PARAM(3);
    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_spelling_suggestion(word, max_edit_distance).c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------

/*Xapian::TermIterator XapianDatabase__spellings_beginIterator;
   CONCEPT_FUNCTION_IMPL(XapianDatabase__spellings_begin, 3)
        T_NUMBER(XapianDatabase__spellings_begin, 0)
        Xapian::Database *CONCEPT_THIS=(Xapian::Database *)PARAM_INT(0);
        if (!CONCEPT_THIS) {
                return (void *)"XapianDatabase__spellings_begin object reference is null (first parameter)";
        }
        SET_STRING(1, (char *)"");

        T_STRING(XapianDatabase__spellings_begin, 2)
        std::string term=(std::string)PARAM(2);
        try {
                XapianDatabase__spellings_beginIterator=CONCEPT_THIS->spellings_begin(term);
                RETURN_NUMBER((SYS_INT)&XapianDatabase__spellings_beginIterator)
        } catch (const Xapian::Error & error) {
                SET_STRING(1, (char *)error.get_msg().c_str());
        }
   END_IMPL
   //------------------------------------------------------------------------
   Xapian::TermIterator XapianDatabase__spellings_endIterator;
   CONCEPT_FUNCTION_IMPL(XapianDatabase__spellings_end, 3)
        T_NUMBER(XapianDatabase__spellings_end, 0)
        Xapian::Database *CONCEPT_THIS=(Xapian::Database *)PARAM_INT(0);
        if (!CONCEPT_THIS) {
                return (void *)"XapianDatabase__spellings_end object reference is null (first parameter)";
        }
        SET_STRING(1, (char *)"");

        T_STRING(XapianDatabase__spellings_end, 2)
        std::string term=(std::string)PARAM(2);
        try {
                XapianDatabase__spellings_endIterator=CONCEPT_THIS->spellings_end(term);
                RETURN_NUMBER((SYS_INT)&XapianDatabase__spellings_endIterator)
        } catch (const Xapian::Error & error) {
                SET_STRING(1, (char *)error.get_msg().c_str());
        }
   END_IMPL*/
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__synonyms_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__synonyms_begin, 3)
    T_NUMBER(XapianDatabase__synonyms_begin, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__synonyms_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__synonyms_begin, 2)
    std::string term = (std::string)PARAM(2);
    try {
        XapianDatabase__synonyms_beginIterator = CONCEPT_THIS->synonyms_begin(term);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__synonyms_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__synonyms_endIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__synonyms_end, 3)
    T_NUMBER(XapianDatabase__synonyms_end, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__synonyms_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__synonyms_end, 2)
    std::string term = (std::string)PARAM(2);
    try {
        XapianDatabase__synonyms_endIterator = CONCEPT_THIS->synonyms_end(term);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__synonyms_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__synonym_keys_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__synonym_keys_begin, 3)
    T_NUMBER(XapianDatabase__synonym_keys_begin, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__synonym_keys_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__synonym_keys_begin, 2)
    std::string term = (std::string)PARAM(2);
    try {
        XapianDatabase__synonym_keys_beginIterator = CONCEPT_THIS->synonym_keys_begin(term);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__synonym_keys_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__synonym_keys_endIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__synonym_keys_end, 3)
    T_NUMBER(XapianDatabase__synonym_keys_end, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__synonym_keys_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__synonym_keys_end, 2)
    std::string term = (std::string)PARAM(2);
    try {
        XapianDatabase__synonym_keys_endIterator = CONCEPT_THIS->synonym_keys_end(term);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__synonym_keys_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDatabase__get_metadata, 3)
    T_NUMBER(XapianDatabase__get_metadata, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__get_metadata object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__get_metadata, 2)
    std::string key = (std::string)PARAM(2);
    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_metadata(key).c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__metadata_keys_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__metadata_keys_begin, 3)
    T_NUMBER(XapianDatabase__metadata_keys_begin, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__metadata_keys_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__metadata_keys_begin, 2)
    std::string term = (std::string)PARAM(2);
    try {
        XapianDatabase__metadata_keys_beginIterator = CONCEPT_THIS->metadata_keys_begin(term);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__metadata_keys_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianDatabase__metadata_keys_endIterator;
CONCEPT_FUNCTION_IMPL(XapianDatabase__metadata_keys_end, 3)
    T_NUMBER(XapianDatabase__metadata_keys_end, 0)
    Xapian::Database * CONCEPT_THIS = (Xapian::Database *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianDatabase__metadata_keys_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianDatabase__metadata_keys_end, 2)
    std::string term = (std::string)PARAM(2);
    try {
        XapianDatabase__metadata_keys_endIterator = CONCEPT_THIS->metadata_keys_end(term);
        RETURN_NUMBER((SYS_INT)&XapianDatabase__metadata_keys_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryCreate, 1)
    SET_STRING(0, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Query())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryCreate2, 4)
    SET_STRING(0, (char *)"");
    T_STRING(XapianQueryCreate2, 1)
    T_NUMBER(XapianQueryCreate2, 2)
    T_NUMBER(XapianQueryCreate2, 3)

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Query(PARAM(1), PARAM_INT(2), PARAM_INT(3)))
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryCreate3, 4)
    SET_STRING(0, (char *)"");
    T_NUMBER(XapianQueryCreate3, 1)
    T_NUMBER(XapianQueryCreate3, 2)
    T_NUMBER(XapianQueryCreate3, 3)

    Xapian::Query * left = (Xapian::Query *)PARAM_INT(2);
    Xapian::Query *right = (Xapian::Query *)PARAM_INT(3);

    if (!left) {
        return (void *)"XapianQueryCreate3: left query is null";
    }
    if (!right) {
        return (void *)"XapianQueryCreate3: left query is null";
    }

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Query((Xapian::Query::op)PARAM_INT(1), *left, *right))
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryCreate4, 4)
    SET_STRING(0, (char *)"");
    T_NUMBER(XapianQueryCreate4, 1)
    T_STRING(XapianQueryCreate4, 2)
    T_STRING(XapianQueryCreate4, 3)

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Query((Xapian::Query::op)PARAM_INT(1), (std::string)PARAM(2), (std::string)PARAM(3)))
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryCreate5, 4)
    SET_STRING(0, (char *)"");
    T_NUMBER(XapianQueryCreate5, 1)
    T_NUMBER(XapianQueryCreate5, 2)
    T_STRING(XapianQueryCreate5, 3)

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Query((Xapian::Query::op)PARAM_INT(1), (Xapian::valueno)PARAM_INT(2), (std::string)PARAM(3)))
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryDestroy, 2)
    T_NUMBER(XapianQueryDestroy, 0)
    Xapian::Query * CONCEPT_THIS = (Xapian::Query *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQuery__get_length, 2)
    T_NUMBER(XapianQuery__get_length, 0)
    Xapian::Query * CONCEPT_THIS = (Xapian::Query *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQuery__get_length object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_length())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianQuery__get_terms_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianQuery__get_terms_begin, 2)
    T_NUMBER(XapianQuery__get_terms_begin, 0)
    Xapian::Query * CONCEPT_THIS = (Xapian::Query *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQuery__get_terms_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianQuery__get_terms_beginIterator = CONCEPT_THIS->get_terms_begin();
        RETURN_NUMBER((SYS_INT)&XapianQuery__get_terms_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianQuery__get_terms_endIterator;
CONCEPT_FUNCTION_IMPL(XapianQuery__get_terms_end, 2)
    T_NUMBER(XapianQuery__get_terms_end, 0)
    Xapian::Query * CONCEPT_THIS = (Xapian::Query *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQuery__get_terms_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianQuery__get_terms_endIterator = CONCEPT_THIS->get_terms_end();
        RETURN_NUMBER((SYS_INT)&XapianQuery__get_terms_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQuery__empty, 2)
    T_NUMBER(XapianQuery__empty, 0)
    Xapian::Query * CONCEPT_THIS = (Xapian::Query *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQuery__empty object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->empty())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQuery__get_description, 2)
    T_NUMBER(XapianQuery__get_description, 0)
    Xapian::Query * CONCEPT_THIS = (Xapian::Query *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQuery__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(XapianQuery__serialise, 2)
        T_NUMBER(XapianQuery__get_description, 0)
        Xapian::Query *CONCEPT_THIS=(Xapian::Query *)PARAM_INT(0);
        if (!CONCEPT_THIS) {
                return (void *)"XapianQuery__serialise object reference is null (first parameter)";
        }
        SET_STRING(1, (char *)"");

        try {
                RETURN_STRING((char *)CONCEPT_THIS->serialise().c_str())
        } catch (const Xapian::Error & error) {
                SET_STRING(1, (char *)error.get_msg().c_str());
        }
   END_IMPL*/
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(XapianQuery__get_parameter, 2)
        T_NUMBER(XapianQuery__get_description, 0)
        Xapian::Query *CONCEPT_THIS=(Xapian::Query *)PARAM_INT(0);
        if (!CONCEPT_THIS) {
                return (void *)"XapianQuery__get_parameter object reference is null (first parameter)";
        }
        SET_STRING(1, (char *)"");

        try {
                RETURN_NUMBER(CONCEPT_THIS->get_parameter())
        } catch (const Xapian::Error & error) {
                SET_STRING(1, (char *)error.get_msg().c_str());
        }
   END_IMPL*/
//------------------------------------------------------------------------

/*Xapian::TermIterator XapianQuery__get_termsIterator;
   CONCEPT_FUNCTION_IMPL(XapianQuery__get_terms, 2)
        T_NUMBER(XapianQuery__get_terms, 0)
        Xapian::Query *CONCEPT_THIS=(Xapian::Query *)PARAM_INT(0);
        if (!CONCEPT_THIS) {
                return (void *)"XapianQuery__get_terms object reference is null (first parameter)";
        }
        SET_STRING(1, (char *)"");

        try {
                XapianQuery__get_termsIterator=CONCEPT_THIS->get_terms();
                RETURN_NUMBER((SYS_INT)&XapianQuery__get_termsIterator)
        } catch (const Xapian::Error & error) {
                SET_STRING(1, (char *)error.get_msg().c_str());
        }
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParserCreate, 1)
    SET_STRING(0, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::QueryParser())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParserDestroy, 2)
    T_NUMBER(XapianQueryParserDestroy, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__set_stemmer, 3)
    T_NUMBER(XapianQueryParser__set_stemmer, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__set_stemmer object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianQueryParser__set_stemmer, 2)
    Xapian::Stem * stemmer = (Xapian::Stem *)PARAM_INT(2);
    if (!stemmer) {
        return (void *)"XapianQueryParser__set_stemmer: stemmer is null";
    }
    try {
        CONCEPT_THIS->set_stemmer(*stemmer);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__set_stemming_strategy, 3)
    T_NUMBER(XapianQueryParser__set_stemming_strategy, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__set_stem_strategy object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianQueryParser__set_stemming_strategy, 2)
    Xapian::QueryParser::stem_strategy strategy = (Xapian::QueryParser::stem_strategy)PARAM_INT(2);
    try {
        CONCEPT_THIS->set_stemming_strategy(strategy);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__set_stopper, 3)
    T_NUMBER(XapianQueryParser__set_stopper, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__set_stopper object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianQueryParser__set_stopper, 2)
    Xapian::Stopper * stop = (Xapian::Stopper *)PARAM_INT(2);
    try {
        CONCEPT_THIS->set_stopper(stop);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__set_default_op, 3)
    T_NUMBER(XapianQueryParser__set_default_op, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__set_default_op object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianQueryParser__set_default_op, 2)
    Xapian::Query::op default_op = (Xapian::Query::op)PARAM(2);
    try {
        CONCEPT_THIS->set_default_op(default_op);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__get_default_op, 2)
    T_NUMBER(XapianQueryParser__get_default_op, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__get_default_op object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_default_op())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__set_database, 3)
    T_NUMBER(XapianQueryParser__set_database, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__set_database object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianQueryParser__set_database, 2)
    Xapian::Database * db = (Xapian::Database *)PARAM_INT(2);
    if (!db) {
        return (void *)"XapianQueryParser__set_database: database is null";
    }
    try {
        CONCEPT_THIS->set_database(*db);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__parse_query, 5)
    T_NUMBER(XapianQueryParser__parse_query, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__parse_query object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianQueryParser__parse_query, 2)
    std::string query_string;
    query_string.append((const char *)PARAM(2), (size_t)PARAM_LEN(2));
    T_NUMBER(XapianQueryParser__parse_query, 3)
    unsigned flags = (unsigned)PARAM(3);
    T_STRING(XapianQueryParser__parse_query, 4)
    std::string default_prefix = (std::string)PARAM(4);
    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Query(CONCEPT_THIS->parse_query(query_string, flags, default_prefix)))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__add_prefix, 4)
    T_NUMBER(XapianQueryParser__add_prefix, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__add_prefix object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianQueryParser__add_prefix, 2)
    std::string field = (std::string)PARAM(2);
    T_STRING(XapianQueryParser__add_prefix, 3)
    std::string prefix = (std::string)PARAM(3);
    try {
        CONCEPT_THIS->add_prefix(field, prefix);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__add_boolean_prefix, 4)
    T_NUMBER(XapianQueryParser__add_boolean_prefix, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__add_boolean_prefix object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianQueryParser__add_boolean_prefix, 2)
    std::string field = (std::string)PARAM(2);
    T_STRING(XapianQueryParser__add_boolean_prefix, 3)
    std::string prefix = (std::string)PARAM(3);
    try {
        CONCEPT_THIS->add_boolean_prefix(field, prefix);
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__add_valuerangeprocessor, 3)
    T_NUMBER(XapianQueryParser__add_valuerangeprocessor, 0)
    SET_STRING(1, (char *)"");
    T_NUMBER(XapianQueryParser__add_valuerangeprocessor, 2)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__add_valuerangeprocessor object reference is null (first parameter)";
    }
    Xapian::ValueRangeProcessor *VRP = (Xapian::ValueRangeProcessor *)PARAM_INT(2);
    if (!VRP) {
        return (void *)"XapianQueryParser__add_valuerangeprocessor object reference is null (3rd parameter)";
    }

    try {
        CONCEPT_THIS->add_valuerangeprocessor(VRP);
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianQueryParser__stoplist_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianQueryParser__stoplist_begin, 2)
    T_NUMBER(XapianQueryParser__stoplist_begin, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__stoplist_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianQueryParser__stoplist_beginIterator = CONCEPT_THIS->stoplist_begin();
        RETURN_NUMBER((SYS_INT)&XapianQueryParser__stoplist_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianQueryParser__stoplist_endIterator;
CONCEPT_FUNCTION_IMPL(XapianQueryParser__stoplist_end, 2)
    T_NUMBER(XapianQueryParser__stoplist_end, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__stoplist_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianQueryParser__stoplist_endIterator = CONCEPT_THIS->stoplist_end();
        RETURN_NUMBER((SYS_INT)&XapianQueryParser__stoplist_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianQueryParser__unstem_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianQueryParser__unstem_begin, 3)
    T_NUMBER(XapianQueryParser__unstem_begin, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__unstem_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianQueryParser__unstem_begin, 2)
    std::string term = (std::string)PARAM(2);
    try {
        XapianQueryParser__unstem_beginIterator = CONCEPT_THIS->unstem_begin(term);
        RETURN_NUMBER((SYS_INT)&XapianQueryParser__unstem_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianQueryParser__unstem_endIterator;
CONCEPT_FUNCTION_IMPL(XapianQueryParser__unstem_end, 3)
    T_NUMBER(XapianQueryParser__unstem_end, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__unstem_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianQueryParser__unstem_end, 2)
    std::string term = (std::string)PARAM(2);
    try {
        XapianQueryParser__unstem_endIterator = CONCEPT_THIS->unstem_end(term);
        RETURN_NUMBER((SYS_INT)&XapianQueryParser__unstem_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__get_corrected_query_string, 2)
    T_NUMBER(XapianQueryParser__get_corrected_query_string, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__get_corrected_query_string object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_corrected_query_string().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianQueryParser__get_description, 2)
    T_NUMBER(XapianQueryParser__get_description, 0)
    Xapian::QueryParser * CONCEPT_THIS = (Xapian::QueryParser *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianQueryParser__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESetCreate, 1)
    SET_STRING(0, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::ESet())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESetDestroy, 2)
    T_NUMBER(XapianESetDestroy, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESet__get_ebound, 2)
    T_NUMBER(XapianESet__get_ebound, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__get_ebound object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_ebound())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESet__size, 2)
    T_NUMBER(XapianESet__size, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__size object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->size())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESet__max_size, 2)
    T_NUMBER(XapianESet__max_size, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__max_size object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->max_size())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESet__empty, 2)
    T_NUMBER(XapianESet__empty, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__empty object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->empty())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESet__swap, 3)
    T_NUMBER(XapianESet__swap, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__swap object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianESet__swap, 2)
    Xapian::ESet * other = (Xapian::ESet *)PARAM_INT(2);
    if (!other) {
        return (void *)"XapianESet__swap: other esset is null";
    }
    try {
        CONCEPT_THIS->swap(*other);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::ESetIterator XapianESet__beginIterator;
CONCEPT_FUNCTION_IMPL(XapianESet__begin, 2)
    T_NUMBER(XapianESet__begin, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianESet__beginIterator = CONCEPT_THIS->begin();
        RETURN_NUMBER((SYS_INT)&XapianESet__beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::ESetIterator XapianESet__endIterator;
CONCEPT_FUNCTION_IMPL(XapianESet__end, 2)
    T_NUMBER(XapianESet__end, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianESet__endIterator = CONCEPT_THIS->end();
        RETURN_NUMBER((SYS_INT)&XapianESet__endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::ESetIterator XapianESet__backIterator;
CONCEPT_FUNCTION_IMPL(XapianESet__back, 2)
    T_NUMBER(XapianESet__back, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__back object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianESet__backIterator = CONCEPT_THIS->back();
        RETURN_NUMBER((SYS_INT)&XapianESet__backIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::ESetIterator XapianESet__getiterIterator;
CONCEPT_FUNCTION_IMPL(XapianESet__getiter, 3)
    T_NUMBER(XapianESet__getiter, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__getiter object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianESet__getiter, 2)
    int index = (int)PARAM(2);
    try {
        XapianESet__getiterIterator = CONCEPT_THIS->operator[](index);
        RETURN_NUMBER((SYS_INT)&XapianESet__getiterIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianESet__get_description, 2)
    T_NUMBER(XapianESet__get_description, 0)
    Xapian::ESet * CONCEPT_THIS = (Xapian::ESet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianESet__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetCreate, 1)
    SET_STRING(0, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::MSet())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSetDestroy, 2)
    T_NUMBER(XapianMSetDestroy, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__fetch, 2)
    T_NUMBER(XapianMSet__fetch, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__fetch object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        CONCEPT_THIS->fetch();
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__convert_to_percent, 3)
    T_NUMBER(XapianMSet__convert_to_percent, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__convert_to_percent object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianMSet__convert_to_percent, 2)
    Xapian::weight wt = (Xapian::weight)PARAM(2);
    try {
        RETURN_NUMBER(CONCEPT_THIS->convert_to_percent(wt))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__convert_to_percent2, 3)
    T_NUMBER(XapianMSet__convert_to_percent2, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__convert_to_percent2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianMSet__convert_to_percent2, 2)
    Xapian::MSetIterator * it = (Xapian::MSetIterator *)PARAM_INT(2);
    if (!it) {
        return (void *)"XapianMSet__convert_to_percent2: iterator is null";
    }
    try {
        RETURN_NUMBER(CONCEPT_THIS->convert_to_percent(*it))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__get_termfreq, 3)
    T_NUMBER(XapianMSet__get_termfreq, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__get_termfreq object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianMSet__get_termfreq, 2)
    std::string tname = (std::string)PARAM(2);
    try {
        RETURN_NUMBER(CONCEPT_THIS->get_termfreq(tname))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__get_termweight, 3)
    T_NUMBER(XapianMSet__get_termweight, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__get_termweight object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_STRING(XapianMSet__get_termweight, 2)
    std::string tname = (std::string)PARAM(2);
    try {
        RETURN_NUMBER(CONCEPT_THIS->get_termweight(tname))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__get_firstitem, 2)
    T_NUMBER(XapianMSet__get_firstitem, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__get_firstitem object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_firstitem())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__get_matches_lower_bound, 2)
    T_NUMBER(XapianMSet__get_matches_lower_bound, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__get_matches_lower_bound object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_matches_lower_bound())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__get_matches_estimated, 2)
    T_NUMBER(XapianMSet__get_matches_estimated, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__get_matches_estimated object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_matches_estimated())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__get_matches_upper_bound, 2)
    T_NUMBER(XapianMSet__get_matches_upper_bound, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__get_matches_upper_bound object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_matches_upper_bound())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__get_max_possible, 2)
    T_NUMBER(XapianMSet__get_max_possible, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__get_max_possible object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_max_possible())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__get_max_attained, 2)
    T_NUMBER(XapianMSet__get_max_attained, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__get_max_attained object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->get_max_attained())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__size, 2)
    T_NUMBER(XapianMSet__size, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__size object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->size())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__max_size, 2)
    T_NUMBER(XapianMSet__max_size, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__max_size object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->max_size())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__empty, 2)
    T_NUMBER(XapianMSet__empty, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__empty object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->empty())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__swap, 3)
    T_NUMBER(XapianMSet__swap, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__swap object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianMSet__swap, 2)
    Xapian::MSet * other = (Xapian::MSet *)PARAM_INT(2);
    if (!other) {
        return (void *)"XapianMSet__swap: other reference is null";
    }
    try {
        CONCEPT_THIS->swap(*other);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::MSetIterator XapianMSet__beginIterator;
CONCEPT_FUNCTION_IMPL(XapianMSet__begin, 2)
    T_NUMBER(XapianMSet__begin, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianMSet__beginIterator = CONCEPT_THIS->begin();
        RETURN_NUMBER((SYS_INT)&XapianMSet__beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::MSetIterator XapianMSet__endIterator;
CONCEPT_FUNCTION_IMPL(XapianMSet__end, 2)
    T_NUMBER(XapianMSet__end, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianMSet__endIterator = CONCEPT_THIS->end();
        RETURN_NUMBER((SYS_INT)&XapianMSet__endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::MSetIterator XapianMSet__backIterator;
CONCEPT_FUNCTION_IMPL(XapianMSet__back, 2)
    T_NUMBER(XapianMSet__back, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__back object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianMSet__backIterator = CONCEPT_THIS->back();
        RETURN_NUMBER((SYS_INT)&XapianMSet__backIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::MSetIterator XapianMSet__getiterIterator;
CONCEPT_FUNCTION_IMPL(XapianMSet__getiter, 3)
    T_NUMBER(XapianMSet__getiter, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__getiter object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianMSet__getiter, 2)
    Xapian::doccount _i = (Xapian::doccount)PARAM(2);
    try {
        XapianMSet__getiterIterator = CONCEPT_THIS->operator[](_i);
        RETURN_NUMBER((SYS_INT)&XapianMSet__getiterIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianMSet__get_description, 2)
    T_NUMBER(XapianMSet__get_description, 0)
    Xapian::MSet * CONCEPT_THIS = (Xapian::MSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianMSet__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSetCreate, 1)
    SET_STRING(0, (char *)"");

    try {
        RETURN_NUMBER((SYS_INT) new Xapian::RSet())
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSetDestroy, 2)
    T_NUMBER(XapianRSetDestroy, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSet__size, 2)
    T_NUMBER(XapianRSet__size, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianRSet__size object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->size())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSet__empty, 2)
    T_NUMBER(XapianRSet__empty, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianRSet__empty object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_NUMBER(CONCEPT_THIS->empty())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSet__add_document, 3)
    T_NUMBER(XapianRSet__add_document, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianRSet__add_document object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianRSet__add_document, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    try {
        CONCEPT_THIS->add_document(did);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSet__add_document2, 3)
    T_NUMBER(XapianRSet__add_document2, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianRSet__add_document2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianRSet__add_document2, 2)
    Xapian::MSetIterator * it = (Xapian::MSetIterator *)PARAM_INT(2);
    if (!it) {
        return (void *)"XapianRSet__add_document2: iterator is null";
    }
    try {
        CONCEPT_THIS->add_document(*it);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSet__remove_document, 3)
    T_NUMBER(XapianRSet__remove_document, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianRSet__remove_document object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianRSet__remove_document, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    try {
        CONCEPT_THIS->remove_document(did);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSet__remove_document2, 3)
    T_NUMBER(XapianRSet__remove_document2, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianRSet__remove_document2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianRSet__remove_document2, 2)
    Xapian::MSetIterator * it = (Xapian::MSetIterator *)PARAM_INT(2);
    if (!it) {
        return (void *)"XapianRSet__remove_document2: iterator is null";
    }
    try {
        CONCEPT_THIS->remove_document(*it);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSet__contains, 3)
    T_NUMBER(XapianRSet__contains, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianRSet__contains object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianRSet__contains, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    try {
        RETURN_NUMBER(CONCEPT_THIS->contains(did))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSet__contains2, 3)
    T_NUMBER(XapianRSet__contains2, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianRSet__contains object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianRSet__contains2, 2)
    Xapian::MSetIterator * it = (Xapian::MSetIterator *)PARAM_INT(2);
    if (!it) {
        return (void *)"XapianRSet__contains: iterator is null";
    }
    try {
        RETURN_NUMBER(CONCEPT_THIS->contains(*it))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianRSet__get_description, 2)
    T_NUMBER(XapianRSet__get_description, 0)
    Xapian::RSet * CONCEPT_THIS = (Xapian::RSet *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianRSet__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquireCreate, 2)
    SET_STRING(0, (char *)"");

    T_NUMBER(XapianEnquireCreate, 1)

    Xapian::Database * db = (Xapian::Database *)PARAM_INT(1);
    if (!db) {
        return (void *)"XapianEnquireCreate: database is null";
    }
    try {
        RETURN_NUMBER((SYS_INT) new Xapian::Enquire(*db))
    } catch (const Xapian::Error& error) {
        SET_STRING(0, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquireDestroy, 2)
    T_NUMBER(XapianEnquireDestroy, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    SET_STRING(1, (char *)"");

    try {
        delete CONCEPT_THIS;
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__set_query, 4)
    T_NUMBER(XapianEnquire__set_query, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__set_query object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__set_query, 2)
    Xapian::Query * query = (Xapian::Query *)PARAM_INT(2);
    if (!query) {
        return (void *)"XapianEnquire__set_query: query is null";
    }
    T_NUMBER(XapianEnquire__set_query, 3)
    Xapian::termcount qlen = (Xapian::termcount)PARAM_INT(3);
    try {
        CONCEPT_THIS->set_query(*query, qlen);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::Query XapianEnquire__get_queryIterator;
CONCEPT_FUNCTION_IMPL(XapianEnquire__get_query, 2)
    T_NUMBER(XapianEnquire__get_query, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__get_query object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        XapianEnquire__get_queryIterator = CONCEPT_THIS->get_query();
        RETURN_NUMBER((SYS_INT)&XapianEnquire__get_queryIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__set_weighting_scheme, 3)
    T_NUMBER(XapianEnquire__set_weighting_scheme, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__set_weighting_scheme object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__set_weighting_scheme, 2)
    Xapian::Weight * weight = (Xapian::Weight *)PARAM_INT(2);
    if (!weight) {
        return (void *)"XapianEnquire__set_weighting_scheme: weight is null";
    }
    try {
        CONCEPT_THIS->set_weighting_scheme(*weight);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__set_collapse_key, 3)
    T_NUMBER(XapianEnquire__set_collapse_key, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__set_collapse_key object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__set_collapse_key, 2)
    Xapian::valueno collapse_key = (Xapian::valueno)PARAM(2);
    try {
        CONCEPT_THIS->set_collapse_key(collapse_key);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__set_docid_order, 3)
    T_NUMBER(XapianEnquire__set_docid_order, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__set_docid_order object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__set_docid_order, 2)
    Xapian::Enquire::docid_order order = (Xapian::Enquire::docid_order)PARAM(2);
    try {
        CONCEPT_THIS->set_docid_order(order);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__set_cutoff, 4)
    T_NUMBER(XapianEnquire__set_cutoff, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__set_cutoff object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__set_cutoff, 2)
    Xapian::percent percent_cutoff = (Xapian::percent)PARAM(2);
    T_NUMBER(XapianEnquire__set_cutoff, 3)
    Xapian::weight weight_cutoff = (Xapian::weight)PARAM(3);
    try {
        CONCEPT_THIS->set_cutoff(percent_cutoff, weight_cutoff);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__set_sort_by_relevance, 2)
    T_NUMBER(XapianEnquire__set_sort_by_relevance, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__set_sort_by_relevance object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        CONCEPT_THIS->set_sort_by_relevance();
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__set_sort_by_value, 4)
    T_NUMBER(XapianEnquire__set_sort_by_value, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__set_sort_by_value object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__set_sort_by_value, 2)
    Xapian::valueno sort_key = (Xapian::valueno)PARAM(2);
    T_NUMBER(XapianEnquire__set_sort_by_value, 3)
    bool reverse = (bool)PARAM(3);
    try {
        CONCEPT_THIS->set_sort_by_value(sort_key, reverse);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__set_sort_by_value_then_relevance, 4)
    T_NUMBER(XapianEnquire__set_sort_by_value_then_relevance, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__set_sort_by_value_then_relevance object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__set_sort_by_value_then_relevance, 2)
    Xapian::valueno sort_key = (Xapian::valueno)PARAM(2);
    T_NUMBER(XapianEnquire__set_sort_by_value_then_relevance, 3)
    bool reverse = (bool)PARAM(3);
    try {
        CONCEPT_THIS->set_sort_by_value_then_relevance(sort_key, reverse);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__set_sort_by_relevance_then_value, 4)
    T_NUMBER(XapianEnquire__set_sort_by_relevance_then_value, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__set_sort_by_value_then_relevance object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__set_sort_by_relevance_then_value, 2)
    Xapian::valueno sort_key = (Xapian::valueno)PARAM(2);
    T_NUMBER(XapianEnquire__set_sort_by_relevance_then_value, 3)
    bool reverse = (bool)PARAM(3);
    try {
        CONCEPT_THIS->set_sort_by_relevance_then_value(sort_key, reverse);
        RETURN_NUMBER(0)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__get_mset, 6)
    T_NUMBER(XapianEnquire__get_mset, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__get_mset object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__get_mset, 2)
    Xapian::doccount first = (Xapian::doccount)PARAM(2);
    T_NUMBER(XapianEnquire__get_mset, 3)
    Xapian::doccount maxitems = (Xapian::doccount)PARAM(3);
    T_NUMBER(XapianEnquire__get_mset, 4)
    Xapian::doccount checkatleast = (Xapian::doccount)PARAM(4);
    T_NUMBER(XapianEnquire__get_mset, 5)
    Xapian::RSet * omrset = (Xapian::RSet *)PARAM_INT(5);
//T_NUMBER(6)
//Xapian::MatchDecider* mdecider=(Xapian::MatchDecider*)PARAM_INT(6);
    try {
        RETURN_NUMBER((SYS_INT) new Xapian::MSet(CONCEPT_THIS->get_mset(first, maxitems, checkatleast, omrset, /*mdecider*/ 0)))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__get_eset, 6)
    T_NUMBER(XapianEnquire__get_eset, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__get_eset object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__get_eset, 2)
    Xapian::termcount maxitems = (Xapian::termcount)PARAM_INT(2);
    T_NUMBER(XapianEnquire__get_eset, 3)
    Xapian::RSet * omrset = (Xapian::RSet *)PARAM_INT(3);
    if (!omrset) {
        return (void *)"XapianEnquire__get_eset omRSet is null";
    }
    T_NUMBER(XapianEnquire__get_eset, 4)
    int flags = (int)PARAM(4);
    T_NUMBER(XapianEnquire__get_eset, 5)
    double k = (double)PARAM(5);
    try {
        RETURN_NUMBER((SYS_INT) new Xapian::ESet(CONCEPT_THIS->get_eset(maxitems, *omrset, flags, k)))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__get_eset2, 4)
    T_NUMBER(XapianEnquire__get_eset2, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__get_eset2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__get_eset2, 2)
    Xapian::termcount maxitems = (Xapian::termcount)PARAM(2);
    T_NUMBER(XapianEnquire__get_eset2, 3)
    Xapian::RSet * omrset = (Xapian::RSet *)PARAM_INT(3);
    if (!omrset) {
        return (void *)"XapianEnquire__get_eset2 omRSet is null";
    }
//T_STRING(4)
//std::string expand_decider=(std::string)PARAM(4);
    try {
        RETURN_NUMBER((SYS_INT) new Xapian::ESet(CONCEPT_THIS->get_eset(maxitems, *omrset /*, expand_decider*/)))
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianEnquire__get_matching_terms_beginIterator;
CONCEPT_FUNCTION_IMPL(XapianEnquire__get_matching_terms_begin, 3)
    T_NUMBER(XapianEnquire__get_matching_terms_begin, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__get_matching_terms_begin object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__get_matching_terms_begin, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    try {
        XapianEnquire__get_matching_terms_beginIterator = CONCEPT_THIS->get_matching_terms_begin(did);
        RETURN_NUMBER((SYS_INT)&XapianEnquire__get_matching_terms_beginIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianEnquire__get_matching_terms_endIterator;
CONCEPT_FUNCTION_IMPL(XapianEnquire__get_matching_terms_end, 3)
    T_NUMBER(XapianEnquire__get_matching_terms_end, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__get_matching_terms_end object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__get_matching_terms_end, 2)
    Xapian::docid did = (Xapian::docid)PARAM(2);
    try {
        XapianEnquire__get_matching_terms_endIterator = CONCEPT_THIS->get_matching_terms_end(did);
        RETURN_NUMBER((SYS_INT)&XapianEnquire__get_matching_terms_endIterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianEnquire__get_matching_terms_begin2Iterator;
CONCEPT_FUNCTION_IMPL(XapianEnquire__get_matching_terms_begin2, 3)
    T_NUMBER(XapianEnquire__get_matching_terms_begin2, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__get_matching_terms_begin2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__get_matching_terms_begin2, 2)
    Xapian::MSetIterator * it = (Xapian::MSetIterator *)PARAM_INT(2);
    if (!it) {
        return (void *)"XapianEnquire__get_matching_terms_begin2 iterator is null";
    }
    try {
        XapianEnquire__get_matching_terms_begin2Iterator = CONCEPT_THIS->get_matching_terms_begin(*it);
        RETURN_NUMBER((SYS_INT)&XapianEnquire__get_matching_terms_begin2Iterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
Xapian::TermIterator XapianEnquire__get_matching_terms_end2Iterator;
CONCEPT_FUNCTION_IMPL(XapianEnquire__get_matching_terms_end2, 3)
    T_NUMBER(XapianEnquire__get_matching_terms_end2, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__get_matching_terms_end2 object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    T_NUMBER(XapianEnquire__get_matching_terms_end2, 2)
    Xapian::MSetIterator * it = (Xapian::MSetIterator *)PARAM_INT(2);
    if (!it) {
        return (void *)"XapianEnquire__get_matching_terms_end2 iterator is null";
    }
    try {
        XapianEnquire__get_matching_terms_end2Iterator = CONCEPT_THIS->get_matching_terms_end(*it);
        RETURN_NUMBER((SYS_INT)&XapianEnquire__get_matching_terms_end2Iterator)
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianEnquire__get_description, 2)
    T_NUMBER(XapianEnquire__get_description, 0)
    Xapian::Enquire * CONCEPT_THIS = (Xapian::Enquire *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianEnquire__get_description object reference is null (first parameter)";
    }
    SET_STRING(1, (char *)"");

    try {
        RETURN_STRING((char *)CONCEPT_THIS->get_description().c_str())
    } catch (const Xapian::Error& error) {
        SET_STRING(1, (char *)error.get_msg().c_str());
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianStringValueRangeProcessorCreate, 1)
    T_NUMBER(XapianStringValueRangeProcessorCreate, 0)

    Xapian::StringValueRangeProcessor * handle = new Xapian::StringValueRangeProcessor(PARAM_INT(0));
    RETURN_NUMBER((long)handle)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianStringValueRangeProcessorDestroy, 1)
    T_NUMBER(XapianStringValueRangeProcessorDestroy, 0)

    Xapian::StringValueRangeProcessor * CONCEPT_THIS = (Xapian::StringValueRangeProcessor *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    delete CONCEPT_THIS;
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(XapianDateValueRangeProcessorCreate, 1, 3)
    T_NUMBER(XapianDateValueRangeProcessorCreate, 0)

    bool prefer_mdy = false;
    int epoch_year = 1970;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(XapianDateValueRangeProcessorCreate, 1)
        prefer_mdy = (bool)PARAM_INT(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(XapianDateValueRangeProcessorCreate, 2)
        epoch_year = PARAM_INT(2);
    }

    Xapian::DateValueRangeProcessor *handle = new Xapian::DateValueRangeProcessor(PARAM_INT(0), prefer_mdy, epoch_year);
    RETURN_NUMBER((long)handle)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianDateValueRangeProcessorDestroy, 1)
    T_NUMBER(XapianDateValueRangeProcessorDestroy, 0)
    Xapian::DateValueRangeProcessor * CONCEPT_THIS = (Xapian::DateValueRangeProcessor *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    delete CONCEPT_THIS;
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(XapianNumberValueRangeProcessorCreate, 1, 3)
    T_NUMBER(XapianNumberValueRangeProcessorCreate, 0)
#if 0
    Xapian::NumberValueRangeProcessor * handle = new Xapian::NumberValueRangeProcessor(PARAM_INT(0));
#else
    std::string str_;
    bool prefix_ = true;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(XapianNumberValueRangeProcessorCreate, 1)
        str_ = PARAM(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(XapianNumberValueRangeProcessorCreate, 2)
        prefix_ = (bool)PARAM_INT(2);
    }
    Xapian::NumberValueRangeProcessor *handle = 0;
    if (str_.size())
        handle = new Xapian::NumberValueRangeProcessor(PARAM_INT(0), str_, prefix_);
    else
        handle = new Xapian::NumberValueRangeProcessor(PARAM_INT(0));
#endif
    RETURN_NUMBER((long)handle)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianNumberValueRangeProcessorDestroy, 1)
    T_NUMBER(XapianNumberValueRangeProcessorDestroy, 0)
    Xapian::NumberValueRangeProcessor * CONCEPT_THIS = (Xapian::NumberValueRangeProcessor *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    delete CONCEPT_THIS;
    SET_NUMBER(0, 0)

    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianSimpleStopperCreate, 0)
    T_NUMBER(XapianSimpleStopperCreate, 0)

    Xapian::SimpleStopper * handle = new Xapian::SimpleStopper();
    RETURN_NUMBER((long)handle)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianSimpleStopperDestroy, 1)
    T_NUMBER(XapianSimpleStopperDestroy, 0)
    Xapian::SimpleStopper * CONCEPT_THIS = (Xapian::SimpleStopper *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        RETURN_NUMBER(0)
        return 0;
    }
    delete CONCEPT_THIS;
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianSimpleStopper__add, 3)
    T_NUMBER(XapianSimpleStopper__add, 0)
    T_STRING(XapianSimpleStopper__add, 1)

    SET_STRING(2, "");

    Xapian::SimpleStopper *CONCEPT_THIS = (Xapian::SimpleStopper *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianSimpleStopper__add object reference is null (first parameter)";
    }
    try {
        CONCEPT_THIS->add(PARAM(1));
    } catch (const Xapian::Error& error) {
        SET_STRING(2, (char *)error.get_msg().c_str());
    }

    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(XapianSimpleStopper__has, 3)
    T_NUMBER(XapianSimpleStopper__has, 0)
    T_STRING(XapianSimpleStopper__has, 1)

    SET_STRING(2, "");

    Xapian::SimpleStopper *CONCEPT_THIS = (Xapian::SimpleStopper *)PARAM_INT(0);
    if (!CONCEPT_THIS) {
        return (void *)"XapianSimpleStopper__has object reference is null (first parameter)";
    }
    bool result = false;
    try {
        result = CONCEPT_THIS->operator()(PARAM(1));
    } catch (const Xapian::Error& error) {
        SET_STRING(2, (char *)error.get_msg().c_str());
    }
    RETURN_NUMBER((long)result);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Xapian__sortable_serialise, 1)
    T_NUMBER(Xapian__sortable_serialise, 0)
    std::string res = Xapian::sortable_serialise(PARAM(0));
    RETURN_BUFFER(res.c_str(), res.size());
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Xapian__sortable_unserialise, 1)
    T_STRING(Xapian__sortable_unserialise, 0)
    std::string s;
    s.append((const char *)PARAM(0), (size_t)PARAM_LEN(0));
    NUMBER res = Xapian::sortable_unserialise(s);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------

