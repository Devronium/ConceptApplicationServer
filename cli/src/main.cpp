//#define DEBUG

#ifdef _WIN32
 #include <windows.h>
//      #include <sys/utime.h>
#else
 #include <dlfcn.h>        // dlopen

 #define HANDLE     void *
 #define HMODULE    HANDLE

 #define SOCKET     int
 #include <utime.h>
#endif

#include "AnsiString.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
//#include <dir.h>
 #include <direct.h>
#else
 #include <unistd.h>
#endif

#ifdef _WIN32
 #define CMP_FUNC    stricmp
#else
 #define CMP_FUNC    strcmp
//#define LINUX_PATH "/opt/Concept/bin/"
#endif

#ifdef _WIN32
 #define LIBRARY       "concept-core.2.0.dll"
 #define LIBRARY_MT    "concept-core.2.0.MT.dll"
#else
 #define LIBRARY       "libconceptcore.2.0.so"
 #define LIBRARY_MT    "libconceptcore.2.0.MT/libconceptcore.2.0.so"
#endif

#define INI_FILE       "concept.ini"

typedef int (*GET_VARIABLE_PROC)(int operation, void *VDESC, void *CONTEXT, int Depth, const char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);
typedef int (*DEBUGGER_CALLBACK)(void *VDESC, void *CONTEXT, int Depth, int line, const char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE);

extern "C" {
    typedef int (*API_INTERPRETER2)(const char *filename, const char *inc_dir, const char *lib_dir, void *fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, const char *SERVER_PUBLIC_KEY, const char *SERVER_PRIVATE_KEY, const char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, void *userdata);
    typedef int (*API_INTERPRETER)(const char *filename, const char *inc_dir, const char *lib_dir, void *fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, const char *SERVER_PUBLIC_KEY, const char *SERVER_PRIVATE_KEY, const char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent);
    typedef int (*CLI_INIT)(int argc, char **argv);
    typedef int (*RESULT_GET)();
}

void Print(char *str, int length) {
    if (!str)
        return;

    if (length <= 0)
        length = strlen(str);

    do {
        int res = fwrite(str, 1, length, stdout);
        if (res < 0)
            break;
        length -= res;
    } while (length > 0);
}

static int next_line = 0;
static int step_into = 0;

