#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...

extern "C" {
CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

CONCEPT_FUNCTION(Hunspell_create)
CONCEPT_FUNCTION(Hunspell_create_key)
CONCEPT_FUNCTION(Hunspell_destroy)
CONCEPT_FUNCTION(Hunspell_spell)
CONCEPT_FUNCTION(Hunspell_suggest)
CONCEPT_FUNCTION(Hunspell_get_dic_encoding)
CONCEPT_FUNCTION(Hunspell_analyze)
CONCEPT_FUNCTION(Hunspell_stem)
CONCEPT_FUNCTION(Hunspell_stem2)
CONCEPT_FUNCTION(Hunspell_generate)
CONCEPT_FUNCTION(Hunspell_generate2)
CONCEPT_FUNCTION(Hunspell_add)
CONCEPT_FUNCTION(Hunspell_add_with_affix)
CONCEPT_FUNCTION(Hunspell_remove)
}
#endif // __LIBRARY_H
