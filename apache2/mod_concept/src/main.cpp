#define READ_BUF_SIZE               0xFFFF
#ifdef BSD
 #define AP_NEED_SET_MUTEX_PERMS    1
#endif

#ifdef _WIN32
 #include <windows.h>
 #include <sys/utime.h>
#else
 #include <pthread.h>      // pthread_create
 #include <dlfcn.h>        // dlopen

 #define HANDLE            void *
 #define HMODULE           HANDLE
 #define LPVOID            void *

 #define SOCKET            int
 #define INVALID_SOCKET    -1
 #include <utime.h>
#endif

#include "AnsiString.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef _WIN32
 #include <io.h>
 #include <direct.h>

 #define pipe(phandles)    _pipe(phandles, 4096, _O_BINARY); setmode(phandles[0], _O_BINARY); setmode(phandles[1], _O_BINARY);
#else
 #include <sys/select.h>
 #include <netinet/tcp.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
 #define PIPE_TYPE    int
#endif

extern "C" {
#include <httpd.h>
#include <http_config.h>
#include <http_protocol.h>
#include <http_request.h>
#include <http_core.h>
#include <http_log.h>
#include <apr_strings.h>
#include <apr_optional.h>
#include <apr_lib.h>
#include <util_script.h>
#ifdef AP_NEED_SET_MUTEX_PERMS
 #include "unixd.h"
#endif
}

#include <map>
#include "semhh.h"

static std::map<unsigned long, void *> LocalBases;
HHSEM map_lock;

#ifdef _WIN32
 #define IPC_FILE_NAME    "logs/Mod_Concept_"
 #define IPC_FILE_LOCK    "logs/Mod_Concept_Lock"
 #define DO_ROOT(pool, file)    ap_server_root_relative(pool, file)
#else
 #define IPC_FILE_NAME    "/tmp/Mod_Concept_"
 #define IPC_FILE_LOCK    "/tmp/Mod_Concept_Lock"
 #define DO_ROOT(pool, file)    file
#endif

static int max_processes = 5;
static apr_global_mutex_t *global_mutex = 0;

static void GlobalLock() {
    if (global_mutex) {
        if (apr_global_mutex_lock(global_mutex) != APR_SUCCESS)
            fprintf(stderr, "mod_concept: error global locking\n");
    }
}

static void GlobalUnlock() {
    if (global_mutex) {
        if (apr_global_mutex_unlock(global_mutex) != APR_SUCCESS)
            fprintf(stderr, "mod_concept: error global unlocking\n");
    }
}

static const char *concept_parameters(cmd_parms *cmd, void *dummy, const char *arg) {
    max_processes = atoi(arg);
    return NULL;
}

static const command_rec concept_cmds[] = {
    AP_INIT_TAKE1("ConceptProcesses", (const char *(*)())concept_parameters, NULL, RSRC_CONF, "the maximum number of concurent processes per script"),
    { NULL }
};

int SetHeaders(const char *key, const char *value, request_rec *r) {
    if (!strcasecmp(key, "content-type"))
        ap_set_content_type(r, value);
    else
    if (!strcasecmp(key, "status")) {
        r->status = atoi(value);
    } else
        apr_table_set(r->headers_out, key, value);
    return 1;
}

struct WorkerBase {
#ifndef _WIN32
    char         stdin_base[0xFF];
    char         stdout_base[0xFF];
    char         backcomm_base[0xFF];
#endif
    char         initialized;
    int          stdin_pipe[2];
    int          stdout_pipe[2];
    int          backcomm_pipe[2];
    char         in_use;
    char         thread_ended;
    unsigned int remaining_sessions;
    // dummy
    const void   *reserved;
    const void   *reserved_apr;
};

#ifndef _WIN32
AnsiString Temp() {
    AnsiString s(getenv("tmp"));

    if (!s.Length())
        s = "/tmp";
    return s;
}

AnsiString fifo_pipe(int *p, unsigned int hash, long order, char *hint) {
    p[0] = 0;
    p[1] = 0;
    AnsiString s("/ConceptFIFO-");
    s += AnsiString(order);
    s += ".";

    char buf[0x20];
    buf[0] = 0;
    sprintf(buf, "%x", hash);
    s += buf;
    s += hint;
    s += ".tmp";

    AnsiString full_path = Temp() + s;
    unlink(full_path.c_str());
    mkfifo(full_path.c_str(), S_IWUSR | S_IRUSR);
    return s;
}

