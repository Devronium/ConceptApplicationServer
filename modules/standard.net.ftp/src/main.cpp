//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
extern "C" {
#include "ftplib.h"
}


netbuf *last_con = 0;
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    AnsiString value = (long)FTPLIB_ASCII;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_ASCII ", value.c_str());

    value = (long)FTPLIB_IMAGE;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_IMAGE", value.c_str());

    value = (long)FTPLIB_CONNMODE;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_CONNMODE", value.c_str());

    value = (long)FTPLIB_CALLBACK;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_CALLBACK", value.c_str());

    value = (long)FTPLIB_IDLETIME;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_IDLETIME", value.c_str());

    value = (long)FTPLIB_CALLBACKARG;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_CALLBACKARG", value.c_str());

    value = (long)FTPLIB_CALLBACKBYTES;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_CALLBACKBYTES", value.c_str());

    value = (long)FTPLIB_DIR;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_DIR", value.c_str());

    value = (long)FTPLIB_DIR_VERBOSE;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_DIR_VERBOSE", value.c_str());

    value = (long)FTPLIB_FILE_READ;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_FILE_READ", value.c_str());

    value = (long)FTPLIB_FILE_WRITE;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "FTPLIB_FILE_WRITE", value.c_str());

    FtpInit();
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpConnect CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "FtpConnect takes one parameter: host");
    LOCAL_INIT;

    char *string = 0;

    GET_CHECK_STRING(0, string, "FtpConnect : parameter 0 should be a string (STATIC STRING)");

    netbuf *nControl;

    if (!FtpConnect(string, &nControl)) {
        nControl = 0;
    }

    last_con = nControl;
    RETURN_NUMBER((long)nControl);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpLogin CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 3, "FtpLogin takes 3 parameters: user,pass[,connection_handle]");
    LOCAL_INIT;

    char   *user = 0;
    char   *pass = 0;
    NUMBER handle;

    GET_CHECK_STRING(0, user, "FtpConnect : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, pass, "FtpConnect : parameter 1 should be a string (STATIC STRING)");
    if (PARAMETERS_COUNT == 3) {
        GET_CHECK_NUMBER(2, handle, "FtpConnect : parameter 2 should be a number (STATIC NUMBER)");
    } else
        handle = (long)last_con;

    netbuf *nControl = last_con;
    (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }


    RETURN_NUMBER(FtpLogin(user, pass, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpQuit CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "FtpQuit takes one parameter: connection_handle");
    LOCAL_INIT;

    NUMBER handle;

    GET_CHECK_NUMBER(0, handle, "FtpQuit : parameter 0 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }

    FtpQuit(nControl);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpOptions CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "FtpOptions takes 3 parameters: option,val,connection_handle");
    LOCAL_INIT;

    NUMBER opt;
    NUMBER val;
    NUMBER handle;

    GET_CHECK_NUMBER(0, opt, "FtpOptions : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, val, "FtpOptions : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(2, handle, "FtpOptions : parameter 2 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }


    RETURN_NUMBER(FtpOptions((int)opt, (long)val, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpSite CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "FtpSite takes2 parameters: site,connection_handle");
    LOCAL_INIT;

    char   *site = 0;
    NUMBER handle;

    GET_CHECK_STRING(0, site, "FtpConnect : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, handle, "FtpConnect : parameter 1 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }


    RETURN_NUMBER(FtpSite(site, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpLastResponse CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "FtpLastResponse takes one parameter: connection_handle");
    LOCAL_INIT;

    NUMBER handle;

    GET_CHECK_NUMBER(0, handle, "FtpLastResponse : parameter 0 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_STRING("");
        return 0;
    }

    RETURN_STRING(FtpLastResponse(nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpSysType CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "FtpSysType takes one parameter: connection_handle");
    LOCAL_INIT;

    NUMBER handle;

    GET_CHECK_NUMBER(0, handle, "FtpSysType : parameter 0 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_STRING("");
        return 0;
    }

    char buffer[0xFFFF];
    if (FtpSysType(buffer, 0xFFFF, nControl)) {
        RETURN_STRING(buffer);
    } else {
        RETURN_STRING("");
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpSize CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "FtpSize takes 3 parameters: path,mode,connection_handle");
    LOCAL_INIT;

    char   *path;
    NUMBER mode;
    NUMBER handle;

    GET_CHECK_STRING(0, path, "FtpSize : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, mode, "FtpSize : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(2, handle, "FtpSize : parameter 2 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(-1);
        return 0;
    }

    int size;
    if (FtpSize(path, &size, (char)mode, nControl)) {
        RETURN_NUMBER(size);
    } else {
        RETURN_NUMBER(-1);
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpModDate CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "FtpModDate takes 2 parameters: path,connection_handle");
    LOCAL_INIT;

    char   *path;
    NUMBER handle;

    GET_CHECK_STRING(0, path, "FtpSize : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, handle, "FtpSize : parameter 1 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_STRING("");
        return 0;
    }

    char buffer[0xFFFF];
    if (FtpModDate(path, buffer, 0xFFFF, nControl)) {
        RETURN_STRING(buffer);
    } else {
        RETURN_STRING("");
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpChdir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "FtpChdir takes 2 parameters: path,connection_handle");
    LOCAL_INIT;

    char   *path;
    NUMBER handle;

    GET_CHECK_STRING(0, path, "FtpChdir : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, handle, "FtpChdir : parameter 1 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER(FtpChdir(path, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpMkdir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "FtpMkdir takes 2 parameters: path,connection_handle");
    LOCAL_INIT;

    char   *path;
    NUMBER handle;

    GET_CHECK_STRING(0, path, "FtpMkdir : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, handle, "FtpMkdir : parameter 1 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER(FtpMkdir(path, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpRmdir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "FtpRmdir takes 2 parameters: path,connection_handle");
    LOCAL_INIT;

    char   *path;
    NUMBER handle;

    GET_CHECK_STRING(0, path, "FtpRmdir : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, handle, "FtpRmdir : parameter 1 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER(FtpRmdir(path, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpNlst CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "FtpNlst takes 3 parameters: out_filename, path,connection_handle");
    LOCAL_INIT;

    char   *out_filename;
    char   *path;
    NUMBER handle;

    GET_CHECK_STRING(0, out_filename, "FtpNlst : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, path, "FtpNlst : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(2, handle, "FtpNlst : parameter 2 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER(FtpNlst(out_filename, path, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpDir CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "FtpDir takes 3 parameters: out_filename, path,connection_handle");
    LOCAL_INIT;

    char   *out_filename;
    char   *path;
    NUMBER handle;

    GET_CHECK_STRING(0, out_filename, "FtpDir : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, path, "FtpDir : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(2, handle, "FtpDir : parameter 2 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER(FtpDir(out_filename, path, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpCDUp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "FtpCDUp takes one parameter: connection_handle");
    LOCAL_INIT;

    NUMBER handle;

    GET_CHECK_NUMBER(0, handle, "FtpCDUp : parameter 0 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(FtpCDUp(nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpPwd CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "FtpPwd takes one parameter: connection_handle");
    LOCAL_INIT;

    NUMBER handle;

    GET_CHECK_NUMBER(0, handle, "FtpCDUp : parameter 0 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_STRING("");
        return 0;
    }

    char buffer[0xFFFF];
    if (FtpPwd(buffer, 0xFFFF, nControl)) {
        RETURN_STRING(buffer);
    } else {
        RETURN_STRING("");
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpGet CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "FtpGet takes 4 parameters: local_filename,path,mode,connection_handle");
    LOCAL_INIT;

    char   *filename;
    char   *path;
    NUMBER mode;
    NUMBER handle;

    GET_CHECK_STRING(0, filename, "FtpGet : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, path, "FtpGet : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(2, mode, "FtpGet : parameter 2 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(3, handle, "FtpGet : parameter 3 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(FtpGet(filename, path, (char)mode, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpPut CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "FtpPut takes 4 parameters: local_filename,path,mode,connection_handle");
    LOCAL_INIT;

    char   *filename;
    char   *path;
    NUMBER mode;
    NUMBER handle;

    GET_CHECK_STRING(0, filename, "FtpPut : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, path, "FtpPut : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(2, mode, "FtpPut : parameter 2 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(3, handle, "FtpPut : parameter 3 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(FtpPut(filename, path, (char)mode, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpDelete CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "FtpDelete takes 2 parameters: path,connection_handle");
    LOCAL_INIT;

    char   *path;
    NUMBER handle;

    GET_CHECK_STRING(0, path, "FtpDelete : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, handle, "FtpDelete : parameter 1 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER(FtpDelete(path, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpRename CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "FtpRename takes 3 parameters: from_name, to_name, connection_handle");
    LOCAL_INIT;

    char   *filename;
    char   *path;
    NUMBER handle;

    GET_CHECK_STRING(0, filename, "FtpRename : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, path, "FtpRename : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(2, handle, "FtpRename : parameter 2 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(FtpRename(filename, path, nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpAccess CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "FtpAccess takes 4 parameters: path, type, mode, connection_handle");
    LOCAL_INIT;

    char   *path;
    NUMBER type;
    NUMBER mode;
    NUMBER handle;

    GET_CHECK_STRING(0, path, "FtpAccess : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, type, "FtpAccess : parameter 1 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(2, mode, "FtpAccess : parameter 2 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(3, handle, "FtpAccess : parameter 3 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }

    netbuf *nData;
    if (FtpAccess(path, (int)type, (char)mode, nControl, &nData)) {
        RETURN_NUMBER((long)nData);
    } else {
        RETURN_NUMBER(0);
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpClose CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "FtpClose takes one parameter: connection_handle");
    LOCAL_INIT;

    NUMBER handle;

    GET_CHECK_NUMBER(0, handle, "FtpClose : parameter 0 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(FtpClose(nControl));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpRead CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "FtpRead takes 2 parameters: size, connection_handle");
    LOCAL_INIT;

    NUMBER handle;
    NUMBER sizel;

    GET_CHECK_NUMBER(0, sizel, "FtpRead : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, handle, "FtpRead : parameter 1 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if ((!nControl) || (sizel <= 0)) {
        RETURN_STRING("");
        return 0;
    }

    char *buffer = new char[(int)sizel];
    int  size    = FtpRead(buffer, (int)sizel, nControl);
    if (size <= 0) {
        RETURN_STRING("");
    } else {
        RETURN_BUFFER(buffer, size);
    }
    delete[] buffer;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_FtpWrite CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "FtpRead takes 2 parameters: data, connection_handle");
    LOCAL_INIT;

    NUMBER handle;
    char   *buf;
    NUMBER len;

    GET_CHECK_BUFFER(0, buf, len, "FtpRead : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, handle, "FtpRead : parameter 1 should be a number (STATIC NUMBER)");

    netbuf *nControl = (netbuf *)(long)handle;
    if (!nControl) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(FtpWrite(buf, (int)len, nControl));
    return 0;
}
//---------------------------------------------------------------------------
