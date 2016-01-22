#ifndef _AMF_REFERENCE_H_
#define _AMF_REFERENCE_H_

#include "amf_integer.h"
#include "amf_data_type.h"

namespace amf
{
class amf_reference : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_REFERENCE;

public:
    AMF_TYPE get_type() const {
        return AMF_TYPE_REFERENCE;
    }

    void encode(char *const data) const {
        char *buf = data;

        amf_integer::encode<2>(buf, m_index);
    }

    boost::int32_t decode(const char *data, boost::uint32_t quota) {
        if (quota < 2)
            return AMF_DECODE_FAILED;

        m_index = amf_integer::decode<2>(data);

        return get_size();
    }

    const boost::uint32_t get_size() const {
        return sizeof(boost::uint16_t);
    }

    const std::string to_string() const {
        return "REFERENCE";
    }

private:
    boost::uint32_t m_index;
};
}
#endif // _AMF_REFERENCE_H_
