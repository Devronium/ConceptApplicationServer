//#define DEBUG
//#define USE_FCGI

#define MAX_HEADER    0xFFFF
#ifdef _WIN32
 #include <windows.h>
 #include <sys/utime.h>
#else
 #define _MULTI_THREADED
 #include <pthread.h>      // pthread_create
 #include <signal.h>
 #include <dlfcn.h>        // dlopen

 #define HANDLE            void *
 #define HMODULE           HANDLE

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
#include "debugger.h"
#include "semhh.h"
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
 #include <io.h>
//#include <dir.h>
 #include <direct.h>
#else
//#include <unistd.h>
 #include <sys/select.h>
 #include <netinet/tcp.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
 #include <pwd.h>
#endif

#ifdef USE_FCGI
extern "C" {
 #include <fcgi_stdio.h>
}
 #define MAX_FCGI_SERV    100
#endif

#ifdef _WIN32
 #define CMP_FUNC         stricmp
#else
 #define CMP_FUNC         strcmp
 #define D_TEMP_DIR       "/tmp"
#endif

#define KEY_EXTENSION     ".appkey"
#define KEY_FILE          "generic.appkey"

#ifdef _WIN32
 #define LIBRARY          "concept-core.2.0.dll"
 #define LIBRARY_MT       "concept-core.2.0.MT.dll"
#else
 #define LIBRARY          "libconceptcore.2.0.so"
 #define LIBRARY_MT       "libconceptcore.2.0.MT/libconceptcore.2.0.so"
#endif

#define INI_FILE          "concept.ini"

#ifdef _WIN32
 #define TEMPLATE_PIPE    "ConceptCGIPipe"
#else
 #define TEMPLATE_PIPE    "ConceptCGIPipeXXXXXXXX.tmp"
#endif

typedef int (*NOTIFY_PARENT)(int pipe_out, int apid, int msg_id, int len, const char *data);

extern "C" {
    typedef int (*API_SETNOTIFY)(NOTIFY_PARENT np);
    typedef int (*API_INTERPRETER2)(const char *filename, const char *inc_dir, const char *lib_dir, void *fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, const char *SERVER_PUBLIC_KEY, const char *SERVER_PRIVATE_KEY, const char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, void *userdata);
    typedef int (*API_INTERPRETER)(const char *filename, const char *inc_dir, const char *lib_dir, void *fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, const char *SERVER_PUBLIC_KEY, const char *SERVER_PRIVATE_KEY, const char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent);

    typedef void * (*API_EXECUTE_INIT)(const char *filename, const char *inc_dir, const char *lib_dir, void *fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, const char *SERVER_PRIVATE_KEY, const char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, void *userdata);
    typedef int (*API_EXECUTE_RUN)(void *PTR, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, const char *SERVER_PUBLIC_KEY, const char *SERVER_PRIVATE_KEY, const char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent);
    typedef int (*API_EXECUTE_DONE)(void *PTR);
}

typedef unsigned   chartype;

static int _print_begin = 0;
static int _headers     = 0;
AnsiString namer;
AnsiString namew;
int        client_sock = -1;
int        c_piper     = 0, c_pipew = 0;

static char done_thread = 0;
#ifndef _WIN32
static pthread_cond_t  signal_done  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t signal_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
AnsiString       HOME_BASE;
AnsiString       Library;
AnsiString       Include;
AnsiString       host;
static int       port             = 2663;
static int       sessions         = 25;
static int       autocompile      = 0;
static int       handle_cgi       = 0;
static int       allow_debug      = 0;
static int       allow_parents    = 0;
API_INTERPRETER2 Concept_Execute  = 0;
API_INTERPRETER  Concept_Compiler = 0;
API_SETNOTIFY    SetNotifyParent  = 0;

API_EXECUTE_INIT Concept_Execute3_Init = 0;
API_EXECUTE_RUN  Concept_Execute3_Run  = 0;
API_EXECUTE_DONE Concept_Execute3_Done = 0;

HMODULE             hDLL = 0;
HHSEM               sem_lock;
static time_t       time_marker      = 0;
static int          watchdog_timeout = 0;
HHSEM               sem_watchdog;
static unsigned int total_written = 0;

static AnsiString pre_buffer;
int final_res     = 0;
int run_debug     = 0;
int is_cached_cgi = 0;
int com_in        = 0;
int com_out       = 0;
int iterations    = 0;
static AnsiString s_file;
static void       *CONCEPT_CONTEXT = 0;

