#ifndef __PIFALIZATOR_H
#define __PIFALIZATOR_H

#include "AnsiString.h"
#include "AnsiList.h"
#include "Codes.h"
#include "AnsiParser.h"
#include "ClassCode.h"
#include "Debugger.h"
#include "ConceptPools.h"
#include "TinyString.h"
#include "StaticList.h"
#include "GarbageCollector.h"
#include "Array.h"
#include "ClassCode.h"

#include "semhh.h"

#ifdef CACHED_LIST
    #include "HashTable.h"
#endif

#ifdef SIMPLE_MULTI_THREADING
    class Semaphore {
        private:
#ifdef USE_SPINLOCKS
            LONG volatile sem;
#else
            HHSEM sem;
#endif
        public:
            char MasterLock;
            int held_at;
            int threadid;

            Semaphore() {
#ifdef USE_SPINLOCKS
                sem = 0;
#else
                seminit(sem, 1);
#endif
                held_at = 0;
                threadid = 0;
                MasterLock = 0;
            }

            void Lock(int line_no) {
#ifdef _WIN32
                int new_threadid = GetCurrentThreadId();
#else
                int new_threadid = pthread_self();
#endif
                if ((MasterLock) && (new_threadid == threadid))
                    return;
#ifdef USE_SPINLOCKS
                do {
                    InterlockedCompareExchange(&sem, new_threadid, 0);
                } while (sem != new_threadid);
#else
                semp(sem);
#endif
                held_at = line_no;
                threadid = new_threadid;

                MasterLock = 1;
            }

            void Unlock(int line_no) {
                if (MasterLock) {
#ifdef _WIN32
                    int new_threadid = GetCurrentThreadId();
#else
                    int new_threadid = pthread_self();
#endif
                    // wrong thread ?
                    if (new_threadid != threadid)
                        return;

                    MasterLock = 0;
                    held_at = 0;
#ifdef USE_SPINLOCKS
                    sem = 0;
#else
                    semv(sem);
#endif
                }
            }

#ifndef USE_SPINLOCKS
            ~Semaphore() {
                semdel(sem);
            }
#endif
    };

 //#define CC_WRITE_LOCK(PIF)       bool IsWriteLocked = false; if (PIF->ThreadsCount > 0) { PIF->MasterLock = 1; semp(PIF->WriteLock); IsWriteLocked = true; }
 //#define CC_WRITE_LOCK2(PIF)      IsWriteLocked      = false; if (PIF->ThreadsCount > 0) { PIF->MasterLock = 1; semp(PIF->WriteLock); IsWriteLocked = true; }
 //#define CC_WRITE_UNLOCK(PIF)     if (IsWriteLocked) { semv(PIF->WriteLock); PIF->MasterLock = 0; IsWriteLocked = false; }
 
 /*#define WRITE_LOCK      if ((!IsWriteLocked) && (PIF->ThreadsCount > 0)) { semp(PIF->WriteLock); PIF->MasterLock = 1; IsWriteLocked = 1; }
 #define WRITE_UNLOCK    if (IsWriteLocked) { IsWriteLocked = 0; PIF->MasterLock = 0; semv(PIF->WriteLock); }*/
 #define NEW_THREAD      semp(PIF->InternalLock); PIF->ThreadsCount++; semv(PIF->InternalLock);
 #define DONE_THREAD     semp(PIF->InternalLock); PIF->ThreadsCount--; semv(PIF->InternalLock);
 #define INTERNAL_LOCK(PIF)      semp(PIF->InternalLock);
 #define INTERNAL_UNLOCK(PIF)    semv(PIF->InternalLock);

#define CC_WRITE_LOCK(PIF)      PIF->WriteLock.Lock(__LINE__);
#define CC_WRITE_LOCK2(PIF)     PIF->WriteLock.Lock(__LINE__);
#define CC_WRITE_UNLOCK(PIF)    PIF->WriteLock.Unlock(__LINE__);

#define WRITE_LOCK              if ((!IsWriteLocked) && (PIF->ThreadsCount > 0)) { PIF->WriteLock.Lock(__LINE__); IsWriteLocked = 1; }
#define WRITE_UNLOCK            if (IsWriteLocked) { PIF->WriteLock.Unlock(__LINE__); IsWriteLocked = 0; }

