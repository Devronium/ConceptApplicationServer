//#define DEBUG

#ifdef _WIN32
 #include <windows.h>
 #include <direct.h>

 #define CMP_FUNC    stricmp
 #define LIBRARY     "concept-core.2.0.dll"
#else
 #include <dlfcn.h>
 #include <unistd.h>

 #define HANDLE      void *
 #define HMODULE     HANDLE

 #define SOCKET      int

 #define CMP_FUNC    strcmp
 #define LIBRARY     "libconceptcore.2.0.so"
#endif

#include "AnsiString.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COMPILER_COPYRIGHT    "Concept Compiler (Accelerator) version 5.0\n(c)2006-2019 Devronium Applications srl\n"

#define INI_FILE              "concept.ini"

typedef int (*GET_VARIABLE_PROC)(int operation, void *VDESC, void *CONTEXT, int Depth, const char *VariableName, char *buffer, int buf_size, void *PIF, void *STACK_TRACE);
typedef int (*DEBUGGER_CALLBACK)(void *VDESC, void *CONTEXT, int Depth, int line, const char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE);

extern "C" {
    typedef int (*API_INTERPRETER)(const char *filename, const char *inc_dir, const char *lib_dir, void *fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, const char *SERVER_PUBLIC_KEY, const char *SERVER_PRIVATE_KEY, const char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent);
}

void Print(char *str, int length) {
    if (length > 0)
        fwrite(str, 1, length, stdout);
    else
    if (str)
        fprintf(stdout, "%s", str);
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

    temp = getenv("PATH_TRANSLATED");
    if (temp != (char *)"") {
        buffer = (char *)temp.c_str();
        for (int i = strlen(buffer) - 1; i >= 0; i--)
            if ((buffer[i] == '/') || (buffer[i] == '\\')) {
                buffer[i + 1] = 0;
                break;
            }
        chdir(buffer);
    }
}

AnsiString GetKeyPath(AnsiString *HOME_BASE, const char *path, const char *category, const char *key, const char *default_value) {
    AnsiString temp;

    temp = GetKey(path, category, key, default_value);
    if (temp[0] == '.')
        temp = *HOME_BASE + temp;
    return temp;
}

int main(int argc, char **argv) {
    char       *filename = 0;
    HMODULE    hDLL      = 0;
    clock_t    start     = 0;
    clock_t    elapsed   = 0;
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
    int autocompile = GetKey(INI_PATH, "Run", "AutoCompile", "0").ToInt();

    if (argc == 2)
        filename = argv[1];
    else {
        fprintf(stderr, "USAGE: %s filename.con\n", argv[0]);
        return -1;
    }


    API_INTERPRETER Concept_Execute = 0;
#ifdef _WIN32
    hDLL = LoadLibrary(LIBRARY);
#else
    hDLL = dlopen(/*HOME_BASE+*/ LIBRARY_PATH, RTLD_LAZY);
#endif

    if (!hDLL) {
        fprintf(stderr, "Error loading core library '%s'\n", LIBRARY);
        return -1;
    }

    fprintf(stdout, "%s", COMPILER_COPYRIGHT);
#ifdef _WIN32
    Concept_Execute = (API_INTERPRETER)GetProcAddress(hDLL, "Concept_Compile");
    if (!Concept_Execute)
        Concept_Execute = (API_INTERPRETER)GetProcAddress(hDLL, "_Concept_Compile");
#else
    Concept_Execute = (API_INTERPRETER)dlsym(hDLL, "Concept_Compile");
#endif
    if (!Concept_Execute) {
        fprintf(stderr, "Corrupted library '%s'\n", LIBRARY);
        return -1;
    }

    SetWebDirectory();

    fprintf(stdout, "Compiling ...\n");
    start = clock();
    int res = Concept_Execute(filename, Include.c_str(), Library.c_str(), (void *)Print, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1);
    elapsed = clock() - start;
    if (res == -3)
        fprintf(stderr, "File not found or insufficient rights: '%s'\n", filename);
    else {
        double sec = (double)elapsed / CLOCKS_PER_SEC;
        int    min = (long)sec / 60;
        int    scs = (long)sec % 60;
        if (min)
            fprintf(stdout, "... done in %i minute(s) and %i second(s)\n", min, scs);
        else
            fprintf(stdout, "... done in %.2f second(s)\n", sec);
    }

#ifdef _WIN32
    FreeLibrary(hDLL);
#else
    dlclose(hDLL);
#endif

    return res;
}
