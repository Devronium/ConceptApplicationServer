#include "libconceptclient.h"
#include "ConceptClient.h"
#include "messages.h"


// ============= from libtomcrypt  ================ //
typedef struct {
    int  size;
    char *name;
    char *prime;
    char *B;
    char *order;
    char *Gx;
    char *Gy;
} ltc_ecc_set_type;

typedef struct {
    void *x;
    void *y;
    void *z;
} ecc_point;

typedef struct {
    int                    type;
    int                    idx;
    const ltc_ecc_set_type *dp;
    ecc_point              pubkey;
    void                   *k;
} ecc_key;  

enum {
    PK_PUBLIC =0,
    PK_PRIVATE=1
}; 

static const ltc_ecc_set_type ecc256 = {
    32,
    "ECC-256",
    "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF",
    "5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B",
    "FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551",
    "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296",
    "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5",
};

extern "C" {
    union prng_state;

    int ltc_init_multi(void **a, ...);
    void ltc_deinit_multi(void *a, ...);

    int mp_read_radix(void *a, const char *str, int radix);
    int mp_toradix(void *a, char *str, int radix);

    #define mp_init_multi                        ltc_init_multi
    #define mp_clear_multi                       ltc_deinit_multi

    int ecc_shared_secret(ecc_key *private_key, ecc_key *public_key, unsigned char *out, unsigned long *outlen);
    int ecc_make_key_ex(prng_state *prng, int wprng, ecc_key *key, const ltc_ecc_set_type *dp);
    void ecc_free(ecc_key *key);
    int find_prng(const char *name);

    void init_dependencies();
}
// ============== end libtomcrypt  ================ //

DLL_EXPORT void SetSocketIO(SOCKET_CONNECT_CALLBACK connect_cb, SOCKET_RECV_CALLBACK recv_cb, SOCKET_SEND_CALLBACK send_cb, SOCKET_EOF_CALLBACK eof_cb, SOCK_CLOSE_CALLBACK close_cb) {
    CConceptClient::connect = connect_cb;
    CConceptClient::recv = recv_cb;
    CConceptClient::send = send_cb;
    CConceptClient::eof = eof_cb;
    CConceptClient::close = close_cb;
}

DLL_EXPORT void *CreateClient(CALLBACK_FUNC cb, PROGRESS_API _notify, char *url, int debug, int parent_pipe) {
    if (!url)
        return NULL;

    int secured = 0;
    if (strstr(url, "ws:") == url) {
        secured = 0;
        url += 3;
    } else
    if (strstr(url, "wss:") == url) {
        secured = 1;
        url += 4;
    } else
    if (strstr(url, "concept:") == url) {
        secured = 0;
        url += 8;
    } else
    if (strstr(url, "concepts:") == url) {
        secured = 1;
        url += 9;
    } else
    if (strstr(url, "http:") == url) {
        secured = 0;
        url += 5;
    } else
    if (strstr(url, "https:") == url) {
        secured = 1;
        url += 6;
    }

    while (*url == '/')
        url++;

    char buf[4096];
    char *host = NULL;
    char *app = strchr(url, '/');
    int port = secured ? DEFAULT_TLS_PORT : DEFAULT_PORT;
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
                port = secured ? DEFAULT_TLS_PORT : DEFAULT_PORT;
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
    int err_code = -1;
    if (CC->Connect(host, port)) {
        err_code = CC->Run(app, parent_pipe);
        if (err_code == 302) {
            if (err_code == 302) {
                // redirect
                AnsiString redirect_host = CC->lastRedirect;
                if (redirect_host.Length()) {
                    delete CC;
                    return CreateClient(cb, _notify, redirect_host.c_str(), debug, parent_pipe);
                }
            }
        }
    }
    if (err_code) {
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

DLL_EXPORT int QueueNonImportantMessage(void *client, char *sender, unsigned char sender_len, int MSG_ID, char *target, unsigned short t_len, char *value, unsigned long v_len, int max_messages) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        if (CC->StackLen() < max_messages) {
            CC->QueueMessage(sender, sender_len, MSG_ID, target, t_len, value, v_len);
            return 1;
        }
        return 2;
    }
    return 0;
}

