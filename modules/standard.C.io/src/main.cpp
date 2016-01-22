#ifdef _WIN32
 #define _WIN32_WINNT    0x0501
#endif
#define WITH_MMAP_FUNCTIONS
//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <utime.h>
#include <time.h>
#include <fcntl.h>

static void        **SIGNAL_LIST = NULL;
static INVOKE_CALL LocalInvoker  = NULL;

#ifdef _WIN32
 #include <windows.h>
 #include <winsock2.h>
 #include <process.h>
 #include <direct.h>
 #include <locale.h>
 #include <Psapi.h>
 #ifdef WITH_MMAP_FUNCTIONS
  #include "mman.h"
 #endif

 #define NEW_LINE           "\r\n"
 #define NEW_LINE2          "\r\n\r\n"

 #define RLIMIT_CPU         0
 #define RLIMIT_FSIZE       1
 #define RLIMIT_DATA        2
 #define RLIMIT_STACK       3
 #define RLIMIT_CORE        4

 #ifndef RLIM_INFINITY
  # define RLIM_INFINITY    (~0UL)
 #endif

 #define pipe(phandles)    _pipe(phandles, 4096, _O_BINARY)
 #define fsync        _commit
 #define fdatasync    _commit
//win32_pipe(phandles)


int win32_pipe(int *handles) {
    HANDLE read_pipe;
    HANDLE write_pipe;

    SECURITY_ATTRIBUTES saAttr;

    saAttr.nLength              = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle       = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (CreatePipe(&read_pipe, &write_pipe, &saAttr, 4096)) {
        handles[0] = _open_osfhandle((intptr_t)read_pipe, _O_RDONLY);
        handles[1] = _open_osfhandle((intptr_t)write_pipe, _O_APPEND);
        return 0;
    }
    return -1;
}

BOOL TerminateProcess2(DWORD dwProcessId, UINT uExitCode) {
    DWORD  dwDesiredAccess = PROCESS_TERMINATE;
    BOOL   bInheritHandle  = FALSE;
    HANDLE hProcess        = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);

    if (hProcess == NULL)
        return FALSE;

    BOOL result = TerminateProcess(hProcess, uExitCode);

    CloseHandle(hProcess);

    return result;
}

intptr_t spawn_mod(char *cmd, char *workingDirectory, void *environment, int fdStdIn, int fdStdOut, int fdStdErr) {
    STARTUPINFO         sInfo;
    PROCESS_INFORMATION pInfo;

    DWORD flags = 0;
    BOOL  status;
    BOOL  inherit;

    ZeroMemory(&sInfo, sizeof(sInfo));
    sInfo.cb         = sizeof(sInfo);
    sInfo.dwFlags    = STARTF_USESTDHANDLES;
    sInfo.hStdInput  = (HANDLE)_get_osfhandle(fdStdIn);
    sInfo.hStdOutput = (HANDLE)_get_osfhandle(fdStdOut);
    sInfo.hStdError  = (HANDLE)_get_osfhandle(fdStdErr);

    /*HANDLE orig;
       if (sInfo.hStdOutput  != GetStdHandle(STD_OUTPUT_HANDLE)) {
        orig=sInfo.hStdOutput;
            DuplicateHandle(GetCurrentProcess(),sInfo.hStdOutput,GetCurrentProcess(),&sInfo.hStdOutput,0,TRUE,DUPLICATE_SAME_ACCESS);
        CloseHandle(orig);
       }
       if (sInfo.hStdInput  != GetStdHandle(STD_INPUT_HANDLE)) {
        orig=sInfo.hStdInput;
            DuplicateHandle(GetCurrentProcess(),sInfo.hStdInput,GetCurrentProcess(),&sInfo.hStdInput,0,TRUE,DUPLICATE_SAME_ACCESS);
        CloseHandle(orig);
       }
       if (sInfo.hStdError  != GetStdHandle(STD_ERROR_HANDLE)) {
        orig=sInfo.hStdError;
            DuplicateHandle(GetCurrentProcess(),sInfo.hStdError,GetCurrentProcess(),&sInfo.hStdError,0,TRUE,DUPLICATE_SAME_ACCESS);
        CloseHandle(orig);
       }*/

    /*if (sInfo.hStdInput  != GetStdHandle(STD_INPUT_HANDLE)  &&
        sInfo.hStdOutput != GetStdHandle(STD_OUTPUT_HANDLE) &&
        sInfo.hStdError  != GetStdHandle(STD_ERROR_HANDLE))
            flags = CREATE_NO_WINDOW;
       else*/
    //flags = CREATE_NO_WINDOW;

    // See #3231
    if ((fdStdIn == 0) && (fdStdOut == 1) && (fdStdErr == 2)) {
        inherit = TRUE;    //FALSE;
    } else {
        inherit = TRUE;
        flags   = CREATE_NO_WINDOW;
    }

    //SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    if (!CreateProcess(NULL, cmd, /*&sa, &sa,*/ NULL, NULL, inherit, flags, environment, workingDirectory, &sInfo, &pInfo))
        return 0;

    CloseHandle(pInfo.hThread);
    return (intptr_t)pInfo.hProcess;
}

 #define fseeko    fseek
 #define ftello    ftell
#else
 #include <sys/ioctl.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/wait.h>
 #include <sys/time.h>
 #include <sys/resource.h>
 #include <sys/statvfs.h>
 #include <unistd.h>
 #include <sys/stat.h>
 #include <signal.h>
 #include <pwd.h>
 #ifdef WITH_MMAP_FUNCTIONS
  #include <sys/mman.h>
 #endif
 #include <termios.h>

 #define NEW_LINE     "\n"
 #define NEW_LINE2    "\n\n"

extern char **environ;
#endif

#ifdef __linux__
 #include <sys/prctl.h>
#endif

#ifndef RLIMIT_LOCKS
 # define RLIMIT_LOCKS    10
#endif

#ifndef RLIMIT_SIGPENDING
 # define RLIMIT_SIGPENDING    11
#endif

#ifndef RLIMIT_MSGQUEUE
 # define RLIMIT_MSGQUEUE    12
#endif

#ifndef RLIMIT_NICE
 # define RLIMIT_NICE    13
#endif

#ifndef RLIMIT_RTPRIO
 # define RLIMIT_RTPRIO    14
#endif

#ifndef RLIMIT_RTTIME
 # define RLIMIT_RTTIME    15
#endif

#ifndef RLIM_NLIMITS
 # define RLIM_NLIMITS    16
#endif

#ifndef RLIMIT_RSS
 # define RLIMIT_RSS    5
#endif

#ifndef RLIMIT_NPROC
 # define RLIMIT_NPROC    6
#endif

#ifndef RLIMIT_NOFILE
 # define RLIMIT_NOFILE    7
#endif

#ifndef RLIMIT_MEMLOCK
 # define RLIMIT_MEMLOCK    8
#endif

#ifndef RLIMIT_AS
 # define RLIMIT_AS                         9
#endif

#define RLIMIT_NOFILE_PORTABLE              100
#define RLIMIT_NPROC_PORTABLE               101
#define RLIMIT_RSS_PORTABLE                 102
#define RLIMIT_MEMLOCK_PORTABLE             103
#define RLIMIT_AS_PORTABLE                  104

#define MAXIMUM_EXEC_PARAMETERS             0x100

#define _SC_ARG_MAX                         0x0000
#define _SC_BC_BASE_MAX                     0x0001
#define _SC_BC_DIM_MAX                      0x0002
#define _SC_BC_SCALE_MAX                    0x0003
#define _SC_BC_STRING_MAX                   0x0004
#define _SC_CHILD_MAX                       0x0005
#define _SC_CLK_TCK                         0x0006
#define _SC_COLL_WEIGHTS_MAX                0x0007
#define _SC_EXPR_NEST_MAX                   0x0008
#define _SC_LINE_MAX                        0x0009
#define _SC_NGROUPS_MAX                     0x000a
#define _SC_OPEN_MAX                        0x000b
#define _SC_PASS_MAX                        0x000c
#define _SC_2_C_BIND                        0x000d
#define _SC_2_C_DEV                         0x000e
#define _SC_2_C_VERSION                     0x000f
#define _SC_2_CHAR_TERM                     0x0010
#define _SC_2_FORT_DEV                      0x0011
#define _SC_2_FORT_RUN                      0x0012
#define _SC_2_LOCALEDEF                     0x0013
#define _SC_2_SW_DEV                        0x0014
#define _SC_2_UPE                           0x0015
#define _SC_2_VERSION                       0x0016
#define _SC_JOB_CONTROL                     0x0017
#define _SC_SAVED_IDS                       0x0018
#define _SC_VERSION                         0x0019
#define _SC_RE_DUP_MAX                      0x001a
#define _SC_STREAM_MAX                      0x001b
#define _SC_TZNAME_MAX                      0x001c
#define _SC_XOPEN_CRYPT                     0x001d
#define _SC_XOPEN_ENH_I18N                  0x001e
#define _SC_XOPEN_SHM                       0x001f
#define _SC_XOPEN_VERSION                   0x0020
#define _SC_XOPEN_XCU_VERSION               0x0021
#define _SC_XOPEN_REALTIME                  0x0022
#define _SC_XOPEN_REALTIME_THREADS          0x0023
#define _SC_XOPEN_LEGACY                    0x0024
#define _SC_ATEXIT_MAX                      0x0025
#define _SC_IOV_MAX                         0x0026
#define _SC_PAGESIZE                        0x0027
#define _SC_PAGE_SIZE                       0x0028
#define _SC_XOPEN_UNIX                      0x0029
#define _SC_XBS5_ILP32_OFF32                0x002a
#define _SC_XBS5_ILP32_OFFBIG               0x002b
#define _SC_XBS5_LP64_OFF64                 0x002c
#define _SC_XBS5_LPBIG_OFFBIG               0x002d
#define _SC_AIO_LISTIO_MAX                  0x002e
#define _SC_AIO_MAX                         0x002f
#define _SC_AIO_PRIO_DELTA_MAX              0x0030
#define _SC_DELAYTIMER_MAX                  0x0031
#define _SC_MQ_OPEN_MAX                     0x0032
#define _SC_MQ_PRIO_MAX                     0x0033
#define _SC_RTSIG_MAX                       0x0034
#define _SC_SEM_NSEMS_MAX                   0x0035
#define _SC_SEM_VALUE_MAX                   0x0036
#define _SC_SIGQUEUE_MAX                    0x0037
#define _SC_TIMER_MAX                       0x0038
#define _SC_ASYNCHRONOUS_IO                 0x0039
#define _SC_FSYNC                           0x003a
#define _SC_MAPPED_FILES                    0x003b
#define _SC_MEMLOCK                         0x003c
#define _SC_MEMLOCK_RANGE                   0x003d
#define _SC_MEMORY_PROTECTION               0x003e
#define _SC_MESSAGE_PASSING                 0x003f
#define _SC_PRIORITIZED_IO                  0x0040
#define _SC_PRIORITY_SCHEDULING             0x0041
#define _SC_REALTIME_SIGNALS                0x0042
#define _SC_SEMAPHORES                      0x0043
#define _SC_SHARED_MEMORY_OBJECTS           0x0044
#define _SC_SYNCHRONIZED_IO                 0x0045
#define _SC_TIMERS                          0x0046
#define _SC_GETGR_R_SIZE_MAX                0x0047
#define _SC_GETPW_R_SIZE_MAX                0x0048
#define _SC_LOGIN_NAME_MAX                  0x0049
#define _SC_THREAD_DESTRUCTOR_ITERATIONS    0x004a
#define _SC_THREAD_KEYS_MAX                 0x004b
#define _SC_THREAD_STACK_MIN                0x004c
#define _SC_THREAD_THREADS_MAX              0x004d
#define _SC_TTY_NAME_MAX                    0x004e
#define _SC_THREADS                         0x004f
#define _SC_THREAD_ATTR_STACKADDR           0x0050
#define _SC_THREAD_ATTR_STACKSIZE           0x0051
#define _SC_THREAD_PRIORITY_SCHEDULING      0x0052
#define _SC_THREAD_PRIO_INHERIT             0x0053
#define _SC_THREAD_PRIO_PROTECT             0x0054
#define _SC_THREAD_SAFE_FUNCTIONS           0x0055
#define _SC_NPROCESSORS_CONF                0x0060
#define _SC_NPROCESSORS_ONLN                0x0061
#define _SC_PHYS_PAGES                      0x0062
#define _SC_AVPHYS_PAGES                    0x0063
#define _SC_MONOTONIC_CLOCK                 0x0064

