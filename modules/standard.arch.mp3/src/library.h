#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(MP3EncoderCreate);
    CONCEPT_FUNCTION(MP3EncoderSamples);
    CONCEPT_FUNCTION(MP3EncoderEncode);
    CONCEPT_FUNCTION(MP3EncoderFlush);
    CONCEPT_FUNCTION(MP3EncoderDone);
}
#endif // __LIBRARY_H