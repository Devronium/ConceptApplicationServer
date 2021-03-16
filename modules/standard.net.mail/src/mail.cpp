#include "mail.h"
#include <string.h>

#ifdef WITH_GSASL
 #include <gsasl.h>
#endif

#ifdef _WIN32
 #define CLOSE_SOCKET    closesocket
#else
 #define CLOSE_SOCKET    close
#endif

#define CRLF             "\r\n"

#define RECV_BUF_SIZE    8192
AnsiString pop3_error;

#define CHECK_RESPONSE(CT_sock, RESP_ID)        \
    {                                           \
        AnsiString resp = GetResponse(CT_sock); \
        int        id   = GetID(resp);          \
        if (id != RESP_ID) {                    \
            Close(CT_sock);                     \
            LAST_ERROR = resp;                  \
            return id;                          \
        }                                       \
    }

#define CHECK_FINAL_RESPONSE(CT_sock, RESP_ID, PROTO) \
    {                                                 \
        PROTO = (char *)"";                           \
        int hd = 0;                                   \
        do {                                          \
            AnsiString resp = GetResponse(CT_sock);   \
            hd = has_data(CT_sock);                   \
            if (!PROTO.Length())                      \
                PROTO = GetPROTO(resp);               \
            if (!hd) {                                \
                int id = GetID(resp);                 \
                if (id != RESP_ID) {                  \
                    Close(CT_sock);                   \
                    LAST_ERROR = resp;                \
                    return id;                        \
                }                                     \
            }                                         \
        } while (hd);                                 \
    }


AnsiString LAST_ERROR;
int        MAIL_COUNT = 0;

int Connect(char *server, int PORT) {
    int sock;
    struct sockaddr_in sin;
    struct hostent     *hp;

    if ((hp = gethostbyname(server)) == 0) {
        return -1;
    }

    memset(&sin, 0, sizeof(sin));
    //INADDR_ANY;
    sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
        CLOSE_SOCKET(sock);
        return -1;
    }

    return sock;
}

int Send(int sock, const char *data) {
    int        len  = strlen(data);
    AnsiString DATA = data;

    DATA += CRLF;
    return send(sock, DATA.c_str(), DATA.Length(), 0);
}

void dump_socket(int socket) {
    struct timeval timeout;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 100000;
    fd_set socks;
    char   buffer[8192];

    int sel_val = 0;
    do {
        FD_ZERO(&socks);
        FD_SET(socket, &socks);
        sel_val = select(FD_SETSIZE, &socks, 0, 0, &timeout);

        if (sel_val > 0)
            recv(socket, buffer, 8192, 0);
    } while (sel_val > 0);
}

int has_data(int socket) {
    struct timeval timeout;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 100000;
    fd_set socks;
    char   buffer[8192];

    int sel_val = 0;
    FD_ZERO(&socks);
    FD_SET(socket, &socks);
    sel_val = select(FD_SETSIZE, &socks, 0, 0, &timeout);

    return sel_val > 0;
}

AnsiString GetResponse(int socket) {
    char       buffer[RECV_BUF_SIZE + 1];
    AnsiString response;

    int rec       = 0;
    int total_rec = 0;

    do {
        rec = recv(socket, buffer + total_rec, /*RECV_BUF_SIZE-total_rec*/ 1, 0);
        if (rec > 0) {
            total_rec        += rec;
            buffer[total_rec] = 0;
            char *ptr = buffer + (total_rec - 2);
            if ((ptr[0] == '\r') && (ptr[1] == '\n')) {
                ptr[0]   = 0;
                response = buffer;
                break;
            }
        }
    } while (rec > 0);
    return response;
}

AnsiString GetResponse2(int socket) {
    char       buffer[RECV_BUF_SIZE + 1];
    AnsiString response;

    int rec       = 0;
    int total_rec = 0;

    do {
        rec = recv(socket, buffer, RECV_BUF_SIZE, 0);
        if (rec > 0) {
            buffer[rec] = 0;
            response   += buffer;

            total_rec += rec;
            if (total_rec >= 5) {
                char *ptr = (char *)response.c_str() + (total_rec - 5);
                if ((ptr[0] == '\r') && (ptr[1] == '\n') && (ptr[2] == '.') && (ptr[3] == '\r') && (ptr[4] == '\n')) {
                    ptr[0] = 0;
                    return AnsiString(response.c_str());
                }
            }
        }
    } while (rec > 0);
    return response;
}

