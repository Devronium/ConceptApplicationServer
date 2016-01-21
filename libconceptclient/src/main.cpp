#include "libconceptclient.h"
#include "ConceptClient.h"
#include "messages.h"

bool ValidateApp(AnsiString app_name, X509_NAME *name) {
    char asnbuf[1024];
    int  idx = 0;

    if ((name) && (name->entries)) {
        STACK_OF(X509_NAME_ENTRY) * e_stack = name->entries;
        int len = sk_X509_NAME_ENTRY_num(e_stack);
        for (int i = 0; i < len; i++) {
            X509_NAME_ENTRY *entry = sk_X509_NAME_ENTRY_value(e_stack, i);
            if ((entry) && (entry->object) && (entry->value)) {
                if (OBJ_obj2txt(asnbuf, sizeof(asnbuf), entry->object, 0)) {
                    AnsiString temp;
                    temp.LoadBuffer((char *)entry->value->data, entry->value->length);
                    if (temp == app_name)
                        return true;
                }
            }
        }
    }
    return false;
} 

int cert_verify(void *ref_Client, int code, void *ssl, int flags) {
    CConceptClient *lpClient = (CConceptClient *)ref_Client;
    char hashbuf[0xFF];
    X509 *cert    = SSL_get_peer_certificate((SSL *)ssl);
    bool mismatch = true;

    if (cert) {
        if (ValidateApp(lpClient->lastApp, X509_get_subject_name(cert)))
            mismatch = false;
        else
        if (ValidateApp(lpClient->lastHOST, X509_get_subject_name(cert)))
            mismatch = false;
        else {
            AnsiString fullpath = lpClient->lastHOST;
            fullpath += "/";
            fullpath += lpClient->lastApp;

            if (ValidateApp(fullpath, X509_get_subject_name(cert)))
                mismatch = false;
        }
    }
    if (mismatch)
        return 0;

    switch (code) {
        case X509_V_OK:
            if (cert)
                X509_free(cert);
            return 1;

        case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
        case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
            if (cert)
                X509_free(cert);
            return (flags == 1);

        case X509_V_ERR_CERT_HAS_EXPIRED:
        case X509_V_ERR_CRL_HAS_EXPIRED:
        case X509_V_ERR_CERT_NOT_YET_VALID:
        case X509_V_ERR_CRL_NOT_YET_VALID:
            if (cert)
                X509_free(cert);
            return (flags == 1);

        default:
            if (cert)
                X509_free(cert);
            return (flags == 1);
    }
    if (cert)
        X509_free(cert);

    if (flags == 2) {
        return 0;
    } else
        return 1;
    return 0;
} 

DLL_EXPORT void SetSocketIO(SOCKET_CONNECT_CALLBACK connect_cb, SOCKET_RECV_CALLBACK recv_cb, SOCKET_SEND_CALLBACK send_cb, SOCKET_EOF_CALLBACK eof_cb, SOCK_CLOSE_CALLBACK close_cb) {
    CConceptClient::connect3 = connect_cb;
    CConceptClient::recv3 = recv_cb;
    CConceptClient::send3 = send_cb;
    CConceptClient::eof3 = eof_cb;
    CConceptClient::close3 = close_cb;
}

DLL_EXPORT void *CreateClient(CALLBACK_FUNC cb, PROGRESS_API _notify, char *url, int debug, int parent_pipe) {
    if (!url)
        return NULL;

    int secured = 0;
    if (strstr(url, "concept:") == url) {
        secured = 0;
        url += 8;
    }
    if (strstr(url, "concepts:") == url) {
        secured = 1;
        url += 9;
    }
    while (*url == '/')
        url++;

    char buf[4096];
    char* host;
    char *app = strchr(url, '/');
    int port = DEFAULT_PORT;
    if (!app) {
        app = "start.con";
        host = url;
    } else {
        app++;
        if (!app[0])
            app = "start.con";

        int len = app - url - 1;
        if ((len > 0) && (len < 4095)) {
            memcpy(buf, url, len);
            host = buf;
            buf[len] = 0;
        } else
            return NULL;
    }
    char *port_str = NULL;
    for (int i = strlen(host) - 1; i >= 0; i--) {
        char c = host[i];
        if (c == ':') {
            port = atoi(host + i + 1);
            if (port <= 0)
                port = DEFAULT_PORT;
            if (host == buf)
                host[i] = 0;
            else {
                if (i < 4095) {
                    memcpy(buf, host, i - 1);
                    buf[i - 1] = 0;
                    host = buf;
                }
            }
            break;
        }
    }

    CConceptClient *CC = new CConceptClient(cb, secured, debug, _notify);
    if (CC->Connect(host, port)) {
        CC->Run(app, parent_pipe);
    } else {
        delete CC;
        CC = NULL;
    }
    return CC;
}

DLL_EXPORT void SetUserData(void *client, void *userdata) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        CC->UserData = userdata;
}

DLL_EXPORT void *GetUserData(void *client) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        return CC->UserData;
    return NULL;
}

