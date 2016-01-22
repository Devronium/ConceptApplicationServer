#ifndef _AMF_DATE_H_
#define _AMF_DATE_H_

#include "amf_data_type.h"
#include "amf_numeric.h"

namespace amf
{
class amf_date : public amf0_data_type
{
public:
    static const AMF_TYPE TYPE = AMF_TYPE_DATE;

public:
    amf_date();

    AMF_TYPE get_type() const;

    void encode(char *const data) const;

    boost::int32_t decode(const char *data, boost::uint32_t quota);

    const boost::uint32_t get_size() const;

    const std::string to_string() const;

private:
    boost::int32_t m_time_zone;

    amf_numeric m_time;
};
}
#endif // _AMF_DATE_H_
