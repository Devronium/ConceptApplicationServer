#include "messages.h"
#include "BasicMessages.h"
#include "ConceptClient.h"
#include <time.h>
#include <errno.h>

#define CHECK_RECONNECT(val)
#define PACKED_ATTR
#define LESS_CHECKS_FOR_SPEED
#define MAX_RT_SIZE     0x9FFF

static signed char is_little_endian = 1;

#ifdef _WIN32
 #include <winsock.h>

const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt) {
    if (af == AF_INET) {
        struct sockaddr_in in;
        memset(&in, 0, sizeof(in));
        in.sin_family = AF_INET;
        memcpy(&in.sin_addr, src, sizeof(struct in_addr));
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    } else if (af == AF_INET6) {
        struct sockaddr_in6 in;
        memset(&in, 0, sizeof(in));
        in.sin6_family = AF_INET6;
        memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    }
    return NULL;
}

#else
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
#endif

uint64_t htonll2(uint64_t a) {
    if (is_little_endian)
        a = ((a & 0x00000000000000FFULL) << 56) |
            ((a & 0x000000000000FF00ULL) << 40) |
            ((a & 0x0000000000FF0000ULL) << 24) |
            ((a & 0x00000000FF000000ULL) << 8) |
            ((a & 0x000000FF00000000ULL) >> 8) |
            ((a & 0x0000FF0000000000ULL) >> 24) |
            ((a & 0x00FF000000000000ULL) >> 40) |
            ((a & 0xFF00000000000000ULL) >> 56);
    return a;
}

void WSFrame(char *data_in, int data_len, char *out_frame, int *out_len, int rtc = 0) {
    uint64_t tmp      = data_len;
    int      size_len = 0;

    // rtc
    if (rtc)
        out_frame[0] = '\x8D';
    else
        out_frame[0] = '\x82';
    out_frame++;
    if (tmp <= 0x7D) {
        *out_frame = tmp;
        size_len++;
        out_frame++;
    } else
    if (tmp < 0xFFFF) {
        *out_frame = 0x7E;
        out_frame++;
        *(unsigned short *)out_frame = htons((unsigned short)tmp);
        size_len  += 3;
        out_frame += 2;
    } else {
        *out_frame = 0x7F;
        out_frame++;
        *(uint64_t *)out_frame = htonll2(tmp);
        size_len  += 9;
        out_frame += 8;
    }
    if (out_frame != data_in)
        memmove(out_frame, data_in, data_len);
    *out_len = data_len + size_len + 1;
}

void IgnoreBytes(CConceptClient *OWNER, int size, bool masked = true) {
    if (size < 0)
        return;
    unsigned char buf;
    int           received  = 0;
    int           rec_count = 0;
    if (masked)
        size += 4;
    do {
        received   = OWNER->Recv((char *)&buf, 1);
        rec_count += received;
    } while ((rec_count < size) && (received > 0));
}

long WSGetPacketSize(CConceptClient *OWNER, int *code, int *masked) {
    int           received  = 0;
    int           rec_count = 0;
    unsigned char buf[9];

    do {
        received   = OWNER->Recv((char *)buf + rec_count, 2 - rec_count);
        rec_count += received;
    } while ((rec_count < 2) && (received > 0));
    if (received <= 0) {
        // connection drop
        *code = -2;
        return -1;
    }
    rec_count = 0;
    // buf[0] is packet marker
    switch (buf[0] & 0x0F) {
        case 0x0:
            // continuation frame
            *code = 0;
            break;

        case 0x1:
            // text frame;
            *code = 0;
            break;

        case 0x02:
            // binary frame
            *code = 0;
            break;

        case 0x08:
            // connection close
            *code = -1;
            break;

        case 0x09:
            // ping
            *code = 1;
            break;

        case 0x0A:
            // pong
            *code = 2;
            break;

        default:
            // unknown message type, drop it
            *code = 2;
            break;
    }

    char s_buf = buf[1] & 0x7F;
    *masked = buf[1] & 0x80;
    switch (s_buf) {
        case 126:
            do {
                received   = OWNER->Recv((char *)buf + rec_count, 2 - rec_count);
                rec_count += received;
            } while ((rec_count < 2) && (received > 0));
            if (received <= 0)
                return -1;

            // ping or pong
            if (*code > 0) {
                IgnoreBytes(OWNER, ntohs(*(unsigned short *)buf));
                if (*code == 1) {
                    // ping, must send pong
                    char pong[6];
                    pong[0] = 0x8A;
                    pong[1] = 0x04;
                    pong[2] = 'P';
                    pong[3] = 'o';
                    pong[4] = 'n';
                    pong[5] = 'g';
                    OWNER->BlockingSend(pong, 6);
                    return 0;
                }
                return -1;
            }
            return ntohs(*(unsigned short *)buf);
            break;

        case 127:
            do {
                received   = OWNER->Recv((char *)buf + rec_count, 8 - rec_count);
                rec_count += received;
            } while ((rec_count < 8) && (received > 0));
            if (received <= 0)
                return -1;

            // ping or pong
            if (*code > 0) {
                IgnoreBytes(OWNER, (long)htonll2(*(uint64_t *)buf));
                return -1;
            }
            return (long)htonll2(*(uint64_t *)buf);
            break;

        default:
            if (s_buf <= 0x7D) {
                if (*code > 0) {
                    exit(0);
                    IgnoreBytes(OWNER, s_buf);
                    return -1;
                }
                return s_buf;
            }
    }
    return -1;
}

