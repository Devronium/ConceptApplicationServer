#ifndef __COMPILEDCLASS_H
#define __COMPILEDCLASS_H

#include "ClassCode.h"
#include "ModuleLink.h"
#include "Array.h"
#include "ConceptPools.h"

#define GET_PIF(THIS_REF)    ((PIFAlizator *)((ClassPool *)(((uintptr_t)THIS_REF) - sizeof(CompiledClass) * THIS_REF->flags - POOL_OFFSET(ClassPool, POOL)))->PIF)

class CompiledClass {
    friend class ConceptInterpreter;
    friend class ClassCode;
    friend class ClassMember;
    friend class Array;
    friend INTEGER Invoke(INTEGER, ...);
    friend int GetVariableByName(int operation, void **VDESC, void **CONTEXT, int Depth, char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);

    friend int MarkRecursive(void *PIF, CompiledClass *CC, signed char reach_id_flag);

private:
    ClassCode    *_Class;
    VariableDATA **_CONTEXT;
public:
    POOLED(CompiledClass)
    void __GO_GARBAGE(void *PIF, GarbageCollector *__gc_obj, GarbageCollector *__gc_array, GarbageCollector *__gc_vars, signed char check_objects = -1);

    INTEGER LINKS;

    ClassCode *GetClass();
    VariableDATA **GetContext();
    VariableDATA *CreateVariable(INTEGER reloc, ClassMember *CM, VariableDATA *CONTAINER = NULL);

    AnsiString GetClassName();

    CompiledClass(ClassCode *CC);
    int Destroy(PIFAlizator *PIF);
    int HasDestructor();
    void UnlinkObjects();

    ~CompiledClass();

    signed char flags;
    signed char reachable;
    inline void operator delete(void *obj) {
        FreeClassObject(obj);
    }
};

typedef struct {
    CompiledClass POOL[OBJECT_POOL_BLOCK_SIZE];
    void          *NEXT;
    void          *PREV;
    void          *PIF;
    unsigned char POOL_VARS;
    unsigned char FIRST_VAR;
} ClassPool;
#endif // __COMPILEDCLASS_H

