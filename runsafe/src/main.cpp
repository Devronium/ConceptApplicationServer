//#define DEBUG
//#define DISABLE_CHECKPOINTS

#include <stdlib.h>
#include <fcntl.h>
#ifdef _WIN32
 #include <openssl/applink.c>
#endif

#ifdef _WIN32
 #include <io.h>
 #include <winsock2.h>
 #include <sys/utime.h>
 #include <direct.h>

 #define CMP_FUNC      stricmp

 #define LIBRARY       "concept-core.2.0.dll"
 #define LIBRARY_MT    "concept-core.2.0.MT.dll"
#else
 #include <dlfcn.h>           // dlopen
 #include <sys/select.h>

 #define HANDLE            void *
 #define HMODULE           HANDLE

 #define SOCKET            int
 #define CMP_FUNC          strcmp

 #define INVALID_SOCKET    -1
 #define SOCKET_ERROR      -1
 #define DWORD             int
 #define LPVOID            void *

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>

 #include <unistd.h>
 #include <pwd.h>

 #define LIBRARY       "libconceptcore.2.0.so"
 #define LIBRARY_MT    "libconceptcore.2.0.MT/libconceptcore.2.0.so"
#endif

#include "AnsiString.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "semhh.h"

#define INI_FILE    "concept.ini"

void InitThreads();

typedef int (*GET_VARIABLE_PROC)(int operation, void *VDESC, void *CONTEXT, int Depth, char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);
typedef int (*DEBUGGER_CALLBACK)(void *VDESC, void *CONTEXT, int Depth, int line, char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE);
typedef void (*CHECK_POINT)(int status);

extern "C" {
typedef int (*API_INTERPRETER)(char *filename, char *inc_dir, char *lib_dir, void *fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent);
typedef int (*API_INTERPRETER2)(char *filename, char *inc_dir, char *lib_dir, void *fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, CHECK_POINT check_point, void *userdata);
typedef int (*API_INTERPRETER3)(char *filename);
typedef int (*API_INTERPRETER_PIPE)(int direct_pipe);
typedef int (*API_INTERPRETER_GETSOCKET)();

typedef void * (*API_EXECUTE_INIT)(char *filename, char *inc_dir, char *lib_dir, void *fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, void *userdata);
typedef int (*API_EXECUTE_RUN)(void *PTR, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, int direct_pipe);
typedef int (*API_EXECUTE_DONE)(void *PTR);
}

#define     DEBUG_RUN         0x01
#define     DEBUG_STEPINTO    0x02
#define     DEBUG_RUNTO       0x03
#define     DEBUG_STEPOVER    0x04

#define MAX_DEBUG_REQUEST     8192

int        __pipe_out = 0;
int        __pipe_in  = 0;
int        __p_apid   = 0;
HHSEM      sem_lock;
static int mt          = 0;
static int link_socket = -1;
static API_EXECUTE_INIT          Concept_Execute3_Init     = 0;
static API_EXECUTE_RUN           Concept_Execute3_RunClone = 0;
static API_EXECUTE_DONE          Concept_Execute3_Done     = 0;
static API_INTERPRETER_GETSOCKET GetSocket = 0;

static char spbk[0x101];
static char sprk[0x101];
static char cpbk[0x101];

typedef struct {
    long       CLIENT_NOTICE; // VARIABILA SETATA DE INTERPRETOR !!!

    int        last_line;
    int        debug_type;
    int        run_to_line;
    AnsiString FileName;
    AnsiString run_to_filename;
    void       *context_id;
    SOCKET     sock;
    SOCKET     DEBUG_SOCKET;
} ClientDebugInfo;

typedef struct {
    void  *handler;
    int   sock;
    int   direct_pipe;
    int   pipein;
    int   pipeout;
    int   apid;
    int   parent_apid;
    int   write_pipe;
    int   csize;
    char  key[0x1FFF];
    char  secured;
    HHSEM sem_write;
} ThreadData;

int  status      = 0;
bool check_point = false;

void CheckPoint(int _status) {
#ifndef DISABLE_CHECKPOINTS
    semp(sem_lock);
#endif
    status      = _status;
    check_point = true;
#ifndef DISABLE_CHECKPOINTS
    semv(sem_lock);
#endif
}

#ifndef _WIN32
static void sig_pipe(int signo) {
    // nothing
}
#endif

#ifdef _WIN32
BOOL sig_term(DWORD fdwCtrlType) {
    if ((link_socket > 0) && (GetSocket)) {
        shutdown(GetSocket(), SD_BOTH);
        link_socket = -1;
        return TRUE;
    }
    return FALSE;
}

#else
void sig_term(int signum) {
    if ((link_socket > 0) && (GetSocket)) {
        shutdown(GetSocket(), SHUT_RDWR);
        link_socket = -1;
    } else
        exit(-9);
}
#endif

ssize_t read_exact(int fd, void *buf, int size) {
    int  written = 0;
    char *ref    = (char *)buf;

    while (size > 0) {
        int res = read(fd, ref, size);
        if (res > 0) {
            size    -= res;
            ref     += res;
            written += res;
        } else {
            written = res;
            break;
        }
    }
    return written;
}

ssize_t write_exact(int fd, void *buf, int size) {
    int  written = 0;
    char *ref    = (char *)buf;

    while (size > 0) {
        int res = write(fd, ref, size);
        if (res > 0) {
            size    -= res;
            ref     += res;
            written += res;
        } else {
            written = res;
            break;
        }
    }
    return written;
}

#ifdef _WIN32
ssize_t pipe_fd_read(int _source_pipe, int *fd, int step) {
    if (fd) {
        intptr_t new_sock = 0;
        *fd = -1;
        if (read_exact(_source_pipe, &new_sock, sizeof(new_sock)) == sizeof(new_sock)) {
            if (new_sock >= 0) {
                if (step <= 1) {
                    *fd = new_sock;
                } else {
                    *fd = _open_osfhandle(new_sock, 0);
                }
            }
            //if (*fd < 0)
            //    *fd=_open_osfhandle(new_sock, _O_APPEND);
            return sizeof(new_sock);
        }
    }
    return -1;
}

#else
ssize_t pipe_fd_read(int _source_pipe, int *fd, int step) {
    ssize_t size;
    char    buf[16];

    if (fd) {
        *fd = -1;
        struct msghdr msg;
        struct iovec  iov;
        union {
            struct cmsghdr cmsghdr;
            char           control[CMSG_SPACE(sizeof(int))];
        } cmsgu;
        struct cmsghdr *cmsg;

        iov.iov_base = buf;
        iov.iov_len  = sizeof(buf);

        msg.msg_name       = NULL;
        msg.msg_namelen    = 0;
        msg.msg_iov        = &iov;
        msg.msg_iovlen     = 1;
        msg.msg_control    = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        size = recvmsg(_source_pipe, &msg, 0);
        if (size < 0)
            return size;

        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && (cmsg->cmsg_len == CMSG_LEN(sizeof(int)))) {
            if (cmsg->cmsg_level != SOL_SOCKET)
                return -2;

            if (cmsg->cmsg_type != SCM_RIGHTS)
                return -3;

            *fd = *((int *)CMSG_DATA(cmsg));
        } else
            *fd = -1;
    } else {
        size = read(_source_pipe, buf, sizeof(buf));
        if (size < 0)
            return size;
    }
    return size;
}
#endif

