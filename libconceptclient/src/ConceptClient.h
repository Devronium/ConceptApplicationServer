#ifndef __CONCEPT_CLIENT_H
#define __CONCEPT_CLIENT_H

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

#include "AnsiString.h"
#include "AnsiList.h"
#include "Parameters.h"
#include "CompensationParser.h"

#ifdef MULTITHREADED
    #include "semhh.h"
    #define SEMAPHORE_TYPE         HHSEM
    #define SEMAPHORE_INIT(x)      seminit(x, 1)
    #define SEMAPHORE_DONE(x)      semdel(x)
    #define SEMAPHORE_LOCK(x)      semp(x)
    #define SEMAPHORE_UNLOCK(x)    semv(x)
#else
#ifdef SEMAPHORE_INCLUDE
    #include SEMAPHORE_INCLUDE
#endif
#ifndef SEMAPHORE_INIT
    #define SEMAPHORE_INIT(x)
    #define SEMAPHORE_DONE(x)
    #define SEMAPHORE_LOCK(x)
    #define SEMAPHORE_UNLOCK(x)
#endif
#endif

extern "C" {
    #include "tlse.h"
}

typedef void (*PROGRESS_API)(void *client, int how_much, int message);
typedef int (*VERIFY_API)(void *clinet, int code, void *ssl, int flags);
typedef int (*CALLBACK_FUNC)(void *client, char *Sender, unsigned short Sender_len, int MSG_ID, char *Target, unsigned short Target_len, char *Value, unsigned long Value_len);
typedef int (*NON_OPAQUE_CALLBACK_FUNC)(TParameters *PARAM);
typedef int (*CONNECT_CALLBACK)(int socket, const char *host, int port, int is_reconnect, unsigned char *secured);
typedef int (*RECV_CALLBACK)(int socket, void *buffer, size_t length, int flags);
typedef int (*SEND_CALLBACK)(int socket, const void *buffer, size_t length, int flags);
typedef int (*SOCK_EOF_CALLBACK)(int socket, int timeout);
typedef int (*CLOSE_CALLBACK)(int socket);

#define DEFAULT_PORT              80
#define DEFAULT_TLS_PORT          443

#define INTEGER                   int

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
    char En_inited;
    char Dec_inited;

    AnsiString ecdhe_x;
    AnsiString ecdhe_y;
    AnsiString ecdhe_z;
    AnsiString ecdhe_k;

    MessageDataContainer() {
        PostFile = NULL;
        server_len = 0;
        p2paddr_len = 0;
        lastaddr_len = 0;
        is_p2p = 0;
        En_inited = 0;
        Dec_inited = 0;
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
    struct TLSContext *tls;
    AnsiString SessionID;
    int        debug;

#ifdef MULTITHREADED
    SEMAPHORE_TYPE sem;
#endif
    AnsiList      MessageStack;

    bool         connection_err;

    int        PORT;

    struct sockaddr last_addr;
    socklen_t       last_length;

    static int sock_eof2(int stream, int sec);
    static int connect2(int socket, struct sockaddr *addr, socklen_t length);
    int ParseCode(const char *buf);
public:
    CALLBACK_FUNC CONCEPT_CALLBACK;
    NON_OPAQUE_CALLBACK_FUNC NON_OPAQUE_CONCEPT_CALLBACK;
    SOCKET CLIENT_SOCKET;
    SOCKET RTSOCKET;

    AnsiString Called_HOST;
    AnsiString lastHOST;
    AnsiString lastApp;
    AnsiString lastRedirect;

    CompensationParser *parser;
    void *UserData;
    PROGRESS_API notify;

    unsigned char  secured;
    unsigned char  ManageTLS;

    MessageDataContainer LinkContainer;

    static int timedout_recv(SOCKET _socket, char *buffer, int size, int flags, int timeout = 5);

    int         Connect(const char *HOST, int PORT = DEFAULT_PORT);
    int         CheckReconnect();

    int         Disconnect();
    int         Run(const char *app_name, int pipe = -1, char *previous_session = NULL);

    void        QueueMessage(char *sender, unsigned char sender_len, int MSG, char *target, unsigned short target_len, char *value, unsigned long value_len);
    long        SendFileMessage(char *sender, int MSG, char *target, char *filename, char force = 0);
    int         SendPending();

    int         Dump(char *);

    int         Send(char *buf, int len, bool real_time = false);
    int         BlockingSend(char *buf, int len, bool real_time = false);
    int         Recv(char *buf, int len);
    int         RecvTimeout(char *buffer, int size, int timeout = 5);
    int         IsEOF();
    int         StackLen();

    CConceptClient(CALLBACK_FUNC cb, int secured, int debug, PROGRESS_API _notify = 0);
    virtual ~CConceptClient();

    static CONNECT_CALLBACK connect;
    static RECV_CALLBACK recv;
    static SEND_CALLBACK send;
    static SOCK_EOF_CALLBACK eof;
    static CLOSE_CALLBACK close;
};
#endif // __CONCEPT_CLIENT_H
