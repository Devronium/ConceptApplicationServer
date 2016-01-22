#include <boost/cstdint.hpp>

#include "amf_string.h"
#include "amf_integer.h"

amf::amf_string::amf_string() {
}

amf::amf_string::amf_string(const std::string& val)
    : m_val(val) {
}

amf::amf_string::amf_string(const char *data)
    : m_val(data) {
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool amf::amf_string::operator<(const amf::amf_string& other) const {
    return m_val.compare(other.get_value()) < 0;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
amf::AMF_TYPE amf::amf_string::get_type() const {
    return AMF_TYPE_STRING;
}

void amf::amf_string::encode(char *const data) const {
    char *buf = data;

    amf_integer::encode<2>(buf, m_val.size());

    buf += 2;

    memcpy(buf, m_val.c_str(), m_val.size());
    buf += m_val.size();
}

boost::int32_t amf::amf_string::decode(const char *data, boost::uint32_t quota) {
    if (quota < 2)
        return AMF_DECODE_FAILED;

    boost::uint32_t length = amf::amf_integer::decode<2>(data);

    if (quota < (length + 2))
        return AMF_DECODE_FAILED;


    if (length > 0) {
        m_val = std::string(data + 2, length);
    }

    return get_size();
}

const boost::uint32_t amf::amf_string::get_size() const {
    return m_val.size() + 2;
}

const std::string& amf::amf_string::get_value() const {
    return m_val;
}

const std::string amf::amf_string::to_string() const {
    return m_val;
}