#ifndef F_RDLCK
 #define F_RDLCK                            1
#endif
#ifndef F_WRLCK
 #define F_WRLCK                            2
#endif
#ifndef F_UNLCK
 #define F_UNLCK                            3
#endif

#ifndef F_GETLK
 #define F_GETLK     5
#endif
#ifndef F_SETLK
 #define F_SETLK     6
#endif
#ifndef F_SETLKW
 #define F_SETLKW    7
#endif

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
//-----------------------------------------------------------------------------------
char **GetParamList(void *arr, INVOKE_CALL _Invoke, char *command) {
    INTEGER type      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;
    char    **ret     = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    ret            = new char * [count + 2];
    ret[0]         = command;
    ret[count + 1] = 0;

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i + 1] = szData;
            } else
                ret[i + 1] = "";
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------------
#ifdef _WIN32
char **GetParamList2(void *arr, INVOKE_CALL _Invoke, char *command) {
    INTEGER type      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;
    char    **ret     = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    ret = new char * [count + 2];
    int  clen  = strlen(command);
    char *cout = new char[clen + 1];
    memcpy(cout, command, clen);
    cout[clen]     = 0;
    ret[0]         = cout;
    ret[count + 1] = 0;

    AnsiString quote("\"");
    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);

            if (type == VARIABLE_STRING) {
                int        len = (int)nData;
                AnsiString s;
                char       has_space = 0;
                for (int j = 0; j < len; j++) {
                    char c = szData[j];
                    switch (c) {
                        case ' ':
                            has_space = 1;
                            s        += c;
                            break;

                        case '"':
                            s        += '\\';
                            s        += c;
                            has_space = 1;
                            break;

                        default:
                            s += c;
                    }
                }

                if ((has_space) || (!len))
                    s = quote + s + quote;

                len = s.Length();

                char *out = new char[len + 1];
                if (len)
                    memcpy(out, s.c_str(), len);
                out[len]   = 0;
                ret[i + 1] = out;
            } else {
                char *out = new char[1];
                out[0] = 0;

                ret[i + 1] = out;
            }
        }
    }
    return ret;
}
#endif
//-----------------------------------------------------------------------------------
//#ifndef _WIN32
intptr_t MyGetPrivateProfileString(
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

int VisNumber(char *value) {
    int len = strlen(value);

    for (int i = 0; i < len; i++) {
        char c = value[i];
        if ((c < '0') || (c > '9'))
            return 0;
    }
    return 1;
}

long MySetPrivateProfileString(char *section, char *key, char *value, char *ini_name) {
    FILE       *in;
    int        start_pos = 0, end_pos = 0, section_start = 0, filesize = 0;
    char       old_value[4096];
    AnsiString pre_data;

    old_value[0] = 0;
    in           = (FILE *)MyGetPrivateProfileString(section, key, "", old_value, 4096, ini_name, &start_pos, &end_pos, &section_start, &filesize);
    AnsiString val;
    if (VisNumber(value))
        val = value;
    else {
        val  = "\"";
        val += value;
        val += "\"";
    }

    if (in) {
        int delta = 0;
        if ((start_pos) && (end_pos)) {
            char *buf = new char [start_pos + 1];
            buf[start_pos] = 0;
            fseek(in, 0, SEEK_SET);
            if (fread(buf, 1, start_pos, in) == start_pos) {
                pre_data.LoadBuffer(buf, start_pos);
            } else {
                delete[] buf;
                fclose(in);
                return 0;
            }
            delete[] buf;
            fseek(in, end_pos, SEEK_SET);
            delta     = filesize - end_pos;
            pre_data += (char *)"\t";
            if (value)
                pre_data += val;
            else
                pre_data += (char *)"\"\"";

            buf        = new char [delta + 1];
            buf[delta] = 0;

            if (fread(buf, 1, delta, in) == delta) {
                pre_data.AddBuffer(buf, delta);
            } else {
                delete[] buf;
                fclose(in);
                return 0;
            }
            delete[] buf;
        } else
        if (section_start) {
            char *buf = new char [section_start + 1];
            buf[section_start] = 0;
            fseek(in, 0L, SEEK_SET);
            if (fread(buf, 1, section_start, in) == section_start) {
                pre_data.LoadBuffer(buf, section_start);
            } else {
                delete[] buf;
                fclose(in);
                return 0;
            }
            delete[] buf;

            delta = filesize - section_start;
            if (key) {
                pre_data += (char *)NEW_LINE2;
                pre_data += key;
                pre_data += (char *)"\t=\t";
                if (value)
                    pre_data += val;
                else
                    pre_data += (char *)"\"\"";
            }
            buf        = new char [delta + 1];
            buf[delta] = 0;
            if (fread(buf, 1, delta, in) == delta) {
                pre_data.AddBuffer(buf, delta);
            } else {
                delete[] buf;
                fclose(in);
                return 0;
            }
            delete[] buf;
        } else {
            fclose(in);
            in = 0;
            pre_data.LoadFile(ini_name);
            if ((section) && (section[0])) {
                pre_data += (char *)NEW_LINE;
                pre_data += (char *)"[";
                pre_data += section;
                pre_data += (char *)"]";
                pre_data += NEW_LINE;
            }
            if (key) {
                pre_data += (char *)NEW_LINE;
                pre_data += key;
                pre_data += (char *)"\t=\t";
                if (value)
                    pre_data += val;
                else
                    pre_data += (char *)"\"\"";
            }
        }
        if (in)
            fclose(in);
        if (pre_data.SaveFile(ini_name) == 0)
            return 1;
    }
    return 0;
}

//#endif
//-----------------------------------------------------------------------------------
AnsiString GetKey(char *ini_name, char *section, char *key, char *def) {
    char value[4096];

    value[0] = 0;
    MyGetPrivateProfileString(section, key, def, value, 4096, ini_name);
    return AnsiString(value);
}

//-----------------------------------------------------------------------------------
long SetKey(char *ini_name, char *section, char *key, char *value) {
    return MySetPrivateProfileString(section, key, value, ini_name);
}

//-----------------------------------------------------------------------------------
#ifdef __GNUC__
static void __cpuid(unsigned int *CPUInfo, int func) {
    __asm__ __volatile__
    (
        "cpuid"
        : "=a" (CPUInfo[0])
        , "=b" (CPUInfo[1])
        , "=c" (CPUInfo[2])
        , "=d" (CPUInfo[3])
        : "a"  (func)
    );
}
#endif
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    LocalInvoker = Invoke;
    AnsiString value = SEEK_SET;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SEEK_SET", value.c_str());
    value = SEEK_CUR;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SEEK_CUR", value.c_str());
    value = SEEK_END;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SEEK_END", value.c_str());

    value = (long)S_IFIFO;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IFIFO", value.c_str());
    value = (long)S_IFCHR;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IFCHR", value.c_str());
    value = (long)S_IFBLK;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IFBLK", value.c_str());
    value = (long)S_IFDIR;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IFDIR", value.c_str());
    value = (long)S_IFREG;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IFREG", value.c_str());
    value = (long)S_IFMT;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IFMT", value.c_str());
    value = (long)S_IEXEC;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IEXEC", value.c_str());
    value = (long)S_IWRITE;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IWRITE", value.c_str());
    value = (long)S_IREAD;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IREAD", value.c_str());
    value = (long)S_IRWXU;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IRWXU", value.c_str());
    value = (long)S_IXUSR;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IXUSR", value.c_str());
    value = (long)S_IWUSR;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IWUSR", value.c_str());
    value = (long)S_IRUSR;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "S_IRUSR", value.c_str());

    value = (long)fileno(stdin);
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "stdin", value.c_str());

    value = (long)fileno(stdout);
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "stdout", value.c_str());

    value = (long)fileno(stderr);
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "stderr", value.c_str());

    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "ON_MSWINDOWS", "_MSWINDOWS()");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SERVER_BITS", "_BITS()");

    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGHUP", "1");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGINT", "2");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGQUIT", "3");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGILL", "4");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGABRT", "6");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGFPE", "8");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGKILL", "9");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGSEGV", "10");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGPIPE", "13");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGALRM", "14");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGTERM", "15");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "SIGCHLD", "17");

    DEFINE_ECONSTANT(RLIMIT_CPU)
    DEFINE_ECONSTANT(RLIMIT_FSIZE)
    DEFINE_ECONSTANT(RLIMIT_DATA)
    DEFINE_ECONSTANT(RLIMIT_STACK)
    DEFINE_ECONSTANT(RLIMIT_CORE)

    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "RLIMIT_RSS", "102");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "RLIMIT_NPROC", "101");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "RLIMIT_NOFILE", "100");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "RLIMIT_MEMLOCK", "103");
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "RLIMIT_AS", "104");

    DEFINE_ECONSTANT(RLIMIT_LOCKS)
    DEFINE_ECONSTANT(RLIMIT_SIGPENDING)
    DEFINE_ECONSTANT(RLIMIT_MSGQUEUE)
    DEFINE_ECONSTANT(RLIMIT_NICE)
    DEFINE_ECONSTANT(RLIMIT_RTPRIO)
    DEFINE_ECONSTANT(RLIMIT_RTTIME)
    DEFINE_ECONSTANT(RLIM_NLIMITS)

    DEFINE_ECONSTANT(_SC_ARG_MAX)
    DEFINE_ECONSTANT(_SC_BC_BASE_MAX)
    DEFINE_ECONSTANT(_SC_BC_DIM_MAX)
    DEFINE_ECONSTANT(_SC_BC_SCALE_MAX)
    DEFINE_ECONSTANT(_SC_BC_STRING_MAX)
    DEFINE_ECONSTANT(_SC_CHILD_MAX)
    DEFINE_ECONSTANT(_SC_CLK_TCK)
    DEFINE_ECONSTANT(_SC_COLL_WEIGHTS_MAX)
    DEFINE_ECONSTANT(_SC_EXPR_NEST_MAX)
    DEFINE_ECONSTANT(_SC_LINE_MAX)
    DEFINE_ECONSTANT(_SC_NGROUPS_MAX)
    DEFINE_ECONSTANT(_SC_OPEN_MAX)
    DEFINE_ECONSTANT(_SC_PASS_MAX)
    DEFINE_ECONSTANT(_SC_2_C_BIND)
    DEFINE_ECONSTANT(_SC_2_C_DEV)
    DEFINE_ECONSTANT(_SC_2_C_VERSION)
    DEFINE_ECONSTANT(_SC_2_CHAR_TERM)
    DEFINE_ECONSTANT(_SC_2_FORT_DEV)
    DEFINE_ECONSTANT(_SC_2_FORT_RUN)
    DEFINE_ECONSTANT(_SC_2_LOCALEDEF)
    DEFINE_ECONSTANT(_SC_2_SW_DEV)
    DEFINE_ECONSTANT(_SC_2_UPE)
    DEFINE_ECONSTANT(_SC_2_VERSION)
    DEFINE_ECONSTANT(_SC_JOB_CONTROL)
    DEFINE_ECONSTANT(_SC_SAVED_IDS)
    DEFINE_ECONSTANT(_SC_VERSION)
    DEFINE_ECONSTANT(_SC_RE_DUP_MAX)
    DEFINE_ECONSTANT(_SC_STREAM_MAX)
    DEFINE_ECONSTANT(_SC_TZNAME_MAX)
    DEFINE_ECONSTANT(_SC_XOPEN_CRYPT)
    DEFINE_ECONSTANT(_SC_XOPEN_ENH_I18N)
    DEFINE_ECONSTANT(_SC_XOPEN_SHM)
    DEFINE_ECONSTANT(_SC_XOPEN_VERSION)
    DEFINE_ECONSTANT(_SC_XOPEN_XCU_VERSION)
    DEFINE_ECONSTANT(_SC_XOPEN_REALTIME)
    DEFINE_ECONSTANT(_SC_XOPEN_REALTIME_THREADS)
    DEFINE_ECONSTANT(_SC_XOPEN_LEGACY)
    DEFINE_ECONSTANT(_SC_ATEXIT_MAX)
    DEFINE_ECONSTANT(_SC_IOV_MAX)
    DEFINE_ECONSTANT(_SC_PAGESIZE)
    DEFINE_ECONSTANT(_SC_PAGE_SIZE)
    DEFINE_ECONSTANT(_SC_XOPEN_UNIX)
    DEFINE_ECONSTANT(_SC_XBS5_ILP32_OFF32)
    DEFINE_ECONSTANT(_SC_XBS5_ILP32_OFFBIG)
    DEFINE_ECONSTANT(_SC_XBS5_LP64_OFF64)
    DEFINE_ECONSTANT(_SC_XBS5_LPBIG_OFFBIG)
    DEFINE_ECONSTANT(_SC_AIO_LISTIO_MAX)
    DEFINE_ECONSTANT(_SC_AIO_MAX)
    DEFINE_ECONSTANT(_SC_AIO_PRIO_DELTA_MAX)
    DEFINE_ECONSTANT(_SC_DELAYTIMER_MAX)
    DEFINE_ECONSTANT(_SC_MQ_OPEN_MAX)
    DEFINE_ECONSTANT(_SC_MQ_PRIO_MAX)
    DEFINE_ECONSTANT(_SC_RTSIG_MAX)
    DEFINE_ECONSTANT(_SC_SEM_NSEMS_MAX)
    DEFINE_ECONSTANT(_SC_SEM_VALUE_MAX)
    DEFINE_ECONSTANT(_SC_SIGQUEUE_MAX)
    DEFINE_ECONSTANT(_SC_TIMER_MAX)
    DEFINE_ECONSTANT(_SC_ASYNCHRONOUS_IO)
    DEFINE_ECONSTANT(_SC_FSYNC)
    DEFINE_ECONSTANT(_SC_MAPPED_FILES)
    DEFINE_ECONSTANT(_SC_MEMLOCK)
    DEFINE_ECONSTANT(_SC_MEMLOCK_RANGE)
    DEFINE_ECONSTANT(_SC_MEMORY_PROTECTION)
    DEFINE_ECONSTANT(_SC_MESSAGE_PASSING)
    DEFINE_ECONSTANT(_SC_PRIORITIZED_IO)
    DEFINE_ECONSTANT(_SC_PRIORITY_SCHEDULING)
    DEFINE_ECONSTANT(_SC_REALTIME_SIGNALS)
    DEFINE_ECONSTANT(_SC_SEMAPHORES)
    DEFINE_ECONSTANT(_SC_SHARED_MEMORY_OBJECTS)
    DEFINE_ECONSTANT(_SC_SYNCHRONIZED_IO)
    DEFINE_ECONSTANT(_SC_TIMERS)
    DEFINE_ECONSTANT(_SC_GETGR_R_SIZE_MAX)
    DEFINE_ECONSTANT(_SC_GETPW_R_SIZE_MAX)
    DEFINE_ECONSTANT(_SC_LOGIN_NAME_MAX)
    DEFINE_ECONSTANT(_SC_THREAD_DESTRUCTOR_ITERATIONS)
    DEFINE_ECONSTANT(_SC_THREAD_KEYS_MAX)
    DEFINE_ECONSTANT(_SC_THREAD_STACK_MIN)
    DEFINE_ECONSTANT(_SC_THREAD_THREADS_MAX)
    DEFINE_ECONSTANT(_SC_TTY_NAME_MAX)
    DEFINE_ECONSTANT(_SC_THREADS)
    DEFINE_ECONSTANT(_SC_THREAD_ATTR_STACKADDR)
    DEFINE_ECONSTANT(_SC_THREAD_ATTR_STACKSIZE)
    DEFINE_ECONSTANT(_SC_THREAD_PRIORITY_SCHEDULING)
    DEFINE_ECONSTANT(_SC_THREAD_PRIO_INHERIT)
    DEFINE_ECONSTANT(_SC_THREAD_PRIO_PROTECT)
    DEFINE_ECONSTANT(_SC_THREAD_SAFE_FUNCTIONS)
    DEFINE_ECONSTANT(_SC_NPROCESSORS_CONF)
    DEFINE_ECONSTANT(_SC_NPROCESSORS_ONLN)
    DEFINE_ECONSTANT(_SC_PHYS_PAGES)
    DEFINE_ECONSTANT(_SC_AVPHYS_PAGES)
    DEFINE_ECONSTANT(_SC_MONOTONIC_CLOCK)

    DEFINE_ECONSTANT(F_RDLCK)
    DEFINE_ECONSTANT(F_WRLCK)
    DEFINE_ECONSTANT(F_UNLCK)
    DEFINE_ECONSTANT(F_SETLK)
    DEFINE_ECONSTANT(F_SETLKW)

