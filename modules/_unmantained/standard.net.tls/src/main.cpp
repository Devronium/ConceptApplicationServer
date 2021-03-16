//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include <unistd.h>

extern "C" {
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/bio.h>
}

typedef struct {
    SSL_CTX    *sslctx;
    int        socket;
    SSL        *ssl;
    char       server;
    AnsiString userdata;
} SSLContainer;

#if defined(WIN32)
 #define MUTEX_TYPE    HANDLE
 #define MUTEX_SETUP(x)      (x) = CreateMutex(NULL, FALSE, NULL)
 #define MUTEX_CLEANUP(x)    CloseHandle(x)
 #define MUTEX_LOCK(x)       WaitForSingleObject((x), INFINITE)
 #define MUTEX_UNLOCK(x)     ReleaseMutex(x)
 #define THREAD_ID     GetCurrentThreadId()
#else
 #include <pthread.h>
 #define MUTEX_TYPE    pthread_mutex_t
 #define MUTEX_SETUP(x)      pthread_mutex_init(&(x), NULL)
 #define MUTEX_CLEANUP(x)    pthread_mutex_destroy(&(x))
 #define MUTEX_LOCK(x)       pthread_mutex_lock(&(x))
 #define MUTEX_UNLOCK(x)     pthread_mutex_unlock(&(x))
 #define THREAD_ID    pthread_self()
#endif

/* This array will store all of the mutexes available to OpenSSL. */
static MUTEX_TYPE *mutex_buf = NULL;

static void locking_function(int mode, int n, const char *file, int line) {
    if (mode & CRYPTO_LOCK)
        MUTEX_LOCK(mutex_buf[n]);
    else
        MUTEX_UNLOCK(mutex_buf[n]);
}

static unsigned long id_function(void) {
    return (unsigned long)THREAD_ID;
}

int thread_setup(void) {
    int i;

    mutex_buf = (MUTEX_TYPE *)malloc(CRYPTO_num_locks() * sizeof(MUTEX_TYPE));
    if (!mutex_buf)
        return 0;
    for (i = 0; i < CRYPTO_num_locks(); i++)
        MUTEX_SETUP(mutex_buf[i]);
    CRYPTO_set_id_callback(id_function);
    CRYPTO_set_locking_callback(locking_function);
    return 1;
}

int thread_cleanup(void) {
    int i;

    if (!mutex_buf)
        return 0;
    CRYPTO_set_id_callback(NULL);
    CRYPTO_set_locking_callback(NULL);
    for (i = 0; i < CRYPTO_num_locks(); i++)
        MUTEX_CLEANUP(mutex_buf[i]);
    free(mutex_buf);
    mutex_buf = NULL;
    return 1;
}

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
/*#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup( MAKEWORD( 2, 2 ), &wsaData);
   #else
   #ifndef SO_NOSIGPIPE
        signal(SIGPIPE, SIG_IGN);
   #endif
 #endif*/
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    DEFINE_ECONSTANT(SSL_OP_SINGLE_DH_USE)
    DEFINE_ECONSTANT(SSL_FILETYPE_PEM)
    DEFINE_ECONSTANT(SSL_FILETYPE_ASN1)
    DEFINE_ECONSTANT(SSL_ERROR_WANT_READ)
    DEFINE_ECONSTANT(SSL_ERROR_WANT_WRITE)
    DEFINE_ECONSTANT(SSL_ERROR_WANT_CONNECT)
    DEFINE_ECONSTANT(SSL_ERROR_WANT_ACCEPT)
    DEFINE_ECONSTANT(SSL_ERROR_NONE)
    DEFINE_ECONSTANT(SSL_ERROR_ZERO_RETURN)
    DEFINE_ECONSTANT(SSL_ERROR_WANT_X509_LOOKUP)
    DEFINE_ECONSTANT(SSL_ERROR_SYSCALL)
    DEFINE_ECONSTANT(SSL_ERROR_SSL)
    DEFINE_ECONSTANT(X509_V_OK)
    DEFINE_ECONSTANT(X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT)
    DEFINE_ECONSTANT(X509_V_ERR_UNABLE_TO_GET_CRL)
    DEFINE_ECONSTANT(X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE)
    DEFINE_ECONSTANT(X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE)
    DEFINE_ECONSTANT(X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY)
    DEFINE_ECONSTANT(X509_V_ERR_CERT_SIGNATURE_FAILURE)
    DEFINE_ECONSTANT(X509_V_ERR_CRL_SIGNATURE_FAILURE)
    DEFINE_ECONSTANT(X509_V_ERR_CERT_NOT_YET_VALID)
    DEFINE_ECONSTANT(X509_V_ERR_CERT_HAS_EXPIRED)
    DEFINE_ECONSTANT(X509_V_ERR_CRL_NOT_YET_VALID)
    DEFINE_ECONSTANT(X509_V_ERR_CRL_HAS_EXPIRED)
    DEFINE_ECONSTANT(X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD)
    DEFINE_ECONSTANT(X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD)
    DEFINE_ECONSTANT(X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD)
    DEFINE_ECONSTANT(X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD)
    DEFINE_ECONSTANT(X509_V_ERR_OUT_OF_MEM)
    DEFINE_ECONSTANT(X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT)
    DEFINE_ECONSTANT(X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN)
    DEFINE_ECONSTANT(X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY)
    DEFINE_ECONSTANT(X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE)
    DEFINE_ECONSTANT(X509_V_ERR_CERT_CHAIN_TOO_LONG)
    DEFINE_ECONSTANT(X509_V_ERR_CERT_REVOKED)
    DEFINE_ECONSTANT(X509_V_ERR_INVALID_CA)
    DEFINE_ECONSTANT(X509_V_ERR_PATH_LENGTH_EXCEEDED)
    DEFINE_ECONSTANT(X509_V_ERR_INVALID_PURPOSE)
    DEFINE_ECONSTANT(X509_V_ERR_CERT_UNTRUSTED)
    DEFINE_ECONSTANT(X509_V_ERR_CERT_REJECTED)
    DEFINE_ECONSTANT(X509_V_ERR_SUBJECT_ISSUER_MISMATCH)
    DEFINE_ECONSTANT(X509_V_ERR_AKID_SKID_MISMATCH)
    DEFINE_ECONSTANT(X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH)
    DEFINE_ECONSTANT(X509_V_ERR_KEYUSAGE_NO_CERTSIGN)
    DEFINE_ECONSTANT(X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER)
    DEFINE_ECONSTANT(X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION)
    DEFINE_ECONSTANT(X509_V_ERR_KEYUSAGE_NO_CRL_SIGN)
    DEFINE_ECONSTANT(X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION)
    DEFINE_ECONSTANT(X509_V_ERR_INVALID_NON_CA)
    DEFINE_ECONSTANT(X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED)
    DEFINE_ECONSTANT(X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE)
    DEFINE_ECONSTANT(X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED)
    DEFINE_ECONSTANT(X509_V_ERR_INVALID_EXTENSION)
    DEFINE_ECONSTANT(X509_V_ERR_INVALID_POLICY_EXTENSION)
    DEFINE_ECONSTANT(X509_V_ERR_NO_EXPLICIT_POLICY)
    DEFINE_ECONSTANT(X509_V_ERR_UNNESTED_RESOURCE)
    DEFINE_ECONSTANT(X509_V_ERR_APPLICATION_VERIFICATION)
    thread_setup();
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    thread_cleanup();
    ERR_free_strings();
    EVP_cleanup();
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSServerContext, 0)
    SSLContainer * container = new SSLContainer();
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
    container->sslctx = SSL_CTX_new(TLSv1_2_server_method());
#else
    container->sslctx = SSL_CTX_new(SSLv23_server_method());
#endif
    SSL_CTX_set_session_cache_mode(container->sslctx, SSL_SESS_CACHE_OFF);
    container->ssl    = 0;
    container->socket = -1;
    container->server = 1;
//container->proxy = NULL;

    RETURN_NUMBER((SYS_INT)container);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSClientContext, 1)
    T_NUMBER(TLSClientContext, 0)
    int socket = PARAM_INT(0);
    if (socket < 0) {
        RETURN_NUMBER(0);
        return 0;
    }

    SSLContainer *container = new SSLContainer();
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
    container->sslctx = SSL_CTX_new(TLSv1_2_client_method());
#else
    container->sslctx = SSL_CTX_new(SSLv23_client_method());
#endif
    SSL_CTX_set_session_cache_mode(container->sslctx, SSL_SESS_CACHE_OFF);
    container->ssl    = SSL_new(container->sslctx);
    container->socket = socket;
    container->server = 0;
