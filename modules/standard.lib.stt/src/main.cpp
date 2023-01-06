//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stddef.h>
extern "C" {
    #include <coqui-stt.h>
}

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_CreateModel, 2)
    T_STRING(STT_CreateModel, 0)

    ModelState *retval = NULL;
    int err = STT_CreateModel(PARAM(0), &retval);
    SET_NUMBER(1, (SYS_INT)(void *)retval);

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_FreeModel, 1)
    T_HANDLE(STT_FreeModel, 0)

    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        STT_FreeModel(ctx);
    SET_NUMBER(0, 0);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_EnableExternalScorer, 2)
    T_HANDLE(STT_EnableExternalScorer, 0)
    T_STRING(STT_EnableExternalScorer, 1)

    int err = -1;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        err = STT_EnableExternalScorer(ctx, PARAM(1));

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_DisableExternalScorer, 1)
    T_HANDLE(STT_DisableExternalScorer, 0)

    int err = -1;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        err = STT_DisableExternalScorer(ctx);

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_AddHotWord, 3)
    T_HANDLE(STT_AddHotWord, 0)
    T_STRING(STT_AddHotWord, 1)
    T_NUMBER(STT_AddHotWord, 2)

    int err = -1;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        err = STT_AddHotWord(ctx, PARAM(1), PARAM(2));

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_EraseHotWord, 2)
    T_HANDLE(STT_EraseHotWord, 0)
    T_STRING(STT_EraseHotWord, 1)

    int err = -1;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        err = STT_EraseHotWord(ctx, PARAM(1));

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_ClearHotWords, 1)
    T_HANDLE(STT_ClearHotWords, 0)

    int err = -1;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        err = STT_ClearHotWords(ctx);

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_SetScorerAlphaBeta, 3)
    T_HANDLE(STT_SetScorerAlphaBeta, 0)
    T_NUMBER(STT_SetScorerAlphaBeta, 1)
    T_NUMBER(STT_SetScorerAlphaBeta, 2)

    int err = -1;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        err = STT_SetScorerAlphaBeta(ctx, PARAM(1), PARAM(2));

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_GetModelSampleRate, 1)
    T_HANDLE(STT_GetModelSampleRate, 0)

    int err = -1;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        err = STT_GetModelSampleRate(ctx);

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_SpeechToText, 2)
    T_HANDLE(STT_SpeechToText, 0)
    T_STRING(STT_SpeechToText, 1)

    char *stt = NULL;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        stt = STT_SpeechToText(ctx, (short *)PARAM(1), PARAM_LEN(1) / 2);

    if (stt) {
        RETURN_STRING(stt);
        STT_FreeString(stt);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
void ToArrayData(void *RESULT, INVOKE_CALL Invoke, Metadata *stt) {
    CREATE_ARRAY(RESULT);

    if (stt) {
        for (INTEGER i = 0; i < stt->num_transcripts; i ++) {
                void *element = NULL;
                Invoke(INVOKE_ARRAY_VARIABLE, RESULT, i, &element);
                CREATE_ARRAY(element);

                void *token = NULL;
                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, element, "tokens", &token);

                CREATE_ARRAY(token);

                for (INTEGER j = 0; j < stt->transcripts[i].num_tokens; j ++)
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, token, j, (INTEGER)VARIABLE_STRING, (char *)stt->transcripts[i].tokens[j].text, (NUMBER)0);

                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, element, "confidence", (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)stt->transcripts[i].confidence);
        }
        STT_FreeMetadata(stt);
    }
}

