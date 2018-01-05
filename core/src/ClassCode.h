#ifndef __CLASSCODE
#define __CLASSCODE

#include "AnsiString.h"
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

#define MAP_RELOC
#define THIS_CLASS    "this"

class PIFAlizator;
class CompiledClass;

typedef struct {
    // general member id
    INTEGER mid;
    // local member index
    CLASS_MEMBERS_DOMAIN lid;
} MemberLink;

class ClassCode {
    friend class PIFAlizator;
    friend class ConceptInterpreter;
    friend class CompiledClass;
    friend class Optimizer;
    friend class ClassMember;
    friend class Array;
    friend class GarbageCollector;
    friend INTEGER Invoke(INTEGER, ...);
    friend int GetVariableByName(int operation, void **VDESC, void **CONTEXT, int Depth, char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);

    friend INTEGER GetClassMember(void *CLASS_PTR, const char *class_member_name, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE);

    friend INTEGER GetClassMemberVariable(void *CLASS_PTR, const char *class_member_name, void **ptr);

    friend VariableDATA *GetClassMember(void *CLASS_PTR, const char *class_member_name);

    friend INTEGER SetClassMember(void *CLASS_PTR, const char *class_member_name, INTEGER TYPE, const char *STRING_VALUE, NUMBER NUMBER_VALUE);

    friend int MarkRecursive(void *PIF, CompiledClass *CC, signed char reach_id_flag, signed char forced_flag);

    friend int GetMemoryStatistics(void *PIF, void *RESULT);

private:
    TinyString NAME;
    AnsiList   *Members;
    TinyString _DEBUG_INFO_FILENAME;
    void       *first_parent;
    INTEGER     CONSTRUCTOR;
    ClassMember *CONSTRUCTOR_MEMBER;

    INTEGER     DESTRUCTOR;
    ClassMember *DESTRUCTOR_MEMBER;

    INTEGER     CLSID;

    STATIC_FLAG ParamList *EMPTY_PARAM_LIST;

    ClassMember          **pMEMBERS;
    CLASS_MEMBERS_DOMAIN pMEMBERS_COUNT;

    MemberLink *RELOCATIONS;

    CLASS_MEMBERS_DOMAIN *RELOCATIONS2;
    CLASS_MEMBERS_DOMAIN DataMembersCount;

    signed char NEEDED;
    signed char DEFINED_LEVEL;
    void FindRelocation(INTEGER mid, INTEGER &i, MemberLink *&result, INTEGER limit);
public:
    POOLED(ClassCode)

    int Inherits(INTEGER CLSID);
    int GetAbsoluteMemberID(int mid);
    int BoundMember(PIFAlizator *PIF, ClassMember *CM, ClassCode *Sender);
    int HasMember(const char *name);
    int HasMemberInCompiled(const char *name, INTEGER *is_variable = NULL);
    ClassMember *MemberID(int mid);
    int GetSerialMembers(CompiledClass *CC, int max_members, char **pmembers, unsigned char *flags, char *access, char *types, char **szValue, double *n_data, void **class_data, void **variable_data, int all_members = 1);
    int CanBeRunStatic(const char *name);

    ClassCode(const char *name, PIFAlizator *P, char binary_form = false);
    ClassMember *AddLinked(PIFAlizator *PIF, ClassMember *CM_i);
    ClassMember *AddMember(PIFAlizator *PIF, const char *name, INTEGER line, const char *FileName, INTEGER ACCESS, char data_only);
    int RemoveMember(PIFAlizator *PIF, const char *name, INTEGER line, const char *FileName);

    VariableDATA *ExecuteMember(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, const RuntimeElement *AE, INTEGER local, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, char property, INTEGER CLSID, INTEGER LOCAL_CLSID, SCStack *PREV, char next_is_asg = 0, VariableDATAPROPERTY **PROPERTIES = NULL, int dataLen = -1, int result_id = -1);
#ifdef SIMPLE_MULTI_THREADING
    VariableDATA *ExecuteDelegate(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, const RuntimeElement *AE, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, SCStack *PREV, INTEGER *thread_lock = NULL);
#else
    VariableDATA *ExecuteDelegate(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, const RuntimeElement *AE, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, SCStack *PREV);
#endif

    TinyString *GetFilename(PIFAlizator *PIF, INTEGER LOCAL_CLSID, TinyString *default_Value);
    void SetProperty(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner, const RuntimeElement *AE, INTEGER local, INTEGER VALUE, VariableDATA **SenderCTX, INTEGER CLSID, INTEGER LOCAL_CLSID, SCStack *PREV);
    CompiledClass *CreateInstance(PIFAlizator *PIF, VariableDATA *Owner, const RuntimeElement *AE, ParamList *FORMAL_PARAM, VariableDATA **SenderCTX, SCStack *PREV, char is_static = 0);
    void GenerateCode(StaticList *GeneralMembers);
    int Serialize(PIFAlizator *PIF, FILE *out, bool is_lib = false);
    int Unserialize(PIFAlizator *PIF, concept_FILE *in, AnsiList *ClassList, bool is_lib = false, int *ClassNames = 0, int *Relocation = 0);
    static int ComputeSharedSize(concept_FILE *in, int general_members);

    static void Hibernate(void *);
    void UpdateNeeded();

    GreenThreadCycle *CreateThread(PIFAlizator *PIF, INTEGER i, VariableDATA *Owner);
    void BeforeDestructor(PIFAlizator *PIF);
    INTEGER Relocation(INTEGER mid);
    void SetRelocation(INTEGER mid, INTEGER index);
    ~ClassCode();

#ifdef JIT_RUNTIME_CHECKS
    inline const char *GetName() { return NAME.c_str(); }
    inline INTEGER GetCLSID() { return CLSID; }
#endif
    void EnsureThreadSafe();
};
#endif // __CLASSCODE

