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
typedef int (*CLIENT_NON_OPAQUE_CALLBACK_FUNC)(void *parameters);
typedef int (*SOCKET_CONNECT_CALLBACK)(int socket, const char *host, int port, int is_reconnect, unsigned char *secured);
typedef int (*SOCKET_RECV_CALLBACK)(int socket, void *buffer, size_t length, int flags);
typedef int (*SOCKET_SEND_CALLBACK)(int socket, const void *buffer, size_t length, int flags);
typedef int (*SOCKET_EOF_CALLBACK)(int socket, int timeout);
typedef int (*SOCK_CLOSE_CALLBACK)(int socket);

#ifdef __cplusplus
extern "C" {
#endif
    DLL_EXPORT void SetSocketIO(SOCKET_CONNECT_CALLBACK connect_cb, SOCKET_RECV_CALLBACK recv_cb, SOCKET_SEND_CALLBACK send_cb, SOCKET_EOF_CALLBACK eof_cb, SOCK_CLOSE_CALLBACK close_cb);
    DLL_EXPORT void *CreateClient(CLIENT_CALLBACK_FUNC cb, CLIENT_PROGRESS_API _notify, char *url, int debug, int parent_pipe);
    DLL_EXPORT void SetUserData(void *client, void *userdata);
    DLL_EXPORT void SetNonOpaqueCallback(void *client, CLIENT_NON_OPAQUE_CALLBACK_FUNC cb);
    DLL_EXPORT void *GetUserData(void *client);
    DLL_EXPORT void DestroyClient(void *client);
    DLL_EXPORT int QueueMessage(void *client, char *sender, unsigned char sender_len, int MSG_ID, char *target, unsigned short t_len, char *value, unsigned long v_len);
    DLL_EXPORT int QueueNonImportantMessage(void *client, char *sender, unsigned char sender_len, int MSG_ID, char *target, unsigned short t_len, char *value, unsigned long v_len, int max_messages);
    DLL_EXPORT int QueueSimpleMessage(void *client, char *sender, int MSG_ID, char *target, char *value, unsigned long v_len);
    DLL_EXPORT int HasMessages(void *client);
    DLL_EXPORT int Iterate(void *client, int max_messages);
    DLL_EXPORT int TryReconnect(void *client);
    DLL_EXPORT int StartTLS(void *client, int flags, SSL_VERIFY_API verify_callback);
    DLL_EXPORT int ResetTLS(void *client);
    DLL_EXPORT int SendAll(void *client);
    DLL_EXPORT const char *GetKey(void *client, int *size);
    DLL_EXPORT int GetSocketRef(void *client);
    DLL_EXPORT int GetRTSocketRef(void *client);
    DLL_EXPORT const char *GetHostInfo(void *client);

    // UDP-based APIs
    DLL_EXPORT void RTC(void *client, int is_on);
    DLL_EXPORT int RTC2(void *client, char *host_data, char *buf, int buf_size);
    DLL_EXPORT int P2P(void *client, char *host_data, char *buf, int buf_size);
    DLL_EXPORT int P2PInfo(void *client, char *host_data, char *buf, int buf_size);
    DLL_EXPORT int UDPFriend(void *client, char *host_data, char *buf, int buf_size);

    DLL_EXPORT void SetCompensateCallbacks(void *client, void *send, void *wait, void *userdata);
    DLL_EXPORT int CompensateCode(void *client, char *code, int len, char *buf, int buf_size);
    DLL_EXPORT int NotifyEvent(void *client, char *entry_point, int Sender, char *EventData, int len);

    DLL_EXPORT int ECDHEMakeKey(char *x, char *y, char *z, char *k);
    DLL_EXPORT int ECDHECompute(const char *x, const char *y, const char *z, const char *k, unsigned char *out, unsigned long *out_len);

    DLL_EXPORT void SetECDHEKey(void *client, const char *k);
    DLL_EXPORT void SetECDHEXYZ(void *client, const char *x, const char *y, const char *z);
    DLL_EXPORT void GetECDHEParameters(void *client, const char **x, const char **y, const char **z, const char **k);
#ifdef __cplusplus
}
#endif
#endif // __LIBRARY_H
