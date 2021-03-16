//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
#include <map>
#include <stdlib.h>
#ifdef _WIN32
 #include <winsock2.h>
#else
 #include <netinet/in.h>
#endif
#define NS_IN6ADDRSZ    16
#define NS_INT16SZ      2
struct NetFlow5Header {
    unsigned short version;
    unsigned short flows;
    unsigned int   uptime_ms;
    unsigned int   time_sec;
    unsigned int   time_nanosec;
    unsigned int   flow_sequence;
    unsigned char  engine_type;
    unsigned char  engine_id;
    unsigned short sampling_interval;
};

struct NetFlow5Record {
    unsigned int   srcaddr;
    unsigned int   dstaddr;
    unsigned int   nexthop;
    unsigned short input;
    unsigned short output;
    unsigned int   dPkts;
    unsigned int   dOctets;
    unsigned int   first;
    unsigned int   last;
    unsigned short srcport;
    unsigned short dstport;
    unsigned char  pad1;
    unsigned char  tcp_flags;
    unsigned char  prot;
    unsigned char  tos;
    unsigned short src_as;
    unsigned short dst_as;
    unsigned char  src_mask;
    unsigned char  dst_mask;
    unsigned short pad2;
};

struct NetFlow9Header {
    unsigned short version;
    unsigned short flows;
    unsigned int   uptime_ms;
    unsigned int   time_sec;
    unsigned int   package_sequence;
    unsigned int   source_id;
};

struct NetFlow9Template {
    unsigned short template_id;
    unsigned short field_count;
};

struct NetFlow9Record {
    unsigned short flowset_id;
    unsigned short length;
};

struct CachedFields {
    unsigned short field_type;
    unsigned short field_length;
};

struct CachedTemplate {
    unsigned short template_id;
    unsigned short fields_count;
    CachedFields   *fields;
};