#ifdef WITH_MMAP_FUNCTIONS
    DEFINE_ECONSTANT(PROT_NONE)
    DEFINE_ECONSTANT(PROT_READ)
    DEFINE_ECONSTANT(PROT_WRITE)
    DEFINE_ECONSTANT(PROT_EXEC)

    DEFINE_ECONSTANT(MAP_FILE)
    DEFINE_ECONSTANT(MAP_SHARED)
    DEFINE_ECONSTANT(MAP_PRIVATE)
 #ifdef MAP_TYPE
    DEFINE_ECONSTANT(MAP_TYPE)
 #endif
    DEFINE_ECONSTANT(MAP_FIXED)
    DEFINE_ECONSTANT(MAP_ANONYMOUS)
    DEFINE_ECONSTANT(MAP_ANON)

    DEFINE_ECONSTANT(MAP_FAILED)

    DEFINE_ECONSTANT(MS_ASYNC)
    DEFINE_ECONSTANT(MS_SYNC)
    DEFINE_ECONSTANT(MS_INVALIDATE)
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        if (SIGNAL_LIST) {
            free(SIGNAL_LIST);
            SIGNAL_LIST = NULL;
        }
        LocalInvoker = NULL;
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fopen CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)": 'fopen' parameters error. This fuction takes 2 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FILE *_C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"fopen: parameter 1 should be of STATIC STRING type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &szParam1, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"fopen: parameter 2 should be of STATIC STRING type";

    // function call
#ifdef _WIN32
    wchar_t *fname = wstr(szParam0);
    if (fname) {
        wchar_t *otype = wstr(szParam1);
        _C_call_result = _wfopen(fname, otype);
        free(fname);
        if (otype)
            free(otype);
    } else
        _C_call_result = fopen((char *)szParam0, "rb");
#else
    _C_call_result = (FILE *)fopen((char *)szParam0, (char *)szParam1);
#endif

    SetVariable(RESULT, VARIABLE_NUMBER, "", (SYS_INT)_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fclose CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'fclose' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    int _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fclose: parameter 1 should be of STATIC NUMBER type";

    if (!nParam0) {
        RETURN_NUMBER(0);
        return 0;
    }

    int fno = (int)nParam0;
    switch (fno) {
        case STDOUT_FILENO:
            nParam0 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam0 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam0 = (SYS_INT)stderr;
            break;
    }
    // function call
    _C_call_result = (int)fclose((FILE *)(SYS_INT)nParam0);

    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], VARIABLE_NUMBER, "", 0);
    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fread CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 4)
        return (void *)": 'fread' parameters error. This fuction takes 4 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    size_t _C_call_result;
    // Specific variables
    char   *szParam0;
    NUMBER nParam1;
    NUMBER nParam2;
    NUMBER nParam3;

    // Variable type check
    // Parameter 1

    /*GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0]-1], &TYPE, &szParam0, &NUMBER_DUMMY);
       if ((TYPE!=VARIABLE_NUMBER) && (TYPE!=VARIABLE_STRING))
       return (void *)"fread: parameter 1 should be of STATIC STRING type";*/
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fread: parameter 2 should be of STATIC NUMBER type";
    // Parameter 3
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &STRING_DUMMY, &nParam2);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fread: parameter 3 should be of STATIC NUMBER type";
    // Parameter 4
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], &TYPE, &STRING_DUMMY, &nParam3);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fread: parameter 4 should be of STATIC NUMBER type";

    if ((int)nParam1 * (int)nParam2 == 0) {
        SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
        return 0;
    }

    int len = (int)nParam1 * (int)nParam2;
    CORE_NEW(len + 1, szParam0);
    if (!szParam0)
        return (void *)"fread: Not enough memory";

    //szParam0=new char[len+1];
    szParam0[0] = szParam0[len] = 0;

    int fno = (int)nParam3;
    switch (fno) {
        case STDOUT_FILENO:
            nParam3 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam3 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam3 = (SYS_INT)stderr;
            break;
    }

    // function call
    _C_call_result = (size_t)fread((void *)szParam0, (size_t)nParam1, (size_t)nParam2, (FILE *)(SYS_INT)nParam3);

#if 0
//        SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0]-1],VARIABLE_STRING,szParam0,_C_call_result);
//        delete[] szParam0;
#else
    // link variable. Concept Core will free it
    if (_C_call_result > 0) {
        SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], -1, szParam0, _C_call_result * (size_t)nParam1);
    } else {
        CORE_DELETE(szParam0);
        SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], VARIABLE_STRING, "", (SYS_INT)0);
    }
