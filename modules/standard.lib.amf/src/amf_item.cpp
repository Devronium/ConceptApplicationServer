#include <boost/cstdint.hpp>

#include "amf_item.h"
#include "amf.h"



/************************************************************************/
/*                                                                      */
/************************************************************************/
void amf::amf_item::encode(char *data, amf_data_ptr val) {
    char *ptr = data;

    amf_integer::encode<1>(ptr, val->get_type());

    ++ptr;

    val->encode(ptr);
}

/************************************************************************/

/**
 *
 * Return: return match type object if ok, or return AsUnsupported instead.
 **/
/************************************************************************/
amf::amf_data_ptr amf::amf_item::decode(const char *data, boost::uint32_t quota) {
    using namespace amf;

    if (quota < 1)
        return amf_data_ptr(new amf_unsupported());

    const char *ptr = data;

    AMF_TYPE value_type = (AMF_TYPE)*ptr;
    ++ptr;

    boost::uint32_t left_quota = quota - 1;
    //////////////////////////////////////////////////////////////////////////
    amf_data_ptr val(new amf_undefined());

    if (value_type == AMF_TYPE_NUMERIC) {
        val = amf_data_ptr(new amf_numeric);
    } else if (value_type == AMF_TYPE_BOOLEAN) {
        val = amf_data_ptr(new amf_boolean);
    } else if (value_type == AMF_TYPE_STRING) {
        val = amf_data_ptr(new amf_string);
    } else if (value_type == AMF_TYPE_OBJECT) {
        val = amf_data_ptr(new amf_object);
    } else if (value_type == AMF_TYPE_NULL_VALUE) {
        val = amf_data_ptr(new amf_null);
    } else if (value_type == AMF_TYPE_UNDEFINED) {
        val = amf_data_ptr(new amf_undefined);
    } else if (value_type == AMF_TYPE_REFERENCE) {
        val = amf_data_ptr(new amf_reference);
    } else if (value_type == AMF_TYPE_ECMA_ARRAY) {
        val = amf_data_ptr(new amf_ecma_array);
    } else if (value_type == AMF_TYPE_STRICT_ARRAY) {
        val = amf_data_ptr(new amf_strict_array);
    } else if (value_type == AMF_TYPE_DATE) {
        val = amf_data_ptr(new amf_date);
    } else if (value_type == AMF_TYPE_LONG_STRING) {
        val = amf_data_ptr(new amf_long_string);
    } else if (value_type == AMF_TYPE_OBJECT_END) {
        val = amf_data_ptr(new amf_undefined);
    } else if (value_type == AMF_TYPE_UNSUPPORTED) {
        val = amf_data_ptr(new amf_unsupported);
    }

    //////////////////////////////////////////////////////////////////////////
    if (val->decode(ptr, left_quota) == AMF_DECODE_FAILED) {
        return amf_data_ptr(new amf_unsupported());
    } else {
        return val;
    }
}