#define LOCK_IF1(var)               if (var->LINKS > 1) { WRITE_LOCK }
#define LOCK_IF2(var1, var2)        if ((var1->LINKS > 1) || (var2->LINKS > 1)) { WRITE_LOCK }
#define LOCK_IF3(var1, var2, var3)  if ((var1->LINKS > 1) || (var2->LINKS > 1) || (var3->LINKS > 1)) { WRITE_LOCK }

 #define ALLOC_LOCK      semp(((PIFAlizator *)PIF)->AllocLock);
 #define ALLOC_UNLOCK    semv(((PIFAlizator *)PIF)->AllocLock);

 #define THREAD_CREATION_LOCKS      , INTEGER *thread_lock

 #define SMART_LOCK(VARIABLE)                                                                                                                        \
    if (VARIABLE->LINKS > 1) {                                                                                                                       \
        WRITE_LOCK                                                                                                                                   \
    } else                                                                                                                                           \
    if (VARIABLE->CLASS_DATA) {                                                                                                                      \
        if (((VARIABLE->TYPE == VARIABLE_CLASS) || (VARIABLE->TYPE == VARIABLE_DELEGATE)) && (((struct CompiledClass *)VARIABLE->CLASS_DATA)->LINKS > 1)) { \
            WRITE_LOCK                                                                                                                               \
        } else                                                                                                                                       \
        if ((VARIABLE->TYPE == VARIABLE_ARRAY) && (((struct Array *)VARIABLE->CLASS_DATA)->LINKS > 1)) {                                                    \
            WRITE_LOCK                                                                                                                               \
        }                                                                                                                                            \
    }
#else
 #define CC_WRITE_LOCK(PIF)
 #define CC_WRITE_UNLOCK(PIF)
 #define CC_WRITE_LOCK2(PIF)

 #define WRITE_LOCK
 #define WRITE_UNLOCK
 #define NEW_THREAD
 #define DONE_THREAD
 #define INTERNAL_LOCK(PIF)
 #define INTERNAL_UNLOCK(PIF)
 #define ALLOC_LOCK
 #define ALLOC_UNLOCK
 #define THREAD_CREATION_LOCKS
 #define THREAD_CREATION_LOCKS_DEF

 #define SMART_LOCK(VARIABLE)
#endif

//#define CACHED_CLASSES
#define CACHED_VARIABLES
#define DEBUGGER_VAR_NAMES

#ifdef CACHED_CLASSES
 #include <map>
#endif

#ifdef CACHED_VARIABLES
 #include "HashTable.h"
#endif

#ifdef DEBUGGER_VAR_NAMES
 #include "HashTable.h"
#endif

#define PDATA_ITEMS               0xFF

#define PRAGMA_WARNINGS           "warnings"
#define PRAGMA_EXCEPTIONS         "lib_exceptions"
#define PRAGMA_IMPLICIT           "implicit"
#define PRAGMA_ENTRY_POINT        "entry_point"
#define PRAGMA_USED               "used"
#define PRAGMA_STRICT             "strict"

#define PRAGMA_ON                 "on"
#define PRAGMA_OFF                "off"

#define MAIN_ENTRY_POINT          "Main"

#define DEFAULT_USE_WARNINGS      1
#define DEFAULT_USE_EXCEPTIONS    0
#define DEFAULT_USE_IMPLICIT      0
#define DEFAULT_STRICT_MODE       1

#define DEFAULT_BIN_EXTENSION     ".accel"
#define DEFAULT_PACK_EXTENSION    ".package"

#define DIRTY_LIMIT               350000
#define STATIC_LINKS_INCREMENT    0x20
#define INSPECT_INCREMENT         0x200
#define INITIAL_INSPECT_SIZE      0x10
//#define USE_RECURSIVE_MARKINGS

typedef struct tsProtoData {
    void               *data;
    DESTROY_PROTO_DATA destroy_func;
} ProtoData;

typedef struct {
    VariableDATA  POOL[POOL_BLOCK_SIZE];
    void          *NEXT;
    void          *PREV;
    void          *PIF;
    unsigned char POOL_VARS;
    unsigned char FIRST_VAR;
} VARPool;

class PIFAlizator {
    friend class Optimizer;
    friend class ClassCode;
    friend class ClassMember;
    friend class ConceptInterpreter;
    friend class GarbageCollector;
    friend void DeturnatedPrint(void *PIF, const char *text, int len, void *userdata);

    friend void *AllocVAR(void *PIF);

    friend void AllocMultipleVars(void **context, void *PIF, int count, int offset);

    friend void *AllocClassObject(void *PIF);

    friend void *AllocArray(void *PIF, bool skip_top);

    friend int CheckReachability(void *PIF, bool skip_top);

    friend int ClearVariablesByCLSID(void *PIF, int CLSID);