static const char *flowkeys[] = {
    0,                       "IN_BYTES",                "IN_PKTS",                   "FLOWS",                   "PROTOCOL",              "SRC_TOS",                      "TCP_FLAGS",          "L4_SRC_PORT",
    "IPV4_SRC_ADDR",         "SRC_MASK",                "INPUT_SNMP",                "L4_DST_PORT",             "IPV4_DST_ADDR",         "DST_MASK",
    "OUTPUT_SNMP",           "IPV4_NEXT_HOP",           "SRC_AS",                    "DST_AS",                  "BGP_IPV4_NEXT_HOP",     "MUL_DST_PKTS",
    "MUL_DST_BYTES",         "LAST_SWITCHED",           "FIRST_SWITCHED",            "OUT_BYTES",               "OUT_PKTS",              "MIN_PKT_LNGTH",
    "MAX_PKT_LNGTH",         "IPV6_SRC_ADDR",           "IPV6_DST_ADDR",             "IPV6_SRC_MASK",           "IPV6_DST_MASK",         "IPV6_FLOW_LABEL",
    "ICMP_TYPE",             "MUL_IGMP_TYPE",           "SAMPLING_INTERVAL",         "SAMPLING_ALGORITHM",      "FLOW_ACTIVE_TIMEOUT",   "FLOW_INACTIVE_TIMEOUT",
    "ENGINE_TYPE",           "ENGINE_ID",               "TOTAL_BYTES_EXP",           "TOTAL_PKTS_EXP",          "TOTAL_FLOWS_EXP",       "*Vendor Proprietary*1",        "IPV4_SRC_PREFIX",
    "IPV4_DST_PREFIX",       "MPLS_TOP_LABEL_TYPE",     "MPLS_TOP_LABEL_IP_ADDR",    "FLOW_SAMPLER_ID",         "FLOW_SAMPLER_MODE",     "FLOW_SAMPLER_RANDOM_INTERVAL",
    "*Vendor Proprietary*2", "MIN_TTL",                 "MAX_TTL",                   "IPV4_IDENT",              "DST_TOS",               "IN_SRC_MAC",
    "OUT_DST_MAC",           "SRC_VLAN",                "DST_VLAN",                  "IP_PROTOCOL_VERSION",     "DIRECTION",             "IPV6_NEXT_HOP",
    "BPG_IPV6_NEXT_HOP",     "IPV6_OPTION_HEADERS",     "*Vendor Proprietary*3",     "*Vendor Proprietary*4",   "*Vendor Proprietary*5", "*Vendor Proprietary*6",
    "*Vendor Proprietary*7", "MPLS_LABEL_1",            "MPLS_LABEL_2",              "MPLS_LABEL_3",            "MPLS_LABEL_4",          "MPLS_LABEL_5",
    "MPLS_LABEL_6",          "MPLS_LABEL_7",            "MPLS_LABEL_8",              "MPLS_LABEL_9",            "MPLS_LABEL_10",         "IN_DST_MAC",
    "OUT_SRC_MAC",           "IF_NAME",                 "IF_DESC",                   "SAMPLER_NAME",            "IN_PERMANENT_BYTES",    "IN_PERMANENT_PKTS",
    "*Vendor Proprietary*8", "FRAGMENT_OFFSET",         "FORWARDING_STATUS",         "MPLS_PAL_RD",             "MPLS_PREFIX_LEN",       "SRC_TRAFFIC_INDEX",
    "DST_TRAFFIC_INDEX",     "APPLICATION_DESCRIPTION", "APPLICATION_TAG",           "APPLICATION_NAME",        "APPLICATION_NAME2",     "postipDiffServCodePoint",      "replication_factor",
    "DEPRECATED",            "DEPRECATED2",             "layer2packetSectionOffset", "layer2packetSectionSize", "layer2packetSectionData"
};

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
inline void IP_to_str(unsigned int ip, char *buf) {
    unsigned char bytes[4];

    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    sprintf(buf, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
}

//-----------------------------------------------------//
inline void MAC_to_str(char *in, char *result) {
    int idx = 0;

    for (int i = 0; i < 6; i++) {
        unsigned char first = in[i] / 0x10;
        unsigned char sec   = in[i] % 0x10;

        if (i)
            result[idx] = ':';
        idx++;

        if (first < 10)
            result[idx] = '0' + first;
        else
            result[idx] = 'a' + (first - 10);
        idx++;
        if (sec < 10)
            result[idx] = '0' + sec;
        else
            result[idx] = 'a' + (sec - 10);
        idx++;
    }
    result[idx] = 0;
}

//-----------------------------------------------------//
static const char *inet_ntop4(const unsigned char *src, char *dst, int size) {
    static const char fmt[] = "%u.%u.%u.%u";
    char tmp[sizeof("255.255.255.255")];

    if (sprintf(tmp, fmt, src[0], src[1], src[2], src[3]) >= size)
        return NULL;

    return strcpy(dst, tmp);
}

static const char *inet_ntop6(const unsigned char *src, char *dst, int size) {
    char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;

    struct {
        int base, len;
    }     best, cur;
    u_int words[NS_IN6ADDRSZ / NS_INT16SZ];
    int   i;

    memset(words, '\0', sizeof words);
    for (i = 0; i < NS_IN6ADDRSZ; i += 2)
        words[i / 2] = (src[i] << 8) | src[i + 1];
    best.base = -1;
    cur.base  = -1;
    best.len  = 0;
    cur.len   = 0;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
                cur.len++;
        } else {
            if (cur.base != -1) {
                if ((best.base == -1) || (cur.len > best.len))
                    best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if ((best.base == -1) || (cur.len > best.len))
            best = cur;
    }
    if ((best.base != -1) && (best.len < 2))
        best.base = -1;

    tp = tmp;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        if ((best.base != -1) && (i >= best.base) &&
            (i < (best.base + best.len))) {
            if (i == best.base)
                *tp++ = ':';
            continue;
        }
        if (i != 0)
            *tp++ = ':';
        if ((i == 6) && (best.base == 0) &&
            ((best.len == 6) || ((best.len == 5) && (words[5] == 0xffff)))) {
            if (!inet_ntop4(src + 12, tp, sizeof tmp - (tp - tmp)))
                return NULL;
            tp += strlen(tp);
            break;
        }
        tp += sprintf(tp, "%x", words[i]);
    }
    if ((best.base != -1) && ((best.base + best.len) == 8))
        *tp++ = ':';
    *tp++ = '\0';


    if ((int)(tp - tmp) > size)
        return NULL;

    return strcpy(dst, tmp);
}

//-----------------------------------------------------//
void FreeCTPL(CachedTemplate *ctpl) {
    if (!ctpl)
        return;

    if (ctpl->fields)
        free(ctpl->fields);
    ctpl->fields_count = 0;
    free(ctpl);
}

