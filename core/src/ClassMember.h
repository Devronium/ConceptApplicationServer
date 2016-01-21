#ifndef __CLASSMEMBER
#define __CLASSMEMBER

#include "AnsiString.h"
#include "AnsiList.h"
#include "ConceptTypes.h"
#include "Errors.h"
#include "AnsiException.h"
#include "ConceptPools.h"
#include "DoubleList.h"

#include <stdio.h>

#define ENOUGH_PARAMETERS(__CM, __FORMAL_PARAM)    ((__FORMAL_PARAM->COUNT >= __CM->MUST_PARAMETERS_COUNT) && (__FORMAL_PARAM->COUNT <= __CM->PARAMETERS_COUNT)) ? 1 : 0

class ClassCode;
class ClassMember {
    friend class ClassCode;
    friend class Optimizer;
    friend class PIFAlizator;
    friend class CompiledClass;
    friend class ConceptInterpreter;
    friend class Array;
    friend INTEGER Invoke(INTEGER, ...);
    friend int GetVariableByName(int operation, void **VDESC, void **CONTEXT, int Depth, char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);

    friend INTEGER GetClassMember(void *CLASS_PTR, char *class_member_name, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE);

    friend VariableDATA *GetClassMember(void *CLASS_PTR, char *class_member_name);

    friend INTEGER SetClassMember(void *CLASS_PTR, char *class_member_name, INTEGER TYPE, char *STRING_VALUE, NUMBER NUMBER_VALUE);

private:
    char *NAME;
    void *Defined_In;
    SmallVariableDESCRIPTOR *VD;

    DoubleList *PIF_DATA;
    DoubleList *VariableDescriptors;

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
public:
    POOLED(ClassMember)

    ClassMember(void *DefinedIn, char *name , char is_data_only, char _not_binary = 1, char is_unserialize = false);
    ~ClassMember(void);
    VariableDATA *Execute(void *PIF, intptr_t CONCEPT_CLASS_ID, VariableDATA *Owner, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, VariableDATA *& THROW_DATA, SCStack *PREV, char is_main = 0);
    void EnsureVD();

    int Serialize(FILE *out, bool is_lib = false);
    int Unserialize(concept_FILE *in, AnsiList *ClassList , bool is_lib, int *Relocation, int *ClassNames);
    static int ComputeSharedSize(concept_FILE *in, signed char& IS_FUNCTION);

    GreenThreadCycle *CreateThread(void *PIF, intptr_t CONCEPT_CLASS_ID, VariableDATA *Owner);
    static void DoneThread(GreenThreadCycle *gtc);
    bool FastOptimizedExecute(void *PIF, void *ref, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX);

#ifdef JIT_RUNTIME_CHECKS
    inline void *GetClass() { return Defined_In; }
    inline const char *GetName() { return NAME; }
#endif
};
#endif // __CLASSMEMBER