#endif

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fwrite CONCEPT_API_PARAMETERS {
    if ((PARAMETERS->COUNT < 4) || (PARAMETERS->COUNT > 5))
        return (void *)": 'fwrite' parameters error. This fuction takes 4 parameters.";

    // General variables
    char    *STRING_DUMMY;
    NUMBER  NUMBER_DUMMY;
    INTEGER TYPE;

    // Result
    size_t _C_call_result;
    size_t offset = 0;
    // Specific variables
    char   *szParam0;
    NUMBER nParam1;
    NUMBER nParam2;
    NUMBER nParam3;
    NUMBER nLen;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &nLen);
    if (TYPE != VARIABLE_STRING)
        return (void *)"fwrite: parameter 1 should be of STATIC STRING type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fwrite: parameter 2 should be of STATIC NUMBER type";
    // Parameter 3
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &STRING_DUMMY, &nParam2);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fwrite: parameter 3 should be of STATIC NUMBER type";
    // Parameter 4
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[3] - 1], &TYPE, &STRING_DUMMY, &nParam3);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fwrite: parameter 4 should be of STATIC NUMBER type";

    if (PARAMETERS->COUNT == 5) {
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[4] - 1], &TYPE, &STRING_DUMMY, &NUMBER_DUMMY);
        if (TYPE != VARIABLE_NUMBER)
            return (void *)"fwrite: parameter 5 should be of STATIC NUMBER type";
        offset = (size_t)NUMBER_DUMMY;
        if (offset < 0)
            offset = 0;
        if (offset >= nLen) {
            SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
            return 0;
        }
    }

    int fno = (int)nParam3;
    switch (fno) {
        case STDOUT_FILENO:
            nParam3 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam3 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam3 = (SYS_INT)stderr;
            break;
    }
    // function call
    NUMBER actual_size = nParam1 * nParam2;
    if (offset) {
        nLen     -= offset;
        szParam0 += offset;
    }
    if (actual_size > nLen) {
        SetVariable(RESULT, VARIABLE_NUMBER, "", -2);
        return 0;
    }

    _C_call_result = (size_t)fwrite((void *)szParam0, (size_t)nParam1, (size_t)nParam2, (FILE *)(SYS_INT)nParam3);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fseek CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 3)
        return (void *)": 'fseek' parameters error. This fuction takes 3 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    int _C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fseek: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &STRING_DUMMY, &nParam1);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fseek: parameter 2 should be of STATIC NUMBER type";
    // Parameter 3
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &STRING_DUMMY, &nParam2);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fseek: parameter 3 should be of STATIC NUMBER type";

    int fno = (int)nParam0;
    switch (fno) {
        case STDOUT_FILENO:
            nParam0 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam0 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam0 = (SYS_INT)stderr;
            break;
    }
    // function call
    _C_call_result = (int)fseeko((FILE *)(SYS_INT)nParam0, nParam1, (int)nParam2);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__ftell CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'ftell' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    long _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"ftell: parameter 1 should be of STATIC NUMBER type";

    int fno = (int)nParam0;
    switch (fno) {
        case STDOUT_FILENO:
            nParam0 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam0 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam0 = (SYS_INT)stderr;
            break;
    }
    // function call
    _C_call_result = ftello((FILE *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fsize CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'fsize' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    long _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"ftell: parameter 1 should be of STATIC NUMBER type";

    long old_position = ftell((FILE *)(SYS_INT)nParam0);

    int fno = (int)nParam0;
    switch (fno) {
        case STDOUT_FILENO:
            nParam0 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam0 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam0 = (SYS_INT)stderr;
            break;
    }
    fseek((FILE *)(SYS_INT)nParam0, 0, SEEK_END);
    // function call
    _C_call_result = (long)ftell((FILE *)(SYS_INT)nParam0);
    fseek((FILE *)(SYS_INT)nParam0, old_position, SEEK_SET);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fgetstring CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 3)
        return (void *)": 'fgetstring' parameters error. This fuction takes 3 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    int _C_call_result;
    // Specific variables
    NUMBER nParam0;
    char   *szParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fgetstring: parameter 1 should be of STATIC NUMBER type";
    // Parameter 2
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &szParam1, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"fgetstring: parameter 2 should be of STATIC STRING type";
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], &TYPE, &STRING_DUMMY, &nParam2);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"fgetstring: parameter 3 should be of STATIC NUMBER type";

    if ((!szParam1) || (!szParam1[0])) {
        return (void *)"fgetstring: separator can't be empty";
    }

    if ((nParam2 <= 0) || (nParam2 > 0xFFFF)) {
        return (void *)"fgetstring: maximum length should be between 1 and 0xFFFF";
    }

    // function call
    _C_call_result = 0;

    char *result = new char[(int)nParam2 + 1];
    result[0] = 0;
    int pos = 0;
    int fno = (int)nParam0;
    switch (fno) {
        case STDOUT_FILENO:
            nParam0 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam0 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam0 = (SYS_INT)stderr;
            break;
    }

    FILE *fin = (FILE *)(SYS_INT)nParam0;
    while (!feof(fin)) {
        char c = fgetc(fin);
        if (strchr(szParam1, c)) {
            if (result[0] != 0)
                break;
        } else {
            result[pos++] = c;
            result[pos]   = 0;
        }
    }
    SetVariable(RESULT, VARIABLE_STRING, result, pos);

    delete[] result;
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__feof CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'feof' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    long _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"feof: parameter 1 should be of STATIC NUMBER type";

    int fno = (int)nParam0;
    switch (fno) {
        case STDOUT_FILENO:
            nParam0 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam0 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam0 = (SYS_INT)stderr;
            break;
    }
    // function call
    _C_call_result = (long)feof((FILE *)(SYS_INT)nParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__remove CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'remove' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    long _C_call_result;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"remove: parameter 1 should be of STATIC STRING type";

    // function call
    _C_call_result = (int)remove(szParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___unlink CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'unlink' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    long _C_call_result;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"unlink: parameter 1 should be of STATIC STRING type";

    // function call
    _C_call_result = (int)unlink(szParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___rmdir CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'rmdir' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    long _C_call_result;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"rmdir: parameter 1 should be of STATIC STRING type";

    // function call
    _C_call_result = (int)rmdir(szParam0);

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__ReadFile CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'ReadFile' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FILE *FIN;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"ReadFile: parameter 1 should be of STATIC STRING type";

#ifdef _WIN32
    wchar_t *fname = wstr(szParam0);
    if (fname) {
        FIN = _wfopen(fname, L"rb");
        free(fname);
    } else
        FIN = fopen((char *)szParam0, "rb");
#else
    FIN = (FILE *)fopen((char *)szParam0, "rb");
#endif
    // function call
    if (FIN) {
        fseek(FIN, 0, SEEK_END);
        long size = (long)ftell(FIN);
        fseek(FIN, 0, SEEK_SET);
        if (size > 0) {
//                char *buffer=new char[size];
            char *buffer = 0;
            CORE_NEW((size + 1), buffer);
            if (!buffer)
                return (void *)"ReadFile: Not enough memory";
            buffer[0] = buffer[size] = 0;
            fread(buffer, 1, size, FIN);
#if 0
//                SetVariable(RESULT,VARIABLE_STRING,buffer,size);
//                delete[] buffer;
#else
            // LINK buffer, no copy, concept core will free it
            SetVariable(RESULT, -1, buffer, size);
#endif
            fclose(FIN);
            return 0;
        }
        fclose(FIN);
    }

    SetVariable(RESULT, VARIABLE_STRING, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__WriteFile CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)": 'WriteFile' parameters error. This fuction takes 2 parameters.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    NUMBER  STR_LEN = 0;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FILE *FOUT;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &STR_LEN);
    if (TYPE != VARIABLE_STRING)
        return (void *)"WriteFile: parameter 1 should be of STATIC STRING type";

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &szParam1, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"WriteFile: parameter 2 should be of STATIC STRING type";

#ifdef _WIN32
    wchar_t *fname = wstr(szParam1);
    if (fname) {
        FOUT = _wfopen(fname, L"wb");
        free(fname);
    } else
        FOUT = fopen((char *)szParam1, "wb");
#else
    FOUT = (FILE *)fopen((char *)szParam1, "wb");
#endif
    // function call
    if (FOUT) {
        long size = (long)STR_LEN;
        fwrite(szParam0, 1, size, FOUT);
        fclose(FOUT);
        SetVariable(RESULT, VARIABLE_NUMBER, "", 1);
    } else
        SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__FileExists CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_FileExists' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FILE *FIN;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"_FileExists: parameter 1 should be of STATIC STRING type";

    FIN = (FILE *)fopen((char *)szParam0, "rb");
    // function call
    if (FIN) {
        fclose(FIN);
        SetVariable(RESULT, VARIABLE_NUMBER, "", 1);
    } else
        SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__DirectoryExists CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_DirectoryExists' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    FILE *FIN;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &NUMBER_DUMMY);
    if (TYPE != VARIABLE_STRING)
        return (void *)"_DirectoryExists: parameter 1 should be of STATIC STRING type";

    DIR *_C_call_result = opendir(szParam0);


    if (_C_call_result) {
        closedir(_C_call_result);
        SetVariable(RESULT, VARIABLE_NUMBER, "", 1);
    } else
        SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fflush CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'fflush' takes 1 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GET_CHECK_NUMBER(0, nParam0, "'fflush' parameter 0 should be a number (STATIC NUMBER).");

    // function call
    int fno = (int)nParam0;
    switch (fno) {
        case STDOUT_FILENO:
            nParam0 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam0 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam0 = (SYS_INT)stderr;
            break;
    }
    _C_call_result = (int)fflush((FILE *)(SYS_INT)nParam0);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fgetc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'fgetc' takes 1 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GET_CHECK_NUMBER(0, nParam0, "'fgetc' parameter 0 should be a number (STATIC NUMBER).");

    int fno = (int)nParam0;
    switch (fno) {
        case STDOUT_FILENO:
            nParam0 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam0 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam0 = (SYS_INT)stderr;
            break;
    }
    // function call
    char sir[2];
    sir[1] = 0;
    //_C_call_result=(int)fgetc((FILE*)(long)nParam0);
    //RETURN_NUMBER(_C_call_result);
    sir[0] = fgetc((FILE *)(SYS_INT)nParam0);
    RETURN_BUFFER(sir, 1);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fputc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'fputc' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    NUMBER nParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_NUMBER(0, nParam0, "'fputc' parameter 0 should be a number (STATIC NUMBER).");
    // Parameter 2
    GET_CHECK_NUMBER(1, nParam1, "'fputc' parameter 1 should be a number (STATIC NUMBER).");

    int fno = (int)nParam1;
    switch (fno) {
        case STDOUT_FILENO:
            nParam1 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam1 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam1 = (SYS_INT)stderr;
            break;
    }
    // function call
    _C_call_result = (int)fputc((int)nParam0, (FILE *)(SYS_INT)nParam1);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fputs CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'fputs' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    char   *szParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'fputs' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_NUMBER(1, nParam1, "'fputs' parameter 1 should be a number (STATIC NUMBER).");

    int fno = (int)nParam1;
    switch (fno) {
        case STDOUT_FILENO:
            nParam1 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam1 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam1 = (SYS_INT)stderr;
            break;
    }
    // function call
    _C_call_result = (int)fputs((char *)szParam0, (FILE *)(SYS_INT)nParam1);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___fileno CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'fileno' takes 1 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GET_CHECK_NUMBER(0, nParam0, "'fileno' parameter 0 should be a number (STATIC NUMBER).");

    int fno = (int)nParam0;
    switch (fno) {
        case STDOUT_FILENO:
            nParam0 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam0 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam0 = (SYS_INT)stderr;
            break;
    }
    // function call
    _C_call_result = (int)fileno((FILE *)(SYS_INT)nParam0);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__freopen CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'freopen' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    FILE *_C_call_result;
    // Specific variables
    char   *szParam0;
    char   *szParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'freopen' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'freopen' parameter 1 should be a string (STATIC STRING).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'freopen' parameter 2 should be a number (STATIC NUMBER).");

    int fno = (int)nParam2;
    switch (fno) {
        case STDOUT_FILENO:
            nParam2 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam2 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam2 = (SYS_INT)stderr;
            break;
    }
    // function call
    _C_call_result = (FILE *)freopen((char *)szParam0, (char *)szParam1, (FILE *)(SYS_INT)nParam2);

    RETURN_NUMBER((SYS_INT)_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fgets CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'fgets' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char   *szParam0;
    NUMBER nParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'fgets' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_NUMBER(1, nParam1, "'fgets' parameter 1 should be a number (STATIC NUMBER).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'fgets' parameter 2 should be a number (STATIC NUMBER).");

    if (!(int)nParam1) {
        RETURN_STRING("");
        return 0;
    }

    char *buffer = new char[(int)nParam1 + 1];
    buffer[(int)nParam1] = 0;

    int fno = (int)nParam2;
    switch (fno) {
        case STDOUT_FILENO:
            nParam2 = (SYS_INT)stdout;
            break;

        case STDIN_FILENO:
            nParam2 = (SYS_INT)stdin;
            break;

        case STDERR_FILENO:
            nParam2 = (SYS_INT)stderr;
            break;
    }
    // function call
    _C_call_result = (char *)fgets((char *)buffer, (int)nParam1, (FILE *)(SYS_INT)nParam2);

    RETURN_STRING(_C_call_result);
    SET_STRING(0, buffer);

    delete[] buffer;
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___chdir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'chdir' takes 1 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'chdir' parameter 0 should be a string (STATIC STRING).");

    // function call
    _C_call_result = (int)chdir((char *)szParam0);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___mkdir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'mkdir' takes 1 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'mkdir' parameter 0 should be a string (STATIC STRING).");

    // function call
#ifdef _WIN32
    _C_call_result = (int)mkdir((char *)szParam0);
#else
    _C_call_result = (int)mkdir((char *)szParam0, 0777L);
#endif

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__exec CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(1, MAXIMUM_EXEC_PARAMETERS, "'exec' takes at least 1 parameter. See help for details.");
    LOCAL_INIT;

    int _C_call_result = 0;
    // Specific variables
    char *szParam[MAXIMUM_EXEC_PARAMETERS + 1];
    char *szPath;

    GET_CHECK_STRING(0, szPath, "All parameters should be of static string type");

    int i;
    for (i = 0; i < PARAMETERS_COUNT; i++) {
        GET_CHECK_STRING(i, szParam[i], "All parameters should be of static string type");
    }
    szParam[i] = 0;

#ifdef _WIN32
    // function call
    _C_call_result = (int)spawnvp(_P_WAIT, szPath, szParam);
#else
    pid_t pid = fork();
    if (pid < 0)
        return (char *)"Error creating Process";
    if (!pid) {
        _C_call_result = (int)execvp(szPath, szParam);
    } else
        waitpid(pid, NULL, 0);
#endif
    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__opendir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'opendir' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    void *_C_call_result;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "Parameter 0 should be a string (STATIC STRING).");
    _C_call_result = opendir(szParam0);

    RETURN_NUMBER((SYS_INT)_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__readdir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'readdir' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    void *_C_call_result;
    // Specific variables
    double nParam0;

    GET_CHECK_NUMBER(0, nParam0, "Parameter 0 should be a number (STATIC NUMBER).");
    if (!(DIR *)(SYS_INT)nParam0) {
        return (void *)"Parameter 0 should be a valid handle";
    }
    _C_call_result = readdir((DIR *)(SYS_INT)nParam0);

    RETURN_NUMBER((SYS_INT)_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__telldir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'telldir' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    double nParam0;

    GET_CHECK_NUMBER(0, nParam0, "Parameter 0 should be a number (STATIC NUMBER).");

    if (!(DIR *)(SYS_INT)nParam0) {
        return (void *)"Parameter 0 should be a valid handle";
    }
    _C_call_result = telldir((DIR *)(SYS_INT)nParam0);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__seekdir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'seekdir' takes 2 parameters. See help for details.");

    LOCAL_INIT;

    // Specific variables
    double nParam0;
    double nParam1;

    GET_CHECK_NUMBER(0, nParam0, "Parameter 0 should be a number (STATIC NUMBER).");
    GET_CHECK_NUMBER(0, nParam1, "Parameter 1 should be a number (STATIC NUMBER).");

    if (!(DIR *)(SYS_INT)nParam0) {
        return (void *)"Parameter 0 should be a valid handle";
    }
    seekdir((DIR *)(SYS_INT)nParam0, (long)nParam1);

    RETURN_NUMBER(0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__rewinddir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'rewinddir' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    // Specific variables
    double nParam0;

    GET_CHECK_NUMBER(0, nParam0, "Parameter 0 should be a number (STATIC NUMBER).");

    if (!(DIR *)(SYS_INT)nParam0) {
        return (void *)"Parameter 0 should be a valid handle";
    }
    rewinddir((DIR *)(SYS_INT)nParam0);

    RETURN_NUMBER(0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__closedir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'closedir' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    double c_result;
    // Specific variables
    double nParam0;

    GET_CHECK_NUMBER(0, nParam0, "Parameter 0 should be a number (STATIC NUMBER).");

    if ((DIR *)(SYS_INT)nParam0)
        c_result = closedir((DIR *)(SYS_INT)nParam0);
    else
        c_result = 0;

    RETURN_NUMBER(c_result);
    return 0;
}
//-----------------------------------------------------------------------------------

/*CONCEPT_DLL_API CONCEPT__dirfd CONCEPT_API_PARAMETERS {
        PARAMETERS_CHECK(1,"'dirfd' takes 1 parameters. See help for details.");

        LOCAL_INIT;

        double       _C_call_result;
        // Specific variables
        double       nParam0;

        GET_CHECK_NUMBER(0, nParam0, "Parameter 0 should be a number (STATIC NUMBER).");

        _C_call_result=dirfd((DIR *)(long)nParam0);

        RETURN_NUMBER(_C_call_result);
        return 0;
   }*/
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__dirname CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'dirname' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    double nParam0;

    GET_CHECK_NUMBER(0, nParam0, "Parameter 0 should be a number (STATIC NUMBER).");

    dirent *dent = (dirent *)(SYS_INT)nParam0;
    if (!dent)
        return (void *)"Parameter 0 should be a valid handle";

    //RETURN_BUFFER(dent->d_name,dent->d_namlen);
    RETURN_STRING(dent->d_name);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__filetype CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'filetype' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    char *szParam0;

    GET_CHECK_STRING(0, szParam0, "Parameter 0 should be a string (STATIC STRING).");

    struct stat buf;
    if (stat(szParam0, &buf)) {
        RETURN_NUMBER(-1);
        return 0;
    }

#ifdef _WIN32
    _C_call_result = (buf.st_mode) & _S_IFMT;
#else
    _C_call_result = (buf.st_mode) & S_IFMT;
#endif

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__filesize CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'filesize' takes 1 parameters. See help for details.");
    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    char *szParam0;

    GET_CHECK_STRING(0, szParam0, "filesize: Parameter 0 should be a string (STATIC STRING).");

    struct stat buf;
    int         res = stat(szParam0, &buf);
    if (!res) {
        RETURN_NUMBER(buf.st_size);
    } else {
        RETURN_NUMBER(-1);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__filelast_acc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'filelast_acc' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    char *szParam0;

    GET_CHECK_STRING(0, szParam0, "filelast_acc: Parameter 0 should be a string (STATIC STRING).");

    struct stat buf;
    //stat(szParam0, &buf);

    //RETURN_NUMBER(buf.st_atime);
    if (!stat(szParam0, &buf)) {
        RETURN_NUMBER(buf.st_atime);
    } else {
        RETURN_NUMBER(-1);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__filelast_mod CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'filelast_mod' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    char *szParam0;

    GET_CHECK_STRING(0, szParam0, "filelast_mod: Parameter 0 should be a string (STATIC STRING).");

    struct stat buf;
    //stat(szParam0, &buf);

    //RETURN_NUMBER(buf.st_mtime);
    if (!stat(szParam0, &buf)) {
        RETURN_NUMBER(buf.st_mtime);
    } else {
        RETURN_NUMBER(-1);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__filelast_ch CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'filelast_ch' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    char *szParam0;

    GET_CHECK_STRING(0, szParam0, "filelast_ch: Parameter 0 should be a string (STATIC STRING).");

    struct stat buf;
    //stat(szParam0, &buf);

    //RETURN_NUMBER(buf.st_ctime);
    if (!stat(szParam0, &buf)) {
        RETURN_NUMBER(buf.st_ctime);
    } else {
        RETURN_NUMBER(-1);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__fileuid CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'fileuid' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    char *szParam0;

    GET_CHECK_STRING(0, szParam0, "fileuid: Parameter 0 should be a string (STATIC STRING).");

    struct stat buf;
    //stat(szParam0, &buf);
    //RETURN_NUMBER(buf.st_uid);
    if (!stat(szParam0, &buf)) {
        RETURN_NUMBER(buf.st_uid);
    } else {
        RETURN_NUMBER(-1);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__filegid CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'filegid' takes 1 parameters. See help for details.");

    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    char *szParam0;

    GET_CHECK_STRING(0, szParam0, "filegid: Parameter 0 should be a string (STATIC STRING).");

    struct stat buf;
    //stat(szParam0, &buf);

    //RETURN_NUMBER(buf.st_gid);
    if (!stat(szParam0, &buf)) {
        RETURN_NUMBER(buf.st_gid);
    } else {
        RETURN_NUMBER(-1);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___stat CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'stat' takes 2 parameters. See help for details.");

    LOCAL_INIT;

    double _C_call_result;
    // Specific variables
    char *szParam0;

    GET_CHECK_STRING(0, szParam0, "stat: Parameter 0 should be a string (STATIC STRING).");
    //GET_CHECK_NUMBER(1, ?, "stat: Parameter 0 should be a string (STATIC STRING).");

    struct stat buf;
    int         result     = stat(szParam0, &buf);
    void        *array_var = PARAMETER(1);

    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, array_var)))
        return (void *)"stat : Failed to INVOKE_CREATE_ARRAY";

    if (!result) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_gid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_gid);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_atime", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_atime);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_ctime", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_ctime);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_dev", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_dev);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_ino", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_ino);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_mode", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_mode);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_mtime", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_mtime);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_nlink", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_nlink);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_rdev", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_rdev);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_size", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_size);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "st_uid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)buf.st_uid);
    }
    RETURN_NUMBER((double)result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___errno CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "'_errno' takes 0 parameters. See help for details.");

    RETURN_NUMBER((double)errno);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__getenv, 1)
    T_STRING(0)

    RETURN_STRING(getenv(PARAM(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__putenv, 1)
    T_STRING(0)
#ifdef _WIN32
    RETURN_NUMBER(_putenv(PARAM(0)))
#else
    RETURN_NUMBER(putenv(PARAM(0)))
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_system, 1)
    T_STRING(0)

    RETURN_NUMBER(system(PARAM(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(rename, 2)
    T_STRING(0)
    T_STRING(1)

    RETURN_NUMBER(rename(PARAM(0), PARAM(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(IniGet, 3, 4)
    T_STRING(0)
    T_STRING(1)
    T_STRING(2)

    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        RETURN_STRING(GetKey(PARAM(0), PARAM(1), PARAM(2), PARAM(3)).c_str());
    } else {
        RETURN_STRING(GetKey(PARAM(0), PARAM(1), PARAM(2), "").c_str());
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(IniSet, 4)
    T_STRING(0)
    T_STRING(1)
    T_STRING(2)
    T_STRING(3)

    RETURN_NUMBER(SetKey(PARAM(0), PARAM(1), PARAM(2), PARAM(3)));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(popen, 2)
    T_STRING(0)
    T_STRING(1)

    RETURN_NUMBER((SYS_INT)popen(PARAM(0), PARAM(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(pclose, 1)
    T_NUMBER(0)
    if (PARAM_INT(0)) {
        RETURN_NUMBER(pclose((FILE *)PARAM_INT(0)))
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_WEXITSTATUS, 1)
    T_NUMBER(0)
#ifdef _WIN32
    RETURN_NUMBER(PARAM_INT(0))
#else
    int param_i0 = PARAM_INT(0);
    RETURN_NUMBER(WEXITSTATUS(param_i0))
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_MSWINDOWS, 0)
#ifdef _WIN32
    RETURN_NUMBER(1);
#else
    RETURN_NUMBER(0);
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_BITS, 0)
    long res = sizeof(void *) * 8;
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(IsSymlink, 1)
    T_STRING(0)
#ifdef _WIN32
    RETURN_NUMBER(0)
#else
    struct stat buf;

    if (lstat(PARAM(0), &buf)) {
        RETURN_NUMBER(-1)
        return 0;
    }

    if (S_ISLNK(buf.st_mode)) {
        RETURN_NUMBER(1)
    } else {
        RETURN_NUMBER(0)
    }
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(utime, 3)
    T_STRING(0)
    T_NUMBER(1)
    T_NUMBER(2)

    utimbuf buf;
    buf.actime  = (time_t)PARAM_INT(1);
    buf.modtime = (time_t)PARAM_INT(2);
    int res = utime(PARAM(0), &buf);
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(dup, 1)
    T_NUMBER(0)

    RETURN_NUMBER(dup(PARAM_INT(0)));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(dup2, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    RETURN_NUMBER(dup2(PARAM_INT(0), PARAM_INT(1)));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(fdopen, 2)
    T_NUMBER(0)
    T_STRING(1)

    RETURN_NUMBER((SYS_INT)fdopen(PARAM_INT(0), PARAM(1)));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(pipe, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    int p[2];
    p[0] = PARAM_INT(0);
    p[1] = PARAM_INT(1);
    int res = pipe(p);

#ifdef _WIN32
    if (res == 0) {
        _setmode(p[0], _O_BINARY);
        _setmode(p[1], _O_BINARY);
    }
#endif
    SET_NUMBER(0, p[0]);
    SET_NUMBER(1, p[1]);
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(socketpair, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    int p[2];
    p[0] = PARAM_INT(0);
    p[1] = PARAM_INT(1);

#ifdef _WIN32
    int res = pipe(p);
    if (res == 0) {
        _setmode(p[0], _O_BINARY);
        _setmode(p[1], _O_BINARY);
    }
#else
    int res = socketpair(AF_LOCAL, SOCK_STREAM, 0, p);
#endif
    SET_NUMBER(0, p[0]);
    SET_NUMBER(1, p[1]);
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(close, 1)
    T_NUMBER(0)
    RETURN_NUMBER(close(PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
#define DUP2CLOSE(oldfd, newfd)    (dup2(oldfd, newfd) == 0 && close(oldfd) == 0)
int remap_pipe_stdin_stdout(int rpipe, int wpipe) {
    /*------------------------------------------------------------------
     * CASE [A]
     *
     * This is the trivial case that probably never happens: the two FDs
     * are already in the right place and we have nothing to do. Though
     * this probably doesn't happen much, it's guaranteed that *doing*
     * any shufflingn would close descriptors that shouldn't have been.
     */
    if ((rpipe == 0) && (wpipe == 1))
        return 1;

    /*----------------------------------------------------------------
     * CASE [B] and [C]
     *
     * These two have the same handling but not the same rules. In case
     * [C] where both FDs are "out of the way", it doesn't matter which
     * of the FDs is closed first, but in case [B] it MUST be done in
     * this order.
     */
    if ((rpipe >= 1) && (wpipe > 1)) {
        return DUP2CLOSE(rpipe, 0) &&
               DUP2CLOSE(wpipe, 1);
    }


    /*----------------------------------------------------------------
     * CASE [D]
     * CASE [E]
     *
     * In these cases, *one* of the FDs is already correct and the other
     * one can just be dup'd to the right place:
     */
    if ((rpipe == 0) && (wpipe >= 1))
        return DUP2CLOSE(wpipe, 1);

    if ((rpipe >= 1) && (wpipe == 1))
        return DUP2CLOSE(rpipe, 0);


    /*----------------------------------------------------------------
     * CASE [F]
     *
     * Here we have the write pipe in the read slot, but the read FD
     * is out of the way: this means we can do this in just two steps
     * but they MUST be in this order.
     */
    if ((rpipe >= 1) && (wpipe == 0)) {
        return DUP2CLOSE(wpipe, 1) &&
               DUP2CLOSE(rpipe, 0);
    }

    /*----------------------------------------------------------------
     * CASE [G]
     *
     * This is the trickiest case because the two file descriptors  are
     * *backwards*, and the only way to make it right is to make a
     * third temporary FD during the swap.
     */
    if ((rpipe == 1) && (wpipe == 0)) {
        const int tmp = dup(wpipe);             /* NOTE! this is not dup2() ! */

        return tmp > 1 &&
               close(wpipe) == 0 &&
               DUP2CLOSE(rpipe, 0) &&
               DUP2CLOSE(tmp, 1);
    }

    /* SHOULD NEVER GET HERE */

    return 0;
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(remap_pipe_stdin_stdout, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    RETURN_NUMBER(remap_pipe_stdin_stdout(PARAM_INT(0), PARAM_INT(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(write, 2)
    T_NUMBER(0)
    T_STRING(1)

    RETURN_NUMBER(write(PARAM_INT(0), PARAM(1), PARAM_LEN(1)));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(read, 3)
    T_NUMBER(0)
    T_NUMBER(2)

    int size = PARAM_INT(2);
    char *c = 0;
    CORE_NEW(size + 1, c);
    if (!c) {
        SET_STRING(1, "");
        return (void *)"read: Not enough memory";
    }
    ssize_t ret = read(PARAM_INT(0), c, size);
    if (ret > 0) {
        c[ret] = 0;
        SetVariable(PARAMETER(1), -1, c, ret);
    } else {
        CORE_DELETE(c);
        SET_STRING(1, "");
    }

    RETURN_NUMBER(ret);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(seek, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)

    RETURN_NUMBER(lseek(PARAM_INT(0), PARAM_INT(1), PARAM_INT(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(umask, 1)
    T_NUMBER(0)

    mode_t res = umask(PARAM_INT(0));
    RETURN_NUMBER(res)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(chmod, 2)
    T_STRING(0)
    T_NUMBER(1)

    int res = chmod(PARAM(0), PARAM_INT(1));
    RETURN_NUMBER(res)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(strerror, 1)
    T_NUMBER(0)
    char *res = strerror(PARAM_INT(0));
    RETURN_STRING(res)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(panicexit, 1)
    T_NUMBER(0)
    exit(PARAM_INT(0));
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(getpid, 0)
#ifdef _WIN32
    int res = _getpid();
#else
    pid_t res = getpid();
#endif
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(getpgrp, 0)
#ifdef _WIN32
    int res = -1;
#else
    int res = (int)getpgrp();
#endif
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(kill, 2)
    T_NUMBER(0)
    T_NUMBER(1)
#ifdef _WIN32
    int res = -1;
    switch (PARAM_INT(1)) {
        case 2:
        case 6:
        case 17:
        case 18:
        case 19:
        case 20:
        case 23:
        case 24:
            res = GenerateConsoleCtrlEvent(CTRL_C_EVENT, PARAM_INT(0));
            break;

        case 0:
        case 1:
        case 3:
        case 9:
        case 15:
            if (TerminateProcess2(PARAM_INT(0), 0))
                res = 0;
            break;
    }

#else
    int res = (int)kill(PARAM_INT(0), PARAM_INT(1));
#endif
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
char **GetCharList2(void *arr, INVOKE_CALL _Invoke) {
    INTEGER type      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;
    char    **ret     = 0;

    int len_e = 0;

    while ((environ) && (environ[len_e]))
        len_e++;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    int size  = count + len_e;
    ret       = new char * [size + 1];
    ret[size] = 0;

    for (int i = 0; i < len_e; i++)
        ret[i] = environ[i];

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i + len_e] = szData;
            } else
                ret[i + len_e] = 0;
        }
    }
    return ret;
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ExecuteProcess, 1, 8)
    T_STRING(0)
    int do_wait = 1;
    SYS_INT res      = -1;
    int     n_stdout = -1;
    int     n_stdin  = -1;
    int     n_stderr = -1;
    char    *path    = 0;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(1);
        if (PARAM_LEN(1) > 0)
            path = PARAM(1);
    }

    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(5)
        n_stdout = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(6)
        n_stdin = PARAM_INT(6);
    }
    if (PARAMETERS_COUNT > 7) {
        T_NUMBER(7)
        n_stderr = PARAM_INT(7);
    }
    char *dummy_param[2];
    dummy_param[0] = PARAM(0);
    dummy_param[1] = 0;

    char **parameters = 0;
    char **env        = 0;

#ifdef _WIN32
    char use_mod_version = 0;
    char do_free_param   = 0;
    if (((n_stdin > -1) && (n_stdin != STDIN_FILENO)) || ((n_stdout > -1) && (n_stdout != STDOUT_FILENO)) || ((n_stderr > -1) && (n_stderr != STDERR_FILENO)))
        use_mod_version = 1;
#endif

    if (PARAMETERS_COUNT > 2) {
        T_ARRAY(2)
#ifdef _WIN32
        if (use_mod_version)
            parameters = GetCharList(PARAMETER(2), Invoke);
        else {
            parameters    = GetParamList2(PARAMETER(2), Invoke, PARAM(0));
            do_free_param = 1;
        }
#else
        parameters = GetParamList(PARAMETER(2), Invoke, PARAM(0));
#endif
    }
    if (PARAMETERS_COUNT > 3) {
        T_ARRAY(3)
#ifdef _WIN32
        if (use_mod_version)
            env = GetCharList(PARAMETER(3), Invoke);
        else
            env = GetCharList2(PARAMETER(3), Invoke);
#else
        env = GetCharList(PARAMETER(3), Invoke);
#endif
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4);
        do_wait = PARAM_INT(4);
    }

    if ((parameters) && (!parameters[0])) {
        delete[] parameters;
        parameters = 0;
    }

    if ((env) && (!env[0])) {
        delete[] env;
        env = 0;
    }

#ifdef _WIN32
    if (use_mod_version) {
        AnsiString cmd(PARAM(0));
        if (parameters) {
            int idx = 0;
            while (parameters[idx]) {
                cmd += " \"";
                char *p = parameters[idx];
                if (strchr(p, '"')) {
                    int len = strlen(p);
                    for (int i = 0; i < len; i++) {
                        char c = p[i];
                        if (c == '"') {
                            cmd += "\\\"";
                        } else
                            cmd += c;
                    }
                } else
                    cmd += p;
                cmd += "\"";
                idx++;
            }
            delete[] parameters;
            parameters = 0;
        }

        AnsiString envstr;
        if (env) {
            if (env[0]) {
                int idx = 0;
                while (env[idx]) {
                    // include the null character
                    envstr.AddBuffer(env[idx], strlen(env[idx]) + 1);
                    //envstr+=env[idx];
                    //envstr+='\0';
                    idx++;
                }
                char *str = GetEnvironmentStrings();
                int  i    = 0;
                while (str) {
                    char c = str[i++];
                    if (c) {
                        envstr += c;
                    } else {
                        envstr += c;
                        if (!str[i])
                            break;
                    }
                }
                FreeEnvironmentStrings(str);
                envstr += '\0';
            }
            delete[] env;
            env = 0;
        }
        res = spawn_mod(cmd.c_str(), path, envstr.Length() ? envstr.c_str() : NULL, n_stdin > -1 ? n_stdin : STDIN_FILENO, n_stdout > -1 ? n_stdout : STDOUT_FILENO, n_stderr > -1 ? n_stderr : STDERR_FILENO);

        if (do_wait) {
            if (res) {
                DWORD exitCode = 0;
                WaitForSingleObject((HANDLE)res, INFINITE);
                GetExitCodeProcess((HANDLE)res, &exitCode);
                //TerminateProcess((HANDLE)res, 0);
                CloseHandle((HANDLE)res);
                res = exitCode;
            } else
                res = -1;
        }
    } else {
        res = spawnvpe(do_wait ? _P_WAIT : _P_NOWAIT, PARAM(0), parameters ? parameters : dummy_param, env);
        if (parameters) {
 #ifdef _WIN32
            if (do_free_param) {
                int idx = 0;
                while (parameters[idx])
                    delete[] parameters[idx++];
            }
 #endif

            delete[] parameters;
        }
        if (env)
            delete[] env;
    }
    RETURN_NUMBER(res);
#else
    pid_t pid = fork();
    if (pid < 0) {
        RETURN_NUMBER(0);
        return 0;
    }
    if (!pid) {
        if (n_stdout > -1) {
            dup2(n_stdout, STDOUT_FILENO);
            close(n_stdout);
        }
        if (n_stdin > -1) {
            dup2(n_stdin, STDIN_FILENO);
            close(n_stdin);
        }
        if (n_stderr > -1) {
            dup2(n_stderr, STDERR_FILENO);
            close(n_stderr);
        }
        if (path)
            chdir(path);
        if (env) {
            int idx = 0;
            while (env[idx]) {
                putenv(env[idx]);
                idx++;
            }
        }
        res = (SYS_INT)execvp(PARAM(0), parameters ? parameters : dummy_param);
        exit(res);
    } else {
        if (parameters)
            delete[] parameters;
        if (env)
            delete[] env;
        if (do_wait) {
            int status = 0;
            waitpid(pid, &status, 0);
            res = status;
        } else
            res = pid;
    }
#endif
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ExecuteJoin, 1)
    T_HANDLE(0)
    SYS_INT pid = PARAM_INT(0);
    int res = 0;
    if (pid > 0) {
#ifdef _WIN32
        DWORD lpExitCode = 0;
        WaitForSingleObject((HANDLE)pid, INFINITE);
        GetExitCodeProcess((HANDLE)pid, &lpExitCode);
        res = lpExitCode;
        CloseHandle((HANDLE)pid);
#else
        waitpid(pid, &res, 0);
#endif
    }
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ExecuteIsRunning, 1)
    T_HANDLE(0)
    SYS_INT pid = PARAM_INT(0);
    int res = 0;
    if (pid > 0) {
#ifdef _WIN32
        DWORD lpExitCode = 0;
        GetExitCodeProcess((HANDLE)pid, &lpExitCode);
        if (lpExitCode == STILL_ACTIVE)
            res = 1;
        //if (WaitForSingleObject((HANDLE)pid, 0) == WAIT_TIMEOUT)
        //    res = 1;
#else
 #ifndef P_PID
        int status = 0;
        if (waitpid(pid, &status, WNOHANG | WUNTRACED) == 0)
            res = 1;
 #else
        int       status = 0;
        siginfo_t info;
        info.si_signo = 0;
        if (waitid(P_PID, pid, &info, WNOHANG | WUNTRACED | WNOWAIT) == 0)
            res = 1;
 #endif
#endif
    }
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(mkfifo, 1, 2)
    T_STRING(0)
    int rights = 0666;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        rights = PARAM_INT(1);
    }
    int res = -1;
#ifndef _WIN32
    res = mkfifo(PARAM(0), rights);
#endif
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(getcwd, 0)
    char buffer[0x1FFF];
    buffer[0] = 0;
    getcwd(buffer, 0x1FFF);
    RETURN_STRING(buffer);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(Duplicate, 1, 2)
    T_HANDLE(0)

    SYS_INT phandle = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        phandle = (SYS_INT)PARAM(1);
        //if (phandle)
        //    phandle = (SYS_INT)OpenProcess(PROCESS_DUP_HANDLE, TRUE, GetProcessId((HANDLE)phandle));
    }
#ifdef _WIN32
    RETURN_NUMBER(-1);
    return 0;

// crashes in eof function
    intptr_t to_duplicate = _get_osfhandle(PARAM_INT(0));
    HANDLE   cp           = GetCurrentProcess();
    HANDLE   out          = NULL;

    if (!phandle)
        phandle = (SYS_INT)cp;
    if (!DuplicateHandle(cp, (HANDLE)to_duplicate, (HANDLE)phandle, &out, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
        CloseHandle((HANDLE)to_duplicate);
        RETURN_NUMBER(-1);
        return 0;
    }

/*int duplicate = _open_osfhandle((intptr_t)out, _O_APPEND);
   if (duplicate < 0)
    duplicate = _open_osfhandle((intptr_t)out, _O_RDONLY);
   if (duplicate < 0)
    CloseHandle((HANDLE)out);*/
//RETURN_NUMBER(duplicate);
    RETURN_NUMBER((intptr_t)out);

//CloseHandle((HANDLE)out);
//CloseHandle((HANDLE)to_duplicate);
//CloseHandle((HANDLE)phandle);
//close(duplicate);
    return 0;
#endif
// not supported
    RETURN_NUMBER(PARAM(0));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(setuid, 1)
    T_NUMBER(0)
#ifdef _WIN32
    RETURN_NUMBER(-2);
#else
    int res = setuid(PARAM_INT(0));
    RETURN_NUMBER(res);
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(setgid, 1)
    T_NUMBER(0)
#ifdef _WIN32
    RETURN_NUMBER(-2);
#else
    int res = setgid(PARAM_INT(0));
    RETURN_NUMBER(res);
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(getpwnam, 1)
    T_STRING(0)

    CREATE_ARRAY(RESULT);
#ifndef _WIN32
    struct passwd data;
    struct passwd *p = NULL;
    char          buf[0x7FFF];

    getpwnam_r(PARAM(0), &data, buf, sizeof(buf), &p);
    if (p) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_name", (INTEGER)VARIABLE_STRING, (char *)p->pw_name, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_passwd", (INTEGER)VARIABLE_STRING, (char *)p->pw_passwd, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_uid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)p->pw_uid);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_gid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)p->pw_gid);

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_gecos", (INTEGER)VARIABLE_STRING, (char *)p->pw_gecos, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_dir", (INTEGER)VARIABLE_STRING, (char *)p->pw_dir, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_shell", (INTEGER)VARIABLE_STRING, (char *)p->pw_shell, (NUMBER)0);
    }
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(getpwuid, 1)
    T_NUMBER(0)

    CREATE_ARRAY(RESULT);
#ifndef _WIN32
    struct passwd data;
    struct passwd *p = NULL;
    char          buf[0x7FFF];

    getpwuid_r(PARAM_INT(0), &data, buf, sizeof(buf), &p);
    if (p) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_name", (INTEGER)VARIABLE_STRING, (char *)p->pw_name, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_passwd", (INTEGER)VARIABLE_STRING, (char *)p->pw_passwd, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_uid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)p->pw_uid);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_gid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)p->pw_gid);

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_gecos", (INTEGER)VARIABLE_STRING, (char *)p->pw_gecos, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_dir", (INTEGER)VARIABLE_STRING, (char *)p->pw_dir, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "pw_shell", (INTEGER)VARIABLE_STRING, (char *)p->pw_shell, (NUMBER)0);
    }
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SetCurrentUser, 2, 3)
    T_STRING(0)
    T_STRING(1)

    AnsiString domain(".");
    if (PARAMETERS_COUNT > 2) {
        T_STRING(2)

        domain = PARAM(2);
    }
