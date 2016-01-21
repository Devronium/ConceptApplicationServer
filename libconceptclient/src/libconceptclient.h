#ifndef __LIBRARY_H
#define __LIBRARY_H

#ifdef _WIN32
 #define DLL_EXPORT      __declspec(dllexport)
#else
 #define DLL_EXPORT
#endif 

#include <stdlib.h>
#ifdef _WIN32
 #ifdef _WIN32_WINNT
  #undef _WIN32_WINNT
 #endif
 #define _WIN32_WINNT    0x0501
 #include <windows.h>
 #include <winsock2.h>
 #include <ws2tcpip.h>
#else
 #include <sys/socket.h>
 #define SOCKET            int
#endif

typedef int (*SSL_VERIFY_API)(void *client, int code, void *ssl, int flags);
typedef int (*CLIENT_CALLBACK_FUNC)(void *client, char *Sender, unsigned short Sender_len, int MSG_ID, char *Target, unsigned short Target_len, char *Value, unsigned long Value_len);
typedef void (*CLIENT_PROGRESS_API)(void *client, int how_much, int message);

typedef int (*SOCKET_CONNECT_CALLBACK)(SOCKET socket, const struct sockaddr *address, socklen_t address_len);
typedef int (*SOCKET_RECV_CALLBACK)(SOCKET socket, void *buffer, size_t length, int flags);
typedef int (*SOCKET_SEND_CALLBACK)(SOCKET socket, const void *buffer, size_t length, int flags);
typedef int (*SOCKET_EOF_CALLBACK)(SOCKET socket, int timeout);
typedef int (*SOCK_CLOSE_CALLBACK)(SOCKET socket);

#ifdef __cplusplus
extern "C" {
#endif
    DLL_EXPORT void SetSocketIO(SOCKET_CONNECT_CALLBACK connect_cb, SOCKET_RECV_CALLBACK recv_cb, SOCKET_SEND_CALLBACK send_cb, SOCKET_EOF_CALLBACK eof_cb, SOCK_CLOSE_CALLBACK close_cb);
    DLL_EXPORT void *CreateClient(CLIENT_CALLBACK_FUNC cb, CLIENT_PROGRESS_API _notify, char *url, int debug, int parent_pipe);
    DLL_EXPORT void SetUserData(void *client, void *userdata);
    DLL_EXPORT void *GetUserData(void *client);
    DLL_EXPORT void DestroyClient(void *client);
    DLL_EXPORT int QueueMessage(void *client, char *sender, unsigned char sender_len, int MSG_ID, char *target, unsigned short t_len, char *value, unsigned long v_len);
    DLL_EXPORT int HasMessages(void *client);
    DLL_EXPORT int Iterate(void *client, int max_messages);
    DLL_EXPORT int TryReconnect(void *client);
    DLL_EXPORT int StartTLS(void *client, int flags, SSL_VERIFY_API verify_callback);
    DLL_EXPORT int ResetTLS(void *client);
    DLL_EXPORT int SendAll(void *client);
    DLL_EXPORT const char *GetKey(void *client, int *size);

    // UDP-based APIs
    DLL_EXPORT void RTC(void *client, int is_on);
    DLL_EXPORT int P2P(void *client, char *host_data, char *buf, int buf_size);
    DLL_EXPORT int P2PInfo(void *client, char *host_data, char *buf, int buf_size);
    DLL_EXPORT int UDPFriend(void *client, char *host_data, char *buf, int buf_size);

    DLL_EXPORT int CompensateCode(void *client, char *code, int len);
    DLL_EXPORT int NotifyEvent(void *client, char *entry_point, int Sender, char *EventData, int len);
#ifdef __cplusplus
}
#endif
#endif // __LIBRARY_H
