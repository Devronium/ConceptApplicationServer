#ifndef _AMF_STRICT_ARRAY_H_
#define _AMF_STRICT_ARRAY_H_

#include <list>

#include "amf_data_type.h"

namespace amf
{
typedef std::list<amf_data_ptr>   strict_array;

class amf_strict_array : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_STRICT_ARRAY;

public:
    AMF_TYPE get_type() const;

    void encode(char *const data) const;

    boost::int32_t decode(const char *data, boost::uint32_t quota);

    const boost::uint32_t get_size() const;

    const strict_array& get_value() const;

    const std::string to_string() const;

private:
    std::list<amf_data_ptr> m_val;
};
}
#endif // _AMF_STRICT_ARRAY_H_
