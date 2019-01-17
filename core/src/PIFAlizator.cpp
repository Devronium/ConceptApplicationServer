#include "PIFAlizator.h"
#include "AnsiParser.h"
#include "TinyString.h"
#include "ConceptTypes.h"
#include "AnsiException.h"
#include "Optimizer.h"
#include "GarbageCollector.h"
#include "ConceptInterpreter.h"
#include "SHManager.h"
#include "ModuleLink.h"
#include "StaticList.h"
#include "BuiltIns.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
 #include <io.h>
 #include <windows.h>
#else
 #include <sys/time.h>
#endif
#ifndef NO_BUILTIN_REGEX
extern "C" {
 #include "builtin/regexp.h"
}
#endif

POOLED_IMPLEMENTATION(PIFAlizator)

#define AddGeneralMember(MNAME)    GeneralMembers->Add(MNAME)

int             PIFAlizator::argc = 0;
char            **PIFAlizator::argv = 0;
int             PIFAlizator::refSOCKET            = -1;
CHECK_POINT     PIFAlizator::CheckPoint           = 0;
SECURE_MESSAGE  SimpleStream::send_secure_message = 0;
HHSEM           PIFAlizator::WorkerLock;
char            PIFAlizator::WorkerLockInitialized= 0;

static const char *level_names[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
static const char *level_colors[] = { "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m" };

ConceptLogContext *get_log_context(PIFAlizator *pif) {
    if (!pif)
        return NULL;

    if (pif->log_context)
        return pif->log_context;

    PIFAlizator *parentPIF = (PIFAlizator *)pif->parentPIF;
    while (parentPIF) {
        if (parentPIF->log_context) {
            pif->log_context = parentPIF->log_context;
            pif->log_context->links ++;
            return pif->log_context;
        }
        parentPIF = (PIFAlizator *)parentPIF->parentPIF;
    }
    pif->log_context = (ConceptLogContext *)malloc(sizeof(ConceptLogContext));
    pif->log_context->logfile = NULL;
    pif->log_context->loglevel = 0;
    seminit(pif->log_context->loglock, 1);
#ifdef _WIN32
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD fdwSaveOldMode;
    GetConsoleMode(hStdout, &fdwSaveOldMode);
    if (SetConsoleMode(hStdout, fdwSaveOldMode | 0x0004))
        pif->log_context->colors = 1;
    else
        pif->log_context->colors = 0;
#else
    pif->log_context->colors = 1;
#endif
    pif->log_context->quiet = 0;
    pif->log_context->owner = pif;
    pif->log_context->links = 1;

    return pif->log_context;
}

#ifdef _WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif
void log_log(PIFAlizator *pif, int level, const char *file, int line, const char *data) {
    ConceptLogContext *log_context = get_log_context(pif);
    if (!log_context)
        return;

    if ((level < log_context->loglevel) || (level < 0) || (level > 5))
        return;

    semp(log_context->loglock);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t now = (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;

    time_t t = (time_t)(now/1000);
    struct tm *lt = localtime(&t);

    if (!log_context->quiet) {
        char buf[16];
        buf[strftime(buf, sizeof(buf), "%H:%M:%S", lt)] = '\0';
        if (log_context->colors)
            fprintf(stderr, "%s.%03d %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m %s\n", buf, (int)(now % 1000), level_colors[level], level_names[level], file, line, data);
        else
            fprintf(stderr, "%s.%03d %-5s %s:%d: %s\n", buf, (int)(now % 1000), level_names[level], file, line, data);
        fflush(stderr);
    }

    if (log_context->logfile) {
        char buf[32];
        buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';
        fprintf(log_context->logfile, "%s.%03d %-5s %s:%d: %s\n", buf, (int)(now % 1000), level_names[level], file, line, data);
        fflush(log_context->logfile);
    }
    semv(log_context->loglock);
}

int log_use_file(PIFAlizator *pif, const char *filename) {
    ConceptLogContext *log_context = get_log_context(pif);
    if (!log_context)
        return 0;

    if (log_context->logfile)
        fclose(log_context->logfile);

    log_context->logfile = fopen(filename, "ab");
    if (!log_context->logfile)
        log_context->logfile = fopen(filename, "wb");

    if (log_context->logfile)
        return 1;

    return 0;
}

void PIFAlizator::Shutdown() {
    if (PIFAlizator::WorkerLockInitialized) {
        semdel(PIFAlizator::WorkerLock);
        PIFAlizator::WorkerLockInitialized = 0;
    }
}

void PIFAlizator::AcknoledgeRunTimeError(SCStack *STACK_TRACE, AnsiException *Exc) {
    // notify the client about the run-time error ...
    // --------------------------------------- //
    struct plainstring cstack;
    plainstring_init(&cstack);

    INTERNAL_LOCK(this)
    int first = 1;
    if ((STACK_TRACE) && (STACK_TRACE->TOP))
        STACK_TRACE = (SCStack *)STACK_TRACE->TOP;

    while (STACK_TRACE) {
        ClassMember *CM = (ClassMember *)STACK_TRACE->CM;
        if (CM) {
            if (CM->Defined_In) {
                plainstring_add(&cstack, ((ClassCode *)CM->Defined_In)->NAME.c_str());
                plainstring_add_char(&cstack, '.');
            } else
                plainstring_add(&cstack, "::");
            plainstring_add(&cstack, CM->NAME);
        } else {
            plainstring_add(&cstack, "STATIC/LIBRARY.STATIC_FUNCTION");
        }
        plainstring_add_char(&cstack, ':');
        if (first) {
            plainstring_add_int(&cstack, Exc->GetLine());
            first   = 0;
        } else
            plainstring_add_int(&cstack, STACK_TRACE->line);
        plainstring_add_char(&cstack, '\n');
        STACK_TRACE = (SCStack *)STACK_TRACE->PREV;
    }
    int buf_size = 8192;
    char *buf = (char *)malloc(buf_size + plainstring_len(&cstack) + 1);
    if (buf) {
        Exc->ToString(buf, &buf_size);
        if (buf_size > 0)
            buf_size++;
        else
            buf_size = 0;
        memcpy(buf + buf_size, plainstring_c_str(&cstack), plainstring_len(&cstack));
        out->ClientError(buf);
        free(buf);
    }
    Errors.Add(Exc, DATA_EXCEPTION);
    plainstring_deinit(&cstack);
    INTERNAL_UNLOCK(this)
}

void PIFAlizator::RunTimeError(int err, const char *text, const RuntimeOptimizedElement *OE, const ClassMember *OWNER, SCStack *STACK_TRACE, const char *ERROR_EXTRA) {
    if (OE)
        this->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(text, OE->Operator_DEBUG_INFO_LINE, err, ERROR_EXTRA ? ERROR_EXTRA : GetKeyWord(OE->Operator_ID), ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
    else
        this->AcknoledgeRunTimeError(STACK_TRACE, new AnsiException(text, 0, err, ERROR_EXTRA ? ERROR_EXTRA : OWNER->NAME, ((ClassCode *)(OWNER->Defined_In))->_DEBUG_INFO_FILENAME, ((ClassCode *)(OWNER->Defined_In))->NAME, OWNER->NAME));
}

void PIFAlizator::DefineConstant(const char *name, const char *value, int is_string) {
    if ((!name) || (!name[0]))
        return;
    if ((value) && (!strcmp(name, value))) {
        Errors.Add(new AnsiException(ERR1350, 0, 1350, name, FileName, ""), DATA_EXCEPTION);
        return;
    }
    VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;

    VD->name   = (char *)name;
    VD->USED   = (signed char)this->INCLUDE_LEVEL;
    VD->BY_REF = basic_constants_count ? 0 : 1;

    struct plainstring c_val;
    plainstring_init(&c_val);

    if (is_string) {
        plainstring_add_char(&c_val, '\"');
    }

    size_t len = strlen(value);

    for (unsigned int i = 0; i < len; i++) {
        if (value [i] == '\\') {
            plainstring_add_char(&c_val, value [i]);
        }
        plainstring_add_char(&c_val, value [i]);
    }

    if (is_string)
        plainstring_add_char(&c_val, '\"');

    VD->value.LoadBuffer(plainstring_c_str(&c_val), plainstring_len(&c_val));
    plainstring_deinit(&c_val);
    ConstantList->Add(VD, DATA_VAR_DESCRIPTOR);
}

void PIFAlizator::Adjust(INTEGER debug, DEBUGGER_CALLBACK DC, void *DEBUGGER_RESERVED, char *SPubK, char *SPrivK, char *CPubK) {
    DebugOn                 = debug;
    DEBUGGER_TRAP           = DC;
    SERVER_PUBLIC_KEY       = SPubK;
    SERVER_PRIVATE_KEY      = SPrivK;
    CLIENT_PUBLIC_KEY       = CPubK;
    this->DEBUGGER_RESERVED = DEBUGGER_RESERVED;
#ifdef CACHED_LIST
    StaticList::UseMap = 1;
#endif
}

PIFAlizator::PIFAlizator(AnsiString INC_DIR, AnsiString LIB_DIR, AnsiString *S, SimpleStream *Out, AnsiString _FileName, INTEGER debug, DEBUGGER_CALLBACK DC, void *DEBUGGER_RESERVED, char *SPubK, char *SPrivK, char *CPubK, PIFAlizator *sibling, void *static_result, void *module_list) {
    out = Out;
    if (Out)
        Out->PIF = this;
    FileName                = _FileName;
    InputStream             = S;
    ENTRY_CLASS             = 0;
    DebugOn                 = debug;
    DEBUGGER_TRAP           = DC;
    SERVER_PUBLIC_KEY       = SPubK;
    SERVER_PRIVATE_KEY      = SPrivK;
    CLIENT_PUBLIC_KEY       = CPubK;
    IMPORT_DIR              = LIB_DIR;
    INCLUDE_DIR             = INC_DIR;
    this->DEBUGGER_RESERVED = DEBUGGER_RESERVED;
    USE_WARN                = DEFAULT_USE_WARNINGS;
    USE_EXC                 = DEFAULT_USE_EXCEPTIONS;
    USE_IMPLICIT            = DEFAULT_USE_IMPLICIT;
    STRICT_MODE             = DEFAULT_STRICT_MODE;
    ASG_OVERLOADED          = 0;
    INCLUDE_LEVEL           = 0;
    this->StaticLinks       = 0;
    this->StaticLinksCount  = 0;
    this->StaticLinksSize   = 0;
    this->var_globals       = 0;
    this->IDGenerator       = 0;
    this->direct_pipe       = 0;
    this->StaticClassList   = 0;
    this->enable_private    = 0;
    this->Helper            = 0;
    // important for built-in constants
    this->basic_constants_count = 0;

    if (!WorkerLockInitialized) {
        seminit(WorkerLock, 1);
        WorkerLockInitialized = 1;
    }
    semp(WorkerLock);
#ifdef USE_MEMORY_SPACE
    FAST_MSPACE_CREATE(this->memory);
#endif
#ifdef CACHED_CLASSES
    HashTable_init(&this->CachedClasses);
#endif
    if (sibling) {
        this->parentPIF = sibling;
        this->ClassList = sibling->ClassList;
        // new AnsiList(0);
        //this->ClassList->GetFromList(sibling->ClassList);
        this->USE_WARN = sibling->USE_WARN;
        this->USE_EXC = sibling->USE_EXC;
        this->USE_IMPLICIT = sibling->USE_IMPLICIT;
        this->STRICT_MODE = sibling->STRICT_MODE;

        this->ModuleList       = sibling->ModuleList;
        this->IncludedList     = sibling->IncludedList;
        this->StaticLinks      = sibling->StaticLinks;
        this->StaticLinksCount = sibling->StaticLinksCount;
        this->StaticLinksSize  = -1;
        this->GeneralMembers   = sibling->GeneralMembers;
        this->ConstantList     = sibling->ConstantList;
        if (!_FileName.Length())
            this->FileName = sibling->FileName;
        if (!INC_DIR.Length())
            this->INCLUDE_DIR = sibling->INCLUDE_DIR;
        if (!LIB_DIR.Length())
            this->IMPORT_DIR = sibling->IMPORT_DIR;
        this->SyncClassList();
        if (sibling->ASG_OVERLOADED)
            ASG_OVERLOADED = 1;
    } else {
        parentPIF = NULL;
        ClassList = new AnsiList();
        if (module_list)
            ModuleList = (AnsiList *)module_list;
        else
            ModuleList = new AnsiList();
        this->IncludedList = new AnsiList();

        GeneralMembers = new StaticList();
        AddGeneralMember(MUL);
        AddGeneralMember(DIV);
        AddGeneralMember(REM);
        AddGeneralMember(SUM);
        AddGeneralMember(DIF);
        AddGeneralMember(SHL);
        AddGeneralMember(SHR);
        AddGeneralMember(LES);
        AddGeneralMember(LEQ);
        AddGeneralMember(GRE);
        AddGeneralMember(GEQ);
        AddGeneralMember(EQU);
        AddGeneralMember(NEQ);
        AddGeneralMember(AND);
        AddGeneralMember(XOR);
        AddGeneralMember(OR);
        AddGeneralMember(BAN);
        AddGeneralMember(BOR);
        AddGeneralMember(ASG);
        AddGeneralMember(ASU);
        AddGeneralMember(ADI);
        AddGeneralMember(AMU);
        AddGeneralMember(ADV);
        AddGeneralMember(ARE);
        AddGeneralMember(AAN);
        AddGeneralMember(AXO);
        AddGeneralMember(AOR);
        AddGeneralMember(ASL);
        AddGeneralMember(ASR);
        AddGeneralMember(SELECTOR_I);

        AddGeneralMember(NOT);
        AddGeneralMember(COM);
        AddGeneralMember(INC);
        AddGeneralMember(DEC);

        this->ConstantList = new ConstantMapType();
        DefineConstant("_C_INCLUDE_DIR", INC_DIR);
        DefineConstant("_C_LIB_DIR", LIB_DIR);
        DefineConstant("_C_FILENAME", _FileName);
        DefineConstant("true", "1", 0);
        DefineConstant("false", "0", 0);
        DefineConstant("null", "0", 0);

        BUILTININIT(this);
    }
    // done with built-in constants
    basic_constants_count = this->ConstantList->Count();

    this->CachedhDLL        = 0;
    this->PROFILE_DRIVEN    = 0;
    this->PROFILE_DRIVEN_ID = 0;

#ifdef SIMPLE_MULTI_THREADING
    //seminit(this->WriteLock, 1);
    seminit(this->InternalLock, 1);
    //this->MasterLock   = 0;
    this->ThreadsCount = 0;
    seminit(this->AllocLock, 1);
#endif
    seminit(this->DelegateLock, 1);

    for (int i = 0; i < PDATA_ITEMS; i++) {
        PDATA[i].data         = NULL;
        PDATA[i].destroy_func = NULL;
    }

    this->static_result      = static_result;
    this->POOL               = 0;
    this->CACHEDPOOL         = 0;
    this->CLASSPOOL          = 0;
    this->CACHEDCLASSPOOL    = 0;
    this->ARRAYPOOL          = 0;
    this->CACHEDARRAYPOOL    = 0;
    this->RootInstance       = 0;
    this->GCROOT             = 0;
    this->object_count       = 0;
    this->dirty_limit        = 10000;
    this->last_gc_run        = 0;
    this->free_vars          = 0;
    this->free_class_objects = 0;
    this->free_arrays        = 0;
    this->last_result        = 0;
    this->is_buffer          = 0;
    this->fixed_class_count  = 0;
    this->in_gc              = 0;
    this->TSClassCount       = 0;
    this->Workers            = 0;
    this->log_context        = 0;

#ifdef DEBUGGER_VAR_NAMES
    HashTable_init(&this->DebugVarNames);
#endif
    MemoryTable_init(&this->LibraryAllocations);
    semv(WorkerLock);
}

void PIFAlizator::SetPipe(int pipein, int pipeout, int apid, int papid, int direct_pipe) {
    this->apid        = apid;
    this->parent_apid = papid;
    this->pipe_read   = pipein;
    this->pipe_write  = pipeout;
    this->direct_pipe = direct_pipe;
}

void PIFAlizator::Clear() {
    if (out->send_secure_message)
        out->send_secure_message = NULL;
    // calling all the destructors in modules, without unloading them
    int count = ModuleList->Count();
    for (INTEGER i = 0; i < count; i++) {
        HMODULE hLIBRARY = (HMODULE)ModuleList->Item(i);
        UnImportModule(hLIBRARY, this);
    }
    this->Warnings.Clear();
    this->Errors.Clear();
    this->UndefinedMembers.Clear();
    this->UndefinedClasses.Clear();
}

PIFAlizator::~PIFAlizator(void) {
#ifdef CACHED_CLASSES
    HashTable_deinit(&this->CachedClasses);
#endif
#ifdef DEBUGGER_VAR_NAMES
    HashTable_deinit(&this->DebugVarNames);
#endif
    if (Helper) {
        delete_OptimizerHelper((struct OptimizerHelper *)Helper);
        Helper = NULL;
    }

    for (int i = 0; i < PDATA_ITEMS; i++) {
        if (PDATA[i].destroy_func)
            PDATA[i].destroy_func(PDATA[i].data, this);
        PDATA[i].destroy_func = NULL;
        PDATA[i].data         = NULL;
    }

    ClassPool *C_POOL = (ClassPool *)this->CLASSPOOL;
    ClassPool *C_NEXT;
    while (C_POOL) {

        C_NEXT = (ClassPool *)C_POOL->NEXT;
        OBJECT_FREE(this, C_POOL);
        C_POOL = C_NEXT;
    }

    ArrayPool *A_POOL = (ArrayPool *)this->ARRAYPOOL;
    ArrayPool *A_NEXT;
    while (A_POOL) {

        A_NEXT = (ArrayPool *)A_POOL->NEXT;
        OBJECT_FREE(this, A_POOL);
        A_POOL = A_NEXT;
    }

    if (!parentPIF) {
        if (this->StaticLinks) {
            free(this->StaticLinks);
            this->StaticLinks = 0;
        }
        while (this->ClassList->Count()) {
            ClassCode *CC = (ClassCode *)this->ClassList->Remove(0);
            CC->BeforeDestructor(this);
            delete CC;
        }
        this->DeletedMembers.Clear();
        int count = ModuleList->Count();
        for (INTEGER i = 0; i < count; i++) {
            HMODULE hLIBRARY = (HMODULE)ModuleList->Item(i);
            if ((this->static_result) || (this->Workers))
                UnImportModule(hLIBRARY, this);
            else
                UnImportModule(hLIBRARY);
        }
        this->ModuleList->Clear();
        delete this->ClassList;
        delete this->ModuleList;
        delete this->IncludedList;
        delete this->GeneralMembers;
        delete this->ConstantList;
    } else {
        int count = ModuleList->Count();
        for (INTEGER i = 0; i < count; i++) {
            HMODULE hLIBRARY = (HMODULE)ModuleList->Item(i);
            UnImportModule(hLIBRARY, this);
        }
        if ((this->StaticLinksSize > 0) && (this->StaticLinks))
            free(this->StaticLinks);
    }

#ifdef SIMPLE_MULTI_THREADING
    //semdel(WriteLock);
    semdel(InternalLock);
    semdel(AllocLock)
#endif
    semdel(this->DelegateLock);

    VARPool *POOL = this->POOL;
    VARPool *NEXT;
    while (POOL) {
        NEXT = (VARPool *)POOL->NEXT;
        FAST_FREE(this, POOL);
        POOL = NEXT;
    }

    GCRoot *root = this->GCROOT;
    while (root) {
        GCRoot *p = root;
        root = (GCRoot *)root->NEXT;
        FAST_FREE(this, p);
    }
    free(StaticClassList);
    StaticClassList = NULL;
    BUILTINDONE();

    for (MemoryTableIterator it = MemoryTable_begin(&this->LibraryAllocations); it != MemoryTable_end(&this->LibraryAllocations); ++it) {
        void *ptr = MemoryTable_key(&this->LibraryAllocations, it);
        if (ptr) {
            fprintf(stderr, "WARNING: %i bytes at address %X allocated in static library was not freed\n", (int)MemoryTable_val(&this->LibraryAllocations, it), ptr);
            FAST_FREE(this, ptr);
        }
    }
    if (this->log_context) {
        this->log_context->links --;
        if (!this->log_context->links) {
            if (this->log_context->logfile)
                fclose(this->log_context->logfile);
            semdel(this->log_context->loglock);
            free(this->log_context);
        }
    }
    MemoryTable_deinit(&this->LibraryAllocations);
#ifdef USE_MEMORY_SPACE
    FAST_MSPACE_DESTROY(this->memory);
#endif
}

unsigned int PIFAlizator::LinkStatic(const char *funname) {
    unsigned int res = 0;
    SYS_INT      ref = LinkFunction(this, funname, ModuleList, &CachedhDLL);

    if (ref) {
        for (int i = 0; i < this->StaticLinksCount; i++) {
            if (this->StaticLinks [i] == ref) {
                return i + 1;
            }
        }

        this->StaticLinksCount++;
        if (this->StaticLinksSize < 0) {
            PIFAlizator *PIF = this;
            while (PIF->parentPIF)
                PIF = (PIFAlizator *)PIF->parentPIF;

            this->StaticLinksSize = PIF->StaticLinksSize;
            if (this->StaticLinksSize) {
                this->StaticLinks = (SYS_INT *)malloc(sizeof(SYS_INT) * this->StaticLinksSize);
                memcpy(this->StaticLinks, PIF->StaticLinks, sizeof(SYS_INT) * this->StaticLinksSize);
            } else {
                this->StaticLinksSize = 0;
                this->StaticLinks     = NULL;
            }
        }

        if (this->StaticLinksCount > this->StaticLinksSize) {
            this->StaticLinksSize += STATIC_LINKS_INCREMENT;
            if (this->StaticLinks) {
                this->StaticLinks = (SYS_INT *)realloc(this->StaticLinks, sizeof(SYS_INT) * this->StaticLinksSize);
            } else {
                this->StaticLinks = (SYS_INT *)malloc(sizeof(SYS_INT) * this->StaticLinksSize);
            }
        }
        this->StaticLinks [this->StaticLinksCount - 1] = ref;
        res = this->StaticLinksCount;
    }
    return res;
}

INTEGER PIFAlizator::AddUndefinedMember(AnsiString& member, TinyString& _CLASS, const char *_MEMBER, intptr_t line) {
    UndefinedMember *UM = new UndefinedMember;

    UM->filename = FileName;
    UM->line     = line;
    UM->name     = member;
    UM->_CLASS   = _CLASS;
    UM->_MEMBER  = _MEMBER;
    UndefinedMembers.Add(UM, DATA_UNDEFINEDMEMBER);
    AddGeneralMember(member);
    return GeneralMembers->Count();
}

INTEGER PIFAlizator::AddUndefinedClass(AnsiString& member, TinyString& _CLASS, const char *_MEMBER, intptr_t line) {
    UndefinedMember *UM = new UndefinedMember;

    UM->filename = FileName;
    UM->line     = line;
    UM->name     = member;
    UM->_CLASS   = _CLASS;
    UM->_MEMBER  = _MEMBER;
    UndefinedClasses.Add(UM, DATA_UNDEFINEDMEMBER);
    return UndefinedClasses.Count();
}

#define DefineMember(MNAME)                                                    \
    {                                                                          \
        INTEGER cnt = UndefinedMembers.Count();                                \
        for (INTEGER i = 0; i < cnt; i++) {                                    \
            UndefinedMember *UM = (UndefinedMember *)UndefinedMembers.Item(i); \
            if (UM->name == MNAME) {                                           \
                UndefinedMembers.Delete(i);                                    \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    }

#define DefineClass(CNAME)                                                     \
    {                                                                          \
        INTEGER cnt = UndefinedClasses.Count();                                \
        for (INTEGER i = 0; i < cnt; i++) {                                    \
            UndefinedMember *UM = (UndefinedMember *)UndefinedClasses.Item(i); \
            if (UM->name == CNAME) {                                           \
                UndefinedClasses.Delete(i);                                    \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    }

#ifdef CACHED_VARIABLES
INTEGER PIFAlizator::VariableIsDescribed(AnsiString& S, DoubleList *VDList, struct HashTable *CachedVariables, char is_hased) {
    if (is_hased) {
        unsigned int res  = HashTable_find(CachedVariables, S.c_str());
        if (res) {
            if (is_hased == -1)
                return res;
            VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)(*VDList) [res - 1];
            if ((VD) && (VD->name == S)) {
                VD->USED = 1;
                return res;
            }
        } else
            return 0;
    }
#else
INTEGER PIFAlizator::VariableIsDescribed(AnsiString& S, DoubleList *VDList) {
#endif
    INTEGER Count = VDList->Count();

    for (INTEGER i = 0; i < Count; i++) {
        VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)VDList->Item(i);
        if (VD->name == S) {
            VD->USED = 1;
            return i + 1;
        }
    }
    return 0;
}

INTEGER PIFAlizator::ConstantIsDescribed(AnsiString& S, ConstantMapType *VDList, VariableDESCRIPTOR **old) {
    if (old)
        *old = NULL;
#ifdef STDMAP_CONSTANTS
    VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)VDList->Find(S.c_str());
    if ((VD) && (VD->name == S)) {
        VD->USED = 1;
        if (old)
            *old = VD;
        return 1;
    }
#else
    INTEGER Count = VDList->Count();

    for (INTEGER i = 0; i < Count; i++) {
        VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)VDList->Item(i);
        if (VD->name == S) {
            VD->USED = 1;
            if (old)
                *old = VD;
            return i + 1;
        }
    }
#endif
    return 0;
}

INTEGER PIFAlizator::ListContains(const char *S, AnsiList *VDList) {
    INTEGER Count = VDList->Count();

    for (INTEGER i = 0; i < Count; i++) {
        TinyString *STR = (TinyString *)(*VDList) [i];
        if (*STR == S) {
            return i + 1;
        }
    }
    return 0;
}

SYS_INT PIFAlizator::ClassExists(const char *name, char by_addr, int *index) {
#ifdef CACHED_CLASSES
    unsigned int pos = HashTable_find(&CachedClasses, name);
    if ((pos) && (by_addr)) {
        *index = (int)pos;
        return (SYS_INT)ClassList->Item(pos - 1);
    }
    return pos;
#endif
    INTEGER Count = ClassList->Count();

    for (INTEGER i = 0; i < Count; i++) {
        ClassCode *CC = (ClassCode *)(*ClassList) [i];
        if (CC->NAME == name) {
            if (CC->NEEDED != -1) {
                if (!CC->NEEDED) {
                    CC->UpdateNeeded();
                }
                CC->NEEDED = 1;
            }
            if (by_addr) {
                if (index) {
                    *index = i + 1;
                }
                return (SYS_INT)CC;
            }
            return i + 1;
        }
    }
    return 0;
}

INTEGER PIFAlizator::BuildVariable(ClassCode *CC, AnsiParser *P, INTEGER on_line, INTEGER ACCESS) {
    AnsiString sPARSE;
    AnsiString s_res;

    P->NextAtom(sPARSE);

    char    is_array = 0;
    INTEGER TYPE     = GetType(sPARSE);
    INTEGER _ID      = GetID(sPARSE);
    char    sign     = 1;

    if ((TYPE == TYPE_OPERATOR) && (_ID == KEY_SELECTOR_I)) {
        is_array = 1;
        P->NextAtom(sPARSE);
        TYPE = GetType(sPARSE);
        _ID  = GetID(sPARSE);
    }

    if ((_ID) || (TYPE != TYPE_METHOD)) {
        Errors.Add(new AnsiException(ERR160, on_line ? on_line : P->LastLine(), 160, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
        return 0;
    }

    int  ref_id    = GeneralMembers->ContainsString(sPARSE.c_str());
    char *ref_name = 0;
    if (!ref_id) {
        AddGeneralMember(sPARSE);
        ref_id = GeneralMembers->Count();
    } else {
        DefineMember(sPARSE);
    }
    ref_name = GeneralMembers->Item(ref_id - 1);

    ClassMember *CM = CC->AddMember(this, ref_name, on_line ? on_line : P->LastLine(), FileName, ACCESS, true);
    if (is_array) {
        CM->VD->TYPE = VARIABLE_ARRAY;
    }

    P->NextAtom(sPARSE);
    TYPE = GetType(sPARSE);
    _ID  = GetID(sPARSE);

    if ((TYPE == TYPE_OPERATOR) && (_ID == KEY_ASG)) {
        if (is_array) {
            Errors.Add(new AnsiException(ERR1170, on_line ? on_line : P->LastLine(), 1170, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        }

        do {
            P->NextAtom(sPARSE);
            TYPE = GetType(sPARSE);
            _ID  = GetID(sPARSE);

            if (TYPE == TYPE_OPERATOR) {
                if (_ID == KEY_DIF) {
                    sign *= -1;
                }
            }
        } while ((TYPE == TYPE_OPERATOR) && ((_ID == KEY_SUM) || (_ID == KEY_DIF)));

        if (TYPE == TYPE_HEX_NUMBER) {
            uint64_t TEMP_NR = 0;
            sscanf(sPARSE.c_str() + 2, "%llx", &TEMP_NR);
            sPARSE = AnsiString((double)TEMP_NR);
            TYPE   = TYPE_NUMBER;
        }

        if ((TYPE == TYPE_STRING) || (TYPE == TYPE_NUMBER)) {
            if (TYPE == TYPE_STRING) {
                StripString(&sPARSE, &s_res);
                CM->VD->value = s_res;
            } else {
                CM->VD->nValue = sPARSE.ToFloat() * sign;
            }

            CM->VD->TYPE = TYPE == TYPE_STRING   ?   VARIABLE_STRING :   VARIABLE_NUMBER;
        } else {
            Errors.Add(new AnsiException(ERR1160, on_line ? on_line : P->LastLine(), 1160, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        }

        P->NextAtom(sPARSE);
        TYPE = GetType(sPARSE);
        _ID  = GetID(sPARSE);
    }

    if ((TYPE != TYPE_SEPARATOR) && (_ID != KEY_SEP)) {
#ifdef OPTIONAL_SEPARATOR
        if (STRICT_MODE) {
            Errors.Add(new AnsiException(ERR340, on_line ? on_line : P->LastLine(), 340, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
            return 1;
        } else
            P->Rollback();
#else
        Errors.Add(new AnsiException(ERR340, on_line ? on_line : P->LastLine(), 340, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
        return 1;
#endif
    }
    return 0;
}

AnsiString PIFAlizator::GetSpecial(AnsiParser *P, ClassCode *CC, ClassMember *CM, AnsiString special) {
    AnsiString result;

    if (special == "line") {
        result = P->LastLine();
    } else
    if (special == "filename") {
        result += "'";
        int len = FileName.Length();
        int pos = 0;
        // len-2 = at least one character for the filename
        for (int i = len - 2; i >= 0; i--) {
            char c = FileName [i];
            if ((c == '/') || (c == '\\')) {
                pos = i + 1;
                break;
            }
        }
        result += AnsiString(FileName.c_str() + pos);
        result += "'";
    } else
    if (special == "path") {
        result += "'";
        int len = FileName.Length();
        for (int i = 0; i < len; i++) {
            char c = FileName [i];
            if (c == '\\') {
                result += '/';
            } else {
                result += c;
            }
        }
        result += "'";
    } else
    if (special == "class") {
        result += "'";
        result += CC->NAME.c_str();
        result += "'";
    } else
    if (special == "member") {
        result += "'";
        result += CM->NAME;
        result += "'";
    } else
    if (special == "time") {
        result = (intptr_t)time(NULL);
    } else {
        Errors.Add(new AnsiException(ERR1209, P->LastLine(), 1209, special, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        P->NextAtom(result);
    }
    return result;
}

INTEGER PIFAlizator::BuildFunction(ClassCode *CC, AnsiParser *P, INTEGER on_line, INTEGER ACCESS, INTEGER OPERATOR, char STATIC, const char *prec_parse, char is_inline) {
    INTEGER             BEGIN_END_LEVEL = 0;
    INTEGER             ATOMIC_LEVEL    = 0;
    bool                is_destructor   = false;
    INTEGER             TYPE            = -1;
    SYS_INT             _ID             = -1;
    INTEGER             PREC_TYPE       = -1;
    SYS_INT             PREC_ID         = -1;
    INTEGER             PREC_2_TYPE     = -1;
    SYS_INT             PREC_2_ID       = -1;
    AnsiString          sPARSE;
    AnsiString          cachedMember;
    AnsiString          cached;
    char                is_cached;
    char                *ref_name        = 0;
    ClassMember         *CM              = 0;
    char                IN_VAR_STATAMENT = 0;
    INTEGER             inline_count     = 0;
    AnsiString          s_res;

    struct MemoryTable  SelectorCacheList;
    MemoryTable_init(&SelectorCacheList);

    int ref_id = 0;

    if (is_inline) {
        sPARSE   = prec_parse;
        sPARSE  += "#";
        sPARSE  += AnsiString((intptr_t)OPERATOR);
        OPERATOR = 0;

        ref_id = GeneralMembers->ContainsString(sPARSE.c_str());
        if (!ref_id) {
            AddGeneralMember(sPARSE);
            ref_id = GeneralMembers->Count();
        }
        ref_name        = GeneralMembers->Item(ref_id - 1);
        CM              = CC->AddMember(this, ref_name, on_line ? on_line : P->LastLine(), FileName, ACCESS, false);
        CM->IS_FUNCTION = 1;
        CM->IS_STATIC   = STATIC;

        cached = "{";
    } else {
        if (prec_parse) {
            sPARSE = prec_parse;
        } else {
            P->NextAtom(sPARSE);
        }

        ref_id = GeneralMembers->ContainsString(sPARSE.c_str());
        if (!ref_id) {
            if (OPERATOR) {
                Errors.Add(new AnsiException(ERR320, on_line ? on_line : P->LastLine(), 320, sPARSE, FileName, CC->NAME, sPARSE), DATA_EXCEPTION);
            }
            AddGeneralMember(sPARSE);
            ref_id = GeneralMembers->Count();
        } else {
            DefineMember(sPARSE);
        }
        ref_name = GeneralMembers->Item(ref_id - 1);

        CM = CC->AddMember(this, ref_name, on_line ? on_line : P->LastLine(), FileName, ACCESS, false);
        CM->IS_FUNCTION = 1;
        CM->IS_OPERATOR = OPERATOR;
        if (!OPERATOR)
            CM->IS_STATIC = STATIC;

        TYPE = GetType(sPARSE);

        if ((TYPE == TYPE_OPERATOR) && (!OPERATOR)) {
            Errors.Add(new AnsiException(ERR300, on_line ? on_line : P->LastLine(), 300, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        } else
        if ((TYPE != TYPE_OPERATOR) && (OPERATOR)) {
            Errors.Add(new AnsiException(ERR310, on_line ? on_line : P->LastLine(), 310, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        }

        if (CC->NAME == CM->NAME) {
            CC->CONSTRUCTOR        = CC->Members->Count();
            CC->CONSTRUCTOR_MEMBER = CM;
        } else
        if (!strcmp(CM->NAME, (char *)C_FINALIZE)) {
            sPARSE  = CC->NAME.c_str();
            sPARSE += "@";
            sPARSE += C_FINALIZE;

            ref_id   = GeneralMembers->ContainsString(sPARSE.c_str());
            ref_name = 0;
            if (!ref_id) {
                AddGeneralMember(sPARSE);
                ref_id = GeneralMembers->Count();
            } else {
                DefineMember(sPARSE);
            }
            ref_name = GeneralMembers->Item(ref_id - 1);

            CM->NAME              = ref_name;
            CC->DESTRUCTOR        = CC->Members->Count();
            CC->DESTRUCTOR_MEMBER = CM;
            is_destructor         = true;
        }
    }
    DoubleList            *PIFList = CM->CDATA->PIF_DATA;
    DoubleList            *VDList  = CM->CDATA->VariableDescriptors;

    struct HashTable NumberConstantMap;
    HashTable_init(&NumberConstantMap);

    VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
    VD->name   = (char *)THIS_CLASS;
    VD->TYPE   = VARIABLE_NUMBER;
    VD->USED   = -1;
    VD->BY_REF = 0;
    VD->nValue = 0;
    VariableDESCRIPTOR *VD_THIS = VD;

    VDList->Add(VD, DATA_VAR_DESCRIPTOR);
#ifdef CACHED_VARIABLES
    struct HashTable CachedVariables;
    HashTable_init(&CachedVariables);

    HashTable_add(&CachedVariables, THIS_CLASS, 1, -1);
#endif

    if (!is_inline) {
        // izolare parametri ...

        char    NEXT_IS_BY_REF = 0;
        char    NEXT_IS_ASG    = 0;
        INTEGER WANT_PARAM     = 0;
        INTEGER COUNTS         = 0;
        char    START_DEFAULT  = 0;
        char    sign           = 1;

        char    NEXT_IS_TYPE  = 0;
        INTEGER NEXT_IS_CLSID = 0;

        VariableDESCRIPTOR *VDPARAM    = 0;
        char       PUSHED_NEXT_IS_TYPE = 0;
        short      PUSH_COUNT          = 0;
        AnsiString sPARSE_FORWARD;
        do {
            COUNTS++;
            if (sPARSE_FORWARD.Length()) {
                sPARSE         = sPARSE_FORWARD;
                sPARSE_FORWARD = "";
            } else {
                P->NextAtom(sPARSE);
            }

            if (!sPARSE.Length())
                break;

            TYPE = GetType(sPARSE);
            _ID  = GetID(sPARSE);

            if ((TYPE == TYPE_OPERATOR) && (_ID == KEY_SELECTOR_I) && (NEXT_IS_TYPE) && (!PUSHED_NEXT_IS_TYPE)) {
                PUSHED_NEXT_IS_TYPE = NEXT_IS_TYPE;
                PUSH_COUNT++;
                NEXT_IS_TYPE  = VARIABLE_ARRAY;
                NEXT_IS_CLSID = 0;
                continue;
            }

            if (TYPE == TYPE_HEX_NUMBER) {
                uint64_t TEMP_NR = 0;
                sscanf(sPARSE.c_str() + 2, "%llx", &TEMP_NR);
                sPARSE = AnsiString((double)TEMP_NR);
                TYPE   = TYPE_NUMBER;
            }

            if (((TYPE == TYPE_STRING) || (TYPE == TYPE_NUMBER)) && (VDPARAM)) {
                if (NEXT_IS_ASG) {
                    char type = TYPE == TYPE_STRING   ?   VARIABLE_STRING :   VARIABLE_NUMBER;
                    if (VDPARAM->TYPE < 0) {
                        if ((VDPARAM->TYPE == -VARIABLE_ARRAY) || (VDPARAM->TYPE == -VARIABLE_CLASS) ||
                            (VDPARAM->TYPE == -VARIABLE_DELEGATE)) {
                            Errors.Add(new AnsiException(ERR1207, on_line ? on_line : P->LastLine(), 1207, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                        } else if (VDPARAM->TYPE != -type) {
                            Errors.Add(new AnsiException(ERR1208, on_line ? on_line : P->LastLine(), 1208, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                        } else
                            type = -type;
                    }

                    StripString(&sPARSE, &s_res);
                    VDPARAM->value  = s_res;
                    VDPARAM->nValue = s_res.ToFloat() * sign;
                    VDPARAM->TYPE   = type;
                    VDPARAM         = 0;
                    NEXT_IS_ASG     = 0;
                    sign            = 1;
                    continue;
                } else {
                    Errors.Add(new AnsiException(ERR1130, on_line ? on_line : P->LastLine(), 1130, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                    NEXT_IS_ASG = 0;
                    continue;
                }
            } else
            if (NEXT_IS_ASG) {
                if (sPARSE == DIF) {
                    sign *= -1;
                    continue;
                } else
                if (sPARSE == SUM) {
                    continue;
                }

                Errors.Add(new AnsiException(ERR1140, on_line ? on_line : P->LastLine(), 1140, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
            }

            if ((TYPE == TYPE_METHOD) && (_ID == KEY_VAR)) {
                NEXT_IS_BY_REF = 1;
                continue;
            } else
            if (TYPE == TYPE_OPERATOR) {
                if (_ID == KEY_P_OPEN) {
                    if (ATOMIC_LEVEL) {
                        Errors.Add(new AnsiException(ERR780, on_line ? on_line : P->LastLine(), 780, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                    } else {
                        ATOMIC_LEVEL++;
                        WANT_PARAM = 1;
                    }
                } else
                if (_ID == KEY_P_CLOSE) {
                    ATOMIC_LEVEL--;
                    if ((WANT_PARAM) && (COUNTS != 2)) {
                        Errors.Add(new AnsiException(ERR790, on_line ? on_line : P->LastLine(), 790, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                    }
                } else
                if ((_ID == KEY_ASG) && (VDPARAM)) {
                    NEXT_IS_ASG = 1;
                    if (!START_DEFAULT) {
                        START_DEFAULT = 1;
                        CM->MUST_PARAMETERS_COUNT--;
                    }
                    continue;
                } else
                if (_ID == KEY_COMMA) {
                    if (!WANT_PARAM) {
                        WANT_PARAM = 1;
                    } else {
                        Errors.Add(new AnsiException(ERR790, on_line ? on_line : P->LastLine(), 790, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                    }
                } else {
                    Errors.Add(new AnsiException(ERR780, on_line ? on_line : P->LastLine(), 780, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                }
            } else
            if ((TYPE == TYPE_METHOD) && (!_ID) && (WANT_PARAM)) {
                if (sPARSE == "number") {
                    NEXT_IS_TYPE  = VARIABLE_NUMBER;
                    NEXT_IS_CLSID = 0;
                    P->NextAtom(sPARSE_FORWARD);
                    if ((!sPARSE_FORWARD.Length()) || ((sPARSE_FORWARD != ",") && (sPARSE_FORWARD != ")")))
                        continue;
                } else
                if (sPARSE == "string") {
                    NEXT_IS_TYPE  = VARIABLE_STRING;
                    NEXT_IS_CLSID = 0;
                    P->NextAtom(sPARSE_FORWARD);
                    if ((!sPARSE_FORWARD.Length()) || ((sPARSE_FORWARD != ",") && (sPARSE_FORWARD != ")")))
                        continue;
                } else
                if (sPARSE == "array") {
                    NEXT_IS_TYPE  = VARIABLE_ARRAY;
                    NEXT_IS_CLSID = 0;
                    P->NextAtom(sPARSE_FORWARD);
                    if ((!sPARSE_FORWARD.Length()) || ((sPARSE_FORWARD != ",") && (sPARSE_FORWARD != ")")))
                        continue;
                } else
                if (sPARSE == "delegate") {
                    NEXT_IS_TYPE  = VARIABLE_DELEGATE;
                    NEXT_IS_CLSID = 0;
                    P->NextAtom(sPARSE_FORWARD);
                    if ((!sPARSE_FORWARD.Length()) || ((sPARSE_FORWARD != ",") && (sPARSE_FORWARD != ")")))
                        continue;
                } else
                if (sPARSE == "object") {
                    NEXT_IS_TYPE  = VARIABLE_CLASS;
                    NEXT_IS_CLSID = 0;
                    P->NextAtom(sPARSE_FORWARD);
                    if ((!sPARSE_FORWARD.Length()) || ((sPARSE_FORWARD != ",") && (sPARSE_FORWARD != ")")))
                        continue;
                } else {
                    INTEGER cls_id = this->ClassExists(sPARSE.c_str());
                    if (cls_id) {
                        NEXT_IS_TYPE  = VARIABLE_CLASS;
                        NEXT_IS_CLSID = cls_id;
                        P->NextAtom(sPARSE_FORWARD);
                        if ((!sPARSE_FORWARD.Length()) || ((sPARSE_FORWARD != ",") && (sPARSE_FORWARD != ")")))
                            continue;
                    }
                }
#ifdef CACHED_VARIABLES
                if (VariableIsDescribed(sPARSE, VDList, &CachedVariables, -1)) {
#else
                if (VariableIsDescribed(sPARSE, VDList)) {
#endif
                    Errors.Add(new AnsiException(ERR800, on_line ? on_line : P->LastLine(), 800, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                }
                if ((START_DEFAULT) && (VDPARAM)) {
                    Errors.Add(new AnsiException(ERR1150, on_line ? on_line : P->LastLine(), 1150, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                } else if (!START_DEFAULT) {
                    CM->MUST_PARAMETERS_COUNT++;
                }

                VDPARAM         = new VariableDESCRIPTOR;
                VDPARAM->BY_REF = NEXT_IS_BY_REF;
                VDPARAM->USED   = 1;
                VDPARAM->name   = sPARSE;
                VDPARAM->TYPE   = VARIABLE_NUMBER;
                VDPARAM->nValue = 0;

                if (NEXT_IS_TYPE) {
                    VDPARAM->TYPE = -NEXT_IS_TYPE;
                    if ((NEXT_IS_TYPE == VARIABLE_ARRAY) && (PUSHED_NEXT_IS_TYPE)) {
                        VDPARAM->nValue     = (unsigned short)PUSHED_NEXT_IS_TYPE | ((unsigned short)PUSH_COUNT << 8);
                        PUSHED_NEXT_IS_TYPE = 0;
                        PUSH_COUNT          = 0;
                    }

                    if (NEXT_IS_TYPE == VARIABLE_CLASS) {
                        VDPARAM->nValue = NEXT_IS_CLSID;
                    }
                }

                CM->PARAMETERS_COUNT++;

                VDList->Add(VDPARAM, DATA_VAR_DESCRIPTOR);
#ifdef CACHED_VARIABLES
                HashTable_add(&CachedVariables, sPARSE.c_str(), VDList->Count(), sPARSE.Length());
#endif
                WANT_PARAM = 0;
            } else {
                Errors.Add(new AnsiException(ERR770, on_line ? on_line : P->LastLine(), 770, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                if (TYPE == TYPE_KEYWORD) {
                    if (_ID == KEY_BEGIN) {
                        BEGIN_END_LEVEL++;
                    }
                    if (_ID == KEY_END) {
                        HashTable_deinit(&NumberConstantMap);
#ifdef CACHED_VARIABLES
                        HashTable_deinit(&CachedVariables);
#endif
                        MemoryTable_deinit(&SelectorCacheList);
                        return ref_id;
                    }
                    ATOMIC_LEVEL = 0;
                }
            }
            NEXT_IS_BY_REF      = 0;
            NEXT_IS_ASG         = 0;
            NEXT_IS_TYPE        = 0;
            NEXT_IS_CLSID       = 0;
            PUSHED_NEXT_IS_TYPE = 0;
        } while (ATOMIC_LEVEL);
        if ((VDPARAM) && (START_DEFAULT)) {
            Errors.Add(new AnsiException(ERR1150, on_line ? on_line : P->LastLine(), 1150, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        }
        if ((is_destructor) && (CM->PARAMETERS_COUNT)) {
            Errors.Add(new AnsiException(ERR1180, on_line ? on_line : P->LastLine(), 1180, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        }
    }
    int  PREC_IS_CONSTANT = 0;
    int  IS_CONSTANT      = 0;
    char IS_ARRAY         = 0;
    TYPE = -1;
    _ID  = 0;
    do {
        PREC_2_TYPE = PREC_TYPE;
        PREC_2_ID   = PREC_ID;

        PREC_TYPE = TYPE;
        PREC_ID   = _ID;

        _ID = 0;

        if (cached.Length()) {
            sPARSE = cached;
            cached = "";
        } else {
            P->NextAtom(sPARSE, 0, PREC_TYPE, PREC_ID);
        }

        char *ptr_parse = sPARSE.c_str();

        if ((!sPARSE.Length()) && (P->Done())) {
            if (BEGIN_END_LEVEL) {
                Errors.Add(new AnsiException(1210, ERR1210, on_line ? on_line : P->LastLine(), BEGIN_END_LEVEL, " level(s)", FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
            }
            break;
        }

        if (ptr_parse [0] == C_SPECIAL) {
            P->NextAtom(sPARSE);
            sPARSE = GetSpecial(P, CC, CM, sPARSE);
            if (!sPARSE.Length()) {
                break;
            }
        }

        TYPE = GetType(sPARSE);

        if (TYPE == TYPE_HEX_NUMBER) {
            uint64_t TEMP_NR = 0;
            sscanf(sPARSE.c_str() + 2, "%llx", &TEMP_NR);
            sPARSE = AnsiString((double)TEMP_NR);
            TYPE   = TYPE_NUMBER;
        }

        if (TYPE < 0) {
            if ((ptr_parse [0] == '"') || (ptr_parse [0] == '\'')) {
                Errors.Add(new AnsiException(ERR830, on_line ? on_line : P->LastLine(), 830, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
            } else {
                Errors.Add(new AnsiException(ERR050, on_line ? on_line : P->LastLine(), 50, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
            }
            continue;
        }

        if ((IN_VAR_STATAMENT) && (!ATOMIC_LEVEL) && (sPARSE == CONCEPT_COMMA)) {
            TYPE = TYPE_KEYWORD;
            _ID  = KEY_VAR;
            AnalizerElement *AE = new AnalizerElement;
            AE->ID               = KEY_SEP;
            AE->TYPE             = TYPE_SEPARATOR;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = sPARSE;
            AE->_HASH_DATA       = 0;
            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);
            continue;
        }

        if ((PREC_TYPE == TYPE_KEYWORD) && (PREC_ID == KEY_VAR)) {

            if (TYPE == TYPE_METHOD) {
                _ID = GetID(sPARSE);
                if (_ID)
                    TYPE = TYPE_KEYWORD;
            }

            if ((TYPE == TYPE_OPERATOR) || (TYPE == TYPE_KEYWORD)) {
                if (!_ID)
                    _ID = GetID(sPARSE);
                if (_ID == KEY_SELECTOR_I) {
                    TYPE     = PREC_TYPE;
                    _ID      = PREC_ID;
                    IS_ARRAY = 1;
                    continue;
                }
                Errors.Add(new AnsiException(ERR890, on_line ? on_line : P->LastLine(), 890, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                continue;
            }
#ifdef CACHED_VARIABLES
            if (VariableIsDescribed(sPARSE, VDList, &CachedVariables, -1)) {
#else
            if (VariableIsDescribed(sPARSE, VDList)) {
#endif
                Errors.Add(new AnsiException(ERR070, on_line ? on_line : P->LastLine(), 70, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                continue;
            } else {
                TYPE = TYPE_VARIABLE;
            }
        } else
#ifdef CACHED_VARIABLES
        if (((PREC_TYPE != TYPE_OPERATOR) || ((PREC_ID != KEY_SEL) && (PREC_ID != KEY_DLL_CALL))) && (TYPE == TYPE_METHOD) && (VariableIsDescribed(sPARSE, VDList, &CachedVariables, 1))) {
#else
        if (((PREC_TYPE != TYPE_OPERATOR) || ((PREC_ID != KEY_SEL) && (PREC_ID != KEY_DLL_CALL))) && (TYPE == TYPE_METHOD) && (VariableIsDescribed(sPARSE, VDList))) {
#endif
            TYPE = TYPE_VARIABLE;
        }

        if (TYPE == TYPE_VARIABLE) {
#ifdef CACHED_VARIABLES
            _ID = VariableIsDescribed(sPARSE, VDList, &CachedVariables, 1);
#else
            _ID = VariableIsDescribed(sPARSE, VDList);
#endif
            if (!_ID) {
                VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
                VD->name   = sPARSE;
                VD->TYPE   = VARIABLE_NUMBER;
                VD->USED   = 0;
                VD->nValue = 0;
                VD->BY_REF = 0;
                if (IS_ARRAY) {
                    VD->TYPE = VARIABLE_ARRAY;
                }

                VDList->Add(VD, DATA_VAR_DESCRIPTOR);
                _ID = VDList->Count();
#ifdef CACHED_VARIABLES
                HashTable_add(&CachedVariables, sPARSE.c_str(), _ID, sPARSE.Length());
#endif
            }
            IS_ARRAY = 0;

            if (_ID == 1) {
                VD_THIS->USED = 1;
                if (STATIC) {
                    Errors.Add(new AnsiException(ERR930, on_line ? on_line : P->LastLine(), 930, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                    continue;
                }
            }
        }

        if (TYPE == TYPE_NUMBER) {
            // double val = sPARSE.ToFloat();
            AnsiString key(sPARSE.ToFloat());
            int    pos = (int)HashTable_find(&NumberConstantMap, key.c_str());
            if (pos > 0) {
                _ID = pos;
            } else {
                VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
                VD->value  = sPARSE;
                VD->nValue = sPARSE.ToFloat();
                VD->USED   = 1;
                VD->TYPE   = VARIABLE_NUMBER;
                VD->BY_REF = 2;

                VDList->Add(VD, DATA_VAR_DESCRIPTOR);
                _ID = VDList->Count();
                HashTable_add(&NumberConstantMap, key.c_str(), _ID, -1);
            }
            TYPE        = TYPE_VARIABLE;
            IS_CONSTANT = 1;
        } else
        if (TYPE == TYPE_STRING) {
            int pos = (int)HashTable_find(&NumberConstantMap, sPARSE.c_str());
            if (pos > 0) {
                _ID = pos;
            } else {
                VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
                StripString(&sPARSE, &s_res);
                if (s_res.Length()) {
                    VD->value.LoadBuffer(s_res.c_str(), s_res.Length());
                }
                VD->USED   = 1;
                if (TYPE == TYPE_STRING) {
                    VD->TYPE = VARIABLE_STRING;
                    VD->nValue = 0;
                } else {
                    VD->TYPE   = VARIABLE_NUMBER;
                    VD->nValue = VD->value.ToFloat();
                }
                VD->BY_REF = 2;

                VDList->Add(VD, DATA_VAR_DESCRIPTOR);
                _ID = VDList->Count();
                if (sPARSE.Length() < 0x100)
                    HashTable_add(&NumberConstantMap, sPARSE.c_str(), _ID, sPARSE.Length());
            }
            TYPE        = TYPE_VARIABLE;
            IS_CONSTANT = 1;
        } else {
            IS_CONSTANT = 0;
        }
#ifndef NO_BUILTIN_REGEX
        // regexp support
        if (TYPE == TYPE_REGEX) {
            AnalizerElement *AE = new AnalizerElement;
            AE->ID               = KEY_P_OPEN;
            AE->TYPE             = TYPE_OPERATOR;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = "(";
            AE->_HASH_DATA       = 0;
            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

            AE                   = new AnalizerElement;
            AE->ID               = KEY_NEW;
            AE->TYPE             = TYPE_OPERATOR;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = "new";
            AE->_HASH_DATA       = 0;
            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

            _ID = ClassExists("RegExp");
            if (!_ID) {
                BUILTINOBJECTS(this, "RegExp");
                _ID = ClassExists("RegExp");
            }
            if (!_ID) {
                AnsiString regexpClass("RegExp");
                Warning(WRN10007, on_line ? on_line : P->LastLine(), 10007, regexpClass, FileName, CC ? CC->NAME.c_str() : "", CM ? CM->NAME : "");

                ClassCode *CC1 = new ClassCode(regexpClass, this);
                CC1->DEFINED_LEVEL = this->INCLUDE_LEVEL;
                CC1->NEEDED        = -1;
                ClassList->Add(CC1, DATA_CLASS_CODE);
                _ID = ClassList->Count();
#ifdef CACHED_CLASSES
                HashTable_add(&this->CachedClasses, CC1->NAME.c_str(), _ID, CC1->NAME.Length());
#endif
                AddUndefinedClass(regexpClass, CC->NAME, CM->NAME, on_line ? on_line : P->LastLine());
            }

            AE                   = new AnalizerElement;
            AE->ID               = _ID;
            AE->TYPE             = TYPE_CLASS;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = "RegExp";
            AE->_HASH_DATA       = 0;
            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

            AE                   = new AnalizerElement;
            AE->ID               = KEY_P_OPEN;
            AE->TYPE             = TYPE_OPERATOR;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = "(";
            AE->_HASH_DATA       = 0;
            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

            VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
            VD->value.LoadBuffer(sPARSE.c_str() + 1, sPARSE.Length() - 2);
            VD->USED   = 1;
            VD->TYPE   = VARIABLE_STRING;
            VD->nValue = 0;
            VD->BY_REF = 0;
            VDList->Add(VD, DATA_VAR_DESCRIPTOR);

            const char *expr = VD->value.c_str();

            AE                   = new AnalizerElement;
            AE->ID               = VDList->Count();
            AE->TYPE             = TYPE_VARIABLE;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = sPARSE;
            AE->_HASH_DATA       = 0;
            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

            if (P->regexp_flags) {
                AE                   = new AnalizerElement;
                AE->ID               = KEY_COMMA;
                AE->TYPE             = TYPE_OPERATOR;
                AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                AE->_INFO_OPTIMIZED  = 0;
                AE->_PARSE_DATA      = ",";
                AE->_HASH_DATA       = 0;
                PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
                VD->USED   = 1;
                VD->TYPE = VARIABLE_NUMBER;
                VD->nValue = P->regexp_flags;
                VD->BY_REF = 0;
                VDList->Add(VD, DATA_VAR_DESCRIPTOR);

                AE                   = new AnalizerElement;
                AE->ID               = VDList->Count();
                AE->TYPE             = TYPE_VARIABLE;
                AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                AE->_INFO_OPTIMIZED  = 0;
                AE->_HASH_DATA       = 0;
                PIFList->Add(AE, DATA_ANALIZER_ELEMENT);
            }

            AE                   = new AnalizerElement;
            AE->ID               = KEY_P_CLOSE;
            AE->TYPE             = TYPE_OPERATOR;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = ")";
            AE->_HASH_DATA       = 0;
            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);


            AE = new AnalizerElement;
            AE->ID               = KEY_P_CLOSE;
            AE->TYPE             = TYPE_OPERATOR;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = ")";
            AE->_HASH_DATA       = 0;
            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

            const char *errorp = NULL;
            Reprog *reg = JS_regcomp(expr, P->regexp_flags, &errorp);
            if (reg)
                JS_regfree(reg);
            if (errorp)
                Errors.Add(new AnsiException(ERR1340, on_line ? on_line : P->LastLine(), 1340, errorp, FileName, CC->NAME), DATA_EXCEPTION);
            continue;
        }
#endif

        if (!_ID) {
            _ID = GetID(sPARSE);

            if (TYPE == TYPE_KEYWORD) {
                if (_ID == KEY_BEGIN) {
                    if ((PREC_TYPE == TYPE_OPERATOR) && (PREC_ID != KEY_P_CLOSE)) {
                        int inline_ref_id = this->BuildFunction(CC, P, P->LastLine(), ACCESS_PUBLIC, ++inline_count, STATIC, CM->NAME, 1);
                        // add this
                        AnalizerElement *AE = new AnalizerElement;
                        AE->ID               = 1;
                        AE->TYPE             = TYPE_VARIABLE;
                        AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                        AE->_INFO_OPTIMIZED  = 0;
                        AE->_PARSE_DATA      = "this";
                        AE->_HASH_DATA       = 0;
                        PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                        AE                   = new AnalizerElement;
                        AE->ID               = KEY_SEL;
                        AE->TYPE             = TYPE_OPERATOR;
                        AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                        AE->_INFO_OPTIMIZED  = 0;
                        AE->_PARSE_DATA      = ".";
                        AE->_HASH_DATA       = 0;
                        PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                        _ID  = inline_ref_id;
                        TYPE = TYPE_METHOD;

                        AE                   = new AnalizerElement;
                        AE->ID               = _ID;
                        AE->TYPE             = TYPE_METHOD;
                        AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                        AE->_INFO_OPTIMIZED  = 0;
                        AE->_PARSE_DATA      = "inline";
                        AE->_HASH_DATA       = 0;
                        PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                        PREC_2_TYPE = TYPE_VARIABLE;
                        PREC_2_ID = 1;
                        PREC_TYPE = TYPE_OPERATOR;
                        PREC_ID = KEY_SEL;
                        continue;
                    }
                    if (ATOMIC_LEVEL) {
                        Errors.Add(new AnsiException(ERR1290, on_line ? on_line : P->LastLine(), 1290, sPARSE, FileName, CM->NAME), DATA_EXCEPTION);
                        ATOMIC_LEVEL = 0;
                    }
                    BEGIN_END_LEVEL++;
                } else if (_ID == KEY_END) {
                    if (ATOMIC_LEVEL) {
                        Errors.Add(new AnsiException(ERR1290, on_line ? on_line : P->LastLine(), 1290, sPARSE, FileName, CM->NAME), DATA_EXCEPTION);
                        ATOMIC_LEVEL = 0;
                    }
                    BEGIN_END_LEVEL--;
                }
            }

            if ((TYPE == TYPE_METHOD) && (_ID)) {
                TYPE = TYPE_KEYWORD;
                if (_ID == KEY_FUNCTION) {
                    AnsiString temp_name(CM->NAME);
                    temp_name += "#";
                    temp_name += AnsiString((intptr_t)++inline_count);
                    int inline_ref_id = this->BuildFunction(CC, P,  0, ACCESS_PUBLIC, 0, STATIC, temp_name.c_str());
                    // add this
                    AnalizerElement *AE = new AnalizerElement;
                    AE->ID               = 1;
                    AE->TYPE             = TYPE_VARIABLE;
                    AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                    AE->_INFO_OPTIMIZED  = 0;
                    AE->_PARSE_DATA      = "this";
                    AE->_HASH_DATA       = 0;
                    PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                    AE                   = new AnalizerElement;
                    AE->ID               = KEY_SEL;
                    AE->TYPE             = TYPE_OPERATOR;
                    AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                    AE->_INFO_OPTIMIZED  = 0;
                    AE->_PARSE_DATA      = ".";
                    AE->_HASH_DATA       = 0;
                    PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                    _ID  = inline_ref_id;
                    TYPE = TYPE_METHOD;

                    AE                   = new AnalizerElement;
                    AE->ID               = _ID;
                    AE->TYPE             = TYPE_METHOD;
                    AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                    AE->_INFO_OPTIMIZED  = 0;
                    AE->_PARSE_DATA      = "inline";
                    AE->_HASH_DATA       = 0;
                    PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                    PREC_2_TYPE = TYPE_VARIABLE;
                    PREC_2_ID = 1;
                    PREC_TYPE = TYPE_OPERATOR;
                    PREC_ID = KEY_SEL;
                    continue;
                }
            }
        }

        if (TYPE == TYPE_KEYWORD) {
            if (((_ID >= KEY_CLASS) && (_ID <= KEY_IMPORT)) || ((_ID >= KEY_OPERATOR) && (_ID <= KEY_STATIC))) {
                Errors.Add(new AnsiException(ERR1270, on_line ? on_line : P->LastLine(), 1270, sPARSE, FileName, CM->NAME), DATA_EXCEPTION);
                HashTable_deinit(&NumberConstantMap);
#ifdef CACHED_VARIABLES
                HashTable_deinit(&CachedVariables);
#endif
                MemoryTable_deinit(&SelectorCacheList);
                return ref_id;
            } else
            if (_ID == KEY_SUPER) {
                ClassCode *PARENT = (ClassCode *)CC->first_parent;
                if (PARENT) {
                    TYPE   = TYPE_METHOD;
                    _ID    = 0;
                    sPARSE = PARENT->NAME.c_str();
                } else {
                    Errors.Add(new AnsiException(ERR980, on_line ? on_line : P->LastLine(), 980, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                    continue;
                }
            } else
            if (_ID == KEY_PRAGMA) {
                BuildPragma(P, CC);
                continue;
            }
        }

        if ((TYPE == TYPE_OPERATOR) && (PREC_IS_CONSTANT) && (GetPriority(_ID) == OPERATOR_LEVEL_8)) {
            Errors.Add(new AnsiException(ERR760, on_line ? on_line : P->LastLine(), 760, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        }
        if (TYPE == TYPE_OPERATOR) {
            if (_ID == KEY_DLL_CALL) {
                int cnt = PIFList->Count() - 1;
                AnalizerElement *AE_PREC = (AnalizerElement *)PIFList->Item(cnt);
                if ((AE_PREC) && (AE_PREC->TYPE == TYPE_METHOD)) {
                    AnalizerElement *AE_PREC2 = (AnalizerElement *)PIFList->Item(cnt - 2);
                    AnalizerElement *AE_PREC1 = (AnalizerElement *)PIFList->Item(cnt - 1);
                    if ((AE_PREC2->TYPE == TYPE_VARIABLE) && (AE_PREC2->ID == 1) &&
                        (AE_PREC1->TYPE == TYPE_OPERATOR) && (AE_PREC1->ID == KEY_SEL)) {
                        PREC_ID = ClassExists(AE_PREC->_PARSE_DATA.c_str());
                        if (PREC_ID) {
                            AE_PREC->ID   = PREC_ID;
                            AE_PREC->TYPE = TYPE_CLASS;
                            PREC_TYPE     = TYPE_CLASS;
                            int cnt = PIFList->Count() - 3;
                            PIFList->Delete(cnt);
                            PIFList->Delete(cnt);
                        }
                    }
                }
            }

            if ((PREC_TYPE == TYPE_OPERATOR) && (PREC_ID == KEY_NEW)) {
                if (_ID == KEY_SELECTOR_I) {
                    _ID  = STATIC_CLASS_ARRAY;
                    TYPE = TYPE_CLASS;
                } else {
                    _ID = 0;
                }
            } else
            if ((PREC_TYPE == TYPE_CLASS) && (_ID == KEY_SEL)) {
                _ID = KEY_DLL_CALL;
            }
        }

        if ((!_ID) && (PREC_TYPE == TYPE_OPERATOR) && (PREC_ID == KEY_NEW)) {
            _ID = ClassExists(sPARSE.c_str());

            if (!_ID) {
                if (BUILTINOBJECTS(this, sPARSE.c_str())) {
                    _ID = ClassExists(sPARSE.c_str());
                }
            }
            if (!_ID) {
                Warning(WRN10007, on_line ? on_line : P->LastLine(), 10007, sPARSE, FileName, CC ? CC->NAME.c_str() : "", CM ? CM->NAME : "");

                ClassCode *CC1 = new ClassCode(sPARSE, this);
                CC1->DEFINED_LEVEL = this->INCLUDE_LEVEL;
                CC1->NEEDED        = -1;
                ClassList->Add(CC1, DATA_CLASS_CODE);
                _ID = ClassList->Count();
#ifdef CACHED_CLASSES
                HashTable_add(&this->CachedClasses, CC1->NAME.c_str(), _ID, CC1->NAME.Length());
#endif
                AddUndefinedClass(sPARSE, CC->NAME, CM->NAME, on_line ? on_line : P->LastLine());
            }
            TYPE = TYPE_CLASS;
        }
        if ((!_ID) &&
            (TYPE == TYPE_METHOD) &&
            (PREC_TYPE == TYPE_OPERATOR) &&
            ((PREC_ID == KEY_SEL) || (PREC_ID == KEY_DLL_CALL))) {
            //------ NOU !!! --------------------------------------------//
            if (PREC_ID == KEY_DLL_CALL) {
                INTEGER         cnt = PIFList->Count();
                AnalizerElement *AE = (AnalizerElement *)PIFList->Item(cnt - 2);
                if ((AE) && (AE->TYPE == TYPE_CLASS) && (AE->ID > 0)) {
                    ClassCode *CC_temp = (ClassCode *)ClassList->Item(AE->ID - 1);
                    if (CC_temp) {
                        AnsiString temp_mname(CC_temp->NAME.c_str());
                        temp_mname += "@";
                        temp_mname += sPARSE;
                        _ID         = CC->HasMember(temp_mname);
                        if (_ID) {
                            PIFList->Delete(cnt - 2);
                            PIFList->Delete(cnt - 2);
                            sPARSE = temp_mname;
                            AnalizerElement *AE = new AnalizerElement;
                            AE->ID               = 1;
                            AE->TYPE             = TYPE_VARIABLE;
                            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                            AE->_INFO_OPTIMIZED  = 0;
                            AE->_PARSE_DATA      = "this";
                            AE->_HASH_DATA       = 0;
                            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);
                            VD_THIS->USED = 1;

                            AE                   = new AnalizerElement;
                            AE->ID               = KEY_SEL;
                            AE->TYPE             = TYPE_OPERATOR;
                            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                            AE->_INFO_OPTIMIZED  = 0;
                            AE->_PARSE_DATA      = ".";
                            AE->_HASH_DATA       = 0;
                            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                            PREC_2_TYPE = TYPE_VARIABLE;
                            PREC_2_ID = 1;
                            PREC_TYPE = TYPE_OPERATOR;
                            PREC_ID = KEY_SEL;
                        }
                    }
                }
            }
            //-----------------------------------------------------------//
            _ID = GeneralMembers->ContainsString(sPARSE.c_str());
            if (!_ID) {
                if (PREC_ID == KEY_DLL_CALL) {
                    INTEGER cnt = PIFList->Count();
                    if (cnt >= 2) {
                        AnalizerElement *AE = (AnalizerElement *)PIFList->Item(cnt - 2);
                        if ((AE->TYPE == TYPE_CLASS) && (AE->ID > 0)) {
                            ClassCode *CC_T = (ClassCode *)ClassList->Item(AE->ID - 1);
                            if ((CC_T->NEEDED > -1) && (CC_T != CC)) {
                                cnt -= 2;
                                PIFList->Delete(cnt);
                                PIFList->Delete(cnt);

                                AnalizerElement *AE = new AnalizerElement;
                                AE->ID               = 1;
                                AE->TYPE             = TYPE_VARIABLE;
                                AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                                AE->_INFO_OPTIMIZED  = 0;
                                AE->_PARSE_DATA      = "this";
                                AE->_HASH_DATA       = 0;
                                PIFList->Add(AE, DATA_ANALIZER_ELEMENT);
                                VD_THIS->USED = 1;

                                AE                   = new AnalizerElement;
                                AE->ID               = KEY_SEL;
                                AE->TYPE             = TYPE_OPERATOR;
                                AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                                AE->_INFO_OPTIMIZED  = 0;
                                AE->_PARSE_DATA      = ".";
                                AE->_HASH_DATA       = 0;
                                PIFList->Add(AE, DATA_ANALIZER_ELEMENT);
                                if (sPARSE == C_FINALIZE) {
                                    sPARSE  = CC_T->NAME.c_str();
                                    sPARSE += "@";
                                    sPARSE += C_FINALIZE;
                                }
                                _ID = GeneralMembers->ContainsString(sPARSE.c_str());

                                PREC_2_TYPE = 0;
                                PREC_2_ID = 0;
                                PREC_TYPE = TYPE_OPERATOR;
                                PREC_ID = KEY_SEL;
                            }
                        }
                    }
                }
                if (!_ID) {
                    _ID = AddUndefinedMember(sPARSE, CC->NAME, CM->NAME, on_line ? on_line : P->LastLine());
                }
            }
        }

        if ((!_ID) && (TYPE == TYPE_METHOD)) {
            _ID = LinkStatic(sPARSE.c_str());
            if (_ID) {
                AnalizerElement *AE = new AnalizerElement;
                AE->ID               = STATIC_CLASS_DLL;
                AE->TYPE             = TYPE_CLASS;
                AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                AE->_INFO_OPTIMIZED  = 0;
                AE->_PARSE_DATA      = "LIBRARY";
                AE->_HASH_DATA       = 0;
                PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                AE                   = new AnalizerElement;
                AE->ID               = KEY_DLL_CALL;
                AE->TYPE             = TYPE_OPERATOR;
                AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                AE->_INFO_OPTIMIZED  = 0;
                AE->_PARSE_DATA      = "::";
                AE->_HASH_DATA       = 0;
                PIFList->Add(AE, DATA_ANALIZER_ELEMENT);
            }
        }
        if ((!STATIC) && (!_ID) && (TYPE == TYPE_METHOD)) {
            is_cached = false;
            if (sPARSE == cachedMember)
                is_cached = true;
            if ((is_cached) || (CC->HasMember(sPARSE))) {
                if (!is_cached)
                    cachedMember = sPARSE;
                if (sPARSE == CC->NAME.c_str()) {
                    P->NextAtom(cached);
                    if ((cached == ".") || (cached == "->") || (cached == "::")) {
                        _ID = 0;
                    } else {
                        _ID = GeneralMembers->ContainsString(sPARSE.c_str());
                    }
                } else {
                    P->NextAtom(cached);
                    if (cached != "::") {
                        _ID = GeneralMembers->ContainsString(sPARSE.c_str());
                    }
                }

                if (_ID) {
                    if (STATIC) {
                        Errors.Add(new AnsiException(ERR930, on_line ? on_line : P->LastLine(), 930, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                        continue;
                    }
                    AnalizerElement *AE = new AnalizerElement;
                    AE->ID               = 1;
                    AE->TYPE             = TYPE_VARIABLE;
                    AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                    AE->_INFO_OPTIMIZED  = 0;
                    AE->_PARSE_DATA      = "this";
                    AE->_HASH_DATA       = 0;
                    PIFList->Add(AE, DATA_ANALIZER_ELEMENT);
                    VD_THIS->USED = 1;

                    AE                   = new AnalizerElement;
                    AE->ID               = KEY_SEL;
                    AE->TYPE             = TYPE_OPERATOR;
                    AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
                    AE->_INFO_OPTIMIZED  = 0;
                    AE->_PARSE_DATA      = ".";
                    AE->_HASH_DATA       = 0;
                    PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

                    PREC_2_TYPE = TYPE_VARIABLE;
                    PREC_2_ID = 1;

                    PREC_TYPE = TYPE_OPERATOR;
                    PREC_ID = KEY_SEL;
                }
            }
        } else
        if ((STATIC) && (!_ID) && (TYPE == TYPE_METHOD)) {
            is_cached = false;
            if (sPARSE == cachedMember)
                is_cached = true;
            if ((is_cached) || (CC->HasMember(sPARSE))) {
                if (!is_cached)
                    cachedMember = sPARSE;
                if (!cached.Length())
                    P->NextAtom(cached);

                if ((cached == ".") || (cached == "->") || (cached == "::")) {
                } else {
                    _ID = GeneralMembers->ContainsString(sPARSE.c_str());

                    if (_ID) {
                        if (STATIC) {
                            Errors.Add(new AnsiException(ERR930, on_line ? on_line : P->LastLine(), 930, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                            continue;
                        }
                    }
                }
            }
        }

        if (!_ID) {
            _ID = ClassExists(sPARSE.c_str());
            if (_ID)
                TYPE = TYPE_CLASS;
        }

        if (!_ID) {
            if ((USE_IMPLICIT) && (TYPE == TYPE_METHOD)) {
                TYPE = TYPE_VARIABLE;
                VariableDESCRIPTOR *VD = new VariableDESCRIPTOR;
                VD->name   = sPARSE;
                VD->TYPE   = VARIABLE_NUMBER;
                VD->USED   = 0;
                VD->nValue = 0;
                VD->BY_REF = 0;
                if (IS_ARRAY) {
                    VD->TYPE = VARIABLE_ARRAY;
                }

                VDList->Add(VD, DATA_VAR_DESCRIPTOR);
                _ID = VDList->Count();
#ifdef CACHED_VARIABLES
                HashTable_add(&CachedVariables, sPARSE.c_str(), _ID, sPARSE.Length());
#endif
                IS_ARRAY = 0;
            } else {
                if (!cached.Length())
                    P->NextAtom(cached);

                if ((cached == ".") || (cached == "::")) {
                    TYPE = TYPE_CLASS;
                    if (BUILTINOBJECTS(this, sPARSE.c_str())) {
                        _ID = ClassExists(sPARSE.c_str());
                    } else {
                        Warning(WRN10007, on_line ? on_line : P->LastLine(), 10007, sPARSE, FileName, CC ? CC->NAME.c_str() : "", CM ? CM->NAME : "");

                        ClassCode *CC1 = new ClassCode(sPARSE, this);
                        CC1->DEFINED_LEVEL = this->INCLUDE_LEVEL;
                        CC1->NEEDED        = -1;
                        ClassList->Add(CC1, DATA_CLASS_CODE);
                        _ID = ClassList->Count();
                        AddUndefinedClass(sPARSE, CC->NAME, CM->NAME, on_line ? on_line : P->LastLine());
#ifdef CACHED_CLASSES
                        HashTable_add(&this->CachedClasses, sPARSE.c_str(), _ID, sPARSE.Length());
#endif
                    }
                } else {
                    Errors.Add(new AnsiException(ERR010, on_line ? on_line : P->LastLine(), 10, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                    continue;
                }
            }
        }

        if ((TYPE == TYPE_OPERATOR) && ((PREC_TYPE == TYPE_VARIABLE) || (PREC_TYPE == TYPE_METHOD) || ((PREC_TYPE == TYPE_OPERATOR) && (PREC_ID == KEY_INDEX_CLOSE)))) {
            if (_ID == KEY_INC) {
                _ID = KEY_INC_LEFT;
                if (PREC_IS_CONSTANT) {
                    Errors.Add(new AnsiException(ERR761, on_line ? on_line : P->LastLine(), 761, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                }
            } else
            if (_ID == KEY_DEC) {
                _ID = KEY_DEC_LEFT;
                if (PREC_IS_CONSTANT) {
                    Errors.Add(new AnsiException(ERR762, on_line ? on_line : P->LastLine(), 762, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                }
            }
        }
        if ((IS_CONSTANT) && (PREC_TYPE == TYPE_OPERATOR)) {
            if (PREC_ID == KEY_INC) {
                Errors.Add(new AnsiException(ERR761, on_line ? on_line : P->LastLine(), 761, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
            } else
            if (PREC_ID == KEY_DEC) {
                Errors.Add(new AnsiException(ERR762, on_line ? on_line : P->LastLine(), 762, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
            }
        }
        PREC_IS_CONSTANT = IS_CONSTANT;

        if ((TYPE == TYPE_OPERATOR) && (_ID == KEY_P_OPEN) && (PREC_TYPE == TYPE_VARIABLE)) {
            INTEGER pos = PIFList->Count();

            AnalizerElement *AE = new AnalizerElement;
            AE->ID               = STATIC_CLASS_DELEGATE;
            AE->TYPE             = TYPE_CLASS;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = "DELEGATE";
            AE->_HASH_DATA       = 0;
            PIFList->Insert(AE, pos - 1, DATA_ANALIZER_ELEMENT);

            AE                   = new AnalizerElement;
            AE->ID               = KEY_DLL_CALL;
            AE->TYPE             = TYPE_OPERATOR;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = "::";
            AE->_HASH_DATA       = 0;
            PIFList->Insert(AE, pos, DATA_ANALIZER_ELEMENT);
        }

        if ((TYPE == TYPE_OPERATOR) && ((PREC_TYPE == TYPE_OPERATOR) || (PREC_TYPE == TYPE_KEYWORD)) && (PREC_ID != KEY_P_CLOSE) && (PREC_ID != KEY_INDEX_CLOSE)) {
            if (_ID == KEY_SUM) {
                _ID = KEY_POZ;
            } else
            if (_ID == KEY_DIF) {
                _ID = KEY_NEG;
            }
        }

        if ((TYPE != TYPE_KEYWORD) || (_ID != KEY_VAR)) {
            AnalizerElement *AE = new AnalizerElement;
            AE->ID               = _ID;
            AE->TYPE             = TYPE;
            AE->_DEBUG_INFO_LINE = on_line ? on_line : P->LastLine();
            AE->_INFO_OPTIMIZED  = 0;
            AE->_PARSE_DATA      = sPARSE;
            AE->_HASH_DATA       = 0;
            PIFList->Add(AE, DATA_ANALIZER_ELEMENT);

            if (TYPE == TYPE_OPERATOR) {
                switch (_ID) {
                    case KEY_P_OPEN:
                    case KEY_INDEX_OPEN:
                        ATOMIC_LEVEL++;
                        break;

                    case KEY_P_CLOSE:
                    case KEY_INDEX_CLOSE:
                        ATOMIC_LEVEL--;
                        break;
                }
            }

            if ((IN_VAR_STATAMENT) && (!ATOMIC_LEVEL)) {
                if ((TYPE == TYPE_SEPARATOR) && (_ID == KEY_SEP)) {
                    IN_VAR_STATAMENT = 0;
                }
            }
            if ((_ID) && (TYPE == TYPE_METHOD) && (PREC_TYPE == TYPE_OPERATOR) && (PREC_ID == KEY_SEL) && ((PREC_2_TYPE == TYPE_VARIABLE) || (PREC_2_TYPE == TYPE_METHOD)) && (PREC_2_ID >= 1)) {
#if __SIZEOF_POINTER__ != 8
                if ((_ID < 0x7FFF) && (PREC_2_ID < 0x7FFF)) {
#endif
                    if (!cached.Length())
                        P->NextAtom(cached);    
                    if (cached != P_OPEN) {
#if __SIZEOF_POINTER__ == 8
                        uintptr_t key = ((uintptr_t)_ID << 32) | (PREC_2_ID);
#else
                        uintptr_t key = (_ID << 16) | (PREC_2_ID);
#endif
                        AnalizerElement *AE_OLD = (AnalizerElement *)MemoryTable_find(&SelectorCacheList, (void *)(intptr_t)key);
                        if (AE_OLD) {
                            // notify usage
                            AE->_INFO_OPTIMIZED = 2;
                            AE_OLD->_INFO_OPTIMIZED = 2;
                        } else
                            MemoryTable_add(&SelectorCacheList, (void *)(intptr_t)key, (intptr_t)AE);
                    }
#if __SIZEOF_POINTER__ != 8
                }
#endif
            }
        } else {
            IN_VAR_STATAMENT = 1;
        }
    } while (BEGIN_END_LEVEL);

    for (int i = 0; i < VDList->Count(); i++) {
        VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)VDList->Item(i);
        if (!VD->USED) {
            if (OPERATOR) {
                Warning3(WRN10003, on_line ? on_line : P->LastLine(), 10003, VD->name.c_str(), " in operator ", CM->NAME, CC ? CC->NAME.c_str() : "", CM ? CM->NAME : "");
            } else {
                Warning3(WRN10003, on_line ? on_line : P->LastLine(), 10003, VD->name.c_str(), " in function ", CM->NAME, CC ? CC->NAME.c_str() : "", CM ? CM->NAME : "");
            }
        }
    }
    HashTable_deinit(&NumberConstantMap);
#ifdef CACHED_VARIABLES
    HashTable_deinit(&CachedVariables);
#endif
    MemoryTable_deinit(&SelectorCacheList);
    return ref_id;
}

INTEGER PIFAlizator::BuildOverride(ClassCode *CC, AnsiParser *P, INTEGER on_line) {
    INTEGER    TYPE = -1;
    INTEGER    _ID  = -1;
    AnsiString sPARSE;

    P->NextAtom(sPARSE);

    TYPE = GetType(sPARSE);
    _ID  = GetID(sPARSE);

    if ((_ID) || (TYPE != TYPE_METHOD)) {
        Errors.Add(new AnsiException(ERR010, on_line ? on_line : P->LastLine(), 010, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
    } else {
        CC->RemoveMember(this, sPARSE, on_line ? on_line : P->LastLine(), FileName);
    }
    return 0;
}

INTEGER PIFAlizator::BuildProperty(ClassCode *CC, AnsiParser *P, INTEGER on_line, INTEGER ACCESS) {
    INTEGER    TYPE = -1;
    INTEGER    _ID  = -1;
    AnsiString sPARSE;

    P->NextAtom(sPARSE);

    int  ref_id    = GeneralMembers->ContainsString(sPARSE.c_str());
    char *ref_name = 0;
    if (!ref_id) {
        AddGeneralMember(sPARSE);
        ref_id = GeneralMembers->Count();
    } else {
        DefineMember(sPARSE);
    }
    ref_name = GeneralMembers->Item(ref_id - 1);

    ClassMember *CM = CC->AddMember(this, ref_name, on_line ? on_line : P->LastLine(), FileName, ACCESS, true);
    CM->IS_FUNCTION = 3;

    TYPE = GetType(sPARSE);
    _ID  = GetID(sPARSE);

    if ((_ID) || (TYPE != TYPE_METHOD)) {
        Errors.Add(new AnsiException(ERR010, on_line ? on_line : P->LastLine(), 010, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
    }

    P->NextAtom(sPARSE);

    TYPE = GetType(sPARSE);
    _ID  = GetID(sPARSE);

    if ((TYPE != TYPE_KEYWORD) && (_ID != KEY_BEGIN)) {
        Errors.Add(new AnsiException(ERR350, on_line ? on_line : P->LastLine(), 350, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        return 1;
    }

    INTEGER SET_MEMBER;
    INTEGER _OUT    = 0;
    INTEGER HAS_GET = 0;
    INTEGER HAS_SET = 0;
    do {
        P->NextAtom(sPARSE);

        TYPE = GetType(sPARSE);
        _ID  = GetID(sPARSE);

        if (((_ID != KEY_SET) && (_ID != KEY_GET)) || (TYPE != TYPE_METHOD)) {
            Errors.Add(new AnsiException(ERR380, on_line ? on_line : P->LastLine(), 380, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        } else {
            if (_ID == KEY_SET) {
                if (HAS_SET) {
                    Errors.Add(new AnsiException(ERR740, on_line ? on_line : P->LastLine(), 740, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                } else {
                    HAS_SET = 1;
                }
            } else
            if (_ID == KEY_GET) {
                if (HAS_GET) {
                    Errors.Add(new AnsiException(ERR730, on_line ? on_line : P->LastLine(), 730, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
                } else {
                    HAS_GET = 1;
                }
            }
        }

        SET_MEMBER = (_ID == KEY_SET);

        P->NextAtom(sPARSE);

        TYPE = GetType(sPARSE);
        _ID  = GetID(sPARSE);

        if ((_ID) || (TYPE != TYPE_METHOD)) {
            Errors.Add(new AnsiException(ERR010, on_line ? on_line : P->LastLine(), 010, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        }

        INTEGER POSITION = GeneralMembers->ContainsString(sPARSE.c_str());
        if (!POSITION) {
            AddUndefinedMember(sPARSE, CC->NAME, CM->NAME, on_line ? on_line : P->LastLine());
            POSITION = GeneralMembers->Count();
        }

        SET_MEMBER  ?   CM->MEMBER_SET = POSITION    :  CM->MEMBER_GET = POSITION;

        P->NextAtom(sPARSE);

        TYPE = GetType(sPARSE);
        _ID  = GetID(sPARSE);

        if ((TYPE == TYPE_OPERATOR) && (_ID == KEY_COMMA)) {
            _OUT = 0;
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_END)) {
            _OUT = 1;
        } else {
            Errors.Add(new AnsiException(ERR360, on_line ? on_line : P->LastLine(), 360, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
            return 1;
        }
    } while (!_OUT);
    if ((HAS_SET) && (!HAS_GET)) {
        Errors.Add(new AnsiException(ERR720, on_line ? on_line : P->LastLine(), 720, NULL_STRING, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        return 1;
    }

    return 0;
}

#ifndef DISABLE_DEPRECATED_FEATURES
INTEGER PIFAlizator::BuildEvent(ClassCode *CC, AnsiParser *P, INTEGER on_line, INTEGER ACCESS) {
    INTEGER    TYPE = -1;
    INTEGER    _ID  = -1;
    AnsiString sPARSE;

    P->NextAtom(sPARSE);

    int  ref_id    = GeneralMembers->ContainsString(sPARSE.c_str());
    char *ref_name = 0;
    if (!ref_id) {
        AddGeneralMember(sPARSE);
        ref_id = GeneralMembers->Count();
    } else {
        DefineMember(sPARSE);
    }
    ref_name = GeneralMembers->Item(ref_id - 1);

    ClassMember *CM = CC->AddMember(this, ref_name, on_line ? on_line : P->LastLine(), FileName, ACCESS, true);
    CM->IS_FUNCTION = 2;

    TYPE = GetType(sPARSE);
    _ID  = GetID(sPARSE);

    if ((_ID) || (TYPE != TYPE_METHOD)) {
        Errors.Add(new AnsiException(ERR010, on_line ? on_line : P->LastLine(), 010, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
    }

    P->NextAtom(sPARSE);

    // faster
    if (sPARSE != TRIGERS) {
        Errors.Add(new AnsiException(ERR330, on_line ? on_line : P->LastLine(), 330, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        return 1;
    }

    P->NextAtom(sPARSE);

    TYPE = GetType(sPARSE);
    _ID  = GetID(sPARSE);

    if ((_ID) || (TYPE != TYPE_METHOD)) {
        Errors.Add(new AnsiException(ERR010, on_line ? on_line : P->LastLine(), 010, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
    }

    INTEGER POSITION = GeneralMembers->ContainsString(sPARSE.c_str());
    if (!POSITION) {
        AddGeneralMember(sPARSE);
        POSITION = GeneralMembers->Count();
    }
    CM->MEMBER_GET = POSITION;

    P->NextAtom(sPARSE);
#ifdef OPTIONAL_SEPARATOR
    if (STRICT_MODE) {
        TYPE = GetType(sPARSE);
        _ID  = GetID(sPARSE);

        if ((TYPE != TYPE_SEPARATOR) && (_ID != KEY_SEP)) {
            Errors.Add(new AnsiException(ERR340, on_line ? on_line : P->LastLine(), 340, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
            return 1;
        }
    } else {
        if (sPARSE != SEP)
            P->Rollback();
    }
#else

    TYPE = GetType(sPARSE);
    _ID  = GetID(sPARSE);

    if ((TYPE != TYPE_SEPARATOR) && (_ID != KEY_SEP)) {
        Errors.Add(new AnsiException(ERR340, on_line ? on_line : P->LastLine(), 340, sPARSE, FileName, CC->NAME, CM->NAME), DATA_EXCEPTION);
        return 1;
    }
#endif
    return 0;
}
#endif

INTEGER PIFAlizator::ExtendClass(ClassCode *CC, AnsiParser *P, INTEGER on_line, INTEGER OWNER_CLSID) {
    AnsiString sPARSE;

    P->NextAtom(sPARSE);

    INTEGER TYPE = GetType(sPARSE);
    INTEGER _ID  = GetID(sPARSE);

    if ((_ID) || (TYPE != TYPE_METHOD)) {
        Errors.Add(new AnsiException(ERR170, on_line ? on_line : P->LastLine(), 170, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
        return 0;
    }

    _ID = ClassExists(sPARSE.c_str());

    if (!_ID) {
        Errors.Add(new AnsiException(ERR180, on_line ? on_line : P->LastLine(), 180, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
        return 0;
    }
    ClassCode *PARENT = (ClassCode *)(*ClassList) [_ID - 1];

    if (PARENT == CC) {
        Errors.Add(new AnsiException(ERR1240, on_line ? on_line : P->LastLine(), 1240, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
        return 0;
    }

    if (_ID > OWNER_CLSID) {
        Errors.Add(new AnsiException(ERR1230, on_line ? on_line : P->LastLine(), 1230, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
        return 0;
    }

    if (!CC->first_parent) {
        CC->first_parent = PARENT;
    }

    if (!PARENT->Members) {
        PARENT->Members = new AnsiList();
        for (INTEGER i = 0; i < PARENT->pMEMBERS_COUNT; i++) {
            ClassMember *CM = PARENT->pMEMBERS[i];
            if (CM)
                PARENT->Members->Add(CM, DATA_CLASS_MEMBER);
        }
    }
    INTEGER MemberCount = PARENT->Members->Count();
    for (INTEGER i = 0; i < MemberCount; i++) {
        ClassMember *CM = (ClassMember *)PARENT->Members->Item(i);
        CC->AddLinked(this, CM);
    }
    return 0;
}

INTEGER PIFAlizator::BuildClass(AnsiParser *P, INTEGER on_line) {
    INTEGER    BEGIN_END_LEVEL = 0;
    INTEGER    TYPE            = 0;
    INTEGER    _ID             = 0;
    INTEGER    ACCESS          = ACCESS_PUBLIC;
    AnsiString sPARSE;

    P->NextAtom(sPARSE);
    INTEGER START_CLASS      = 0;
    INTEGER cls_index        = 0xFFFFFFF;
    char    STATIC           = 0;
    char    ALREADY_REPORTED = 0;
    char    ALREADY_DEFINED  = 0;

    ClassCode *IMPLICIT_PTR = (ClassCode *)ClassExists(sPARSE.c_str(), true, &cls_index);
    if ((IMPLICIT_PTR) && (IMPLICIT_PTR->NEEDED != -1)) {
        Errors.Add(new AnsiException(ERR750, on_line ? on_line : P->LastLine(), 750, sPARSE, FileName), DATA_EXCEPTION);
        IMPLICIT_PTR    = NULL;
        ALREADY_DEFINED = 1;
    }

    ClassCode *CC = IMPLICIT_PTR;

    if (!CC) {
        CC = new ClassCode(sPARSE, this);
        if (!ALREADY_DEFINED) {
            ClassList->Add(CC, DATA_CLASS_CODE);
#ifdef CACHED_CLASSES
            HashTable_add(&this->CachedClasses, sPARSE.c_str(), this->ClassList->Count(), sPARSE.Length());
#endif
        }
    } else {
        CC->NEEDED = 1;
        DefineClass(sPARSE);
    }

    CC->DEFINED_LEVEL = this->INCLUDE_LEVEL;

    if (CC->NAME == (char *)MAIN_ENTRY_POINT) {
        if (CC->NEEDED != -1) {
            CC->NEEDED = 1;
        }
        ENTRY_CLASS = ClassList->Count();
    }

    do {
        P->NextAtom(sPARSE);

        if (sPARSE == NULL_STRING) {
            Errors.Add(new AnsiException(ERR1210, on_line ? on_line : P->LastLine(), 1210, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
            break;
        }

        TYPE = GetType(sPARSE);
        _ID  = 0;

        if (TYPE == TYPE_HEX_NUMBER) {
            uint64_t TEMP_NR = 0;
            sscanf(sPARSE.c_str() + 2, "%llx", &TEMP_NR);
            sPARSE = AnsiString((double)TEMP_NR);
            TYPE   = TYPE_NUMBER;
        }

        if (!_ID) {
            _ID = GetID(sPARSE);
            if ((TYPE == TYPE_METHOD) && (_ID)) {
                TYPE = TYPE_KEYWORD;
            }

            if (TYPE == TYPE_SEPARATOR) {
                continue;
            }

            if (TYPE == TYPE_KEYWORD) {
                if (_ID == KEY_BEGIN) {
                    BEGIN_END_LEVEL++;
                    START_CLASS = 1;
                    continue;
                } else if (_ID == KEY_END) {
                    BEGIN_END_LEVEL--;
                    continue;
                }
            }
            if (_ID == KEY_PRAGMA) {
                BuildPragma(P, CC);
                continue;
            }
        }

        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_STATIC)) {
            STATIC = 1;
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_PUBLIC)) {
            ACCESS = ACCESS_PUBLIC;
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_PRIVATE)) {
            ACCESS = ACCESS_PRIVATE;
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_PROTECTED)) {
            ACCESS = ACCESS_PROTECTED;
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_EXTENDS)) {
            if (!START_CLASS) {
                ExtendClass(CC, P, on_line, cls_index);
            } else {
                Errors.Add(new AnsiException(ERR470, on_line ? on_line : P->LastLine(), 470, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
            }
            ALREADY_REPORTED = 0;
        } else
        if ((TYPE == TYPE_KEYWORD) && ((_ID == KEY_FUNCTION) || (_ID == KEY_OPERATOR))) {
            if ((STATIC) && (_ID != KEY_FUNCTION)) {
                Errors.Add(new AnsiException(ERR940, on_line ? on_line : P->LastLine(), 940, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
            }
            BuildFunction(CC, P, on_line, ACCESS, _ID == KEY_OPERATOR, (_ID != KEY_OPERATOR) ? STATIC : 0);
            ACCESS           = ACCESS_PUBLIC;
            STATIC           = 0;
            ALREADY_REPORTED = 0;
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_VAR)) {
            if (STATIC) {
                Errors.Add(new AnsiException(ERR940, on_line ? on_line : P->LastLine(), 940, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
                STATIC = 0;
            }
            BuildVariable(CC, P, on_line, ACCESS);
            ACCESS           = ACCESS_PUBLIC;
            ALREADY_REPORTED = 0;
        } else
#ifndef DISABLE_DEPRECATED_FEATURES
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_CEVENT)) {
            if (STATIC) {
                Errors.Add(new AnsiException(ERR940, on_line ? on_line : P->LastLine(), 940, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
                STATIC = 0;
            }
            BuildEvent(CC, P, on_line, ACCESS);
            ACCESS           = ACCESS_PUBLIC;
            ALREADY_REPORTED = 0;
        } else
#endif
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_PROPERTY)) {
            if (STATIC) {
                Errors.Add(new AnsiException(ERR940, on_line ? on_line : P->LastLine(), 940, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
                STATIC = 0;
            }
            BuildProperty(CC, P, on_line, ACCESS);
            ACCESS           = ACCESS_PUBLIC;
            ALREADY_REPORTED = 0;
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_OVERRIDE)) {
            if (STATIC) {
                Errors.Add(new AnsiException(ERR940, on_line ? on_line : P->LastLine(), 940, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
                STATIC = 0;
            }
            if (ACCESS != ACCESS_PUBLIC) {
                Errors.Add(new AnsiException(ERR710, on_line ? on_line : P->LastLine(), 710, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
            }
            BuildOverride(CC, P, on_line);
            ACCESS           = ACCESS_PUBLIC;
            ALREADY_REPORTED = 0;
        } else {
            if (TYPE == TYPE_METHOD) {
                BuildFunction(CC, P, on_line, ACCESS, 0, STATIC, sPARSE.c_str());
                ACCESS           = ACCESS_PUBLIC;
                STATIC           = 0;
                ALREADY_REPORTED = 0;
            } else
            if ((!ALREADY_REPORTED) || (!START_CLASS)) {
                if (TYPE == TYPE_KEYWORD) {
                    if (_ID == KEY_CLASS) {
                        Errors.Add(new AnsiException(ERR1280, on_line ? on_line : P->LastLine(), 1280, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
                        return BuildClass(P);
                    } else {
                        Errors.Add(new AnsiException(ERR430, on_line ? on_line : P->LastLine(), 430, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
                    }
                } else
                if (TYPE == TYPE_OPERATOR) {
                    Errors.Add(new AnsiException(ERR440, on_line ? on_line : P->LastLine(), 440, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
                } else {
                    Errors.Add(new AnsiException(ERR460, on_line ? on_line : P->LastLine(), 460, sPARSE, FileName, CC->NAME), DATA_EXCEPTION);
                }
                ALREADY_REPORTED = 1;
            }
            STATIC = 0;
            ACCESS = ACCESS_PUBLIC;
        }
    } while ((BEGIN_END_LEVEL) || (!START_CLASS));
    if ((ALREADY_DEFINED) && (CC))
        delete CC;
    return 0;
}

INTEGER PIFAlizator::Warning(const char *WRN, int line, int wrn_code, const char *extra, const char *filename, const char *class_name, const char *member_name) {
    if (USE_WARN) {
        Warnings.Add(new AnsiException(WRN, line, wrn_code, extra, filename ? filename : FileName.c_str(), class_name, member_name), DATA_EXCEPTION);
        return 1;
    }
    return 0;
}

INTEGER PIFAlizator::Warning3(const char *WRN, int line, int wrn_code, const char *extra1, const char *extra2, const char *extra3, const char *filename, const char *class_name, const char *member_name) {
    if (USE_WARN) {
        Warnings.Add(new AnsiException(wrn_code, WRN, line, extra1, extra2, extra3, filename ? filename : FileName.c_str(), class_name, member_name), DATA_EXCEPTION);
        return 1;
    }
    return 0;
}

INTEGER PIFAlizator::BuildPragma(AnsiParser *P, ClassCode *CC) {
    AnsiString pragma = P->GetConstant();
    AnsiParser PRAGMA_PARSER(&pragma, 0);
    AnsiString first_param;

    PRAGMA_PARSER.NextAtom(first_param, 1);

    if (first_param == PRAGMA_WARNINGS) {
        PRAGMA_PARSER.NextAtom(first_param, 1);
        if (first_param == PRAGMA_ON) {
            USE_WARN = 1;
        } else
        if (first_param == PRAGMA_OFF) {
            USE_WARN = 0;
        } else {
            Warning(WRN10006, P->LastLine(), 10006, first_param, 0, CC ? CC->NAME.c_str() : "");
        }
    } else
    if (first_param == PRAGMA_EXCEPTIONS) {
        PRAGMA_PARSER.NextAtom(first_param, 1);
        if (first_param == PRAGMA_ON) {
            USE_EXC = 1;
        } else
        if (first_param == PRAGMA_OFF) {
            USE_EXC = 0;
        } else {
            Warning(WRN10006, P->LastLine(), 10006, first_param, 0, CC ? CC->NAME.c_str() : "");
        }
    } else
    if (first_param == PRAGMA_IMPLICIT) {
        PRAGMA_PARSER.NextAtom(first_param, 1);
        if (first_param == PRAGMA_ON) {
            USE_IMPLICIT = 1;
        } else
        if (first_param == PRAGMA_OFF) {
            USE_IMPLICIT = 0;
        } else {
            Warning(WRN10006, P->LastLine(), 10006, first_param, 0, CC ? CC->NAME.c_str() : "");
        }
    } else
    if (first_param == PRAGMA_USED) {
        if ((CC) && (CC->NEEDED != -1)) {
            if (!CC->NEEDED)
                CC->UpdateNeeded();
            CC->NEEDED = 1;
        }
    } else
    if (first_param == PRAGMA_STRICT) {
        PRAGMA_PARSER.NextAtom(first_param, 1);
        if (first_param == PRAGMA_ON) {
            STRICT_MODE = 1;
        } else
        if (first_param == PRAGMA_OFF) {
            STRICT_MODE = 0;
        } else {
            Warning(WRN10006, P->LastLine(), 10006, first_param, 0, CC ? CC->NAME.c_str() : "");
        }
    } else {
        Warning(WRN10005, P->LastLine(), 10005, first_param, CC ? CC->NAME.c_str() : "");
    }

    return 0;
}

INTEGER PIFAlizator::IncludePackage(const char *filename) {
    AnsiString RELATIVE_MODULE_NAME = TEMP_INC_DIR + filename;
    AnsiString MODULE_NAME_DIR;
    RELATIVE_MODULE_NAME = this->NormalizePath(&RELATIVE_MODULE_NAME);
    // filename with no absolute path in it
    AnsiString clean_filename_dir;

    if (ListContains(RELATIVE_MODULE_NAME.c_str(), IncludedList))
        return 1;

    FILE *in = fopen(AnsiString(RELATIVE_MODULE_NAME + (char *)DEFAULT_PACK_EXTENSION).c_str(), "rb");

    if (!in) {
        MODULE_NAME_DIR = INCLUDE_DIR + filename;
        if (ListContains(filename, IncludedList)) {
            return 1;
        }

        in = fopen(AnsiString(MODULE_NAME_DIR + (char *)DEFAULT_PACK_EXTENSION).c_str(), "rb");
        clean_filename_dir = filename;
        filename           = MODULE_NAME_DIR.c_str();;
    } else {
        filename           = RELATIVE_MODULE_NAME.c_str();
        clean_filename_dir = RELATIVE_MODULE_NAME;
    }
    if (in) {
        fclose(in);
        IncludedList->Add(new TinyString(clean_filename_dir), DATA_TINYSTRING);

        AnsiString OLD_TEMP_INC_DIR = TEMP_INC_DIR;
        TEMP_INC_DIR = GetPath(&clean_filename_dir);

        Unserialize(AnsiString(AnsiString(filename) + (char *)DEFAULT_PACK_EXTENSION).c_str(), true);

        TEMP_INC_DIR = OLD_TEMP_INC_DIR;

        return 1;
    }
    return 0;
}

AnsiString PIFAlizator::NormalizePath(AnsiString *MODULE_NAME) {
    char *str = MODULE_NAME->c_str();
    int  len  = MODULE_NAME->Length();

    int path_pos[0xFF];
    int path_count  = 0;
    int ignore_next = 0;

    AnsiString result;
#ifndef _WIN32
    if ((str) && (len > 0) && (str[0] == '/'))
        result = "/";
#endif

    // warning: prec_str is in reverse order !
    AnsiString prec_str;

    for (int i = len - 1; i >= -1; i--) {
        // force a '/'
        char c = (i >= 0) ? str[i] : '/';
        if (c == '\\')
            c = '/';

        if (c == '/') {
            if (prec_str.Length()) {
                if (i >= 0)
                    str[i] = 0;
                if (prec_str == "..") {
                    ignore_next++;
                } else
                if (prec_str != ".") {
                    if (ignore_next)
                        ignore_next--;
                    else {
                        if (path_count < 0xFF)
                            path_pos[path_count++] = i + 1;
                    }
                }
                prec_str = "";
            }
        } else
            prec_str += c;
    }

    for (int i = 0; i < ignore_next; i++)
        result += "../";
    char not_first = 0;
    for (int i = path_count - 1; i >= 0; i--) {
        int pos = path_pos[i];
        if (pos >= 0) {
            if (not_first)
                result += "/";
            else
                not_first = 1;
            result += str + pos;
        }
    }
    return result;
}

INTEGER PIFAlizator::RuntimeIncludeCode(const char *CODE) {
    INCLUDE_LEVEL++;
    AnsiString ModuleStream = CODE;
    Execute(&ModuleStream, 0, USE_WARN, USE_EXC, USE_IMPLICIT);
    INCLUDE_LEVEL--;
    return 1;
}

INTEGER PIFAlizator::IncludeFile(const char *MODULE_NAME, INTEGER on_line) {
    INCLUDE_LEVEL++;
    AnsiString FILENAME             = MODULE_NAME;
    AnsiString RELATIVE_MODULE_NAME = TEMP_INC_DIR + MODULE_NAME;
    AnsiString clean_filename_dir;
    AnsiString ModuleStream;
    AnsiString OldFile;
    AnsiString OLD_TEMP_INC_DIR;
    AnsiString MODULE_NAME_DIR;

    RELATIVE_MODULE_NAME = this->NormalizePath(&RELATIVE_MODULE_NAME);

    if (!IncludePackage(MODULE_NAME)) {
        if (ModuleStream.LoadFile(RELATIVE_MODULE_NAME.c_str())) {
            MODULE_NAME_DIR = INCLUDE_DIR + MODULE_NAME;
            if (ModuleStream.LoadFile(MODULE_NAME_DIR.c_str())) {
                Errors.Add(new AnsiException(ERR270, on_line, 270, MODULE_NAME, FileName), DATA_EXCEPTION);
                INCLUDE_LEVEL--;
                return 0;
            }
            clean_filename_dir = MODULE_NAME;
            MODULE_NAME        = MODULE_NAME_DIR.c_str();
        } else {
            MODULE_NAME        = RELATIVE_MODULE_NAME.c_str();
            clean_filename_dir = RELATIVE_MODULE_NAME;
        }
        if (!ListContains(clean_filename_dir.c_str(), IncludedList)) {
            IncludedList->Add(new TinyString(clean_filename_dir), DATA_TINYSTRING);
            OldFile = FileName;
            FileName = MODULE_NAME;

            OLD_TEMP_INC_DIR = TEMP_INC_DIR;
            TEMP_INC_DIR = GetPath(&clean_filename_dir);
            Execute(&ModuleStream, 0, USE_WARN, USE_EXC, USE_IMPLICIT);
            TEMP_INC_DIR = OLD_TEMP_INC_DIR;

            FileName = OldFile;
        }
    }
    INCLUDE_LEVEL--;
    return 1;
}

AnsiString PIFAlizator::GetPath(AnsiString *S) {
    int        len = S->Length();
    AnsiString result;
    AnsiString tmp;

    char *s = S->c_str();

    for (int i = len - 1; i >= 0; i--) {
        if ((s [i] == '/') || (s [i] == '\\')) {
            tmp = s;
            char *tmp2 = tmp.c_str();
            if (i < len - 1) {
                tmp2 [i + 1] = 0;
            }
            result = tmp2;
            break;
        }
    }
    return result;
}

INTEGER PIFAlizator::Execute(AnsiString *Stream, INTEGER on_line, char _USE_WARN, char _USE_EXC, char _USE_IMPLICIT) {
    AnsiParser P(Stream, ConstantList);
    AnsiString sPARSE;

    INTEGER _ID;
    INTEGER TYPE;

    P.Atomize();

    do {
        _ID = 0;

        P.NextAtom(sPARSE);

        if (!sPARSE.Length())
            break;

        TYPE = GetType(sPARSE);

        if (TYPE == TYPE_HEX_NUMBER) {
            uint64_t TEMP_NR = 0;
            sscanf(sPARSE.c_str() + 2, "%llx", &TEMP_NR);
            sPARSE = AnsiString((double)TEMP_NR);
            TYPE   = TYPE_NUMBER;
        }

        if (TYPE < 0) {
            Errors.Add(new AnsiException(ERR050, on_line ? on_line : P.LastLine(), 50, sPARSE, FileName), DATA_EXCEPTION);
            continue;
        }

        if (!_ID) {
            _ID = GetID(sPARSE);

            if ((TYPE == TYPE_METHOD) && (_ID)) {
                TYPE = TYPE_KEYWORD;
            }
        }

        if (!_ID) {
            Errors.Add(new AnsiException(ERR010, on_line ? on_line : P.LastLine(), 10, sPARSE, FileName), DATA_EXCEPTION);
            continue;
        }

        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_CLASS)) {
            BuildClass(&P, on_line);
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_INCLUDE)) {
            AnsiString MODULE_NAME = Strip2(P.GetConstant());
            if (!IncludeFile(MODULE_NAME, on_line ? on_line : P.LastLine())) {
                continue;
            }
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_DEFINE)) {
            P.NextAtom(sPARSE, 1);
            if (GetType(sPARSE) != TYPE_METHOD) {
                Errors.Add(new AnsiException(ERR820, on_line ? on_line : P.LastLine(), 820, sPARSE, FileName), DATA_EXCEPTION);
            } else {
                AnsiString         value = P.GetConstant();
                if (value == sPARSE) {
                    Errors.Add(new AnsiException(ERR1350, on_line ? on_line : P.LastLine(), 1350, sPARSE, FileName), DATA_EXCEPTION);
                } else {
                    VariableDESCRIPTOR *VD   = new VariableDESCRIPTOR;
                    VD->BY_REF = 0;
                    VD->name   = sPARSE;
                    VD->value  = value;
                    VD->USED   = this->INCLUDE_LEVEL;

                    VariableDESCRIPTOR *oldConstant = NULL;
                    INTEGER POS = ConstantIsDescribed(sPARSE, ConstantList, &oldConstant);
                    if (POS) {
                        if ((oldConstant) && (oldConstant->value != VD->value))
                            Warning(WRN10009, on_line ? on_line : P.LastLine(), 10009, sPARSE);
                        else
                            Warning(WRN10004, on_line ? on_line : P.LastLine(), 10004, sPARSE);
#ifdef STDMAP_CONSTANTS
                        ConstantList->Delete(sPARSE.c_str());
#else
                        ConstantList->Delete(POS - 1);
#endif
                    }

                    ConstantList->Add(VD, DATA_VAR_DESCRIPTOR);
                }
            }
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_IMPORT)) {
            sPARSE = Strip2(P.GetConstant());
            sPARSE = this->NormalizePath(&sPARSE);
            if (!ModuleNamesList.ContainsString(sPARSE.c_str())) {
                ModuleNamesList.Add(sPARSE.c_str(), sPARSE.Length());
                AnsiString tmp(IMPORT_DIR);
                tmp += sPARSE;
                if (!ImportModule(tmp, &Errors, on_line ? on_line : P.LastLine(), FileName, ModuleList, this, 1))
                    ImportModule(sPARSE, &Errors, on_line ? on_line : P.LastLine(), FileName, ModuleList, this);
            }
        } else
        if ((TYPE == TYPE_KEYWORD) && (_ID == KEY_PRAGMA)) {
            BuildPragma(&P);
        } else {
            if (TYPE == TYPE_KEYWORD) {
                Errors.Add(new AnsiException(ERR430, on_line ? on_line : P.LastLine(), 430, sPARSE, FileName), DATA_EXCEPTION);
            } else
            if (TYPE == TYPE_OPERATOR) {
                Errors.Add(new AnsiException(ERR440, on_line ? on_line : P.LastLine(), 440, sPARSE, FileName), DATA_EXCEPTION);
            } else
            if (TYPE == TYPE_METHOD) {
                Errors.Add(new AnsiException(ERR450, on_line ? on_line : P.LastLine(), 450, sPARSE, FileName), DATA_EXCEPTION);
            } else
            if (TYPE == TYPE_SEPARATOR) {
                Errors.Add(new AnsiException(ERR480, on_line ? on_line : P.LastLine(), 480, sPARSE, FileName), DATA_EXCEPTION);
            } else {
                Errors.Add(new AnsiException(ERR460, on_line ? on_line : P.LastLine(), 460, sPARSE, FileName), DATA_EXCEPTION);
            }
        }
    } while (sPARSE != NULL_STRING);
    this->SyncClassList();
    USE_IMPLICIT = _USE_IMPLICIT;
    return 0;
}

void PIFAlizator::UpdatePath() {
    TEMP_INC_DIR = GetPath(&FileName);
}

void PIFAlizator::SyncClassList() {
    if (this->ClassList) {
        int count = this->ClassList->Count();
        if (count) {
            this->StaticClassList = (ClassCode **)realloc(this->StaticClassList, count * sizeof(ClassCode *));
            this->ClassList->ToStatic((void **)this->StaticClassList, count);
            // for (int i = 0; i < count; i++)
            //    this->StaticClassList[i] = (ClassCode *)ClassList->Item(i);
        }
    }
}

INTEGER PIFAlizator::CheckUndefinedMembers() {
    int cnt = this->UndefinedMembers.Count();

    for (int i = 0; i < cnt; i++) {
        UndefinedMember *UM  = (UndefinedMember *)UndefinedMembers.Item(i);
        Errors.Insert(new AnsiException(ERR140, UM->line, 140, UM->name, UM->filename, UM->_CLASS, UM->_MEMBER), 0, DATA_EXCEPTION);
    }
    return cnt;
}

INTEGER PIFAlizator::CheckUndefinedClasses() {
    int cnt = this->UndefinedClasses.Count();

    for (int i = 0; i < cnt; i++) {
        UndefinedMember *UM  = (UndefinedMember *)UndefinedClasses.Item(i);
        Errors.Insert(new AnsiException(ERR100, UM->line, 100, UM->name, UM->filename, UM->_CLASS, UM->_MEMBER), 0, DATA_EXCEPTION);
    }
    return cnt;
}

INTEGER PIFAlizator::Execute() {
    int result = Execute(InputStream);

    CheckUndefinedMembers();
    CheckUndefinedClasses();
    return result;
}

INTEGER PIFAlizator::ErrorCount() {
    return Errors.Count();
}

INTEGER PIFAlizator::WarningCount() {
    return Warnings.Count();
}

#define COMPILED_FILE_ID    "CONC0100"
#define PACKAGE_FILE_ID     "PONC0100"

#define COMPILED_FILE_IDv2  "CONC0101"
#define PACKAGE_FILE_IDv2   "PONC0101"

#define COMPILED_FILE_IDv3  "CONC0102"
#define PACKAGE_FILE_IDv3   "PONC0102"

int PIFAlizator::Serialize(char *filename, bool is_lib) {
    FILE *out = fopen(filename, "wb");

    if (out) {
        if (is_lib) {
            concept_fwrite(PACKAGE_FILE_IDv3, strlen(PACKAGE_FILE_IDv3), 1, out);
        } else {
            concept_fwrite(COMPILED_FILE_IDv3, strlen(COMPILED_FILE_IDv3), 1, out);
        }
        char PRAGMA_SET = USE_WARN | (USE_EXC * 2);
        fputc(PRAGMA_SET, out);

        INTEGER module_list = this->ModuleNamesList.Count();
        concept_fwrite_int(&module_list, sizeof(module_list), 1, out);
        for (INTEGER i = 0; i < module_list; i++) {
            AnsiString member_name(ModuleNamesList [i]);
            member_name.Serialize(out, SERIALIZE_16BIT_LENGTH);
        }

        if (is_lib) {
#ifdef STDMAP_CONSTANTS
            this->ConstantList->Serialize(out);
#else
            INTEGER constant_list = this->ConstantList->Count();

            INTEGER temp = 0;

            for (INTEGER i = basic_constants_count; i < constant_list; i++) {
                VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)ConstantList->Item(i);
                if (!VD->USED) {
                    temp++;
                }
            }

            concept_fwrite_int(&temp, sizeof(temp), 1, out);
            for (INTEGER i = basic_constants_count; i < constant_list; i++) {
                VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)ConstantList->Item(i);
                if (!VD->USED) {
                    VD->name.Serialize(out, SERIALIZE_8BIT_LENGTH);
                    VD->value.Serialize(out, SERIALIZE_16BIT_LENGTH);
                }
            }
#endif
            INTEGER include_list = this->IncludedList->Count();
            concept_fwrite_int(&include_list, sizeof(include_list), 1, out);
            for (INTEGER i = 0; i < include_list; i++) {
                TinyString *member_name = (TinyString *)IncludedList->Item(i);
                member_name->Serialize(out, SERIALIZE_16BIT_LENGTH);
            }

            INTEGER class_list = this->ClassList->Count();
            concept_fwrite_int(&class_list, sizeof(class_list), 1, out);
            for (INTEGER i = 0; i < class_list; i++) {
                ClassCode *CC1 = (ClassCode *)ClassList->Item(i);
                CC1->NAME.Serialize(out, SERIALIZE_16BIT_LENGTH);
            }
        }

        // salvez GeneralMembers
        INTEGER general_members = this->GeneralMembers->Count();
        concept_fwrite_int(&general_members, sizeof(general_members), 1, out);
        for (INTEGER i = 0; i < general_members; i++) {
            TinyString member_name(GeneralMembers->Item(i));
            member_name.Serialize(out, SERIALIZE_16BIT_LENGTH);
        }

        INTEGER class_count = ClassList->Count();
        if (is_lib) {
            INTEGER class_count2 = 0;
            for (INTEGER ii = 0; ii < class_count; ii++) {
                ClassCode *CC = (ClassCode *)(*ClassList) [ii];
                if (CC->DEFINED_LEVEL == 0) {
                    class_count2++;
                }
            }
            concept_fwrite_int(&class_count2, sizeof(class_count2), 1, out);
        } else {
            //---------------------------------------------------//
            INTEGER class_count2 = 0;
            for (INTEGER ii = 0; ii < class_count; ii++) {
                ClassCode *CC = (ClassCode *)(*ClassList) [ii];
                if (CC->DEFINED_LEVEL != -1) {
                    class_count2++;
                }
            }
            concept_fwrite_int(&class_count2, sizeof(class_count2), 1, out);
            //---------------------------------------------------//
        }
        for (INTEGER ii = 0; ii < class_count; ii++) {
            ClassCode *CC = (ClassCode *)(*ClassList) [ii];
            if (is_lib) {
                if (CC->DEFINED_LEVEL == 0) {
                    CC->Serialize(this, out, is_lib);
                }
            } else
            if (CC->DEFINED_LEVEL != -1) {
                CC->Serialize(this, out, is_lib);
            }
        }
        if (!is_lib) {
            concept_fwrite_int(&ENTRY_CLASS, sizeof(ENTRY_CLASS), 1, out);
        }

        fclose(out);
        return 1;
    }
    return -1;
}

int PIFAlizator::ComputeSharedSize(char *filename) {
    char         static_buffer [0xFF];
    concept_FILE *in = concept_fopen(filename, "rb");

    if (!in) {
        return 0;
    }

    const char *cftype = COMPILED_FILE_ID;
    int        size    = (int)strlen(cftype);

    FREAD_FAIL(static_buffer, size, 1, in);
    static_buffer [size] = 0;

    int version = 1;
    if (!strcmp(static_buffer, COMPILED_FILE_IDv3)) {
        version = 2;
    } else
    if (strcmp(static_buffer, COMPILED_FILE_IDv2)) {
        if (strcmp(static_buffer, cftype)) {
            concept_fclose(in);
            return 0;
        }
        version = 0;
    }

    // pragma !
    SKIP(1, in);
    INTEGER module_list;
    FREAD_INT_FAIL(&module_list, sizeof(module_list), 1, in);

    for (INTEGER i = 0; i < module_list; i++) {
        AnsiString::ComputeSharedSize(in, SERIALIZE_16BIT_LENGTH);
    }

    INTEGER general_members;
    FREAD_INT_FAIL(&general_members, sizeof(general_members), 1, in);
    for (INTEGER i = 0; i < general_members; i++) {
        size += AnsiString::ComputeSharedSize(in, SERIALIZE_16BIT_LENGTH);
    }

    INTEGER class_count;
    FREAD_INT_FAIL(&class_count, sizeof(class_count), 1, in);
    for (INTEGER ii = 0; ii < class_count; ii++) {
        size += ClassCode::ComputeSharedSize(in, general_members, version);
    }

    INTEGER Cached_ENTRY_CLASS = -1;
    FREAD_INT_FAIL(&Cached_ENTRY_CLASS, sizeof(Cached_ENTRY_CLASS), 1, in);

    concept_fclose(in);

    return size;
}

int PIFAlizator::Unserialize(char *filename, bool is_lib) {
    char old_enable_private = this->enable_private;
    this->enable_private = 1;
    concept_FILE *in = concept_fopen(filename, "rb");
    char         static_buffer [0xFF];
    int          *Classes            = 0;
    int          *Members_Relocation = 0;

#ifdef CACHED_LIST
    if (!is_lib)
        StaticList::UseMap = 0;
#endif

    if (in) {
        const char *cftype2 = COMPILED_FILE_IDv3;
        const char *cftype = COMPILED_FILE_IDv2;
        const char *cftype_old = COMPILED_FILE_ID;
        int version = 1;
        if (is_lib) {
            cftype2 = PACKAGE_FILE_IDv3;
            cftype = PACKAGE_FILE_IDv2;
            cftype_old = PACKAGE_FILE_ID;
        }

        int size = (int)strlen(cftype);
        if (!concept_fread_buffer(static_buffer, size, 1, in)) {
            concept_fclose(in);
            Errors.Add(new AnsiException(ERR1120, 0, 1120, filename, FileName), DATA_EXCEPTION);
            this->enable_private = old_enable_private;
            return -3;
        }
        static_buffer [size] = 0;

        if (!strcmp(static_buffer, cftype2)) {
            version = 2;
        } else
        if (strcmp(static_buffer, cftype)) {
            if (strcmp(static_buffer, cftype_old)) {
                concept_fclose(in);
                Errors.Add(new AnsiException(ERR1120, 0, 1120, filename, FileName), DATA_EXCEPTION);
                this->enable_private = old_enable_private;
                return -3;
            }
            version = 0;
        }
        char PRAGMA_SET = 0;
        concept_fread(&PRAGMA_SET, 1, 1, in);

        USE_WARN = PRAGMA_SET % 2;
        USE_EXC  = PRAGMA_SET / 2;

        INTEGER module_list;
        if (!concept_fread_int(&module_list, sizeof(module_list), 1, in)) {
            concept_fclose(in);
            Errors.Add(new AnsiException(ERR1120, 0, 1120, filename, FileName), DATA_EXCEPTION);
            this->enable_private = old_enable_private;
            return -3;
        }
        for (INTEGER i = 0; i < module_list; i++) {
            AnsiString member_name;
            member_name.Unserialize(in, SERIALIZE_16BIT_LENGTH);
            if ((is_lib) && (ModuleNamesList.ContainsString(member_name.c_str())))
                continue;
            ModuleNamesList.Add(member_name.c_str(), member_name.Length());

            AnsiString tmp(IMPORT_DIR);
            tmp += member_name;
            if (!ImportModule(tmp, &Errors, 0, "", ModuleList, this, 1))
                ImportModule(member_name, &Errors, 0, "", ModuleList, this);
        }

        if (is_lib) {
            INTEGER constant_list;
            if (!concept_fread_int(&constant_list, sizeof(constant_list), 1, in)) {
                concept_fclose(in);
                Errors.Add(new AnsiException(ERR1120, 0, 1120, filename, FileName), DATA_EXCEPTION);
                this->enable_private = old_enable_private;
                return -3;
            }

            StaticString ct_name;
            StaticString ct_value;
            for (INTEGER i = 0; i < constant_list; i++) {
                ct_name.Unserialize(in, SERIALIZE_8BIT_LENGTH);
                ct_value.Unserialize(in, SERIALIZE_16BIT_LENGTH);
                this->DefineConstant(ct_name.c_str(), ct_value.c_str(), false);
            }

            INTEGER include_list;
            if (!concept_fread_int(&include_list, sizeof(include_list), 1, in)) {
                concept_fclose(in);
                Errors.Add(new AnsiException(ERR1120, 0, 1120, filename, FileName), DATA_EXCEPTION);
                this->enable_private = old_enable_private;
                return -3;
            }

            AnsiString include_name;
            for (INTEGER i = 0; i < include_list; i++) {
                include_name.Unserialize(in, SERIALIZE_16BIT_LENGTH);
                this->enable_private = old_enable_private;
                IncludeFile(include_name, 0);
                this->enable_private = 1;
            }

            INTEGER class_list;
            if (!concept_fread_int(&class_list, sizeof(class_list), 1, in)) {
                concept_fclose(in);
                Errors.Add(new AnsiException(ERR1120, 0, 1120, filename, FileName), DATA_EXCEPTION);
                this->enable_private = old_enable_private;
                return -3;
            }
            if (class_list) {
                Classes = new int [class_list];
            }

            StaticString class_name;
            for (INTEGER i = 0; i < class_list; i++) {
                class_name.Unserialize(in, SERIALIZE_16BIT_LENGTH);
                Classes [i] = ClassExists(class_name.c_str()) - 1;
                if (Classes [i] < 0) {
                    this->enable_private = old_enable_private;
                    if (BUILTINOBJECTS(this, class_name.c_str())) {
                        Classes [i] = ClassExists(class_name.c_str()) - 1;
                    }
                    this->enable_private = 1;
                }
            }

            INTEGER general_members;
            if (!concept_fread_int(&general_members, sizeof(general_members), 1, in)) {
                concept_fclose(in);
                Errors.Add(new AnsiException(ERR1120, 0, 1120, filename, FileName), DATA_EXCEPTION);
                if (Classes) {
                    delete[] Classes;
                }
                this->enable_private = old_enable_private;
                return -3;
            }

            if (general_members)
                Members_Relocation = new int [general_members];

            AnsiString member_name;
            for (INTEGER i = 0; i < general_members; i++) {
                member_name.Unserialize(in, SERIALIZE_16BIT_LENGTH);
                int reloc = GeneralMembers->ContainsString(member_name.c_str());
                if (!reloc) {
                    AddGeneralMember(member_name);
                    reloc = GeneralMembers->Count();
                }
                Members_Relocation [i] = reloc - 1;
            }
        } else {
            INTEGER general_members;
            if (!concept_fread_int(&general_members, sizeof(general_members), 1, in)) {
                concept_fclose(in);
                Errors.Add(new AnsiException(ERR1120, 0, 1120, filename, FileName), DATA_EXCEPTION);
                this->enable_private = old_enable_private;
                return -3;
            }
            bool is_pooled  = this->is_buffer ? 0 : SHIsPooled();
            bool is_created = this->is_buffer ? 0 : SHIsCreated();

            GeneralMembers->ReInit(general_members, is_pooled);
            TinyString member_name;
            if ((is_pooled) && (!is_created)) {
                for (INTEGER i = 0; i < general_members; i++) {
                    GeneralMembers->PoolMap(AnsiString::ComputeSharedSize(in, SERIALIZE_16BIT_LENGTH));
                }
            } else {
                for (INTEGER i = 0; i < general_members; i++) {
                    member_name.Unserialize(in, SERIALIZE_16BIT_LENGTH);
                    AddGeneralMember(member_name);
                }
            }
        }

        INTEGER class_count;
        if (!concept_fread_int(&class_count, sizeof(class_count), 1, in)) {
            concept_fclose(in);
            Errors.Add(new AnsiException(ERR1120, 0, 1120, filename, FileName), DATA_EXCEPTION);
            if (Classes) {
                delete[] Classes;
                Classes = 0;
            }
            if (Members_Relocation) {
                delete[] Members_Relocation;
                Members_Relocation = 0;
            }
            this->enable_private = old_enable_private;
            return -3;
        }

        for (INTEGER ii = 0; ii < class_count; ii++) {
            ClassCode *CC = new ClassCode(NULL_STRING, this, !is_lib);
            ClassList->Add(CC, DATA_CLASS_CODE);
            CC->Unserialize(this, in, ClassList, is_lib, Classes, Members_Relocation, version);
#ifdef CACHED_CLASSES
            HashTable_add(&this->CachedClasses, CC->NAME.c_str(), this->ClassList->Count(), CC->NAME.Length());
#endif
        }

        if (!is_lib) {
            concept_fread_int(&ENTRY_CLASS, sizeof(ENTRY_CLASS), 1, in);
        }
        concept_fclose(in);

        if (Classes) {
            delete[] Classes;
        }
        if (Members_Relocation) {
            delete[] Members_Relocation;
        }
        DoneLinking();
        this->SyncClassList();
        this->enable_private = old_enable_private;
        return 0;
    }
    this->enable_private = old_enable_private;
    return -3;
}

void PIFAlizator::OptimizeMember(ClassMember *CM) {
    if ((!CM) || (CM->OPTIMIZER) || (CM->IS_FUNCTION != 1) || (!CM->Defined_In))
        return;

    ClassCode *CC = (ClassCode *)CM->Defined_In;

    void *old_helper = this->Helper;
    this->Helper = NULL;

    CM->OPTIMIZER = new_Optimizer(this, CM->CDATA->PIF_DATA, CM->CDATA->VariableDescriptors, CC->_DEBUG_INFO_FILENAME.c_str(), CC, CM);

    OptimizerHelper *helper = Optimizer_GetHelper(this);
    helper->ParameterList = new AnsiList();
    helper->OptimizedPIF = new AnsiList();

    Optimizer_Optimize((struct Optimizer *)CM->OPTIMIZER, this);
    Optimizer_GenerateIntermediateCode((struct Optimizer *)CM->OPTIMIZER, this);

    delete helper->ParameterList;
    helper->ParameterList = NULL;
    delete helper->OptimizedPIF;
    helper->OptimizedPIF = NULL;
    delete_OptimizerHelper(helper);

    this->Helper = old_helper;
}

void PIFAlizator::Optimize(int start, char use_compiled_code, char use_lock) {
    int class_count = ClassList->Count();

    for (INTEGER ii = start; ii < class_count; ii++) {
        ClassCode *CC = (ClassCode *)ClassList->Item(ii);
        if (use_compiled_code) {
            if (!CC->NEEDED)
                continue;
        } else
            CC->CLSID = ii;

        int members_count = CC->Members ? CC->Members->Count() : CC->pMEMBERS_COUNT;

        if (members_count > 65535) {
            Errors.Add(new AnsiException(ERR1250, 0, 1250, "", CC->_DEBUG_INFO_FILENAME, CC->NAME), DATA_EXCEPTION);
            continue;
        }

        if (!use_compiled_code) {
            if (use_lock)
                semp(WorkerLock);
            for (INTEGER jj = 0; jj < members_count; jj++) {
                ClassMember *CM = CC->Members ? (ClassMember *)CC->Members->Item(jj) : CC->pMEMBERS[jj];

                INTEGER LOCAL_CLASSID = ((ClassCode *)CM->Defined_In)->CLSID;
                if ((CM->IS_FUNCTION == 1) && (CM->Defined_In == CC) && (!CM->OPTIMIZER)) {
                    CM->OPTIMIZER = new_Optimizer(this, CM->CDATA->PIF_DATA, CM->CDATA->VariableDescriptors, ((ClassCode *)(CM->Defined_In))->_DEBUG_INFO_FILENAME.c_str(), CC, CM);
                    OptimizerHelper *helper = Optimizer_GetHelper(this);
                    helper->ParameterList = new AnsiList();
                    helper->OptimizedPIF = new AnsiList();

                    Optimizer_Optimize((struct Optimizer *)CM->OPTIMIZER, this);
                    Optimizer_GenerateIntermediateCode((struct Optimizer *)CM->OPTIMIZER, this);

                    delete helper->ParameterList;
                    helper->ParameterList = NULL;

                    delete helper->OptimizedPIF;
                    helper->OptimizedPIF = NULL;
                }
            }
            if (use_lock)
                semv(WorkerLock);
        }
        if (!CC->pMEMBERS) {
            CC->GenerateCode(GeneralMembers);
        }
    }
    PIF.Clear();
    DoneLinking();
}

bool PIFAlizator::CheckRunable() {
    if (!ENTRY_CLASS) {
        Errors.Add(new AnsiException(ERR080, 0, 80, MAIN_ENTRY_POINT, FileName), DATA_EXCEPTION);
        return false;
    }
    ClassCode *CC = (ClassCode *)(*ClassList) [ENTRY_CLASS - 1];
    if (!CC->CONSTRUCTOR) {
        Errors.Add(new AnsiException(ERR090, 0, 90, MAIN_ENTRY_POINT, FileName), DATA_EXCEPTION);
        return false;
    }
    return true;
}

void *PIFAlizator::GetStartingPoint() {
    ClassCode *CC = (ClassCode *)(*ClassList) [ENTRY_CLASS - 1];

    return CC;
}

#ifdef PRINT_DEBUG_INFO
AnsiString PIFAlizator::DEBUG_INFO() {
    AnsiString res;

    for (INTEGER ii = 0; ii < ClassList->Count(); ii++) {
        ClassCode *CC = (ClassCode *)(*ClassList) [ii];
        res += AnsiString("In class \"") + CC->NAME.c_str() + AnsiString("\" :\n");
        for (INTEGER jj = 0; jj < CC->Members->Count(); jj++) {
            ClassMember *CM = (ClassMember *)CC->Members->Item(jj);

            if (CM->OPTIMIZER) {
                res += ((struct Optimizer *)CM->OPTIMIZER)->DEBUG_INFO();
            }
            if (CM->CDATA) {
                DoubleList *PIFList = CM->CDATA->PIF_DATA;
                DoubleList *VDList  = CM->CDATA->VariableDescriptors;

                res += AnsiString("In class member \"") + CM->NAME + AnsiString("\" :\n");
                res += "------------ Program internal form ----------------------------------------\n";
                for (INTEGER i = 0; i < PIFList->Count(); i++) {
                    AnalizerElement *AE = (AnalizerElement *)(*PIFList) [i];
                    AnsiString      strOperator;
                    if ((AE->TYPE == TYPE_KEYWORD) || (AE->TYPE == TYPE_OPERATOR)) {
                        strOperator = GetKeyWord(AE->ID);
                    }

                    res += AnsiString(i) + AnsiString(". ") +
                           AnsiString("\tID: ") + AnsiString(AE->ID) +
                           AnsiString(" '") + strOperator +
                           AnsiString("'\t\tTYPE:") + AnsiString(AE->TYPE) +
                           AnsiString("\t\tLINE:") + AnsiString(AE->_DEBUG_INFO_LINE) +
                           AnsiString("\n");
                }
                res += "------------ Variables/CT/DATA --------------------------------------------\n";
                for (INTEGER j = 0; j < VDList->Count(); j++) {
                    VariableDESCRIPTOR *VD = (VariableDESCRIPTOR *)VDList->Item(j);
                    res += AnsiString(j) + AnsiString(". ") +
                           AnsiString("\tName: ") + ((VD->name == NULL_STRING) ? AnsiString("(ct)") : VD->name) +
                           AnsiString("\t\tType: ") + AnsiString(VD->TYPE) +
                           AnsiString("\t\tValue: ") + ((VD->value.Length()) ? AnsiString("(null)") : AnsiString(VD->value.c_str())) +
                           AnsiString("\n");
                }
            }
        }
    }
    res += PRINT_ERRORS();
    return res;
}
#endif

AnsiString PIFAlizator::SerializeWarningsErrors(int ser_warnings) {
    AnsiString result;
    AnsiList   *Target = &Errors;

    if (ser_warnings) {
        Target = &Warnings;
    }

    int count = Target->Count();
    for (INTEGER k = 0; k < count; k++) {
        AnsiException *Ex = (AnsiException *)Target->Item(k);
        result += Ex->GetFileName();
        result += "<:>";
        result += AnsiString((intptr_t)Ex->GetLine());
        result += "<:>";
        result += (char *)Ex->GetClass();
        result += "<:>";
        result += Ex->GetMember();
        result += "<:>";
        result += AnsiString((intptr_t)Ex->GetID());
        result += "<:>";
        result += Ex->GetText();
        result += "<:>";
        result += Ex->GetExtra();
        if (k != count - 1) {
            result += "<:>";
        }
    }
    return result;
}

AnsiString PIFAlizator::PRINT_ERRORS(int html) {
    AnsiString res;

    if (html == 1) {
        if (!this->out->userdata) {
            res = "Content-Type: text/plain\r\n\r\n";
        }
    }
    res += "\n------------ Errors -------------------------------------------------------\n";
    AnsiString filename;
    AnsiString _CLASS;
    AnsiString _MEMBER;
    int        count = Errors.Count();
    for (INTEGER k = 0; k < count; k++) {
        AnsiException *Ex = (AnsiException *)Errors [k];

        if (filename != Ex->GetFileName()) {
            filename = Ex->GetFileName();
            res     += "In ";
            res     += filename;
            res     += ":\n";
        }
        if (_CLASS != Ex->GetClass()) {
            _CLASS = Ex->GetClass();
            if (_CLASS != "") {
                res += "  In class ";
                res += _CLASS;
                res += ":\n";
            } else {
                res += "  Outside class:\n";
            }
        }

        if (_MEMBER != Ex->GetMember()) {
            _MEMBER = Ex->GetMember();
            if (_MEMBER != "") {
                res += "    In member ";
                res += _CLASS;
                res += ".";
                res += (char *)_MEMBER;
                res += ":\n";
            } else {
                res += "    Outside class member:\n";
            }
        }
        res += "      ";
        res += AnsiString(k) + AnsiString(". ") +
               AnsiString("E") + AnsiString(Ex->GetID()) +
               AnsiString(" on line ") + AnsiString(Ex->GetLine()) +
               AnsiString(": ") + Ex->GetText();
        const char *extra = Ex->GetExtra();
        if ((extra) && (extra[0])) {
            res += AnsiString(" (\'") + extra + AnsiString("\') ");
        }

        res += AnsiString("\n");
    }
    return res;
}

AnsiString PIFAlizator::PRINT_WARNINGS(int html) {
    AnsiString res;

    if (html == 1) {
        if (!this->out->userdata) {
            res = "Content-Type: text/plain\r\n\r\n";
        }
    }
    res += "------------ Warnings -----------------------------------------------------\n";
    for (INTEGER k = 0; k < Warnings.Count(); k++) {
        AnsiException *Ex = (AnsiException *)Warnings [k];
        res += AnsiString(k) + AnsiString(". ") +
               AnsiString("\tID: ") + AnsiString(Ex->GetID()) +
               AnsiString("\tText: ") + Ex->GetText() +
               AnsiString("\tLine: ") + AnsiString(Ex->GetLine()) +
               AnsiString("\tExtra: ") + Ex->GetExtra() +
               AnsiString("\tFile name: ") + Ex->GetFileName() +
               AnsiString("\n");
    }
    return res;
}

#ifdef PRINT_DEBUG_INFO
AnsiString PIFAlizator::DEBUG_CLASS_CONFIGURATION() {
    AnsiString res;

    res += "------------ Class Configuration ------------------------------------------\n";
    INTEGER CLASS_COUNT = ClassList->Count();
    for (INTEGER i = 0; i < CLASS_COUNT; i++) {
        ClassCode *CC           = (ClassCode *)(*ClassList) [i];
        INTEGER   MEMBERS_COUNT = CC->Members->Count();
        res += "Class '";
        res += CC->NAME.c_str();
        res += "' class ID :";
        res += (char *)AnsiString(CC->CLSID);
        res += "\n";
        for (INTEGER j = 0; j < MEMBERS_COUNT; j++) {
            ClassMember *CM = (ClassMember *)CC->Members->Item(j);
            res += "\t::";
            res += CM->NAME;
            if (CM->IS_FUNCTION == 1) {
                res += " is a function\n\t\tDATA:\n";
                for (INTEGER k = 0; k < ((struct Optimizer *)CM->OPTIMIZER)->dataCount; k++) {
                    res += "\t\t\tTYPE:";
                    res += AnsiString((intptr_t)((struct Optimizer *)CM->OPTIMIZER)->DATA [k].TYPE);
                    res += "\n";
                }
                res += "\t\tCODE:\n";
                for (INTEGER k = 0; k < ((struct Optimizer *)CM->OPTIMIZER)->codeCount; k++) {
                    res += "\t\t\tOPERATOR TYPE:";
                    res += AnsiString(IS_KEYWORD((&(((struct Optimizer *)CM->OPTIMIZER)->CODE [k]))) ? "keyword" : "operator");
                    res += "\tOPERATOR ID:";
                    res += AnsiString(((struct Optimizer *)CM->OPTIMIZER)->CODE [k].Operator_ID);
                    res += "\n";
                }
            } else {
                res += "\n";
            }
        }
        res += "\n\tDYNAMIC MEMBERS:\n";
        for (INTEGER j = 0; j < CC->pMEMBERS_COUNT; j++) {
            ClassMember *CM = (ClassMember *)CC->pMEMBERS [j];
            res += "\t\t[+]\t";
            res += AnsiString(j);
            res += "\t";
            if (CM) {
                res += CM->NAME;
            } else {
                res += "___NADA___";
            }
            res += "\n";
        }
    }
    return res;
}
#endif

char *PIFAlizator::CheckMember(const char *member_name) {
    int  ref_id    = GeneralMembers->ContainsString(member_name);
    char *ref_name = 0;

    if (!ref_id) {
        AddGeneralMember(member_name);
        ref_id = GeneralMembers->Count();
    } else {
        DefineMember(member_name);
    }
    ref_name = GeneralMembers->Item(ref_id - 1);
    return ref_name;
}

INTEGER PIFAlizator::FindVariableByName(void *key, const char *name) {
#ifdef DEBUGGER_VAR_NAMES
    int len = strlen(name);
    char buf[0x100 + sizeof(void *) * 2 + 1];
    buf[0] = 0;
#if __SIZEOF_POINTER__ == 8
    sprintf(buf, "%16llx", (unsigned long long)key);
#else
    sprintf(buf, "%08x", (unsigned int)key);
#endif
    if (len > 0x100)
        len = 0x100;
    memcpy(buf + sizeof(void *) * 2, name, len);
    buf[sizeof(void *) * 2 + len] = 0;
    return HashTable_find(&DebugVarNames, buf) - 1;
#else
    return -1;
#endif
}

void PIFAlizator::RegisterVariableName(void *key, const char *name, INTEGER val) {
#ifdef DEBUGGER_VAR_NAMES
    int len = strlen(name);
    char buf[0x100 + sizeof(void *) * 2 + 1];
    buf[0] = 0;
#if __SIZEOF_POINTER__ == 8
    sprintf(buf, "%16llx", (unsigned long long)key);
#else
    sprintf(buf, "%08x", (unsigned int)key);
#endif
    if (len > 0x100)
        len = 0x100;
    memcpy(buf + sizeof(void *) * 2, name, len);
    buf[sizeof(void *) * 2 + len] = 0;
    HashTable_add(&DebugVarNames, buf, val + 1, -1);
#endif
}

void PIFAlizator::EnsureThreadSafe() {
    INTEGER Count = ClassList->Count();

    if ((!StaticClassList) || (TSClassCount == Count))
        return;

    TSClassCount = Count;
    for (INTEGER i = 0; i < Count; i++) {
        ClassCode *CC = StaticClassList [i];
        if (CC)
            CC->EnsureThreadSafe();
    }
}