static void OpenBase(WorkerBase *Base) {
    AnsiString tmp = Temp();

    Base->stdout_pipe[0]   = open(tmp + Base->stdout_base, O_RDONLY);
    Base->stdout_pipe[1]   = Base->stdout_pipe[0];
    Base->stdin_pipe[0]    = open(tmp + Base->stdin_base, O_WRONLY);
    Base->stdin_pipe[1]    = Base->stdin_pipe[0];
    Base->backcomm_pipe[0] = open(tmp + Base->backcomm_base, O_RDONLY);
    Base->backcomm_pipe[1] = Base->backcomm_pipe[0];

    if (Base->stdout_pipe[0] <= 0)
        fprintf(stderr, "Error opening FIFO/stdout\n");
    if (Base->stdin_base[0] <= 0)
        fprintf(stderr, "Error opening FIFO/stdin\n");
    if (Base->backcomm_base[0] <= 0)
        fprintf(stderr, "Error opening FIFO/comm\n");
}

static void CloseBase(WorkerBase *Base) {
    close(Base->stdout_pipe[0]);
    close(Base->stdin_base[0]);
    close(Base->backcomm_base[0]);
}
#endif

static void InitBase(WorkerBase *Base, const void *filename, long order, unsigned int hash, void *apr_context) {
    Base->in_use             = 1;
    Base->thread_ended       = 0;
    Base->initialized        = 1;
    Base->remaining_sessions = 1;
#ifdef _WIN32
    pipe(Base->stdout_pipe);
    pipe(Base->backcomm_pipe);
    pipe(Base->stdin_pipe);
#else
    AnsiString temp = fifo_pipe(Base->stdout_pipe, hash, order, "stdout");
    memcpy(Base->stdout_base, temp.c_str(), temp.Length() + 1);
    temp = fifo_pipe(Base->backcomm_pipe, hash, order, "comm");
    memcpy(Base->backcomm_base, temp.c_str(), temp.Length() + 1);
    temp = fifo_pipe(Base->stdin_pipe, hash, order, "stdin");
    memcpy(Base->stdin_base, temp.c_str(), temp.Length() + 1);
#endif
    Base->reserved     = filename;
    Base->reserved_apr = apr_context;
}

static void DoneBase(WorkerBase *Base) {
    Base->in_use = 1;
#ifdef _WIN32
    close(Base->stdin_pipe[0]);
    close(Base->stdin_pipe[1]);
    close(Base->stdout_pipe[0]);
    close(Base->stdout_pipe[1]);
    close(Base->backcomm_pipe[0]);
    close(Base->backcomm_pipe[1]);
#else
    AnsiString tmp = Temp();
    unlink(tmp + Base->stdout_base);
    unlink(tmp + Base->stdin_base);
    unlink(tmp + Base->backcomm_base);
#endif
    Base->thread_ended = 1;
    apr_shm_t *t = (apr_shm_t *)Base->reserved_apr;
    memset(Base, 0, sizeof(WorkerBase));
    Base->remaining_sessions = 1;

    if (t)
        apr_shm_detach(t);
}

class WorkerProcess {
public:

    WorkerBase *Base;

    const void    *reserved;
    char          *headers;
    apr_shm_t     *dzone;
    int           headers_index;
    int           size;
    unsigned long outputsize;

    WorkerProcess(WorkerBase *_Base, apr_shm_t *m) {
        this->Base    = _Base;
        headers       = 0;
        headers_index = 0;
        size          = 0;
        reserved      = 0;
        outputsize    = 0;
        dzone         = m;
    }

    char GetEnded() {
        char res = 0;

        GlobalLock();
        res = Base->thread_ended;
        GlobalUnlock();
        return res;
    }

    char *HeaderEnd(char **dif, int *len) {
        *len = 0;
        *dif = 0;
        if (!headers)
            return 0;

        headers[headers_index] = 0;
        int  delta       = 2;
        int  empty_lines = 0;
        int  line_len    = 1;
        int  cached_len  = 0;
        char *end        = 0;
        for (int i = 0; i < headers_index; i++) {
            char c = headers[i];
            if (c == '\n') {
                if (!line_len) {
                    *len       = headers_index - i - 1;
                    *dif       = headers + i + 1;
                    headers[i] = 0;
                    break;
                } else
                    line_len = 0;
            } else
            if (c != '\r')
                line_len++;
        }
        return *dif;
    }

    void DoHeaders(request_rec *r) {
        char *ptr = headers;

        headers[headers_index] = 0;
        while ((ptr) && (*ptr)) {
            char *sep = strchr(ptr, ':');
            if (sep) {
                sep[0] = 0;
                sep++;
                // trim
                while (*sep == ' ')
                    sep++;
                char *nl = strchr(sep, '\n');
                if (nl) {
                    nl[0] = 0;
                    char *s2 = strchr(sep, '\r');
                    if (s2)
                        s2[0] = 0;
                }
                SetHeaders((const char *)ptr, (const char *)sep, r);
                if (nl) {
                    ptr = nl + 1;
                    while (*ptr == ' ')
                        ptr++;
                } else
                    break;
            } else
                break;
        }
    }

    void AddHeader(char *buffer, int size) {
        if (!headers)
            this->DoHeadersBuffer();
        if (headers_index + size < this->size) {
            memcpy(headers + headers_index, buffer, size);
            headers_index         += size;
            headers[headers_index] = 0;
        }
    }

