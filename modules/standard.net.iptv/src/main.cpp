//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
    #include <arpa/inet.h>
#endif

#define MAX_PACKET_LOOK_AHEAD   20

static const unsigned int crc_table[256] = {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
        0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
        0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
        0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
        0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
        0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
        0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
        0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
        0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
        0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
        0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
        0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
        0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
        0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
        0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
        0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
        0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
        0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
        0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
        0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
        0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
        0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
        0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
        0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
        0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
        0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
        0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
        0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
        0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
        0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
        0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
        0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
        0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4};

unsigned int fast_crc32(const char *data, int len) {
        register int i;
        unsigned int crc = 0xffffffff;

        for (i=0; i<len; i++)
                crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *data++) & 0xff];

        return crc;
}

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(M2TSTable, 4, 7)
    T_NUMBER("M2TSTable", 0)
    T_NUMBER("M2TSTable", 1)
    T_NUMBER("M2TSTable", 2)
    int network_id = 1;
    int pcr_id = -1;
    int version = 0;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER("M2TSTable", 4)
        network_id = PARAM_INT(4);
        if (network_id < 0)
            network_id = 1;
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER("M2TSTable", 5)
        pcr_id = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER("M2TSTable", 6)
        version = PARAM_INT(6);
        if (version < 0)
            version = 0;
        if (version > 32)
            version %= 32;
    }
    unsigned char buffer[2048];
    int pid = PARAM_INT(0);
    unsigned char table_id = (unsigned char)PARAM_INT(1);
    // table type
    buffer[0] = table_id;
    if (pid == 0) {
        T_ARRAY("M2TSTable", 3)

        buffer[1] = 0xB0; // 0b10110000
        // buffer[2] = length ( + 2 bit buffer[1])
        buffer[2] = 0;
        unsigned short extension = (unsigned short)PARAM_INT(2);
        buffer[3] = extension / 0x100;
        buffer[4] = extension % 0x100;
        buffer[5] = 0xC0 | (version << 1) | 1;
        // section number
        buffer[6] = 0;
        // last section number
        buffer[7] = 0;

        buffer[8] = 0xE0;
        if (pcr_id < 0)
                pcr_id = 0x1FFF;

        buffer[8] |= pcr_id / 0x100;
        buffer[9] = pcr_id % 0x100;
        buffer[10] = 0xF0;
        buffer[11] = 0x00;
        
        int pos = 12;
        int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(3));
        for (int i = 0; i < count; i++) {
            void *newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(3), i, &newpData);
            if (newpData) {
                char    *str;
                INTEGER type;
                NUMBER  nr;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &str, &nr);
                if (type == VARIABLE_ARRAY) {
                    int stream_id = -1;
                    int stream_type = -1;
                    char *language = 0;
                    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, newpData, "pid", &type, &str, &nr);
                    if (type == VARIABLE_NUMBER)
                        stream_id = (INTEGER)nr;
                    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, newpData, "type", &type, &str, &nr);
                    if (type == VARIABLE_NUMBER)
                        stream_type = (INTEGER)nr;

                    if ((stream_id > 0) && (stream_type > 0)) {
                        unsigned char *esinfo = 0;
                        int esinfo_len = 0;
                        int lang_len = 0;

                        Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, newpData, "esinfo", &type, &str, &nr);
                        if ((type == VARIABLE_STRING) && (str) && (nr) && (nr < 0xFF)) {
                            esinfo = (unsigned char *)str;
                            esinfo_len = (int)nr;
                        } else {
                            Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, newpData, "language", &type, &str, &nr);
                            if ((type == VARIABLE_STRING) && (str) && (str[0]))
                                language = str;
                            else
                                language = 0;

                            if (language) {
                                lang_len = strlen(language);
                                if (lang_len > 10)
                                    lang_len = 10;
                            }
                        }

                        buffer[pos] = stream_type;
                        pos++;
                        buffer[pos] = 0xE0;
                        buffer[pos] |= stream_id / 0x100;
                        pos++;
                        buffer[pos] = stream_id % 0x100;
                        pos++;

                        if (esinfo) {
                            buffer[pos] = 0xF0 | (esinfo_len / 0x100);
                            pos++;
                            buffer[pos] = esinfo_len % 0x100;
                            pos++;
                            memcpy(buffer + pos, esinfo, esinfo_len);
                            pos += esinfo_len;
                        } else
                        if (lang_len) {
                            int llen = lang_len + 3;
                            buffer[pos] = 0xF0 | (llen / 0x100);
                            pos++;
                            buffer[pos] = llen % 0x100;
                            pos++;
                            buffer[pos] = 0x0A;
                            pos++;
                            buffer[pos] = lang_len + 1;
                            pos++;
                            memcpy(buffer + pos, language, lang_len);
                            pos += lang_len;
                            buffer[pos] = 0x00;
                            pos++;
                        } else {
                            buffer[pos] = 0xF0;
                            pos++;
                            buffer[pos] = 0x00;
                            pos++;
                        }
                    }
                }
            }
            if (pos >= 1021)
                break;
        }
        int len_ref = pos + 1;
        buffer[1] |= len_ref  / 0x100;
        buffer[2] = len_ref % 0x100;
        unsigned int crc = fast_crc32((char *)buffer, pos);
        crc = htonl(crc);
        memcpy(buffer + pos, &crc, 4);
        pos += 4;

        RETURN_BUFFER((char *)buffer, pos);
        return 0;
        // E1 00 F0 00 02
        // E1 00 F0 00 03
        // E1 01 F0 06 0A 04 rum 00
    } else
    if (pid == 0x11) {
        T_ARRAY("M2TSTable", 3)

        buffer[1] = 0xF0;
        // buffer[2] = length ( + 2 bit buffer[1])
        buffer[2] = 0;
        unsigned short extension = (unsigned short)PARAM_INT(2);
        buffer[3] = extension / 0x100;
        buffer[4] = extension % 0x100;

        buffer[5] = 0xC0 | (version << 1) | 1;
        // buffer[5] = 0xC1;
        // section number
        buffer[6] = 0;
        // last section number
        buffer[7] = 0;

        buffer[8] = 0xFF;//network_id / 0x100;
        buffer[9] = network_id % 0x100;
        buffer[10] = 0xFF;
        // service id 
        int pos = 11;

        int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(3));
        for (int i = 0; i < count; i++) {
            void *newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(3), i, &newpData);
            if (newpData) {
                char    *str;
                INTEGER type;
                NUMBER  nr;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &str, &nr);
                if (type == VARIABLE_ARRAY) {
                    int service_id = 1;

                    Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, newpData, "id", &type, &str, &nr);
                    if (type == VARIABLE_NUMBER)
                        service_id = (INTEGER)nr;

                    if (service_id > 0) {
                        char *network = "Concept";
                        char *service = "Service01";
                        int service_type = 1;

                        Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, newpData, "network", &type, &str, &nr);
                        if ((type == VARIABLE_STRING) && (str) && (str[0]))
                            network = str;

                        Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, newpData, "service", &type, &str, &nr);
                        if ((type == VARIABLE_STRING) && (str) && (str[0]))
                            service = str;

                        Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, newpData, "type", &type, &str, &nr);
                        if (type == VARIABLE_NUMBER) {
                            service_type = (INTEGER)nr;
                            if (service_type <= 0)
                                service_type = 1;
                        }

                        buffer[pos] = service_id / 0x100;
                        pos++;
                        buffer[pos] = service_id % 0x100;
                        pos++;
                        buffer[pos] = 0xFC;
                        pos++;

                        int service_len = strlen(service);
                        if (service_len > 0xFF)
                            service_len = 0xFF;
                        int network_len = strlen(network);
                        if (network_len > 0xFF)
                            network_len = 0xFF;

                        // loop len 4 bits
                        buffer[pos] = 0x80;
                        pos++;
                        // loop len 8 bits
                        buffer[pos] = network_len + service_len + 5;
                        pos++;

                        buffer[pos] = 0x48;
                        pos++;

                        // descriptor length
                        buffer[pos] = network_len + service_len + 3;
                        pos++;

                        buffer[pos] = service_type;
                        pos++;
                        buffer[pos] = network_len;
                        pos++;
                        memcpy(buffer + pos, network, network_len);
                        pos += network_len;
                        buffer[pos] = service_len;
                        pos++;
                        memcpy(buffer + pos, service, service_len);
                        pos += service_len;
                    }
                }
            }
            if (pos >= 1021)
                break;
        }
        int len_ref = pos + 1;
        buffer[1] |= len_ref  / 0x100;
        buffer[2] = len_ref % 0x100;

        unsigned long crc = fast_crc32((char *)buffer, pos);
        crc = htonl(crc);
        memcpy(buffer + pos, &crc, 4);
        pos += 4;

        RETURN_BUFFER((char *)buffer, pos);
        return 0;
    } else {
        T_NUMBER("M2TSTable", 3)
        int pcr_pid = PARAM_INT(3);
        if ((pcr_pid >= 0) && (pcr_pid <= 0xFFFF)) {
            // PMT
            buffer[1] = 0xB0; // 0b10110000
            // buffer[2] = length ( + 2 bit buffer[1])
            buffer[2] = 0x0D;
            unsigned short extension = (unsigned short)PARAM_INT(2);
            buffer[3] = extension / 0x100;
            buffer[4] = extension % 0x100;
            // buffer[5] = 0xC1;
            buffer[5] = 0xC0 | (version << 1) | 1;
            // section number
            buffer[6] = 0;
            // last section number
            buffer[7] = 0;

            buffer[8] = 0x00;

            buffer[8] |= pcr_pid / 0x100;
            buffer[9] = pcr_pid % 0x100;
            buffer[10] = 0xF0;
            buffer[11] = 0;

            unsigned long crc = fast_crc32((char *)buffer, 12);
            crc = htonl(crc);
            memcpy(buffer + 12, &crc, 4);

            RETURN_BUFFER((char *)buffer, 16);
            return 0;
        }
    }

    RETURN_STRING("");
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(M2TSPacket, 3, 5)
    T_NUMBER("M2TSPacket", 0)
    T_NUMBER("M2TSPacket", 1)
    T_STRING("M2TSPacket", 2)
    int priority = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER("M2TSPacket", 3)
        priority = PARAM_INT(3);
    }
    INTEGER pid = PARAM_INT(0);
    INTEGER counter = PARAM_INT(1);
    if ((pid >= 0) && (pid <= 0x1FFF)) {
        unsigned char packet[188];
        packet[0] = 'G';
        // set pusi flag
        if (priority == -1)
            packet[1] = 0x00;
        else
        if (priority)
            packet[1] = 0x60;
        else
            packet[1] = 0x40;

        unsigned char pid_hi = pid / 0x100;
        unsigned char pid_lo = pid % 0x100;

        packet[1] |= pid_hi;
        packet[2] = pid_lo;

        // payload only
        packet[3] = 0x10;

        if (counter < 0)
            counter = 0;
        packet[3] |= counter % 16;

        // padding
        packet[4] = 0;

        int len = PARAM_LEN(2);
        if (len < 0)
            len = 0;
        if (len > 183) {
            if (PARAMETERS_COUNT > 4) {
                SET_BUFFER(4, PARAM(2) + 183, len - 183);
            }
            len = 183;
        } else {
            memset(packet + 5 + len, 0xFF, 183 - len);
            if (PARAMETERS_COUNT > 4) {
                SET_STRING(4, "");
            }
        }

        memcpy(packet + 5, PARAM(2), len);
        RETURN_BUFFER((const char *)packet, 188);
    } else {
        if (PARAMETERS_COUNT > 4) {
            SET_STRING(4, "");
        }
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(M2TSChpid, 2)
    T_STRING("M2TSChpid", 0)
    T_NUMBER("M2TSChpid", 1)
    int len = PARAM_LEN(0);
    INTEGER pid = PARAM_INT(1);
    unsigned char *str = (unsigned char *)PARAM(0);
    int blocks = 0;
    if ((len >= 188) && (pid >= 0) && (pid <= 0x1FFF)) {
        int index = 0;
        unsigned char pid_hi = pid / 0x100;
        pid_hi |= 0xE0;
        unsigned char pid_lo = pid % 0x100;
        while (len >= 188) {
            if (str[index] != 'G')
                break;
            str[index + 1] &= pid_hi;
            str[index + 2] = pid_lo;
            blocks++;
            index += 188;
            len -= 188;
        }
    }
    RETURN_NUMBER(blocks);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(M2TSPTS, 1)
    T_STRING("M2TSPTS", 0)
    int len = PARAM_LEN(0);
    unsigned char *str = (unsigned char *)PARAM(0);
    NUMBER pts = -1;
    if (len >= 188) {
        if (str[0] == 'G') {
            unsigned short afc = (str[3] & 0x30) >> 4;
            int start = 4;
            if (afc & 0x01) {
                if (afc == 0x03) {
                    unsigned char size = str[start];
                    start++;
                    start += size;
                    if (start >= len) {
                        RETURN_NUMBER(0);
                        return 0;
                    }
                }
                if ((start + 12 < len) && (str[start] == 0x00) && (str[start + 1] == 0x00) && (str[start + 2] == 0x01)) {
                    // 3 + 1 + 1 + 1 + 1
                    // PES start code prefix
                    start += 3;
                    unsigned char stream_id = str[start];
                    if ((stream_id >= 0xBD) && (stream_id <= 0xEF)) {
                        start++;
                        unsigned short pes_packet_length = ntohs((*(unsigned short *)&str[start]));
                        start += 2;
                        if (pes_packet_length > 8) {
                            start++;
                            unsigned char dts_flags = str[start] >> 6;
                            if (dts_flags == 0x02) {
                                start ++;
                                unsigned char pes_header_length = str[start];
                                start ++;
                                if (pes_header_length >= 5) {
                                    //unsigned char clock_1 
                                    unsigned short pts_32_30 = (str[start] & 0x7) >> 1;
                                    start ++;
                                    unsigned char pts_29_15_hi = *(unsigned short *)&str[start];
                                    start ++;
                                    unsigned char pts_29_15_lo = *(unsigned short *)&str[start];
                                    pts_29_15_lo >>= 1;
                                    start ++;
                                    unsigned short pts_14_0_hi = *(unsigned short *)&str[start];
                                    start ++;
                                    unsigned short pts_14_0_lo = *(unsigned short *)&str[start];
                                    pts_14_0_lo >>= 1;
                                    start ++;
                                    if (stream_id == 0xC0) {
                                        unsigned char pts_ref[5];
                                        pts_ref[4]  = pts_14_0_lo; // 7 bit
                                        pts_ref[4] |= pts_14_0_hi << 7; // 1 bit from hi

                                        pts_ref[3]  = pts_14_0_hi >> 1; // 7 bit from hi
                                        pts_ref[3] |= pts_29_15_lo << 7; // 1 bit from lo

                                        pts_ref[2]  = pts_29_15_lo >> 2; // 6 bit from lo
                                        pts_ref[2] |= pts_29_15_hi << 6; // 2 bit from hi
                                        
                                        pts_ref[1]  = pts_29_15_hi >> 2; // 6 bit from hi
                                        pts_ref[1] |= (pts_32_30 & 0x06) << 6; // 2 bit
                                        pts_ref[0]  = pts_32_30 >> 2; // 1 bit

                                        unsigned int tstamp = ntohl(*(unsigned int *)&pts_ref[1]);
                                        pts = pts_ref[0] * 0x1000000LL + tstamp;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    RETURN_NUMBER(pts);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(M2TSChcont, 2)
    T_STRING("M2TSChpid", 0)
    T_NUMBER("M2TSChpid", 1)
    int len = PARAM_LEN(0);
    INTEGER cont = PARAM_INT(1);
    unsigned char *str = (unsigned char *)PARAM(0);
    int blocks = 0;
    if ((len >= 188) && (cont >= 0) && (cont <= 15)) {
        if (str[0] == 'G') {
            blocks = 1;
            str[3] = (str[3] & 0xF0) | cont;
        }
    }
    RETURN_NUMBER(blocks);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(M2TSPeek, 1, 4)
    T_STRING("M2TSPeek", 0)
    INTEGER offset = 0;
    INTEGER len = (INTEGER)PARAM_LEN(0);
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER("M2TSPeek", 1);
        offset = (INTEGER)PARAM(1);
        if (offset > 0)
            len -= offset;
        else
            offset = 0;
        if (PARAMETERS_COUNT > 2) {
            SET_STRING(2, "");
        }
        if (PARAMETERS_COUNT > 3) {
            SET_NUMBER(3, 0);
        }
    }
    const unsigned char *str = (unsigned char *)PARAM(0) + offset;
    if (len >= 188) {
        if (str[0] == 'G') {
            int index = 0;
            int buf_len = 0;
            unsigned short prec_pid = 0;
            unsigned short pusi  = 0;
            unsigned short pid = 0;
            const unsigned char *start = str;
            do {
                pusi = str[1] & 0x40;
                unsigned short pid_hi = str[1] & 0x1F;
                pid = pid_hi * 0x100 + str[2];
                // unsigned short afc = (str[3] & 0x30) >> 4;

                if ((buf_len) && (prec_pid != pid)) {
                    pid = prec_pid;
                    break;
                }
                if (pusi) {
                    if (prec_pid > 0)
                        pid = prec_pid;
                    else
                        buf_len = 188;
                    break;
                }

                buf_len += 188;
                str += 188;
                len -= 188;
                prec_pid = pid;
            } while ((str[0] == 'G') && (len >= 188));
            if ((PARAMETERS_COUNT > 2)) {
                SET_BUFFER(2, (const char *)start, buf_len);
                if ((PARAMETERS_COUNT > 3) && (pusi)) {
                    SET_NUMBER(3, 1);
                }
            }
            RETURN_NUMBER(pid);
        } else {
            RETURN_NUMBER(-1);
        }
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(M2TSDemux, 1, 7)
    T_STRING("M2TSDemux", 0)
    INTEGER offset = 0;
    INTEGER len = (INTEGER)PARAM_LEN(0);
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER("M2TSDemux", 1);
        offset = (INTEGER)PARAM(1);
        if (offset > 0)
            len -= offset;
        else
            offset = 0;
    }
    if (PARAMETERS_COUNT > 2) {
        SET_NUMBER(2, -1);
    }
    if (PARAMETERS_COUNT > 3) {
        SET_STRING(3, "");
    }
    if (PARAMETERS_COUNT > 4) {
        SET_NUMBER(4, 0);
    }
    if (PARAMETERS_COUNT > 5) {
        SET_NUMBER(5, -1);
    }
    if (PARAMETERS_COUNT > 6) {
        SET_NUMBER(6, 0);
    }
    const unsigned char *str = (unsigned char *)PARAM(0) + offset;
    INTEGER start = -1;
    INTEGER buf_start = 0;
    int packet_size = 188;
    int data_size = packet_size;
    unsigned short err = 0;
    if (len >= packet_size) {
        for (INTEGER i = 0; i < len; i++) {
            if ((str[i] == 'G') && (!(str[i + 1] & 0x80))) {
                start = i;
                break;
            }
        }
        if (start < 0) {
            RETURN_NUMBER(0);
            return 0;
        }
        buf_start = start;
        if (len - start >= packet_size) {
            start++;
            unsigned short tei = str[start] & 0x80;
            unsigned short pusi = str[start] & 0x40;
            unsigned short priority = str[start] & 0x20;
            unsigned short pid_hi = str[start] & 0x1F;
            start++;
            unsigned short pid = pid_hi * 0x100 + str[start];
            start++;
            unsigned short tsc = (str[start] & 0xC0) >> 6;
            unsigned short afc = (str[start] & 0x30) >> 4;
            unsigned short cont_counter = str[start] & 0x0F;
            unsigned char pointer_field = 0;
            unsigned char pointer_padding = 0;
            start++;
            if ((afc) && (len - start < 1)) {
                RETURN_NUMBER(0);
                return 0;
            }
            if (afc & 0x02) {
                // adaptation field
                unsigned char size = str[start];
                start++;
                if ((PARAMETERS_COUNT > 4) && (str[start] & 0x40))
                    SET_NUMBER(4, (NUMBER)1);
                start += size;
                if (start >= len) {
                    RETURN_NUMBER(0);
                    return 0;
                }
            }
            if (PARAMETERS_COUNT > 3) {
                SET_BUFFER(3, (const char *)&str[buf_start], packet_size);
            }
            if (PARAMETERS_COUNT > 5) {
                SET_NUMBER(5, (NUMBER)cont_counter);
            }
            int has_info = 1;
            if (PARAMETERS_COUNT > 6) {
                if (pusi) {
                    pointer_field = str[start];
                    start++;
                    for (unsigned int i = 0; i < pointer_field; i++) {
                        pointer_padding = str[start];
                        start++;
                        if (start >= len) {
                            err = 1;
                            break;
                        }
                    }
                }
                unsigned short section_len = 0;
                unsigned short table_id = 0;
                if (!err) {
                    if (pusi) {
                        INTEGER table_start = start;
                        // table header (32bit)
                        table_id = str[start];
                        if ((table_id == 2) || (table_id == 0) || (table_id == 0x42)) {
                            CREATE_ARRAY(PARAMETER(6));
                            unsigned short extension = 0;
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "table_id", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)table_id);
                            start ++;

                            section_len = (str[start] & 0x03) * 0x100;
                            start ++;

                            section_len += str[start];
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "section_len", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)section_len);
                            start ++;
                            if ((start + section_len < len) && (section_len >= 8) && (section_len <= 1021)) {
                                extension = ntohs((*(unsigned short *)&str[start]));
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "extension_id", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)extension);
                                start += 2;
                                // various flags
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "flags", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)str[start]);
                                int version = (str[start] & 0x3E) >> 1;
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "version", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)version);
                                start ++;
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "section_number", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)str[start]);
                                start++;
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "last_section_number", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)str[start]);
                                start++;

                                int payload_len = section_len - 9;

                                int header_size = start - buf_start;
                                int remaining = 188 - header_size;
                                int extra_packets = 0;
                                if (remaining < 0)
                                    remaining = 0;

                                unsigned char *ref_str = (unsigned char *)str;
                                // 4 bytes crc
                                if ((remaining) && (payload_len > remaining - 4)) {
                                    extra_packets = payload_len / remaining;
                                    if (payload_len % remaining)
                                        extra_packets ++;
                                    if ((start + payload_len) < (len - extra_packets * packet_size)) {
                                        ref_str = (unsigned char *)malloc(start + (extra_packets + 1) * packet_size);
                                        memcpy(ref_str, str, packet_size);
                                        int ref_offset = packet_size;
                                        int str_offset = packet_size;

                                        header_size = 4;
                                        int rem2 = packet_size - header_size;
                                        int packets_remaining = extra_packets;
                                        int skips = 0;
                                        for (int i = 0; i < extra_packets; i++) {
                                            const unsigned char *str_temp = str + str_offset;
                                            if (str_temp[0] != 'G') {
                                                // invalid packet, discard
                                                memset(ref_str + ref_offset, 0, packet_size * packets_remaining);
                                                break;
                                            }

                                            pusi = str_temp[1] & 0x40;
                                            unsigned short temp_pid_hi = str_temp[1] & 0x1F;
                                            unsigned short temp_pid = temp_pid_hi * 0x100 + str_temp[2];
                                            if (temp_pid != pid) {
                                                str_offset += packet_size;
                                                extra_packets ++;
                                                if (((start + payload_len) >= (len - extra_packets * packet_size)) || (skips > MAX_PACKET_LOOK_AHEAD)) {
                                                    memset(ref_str + ref_offset, 0, packet_size * packets_remaining);
                                                    break;
                                                }
                                                skips++;
                                                continue;
                                            }

                                            memcpy(ref_str + ref_offset, str + str_offset + header_size, rem2);
                                            ref_offset += rem2;
                                            str_offset += packet_size;
                                            data_size += packet_size;
                                            packets_remaining--;
                                        }
                                    } else {
                                        extra_packets = 0;
                                        payload_len = remaining;
                                    }
                                }
                                int payload_start = start;
                                int payload_offset = payload_len;

                                switch (pid) {
                                    case 0:
                                        // Program Association Table
                                        if (table_id == 0) {
                                            unsigned short prog_no = ntohs((*(unsigned short *)&ref_str[start]));
                                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "channel", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)prog_no);
                                            start += 2;
                                        }
                                        break;
                                    case 1:
                                        // Conditional Access tables
                                        break;
                                    case 2:
                                        // Transport Stream Description tables
                                        break;
                                    case 17:
                                        // Service Description Table, Bouquet Association table
                                        if ((table_id == 0x42) || (table_id == 0x46)) {
                                            start += 3;
                                            int remaining_len = payload_len;
                                            void *newpData = 0;
                                            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, PARAMETER(6), "channels", &newpData);
                                            CREATE_ARRAY(newpData);

                                            INTEGER i = 0;
                                            while (remaining_len > 10) {
                                                void *newpData2 = 0;
                                                Invoke(INVOKE_ARRAY_VARIABLE, newpData, i, &newpData2);
                                                CREATE_ARRAY(newpData2);

                                                unsigned short service_id = ntohs((*(unsigned short *)&ref_str[start]));
                                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData2, "service_id", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)service_id);


                                                start += 2;
                                                start++;
                                                unsigned short loop_len = (ref_str[start] & 0x0F) * 0x100;
                                                start++;
                                                loop_len += ref_str[start];
                                                start++;
                                                int ref_loop_len = loop_len;
                                                int ref_start = start;
                                                while (ref_loop_len >= 6) {
                                                    unsigned char descriptor_len = ref_str[ref_start + 1];
                                                    if (ref_str[ref_start] == 0x48) {
                                                        unsigned char es_len = ref_str[ref_start + 3];
                                                        unsigned char es_len2 = 0;
                                                        if (es_len > ref_loop_len - 5) {
                                                            es_len = ref_loop_len - 5;
                                                        } else {
                                                            es_len2 = ref_str[ref_start + 4 + es_len ];
                                                            if (es_len2 > ref_loop_len - es_len - 5)
                                                                break;
                                                        }
                                                        switch (ref_str[ref_start]) {
                                                            case 0x48:
                                                                if (es_len > 0)
                                                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData2, "network", (INTEGER)VARIABLE_STRING, (char *)&ref_str[ref_start + 4], (NUMBER)es_len);
                                                                if (es_len2)
                                                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData2, "service", (INTEGER)VARIABLE_STRING, (char *)&ref_str[ref_start + 5 + es_len], (NUMBER)es_len2);
                                                                break;
                                                        }
                                                    }
                                                    ref_loop_len -= descriptor_len + 2;
                                                    ref_start += descriptor_len + 2;
                                                }
                                                start += loop_len;

                                                remaining_len -= 4 + loop_len;
                                                i++;
                                            }
                                        }
                                        // if (payload_len > 0)
                                        //    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "table_data", (INTEGER)VARIABLE_STRING, (char *)&ref_str[payload_start], (NUMBER)payload_len);
                                        break;
                                    default:
                                        if ((pid >= 32) && (pid <= 8182) && (start + 5 < len) && (table_id == 2)) {
                                            int remaining_len = payload_len;
                                            // pmt, pmt-e, mgt or mgt-e
                                            unsigned short pcr_pid = (ref_str[start] & 0x1F) * 0x100;
                                            start++;
                                            pcr_pid += ref_str[start];
                                            start ++;

                                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "pcr", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pcr_pid);
                                            unsigned short program_info_len = (ref_str[start] & 0x03) * 0x100;
                                            start++;
                                            program_info_len += ref_str[start];
                                            start++;
                                            // skip program info
                                            if (program_info_len)
                                                start += program_info_len;
                                        
                                            remaining_len -= 4 + program_info_len;
                                            has_info = 2;
                                            if ((remaining_len >= 5) && (start + remaining_len < len)) {
                                                void *newpData = 0;
                                                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, PARAMETER(6), "descriptors", &newpData);
                                                CREATE_ARRAY(newpData);
                                                INTEGER i = 0;
                                                while (remaining_len >= 5) {
                                                    void *newpData2 = 0;
                                                    Invoke(INVOKE_ARRAY_VARIABLE, newpData, i, &newpData2);
                                                    CREATE_ARRAY(newpData2);
                                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData2, "type", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ref_str[start]);
                                                    start++;
                                                    unsigned short epid = (ref_str[start] & 0x1F) * 0x100;
                                                    start++;
                                                    epid += ref_str[start];
                                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData2, "pid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)epid);
                                                    start++;

                                                    unsigned short es_info_len = (ref_str[start] & 0x03) * 0x100;
                                                    start++;
                                                    es_info_len += ref_str[start];
                                                    start++;
                                                    if (start + es_info_len > len)
                                                        break;
                                                    if (es_info_len > 2) {
                                                        unsigned int es_len = ref_str[1];
                                                        if (es_len > es_info_len - 2)
                                                            es_len = es_info_len - 2;
                                                        int es_offset = 0;
                                                        int do_break = 0;
                                                        while (es_len > 0) {
                                                            switch (ref_str[start + es_offset]) {
                                                                case 0x09:
                                                                case 0x52:
                                                                case 0x81:
                                                                    es_len -= ref_str[start + es_offset + 1] + 2;
                                                                    if (es_len < 0)
                                                                        es_len = 0;
                                                                    es_offset += ref_str[start + es_offset + 1] + 2;
                                                                    if (es_offset > es_len)
                                                                        es_offset = 0;
                                                                    break;
                                                                case 0x0A:
                                                                case 0x56:
                                                                case 0x59:
                                                                    if (es_len > 3)
                                                                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData2, "language", (INTEGER)VARIABLE_STRING, (char *)&ref_str[start + 2 + es_offset], (NUMBER)3);
                                                                    else
                                                                    if (es_len > 0)
                                                                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData2, "language", (INTEGER)VARIABLE_STRING, (char *)&ref_str[start + 2 + es_offset], (NUMBER)es_len);
                                                                    do_break = 1;
                                                                    break;
                                                                default:
                                                                    do_break = 1;
                                                                    break;
                                                            }
                                                            if (do_break)
                                                                break;
                                                        }
                                                    }
                                                    if (es_info_len)
                                                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData2, "esinfo", (INTEGER)VARIABLE_STRING, (char *)&ref_str[start], (NUMBER)es_info_len);
                                                    start += es_info_len;
                                                    i++;
                                                    remaining_len -= 5 + es_info_len;
                                                }
                                            }
                                            payload_offset = remaining_len;
                                        }
                                }
                                if (payload_len > 0) {
                                    start += payload_offset;
                                    if (start <= len - 4) {
                                        unsigned long crc = ntohl((*(unsigned int *)&ref_str[start]));
                                        unsigned long crc2 = fast_crc32((char *)&ref_str[table_start], start - table_start);
                                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "valid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(crc == crc2));
                                        start += 4;
                                        if (crc != crc2)
                                            err = 4;
                                    } else
                                        err = 3;
                                }
                                if (extra_packets)
                                    free(ref_str);
                            } else
                                err = 2;

                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "cont", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)cont_counter);
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "pid", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pid);
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "afc", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)afc);
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "tsc", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)tsc);
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "tei", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)tei);
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "pusi", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pusi);
                            if (pusi) {
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "pointer", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pointer_field);
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "padding", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)pointer_padding);
                            }
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "priority", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)priority);
                            if (pid == 8191)
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "null", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)1);
                            if (err)
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(6), "parse_error", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)err);
                        }
                    }
                }
            }
            if ((err) && (err != 4) && (err != 2)) {
                start = offset + buf_start + 1;
                has_info = -err;
                if (PARAMETERS_COUNT > 3) {
                    SET_STRING(3, "");
                }
            } else
                start = offset + packet_size;
            if (PARAMETERS_COUNT > 1) {
                SET_NUMBER(1, start);
                if (PARAMETERS_COUNT > 2) {
                    SET_NUMBER(2, pid);
                }
            }
            RETURN_NUMBER(has_info);
            return 0;
        }
    }
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(M2TSSyncOffset, 2)
    T_STRING("M2TSSyncOffset", 0)
    T_NUMBER("M2TSSyncOffset", 1)
    INTEGER offset = PARAM_INT(1);
    if (offset >= 0) {
        INTEGER limit = PARAM_LEN(0) - 188 * 3;
        const unsigned char *str = (unsigned char *)PARAM(0);
        for (int i  = offset; i < limit; i++) {
            if ((str[i] == 'G') && (str[i + 188] == 'G') && (str[i + 188 * 2] == 'G')) {
                offset = i;
                break;
            }
        }
    }
    RETURN_NUMBER(offset);
END_IMPL
//=====================================================================================//
