#ifndef _AMF_OBJECT_H_
#define _AMF_OBJECT_H_

#include "amf_data_type.h"
#include "amf_string.h"

namespace amf
{
class amf_object : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_OBJECT;

public:
    amf_object();

    explicit amf_object(ecma_array_t& arr);

public:
    AMF_TYPE get_type() const;

    void encode(char *const data) const;

    boost::int32_t decode(const char *data, boost::uint32_t quota);

    const boost::uint32_t get_size() const;

    const ecma_array_t& get_value() const;


    const amf_data_ptr get_properity(const std::string& key) const;

    void set_properity(const std::string& key, amf_data_ptr val);

    void remove_properity(const std::string& key);

    void add_properity(amf_string key, amf_data_ptr val);


    const std::string to_string() const;

private:
    ecma_array_t m_val;
};
}
#endif // _AMF_OBJECT_H_