DLL_EXPORT void DestroyClient(void *client) {
    if (client)
        delete (CConceptClient *)client;
}

DLL_EXPORT int QueueMessage(void *client, char *sender, unsigned char sender_len, int MSG_ID, char *target, unsigned short t_len, char *value, unsigned long v_len) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        CC->QueueMessage(sender, sender_len, MSG_ID, target, t_len, value, v_len);
        return 1;
    }
    return 0;
}

DLL_EXPORT int SendAll(void *client) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        return CC->SendPending();

    return -1;
}

DLL_EXPORT int HasMessages(void *client) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        return !CC->IsEOF();
    }
    return -1;
}

DLL_EXPORT int Iterate(void *client, int max_messages) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        int messages = 0;
        TParameters PARAM;
        char *localCERT = CC->secured ? CC->LOCAL_PRIVATE_KEY.c_str() : 0;
        CC->SendPending();
        while ((max_messages) && (!CC->IsEOF())) {
            PARAM.Owner = CC;
            PARAM.UserData = 0;
            PARAM.Flags = 0;
            int res = get_message(CC, &PARAM, CC->CLIENT_SOCKET, localCERT, (PROGRESS_API)CC->notify, false);
            if (res == -2)
                return messages;
            if (res <= 0)
                return -1;
            if (CC->CONCEPT_CALLBACK)
                CC->CONCEPT_CALLBACK(client, PARAM.Sender.c_str(), PARAM.Sender.Length(), PARAM.ID, PARAM.Target.c_str(), PARAM.Target.Length(), PARAM.Value.c_str(), PARAM.Value.Length());
            CC->SendPending();
            messages++;
            max_messages--;
        }
        return messages;
    }
    return -1;
}

DLL_EXPORT int TryReconnect(void *client) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        return CC->CheckReconnect();

    return -1;
}

DLL_EXPORT int StartTLS(void *client, int verify_level, SSL_VERIFY_API verify_callback) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        if (verify_callback)
            return CC->InitTLS(verify_callback, verify_level);
        return CC->InitTLS(cert_verify, verify_level);
    }

    return -1;
}

DLL_EXPORT int ResetTLS(void *client) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        CC->ResetTLS();
        return 1;
    }
    return 0;
}

DLL_EXPORT const char *GetKey(void *client, int *size) {
    size = 0;
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        if (size)
            *size = CC->LOCAL_PRIVATE_KEY.Length();
        return CC->LOCAL_PRIVATE_KEY.c_str();
    }
    return NULL;
}

DLL_EXPORT void RTC(void *client, int is_on) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        InitUDP(CC, is_on);
}

DLL_EXPORT int P2P(void *client, char *host_data, char *buf, int buf_size) {
    CConceptClient *CC = (CConceptClient *)client;
    if ((CC) && (buf_size > 0) && (buf)) {
        AnsiString temp = SwitchP2P(CC, host_data);
        int size = temp.Length();
        if (size >= buf_size)
            size = buf_size - 1;
        memcpy(buf, temp.c_str(), size);
        buf[size] = 0;
        return size;
    }
    return 0;
}

DLL_EXPORT int P2PInfo(void *client, char *host_data, char *buf, int buf_size) {
    CConceptClient *CC = (CConceptClient *)client;
    if ((CC) && (buf_size > 0) && (buf)) {
        AnsiString temp = P2PInfo(CC, host_data);
        int size = temp.Length();
        if (size >= buf_size)
            size = buf_size - 1;
        memcpy(buf, temp.c_str(), size);
        buf[size] = 0;
        return size;
    }
    return 0;
}

DLL_EXPORT int UDPFriend(void *client, char *host_data, char *buf, int buf_size) {
    CConceptClient *CC = (CConceptClient *)client;
    if ((CC) && (buf_size > 0) && (buf)) {
        AnsiString temp = InitUDP3(CC, host_data);
        int size = temp.Length();
        if (size >= buf_size)
            size = buf_size - 1;
        memcpy(buf, temp.c_str(), size);
        buf[size] = 0;
        return size;
    }
    return 0;
}

DLL_EXPORT int CompensateCode(void *client, char *code, int len, char *buf, int buf_size) {
    CConceptClient *CC = (CConceptClient *)client;
    if ((CC) && (len > 0)) {
        if (!CC->parser)
            CC->parser = new CompensationParser(NULL, NULL, CC);
        AnsiString res = CC->parser->Parse(code, len);
        if ((buf) && (buf_size > 0)) {
            int size = res.Length();
            if (size >= buf_size)
                size = buf_size - 1;
            memcpy(buf, res.c_str(), size);
            buf[size] = 0;
        }
    }
    return 0;
}

DLL_EXPORT int NotifyEvent(void *client, char *entry_point, int Sender, char *EventData, int len) {
    CConceptClient *CC = (CConceptClient *)client;
    if ((CC) && (CC->parser)) {
        AnsiString temp;
        if (len > 0)
            temp.LoadBuffer(EventData, len);
        CC->parser->NotifyEvent(entry_point, Sender, temp);
        return 1;
    }
    return 0;
}
