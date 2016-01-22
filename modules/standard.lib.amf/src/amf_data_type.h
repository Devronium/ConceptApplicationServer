#ifndef _AMF_DATA_TYPE_H_
#define _AMF_DATA_TYPE_H_

#include <string>

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

const boost::int32_t AMF_DECODE_FAILED = -1;

namespace amf
{
enum AMF_TYPE {
    AMF_TYPE_NUMERIC = 0x00,
    AMF_TYPE_BOOLEAN,
    AMF_TYPE_STRING,
    AMF_TYPE_OBJECT,
    AMF_TYPE_MOVIECLIP,
    AMF_TYPE_NULL_VALUE,
    AMF_TYPE_UNDEFINED,
    AMF_TYPE_REFERENCE,
    AMF_TYPE_ECMA_ARRAY,
    AMF_TYPE_OBJECT_END,
    AMF_TYPE_STRICT_ARRAY,
    AMF_TYPE_DATE,
    AMF_TYPE_LONG_STRING,
    AMF_TYPE_UNSUPPORTED,
    AMF_TYPE_RECORD_SET,
    AMF_TYPE_XML_OBJECT,
    AMF_TYPE_TYPED_OBJECT,
    AMF_TYPE_AMF3
};

class amf0_data_type
{
public:
    virtual ~amf0_data_type() {}

    virtual AMF_TYPE get_type() const = 0;

    /**
     * Caution: I don't check the pointer here.
     */
    virtual void encode(char *const data) const = 0;

    /**
     * @return Return the MemSzie without data type(1 byte) if success decode.
     *         if failed, return AMF_DECODE_FAILED
     */
    virtual boost::int32_t decode(const char *data, boost::uint32_t quota) = 0;

    /**
     * @return Return the real memory size of an AMF object which doesn't contain
     *         the flag size.
     */
    virtual const boost::uint32_t    get_size() const = 0;

    virtual const std::string to_string() const = 0;
};

typedef boost::shared_ptr<amf0_data_type>   amf_data_ptr;
}
#endif // _AMF_DATA_TYPE_H_