int DEBUGGER_TRAP(void *VDESC, void *CONTEXT, int Depth, int line, const char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE) {
    // to do !
    char varname[0xFF];
    char buffer[0xFFFF];
    char operation[0xFF];

    if ((!next_line) || (line == next_line) || (line < step_into)) {
        next_line = 0;
        step_into = 0;
        do {
            std::cout << "DEBUGGER TRAPPED ON LINE " << line << ".\nOperation ('bt', 'stepover', 'stepinto', 'watch'/'p', 'break', 'goto', 'run') : ";
            std::cin >> operation;
            if (!strcmp(operation, "stepover")) {
                next_line = line + 1;
                return 0;
            }
            if (!strcmp(operation, "stepinto")) {
                step_into = line;
                return 0;
            }
            if (!strcmp(operation, "break"))
                return 1;
            if (!strcmp(operation, "run")) {
                next_line = -1;
                return 0;
            }
            if (!strcmp(operation, "goto")) {
                std::cout << "Run to line : ";
                std::cin >> next_line;
                return 0;
            }
            if ((!strcmp(operation, "watch")) || (!strcmp(operation, "p"))) {
                std::cout << "Variable name : ";
                std::cin >> varname;
                if (GVP(0, VDESC, CONTEXT, Depth, varname, buffer, 0xFFFF, PIF, STACK_TRACE))
                    std::cout << varname << " : " << buffer << "\n";
                else
                    std::cout << varname << " : " << "(variable not defined)\n";
            }
            if ((!strcmp(operation, "stack")) || (!strcmp(operation, "bt"))) {
                if (GVP(-1, VDESC, CONTEXT, Depth, varname, buffer, 0xFFFF, PIF, STACK_TRACE))
                    std::cout << buffer << "\n";
                else
                    std::cout << "no stack info\n";
            }
        } while (1);
    }
    return 0;
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

AnsiString SetWebDirectory(AnsiString temp) {
    char       *buffer;
    AnsiString temp2;

    if (temp != (char *)"") {
        buffer = (char *)temp.c_str();
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

int need_compilation(const char *filename) {
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

AnsiString GetKeyPath(AnsiString *HOME_BASE, const char *path, const char *category, const char *key, const char *default_value) {
    AnsiString temp;

    temp = GetKey(path, category, key, default_value);
    if (temp[0] == '.')
        temp = *HOME_BASE + temp;
    return temp;
}

int main(int argc, char **argv) {
    const char *filename = 0;
    AnsiString ftemp;
    HMODULE    hDLL      = 0;
    int        debug     = 0;
    AnsiString HOME_BASE = GetDirectory();

#ifdef _WIN32
    AnsiString INI_PATH = HOME_BASE + INI_FILE;
#endif

#ifdef _WIN32
    AnsiString Library = GetKeyPath(&HOME_BASE, INI_PATH, "Paths", "Library", "Library") + "\\";
    AnsiString Include = GetKeyPath(&HOME_BASE, INI_PATH, "Paths", "Include", "Include") + "\\";
#else
    AnsiString Include = GetKeyPath(&HOME_BASE, INI_PATH, "Paths", "Include", "Include") + "/";
    AnsiString Library = GetKeyPath(&HOME_BASE, INI_PATH, "Paths", "Library", "Library") + "/";
#endif
    int  autocompile  = GetKey(INI_PATH, "Run", "AutoCompile", "0").ToInt();
    int  arg_start    = 2;
    char *force_chdir = getenv("CONCEPT_FORCE_CHDIR");
    int  chdir        = 0;
    int  mt           = 0;
    if (force_chdir)
        chdir = atoi(force_chdir);

    if (argc >= 3) {
        filename = argv[2];
        for (int i = 1; i < argc; i++) {
            char *arg = argv[i];
            if (arg[0] == '-') {
                if (!strcmp(arg, "-debug")) {
                    debug = 1;
                    arg_start++;
                    //arg_start = 3;
                } else
                if (!strcmp(arg, "-chdir")) {
                    chdir = 1;
                    arg_start++;
                    //arg_start = 3;
                } else
                if (!strcmp(arg, "-mt")) {
                    mt = 1;
                    arg_start++;
                } else {
                    fprintf(stderr, "Unknown option: %s\n", arg);
                    return -1;
                }
            } else {
                filename = argv[i];
                break;
            }
        }
    } else
    if (argc == 2) {
        filename  = argv[1];
        arg_start = 2;
    } else {
        fprintf(stderr, "USAGE: %s [-debug] [-chdir] [-mt] filename [arguments]\n", argv[0]);
        return -1;
    }

    if (!mt) {
        AnsiString manifest(filename);
        manifest += (char *)".manifest";
        mt        = GetKey(manifest.c_str(), "Application", "Multithreading", "0").ToInt();
    }

    API_INTERPRETER2 Concept_Execute  = 0;
    API_INTERPRETER  Concept_Compiler = 0;
    CLI_INIT         ARGInit          = 0;
    RESULT_GET       LastResult       = 0;
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
        if (mt)
            fprintf(stderr, "Error loading core library '%s\n", LIBRARY_MT);
        else
            fprintf(stderr, "Error loading core library '%s\n", LIBRARY);
        return -1;
    }

#ifdef _WIN32
    LastResult = (RESULT_GET)GetProcAddress(hDLL, "LastResult");
    if (!LastResult)
        LastResult = (RESULT_GET)GetProcAddress(hDLL, "_LastResult");
#else
    LastResult = (RESULT_GET)dlsym(hDLL, "LastResult");
#endif

#ifdef _WIN32
    ARGInit = (CLI_INIT)GetProcAddress(hDLL, "SetArguments");
    if (!ARGInit)
        ARGInit = (CLI_INIT)GetProcAddress(hDLL, "_SetArguments");
#else
    ARGInit = (CLI_INIT)dlsym(hDLL, "SetArguments");
#endif
    if (ARGInit)
        ARGInit(argc - arg_start, &argv[arg_start]);

#ifdef _WIN32
    Concept_Execute = (API_INTERPRETER2)GetProcAddress(hDLL, "Concept_Execute");
    if (!Concept_Execute)
        Concept_Execute = (API_INTERPRETER2)GetProcAddress(hDLL, "_Concept_Execute");
#else
    Concept_Execute = (API_INTERPRETER2)dlsym(hDLL, "Concept_Execute");
#endif
    if (!Concept_Execute) {
        fprintf(stderr, "Corrupted library : '%s'\n", LIBRARY);
        return -1;
    }

    if (chdir) {
        ftemp = SetWebDirectory(filename);
        if (ftemp.Length())
            filename = ftemp.c_str();
    }

#ifdef _WIN32
    Concept_Compiler = (API_INTERPRETER)GetProcAddress(hDLL, "Concept_Compile");
    if (!Concept_Compiler)
        Concept_Compiler = (API_INTERPRETER)GetProcAddress(hDLL, "_Concept_Compile");
#else
    Concept_Compiler = (API_INTERPRETER)dlsym(hDLL, "Concept_Compile");
#endif
    // check for autocompile
    int res = 0;
    if ((!debug) && (autocompile) && (need_compilation(filename)) && (Concept_Compiler))
        res = Concept_Compiler(filename, Include, Library, (void *)Print, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1);


    if (!res) {
        res = Concept_Execute(filename, Include, Library, (void *)Print, 0, debug, DEBUGGER_TRAP, 0, 0, 0, 0, -1, -1, -1, -1, 0);
        if (res == -3)
            fprintf(stderr, "File not found or insufficient rights: %s\n", filename);
        else 
        if (LastResult)
            res = LastResult();
    }

#ifdef _WIN32
    FreeLibrary(hDLL);
#else
    dlclose(hDLL);
#endif

    return res;
}
