#ifndef _AMF_UNDEFINED_H_
#define _AMF_UNDEFINED_H_

#include "amf_data_type.h"

namespace amf
{
class amf_undefined : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_UNDEFINED;

public:
    AMF_TYPE get_type() const {
        return AMF_TYPE_UNDEFINED;
    }

    void encode(char *const data) const {
        *data = *data;
    }

    boost::int32_t decode(const char *data, boost::uint32_t quota) {
        quota = *data;

        return this->get_size();
    }

    const boost::uint32_t get_size() const {
        return 0;
    }

    const std::string to_string() const {
        return "Undefined";
    }
};
}
#endif // _AMF_UNDEFINED_H_