#ifdef _WIN32
    int    retval = 0;
    HANDLE hToken = 0;

    retval = LogonUser(
        PARAM(0),
        domain.c_str(),
        PARAM(0),
        LOGON32_LOGON_SERVICE,
        LOGON32_PROVIDER_DEFAULT,
        &hToken
        );

    if (!retval) {
        RETURN_NUMBER(0)
    } else {
        retval = ImpersonateLoggedOnUser(hToken);
        CloseHandle(hToken);
        if (!retval) {
            RETURN_NUMBER(0)
        } else {
            RETURN_NUMBER(1)
        }
    }

#else
    struct passwd *pwd = getpwnam((const char *)PARAM(0));
    if (pwd) {
        setuid(pwd->pw_uid);
        setgid(pwd->pw_gid);
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(0);
    }
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(setrlimit, 2, 3)
    T_NUMBER(0)
    T_NUMBER(1)

    int t = PARAM_INT(0);

    int soft_limit = PARAM_INT(1);
    int hard_limit = -1;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        hard_limit = PARAM_INT(2);
    }
#ifdef _WIN32
    if (t == 1) {
        int res = _setmaxstdio(soft_limit);
        if (res < 0) {
            RETURN_NUMBER(-1);
            res = _setmaxstdio(2048);
        } else {
            RETURN_NUMBER(0);
        }
    } else {
        RETURN_NUMBER(-1);
    }