//container->proxy = NULL;
    SSL_set_fd(container->ssl, socket);

    RETURN_NUMBER((SYS_INT)container);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(TLSCertificateFile, 2, 3)
    T_HANDLE(TLSCertificateFile, 0)
    T_STRING(TLSCertificateFile, 1)
    int type = SSL_FILETYPE_PEM;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(TLSCertificateFile, 2)
        type = PARAM_INT(2);
    }

    SSLContainer *container = (SSLContainer *)(SYS_INT)PARAM(0);
    int          use_cert   = SSL_CTX_use_certificate_file(container->sslctx, PARAM(1), type);
    RETURN_NUMBER(use_cert);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSChainFile, 2)
    T_HANDLE(TLSChainFile, 0)
    T_STRING(TLSChainFile, 1)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int use_cert = SSL_CTX_use_certificate_chain_file(container->sslctx, PARAM(1));
    RETURN_NUMBER(use_cert);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSAddCA, 2)
    T_HANDLE(TLSAddCA, 0)
    T_STRING(TLSAddCA, 1)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    if (!container->ssl) {
        RETURN_NUMBER(-1);
        return 0;
    }
    int res = 0;
    BIO *bio;
    bio = BIO_new(BIO_s_mem());
    if (bio) {
        BIO_puts(bio, PARAM(1));
        X509 *certificate = PEM_read_bio_X509(bio, NULL, NULL, NULL);
        if (certificate) {
            res = SSL_add_client_CA(container->ssl, certificate);
            X509_free(certificate);
        }
        BIO_free(bio);
    }
    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSTrust, 2)
    T_HANDLE(TLSTrust, 0)
    T_STRING(TLSTrust, 1)

    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = SSL_CTX_load_verify_locations(container->sslctx, PARAM(1), NULL);

    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSTrustBuffer, 2)
    T_HANDLE(TLSTrustBuffer, 0)
    T_STRING(TLSTrustBuffer, 1)

    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = -1;

    X509 *cert;
    BIO  *mem;
    mem = BIO_new(BIO_s_mem());
    if (mem) {
        BIO_puts(mem, PARAM(1));
        cert = PEM_read_bio_X509(mem, NULL, 0, NULL);
        BIO_free(mem);
        if (cert)
            res = X509_STORE_add_cert(SSL_CTX_get_cert_store(container->sslctx), cert);
    }

    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSTrustLocation, 2)
    T_HANDLE(TLSTrustLocation, 0)
    T_STRING(TLSTrustLocation, 1)

    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = SSL_CTX_load_verify_locations(container->sslctx, NULL, PARAM(1));

    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSSetCAFile, 2)
    T_HANDLE(TLSSetCAFile, 0)
    T_STRING(TLSSetCAFile, 1)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);

    SSL_CTX_set_client_CA_list(container->sslctx, SSL_load_client_CA_file(PARAM(1)));
    RETURN_NUMBER(1)
END_IMPL
//------------------------------------------------------------------------
int pem_passwd_cb(char *buf, int size, int rwflag, void *password) {
    SSLContainer *container = (SSLContainer *)password;

    if (container) {
        strncpy(buf, container->userdata.c_str(), size);
        buf[size - 1] = '\0';
        return strlen(buf);
    }
    return 0;
}

CONCEPT_FUNCTION_IMPL(TLSPassphrase, 2)
    T_HANDLE(TLSPassphrase, 0)
    T_STRING(TLSPassphrase, 1)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);

    if ((container) && (container->sslctx)) {
        SSL_CTX_set_default_passwd_cb(container->sslctx, pem_passwd_cb);
        container->userdata.LoadBuffer(PARAM(1), PARAM_LEN(1));
        SSL_CTX_set_default_passwd_cb_userdata(container->sslctx, container);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSCertificate, 2)
    T_HANDLE(TLSCertificate, 0)
    T_STRING(TLSCertificate, 1)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = 0;
    BIO *bio;
    bio = BIO_new(BIO_s_mem());
    if (bio) {
        BIO_puts(bio, PARAM(1));
        X509 *certificate = PEM_read_bio_X509(bio, NULL, NULL, NULL);
        if (certificate) {
            res = SSL_CTX_use_certificate(container->sslctx, certificate);
            X509_free(certificate);
        }
        BIO_free(bio);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(TLSPrivateKeyFile, 2, 3)
    T_HANDLE(TLSPrivateKeyFile, 0)
    T_STRING(TLSPrivateKeyFile, 1)
    int type = SSL_FILETYPE_PEM;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(TLSPrivateKeyFile, 2)
        type = PARAM_INT(2);
    }
    SSLContainer *container = (SSLContainer *)(SYS_INT)PARAM(0);
    int          use_cert   = SSL_CTX_use_PrivateKey_file(container->sslctx, PARAM(1), type);
    RETURN_NUMBER(use_cert);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSPrivateKey, 2)
    T_HANDLE(TLSPrivateKey, 0)
    T_STRING(TLSPrivateKey, 1)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = 0;
    BIO *bio;
    bio = BIO_new(BIO_s_mem());
    if (bio) {
        BIO_puts(bio, PARAM(1));
        EVP_PKEY *certificate = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
        if (certificate) {
            res = SSL_CTX_use_PrivateKey(container->sslctx, certificate);
            EVP_PKEY_free(certificate);
        }
        BIO_free(bio);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSSet, 2)
    T_HANDLE(TLSSet, 0)
    T_NUMBER(TLSSet, 1)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = SSL_CTX_set_options(container->sslctx, PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSAccept, 3)
    T_HANDLE(TLSAccept, 0)
    T_NUMBER(TLSAccept, 1)

    SET_NUMBER(2, 0);

    SSLContainer *container = (SSLContainer *)(SYS_INT)PARAM(0);
    int          socket     = PARAM_INT(1);
    if (socket < 0) {
        RETURN_NUMBER(-2);
        return 0;
    }
    int res = 0;
    if (container->server == 1) {
        SSL *ssl = SSL_new(container->sslctx);

        SSL_set_fd(ssl, socket);
        res = SSL_accept(ssl);
        if (res < 0) {
            if (SSL_get_error(ssl, res) == SSL_ERROR_WANT_READ) {
                SSL_free(ssl);
                RETURN_NUMBER(1);
                return 0;
            } else {
                SSL_free(ssl);
                RETURN_NUMBER(-1);
                return 0;
            }
        } else {
            SSLContainer *container2 = new SSLContainer();
            container2->sslctx = container->sslctx;
            container2->ssl    = ssl;
            container2->socket = socket;
            container2->server = 2;
            //container2->proxy = NULL;
            SET_NUMBER(2, (SYS_INT)container2);
        }
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSConnect, 1)
    T_HANDLE(TLSConnect, 0)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    if (!container->ssl) {
        RETURN_NUMBER(-1);
        return 0;
    }
    int res = SSL_connect(container->ssl);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSDone, 1)
    T_NUMBER(TLSDone, 0)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    if (container) {
        /*if (container->proxy) {
           #ifdef _WIN32
            // don't wait for the thread
            TerminateThread(container->proxy, INFINITE);
            CloseHandle(container->proxy);
           #else
            // don't wait for the thread
            void *data;
            pthread_cancel(container->proxy, &data);
           #endif
            if (container->fd_in > 0)
                close(container->fd_in);
            if (container->fd_out > 0)
                close(container->fd_out);
           }*/

        if (container->ssl) {
            SSL_shutdown(container->ssl);
            SSL_free(container->ssl);
        }
        if (container->server != 2) {
            SSL_CTX_free(container->sslctx);
        }

        delete container;

        SET_NUMBER(0, 0);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSWrite, 2)
    T_HANDLE(TLSWrite, 0)
    T_STRING(TLSWrite, 1)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = -1;
    if (container->ssl)
        res = SSL_write(container->ssl, PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSRead, 3)
    T_HANDLE(TLSRead, 0)
    T_NUMBER(TLSRead, 2)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res  = -1;
    int size = PARAM_INT(2);
    if ((container->ssl) && (size > 0)) {
        char *out;
        CORE_NEW(size + 1, out);
        if (out) {
            res = SSL_read(container->ssl, out, size);
            if (res > 0) {
                out[res] = 0;
                SetVariable(PARAMETER(1), -1, out, res);
            } else {
                CORE_DELETE(out);
                SET_STRING(1, "");
            }
        }
    }
    if (res <= 0) {
        SET_STRING(1, "");
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSPending, 1)
    T_HANDLE(TLSPending, 0)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = -1;
    if (container->ssl)
        res = SSL_pending(container->ssl);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(TLSError, 0, 1)
    int res = ERR_get_error();
    char buf[0xFFF];
    if (PARAMETERS_COUNT > 0) {
        ERR_error_string_n(res, buf, 0xFFF);
        SET_STRING(0, buf)
    }
    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSGetError, 2)
    T_HANDLE(TLSGetError, 0)
    T_NUMBER(TLSGetError, 1)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = SSL_ERROR_NONE;
    if (container->ssl)
        res = SSL_get_error(container->ssl, PARAM_INT(1));
    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSCheck, 1)
    T_HANDLE(TLSCheck, 0)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = SSL_CTX_check_private_key(container->sslctx);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSVerify, 1)
    T_HANDLE(TLSVerify, 0)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    int res = -1;
