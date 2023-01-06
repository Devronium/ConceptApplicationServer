#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(STT_CreateModel)
    CONCEPT_FUNCTION(STT_FreeModel)

    CONCEPT_FUNCTION(STT_EnableExternalScorer)
    CONCEPT_FUNCTION(STT_DisableExternalScorer)

    CONCEPT_FUNCTION(STT_AddHotWord)
    CONCEPT_FUNCTION(STT_EraseHotWord)
    CONCEPT_FUNCTION(STT_ClearHotWords)
    CONCEPT_FUNCTION(STT_SetScorerAlphaBeta)

    CONCEPT_FUNCTION(STT_GetModelSampleRate)

    CONCEPT_FUNCTION(STT_SpeechToText)
    CONCEPT_FUNCTION(STT_SpeechToTextWithMetadata)

    CONCEPT_FUNCTION(STT_CreateStream)
    CONCEPT_FUNCTION(STT_FeedAudioContent)
    CONCEPT_FUNCTION(STT_IntermediateDecode)
    CONCEPT_FUNCTION(STT_IntermediateDecodeWithMetadata)
    CONCEPT_FUNCTION(STT_IntermediateDecodeFlushBuffers)
    CONCEPT_FUNCTION(STT_IntermediateDecodeWithMetadataFlushBuffers)
    CONCEPT_FUNCTION(STT_FinishStream)
    CONCEPT_FUNCTION(STT_FinishStreamWithMetadata)
    CONCEPT_FUNCTION(STT_FreeStream)

    CONCEPT_FUNCTION(STT_Version)
}
#endif // __LIBRARY_H
