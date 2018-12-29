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

struct OptimizerHelper {
    PIFAlizator *PIFOwner;
    ClassCode *_CLASS;
    DoubleList *PIFList;
    DoubleList *VDList;
    AnsiList *OptimizedPIF;
    AnsiList *ParameterList;
    AnsiList *CONTINUE_Elements;
    AnsiList *BREAK_Elements;
    const char *_DEBUG_INFO_FILENAME;
    const char *_MEMBER;
    INTEGER LAST_DEBUG_TRAP;
    INTEGER PIF_POSITION;
    INTEGER CATCH_ELEMENT;
    INTEGER LOCAL_VARIABLES;
    INTEGER JUNK;

    char NO_WARNING_EMPTY;
    char NO_WARNING_ATTR;
    char _clean_condition;
    char has_references;
    char has_loops;
};


void delete_OptimizerHelper(struct OptimizerHelper *self);
struct OptimizerHelper *new_OptimizerHelper();

struct Optimizer {
    void *INTERPRETER;
    RuntimeOptimizedElement *CODE;
    INTEGER codeCount;
    RuntimeVariableDESCRIPTOR *DATA;
    INTEGER              dataCount;
    ParamList            *PARAMS;
    INTEGER              paramCount;
};

INTEGER Optimizer_OptimizeSwitch(struct Optimizer *self, struct OptimizerHelper *helper, TempVariableManager *TVM);
INTEGER Optimizer_OptimizeArray(struct Optimizer *self, struct OptimizerHelper *helper, TempVariableManager *TVM);
INTEGER Optimizer_OptimizeForExpression(struct Optimizer *self, struct OptimizerHelper *helper, TempVariableManager *TVM, bool is_increment = false);
INTEGER Optimizer_OptimizeExpression(struct Optimizer *self, struct OptimizerHelper *helper, TempVariableManager *TVM, INTEGER ID, INTEGER TYPE, INTEGER IS_PARAM_LIST = 0, INTEGER FIRST_CALL = 1, INTEGER FLAGS = 0, AnalizerElement *PREC_METHOD = 0, signed char may_skip_result = 0);
INTEGER Optimizer_OptimizeKeyWord(struct Optimizer *self, struct OptimizerHelper *helper, TempVariableManager *TVM, INTEGER ID, INTEGER TYPE);
INTEGER Optimizer_OptimizeAny(struct Optimizer *self, struct OptimizerHelper *helper, TempVariableManager *TVM, INTEGER ID = KEY_SEP, INTEGER TYPE = TYPE_SEPARATOR, char FLAGS = 0);
INTEGER Optimizer_GetFirstOperator(struct Optimizer *self, DoubleList *LST, INTEGER start, INTEGER end);
INTEGER Optimizer_Check(struct Optimizer *self, struct OptimizerHelper *helper, AnalizerElement *Target, AnalizerElement *AE);
INTEGER Optimizer_Require(struct Optimizer *self, struct OptimizerHelper *helper, AnalizerElement *Element);

void Optimizer_BuildParameterList(struct Optimizer *self, struct OptimizerHelper *helper, INTEGER START_POS, AnalizerElement *METHOD);
bool Optimizer_TryCheckParameters(struct OptimizerHelper *helper, OptimizedElement *OE, int p_count, int *minp, int *maxp, ClassMember **target_CM = 0, ClassCode *owner_CC = 0);
void Optimizer_AddProfilerCode(struct OptimizerHelper *helper, int code);
void Optimizer_RemoveCode(struct OptimizerHelper *helper, INTEGER index);
void Optimizer_OptimizePass2(struct OptimizerHelper *helper);
OptimizerHelper *Optimizer_GetHelper(PIFAlizator *P);
struct Optimizer *new_Optimizer(PIFAlizator *P, DoubleList *_PIFList, DoubleList *_VDList, const char *Filename, ClassCode *cls, const char *member, bool is_unserialized = false);
int Optimizer_Optimize(struct Optimizer *self, PIFAlizator *P);
int Optimizer_CanInline(struct Optimizer *self, ClassMember *owner, const char **remotename);
#ifdef PRINT_DEBUG_INFO
    AnsiString Optimizer_DEBUG_INFO(struct Optimizer *self);
#endif

void delete_Optimizer(struct Optimizer *self);
int Optimizer_Serialize(struct Optimizer *self, PIFAlizator *PIFOwner, FILE *out, bool is_lib = false, int version = 2);
int Optimizer_Unserialize(struct Optimizer *self, PIFAlizator *PIFOwner, concept_FILE *in, AnsiList *ModuleList, bool is_lib = false, int *ClassNames = 0, int *Relocation = 0, int version = 1);
int Optimizer_ComputeSharedSize(concept_FILE *in, int version = 1);
void Optimizer_GenerateIntermediateCode(struct Optimizer *self, PIFAlizator *P);

#endif // __OPTIMIZER_H

