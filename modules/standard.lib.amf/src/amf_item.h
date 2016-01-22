#ifndef _AMF_ITEM_H_
#define _AMF_ITEM_H_

#include "amf_data_type.h"

namespace amf
{
class amf_item
{
public:
    static void encode(char *data, amf_data_ptr val);

    /**
     * @return Return AsUnsupportedPtr when failed.
     */
    static amf_data_ptr decode(const char *data, boost::uint32_t quota);
};
}
#endif // _AMF_ITEM_H_
