#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

CONCEPT_FUNCTION(opus_encoder_create)
CONCEPT_FUNCTION(opus_encode)
CONCEPT_FUNCTION(opus_encoder_ctl)
CONCEPT_FUNCTION(opus_encoder_destroy)

CONCEPT_FUNCTION(opus_decoder_create)
CONCEPT_FUNCTION(opus_decode)
CONCEPT_FUNCTION(opus_decoder_ctl)
CONCEPT_FUNCTION(opus_decoder_destroy)
//CONCEPT_FUNCTION(opus_packet_parse)
CONCEPT_FUNCTION(opus_packet_get_bandwidth)
CONCEPT_FUNCTION(opus_packet_get_nb_channels)
CONCEPT_FUNCTION(opus_packet_get_nb_frames)
CONCEPT_FUNCTION(opus_packet_get_nb_samples)
CONCEPT_FUNCTION(opus_decoder_get_nb_samples)

CONCEPT_FUNCTION(opus_repacketizer_create)
CONCEPT_FUNCTION(opus_repacketizer_destroy)
CONCEPT_FUNCTION(opus_repacketizer_cat)
CONCEPT_FUNCTION(opus_repacketizer_out_range)
CONCEPT_FUNCTION(opus_repacketizer_get_nb_frames)
CONCEPT_FUNCTION(opus_repacketizer_out)

/*CONCEPT_FUNCTION(opus_multistream_encoder_create)
   CONCEPT_FUNCTION(opus_multistream_encoder_init)
   CONCEPT_FUNCTION(opus_multistream_encode)
   CONCEPT_FUNCTION(opus_multistream_encoder_destroy)
   CONCEPT_FUNCTION(opus_multistream_encoder_ctl)
   CONCEPT_FUNCTION(opus_multistream_decoder_create)
   CONCEPT_FUNCTION(opus_multistream_decoder_init)
   CONCEPT_FUNCTION(opus_multistream_decode)
   CONCEPT_FUNCTION(opus_multistream_decoder_ctl)
   CONCEPT_FUNCTION(opus_multistream_decoder_destroy)*/

CONCEPT_FUNCTION(alaw2ulaw)
CONCEPT_FUNCTION(ulaw2alaw)
CONCEPT_FUNCTION(alaw2pcm)
CONCEPT_FUNCTION(ulaw2pcm)

CONCEPT_FUNCTION(pcm2ulaw)
CONCEPT_FUNCTION(pcm2alaw)

CONCEPT_FUNCTION(downsample44k)
}
#endif // __LIBRARY_H
