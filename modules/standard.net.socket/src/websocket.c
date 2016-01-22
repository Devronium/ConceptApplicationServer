/*
 * Copyright (c) 2010 Putilov Andrey
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "websocket.h"
#ifndef _WIN32
 #include <arpa/inet.h>
#endif

#ifndef TRUE
 #define TRUE     1
#endif
#ifndef FALSE
 #define FALSE    1
#endif

static char       rn[] PROGMEM = "\r\n";
static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#define LITTLE_ENDIAN

void nullhandshake(struct handshake *hs) {
    hs->host     = NULL;
    hs->key      = NULL;
    hs->key1     = NULL;
    hs->key2     = NULL;
    hs->origin   = NULL;
    hs->protocol = NULL;
    hs->resource = NULL;
    hs->version  = 0;
}

static uint32_t doStuffToObtainAnInt32(const char *key) {
    char    res_decimals[15] = "";
    char    *tail_res        = res_decimals;
    uint8_t space_count      = 0;
    uint8_t i = 0;

    do {
        if (isdigit(key[i]))
            strncat(tail_res++, &key[i], 1);
        if (key[i] == ' ')
            space_count++;
    } while (key[++i]);

    return (uint32_t)strtoul(res_decimals, NULL, 10) / space_count;
}

static char *get_upto_linefeed(const char *start_from) {
    char    *write_to;
    uint8_t new_length = strstr_P(start_from, rn) - start_from + 1;

    assert(new_length);
    write_to = (char *)malloc(new_length);     //+1 for '\x00'
    assert(write_to);
    memcpy(write_to, start_from, new_length - 1);
    write_to[new_length - 1] = 0;

    return write_to;
}

enum ws_frame_type ws_parse_handshake(const uint8_t *input_frame, size_t input_len,
                                      struct handshake *hs) {
    const char *input_ptr = (const char *)input_frame;
    const char *end_ptr   = (const char *)input_frame + input_len;

    // measure resource size
    char *first = strchr((const char *)input_frame, ' ');

    if (!first)
        return WS_ERROR_FRAME;
    first++;
    char *second = strchr(first, ' ');
    if (!second)
        return WS_ERROR_FRAME;

    if (hs->resource) {
        free(hs->resource);
        hs->resource = NULL;
    }
    hs->resource = (char *)malloc(second - first + 1);     // +1 is for \x00 symbol
    assert(hs->resource);

    if (sscanf_P(input_ptr, PSTR("GET %s HTTP/1.1\r\n"), hs->resource) != 1)
        return WS_ERROR_FRAME;
    input_ptr = strstr_P(input_ptr, rn) + 2;

    /*
            parse next lines
     */