int NotifyParent(int msg_id, AnsiString DATA) {
    if ((__p_apid <= 0) || (__pipe_in <= 0))
        return 0;

    int params[3];
    params[0] = __p_apid;
    params[1] = msg_id;
    params[2] = DATA.Length();

    write_exact(__pipe_out, params, sizeof(int) * 3);

    if (params[2])
        write_exact(__pipe_out, DATA.c_str(), params[2]);

    return 1;
}

AnsiString SetWebDirectory(char *filename) {
    char       *buffer;
    AnsiString temp  = filename;
    AnsiString temp2 = filename;

    if (temp != (char *)"") {
        buffer = temp.c_str();
        for (int i = strlen(buffer) - 1; i >= 0; i--)
            if ((buffer[i] == '/') || (buffer[i] == '\\')) {
                temp2         = (char *)buffer + i + 1;
                buffer[i + 1] = 0;
                break;
            }
        if ((buffer) && (buffer[0]))
            chdir(buffer);
    }
    return temp2;
}

int need_compilation(char *filename) {
    AnsiString accel_filename = filename;

    accel_filename += ".accel";
    struct stat buf1, buf2;
    // if some error, report no need for compilation
    if (stat(filename, &buf1))
        return 0;

    if (stat(accel_filename.c_str(), &buf2))
        return 1;

    // last modification time don't match ... need recompilation ...
    if (buf1.st_mtime != buf2.st_mtime)
        return 1;

    return 0;
}

void Print(char *str, int length) {
    if (length > 0)
        fwrite(str, 1, length, stdout);
    else
    if (str)
        fprintf(stdout, "%s", str);
}

int GetParent(int *parent, int *msg_id, AnsiString *DATA) {
    if ((__p_apid <= 0) || (__pipe_out <= 0))
        return 0;

    int params[3];

    if (read_exact(__pipe_in, params, sizeof(int) * 3) != sizeof(int) * 3)
        return 0;

    *parent = params[0];
    *msg_id = params[1];

    if (params[2]) {
        char *data = new char [params[2] + 1];
        data[params[2]] = 0;
        int rd = read_exact(__pipe_in, data, params[2]);
        *DATA = data;
        delete[] data;
        if (rd != params[2])
            return 0;
    }

    return 1;
}

int TALK_TO_CLIENT(void *VDESC, void *CONTEXT, int Depth, int line, char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE) {
    ClientDebugInfo *DINFO = (ClientDebugInfo *)DEBUGGER_RESERVED;

    AnsiString SERVER_QUERY = filename;
    int        size         = SERVER_QUERY.Length() + 1 + sizeof(int);

    send(DINFO->DEBUG_SOCKET, (char *)&size, sizeof(int), 0);
    send(DINFO->DEBUG_SOCKET, (char *)&line, sizeof(int), 0);
    send(DINFO->DEBUG_SOCKET, SERVER_QUERY, SERVER_QUERY.Length() + 1, 0);

    while (1) {
        int size      = 0;
        int received  = 0;
        int rec_count = 0;
        do {
            received   = recv(DINFO->DEBUG_SOCKET, (char *)&size + rec_count, sizeof(int) - rec_count, 0);
            rec_count += received;
        } while ((rec_count < sizeof(int)) && (received > 0));

        // host is down ...
        if ((received <= 0) || (size > MAX_DEBUG_REQUEST) || (size <= 0))
            return 1;

        char *buffer = new char[size + 1];
        buffer[size] = 0;
        rec_count    = 0;
        do {
            received   = recv(DINFO->DEBUG_SOCKET, buffer + rec_count, size - rec_count, 0);
            rec_count += received;
        } while ((received > 0) && (rec_count < size));
        AnsiString command = buffer;
        delete[] buffer;

        if (received <= 0)
            return 1;

        if (command == (char *)"stepinto") {
            DINFO->debug_type    = DEBUG_STEPINTO;
            DINFO->CLIENT_NOTICE = 0;
            break;
        } else
        if (command == (char *)"stepover") {
            DINFO->context_id    = CONTEXT;
            DINFO->debug_type    = DEBUG_STEPOVER;
            DINFO->CLIENT_NOTICE = (intptr_t)CONTEXT;
            break;
        } else
        if (command == (char *)"run") {
            DINFO->debug_type    = DEBUG_RUN;
            DINFO->CLIENT_NOTICE = 0;
            break;
        } else
        if (command == (char *)"break") {
            DINFO->debug_type    = DEBUG_RUN;
            DINFO->CLIENT_NOTICE = 0;
            return 1;
            break;
        } else
        if (command.Pos((char *)"runto ") == 1) {
            AnsiString line = (char *)command.c_str() + 6;
            DINFO->debug_type      = DEBUG_RUNTO;
            DINFO->run_to_line     = line.ToInt();
            DINFO->run_to_filename = filename;
            DINFO->CLIENT_NOTICE   = 0;
            break;
        } else
        if (command.Pos((char *)"watch ") == 1) {
            AnsiString varname = (char *)command.c_str() + 6;
            char       var_buffer[MAX_DEBUG_REQUEST + 1];
            strcpy(var_buffer, "(Variable not defined)");
            var_buffer[MAX_DEBUG_REQUEST] = 0;
            if (GVP(0, VDESC, CONTEXT, Depth, varname, var_buffer, MAX_DEBUG_REQUEST, PIF, STACK_TRACE)) {
                // to do ... variable undefined ...
            }

            int res_size = strlen(var_buffer) + 1;
            send(DINFO->DEBUG_SOCKET, (char *)&res_size, sizeof(int), 0);
            send(DINFO->DEBUG_SOCKET, var_buffer, res_size, 0);
        }
    }
    return 0;
}

int DEBUGGER_TRAP(void *VDESC, void *CONTEXT, int Depth, int line, char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE) {
    ClientDebugInfo *DINFO = (ClientDebugInfo *)DEBUGGER_RESERVED;
    int             result = 0;

    switch (DINFO->debug_type) {
        case DEBUG_RUN:
            break;

        case DEBUG_RUNTO:
            if ((DINFO->run_to_line == line) && (DINFO->run_to_filename == filename))
                // Notify Client
                result = TALK_TO_CLIENT(VDESC, CONTEXT, Depth, line, filename, GVP, DEBUGGER_RESERVED, PIF, STACK_TRACE);
            break;

        case DEBUG_STEPINTO:
            if ((DINFO->last_line != line) || (DINFO->FileName != filename))
                // Notify Client
                // s-a schimbat linia ...
                result = TALK_TO_CLIENT(VDESC, CONTEXT, Depth, line, filename, GVP, DEBUGGER_RESERVED, PIF, STACK_TRACE);
            break;

        case DEBUG_STEPOVER:
            if ((DINFO->last_line != line) || (DINFO->FileName != filename))
                if ((CONTEXT == DINFO->context_id) || (DINFO->CLIENT_NOTICE == -1)) {
                    result = TALK_TO_CLIENT(VDESC, CONTEXT, Depth, line, filename, GVP, DEBUGGER_RESERVED, PIF, STACK_TRACE);
                    DINFO->CLIENT_NOTICE = 0;
                }
            break;
    }
    DINFO->FileName  = filename;
    DINFO->last_line = line;
    return result;
}

