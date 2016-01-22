#include <sstream>
#include <boost/foreach.hpp>

#include "amf_list.h"
#include "amf.h"

typedef std::vector<amf::amf_data_ptr>   amf_list_t;

void amf::amf_list::add(amf::amf_data_ptr as) {
    m_list.push_back(as);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void amf::amf_list::encode(char *data) const {
    char *ptr = data;

    BOOST_FOREACH(const amf::amf_data_ptr & item, m_list) {
        amf_item::encode(ptr, item);

        ptr += item->get_size() + 1;
    }
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
boost::int32_t amf::amf_list::decode(const char *data, boost::uint32_t quota) {
    boost::uint32_t left_quota = quota;

    const char *ptr = data;

    while (left_quota > 0) {
        amf::amf_data_ptr item = amf_item::decode(ptr, left_quota);

        if (item->get_type() == AMF_TYPE_UNSUPPORTED) {
            return AMF_DECODE_FAILED;
        }

        m_list.push_back(item);

        boost::uint32_t item_size = item->get_size() + 1;

        ptr        += item_size;
        left_quota -= item_size;
    }

    return get_size();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
boost::uint32_t amf::amf_list::get_size(void) const {
    boost::uint32_t total_size = 0;

    BOOST_FOREACH(const amf::amf_data_ptr & item, m_list) {
        total_size += item->get_size();
    }

    return total_size + m_list.size();
}

boost::uint32_t amf::amf_list::count() const {
    return m_list.size();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
std::string amf::amf_list::to_string() const {
    std::ostringstream rt;

    BOOST_FOREACH(const amf::amf_data_ptr & item, m_list) {
        rt << item->to_string() << std::endl;
    }

    return rt.str();
}

const amf::amf_data_ptr amf::amf_list::at(int pos) const {
    return m_list.at(pos);
}
