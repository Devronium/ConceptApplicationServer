#ifndef _AMF_NUMERIC_H_
#define _AMF_NUMERIC_H_

#include "amf_data_type.h"

namespace amf
{
class amf_numeric : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_NUMERIC;

public:
    amf_numeric();

    explicit amf_numeric(double val);

public:
    AMF_TYPE get_type() const;

    void encode(char *const data) const;

    boost::int32_t decode(const char *data, boost::uint32_t quota);

    const boost::uint32_t get_size() const;

    const double get_value() const;

    const std::string to_string() const;

private:
    volatile double m_val;
};
}
#endif // _AMF_NUMERIC_H_
