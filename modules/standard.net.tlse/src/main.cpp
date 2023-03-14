//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include "tlse.c"

static INVOKE_CALL InvokePtr = 0;

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(TLS_V13)
    DEFINE_ECONSTANT(TLS_V12)
    DEFINE_ECONSTANT(TLS_V11)
    DEFINE_ECONSTANT(TLS_V10)
    DEFINE_ECONSTANT(DTLS_V13)
    DEFINE_ECONSTANT(DTLS_V12)
    DEFINE_ECONSTANT(DTLS_V10)
    DEFINE_ECONSTANT(TLS_RSA_WITH_AES_128_CBC_SHA)
    DEFINE_ECONSTANT(TLS_RSA_WITH_AES_256_CBC_SHA)
    DEFINE_ECONSTANT(TLS_RSA_WITH_AES_128_CBC_SHA256)
    DEFINE_ECONSTANT(TLS_RSA_WITH_AES_256_CBC_SHA256)
    DEFINE_ECONSTANT(TLS_RSA_WITH_AES_128_GCM_SHA256)
    DEFINE_ECONSTANT(TLS_RSA_WITH_AES_256_GCM_SHA384)
    DEFINE_ECONSTANT(TLS_DHE_RSA_WITH_AES_128_CBC_SHA)
    DEFINE_ECONSTANT(TLS_DHE_RSA_WITH_AES_256_CBC_SHA)
    DEFINE_ECONSTANT(TLS_DHE_RSA_WITH_AES_128_CBC_SHA256)
    DEFINE_ECONSTANT(TLS_DHE_RSA_WITH_AES_256_CBC_SHA256)
    DEFINE_ECONSTANT(TLS_DHE_RSA_WITH_AES_128_GCM_SHA256)
    DEFINE_ECONSTANT(TLS_DHE_RSA_WITH_AES_256_GCM_SHA384)
    DEFINE_ECONSTANT(TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA)
    DEFINE_ECONSTANT(TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA)
    DEFINE_ECONSTANT(TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256)
    DEFINE_ECONSTANT(TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256)
    DEFINE_ECONSTANT(TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384)
    DEFINE_ECONSTANT(TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA)
    DEFINE_ECONSTANT(TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA)
    DEFINE_ECONSTANT(TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256)
    DEFINE_ECONSTANT(TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384)
    DEFINE_ECONSTANT(TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256)
    DEFINE_ECONSTANT(TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384)
    DEFINE_ECONSTANT(TLS_FALLBACK_SCSV)
    DEFINE_ECONSTANT(TLS_UNSUPPORTED_ALGORITHM)
    DEFINE_ECONSTANT(TLS_RSA_SIGN_RSA)
    DEFINE_ECONSTANT(TLS_RSA_SIGN_MD5)
    DEFINE_ECONSTANT(TLS_RSA_SIGN_SHA1)
    DEFINE_ECONSTANT(TLS_RSA_SIGN_SHA256)
    DEFINE_ECONSTANT(TLS_RSA_SIGN_SHA384)
    DEFINE_ECONSTANT(TLS_RSA_SIGN_SHA512)
    DEFINE_ECONSTANT(TLS_EC_PUBLIC_KEY)
    DEFINE_ECONSTANT(TLS_EC_prime192v1)
    DEFINE_ECONSTANT(TLS_EC_prime192v2)
    DEFINE_ECONSTANT(TLS_EC_prime192v3)
    DEFINE_ECONSTANT(TLS_EC_prime239v1)
    DEFINE_ECONSTANT(TLS_EC_prime239v2)
    DEFINE_ECONSTANT(TLS_EC_prime239v3)
    DEFINE_ECONSTANT(TLS_EC_prime256v1)
    DEFINE_ECONSTANT(TLS_EC_secp224r1)
    DEFINE_ECONSTANT(TLS_EC_secp256r1)
    DEFINE_ECONSTANT(TLS_EC_secp384r1)
    DEFINE_ECONSTANT(TLS_EC_secp521r1)
    DEFINE_ECONSTANT(TLS_NEED_MORE_DATA)
    DEFINE_ECONSTANT(TLS_GENERIC_ERROR)
    DEFINE_ECONSTANT(TLS_BROKEN_PACKET)
    DEFINE_ECONSTANT(TLS_NOT_UNDERSTOOD)
    DEFINE_ECONSTANT(TLS_NOT_SAFE)
    DEFINE_ECONSTANT(TLS_NO_COMMON_CIPHER)
    DEFINE_ECONSTANT(TLS_UNEXPECTED_MESSAGE)
    DEFINE_ECONSTANT(TLS_CLOSE_CONNECTION)
    DEFINE_ECONSTANT(TLS_COMPRESSION_NOT_SUPPORTED)
    DEFINE_ECONSTANT(TLS_NO_MEMORY)
    DEFINE_ECONSTANT(TLS_NOT_VERIFIED)
    DEFINE_ECONSTANT(TLS_INTEGRITY_FAILED)
    DEFINE_ECONSTANT(TLS_ERROR_ALERT)
    DEFINE_ECONSTANT(TLS_BROKEN_CONNECTION)
    DEFINE_ECONSTANT(TLS_BAD_CERTIFICATE)
    DEFINE_ECONSTANT(TLS_UNSUPPORTED_CERTIFICATE)

    DEFINE_ECONSTANT(close_notify)
    DEFINE_ECONSTANT(unexpected_message)
    DEFINE_ECONSTANT(bad_record_mac)
    DEFINE_ECONSTANT(decryption_failed_RESERVED)
    DEFINE_ECONSTANT(record_overflow)
    DEFINE_ECONSTANT(decompression_failure)
    DEFINE_ECONSTANT(handshake_failure)
    DEFINE_ECONSTANT(no_certificate_RESERVED)
    DEFINE_ECONSTANT(bad_certificate)
    DEFINE_ECONSTANT(unsupported_certificate)
    DEFINE_ECONSTANT(certificate_revoked)
    DEFINE_ECONSTANT(certificate_expired)
    DEFINE_ECONSTANT(certificate_unknown)
    DEFINE_ECONSTANT(illegal_parameter)
    DEFINE_ECONSTANT(unknown_ca)
    DEFINE_ECONSTANT(access_denied)
    DEFINE_ECONSTANT(decode_error)
    DEFINE_ECONSTANT(decrypt_error)
    DEFINE_ECONSTANT(export_restriction_RESERVED)
    DEFINE_ECONSTANT(protocol_version)
    DEFINE_ECONSTANT(insufficient_security)
    DEFINE_ECONSTANT(internal_error)
    DEFINE_ECONSTANT(inappropriate_fallback)
    DEFINE_ECONSTANT(user_canceled)
    DEFINE_ECONSTANT(no_renegotiation)
    DEFINE_ECONSTANT(unsupported_extension)
    DEFINE_ECONSTANT(no_error)

    DEFINE_ECONSTANT(SRTP_NULL)
    DEFINE_ECONSTANT(SRTP_AES_CM)
    DEFINE_ECONSTANT(SRTP_AUTH_NULL)
    DEFINE_ECONSTANT(SRTP_AUTH_HMAC_SHA1)

    tls_init();
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(TLSEServer, 0, 1)
    int version = TLS_V13;
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(TLSEServer, 0);
        version = PARAM_INT(0);
    }
    TLSContext *context = tls_create_context(1, version);
    RETURN_NUMBER((SYS_INT)context);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(TLSEClient, 0, 1)
    int version = TLS_V13;
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(TLSEClient, 0);
        version = PARAM_INT(0);
    }
    TLSContext *context = tls_create_context(0, version);
    RETURN_NUMBER((SYS_INT)context);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEDone, 1)
    T_NUMBER(TLSEDone, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    if (context) {
        if (context->user_data)
            Invoke(INVOKE_FREE_VARIABLE, context->user_data);
        SET_NUMBER(0, 0);
        tls_destroy_context(context);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEAccept, 1)
    T_HANDLE(TLSEAccept, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    TLSContext *res = tls_accept(context);
    RETURN_NUMBER((SYS_INT)res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSERequestClientCertificate, 1)
    T_HANDLE(TLSERequestClientCertificate, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    int res = tls_request_client_certificate(context);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(TLSEWrite, 2, 3)
    T_HANDLE(TLSEWrite, 0)
    T_STRING(TLSEWrite, 1)
    int offset = 0;
    int len = PARAM_LEN(1);
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(TLSEWrite, 2)
        offset = PARAM_INT(2);
        if (offset < 0)
            offset = 0;
        if (offset >= len) {
            RETURN_NUMBER(0);
            return 0;
        }
        len -= offset;
    }

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    int res = tls_write(context, (unsigned char *)PARAM(1) + offset, len);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSERead, 3)
    T_HANDLE(TLSERead, 0)
    T_NUMBER(TLSERead, 2)
    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    int size = PARAM_INT(2);
    if (size <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }
    int res = 0;
    if (context->application_buffer_len) {
        char *out;
        CORE_NEW(size + 1, out);
        if (out) {
            res = tls_read(context, (unsigned char *)out, size);
            if (res > 0) {
                out[res] = 0;
                SetVariable(PARAMETER(1), -1, out, res);
            } else {
                CORE_DELETE(out);
            }
        }
    }
    if (res <= 0) {
        SET_STRING(1, "");
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
int CopyCertificateInfo(INVOKE_CALL Invoke, void *certificates, TLSCertificate **certificate_chain, int len) {
    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, certificates)))
        return -1;
    INTEGER index = 0;
    for (int i = 0; i < len; i++) {
        TLSCertificate *certificate = certificate_chain[i];
        if (certificate) {
            void *data = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, certificates, index++, &data);
            if (data) {
                CREATE_ARRAY(data);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "algorithm", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)certificate->algorithm);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "key_algorithm", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)certificate->key_algorithm);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "ec_algorithm", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)certificate->ec_algorithm);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "exponent", (INTEGER)VARIABLE_STRING, (char *)certificate->exponent, (NUMBER)certificate->exponent_len); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "pk", (INTEGER)VARIABLE_STRING, (char *)certificate->pk, (NUMBER)certificate->pk_len); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "priv", (INTEGER)VARIABLE_STRING, (char *)certificate->pk, (NUMBER)certificate->priv_len); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "issuer_country", (INTEGER)VARIABLE_STRING, (char *)certificate->issuer_country, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "issuer_state", (INTEGER)VARIABLE_STRING, (char *)certificate->issuer_state, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "issuer_location", (INTEGER)VARIABLE_STRING, (char *)certificate->issuer_location, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "issuer_entity", (INTEGER)VARIABLE_STRING, (char *)certificate->issuer_entity, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "issuer_subject", (INTEGER)VARIABLE_STRING, (char *)certificate->issuer_subject, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "not_before", (INTEGER)VARIABLE_STRING, (char *)certificate->not_before, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "not_after", (INTEGER)VARIABLE_STRING, (char *)certificate->not_after, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "country", (INTEGER)VARIABLE_STRING, (char *)certificate->country, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "state", (INTEGER)VARIABLE_STRING, (char *)certificate->state, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "location", (INTEGER)VARIABLE_STRING, (char *)certificate->location, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "entity", (INTEGER)VARIABLE_STRING, (char *)certificate->entity, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "subject", (INTEGER)VARIABLE_STRING, (char *)certificate->subject, (NUMBER)0); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "serial_number", (INTEGER)VARIABLE_STRING, (char *)certificate->serial_number, (NUMBER)certificate->serial_len); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "sign_key", (INTEGER)VARIABLE_STRING, (char *)certificate->sign_key, (NUMBER)certificate->sign_len); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "der_bytes", (INTEGER)VARIABLE_STRING, (char *)certificate->der_bytes, (NUMBER)certificate->der_len); 
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "bytes", (INTEGER)VARIABLE_STRING, (char *)certificate->bytes, (NUMBER)certificate->len); 
            }
        }
    }
    return len;
}