SOCKET CONTACT_DEBUGER(char *hostname, int port) {
    struct sockaddr_in sin;
    struct hostent     *hp;
    SOCKET             DEBUG_SOCKET = 0;

    if ((hp = gethostbyname(hostname)) == 0)
        return 0;

    memset(&sin, 0, sizeof(sin));
    //INADDR_ANY;
    sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(port);

    // creez socket-ul
    if ((DEBUG_SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        return 0;

    if (connect(DEBUG_SOCKET, (struct sockaddr *)&sin, sizeof(sin)) == INVALID_SOCKET)
        return 0;

    return DEBUG_SOCKET;
}

int PROCESS_COMMAND(AnsiString DATA, ClientDebugInfo *DINFO, void *CONTEXT) {
    int result = 0;

    if (DATA == (char *)"stepinto") {
        DINFO->debug_type    = DEBUG_STEPINTO;
        DINFO->CLIENT_NOTICE = 0;
    } else
    if (DATA == (char *)"stepover") {
        DINFO->context_id    = CONTEXT;
        DINFO->debug_type    = DEBUG_STEPOVER;
        DINFO->CLIENT_NOTICE = (intptr_t)CONTEXT;
    } else
    if (DATA == (char *)"run") {
        DINFO->debug_type    = DEBUG_RUN;
        DINFO->CLIENT_NOTICE = 0;
    } else
    if (DATA == (char *)"break") {
        DINFO->debug_type    = DEBUG_RUN;
        DINFO->CLIENT_NOTICE = 0;
        result = 1;
    }

    return result;
}

AnsiString REGISTERED_FILES[0xFF];
int        REGISTERED_COUNT = 0;

int IS_REGISTERED(AnsiString filename) {
    for (int i = 0; i < REGISTERED_COUNT; i++)
        if (REGISTERED_FILES[i] == filename)
            return i + 1;
    if (!REGISTERED_COUNT)
        return -1;
    return 0;
}

int REGISTER_FILE(AnsiString filename) {
    if (IS_REGISTERED(filename) > 0)
        return 0;
    REGISTERED_FILES[REGISTERED_COUNT++] == filename;
    return 1;
}

typedef struct {
    AnsiString filename;
    int        line;
} BREAKPOINT;

#define MAX_BREAKPOINTS    0xFF
BREAKPOINT breakpoints[MAX_BREAKPOINTS];
int        max_breakpoints   = 0;
int        breakpoints_count = 0;

int AddBreakpoint(char *filename, int line) {
    int target = 0;
    int i      = 0;

    for (i = 0; i < max_breakpoints; i++)
        if (breakpoints[i].line == -1)
            break;

    target = i;
    if (target >= MAX_BREAKPOINTS)
        return 0;

    breakpoints[i].filename = filename;
    breakpoints[i].line     = line;

    max_breakpoints++;
    breakpoints_count++;

    return 1;
}

int RemoveBreakpoint(char *filename, int line) {
    for (int i = 0; i < max_breakpoints; i++)
        if ((breakpoints[i].line == line) && (breakpoints[i].filename == filename)) {
            breakpoints[i].line = -1;
            breakpoints_count--;
            return 1;
        }
    return 0;
}

void ClearBreakpoints() {
    max_breakpoints   = 0;
    breakpoints_count = 0;
}

int IsBreakpoint(char *filename, int line) {
    for (int i = 0; i < max_breakpoints; i++)
        if ((breakpoints[i].line == line) && (breakpoints[i].filename == filename))
            return 1;
    return 0;
}

int CONCEPT_DEBUGGER_TRAP(void *VDESC, void *CONTEXT, int Depth, int line, char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE) {
    ClientDebugInfo *DINFO = (ClientDebugInfo *)DEBUGGER_RESERVED;

    int  result = 0;
    int  parent;
    int  msg_id;
    char buffer2[0xFFFF];

    switch (DINFO->debug_type) {
        case DEBUG_RUNTO:
            break;

        case DEBUG_RUN:
            if (!IsBreakpoint(filename, line))
                break;

        case DEBUG_STEPINTO:
            {
                AnsiString DATA;
                DATA  = filename;
                DATA += (char *)"<:>";
                DATA += AnsiString((long)line);
                NotifyParent(-0x100, DATA);

                do {
                    GetParent(&parent, &msg_id, &DATA);
                    if (msg_id == -0x100)
                        result = PROCESS_COMMAND(DATA, DINFO, CONTEXT);
                    else
                    if (msg_id == -0x101)
                        REGISTER_FILE(DATA);
                    else
                    if (msg_id == -0x102) {
                        char var_buffer[MAX_DEBUG_REQUEST + 1];
                        strcpy(var_buffer, "(Variable not defined)");
                        var_buffer[MAX_DEBUG_REQUEST] = 0;
                        if (!GVP(0, VDESC, CONTEXT, Depth, DATA, var_buffer, MAX_DEBUG_REQUEST, PIF, STACK_TRACE))
                            // to do ... variable undefined ...
                            strcpy(var_buffer, "(Variable not defined)");

                        DATA += (char *)"<:>";
                        DATA += var_buffer;
                        NotifyParent(-0x102, DATA);
                    } else
                    if (msg_id == -0x103) {
                        char var_buffer[MAX_DEBUG_REQUEST + 1];
                        int  index = DATA.Pos(":");
                        if (index > 1) {
                            AnsiString varvalue = (char *)DATA.c_str() + index;
                            DATA.c_str()[index - 1] = 0;
                            AnsiString varname = DATA.c_str();
                            // first parameter is 1, to SET variable (0 is for GET)
                            if (!GVP(1, VDESC, CONTEXT, Depth, varname, varvalue, varvalue.Length() + 1, PIF, STACK_TRACE)) {
                                // to do ... variable undefined ...
                            } else {
                                var_buffer[MAX_DEBUG_REQUEST] = 0;
                                if (!GVP(0, VDESC, CONTEXT, Depth, varname, var_buffer, MAX_DEBUG_REQUEST, PIF, STACK_TRACE)) {
                                    // to do ... variable undefined ...
                                } else {
                                    DATA += (char *)"<:>";
                                    DATA += var_buffer;
                                    NotifyParent(-0x102, DATA);
                                }
                            }
                        }
                    } else
                    if (msg_id == -0x104) {
                        GVP(-1, 0, 0, 0, 0, buffer2, 0xFFFF, PIF, STACK_TRACE);
                        NotifyParent(-0x104, buffer2);
                    } else
                    if (msg_id == -0x105) {
                        int index = DATA.Pos(":");
                        // add breakpoint
                        if (index > 1) {
                            int break_line = AnsiString((char *)DATA.c_str() + index).ToInt();
                            DATA.c_str()[index - 1] = 0;
                            AnsiString fname = DATA.c_str();
                            AddBreakpoint(fname, break_line);
                        }
                    } else
                    if (msg_id == -0x106) {
                        // remove breakpoint
                        int index = DATA.Pos(":");
                        if (index > 1) {
                            int break_line = AnsiString((char *)DATA.c_str() + index).ToInt();
                            DATA.c_str()[index - 1] = 0;
                            AnsiString fname = DATA.c_str();
                            RemoveBreakpoint(fname, break_line);
                        }
                    } else
                    if (msg_id == -0x107)
                        ClearBreakpoints();
                } while (msg_id != -0x100);
            }
            break;

        case DEBUG_STEPOVER:
            if ((DINFO->last_line != line) || (DINFO->FileName != filename))
                if ((CONTEXT == DINFO->context_id) || (DINFO->CLIENT_NOTICE == -1)) {
                    AnsiString DATA;
                    DATA  = filename;
                    DATA += (char *)"<:>";
                    DATA += AnsiString((long)line);
                    NotifyParent(-0x100, DATA);

                    do {
                        GetParent(&parent, &msg_id, &DATA);
                        if (msg_id == -0x100)
                            result = PROCESS_COMMAND(DATA, DINFO, CONTEXT);
                        else
                        if (msg_id == -0x101)
                            REGISTER_FILE(DATA);
                        else
                        if (msg_id == -0x102) {
                            char var_buffer[MAX_DEBUG_REQUEST + 1];
                            strcpy(var_buffer, "(Variable not defined)");
                            var_buffer[MAX_DEBUG_REQUEST] = 0;
                            if (!GVP(0, VDESC, CONTEXT, Depth, DATA, var_buffer, MAX_DEBUG_REQUEST, PIF, STACK_TRACE))
                                // to do ... variable undefined ...
                                strcpy(var_buffer, "(Variable not defined)");

                            DATA += (char *)"<:>";
                            DATA += var_buffer;
                            NotifyParent(-0x102, DATA);
                        } else
                        if (msg_id == -0x103) {
                            char var_buffer[MAX_DEBUG_REQUEST + 1];
                            int  index = DATA.Pos(":");
                            if (index > 1) {
                                AnsiString varvalue = (char *)DATA.c_str() + index;
                                DATA.c_str()[index - 1] = 0;
                                AnsiString varname = DATA.c_str();
                                // first parameter is 1, to SET variable (0 is for GET)
                                if (!GVP(1, VDESC, CONTEXT, Depth, varname, varvalue, varvalue.Length() + 1, PIF, STACK_TRACE)) {
                                    // to do ... variable undefined ...
                                } else {
                                    var_buffer[MAX_DEBUG_REQUEST] = 0;
                                    if (!GVP(0, VDESC, CONTEXT, Depth, varname, var_buffer, MAX_DEBUG_REQUEST, PIF, STACK_TRACE)) {
                                        // to do ... variable undefined ...
                                    } else {
                                        DATA += (char *)"<:>";
                                        DATA += var_buffer;
                                        NotifyParent(-0x102, DATA);
                                    }
                                }
                            }
                        } else
                        if (msg_id == -0x104) {
                            GVP(-1, 0, 0, 0, 0, buffer2, 0xFFFF, PIF, STACK_TRACE);
                            NotifyParent(-0x104, buffer2);
                        } else
                        if (msg_id == -0x105) {
                            int index = DATA.Pos(":");
                            // add breakpoint
                            if (index > 1) {
                                int break_line = AnsiString((char *)DATA.c_str() + index).ToInt();
                                DATA.c_str()[index - 1] = 0;
                                AnsiString fname = DATA.c_str();
                                AddBreakpoint(fname, break_line);
                            }
                        } else
                        if (msg_id == -0x106) {
                            // remove breakpoint
                            int index = DATA.Pos(":");
                            if (index > 1) {
                                int break_line = AnsiString((char *)DATA.c_str() + index).ToInt();
                                DATA.c_str()[index - 1] = 0;
                                AnsiString fname = DATA.c_str();
                                RemoveBreakpoint(fname, break_line);
                            }
                        } else
                        if (msg_id == -0x107)
                            ClearBreakpoints();
                    } while (msg_id != -0x100);

                    DINFO->CLIENT_NOTICE = 0;
                }
            break;
    }
    DINFO->FileName  = filename;
    DINFO->last_line = line;

    return result;
}

int RunDebugConceptApplication2(char *filename, char *Include, char *Library, SOCKET sock, int pipein, int pipeout, int apid, int parent_apid, char *KEY) {
    HMODULE          hDLL;
    API_INTERPRETER2 Concept_Execute;

#ifdef _WIN32
    hDLL = LoadLibrary(mt ? LIBRARY_MT : LIBRARY);
#else
    // linux module
    hDLL = dlopen(mt ? LIBRARY_PATH_MT : LIBRARY_PATH, RTLD_LAZY);
#endif
    if (!hDLL) {
        fprintf(stderr, "Error loading core library '%s'\n", LIBRARY);
        return -1;
    }

#ifdef _WIN32
    Concept_Execute = (API_INTERPRETER2)GetProcAddress(hDLL, "Concept_Execute2");
#else
    Concept_Execute = (API_INTERPRETER2)dlsym(hDLL, "Concept_Execute2");
#endif
    if (!Concept_Execute) {
        fprintf(stderr, "Corrupted library '%s'\n", LIBRARY);
#ifdef _WIN32
        FreeLibrary(hDLL);
#else
        dlclose(hDLL);
#endif
        return -1;
    }

    ClientDebugInfo DINFO;
    DINFO.CLIENT_NOTICE = 0;
    DINFO.last_line     = 0;
    DINFO.sock          = sock;
    DINFO.debug_type    = DEBUG_STEPINTO;
    DINFO.run_to_line   = 0;
    DINFO.FileName      = filename;
    DINFO.context_id    = 0;
    DINFO.DEBUG_SOCKET  = 0;

#ifdef _WIN32
    GetSocket = (API_INTERPRETER_GETSOCKET)GetProcAddress(hDLL, "Concept_GetSocket");
#else
    GetSocket = (API_INTERPRETER_GETSOCKET)dlsym(hDLL, "Concept_GetSocket");
#endif
    if (!GetSocket)
        fprintf(stderr, "Warning: '%s' has no GetSocket.\n", LIBRARY);

    int res = Concept_Execute(filename, Include, Library, (void *)Print, sock, 1, CONCEPT_DEBUGGER_TRAP, &DINFO, KEY, KEY, KEY, pipein, pipeout, apid, parent_apid, CheckPoint, 0);
    GetSocket = 0;

    if (res == -3)
        fprintf(stderr, "File not found or insufficient rights: %s.\n", filename);
#ifdef _WIN32
    FreeLibrary(hDLL);
#else
    dlclose(hDLL);
#endif
    return res;
}

AnsiString Strip(char *filename) {
    AnsiString fname = filename;

    if (fname[0] == '"') {
        int len = fname.Length();
        if (len > 2) {
            fname = filename + 1;
            fname.c_str()[len - 2] = 0;
        }
    }
    return fname;
}

void SetEnvVar(char *varname, const char *value) {
#ifdef _WIN32
    AnsiString my_str = varname;
    my_str += (char *)"=";
    my_str += (char *)value;
    _putenv(my_str.c_str());
#else
    setenv(varname, value, 1);
#endif
}

void EnvSocketVars(int socket, int secured, int debug) {
    SetEnvVar("REMOTE_SECURED", secured ? "1" : "0");
    SetEnvVar("REMOTE_DEBUG", debug ? "1" : "0");
}

#ifdef _WIN32
wchar_t *wstr(const char *utf8) {
    int     len    = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t *utf16 = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));

    if (utf16) {
        int size = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, utf16, len);
        if (size == (size_t)(-1)) {
            free(utf16);
            utf16 = NULL;
        }
    }
    return utf16;
}
#endif

