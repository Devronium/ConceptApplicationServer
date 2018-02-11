#ifndef __CLASSCODE
#define __CLASSCODE

#include "AnsiList.h"
#include "StaticList.h"
#include "ClassMember.h"
#include "PIFAlizator.h"
#include "CompiledClass.h"
#include "Optimizer.h"
#include "Array.h"
#include "GarbageCollector.h"
#include "ConceptPools.h"
#include <stdio.h>

#define THIS_CLASS    "this"

class PIFAlizator;
struct CompiledClass;

typedef struct {
    // general member id
    INTEGER mid;
    // local member index
    CLASS_MEMBERS_DOMAIN lid;
} MemberLink;

#ifdef HASH_RELOCATION
#include "khash.h"

KHASH_MAP_INIT_INT(inthashtable, INTEGER);
#endif

class ClassCode {
public:
    TinyString NAME;
    AnsiList   *Members;
    TinyString _DEBUG_INFO_FILENAME;
    void  *first_parent;
    INTEGER     CONSTRUCTOR;
    ClassMember *CONSTRUCTOR_MEMBER;

    INTEGER     DESTRUCTOR;
    ClassMember *DESTRUCTOR_MEMBER;

    INTEGER     CLSID;

    STATIC_FLAG ParamList *EMPTY_PARAM_LIST;

    ClassMember          **pMEMBERS;
    CLASS_MEMBERS_DOMAIN pMEMBERS_COUNT;
#ifdef HASH_RELOCATION
    khash_t (inthashtable) *RELOCATIONS;
#else
    MemberLink *RELOCATIONS;
#endif

    CLASS_MEMBERS_DOMAIN *RELOCATIONS2;
    CLASS_MEMBERS_DOMAIN DataMembersCount;

    signed char NEEDED;
    signed char DEFINED_LEVEL;
#ifndef HASH_RELOCATION
    void FindRelocation(INTEGER mid, INTEGER &i, MemberLink *&result, INTEGER limit);
#endif
// public:
    POOLED(ClassCode)
    void BuildParameterError(PIFAlizator *PIF, int line, int FORMAL_PARAM_COUNT, const ClassMember *pMEMBER_i, SCStack *PREV, int ack_error = 1) const;

    int Inherits(INTEGER CLSID) const;
    int GetAbsoluteMemberID(int mid) const;
    int BoundMember(PIFAlizator *PIF, ClassMember *CM, ClassCode *Sender);
    int HasMember(const char *name) const;
    int HasMemberInCompiled(const char *name, INTEGER *is_variable = NULL) const;
    ClassMember *MemberID(int mid);
    int GetSerialMembers(CompiledClass *CC, int max_members, char **pmembers, unsigned char *flags, char *access, char *types, char **szValue, double *n_data, void **class_data, void **variable_data, int all_members = 1) const;
    int CanBeRunStatic(const char *name, ClassMember **member = NULL);

    ClassCode(const char *name, PIFAlizator *P, char binary_form = false);
    ClassMember *AddLinked(PIFAlizator *PIF, ClassMember *CM_i);
    ClassMember *AddMember(PIFAlizator *PIF, const char *name, INTEGER line, const char *FileName, INTEGER ACCESS, char data_only);
    int RemoveMember(PIFAlizator *PIF, const char *name, INTEGER line, const char *FileName);

    VariableDATA *ExecuteMember(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, const RuntimeOptimizedElement *OE, INTEGER local, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, char property, INTEGER CLSID, INTEGER LOCAL_CLSID, VariableDATA **LOCAL_THROW, SCStack *PREV, char next_is_asg = 0, VariableDATAPROPERTY **PROPERTIES = NULL, int dataLen = -1, int result_id = -1) const;
#ifdef SIMPLE_MULTI_THREADING
    VariableDATA *ExecuteDelegate(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, const RuntimeOptimizedElement *OE, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, VariableDATA **LOCAL_THROW, SCStack *PREV, INTEGER *thread_lock = NULL) const;
#else
    VariableDATA *ExecuteDelegate(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, const RuntimeOptimizedElement *OE, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, VariableDATA **LOCAL_THROW, SCStack *PREV) const;
#endif

    const TinyString *GetFilename(PIFAlizator *PIF, INTEGER LOCAL_CLSID, const TinyString *default_Value) const;
    void SetProperty(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, const RuntimeOptimizedElement *OE, INTEGER local, INTEGER VALUE, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, VariableDATA **LOCAL_THROW, SCStack *PREV) const;
    CompiledClass *CreateInstance(PIFAlizator *PIF, VariableDATA *Owner, const RuntimeOptimizedElement *OE, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, SCStack *PREV, char is_static = 0) const;
    void GenerateCode(StaticList *GeneralMembers);
    int Serialize(PIFAlizator *PIF, FILE *out, bool is_lib = false, int version = 1);
    int Unserialize(PIFAlizator *PIF, concept_FILE *in, AnsiList *ClassList, bool is_lib = false, int *ClassNames = 0, int *Relocation = 0, int version = 1);
    static int ComputeSharedSize(concept_FILE *in, int general_members, int version = 1);

    static void Hibernate(void *);
    void UpdateNeeded();

    GreenThreadCycle *CreateThread(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner) const;
    void BeforeDestructor(PIFAlizator *PIF);
    INTEGER Relocation(INTEGER mid) const;
    void SetRelocation(INTEGER mid, INTEGER index);
    ~ClassCode();

#ifdef JIT_RUNTIME_CHECKS
    inline const char *GetName() const { return NAME.c_str(); }
    inline INTEGER GetCLSID() const { return CLSID; }
#endif
    void EnsureThreadSafe();
};
#endif // __CLASSCODE