AnsiString GetResponseBySize(int socket, int size) {
    char       buffer[RECV_BUF_SIZE + 1];
    AnsiString response;

    int rec           = 0;
    int total_rec     = 0;
    int rec_remaining = size;

    do {
        if (rec_remaining > RECV_BUF_SIZE)
            rec_remaining = RECV_BUF_SIZE;

        rec = recv(socket, buffer, rec_remaining, 0);
        if (rec > 0) {
            total_rec  += rec;
            buffer[rec] = 0;
            AnsiString recv;
            recv.LoadBuffer(buffer, rec);

            response += recv;

            if (total_rec >= size)
                break;
            rec_remaining = size - total_rec;
        }
    } while (rec > 0);
    return response;
}

AnsiString GetResponseByByte(int socket) {
    char       buffer[1];
    AnsiString response;

    int  rec  = 0;
    char prec = 0;

    do {
        rec = recv(socket, buffer, 1, 0);
        if (rec > 0) {
            if ((prec == '\r') && (buffer[0] == '\n')) {
                break;
            } else if (buffer[0] != '\r')
                response += buffer[0];
        }
        prec = buffer[0];
    } while (rec > 0);
    return response;
}

int GetID(AnsiString response) {
    int  ID   = 0;
    int  pos  = response.Pos(" ");
    char *ptr = (char *)response.c_str();

    if (pos > 0) {
        ptr[pos - 1] = 0;
        ID           = AnsiString(ptr).ToInt();
    }
    return ID;
}

AnsiString GetPROTO(AnsiString response) {
    AnsiString proto;
    int        pos  = response.Pos("250-AUTH ");
    const char *ptr = response.c_str();

    if (pos > 0) {
        proto = AnsiString(ptr + pos + 8);
    }
    return proto;
}

void Close(int sock) {
    CLOSE_SOCKET(sock);
}

AnsiString send_mail_error() {
    return LAST_ERROR;
}

#ifdef WITH_GSASL
 #include <iostream>
AnsiString Get334(int sock) {
    AnsiString resp = GetResponse(sock);
    char       *p   = resp.c_str();
    AnsiString ret;

    if (resp.Length() > 4) {
        if ((p[0] != '3') || (p[1] != '3') || (p[2] != '4') || (p[3] != ' ')) {
            std::cout << resp.c_str();
            return ret;
        }

        p  += 4;
        ret = p;
    }
    return ret;
}

int send_mail_login(void *ctx, char *from, char *to, char *headers, char *content, char *server, int port, char *username, char *password) {
    int        sock;
    AnsiString auth;

    LAST_ERROR = (char *)"";
    MAIL_COUNT = 0;

    sock = Connect(server, port);
    if (sock == -1)
        return sock;

    CHECK_RESPONSE(sock, 220);

    // se adauga automat \r\n
    Send(sock, AnsiString("EHLO ") + server);
    CHECK_FINAL_RESPONSE(sock, 250, auth);
    //=======================================//
    //std::cout << auth.c_str() << "\n";
    if (auth.c_str()) {
        int        rc;
        const char *suggestion = gsasl_client_suggest_mechanism((Gsasl *)ctx, auth.c_str());
        //std::cout << suggestion << "\n";
        if (suggestion) {
            Gsasl_session *session;
            if ((rc = gsasl_client_start((Gsasl *)ctx, suggestion, &session)) != GSASL_OK) {
                //printf ("Cannot initialize client (%d): %s\n", rc, gsasl_strerror (rc));
                return -1;
            }
            gsasl_property_set(session, GSASL_AUTHID, username);
            gsasl_property_set(session, GSASL_PASSWORD, password);

            AnsiString auth_string = "AUTH ";
            auth_string += (char *)suggestion;
            Send(sock, auth_string);

            std::cout << "Use: " << suggestion << "\n";

            do {
                char *p = NULL;
                do {
                    AnsiString resp = Get334(sock);
                    std::cout << "RES : " << resp.c_str() << "\n";

                    if (resp.Length()) {
                        int rc = gsasl_step64(session, resp.c_str(), &p);

                        if ((rc == GSASL_NEEDS_MORE) || (rc == GSASL_OK)) {
                            std::cout << "DO : " << p << "\n";
                            free(p);
                        }
                    }
                } while (rc == GSASL_NEEDS_MORE);
                if (rc != GSASL_OK) {
                    std::cout << "Er: " << gsasl_strerror(rc) << "\n";
                } else {
                    if (p)
                        Send(sock, p);
                }
            } while (false);

            gsasl_finish(session);
        }
    }
    // auth section !
    //=======================================//
    AnsiString FROM = "MAIL FROM:<";
    FROM += from;
    FROM += ">";

    Send(sock, FROM);
    CHECK_RESPONSE(sock, 250);

    AnsiString TO = "RCPT TO:<";
    TO += to;
    TO += ">";

    Send(sock, TO);
    CHECK_RESPONSE(sock, 250);

    Send(sock, "DATA");
    CHECK_RESPONSE(sock, 354);     // Start mail input

    if ((headers) && (headers[0])) {
        Send(sock, headers);
        //CHECK_RESPONSE(sock, 250);
        //Send(sock, CRLF);
    }

    Send(sock, content);
    // end mail !
    Send(sock, ".");
    CHECK_RESPONSE(sock, 250);

    Send(sock, "QUIT");
    Close(sock);

    MAIL_COUNT = 1;
    return 0;
}
#endif


