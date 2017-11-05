#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(lucy_new)
    CONCEPT_FUNCTION(lucy_done)
    CONCEPT_FUNCTION(lucy_add)
    CONCEPT_FUNCTION(lucy_search)

    CONCEPT_FUNCTION(lucy_indexer_new)
    CONCEPT_FUNCTION(lucy_indexer_add)
    CONCEPT_FUNCTION(lucy_indexer_remove)
    CONCEPT_FUNCTION(lucy_indexer_remove_query)
    CONCEPT_FUNCTION(lucy_indexer_commit)
    CONCEPT_FUNCTION(lucy_indexer_optimize)
    CONCEPT_FUNCTION(lucy_indexer_done)

    CONCEPT_FUNCTION(lucy_valid_utf8)
}
#endif // __LIBRARY_H
