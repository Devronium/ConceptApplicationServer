#ifndef __MAIL_H
#define __MAIL_H

//#define  WITH_GSASL

/*#ifdef WITH_GSASL
   #include <gsasl.h>
 #endif*/

#ifdef _WIN32
 #include <windows.h>
 #include <winsock.h>
#else
 #define INVALID_SOCKET    -1
 #define SOCKET_ERROR      -1

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/wait.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <unistd.h>
#endif

#include "AnsiString.h"

#ifdef WITH_GSASL
int send_mail_login(void *ctx, char *from, char *to, char *headers, char *content, char *server, int port, char *username, char *password);
#endif

int send_mail(char *from, char *to, char *headers, char *content, char *server, int port);
AnsiString send_mail_error();
int send_newsletter(char *from, char **to, char **headers, char **content, int subscribers, char fast, char *server, int port);
int send_mails_count();

int pop3_open(char *host, char *username, char *password, int port = 110);
int pop3_open_secured(char *host, char *username, char *password, int port = 110);

int pop3_count_messages(int handle);
AnsiString pop3_get_message(int handle, int index);
int pop3_delete_message(int handle, int index);
int pop3_close(int handle);
AnsiString pop3_last_error();
#endif // __MAIL_H