DLL_EXPORT int QueueSimpleMessage(void *client, char *sender, int MSG_ID, char *target, char *value, unsigned long v_len) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        CC->QueueMessage(sender, strlen(sender), MSG_ID, target, strlen(target), value, v_len);
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
        CC->SendPending();
        while ((max_messages) && (!CC->IsEOF())) {
            PARAM.Owner = CC;
            PARAM.UserData = 0;
            PARAM.Flags = 0;
            int res = get_message(CC, &PARAM, (PROGRESS_API)CC->notify);
            if (res == -2)
                return messages;
            if (res <= 0)
                return -1;
            if (CC->NON_OPAQUE_CONCEPT_CALLBACK)
                CC->NON_OPAQUE_CONCEPT_CALLBACK(&PARAM);
            else
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

DLL_EXPORT int RTC2(void *client, char *host_data, char *buf, int buf_size) {
    CConceptClient *CC = (CConceptClient *)client;
    if ((CC) && (buf_size > 0) && (buf)) {
        AnsiString temp = InitUDP2(CC, host_data);
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

DLL_EXPORT void SetCompensateCallbacks(void *client, void *send, void *wait, void *userdata) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        if (CC->parser)
            CC->parser->SetCallbacks((PROTOCOL_MESSAGE_CB)send, (PROTOCOL_MESSAGE_CB)wait, CC);
        else
            CC->parser = new CompensationParser((PROTOCOL_MESSAGE_CB)send, (PROTOCOL_MESSAGE_CB)wait, CC);
    }
}

DLL_EXPORT int CompensateCode(void *client, char *code, int len, char *buf, int buf_size) {
    CConceptClient *CC = (CConceptClient *)client;
    if ((buf) && (buf_size > 0))
        buf[0] = 0;

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
            return size;
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

int __private_eccdh_shared_secret(const ltc_ecc_set_type *dp, const char *pub_key_x, const char *pub_key_y, const char *pub_key_z, const char *priv_key, unsigned char *out, unsigned long *outlen) {
    int err;
 
    ecc_key public_key;
    ecc_key private_key;

    memset(&public_key, 0, sizeof(ecc_key));
    memset(&private_key, 0, sizeof(ecc_key));

    if (mp_init_multi(&public_key.pubkey.x, &public_key.pubkey.y, &public_key.pubkey.z, &private_key.k, NULL))
        return -1;
    
    if ((err = mp_read_radix(public_key.pubkey.x, pub_key_x, 16)) != 0) {
        mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
        return -1;
    }
    if ((err = mp_read_radix(public_key.pubkey.y, pub_key_y, 16)) != 0) {
        mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
        return -1;
    }
    if ((err = mp_read_radix(public_key.pubkey.z, pub_key_z, 16)) != 0) {
        mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
        return -1;
    }
    if ((err = mp_read_radix(private_key.k, priv_key, 16)) != 0) {
        mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
        return -1;
    }

    public_key.idx = -1;
    public_key.dp = dp;
    public_key.type = PK_PUBLIC;

    private_key.idx = -1;
    private_key.dp = dp;
    private_key.type = PK_PRIVATE;

    err = ecc_shared_secret(&private_key, &public_key, out, outlen);
    mp_clear_multi(public_key.pubkey.x, public_key.pubkey.y, public_key.pubkey.z, private_key.k, NULL);
    return err;
}

DLL_EXPORT int ECDHEMakeKey(char *x, char *y, char *z, char *k) {
    ecc_key ecc_dhe;
    init_dependencies();
    int res = ecc_make_key_ex(NULL, find_prng("sprng"), &ecc_dhe, &ecc256);
    if (!res) {
        mp_toradix(ecc_dhe.pubkey.x, x, 16);
        mp_toradix(ecc_dhe.pubkey.y, y, 16);
        mp_toradix(ecc_dhe.pubkey.z, z, 16);
        mp_toradix(ecc_dhe.k, k, 16);
        ecc_free(&ecc_dhe);
    }
    return res;
}

DLL_EXPORT int ECDHECompute(const char *x, const char *y, const char *z, const char *k, unsigned char *out, unsigned long *out_len) {
    return __private_eccdh_shared_secret(&ecc256, x, y, z, k, out, out_len);
}

DLL_EXPORT void SetECDHEKey(void *client, const char *k) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        CC->LinkContainer.ecdhe_k = k;
}

DLL_EXPORT void SetECDHEXYZ(void *client, const char *x, const char *y, const char *z) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        CC->LinkContainer.ecdhe_x = x;
        CC->LinkContainer.ecdhe_y = y;
        CC->LinkContainer.ecdhe_z = z;
    }
}

DLL_EXPORT void GetECDHEParameters(void *client, const char **x, const char **y, const char **z, const char **k) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC) {
        *x = CC->LinkContainer.ecdhe_x.c_str();
        *y = CC->LinkContainer.ecdhe_y.c_str();
        *z = CC->LinkContainer.ecdhe_z.c_str();
        *k = CC->LinkContainer.ecdhe_k.c_str();
    } else {
        *x = NULL;
        *y = NULL;
        *z = NULL;
        *k = NULL;
    }
}

DLL_EXPORT int GetSocketRef(void *client) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        return CC->CLIENT_SOCKET;
    return -1;
}

DLL_EXPORT int GetRTSocketRef(void *client) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        return CC->CLIENT_SOCKET;
    return -1;
}

DLL_EXPORT void SetNonOpaqueCallback(void *client, CLIENT_NON_OPAQUE_CALLBACK_FUNC cb) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        CC->NON_OPAQUE_CONCEPT_CALLBACK = (NON_OPAQUE_CALLBACK_FUNC)cb;
}

DLL_EXPORT const char *GetHostInfo(void *client) {
    CConceptClient *CC = (CConceptClient *)client;
    if (CC)
        return CC->Called_HOST.c_str();

    return NULL;
}