int certificate_verify(TLSContext *context, TLSCertificate **certificate_chain, int len) {
    if (context->user_data) {
        void *RES       = 0;
        void *EXCEPTION = 0;
        int res = bad_certificate;
        char    *szData;
        INTEGER type;
        NUMBER  nData;

        INVOKE_CALL Invoke = InvokePtr;

        void *certificates = NULL;
        CREATE_VARIABLE(certificates);
        if (certificates) {
            CopyCertificateInfo(Invoke, certificates, certificate_chain, len);
            Invoke(INVOKE_GET_VARIABLE, certificates, &type, &szData, &nData);
            Invoke(INVOKE_CALL_DELEGATE, context->user_data, &RES, &EXCEPTION, (INTEGER)2,
                 (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)context,
                 (INTEGER)type, szData, nData
            );
            if (RES) {
                Invoke(INVOKE_GET_VARIABLE, RES, &type, &szData, &nData);
                if (type == VARIABLE_NUMBER)
                    res = (int)nData;
                Invoke(INVOKE_FREE_VARIABLE, RES);
            }
            if (EXCEPTION)
                Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);
            Invoke(INVOKE_FREE_VARIABLE, certificates);
        }
        return res;
    }
    int i;
    int err;
    if (certificate_chain) {
        for (i = 0; i < len; i++) {
            TLSCertificate *certificate = certificate_chain[i];
            // check validity date
            err = tls_certificate_is_valid(certificate);
            if (err)
                return err;
        }
    }
    // check if chain is valid
    err = tls_certificate_chain_is_valid(certificate_chain, len);
    if (err)
        return err;

    // check certificate chain
    if ((!context->is_server) && (context->sni) && (len > 0)) {
        err = tls_certificate_valid_subject(certificate_chain[0], context->sni);
        if (err)
            return err;
    }

    if ((context->root_certificates) && (context->root_count)) {
        err = tls_certificate_chain_is_valid_root(context, certificate_chain, len);
        if (err)
            return err;
    }
    return no_error;
}

