//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <atomic>

#define QUEUE_LOCK(sm)      while (sm.test_and_set(std::memory_order_acquire)) { }
#define QUEUE_UNLOCK(sm)    sm.clear(std::memory_order_release);
#define QUEUE_DONE(sm)
#define QUEUE_CREATE(sm)
#define QUEUE_SEMAPHORE     std::atomic_flag

#include "firewall.c"

struct firewall_container *firewall = NULL;
QUEUE_SEMAPHORE semaphore = ATOMIC_FLAG_INIT;

#define ENSURE_FIREWALL if (!firewall) firewall = firewall_init(4 * 60, 3600 * 4, 2, 10, 200);
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    QUEUE_CREATE(semaphore);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        if (firewall)
            firewall_free(firewall);

        QUEUE_DONE(semaphore);
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_is_blocked, 1)
    T_STRING(firewall_is_blocked, 0)

    ENSURE_FIREWALL;

    if (!firewall) {
        RETURN_NUMBER(-1);
        return 0;
    }

    QUEUE_LOCK(semaphore);
    int e = firewall_is_blocked(firewall, PARAM(0));
    QUEUE_UNLOCK(semaphore);

    RETURN_NUMBER(e);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(firewall_light_block, 1, 2)
    T_STRING(firewall_light_block, 0)

    int counter = 1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(firewall_light_block, 1)
        counter = PARAM_INT(1);
        if (counter <= 0)
            counter = 1;
    }

    ENSURE_FIREWALL;

    QUEUE_LOCK(semaphore);
    int e = firewall_light_block(firewall, PARAM(0), counter);
    QUEUE_UNLOCK(semaphore);

    RETURN_NUMBER(e);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(firewall_heavy_block, 1, 2)
    T_STRING(firewall_heavy_block, 0)

    int counter = 1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(firewall_heavy_block, 1)
        counter = PARAM_INT(1);
        if (counter <= 0)
            counter = 1;
    }

    ENSURE_FIREWALL;

    if (!firewall) {
        RETURN_NUMBER(-1);
        return 0;
    }

    QUEUE_LOCK(semaphore);
    int e = firewall_heavy_block(firewall, PARAM(0), counter);
    QUEUE_UNLOCK(semaphore);

    RETURN_NUMBER(e);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_always_block, 1)
    T_STRING(firewall_always_block, 0)

    ENSURE_FIREWALL;

    if (!firewall) {
        RETURN_NUMBER(-1);
        return 0;
    }

    QUEUE_LOCK(semaphore);
    int e = firewall_always_block(firewall, PARAM(0));
    QUEUE_UNLOCK(semaphore);

    RETURN_NUMBER(e);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_inc_request_count, 1)
    T_STRING(firewall_inc_request_count, 0)

    ENSURE_FIREWALL;

    if (!firewall) {
        RETURN_NUMBER(-1);
        return 0;
    }

    QUEUE_LOCK(semaphore);
    int e = firewall_inc_request_count(firewall, PARAM(0));
    QUEUE_UNLOCK(semaphore);

    RETURN_NUMBER(e);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(firewall_flag, 1, 2);
    T_STRING(firewall_flag, 0)

    int counter = 1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(firewall_flag, 1)
        counter = PARAM_INT(1);
    }

    ENSURE_FIREWALL;

    if (!firewall) {
        RETURN_NUMBER(-1);
        return 0;
    }

    QUEUE_LOCK(semaphore);
    int e = firewall_flag(firewall, PARAM(0), counter);
    QUEUE_UNLOCK(semaphore);

    RETURN_NUMBER(e);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_is_flagged, 1)
    T_STRING(firewall_is_flagged, 0)

    ENSURE_FIREWALL;

    if (!firewall) {
        RETURN_NUMBER(-1);
        return 0;
    }

    QUEUE_LOCK(semaphore);
    int e = firewall_is_flagged(firewall, PARAM(0));
    QUEUE_UNLOCK(semaphore);

    RETURN_NUMBER(e);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_reset, 0)
    if (firewall) {
        QUEUE_LOCK(semaphore);
        firewall_free(firewall);
        firewall = NULL;
        QUEUE_UNLOCK(semaphore);
    }
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_set_light_threshold, 1)
    T_NUMBER(firewall_set_light_threshold, 0)

    ENSURE_FIREWALL;

    if (firewall) {
        firewall->rotation_light_threshold = PARAM_INT(0);
        if (firewall->rotation_light_threshold <= 0)
            firewall->rotation_light_threshold = 10;
    }

    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_set_heavy_threshold, 1)
    T_NUMBER(firewall_set_heavy_threshold, 0)

    ENSURE_FIREWALL;

    if (firewall) {
        firewall->rotation_heavy_threshold = PARAM_INT(0);
        if (firewall->rotation_heavy_threshold <= 0)
            firewall->rotation_heavy_threshold = 200;
    }

    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_set_light_timeout, 1)
    T_NUMBER(firewall_set_light_timeout, 0)

    ENSURE_FIREWALL;

    if (firewall) {
        firewall->rotation_light_timeout = PARAM_INT(0);
        if (firewall->rotation_light_timeout <= 0)
            firewall->rotation_light_timeout = 4 * 60;

        firewall->rotation_light_timestamp = time(NULL) + firewall->rotation_light_timeout;
    }

    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_set_heavy_timeout, 1)
    T_NUMBER(firewall_set_heavy_timeout, 0)

    ENSURE_FIREWALL;

    if (firewall) {
        firewall->rotation_heavy_timeout = PARAM_INT(0);
        if (firewall->rotation_heavy_timeout <= 0)
            firewall->rotation_heavy_timeout = 3600 * 4;

        firewall->rotation_heavy_timeout = time(NULL) + firewall->rotation_heavy_timeout;
    }

    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(firewall_set_requests_per_minute_threshold, 1)
    T_NUMBER(firewall_set_requests_per_minute_threshold, 0)

    ENSURE_FIREWALL;

    if (firewall) {
        firewall->requests_per_minute_threshold = PARAM_INT(0);
        if (firewall->requests_per_minute_threshold <= 0)
            firewall->requests_per_minute_threshold = 200;
    }

    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
