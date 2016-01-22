#include <sstream>
#include <boost/foreach.hpp>

#include "amf_ecma_array.h"
#include "amf.h"

amf::AMF_TYPE amf::amf_ecma_array::get_type() const {
    return AMF_TYPE_ECMA_ARRAY;
}

/************************************************************************/
// A MixedArray is indicated by code 0¡Á08, then a Long representing the highest
// numeric index in the array, or 0 if there are none or they are all negative.
// After that follow the elements in key : value pairs. Each key is an UTF8 string
// containing the string length. Numeric keys are represented as strings.
//
// A MixedArray is an instance of the ActionScript Array with either of the conditions:
//
//  1. The array contains floating point or negative indices
//  2. The array contains non-numeric keys
//
// MixedArray ends with an empty UTF8 string (0¡Á00 0¡Á00) and 0¡Á09. It is very similar
// to an 0¡Á03 Object; however the host should convert numeric looking keys to numeric
// keys, unlike Object.
/************************************************************************/
void amf::amf_ecma_array::encode(char *const data) const {
    char *ptr = data;

    amf_integer::encode<sizeof(boost::uint32_t)>(ptr, m_val.size());

    BOOST_FOREACH(const ecma_pair_t &item, m_val) {
        item.first.encode(ptr);
        ptr += item.first.get_size();

        amf_item::encode(ptr, item.second);
        ptr += item.second->get_size() + 1;
    }

    amf_integer::encode<2>(ptr, 0);
    ptr += 2;

    amf_integer::encode<1>(ptr, AMF_TYPE_OBJECT_END);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
boost::int32_t amf::amf_ecma_array::decode(const char *data, boost::uint32_t quota) {
    if (quota < (sizeof(boost::uint32_t) + 3))
        return AMF_DECODE_FAILED;

    const char      *ptr       = data;
    boost::uint32_t left_quota = quota;

    amf_integer::decode<sizeof(boost::uint32_t)>(ptr);

    ptr        += sizeof(boost::uint32_t);
    left_quota -= sizeof(boost::uint32_t);

    // TODO - array size?

    while (left_quota > 0) {
        //////////////////////////////////////////////////////////////////////////
        amf_string      str;
        boost::uint32_t str_size = str.decode(ptr, left_quota);

        if (str_size == AMF_DECODE_FAILED) {
            return AMF_DECODE_FAILED;
        }

        ptr        += str_size;
        left_quota -= str_size;

        if (str.get_value().empty()) {
            break;
        }

        //////////////////////////////////////////////////////////////////////////
        amf_data_ptr pval = amf_item::decode(ptr, left_quota);

        if (pval->get_type() == AMF_TYPE_UNSUPPORTED) {
            return AMF_DECODE_FAILED;
        }

        m_val.insert(std::make_pair(str, pval));

        boost::uint32_t item_size = pval->get_size() + 1;

        ptr        += item_size;
        left_quota -= item_size;
    }

    if ((left_quota < 1) || (amf_integer::decode<1>(ptr) != AMF_TYPE_OBJECT_END)) {
        return AMF_DECODE_FAILED;
    }

    return get_size();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const boost::uint32_t amf::amf_ecma_array::get_size() const {
    boost::uint32_t total_size = 0;

    BOOST_FOREACH(const ecma_pair_t &item, m_val) {
        total_size += item.first.get_size();
        total_size += item.second->get_size();
    }

    // size(4) + data + 00 00 09
    return total_size + m_val.size() + sizeof(boost::uint32_t) + 3;
}

const amf::ecma_array_t& amf::amf_ecma_array::get_value() const {
    return m_val;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const amf::amf_data_ptr amf::amf_ecma_array::get_properity(const std::string& key) const {
    BOOST_FOREACH(const ecma_pair_t &item, m_val) {
        if (item.first.get_value() == key) {
            return item.second;
        }
    }

    return boost::shared_ptr<amf_undefined>(new amf_undefined());
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void amf::amf_ecma_array::remove_properity(const std::string& key) {
    ecma_array_t::iterator pos = m_val.find(amf_string(key));

    if (pos != m_val.end()) {
        m_val.erase(pos);
    }
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

const std::string amf::amf_ecma_array::to_string() const {
    std::ostringstream rt;

    BOOST_FOREACH(const ecma_pair_t &item, m_val) {
        rt.width(20);
        rt << item.first.get_value() << " : ";
        rt << item.second->to_string() << std::endl;
    }

    return rt.str();
}
