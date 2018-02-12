#ifndef __CLASSMEMBER
#define __CLASSMEMBER

#include "AnsiList.h"
#include "ConceptTypes.h"
#include "Errors.h"
#include "AnsiException.h"
#include "ConceptPools.h"
#include "DoubleList.h"

#include <stdio.h>

#define ENOUGH_PARAMETERS(__CM, __FORMAL_PARAM)    ((__FORMAL_PARAM->COUNT >= __CM->MUST_PARAMETERS_COUNT) && (__FORMAL_PARAM->COUNT <= __CM->PARAMETERS_COUNT)) ? 1 : 0

class ClassCode;

class CompilerData {
public:
    DoubleList *PIF_DATA;
    DoubleList *VariableDescriptors;

    CompilerData() {
        PIF_DATA = new DoubleList();
        VariableDescriptors = new DoubleList();
    }

    ~CompilerData() {
        if (PIF_DATA)
            delete PIF_DATA;
        if (VariableDescriptors)
            delete VariableDescriptors;
    }
};

class ClassMember {
public:
    const char *NAME;
    void *Defined_In;
    SmallVariableDESCRIPTOR *VD;

    CompilerData *CDATA;

    void *OPTIMIZER;

    INTEGER MEMBER_GET;
    union {
        INTEGER           MEMBER_SET;
        short  PARAMETERS_COUNT;
    };
    short  MUST_PARAMETERS_COUNT;
    signed char       ACCESS;
    signed char       IS_OPERATOR;
    signed char       IS_FUNCTION;
    signed char       IS_STATIC;
    unsigned short    _DEBUG_STARTLINE;
// public:
    POOLED(ClassMember)

    ClassMember(void *DefinedIn, const char *name , char is_data_only, char _not_binary = 1, char is_unserialize = false);
    ~ClassMember(void);
#ifdef SIMPLE_MULTI_THREADING
    VariableDATA *Execute(void *PIF, intptr_t CONCEPT_CLASS_ID, VariableDATA *Owner, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, VariableDATA *& THROW_DATA, SCStack *PREV, char is_main = 0, INTEGER *thread_lock = NULL);
#else
    VariableDATA *Execute(void *PIF, intptr_t CONCEPT_CLASS_ID, VariableDATA *Owner, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, VariableDATA *& THROW_DATA, SCStack *PREV, char is_main = 0);
#endif
    void EnsureVD();

    int Serialize(void *PIF, FILE *out, bool is_lib = false, int version = 1);
    int Unserialize(concept_FILE *in, AnsiList *ClassList , bool is_lib, int *Relocation, int *ClassNames);
    static int ComputeSharedSize(concept_FILE *in, signed char& IS_FUNCTION);

    GreenThreadCycle *CreateThread(void *PIF, intptr_t CONCEPT_CLASS_ID, VariableDATA *Owner);
    static void DoneThread(GreenThreadCycle *gtc);
    bool FastOptimizedExecute(void *PIF, void *ref, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX);

#ifdef JIT_RUNTIME_CHECKS
    inline void *GetClass() { return Defined_In; }
    inline const char *GetName() { return NAME; }
#endif
    void EnsureThreadSafe();
    void EndMainCall(void *PIF, VariableDATA *&RESULT, VariableDATA *&THROW_DATA, SCStack *PREV, SCStack *STACK_TRACE);
};
#endif // __CLASSMEMBER