#define input_ptr_len    (input_len - (input_ptr - input_frame))
#define prepare(x)    do { if (x) { free(x); x = NULL; } } while (0)
    uint8_t connection_flag = FALSE;
    uint8_t upgrade_flag    = FALSE;
    while (input_ptr < end_ptr && input_ptr[0] != '\r' && input_ptr[1] != '\n') {
        if (memcmp_P(input_ptr, host, strlen_P(host)) == 0) {
            input_ptr += strlen_P(host);
            prepare(hs->host);
            hs->host = get_upto_linefeed(input_ptr);
        } else
        if (memcmp_P(input_ptr, origin, strlen_P(origin)) == 0) {
            input_ptr += strlen_P(origin);
            prepare(hs->origin);
            hs->origin = get_upto_linefeed(input_ptr);
        } else
        if (memcmp_P(input_ptr, protocol, strlen_P(protocol)) == 0) {
            input_ptr += strlen_P(protocol);
            prepare(hs->protocol);
            hs->protocol = get_upto_linefeed(input_ptr);
        } else
        if (memcmp_P(input_ptr, key1, strlen_P(key1)) == 0) {
            input_ptr += strlen_P(key1);
            prepare(hs->key1);
            hs->key1 = get_upto_linefeed(input_ptr);
        } else
        if (memcmp_P(input_ptr, key2, strlen_P(key2)) == 0) {
            input_ptr += strlen_P(key2);
            prepare(hs->key2);
            hs->key2 = get_upto_linefeed(input_ptr);
        } else
        if (memcmp_P(input_ptr, key, strlen_P(key)) == 0) {
            input_ptr += strlen_P(key);
            prepare(hs->key);
            hs->key = get_upto_linefeed(input_ptr);
        } else
        if (memcmp_P(input_ptr, version, strlen_P(version)) == 0) {
            input_ptr  += strlen_P(version);
            hs->version = atoi(get_upto_linefeed(input_ptr));
        } else
        if (memcmp_P(input_ptr, connection, strlen_P(connection)) == 0) {
            connection_flag = TRUE;
        } else
        if (memcmp_P(input_ptr, upgrade2, strlen_P(upgrade2)) == 0) {
            upgrade_flag = TRUE;
        } else
        if (memcmp_P(input_ptr, upgrade, strlen_P(upgrade)) == 0) {
            upgrade_flag = TRUE;
        }

        input_ptr = strstr_P(input_ptr, rn) + 2;
    }

    // we have read all data, so check them
    if (!hs->host || !hs->origin || ((!hs->key1 || !hs->key2) && (!hs->key)) ||
        !connection_flag || !upgrade_flag)
        return WS_ERROR_FRAME;

    input_ptr += 2;     // skip empty line
    if (hs->version == 8) {
        if (end_ptr - input_ptr < 8)
            return WS_INCOMPLETE_FRAME;
        memcpy(hs->key3, input_ptr, 8);
    }

    return WS_OPENING_FRAME;
}

void encodeblock(unsigned char in[3], unsigned char out[4], int len) {
    out[0] = cb64[in[0] >> 2];
    out[1] = cb64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
    out[2] = (unsigned char)(len > 1 ? cb64[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)] : '=');
    out[3] = (unsigned char)(len > 2 ? cb64[in[2] & 0x3f] : '=');
}

/*
** encode
**
** base64 encode a stream adding padding and line breaks as per spec.
*/
long encode(char *in_buffer, int in_buffer_size, char *out_buffer) {
    int           linesize = 72;
    unsigned char in[3], out[4];
    int           i, len, blocksout = 0;

    char *ptr     = in_buffer;
    char *out_ptr = out_buffer;

    while (ptr <= in_buffer + in_buffer_size) {
        len = 0;
        for (i = 0; i < 3; i++) {
            in[i] = (unsigned char)ptr[0];
            ptr++;

            if (ptr <= in_buffer + in_buffer_size) {
                len++;
            } else {
                in[i] = 0;
            }
        }
        if (len) {
            encodeblock(in, out, len);
            for (i = 0; i < 4; i++) {
                out_ptr[0] = out[i];
                out_ptr++;
            }
            blocksout++;
        }
        if ((blocksout >= (linesize / 4)) || (ptr > in_buffer + in_buffer_size)) {
            /*if( blocksout ) {
                out_ptr[0]='\r';
                out_ptr[1]='\n';
                out_ptr+=2;
               }*/
            blocksout = 0;
        }
    }
    return (intptr_t)out_ptr - (intptr_t)out_buffer;
}

