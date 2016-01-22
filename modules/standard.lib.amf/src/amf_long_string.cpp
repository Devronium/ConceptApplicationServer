#include <string>

#include "amf_long_string.h"
#include "amf_integer.h"

amf::amf_long_string::amf_long_string() {
}

amf::amf_long_string::amf_long_string(const std::string& val)
    : m_val(val) {
}

amf::amf_long_string::amf_long_string(const char *data)
    : m_val(data) {
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool amf::amf_long_string::operator<(const amf::amf_long_string& other) const {
    return m_val.compare(other.get_value()) < 0;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
amf::AMF_TYPE amf::amf_long_string::get_type() const {
    return AMF_TYPE_LONG_STRING;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void amf::amf_long_string::encode(char *const data) const {
    char *buf = data;

    amf_integer::encode<sizeof(boost::uint32_t)>(buf, m_val.size());

    buf += sizeof(boost::uint32_t);

    memcpy(buf, m_val.c_str(), m_val.size());
    buf += m_val.size();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
boost::int32_t amf::amf_long_string::decode(const char *data, boost::uint32_t quota) {
    if (quota < sizeof(boost::uint32_t)) {
        return AMF_DECODE_FAILED;
    }

    boost::uint32_t length = amf_integer::decode<sizeof(boost::uint32_t)>(data);

    if (quota < (length + sizeof(boost::uint32_t)))
        return AMF_DECODE_FAILED;


    if (length > 0) {
        m_val = std::string(data + sizeof(boost::uint32_t), length);
    }

    return get_size();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const boost::uint32_t amf::amf_long_string::get_size() const {
    return m_val.size() + sizeof(boost::uint32_t);
}

const std::string& amf::amf_long_string::get_value() const {
    return m_val;
}

const std::string amf::amf_long_string::to_string() const {
    return m_val;
}
