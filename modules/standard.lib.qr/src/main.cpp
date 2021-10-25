//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "QR_Encode.h"

#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"

static INVOKE_CALL InvokePtr = 0;

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_SCONSTANT("QR_LEVEL_L", "0")
    DEFINE_SCONSTANT("QR_LEVEL_M", "1")
    DEFINE_SCONSTANT("QR_LEVEL_Q", "2")
    DEFINE_SCONSTANT("QR_LEVEL_H", "3")

    DEFINE_SCONSTANT("QR_E_EmptyInput", "-1")
    DEFINE_SCONSTANT("QR_E_InvalidVersion", "-2")
    DEFINE_SCONSTANT("QR_E_VersionNotFit", "-3")

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
struct jpeg_buffer {
    unsigned char *data;
    int size;
};

static void jpeg_write(void *context, void *data, int size) {
    // to do
    struct jpeg_buffer *buffer = (struct jpeg_buffer *)context;
    void *old_buffer = buffer->data;
    int old_size = buffer->size;
    buffer->size += size;
    buffer->data = (unsigned char *)realloc(buffer->data, buffer->size);
    if (!buffer->data) {
        free(old_buffer);
        buffer->data = NULL;
        buffer->size = 0;
    }
    memcpy(buffer->data + old_size, data, size);
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(QR, 1, 5)
    T_STRING(QR, 0)

    unsigned char encoded[MAX_BITDATA];

    QR_Level level = QR_LEVEL_H;
    int version = 0;
    int format = 4;
    QR_MaskPattern mask = QR_MaskAuto;

    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(QR, 1);
        format = (int)PARAM_INT(1);
    }

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(QR, 2);
        level = (QR_Level)PARAM_INT(2);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(QR, 3);
        version = PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(QR, 4);
        mask = (QR_MaskPattern)PARAM_INT(4);
    }

    int width = EncodeData(level, version, mask, PARAM(0), PARAM_LEN(0), encoded);
    if (width <= 0) {
        RETURN_NUMBER(width);
    } else {
        if (format > 0) {
            int i, j, k, l;
            unsigned char *encoded_jpeg = (unsigned char *)malloc(width * width * 3 * format * format);
            if (!encoded_jpeg) {
                RETURN_NUMBER(0);
                return 0;
            }
            // assume no compression
            struct jpeg_buffer buffer;
            buffer.data = NULL;
            buffer.size = 0;

            int len = 0;
            for (i = 0; i < width; i++) {
                for (l = 0; l < format; l ++) {
                    for (j = 0; j < width; j++) {
                        long byte = (i * width + j) / 8;
                        long bit = (i * width + j) % 8;
                        if (encoded[byte] & (0x80 >> bit)) {
                            for (k = 0; k < format; k ++) {
                                encoded_jpeg[len ++] = 0x00;
                                encoded_jpeg[len ++] = 0x00;
                                encoded_jpeg[len ++] = 0x00;
                            }
                        } else {
                            for (k = 0; k < format; k ++) {
                                encoded_jpeg[len ++] = 0xFF;
                                encoded_jpeg[len ++] = 0xFF;
                                encoded_jpeg[len ++] = 0xFF;
                            }
                        }
                    }
                }
            }

            int is_ok = tje_encode_with_func(jpeg_write, &buffer, 3, width * format, width * format, 3, encoded_jpeg);

            free(encoded_jpeg);

            if (is_ok) {
                RETURN_BUFFER((const char *)buffer.data, buffer.size);
            } else {
                RETURN_NUMBER(0);
            }
            free(buffer.data);
           
        } else {
            int size = ((width * width) / 8) + (((width * width) % 8) ? 1 : 0);
            RETURN_BUFFER((const char *)encoded, size);
        }
    }
END_IMPL
//------------------------------------------------------------------------
