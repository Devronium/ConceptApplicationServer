#include <sstream>

#include "amf_boolean.h"

amf::amf_boolean::amf_boolean()
    : m_val(false) {
}

amf::amf_boolean::amf_boolean(bool val)
    : m_val(val) {
}

amf::AMF_TYPE amf::amf_boolean::get_type() const {
    return AMF_TYPE_BOOLEAN;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void amf::amf_boolean::encode(char *const data) const {
    char *buf = data;

    *buf = m_val;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
boost::int32_t amf::amf_boolean::decode(const char *data, boost::uint32_t quota) {
    if (quota >= get_size()) {
        m_val = static_cast<bool>(*data == 0x01);

        return get_size();
    } else {
        return AMF_DECODE_FAILED;
    }
}

const boost::uint32_t amf::amf_boolean::get_size() const {
    return 1;
}

const bool amf::amf_boolean::get_value() const {
    return m_val;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const std::string amf::amf_boolean::to_string() const {
    std::ostringstream rt;

    rt << std::ios::boolalpha << m_val;

    return rt.str();
}
