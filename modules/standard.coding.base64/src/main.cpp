//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
//-----------------------------------------------------------------------------------

/*
** Translation Table as described in RFC1113
*/
static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[] = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
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
            if (blocksout) {
                out_ptr[0] = '\r';
                out_ptr[1] = '\n';
                out_ptr   += 2;
            }
            blocksout = 0;
        }
    }
    return (long)out_ptr - (long)out_buffer;
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
void decodeblock(unsigned char in[4], unsigned char out[3]) {
    out[0] = (unsigned char )(in[0] << 2 | in[1] >> 4);
    out[1] = (unsigned char )(in[1] << 4 | in[2] >> 2);
    out[2] = (unsigned char )(((in[2] << 6) & 0xc0) | in[3]);
}

/*
** decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
*/
int decode(char *in_buffer, int in_buffer_size, char *out_buffer) {
    unsigned char in[4], out[3], v;
    int           i, len;

    char *ptr     = in_buffer;
    char *out_ptr = out_buffer;

    while (ptr <= in_buffer + in_buffer_size) {
        for (len = 0, i = 0; i < 4 && (ptr <= in_buffer + in_buffer_size); i++) {
            v = 0;
            while ((ptr <= in_buffer + in_buffer_size) && v == 0) {
                v = (unsigned char)ptr[0];
                ptr++;
                v = (unsigned char)((v < 43 || v > 122) ? 0 : cd64[v - 43]);
                if (v) {
                    v = (unsigned char)((v == '$') ? 0 : v - 61);
                }
            }
            if (ptr <= in_buffer + in_buffer_size) {
                len++;
                if (v) {
                    in[i] = (unsigned char)(v - 1);
                }
            } else {
                in[i] = 0;
            }
        }
        if (len) {
            decodeblock(in, out);
            for (i = 0; i < len - 1; i++) {
                out_ptr[0] = out[i];
                out_ptr++;
            }
        }
    }
    return (long)out_ptr - (long)out_buffer;
}

/*
** returnable errors
**
** Error codes returned to the operating system.
**
*/
#define B64_SYNTAX_ERROR        1
#define B64_FILE_ERROR          2
#define B64_FILE_IO_ERROR       3
#define B64_ERROR_OUT_CLOSE     4
#define B64_LINE_SIZE_TO_MIN    5

/*
** b64_message
**
** Gather text messages in one place.
**
*/
char *b64_message(int errcode) {
#define B64_MAX_MESSAGES    6
    char *msgs[B64_MAX_MESSAGES] = {
        "b64:000:Invalid Message Code.",
        "b64:001:Syntax Error -- check help for usage.",
        "b64:002:File Error Opening/Creating Files.",
        "b64:003:File I/O Error -- Note: output file not removed.",
        "b64:004:Error on output file close.",
        "b64:004:linesize set to minimum."
    };
    char *msg = msgs[0];

    if ((errcode > 0) && (errcode < B64_MAX_MESSAGES)) {
        msg = msgs[errcode];
    }

    return msg;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_mime_encode CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'mime_encode' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Specific variables
    char   *szParam0;
    NUMBER nParam0Length;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &nParam0Length);
    if (TYPE != VARIABLE_STRING)
        return (void *)"mime_encode: parameter 1 should be of STATIC STRING type";

    if ((int)nParam0Length <= 0) {
        RETURN_STRING("");
        return 0;
    }

    // function call
    char *out_buffer = new char[(int)nParam0Length * 2 /*4/3*/ + 0xFF];
    long size        = encode(szParam0, (int)nParam0Length, out_buffer);

    SetVariable(RESULT, VARIABLE_STRING, out_buffer, size);
    delete[] out_buffer;
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_mime_decode CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": 'mime_decode' parameters error. This fuction takes one parameter.";

    // General variables
    NUMBER  NUMBER_DUMMY;
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Specific variables
    char   *szParam0;
    NUMBER nParam0Length;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szParam0, &nParam0Length);
    if (TYPE != VARIABLE_STRING)
        return (void *)"mime_decode: parameter 1 should be of STATIC STRING type";

    if ((int)nParam0Length <= 0) {
        RETURN_STRING("");
        return 0;
    }

    // function call
    char *out_buffer = new char[(int)nParam0Length];
    long size        = decode(szParam0, (int)nParam0Length, out_buffer);

    //SetVariable(RESULT,VARIABLE_NUMBER,"",size);
    SetVariable(RESULT, VARIABLE_STRING, out_buffer, size);
    delete[] out_buffer;
    return 0;
}
//-----------------------------------------------------------------------------------
