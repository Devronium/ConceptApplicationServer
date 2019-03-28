//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stdlib.h>
#include <string.h>
#include "lfq.h"
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(LFQueue, 0, 1)
    int max_consume_thread = 1;
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(LFQueue, 0)
        max_consume_thread = PARAM_INT(0);
        if (max_consume_thread < 1)
            max_consume_thread = 1;

    }
    struct lfq_ctx *queue = (struct lfq_ctx *)malloc(sizeof(struct lfq_ctx));
    if (queue) {
        if (lfq_init(queue, max_consume_thread)) {
            free(queue);
            queue = NULL;
        }
    }
    RETURN_NUMBER((uintptr_t)queue);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(LFQueueEnqueue, 2)
    T_HANDLE(LFQueueEnqueue, 0)
    T_STRING(LFQueueEnqueue, 1)

    struct lfq_ctx *queue = (struct lfq_ctx *)(uintptr_t)PARAM(0);

    unsigned int len = (unsigned int)PARAM_LEN(1);
    char *buf = malloc(sizeof(char) * len + sizeof(unsigned int));
    int err = -1;
    if (buf) {
        memcpy(buf, &len, sizeof(unsigned int));
        memcpy(buf + sizeof(unsigned int), PARAM(1), len);
        err = lfq_enqueue(queue, buf);
        if (err)
            free(buf);
    }
    RETURN_NUMBER(err);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(LFQueueDequeue, 1, 2)
    T_HANDLE(LFQueueDequeue, 0)
    int tid = -1;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(LFQueueDequeue, 1)
        tid = PARAM_INT(1);
    }

    struct lfq_ctx *queue = (struct lfq_ctx *)(uintptr_t)PARAM(0);
    void *buf;
    if (tid < 0)
        buf = lfq_dequeue(queue);
    else
        buf = lfq_dequeue_tid(queue, tid);

    if (buf) {
        unsigned int len;
        memcpy(&len, buf, sizeof(unsigned int));
        char *ptr = (char *)buf;
        ptr += sizeof(unsigned int);
        if (len) {
            RETURN_BUFFER(ptr, len);
        } else {
            RETURN_STRING("");
        }
        free(buf);
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(LFQueueDestroy, 1)
    T_HANDLE(LFQueueRelease, 0)

    struct lfq_ctx *queue = (struct lfq_ctx *)(uintptr_t)PARAM(0);
    lfq_clean(queue);
    free(queue);

    SET_NUMBER(0, 0);
    return 0;
END_IMPL
//---------------------------------------------------------------------------
