#ifndef __TEMPLATEELEMENT_H
#define __TEMPLATEELEMENT_H

#include "AnsiString.h"
#include "AnsiList.h"

#define ADD_INFO_ERROR(error)                        error += "(at "; error += AnsiString((long)line); error += " "; error += filename; error += ")\n";

#define ADD_INFO_ERROR_2(error, _line, _filename)    error += (char *)"(at line "; error += AnsiString((long)_line); error += (char *)" "; error += _filename; error += ")\n";

enum ElementType {
    HTML,
    VARIABLE,
    IF,
    FOREACH
};

enum Modifiers {
    NONE,
    LENGTH,
    KEY,
    INDEX,
    DAY,
    MONTH,
    YEAR,
    HOUR,
    MINUTE,
    SECOND,
    DAYOFWEEK,
    DST,
    LIMIT
};

#define TPL_NO_COMPARE          0
#define TPL_COMPARE_STRING      1
#define TPL_COMPARE_NUMBER      2
#define TPL_COMPARE_CONSTANT    3

#define COMPARATOR_EQU          0 // equal
#define COMPARATOR_LES          1 // less
#define COMPARATOR_GRE          2 // greater
#define COMPARATOR_LEQ          3 // less or equal
#define COMPARATOR_GEQ          4 // great or equal
#define COMPARATOR_NEQ          5 // not equal

class CTemplateElement {
    friend class CTemplatizer;
private:
    ElementType    Type;
    AnsiList       *BlockTrue;
    AnsiList       *BlockFalse;
    AnsiString     HTML_DATA;
    int            VAR_INDEX;
    Modifiers      modifier;
    AnsiList       *members_chain;
    unsigned short line;
    AnsiString     filename;

    double     nCompareTo;
    AnsiString szCompareTo;

    signed char CompareType;
    signed char comparator;

public:
    inline CTemplateElement(ElementType type, unsigned short line, AnsiString filename = "") {
        Type           = type;
        members_chain  = 0;
        this->line     = line;
        this->filename = filename;
        if (type == IF) {
            BlockTrue  = new AnsiList();
            BlockFalse = new AnsiList();
        } else
        if (type == FOREACH) {
            BlockTrue  = new AnsiList();
            BlockFalse = 0;
        } else {
            BlockTrue  = 0;
            BlockFalse = 0;
        }
        VAR_INDEX = -1;
        modifier  = NONE;

        CompareType = TPL_NO_COMPARE;
        comparator  = COMPARATOR_EQU;
    }

    ~CTemplateElement() {
        if (BlockTrue)
            delete BlockTrue;
        if (BlockFalse)
            delete BlockFalse;
        if (members_chain)
            delete members_chain;
    }

    inline void SetVarIndex(int index) { VAR_INDEX = index; }
    inline int GetVarIndex()     { return VAR_INDEX;    }

    inline void SetHTML(AnsiString DATA) { HTML_DATA = DATA; }
    inline AnsiString GetHTML()  { return HTML_DATA;    }

    inline ElementType GetType() { return Type;         }
    inline AnsiList *GetTrue()   { return BlockTrue;    }
    inline AnsiList *GetFalse()  { return BlockFalse;   }
};
#endif //__TEMPLATEELEMENT_H