//if (container->sslctx)
//    SSL_CTX_set_default_verify_paths(container->sslctx);
    if (container->ssl) {
        res = SSL_get_verify_result(container->ssl);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
void AddKey(X509_NAME *name, void *RESULT, INVOKE_CALL Invoke, char *key) {
    char asnbuf[1024];

    if ((name) && (name->entries)) {
        void *var = 0;
        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, key, &var);
        if (var) {
            CREATE_ARRAY(var);
            STACK_OF(X509_NAME_ENTRY) * e_stack = name->entries;
            int len = sk_X509_NAME_ENTRY_num(e_stack);
            for (int i = 0; i < len; i++) {
                X509_NAME_ENTRY *entry = sk_X509_NAME_ENTRY_value(e_stack, i);
                if ((entry) && (entry->object) && (entry->value)) {
                    if (OBJ_obj2txt(asnbuf, sizeof(asnbuf), entry->object, 0))
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, asnbuf, (INTEGER)VARIABLE_STRING, (char *)entry->value->data, (NUMBER)entry->value->length);
                }
            }
        }
    }
}

CONCEPT_FUNCTION_IMPL(TLSCertificateInfo, 1)
    T_HANDLE(TLSCertificateInfo, 0)
    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    char *line;
    char hashbuf[0xFF];
    CREATE_ARRAY(RESULT);
    if (!container->ssl)
        return 0;

    X509 *cert = SSL_get_peer_certificate(container->ssl);
    if (cert != NULL) {
        AddKey(X509_get_subject_name(cert), RESULT, Invoke, "subject");
        AddKey(X509_get_issuer_name(cert), RESULT, Invoke, "issuer");
        ASN1_INTEGER *serial = X509_get_serialNumber(cert);
        BIGNUM       *bnser  = ASN1_INTEGER_to_BN(serial, NULL);
        if (bnser) {
            line = BN_bn2hex(bnser);
            BN_free(bnser);
            if (line) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "serialNumber", (INTEGER)VARIABLE_STRING, (char *)line, (NUMBER)0);
                OPENSSL_free(line);
            }
        }
        ASN1_TIME *IssuedOn = X509_get_notBefore(cert);
        if (IssuedOn)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "issued", (INTEGER)VARIABLE_STRING, (char *)IssuedOn->data, (NUMBER)0);

        ASN1_TIME *ExpiresOn = X509_get_notAfter(cert);
        if (ExpiresOn)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "expires", (INTEGER)VARIABLE_STRING, (char *)ExpiresOn->data, (NUMBER)0);

        const EVP_MD  *digest = EVP_get_digestbyname("sha1");
        unsigned char md[EVP_MAX_MD_SIZE];
        unsigned int  n;
        X509_digest(cert, digest, md, &n);
        hashbuf[0] = 0;
        char *ptr = hashbuf;
        for (int i = 0; i < n; i++) {
            sprintf(ptr, "%02X", md[i]);
            ptr += 2;
        }
        if (n)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sha1", (INTEGER)VARIABLE_STRING, (char *)hashbuf, (NUMBER)n * 2);

        digest = EVP_get_digestbyname("md5");
        X509_digest(cert, digest, md, &n);
        ptr = hashbuf;
        for (int i = 0; i < n; i++) {
            sprintf(ptr, "%02X", md[i]);
            ptr += 2;
        }
        if (n)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "md5", (INTEGER)VARIABLE_STRING, (char *)hashbuf, (NUMBER)n * 2);


        ASN1_BIT_STRING *pubKey = X509_get0_pubkey_bitstr(cert);
        if (pubKey) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "publicKey", (INTEGER)VARIABLE_STRING, (char *)pubKey->data, (NUMBER)pubKey->length);
            OPENSSL_free(line);
        }
        X509_free(cert);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSLocalInfo, 1)
    T_STRING(TLSLocalInfo, 0)

    char *line;
    char hashbuf[0xFF];

    CREATE_ARRAY(RESULT);
    BIO *bio;
    bio = BIO_new(BIO_s_mem());
    if (bio) {
        BIO_puts(bio, PARAM(0));
        X509 *cert = PEM_read_bio_X509(bio, NULL, NULL, NULL);
        if (cert) {
            AddKey(X509_get_subject_name(cert), RESULT, Invoke, "subject");
            AddKey(X509_get_issuer_name(cert), RESULT, Invoke, "issuer");
            ASN1_INTEGER *serial = X509_get_serialNumber(cert);
            BIGNUM       *bnser  = ASN1_INTEGER_to_BN(serial, NULL);
            if (bnser) {
                line = BN_bn2hex(bnser);
                BN_free(bnser);
                if (line) {
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "serialNumber", (INTEGER)VARIABLE_STRING, (char *)line, (NUMBER)0);
                    OPENSSL_free(line);
                }
            }
            ASN1_TIME *IssuedOn = X509_get_notBefore(cert);
            if (IssuedOn)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "issued", (INTEGER)VARIABLE_STRING, (char *)IssuedOn->data, (NUMBER)0);

            ASN1_TIME *ExpiresOn = X509_get_notAfter(cert);
            if (ExpiresOn)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "expires", (INTEGER)VARIABLE_STRING, (char *)ExpiresOn->data, (NUMBER)0);

            const EVP_MD  *digest = EVP_get_digestbyname("sha1");
            unsigned char md[EVP_MAX_MD_SIZE];
            unsigned int  n;
            X509_digest(cert, digest, md, &n);
            hashbuf[0] = 0;
            char *ptr = hashbuf;
            for (int i = 0; i < n; i++) {
                sprintf(ptr, "%02X", md[i]);
                ptr += 2;
            }
            if (n)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sha1", (INTEGER)VARIABLE_STRING, (char *)hashbuf, (NUMBER)n * 2);

            digest = EVP_get_digestbyname("md5");
            X509_digest(cert, digest, md, &n);
            ptr = hashbuf;
            for (int i = 0; i < n; i++) {
                sprintf(ptr, "%02X", md[i]);
                ptr += 2;
            }
            if (n)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "md5", (INTEGER)VARIABLE_STRING, (char *)hashbuf, (NUMBER)n * 2);


            ASN1_BIT_STRING *pubKey = X509_get0_pubkey_bitstr(cert);
            if (pubKey) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "publicKey", (INTEGER)VARIABLE_STRING, (char *)pubKey->data, (NUMBER)pubKey->length);
                OPENSSL_free(line);
            }

            X509_free(cert);
        }
        BIO_free(bio);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSSave, 1)
    T_HANDLE(TLSSave, 0)

    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    if ((container) && (container->ssl)) {
        SSL_SESSION *session = SSL_get_session(container->ssl);
        int         size     = i2d_SSL_SESSION(session, (unsigned char **)0);
        if (size <= 0) {
            RETURN_STRING("");
            return 0;
        }

        unsigned char *out;
        unsigned char *orig_out;
        CORE_NEW(size + 1, out);
        if (out) {
            orig_out = out;
            int e_size = i2d_SSL_SESSION(session, &out);
            if (e_size != size) {
                CORE_DELETE(out);
                RETURN_STRING("");
                return 0;
            }
            SetVariable(RESULT, -1, (char *)orig_out, e_size);
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSRestore, 2)
    T_HANDLE(TLSRestore, 0)
    T_STRING(TLSRestore, 1)

    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    if ((container) && (container->ssl)) {
        const unsigned char *ptr     = (unsigned char *)PARAM(1);
        SSL_SESSION         *session = d2i_SSL_SESSION((SSL_SESSION **)0, &ptr, PARAM_INT(1));
        // session link count is 1
        if (session) {
            // session link count is incremented by 1
            SSL_set_session(container->ssl, session);

            // decrement link count by 1
            SSL_SESSION_free(session);
            RETURN_NUMBER(1);
            return 0;
        }
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------

/*#define SERIAL_PUSH(size, out, type, val)       *((type *)out) = (type)val; out+=sizeof(type); size+=sizeof(type);
   #define SERIAL_PUSH_STR(size, out, val, len)    if (len > 0) { memcpy(out, val, len); out+=len; size+=len; }
   #define SERIAL_POP(size, out, type, val)        if (size >= sizeof(type)) { val = *((type *)out); out+=sizeof(type); size-=sizeof(type); }
   #define SERIAL_POP_STR(size, out, val, len)     if ((size >= len) && (len > 0)) {\
                                                    if (!val)\
                                                        val = (unsigned char *)OPENSSL_malloc(len);\
                                                    memcpy(val, out, len);\
                                                    out+=len;\
                                                    size-=len;\
                                                }
   #define SERIAL_POP_STR3(size, out, val, len)   if ((size >= len) && (len > 0)) {\
                                                    if (!val)\
                                                        val = (char *)OPENSSL_malloc(len);\
                                                    memcpy(val, out, len);\
                                                    out+=len;\
                                                    size-=len;\
                                                }
   #define SERIAL_POP_STR2(size, out, val, len)    if ((size >= len) && (len > 0)) { memcpy(val, out, len); out+=len; size-=len; }

   char *SerializeBIO(int &size, char *out, BIO *b) {
    char method = 0;
    if (b) {
        if (b->method == BIO_s_mem()) {
            method = 1;
        } else
        if (b->method == BIO_s_socket()) {
            method = 2;
        } else
        if (b->method == BIO_s_connect()) {
            method = 3;
        } else
        if (b->method == BIO_s_accept()) {
            method = 4;
        } else
        if (b->method == BIO_s_fd()) {
            method = 5;
        } else
        if (b->method == BIO_s_bio()) {
            method = 6;
        } else
        if (b->method == BIO_s_null()) {
            method = 7;
        } else
        if (b->method == BIO_f_null()) {
            method = 8;
        } else
        if (b->method == BIO_f_buffer()) {
            method = 9;
        } else
        if (b->method == BIO_f_nbio_test()) {
            method = 10;
        } else
        if (b->method == BIO_s_datagram()) {
            method = 11;
        }
    }
    out[0] = method;
    out++;
    size++;

    if (!method)
        return out;

    SERIAL_PUSH(size, out, int, b->init);
    SERIAL_PUSH(size, out, int, b->shutdown);
    SERIAL_PUSH(size, out, int, b->flags);
    SERIAL_PUSH(size, out, int, b->num);
    SERIAL_PUSH(size, out, int, b->retry_reason);
    SERIAL_PUSH(size, out, int, b->num_read);
    SERIAL_PUSH(size, out, int, b->num_write);

    return out;
   }

   char *UnserializeBIO(int &size, char *out, BIO **b) {
    if (size < 1)
        return out;
    char method = out[0];
    out++;
    size--;
 * b = NULL;

    switch (method) {
        case 1:
 * b = BIO_new(BIO_s_mem());
            break;
        case 2:
 * b = BIO_new(BIO_s_socket());
            break;
        case 3:
 * b = BIO_new(BIO_s_connect());
            break;
        case 4:
 * b = BIO_new(BIO_s_accept());
            break;
        case 5:
 * b = BIO_new(BIO_s_fd());
            break;
        case 6:
 * b = BIO_new(BIO_s_bio());
            break;
        case 7:
 * b = BIO_new(BIO_s_null());
            break;
        case 8:
 * b = BIO_new(BIO_f_null());
            break;
        case 9:
 * b = BIO_new(BIO_f_buffer());
            break;
        case 10:
 * b = BIO_new(BIO_f_nbio_test());
            break;
        case 11:
 * b = BIO_new(BIO_s_datagram());
            break;
    }
    if (*b) {
        SERIAL_POP(size, out, int, (*b)->init);
        SERIAL_POP(size, out, int, (*b)->shutdown);
        SERIAL_POP(size, out, int, (*b)->flags);
        SERIAL_POP(size, out, int, (*b)->num);
        SERIAL_POP(size, out, int, (*b)->retry_reason);
        SERIAL_POP(size, out, int, (*b)->num_read);
        SERIAL_POP(size, out, int, (*b)->num_write);
    }
    return out;
   }

   int GetEVP_CIPHER_CTXSize(EVP_CIPHER_CTX *ctx) {
    if (!ctx)
        return sizeof(int);
    int size = sizeof(int) * 9;
    size += EVP_MAX_IV_LENGTH;
    size += EVP_MAX_IV_LENGTH;
    size += EVP_MAX_BLOCK_LENGTH;
    size += EVP_MAX_BLOCK_LENGTH;
    if ((ctx) && (ctx->cipher) && (ctx->cipher->ctx_size > 0))
        size += ctx->cipher->ctx_size;
    return size;
   }

   char *SerializeEVP_CIPHER_CTX(int &size, char *out, EVP_CIPHER_CTX *ctx) {
    if (!ctx)  {
        SERIAL_PUSH(size, out, int, -1);
        return out;
    }
    int nid = 0;
    if (ctx->cipher)
        nid = EVP_CIPHER_nid(ctx->cipher);
    SERIAL_PUSH(size, out, int, nid);

    SERIAL_PUSH(size, out, int, ctx->encrypt);
    SERIAL_PUSH(size, out, int, ctx->buf_len);
    SERIAL_PUSH(size, out, int, ctx->num);
    SERIAL_PUSH(size, out, int, ctx->key_len);
    SERIAL_PUSH(size, out, unsigned long, ctx->flags);
    SERIAL_PUSH(size, out, int, ctx->final_used);
    SERIAL_PUSH(size, out, int, ctx->block_mask);

    SERIAL_PUSH_STR(size, out, ctx->oiv, sizeof(ctx->oiv));
    SERIAL_PUSH_STR(size, out, ctx->iv, sizeof(ctx->iv));
    SERIAL_PUSH_STR(size, out, ctx->buf, sizeof(ctx->buf));
    SERIAL_PUSH_STR(size, out, ctx->final, sizeof(ctx->final));

    if ((ctx->cipher_data) && (ctx->cipher) && (ctx->cipher->ctx_size > 0)) {
        SERIAL_PUSH(size, out, int, ctx->cipher->ctx_size);
        SERIAL_PUSH_STR(size, out, ctx->cipher_data, ctx->cipher->ctx_size);
    } else {
        SERIAL_PUSH(size, out, int, 0);
    }
    return out;
   }

   char *UnserializeEVP_CIPHER_CTX(int &size, char *out, EVP_CIPHER_CTX **ctx) {
    int nid = -1;
    SERIAL_POP(size, out, int, nid);
    if (nid < 0)
        return out;
    if (!(*ctx)) {
 * ctx = EVP_CIPHER_CTX_new();//(EVP_CIPHER_CTX *)OPENSSL_malloc(sizeof(EVP_CIPHER_CTX));
        EVP_CIPHER_CTX_init(*ctx);
    }
    (*ctx)->cipher = nid ? EVP_get_cipherbynid(nid) : NULL;

    SERIAL_POP(size, out, int, (*ctx)->encrypt);
    SERIAL_POP(size, out, int, (*ctx)->buf_len);
    SERIAL_POP(size, out, int, (*ctx)->num);
    SERIAL_POP(size, out, int, (*ctx)->key_len);
    SERIAL_POP(size, out, unsigned long, (*ctx)->flags);
    SERIAL_POP(size, out, int, (*ctx)->final_used);
    SERIAL_POP(size, out, int, (*ctx)->block_mask);

    SERIAL_POP_STR2(size, out, (*ctx)->oiv, sizeof((*ctx)->oiv));
    SERIAL_POP_STR2(size, out, (*ctx)->iv, sizeof((*ctx)->iv));
    SERIAL_POP_STR2(size, out, (*ctx)->buf, sizeof((*ctx)->buf));
    SERIAL_POP_STR2(size, out, (*ctx)->final, sizeof((*ctx)->final));

    int ctx_size = 0;
    SERIAL_POP(size, out, int, ctx_size);
    if (ctx_size > 0) {
        SERIAL_POP_STR(size, out, (*ctx)->cipher_data, ctx_size);
    }
    return out;
   }

   int GetEVP_MD_CTXSize(EVP_MD_CTX *ctx) {
    if (!ctx)
        return sizeof(int);
    int size = sizeof(int) * 2 + sizeof(unsigned long);
    if ((ctx) && (ctx->digest) && (ctx->digest->ctx_size > 0))
        size += ctx->digest->ctx_size;
    return size;
   }

   char *SerializeEVP_MD_CTX(int &size, char *out, EVP_MD_CTX *ctx) {
    int nid = -1;
    if ((!ctx) || (!ctx->digest)) {
        SERIAL_PUSH(size, out, int, nid);
        return out;
    }
    nid = EVP_MD_nid(ctx->digest);
    SERIAL_PUSH(size, out, int, nid);
    SERIAL_PUSH(size, out, unsigned long, ctx->flags);
    if (ctx->md_data) {
        SERIAL_PUSH(size, out, int, ctx->digest->md_size);
        SERIAL_PUSH_STR(size, out, ctx->md_data, ctx->digest->ctx_size);
    } else {
        SERIAL_PUSH(size, out, int, 0);
    }
    //???
        EVP_PKEY_CTX *pctx;
    return out;
   }

   char *UnserializeEVP_MD_CTX(int &size, char *out, EVP_MD_CTX **ctx) {
    int nid = -1;
    SERIAL_POP(size, out, int, nid);
    if (nid < 0)
        return out;
    if (!(*ctx)) {
 * ctx = EVP_MD_CTX_create();
        EVP_MD_CTX_init(*ctx);
    }
    (*ctx)->digest = EVP_get_digestbynid(nid);
    SERIAL_POP(size, out, unsigned long, (*ctx)->flags);

    int md_size = 0;
    SERIAL_PUSH(size, out, int, md_size);
    if (md_size) {
        SERIAL_POP_STR(size, out, (*ctx)->md_data, md_size);
    }

    //if ((*ctx)->digest)
    //    (*ctx)->update = (*ctx)->digest->update;
    return out;
   }

   CONCEPT_FUNCTION_IMPL(TLSSerialize, 1)
    T_HANDLE(TLSSerialize, 0)

    SSLContainer *container=(SSLContainer *)(SYS_INT)PARAM(0);
    if ((container) && (container->ssl)) {
        char *out = 0;
        SSL_SESSION *session = SSL_get_session(container->ssl);
        int session_size = i2d_SSL_SESSION(session, (unsigned char **)0);
        if (session_size < 0)
            session_size = 0;

        // (sizeof(int) * 7  + 1) = bio serialized size
        int e_size = sizeof(SSL) + container->ssl->packet_length + session_size + (sizeof(int) * 7  + 1) * 3;
        if (container->ssl->s3)
            e_size += sizeof(ssl3_state_st) + container->ssl->s3->tmp.key_block_length + container->ssl->s3->rbuf.len + container->ssl->s3->wbuf.len;

        e_size += GetEVP_CIPHER_CTXSize(container->ssl->enc_read_ctx);
        e_size += GetEVP_CIPHER_CTXSize(container->ssl->enc_write_ctx);
        e_size += GetEVP_MD_CTXSize(container->ssl->read_hash);
        e_size += GetEVP_MD_CTXSize(container->ssl->write_hash);

        CORE_NEW(e_size + 1, out);
        if (out) {
            char *orig_out = out;
            out[sizeof(SSL)] = 0;
            int size = 0;
 **#ifndef OPENSSL_NO_SSL2
            if (container->ssl->method == SSLv2_method()) {
                SERIAL_PUSH(size, out, char, 1);
            } else
            if (container->ssl->method == SSLv2_server_method()) {
                SERIAL_PUSH(size, out, char, 2);
            } else
            if (container->ssl->method == SSLv2_client_method()) {
                SERIAL_PUSH(size, out, char, 3);
            } else
 **#endif
 **#ifndef OPENSSL_NO_SSL3
            if (container->ssl->method == SSLv3_method()) {
                SERIAL_PUSH(size, out, char, 4);
            } else
            if (container->ssl->method == SSLv3_server_method()) {
                SERIAL_PUSH(size, out, char, 5);
            } else
            if (container->ssl->method == SSLv3_client_method()) {
                SERIAL_PUSH(size, out, char, 6);
            } else
 **#endif
            if (container->ssl->method == SSLv23_method()) {
                SERIAL_PUSH(size, out, char, 7);
            } else
            if (container->ssl->method == SSLv23_server_method()) {
                SERIAL_PUSH(size, out, char, 8);
            } else
            if (container->ssl->method == SSLv23_client_method()) {
                SERIAL_PUSH(size, out, char, 9);
            } else
 **#ifndef OPENSSL_NO_TLS1
            if (container->ssl->method == TLSv1_method()) {
                SERIAL_PUSH(size, out, char, 10);
            } else
            if (container->ssl->method == TLSv1_server_method()) {
                SERIAL_PUSH(size, out, char, 11);
            } else
            if (container->ssl->method == TLSv1_client_method()) {
                SERIAL_PUSH(size, out, char, 12);
            } else
 **#endif
            if (container->ssl->method == TLSv1_1_method()) {
                SERIAL_PUSH(size, out, char, 13);
            } else
            if (container->ssl->method == TLSv1_1_server_method()) {
                SERIAL_PUSH(size, out, char, 14);
            } else
            if (container->ssl->method == TLSv1_1_client_method()) {
                SERIAL_PUSH(size, out, char, 15);
            } else
            if (container->ssl->method == TLSv1_2_method()) {
                SERIAL_PUSH(size, out, char, 16);
            } else
            if (container->ssl->method == TLSv1_2_server_method()) {
                SERIAL_PUSH(size, out, char, 17);
            } else
            if (container->ssl->method == TLSv1_2_client_method()) {
                SERIAL_PUSH(size, out, char, 18);
            } else
            if (container->ssl->method == DTLSv1_method()) {
                SERIAL_PUSH(size, out, char, 19);
            } else
            if (container->ssl->method == DTLSv1_server_method()) {
                SERIAL_PUSH(size, out, char, 20);
            } else
            if (container->ssl->method == DTLSv1_client_method()) {
                SERIAL_PUSH(size, out, char, 21);
            } else {
                SERIAL_PUSH(size, out, char, 0);
            }
            SERIAL_PUSH(size, out, int, container->ssl->version);
            SERIAL_PUSH(size, out, int, container->ssl->type);
            SERIAL_PUSH(size, out, int, container->ssl->rwstate);
            SERIAL_PUSH(size, out, int, container->ssl->in_handshake);
            SERIAL_PUSH(size, out, int, container->ssl->server);
            SERIAL_PUSH(size, out, int, container->ssl->new_session);
            SERIAL_PUSH(size, out, int, container->ssl->quiet_shutdown);
            SERIAL_PUSH(size, out, int, container->ssl->shutdown);

            SERIAL_PUSH(size, out, int, container->ssl->state);
            SERIAL_PUSH(size, out, int, container->ssl->rstate);
            SERIAL_PUSH(size, out, int, container->ssl->init_num);
            SERIAL_PUSH(size, out, int, container->ssl->init_off);
            SERIAL_PUSH(size, out, int, container->ssl->packet_length);
            SERIAL_PUSH_STR(size, out, container->ssl->packet, container->ssl->packet_length);
            SERIAL_PUSH(size, out, int, container->ssl->read_ahead);
            SERIAL_PUSH(size, out, int, container->ssl->mac_flags);
            SERIAL_PUSH(size, out, int, container->ssl->hit);
            SERIAL_PUSH(size, out, int, container->ssl->sid_ctx_length);
            SERIAL_PUSH_STR(size, out, container->ssl->sid_ctx, container->ssl->sid_ctx_length);
            SERIAL_PUSH(size, out, int, container->ssl->verify_mode);
            SERIAL_PUSH(size, out, int, container->ssl->error);
            SERIAL_PUSH(size, out, int, container->ssl->error_code);
            SERIAL_PUSH(size, out, int, container->ssl->debug);
            SERIAL_PUSH(size, out, long, container->ssl->verify_result);
            SERIAL_PUSH(size, out, unsigned long, container->ssl->options);
            SERIAL_PUSH(size, out, unsigned long, container->ssl->mode);
            SERIAL_PUSH(size, out, long, container->ssl->max_cert_list);
            SERIAL_PUSH(size, out, int, container->ssl->first_packet);
            SERIAL_PUSH(size, out, int, container->ssl->client_version);
            SERIAL_PUSH(size, out, unsigned int, container->ssl->max_send_fragment);

            unsigned short host_len = 0;
            if (container->ssl->tlsext_hostname)
                host_len = (unsigned short)strlen(container->ssl->tlsext_hostname);
            SERIAL_PUSH(size, out, unsigned short, host_len);
            if (host_len) {
                SERIAL_PUSH_STR(size, out, container->ssl->tlsext_hostname, host_len);
            }
            SERIAL_PUSH(size, out, int, container->ssl->servername_done);
            SERIAL_PUSH(size, out, int, container->ssl->tlsext_status_type);
            SERIAL_PUSH(size, out, int, container->ssl->tlsext_status_expected);
            SERIAL_PUSH(size, out, int, container->ssl->tlsext_ocsp_resplen);
            SERIAL_PUSH_STR(size, out, container->ssl->tlsext_ocsp_resp, container->ssl->tlsext_ocsp_resplen);

            SERIAL_PUSH(size, out, size_t, container->ssl->tlsext_ecpointformatlist_length);
            SERIAL_PUSH_STR(size, out, container->ssl->tlsext_ecpointformatlist, container->ssl->tlsext_ecpointformatlist_length);
            SERIAL_PUSH(size, out, size_t, container->ssl->tlsext_ellipticcurvelist_length);
            SERIAL_PUSH_STR(size, out, container->ssl->tlsext_ellipticcurvelist, container->ssl->tlsext_ellipticcurvelist_length);

            SERIAL_PUSH(size, out, size_t, container->ssl->tlsext_opaque_prf_input_len);
            SERIAL_PUSH_STR(size, out, container->ssl->tlsext_opaque_prf_input, container->ssl->tlsext_opaque_prf_input_len);

            SERIAL_PUSH(size, out, int, container->ssl->next_proto_negotiated_len);
            SERIAL_PUSH_STR(size, out, container->ssl->next_proto_negotiated, container->ssl->next_proto_negotiated_len);

            SERIAL_PUSH(size, out, int, container->ssl->tlsext_heartbeat);
            SERIAL_PUSH(size, out, int, container->ssl->tlsext_hb_pending);
            SERIAL_PUSH(size, out, int, container->ssl->tlsext_hb_seq);

            SERIAL_PUSH(size, out, int, session_size);
            char *ptr = out;
            i2d_SSL_SESSION(session, (unsigned char **)&ptr);
            out += session_size;
            size += session_size;

            out = SerializeBIO(size, out, container->ssl->rbio);
            out = SerializeBIO(size, out, container->ssl->wbio);
            out = SerializeBIO(size, out, container->ssl->bbio);

            out = SerializeEVP_CIPHER_CTX(size, out, container->ssl->enc_read_ctx);
            out = SerializeEVP_CIPHER_CTX(size, out, container->ssl->enc_write_ctx);
            out = SerializeEVP_MD_CTX(size, out, container->ssl->read_hash);
            out = SerializeEVP_MD_CTX(size, out, container->ssl->write_hash);

            if (container->ssl->s3) {
                out[0] = 3;
                out++;
                size++;
                SERIAL_PUSH(size, out, long, container->ssl->s3->flags);
                SERIAL_PUSH(size, out, int, container->ssl->s3->delay_buf_pop_ret);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->read_sequence, sizeof(container->ssl->s3->read_sequence));
                SERIAL_PUSH(size, out, int, container->ssl->s3->read_mac_secret_size);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->read_mac_secret, container->ssl->s3->read_mac_secret_size);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->write_sequence, sizeof(container->ssl->s3->write_sequence));
                SERIAL_PUSH(size, out, int, container->ssl->s3->write_mac_secret_size);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->write_mac_secret, container->ssl->s3->write_mac_secret_size);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->server_random, SSL3_RANDOM_SIZE);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->client_random, SSL3_RANDOM_SIZE);

                SERIAL_PUSH(size, out, int, container->ssl->s3->need_empty_fragments);
                SERIAL_PUSH(size, out, int, container->ssl->s3->empty_fragment_done);
                SERIAL_PUSH(size, out, int, container->ssl->s3->init_extra);

                // ignore rest of the fields rec fields.
                SERIAL_PUSH(size, out, int, container->ssl->s3->rrec.type);
                SERIAL_PUSH(size, out, int, container->ssl->s3->wrec.type);

                SERIAL_PUSH(size, out, int, container->ssl->s3->alert_fragment_len);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->alert_fragment, container->ssl->s3->alert_fragment_len);

                SERIAL_PUSH(size, out, int, container->ssl->s3->handshake_fragment_len);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->handshake_fragment, container->ssl->s3->handshake_fragment_len);

                SERIAL_PUSH(size, out, unsigned int, container->ssl->s3->wnum);
                SERIAL_PUSH(size, out, int, container->ssl->s3->wpend_tot);
                SERIAL_PUSH(size, out, int, container->ssl->s3->wpend_type);
                SERIAL_PUSH(size, out, int, container->ssl->s3->wpend_ret);
                out = SerializeBIO(size, out, container->ssl->s3->handshake_buffer);
                //??EVP_MD_CTX **handshake_dgst;
                int dgst_count = 0;
                if (container->ssl->s3->handshake_dgst) {
                    // MAX_DIGGEST = 6
                    for (int i=0; i<6; i++)  {
                        if (container->ssl->s3->handshake_dgst[i]) {
                            EVP_MD_CTX *ctx = container->ssl->s3->handshake_dgst[i];
                            SERIAL_PUSH(size, out, int, i);
                            //fprintf(stderr, "HERE %x", ctx->md_data);
                        }
                    }
                }
                SERIAL_PUSH(size, out, int, -1);

                SERIAL_PUSH(size, out, int, container->ssl->s3->change_cipher_spec);
                SERIAL_PUSH(size, out, int, container->ssl->s3->warn_alert);
                SERIAL_PUSH(size, out, int, container->ssl->s3->fatal_alert);
                SERIAL_PUSH(size, out, int, container->ssl->s3->alert_dispatch);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->send_alert, sizeof(container->ssl->s3->send_alert));

                SERIAL_PUSH(size, out, int, container->ssl->s3->renegotiate);
                SERIAL_PUSH(size, out, int, container->ssl->s3->total_renegotiations);
                SERIAL_PUSH(size, out, int, container->ssl->s3->num_renegotiations);
                SERIAL_PUSH(size, out, int, container->ssl->s3->in_read_app_data);

                SERIAL_PUSH_STR(size, out, container->ssl->s3->tmp.cert_verify_md, sizeof(container->ssl->s3->tmp.cert_verify_md));
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.finish_md_len);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->tmp.finish_md, container->ssl->s3->tmp.finish_md_len);
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.peer_finish_md_len);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->tmp.peer_finish_md, container->ssl->s3->tmp.peer_finish_md_len);

                SERIAL_PUSH(size, out, unsigned long, container->ssl->s3->tmp.message_size);
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.message_type);

                const char *cipher_id = SSL_CIPHER_get_name(container->ssl->s3->tmp.new_cipher);
                if (!cipher_id)
                    cipher_id = "";

                unsigned short c_len = (unsigned short)strlen(cipher_id);
                SERIAL_PUSH(size, out, unsigned short, c_len);
                SERIAL_PUSH_STR(size, out, cipher_id, c_len);
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.next_state);
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.reuse_message);
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.cert_req);
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.ctype_num);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->tmp.ctype, sizeof(container->ssl->s3->tmp.ctype));

                //STACKOF(X590)??
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.key_block_length);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->tmp.key_block, container->ssl->s3->tmp.key_block_length);
                if (container->ssl->s3->tmp.new_sym_enc) {
                    int nid = EVP_CIPHER_nid(container->ssl->s3->tmp.new_sym_enc);
                    SERIAL_PUSH(size, out, int, nid);
                } else {
                    SERIAL_PUSH(size, out, int, -1);
                }
                if (container->ssl->s3->tmp.new_hash) {
                    int nid = EVP_MD_nid(container->ssl->s3->tmp.new_hash);
                    SERIAL_PUSH(size, out, int, nid);
                } else {
                    SERIAL_PUSH(size, out, int, -1);
                }
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.new_mac_pkey_type);
                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.new_mac_secret_size);

                SERIAL_PUSH(size, out, int, container->ssl->s3->tmp.cert_request);
                SERIAL_PUSH(size, out, char, container->ssl->s3->previous_client_finished_len);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->previous_client_finished, container->ssl->s3->previous_client_finished_len);
                SERIAL_PUSH(size, out, char, container->ssl->s3->previous_server_finished_len);
                SERIAL_PUSH_STR(size, out, container->ssl->s3->previous_server_finished, container->ssl->s3->previous_server_finished_len);
                SERIAL_PUSH(size, out, int, container->ssl->s3->send_connection_binding);
                SERIAL_PUSH(size, out, int, container->ssl->s3->next_proto_neg_seen);
                SERIAL_PUSH(size, out, char, container->ssl->s3->is_probably_safari);
            } else {
                out[0] = 0;
                out++;
                size++;
            }
            out[0] = 0;
            SetVariable(RESULT,-1,(char *)orig_out,size);
            return 0;
        }
    }
    RETURN_STRING("");
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(TLSUnserialize, 1, 3)
    T_STRING(TLSUnserialize, 0)

    SSLContainer *container=NULL;
    SSLContainer *container_orig=NULL;

    int sock = -1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(TLSUnserialize, 1)
        container = (SSLContainer *)(SYS_INT)PARAM(1);
        if (PARAMETERS_COUNT > 2) {
            T_NUMBER(TLSUnserialize, 2)
            sock = PARAM_INT(2);
        }
    }
    if (!container) {
        container = new SSLContainer();
        container->sslctx = SSL_CTX_new(SSLv23_server_method());
        container->ssl = 0;
        container->socket = sock;
        container->server = 1;
    } else {
        SSLContainer *container2 = new SSLContainer();
        container2->sslctx = container->sslctx;
        container2->ssl = 0;
        container2->socket = sock;
        container2->server = 2;
        container_orig = container;
        container = container2;
    }
    if (container)  {
        int size = PARAM_LEN(0);
        char *out = PARAM(0);
        SSL *ssl = SSL_new(container->sslctx);
        memset(ssl, 0, sizeof(SSL));
        if (sock > -1)
            SSL_set_fd(ssl, sock);
        ssl->references = 1;
        ssl->ctx = container->sslctx;

        // inca de serializat //
        ssl->read_hash = container_orig->ssl->read_hash;
        ssl->read_hash->pctx = NULL;
        ssl->write_hash = container_orig->ssl->write_hash;
        ssl->write_hash->pctx = NULL;
        EVP_PKEY *key = SSL_get_privatekey(container_orig->ssl);


        //ssl->write_hash->pctx = EVP_PKEY_CTX_new(ssl->cert, key->engine);

        char method_index = out[0];
        out++;
        size--;

        // == pana aici == //
 **#ifndef OPENSSL_NO_SSL2
        if (method_index == 1) {
            ssl->method = SSLv2_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 2) {
            ssl->method = SSLv2_server_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 3) {
            ssl->method = SSLv2_client_method();
            ssl->handshake_func = ssl->method->ssl_connect;
        } else
 **#endif
 **#ifndef OPENSSL_NO_SSL3
        if (method_index == 4) {
            ssl->method = SSLv3_method();
        } else
        if (method_index == 5) {
            ssl->method = SSLv3_server_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 6) {
            ssl->method = SSLv3_client_method();
            ssl->handshake_func = ssl->method->ssl_connect;
        } else
 **#endif
        if (method_index == 7) {
            ssl->method = SSLv23_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 8) {
            ssl->method = SSLv23_server_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 9) {
            ssl->method = SSLv23_client_method();
            ssl->handshake_func = ssl->method->ssl_connect;
        } else
 **#ifndef OPENSSL_NO_TLS1
        if (method_index == 10) {
            ssl->method = TLSv1_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 11) {
            ssl->method = TLSv1_server_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 12) {
            ssl->method = TLSv1_client_method();
            ssl->handshake_func = ssl->method->ssl_connect;
        } else
 **#endif
        if (method_index == 13) {
            ssl->method = TLSv1_1_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 14) {
            ssl->method = TLSv1_1_server_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 15) {
            ssl->method = TLSv1_1_client_method();
            ssl->handshake_func = ssl->method->ssl_connect;
        } else
        if (method_index == 16) {
            ssl->method = TLSv1_2_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 17) {
            ssl->method = TLSv1_2_server_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 18) {
            ssl->method = TLSv1_2_client_method();
            ssl->handshake_func = ssl->method->ssl_connect;
        } else
        if (method_index == 19) {
            ssl->method = DTLSv1_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 20) {
            ssl->method = DTLSv1_server_method();
            ssl->handshake_func = ssl->method->ssl_accept;
        } else
        if (method_index == 21) {
            ssl->method = DTLSv1_client_method();
            ssl->handshake_func = ssl->method->ssl_connect;
        }

        SERIAL_POP(size, out, int, ssl->version);
        SERIAL_POP(size, out, int, ssl->type);
        SERIAL_POP(size, out, int, ssl->rwstate);
        SERIAL_POP(size, out, int, ssl->in_handshake);
        SERIAL_POP(size, out, int, ssl->server);
        SERIAL_POP(size, out, int, ssl->new_session);
        SERIAL_POP(size, out, int, ssl->quiet_shutdown);
        SERIAL_POP(size, out, int, ssl->shutdown);
        SERIAL_POP(size, out, int, ssl->state);
        SERIAL_POP(size, out, int, ssl->rstate);
        SERIAL_POP(size, out, int, ssl->init_num);
        SERIAL_POP(size, out, int, ssl->init_off);
        SERIAL_POP(size, out, int, ssl->packet_length);
        SERIAL_POP_STR(size, out, ssl->packet, ssl->packet_length);
        SERIAL_POP(size, out, int, ssl->read_ahead);
        SERIAL_POP(size, out, int, ssl->mac_flags);
        SERIAL_POP(size, out, int, ssl->hit);
        SERIAL_POP(size, out, int, ssl->sid_ctx_length);
        SERIAL_POP_STR2(size, out, ssl->sid_ctx, ssl->sid_ctx_length);
        SERIAL_POP(size, out, int, ssl->verify_mode);
        SERIAL_POP(size, out, int, ssl->error);
        SERIAL_POP(size, out, int, ssl->error_code);
        SERIAL_POP(size, out, int, ssl->debug);
        SERIAL_POP(size, out, long, ssl->verify_result);
        SERIAL_POP(size, out, unsigned long, ssl->options);
        SERIAL_POP(size, out, unsigned long, ssl->mode);
        SERIAL_POP(size, out, long, ssl->max_cert_list);
        SERIAL_POP(size, out, int, ssl->first_packet);
        SERIAL_POP(size, out, int, ssl->client_version);
        SERIAL_POP(size, out, unsigned int, ssl->max_send_fragment);

        unsigned short host_len = 0;
        SERIAL_POP(size, out, unsigned short, host_len);
        SERIAL_POP_STR3(size, out, ssl->tlsext_hostname, host_len);
        SERIAL_POP(size, out, int, ssl->servername_done);
        SERIAL_POP(size, out, int, ssl->tlsext_status_type);
        SERIAL_POP(size, out, int, ssl->tlsext_status_expected);
        SERIAL_POP(size, out, int, ssl->tlsext_ocsp_resplen);
        SERIAL_POP_STR(size, out, ssl->tlsext_ocsp_resp, ssl->tlsext_ocsp_resplen);

        SERIAL_POP(size, out, size_t, ssl->tlsext_ecpointformatlist_length);
        SERIAL_POP_STR(size, out, ssl->tlsext_ecpointformatlist, ssl->tlsext_ecpointformatlist_length);
        SERIAL_POP(size, out, size_t, ssl->tlsext_ellipticcurvelist_length);
        SERIAL_POP_STR(size, out, ssl->tlsext_ellipticcurvelist, ssl->tlsext_ellipticcurvelist_length);

        SERIAL_POP(size, out, size_t, ssl->tlsext_opaque_prf_input_len);
        SERIAL_POP_STR(size, out, ssl->tlsext_opaque_prf_input, ssl->tlsext_opaque_prf_input_len);

        SERIAL_POP(size, out, int, ssl->next_proto_negotiated_len);
        SERIAL_POP_STR(size, out, ssl->next_proto_negotiated, ssl->next_proto_negotiated_len);

        SERIAL_POP(size, out, int, ssl->tlsext_heartbeat);
        SERIAL_POP(size, out, int, ssl->tlsext_hb_pending);
        SERIAL_POP(size, out, int, ssl->tlsext_hb_seq);

        int session_size = 0;
        SERIAL_POP(size, out, int, session_size);
        if ((session_size > 0) && (size >= session_size)) {
            const unsigned char *ptr=(unsigned char *)out;
            ssl->session = d2i_SSL_SESSION((SSL_SESSION **) 0, &ptr, session_size);
            size -= session_size;
            out += session_size;
            fprintf(stderr, "HOST: %s\n", ssl->session->tlsext_hostname);
        }
        out = UnserializeBIO(size, out, &ssl->rbio);
        out = UnserializeBIO(size, out, &ssl->wbio);
        out = UnserializeBIO(size, out, &ssl->bbio);

        out = UnserializeEVP_CIPHER_CTX(size, out, &ssl->enc_read_ctx);
        out = UnserializeEVP_CIPHER_CTX(size, out, &ssl->enc_write_ctx);
        out = UnserializeEVP_MD_CTX(size, out, &ssl->read_hash);
        out = UnserializeEVP_MD_CTX(size, out, &ssl->write_hash);
        if (out[0] == 3) {
            out++;
            size++;
            ssl->s3 = (SSL3_STATE *)OPENSSL_malloc(sizeof(SSL3_STATE));
            memset(ssl->s3, 0, sizeof(SSL3_STATE));
            SERIAL_POP(size, out, long, ssl->s3->flags);
            SERIAL_POP(size, out, int, ssl->s3->delay_buf_pop_ret);
            SERIAL_POP_STR2(size, out, ssl->s3->read_sequence, sizeof(ssl->s3->read_sequence));
            SERIAL_POP(size, out, int, ssl->s3->read_mac_secret_size);
            SERIAL_POP_STR2(size, out, ssl->s3->read_mac_secret, ssl->s3->read_mac_secret_size);
            SERIAL_POP_STR2(size, out, ssl->s3->write_sequence, sizeof(ssl->s3->write_sequence));
            SERIAL_POP(size, out, int, ssl->s3->write_mac_secret_size);
            SERIAL_POP_STR2(size, out, ssl->s3->write_mac_secret, ssl->s3->write_mac_secret_size);
            SERIAL_POP_STR2(size, out, ssl->s3->server_random, SSL3_RANDOM_SIZE);
            SERIAL_POP_STR2(size, out, ssl->s3->client_random, SSL3_RANDOM_SIZE);

            SERIAL_POP(size, out, int, ssl->s3->need_empty_fragments);
            SERIAL_POP(size, out, int, ssl->s3->empty_fragment_done);
            SERIAL_POP(size, out, int, ssl->s3->init_extra);

            SERIAL_POP(size, out, int, ssl->s3->rrec.type);
            SERIAL_POP(size, out, int, ssl->s3->wrec.type);

            SERIAL_POP(size, out, int, ssl->s3->alert_fragment_len);
            SERIAL_POP_STR2(size, out, ssl->s3->alert_fragment, ssl->s3->alert_fragment_len);

            SERIAL_POP(size, out, int, ssl->s3->handshake_fragment_len);
            SERIAL_POP_STR2(size, out, ssl->s3->handshake_fragment, ssl->s3->handshake_fragment_len);

            SERIAL_POP(size, out, unsigned int, ssl->s3->wnum);
            SERIAL_POP(size, out, int, ssl->s3->wpend_tot);
            SERIAL_POP(size, out, int, ssl->s3->wpend_type);
            SERIAL_POP(size, out, int, ssl->s3->wpend_ret);
            out = UnserializeBIO(size, out, &ssl->s3->handshake_buffer);
            int nid = -1;
            do {
                SERIAL_POP(size, out, int, nid);
                //EVP_MD_CTX *ctx = ssl->s3->handshake_dgst[i];
            } while (nid >= 0);

            SERIAL_POP(size, out, int, ssl->s3->change_cipher_spec);
            SERIAL_POP(size, out, int, ssl->s3->warn_alert);
            SERIAL_POP(size, out, int, ssl->s3->fatal_alert);
            SERIAL_POP(size, out, int, ssl->s3->alert_dispatch);
            SERIAL_POP_STR2(size, out, ssl->s3->send_alert, sizeof(ssl->s3->send_alert));

            SERIAL_POP(size, out, int, ssl->s3->renegotiate);
            SERIAL_POP(size, out, int, ssl->s3->total_renegotiations);
            SERIAL_POP(size, out, int, ssl->s3->num_renegotiations);
            SERIAL_POP(size, out, int, ssl->s3->in_read_app_data);

            SERIAL_POP_STR2(size, out, ssl->s3->tmp.cert_verify_md, sizeof(ssl->s3->tmp.cert_verify_md));
            SERIAL_POP(size, out, int, ssl->s3->tmp.finish_md_len);
            SERIAL_POP_STR2(size, out, ssl->s3->tmp.finish_md, ssl->s3->tmp.finish_md_len);
            SERIAL_POP(size, out, int, ssl->s3->tmp.peer_finish_md_len);
            SERIAL_POP_STR2(size, out, ssl->s3->tmp.peer_finish_md, ssl->s3->tmp.peer_finish_md_len);

            SERIAL_POP(size, out, unsigned long, ssl->s3->tmp.message_size);
            SERIAL_POP(size, out, int, ssl->s3->tmp.message_type);

            int c_len = 0;
            SERIAL_POP(size, out, unsigned short, c_len);
            if (c_len) {
                unsigned char *cipher_name = (unsigned char *)malloc(c_len + 1);
                cipher_name[0] = 0;
                cipher_name[c_len] = 0;
                SERIAL_POP_STR(size, out, cipher_name, c_len);
                // search cipher
                // not implemented
                //fprintf(stderr, "CIPHER: %s\n", (int)cipher_name);
                free(cipher_name);
            }

            SERIAL_POP(size, out, int, ssl->s3->tmp.next_state);
            SERIAL_POP(size, out, int, ssl->s3->tmp.reuse_message);
            SERIAL_POP(size, out, int, ssl->s3->tmp.cert_req);
            SERIAL_POP(size, out, int, ssl->s3->tmp.ctype_num);
            SERIAL_POP_STR2(size, out, ssl->s3->tmp.ctype, sizeof(ssl->s3->tmp.ctype));

            SERIAL_POP(size, out, int, ssl->s3->tmp.key_block_length);
            SERIAL_POP_STR(size, out, ssl->s3->tmp.key_block, ssl->s3->tmp.key_block_length);
            nid = -1;
            SERIAL_POP(size, out, int, nid);
            nid = -1;
            SERIAL_POP(size, out, int, nid);
            SERIAL_POP(size, out, int, ssl->s3->tmp.new_mac_pkey_type);
            SERIAL_POP(size, out, int, ssl->s3->tmp.new_mac_secret_size);

            SERIAL_POP(size, out, int, ssl->s3->tmp.cert_request);
            SERIAL_POP(size, out, char, ssl->s3->previous_client_finished_len);
            SERIAL_POP_STR2(size, out, ssl->s3->previous_client_finished, ssl->s3->previous_client_finished_len);
            SERIAL_POP(size, out, char, ssl->s3->previous_server_finished_len);
            SERIAL_POP_STR2(size, out, ssl->s3->previous_server_finished, ssl->s3->previous_server_finished_len);
            SERIAL_POP(size, out, int, ssl->s3->send_connection_binding);
            SERIAL_POP(size, out, int, ssl->s3->next_proto_neg_seen);
            SERIAL_POP(size, out, char, ssl->s3->is_probably_safari);
        } else {
            out++;
            size++;
        }
        //if (container->ssl)
        //    SSL_free(container->ssl);
        //ssl->s3 = container->ssl->s3;

        container->ssl = ssl;
        RETURN_NUMBER((SYS_INT)container);
        return 0;
    }
    RETURN_NUMBER((SYS_INT)container);
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSRenegotiate, 1)
    T_HANDLE(TLSRenegotiate, 0)

    SSLContainer * container = (SSLContainer *)(SYS_INT)PARAM(0);
    if ((container) && (container->ssl)) {
        if (SSL_renegotiate(container->ssl) <= 0) {
            RETURN_NUMBER(-2);
            return 0;
        }
        if (SSL_do_handshake(container->ssl) <= 0) {
            RETURN_NUMBER(-3);
            return 0;
        }

        if (container->server) {
            container->ssl->state = SSL_ST_ACCEPT;
            if (SSL_do_handshake(container->ssl) <= 0) {
                RETURN_NUMBER(-4);
                return 0;
            }
        }
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER(-1);
END_IMPL
//------------------------------------------------------------------------

/*#ifdef _WIN32
   DWORD WINAPI ProxyFunction(LPVOID DPARAM) {
   #else
   LPVOID ProxyFunction(LPVOID DPARAM) {
   #endif
        SSLContainer *container=(SSLContainer *)DPARAM;
    int socket = container->socket;
    int fd_in = container->fd_in;
    int fd_out = container->fd_out;
    struct timeval timeout;
    int t_out = 0;
    SSL *ssl = container->ssl;
   #ifdef _WIN32
    timeout.tv_sec = 0;
    timeout.tv_usec = t_out;
    fd_set socks;
    FD_ZERO(&socks);
    FD_SET(socket,&socks);
   #else
    struct pollfd ufds[1];
    ufds[0].fd = socket;
    ufds[0].events = POLLIN;

    struct pollfd ufds2[1];
    ufds2[0].fd = fd_in;
    ufds2[0].events = POLLIN;
   #endif
    char out[0xFFF];
    int has_data;
    int eof_val;
    while (ssl) {
        int ops = 0;
   #ifdef _WIN32
        int sel_val = select(FD_SETSIZE, &socks, 0, 0, &timeout);

        has_data = (sel_val!=0);
        if (sel_val < 0)
            break;
   #else
        int poll_val = poll(ufds, 1, (int)t_out);
        if (poll_val < 0)
            break;
        has_data = poll_val;
   #endif
        if (has_data) {
            ops++;
            int res = SSL_read(ssl, out, 0xFFF);
            if (res < 0) {
                int err = SSL_get_error(ssl, res);
                if ((err != SSL_ERROR_WANT_READ) && (err != SSL_ERROR_WANT_WRITE))
                    break;
            } else {
                int size = res;
                char *ptr = out;
                do {
                    res = write(fd_out, ptr, size);
                    if (res < 0)
                        break;
                    ptr += res;
                    size -= res;
                } while (size > 0);
                if (res < 0)
                    break;
            }
        }
   #ifdef _WIN32
        eof_val = eof(fd_in);
        if (eof_val < 0)
            break;
   #else
        poll_val = poll(ufds2, 1, (int)t_out);
        if (poll_val < 0)
            break;
        if (poll_val > 0)
            eof_val = 0;
        else
            eof_val = 1;
   #endif
        if (!eof_val) {
            int res = read(fd_in, out, 0xFFF);
            if (res < 0)
                break;
            if (res > 0) {
                ops++;
                int size = res;
                char *ptr = out;
                do {
                    res = SSL_write(ssl, ptr, size);
                    if (res < 0)
                        break;
                    ptr += res;
                    size -= res;
                } while (size > 0);
            }
        }

        if (!ops)
   #ifdef _WIN32
            Sleep(20);
   #else
            usleep(20000);
   #endif
    }
    close(fd_in);
    close(fd_out);
    container->fd_in = -1;
    container->fd_out = -1;
    return 0;
   }

   CONCEPT_FUNCTION_IMPL(TLSProxy, 3)
    T_HANDLE(TLSProxy, 0)
    T_NUMBER(TLSProxy, 1) // fd_in
    T_NUMBER(TLSProxy, 2) // fd_out
    SSLContainer *container=(SSLContainer *)(SYS_INT)PARAM(0);
    if (container->proxy)
        RETURN_NUMBER(-1);
    if ((container->ssl) && (container->socket > 0)) {
        container->fd_in  = PARAM_INT(1);
        container->fd_out = PARAM_INT(2);
   #ifdef _WIN32
        DWORD threadID = 0;
        container->proxy = CreateThread(NULL, 0, ProxyFunction, (LPVOID)container, 0, &threadID);
   #else
        pthread_create(&container->proxy, NULL, ProxyFunction,(LPVOID)container);
   #endif
        RETURN_NUMBER(1);
        return 0;
    }
    RETURN_NUMBER(0);
   END_IMPL*/
//--------------------------------------