CONCEPT_FUNCTION_IMPL(TLSEConsume, 2)
    T_HANDLE(TLSEConsume, 0)
    T_STRING(TLSEConsume, 1)
    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);

    int res = tls_consume_stream(context, (unsigned char *)PARAM(1), PARAM_LEN(1), certificate_verify);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEstablished, 1)
    T_HANDLE(TLSEstablished, 0)
    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    int res = tls_established(context);
    if ((context->user_data) && (res)) {
        Invoke(INVOKE_FREE_VARIABLE, context->user_data);
        context->user_data = NULL;
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEPending, 1)
    T_HANDLE(TLSEPending, 0)
    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    unsigned int out_buffer_len = 0;
    const unsigned char *out_buffer = tls_get_write_buffer(context, &out_buffer_len);
    if ((out_buffer) && (out_buffer_len)) {
        RETURN_BUFFER((char *)out_buffer, out_buffer_len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEReadBufferSize, 1)
    T_HANDLE(TLSEReadBufferSize, 0)
    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    unsigned int res = context->application_buffer_len;
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSESent, 2)
    T_HANDLE(TLSESent, 0)
    T_NUMBER(TLSESent, 1)

    int sent_size = PARAM_INT(1);
    if (sent_size <= 0) {
        RETURN_NUMBER(-1);
        return 0;
    }
    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    unsigned int out_buffer_len = 0;
    const unsigned char *out_buffer = tls_get_write_buffer(context, &out_buffer_len);

    if ((out_buffer) && (out_buffer_len)) {
        if (sent_size < out_buffer_len) {
            int new_len = out_buffer_len - sent_size;
            memmove(context->tls_buffer, context->tls_buffer + sent_size, new_len);
            context->tls_buffer_len = new_len;
        } else
            tls_buffer_clear(context);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEClearCertificates, 1)
    T_HANDLE(TLSEClear, 0)
    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);

    int res = tls_clear_certificates(context);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSELoadKeys, 3)
    T_HANDLE(TLSELoadKeys, 0)
    T_STRING(TLSELoadKeys, 1)
    T_STRING(TLSELoadKeys, 2)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    int res = tls_load_certificates(context, (unsigned char *)PARAM(1), PARAM_LEN(1));
    if (res < 0) {
        RETURN_NUMBER(res);
        return 0;
    }

    res = tls_load_private_key(context, (unsigned char *)PARAM(2), PARAM_LEN(2));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSELoadRootCA, 2)
    T_HANDLE(TLSELoadRootCA, 0)
    T_STRING(TLSELoadRootCA, 1)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    int res = tls_load_root_certificates(context, (unsigned char *)PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEChipher, 1)
    T_HANDLE(TLSEChipher, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    RETURN_NUMBER(context->cipher);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEChipherName, 1)
    T_HANDLE(TLSEChipherName, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    const char *cipher_name = tls_cipher_name(context);
    RETURN_STRING(cipher_name);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEShutdown, 1)
    T_HANDLE(TLSEShutdown, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    tls_close_notify(context);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEBroken, 1)
    T_HANDLE(TLSEBroken, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);

    RETURN_NUMBER(context->critical_error);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(TLSESetExportable, 1, 2)
    T_HANDLE(TLSESetExportable, 0)
    int exportable = 1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(TLSESetExportable, 1)
        exportable = PARAM_INT(1);
    }
    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    tls_make_exportable(context, exportable);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEExport, 1)
    T_HANDLE(TLSEExport, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    unsigned char buffer[0xFFF];
    int size = tls_export_context(context, buffer, sizeof(buffer), 1);
    if (size > 0) {
        RETURN_BUFFER((char *)buffer, size);
    } else {
        int new_size = sizeof(buffer) - size;
        char *out;
        CORE_NEW(new_size + 1, out);
        if (out) {
            size = tls_export_context(context, (unsigned char *)out, new_size, 1);
            if (size > 0) {
                SetVariable(RESULT, -1, out, size);
            } else {
                CORE_DELETE(out);
                RETURN_STRING("");
            }
        } else {
            RETURN_STRING("");
        }
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEImport, 1)
    T_STRING(TLSEImport, 0)

    TLSContext *context = tls_import_context((unsigned char *)PARAM(0), PARAM_LEN(0));
    RETURN_NUMBER((SYS_INT)context);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEGetSNI, 1)
    T_HANDLE(TLSEGetSNI, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    const char *sni = context->sni;
    if (sni) {
        RETURN_STRING(sni)
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSESetSNI, 2)
    T_HANDLE(TLSESetSNI, 0)
    T_STRING(TLSESetSNI, 1)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    int res = tls_sni_set(context, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSESetVerify, 2)
    T_HANDLE(TLSESetVerify, 0)
    T_DELEGATE(TLSESetVerify, 1)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    if (context->user_data)
        Invoke(INVOKE_FREE_VARIABLE, context->user_data);

    DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(1), context->user_data);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(TLSECertificateInfo, 1, 2)
    T_HANDLE(TLSECertificateInfo, 0)

    int cert_list = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(TLSECertificateInfo, 1)
        cert_list = PARAM_INT(1);
    }
    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    switch (cert_list) {
        case 0:
            CopyCertificateInfo(Invoke, RESULT, context->certificates, context->certificates_count);
            break;
        case 1:
            CopyCertificateInfo(Invoke, RESULT, context->client_certificates, context->client_certificates_count);
            break;
        case 2:
            CopyCertificateInfo(Invoke, RESULT, context->root_certificates, context->root_count);
            break;
        default:
            CREATE_ARRAY(RESULT);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEConnect, 1)
    T_HANDLE(TLSEConnect, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    int res = tls_client_connect(context);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEError, 1)
    T_HANDLE(TLSEError, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    RETURN_NUMBER(context->error_code);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEAddALPN, 2)
    T_HANDLE(TLSEAddALPN, 0)
    T_STRING(TLSEAddALPN, 1)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    int err = tls_add_alpn(context, PARAM(1));
    RETURN_NUMBER(err);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(TLSEALPN, 1)
    T_HANDLE(TLSEALPN, 0)

    TLSContext *context = (TLSContext *)(SYS_INT)PARAM(0);
    const char *alpn = tls_alpn(context);
    if (alpn) {
        RETURN_STRING(alpn);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SRTPInit, 1, 2)
    T_STRING(SRTPInit, 0)
    int tag_bits = 32;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(SRTPInit, 1)
        tag_bits = PARAM_INT(1);
    }
    struct SRTPContext *context = srtp_init(SRTP_AES_CM, SRTP_AUTH_HMAC_SHA1);
    if (context) {
        if (srtp_inline(context, PARAM(0), tag_bits)) {
            srtp_destroy(context);
            context = NULL;
        }
    }
    RETURN_NUMBER((SYS_INT)context);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SRTPEncrypt, 3)
    T_HANDLE(SRTPEncrypt, 0)
    T_STRING(SRTPEncrypt, 1)
    T_STRING(SRTPEncrypt, 2)
    SRTPContext *ctx = (SRTPContext *)(SYS_INT)PARAM(0);
    int out_buffer_len = PARAM_LEN(2) + 32;
    char *out = NULL;
    CORE_NEW(out_buffer_len + 1, out);
    if (out) {
        int res = srtp_encrypt(ctx, (unsigned char *)PARAM(1), PARAM_LEN(1), (unsigned char *)PARAM(2), PARAM_LEN(2), (unsigned char *)out, &out_buffer_len);
        if (res) {
            CORE_DELETE(out);
            RETURN_STRING("");
        } else {
            out[out_buffer_len] = 0;
            SetVariable(RESULT, -1, out, out_buffer_len);
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SRTPDecrypt, 3)
    T_HANDLE(SRTPDecrypt, 0)
    T_STRING(SRTPDecrypt, 1)
    T_STRING(SRTPDecrypt, 2)
    SRTPContext *ctx = (SRTPContext *)(SYS_INT)PARAM(0);
    int out_buffer_len = PARAM_LEN(2) + 1;
    char *out = NULL;
    CORE_NEW(out_buffer_len + 1, out);
    if (out) {
        int res = srtp_decrypt(ctx, (unsigned char *)PARAM(1), PARAM_LEN(1), (unsigned char *)PARAM(2), PARAM_LEN(2), (unsigned char *)out, &out_buffer_len);
        if (res) {
            CORE_DELETE(out);
            RETURN_STRING("");
        } else {
            out[out_buffer_len] = 0;
            SetVariable(RESULT, -1, out, out_buffer_len);
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SRTPDone, 1)
    T_HANDLE(SRTPDone, 0)
    SRTPContext *ctx = (SRTPContext *)(SYS_INT)PARAM(0);
    srtp_destroy(ctx);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