    void DoHeadersBuffer(int size = READ_BUF_SIZE + 0xFF) {
        DoneHeaders();
        headers       = (char *)malloc(size + 1);
        headers[0]    = 0;
        headers_index = 0;
        this->size    = size;
    }

    void DoneHeaders() {
        if (headers) {
            free(headers);
            headers       = 0;
            size          = 0;
            headers_index = 0;
        }
    }

    ~WorkerProcess() {
#ifndef _WIN32
        CloseBase(Base);
#endif
        GlobalLock();
        Base->in_use = 0;
        GlobalUnlock();
        Base = 0;
        DoneHeaders();
        if (dzone) {
            apr_shm_detach(dzone);
            dzone = 0;
        }
    }
};

static AnsiString env_buffer;
static AnsiString HOME_PATH;

unsigned int murmur_hash(const void *key) {
    if (!key)
        return 0;
    int len = strlen((char *)key);
    if (!len)
        return 0;

    const unsigned int  m     = 0x5bd1e995;
    const int           r     = 24;
    unsigned int        seed  = 0x7870AAFF;
    const unsigned char *data = (const unsigned char *)key;
    //int len = strlen((const char *)data);
    if (!len)
        return 0;

    unsigned int h = seed ^ len;

    while (len >= 4) {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len  -= 4;
    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;

        case 2:
            h ^= data[1] << 8;

        case 1:
            h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

static const char *lookup_header(const char *name, request_rec *r) {
    return apr_table_get(r->headers_in, name);
}

void EnvEmulator(request_rec *r, AnsiString& result) {
    result += "HTTP_USER_AGENT=";
    result += (char *)apr_table_get(r->headers_in, "User-Agent");
    result += "\nHTTP_REFERER=";
    result += (char *)apr_table_get(r->headers_in, "Referer");
    result += "\nHTTP_COOKIE=";
    result += (char *)apr_table_get(r->headers_in, "Cookie");
    result += "\nHTTP_FORWARDED=";
    result += (char *)apr_table_get(r->headers_in, "Forwarded");
    result += "\nHTTP_HOST=";
    result += (char *)apr_table_get(r->headers_in, "Host");
    result += "\nHTTP_PROXY_CONNECTION=";
    result += (char *)apr_table_get(r->headers_in, "Proxy-Connection");
    result += "\nHTTP_ACCEPT=";
    result += (char *)apr_table_get(r->headers_in, "Accept");
    result += "\nHTTP_ACCEPT_LANGUAGE=";
    result += (char *)apr_table_get(r->headers_in, "HTTP_ACCEPT_LANGUAGE");
    result += "\nHTTP_ACCEPT_ENCODING=";
    result += (char *)apr_table_get(r->headers_in, "HTTP_ACCEPT_ENCODING");
    result += "\nHTTP_ACCEPT_CHARSET=";
    result += (char *)apr_table_get(r->headers_in, "HTTP_ACCEPT_CHARSET");
    result += "\nHTTP_CONNECTION=";
    result += (char *)apr_table_get(r->headers_in, "HTTP_CONNECTION");
    result += "\nREDIRECT_HANDLER=";
    result += (char *)apr_table_get(r->subprocess_env, "REDIRECT_HANDLER");
    result += "\nREDIRECT_STATUS=";
    result += (char *)apr_table_get(r->subprocess_env, "REDIRECT_STATUS");
    result += "\nREQUEST_METHOD=";
    result += (char *)r->method;
    result += "\nREQUEST_SCHEME=";
    result += (char *)ap_http_scheme(r);
    result += "\nREQUEST_URI=";
    result += (char *)r->uri;
    result += "\nREQUEST_FILENAME=";
    result += (char *)r->filename;
    result += "\nREMOTE_ADDR=";
    result += (char *)r->connection->remote_ip;
    result += "\nREMOTE_IDENT=";
    result += (char *)ap_get_remote_logname(r);
    result += "\nREMOTE_USER=";
    result += (char *)r->user;
    result += "\nSERVER_ADMIN=";
    result += (char *)r->server->server_admin;
    result += "\nSERVER_NAME=";
    result += (char *)ap_get_server_name(r);
    result += "\nSERVER_PORT=";
    result += AnsiString((long)ap_get_server_port(r));
    result += "\nSERVER_PROTOCOL=";
    result += (char *)r->protocol;
    result += "\nSCRIPT_FILENAME=";
    result += (char *)r->filename;
    result += "\nSERVER_SIGNATURE=";
    result += (char *)apr_table_get(r->subprocess_env, "SERVER_SIGNATURE");
    result += "\nSERVER_ADDR=";
    result += (char *)apr_table_get(r->subprocess_env, "SERVER_ADDR");
    result += "\nSERVER_PORT=";
    result += (char *)apr_table_get(r->subprocess_env, "SERVER_PORT");
    result += "\nREMOTE_PORT=";
    result += (char *)apr_table_get(r->subprocess_env, "REMOTE_PORT");
    result += "\nREDIRECT_URL=";
    result += (char *)apr_table_get(r->subprocess_env, "REDIRECT_URL");
    result += "\nSCRIPT_NAME=";
    result += (char *)apr_table_get(r->subprocess_env, "SCRIPT_NAME");
    result += "\nSERVER_SOFTWARE=";
    result += (char *)ap_get_server_banner();
    result += (char *)" ConceptWEB Embedded";
    result += "\nQUERY_STRING=";
    result += (char *)r->args;
    result += "\nIS_SUBREQ=";
    result += (char *)(r->main != NULL ? "true" : "false");
    result += "\nDOCUMENT_ROOT=";
    result += (char *)ap_document_root(r);
    result += "\nAUTH_TYPE=";
    result += (char *)r->ap_auth_type;
    result += "\nTHE_REQUEST=";
    result += (char *)r->the_request;
    result += "\nPATH_INFO=";
    if ((r->path_info) && (r->path_info[0]))
        result += (char *)r->path_info;
    else
        result += (char *)r->uri;
    result += "\nPATH_TRANSLATED=";
    result += (char *)apr_table_get(r->subprocess_env, "PATH_TRANSLATED");
    result += "\nCONTENT_TYPE=";
    result += (char *)apr_table_get(r->subprocess_env, "CONTENT_TYPE");
    result += "\nGATEWAY_INTERFACE=CGI/1.1";
}

static apr_off_t get_content_length(request_rec *r) {
    apr_off_t len = 0;

    if (r->clength > 0)
        return r->clength;
    else if (r->main == NULL) {
        const char *clp = apr_table_get(r->headers_in, "Content-Length");

        if (clp) {
            char *errp;
            if (apr_strtoff(&len, clp, &errp, 10) || *errp || (len < 0))
                len = 0;
        }
    }

    return len;
}

int check_type(apr_bucket *bucket, char *what) {
    if (!bucket->type)
        return 0;
    int res = strncmp((char *)bucket->type, what, 3);
    return !res;
}

apr_off_t getPOST3(int fd, request_rec *r) {
    apr_size_t         len;
    apr_bucket_brigade *brigade;
    apr_off_t          count_bytes = get_content_length(r);

    if (count_bytes) {
        count_bytes += 8192;

        if (ap_setup_client_block(r, REQUEST_CHUNKED_DECHUNK) != OK) {
            fprintf(stderr, "mod_concept: Error reading client block\n");
            return 0;
        }
    } else
        return 0;

    char buf[4096];
    if (ap_should_client_block(r)) {
        while ((len = ap_get_client_block(r, buf, 4096)) > 0)
            write(fd, buf, len);
    }
    return count_bytes;
}

void Print(char *str, int length, request_rec *r, WorkerProcess *wp = 0) {
    if (r) {
        if (wp) {
            wp->outputsize += length;
            if ((length > 0) && (wp->headers)) {
                wp->AddHeader(str, length);
                char *dif;
                int  len;
                if (wp->HeaderEnd(&dif, &len)) {
                    wp->DoHeaders(r);
                    if ((len) && (dif))
                        ap_rwrite(dif, len, r);
                    wp->DoneHeaders();
                }
                return;
            }
        }
        if (length > 0) {
            ap_rwrite(str, length, r);
        } else
            ap_rputs(str, r);
    }
}

#ifdef _WIN32
extern "C" {
extern BOOL WINAPI SetDllDirectoryA(LPCTSTR lpathName);
}
#endif

AnsiString GetDirectory() {
#ifdef _WIN32
    char       tempPath[8192];
    char       file_buffer[8193];
    AnsiString tp_path;

    tempPath[0] = 0;

    if (GetTempPath(8192, tempPath) > 0)
        tp_path = tempPath;

    tp_path += "\\GetConceptDirectory.tmp";

    int res = system(AnsiString("HTTPModHelper.exe > \"") + AnsiString(tp_path) + "\"");
    if (res != 0xAB)
        fprintf(stderr, "mod_concept: cannot determine the Concept instalation path, code %i\n", res);
    char *path = getenv("CONCEPT_BIN_PATH");
    if (!path) {
        FILE *in = fopen(tp_path.c_str(), "rb");
        if (in) {
            int size = fread(file_buffer, 1, 8192, in);
            if (size > 0) {
                file_buffer[size] = 0;
                path = file_buffer;
            }
            fclose(in);
        }
    }
    unlink(tp_path.c_str());

    if (path) {
 #ifdef _WIN32
        SetDllDirectoryA(path);
 #endif
        return AnsiString(path);
    } else {
        fprintf(stderr, "mod_concept: cannot determine the Concept instalation path (no environment CONCEPT_BIN_PATH is set)\n", path);
        return AnsiString("");
    }
#else
    return LINUX_PATH;
#endif
}

void CreateWorker(WorkerBase *Base) {
#ifdef _WIN32
    AnsiString stdin_pipe((long)Base->stdin_pipe[0]);
    AnsiString stdout_pipe((long)Base->stdout_pipe[1]);
    AnsiString stderr_pipe((long)STDERR_FILENO);
    AnsiString comm_pipe((long)Base->stdin_pipe[0]);
    AnsiString backcomm_pipe((long)Base->backcomm_pipe[1]);
#else
    AnsiString stderr_pipe((long)STDERR_FILENO);
#endif
    char *path = HOME_PATH.c_str();
    semp(map_lock);
    LocalBases[(unsigned long)Base] = Base;
    semv(map_lock);
#ifdef _WIN32
    AnsiString reserved = (char *)"\"";
    reserved += (char *)Base->reserved;
    reserved += (char *)"\"";
    int res = _spawnl(_P_WAIT, path, (char *)reserved.c_str() /*Base->reserved*/, stdin_pipe.c_str(), stdout_pipe.c_str(), stderr_pipe.c_str(), comm_pipe.c_str(), backcomm_pipe.c_str(), 0);
#else
    int res = 0;

    /*pid_t pid = fork();
       if (pid < 0)
        fprintf(stderr, "%s", "mod_concept: error in fork()\n");
       else
       if (!pid) {
        execl(path, (char *)Base->reserved, Base->stdin_base, Base->stdout_base, stderr_pipe.c_str(), Base->stdin_base, Base->backcomm_base, 0);
       } else {
        waitpid(pid, &res, WNOHANG);
        res=WEXITSTATUS(res);
       }*/
    apr_proc_t proc;
    apr_pool_t *pool = 0;
    apr_pool_create(&pool, NULL);
    apr_status_t rv = apr_proc_fork(&proc, pool);
    if (rv == APR_INCHILD) {
        execl(path, (char *)Base->reserved, Base->stdin_base, Base->stdout_base, stderr_pipe.c_str(), Base->stdin_base, Base->backcomm_base, 0);
    } else
    if (rv == APR_INPARENT) {
        apr_proc_wait(&proc, &res, NULL, APR_WAIT);
    } else {
        fprintf(stderr, "%s", "mod_concept: error in fork()\n");
    }
    if (pool)
        apr_pool_destroy(pool);
#endif
    semp(map_lock);
    LocalBases.erase((unsigned long)Base);
    semv(map_lock);
    GlobalLock();
    Base->thread_ended = 1;
    GlobalUnlock();

    int is_used    = 1;
    int iterations = 0;
    while (is_used) {
        GlobalLock();
        is_used = Base->in_use;
        // ensure nobody uses it anymore
        if (!is_used)
            Base->in_use = 1;
        GlobalUnlock();
        if (is_used)
#ifdef _WIN32
            Sleep(300);
#else
            usleep(300000);
#endif
        iterations++;
        if (iterations > 3)
            break;
    }
    GlobalLock();
    DoneBase(Base);
    GlobalUnlock();
    if (res < 0)
        fprintf(stderr, "mod_concept: CGI handler '%s' returned error code %i\n", path, res);
}

#ifdef _WIN32
DWORD WINAPI ModExecuteT(LPVOID userdata) {
#else
LPVOID ModExecuteT(LPVOID userdata) {
#endif
    CreateWorker((WorkerBase *)userdata);
    return 0;
}

WorkerProcess *GetWorker(request_rec *r, unsigned int hash, const char *scriptname) {
    WorkerProcess *wp = 0;

    AnsiString file(IPC_FILE_NAME);
    char       buf[0xFF];

    buf[0] = 0;
    sprintf(buf, "%u", hash);
    file += buf;

    char *filename = DO_ROOT(r->pool, file.c_str());

    if (max_processes <= 0)
        max_processes = 5;


    apr_shm_t  *t          = 0;
    apr_shm_t  *target_ref = 0;
    apr_pool_t *pool       = r->server->process->pool;

    int ap_res = apr_shm_attach(&t, filename, pool);
    if (ap_res != APR_SUCCESS) {
        unlink(filename);
        ap_res = apr_shm_create(&t, sizeof(WorkerBase) * max_processes, filename, pool);
        if (ap_res != APR_SUCCESS) {
            ap_log_error(APLOG_MARK, APLOG_CRIT, ap_res, r->server, "mod_concept: error creating shared object '%s'", filename);
            //fprintf(stderr, "mod_concept: error creating shared memory buffer\n");
            return 0;
        } else {
            memset(apr_shm_baseaddr_get(t), 0, sizeof(WorkerBase) * max_processes);
        }
    }
    target_ref = t;


    WorkerBase *base      = 0;
    WorkerBase *base_init = 0;

    int iterations = 0;
    do {
        GlobalLock();
        WorkerBase *base_list = (WorkerBase *)apr_shm_baseaddr_get(t);
        for (int i = 0; i < max_processes; i++) {
            WorkerBase *iterator = &base_list[i];
            if ((iterator->initialized) && (!iterator->in_use) && (!iterator->thread_ended) && (iterator->remaining_sessions)) {
                base         = iterator;
                base->in_use = 1;
                break;
            }
        }
        GlobalUnlock();
        // alow to unlock if just terminated
        if (!base) {
            GlobalLock();
            for (int i = 0; i < max_processes; i++) {
                WorkerBase *iterator = &base_list[i];
                if (!iterator->initialized) {
                    base      = iterator;
                    base_init = iterator;
                    InitBase(base_init, scriptname, i, hash, target_ref);
                    base->in_use = 1;
                    // we don't want to dettach this one
                    target_ref = 0;
                    break;
                }
            }
            GlobalUnlock();
        }
        iterations++;
        if (!base) {
#ifdef _WIN32
            Sleep(50);
#else
            usleep(50000);
#endif
        }
    } while ((!base) && (iterations < 100));

    if (base_init) {
#ifdef _WIN32
        DWORD  threadID;
        HANDLE handle = CreateThread(NULL, 0, ModExecuteT, (LPVOID)base_init, 0, &threadID);
#else
        pthread_t threadID;
        pthread_create(&threadID, NULL, ModExecuteT, (LPVOID)base_init);
#endif
    }

    if (base) {
        wp           = new WorkerProcess(base, target_ref);
        wp->reserved = scriptname;
        return wp;
    }
    if (target_ref)
        apr_shm_detach(target_ref);
    return 0;
}

#ifndef _WIN32
int eof(int stream) {
    struct timeval timeout;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 1;
    fd_set s;
    fd_set fds_exception;

    FD_ZERO(&s);
    FD_ZERO(&fds_exception);

    FD_SET(stream, &s);
    FD_SET(stream, &fds_exception);

    int val = select(stream + 1, &s, NULL, &fds_exception, &timeout);

    //if ((val<0) && (!errno))
    //    return 1;
    // error !
    if (val < 0)
        return -1;
    if (val) {
        if (FD_ISSET(stream, &s))
            return 0;
    }
    return 1;
}
#endif


int pipe_write(int f, char *buf, int size) {
    if (!size)
        return 0;

    int w_size = 4096;
    if (size < w_size)
        return write(f, buf, size);

    int res           = 0;
    int total_written = 0;
    while (size > 0) {
        int res_w = write(f, buf + total_written, w_size);
        if (res_w <= 0)
            return res_w;

        total_written += res_w;
        size          -= total_written;

        if (size < w_size)
            w_size = size;
    }
    return total_written;
}

int ModExecute(char *filename, request_rec *r) {
    char         run_result = -20;
    unsigned int hash       = murmur_hash(filename);

    if (!hash) {
        return -20;
    }

    WorkerProcess *wp = GetWorker(r, hash, apr_table_get(r->subprocess_env, "PATH_TRANSLATED"));

    if (wp) {
        GlobalLock();
        wp->Base->remaining_sessions = 0;
        GlobalUnlock();

        char *buf = (char *)malloc(READ_BUF_SIZE + 1);
#ifndef _WIN32
        OpenBase(wp->Base);
#endif
        int iterate = 1;
        // clean it
        while (eof(wp->Base->stdout_pipe[0]) == 0) {
            if (read(wp->Base->stdout_pipe[0], buf, READ_BUF_SIZE) <= 0) {
                iterate = 0;
                break;
            }
        }

        if ((iterate) && (wp->GetEnded()))
            iterate = 0;

        AnsiString env;
        EnvEmulator(r, env);
        env += (char *)"\nCONTENT_LENGTH=";
        env += AnsiString((long)get_content_length(r));
        env += (char *)"\n\n";

        if (iterate) {
            pipe_write(wp->Base->stdin_pipe[1], env.c_str(), env.Length());
            getPOST3(wp->Base->stdin_pipe[1], r);
        }

        int res   = 0;
        int clean = 0;

        wp->DoHeadersBuffer();
        unsigned int remaining_sessions = 1;
        while (iterate) {
            int read_op    = 0;
            int eof_result = eof(wp->Base->stdout_pipe[0]);
            if (eof_result < 0)
                break;
            while (!eof_result) {
                res = read(wp->Base->stdout_pipe[0], buf, READ_BUF_SIZE);
                if (res > 0) {
                    //if (res) {
                    read_op++;
                    buf[res] = 0;
                    Print(buf, res, r, wp);
                    //}
                } else {
                    fprintf(stderr, "mod_concept: Error in CGI/stdout (result=%i, errno=%i)\n", res, errno);
                    iterate = 0;
                    break;
                }
                eof_result = eof(wp->Base->stdout_pipe[0]);
                if (eof_result < 0)
                    break;

                if (wp->GetEnded()) {
                    free(buf);
                    delete wp;
                    return -1;
                }
            }

            if (eof_result < 0) {
                GlobalLock();
                wp->Base->remaining_sessions = 0;
                GlobalUnlock();
                iterate = 0;
                break;
            }

            if (eof_result) {
                eof_result = eof(wp->Base->backcomm_pipe[0]);
                if (eof_result < 0) {
                    GlobalLock();
                    wp->Base->remaining_sessions = 0;
                    GlobalUnlock();
                    break;
                }
                if (!eof_result) {
                    iterate = 0;
                    unsigned int total_size = 0;
                    res = read(wp->Base->backcomm_pipe[0], &total_size, sizeof(total_size));
                    if (res != sizeof(total_size)) {
                        GlobalLock();
                        wp->Base->remaining_sessions = 0;
                        GlobalUnlock();
                        break;
                    }

                    remaining_sessions = 1;
                    int res2 = read(wp->Base->backcomm_pipe[0], &remaining_sessions, sizeof(remaining_sessions));
                    if (res2 != sizeof(remaining_sessions)) {
                        GlobalLock();
                        wp->Base->remaining_sessions = 0;
                        GlobalUnlock();
                        break;
                    }
                    GlobalLock();
                    wp->Base->remaining_sessions = remaining_sessions;
                    GlobalUnlock();

                    if (res == sizeof(unsigned int)) {
                        run_result = 0;
                        //while ((!eof(wp->stdout_pipe[0])) || (wp->outputsize < total_size) || (!wp->outputsize)) {
                        eof_result = eof(wp->Base->stdout_pipe[0]);
                        if (eof_result < 0)
                            break;
                        while ((!eof_result) /* || (wp->outputsize < total_size)*/) {
                            if (wp->GetEnded())
                                break;
                            res = read(wp->Base->stdout_pipe[0], buf, sizeof(buf));
                            if (res <= 0)
                                break;
                            Print(buf, res, r, wp);
                            eof_result = eof(wp->Base->stdout_pipe[0]);
                            if (eof_result < 0)
                                break;
                        }
                    }
                    write(wp->Base->stdin_pipe[1], &remaining_sessions, sizeof(remaining_sessions));
                    clean = 1;
                    break;
                }
            }
            if (!read_op) {
                if (wp->GetEnded()) {
                    clean = 1;
                    break;
                }
#ifdef _WIN32
                Sleep(1);
#else
                usleep(1000);
#endif
            }
        }
        free(buf);
        if (run_result) {
            Print("<br /><b>ERROR :</b> Script reported errors.", -1, r);
        }
        if ((!clean) && (res <= 0)) {
            remaining_sessions = 0;
            write(wp->Base->stdin_pipe[1], &remaining_sessions, sizeof(remaining_sessions));
        }
        //if (!remaining_sessions)
        //    wp->Base->done_thread=-1;
        delete wp;
    } else {
        Print("<br /><b>ERROR :</b> Server is too busy. Please retry later.", -1, r);
        return HTTP_SERVICE_UNAVAILABLE;
    }

    const char *location = apr_table_get(r->headers_out, "Location");

    if (location && (location[0] == '/') && (r->status == 200)) {
        r->method        = apr_pstrdup(r->pool, "GET");
        r->method_number = M_GET;

        apr_table_unset(r->headers_in, "Content-Length");

        ap_internal_redirect_handler(location, r);
        return OK;
    } else if (location && (r->status == 200))     {
        return HTTP_MOVED_TEMPORARILY;
    }
    return 0;
}

int check_file(char *filename) {
    FILE *in = 0;

    //char *filename=getenv("PATH_TRANSLATED");
    in = fopen(filename, "rb");
    if (in) {
        fclose(in);
        return 1;
    }
    return 0;
}

#define CONCEPT_MAGIC_TYPE    "application/x-httpd-concept"

static void add_ssi_vars(request_rec *r) {
    apr_table_t *e = r->subprocess_env;

    if (r->path_info && (r->path_info[0] != '\0')) {
        request_rec *pa_req;

        apr_table_setn(e, "PATH_INFO", ap_escape_shell_cmd(r->pool,
                                                           r->path_info));

        pa_req = ap_sub_req_lookup_uri(ap_escape_uri(r->pool, r->path_info),
                                       r, NULL);
        if (pa_req->filename)
            apr_table_setn(e, "PATH_TRANSLATED",
                           apr_pstrcat(r->pool, pa_req->filename,
                                       pa_req->path_info, NULL));
        else {
            apr_table_setn(e, "PATH_TRANSLATED", r->filename);
            apr_table_setn(e, "PATH_INFO", r->uri);
        }
        ap_destroy_sub_req(pa_req);
    } else {
        apr_table_setn(e, "PATH_TRANSLATED", r->filename);
        apr_table_setn(e, "PATH_INFO", r->uri);
    }

    if (r->args) {
        char *arg_copy = apr_pstrdup(r->pool, r->args);

        apr_table_setn(e, "QUERY_STRING", r->args);
        ap_unescape_url(arg_copy);
        apr_table_setn(e, "QUERY_STRING_UNESCAPED",
                       ap_escape_shell_cmd(r->pool, arg_copy));
    }
}

static int mod_concept_method_handler(request_rec *r) {
    if (strcmp(r->handler, CONCEPT_MAGIC_TYPE))
        return DECLINED;

    if ((r->finfo.filetype == 0) || (!check_file(r->filename))) {
        fprintf(stderr, "script '%s' not found or unable to stat\n", r->uri);
        return HTTP_NOT_FOUND;
    }
    if (r->finfo.filetype == APR_DIR) {
        fprintf(stderr, "attempt to invoke directory '%s' as script\n", r->uri);
        return HTTP_FORBIDDEN;
    }

    ap_add_common_vars(r);
    ap_add_cgi_vars(r);
    add_ssi_vars(r);

    SetHeaders("Content-Type", "text/html", r);
    int res = ModExecute(r->filename, r);
    if (res < 0)
        fprintf(stderr, "mod_concept: Error executing script (%s), code: %i\n", r->filename, res);

    if (res > 0)
        return res;
    return OK;
}

static int concept_pre_config(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp) {
    HOME_PATH = GetDirectory();
#ifdef _WIN32
    HOME_PATH += "concept-cgi.exe";
#else
    HOME_PATH += "conceptcgi";
#endif
    return OK;
}

static void concept_child_init(apr_pool_t *p, server_rec *s) {
    char *full_name = DO_ROOT(p, IPC_FILE_LOCK);

    if (!global_mutex) {
        int rv = apr_global_mutex_child_init(&global_mutex, full_name, p);
        if (rv != APR_SUCCESS)
            ap_log_error(APLOG_MARK, APLOG_CRIT, rv, s, "mod_concept: Failed to attach to mod_concept shared memory file '%s'", full_name);
    }
}

static int concept_server_startup(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s) {
    void       *data         = NULL;
    const char *userdata_key = "mod_concept_post_config";

    apr_pool_userdata_get(&data, userdata_key, s->process->pool);
    if (data == NULL) {
        apr_pool_userdata_set((const void *)1, userdata_key, apr_pool_cleanup_null, s->process->pool);
        return OK; /* This would be the first time through */
    }
    if (!global_mutex) {
        char *full_name = DO_ROOT(ptemp, IPC_FILE_LOCK);
        unlink(full_name);
        int rv = apr_global_mutex_create(&global_mutex, full_name, APR_LOCK_DEFAULT, pconf);
        if (rv != APR_SUCCESS) {
            //fprintf(stderr, "mod_concept: error creating mutex object (%s)\n", full_name);
            ap_log_error(APLOG_MARK, APLOG_CRIT, rv, s, "mod_concept: error creating mutex object '%s'", full_name);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
#ifdef AP_NEED_SET_MUTEX_PERMS
        unixd_set_global_mutex_perms(global_mutex);
#endif
    }
    return OK;
}

static apr_status_t destroy_pool(void *s) {
    // child done !
    std::map<unsigned long, void *>::iterator it;

    semp(map_lock);

    for (it = LocalBases.begin(); it != LocalBases.end(); it++) {
        WorkerBase *Base = (WorkerBase *)(*it).second;
        if (Base) {
            GlobalLock();
            DoneBase(Base);
            GlobalUnlock();
        }
    }
    semv(map_lock);

    semdel(map_lock);
    return APR_SUCCESS;
}

static void concept_apache_child_init(apr_pool_t *pool, server_rec *s) {
    seminit(map_lock, 1);
    apr_pool_cleanup_register(pool, s, destroy_pool, destroy_pool);
}

static void mod_concept_register_hooks(apr_pool_t *p) {
    // I think this is the call to make to register a handler for method calls (GET PUT et. al.).
    // We will ask to be last so that the comment has a higher tendency to
    // go at the end.
    ap_hook_pre_config(concept_pre_config, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_child_init(concept_child_init, NULL, NULL, APR_HOOK_REALLY_FIRST);
    ap_hook_post_config(concept_server_startup, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_handler(mod_concept_method_handler, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_child_init(concept_apache_child_init, NULL, NULL, APR_HOOK_MIDDLE);
}

module AP_MODULE_DECLARE_DATA concept_module =
{
    // Only one callback function is provided.  Real
    // modules will need to declare callback functions for
    // server/directory configuration, configuration merging
    // and other tasks.
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    concept_cmds,
    mod_concept_register_hooks                          /* callback for registering hooks */
};

#ifdef _WIN32
// DLL Entry Point ... nothing ...
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void *lpReserved) {
    return 1;
}
#endif
