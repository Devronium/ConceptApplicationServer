#include "stdlibrary.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef _WIN32
#include <errno.h>
#include <sys/stat.h>

// DLL Entry Point ... nothing ...
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void *lpReserved) {
    return 1;
}
#endif
//-----------------------------------------------------------------------------------
float *GetFloatList(void *arr, INVOKE_CALL _Invoke) {
    void    *newpData = 0;
    float   *ret      = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    ret        = new float[count + 1];
    ret[count] = 0;

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i] = 0;
            } else
                ret[i] = nData;
        }
    }
    return ret;
}

//-----------------------------------------------------//
char **GetCharList(void *arr, INVOKE_CALL _Invoke) {
    void    *newpData = 0;
    char    **ret     = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    ret        = new char * [count + 1];
    ret[count] = 0;

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i] = szData;
            } else
                ret[i] = 0;
        }
    }
    return ret;
}

//-----------------------------------------------------//
int *GetIntList(void *arr, INVOKE_CALL _Invoke) {
    void    *newpData = 0;
    int     *ret      = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    ret        = new int[count + 1];
    ret[count] = 0;

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i] = 0;
            } else
                ret[i] = (int)nData;
        }
    }
    return ret;
}

//-----------------------------------------------------//
double *GetDoubleList(void *arr, INVOKE_CALL _Invoke) {
    void    *newpData = 0;
    double  *ret      = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    ret        = new double[count + 1];
    ret[count] = 0;

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i] = 0;
            } else
                ret[i] = nData;
        }
    }
    return ret;
}

//-----------------------------------------------------//
bool *GetBoolList(void *arr, INVOKE_CALL _Invoke) {
    void    *newpData = 0;
    bool    *ret      = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    ret        = new bool[count + 1];
    ret[count] = 0;

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i] = 0;
            } else
                ret[i] = (bool)nData;
        }
    }
    return ret;
}
//-----------------------------------------------------//
#ifdef _WIN32
char *realpath(const char *path, char resolved_path[PATH_MAX]) {
    char *return_path = 0;

    if (path) {
        if (resolved_path) {
            return_path = resolved_path;
        } else {
            return_path = (char *)malloc(PATH_MAX); 
        }

        if (return_path) {
            //This is a Win32 API function similar to what realpath() is supposed to do
            size_t size = GetFullPathNameA(path, PATH_MAX, return_path, 0);

            //GetFullPathNameA() returns a size larger than buffer if buffer is too small
            if (size > PATH_MAX) {
                if (return_path != resolved_path) {
                    size_t new_size;
                    
                    free(return_path);
                    return_path = (char *)malloc(size);

                    if (return_path) {
                        new_size = GetFullPathNameA(path, size, return_path, 0); //Try again

                        if (new_size > size) {
                            free(return_path);
                            return_path = 0;
                            errno = ENAMETOOLONG;
                        } else {
                            size = new_size;
                        }
                    } else {
                        errno = EINVAL;
                    }    
                } else {
                    return_path = 0;
                    errno = ENAMETOOLONG;
                }
            }

            if (!size)  {
                if (return_path != resolved_path)
                    free(return_path);
                
                return_path = 0;

                //Convert MS errors into standard errors
                switch (GetLastError()) {
                    case ERROR_FILE_NOT_FOUND:
                        errno = ENOENT;
                        break;

                    case ERROR_PATH_NOT_FOUND: case ERROR_INVALID_DRIVE:
                        errno = ENOTDIR;
                        break;

                    case ERROR_ACCESS_DENIED:
                        errno = EACCES;
                        break;
                    
                    default: //Unknown Error
                        errno = EIO;
                        break;
                }
            }

            //If we get to here with a valid return_path, we're still doing good
            if (return_path) {
                struct stat stat_buffer;

                //Make sure path exists, stat() returns 0 on success
                if (stat(return_path, &stat_buffer))  {
                    if (return_path != resolved_path)
                        free(return_path);
                
                    return_path = 0;
                }
            }
        } else {
            errno = EINVAL;
        }
    } else {
        errno = EINVAL;
    }

    return return_path;
}
#endif
//-----------------------------------------------------------------------------------
int IsSandBoxed(INVOKE_CALL Invoke, void *HANDLER) {
    int is_sandbox = Invoke(INVOKE_IS_SANDBOX, HANDLER);
    if (is_sandbox == 1)
        return 1;
    return 0;
}
//-----------------------------------------------------------------------------------
char *SafePath(char *path, INVOKE_CALL Invoke, void *HANDLER) {
    if (!IsSandBoxed(Invoke, HANDLER))
        return strdup(path ? path : "");

    const char *file_path = NULL;
    static const char local_path[] = "./";

    if (!IS_OK(Invoke(INVOKE_FILENAME, HANDLER, &file_path)))
        file_path = NULL;

    if ((!file_path) && (!file_path[0]))
        file_path = local_path;

    // at least 1 byte for real path (eg: / )
    char sandboxed_path[PATH_MAX - 1];
    sandboxed_path[0] = 0;

    if ((file_path) && (file_path[0])) {
        for (int i = strlen(file_path) - 1; i >= 0; i --) {
            if ((file_path[i] == '/') || (file_path[i] == '\\')) {
                if (i < (int)sizeof(sandboxed_path)) {
                    char buffer[PATH_MAX - 1];
                    buffer[0] = 0;
                    strncat(buffer, file_path, i);
                    if (realpath(buffer, sandboxed_path)) {
                        int len = strlen(sandboxed_path);
                        if (len < ((int)sizeof(sandboxed_path) - 1)) {
                            sandboxed_path[len] = '/';
                            sandboxed_path[len + 1] = 0;
                        }
                    } else  {
                        sandboxed_path[0] = 0;
                    }
                }
                break;
            }
        }
    }


    if (!sandboxed_path[0])
        return strdup("");

    char path_temp[PATH_MAX];
    path_temp[0] = 0;

    strncat(path_temp, sandboxed_path, sizeof(path_temp) - 1);
    strncat(path_temp, path, sizeof(path_temp) - strlen(sandboxed_path) - 1);

    char *full_path = realpath(path_temp, NULL);
    if (full_path) {
        // ensure sandboxed
        // avoid path hijaking
        if (strstr(full_path, sandboxed_path) != full_path) {
            int len = strlen(sandboxed_path);
            if (len > 0) {
                // support mix of / and \ paths
                if (sandboxed_path[len - 1] == '/') {
                    sandboxed_path[len - 1] = '\\';
                } else
                if (sandboxed_path[len - 1] == '\\') {
                    sandboxed_path[len - 1] = '/';
                }
                if (strstr(full_path, sandboxed_path) != full_path)
                    full_path[0] = 0;
            } else
                full_path[0] = 0;
        }
    } else {
        full_path = (char *)malloc(sizeof(char));
        // always return non-null
        full_path [0] = 0;
    }
    return full_path;
}
//-----------------------------------------------------------------------------------
