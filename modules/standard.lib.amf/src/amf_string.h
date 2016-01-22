#ifndef _AMF_STRING_H_
#define _AMF_STRING_H_

#include <map>

#include "amf_data_type.h"

namespace amf
{
class amf_string : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_STRING;

public:
    amf_string();

    explicit amf_string(const std::string& val);

    explicit amf_string(const char *data);

    bool operator<(const amf_string& other) const;

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

typedef std::pair<amf_string, amf_data_ptr>   ecma_pair_t;
typedef std::map<amf_string, amf_data_ptr>    ecma_array_t;
}
#endif // _AMF_STRING_H_