#else
    struct rlimit rlim;
    int           res = -1;

    switch (t) {
        case RLIMIT_NOFILE_PORTABLE:
            t = RLIMIT_NOFILE;
            break;

        case RLIMIT_NPROC_PORTABLE:
            t = RLIMIT_NPROC;
            break;

        case RLIMIT_RSS_PORTABLE:
            t = RLIMIT_RSS;
            break;

        case RLIMIT_MEMLOCK_PORTABLE:
            t = RLIMIT_MEMLOCK;
            break;

        case RLIMIT_AS_PORTABLE:
            t = RLIMIT_AS;
            break;
    }
    if (!getrlimit(t, &rlim)) {
        if (PARAMETERS_COUNT > 2)
            rlim.rlim_max = hard_limit;

        rlim.rlim_cur = PARAM_INT(1);

        res = setrlimit(t, &rlim);
    }
    RETURN_NUMBER(res);
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(getrlimit, 1)
    T_NUMBER(0)

    int t = PARAM_INT(0);
    CREATE_ARRAY(RESULT);
#ifdef _WIN32
    if (t == RLIMIT_NOFILE_PORTABLE) {
        int res = _getmaxstdio();
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rlim_cur", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)res);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rlim_max", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)2048);
    }
#else
    switch (t) {
        case RLIMIT_NOFILE_PORTABLE:
            t = RLIMIT_NOFILE;
            break;

        case RLIMIT_NPROC_PORTABLE:
            t = RLIMIT_NPROC;
            break;

        case RLIMIT_RSS_PORTABLE:
            t = RLIMIT_RSS;
            break;

        case RLIMIT_MEMLOCK_PORTABLE:
            t = RLIMIT_MEMLOCK;
            break;

        case RLIMIT_AS_PORTABLE:
            t = RLIMIT_AS;
            break;
    }
    struct rlimit rlim;
    int           res = getrlimit(t, &rlim);
    if (!res) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rlim_cur", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)rlim.rlim_cur);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rlim_max", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)rlim.rlim_max);
    }