long WSReceive(CConceptClient *OWNER, char *buffer, int size, int masked = 0) {
    char mask[4];
    int  received  = 0;
    int  rec_count = 0;

    if (masked) {
        do {
            received   = OWNER->Recv((char *)&mask + rec_count, 4 - rec_count);
            rec_count += received;
        } while ((rec_count < 4) && (received > 0));

        if (received <= 0)
            return -1;
        if (size == 0)
            return 0;
        rec_count = 0;
    }
    do {
        received   = OWNER->Recv((char *)buffer + rec_count, size - rec_count);
        rec_count += received;
    } while ((rec_count < size) && (received > 0));

    if (received <= 0)
        return -1;

    if (masked) {
        for (int i = 0; i < size; i++)
            buffer[i] = buffer[i] ^ mask[i % 4];
    }
    return size;
} 
//-----------------------------------------------------------------------------------
int sock_eof_timeout(int stream, int timeoutms) {
    if ((stream == INVALID_SOCKET) || (stream < 0))
        return -1;

    int sec = timeoutms / 1000;
    int ms  = timeoutms % 1000;

    struct timeval timeout;

    timeout.tv_sec  = sec;
    timeout.tv_usec = ms * 1000;

    fd_set s;
    FD_ZERO(&s);
    FD_SET(stream, &s);

    int val = select(stream + 1, &s, 0, 0, &timeout);

    if (val < 0)
        return -1;
    if (val) {
        if (FD_ISSET(stream, &s))
            return 0;
    }
    return 1;
}