int send_mail(char *from, char *to, char *headers, char *content, char *server, int port) {
    int sock;

    LAST_ERROR = (char *)"";
    MAIL_COUNT = 0;

    sock = Connect(server, port);
    if (sock == -1)
        return sock;

    CHECK_RESPONSE(sock, 220);

    // se adauga automat \r\n
    Send(sock, AnsiString("HELO ") + server);
    CHECK_RESPONSE(sock, 250);

    AnsiString FROM = "MAIL FROM:<";
    FROM += from;
    FROM += ">";

    Send(sock, FROM);
    CHECK_RESPONSE(sock, 250);

    AnsiString TO = "RCPT TO:<";
    TO += to;
    TO += ">";

    Send(sock, TO);
    CHECK_RESPONSE(sock, 250);

    Send(sock, "DATA");
    CHECK_RESPONSE(sock, 354);     // Start mail input

    if ((headers) && (headers[0])) {
        Send(sock, headers);
        //CHECK_RESPONSE(sock, 250);
        //Send(sock, CRLF);
    }

    Send(sock, content);
    // end mail !
    Send(sock, ".");
    CHECK_RESPONSE(sock, 250);

    Send(sock, "QUIT");
    Close(sock);

    MAIL_COUNT = 1;
    return 0;
}

int send_newsletter(char *from, char **to, char **headers, char **content, int subscribers, char fast, char *server, int port) {
    int sock;

    LAST_ERROR = (char *)"";

    sock = Connect(server, port);
    if (sock == -1)
        return sock;

    MAIL_COUNT = 0;
    if (!fast) {
        CHECK_RESPONSE(sock, 220);
    }

    // se adauga automat \r\n
    Send(sock, AnsiString("HELO ") + server);
    if (!fast) {
        CHECK_RESPONSE(sock, 250);
    }

    for (int i = 0; i < subscribers; i++) {
        AnsiString FROM = "MAIL FROM:<";
        FROM += from;
        FROM += ">";

        Send(sock, FROM);
        if (!fast) {
            CHECK_RESPONSE(sock, 250);
        }

        AnsiString TO = "RCPT TO:<";
        TO += to[i];
        TO += ">";

        Send(sock, TO);
        if (!fast) {
            CHECK_RESPONSE(sock, 250);
        }

        Send(sock, "DATA");
        if (!fast) {
            CHECK_RESPONSE(sock, 354);             // Start mail input
        }

        if ((headers[i]) && (headers[i][0])) {
            Send(sock, headers[i]);
            //Send(sock, CRLF);
        }

        Send(sock, content[i]);

        // end mail !
        Send(sock, ".");
        if (!fast) {
            CHECK_RESPONSE(sock, 250);
        }
        MAIL_COUNT++;
    }
    Send(sock, "QUIT");
    Close(sock);

    return 0;
}

int send_mails_count() {
    return MAIL_COUNT;
}

int RESPONSE_OK(AnsiString *response) {
    int result = 0;

    if (response->Length()) {
        result = (*response)[0] == '+';
    }
    if (!result)
        pop3_error = response->c_str();
    return result;
}