#endif
END_IMPL
//---------------------------------------------------------------------------
#ifdef _WIN32
int set_nonblocking_flag(int desc, bool value) {
    HANDLE h = (HANDLE)_get_osfhandle(desc);

    if (h == INVALID_HANDLE_VALUE) {
        errno = EBADF;
        return -1;
    }
    if (GetFileType(h) == FILE_TYPE_PIPE) {
        DWORD state;
        if (GetNamedPipeHandleState(h, &state, NULL, NULL, NULL, NULL, 0)) {
            if ((state & PIPE_NOWAIT) != 0) {
                if (value)
                    return 0;
                state &= ~PIPE_NOWAIT;
            } else {
                if (!value)
                    return 0;
                state |= PIPE_NOWAIT;
            }
            if (SetNamedPipeHandleState(h, &state, NULL, NULL))
                return 0;
            errno = 45;
            return -1;
        } else {
            errno = 45;
            return -1;
        }
    } else {
        if (!value)
            return 0;
        errno = 45;
        return -1;
    }
}
#endif

CONCEPT_FUNCTION_IMPL(SetNonBlocking, 2)
    T_NUMBER(0)
    T_NUMBER(1)
    int fd = PARAM_INT(0);
#ifdef _WIN32
    int res = -2;
    if (PARAM_INT(1))
        res = set_nonblocking_flag(fd, true);
    else
        res = set_nonblocking_flag(fd, false);
#else
    int flags = 0;
 #if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;

    int flags2 = flags;
    if (PARAM_INT(1))
        flags2 |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    int res = fcntl(fd, F_SETFL, flags2);
 #else
    if (PARAM_INT(1))
        flags = 1;
    int res = ioctl(fd, FIONBIO, &flags);
 #endif
#endif
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(DescriptorWrite, 2, 3)
    T_NUMBER(0)
    T_NUMBER(1)

    int sock = PARAM_INT(0);
    int fd = PARAM_INT(1);

    SYS_INT refPID = -1;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        refPID = (SYS_INT)PARAM(2);
    }
#ifdef _WIN32
    int size           = -1;
    int duplicate_sock = fd;
    if (refPID > 0) {
        WSAPROTOCOL_INFO protInfo;
        DWORD            pid = GetProcessId((HANDLE)refPID);
        if (WSADuplicateSocket(fd, pid, &protInfo) == INVALID_SOCKET) {
            RETURN_NUMBER(-2);
            return 0;
        }

        int             family = AF_INET;
        struct sockaddr sa;
        int             len;
        if (!getsockname(fd, &sa, &len))
            family = sa.sa_family;

        duplicate_sock = WSASocket(family, SOCK_STREAM, IPPROTO_TCP, &protInfo, 0, WSA_FLAG_OVERLAPPED);

        if (duplicate_sock == INVALID_SOCKET) {
            RETURN_NUMBER(-3);
            return 0;
        }
    }
    long sbuf = duplicate_sock;
    size = write(sock, &sbuf, sizeof(long));
#else
    ssize_t       size;
    struct msghdr msg;
    struct iovec  iov;
    AnsiString    buf((long)fd);
    union {
        struct cmsghdr cmsghdr;
        char           control[CMSG_SPACE(sizeof(int))];
    } cmsgu;
    struct cmsghdr *cmsg;

    iov.iov_base = buf.c_str();
    iov.iov_len  = buf.Length();

    msg.msg_name    = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    if (fd != -1) {
        msg.msg_control    = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);

        cmsg             = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len   = CMSG_LEN(sizeof(int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type  = SCM_RIGHTS;

        *((int *)CMSG_DATA(cmsg)) = fd;
    } else {
        msg.msg_control    = NULL;
        msg.msg_controllen = 0;
    }

    size = sendmsg(sock, &msg, 0);
#endif
    RETURN_NUMBER(size);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DescriptorRead, 2)
    T_NUMBER(0)
    SET_NUMBER(1, -1)

    int sock = PARAM_INT(0);
    int res = -1;
#ifdef _WIN32
    long new_sock;
    if (read(sock, &new_sock, sizeof(new_sock)) == sizeof(new_sock)) {
        SET_NUMBER(1, new_sock);
        res = 0;
    } else
        res = -1;
#else
    ssize_t size;
    char    buf[16];
    long    new_sock = 0;

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
    size = recvmsg(sock, &msg, 0);
    if (size < 0) {
        RETURN_NUMBER(-1);
        return 0;
    }

    cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg && (cmsg->cmsg_len == CMSG_LEN(sizeof(int)))) {
        if (cmsg->cmsg_level != SOL_SOCKET) {
            RETURN_NUMBER(-2);
            return 0;
        }

        if (cmsg->cmsg_type != SCM_RIGHTS) {
            RETURN_NUMBER(-2);
            return 0;
        }

        int fd = *((int *)CMSG_DATA(cmsg));
        SET_NUMBER(1, fd);
        res = 0;
    }
#endif
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
#ifdef _WIN32
struct rusage {
    struct timeval ru_utime;    /* user CPU time used */
    struct timeval ru_stime;    /* system CPU time used */
    long           ru_maxrss;   /* maximum resident set size */
    long           ru_ixrss;    /* integral shared memory size */
    long           ru_idrss;    /* integral unshared data size */
    long           ru_isrss;    /* integral unshared stack size */
    long           ru_minflt;   /* page reclaims (soft page faults) */
    long           ru_majflt;   /* page faults (hard page faults) */
    long           ru_nswap;    /* swaps */
    long           ru_inblock;  /* block input operations */
    long           ru_oublock;  /* block output operations */
    long           ru_msgsnd;   /* IPC messages sent */
    long           ru_msgrcv;   /* IPC messages received */
    long           ru_nsignals; /* signals received */
    long           ru_nvcsw;    /* voluntary context switches */
    long           ru_nivcsw;   /* involuntary context switches */
};

static void usage_to_timeval(FILETIME *ft, struct timeval *tv) {
    ULARGE_INTEGER time;

    time.LowPart  = ft->dwLowDateTime;
    time.HighPart = ft->dwHighDateTime;

    tv->tv_sec = time.QuadPart / 10000000;
}

int getrusage(SYS_INT who, struct rusage *usage) {
    FILETIME creation_time, exit_time, kernel_time, user_time;
    PROCESS_MEMORY_COUNTERS pmc;

    memset(usage, 0, sizeof(struct rusage));

    if (who == 0) {
        if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time))
            return -1;

        if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
            return -1;
    } else
    if (who == 1) {
        if (!GetThreadTimes(GetCurrentThread(), &creation_time, &exit_time, &kernel_time, &user_time))
            return -1;

        memset(&pmc, 0, sizeof(pmc));
    } else {
        HANDLE h = (HANDLE)who;
        if (!h)
            return -1;

        if (!GetProcessTimes(h, &creation_time, &exit_time, &kernel_time, &user_time)) {
            //CloseHandle(h);
            return -1;
        }

        if (!GetProcessMemoryInfo(h, &pmc, sizeof(pmc))) {
            //CloseHandle(h);
            return -1;
        }
        //CloseHandle(h);
    }

    usage_to_timeval(&kernel_time, &usage->ru_stime);
    usage_to_timeval(&user_time, &usage->ru_utime);
    usage->ru_majflt = pmc.PageFaultCount;
    usage->ru_maxrss = pmc.PeakWorkingSetSize / 1024;
    return 0;
}
#endif
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(getrusage, 1, 2)
    T_NUMBER(0);

    bool is_process_handle = true;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1);
        if (!PARAM_INT(1))
            is_process_handle = false;
    }
    SYS_INT proc_pid = (SYS_INT)PARAM(0);
    CREATE_ARRAY(RESULT);