enum ws_frame_type ws_get_handshake_answer(const struct handshake *hs,
                                           uint8_t *out_frame, size_t *out_len) {
    assert(out_frame && *out_len);
    // hs->key3 is always not NULL
    assert(hs && hs->origin && hs->host && hs->resource);

    uint8_t raw_md5[16];
    uint8_t keys[16];
    uint8_t raw_sha1[64];
    raw_sha1[0] = 0;

    unsigned int written;
    if (hs->version == 8) {
        if ((!hs->key1) || (!hs->key2)) {
            out_len = 0;
            return 0;
        }

        uint8_t  chrkey1[4];
        uint8_t  chrkey2[4];
        uint32_t key1 = doStuffToObtainAnInt32(hs->key1);
        uint32_t key2 = doStuffToObtainAnInt32(hs->key2);
        uint8_t  i;
        for (i = 0; i < 4; i++)
            chrkey1[i] = key1 << (8 * i) >> (8 * 3);
        for (i = 0; i < 4; i++)
            chrkey2[i] = key2 << (8 * i) >> (8 * 3);

        memcpy(keys, chrkey1, 4);
        memcpy(&keys[4], chrkey2, 4);
        memcpy(&keys[8], hs->key3, 8);
        md5(raw_md5, keys, sizeof(keys) * 8);
    }
    if (hs->version == 13) {
        if (!hs->key) {
            out_len = 0;
            return 0;
        }
        char key_buffer[0xFFF];
        int  p = strlen_P(hs->key);
        if (p > 2048)
            p = 2048;
        memcpy(key_buffer, hs->key, p);
        int p2 = strlen_P(magic);
        memcpy(key_buffer + p, magic, p2);
        key_buffer[p + p2] = 0;

        SHA1Context CTX;

        SHA1Reset(&CTX);
        SHA1Input(&CTX, key_buffer, p + p2);
        SHA1Result(&CTX);

        unsigned char sha1_sum[20];

        unsigned char *sha1_ptr = (unsigned char *)CTX.Message_Digest;
        int           j;
        int           k;
        for (j = 0; j < 5; j++) {
#ifdef LITTLE_ENDIAN
            for (k = 3; k >= 0; k--) {
                sha1_sum[j * 4 + 3 - k] = sha1_ptr[j * 4 + k];
            }
#else
            for (k = 0; k < 4; k++)
                sha1_sum[j * 4 + k] = sha1_ptr[j * 4 + k];
#endif
        }
        int len = encode(sha1_sum, 20, raw_sha1);
        raw_sha1[len] = 0;
    }
    if (hs->version == 13) {
        written = sprintf_P((char *)out_frame,
                            PSTR("HTTP/1.1 101 Switching Protocols\r\n"
                                 "Upgrade: websocket\r\n"
                                 "Connection: Upgrade\r\n"
                                 "Sec-WebSocket-Accept: %s\r\n"),
                            raw_sha1);
    } else {
        written = sprintf_P((char *)out_frame,
                            PSTR("HTTP/1.1 101 WebSocket Protocol Handshake\r\n"
                                 "Upgrade: WebSocket\r\n"
                                 "Connection: Upgrade\r\n"
                                 "Sec-WebSocket-Origin: %s\r\n"
                                 "Sec-WebSocket-Location: ws://%s%s\r\n"),
                            hs->origin,
                            hs->host,
                            hs->resource);
    }
    if (hs->protocol)
        written += sprintf_P((char *)out_frame + written,
                             PSTR("Sec-WebSocket-Protocol: %s\r\n"), hs->protocol);
    written += sprintf_P((char *)out_frame + written, rn);

    // if assert fail, that means, that we corrupt memory
    if (hs->version == 8) {
        assert(written <= *out_len && written + sizeof(keys) <= *out_len);
        memcpy(out_frame + written, raw_md5, sizeof(keys));
        *out_len = written + sizeof(keys);
    } else {
        *out_len = written;
    }

    return WS_OPENING_FRAME;
}

uint32_t SwapShort(uint16_t a) {
    a = ((a & 0x00FF) << 8) | ((a & 0xFF00) >> 8);
    return a;
}

uint32_t SwapWord(uint32_t a) {
    a = ((a & 0x000000FF) << 24) |
        ((a & 0x0000FF00) << 8) |
        ((a & 0x00FF0000) >> 8) |
        ((a & 0xFF000000) >> 24);
    return a;
}

