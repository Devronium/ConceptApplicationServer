#ifndef __COMPILEDCLASS_H
#define __COMPILEDCLASS_H

#include "ClassCode.h"
#include "ModuleLink.h"
#include "Array.h"
#include "ConceptPools.h"

#define GET_PIF(THIS_REF)    ((PIFAlizator *)((ClassPool *)(((uintptr_t)THIS_REF) - sizeof(struct CompiledClass) * THIS_REF->flags - POOL_OFFSET(ClassPool, POOL)))->PIF)

struct CompiledClass {
    VariableDATA **_CONTEXT;
    const ClassCode *_Class;
    INTEGER LINKS;
    signed char flags;
    signed char reachable;
};

void CompiledClass__GO_GARBAGE(struct CompiledClass *self, void *PIF, GarbageCollector *__gc_obj, GarbageCollector *__gc_array, GarbageCollector *__gc_vars, signed char check_objects = -1);
const ClassCode *CompiledClass_GetClass(const struct CompiledClass *self);
VariableDATA **CompiledClass_GetContext(const struct CompiledClass *self);
VariableDATA *CompiledClass_CreateVariable(struct CompiledClass *self, INTEGER reloc, ClassMember *CM, VariableDATA *CONTAINER = NULL);
const char *CompiledClass_GetClassName(const struct CompiledClass *self);
struct CompiledClass *new_CompiledClass(void *PIF, const ClassCode *CC);
int CompiledClass_Destroy(struct CompiledClass *self, PIFAlizator *PIF, SCStack *STACK_TRACE = NULL);
int CompiledClass_HasDestructor(const struct CompiledClass *self);
void CompiledClass_UnlinkObjects(struct CompiledClass *self);
void delete_CompiledClass(struct CompiledClass *self, SCStack *STACK_TRACE = NULL);

typedef struct {
    CompiledClass POOL[OBJECT_POOL_BLOCK_SIZE];
    void          *NEXT;
    void          *PREV;
    void          *PIF;
    unsigned char POOL_VARS;
    unsigned char FIRST_VAR;
} ClassPool;
#endif // __COMPILEDCLASS_H