CONCEPT_FUNCTION_IMPL(STT_SpeechToTextWithMetadata, 3)
    T_HANDLE(STT_SpeechToTextWithMetadata, 0)
    T_STRING(STT_SpeechToTextWithMetadata, 1)
    T_NUMBER(STT_SpeechToTextWithMetadata, 2)

    Metadata *stt = NULL;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        stt = STT_SpeechToTextWithMetadata(ctx, (short *)PARAM(1), PARAM_LEN(1) / 2, PARAM_INT(2));

    ToArrayData(RESULT, Invoke, stt);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_CreateStream, 2)
    T_HANDLE(STT_SpeechToTextWithMetadata, 0)

    int err = -1;
    StreamingState *retval = NULL;
    ModelState *ctx = (ModelState *)(SYS_INT)PARAM(0);
    if (ctx)
        err = STT_CreateStream(ctx, &retval);

    SET_NUMBER(1, (SYS_INT)(void *)retval);

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_FeedAudioContent, 2)
    T_HANDLE(STT_FeedAudioContent, 0)
    T_STRING(STT_FeedAudioContent, 1)

    StreamingState *aSctx = (StreamingState *)(SYS_INT)PARAM(0);
    if (aSctx)
        STT_FeedAudioContent(aSctx, (short *)PARAM(1), PARAM_LEN(1) / 2);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_IntermediateDecode, 1)
    T_HANDLE(STT_IntermediateDecode, 0)

    char *stt = NULL;
    StreamingState *aSctx = (StreamingState *)(SYS_INT)PARAM(0);
    if (aSctx)
        stt = STT_IntermediateDecode(aSctx);

    if (stt) {
        RETURN_STRING(stt);
        STT_FreeString(stt);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_IntermediateDecodeWithMetadata, 1)
    T_HANDLE(STT_IntermediateDecodeWithMetadata, 0)
    T_NUMBER(STT_IntermediateDecodeWithMetadata, 1)

    Metadata *stt = NULL;
    StreamingState *aSctx = (StreamingState *)(SYS_INT)PARAM(0);
    if (aSctx)
        stt = STT_IntermediateDecodeWithMetadata(aSctx, PARAM_INT(1));

    ToArrayData(RESULT, Invoke, stt);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_IntermediateDecodeFlushBuffers, 0)
    T_HANDLE(STT_IntermediateDecodeFlushBuffers, 0)

    char *stt = NULL;
    StreamingState *aSctx = (StreamingState *)(SYS_INT)PARAM(0);
    if (aSctx)
        stt = STT_IntermediateDecodeFlushBuffers(aSctx);

    if (stt) {
        RETURN_STRING(stt);
        STT_FreeString(stt);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_IntermediateDecodeWithMetadataFlushBuffers, 2)
    T_HANDLE(STT_IntermediateDecodeWithMetadataFlushBuffers, 0)
    T_NUMBER(STT_IntermediateDecodeWithMetadataFlushBuffers, 1)

    Metadata *stt = NULL;
    StreamingState *aSctx = (StreamingState *)(SYS_INT)PARAM(0);
    if (aSctx)
        stt = STT_IntermediateDecodeWithMetadataFlushBuffers(aSctx, PARAM_INT(1));

    ToArrayData(RESULT, Invoke, stt);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_FinishStream, 1)
    T_HANDLE(STT_FinishStream, 0)

    char *stt = NULL;
    StreamingState *aSctx = (StreamingState *)(SYS_INT)PARAM(0);
    if (aSctx)
        stt = STT_FinishStream(aSctx);

    if (stt) {
        RETURN_STRING(stt);
        STT_FreeString(stt);
    } else {
        RETURN_STRING("");
    }
    SET_NUMBER(0, 0)
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_FinishStreamWithMetadata, 2)
    T_HANDLE(STT_FinishStreamWithMetadata, 0)
    T_NUMBER(STT_FinishStreamWithMetadata, 1)

    Metadata *stt = NULL;
    StreamingState *aSctx = (StreamingState *)(SYS_INT)PARAM(0);
    if (aSctx)
        stt = STT_FinishStreamWithMetadata(aSctx, PARAM_INT(1));

    ToArrayData(RESULT, Invoke, stt);

    SET_NUMBER(0,0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_FreeStream, 1)
    T_HANDLE(STT_FreeStream, 0)

    StreamingState *aSctx = (StreamingState *)(SYS_INT)PARAM(0);
    if (aSctx)
        STT_FreeStream(aSctx);

    SET_NUMBER(0, 0);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(STT_Version, 0)
    char *version = STT_Version();
    RETURN_STRING(version);
    STT_FreeString(version);
END_IMPL
//=====================================================================================//