const char *memstr(const char *haystack, const char *needle, int size, int needlesize) {
    const char *p;

    for (p = haystack; p <= (haystack - needlesize + size); p++)
        if (memcmp(p, needle, needlesize) == 0)
            return p;
    return NULL;
}

#ifdef _WIN32
void switch_to_binary() {
    setmode(STDOUT_FILENO, O_BINARY);
    setmode(STDIN_FILENO, O_BINARY);
}
#endif

inline void FinishPrint() {
    if (!_print_begin) {
        pre_buffer = AnsiString("Content-Type: text/html\r\n\r\n") + pre_buffer;
        fwrite(pre_buffer.c_str(), pre_buffer.Length(), 1, stdout);
        total_written += pre_buffer.Length();
        fflush(stdout);
        _headers     = 1;
        _print_begin = 1;
        pre_buffer   = (char *)"";
    }
}

char *def_strcasestr(const char *phaystack, const char *pneedle) {
    register const unsigned char *haystack, *needle;
    register chartype            b, c;

    haystack = (const unsigned char *)phaystack;
    needle   = (const unsigned char *)pneedle;

    b = _tolower(*needle);
    if (b != '\0') {
        haystack--;                             /* possible ANSI violation */
        do {
            c = *++haystack;
            if (c == '\0')
                goto ret0;
        } while (_tolower(c) != (int)b);

        c = _tolower(*++needle);
        if (c == '\0')
            goto foundneedle;
        ++needle;
        goto jin;

        for ( ; ; ) {
            register chartype            a;
            register const unsigned char *rhaystack, *rneedle;

            do {
                a = *++haystack;
                if (a == '\0')
                    goto ret0;
                if (_tolower(a) == (int)b)
                    break;
                a = *++haystack;
                if (a == '\0')
                    goto ret0;
shloop:
                ;
            } while (_tolower(a) != (int)b);

jin:      a = *++haystack;
            if (a == '\0')
                goto ret0;

            if (_tolower(a) != (int)c)
                goto shloop;

            rhaystack = haystack-- + 1;
            rneedle   = needle;
            a         = _tolower(*rneedle);

            if (_tolower(*rhaystack) == (int)a)
                do {
                    if (a == '\0')
                        goto foundneedle;
                    ++rhaystack;
                    a = _tolower(*++needle);
                    if (_tolower(*rhaystack) != (int)a)
                        break;
                    if (a == '\0')
                        goto foundneedle;
                    ++rhaystack;
                    a = _tolower(*++needle);
                } while (_tolower(*rhaystack) == (int)a);

            needle = rneedle;           /* took the register-poor approach */

            if (a == '\0')
                break;
        }
    }
foundneedle:
    return (char *)haystack;
ret0:
    return 0;
}

