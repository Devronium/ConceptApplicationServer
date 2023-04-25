#ifndef __LIBRARY_H
#define __LIBRARY_H


// user definition ...

extern "C" {
CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

CONCEPT_FUNCTION(_zip_open)
CONCEPT_FUNCTION(_unzip)
CONCEPT_FUNCTION(_zip)
CONCEPT_FUNCTION(__zip_name_locate)
CONCEPT_FUNCTION(_zip_fopen)
CONCEPT_FUNCTION(_zip_fopen_index)
CONCEPT_FUNCTION(_zip_fread)
CONCEPT_FUNCTION(_zip_fclose)
CONCEPT_FUNCTION(_zip_close)
CONCEPT_FUNCTION(_zip_stat)
CONCEPT_FUNCTION(_zip_get_archive_comment)
CONCEPT_FUNCTION(_zip_get_file_comment)
CONCEPT_FUNCTION(__zip_get_name)
CONCEPT_FUNCTION(_zip_get_num_files)
CONCEPT_FUNCTION(_zip_add)
CONCEPT_FUNCTION(__zip_replace)
CONCEPT_FUNCTION(_zip_set_file_comment)
CONCEPT_FUNCTION(_zip_source_buffer)
CONCEPT_FUNCTION(_zip_source_file)
CONCEPT_FUNCTION(_zip_source_filep)
CONCEPT_FUNCTION(_zip_source_zip)
CONCEPT_FUNCTION(_zip_source_free)
CONCEPT_FUNCTION(_zip_rename)
CONCEPT_FUNCTION(_zip_delete)
CONCEPT_FUNCTION(__zip_unchange)
CONCEPT_FUNCTION(_zip_unchange_all)
CONCEPT_FUNCTION(_zip_unchange_archive)
CONCEPT_FUNCTION(_zip_set_archive_comment)
CONCEPT_FUNCTION(_zip_strerror)
CONCEPT_FUNCTION(_zip_file_strerror)
}
#endif // __LIBRARY_H
