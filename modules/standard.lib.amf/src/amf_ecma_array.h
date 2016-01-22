#ifndef _AMF_ECMA_ARRAY_H_
#define _AMF_ECMA_ARRAY_H_

#include "amf_data_type.h"
#include "amf_string.h"

namespace amf
{
class amf_ecma_array : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_ECMA_ARRAY;

public:
    AMF_TYPE get_type() const;

    void encode(char *const data) const;

    boost::int32_t decode(const char *data, boost::uint32_t quota);

    const boost::uint32_t get_size() const;

    const ecma_array_t& get_value() const;


    const amf_data_ptr get_properity(const std::string& key) const;

    void remove_properity(const std::string& key);


    const std::string to_string() const;

private:
    ecma_array_t m_val;
};
}
#endif // _AMF_ECMA_ARRAY_H_
