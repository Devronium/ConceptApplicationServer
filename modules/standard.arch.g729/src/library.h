#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(g729_encoder_create);
    CONCEPT_FUNCTION(g729_encode);
    CONCEPT_FUNCTION(g729_encoder_destroy);
    CONCEPT_FUNCTION(g729_decoder_create);
    CONCEPT_FUNCTION(g729_decode);
    CONCEPT_FUNCTION(g729_decoder_destroy);
}
#endif // __LIBRARY_H
