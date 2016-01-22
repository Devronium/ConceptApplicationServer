#ifndef _AMF_INTEGER_H_
#define _AMF_INTEGER_H_

#ifdef _WIN32
 #include <WinSock.h>
#else
 #include <arpa/inet.h>
 #include <string.h>
#endif
#include <boost/static_assert.hpp>
#include <boost/cstdint.hpp>

namespace amf
{
class amf_integer
{
public:
    template<int bytes>
    static void encode(char *data, boost::uint32_t val) {
        // only 1, 2 , 3, 4 is the legal value
        BOOST_STATIC_ASSERT((bytes > 0) && (bytes < 5));

        boost::uint32_t n_val = htonl(val);

        char *ptr = (char *)&n_val;
        memcpy(data, ptr + (sizeof(boost::uint32_t) - bytes), bytes);
    }

    template<int bytes>
    static boost::uint32_t decode(const char *data) {
        // only 1, 2 , 3, 4 is the legal value
        BOOST_STATIC_ASSERT((bytes > 0) && (bytes < 5));

        boost::uint32_t val = 0;

        char *ptr = (char *)&val;
        memcpy(ptr + (sizeof(boost::uint32_t) - bytes), data, bytes);

        return ntohl(val);
    }
};
}
#endif // _AMF_INTEGER_H_
