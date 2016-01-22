#include <sstream>
#include <boost/detail/endian.hpp>

#include "amf_numeric.h"

amf::amf_numeric::amf_numeric()
    : m_val(0.0) {
}

amf::amf_numeric::amf_numeric(double val)
    : m_val(val) {
}

amf::AMF_TYPE amf::amf_numeric::get_type() const {
    return AMF_TYPE_NUMERIC;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void amf::amf_numeric::encode(char *const data) const {
    char *buf  = data;
    char *dPtr = (char *)&m_val;

#if defined(BOOST_LITTLE_ENDIAN)
    for (int i = 7; i >= 0; --i) {
        buf[i] = *dPtr;
        dPtr++;
    }
#elif BOOST_BIG_ENDIAN
    for (int i = 0; i <= 7; ++i) {
        buf[i] = *dPtr;
        dPtr++;
    }
#endif
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
boost::int32_t amf::amf_numeric::decode(const char *data, boost::uint32_t quota) {
    if (quota < get_size())
        return AMF_DECODE_FAILED;

    char *dPtr = (char *)&m_val;

#if defined(BOOST_LITTLE_ENDIAN)
    for (int i = 7; i >= 0; --i) {
        *dPtr = data[i];
        dPtr++;
    }
#elif defined(BOOST_BIG_ENDIAN)
    for (int i = 0; i <= 7; ++i) {
        *dPtr = data[i];
        dPtr++;
    }
#endif

    return get_size();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const boost::uint32_t amf::amf_numeric::get_size() const {
    return 8;
}

const double amf::amf_numeric::get_value() const {
    return m_val;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const std::string amf::amf_numeric::to_string() const {
    std::ostringstream rt;

    rt << std::ios::fixed << m_val;

    return rt.str();
}
