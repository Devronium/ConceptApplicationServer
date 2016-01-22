//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include <libssh2.h>
#ifdef _WIN32
 #include <windows.h>
#else
 #include <unistd.h>
#endif

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(LIBSSH2_ERROR_NONE)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_SOCKET_NONE)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_BANNER_RECV)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_BANNER_SEND)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_INVALID_MAC)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_KEX_FAILURE)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_ALLOC)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_SOCKET_SEND)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_TIMEOUT)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_HOSTKEY_INIT)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_HOSTKEY_SIGN)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_DECRYPT)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_SOCKET_DISCONNECT)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_PROTO)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_PASSWORD_EXPIRED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_FILE)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_METHOD_NONE)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_AUTHENTICATION_FAILED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_CHANNEL_OUTOFORDER)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_CHANNEL_FAILURE)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_CHANNEL_UNKNOWN)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_CHANNEL_CLOSED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_CHANNEL_EOF_SENT)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_SCP_PROTOCOL)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_ZLIB)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_SOCKET_TIMEOUT)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_SFTP_PROTOCOL)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_REQUEST_DENIED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_METHOD_NOT_SUPPORTED)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_INVAL)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_INVALID_POLL_TYPE)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_PUBLICKEY_PROTOCOL)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_EAGAIN)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_BUFFER_TOO_SMALL)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_BAD_USE)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_COMPRESS)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_OUT_OF_BOUNDARY)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_AGENT_PROTOCOL)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_SOCKET_RECV)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_ENCRYPT)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_BAD_SOCKET)
    DEFINE_ECONSTANT(LIBSSH2_ERROR_KNOWN_HOSTS)
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SSHConnect, 1, 2)
    T_NUMBER(0)
    LIBSSH2_SESSION * session = libssh2_session_init();
    int sock = PARAM_INT(0);
    int rc   = 0;
    if (PARAMETERS_COUNT > 1) {
        SET_NUMBER(1, 0);
    }
    if (!session) {
        RETURN_NUMBER(0);
        return 0;
    }

    while ((rc = libssh2_session_handshake(session, sock)) == LIBSSH2_ERROR_EAGAIN) {
#ifdef _WIN32
        Sleep(1);
#else
        usleep(1000);
#endif
    }
    if (PARAMETERS_COUNT > 1) {
        SET_NUMBER(1, rc);
    }
    if (rc) {
        libssh2_session_free(session);
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER((SYS_INT)session);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHAuth, 3)
    T_HANDLE(0)
    T_STRING(1)
    T_STRING(2)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);
    int rc = 0;
    while ((rc = libssh2_userauth_password(session, PARAM(1), PARAM(2))) == LIBSSH2_ERROR_EAGAIN) {
#ifdef _WIN32
        Sleep(1);
#else
        usleep(1000);
#endif
    }
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHAuthPublicKey, 3)
    T_HANDLE(0)
    T_STRING(1)
    T_STRING(2)
    T_STRING(3)
    T_STRING(4)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);
    int rc = 0;
    while ((rc = libssh2_userauth_publickey_fromfile(session, PARAM(1), PARAM(2), PARAM(3), PARAM(4))) == LIBSSH2_ERROR_EAGAIN) {
#ifdef _WIN32
        Sleep(1);
#else
        usleep(1000);
#endif
    }
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHChannelOpen, 1)
    LIBSSH2_CHANNEL * channel = 0;
    T_HANDLE(0)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);

    while ((channel = libssh2_channel_open_session(session)) == NULL && libssh2_session_last_error(session, NULL, NULL, 0) == LIBSSH2_ERROR_EAGAIN) {
#ifdef _WIN32
        Sleep(10);
#else
        usleep(10000);
#endif
    }
    RETURN_NUMBER((SYS_INT)channel);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHFingerprint, 1)
    T_HANDLE(0)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);

    size_t     len  = 0;
    int        type = 0;
    const char *res = libssh2_session_hostkey(session, &len, &type);
    if (len > 0) {
        RETURN_BUFFER(res, len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHLastError, 1)
    T_HANDLE(0)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);
    int rc = libssh2_session_last_error(session, NULL, NULL, 0);
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHLastErrorString, 1)
    T_HANDLE(0)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);
    char *msg = 0;
    int  len  = 0;
    int  rc   = libssh2_session_last_error(session, &msg, &len, 0);
    if ((len > 0) && (msg)) {
        RETURN_BUFFER(msg, len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHExec, 2)
    T_HANDLE(0)
    T_STRING(1)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);
    int rc = 0;
    while ((rc = libssh2_channel_exec(channel, PARAM(1))) == LIBSSH2_ERROR_EAGAIN) {
#ifdef _WIN32
        Sleep(20);
#else
        usleep(20000);
#endif
    }
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SSHRead, 2, 4)
    T_HANDLE(0)
    T_NUMBER(1)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);
    int is_stderr = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        is_stderr = PARAM_INT(2);
    }
    char *buffer = 0;
    int  size    = PARAM_INT(1);
    CORE_NEW(size, buffer);
    int rc = 0;
    if (is_stderr)
        rc = libssh2_channel_read_stderr(channel, buffer, size);
    else
        rc = libssh2_channel_read(channel, buffer, size);
    if (PARAMETERS_COUNT > 3) {
        SET_NUMBER(3, rc);
    }

    if (rc <= 0) {
        CORE_DELETE(buffer);
        RETURN_STRING("");
    } else {
        SetVariable(RESULT, -1, buffer, rc);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SSHWrite, 2, 3)
    T_HANDLE(0)
    T_STRING(1)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);

    int is_stderr = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        is_stderr = PARAM_INT(2);
    }
    int rc = 0;
    if (is_stderr)
        rc = libssh2_channel_write_stderr(channel, PARAM(1), PARAM_LEN(1));
    else
        rc = libssh2_channel_write(channel, PARAM(1), PARAM_LEN(1));
    libssh2_channel_flush(channel);
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHShell, 1)
    T_HANDLE(0)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);

    int rc = libssh2_channel_shell(channel);
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHProcess, 3)
    T_HANDLE(0)
    T_STRING(1)
    T_STRING(2)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);

    int rc = libssh2_channel_process_startup(channel, PARAM(1), PARAM_LEN(1), PARAM_LEN(2) ? PARAM(2) : NULL, PARAM_LEN(2));
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHSendEOF, 1)
    T_HANDLE(0)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);

    int rc = libssh2_channel_send_eof(channel);
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHChannelClose, 1)
    T_HANDLE(0)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);
    int rc = 0;
    while ((rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN) {
#ifdef _WIN32
        Sleep(20);
#else
        usleep(20000);
#endif
    }
    if (rc == 0) {
        libssh2_channel_free(channel);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHDisconnect, 1)
    T_HANDLE(0)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);
    int rc = libssh2_session_disconnect(session, "Normal Shutdown");
    if (rc == 0) {
        libssh2_session_free(session);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHSetBlocking, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);

    libssh2_session_set_blocking(session, PARAM_INT(1));
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHSendKeepAlive, 2)
    T_HANDLE(0)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);
    int seconds = 0;
    int rc      = libssh2_keepalive_send(session, &seconds);
    SET_NUMBER(1, seconds);
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHSetKeepAlive, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    LIBSSH2_SESSION * session = (LIBSSH2_SESSION *)PARAM_INT(0);

    libssh2_keepalive_config(session, PARAM_INT(1), PARAM_INT(2));
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHChannelEOF, 1)
    T_HANDLE(0)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);
    int rc = libssh2_channel_eof(channel);
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHChannelPTY, 2)
    T_HANDLE(0)
    T_STRING(1)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);
    int rc = libssh2_channel_request_pty(channel, PARAM(1));
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SSHChannelSetEnv, 3)
    T_HANDLE(0)
    T_STRING(1)
    T_STRING(2)
    LIBSSH2_CHANNEL * channel = (LIBSSH2_CHANNEL *)PARAM_INT(0);
    int rc = libssh2_channel_setenv(channel, PARAM(1), PARAM(2));
    RETURN_NUMBER(rc);
END_IMPL
//------------------------------------------------------------------------
