#ifndef _AMF_BOOLEAN_H_
#define _AMF_BOOLEAN_H_

#include "amf_data_type.h"

namespace amf
{
class amf_boolean : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_BOOLEAN;

public:
    amf_boolean();

    explicit amf_boolean(bool val);

public:
    AMF_TYPE get_type() const;

    void encode(char *const data) const;

    boost::int32_t decode(const char *data, boost::uint32_t quota);

    const boost::uint32_t get_size() const;

    const bool get_value() const;

    const std::string to_string() const;

private:
    volatile bool m_val;
};
}
#endif // _AMF_BOOLEAN_H_
