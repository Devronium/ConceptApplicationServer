#include <sstream>
#include <boost/foreach.hpp>

#include "amf_object.h"
#include "amf.h"

amf::amf_object::amf_object() {
}

amf::amf_object::amf_object(amf::ecma_array_t& arr)
    : m_val(arr) {
}

amf::AMF_TYPE amf::amf_object::get_type() const {
    return AMF_TYPE_OBJECT;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void amf::amf_object::encode(char *const data) const {
    char *ptr = data;

    BOOST_FOREACH(const ecma_pair_t &item, m_val) {
        item.first.encode(ptr);
        ptr += item.first.get_size();

        amf_item::encode(ptr, item.second);
        ptr += item.second->get_size() + 1;
    }

    amf_integer::encode<2>(ptr, 0);
    ptr += 2;

    amf_integer::encode<1>(ptr, 9);
    ++ptr;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
boost::int32_t amf::amf_object::decode(const char *data, boost::uint32_t quota) {
    const char      *ptr       = data;
    boost::uint32_t left_quota = quota;

    while (left_quota > 0) {
        amf::amf_string str;

        boost::uint32_t str_size = str.decode(ptr, left_quota);

        if (str_size == AMF_DECODE_FAILED) {
            return AMF_DECODE_FAILED;
        }

        ptr        += str_size;
        left_quota -= str_size;

        if (str.get_value().size() == 0) {
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

    if ((left_quota < 1) || (amf::amf_integer::decode<1>(ptr) != AMF_TYPE_OBJECT_END)) {
        return AMF_DECODE_FAILED;
    }

    return get_size();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const boost::uint32_t amf::amf_object::get_size() const {
    boost::uint32_t total_size = 0;

    for (ecma_array_t::const_iterator itor = m_val.begin();
         itor != m_val.end(); ++itor) {
        total_size += itor->first.get_size();
        total_size += itor->second->get_size();
    }

    // data + 00 00 09
    return total_size + m_val.size() + 3;
}

const amf::ecma_array_t& amf::amf_object::get_value() const {
    return m_val;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const amf::amf_data_ptr amf::amf_object::get_properity(const std::string& key) const {
    BOOST_FOREACH(const ecma_pair_t &item, m_val) {
        if (item.first.get_value() == key) {
            return item.second;
        }
    }

    return boost::shared_ptr<amf_undefined>(new amf_undefined());
}

void amf::amf_object::set_properity(const std::string& key, amf::amf_data_ptr val) {
    remove_properity(key);

    add_properity(amf::amf_string(key), val);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void amf::amf_object::add_properity(amf::amf_string key, amf::amf_data_ptr val) {
    m_val.insert(std::make_pair(key, val));
}

void amf::amf_object::remove_properity(const std::string& key) {
    m_val.erase(amf::amf_string(key));
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const std::string amf::amf_object::to_string() const {
    std::ostringstream rt;

    BOOST_FOREACH(const ecma_pair_t &item, m_val) {
        rt.width(20);

        rt << item.first.get_value() << " : ";
        rt << item.second->to_string() << std::endl;
    }

    return rt.str();
}
