#ifndef __ANSIPARSER_H
#define __ANSIPARSER_H

#include "AnsiString.h"
#include "Codes.h"
#include "AnsiList.h"
#include "ConceptTypes.h"
#include "ConceptPools.h"

#define MAX_STR_VAR    1024

class AnsiParser {
private:
    AnsiString *Target;
    ConstantMapType *ConstantList;
    intptr_t   position;
    intptr_t   rollback_position;
    intptr_t   rollback_line;
    intptr_t   linia_curenta;

    AnsiString virtual_result;
    char       has_virtual_result;

    char put_prefix;
    char is_end;
    char put_a_quote;
    char in_formatted;
    char in_expr;
public:
    char regexp_flags;
    POOLED(AnsiParser)

    AnsiParser(AnsiString *TRG, ConstantMapType *Constants);
    ~AnsiParser(void);

    void Atomize();
    intptr_t LastLine();

    void NextAtom(AnsiString& result, int no_constants = 0, int TYPE = -1, int ID = -1);
    AnsiString GetConstant();
    bool Rollback();
    bool Done();
};
#endif

