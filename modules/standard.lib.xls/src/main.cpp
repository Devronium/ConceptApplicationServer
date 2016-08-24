//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
extern "C" {
#include <libxls/xls.h>
}

//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_getVersion, 0)
    RETURN_STRING(xls_getVersion())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_open, 2)
    T_STRING(xls_open, 0)
    T_STRING(xls_open, 1)
    RETURN_NUMBER((long)xls_open(PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_close, 1)
    T_HANDLE(xls_close, 0)
    xlsWorkBook * wb = (xlsWorkBook *)PARAM_INT(0);
    xls_close(wb);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_parseWorkBook, 1)
    T_HANDLE(xls_parseWorkBook, 0)
    xlsWorkBook * wb = (xlsWorkBook *)PARAM_INT(0);
    xls_parseWorkBook(wb);

    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_parseWorkSheet, 1)
    T_HANDLE(xls_parseWorkSheet, 0)
    xlsWorkSheet * ws = (xlsWorkSheet *)PARAM_INT(0);
    xls_parseWorkSheet(ws);

    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_getWorkSheet, 2)
    T_HANDLE(xls_getWorkSheet, 0)
    T_NUMBER(xls_getWorkSheet, 1)
    xlsWorkBook * wb = (xlsWorkBook *)PARAM_INT(0);
    int sheet = PARAM_INT(1);
    if ((sheet >= 0) && (sheet < wb->sheets.count)) {
        RETURN_NUMBER((long)xls_getWorkSheet(wb, PARAM_INT(1)))
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_cell, 3)
    T_HANDLE(xls_cell, 0)
    T_NUMBER(xls_cell, 1)
    T_NUMBER(xls_cell, 2)
    xlsWorkSheet * pWS = (xlsWorkSheet *)PARAM_INT(0);

    int irow  = PARAM_INT(1);
    int icell = PARAM_INT(2);

    if ((irow >= 0) && (irow <= pWS->rows.lastrow) && (icell >= 0) && (icell <= pWS->rows.lastcol)) {
        st_cell::st_cell_data *cell = &pWS->rows.row[irow].cells.cell[icell];
        if ((cell->id == 0x27e) || (cell->id == 0x0BD) || (cell->id == 0x203)) {
            RETURN_NUMBER(cell->d);
        }  else
        if (cell->str != NULL) {
            RETURN_STRING(cell->str);
        } else {
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_getLastRow, 1)
    T_HANDLE(xls_getLastRow, 0)
    xlsWorkSheet * pWS = (xlsWorkSheet *)PARAM_INT(0);
    RETURN_NUMBER(pWS->rows.lastrow)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_getLastCol, 1)
    T_HANDLE(xls_getLastCol, 0)
    xlsWorkSheet * pWS = (xlsWorkSheet *)PARAM_INT(0);
    RETURN_NUMBER(pWS->rows.lastcol)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_getCellProperties, 3)
    T_HANDLE(xls_getCellProperties, 0)
    T_NUMBER(xls_getCellProperties, 1)
    T_NUMBER(xls_getCellProperties, 2)
    xlsWorkSheet * pWS = (xlsWorkSheet *)PARAM_INT(0);

    int irow  = PARAM_INT(1);
    int icell = PARAM_INT(2);

    CREATE_ARRAY(RESULT);
    if ((irow >= 0) && (irow <= pWS->rows.lastrow) && (icell >= 0) && (icell <= pWS->rows.lastcol)) {
        st_cell::st_cell_data *cell = &pWS->rows.row[irow].cells.cell[icell];
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "id", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->id);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "row", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->row);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "col", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->col);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "xf", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->xf);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "d", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->d);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "l", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->l);
        if (cell->str)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "str", (INTEGER)VARIABLE_STRING, (char *)cell->str, (NUMBER)0);
        else
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "str", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ishiden", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->ishiden);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "width", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->width);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "colspan", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->colspan);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rowspan", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->rowspan);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_getRowProperties, 2)
    T_HANDLE(xls_getRowProperties, 0)
    T_NUMBER(xls_getRowProperties, 1)
    xlsWorkSheet * pWS = (xlsWorkSheet *)PARAM_INT(0);

    int irow = PARAM_INT(1);

    CREATE_ARRAY(RESULT);
    if ((irow >= 0) && (irow <= pWS->rows.lastrow)) {
        st_row::st_row_data *row = &pWS->rows.row[irow];
        if (row) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "index", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)row->index);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "fcell", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)row->fcell);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "lcell", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)row->lcell);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "flags", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)row->flags);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "xf", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)row->xf);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "xfflags", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)row->xfflags);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "cells.count", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)row->cells.count);
        }
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_row, 2)
    T_HANDLE(xls_row, 0)
    T_NUMBER(xls_row, 1)
    xlsWorkSheet * pWS = (xlsWorkSheet *)PARAM_INT(0);

    int irow = PARAM_INT(1);

    CREATE_ARRAY(RESULT);
    int index = 0;
    if ((irow >= 0) && (irow <= pWS->rows.lastrow)) {
        st_row::st_row_data *row = &pWS->rows.row[irow];
        if (row) {
            for (int tt = 0; tt <= pWS->rows.lastcol; tt++) {
                st_cell::st_cell_data *cell = &row->cells.cell[tt];
                if ((cell) && (!cell->ishiden)) {
                    if ((cell->id == 0x27e) || (cell->id == 0x0BD) || (cell->id == 0x203)) {
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->d);
                    }  else
                    if (cell->str != NULL) {
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, (INTEGER)VARIABLE_STRING, (char *)cell->str, (NUMBER)0);
                    } else {
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
                    }
                    index++;
                }
            }
        }
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_matrix, 1)
    T_HANDLE(xls_matrix, 0)
    xlsWorkSheet * pWS = (xlsWorkSheet *)PARAM_INT(0);

    CREATE_ARRAY(RESULT);
    for (int irow = 0; irow < pWS->rows.lastrow; irow++) {
        void *container = 0;
        Invoke(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)irow, &container);
        if (container) {
            Invoke(INVOKE_CREATE_ARRAY, container);
            st_row::st_row_data *row = &pWS->rows.row[irow];
            int index = 0;
            for (int tt = 0; tt <= pWS->rows.lastcol; tt++) {
                st_cell::st_cell_data *cell = &row->cells.cell[tt];
                if ((cell) && (!cell->ishiden)) {
                    if ((cell->id == 0x27e) || (cell->id == 0x0BD) || (cell->id == 0x203)) {
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, container, (INTEGER)index, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cell->d);
                    }  else
                    if (cell->str != NULL) {
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, container, (INTEGER)index, (INTEGER)VARIABLE_STRING, (char *)cell->str, (NUMBER)0);
                    } else {
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, container, (INTEGER)index, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
                    }
                    index++;
                }
            }
        }
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_getWorkSheetProperties, 1)
    T_HANDLE(xls_getWorkSheetProperties, 0)
    xlsWorkSheet * pWS = (xlsWorkSheet *)PARAM_INT(0);

    CREATE_ARRAY(RESULT);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "filepos", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWS->filepos);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "defcolwidth", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWS->defcolwidth);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "maxcol", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWS->maxcol);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "colinfo.count", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWS->colinfo.count);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(xls_getWorkBookProperties, 1)
    T_HANDLE(xls_getWorkBookProperties, 0)
    xlsWorkBook * pWB = (xlsWorkBook *)PARAM_INT(0);

    CREATE_ARRAY(RESULT);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "filepos", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->filepos);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "is5ver", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->is5ver);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "type", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->type);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "codepage", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->codepage);
    if (pWB->charset)
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "charset", (INTEGER)VARIABLE_STRING, (char *)pWB->charset, (NUMBER)0);
    else
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "charset", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sheets.count", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->sheets.count);

    void *container = 0;
    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "sheets", &container);
    if (container) {
        Invoke(INVOKE_CREATE_ARRAY, container);
        for (int i = 0; i < pWB->sheets.count; i++) {
            void *newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, container, (INTEGER)i, &newpData);
            Invoke(INVOKE_CREATE_ARRAY, newpData);
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "sheets", &container);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "filepos", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->sheets.sheet[i].filepos);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "visibility", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->sheets.sheet[i].visibility);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "type", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->sheets.sheet[i].type);
            if (pWB->sheets.sheet[i].name)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "name", (INTEGER)VARIABLE_STRING, (char *)pWB->sheets.sheet[i].name, (NUMBER)0);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "name", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
        }
    }

    void *container2 = 0;
    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "xfs", &container2);
    if (container2) {
        Invoke(INVOKE_CREATE_ARRAY, container2);
        for (int i = 0; i < pWB->xfs.count; i++) {
            void *newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, container2, (INTEGER)i, &newpData);
            Invoke(INVOKE_CREATE_ARRAY, newpData);
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "xfs", &container2);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "font", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->xfs.xf[i].font);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "format", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->xfs.xf[i].format);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "type", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->xfs.xf[i].type);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "align", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->xfs.xf[i].align);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "rotation", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->xfs.xf[i].rotation);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "ident", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->xfs.xf[i].ident);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "usedattr", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->xfs.xf[i].usedattr);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "linestyle", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->xfs.xf[i].linestyle);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "linecolor", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)xls_getColor(pWB->xfs.xf[i].linecolor, 0));
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "groundcolor", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)xls_getColor((WORD)(pWB->xfs.xf[i].groundcolor & 0x7f), 1));
        }
    }

    void *container3 = 0;
    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "fonts", &container3);
    if (container3) {
        Invoke(INVOKE_CREATE_ARRAY, container3);
        for (int i = 0; i < pWB->fonts.count; i++) {
            void *newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, container3, (INTEGER)i, &newpData);
            Invoke(INVOKE_CREATE_ARRAY, newpData);
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "fonts", &container3);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "height", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->fonts.font[i].height);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "flag", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->fonts.font[i].flag);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "color", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)xls_getColor(pWB->fonts.font[i].color, 0));
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "bold", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->fonts.font[i].bold);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "escapement", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->fonts.font[i].escapement);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "underline", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->fonts.font[i].underline);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "family", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->fonts.font[i].family);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "charset", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->fonts.font[i].charset);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "name", (INTEGER)VARIABLE_STRING, (char *)pWB->fonts.font[i].name, (NUMBER)0);
        }
    }

    void *container4 = 0;
    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "formats", &container4);
    if (container4) {
        Invoke(INVOKE_CREATE_ARRAY, container4);
        for (int i = 0; i < pWB->formats.count; i++) {
            void *newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, container4, (INTEGER)i, &newpData);
            Invoke(INVOKE_CREATE_ARRAY, newpData);
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "formats", &container4);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "index", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pWB->formats.format[i].index);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "value", (INTEGER)VARIABLE_STRING, (char *)pWB->formats.format[i].value, (NUMBER)0);
        }
    }

END_IMPL
//------------------------------------------------------------------------