void Print(const char *str, int length = 0) {
    if (length <= 0)
        length = strlen(str);

    if (!_headers) {
        pre_buffer.AddBuffer(str, length);
        int  len  = pre_buffer.Length();
        const char *ptr = memstr(pre_buffer.c_str(), "\r\n\r\n", len, 4);
        if ((len > 4) && (ptr)) {
            _headers = 1;
            const char *buf = pre_buffer.c_str();
            if ((len <= 12) && (!def_strcasestr(buf, "Content-Type"))) {
                const char *ptr2 = strstr(buf, "\n---- RUN TIME ERROR --------------------------------------------\n");
                if ((ptr2) && ((uintptr_t)ptr2 < (uintptr_t)ptr))
                    pre_buffer = AnsiString("Content-Type: text/plain\r\n") + pre_buffer;
                else
                    pre_buffer = AnsiString("Content-Type: text/html\r\n") + pre_buffer;
                buf = pre_buffer.c_str();
                len = pre_buffer.Length();
            }
            total_written += len;
            fwrite(buf, len, 1, stdout);
            fflush(stdout);
            _print_begin = 1;
        } else if (len > MAX_HEADER)
            FinishPrint();
    } else {
        if (length > 0) {
            fwrite(str, length, 1, stdout);
            total_written += length;
        } else if (str)
            fprintf(stdout, "%s", str);

        fflush(stdout);
        _print_begin = 1;
    }
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

AnsiString GetKey(const char *ini_name, const char *section, const char *key, const char *def) {
    char value[4096];

    value[0] = 0;
    ConceptGetPrivateProfileString(section, key, def, value, 4096, ini_name);
    return AnsiString(value);
}

AnsiString GetDirectory() {
#ifdef _WIN32
    char buffer[4096];
    buffer[0] = 0;
    GetModuleFileName(NULL, buffer, 4096);
    for (int i = strlen(buffer) - 1; i >= 0; i--)
        if ((buffer[i] == '/') || (buffer[i] == '\\')) {
            buffer[i + 1] = 0;
            break;
        }
    return AnsiString(buffer);
#else
    return LINUX_PATH;
#endif
}

void SetWebDirectory() {
    char *buffer;
    AnsiString temp;

    temp = s_file.Length() ? s_file.c_str() : getenv("PATH_TRANSLATED");
    if (temp != "") {
        buffer = (char *)temp.c_str();
        for (int i = strlen(buffer) - 1; i >= 0; i--)
            if ((buffer[i] == '/') || (buffer[i] == '\\')) {
                buffer[i + 1] = 0;
                break;
            }
        chdir(buffer);
    }
}

int need_compilation(const char *filename) {
    AnsiString accel_filename(filename);

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

int Connect(const char *HOST, int PORT) {
    struct sockaddr_in sin;
    struct hostent     *hp;
    int CLIENT_SOCKET = -1;

    if ((hp = gethostbyname(HOST)) == 0) {
        fprintf(stderr, "\nUnable to resolve specified host\n");
        return -1;
    }

    memset(&sin, 0, sizeof(sin));
    //INADDR_ANY;
    sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);

    // creez socket-ul
    if ((CLIENT_SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        fprintf(stderr, "\nError creating sockets\n");
        return -1;
    }

    int flag = 1;
    setsockopt(CLIENT_SOCKET,                     /* socket affected */
               IPPROTO_TCP,                       /* set option at TCP level */
               TCP_NODELAY,                       /* name of option */
               (char *)&flag,                     /* the cast is historical cruft */
               sizeof(int));                      /* length of option value */

    if (connect(CLIENT_SOCKET, (struct sockaddr *)&sin, sizeof(sin)) == INVALID_SOCKET) {
        fprintf(stderr, "Error connecting to specified host\n");
        return -1;
    }

    return CLIENT_SOCKET;
}

void Done() {
    if (client_sock == -1)
        return;
#ifdef _WIN32
    closesocket(client_sock);
#else
    close(client_sock);
#endif
    client_sock = -1;

    if (c_piper > 0) {
        close(c_piper);
        c_piper = 0;
        unlink(namer);
    }
    if (c_pipew > 0) {
        close(c_pipew);
        c_pipew = 0;
        unlink(namew);
    }
}

int NotifyServer(int CLIENT_SOCKET, const char *key, int *pipew, int *piper, int *_APID, int *_parentAPID, bool *debug) {
    if (CLIENT_SOCKET == -1) {
        fprintf(stderr, "Error getting APID for CGI application\n");
        return -1;
    }

#ifdef _WIN32
    char tmp[8192];
    tmp[0] = 0;
    GetTempPath(8192, tmp);
#else
    const char *tmp = D_TEMP_DIR;
    AnsiString temp(tmp);
    temp += "/";

    AnsiString tplw(temp);
    AnsiString tplr(temp);
#endif


#ifdef _WIN32
    char szTempName[8192];
    GetTempFileName(tmp, TEMPLATE_PIPE, 0, szTempName);
    namew = szTempName;
#else
    tplw += TEMPLATE_PIPE;
    tplr += TEMPLATE_PIPE;

    namew = mktemp((char *)tplw.c_str());
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
#endif

#ifdef _WIN32
    *pipew = open(namew.c_str(), O_WRONLY | O_BINARY | O_CREAT);
#else
    *pipew = open(namew.c_str(), O_WRONLY | O_CREAT, mode);
#endif

#ifdef _WIN32
    GetTempFileName(tmp, TEMPLATE_PIPE, 0, szTempName);
    namer = szTempName;
#else
    namer = mktemp((char *)tplr.c_str());
#endif


#ifdef _WIN32
    *piper = open(namer.c_str(), O_RDONLY | O_BINARY | O_CREAT);
#else
    *piper = open(namer.c_str(), O_RDONLY | O_CREAT, mode);
#endif


    AnsiString query;

    query  = "piper ";
    query += namew; // write for us is read for server and vice-versa
    query += "\r\npipew ";
    query += namer;
    query += "\r\ngetapid\r\n";

    if ((key) && (key[0])) {
        query += "getparent ";
        query += key;
        query += "\r\n";
    }

    int len = query.Length();

    if (send(CLIENT_SOCKET, query.c_str(), len, 0) != len) {
        fprintf(stderr, "Concept Server query (request APID) failed\n");
        return -1;
    }

    int APID;
    int rec_count = 0;
    int received  = 0;
    do {
        received   = recv(CLIENT_SOCKET, (char *)&APID + rec_count, sizeof(int) - rec_count, 0);
        rec_count += received;
    } while ((rec_count < sizeof(int)) && (received > 0));

    if (received == sizeof(int))
        *_APID = ntohl(APID);
    else {
        fprintf(stderr, "Concept Server query (request APID) failed (APID size is invalid)\n");
        return -1;
    }


    if ((key) && (key[0])) {
        int parentAPID = 0;
        rec_count = 0;
        received  = 0;
        do {
            received   = recv(CLIENT_SOCKET, (char *)&parentAPID + rec_count, sizeof(int) - rec_count, 0);
            rec_count += received;
        } while ((rec_count < sizeof(int)) && (received > 0));

        if (received == sizeof(int)) {
            int pAPID = ntohl(parentAPID);
            if (pAPID < 0) {
                *debug = true;
                pAPID *= -1;
            }
            *_parentAPID = pAPID;
        } else {
            fprintf(stderr, "Concept Server query (request PARENT APID) failed (APID size is invalid)\n");
            return -1;
        }
    }

    return 0;
}

#ifdef _WIN32
AnsiString INI_PATH;
#endif

AnsiString GetKeyPath(AnsiString *HOME_BASE, const char *path, const char *category, const char *key, const char *default_value) {
    AnsiString temp;

    temp = GetKey(path, category, key, default_value);
    if (temp[0] == '.')
        temp = *HOME_BASE + temp;
    return temp;
}

int ConceptCGIInit(int is_cached_cgi) {
    HOME_BASE = GetDirectory();
    //char *filename=getenv("PATH_TRANSLATED");
    hDLL = 0;

#ifdef _WIN32
    Library = GetKeyPath(&HOME_BASE, INI_PATH, "Paths", "Library", "Library") + "\\";
    Include = GetKeyPath(&HOME_BASE, INI_PATH, "Paths", "Include", "Include") + "\\";
#else
    Include = GetKeyPath(&HOME_BASE, INI_PATH, "Paths", "Include", "Include") + "/";
    Library = GetKeyPath(&HOME_BASE, INI_PATH, "Paths", "Library", "Library") + "/";
#endif
    int autocompile = GetKey(INI_PATH, "Run", "AutoCompile", "0").ToInt();

    host          = GetKey(INI_PATH, "Server", "APIDHost", "localhost");
    port          = GetKey(INI_PATH, "Server", "CGIPort", "2663").ToInt();
    handle_cgi    = GetKey(INI_PATH, "Server", "HandleCGIRequests", "0").ToInt();
    allow_debug   = GetKey(INI_PATH, "Web", "AllowDebug", "0").ToInt();
    allow_parents = GetKey(INI_PATH, "Web", "AllowCASParent", "0").ToInt();
    sessions      = GetKey(INI_PATH, "Web", "CGIProcessSessions", "25").ToInt();

    if (sessions <= 0)
        sessions = 1;

#ifdef _WIN32
    switch_to_binary();
#endif

    const char *filename = s_file.Length() ? s_file.c_str() : getenv("PATH_TRANSLATED");
    AnsiString manifest(filename);
    manifest += (char *)".manifest";
    int mt = GetKey(manifest.c_str(), "Application", "Multithreading", "0").ToInt();

#ifdef _WIN32
    if (mt)
        hDLL = LoadLibrary(LIBRARY_MT);
    else
        hDLL = LoadLibrary(LIBRARY);
#else
    if (mt)
        hDLL = dlopen(LIBRARY_PATH_MT, RTLD_LAZY);
    else
        hDLL = dlopen(LIBRARY_PATH, RTLD_LAZY);
#endif

    if (!hDLL) {
        fprintf(stderr, "%s%c%c%c%c", "Content-Type: text/plain", 13, 10, 13, 10);
        fprintf(stderr, "Error loading core library '%s'\n", LIBRARY);
        done_thread = 1;
        return 0;
    }

    if (is_cached_cgi) {
#ifdef _WIN32
        Concept_Execute3_Init = (API_EXECUTE_INIT)GetProcAddress(hDLL, "Concept_Execute3_Init");
        if (!Concept_Execute3_Init)
            Concept_Execute3_Init = (API_EXECUTE_INIT)GetProcAddress(hDLL, "_Concept_Execute3_Init");
        if (Concept_Execute3_Init) {
            Concept_Execute3_Run = (API_EXECUTE_RUN)GetProcAddress(hDLL, "Concept_Execute3_Run");
            if (!Concept_Execute3_Run)
                Concept_Execute3_Run = (API_EXECUTE_RUN)GetProcAddress(hDLL, "_Concept_Execute3_Run");
            Concept_Execute3_Done = (API_EXECUTE_DONE)GetProcAddress(hDLL, "Concept_Execute3_Done");
            if (!Concept_Execute3_Done)
                Concept_Execute3_Done = (API_EXECUTE_DONE)GetProcAddress(hDLL, "_Concept_Execute3_Done");
        }
#else
        Concept_Execute3_Init = (API_EXECUTE_INIT)dlsym(hDLL, "Concept_Execute3_Init");
        Concept_Execute3_Run  = (API_EXECUTE_RUN)dlsym(hDLL, "Concept_Execute3_Run");
        Concept_Execute3_Done = (API_EXECUTE_DONE)dlsym(hDLL, "Concept_Execute3_Done");
#endif
    }


#ifdef _WIN32
    Concept_Execute = (API_INTERPRETER2)GetProcAddress(hDLL, "Concept_Execute");
    if (!Concept_Execute)
        Concept_Execute = (API_INTERPRETER2)GetProcAddress(hDLL, "_Concept_Execute");
#else
    Concept_Execute = (API_INTERPRETER2)dlsym(hDLL, "Concept_Execute");
#endif
    if (!Concept_Execute) {
        fprintf(stdout, "%s%c%c%c%c", "Content-Type: text/plain", 13, 10, 13, 10);
        fprintf(stdout, "Corrupted library : '%s'\n", LIBRARY);
        done_thread = 1;
        return 0;
    }

#ifdef _WIN32
    Concept_Compiler = (API_INTERPRETER)GetProcAddress(hDLL, "Concept_Compile");
    if (!Concept_Compiler)
        Concept_Compiler = (API_INTERPRETER)GetProcAddress(hDLL, "_Concept_Compile");

    SetNotifyParent = (API_SETNOTIFY)GetProcAddress(hDLL, "SetNotifyParent");
    if (!SetNotifyParent)
        SetNotifyParent = (API_SETNOTIFY)GetProcAddress(hDLL, "_SetNotifyParent");
#else
    Concept_Compiler = (API_INTERPRETER)dlsym(hDLL, "Concept_Compile");
    SetNotifyParent  = (API_SETNOTIFY)dlsym(hDLL, "SetNotifyParent");
#endif

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
    return 1;
}

void ConceptCGIDone() {
    if ((CONCEPT_CONTEXT) && (Concept_Execute3_Done)) {
        Concept_Execute3_Done(CONCEPT_CONTEXT);
        CONCEPT_CONTEXT = 0;
    }
    if (!hDLL)
        return;
#ifdef _WIN32
    FreeLibrary(hDLL);
#else
    dlclose(hDLL);
#endif
    hDLL = 0;
}

#ifdef _WIN32
DWORD WINAPI thread_main(void *) {
#else
void *thread_main(void *) {
#endif
    SetWebDirectory();

    const char *filename = s_file.Length() ? s_file.c_str() : getenv("PATH_TRANSLATED");
    if (!filename) {
        fprintf(stdout, "%s%c%c%c%c", "Content-Type: text/plain", 13, 10, 13, 10);
        fprintf(stdout, "Invalid WEBServer query\n");
        semp(sem_lock);
        done_thread = 1;
#ifndef _WIN32
        pthread_mutex_lock(&signal_mutex);
        pthread_cond_signal(&signal_done);
        pthread_mutex_unlock(&signal_mutex);
#endif
        semv(sem_lock);
        return 0;
    }
#ifdef _WIN32
    WSADATA data;
    WSAStartup(MAKEWORD(1, 1), &data);
#endif
    // check for autocompile
    int res = 0;
    if (SetNotifyParent)
        SetNotifyParent(RemoteNotify);
    if ((autocompile) && (need_compilation(filename)) && (Concept_Compiler))
        res = Concept_Compiler(filename, Include, Library, (void *)Print, 0, -1, 0, 0, 0, 0, 0, -1, -1, -1, -1);

    int  APID       = 0;
    int  parentAPID = -1;
    bool debug      = false;
    //char *key=0;
    AnsiString my_key;

    if ((allow_parents) || (allow_debug))
        if ((my_key.LoadFile(AnsiString(filename) + KEY_EXTENSION) != 0) || (!my_key.Length()))
            my_key.LoadFile(KEY_FILE);

    if (handle_cgi) {
        client_sock = Connect(host.c_str(), port);
        NotifyServer(client_sock, my_key.c_str(), &c_pipew, &c_piper, &APID, &parentAPID, &debug);
    }
    if (!allow_debug)
        debug = false;
    else
    if (!allow_parents)
        parentAPID = -1;


    if (!res) {
        int pipe_in  = c_piper ? c_piper : -1;
        int pipe_out = c_pipew ? c_pipew : -1;

        int  apid     = APID ? APID : -1;
        void *debuger = 0;
        if (debug) {
            debuger   = InitDebuger(filename, pipe_out, pipe_in, parentAPID);
            run_debug = 1;

            if ((CONCEPT_CONTEXT) && (Concept_Execute3_Done)) {
                Concept_Execute3_Done(CONCEPT_CONTEXT);
                CONCEPT_CONTEXT = 0;
            }

            res = Concept_Execute(filename, Include, Library, (void *)Print, 0, -2, CONCEPT_DEBUGGER_TRAP, debuger, 0, 0, 0, pipe_in, pipe_out, apid, parentAPID, 0);
        } else {
            if ((!CONCEPT_CONTEXT) && (Concept_Execute3_Init)) {
                CONCEPT_CONTEXT = Concept_Execute3_Init(filename, Include, Library, (void *)Print, 0, -1, 0, 0, 0, 0, 0, pipe_in, pipe_out, apid, parentAPID, 0);
                if (CONCEPT_CONTEXT)
                    res = Concept_Execute3_Run(CONCEPT_CONTEXT, 0, -1, 0, 0, 0, 0, 0, pipe_in, pipe_out, apid, parentAPID);
            } else
            if (CONCEPT_CONTEXT)
                res = Concept_Execute3_Run(CONCEPT_CONTEXT, 0, -1, 0, 0, 0, 0, 0, pipe_in, pipe_out, apid, parentAPID);
            else
                res = Concept_Execute(filename, Include, Library, (void *)Print, 0, -1, 0, 0, 0, 0, 0, pipe_in, pipe_out, apid, parentAPID, 0);
        }
        if (res == -3) {
            Print("<HEAD><TITLE>404 Not Found</TITLE></HEAD>\n", 0);
            Print("<BODY><H1>404 Not Found</H1>\n", 0);
            Print("The requested Concept script/URL was not found on this server.\n", 0);
            Print("</BODY>\n", 0);

            //fprintf(stdout, "Content-Type: text/html\r\nHTTP/1.0 404 Not Found\r\n");
            //fprintf(stdout, "%s%c%c\n","Content-Type: text/plain;charset=iso-8859-1",13,10);
            //fprintf(stdout, "File not found or insufficient rights : %s\r\n",filename);
            final_res = -1;
        }
        FinishPrint();
    }


    if (handle_cgi)
        Done();

    semp(sem_lock);
    done_thread = 1;
#ifndef _WIN32
    pthread_mutex_lock(&signal_mutex);
    pthread_cond_signal(&signal_done);
    pthread_mutex_unlock(&signal_mutex);
#endif
    semv(sem_lock);

#ifdef _WIN32
    WSACleanup();
#endif
    return /*res*/ 0;
}

int check_changed(time_t& last_mod, off_t& last_size, const char *path) {
    struct stat buf;

    if (!stat(path, &buf)) {
        if (!last_mod)
            last_mod = buf.st_mtime;
        else
        if (last_mod != buf.st_mtime)
            return 1;

        if (!last_size)
            last_size = buf.st_size;
        else
        if (last_size != buf.st_size)
            return 1;
    }
    return 0;
}

int check_file() {
    FILE *in       = 0;
    const char *filename = s_file.Length() ? s_file.c_str() : getenv("PATH_TRANSLATED");

    in = fopen(filename, "rb");
    if (in) {
        fclose(in);
        return 1;
    }

    fprintf(stdout, "%s%c%c%c%c", "Content-Type: text/html", 10, 13, 10, 13);
    fprintf(stdout, "<HEAD><TITLE>404 Not Found</TITLE></HEAD>\n");
    fprintf(stdout, "<BODY><H1>404 Not Found</H1>\n");
    fprintf(stdout, "The requested Concept script/URL was not found on this server.\n");
    fprintf(stdout, "</BODY>\n");
    return 0;
}

int ConceptCGIClient(int seconds) {
    done_thread = 0;
    int err = 0;
    if (!check_file())
        return 0;
    if (seconds <= 0) {
        thread_main(0);
        return 0;
    }
#ifdef _WIN32
    DWORD  threadID;
    HANDLE hthread = CreateThread(NULL, 0, thread_main, 0, 0, &threadID);
    if (WaitForSingleObject(hthread, seconds * 1000) != WAIT_OBJECT_0) {
        Print("<B>ERROR :</B> Script exceeded maximum allowed execution time.");
        FinishPrint();
        return -1;
        //exit(0);
    }
#else
    pthread_mutex_init(&signal_mutex, NULL);
    pthread_cond_init(&signal_done, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t threadID;
    pthread_create(&threadID, &attr, thread_main, NULL);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += seconds;

    pthread_mutex_lock(&signal_mutex);
    int res = pthread_cond_timedwait(&signal_done, &signal_mutex, &ts);
    pthread_mutex_unlock(&signal_mutex);
    if (res == ETIMEDOUT) {
        Print("<B>ERROR :</B> Script exceeded maximum allowed execution time.");
        FinishPrint();
        return -1;
    }
    pthread_join(threadID, NULL);
    pthread_attr_destroy(&attr);
    pthread_cond_destroy(&signal_done);
    pthread_mutex_destroy(&signal_mutex);
    signal_done  = PTHREAD_COND_INITIALIZER;
    signal_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
    FinishPrint();

    done_thread = 1;
    return 0;
}

#ifdef _WIN32

BOOL sigterm_handler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            Done();
            exit(0);
            break;
    }
    return FALSE;
}

#else

void sigterm_handler(int sig) {
    Done();
    exit(0);
}

AnsiString Temp() {
    AnsiString s(getenv("tmp"));

    if (!s.Length())
        s = "/tmp";
    return s;
}
#endif

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

#ifdef _WIN32
DWORD WINAPI watchdog_main(void *) {
#else
void *watchdog_main(void *) {
#endif
    while (true) {
        semp(sem_watchdog);
        if (time_marker) {
            time_t delta = time(NULL) - time_marker;
            if (delta > watchdog_timeout) {
                // nothing to process in the last watchdog_timeout seconds
                exit(0);
            }
        }
        semv(sem_watchdog);
        // minimize processing
#ifdef _WIN32
        Sleep(5000);
#else
        sleep(5);
#endif
    }
    return 0;
}

void InitWatchdog() {
    watchdog_timeout = GetKey(INI_PATH, "Web", "IdleTimeout", "60").ToInt();
    // disabled ?
    if (watchdog_timeout <= 0)
        return;
    time_marker = 0;
#ifdef _WIN32
    DWORD  threadID;
    HANDLE hthread = CreateThread(NULL, 0, watchdog_main, 0, 0, &threadID);
#else
    pthread_t threadID;
    pthread_create(&threadID, NULL, watchdog_main, NULL);
#endif
}

int main(int argc, char **argv) {
    s_file = (char *)getenv("PATH_TRANSLATED");
#ifdef _WIN32
    HOME_BASE = GetDirectory();
    INI_PATH  = HOME_BASE + INI_FILE;
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)sigterm_handler, TRUE);
#else
    signal(SIGTERM, sigterm_handler);
#endif
    if (!ConceptCGIInit(argc == 6))
#ifdef USE_FCGI
        return -1;
#else
        return 0;
#endif
    if (argc == 6) {
        s_file = argv[0];
#ifdef _WIN32
        AnsiString stdin_pipe(argv[1]);
        AnsiString stdout_pipe(argv[2]);
        AnsiString stderr_pipe(argv[3]);
        AnsiString comm_pipe(argv[4]);
        AnsiString backcomm_pipe(argv[5]);

        dup2(stdin_pipe.ToInt(), STDIN_FILENO);
        dup2(stdout_pipe.ToInt(), STDOUT_FILENO);
        dup2(stderr_pipe.ToInt(), STDERR_FILENO);

        putenv("CONCEPT_CGI=ConceptCGIHandler");
        com_in  = comm_pipe.ToInt();
        com_out = backcomm_pipe.ToInt();
#else
        AnsiString temp = Temp();

        AnsiString stdin_pipe(temp);
        stdin_pipe += argv[1];

        AnsiString stdout_pipe(temp);
        stdout_pipe += argv[2];

        // by file descriptor
        AnsiString stderr_pipe(argv[3]);

        int fd = open(stdout_pipe.c_str(), O_WRONLY);
        if (!fd)
            return -1;
        dup2(fd, STDOUT_FILENO);
        close(fd);

        fd = open(stdin_pipe.c_str(), O_RDONLY);
        if (!fd)
            return -1;
        dup2(fd, STDIN_FILENO);
        close(fd);

        dup2(stderr_pipe.ToInt(), STDERR_FILENO);

        AnsiString comm_pipe(temp);
        comm_pipe += argv[4];

        AnsiString backcomm_pipe(temp);
        backcomm_pipe += argv[5];

        setenv("CONCEPT_CGI", "ConceptCGIHandler", 1);

        if (comm_pipe == stdin_pipe)
            com_in = STDIN_FILENO;
        else
            com_in = open(backcomm_pipe.c_str(), O_WRONLY);

        com_out = open(backcomm_pipe.c_str(), O_WRONLY);
#endif

#ifdef _WIN32
        switch_to_binary();
#endif

        is_cached_cgi = 1;
    }
    seminit(sem_lock, 1);
    seminit(sem_watchdog, 1);

    int seconds = GetKey(INI_PATH, "Web", "Timeout", "30").ToInt();
    if (seconds < 0)
        seconds = 0;

#ifdef USE_FCGI
    int connection_count = 0;
    while (FCGI_Accept() >= 0) {
        if (check_file()) {
            ConceptCGIClient(seconds);
            connection_count++;
            if (connection_count == MAX_FCGI_SERV)
                break;
        } else {
            fprintf(stdout, "%s%c%c%c%c", "Content-Type: text/html", 10, 13, 10, 13);
            fprintf(stdout, "<HEAD><TITLE>404 Not Found</TITLE></HEAD>\n");
            fprintf(stdout, "<BODY><H1>404 Not Found</H1>\n");
            fprintf(stdout, "The requested Concept script/URL was not found on this server.\n");
            fprintf(stdout, "</BODY>\n");
        }
    }
#else
    char buffer[16382];
    if (check_file()) {
        if (is_cached_cgi) {
            time_t     last_mod     = 0;
            off_t      last_size    = 0;
            time_t     last_mod_a   = 0;
            off_t      last_size_a  = 0;
            int        file_changed = 0;
            AnsiString s_accel(s_file);
            s_accel += (char *)".accel";
            InitWatchdog();
            for (int i = 0; i < sessions; i++) {
                file_changed  = check_changed(last_mod, last_size, s_file.c_str());
                file_changed += check_changed(last_mod_a, last_size_a, s_accel.c_str());
                total_written = 0;
                _print_begin  = 1;
                _headers      = 1;
                pre_buffer    = (char *)"";
                int index = 0;
                while (true) {
                    char c;
                    semp(sem_watchdog);
                    time_marker = time(NULL);
                    semv(sem_watchdog);
                    int p_res = read(com_in, &c, 1);
                    if (p_res <= 0) {
                        return 0;
                    }
                    semp(sem_watchdog);
                    time_marker = 0;
                    semv(sem_watchdog);

                    if (c != '\n') {
                        if (index < 16381)
                            buffer[index++] = c;
                    } else {
                        if (index) {
                            buffer[index] = 0;
 #ifdef _WIN32
                            putenv(buffer);
 #else
                            char *val = strchr(buffer, '=');
                            if (val) {
                                val[0] = 0;
                                val++;
                                setenv(buffer, val, 1);
                            }
 #endif
                            index = 0;
                        } else
                            break;
                    }
                }
                unsigned int remaining_sessions = sessions - i - 1;
                if (ConceptCGIClient(seconds) < 0) {
                    remaining_sessions = 0;
                    sessions           = 0;
                }
                fflush(stdout);
                write(com_out, &total_written, sizeof(total_written));
                if (file_changed) {
                    remaining_sessions = 0;
                    sessions           = 0;
                }
                write(com_out, &remaining_sessions, sizeof(remaining_sessions));
                if (read(com_in, &remaining_sessions, sizeof(remaining_sessions)) != sizeof(remaining_sessions))
                    sessions = 0;
                else {
                    if (!remaining_sessions)
                        sessions = 0;
                }
            }
            //close(com_out);
            //close(com_in);
        } else
            ConceptCGIClient(seconds);
    }
#endif

    ConceptCGIDone();
    //semdel(sem_lock);
    //semdel(sem_watchdog);
#ifdef USE_FCGI
    return 1;
#else
    return 0;
#endif
}