    friend INTEGER Invoke(INTEGER INVOKE_TYPE, ...);

    friend int GetVariableByName(int operation, void **VDESC, void **CONTEXT, int Depth, char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);

    friend int GetMemoryStatistics(void *PIF, void *RESULT);

    friend void BUILTININIT(void *PIF);

    friend int BUILTINOBJECTS(void *PIF, const char *classname);

    friend void *BUILTINADDR(void *PIF, const char *name, unsigned char *is_private);

    StaticList *GeneralMembers;
    int        pipe_read;
    int        pipe_write;
    int        apid;
    int        parent_apid;
    void       *parentPIF;

    SimpleStream *out;

    INTEGER DebugOn;

    AnsiString FileName;
    AnsiString *InputStream;

    AnsiList  *ClassList;
    ClassCode **StaticClassList;

    AnsiList *IncludedList;
    AnsiList *ModuleList;
    StaticList ModuleNamesList;
    ConstantMapType *ConstantList;
    AnsiList UndefinedMembers;
    AnsiList UndefinedClasses;
    AnsiList DeletedMembers;

    SYS_INT *StaticLinks;
    int     StaticLinksCount;
    int     StaticLinksSize;

    unsigned int LinkStatic(const char *funname);
    INTEGER AddUndefinedMember(AnsiString& member, TinyString& _CLASS, const char *_MEMBER, intptr_t line);
    INTEGER CheckUndefinedMembers();
    INTEGER AddUndefinedClass(AnsiString& member, TinyString& _CLASS, const char *_MEMBER, intptr_t line);
    INTEGER CheckUndefinedClasses();

#ifdef CACHED_VARIABLES
    INTEGER VariableIsDescribed(AnsiString& S, DoubleList *VDList, HashTable *CachedVariables = 0, char is_hased = 0);
#else
    INTEGER VariableIsDescribed(AnsiString& S, DoubleList *VDList);
#endif
    INTEGER ConstantIsDescribed(AnsiString& S, ConstantMapType *VDList);

    AnsiString GetSpecial(AnsiParser *P, ClassCode *CC, ClassMember *CM, AnsiString special);
    INTEGER BuildFunction(ClassCode *CC, AnsiParser *P, INTEGER on_line = 0, INTEGER ACCESS = ACCESS_PUBLIC, INTEGER OPERATOR = 0, char STATIC = 0, const char *prec_parse = 0, char is_inline = 0);
    INTEGER BuildProperty(ClassCode *CC, AnsiParser *P, INTEGER on_line = 0, INTEGER ACCESS = ACCESS_PUBLIC);
    INTEGER BuildEvent(ClassCode *CC, AnsiParser *P, INTEGER on_line = 0, INTEGER ACCESS = ACCESS_PUBLIC);
    INTEGER BuildOverride(ClassCode *CC, AnsiParser *P, INTEGER on_line);
    INTEGER IncludePackage(const char *filename);
    INTEGER IncludeFile(const char *MODULE_NAME, INTEGER on_line);
    INTEGER RuntimeIncludeCode(const char *CODE);

    AnsiString NormalizePath(AnsiString *MODULE_NAME);

    INTEGER BuildClass(AnsiParser *P, INTEGER on_line = 0);

    INTEGER BuildPragma(AnsiParser *P, ClassCode *CC = NULL);

    INTEGER Execute(AnsiString *Stream, INTEGER on_line = 0, char _USE_WARN = DEFAULT_USE_WARNINGS, char _USE_EXC = DEFAULT_USE_EXCEPTIONS, char _USE_IMPLICIT = DEFAULT_USE_IMPLICIT);
    SYS_INT ClassExists(const char *name, char by_addr = 0, int *index = 0);
    INTEGER ListContains(const char *S, AnsiList *VDList);
    INTEGER BuildVariable(ClassCode *CC, AnsiParser *P, INTEGER on_line, INTEGER ACCESS);
    INTEGER ExtendClass(ClassCode *CC, AnsiParser *P, INTEGER on_line, INTEGER OWNER_CLSID);

    INTEGER ENTRY_CLASS;

    DEBUGGER_CALLBACK DEBUGGER_TRAP;
    void              *DEBUGGER_RESERVED;
    char              *SERVER_PUBLIC_KEY;
    char              *SERVER_PRIVATE_KEY;
    char              *CLIENT_PUBLIC_KEY;

    AnsiString INCLUDE_DIR;
    AnsiString IMPORT_DIR;
    AnsiString TEMP_INC_DIR;