void FreeCHTPLHandle(std::map<unsigned short, CachedTemplate *> *map) {
    for (std::map<unsigned short, CachedTemplate *>::iterator it = map->begin(); it != map->end(); ++it) {
        FreeCTPL(it->second);
    }
    delete map;
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ParseNetflowPacket, 1, 2)
    T_STRING(ParseNetflowPacket, 0)

    std::map<unsigned short, CachedTemplate *> *handle = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(ParseNetflowPacket, 1)
        handle = (std::map<unsigned short, CachedTemplate *> *)(SYS_INT) PARAM(1);
    }

    CREATE_ARRAY(RESULT);
    char *buf = PARAM(0);
    char ipbuf[16];
    char ipv6buf[50];
    char macbuf[20];
    buf[15] = 0;
    unsigned short version = ntohs(*(unsigned short *)buf);
    if ((version == 5) && ((size_t)PARAM_LEN(0) >= sizeof(NetFlow5Header))) {
        NetFlow5Header header;
        NetFlow5Record record;
        memcpy(&header, buf, sizeof(NetFlow5Header));
        header.version      = ntohs(header.version);
        header.flows        = ntohs(header.flows);
        header.uptime_ms    = ntohl(header.uptime_ms);
        header.time_sec     = ntohl(header.time_sec);
        header.time_nanosec = ntohl(header.time_nanosec);

        void *newpData = 0;
        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"metadata", &newpData);
        if (newpData) {
            CREATE_ARRAY(newpData);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "version", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.version);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "flows", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.flows);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "uptime_ms", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.uptime_ms);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "time_sec", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.time_sec);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "time_nanosec", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.time_nanosec);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "engine_type", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.engine_type);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "engine_id", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.engine_id);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "sampling_interval", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.sampling_interval);
        }
        int len = PARAM_LEN(0) - sizeof(NetFlow5Header);
        buf += sizeof(NetFlow5Header);
        int flows = header.flows;
        if (len >= (int)(header.flows * sizeof(NetFlow5Record))) {
            newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"flows", &newpData);
            if (newpData) {
                CREATE_ARRAY(newpData);
                for (INTEGER i = 0; i < flows; i++) {
                    void *flowData = 0;
                    Invoke(INVOKE_ARRAY_VARIABLE, newpData, i, &flowData);
                    if (flowData) {
                        memcpy(&record, buf, sizeof(NetFlow5Record));

                        record.srcaddr = ntohl(record.srcaddr);
                        record.dstaddr = ntohl(record.dstaddr);
                        record.nexthop = ntohl(record.nexthop);
                        record.input   = ntohs(record.input);
                        record.output  = ntohs(record.output);
                        record.dPkts   = ntohl(record.dPkts);
                        record.dOctets = ntohl(record.dOctets);
                        record.first   = ntohl(record.first);
                        record.last    = ntohl(record.last);
                        record.srcport = ntohs(record.srcport);
                        record.dstport = ntohs(record.dstport);
                        record.src_as  = ntohs(record.src_as);
                        record.dst_as  = ntohs(record.dst_as);

                        CREATE_ARRAY(flowData);
                        IP_to_str(record.srcaddr, ipbuf);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "IPV4_SRC_ADDR", (INTEGER)VARIABLE_STRING, (char *)ipbuf, (NUMBER)0);
                        IP_to_str(record.dstaddr, ipbuf);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "IPV4_DST_ADDR", (INTEGER)VARIABLE_STRING, (char *)ipbuf, (NUMBER)0);
                        IP_to_str(record.nexthop, ipbuf);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "IPV4_NEXT_HOP", (INTEGER)VARIABLE_STRING, (char *)ipbuf, (NUMBER)0);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "INPUT_SNMP", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.input);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "OUTPUT_SNMP", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.output);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "IN_PKTS", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.dPkts);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "IN_BYTES", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.dOctets);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "FIRST_SWITCHED", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.first);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "LAST_SWITCHED", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.last);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "L4_SRC_PORT", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.srcport);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "L4_DST_PORT", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.dstport);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "pad1", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.pad1);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "TCP_FLAGS", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.tcp_flags);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "PROTOCOL", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.prot);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "SRC_TOS", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.tos);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "SRC_AS", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.src_as);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "DST_AS", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.dst_as);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "SRC_MASK", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.src_mask);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "DST_MASK", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.dst_mask);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, flowData, "pad2", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.pad2);
                    } else
                        return 0;
                    buf += sizeof(NetFlow5Record);
                    len -= sizeof(NetFlow5Record);
                }
            }
        }
    } else
    if ((version == 9) && (PARAM_LEN(0) >= (int)sizeof(NetFlow9Header))) {
        NetFlow9Header   header;
        NetFlow9Template tpl;
        NetFlow9Record   record;
        memcpy(&header, buf, sizeof(NetFlow9Header));
        header.version          = ntohs(header.version);
        header.flows            = ntohs(header.flows);
        header.uptime_ms        = ntohl(header.uptime_ms);
        header.time_sec         = ntohl(header.time_sec);
        header.package_sequence = ntohl(header.package_sequence);
        header.source_id        = ntohl(header.source_id);
        AnsiString temp;

        void *newpData = 0;
        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"metadata", &newpData);
        if (newpData) {
            CREATE_ARRAY(newpData);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "version", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.version);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "flows", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.flows);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "uptime_ms", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.uptime_ms);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "time_sec", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.time_sec);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "package_sequence", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.package_sequence);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "source_id", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)header.source_id);
        }

        int len = PARAM_LEN(0) - sizeof(NetFlow9Header);
        buf += sizeof(NetFlow9Header);

        void *flows = 0;
        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"flows", &flows);
        if (!flows)
            return 0;
        CREATE_ARRAY(flows);
        INTEGER flow_index = 0;

        void    *templates     = 0;
        INTEGER template_index = 0;

        int  sets  = header.flows;
        char cache = 0;
        while ((len > (int)sizeof(unsigned short)) && (sets)) {
            memcpy(&record, buf, sizeof(NetFlow9Record));
            record.flowset_id = ntohs(record.flowset_id);
            record.length     = ntohs(record.length);
            if (len < record.length)
                break;

            len -= sizeof(NetFlow9Record);
            buf += sizeof(NetFlow9Record);
            if (record.flowset_id <= 255) {
                // is template
                if (!template_index) {
                    // first template
                    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"templates", &templates);
                    if (!templates)
                        return 0;
                    CREATE_ARRAY(templates);

                    if (handle)
                        FreeCHTPLHandle(handle);
                    if (PARAMETERS_COUNT > 1) {
                        handle = new std::map<unsigned short, CachedTemplate *>();
                        SET_NUMBER(1, (SYS_INT)handle);
                        cache = 1;
                    }
                }
                int datalen = record.length - sizeof(NetFlow9Record);
                do {
                    if (len < (int)sizeof(NetFlow9Template))
                        return 0;
                    memcpy(&tpl, buf, sizeof(NetFlow9Template));
                    tpl.template_id = ntohs(tpl.template_id);
                    tpl.field_count = ntohs(tpl.field_count);
                    buf            += sizeof(NetFlow9Template);
                    len            -= sizeof(NetFlow9Template);
                    Invoke(INVOKE_ARRAY_VARIABLE, templates, template_index++, &newpData);
                    datalen -= sizeof(NetFlow9Template);
                    if (newpData) {
                        CREATE_ARRAY(newpData);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "flowset_id", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.flowset_id);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "length", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)record.length);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "template_id", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tpl.template_id);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "field_count", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tpl.field_count);

                        int len2 = len - sizeof(NetFlow9Template);
                        if (len2 < (int)(tpl.field_count * sizeof(unsigned short) * 2))
                            return 0;

                        void *fields = 0;
                        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, newpData, "fields", &fields);
                        if (!fields)
                            return 0;
                        CREATE_ARRAY(fields);

                        CachedTemplate *t = 0;
                        if (cache) {
                            t = (CachedTemplate *)malloc(sizeof(CachedTemplate *));
                            if (t) {
                                t->fields_count            = tpl.field_count;
                                t->fields                  = (CachedFields *)malloc(t->fields_count * sizeof(CachedFields));
                                (*handle)[tpl.template_id] = t;
                            }
                        }

                        int field_index = 0;
                        for (INTEGER i = 0; i < tpl.field_count; i++) {
                            unsigned short value = *(unsigned short *)buf;
                            value    = ntohs(value);
                            buf     += sizeof(unsigned short);
                            datalen -= sizeof(unsigned short);

                            unsigned short length = *(unsigned short *)buf;
                            length   = ntohs(length);
                            buf     += sizeof(unsigned short);
                            datalen -= sizeof(unsigned short);

                            if (t) {
                                t->fields[field_index].field_type   = value;
                                t->fields[field_index].field_length = length;
                            }
                            void *elem = 0;
                            Invoke(INVOKE_ARRAY_VARIABLE, fields, field_index, &elem);
                            if (elem) {
                                CREATE_ARRAY(elem);
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, (char *)"type", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)value);
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, elem, (char *)"length", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)length);
                            }
                            field_index++;
                        }
                    } else
                        return 0;
                } while (datalen > 0);
            } else {
                CachedTemplate *t = 0;
                if (handle)
                    t = (*handle)[record.flowset_id];

                if (t) {
                    int datalen = record.length - sizeof(NetFlow9Record);

                    int padding = 4 - record.length % 4;
                    do {
                        //int total=0;
                        Invoke(INVOKE_ARRAY_VARIABLE, flows, flow_index++, &newpData);
                        if (!newpData)
                            break;
                        CREATE_ARRAY(newpData);

                        int     len2  = t->fields_count;
                        for (int i = 0; i < len2; i++) {
                            int field_len  = t->fields[i].field_length;
                            int field_type = t->fields[i].field_type;
                            if (len < field_len) {
                                //fprintf(stderr, "ERROR %i < %i (%i/%i)", len, field_len, i, len2);
                                return 0;
                            }
                            unsigned int   ival;
                            unsigned short sval;
                            unsigned char  cval;
                            const char     *key;
                            switch (field_type) {
                                case 8:
                                case 12:
                                case 15:
                                case 18:
                                    ival = *(unsigned int *)buf;
                                    ival = htonl(ival);
                                    IP_to_str(ival, ipbuf);
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, flowkeys[field_type], (INTEGER)VARIABLE_STRING, (char *)ipbuf, (NUMBER)0);
                                    break;

                                case 21:
                                case 22:
                                case 34:
                                case 44:
                                case 45:
                                case 47:
                                case 50:
                                case 64:
                                case 92:
                                case 93:
                                case 99:
                                    ival = *(unsigned int *)buf;
                                    ival = htonl(ival);
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, flowkeys[field_type], (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)ival);
                                    break;

                                case 27:
                                case 28:
                                case 62:
                                case 63:
                                    inet_ntop6((const unsigned char *)buf, ipv6buf, sizeof(ipv6buf));
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, flowkeys[field_type], (INTEGER)VARIABLE_STRING, (char *)ipv6buf, (NUMBER)0);
                                    break;

                                case 80:
                                case 81:
                                    MAC_to_str(buf, macbuf);
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, flowkeys[field_type], (INTEGER)VARIABLE_STRING, (char *)macbuf, (NUMBER)0);
                                    break;

                                case 70:
                                case 71:
                                case 72:
                                case 73:
                                case 74:
                                case 75:
                                case 76:
                                case 77:
                                case 78:
                                case 79:
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, flowkeys[field_type], (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)field_len);
                                    break;

                                case 7:
                                case 11:
                                    sval = *(unsigned short *)buf;
                                    sval = htons(sval);
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, flowkeys[field_type], (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)sval);
                                    break;

                                case 1:
                                case 2:
                                case 10:
                                case 14:
                                case 16:
                                case 17:
                                case 19:
                                case 20:
                                case 23:
                                case 24:
                                default:
                                    if (field_type > 104) {
                                        temp  = (char *)"FieldType#";
                                        temp += (long)field_type;
                                        key   = temp.c_str();
                                    } else
                                        key = flowkeys[field_type];
                                    if (field_len == 4) {
                                        ival = *(unsigned int *)buf;
                                        ival = htonl(ival);
                                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, key, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)ival);
                                    } else
                                    if (field_len == 2) {
                                        sval = *(unsigned short *)buf;
                                        sval = htons(sval);
                                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, key, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)sval);
                                    } else
                                    if (field_len == 1) {
                                        cval = *(unsigned char *)buf;
                                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, key, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)cval);
                                    }
                                    break;
                            }


                            unsigned short value = *(unsigned short *)buf;
                            value = ntohs(value);
                            //total+=field_len;
                            buf += field_len;
                            len -= field_len;

                            datalen -= field_len;
                        }
                        //if (total%4)
                        //    padding=total%4;
                        //else
                        //    padding=0;
                        //buf+=padding;
                        //len-=padding;
                        //datalen-=padding;
                        //fprintf(stderr, "DATALEN: %i (%i)\n", datalen, len);
                    } while (datalen > padding);
                    buf += padding;
                    len -= padding;
                } else {
                    int len2 = record.length - sizeof(NetFlow9Record);
                    buf += len2;
                    len -= len2;
                    if (len <= 0)
                        break;
                }
            }
            sets--;
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DoneNetflowPacket, 1)
    T_NUMBER(DoneNetflowPacket, 0)
    std::map<unsigned short, CachedTemplate *> *handle = 0;
    handle = (std::map<unsigned short, CachedTemplate *> *)(SYS_INT) PARAM(0);
    if (handle) {
        SET_NUMBER(0, 0);
        FreeCHTPLHandle(handle);
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//

