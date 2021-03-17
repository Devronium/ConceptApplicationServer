#include "debugger.h"
#include <string.h>

#define     DEBUG_RUN         0x01
#define     DEBUG_STEPINTO    0x02
#define     DEBUG_RUNTO       0x03
#define     DEBUG_STEPOVER    0x04

#define MAX_DEBUG_REQUEST     8192

int             __pipe_out = 0;
int             __pipe_in  = 0;
int             __p_apid   = 0;
ClientDebugInfo DINFO;

BREAKPOINT breakpoints[MAX_BREAKPOINTS];
int        max_breakpoints   = 0;
int        breakpoints_count = 0;

AnsiString REGISTERED_FILES[0xFF];
int        REGISTERED_COUNT = 0;


/*void write_debug(char *what) {
 *      FILE *f=fopen("c:\\debug.txt","rt+");
 *      if (f) {
 *              fseek(f, 0, SEEK_END);
 *              fwrite(what, 1, strlen(what), f);
 *              fclose(f);
 *      }
 * }
 */

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

int full_read(int fid, void *buf, int size) {
    int  r_size = 0;
    int  ret    = 0;
    char *pbuf  = (char *)buf;

    if (size <= 0)
        return 0;
    do {
        ret = read(fid, pbuf + r_size, size);
        if (ret > 0) {
            r_size += ret;
            size   -= r_size;
        } else
#ifdef _WIN32
            Sleep(10);
#else
            usleep(10000);
#endif
    } while ((ret >= 0) && (r_size < size));
    return r_size;
}

int GetParent(int *parent, int *msg_id, AnsiString *DATA) {
    if ((__p_apid <= 0) || (__pipe_out <= 0))
        return 0;

    int params[3];

    int size = full_read(__pipe_in, params, sizeof(int) * 3);

    if (size != sizeof(int) * 3)
        return -1;

    *parent = params[0];
    *msg_id = params[1];

    if (params[2]) {
        char *data = new char [params[2] + 1];
        data[params[2]] = 0;
        int rd = full_read(__pipe_in, data, params[2]);
        *DATA = data;
        delete[] data;
        if (rd != params[2])
            return 0;
    }
    return 1;
}

ClientDebugInfo *InitDebuger(AnsiString filename, int pipe_out, int pipe_in, int papid) {
    __pipe_out = pipe_out;
    __pipe_in  = pipe_in;
    __p_apid   = papid;

    DINFO.CLIENT_NOTICE = 0;
    DINFO.last_line     = 0;
    DINFO.sock          = 0;
    DINFO.debug_type    = DEBUG_STEPINTO;
    DINFO.run_to_line   = 0;
    DINFO.FileName      = filename;
    DINFO.context_id    = 0;
    DINFO.DEBUG_SOCKET  = 0;

    return &DINFO;
}

int NotifyParent(int msg_id, AnsiString DATA) {
    if ((__p_apid <= 0) || (__pipe_in <= 0))
        return 0;

    int params[3];
    params[0] = __p_apid;
    params[1] = msg_id;
    params[2] = DATA.Length();


    write(__pipe_out, params, sizeof(int) * 3);

    if (params[2])
        write(__pipe_out, DATA.c_str(), params[2]);

    return 1;
}

int RemoteNotify(int pipe_out, int apid, int msg_id, int len, char *data) {
    // ignore pipe_out parameter ...

    if (apid <= 0)
        return 0;

    int params[3];
    params[0] = apid;
    params[1] = msg_id;
    params[2] = len;

    write(__pipe_out, params, sizeof(int) * 3);

    if (params[2])
        write(__pipe_out, data, params[2]);

    return 1;
}

/*void write_debug(char *what) {
 *      FILE *f=fopen("c:\\debug.txt","rt+");
 *      if (f) {
 *              fseek(f, 0, SEEK_END);
 *              fwrite(what, 1, strlen(what), f);
 *              fclose(f);
 *      }
 * }*/

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

    /*write_debug("Breakpoint added: ");
     * write_debug(filename);
     * write_debug(", line ");
     * write_debug(AnsiString((long)line).c_str());
     * write_debug("\n");*/

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

char *ExtractFilename(char *filename) {
    int len = strlen(filename);

    for (int i = len - 1; i >= 0; i--)
        if ((filename[i] == '/') || (filename[i] == '\\'))
            return filename + i + 1;
    return filename;
}

int IsBreakpoint(char *filename, int line) {
    for (int i = 0; i < max_breakpoints; i++)
        if ((breakpoints[i].line == line) && (breakpoints[i].filename == filename))
            return 1;
    return 0;
}

int CONCEPT_DEBUGGER_TRAP(void *VDESC, void *CONTEXT, int Depth, int line, char *filename, GET_VARIABLE_PROC GVP, void *DEBUGGER_RESERVED, void *PIF, void *STACK_TRACE) {
    ClientDebugInfo *DINFO = (ClientDebugInfo *)DEBUGGER_RESERVED;
    // to do !
    // Debugger is now trapped !
    //if (DINFO->FileName!=filename)
    //   return 0;
    int  result = 0;
    int  parent;
    int  msg_id;
    char buffer2[0xFFFF];

    //if (!IS_REGISTERED(filename))
    //    return result;

    if (!DINFO)
        return result;
    char *filename2 = ExtractFilename(filename);

    switch (DINFO->debug_type) {
        case DEBUG_RUNTO:
            break;

        case DEBUG_RUN:
            if ((!IsBreakpoint(filename2, line)) && (!IsBreakpoint(filename, line)))
                break;

        case DEBUG_STEPINTO:
            {
                AnsiString DATA;
                if ((filename2) && (filename2[0]))
                    DATA = filename2;
                else
                    DATA = filename;
                DATA += (char *)"<:>";
                DATA += AnsiString((long)line);
                NotifyParent(-0x100, DATA);

                do {
                    msg_id = 0;
                    DATA.LoadBuffer(0, 0);
                    if (GetParent(&parent, &msg_id, &DATA) == -1) {
                        DINFO->debug_type = DEBUG_RUN;
                        break;
                    }
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
                    }
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
                    if ((filename2) && (filename2[0]))
                        DATA = filename2;
                    else
                        DATA = filename;
                    DATA += (char *)"<:>";
                    DATA += AnsiString((long)line);
                    NotifyParent(-0x100, DATA);

                    do {
                        if (GetParent(&parent, &msg_id, &DATA) == -1) {
                            DINFO->debug_type = DEBUG_RUN;
                            break;
                        }

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