intptr_t ConceptGetPrivateProfileString(
    const char *lpAppName,
    const char *lpKeyName,
    const char *lpDefault,
    char *lpReturnedString,
    long nSize,
    const char *lpFileName, int *start_pos = 0, int *end_pos = 0, int *section_start = 0, int *f_size = 0) {
    FILE *in         = 0;
    long filesize    = 0;
    char *filebuffer = 0;
    char section_name[4096];
    char key_name[4096];
    int  error = 0;
    char found = 0;

#ifdef _WIN32
    wchar_t *fname = wstr(lpFileName);
    if (fname) {
        in = _wfopen(fname, L"rb");
        free(fname);
    } else
        in = fopen(lpFileName, "rb");
#else
    in = fopen(lpFileName, "rb");
#endif
    if (in) {
        fseek(in, 0L, SEEK_END);
        filesize = ftell(in);
        if (f_size)
            *f_size = filesize;
        fseek(in, 0L, SEEK_SET);
        if (filesize) {
            filebuffer = new char[filesize];
            if (filebuffer) {
                if (fread(filebuffer, 1, filesize, in) == filesize) {
                    char in_name           = 0;
                    char in_quote          = 0;
                    char in_looked_section = 0;
                    char in_looked_key     = 0;
                    char in_looked_value   = 0;
                    char in_comment        = 0;
                    char in_key            = 0;
                    char in_escape         = 0;
                    int  temp_pos          = 0;
                    int  key_pos           = 0;
                    int  value_pos         = 0;

                    section_name[0] = 0;
                    key_name[0]     = 0;

                    int i;
                    for (i = 0; i < filesize; i++) {
                        if (in_comment) {
                            if ((filebuffer[i] == '\n') || (filebuffer[i] == '\r'))
                                in_comment = 0;
                            continue;
                        }

                        if (((filebuffer[i] == '\n') || (filebuffer[i] == '\r')) && (in_looked_value)) {
                            lpReturnedString[value_pos++] = 0;
                            if (end_pos)
                                *end_pos = i;
                            break;
                        }

                        if (!in_quote) {
                            if (filebuffer[i] == ';') {
                                if (in_looked_value)
                                    break;
                                in_comment = 1;
                                continue;
                            }

                            if (filebuffer[i] == '[') {
                                // a inceput o sectziune noua, iar sectziunea precedenta este cea cautata ...
                                if (in_looked_section) {
                                    error = 1;
                                    break;
                                }
                                temp_pos = 0;
                                in_name  = 1;
                                continue;
                            }
                            if (in_name) {
                                if (filebuffer[i] == ']') {
                                    in_name = 0;
                                    section_name[temp_pos] = 0;
                                    if (!strcmp(section_name, lpAppName)) {
                                        in_looked_section = 1;
                                        if (section_start)
                                            *section_start = i + 1;
                                    }
                                    continue;
                                }
                                if ((filebuffer[i] == '\n') || (filebuffer[i] == '\r')) {
                                    in_name = 0;
                                    continue;
                                }
                                if (in_name)
                                    section_name[temp_pos++] = filebuffer[i];
                            }
                        }

                        // schimb flag-ul de ghilimele, daca le-am gasit ...
                        if (!in_name) {
                            if ((filebuffer[i] == '\\') && (!in_escape)) {
                                if ((i < filesize - 1) && (filebuffer[i + 1] == '"')) {
                                    in_escape = 1;
                                    continue;
                                }
                            }
                            if ((filebuffer[i] == '"') && (!in_escape)) {
                                in_quote = !in_quote;
                                continue;
                            }
                            if (((filebuffer[i] == '\n') || (filebuffer[i] == '\r')) && (in_quote)) {
                                in_escape = 0;
                                in_quote  = 0;
                                continue;
                            }
                            if (in_escape)
                                in_escape = 0;
                        }

                        if (in_looked_key) {
                            if ((filebuffer[i] == '=') && (!in_looked_value)) {
                                in_looked_value = 1;
                                if ((start_pos) && (i < filesize - 1)) {
                                    *start_pos = i;
                                    if (i < filesize - 1)
                                        (*start_pos)++;
                                }
                                continue;
                            }
                            if (in_looked_value) {
                                if (value_pos < nSize - 1) {
                                    if (value_pos)
                                        lpReturnedString[value_pos++] = filebuffer[i];
                                    else if ((filebuffer[i] != ' ') && (filebuffer[i] != '\t'))
                                        lpReturnedString[value_pos++] = filebuffer[i];
                                } else {
                                    lpReturnedString[value_pos++] = 0;
                                    if (end_pos)
                                        *end_pos = i;
                                    break;
                                }
                                continue;
                            }
                        }

                        if (in_looked_section) {
                            if (((filebuffer[i] >= '0') && (filebuffer[i] <= '9')) ||
                                ((filebuffer[i] >= 'a') && (filebuffer[i] <= 'z')) ||
                                ((filebuffer[i] >= 'A') && (filebuffer[i] <= 'Z')) ||
                                (filebuffer[i] == '_') || (filebuffer[i] == '(') ||
                                (filebuffer[i] == ')') || (filebuffer[i] == '.') ||
                                (filebuffer[i] == '-')) {
                                if (in_key == 0) {
                                    key_pos = 0;
                                    in_key  = 1;
                                }
                                key_name[key_pos++] = filebuffer[i];
                            } else {
                                if (in_key) {
                                    key_name[key_pos] = 0;
                                    if (!strcmp(key_name, lpKeyName)) {
                                        in_looked_key = 1;
                                        found         = 1;
                                    }
                                    in_key = 0;
                                    if (filebuffer[i] == '=')
                                        i--;
                                }
                            }
                            // TO DO ... sunt in sectiunea cautata ...
                        }
                    }
                    // in caz ca e ultima linie din fisier ...
                    if ((i == filesize) && (in_looked_value)) {
                        lpReturnedString[value_pos++] = 0;
                        if (end_pos)
                            *end_pos = i;
                    }
                } else
                    error = 1;
            } else
                error = 1;
        } else
            error = 1;
        if ((!start_pos) || (!end_pos) || (!section_start))
            fclose(in);
    } else
        error = 1;

    if ((error) || (!found))
        strncpy(lpReturnedString, lpDefault, nSize);
    if (filebuffer) {
        delete[] filebuffer;
        filebuffer = 0;
    }
    return (intptr_t)in;
}

