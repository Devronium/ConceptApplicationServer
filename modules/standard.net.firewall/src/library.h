#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(firewall_is_blocked);
    CONCEPT_FUNCTION(firewall_light_block);
    CONCEPT_FUNCTION(firewall_heavy_block);
    CONCEPT_FUNCTION(firewall_always_block);
    CONCEPT_FUNCTION(firewall_flag);
    CONCEPT_FUNCTION(firewall_is_flagged);
    CONCEPT_FUNCTION(firewall_inc_request_count);
    CONCEPT_FUNCTION(firewall_reset);

    CONCEPT_FUNCTION(firewall_set_light_threshold);
    CONCEPT_FUNCTION(firewall_set_heavy_threshold);
    CONCEPT_FUNCTION(firewall_set_light_timeout);
    CONCEPT_FUNCTION(firewall_set_heavy_timeout);
    CONCEPT_FUNCTION(firewall_set_requests_per_minute_threshold);
}
#endif // __LIBRARY_H
