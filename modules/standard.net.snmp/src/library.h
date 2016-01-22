#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

CONCEPT_FUNCTION(add_mibdir)
CONCEPT_FUNCTION(read_mib)
CONCEPT_FUNCTION(netsnmp_init_mib)
CONCEPT_FUNCTION(shutdown_mib)

CONCEPT_FUNCTION(snmp_open)
CONCEPT_FUNCTION(snmp_close)
CONCEPT_FUNCTION(snmp_error)
CONCEPT_FUNCTION(snmp_sess_error)
CONCEPT_FUNCTION(snmp_open)


CONCEPT_FUNCTION(snmp_pdu_create)
//CONCEPT_FUNCTION(snmp_parse_oid)
CONCEPT_FUNCTION(snmp_add_null_var)
CONCEPT_FUNCTION(snmp_add_var)
CONCEPT_FUNCTION(snmp_synch_response)
CONCEPT_FUNCTION(snmp_clone_pdu)
CONCEPT_FUNCTION(snmp_fix_pdu)
CONCEPT_FUNCTION(snmp_free_pdu)

CONCEPT_FUNCTION(netsnmp_ds_toggle_boolean)
CONCEPT_FUNCTION(netsnmp_ds_get_boolean)
CONCEPT_FUNCTION(netsnmp_ds_set_boolean)
CONCEPT_FUNCTION(netsnmp_ds_get_int)
CONCEPT_FUNCTION(netsnmp_ds_set_int)
CONCEPT_FUNCTION(netsnmp_ds_get_string)
CONCEPT_FUNCTION(netsnmp_ds_set_string)
}
#endif // __LIBRARY_H
