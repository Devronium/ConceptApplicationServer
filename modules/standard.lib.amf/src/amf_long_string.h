#ifndef _AMF_LONG_STING_H_
#define _AMF_LONG_STING_H_

#include "amf_data_type.h"

namespace amf
{
class amf_long_string : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_LONG_STRING;

public:
    amf_long_string();

    explicit amf_long_string(const std::string& val);

    explicit amf_long_string(const char *data);

    bool operator<(const amf_long_string& other) const;

public:
    AMF_TYPE get_type() const;

    void encode(char *const data) const;

    boost::int32_t decode(const char *data, boost::uint32_t quota);

    const boost::uint32_t get_size() const;

    const std::string& get_value() const;

    const std::string to_string() const;

private:
    std::string m_val;
};
}
#endif // _AMF_LONG_STING_H_
