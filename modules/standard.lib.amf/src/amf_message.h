#ifndef _AMF_MESSAGE_H_
#define _AMF_MESSAGE_H_

#include "amf_string.h"
#include "amf_list.h"
#include "amf_numeric.h"

namespace amf
{
class amf_message
{
public:
    bool is_valid_msg(void) const {
        if (m_list.count() < 1) {
            return false;
        }

        if (m_list.at(0)->get_type() != AMF_TYPE_STRING) {
            return false;
        }

        return true;
    }

    void encode(char *data) const {
        m_list.encode(data);
    }

    boost::int32_t decode(const char *data, boost::uint32_t quota) {
        return m_list.decode(data, quota);
    }

    const std::string& get_command_name() const {
        return reinterpret_cast<const amf_string *>(m_list.at(0).get())->get_value();
    }

private:
    amf_list m_list;
};
}
#endif // _AMF_MESSAGE_H_