    char         USE_WARN;
    char         USE_EXC;
    char         USE_IMPLICIT;
    char         STRICT_MODE;
    char         *PROFILE_DRIVEN;
    unsigned int PROFILE_DRIVEN_ID;
    int          INCLUDE_LEVEL;

    char         enable_private;
    INTEGER Warning(const char *WRN, int line, int wrn_code, const char *extra, const char *filename = 0, const char *class_name = 0, const char *member_name = 0);
    INTEGER Warning3(const char *WRN, int line, int wrn_code, const char *extra1, const char *extra2, const char *extra3, const char *filename = 0, const char *class_name = 0, const char *member_name = 0);
    void DefineConstant(const char *name, const char *value, int is_string = 1);

    char *CheckMember(const char *member_name);

    char basic_constants_count;
#ifdef CACHED_LIST
    HashTable CachedClasses;
#endif

    HHSEM DelegateLock;

    Array        *var_globals;
    unsigned int IDGenerator;
    void         *static_result;
    unsigned int Workers;
    unsigned int TSClassCount;
    ProtoData PDATA[PDATA_ITEMS];
public:
#ifdef SIMPLE_MULTI_THREADING
    HHSEM AllocLock;
#endif

#ifdef SIMPLE_MULTI_THREADING
    //HHSEM WriteLock;
    //char  MasterLock;
    Semaphore WriteLock;
    HHSEM     InternalLock;
    uintptr_t ThreadsCount;
#endif

    VARPool *POOL;
    VARPool *CACHEDPOOL;

    void *CLASSPOOL;
    void *CACHEDCLASSPOOL;

    void *ARRAYPOOL;
    void *CACHEDARRAYPOOL;
    int  object_count;

    int    free_vars;
    int    free_class_objects;
    int    free_arrays;
    void   *RootInstance;
    GCRoot *GCROOT;
    int    dirty_limit;
    int    last_gc_run;
    char   in_gc;

    static int refSOCKET;
    POOLED(PIFAlizator)
    void *CachedhDLL;
    static int         argc;
    static char        **argv;
    static HHSEM       WorkerLock;
    static char        WorkerLockInitialized;
    int                last_result;
    int                direct_pipe;
    int                fixed_class_count;
    static CHECK_POINT CheckPoint;

    static AnsiString GetPath(AnsiString *S);
    void AcknoledgeRunTimeError(SCStack *STACK_TRACE, AnsiException *Exc);

    DoubleList PIF;
    DoubleList VariableDescriptors;
    AnsiList   Errors;
    AnsiList   Warnings;

    char is_buffer;

#ifdef PRINT_DEBUG_INFO
    AnsiString DEBUG_CLASS_CONFIGURATION();
    AnsiString DEBUG_INFO();
#endif
    AnsiString PRINT_ERRORS(int html = 0);
    AnsiString PRINT_WARNINGS(int html = 0);
    AnsiString SerializeWarningsErrors(int ser_warnings = 0);

    INTEGER ErrorCount();
    INTEGER WarningCount();

    void Adjust(INTEGER debug, DEBUGGER_CALLBACK DC, void *DEBUGGER_RESERVED, char *SPubK, char *SPrivK, char *CPubK);

    PIFAlizator(AnsiString INC_DIR, AnsiString LIB_DIR, AnsiString *S, SimpleStream *Out, AnsiString _FileName = "", INTEGER debug = 0, DEBUGGER_CALLBACK DC = 0, void *DEBUGGER_RESERVED = 0, char *SPubK = 0, char *SPrivK = 0, char *CPubK = 0, PIFAlizator *sibling = NULL, void *static_result = NULL, void *module_list = NULL);
    void SetPipe(int pipein, int pipeout, int apid, int papid, int direct_pipe);
    INTEGER Execute();
    void UpdatePath();
    void Optimize(int start = 0, char use_compiled_code = 0, char use_lock = 1);
    void OptimizeMember(ClassMember *CM);
    bool CheckRunable();
    void *GetStartingPoint();
    int Serialize(char *filename, bool is_lib = false);
    int Unserialize(char *filename, bool is_lib = false);
    static int ComputeSharedSize(char *filename);
    void Hibernate();
    void SyncClassList();

#ifdef DEBUGGER_VAR_NAMES
    HashTable DebugVarNames;
#endif
    void *Helper;

    INTEGER FindVariableByName(void *key, const char *name);
    void RegisterVariableName(void *key, const char *name, INTEGER val);

    void Clear();
    ~PIFAlizator(void);

    void EnsureThreadSafe();
    static void Shutdown();
};
#endif //__PIFALIZATOR_H

