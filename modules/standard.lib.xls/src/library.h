#include "stdlibrary.h"

extern "C" {
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS;

CONCEPT_FUNCTION(xls_getVersion)

CONCEPT_FUNCTION(xls_open)
CONCEPT_FUNCTION(xls_close)

CONCEPT_FUNCTION(xls_parseWorkBook)
CONCEPT_FUNCTION(xls_parseWorkSheet)

CONCEPT_FUNCTION(xls_getWorkSheet)
CONCEPT_FUNCTION(xls_row)
CONCEPT_FUNCTION(xls_cell)
CONCEPT_FUNCTION(xls_matrix)

CONCEPT_FUNCTION(xls_getLastRow)
CONCEPT_FUNCTION(xls_getLastCol)
CONCEPT_FUNCTION(xls_getCellProperties)
CONCEPT_FUNCTION(xls_getRowProperties)
CONCEPT_FUNCTION(xls_getWorkSheetProperties)
CONCEPT_FUNCTION(xls_getWorkBookProperties)
}