AnsiString GetKey(char *ini_name, char *section, char *key, char *def) {
    char value[4096];

    value[0] = 0;
    ConceptGetPrivateProfileString(section, key, def, value, 4096, ini_name);
    return AnsiString(value);
}

#ifdef _WIN32
DWORD WINAPI RunClone(void *d) {
#else
void *RunClone(void *d) {
#endif
    ThreadData *data       = (ThreadData *)d;
    HHSEM      sem_write   = data->sem_write;
    void       *handler    = data->handler;
    int        sock        = data->sock;
    int        direct_pipe = data->direct_pipe;
    int        pipein      = data->pipein;
    int        pipeout     = data->pipeout;
    int        apid        = data->apid;
    int        parent_apid = data->parent_apid;
    int        write_pipe  = data->write_pipe;

    char *SERVER_PUBLIC_KEY  = NULL;
    char *SERVER_PRIVATE_KEY = NULL;
    char *CLIENT_PUBLIC_KEY  = NULL;
    char *key = NULL;

    if (data->csize) {
        key = (char *)malloc(data->csize + 1);
        key[data->csize] = 0;
        memcpy(key, data->key, data->csize);
        if ((data->csize <= 16) || (!memcmp(data->key, "----SSL DATA----", 16))) {
            if (data->secured) {
                CLIENT_PUBLIC_KEY  = key;
                SERVER_PRIVATE_KEY = key;
            }
        } else
            CLIENT_PUBLIC_KEY = key;
        SERVER_PUBLIC_KEY = key;
    }
    free(data);

    int res = Concept_Execute3_RunClone(handler, sock, 0, 0, 0, SERVER_PUBLIC_KEY, SERVER_PRIVATE_KEY, CLIENT_PUBLIC_KEY, pipein, pipeout, apid, parent_apid, direct_pipe);
    if (key)
        free(key);
    semp(sem_write);
    int wr = write_exact(write_pipe, &apid, sizeof(apid));
    if (wr != sizeof(apid))
        fprintf(stderr, "Error in write %i\n", errno);
    semv(sem_write);
    if (res < 0)
        fprintf(stderr, "Result: %i\n", res);
    if (pipein > 0)
        close(pipein);
    if (pipeout > 0)
        close(pipeout);
    if (direct_pipe > 0)
        close(direct_pipe);
#ifdef _WIN32
    ExitThread(0);
#endif
    return 0;
}

int main3(int argc, char **argv) {
    AnsiString use_pools_str = (char *)getenv("CONCEPT_UseSharedMemoryPool");
    int        use_pools     = use_pools_str.ToInt();
    int        size          = -1;
    HMODULE    hDLL          = NULL;

    API_INTERPRETER3 Concept_CachedInit = 0;
    API_INTERPRETER3 Concept_CachedDone = 0;

    char *filename = argv[1];

    char *Include = argv[2];
    char *Library = argv[3];

    int read_pipe  = atoi(argv[4]);
    int write_pipe = atoi(argv[5]);

    if ((read_pipe <= 0) || (write_pipe <= 0)) {
        fprintf(stderr, "Invalid parameters received (pipes).\n");
        return -2;
    }

    /*int new_read_pipe = dup(read_pipe);
       int new_write_pipe = dup(write_pipe);
       close(read_pipe);
       close(write_pipe);
       read_pipe = new_read_pipe;
       write_pipe = new_write_pipe;*/

#ifdef _WIN32
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    char *concept_root = getenv("CONCEPT_ROOT");
    if (concept_root)
        chdir(concept_root);
#endif

    AnsiString manifest(filename);
    manifest += (char *)".manifest";
    mt        = GetKey(manifest.c_str(), "Application", "Multithreading", "0").ToInt();

    AnsiString concept_user = GetKey(manifest.c_str(), "Server", "ConceptUser", "");
    if (concept_user.Length()) {
#ifdef _WIN32
        int    retval = 0;
        HANDLE hToken = 0;

        if (!LogonUser(concept_user.c_str(), ".", NULL, LOGON32_LOGON_SERVICE, LOGON32_PROVIDER_DEFAULT, &hToken))
            fprintf(stderr, "Warning: Cannot run as '%s'\n", concept_user.c_str());
#else
        struct passwd *pwd = getpwnam(concept_user.c_str());
        if (pwd) {
            setuid(pwd->pw_uid);
            setgid(pwd->pw_gid);
        } else {
            fprintf(stderr, "Warning: Cannot run as '%s'\n", concept_user.c_str());
        } 
#endif
    }

    AnsiString got_file = SetWebDirectory(filename);
    filename = got_file.c_str();

#ifdef _WIN32
    hDLL = LoadLibrary(mt ? LIBRARY_MT : LIBRARY);
#else
    // linux module
    hDLL = dlopen(mt ? LIBRARY_PATH_MT : LIBRARY_PATH, RTLD_LAZY);
#endif

    if (!hDLL) {
        fprintf(stderr, "Error loading core library '%s'\n", LIBRARY);
        return -1;
    }

    use_pools = use_pools_str.ToInt();
    if (use_pools) {
#ifdef _WIN32
        Concept_CachedInit = (API_INTERPRETER3)GetProcAddress(hDLL, "Concept_CachedInit");
#else
        Concept_CachedInit = (API_INTERPRETER3)dlsym(hDLL, "Concept_CachedInit");
#endif
        if (Concept_CachedInit) {
            size = Concept_CachedInit(filename);
            if (size < 0)
                fprintf(stderr, "Caching disabled - no valid compiled version for %s\n", filename);
            else
                fprintf(stderr, "Cache init (%s), computed size: %i\n", filename, size);

#ifdef _WIN32
            Concept_CachedDone = (API_INTERPRETER3)GetProcAddress(hDLL, "Concept_CachedDone");
#else
            Concept_CachedDone = (API_INTERPRETER3)dlsym(hDLL, "Concept_CachedDone");
#endif
        }
    }

#ifdef _WIN32
    Concept_Execute3_Init = (API_EXECUTE_INIT)GetProcAddress(hDLL, "Concept_Execute3_Init");
    if (!Concept_Execute3_Init)
        Concept_Execute3_Init = (API_EXECUTE_INIT)GetProcAddress(hDLL, "_Concept_Execute3_Init");
    if (Concept_Execute3_Init) {
        Concept_Execute3_RunClone = (API_EXECUTE_RUN)GetProcAddress(hDLL, "Concept_Execute3_RunClone");
        if (!Concept_Execute3_RunClone)
            Concept_Execute3_RunClone = (API_EXECUTE_RUN)GetProcAddress(hDLL, "_Concept_Execute3_RunClone");
        Concept_Execute3_Done = (API_EXECUTE_DONE)GetProcAddress(hDLL, "Concept_Execute3_Done");
        if (!Concept_Execute3_Done)
            Concept_Execute3_Done = (API_EXECUTE_DONE)GetProcAddress(hDLL, "_Concept_Execute3_Done");
    }
#else
    Concept_Execute3_Init     = (API_EXECUTE_INIT)dlsym(hDLL, "Concept_Execute3_Init");
    Concept_Execute3_RunClone = (API_EXECUTE_RUN)dlsym(hDLL, "Concept_Execute3_RunClone");
    Concept_Execute3_Done     = (API_EXECUTE_DONE)dlsym(hDLL, "Concept_Execute3_Done");
#endif
    if ((!Concept_Execute3_Init) || (!Concept_Execute3_RunClone) || (!Concept_Execute3_Done)) {
        fprintf(stderr, "Concept core library does not support multiple users per process. Please upgrade %s.\n", LIBRARY);
        return -1;
    }

    void  *handler = Concept_Execute3_Init(filename, Include, Library, (void *)Print, 0, 0, 0, 0, NULL, NULL, NULL, -1, -1, -1, -1, 0);
    char  keybuffer[0x1FFF];
    HHSEM sem_write;
    seminit(sem_write, 1);
    int exit_code = 0;
    while (handler) {
        int sock        = -1;
        int direct_pipe = -1;
        int pipein      = -1;
        int pipeout     = -1;
        int apid        = -1;
        int parent_apid = -1;

        char *SERVER_PUBLIC_KEY  = NULL;
        char *SERVER_PRIVATE_KEY = NULL;
        char *CLIENT_PUBLIC_KEY  = NULL;

        if (read_exact(read_pipe, &apid, sizeof(apid)) != sizeof(apid)) {
            exit_code = -1;
            break;
        }

        if (apid == -1) {
            exit_code = 1;
            break;
        }

        if (read_exact(read_pipe, &parent_apid, sizeof(parent_apid)) != sizeof(parent_apid)) {
            exit_code = -2;
            break;
        }

        if (pipe_fd_read(read_pipe, &sock, 1) < 0) {
            exit_code = -3;
            break;
        }
        if (pipe_fd_read(read_pipe, &pipein, 2) < 0) {
            exit_code = -4;
            break;
        }
        if (pipe_fd_read(read_pipe, &pipeout, 3) < 0) {
            exit_code = -5;
            break;
        }
        if (pipe_fd_read(read_pipe, &direct_pipe, 4) < 0) {
            exit_code = -6;
            break;
        }

        int csize;
        if (read_exact(read_pipe, &csize, sizeof(csize)) != sizeof(csize)) {
            exit_code = -7;
            break;
        }
        char secured = 0;
        if (csize > 0) {
            // secured session
            if (csize < sizeof(keybuffer) - 1) {
                if (read_exact(read_pipe, keybuffer, csize) != csize) {
                    exit_code = -8;
                    break;
                }

                keybuffer[csize]   = 0;
                CLIENT_PUBLIC_KEY  = keybuffer;
                SERVER_PUBLIC_KEY  = keybuffer;
                SERVER_PRIVATE_KEY = keybuffer;
                secured            = 1;
            }
        } else {
            if (read_exact(read_pipe, &csize, sizeof(csize)) != sizeof(csize)) {
                exit_code = -9;
                break;
            }

            if ((csize > 0) && (csize < sizeof(keybuffer) - 1)) {
                if (read_exact(read_pipe, keybuffer, csize) != csize) {
                    exit_code = -10;
                    break;
                }
                keybuffer[csize]   = 0;
                CLIENT_PUBLIC_KEY  = keybuffer;
                SERVER_PUBLIC_KEY  = NULL;
                SERVER_PRIVATE_KEY = NULL;
            }
        }
        ThreadData *data = (ThreadData *)malloc(sizeof(ThreadData));
        if (!data) {
            exit_code = -11;
            fprintf(stderr, "Error allocating thread memory.\n");
            break;
        }
        data->sem_write   = sem_write;
        data->handler     = handler;
        data->sock        = sock;
        data->direct_pipe = direct_pipe;
        data->pipein      = pipein;
        data->pipeout     = pipeout;
        data->apid        = apid;
        data->parent_apid = parent_apid;
        data->write_pipe  = write_pipe;
        data->secured     = secured;
        if (CLIENT_PUBLIC_KEY) {
            memcpy(data->key, keybuffer, csize);
            data->key[csize] = 0;
            data->csize      = csize;
        } else {
            data->key[0] = 0;
            data->csize  = 0;
        }
#ifdef _WIN32
        DWORD  threadID;
        HANDLE h = CreateThread(NULL, 0, RunClone, data, 0, &threadID);
        if (h) {
            CloseHandle(h);
        } else {
            fprintf(stderr, "Error creating worker thread\n");
            semp(sem_write);
            write_exact(write_pipe, &apid, sizeof(apid));
            semv(sem_write);
        }
#else
        pthread_t      threadID;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if (pthread_create(&threadID, &attr, RunClone, (LPVOID)data)) {
            fprintf(stderr, "Error creating worker thread\n");
            semp(sem_write);
            write_exact(write_pipe, &apid, sizeof(apid));
            semv(sem_write);
        }
#endif
    }
    if (exit_code < 0)
        fprintf(stderr, "Worker gone, error %i, errno: %i\n", exit_code, errno);
    else
        fprintf(stderr, "Worker gone\n");

    // ugly, but safe to skip cleaning-up, the process will be closed anyway
    // if (handler)
    //     Concept_Execute3_Done(handler);

    if ((size > 0) && (Concept_CachedInit) && (Concept_CachedDone))
        Concept_CachedDone(filename);

#ifdef _WIN32
    FreeLibrary(hDLL);
    WSACleanup();
#else
    dlclose(hDLL);
#endif
    return 0;
}

char *hex_to_string(char *str, char *ref, int max_len = 0x1FFF) {
    int len = strlen(str);

    if (len / 2 > max_len)
        return NULL;
    int           idx  = 0;
    unsigned char *res = (unsigned char *)ref;
    if ((len > 0) && (len % 2 == 0)) {
        for (int i = 0; i < len; i += 2) {
            unsigned char c1 = str[i];
            unsigned char c2 = str[i + 1];
            if ((c1 >= 'a') && (c1 <= 'f'))
                c1 = 10 + c1 - 'a';
            else
            if ((c1 >= 'A') && (c1 <= 'F'))
                c1 = 10 + c1 - 'A';
            else
            if ((c1 >= '0') && (c1 <= '9'))
                c1 -= '0';
            else
                return NULL;

            if ((c2 >= 'a') && (c2 <= 'f'))
                c2 = 10 + c2 - 'a';
            else
            if ((c2 >= 'A') && (c2 <= 'F'))
                c2 = 10 + c2 - 'A';
            else
            if ((c2 >= '0') && (c2 <= '9'))
                c2 -= '0';
            else
                return NULL;

            unsigned char c = c1 * 16 + c2;
            res[idx++] = c;
        }
    }
    res[idx] = 0;
    return ref;
}

int main2(int argc, char **argv) {
    if (argc < 10) {
        fprintf(stderr, "This program is used internaly by Concept Server and should not be instantiated from shell.\n");
        return -10;
    }
    char *arguments = argv[argc - 1];

    if (arguments)
        SetEnvVar("CONCEPT_ARGUMENTS", arguments);

    AnsiString use_pools_str = (char *)getenv("CONCEPT_UseSharedMemoryPool");
    int        use_pools     = 0;
    use_pools = use_pools_str.ToInt();
#ifdef _WIN32
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    char *concept_root = getenv("CONCEPT_ROOT");
    if (concept_root)
        chdir(concept_root);
#endif

    int debug = 0;
    if ((argc >= 10) && (argv[0][1])) {
        AnsiString is_debug((char *)getenv("CONCEPT_DEBUG"));

        if (is_debug.ToInt() == 1) {
            SetEnvVar("CONCEPT_DEBUG", "-1");
            debug = 1;
        }
    } else
        debug = atoi(argv[0]);

    char *filename = argv[6];
    SetEnvVar("CONCEPT_FILENAME", filename);
    SOCKET sock        = /*dup*/ ((SOCKET)AnsiString(argv[7]).ToInt());
    char   *Include    = argv[8];
    char   *Library    = argv[9];
    char   *DEBUG_HOST = 0;
    int    DEBUG_PORT  = 2663;
    int    direct_pipe = AnsiString(argv[1]).ToInt();
    int    pipein      = AnsiString(argv[2]).ToInt();
    int    pipeout     = AnsiString(argv[3]).ToInt();
    int    apid        = AnsiString(argv[4]).ToInt();
    int    parent_apid = AnsiString(argv[5]).ToInt();

    char *SERVER_PUBLIC_KEY  = 0;
    char *SERVER_PRIVATE_KEY = 0;
    char *CLIENT_PUBLIC_KEY  = 0;

    int  autocompile = 0;

    int secured = 0;

    AnsiString manifest(filename);
    manifest += (char *)".manifest";
    mt        = GetKey(manifest.c_str(), "Application", "Multithreading", "0").ToInt();

    AnsiString got_file = SetWebDirectory(filename);
    filename = got_file.c_str();
    AnsiString temp1;
    AnsiString temp2;
    AnsiString temp3;

    __pipe_out  = pipeout;
    __pipe_in   = pipein;
    __p_apid    = parent_apid;
    link_socket = sock;

    SERVER_PUBLIC_KEY  = (argc > 10) && (argv[10][0]) ? hex_to_string(argv[10], spbk) : 0;
    SERVER_PRIVATE_KEY = (argc > 11) && (argv[11][0]) ? hex_to_string(argv[11], sprk) : 0;
    CLIENT_PUBLIC_KEY  = (argc > 12) && (argv[12][0]) ? hex_to_string(argv[12], cpbk) : 0;
    if ((SERVER_PRIVATE_KEY) && (CLIENT_PUBLIC_KEY))
        secured = 1;

    if ((debug) && (__pipe_out > 0) && (__pipe_in > 0) && (__p_apid > 0)) {
        int code = RunDebugConceptApplication2(filename, Include, Library, sock, pipein, pipeout, apid, parent_apid, secured ? SERVER_PRIVATE_KEY : NULL);

        if (code != 100)
            return 0;
    }

    HMODULE              hDLL;
    API_INTERPRETER2     Concept_Execute    = 0;
    API_INTERPRETER3     Concept_CachedInit = 0;
    API_INTERPRETER3     Concept_CachedDone = 0;
    API_INTERPRETER_PIPE SetDirectPipe      = 0;

    int size = -1;

#ifdef _WIN32
    hDLL = LoadLibrary(mt ? LIBRARY_MT : LIBRARY);
#else
    // linux module
    hDLL = dlopen(mt ? LIBRARY_PATH_MT : LIBRARY_PATH, RTLD_LAZY);
#endif

    if (!hDLL) {
        fprintf(stderr, "Error loading core library '%s'\n", LIBRARY);
        return -1;
    }

    EnvSocketVars(sock, secured, debug);
    if (use_pools) {
#ifdef _WIN32
        Concept_CachedInit = (API_INTERPRETER3)GetProcAddress(hDLL, "Concept_CachedInit");
#else
        Concept_CachedInit = (API_INTERPRETER3)dlsym(hDLL, "Concept_CachedInit");
#endif
        if (Concept_CachedInit) {
            /// to do !
            size = Concept_CachedInit(filename);
            if (size < 0)
                fprintf(stderr, "Caching disabled - no valid compiled version for %s\n", filename);
            else
                fprintf(stderr, "Cache init (%s), computed size: %i\n", filename, size);

#ifdef _WIN32
            Concept_CachedDone = (API_INTERPRETER3)GetProcAddress(hDLL, "Concept_CachedDone");
#else
            Concept_CachedDone = (API_INTERPRETER3)dlsym(hDLL, "Concept_CachedDone");
#endif
        }
    }

#ifdef _WIN32
    Concept_Execute = (API_INTERPRETER2)GetProcAddress(hDLL, "Concept_Execute2");
#else
    Concept_Execute = (API_INTERPRETER2)dlsym(hDLL, "Concept_Execute2");
#endif
    if (!Concept_Execute) {
        fprintf(stderr, "Corrupted library '%s'\n", LIBRARY);
#ifdef _WIN32
        FreeLibrary(hDLL);
#else
        dlclose(hDLL);
#endif
        return -1;
    }
#ifdef _WIN32
    SetDirectPipe = (API_INTERPRETER_PIPE)GetProcAddress(hDLL, "SetDirectPipe");
#else
    SetDirectPipe = (API_INTERPRETER_PIPE)dlsym(hDLL, "SetDirectPipe");
#endif
    if (SetDirectPipe)
        SetDirectPipe(direct_pipe);
    else
        fprintf(stderr, "Warning: '%s' has no SetDirectPipe. No persistent connections.\n", LIBRARY);

#ifdef _WIN32
    GetSocket = (API_INTERPRETER_GETSOCKET)GetProcAddress(hDLL, "Concept_GetSocket");
#else
    GetSocket = (API_INTERPRETER_GETSOCKET)dlsym(hDLL, "Concept_GetSocket");
#endif
    if (!GetSocket)
        fprintf(stderr, "Warning: '%s' has no GetSocket.\n", LIBRARY);

    InitThreads();
    int res = Concept_Execute(filename, Include, Library, (void *)Print, sock, 0, 0, 0, SERVER_PUBLIC_KEY, SERVER_PRIVATE_KEY, CLIENT_PUBLIC_KEY, pipein, pipeout, apid, parent_apid, CheckPoint, 0);
    GetSocket = 0;
    if (res) {
        switch (res) {
            case -1:
            case -2:
            case -3:
                fprintf(stderr, "Result: %i\n", res);
                break;
        }
    }
    if ((size > 0) && (Concept_CachedInit) && (Concept_CachedDone))
        Concept_CachedDone(filename);

#ifdef _WIN32
    FreeLibrary(hDLL);
    WSACleanup();
#else
    dlclose(hDLL);
#endif
    return res;
}

int  done    = 0;
long seconds = 0;

#ifdef _WIN32
DWORD WINAPI thread_watcher(void *) {
#else
void *thread_watcher(void *) {
#endif
    int done_flag  = 0;
    int ref_status = 0;
    while (!done_flag) {
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
        seconds++;

        semp(sem_lock)
        done_flag  = done;
        ref_status = status;
        if (check_point) {
            seconds     = 0;
            check_point = false;
        }
        semv(sem_lock);
        if ((ref_status > 0) && (seconds > ref_status)) {
#ifdef _WIN32
            TerminateProcess(GetCurrentProcess(), 101);
#else
            exit(101);
#endif
        }
    }
    return 0;
}

void InitThreads() {
#ifndef DISABLE_CHECKPOINTS
 #ifdef _WIN32
    DWORD threadID;
    CreateThread(NULL, 0, thread_watcher, 0, 0, &threadID);
 #else
    pthread_t threadID;
    pthread_create(&threadID, NULL, thread_watcher, 0);
 #endif
#endif
}

int main(int argc, char **argv) {
    int result;

#ifndef _WIN32
    signal(SIGPIPE, sig_pipe);
    signal(SIGTERM, sig_term);
#else
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)sig_term, TRUE);
#endif

#ifndef DISABLE_CHECKPOINTS
    seminit(sem_lock, 1);
#endif

    if (argc == 6)
        result = main3(argc, argv);
    else
        result = main2(argc, argv);
    done = 1;

#ifndef DISABLE_CHECKPOINTS
    semdel(sem_lock);
#endif
    return result;
}
