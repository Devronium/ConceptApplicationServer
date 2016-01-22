#include <sstream>

#include "amf_date.h"
#include "amf_integer.h"

amf::amf_date::amf_date()
    : m_time(0) {
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void amf::amf_date::encode(char *const data) const {
    char *ptr = data;

    amf_integer::encode<2>(ptr, m_time_zone);

    ptr += 2;

    m_time.encode(data);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
boost::int32_t amf::amf_date::decode(const char *data, boost::uint32_t quota) {
    if (quota < get_size())
        return AMF_DECODE_FAILED;

    const char *ptr = data;

    m_time_zone = amf_integer::decode<2>(ptr);
    ptr        += 2;

    m_time.decode(ptr, quota - 2);

    return get_size();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
amf::AMF_TYPE amf::amf_date::get_type() const {
    return AMF_TYPE_DATE;
}

const boost::uint32_t amf::amf_date::get_size() const {
    return 2 + 8;
}

const std::string amf::amf_date::to_string() const {
    std::ostringstream rt;

    rt << "Timezone: " << m_time_zone << "; " << "Time: " << m_time.to_string();

    return rt.str();
}