#ifdef _WIN32
    if (!is_process_handle) {
        proc_pid = (SYS_INT)OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_pid);
        if (!proc_pid)
            return 0;
    }
#endif

    struct rusage usagedata;
    if (!getrusage(proc_pid, &usagedata)) {
        unsigned long useconds = usagedata.ru_utime.tv_sec * 1000000 + usagedata.ru_utime.tv_usec;
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_utime", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)useconds);

        useconds = usagedata.ru_stime.tv_sec * 1000000 + usagedata.ru_stime.tv_usec;
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_stime", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)useconds);

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_maxrss", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_maxrss);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_ixrss", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_ixrss);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_idrss", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_idrss);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_isrss", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_isrss);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_minflt", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_minflt);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_majflt", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_majflt);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_nswap", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_nswap);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_inblock", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_inblock);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_oublock", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_oublock);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_msgsnd", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_msgsnd);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_msgrcv", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_msgrcv);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_nsignals", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_nsignals);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_nvcsw", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_nvcsw);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ru_nivcsw", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)usagedata.ru_nivcsw);
    }
#ifdef _WIN32
    if ((!is_process_handle) && (proc_pid))
        CloseHandle((HANDLE)proc_pid);
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sysconf, 1)
    T_NUMBER(0)
#ifdef _WIN32
    int param = PARAM_INT(0);
    long        res = -1;
    SYSTEM_INFO sysinfo;
    UINT64      v = 0;
    switch (param) {
 #ifdef _HAVE_GetPhysicallyInstalledSystemMemory
        case _SC_PHYS_PAGES:
            GetSystemInfo(&sysinfo);
            GetPhysicallyInstalledSystemMemory(&v);
            if (sysinfo.dwPageSize > 0)
                res = v * 1024 / sysinfo.dwPageSize;
            break;
 #endif
        case _SC_PAGESIZE:
            GetSystemInfo(&sysinfo);
            res = sysinfo.dwPageSize;
            break;

        case _SC_NPROCESSORS_ONLN:
            GetSystemInfo(&sysinfo);
            res = sysinfo.dwNumberOfProcessors;
            break;
    }
#else
    long res = sysconf(PARAM_INT(0));
#endif
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__PID, 1)
    T_NUMBER(0)
#ifdef _WIN32
    DWORD pid = PARAM_INT(0);
    if (PARAM_INT(0) > 0)
        pid = GetProcessId((HANDLE)PARAM_INT(0));
    RETURN_NUMBER(pid);
#else
    RETURN_NUMBER(PARAM(0));
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__cpuid, 1)
    T_NUMBER(0)

    unsigned int CPUInfo[4];
    __cpuid(CPUInfo, PARAM_INT(0));

    CREATE_ARRAY(RESULT);
    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)0, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)CPUInfo[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)1, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)CPUInfo[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)2, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)CPUInfo[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)CPUInfo[3]);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ftruncate, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    FILE * file = (FILE *)(SYS_INT)PARAM(0);
    int fd = fileno(file);
#ifdef _WIN32
    int res = chsize(fd, PARAM(1));
#else
    int res = ftruncate(fd, PARAM(1));
#endif
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(LockFileBytes, 4, 6)
    T_HANDLE(0)
    T_NUMBER(1) // lock_type
    T_NUMBER(2) // start
    T_NUMBER(3) // len

    int f_lock_flags = F_SETLK;
    int whence = SEEK_SET;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(4)
        whence = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(5)
        f_lock_flags = PARAM_INT(5);
    }

    FILE *file = (FILE *)(SYS_INT)PARAM(0);
#ifdef _WIN32
    if (whence != SEEK_SET) {
        HANDLE f = (HANDLE)_get_osfhandle(fileno(file));
        if (f) {
            int64_t start    = PARAM(2);
            int64_t len      = PARAM(3);
            DWORD   start_lo = (start << 32) >> 32;
            DWORD   start_hi = start >> 32;

            DWORD len_lo = (start << 32) >> 32;
            DWORD len_hi = len >> 32;
            bool  result;
            if (PARAM_INT(1) == F_UNLCK)
                result = UnlockFile(f, start_lo, start_hi, len_lo, len_hi);
            else
                result = LockFile(f, start_lo, start_hi, len_lo, len_hi);
        }
    }
    RETURN_NUMBER(0);
#else
    struct flock fl;
    fl.l_type   = PARAM_INT(1);
    fl.l_whence = whence;
    fl.l_start  = PARAM_INT(2);
    fl.l_len    = PARAM_INT(3);

    int res = fcntl(fileno(file), f_lock_flags);
    if (res) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(1);
    }
#endif
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__ProcessTitle, 1)
    T_STRING(0)
    int res = -2;
#ifdef _WIN32
    res = 0;
    SetConsoleTitle(PARAM(0));
#endif
#ifdef __linux__
    res = prctl(PR_SET_NAME, PARAM(0));
#else
 #ifdef __FreeBSD__
    res = 0;
    setproctitle("%s", PARAM(0));
 #endif
#endif
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------------------------------------
#ifdef WITH_MMAP_FUNCTIONS
CONCEPT_FUNCTION_IMPL(mmap, 6)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)
    T_NUMBER(3)
    T_NUMBER(4)
    T_NUMBER(5)

    void *addr = (void *)(SYS_INT)PARAM(0);
    size_t len     = (SYS_INT)PARAM(1);
    int    prot    = PARAM_INT(2);
    int    flags   = PARAM_INT(3);
    int    filedes = PARAM_INT(4);
    off_t  off     = (SYS_INT)PARAM(5);
    void   *res    = mmap(addr, len, prot, flags, filedes, off);
    RETURN_NUMBER((SYS_INT)res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(munmap, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    void *addr = (void *)(SYS_INT)PARAM(0);
    size_t len = (SYS_INT)PARAM(1);
    int    res = munmap(addr, len);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mprotect, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    void *addr = (void *)(SYS_INT)PARAM(0);
    size_t len  = (SYS_INT)PARAM(1);
    int    prot = PARAM_INT(2);
    int    res  = mprotect(addr, len, prot);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(msync, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    void *addr = (void *)(SYS_INT)PARAM(0);
    size_t len   = (SYS_INT)PARAM(1);
    int    flags = PARAM_INT(2);
    int    res   = mprotect(addr, len, flags);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mlock, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    void *addr = (void *)(SYS_INT)PARAM(0);
    size_t len = (SYS_INT)PARAM(1);
    int    res = mlock(addr, len);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(munlock, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    void *addr = (void *)(SYS_INT)PARAM(0);
    size_t len = (SYS_INT)PARAM(1);
    int    res = munlock(addr, len);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mread, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    void *addr = (void *)(SYS_INT)PARAM(0);
    SYS_INT offset = (SYS_INT)PARAM(1);
    SYS_INT len    = PARAM_INT(2);
    if (len > 0) {
        RETURN_BUFFER((char *)addr + offset, len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mwrite, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_STRING(2)
    void *addr = (void *)(SYS_INT)PARAM(0);
    SYS_INT offset = (SYS_INT)PARAM(1);
    memcpy((char *)addr + offset, PARAM(2), PARAM_LEN(2));
    RETURN_NUMBER(PARAM_LEN(2))
END_IMPL
#endif
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(fsync2, 1)
    T_HANDLE(0)
    FILE * f = (FILE *)(SYS_INT)PARAM(0);
    int res = fsync(fileno(f));
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(fsync, 1)
    T_NUMBER(0)
    int res = fsync(PARAM_INT(0));
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(fdatasync2, 1)
    T_HANDLE(0)
    FILE * f = (FILE *)(SYS_INT)PARAM(0);
#if _POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE >= 500
    int res = fdatasync(fileno(f));
#else
    int res = fsync(fileno(f));
#endif
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(fdatasync, 1)
    T_NUMBER(0)
#if _POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE >= 500
    int res = fdatasync(PARAM_INT(0));
#else
    int res = fsync(PARAM_INT(0));
#endif
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(availablespace, 1)
    T_STRING(0)
    NUMBER free_space = -1;
#ifdef _WIN32
    ULARGE_INTEGER lpFreeBytesAvailable;
    ULARGE_INTEGER lpTotalNumberOfBytes;
    ULARGE_INTEGER lpTotalNumberOfFreeBytes;
    char           *targetdir = PARAM(0);
    if (PARAM_LEN(0) == 0)
        targetdir = NULL;
    if (GetDiskFreeSpaceExA(targetdir, &lpFreeBytesAvailable, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes))
        free_space = lpFreeBytesAvailable.QuadPart;
#else
    struct statvfs buf;
    if (!statvfs(PARAM(0), &buf)) {
        free_space  = buf.f_bsize;
        free_space *= buf.f_bfree;
    }
#endif
    RETURN_NUMBER(free_space);
END_IMPL
//-----------------------------------------------------------------------------------
void CallDelegate(int signum) {
    void *RES             = 0;
    void *EXCEPTION       = 0;
    void *SIGNAL_DELEGATE = NULL;

    if ((SIGNAL_LIST) && (signum >= 0) && (signum < 0xFF))
        SIGNAL_DELEGATE = SIGNAL_LIST[signum];
    if ((SIGNAL_DELEGATE) && (LocalInvoker)) {
        LocalInvoker(INVOKE_CALL_DELEGATE, SIGNAL_DELEGATE, &RES, &EXCEPTION, (INTEGER)1, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)signum);
        if (EXCEPTION)
            LocalInvoker(INVOKE_FREE_VARIABLE, EXCEPTION);
        if (RES)
            LocalInvoker(INVOKE_FREE_VARIABLE, RES);
    }
}

#ifdef _WIN32
BOOL signal_handler(DWORD fdwCtrlType) {
    CallDelegate(0);
    return TRUE;
}

#else
void signal_handler(int signum) {
    CallDelegate(signum);
}
#endif

CONCEPT_FUNCTION_IMPL(signal, 2)
    T_NUMBER(0)
    T_DELEGATE(1)
    int res = -1;
    int signum = PARAM_INT(0);
#ifdef _WIN32
    switch (signum) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
        case 9:
        case 15:
        case 17:
        case 18:
        case 19:
        case 20:
        case 23:
        case 24:
            signum = 0;
            SetConsoleCtrlHandler((PHANDLER_ROUTINE)signal_handler, TRUE);
            res = 0;
            break;
    }
#else
    if ((signum >= 0) && (signum <= 0xFF)) {
        signal(signum, signal_handler);
        res = 0;
    }
#endif
    if (!res) {
        Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
        if (!SIGNAL_LIST) {
            SIGNAL_LIST = (void **)malloc(0xFF * sizeof(void *));
            memset(SIGNAL_LIST, 0, 0xFF * sizeof(void *));
        }
        if (SIGNAL_LIST[signum])
            Invoke(INVOKE_FREE_VARIABLE, SIGNAL_LIST[signum]);
        SIGNAL_LIST[signum] = PARAMETER(1);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(alarm, 1)
    T_NUMBER(0)
#ifdef _WIN32
    RETURN_NUMBER(-1);
#else
    unsigned res = alarm(PARAM_INT(0));
    RETURN_NUMBER(res);
#endif
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(getchar, 0)
    int res = getc(stdin);
    if (res >= 0) {
        char buf[2];
        buf[0] = res;
        buf[1] = 0;
        RETURN_BUFFER(buf, 1);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gets, 0)
    char buf[8192];
    char *res = fgets(buf, 8192, stdin);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(getpassword, 0, 1)
    char buf[8192];
#ifdef _WIN32
    DWORD fdwSaveOldMode = 0;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &fdwSaveOldMode);
    SetConsoleMode(hStdin, fdwSaveOldMode & ~ENABLE_ECHO_INPUT);
#else
    struct termios oflags, nflags;
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;
    tcsetattr(fileno(stdin), TCSANOW, &nflags);
#endif
    char *res = fgets(buf, 8192, stdin);
#ifdef _WIN32
    SetConsoleMode(hStdin, fdwSaveOldMode);
#else
    tcsetattr(fileno(stdin), TCSANOW, &oflags);
#endif
    if (res) {
        int len = strlen(res);
        while ((len > 0) && ((res[len-1] == '\r') || (res[len-1] == '\n')))
            len--;
        if (len > 0) {
            RETURN_BUFFER(res, len);
        } else {
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------------------------------------