uint64_t SwapDWord(uint64_t a) {
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

enum ws_frame_type ws_make_frame(const uint8_t *data, size_t data_len,
                                 uint8_t *out_frame, size_t *out_len, enum ws_frame_type frame_type) {
    assert(out_frame && *out_len);
    assert(data);

    if (frame_type == WS_TEXT_FRAME) {
        // check on latin alphabet. If not - return error
        uint8_t *data_ptr = (uint8_t *)data;
        uint8_t *end_ptr  = (uint8_t *)data + data_len;
        do {
            if (*data_ptr >> 7)
                return WS_ERROR_FRAME;
        } while ((++data_ptr < end_ptr));

        assert(*out_len >= data_len + 2);
        out_frame[0] = '\x00';
        memcpy(&out_frame[1], data, data_len);
        out_frame[data_len + 1] = '\xFF';
        *out_len = data_len + 2;
    } else if (frame_type == WS_BINARY_FRAME) {
        uint64_t tmp = data_len;

        /*uint8_t out_size_buf[sizeof (size_t)+2];
           uint8_t *size_ptr = out_size_buf;
           while (tmp > 0xFF) {
         * size_ptr = ((unsigned char)tmp) | 0x7F;
                tmp >>= 7;
                size_ptr++;
           }
         * size_ptr = tmp;*/
        int size_len = 0;
        out_frame[0] = '\x80';
        out_frame++;
        if (tmp <= 0x7D) {
            *out_frame = tmp;
            size_len++;
            out_frame++;
        } else
        if (tmp < 0xFFFF) {
            *out_frame = 0x7E;
            out_frame++;
#ifdef LITTLE_ENDIAN
            *(unsigned short *)out_frame = SwapShort((unsigned short)tmp);
#else
            *(unsigned short *)out_frame = (unsigned short)tmp;
#endif
            size_len  += 3;
            out_frame += 2;
        } else {
            *out_frame = 0x7F;
            out_frame++;
#ifdef LITTLE_ENDIAN
            *(uint64_t *)out_frame = SwapDWord(tmp);
#else
            *(uint64_t *)out_frame = tmp;
#endif
            size_len  += 9;
            out_frame += 8;
        }
        memcpy(out_frame, data, data_len);
        *out_len = data_len + size_len + 1;
    }

    return frame_type;
}

enum ws_frame_type ws_parse_input_frame(const uint8_t *input_frame, size_t input_len,
                                        uint8_t **out_data_ptr, size_t *out_len) {
    enum ws_frame_type frame_type;

    assert(out_len);
    assert(input_len);

    if (input_len < 2)
        return WS_INCOMPLETE_FRAME;

    if ((input_frame[0] & 0x80) != 0x80) { // text frame
        const uint8_t *data_start = &input_frame[1];
        uint8_t       *end        = (uint8_t *)memchr(data_start, 0xFF, input_len - 1);
        if (end) {
            assert((size_t)(end - data_start) <= input_len);
            *out_data_ptr = (uint8_t *)data_start;
            *out_len      = end - data_start;
            frame_type    = WS_TEXT_FRAME;
        } else {
            frame_type = WS_INCOMPLETE_FRAME;
        }
    } else if ((input_frame[0] & 0x80) == 0x80) { // binary frame
        if ((input_frame[0] == 0xFF) && (input_frame[1] == 0x00))
            frame_type = WS_CLOSING_FRAME;
        else {
            const uint8_t *frame_ptr  = &input_frame[1];
            uint64_t      data_length = 0;
            switch (*frame_ptr) {
                case 0x7E:
                    frame_ptr++;
#ifdef LITTLE_ENDIAN
                    data_length = SwapShort(*(unsigned short *)frame_ptr);
#else
                    data_length = *(unsigned short *)frame_ptr;
#endif
                    frame_ptr += 2;
                    if (data_length > input_len - 3)
                        return WS_ERROR_FRAME;
                    break;

                case 0x7F:
#ifdef LITTLE_ENDIAN
                    data_length = SwapDWord(*(uint64_t *)frame_ptr);
#else
                    data_length = *(uint64_t *)frame_ptr;
#endif
                    frame_ptr += 8;
                    if (data_length > input_len - 9)
                        return WS_ERROR_FRAME;
                    break;

                default:
                    if (*frame_ptr <= 0x7D) {
                        data_length = *frame_ptr;
                        frame_ptr++;
                        if (data_length > input_len - 2)
                            return WS_ERROR_FRAME;
                    } else
                        return WS_ERROR_FRAME;
            }
            *out_data_ptr = (uint8_t *)frame_ptr;
            *out_len      = data_length;
            frame_type    = WS_BINARY_FRAME;
        }
    } else
        frame_type = WS_ERROR_FRAME;


    return frame_type;
}
