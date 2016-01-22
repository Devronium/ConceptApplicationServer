#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

CONCEPT_FUNCTION(speex_bits_init)              // (SpeexBits* bits)
CONCEPT_FUNCTION(speex_bits_init_buffer)       // (SpeexBits* bits, void* buff, int buf_size)
CONCEPT_FUNCTION(speex_bits_set_bit_buffer)    // (SpeexBits* bits, void* buff, int buf_size)
CONCEPT_FUNCTION(speex_bits_destroy)           // (SpeexBits* bits)
CONCEPT_FUNCTION(speex_bits_reset)             // (SpeexBits* bits)
CONCEPT_FUNCTION(speex_bits_rewind)            // (SpeexBits* bits)
CONCEPT_FUNCTION(speex_bits_read_from)         // (SpeexBits* bits, char* bytes, int len)
CONCEPT_FUNCTION(speex_bits_read_whole_bytes)  // (SpeexBits* bits, char* bytes, int len)
CONCEPT_FUNCTION(speex_bits_write)             // (SpeexBits* bits, char* bytes, int max_len)
CONCEPT_FUNCTION(speex_bits_write_whole_bytes) // (SpeexBits* bits, char* bytes, int max_len)
CONCEPT_FUNCTION(speex_bits_pack)              // (SpeexBits* bits, int data, int nbBits)
CONCEPT_FUNCTION(speex_bits_unpack_signed)     // (SpeexBits* bits, int nbBits)
CONCEPT_FUNCTION(speex_bits_unpack_unsigned)   // (SpeexBits* bits, int nbBits)
CONCEPT_FUNCTION(speex_bits_nbytes)            // (SpeexBits* bits)
CONCEPT_FUNCTION(speex_bits_peek_unsigned)     // (SpeexBits* bits, int nbBits)
CONCEPT_FUNCTION(speex_bits_peek)              // (SpeexBits* bits)
CONCEPT_FUNCTION(speex_bits_advance)           // (SpeexBits* bits, int n)
CONCEPT_FUNCTION(speex_bits_remaining)         // (SpeexBits* bits)
CONCEPT_FUNCTION(speex_bits_insert_terminator) // (SpeexBits* bits)

CONCEPT_FUNCTION(speex_encoder_init)           // (SpeexMode* mode)
CONCEPT_FUNCTION(speex_encoder_destroy)        // (void* state)
CONCEPT_FUNCTION(speex_encode_int)             // (void* state, spx_int16_t* in, SpeexBits* bits)
CONCEPT_FUNCTION(speex_encoder_ctl)            // (void* state, int request, void* ptr)
CONCEPT_FUNCTION(speex_decoder_init)           // (SpeexMode* mode)
CONCEPT_FUNCTION(speex_decoder_destroy)        // (void* state)
CONCEPT_FUNCTION(speex_decode_int)             // (void* state, SpeexBits* bits, spx_int16_t* out)
CONCEPT_FUNCTION(speex_decoder_ctl)            // (void* state, int request, void* ptr)
CONCEPT_FUNCTION(speex_mode_query)             // (SpeexMode* mode, int request, void* ptr)
CONCEPT_FUNCTION(speex_lib_ctl)                // (int request, void* ptr)
CONCEPT_FUNCTION(speex_lib_get_mode)           // (int mode)

CONCEPT_FUNCTION(alaw2ulaw)
CONCEPT_FUNCTION(ulaw2alaw)
CONCEPT_FUNCTION(alaw2pcm)
CONCEPT_FUNCTION(ulaw2pcm)

CONCEPT_FUNCTION(pcm2ulaw)
CONCEPT_FUNCTION(pcm2alaw)

CONCEPT_FUNCTION(s162alaw)
CONCEPT_FUNCTION(alaw2s16)

CONCEPT_FUNCTION(downsample44k)

CONCEPT_FUNCTION(speex_echo_state_init)
CONCEPT_FUNCTION(speex_echo_state_destroy)
CONCEPT_FUNCTION(speex_echo_state_reset)
CONCEPT_FUNCTION(speex_echo_playback)
CONCEPT_FUNCTION(speex_echo_capture)
CONCEPT_FUNCTION(speex_echo_cancellation)
}
#endif // __LIBRARY_H
