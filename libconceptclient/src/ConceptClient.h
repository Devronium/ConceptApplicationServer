#ifndef __CONCEPT_CLIENT_H
#define __CONCEPT_CLIENT_H

#ifdef __WITH_TOMCRYPT
 #include "tomcrypt/tomcrypt.h"
#endif
#ifdef _WIN32
 #ifdef _WIN32_WINNT
  #undef _WIN32_WINNT
 #endif
 #define _WIN32_WINNT    0x0501
 #include <windows.h>
 #include <winsock2.h>
 #include <ws2tcpip.h>
 #define THREAD_TYPE2      DWORD WINAPI
 #define HANDLE2           HANDLE
#else
 #define SOCKET            int
 #define INVALID_SOCKET    -1
 #define SOCKET_ERROR      -1
 #define DWORD             long
 #define LPVOID            void *
 #define WINAPI
 #define THREAD_TYPE2      LPVOID
 #define closesocket       close

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/wait.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <unistd.h>
 
 #include <sys/select.h>
 #include <netinet/tcp.h>
 #include <arpa/inet.h>

 #define HANDLE2    void *
 #define HMODULE    HANDLE

 #define Sleep(x)    usleep(x * 1000)
#endif

#ifndef NOSSL
extern "C" {
 #include <openssl/bio.h>
 #include <openssl/ssl.h>
 #include <openssl/err.h>
 #include <openssl/x509.h>
}
#endif

#include "AnsiString.h"
#include "AnsiList.h"
#include "CompensationParser.h"
#include "semhh.h"

typedef void (*PROGRESS_API)(void *client, int how_much, int message);
typedef int (*VERIFY_API)(void *clinet, int code, void *ssl, int flags);
typedef int (*CALLBACK_FUNC)(void *client, char *Sender, unsigned short Sender_len, int MSG_ID, char *Target, unsigned short Target_len, char *Value, unsigned long Value_len);

typedef int (*CONNECT_CALLBACK)(SOCKET socket, const struct sockaddr *address, socklen_t address_len);
typedef int (*RECV_CALLBACK)(SOCKET socket, void *buffer, size_t length, int flags);
typedef int (*SEND_CALLBACK)(SOCKET socket, const void *buffer, size_t length, int flags);
typedef int (*SOCK_EOF_CALLBACK)(SOCKET socket, int timeout);
typedef int (*CLOSE_CALLBACK)(SOCKET socket);

#define DEFAULT_PORT              2662

#define INTEGER                   int

#define RANDOM_KEY_SIZE       16

class CConceptClient;

typedef struct Parameters {
    AnsiString     Sender;
    int            ID;
    AnsiString     Target;
    AnsiString     Value;
    CConceptClient *Owner;
    signed char     Flags;
    void           *UserData;
} TParameters;

class MessageDataContainer {
public:
    AnsiList BufferedMessages;
    FILE *PostFile;
    struct sockaddr_storage serveraddr;
    socklen_t server_len;
    struct sockaddr_storage p2paddr;
    socklen_t p2paddr_len;
    struct sockaddr_storage lastaddr;
    socklen_t lastaddr_len;
    int       is_p2p;

    MessageDataContainer() {
        PostFile = NULL;
        server_len = 0;
        p2paddr_len = 0;
        lastaddr_len = 0;
        is_p2p = 0;
    }

    ~MessageDataContainer() {
        if (PostFile) {
            fclose(PostFile);
            PostFile = NULL;
        }
    }
};

class CConceptClient {
private:
    AnsiString REMOTE_PUBLIC_KEY;
    AnsiString SessionID;
    int        debug;

    int NON_CRITICAL_SECTION;

    HHSEM         sem;
    AnsiList      MessageStack;

    bool         connection_err;

    int        PORT;
#ifndef NOSSL
    bool    in_ssl_neg;
    SSL_CTX *sslctx;
    SSL     *ssl;
#endif

    struct sockaddr last_addr;
    socklen_t       last_length;

    static int sock_eof2(int stream, int sec);
    static int connect2(int socket, struct sockaddr *addr, socklen_t length);

public:
    CALLBACK_FUNC CONCEPT_CALLBACK;
    SOCKET CLIENT_SOCKET;
    SOCKET RTSOCKET;

    AnsiString Called_HOST;
    AnsiString lastHOST;
    AnsiString lastApp;
    CompensationParser *parser;
    void *UserData;
    PROGRESS_API notify;

    AnsiString LOCAL_PUBLIC_KEY;
    AnsiString LOCAL_PRIVATE_KEY;
    int        secured;
    MessageDataContainer LinkContainer;

    static int timedout_recv(SOCKET _socket, char *buffer, int size, int flags, int timeout = 5);

    void        SetCritical(int status);

    int         Connect(char *HOST, int PORT = DEFAULT_PORT);
    int         CheckReconnect();

    int         Disconnect();
    int         Run(char *app_name, int pipe = -1);

    void        QueueMessage(char *sender, unsigned char sender_len, int MSG, char *target, unsigned short target_len, char *value, unsigned long value_len);
    long        SendFileMessage(char *sender, int MSG, char *target, char *filename, char force = 0);
    int         SendPending();
    AnsiString  Encrypt(char *buf, int size, AnsiString *key);

    void        GenerateRandomAESKey(AnsiString *, int);

    int         NoErrors();
    int         Dump(char *);

    int         InitTLS(VERIFY_API verify, int flags = 2);
    void        ResetTLS();
    int         Send(char *buf, int len);
    int         Recv(char *buf, int len);
    int         RecvTimeout(char *buffer, int size, int timeout = 5);
    int         IsEOF();

    CConceptClient(CALLBACK_FUNC cb, int secured, int debug, PROGRESS_API _notify = 0);
    virtual ~CConceptClient();

    static CONNECT_CALLBACK connect3;
    static RECV_CALLBACK recv3;
    static SEND_CALLBACK send3;
    static SOCK_EOF_CALLBACK eof3;
    static CLOSE_CALLBACK close3;

    static int mod_send(SOCKET socket, const char *buffer, size_t length, int flags);
    static int mod_eof(SOCKET socket);
};
#endif // __CONCEPT_CLIENT_H
