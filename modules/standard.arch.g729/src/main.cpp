//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "pointer_list.h"
#include <string.h>
extern "C" {
    #include "bcg729/encoder.h"
    #include "bcg729/decoder.h"
}

DEFINE_LIST(g729_encoder);
DEFINE_LIST(g729_decoder);
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    INIT_LIST(g729_encoder);
    INIT_LIST(g729_decoder);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        DEINIT_LIST(g729_encoder);
        DEINIT_LIST(g729_decoder);
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(g729_encoder_create, 0, 1)
    int vad = 0;

    if (PARAMETERS_COUNT > 0){ 
        T_NUMBER(g729_encoder_create, 0);
        vad = PARAM_INT(0);
    }

    bcg729EncoderChannelContextStruct *encoder = initBcg729EncoderChannel(vad);
    if (encoder) {
        RETURN_NUMBER(MAP_POINTER(g729_encoder, encoder, NULL));
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(g729_encode, 2)
    T_HANDLE(g729_encode, 0);
    T_STRING(g729_encode, 1);

    bcg729EncoderChannelContextStruct *encoder = (bcg729EncoderChannelContextStruct *)GET_POINTER(g729_encoder, (SYS_INT)PARAM(0), NULL);
    if ((!encoder) || (PARAM_LEN(1) < 160)){
        RETURN_STRING("");
        return 0;
    }

    int16_t *inputBuffer = (int16_t *)PARAM(1);
    int inputBufferSize = PARAM_LEN(1) / 2;

    char *out_buffer = NULL;
    int out_buffer_size = inputBufferSize / 8 + 1;
    int bitstream_length = 0;

    CORE_NEW(out_buffer_size, out_buffer);
    if (!out_buffer) {
        RETURN_STRING("");
        return 0;
    }

    out_buffer[0] = 0;
    uint8_t *bitStream = (uint8_t *)out_buffer;
    while (inputBufferSize >= 80) {
        uint8_t bitStreamLength;
        bcg729Encoder(encoder, inputBuffer, bitStream, &bitStreamLength);

        inputBuffer += 80;
        inputBufferSize -= 80;

        if (bitStreamLength >= 2) {
            bitstream_length += bitStreamLength;
            out_buffer[bitstream_length] = 0;
            bitStream += bitStreamLength;
            // any silence frame must be last
            if (bitStreamLength == 2)
                break;
        }
    }
    if (!bitstream_length) {
        CORE_DELETE(out_buffer);
        RETURN_STRING("");
        return 0;
    }
    SetVariable(RESULT, -1, out_buffer, bitstream_length);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(g729_encoder_destroy, 1)
    T_HANDLE(g729_encode, 0);

    bcg729EncoderChannelContextStruct *encoder = (bcg729EncoderChannelContextStruct *)FREE_POINTER(g729_encoder, (SYS_INT)PARAM(0), NULL);
    if (encoder)
        closeBcg729EncoderChannel(encoder);

    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(g729_decoder_create, 0)
    bcg729DecoderChannelContextStruct *decoder = initBcg729DecoderChannel();
    if (decoder) {
        RETURN_NUMBER(MAP_POINTER(g729_decoder, decoder, NULL));
    } else {
        RETURN_NUMBER(0);
    }
    RETURN_NUMBER(MAP_POINTER(g729_decoder, decoder, NULL));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(g729_decode, 2)
    T_HANDLE(g729_decode, 0);
    T_STRING(g729_decode, 1);

    bcg729DecoderChannelContextStruct *decoder = (bcg729DecoderChannelContextStruct *)GET_POINTER(g729_decoder, (SYS_INT)PARAM(0), NULL);
    if ((!decoder) || (PARAM_LEN(1) < 2)) {
        RETURN_STRING("");
        return 0;
    }


    uint8_t *bitStream = (uint8_t *)PARAM(1);
    int inputBufferSize = PARAM_LEN(1);

    int outputBufferSize = inputBufferSize * 16 + 160 + 1;

    char *out_buffer = NULL;

    CORE_NEW(outputBufferSize, out_buffer);
    if (!out_buffer) {
        RETURN_STRING("");
        return 0;
    }

    out_buffer[0] = 0;

    int16_t *outputBuffer = (int16_t *)out_buffer;

    int out_buffer_decoded_len = 0;

    while (inputBufferSize >= 2) {
        int frame_size = inputBufferSize >= 10 ? 10 : 2;

        bcg729Decoder(decoder, bitStream, frame_size, 0, frame_size == 2, 0, outputBuffer);

        bitStream += frame_size;
        inputBufferSize -= frame_size;

        outputBuffer += 80;
        out_buffer_decoded_len += 160;
        out_buffer[out_buffer_decoded_len] = 0;
    }
    if (!out_buffer_decoded_len) {
        CORE_DELETE(out_buffer);
        RETURN_STRING("");
        return 0;
    }
    SetVariable(RESULT, -1, out_buffer, out_buffer_decoded_len);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(g729_decoder_destroy, 1)
    T_HANDLE(g729_encode, 0);

    bcg729DecoderChannelContextStruct *decoder = (bcg729DecoderChannelContextStruct *)FREE_POINTER(g729_decoder, (SYS_INT)PARAM(0), NULL);
    if (decoder)
        closeBcg729DecoderChannel(decoder);

    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
