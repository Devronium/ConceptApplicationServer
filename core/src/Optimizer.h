#ifndef __OPTIMIZER_H
#define __OPTIMIZER_H

#include "PIFAlizator.h"
#include "ConceptTypes.h"
#include "Codes.h"
#include <stdio.h>
#include "ConceptPools.h"
#include "DoubleList.h"

#define DELTA_REF(base, p)      ((INTEGER *)(((SYS_INT)p) - ((SYS_INT)base)))
#define DELTA_UNREF(base, p)    ((INTEGER *)(p ? ((SYS_INT)base + (SYS_INT)p) : 0))

class TempVariableManager;

class Optimizer {
    friend class ConceptInterpreter;
    friend class ClassMember;
    friend class PIFAlizator;
    friend INTEGER Invoke(INTEGER INVOKE_TYPE, ...);

private:
    STATIC_FLAG ClassCode *_CLASS;
    STATIC_FLAG char      *_MEMBER;
    STATIC_FLAG INTEGER   LAST_DEBUG_TRAP;

    STATIC_FLAG DoubleList *PIFList;
    STATIC_FLAG DoubleList *VDList;
    STATIC_FLAG INTEGER    PIF_POSITION;
    STATIC_FLAG char       *_DEBUG_INFO_FILENAME;

    STATIC_FLAG char NO_WARNING_EMPTY;
    STATIC_FLAG char NO_WARNING_ATTR;

    STATIC_FLAG AnsiList *OptimizedPIF;
    STATIC_FLAG AnsiList *ParameterList;

    void *INTERPRETER;
    RuntimeOptimizedElement *CODE;
    INTEGER codeCount;
    RuntimeVariableDESCRIPTOR *DATA;
    INTEGER              dataCount;
    ParamList            *PARAMS;
    INTEGER              paramCount;
    STATIC_FLAG AnsiList *CONTINUE_Elements;
    STATIC_FLAG AnsiList *BREAK_Elements;

    STATIC_FLAG char _clean_condition;

    STATIC_FLAG INTEGER CATCH_ELEMENT;

    INTEGER OptimizeSwitch(TempVariableManager *TVM);
    INTEGER OptimizeArray(TempVariableManager *TVM);
    INTEGER OptimizeForExpression(TempVariableManager *TVM, bool is_increment = false);
    INTEGER OptimizeExpression(TempVariableManager *TVM, INTEGER ID, INTEGER TYPE, INTEGER IS_PARAM_LIST = 0, INTEGER FIRST_CALL = 1, INTEGER FLAGS = 0, AnalizerElement *PREC_METHOD = 0);
    INTEGER OptimizeKeyWord(TempVariableManager *TVM, INTEGER ID, INTEGER TYPE);
    INTEGER OptimizeAny(TempVariableManager *TVM, INTEGER ID = KEY_SEP, INTEGER TYPE = TYPE_SEPARATOR, char FLAGS = 0);

    INTEGER GetFirstOperator(DoubleList *LST, INTEGER start, INTEGER end);
    INTEGER Check(AnalizerElement *Target, AnalizerElement *AE);

    INTEGER Require(AnalizerElement *Element);

    void CopyElement(AnalizerElement *SRC, AnalizerElement *DEST);
    void CopyElement(AnalizerElement *SRC, SHORT_AnalizerElement *DEST);
    void CopyElement(AnalizerElement *SRC, PartialAnalizerElement *DEST);
    void BuildParameterList(INTEGER START_POS, AnalizerElement *METHOD);
    bool TryCheckParameters(OptimizedElement *OE, int p_count, int *minp, int *maxp, ClassMember **target_CM = 0, ClassCode *owner_CC = 0);
    void AddProfilerCode(int code);

public:
    STATIC_FLAG PIFAlizator *PIFOwner;

    POOLED(Optimizer)
    Optimizer(PIFAlizator *P, DoubleList *_PIFList, DoubleList *_VDList, char *Filename, ClassCode *cls, char *member, bool is_unserialized = false);
    int Optimize();
    int OptimizeMemoryUsage();
    AnsiString DEBUG_INFO();

    ~Optimizer();
    int Serialize(FILE *out, bool is_lib = false);
    int Unserialize(concept_FILE *in, AnsiList *ModuleList, bool is_lib = false, int *ClassNames = 0, int *Relocation = 0);
    static int ComputeSharedSize(concept_FILE *in);
    void GenerateIntermediateCode();
};
#endif // __OPTIMIZER_H