//---------------------------------------------------------------------------
int check_clientaddr(sockaddr_storage *cliaddr, sockaddr_storage *server_addr) {
    if (cliaddr->ss_family != server_addr->ss_family) {
        return 0;
    }

    if (cliaddr->ss_family == AF_INET) {
        struct sockaddr_in *s  = (struct sockaddr_in *)server_addr;
        struct sockaddr_in *s2 = (struct sockaddr_in *)cliaddr;
        if (memcmp(&s->sin_addr, &s2->sin_addr, sizeof(s2->sin_addr)))
            return 0;
        return 1;
    } else
    if (cliaddr->ss_family == AF_INET6) {
        struct sockaddr_in6 *s  = (struct sockaddr_in6 *)server_addr;
        struct sockaddr_in6 *s2 = (struct sockaddr_in6 *)cliaddr;
        if (memcmp(&s->sin6_addr, &s2->sin6_addr, sizeof(s2->sin6_addr)))
            return 0;
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------------
int peek(CConceptClient *OWNER, int socket) {
    struct sockaddr_storage cliaddr;
    socklen_t len = sizeof(cliaddr);
    char      buffer[65535];

    int res = recvfrom(socket, buffer, sizeof(buffer), MSG_PEEK, (struct sockaddr *)&cliaddr, &len);
    if ((res > 0) && (res <= 2)) {
        // ignore "hi" messages
        recvfrom(socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
        return 0;
    }
    if (res > 0) {
        if (!check_clientaddr(&cliaddr, &OWNER->LinkContainer.serveraddr)) {
            int res = recvfrom(socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
            if (res < 0)
                return -1;
            return 0;
        }
    }
    return res;
}

//-----------------------------------------------------------------------------------
int recv2(CConceptClient *OWNER, int socket, char *buffer, int size, int flags) {
    struct sockaddr_storage cliaddr;
    socklen_t len = sizeof(cliaddr);
    int       res = recvfrom(socket, buffer, size, 0, (struct sockaddr *)&cliaddr, &len);

    if (res > 0) {
        // check host
        if (!check_clientaddr(&cliaddr, &OWNER->LinkContainer.serveraddr)) {
            if ((OWNER->LinkContainer.is_p2p == 1) /*&& (res == 2)*/) {
                memset(&OWNER->LinkContainer.p2paddr, 0, sizeof(OWNER->LinkContainer.p2paddr));
                memcpy(&OWNER->LinkContainer.p2paddr, &cliaddr, len);
                OWNER->LinkContainer.p2paddr_len = len;
                OWNER->LinkContainer.is_p2p      = 2;
                char connect_ip[0xFF];
                connect_ip[0] = 0;

                inet_ntop(AF_INET, &((struct sockaddr_in *)&OWNER->LinkContainer.p2paddr)->sin_addr, connect_ip, INET6_ADDRSTRLEN + 1);
                return 0;
            }
            if (!OWNER->LinkContainer.is_p2p) {
                if (res == 2) {
                    memset(&OWNER->LinkContainer.lastaddr, 0, sizeof(OWNER->LinkContainer.lastaddr));
                    memcpy(&OWNER->LinkContainer.lastaddr, &cliaddr, len);
                    OWNER->LinkContainer.lastaddr_len = len;
                }
                return 0;
            }
        }
        if ((res == 2) && (OWNER->LinkContainer.is_p2p == 2)) {
            // skip hello package
            return 0;
        }
    }
    return res;
}

//-----------------------------------------------------------------------------------
void ResetMessages(CConceptClient *OWNER) {
    OWNER->LinkContainer.En_inited  = 0;
    OWNER->LinkContainer.Dec_inited = 0;
}

//-----------------------------------------------------------------------------------
void set_next_post_filename(CConceptClient *OWNER, char *filename) {
    if (OWNER->LinkContainer.PostFile)
        fclose(OWNER->LinkContainer.PostFile);
    OWNER->LinkContainer.PostFile = fopen(filename, "wb");
}

//-----------------------------------------------------------------------------------
int session_send(CConceptClient *OWNER, char *buf, int size, int flags, int is_realtime = 0) {
    int nsent;
    int len      = size;
    int RTSOCKET = OWNER->RTSOCKET;

    if (is_realtime) {
        if ((OWNER) && (RTSOCKET > 0) && (OWNER->LinkContainer.serveraddr.ss_family)) {
            if ((OWNER->LinkContainer.is_p2p == 2) && (OWNER->LinkContainer.p2paddr.ss_family)) {
                nsent = sendto(RTSOCKET, buf, len, 0, (struct sockaddr *)&OWNER->LinkContainer.p2paddr, OWNER->LinkContainer.p2paddr_len);
            } else {
                nsent = sendto(RTSOCKET, buf, len, 0, (struct sockaddr *)&OWNER->LinkContainer.serveraddr, OWNER->LinkContainer.server_len);
            }
            if (nsent <= 0) {
#ifdef _WIN32
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    if (WSAGetLastError() == WSAEMSGSIZE) {
#else
                if (errno != EAGAIN) {
                    if (errno == EMSGSIZE) {
#endif
                        fprintf(stderr, "Fall back to TCP socket (errno: %i, size: %i)\n", (int)errno, size);
                        RTSOCKET = -1;
                    } else {
                        RTSOCKET        = INVALID_SOCKET;
                        fprintf(stderr, "Dropping RT socket (send) (errno: %i, size: %i)\n", (int)errno, size);
                    }
                }
            }
        }
        return size;
    }

    while (len > 0) {
        nsent = OWNER->Send(buf, len);

        if (nsent < 0) {
            if (errno == EAGAIN) {          // || EWOULDBLOCK !!!!!!
#ifdef _WIN32
                Sleep(5);
#else
                usleep(5000);
#endif
                continue;
            } else
                return -1;
        }

        buf += nsent;
        len -= nsent;
    }
    return size;     // ok, all data sent
}

//-----------------------------------------------------------------------------------
inline int post_write(CConceptClient *OWNER, char *buf, int len) {
    if (!OWNER->LinkContainer.PostFile)
        return -1;
    return fwrite(buf, 1, len, OWNER->LinkContainer.PostFile);
}

//-----------------------------------------------------------------------------------
inline void post_done(CConceptClient *OWNER) {
    if (OWNER->LinkContainer.PostFile) {
        fclose(OWNER->LinkContainer.PostFile);
        OWNER->LinkContainer.PostFile = 0;
    }
}

//-----------------------------------------------------------------------------------
#define ADJUST_SIZE(s)    size2 -= s; if (size2 < 0) { return -1; }
int read_network_int(char *buffer, int base = 0) {
    int Int32;

    Int32  = (int)(unsigned char)buffer[base + 0] * 0x1000000;
    Int32 += (int)(unsigned char)buffer[base + 1] * 0x10000;
    Int32 += (int)(unsigned char)buffer[base + 2] * 0x100;
    Int32 += (int)(unsigned char)buffer[base + 3];
    return Int32;
}

unsigned short read_network_short(char *buffer, int base = 0) {
    char PACKED_ATTR temp[2];

    temp[0] = buffer[base + 0];
    temp[1] = buffer[base + 1];

    return ntohs(*(unsigned short *)temp);
}

void write_network_int(char *buffer, int val) {
    char PACKED_ATTR temp[4];

    *(int *)temp = htonl(val);

    buffer[0] = temp[0];
    buffer[1] = temp[1];
    buffer[2] = temp[2];
    buffer[3] = temp[3];
}

void write_network_uint(char *buffer, unsigned val) {
    char PACKED_ATTR temp[4];

    *(unsigned int *)temp = htonl(val);

    buffer[0] = temp[0];
    buffer[1] = temp[1];
    buffer[2] = temp[2];
    buffer[3] = temp[3];
}

void write_network_short(char *buffer, unsigned short val) {
    char PACKED_ATTR temp[2];

    *(unsigned short *)temp = htons(val);

    buffer[0] = temp[0];
    buffer[1] = temp[1];
}

int DeSerializeBuffer(char *buffer, int size, AnsiString *Owner, int *message, AnsiString *Target, AnsiString *Value, unsigned int compressed = 0) {
    // OWNER poate avea maxim 256 octeti
    int  size2 = size;
    int  res   = 0;
    char *orig = buffer;

    if (compressed) {
        unsigned short owner = read_network_short(orig);
        *Owner   = (long)owner;
        *message = 0x110;
        Target->LoadBuffer("1003", 4);
        Value->LoadBuffer(buffer + 2, size - 2);
        return res;
    }

    ADJUST_SIZE(1);
    unsigned char owner_size = (unsigned char)buffer[0];
    buffer++;
    int proc_size = 1;
    if (owner_size) {
        ADJUST_SIZE(owner_size);
        Owner->LoadBuffer(buffer, owner_size);
        //Owner->LoadBuffer(orig, proc_size, owner_size);
        buffer    += owner_size;
        proc_size += owner_size;
    } else
        *Owner = (char *)"";

    ADJUST_SIZE(sizeof(int));

    *message = read_network_int(orig, proc_size);

    buffer    += sizeof(int);
    proc_size += sizeof(int);
    ADJUST_SIZE(sizeof(unsigned short));
    unsigned short target_size = read_network_short(orig, proc_size);//ntohs(*(unsigned short *)buffer);
    buffer    += sizeof(short);
    proc_size += sizeof(short);
    if (target_size > 0) {
        ADJUST_SIZE(target_size);
        Target->LoadBuffer(buffer, target_size);
        //Target->LoadBuffer(orig, proc_size, target_size);
        buffer    += target_size;
        proc_size += target_size;
    } else {
        if (*message == 0x110)
            *Target = (char *)"1003";
        else
            *Target = (char *)"";
    }
    int last_size = size - proc_size; //(buffer-ptr);
    if (last_size > 0) {
        Value->LoadBuffer(buffer, last_size);
        //Value->LoadBuffer(orig, proc_size, last_size);
        res = proc_size;
    } else
        *Value = (char *)"";
    return res;
}

//-----------------------------------------------------------------------------------
char *SerializeBuffer(CConceptClient *Client, char **buffer, int *size, AnsiString *Owner, int message, AnsiString *Target, AnsiString *Value, bool buferize, FILE **file_buffer, int *fsize, char no_key, int *is_realtime, int *remaining) {
    // OWNER poate avea maxim 256 octeti
    int vsize = Value->Length();
    int tlen  = Target->Length();

    *fsize = 0;
    FILE *in = NULL;
    *file_buffer = NULL;
    if ((message == MSG_REQUEST_FOR_FILE) && (vsize)) {
        vsize = 0;
        in = fopen(Value->c_str(), "rb");
        if (in) {
            fseek(in, 0, SEEK_END);
            if (buferize)
                vsize = ftell(in);
            else
                *fsize = ftell(in);
            fseek(in, 0, SEEK_SET);
        }
    }
    if ((message == 0x110) && (!tlen)) {
        if ((no_key) && (vsize > 0)) {
            unsigned int owner = Owner->ToInt();
            if ((Client) && (Client->RTSOCKET > 0) && (Client->LinkContainer.serveraddr.ss_family) && (owner <= 0xFFFF) && (owner >= 0) && (is_realtime)) {
                *is_realtime = 1;
                int offset = 0;
                if (remaining)
                    offset = *remaining;
                int vsize2 = vsize - offset;
                if (vsize2 < 0)
                    vsize2 = 0;
                if (vsize2 > MAX_RT_SIZE) {
                    vsize2 = MAX_RT_SIZE;

                    if (remaining)
                        *remaining += vsize2;
                } else {
                    if (remaining)
                        *remaining = 0;
                }
                *size = vsize2 + sizeof(short);
                char *ptr = new char[*size];
                write_network_short(ptr, owner);
                char *buf = ptr + sizeof(short);
                *buffer = ptr;
                memcpy(buf, Value->c_str() + offset, vsize2);
                return ptr;
            } else {
                message = 0x1001;
                *Target = (char *)"350";
                tlen    = Target->Length();
            }
        } else {
            message = 0x1001;
            *Target = (char *)"350";
            tlen    = Target->Length();
        }
    }
    if (remaining)
        *remaining = 0;
    *size = sizeof(char) + Owner->Length() + sizeof(int) + sizeof(short) + tlen + vsize;
    char *ptr = new char[*size + 10];
    write_network_int(ptr, *size + *fsize);
    char *buf = ptr + 10;
    *buffer = buf;

    *(unsigned char *)buf = (unsigned char)Owner->Length();
    buf += sizeof(char);

    memcpy(buf, Owner->c_str(), Owner->Length());
    buf += Owner->Length();

    write_network_int(buf, message);
    buf += sizeof(int);

    write_network_short(buf, (unsigned short)tlen);
    buf += sizeof(short);

    memcpy(buf, Target->c_str(), Target->Length());
    buf += Target->Length();

    if (in) {
        if (buferize) {
            int r = 0;
            do {
                r = fread(buf, 1, RBUF_SIZE, in);
                if (r > 0)
                    buf += r;
            } while (r == RBUF_SIZE);
            fclose(in);
        } else
            *file_buffer = in;
    } else
        memcpy(buf, Value->c_str(), Value->Length());

    WSFrame(ptr + 10, *size, ptr, size);
    *buffer = ptr;

    return ptr;
}
//----------------------------------------------------------------------------------- 
int send_message(CConceptClient *OWNER, AnsiString SENDER_NAME, int MESSAGE_ID, AnsiString MESSAGE_TARGET, AnsiString& MESSAGE_DATA, PROGRESS_API notify_parent) {
    INTEGER res              = 0;
    char    *buffer          = 0;
    int     in_content_size  = 0;
    FILE    *in              = 0;
    int     file_buffer_size = 0;
    int     is_realtime      = 0;
    int     remaining        = 0;
    bool    big_message      = false;
    int     total_sent       = 0;

    if ((MESSAGE_ID == 0x1001) && (MESSAGE_TARGET == (char *)"350")) {
        MESSAGE_ID     = 0x110;
        MESSAGE_TARGET = "";
    }
    do {
        char *base_ptr = SerializeBuffer(OWNER, &buffer, &in_content_size, &SENDER_NAME, MESSAGE_ID, &MESSAGE_TARGET, &MESSAGE_DATA, true, &in, &file_buffer_size, true, &is_realtime, &remaining);

        if ((in_content_size + file_buffer_size >= BIG_MESSAGE) && (notify_parent)) {
            // notify big message !
            big_message = true;
            notify_parent(OWNER, -1, 1);
            int chunks = in_content_size / CHUNK_SIZE;
            if (in_content_size % CHUNK_SIZE)
                chunks++;
            for (int i = 0; i < chunks; i++) {
                int chunk_size = (i < chunks - 1) ? CHUNK_SIZE : in_content_size % CHUNK_SIZE;
                int to_send    = chunk_size;
                res = OWNER->BlockingSend(buffer + (i * CHUNK_SIZE), chunk_size);
                if (res != to_send) {
                    notify_parent(OWNER, -1, 2);
                    if (in) {
                        fclose(in);
                        in = 0;
                    }
                    break;
                }
                total_sent = i * CHUNK_SIZE + res;
                notify_parent(OWNER, (int)(((double)(total_sent) / (in_content_size + file_buffer_size)) * 100), 0);
            }
            if (!in)
                notify_parent(OWNER, 101, 0);
            // done big message !
        } else {
            session_send(OWNER, base_ptr, in_content_size, 0, is_realtime);
        }
        if (base_ptr)
            delete[] base_ptr;
    } while (remaining > 0);

    if (in) {
        int  r          = 0;
        char *file_buf  = new char[RBUF_SIZE];
        int  chunk_size = 0;
        do {
            chunk_size = fread(file_buf, 1, RBUF_SIZE, in);
            if (chunk_size > 0) {
                res = OWNER->BlockingSend(file_buf, chunk_size);
                if (res != chunk_size) {
                    if (big_message)
                        notify_parent(OWNER, -1, 2);
                    break;
                }
                total_sent += res;
                if (big_message)
                    notify_parent(OWNER, (int)(((double)(total_sent) / (in_content_size + file_buffer_size)) * 100), 0);
            }
        } while (chunk_size == RBUF_SIZE);
        delete[] file_buf;
        fclose(in);
        in = NULL;
        if (big_message)
            notify_parent(OWNER, 101, 0);
    }
    return res;
}
//---------------------------------------------------------------------------
int get_message(CConceptClient *OWNER, TParameters *PARAM, PROGRESS_API notify_parent) {
    unsigned int size2          = 0;
    int          size           = 0;
    int          received       = 0;
    int          result         = 0;
    int          rec_count      = 0;
    int          buf_size       = 0;
    char         *buffer        = 0;
    char         *output        = 0;
    int          filemarker     = 0;
    int          MSG_ID         = 0;
    int          remaining      = 0;
    char         buf_temp[0xFFFF];

    if (OWNER->LinkContainer.BufferedMessages.Count()) {
        TParameters *clone = (TParameters *)OWNER->LinkContainer.BufferedMessages.Remove(0);
        PARAM->Sender = clone->Sender;
        PARAM->ID     = clone->ID;
        PARAM->Target = clone->Target;
        PARAM->Value  = clone->Value;
        delete clone;
        return 1;
    }
    if (OWNER->RTSOCKET != INVALID_SOCKET) {
        if (sock_eof_timeout(OWNER->RTSOCKET, 0) == 0) {
            received = recv2(OWNER, OWNER->RTSOCKET, buf_temp, sizeof(buf_temp), 0);

            if (received <= 0) {
                PARAM->Sender = (char *)"";
                PARAM->ID     = 0;
                PARAM->Target = (char *)"";
                PARAM->Value  = (char *)"";
                return 1;
            }

            DeSerializeBuffer(buf_temp, received, &PARAM->Sender, &MSG_ID, &PARAM->Target, &PARAM->Value, 1);
            // just for safety
            switch (MSG_ID) {
                case 0x1001:
                    // event on buffer
                    if (PARAM->Target == (char *)"350") {
                        MSG_ID        = 0x110;
                        PARAM->Target = (char *)"1003";
                    } else
                        MSG_ID = -0x1001;
                    break;

                case 0x110:
                    if ((PARAM->Target != (char *)"1003") && (PARAM->Target != (char *)"1007"))
                        MSG_ID = -0x110;
                    break;

                default:
                    MSG_ID = -0x1000;
            }
            PARAM->ID = MSG_ID;

            return received;
        }
    }

    int code;
    int masked;
    size = WSGetPacketSize(OWNER, &code, &masked);
    if (size <= 0) {
        if (code) {
            if (OWNER->LinkContainer.PostFile)
                post_done(OWNER);
        }
        return 0;
    }

    buf_size = size;
    int  initial_buf   = 0;

    if (size) {
        if (OWNER->LinkContainer.PostFile) {
            buf_size = size > FILE_MESSAGE ? FILE_MESSAGE : size;
            if (size > FILE_MESSAGE)
                initial_buf = buf_size + 5;
            else
                initial_buf = buf_size;
        }
        buffer = new char[buf_size + 1];
    }

    //----------------------------------------//
    if ((size >= BIG_MESSAGE) && (notify_parent))
        // notify big message !
        notify_parent(OWNER, -1, 3);
    //----------------------------------------//
    rec_count = 0;
    do {
        if (OWNER->LinkContainer.PostFile) {
            if (filemarker) {
                remaining = size - rec_count;
                if (remaining > buf_size)
                    remaining = buf_size;
                received = OWNER->Recv(buffer, remaining);
            } else
                received = OWNER->Recv(buffer + rec_count, initial_buf - rec_count);

            rec_count += received;
            if (received > 0) {
                if (filemarker)
                    post_write(OWNER, buffer, received);
                else
                if (rec_count == initial_buf) {
                    PARAM->Sender = (char *)"";
                    PARAM->Target = (char *)"";
                    int fpos = 0;

                    fpos = DeSerializeBuffer(buffer, rec_count, &PARAM->Sender, &MSG_ID, &PARAM->Target, &PARAM->Value);
                    if ((fpos > 0) && (fpos < rec_count))
                        post_write(OWNER, buffer + fpos, rec_count - fpos);

                    PARAM->ID = MSG_ID;
                    PARAM->Value.LoadBuffer(0, 0);
                    filemarker = 1;
                }
            } else {
                CHECK_RECONNECT(received);
            }
        } else {
            received = OWNER->RecvTimeout(buffer + rec_count, size - rec_count);
            if (received > 0) {
                rec_count += received;
            } else {
                CHECK_RECONNECT(received);
            }
        }

        if ((size >= BIG_MESSAGE) && (notify_parent))
            notify_parent(OWNER, (int)(((double)rec_count / size) * 100), 0);
    } while ((received > 0) && (rec_count < size));

    if ((size >= BIG_MESSAGE) && (notify_parent))
        notify_parent(OWNER, 101, 0);
    // done big message !

    if (received <= 0) {
        if (buffer)
            delete[] buffer;
        return 0;
    }

    if (OWNER->LinkContainer.PostFile)
        post_done(OWNER);
    else {
        DeSerializeBuffer(buffer, size, &PARAM->Sender, &MSG_ID, &PARAM->Target, &PARAM->Value, size2);
        PARAM->ID = MSG_ID;
    }
    if (buffer)
        delete[] buffer;

    return size;
}

//---------------------------------------------------------------------------
int have_messages(CConceptClient *OWNER) {
    return !OWNER->IsEOF();
}

//---------------------------------------------------------------------------
int wait_message(CConceptClient *OWNER, TParameters *PARAM, int MESSAGE_ID, PROGRESS_API notify_parent) {
    int res = 0;

    do {
        res = get_message(OWNER, PARAM, notify_parent);
        if (res <= 0)
            break;
        if ((PARAM) && (PARAM->ID == MESSAGE_ID))
            return res;
        else {
            TParameters *clone = new TParameters;
            clone->Sender = PARAM->Sender;
            clone->ID     = PARAM->ID;
            clone->Target = PARAM->Target;
            clone->Value  = PARAM->Value;
            clone->Owner  = PARAM->Owner;

            OWNER->LinkContainer.BufferedMessages.Add(clone, DATA_MESSAGE);
        }
    } while (res > 0);
    return res;
}

//---------------------------------------------------------------------------
void InitUDP(CConceptClient *owner, int port) {
    char                    buffer[0xFF];
    int                     sockfd;
    struct addrinfo         hints;
    struct addrinfo         *res = 0;
    struct sockaddr_storage cliaddr;

    if (!owner)
        return;

    if (owner->RTSOCKET) {
#ifdef _WIN32
        closesocket(owner->RTSOCKET);
#else
        close(owner->RTSOCKET);
#endif
        owner->RTSOCKET = INVALID_SOCKET;
    }

    if (port < 0)
        return;

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    AnsiString port_str((long)port);
    int        gerr = getaddrinfo(owner->Called_HOST, port_str.c_str(), &hints, &res);
    if ((gerr != 0) || (!res)) {
        AnsiString err((char *)"Error in getaddrinfo(): ");
        err += owner->Called_HOST;
        err += (char *)":";
        err += port_str;
        err += (char *)" ";
        err += (char *)gai_strerror(gerr);
        return;
    }

    if ((res->ai_family != AF_INET) && (res->ai_family != AF_INET6)) {
        freeaddrinfo(res);
        return;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        freeaddrinfo(res);
        return;
    }

    int flag2 = 0x10;
    setsockopt(sockfd, IPPROTO_IP, IP_TOS, (char *)&flag2, sizeof(int));

    flag2 = 0xFFFF;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&flag2, sizeof(int));

    memcpy(&owner->LinkContainer.serveraddr, res->ai_addr, res->ai_addrlen);
    owner->LinkContainer.server_len = res->ai_addrlen;
    socklen_t cllen = sizeof(cliaddr);
    if (sendto(sockfd, "1", 1, 0, (struct sockaddr *)&owner->LinkContainer.serveraddr, owner->LinkContainer.server_len) == 1) {
        if (sock_eof_timeout(sockfd, 500) == 0) {
            if (recvfrom(sockfd, buffer, 1, 0, (struct sockaddr *)&cliaddr, &cllen) == 1) {
                sendto(sockfd, "2", 1, 0, (struct sockaddr *)&owner->LinkContainer.serveraddr, owner->LinkContainer.server_len);
                owner->RTSOCKET = sockfd;
            }
        }
    }
    freeaddrinfo(res);
}

int CreateUDPSocket(int port, int ipv6, unsigned short& out_port) {
    struct sockaddr_in6 sin6;
    struct sockaddr_in  sin;
    int sockfd = -1;

    if (ipv6) {
        memset(&sin6, 0, sizeof(struct sockaddr_in6));
        if (port > 0)
            sin6.sin6_port = htons(port);
        sin6.sin6_family = AF_INET6;
        sin6.sin6_addr   = in6addr_any;
        sockfd           = socket(AF_INET6, SOCK_DGRAM, 0);
        if (port > 0)
            bind(sockfd, (struct sockaddr *)&sin6, sizeof(sin6));
        out_port = ntohs(sin6.sin6_port);
    } else {
        memset(&sin, 0, sizeof(struct sockaddr_in));
        sin.sin_family = AF_INET;
        if (port > 0)
            sin.sin_port = htons(port);
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (port > 0)
            bind(sockfd, (struct sockaddr *)&sin, sizeof(sin));
        out_port = ntohs(sin.sin_port);
    }
    int flag2 = 0x10;
    setsockopt(sockfd, IPPROTO_IP, IP_TOS, (char *)&flag2, sizeof(int));
    flag2 = 0xFFFF;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&flag2, sizeof(int));
    return sockfd;
}

int STUN(int sockfd, char *stun_server, char *stun_port, char *return_ip, unsigned short *return_port) {
    if ((sockfd == INVALID_SOCKET) || (sockfd < 0))
        return 0;

    struct addrinfo hints;
    struct addrinfo *res = 0;
    unsigned char   buf[300];
    unsigned char   bindingReq[20];
    unsigned short  attr_type;
    unsigned short  attr_length;
    unsigned short  port = 0;

    srand(time(NULL));
    memset(&hints, 0, sizeof hints);
    hints.ai_family            = AF_INET;//AF_UNSPEC;
    hints.ai_socktype          = SOCK_DGRAM;
    hints.ai_protocol          = IPPROTO_UDP;
    *(short *)(&bindingReq[0]) = htons(0x0001);     // stun_method
    *(short *)(&bindingReq[2]) = htons(0x0000);     // msg_length
    *(int *)(&bindingReq[4])   = htonl(0x2112A442); // magic cookie
    int _id = rand() & 0xff;
    _id |= (rand() & 0xff) << 8;
    _id |= (rand() & 0xff) << 16;
    _id |= (rand() & 0xff) << 24;
    *(int *)(&bindingReq[8])  = htonl(_id);
    *(int *)(&bindingReq[12]) = htonl(0x0714278f);
    *(int *)(&bindingReq[16]) = htonl(0x5ded3221);

    int gerr = getaddrinfo(stun_server, stun_port, &hints, &res);
    if ((gerr != 0) || (!res))
        return 0;
    if ((res->ai_family != AF_INET) && (res->ai_family != AF_INET6)) {
        freeaddrinfo(res);
        return 0;
    }
    int n = sendto(sockfd, (char *)bindingReq, sizeof(bindingReq), 0, (struct sockaddr *)res->ai_addr, res->ai_addrlen);
    if (n == -1) {
#ifdef _WIN32
        closesocket(sockfd);
#else
        close(sockfd);
#endif
        freeaddrinfo(res);
        return 0;
    }
    freeaddrinfo(res);
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    fd_set s;
    FD_ZERO(&s);
    FD_SET(sockfd, &s);

    int val = select(FD_SETSIZE, &s, 0, 0, &timeout);
    if (val < 0)
        return 0;

    if ((!val) || (!FD_ISSET(sockfd, &s)))
        return 0;

    n = recvfrom(sockfd, (char *)buf, 300, 0, NULL, 0); // recv UDP
    if ((n > 0) && (*(short *)(&buf[0]) == htons(0x0101))) {
        // int n2 = htons(*(short *)(&buf[2]));
        if (ntohl(*(int *)(&buf[8])) != _id)
            return 0;
        int i = 20;
        while (i + 2 < n) {
            attr_type   = htons(*(short *)(&buf[i]));
            attr_length = htons(*(short *)(&buf[i + 2]));

            if ((attr_type == 0x0020) || (attr_type == 0x8020)) {
                if (i + 6 >= n)
                    return 0;
                port  = ntohs(*(short *)(&buf[i + 6]));
                port ^= 0x2112;

                *return_port = port;
                if ((attr_length != 8) || (i + 11 >= n))
                    return 0;
                sprintf(return_ip, "%d.%d.%d.%d", buf[i + 8] ^ 0x21, buf[i + 9] ^ 0x12, buf[i + 10] ^ 0xA4, buf[i + 11] ^ 0x42);
                return 1;
            }
            i += (4 + attr_length);
        }
    }
    return 0;
}

AnsiString InitUDP2(CConceptClient *owner, char *host) {
    AnsiString result;

    if (owner->RTSOCKET) {
#ifdef _WIN32
        closesocket(owner->RTSOCKET);
#else
        close(owner->RTSOCKET);
#endif
        owner->RTSOCKET = INVALID_SOCKET;
    }

    if ((!owner) || (!host) || (!host[0]))
        return result;

    int s_len = strlen(host);

    char *stun_server = host;
    char *p_str       = strchr(host, ',');
    if ((!p_str) || (p_str[1] == '-'))
        return result;
    p_str[0] = 0;
    p_str++;

    char *local_host = strchr(p_str, ',');
    if ((!local_host) || (local_host[1] == '-'))
        return result;
    local_host[0] = 0;
    local_host++;

    char *local_port = strchr(local_host, ',');
    if ((!local_port) || (local_port[1] == '-'))
        return result;
    local_port[0] = 0;
    local_port++;
    AnsiString lport(local_port);

    struct addrinfo hints;
    struct addrinfo *a_res = 0;
    memset(&owner->LinkContainer.serveraddr, 0, sizeof(owner->LinkContainer.serveraddr));
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int gerr = getaddrinfo(local_host, local_port, &hints, &a_res);
    if ((gerr != 0) || (!a_res)) {
        AnsiString err((char *)"Error in getaddrinfo(): ");
        err += host;
        err += (char *)":";
        err += p_str;
        err += (char *)" ";
        err += (char *)gai_strerror(gerr);
        fprintf(stderr, "Dropping RT socket: %s\n", err.c_str());
        result = "";
        return result;
    }
    if ((a_res->ai_family != AF_INET) && (a_res->ai_family != AF_INET6)) {
        freeaddrinfo(a_res);
        fprintf(stderr, "Invalid socket family\n");
        return result;
    }

    memcpy(&owner->LinkContainer.serveraddr, a_res->ai_addr, a_res->ai_addrlen);
    owner->LinkContainer.server_len = a_res->ai_addrlen;

    unsigned short connect_port = 0;
    int            socket       = CreateUDPSocket(lport.ToInt(), a_res->ai_family == AF_INET6, connect_port);
    freeaddrinfo(a_res);

    if (socket < 0)
        return result;

    char connect_ip[0xFF];
    connect_ip[0] = 0;
    if (stun_server[0] == '*') {
        strcpy(connect_ip, "self");
    } else {
        int res = STUN(socket, stun_server, p_str, connect_ip, &connect_port);
        if (!res) {
#ifdef _WIN32
            closesocket(socket);
#else
            close(socket);
#endif
            return result;
        }
    }
    owner->RTSOCKET = socket;
    result          = connect_ip;
    result         += (char *)",";
    result         += AnsiString((long)connect_port);
    return result;
}

AnsiString InitUDP3(CConceptClient *owner, char *host) {
    AnsiString result;

    if ((owner) && (owner->RTSOCKET)) {
        char *p_str = strchr(host, ',');
        if ((!p_str) || (p_str[1] == '-')) {
#ifdef _WIN32
            closesocket(owner->RTSOCKET);
#else
            close(owner->RTSOCKET);
#endif
            owner->RTSOCKET = INVALID_SOCKET;
            return result;
        }
        p_str[0] = 0;
        p_str++;

        struct addrinfo hints;
        struct addrinfo *res = 0;

        memset(&hints, 0, sizeof hints);
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        int gerr = getaddrinfo(host, p_str, &hints, &res);
        if ((gerr != 0) || (!res)) {
            AnsiString err((char *)"Error in getaddrinfo(): ");
            err += host;
            err += (char *)":";
            err += p_str;
            err += (char *)" ";
            err += (char *)gai_strerror(gerr);
#ifdef _WIN32
            closesocket(owner->RTSOCKET);
#else
            close(owner->RTSOCKET);
#endif
            owner->RTSOCKET = INVALID_SOCKET;
            return result;
        }

        if ((res->ai_family != AF_INET) && (res->ai_family != AF_INET6)) {
            freeaddrinfo(res);
#ifdef _WIN32
            closesocket(owner->RTSOCKET);
#else
            close(owner->RTSOCKET);
#endif
            owner->RTSOCKET = INVALID_SOCKET;
            return result;
        }
        memcpy(&owner->LinkContainer.serveraddr, res->ai_addr, res->ai_addrlen);
        owner->LinkContainer.server_len = res->ai_addrlen;

        if (sendto(owner->RTSOCKET, "hi", 2, 0, (struct sockaddr *)&owner->LinkContainer.serveraddr, owner->LinkContainer.server_len) >= 0)
            result = (char *)"1";
        freeaddrinfo(res);
    }
    return result;
}

AnsiString P2PInfo(CConceptClient *owner, char *host) {
    AnsiString result;

    if (owner->LinkContainer.is_p2p == 2) {
        char connect_ip[INET6_ADDRSTRLEN + 1];
        connect_ip[0] = 0;
        connect_ip[INET6_ADDRSTRLEN] = 0;
        int connect_port = 0;
        if (owner->LinkContainer.p2paddr.ss_family == AF_INET) {
            inet_ntop(owner->LinkContainer.p2paddr.ss_family, &((struct sockaddr_in *)&owner->LinkContainer.p2paddr)->sin_addr, connect_ip, INET6_ADDRSTRLEN + 1);
            connect_port = ntohs(((struct sockaddr_in *)&owner->LinkContainer.p2paddr)->sin_port);
        } else
        if (owner->LinkContainer.p2paddr.ss_family == AF_INET6) {
            inet_ntop(owner->LinkContainer.p2paddr.ss_family, &((struct sockaddr_in6 *)&owner->LinkContainer.p2paddr)->sin6_addr, connect_ip, INET6_ADDRSTRLEN + 1);
            connect_port = ntohs(((struct sockaddr_in6 *)&owner->LinkContainer.p2paddr)->sin6_port);
        }
        result  = connect_ip;
        result += ":";
        result += AnsiString((long)connect_port);
    }
    return result;
}

AnsiString SwitchP2P(CConceptClient *owner, char *host) {
    AnsiString result;

    owner->LinkContainer.is_p2p = 0;
    memset(&owner->LinkContainer.p2paddr, 0, sizeof(owner->LinkContainer.p2paddr));
    owner->LinkContainer.p2paddr_len = 0;
    if (owner->RTSOCKET <= 0)
        return result;

    if ((!owner) || (!host) || (!host[0]))
        return result;

    int s_len = strlen(host);

    char *stun_server = host;
    char *p_str       = strchr(host, ',');
    if ((!p_str) || (p_str[1] == '-'))
        return result;
    p_str[0] = 0;
    p_str++;

    char *local_host = strchr(p_str, ',');
    if ((!local_host) || (local_host[1] == '-'))
        return result;
    local_host[0] = 0;
    local_host++;

    char *local_port = strchr(local_host, ',');
    if ((!local_port) || (local_port[1] == '-'))
        return result;
    local_port[0] = 0;
    local_port++;
    AnsiString lport(local_port);

    struct addrinfo hints;
    struct addrinfo *a_res = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int gerr = getaddrinfo(local_host, local_port, &hints, &a_res);
    if ((gerr != 0) || (!a_res)) {
        AnsiString err((char *)"Error in getaddrinfo(): ");
        err += host;
        err += (char *)":";
        err += p_str;
        err += (char *)" ";
        err += (char *)gai_strerror(gerr);
        fprintf(stderr, "Dropping RT socket\n");
        result = "";
        return result;
    }
    if ((a_res->ai_family != AF_INET) && (a_res->ai_family != AF_INET6)) {
        freeaddrinfo(a_res);
        fprintf(stderr, "Invalid socket family\n");
        return result;
    }

    memcpy(&owner->LinkContainer.p2paddr, a_res->ai_addr, a_res->ai_addrlen);
    owner->LinkContainer.p2paddr_len = a_res->ai_addrlen;

    unsigned short connect_port = 0;

    char connect_ip[0xFF];
    connect_ip[0] = 0;
    if (stun_server[0] == '*') {
        if (a_res->ai_family == AF_INET) {
            inet_ntop(a_res->ai_family, &((struct sockaddr_in *)&owner->LinkContainer.p2paddr)->sin_addr, connect_ip, INET6_ADDRSTRLEN + 1);
            connect_port = ntohs(((struct sockaddr_in *)&owner->LinkContainer.p2paddr)->sin_port);
        } else {
            inet_ntop(a_res->ai_family, &((struct sockaddr_in6 *)&owner->LinkContainer.p2paddr)->sin6_addr, connect_ip, INET6_ADDRSTRLEN + 1);
            connect_port = ntohs(((struct sockaddr_in6 *)&owner->LinkContainer.p2paddr)->sin6_port);
        }
        freeaddrinfo(a_res);
    } else {
        freeaddrinfo(a_res);
        int res = STUN(owner->RTSOCKET, stun_server, p_str, connect_ip, &connect_port);
        if (!res)
            return result;
    }
    result  = connect_ip;
    result += (char *)",";
    result += AnsiString((long)connect_port);
    owner->LinkContainer.is_p2p  = 1;
    if (owner->LinkContainer.lastaddr_len) {
        // already received hello package
        memcpy(&owner->LinkContainer.p2paddr, &owner->LinkContainer.lastaddr, owner->LinkContainer.lastaddr_len);
        owner->LinkContainer.p2paddr_len = owner->LinkContainer.lastaddr_len;
        memset(&owner->LinkContainer.lastaddr, 0, sizeof(owner->LinkContainer.lastaddr));
        owner->LinkContainer.lastaddr_len = 0;
        owner->LinkContainer.is_p2p       = 2;
    }
    sendto(owner->RTSOCKET, "hi", 2, 0, (struct sockaddr *)&owner->LinkContainer.p2paddr, owner->LinkContainer.p2paddr_len);
    sendto(owner->RTSOCKET, "hi", 2, 0, (struct sockaddr *)&owner->LinkContainer.p2paddr, owner->LinkContainer.p2paddr_len);
    sendto(owner->RTSOCKET, "hi", 2, 0, (struct sockaddr *)&owner->LinkContainer.p2paddr, owner->LinkContainer.p2paddr_len);
    return result;
}

void SetVectors(void *client, unsigned char *v_send, int v_send_len, unsigned char *v_recv, int v_recv_len) {
} 
