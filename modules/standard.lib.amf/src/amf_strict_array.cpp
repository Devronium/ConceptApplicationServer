#include <sstream>
#include <boost/foreach.hpp>

#include "amf_strict_array.h"
#include "amf.h"

amf::AMF_TYPE amf::amf_strict_array::get_type() const {
    return AMF_TYPE_STRICT_ARRAY;
}

/************************************************************************/
// An array is indicated by 0x0A, then a Long for array length, then the
// array elements themselves. Arrays are always sparse; values for inexistant
// keys are set to null (0¡Á06) to maintain sparsity. Take the following example:

// var a = new Array();
// a[0] = "something";
// a[10000] = "Something else";

// Flash will encode this as 0x0A, then length 10001, then the first element,
// 9999 times 0¡Á06, then the final element, for a total of over 10 KB. For
// arrays with very large gaps, the user may want to insert a dummy string
// key so that the array will be forcefully typed as a MixedArray.
/************************************************************************/
void amf::amf_strict_array::encode(char *const data) const {
    char *ptr = data;

    amf_integer::encode<sizeof(boost::uint32_t)>(ptr, m_val.size());

    BOOST_FOREACH(const amf_data_ptr &item, m_val) {
        amf_item::encode(ptr, item);

        ptr += item->get_size() + 1;
    }
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
boost::int32_t amf::amf_strict_array::decode(const char *data, boost::uint32_t quota) {
    if (quota < (sizeof(boost::uint32_t) + 3))
        return AMF_DECODE_FAILED;

    const char      *ptr       = data;
    boost::uint32_t left_quota = quota;

    // 1. array size
    const boost::uint32_t array_size = amf_integer::decode<sizeof(boost::uint32_t)>(ptr);

    ptr        += sizeof(boost::uint32_t);
    left_quota -= sizeof(boost::uint32_t);

    // 2. for each element
    for (boost::uint32_t i = 0; (i < array_size) && (left_quota > 0); ++i) {
        amf_data_ptr pval = amf_item::decode(ptr, left_quota);

        if (pval->get_type() == AMF_TYPE_UNSUPPORTED) {
            return AMF_DECODE_FAILED;
        }

        m_val.push_back(pval);

        boost::uint32_t item_size = pval->get_size() + 1;

        ptr        += item_size;
        left_quota -= item_size;
    }

    return get_size();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const boost::uint32_t amf::amf_strict_array::get_size() const {
    boost::uint32_t total_size = 0;

    BOOST_FOREACH(const amf_data_ptr &item, m_val) {
        total_size += item->get_size();
    }

    // size(4) + data + 00 00 09
    return total_size + m_val.size() + sizeof(boost::uint32_t);    // + 3;
}

const amf::strict_array& amf::amf_strict_array::get_value() const {
    return m_val;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const std::string amf::amf_strict_array::to_string() const {
    std::ostringstream rt;

    BOOST_FOREACH(const amf_data_ptr &item, m_val) {
        rt.width(20);

        rt << item->to_string() << std::endl;
    }

    return rt.str();
}
