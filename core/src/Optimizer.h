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

class OptimizerHelper {
public:
    PIFAlizator *PIFOwner;
    ClassCode *_CLASS;
    const char*_MEMBER;
    INTEGER   LAST_DEBUG_TRAP;
    DoubleList *PIFList;
    DoubleList *VDList;
    INTEGER    PIF_POSITION;
    const char *_DEBUG_INFO_FILENAME;
    char NO_WARNING_EMPTY;
    char NO_WARNING_ATTR;
    AnsiList *OptimizedPIF;
    AnsiList *ParameterList;
    AnsiList *CONTINUE_Elements;
    AnsiList *BREAK_Elements;
    char _clean_condition;
    INTEGER CATCH_ELEMENT;

    OptimizerHelper() {
        PIFOwner = 0;
        _CLASS = 0;
        _MEMBER = 0;
        LAST_DEBUG_TRAP = 0;
        PIFList = 0;
        VDList = 0;
        PIF_POSITION = 0;
        _DEBUG_INFO_FILENAME = 0;
        NO_WARNING_EMPTY = 0;
        NO_WARNING_ATTR = 0;
        OptimizedPIF = 0;
        ParameterList = 0;
        CONTINUE_Elements = 0;
        BREAK_Elements = 0;
        _clean_condition = 0;
        CATCH_ELEMENT = 0;
    }

    ~OptimizerHelper() {
        if (ParameterList)
            delete ParameterList;
        if (OptimizedPIF)
            delete OptimizedPIF;
    }
};

class Optimizer {
    friend class ConceptInterpreter;
    friend class ClassMember;
    friend class PIFAlizator;
    friend INTEGER Invoke(INTEGER INVOKE_TYPE, ...);
public:
    static PIFAlizator *PIFOwner;
    void *INTERPRETER;
    RuntimeOptimizedElement *CODE;
    INTEGER codeCount;
    RuntimeVariableDESCRIPTOR *DATA;
    INTEGER              dataCount;
    ParamList            *PARAMS;
    INTEGER              paramCount;

    INTEGER OptimizeSwitch(OptimizerHelper *helper, TempVariableManager *TVM);
    INTEGER OptimizeArray(OptimizerHelper *helper, TempVariableManager *TVM);
    INTEGER OptimizeForExpression(OptimizerHelper *helper, TempVariableManager *TVM, bool is_increment = false);
    INTEGER OptimizeExpression(OptimizerHelper *helper, TempVariableManager *TVM, INTEGER ID, INTEGER TYPE, INTEGER IS_PARAM_LIST = 0, INTEGER FIRST_CALL = 1, INTEGER FLAGS = 0, AnalizerElement *PREC_METHOD = 0, signed char may_skip_result = 0);
    INTEGER OptimizeKeyWord(OptimizerHelper *helper, TempVariableManager *TVM, INTEGER ID, INTEGER TYPE);
    INTEGER OptimizeAny(OptimizerHelper *helper, TempVariableManager *TVM, INTEGER ID = KEY_SEP, INTEGER TYPE = TYPE_SEPARATOR, char FLAGS = 0);

    INTEGER GetFirstOperator(DoubleList *LST, INTEGER start, INTEGER end);
    INTEGER Check(OptimizerHelper *helper, AnalizerElement *Target, AnalizerElement *AE);

    INTEGER Require(OptimizerHelper *helper, AnalizerElement *Element);

    void CopyElement(AnalizerElement *SRC, AnalizerElement *DEST);
    void CopyElement(AnalizerElement *SRC, SHORT_AnalizerElement *DEST);
    void CopyElement(AnalizerElement *SRC, PartialAnalizerElement *DEST);
    void BuildParameterList(OptimizerHelper *helper, INTEGER START_POS, AnalizerElement *METHOD);
    bool TryCheckParameters(OptimizerHelper *helper, OptimizedElement *OE, int p_count, int *minp, int *maxp, ClassMember **target_CM = 0, ClassCode *owner_CC = 0);
    void AddProfilerCode(OptimizerHelper *helper, int code);

public:
    static OptimizerHelper *GetHelper(PIFAlizator *P);
    POOLED(Optimizer)
    Optimizer(PIFAlizator *P, DoubleList *_PIFList, DoubleList *_VDList, const char *Filename, ClassCode *cls, const char *member, bool is_unserialized = false);
    int Optimize(PIFAlizator *P);
    int CanInline(ClassMember *owner, const char **remotename);
#ifdef PRINT_DEBUG_INFO
    AnsiString DEBUG_INFO();
#endif

    ~Optimizer();
    int Serialize(PIFAlizator *PIFOwner, FILE *out, bool is_lib = false, int version = 1);
    int Unserialize(PIFAlizator *PIFOwner, concept_FILE *in, AnsiList *ModuleList, bool is_lib = false, int *ClassNames = 0, int *Relocation = 0, int version = 1);
    static int ComputeSharedSize(concept_FILE *in, int version = 1);
    void GenerateIntermediateCode(PIFAlizator *P);
};
#endif // __OPTIMIZER_H