int GET_FIRST_COUNT(AnsiString *response) {
    int pos = response->Pos(" ");

    if (pos > 0) {
        AnsiString partial = response->c_str() + pos;
        int        pos2    = partial.Pos(" ");
        if (pos2 > 0) {
            ((char *)partial.c_str())[pos2] = 0;
            AnsiString value = partial.c_str();
            return value.ToInt();
        } else
            return 0;
    }
    return 0;
}

int pop3_open(char *host, char *username, char *password, int port) {
    int handle = Connect(host, port);

    if (handle != -1) {
        AnsiString response = GetResponse(handle);
        if (!RESPONSE_OK(&response)) {
            Close(handle);
            return -1;
        }

        AnsiString SERVER_DATA = "USER ";
        SERVER_DATA += username;

        // trimitere mesaj ...
        Send(handle, SERVER_DATA.c_str());

        response = GetResponse(handle);
        if (!RESPONSE_OK(&response)) {
            Close(handle);
            return -1;
        }

        SERVER_DATA  = "PASS ";
        SERVER_DATA += password;
        Send(handle, SERVER_DATA.c_str());

        response = GetResponse(handle);
        if (!RESPONSE_OK(&response)) {
            Close(handle);
            return -1;
        }

        return handle;
    }
    return -1;
}

int pop3_open_secured(char *host, char *username, char *password, int port) {
    int handle = Connect(host, port);

    if (handle != -1) {
        AnsiString response = GetResponse(handle);
        if (!RESPONSE_OK(&response)) {
            Close(handle);
            return -1;
        }

        AnsiString SERVER_DATA = "APOP ";
        SERVER_DATA += username;
        SERVER_DATA += " ";
        SERVER_DATA += password;

        // trimitere mesaj ...
        Send(handle, SERVER_DATA.c_str());

        response = GetResponse(handle);
        if (!RESPONSE_OK(&response)) {
            Close(handle);
            return -1;
        }

        return handle;
    }
    return -1;
}

int pop3_count_messages(int handle) {
    if (handle < 0)
        return -1;

    // trimitere mesaj ...
    Send(handle, "STAT");

    AnsiString response = GetResponse(handle);
    if (!RESPONSE_OK(&response)) {
        return -1;
    }

    int messages = GET_FIRST_COUNT(&response);

    return messages;
}

AnsiString pop3_get_message(int handle, int index) {
    AnsiString message;

    if (handle < 0)
        return message;
    AnsiString SERVER_DATA = "RETR ";
    SERVER_DATA += AnsiString(index);

    Send(handle, SERVER_DATA.c_str());

    AnsiString response = GetResponseByByte(handle);
    if (!RESPONSE_OK(&response))
        return message;

    int size = 0;
    int pos  = response.Pos("+OK ");

    if (pos > 0) {
        char *ptr = (char *)response.c_str() + 4;
        AnsiString substr = ptr;
        pos = substr.Pos(" ");

        if (pos > 0) {
            ptr[pos - 1] = 0;
            size         = AnsiString(ptr).ToInt();
        } else
            size = substr.ToInt();
    } else
    if (response == (char *)"+OK") {
        message = GetResponse2(handle); // .\r\n
        dump_socket(handle);
        return message;
    }

    if (size <= 0) {
        // try ...
        if (response.Pos("+OK") < 2) {
            message = GetResponse2(handle);           // .\r\n
        }
        dump_socket(handle);
        return message;
    }

    message = GetResponseBySize(handle, size);
    //GetResponse(handle); // .\r\n
    dump_socket(handle);

    return message;
}

int pop3_delete_message(int handle, int index) {
    if (handle < 0)
        return 0;

    AnsiString SERVER_DATA = "DELE ";
    SERVER_DATA += AnsiString(index);

    Send(handle, SERVER_DATA.c_str());

    AnsiString response = GetResponse(handle);
    if (!RESPONSE_OK(&response)) {
        return 0;
    }
    return 1;
}

int pop3_close(int handle) {
    if (handle < 0)
        return 0;

    Send(handle, "QUIT");
    AnsiString response = GetResponse(handle);
    Close(handle);
    if (!RESPONSE_OK(&response)) {
        return 0;
    }
    return 1;
}

AnsiString pop3_last_error() {
    return pop3_error;
}
