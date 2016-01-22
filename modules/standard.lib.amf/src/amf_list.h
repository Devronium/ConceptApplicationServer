#ifndef _AMF_LIST_H_
#define _AMF_LIST_H_

#include <vector>

#include "amf_data_type.h"

namespace amf
{
class amf_list
{
public:
    void add(amf_data_ptr as);

    void encode(char *data) const;

    boost::int32_t decode(const char *data, boost::uint32_t quota);

    boost::uint32_t get_size(void) const;

    boost::uint32_t count() const;

    std::string to_string() const;

    const amf_data_ptr at(int pos) const;

private:
    std::vector<amf_data_ptr> m_list;
};
}
#endif // _AMF_LIST_H_
