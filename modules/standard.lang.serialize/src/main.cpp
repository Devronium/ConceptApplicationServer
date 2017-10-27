//#define GO_DEBUG
//#define WITH_LIBXML2

//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
#include <stdint.h>
#include <map>
#ifndef _WIN32
 #include <netinet/in.h>
#endif
#ifdef WITH_LIBXML2
 #include <libxml/xmlreader.h>
 #define XML_NEXT(cur_node)                              cur_node->next
 #define XML_NODE_TYPE(cur_node)                         (char *)cur_node->name
 #define XML_NODE_PROP(cur_node, attr)                   (char *)xmlGetProp(cur_node, BAD_CAST attr);
 #define XML_CONTENT(cur_node)                           (cur_node->children ? (char *)cur_node->children->content : (char *)"")
 #define XML_FIRST_CHILD(cur_node)                       cur_node->children
 #define XML_FREE_PROP_STR(str)                          if (str) xmlFree(str)
 #define XML_IS_NODE(cur_node)                           (cur_node->type == XML_ELEMENT_NODE)
 #define XML_DECLARE_NODE(node)                          xmlNodePtr node = NULL;
 #define XML_NODE        xmlNodePtr
 #define XML_NODE_REF    xmlNodePtr
 #define XML_REF(node)                                   node
 #define XML_DEREF(node)                                 node
 #define XML_NEW_NODE(node_name, parent)                 xmlNewNode(NULL, BAD_CAST node_name)
 #define XML_SET_PROP(node, attr, val)                   xmlNewProp(node, BAD_CAST attr, BAD_CAST val)
 #define XML_ADD_CHILD(parent, node)                     xmlAddChild(parent, node);
 #define XML_NEW_ROOT(node, node_name, doc)              { node = xmlNewNode(NULL, BAD_CAST node_name); xmlDocSetRootElement(doc, node); }
 #define XML_NULL_NODE(node)                             node = NULL;
 #define XML_NEW_TEXT_NODE(node, data, len, parent)      node = xmlNewTextLen(BAD_CAST data, len);
 #define XML_NEW_TEXT_NODE_NUMBER(node, data, parent)    node = xmlNewText(BAD_CAST AnsiString(data).c_str());
#else
 #include "pugixml.hpp"
 #define xmlDoc        pugi::xml_document
 #define xmlDocPtr     pugi::xml_document *
 #define xmlNodePtr    pugi::xml_node
//#define xmlFree(nop)
//#define xmlChar char
 #define BAD_CAST

 #define XML_NEXT(cur_node)                              cur_node.next_sibling()
 #define XML_NODE_TYPE(cur_node)                         (char *)cur_node.name()
 #define XML_NODE_PROP(cur_node, attr)                   (char *)cur_node.attribute(attr).as_string()
 #define XML_CONTENT(cur_node)                           (char *)cur_node.first_child().value()
 #define XML_FIRST_CHILD(cur_node)                       cur_node.first_child()
 #define XML_FREE_PROP_STR(str)                          { }
 #define XML_IS_NODE(cur_node)                           1
 #define XML_DECLARE_NODE(node)                          pugi::xml_node node;
 #define XML_NODE        pugi::xml_node
 #define XML_NODE_REF    pugi::xml_node *
 #define XML_REF(node)                                   (&node)
 #define XML_DEREF(node)                                 (*node)
 #define XML_NEW_NODE(node_name, parent)                 parent.append_child(node_name);
 #define XML_SET_PROP(node, attr, val)                   node.append_attribute(attr) = val;
 #define XML_ADD_CHILD(parent, node)                     { }
 #define XML_NEW_ROOT(node, node_name, doc)              { node = doc->append_child(node_name); }
 #define XML_NULL_NODE(node)                             { }
 #define XML_NEW_TEXT_NODE(node, data, len, parent)      { if ((len) && (data)) parent.text().set(data); }
 #define XML_NEW_TEXT_NODE_NUMBER(node, data, parent)    parent.text().set(data);

class SimpleWriter : public pugi::xml_writer {
public:
    AnsiString buffer;

    void write(const void *data, size_t size) {
        buffer.AddBuffer((char *)data, size, 256);
    }
};
#endif
#include "miniz.c"

typedef struct {
    unsigned int val;
    unsigned char bits;
} HuffCode;

typedef struct {
    unsigned int val;
    unsigned char symbol;
} HuffChar;

static const HuffCode HuffTable[] = {{0x1ff8, 13}, {0x7fffd8, 23}, {0xfffffe2, 28}, {0xfffffe3, 28}, {0xfffffe4, 28}, {0xfffffe5, 28}, {0xfffffe6, 28}, {0xfffffe7, 28}, {0xfffffe8, 28}, {0xffffea, 24}, {0x3ffffffc, 30}, {0xfffffe9, 28}, {0xfffffea, 28}, {0x3ffffffd, 30}, {0xfffffeb, 28}, {0xfffffec, 28}, {0xfffffed, 28}, {0xfffffee, 28}, {0xfffffef, 28}, {0xffffff0, 28}, {0xffffff1, 28}, {0xffffff2, 28}, {0x3ffffffe, 30}, {0xffffff3, 28}, {0xffffff4, 28}, {0xffffff5, 28}, {0xffffff6, 28}, {0xffffff7, 28}, {0xffffff8, 28}, {0xffffff9, 28}, {0xffffffa, 28}, {0xffffffb, 28}, {0x14, 6}, {0x3f8, 10}, {0x3f9, 10}, {0xffa, 12}, {0x1ff9, 13}, {0x15, 6}, {0xf8, 8}, {0x7fa, 11}, {0x3fa, 10}, {0x3fb, 10}, {0xf9, 8}, {0x7fb, 11}, {0xfa, 8}, {0x16, 6}, {0x17, 6}, {0x18, 6}, {0x0, 5}, {0x1, 5}, {0x2, 5}, {0x19, 6}, {0x1a, 6}, {0x1b, 6}, {0x1c, 6}, {0x1d, 6}, {0x1e, 6}, {0x1f, 6}, {0x5c, 7}, {0xfb, 8}, {0x7ffc, 15}, {0x20, 6}, {0xffb, 12}, {0x3fc, 10}, {0x1ffa, 13}, {0x21, 6}, {0x5d, 7}, {0x5e, 7}, {0x5f, 7}, {0x60, 7}, {0x61, 7}, {0x62, 7}, {0x63, 7}, {0x64, 7}, {0x65, 7}, {0x66, 7}, {0x67, 7}, {0x68, 7}, {0x69, 7}, {0x6a, 7}, {0x6b, 7}, {0x6c, 7}, {0x6d, 7}, {0x6e, 7}, {0x6f, 7}, {0x70, 7}, {0x71, 7}, {0x72, 7}, {0xfc, 8}, {0x73, 7}, {0xfd, 8}, {0x1ffb, 13}, {0x7fff0, 19}, {0x1ffc, 13}, {0x3ffc, 14}, {0x22, 6}, {0x7ffd, 15}, {0x3, 5}, {0x23, 6}, {0x4, 5}, {0x24, 6}, {0x5, 5}, {0x25, 6}, {0x26, 6}, {0x27, 6}, {0x6, 5}, {0x74, 7}, {0x75, 7}, {0x28, 6}, {0x29, 6}, {0x2a, 6}, {0x7, 5}, {0x2b, 6}, {0x76, 7}, {0x2c, 6}, {0x8, 5}, {0x9, 5}, {0x2d, 6}, {0x77, 7}, {0x78, 7}, {0x79, 7}, {0x7a, 7}, {0x7b, 7}, {0x7ffe, 15}, {0x7fc, 11}, {0x3ffd, 14}, {0x1ffd, 13}, {0xffffffc, 28}, {0xfffe6, 20}, {0x3fffd2, 22}, {0xfffe7, 20}, {0xfffe8, 20}, {0x3fffd3, 22}, {0x3fffd4, 22}, {0x3fffd5, 22}, {0x7fffd9, 23}, {0x3fffd6, 22}, {0x7fffda, 23}, {0x7fffdb, 23}, {0x7fffdc, 23}, {0x7fffdd, 23}, {0x7fffde, 23}, {0xffffeb, 24}, {0x7fffdf, 23}, {0xffffec, 24}, {0xffffed, 24}, {0x3fffd7, 22}, {0x7fffe0, 23}, {0xffffee, 24}, {0x7fffe1, 23}, {0x7fffe2, 23}, {0x7fffe3, 23}, {0x7fffe4, 23}, {0x1fffdc, 21}, {0x3fffd8, 22}, {0x7fffe5, 23}, {0x3fffd9, 22}, {0x7fffe6, 23}, {0x7fffe7, 23}, {0xffffef, 24}, {0x3fffda, 22}, {0x1fffdd, 21}, {0xfffe9, 20}, {0x3fffdb, 22}, {0x3fffdc, 22}, {0x7fffe8, 23}, {0x7fffe9, 23}, {0x1fffde, 21}, {0x7fffea, 23}, {0x3fffdd, 22}, {0x3fffde, 22}, {0xfffff0, 24}, {0x1fffdf, 21}, {0x3fffdf, 22}, {0x7fffeb, 23}, {0x7fffec, 23}, {0x1fffe0, 21}, {0x1fffe1, 21}, {0x3fffe0, 22}, {0x1fffe2, 21}, {0x7fffed, 23}, {0x3fffe1, 22}, {0x7fffee, 23}, {0x7fffef, 23}, {0xfffea, 20}, {0x3fffe2, 22}, {0x3fffe3, 22}, {0x3fffe4, 22}, {0x7ffff0, 23}, {0x3fffe5, 22}, {0x3fffe6, 22}, {0x7ffff1, 23}, {0x3ffffe0, 26}, {0x3ffffe1, 26}, {0xfffeb, 20}, {0x7fff1, 19}, {0x3fffe7, 22}, {0x7ffff2, 23}, {0x3fffe8, 22}, {0x1ffffec, 25}, {0x3ffffe2, 26}, {0x3ffffe3, 26}, {0x3ffffe4, 26}, {0x7ffffde, 27}, {0x7ffffdf, 27}, {0x3ffffe5, 26}, {0xfffff1, 24}, {0x1ffffed, 25}, {0x7fff2, 19}, {0x1fffe3, 21}, {0x3ffffe6, 26}, {0x7ffffe0, 27}, {0x7ffffe1, 27}, {0x3ffffe7, 26}, {0x7ffffe2, 27}, {0xfffff2, 24}, {0x1fffe4, 21}, {0x1fffe5, 21}, {0x3ffffe8, 26}, {0x3ffffe9, 26}, {0xffffffd, 28}, {0x7ffffe3, 27}, {0x7ffffe4, 27}, {0x7ffffe5, 27}, {0xfffec, 20}, {0xfffff3, 24}, {0xfffed, 20}, {0x1fffe6, 21}, {0x3fffe9, 22}, {0x1fffe7, 21}, {0x1fffe8, 21}, {0x7ffff3, 23}, {0x3fffea, 22}, {0x3fffeb, 22}, {0x1ffffee, 25}, {0x1ffffef, 25}, {0xfffff4, 24}, {0xfffff5, 24}, {0x3ffffea, 26}, {0x7ffff4, 23}, {0x3ffffeb, 26}, {0x7ffffe6, 27}, {0x3ffffec, 26}, {0x3ffffed, 26}, {0x7ffffe7, 27}, {0x7ffffe8, 27}, {0x7ffffe9, 27}, {0x7ffffea, 27}, {0x7ffffeb, 27}, {0xffffffe, 28}, {0x7ffffec, 27}, {0x7ffffed, 27}, {0x7ffffee, 27}, {0x7ffffef, 27}, {0x7fffff0, 27}, {0x3ffffee, 26}, {0x3fffffff, 30}}; 
static const int HuffLengths[] = {5, 6, 7, 8, 10, 11, 12, 13, 14, 15, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 30, 0};
static const unsigned short HuffQuickVals[] = {0x530, 0x531, 0x532, 0x561, 0x563, 0x565, 0x569, 0x56f, 0x573, 0x574, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x620, 0x625, 0x62d, 0x62e, 0x62f, 0x633, 0x634, 0x635, 0x636, 0x637, 0x638, 0x639, 0x63d, 0x641, 0x65f, 0x662, 0x664, 0x666, 0x667, 0x668, 0x66c, 0x66d, 0x66e, 0x670, 0x672, 0x675, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x73a, 0x742, 0x743, 0x744, 0x745, 0x746, 0x747, 0x748, 0x749, 0x74a, 0x74b, 0x74c, 0x74d, 0x74e, 0x74f, 0x750, 0x751, 0x752, 0x753, 0x754, 0x755, 0x756, 0x757, 0x759, 0x76a, 0x76b, 0x771, 0x776, 0x777, 0x778, 0x779, 0x77a, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x826, 0x82a, 0x82c, 0x83b, 0x858, 0x85a, 0, 0};

static const HuffChar slow_values_10[] = {{0x3f8, 33}, {0x3f9, 34}, {0x3fa, 40}, {0x3fb, 41}, {0x3fc, 63}, {0, 0}};
static const HuffChar slow_values_11[] = {{0x7fa, 39}, {0x7fb, 43}, {0x7fc, 124}, {0, 0}};
static const HuffChar slow_values_12[] = {{0xffa, 35}, {0xffb, 62}, {0, 0}};
static const HuffChar slow_values_13[] = {{0x1ff8, 0}, {0x1ff9, 36}, {0x1ffa, 64}, {0x1ffb, 91}, {0x1ffc, 93}, {0x1ffd, 126}, {0, 0}};
static const HuffChar slow_values_14[] = {{0x3ffc, 94}, {0x3ffd, 125}, {0, 0}};
static const HuffChar slow_values_15[] = {{0x7ffc, 60}, {0x7ffd, 96}, {0x7ffe, 123}, {0, 0}};
static const HuffChar slow_values_19[] = {{0x7fff0, 92}, {0x7fff1, 195}, {0x7fff2, 208}, {0, 0}};
static const HuffChar slow_values_20[] = {{0xfffe6, 128}, {0xfffe7, 130}, {0xfffe8, 131}, {0xfffe9, 162}, {0xfffea, 184}, {0xfffeb, 194}, {0xfffec, 224}, {0xfffed, 226}, {0, 0}};
static const HuffChar slow_values_21[] = {{0x1fffdc, 153}, {0x1fffdd, 161}, {0x1fffde, 167}, {0x1fffdf, 172}, {0x1fffe0, 176}, {0x1fffe1, 177}, {0x1fffe2, 179}, {0x1fffe3, 209}, {0x1fffe4, 216}, {0x1fffe5, 217}, {0x1fffe6, 227}, {0x1fffe7, 229}, {0x1fffe8, 230}, {0, 0}};
static const HuffChar slow_values_22[] = {{0x3fffd2, 129}, {0x3fffd3, 132}, {0x3fffd4, 133}, {0x3fffd5, 134}, {0x3fffd6, 136}, {0x3fffd7, 146}, {0x3fffd8, 154}, {0x3fffd9, 156}, {0x3fffda, 160}, {0x3fffdb, 163}, {0x3fffdc, 164}, {0x3fffdd, 169}, {0x3fffde, 170}, {0x3fffdf, 173}, {0x3fffe0, 178}, {0x3fffe1, 181}, {0x3fffe2, 185}, {0x3fffe3, 186}, {0x3fffe4, 187}, {0x3fffe5, 189}, {0x3fffe6, 190}, {0x3fffe7, 196}, {0x3fffe8, 198}, {0x3fffe9, 228}, {0x3fffea, 232}, {0x3fffeb, 233}, {0, 0}};
static const HuffChar slow_values_23[] = {{0x7fffd8, 1}, {0x7fffd9, 135}, {0x7fffda, 137}, {0x7fffdb, 138}, {0x7fffdc, 139}, {0x7fffdd, 140}, {0x7fffde, 141}, {0x7fffdf, 143}, {0x7fffe0, 147}, {0x7fffe1, 149}, {0x7fffe2, 150}, {0x7fffe3, 151}, {0x7fffe4, 152}, {0x7fffe5, 155}, {0x7fffe6, 157}, {0x7fffe7, 158}, {0x7fffe8, 165}, {0x7fffe9, 166}, {0x7fffea, 168}, {0x7fffeb, 174}, {0x7fffec, 175}, {0x7fffed, 180}, {0x7fffee, 182}, {0x7fffef, 183}, {0x7ffff0, 188}, {0x7ffff1, 191}, {0x7ffff2, 197}, {0x7ffff3, 231}, {0x7ffff4, 239}, {0, 0}};
static const HuffChar slow_values_24[] = {{0xffffea, 9}, {0xffffeb, 142}, {0xffffec, 144}, {0xffffed, 145}, {0xffffee, 148}, {0xffffef, 159}, {0xfffff0, 171}, {0xfffff1, 206}, {0xfffff2, 215}, {0xfffff3, 225}, {0xfffff4, 236}, {0xfffff5, 237}, {0, 0}};
static const HuffChar slow_values_25[] = {{0x1ffffec, 199}, {0x1ffffed, 207}, {0x1ffffee, 234}, {0x1ffffef, 235}, {0, 0}};
static const HuffChar slow_values_26[] = {{0x3ffffe0, 192}, {0x3ffffe1, 193}, {0x3ffffe2, 200}, {0x3ffffe3, 201}, {0x3ffffe4, 202}, {0x3ffffe5, 205}, {0x3ffffe6, 210}, {0x3ffffe7, 213}, {0x3ffffe8, 218}, {0x3ffffe9, 219}, {0x3ffffea, 238}, {0x3ffffeb, 240}, {0x3ffffec, 242}, {0x3ffffed, 243}, {0x3ffffee, 255}, {0, 0}};
static const HuffChar slow_values_27[] = {{0x7ffffde, 203}, {0x7ffffdf, 204}, {0x7ffffe0, 211}, {0x7ffffe1, 212}, {0x7ffffe2, 214}, {0x7ffffe3, 221}, {0x7ffffe4, 222}, {0x7ffffe5, 223}, {0x7ffffe6, 241}, {0x7ffffe7, 244}, {0x7ffffe8, 245}, {0x7ffffe9, 246}, {0x7ffffea, 247}, {0x7ffffeb, 248}, {0x7ffffec, 250}, {0x7ffffed, 251}, {0x7ffffee, 252}, {0x7ffffef, 253}, {0x7fffff0, 254}, {0, 0}};
static const HuffChar slow_values_28[] = {{0xfffffe2, 2}, {0xfffffe3, 3}, {0xfffffe4, 4}, {0xfffffe5, 5}, {0xfffffe6, 6}, {0xfffffe7, 7}, {0xfffffe8, 8}, {0xfffffe9, 11}, {0xfffffea, 12}, {0xfffffeb, 14}, {0xfffffec, 15}, {0xfffffed, 16}, {0xfffffee, 17}, {0xfffffef, 18}, {0xffffff0, 19}, {0xffffff1, 20}, {0xffffff2, 21}, {0xffffff3, 23}, {0xffffff4, 24}, {0xffffff5, 25}, {0xffffff6, 26}, {0xffffff7, 27}, {0xffffff8, 28}, {0xffffff9, 29}, {0xffffffa, 30}, {0xffffffb, 31}, {0xffffffc, 127}, {0xffffffd, 220}, {0xffffffe, 249}, {0, 0}};
static const HuffChar slow_values_30[] = {{0x3ffffffc, 10}, {0x3ffffffd, 13}, {0x3ffffffe, 22}, {0, 0}};

static const HuffChar *slow_table[] = {slow_values_10, slow_values_11, slow_values_12, slow_values_13, slow_values_14, slow_values_15, NULL, NULL, NULL, slow_values_19, slow_values_20, slow_values_21, slow_values_22, slow_values_23, slow_values_24, slow_values_25, slow_values_26, slow_values_27, slow_values_28, NULL, slow_values_30};

#define TOINT(a)    (a ? atol(a) : 0)

#define MAX_OBJECTS    0x1FFFF

typedef struct {
    std::map<void *, int> BACK_CACHE;
    void       *BACK_REFERENCES[MAX_OBJECTS];
    void       *BACK_REFERENCES2[MAX_OBJECTS];
    char       BACK_TYPES[MAX_OBJECTS];
    int        BACK_REF_COUNT;
    void       *top_variable;
    AnsiString full_error;
    xmlDocPtr  doc;

    void       *err_ser;
    char       *buf;

    char       **filters;
    int        filters_count;
    int        filters_used;
    int        filters_set;

    int        size;
    int        offset;
    int        is_buffer;
    int        increment;
    int        filter_set;
    int        no_defaults;
} RefContainer;

INVOKE_CALL LocalInvoker;

void Serialize(RefContainer *rc, void *pData, XML_NODE_REF parent, char is_simple = 0, char is_simple_array_element = 0);
void SerializeVariable(RefContainer *rc, char *member, int type, char *szData, NUMBER nData, void *class_data, void *variable_data, XML_NODE_REF parent, char is_simple = 0);
void SerializeArray(RefContainer *rc, void *pData, void *arr_id, XML_NODE_REF parent, char is_simple = 0);
void DoObject(RefContainer *rc, void *pData, void *parent, bool root_only = false);
void DoArray(RefContainer *rc, void *pData, void *parent);
int bin_write(RefContainer *rc, char *data, int d_size, int write_increment = 0x1000);
int bin_write_int(RefContainer *rc, int v);
int bin_write_size(RefContainer *rc, unsigned long long v);
int bin_write_char(RefContainer *rc, char v);
int bin_write_double(RefContainer *rc, double v);

#ifdef GO_DEBUG
 #include <iostream>
 #define DEBUG(data)                   std::cout << (data) << "\n"; std::cout.flush();
 #define DEBUG2(data, data2)           std::cout << (data) << ": " << (data2) << "\n"; std::cout.flush();
 #define DEBUG3(data, data2, data3)    std::cout << (data) << ": " << (data2) << ", " << (data3) << "\n"; std::cout.flush();
#else
 #define DEBUG(data)
 #define DEBUG2(data, data1)
 #define DEBUG3(data, data1, data2)
#endif

#define RESET_ERROR    if (xml_error.Length()) xml_error.LoadBuffer(0, 0);

#define RAISE_PACK_ERROR(err)                            { free(buf); RETURN_STRING(""); return 0; }
#define CHECK_PACK_SIZE(i, len)                          if (i >= len) RAISE_PACK_ERROR(-2)
#define CHECK_PACK_VALID_SIZE(size, min_val, max_val)    if ((size < min_val) || (size > max_val)) RAISE_PACK_ERROR(-3)
#define PACK_BUFFER_INCREMENT    0x80
#define PACK_ENSURE_BUFFER(data_size)                    if (buf_size - buf_index < data_size) { if (data_size > PACK_BUFFER_INCREMENT) { buf_size += data_size / PACK_BUFFER_INCREMENT * PACK_BUFFER_INCREMENT; } buf_size += PACK_BUFFER_INCREMENT; buf = (signed char *)realloc(buf, buf_size); }
#define PACK_PANIC               { if (buf_index) { RETURN_BUFFER((char *)buf, buf_index); } else { RETURN_STRING(""); } free(buf); return 0; }
#define PACK_ARRAY(size, code)                                                                    \
    void *pData = LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index - 1] - 1];                    \
    int count = LocalInvoker(INVOKE_GET_ARRAY_COUNT, pData);                                      \
    if (count) {                                                                                  \
        PACK_ENSURE_BUFFER(count * size)                                                          \
        for (INTEGER j = 0; j < count; j++) {                                                     \
            void *elem_data;                                                                      \
            LocalInvoker(INVOKE_ARRAY_VARIABLE, pData, j, &elem_data);                            \
            if (elem_data) {                                                                      \
                LocalInvoker(INVOKE_GET_VARIABLE, elem_data, &TYPE, &szDUMMY_FILL, &nDUMMY_FILL); \
                if (TYPE == VARIABLE_STRING) {                                                    \
                    nDUMMY_FILL = atof(szDUMMY_FILL);                                             \
                    TYPE        = VARIABLE_NUMBER;                                                \
                }                                                                                 \
                if (TYPE == VARIABLE_NUMBER) {                                                    \
                    code;                                                                         \
                }                                                                                 \
            }                                                                                     \
        }                                                                                         \
    }

uint16_t swap_uint16(uint16_t val) {
    return (val << 8) | (val >> 8);
}

int16_t swap_int16(int16_t val) {
    return (val << 8) | ((val >> 8) & 0xFF);
}

uint32_t swap_uint32(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

int32_t swap_int32(int32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

int64_t swap_int64(int64_t val) {
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
}

uint64_t swap_uint64(uint64_t val) {
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
}

//---------------------------------------------------------------------------
static AnsiString xml_error;
#ifdef WITH_LIBXML2
 #ifdef _WIN32
xmlFreeFunc xmlFree = 0;
 #endif

void MyGenericErrorFunc(void *ctx, const char *msg, ...) {
    va_list args;

    va_start(args, msg);
    char buffer[0xFFF];
    buffer[0] = 0;
    vsprintf(buffer, msg, args);
    va_end(args);
    xml_error += buffer;
}
#endif
//---------------------------------------------------------------------------
static signed char is_little_endian = 1;
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
 #ifdef WITH_LIBXML2
    xmlMallocFunc  xmlMalloc  = 0;
    xmlReallocFunc xmlRealloc = 0;
    if (!xmlFree)
        xmlMemGet(&xmlFree, &xmlMalloc, &xmlRealloc, NULL);
 #endif
#endif
    LocalInvoker = Invoke;
    short int number  = 0x1;
    char      *numPtr = (char *)&number;
    if (numPtr[0] != 1)
        is_little_endian = 0;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SerializeError, 0)
    INTEGER len = xml_error.Length();
    if (len > 0) {
        AnsiString res = "<pre><![CDATA[\n";
        res += xml_error;
        res += "\n]]></pre>";
        RETURN_BUFFER(xml_error.c_str(), xml_error.Length())
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
int CheckBack(RefContainer *rc, void *pData) {
    // check if a reference is already serialized (anti-cyclic reference)
    int i = rc->BACK_CACHE[pData];
    if (i)
        return i;

    if (rc->BACK_REF_COUNT < MAX_OBJECTS) {
        rc->BACK_REFERENCES[rc->BACK_REF_COUNT++] = pData;
        rc->BACK_CACHE[pData] = rc->BACK_REF_COUNT;
    }
    return 0;
}

//---------------------------------------------------------------------------
void SerializeArray(RefContainer *rc, void *pData, void *arr_id, XML_NODE_REF parent, char is_simple) {
    void *newpData;
    char *key;

    XML_DECLARE_NODE(node);
    XML_DECLARE_NODE(node2);
    if (rc->is_buffer) {
        int ref_ptr = CheckBack(rc, arr_id);
        if (ref_ptr) {
            bin_write_int(rc, -ref_ptr);
            return;
        }
        bin_write_int(rc, rc->BACK_REF_COUNT);
    } else
    if (!is_simple) {
        int ref_ptr = CheckBack(rc, arr_id);
        if (ref_ptr) {
            DEBUG("...backreferenced array");
            node = XML_NEW_NODE("cyclic_reference", XML_DEREF(parent));
            XML_SET_PROP(node, "refID", AnsiString((long)ref_ptr).c_str());

            XML_ADD_CHILD(parent, node);
            return;
        }
    }


    DEBUG2("           array", arr_id);
    int count = LocalInvoker(INVOKE_GET_ARRAY_COUNT, pData);

    if (rc->is_buffer) {
        bin_write_size(rc, count);
    } else
    if (is_simple) {
        node = XML_DEREF(parent);
        if (!node) {
            XML_NEW_ROOT(node, "array", rc->doc);
            //xmlDocSetRootElement(rc->doc, node);
        }
    } else {
        node = XML_NEW_NODE("array", XML_DEREF(parent));
        XML_SET_PROP(node, "cycid", AnsiString((long)rc->BACK_REF_COUNT).c_str());
        XML_ADD_CHILD(parent, node);
    }

    char **keys = NULL;
    if (count > 0) {
        keys = (char **)malloc(count * sizeof(char *));
        LocalInvoker(INVOKE_ARRAY_KEYS, pData, keys, (INTEGER)count);

        rc->increment += count * (sizeof(NUMBER) + 2);
    }

    for (int i = 0; i < count; i++) {
        newpData = 0;
        key      = 0;
        LocalInvoker(INVOKE_ARRAY_VARIABLE, pData, i, &newpData);
        //LocalInvoker(INVOKE_GET_ARRAY_KEY,pData,i,&key);
        if (keys)
            key = keys[i];
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            LocalInvoker(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);

            if (rc->is_buffer) {
                if (key) {
                    int key_len = strlen(key);
                    bin_write_size(rc, key_len);
                    bin_write(rc, key, key_len);
                } else
                    bin_write_size(rc, 0);
            } else {
                if (is_simple == 2) {
                    if ((key) && (key[0])) {
                        AnsiString parsed_key = key;
                        char       *pkey      = parsed_key.c_str();
                        int        len        = parsed_key.Length();
                        for (int i = 0; i < len; i++) {
                            if (!strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.:-", pkey[i]))
                                pkey[i] = '_';
                        }
                        node2 = XML_NEW_NODE(pkey, node);
                        XML_ADD_CHILD(node, node2);
                    } else {
                        if (type == VARIABLE_CLASS) {
                            char *class_name = "class";
                            LocalInvoker(INVOKE_GET_SERIAL_CLASS, szData, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);
                            node2 = XML_NEW_NODE(class_name, node);
                        } else
                        if (type == VARIABLE_STRING) {
                            node2 = XML_NEW_NODE("string", node);
                        } else
                        if (type == VARIABLE_NUMBER) {
                            node2 = XML_NEW_NODE("number", node);
                        } else
                        if (type == VARIABLE_ARRAY) {
                            node2 = XML_NEW_NODE("array", node);
                        } else
                        if (type == VARIABLE_DELEGATE) {
                            node2 = XML_NEW_NODE("delegate", node);
                        }
                        XML_ADD_CHILD(node, node2);
                    }
                } else {
                    node2 = XML_NEW_NODE("element", node);
                    XML_ADD_CHILD(node, node2);
                }

                if (is_simple != 2) {
                    if ((key) && (key[0]))
                        XML_SET_PROP(node2, "key", key);
                    XML_SET_PROP(node2, "index", AnsiString((long)i).c_str());
                }
            }

            if (type == VARIABLE_DELEGATE) {
                char *deleg_name = 0;
                //xmlNewProp(node2, BAD_CAST "type", BAD_CAST "delegate");
                LocalInvoker(INVOKE_GET_MEMBER_FROM_ID, (void *)szData, (long)nData, &deleg_name);
                nData = (long)deleg_name;
                char *class_name = 0;
                LocalInvoker(INVOKE_GET_SERIAL_CLASS, (void *)szData, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);
                SerializeVariable(rc, /*key*/ 0, type, class_name, nData, /*0*/ (void *)szData, newpData, XML_REF(node2), is_simple);
            } else
            if ((type != VARIABLE_CLASS) || (rc->is_buffer)) {
                SerializeVariable(rc, /*key*/ 0, type, szData, nData, /*0*/ (void *)szData, newpData, XML_REF(node2), is_simple);
            } else {
                if (!is_simple)
                    XML_SET_PROP(node2, "type", "object");
                Serialize(rc, szData, XML_REF(node2), is_simple, 1);
            }
        }
    }
    if (keys)
        free(keys);
}

//---------------------------------------------------------------------------
void SerializeVariable(RefContainer *rc, char *member, int type, char *szData, NUMBER nData, void *class_data, void *variable_data, XML_NODE_REF parent, char is_simple) {
    DEBUG2("      -> variable", member ? member : "array element");
    XML_NODE node;
    XML_NODE node2;

    if (rc->is_buffer) {
        XML_NULL_NODE(node);
        XML_NULL_NODE(node2);
        if (member) {
            int size = strlen(member);
            bin_write_size(rc, size);
            bin_write(rc, member, size);
        }
    } else
    if (member) {
        if (is_simple) {
            node = XML_NEW_NODE((member ? member : "%NO"), XML_DEREF(parent));
            XML_ADD_CHILD(parent, node);
        } else {
            node = XML_NEW_NODE("member", XML_DEREF(parent));
            XML_ADD_CHILD(parent, node);
            XML_SET_PROP(node, "name", (member ? member : "%NO"));
        }
    } else {
        node = XML_DEREF(parent);
    }

    switch (type) {
        case VARIABLE_STRING:
            DEBUG2("           string", (void *)szData);
            DEBUG2("           length", (long)nData);
            if (nData > 0)
                DEBUG2("           content[0]", szData[0]);
            if (rc->is_buffer) {
                bin_write_char(rc, type);
                bin_write_size(rc, (int)nData);
                bin_write(rc, szData, (int)nData);
            } else {
                if (is_simple != 2)
                    XML_SET_PROP(node, "type", "string");
                if ((szData) && (nData > 0)) {
                    //node2 = xmlNewTextLen(BAD_CAST szData, (int)nData);
                    XML_NEW_TEXT_NODE(node2, szData, (unsigned long)nData, node);
                    XML_ADD_CHILD(node, node2);
                }
            }
            break;

        case VARIABLE_NUMBER:
            DEBUG("           number");
            if (rc->is_buffer) {
                bin_write_char(rc, type);
                bin_write_double(rc, nData);
            } else {
                if (is_simple != 2)
                    XML_SET_PROP(node, "type", "number");
                //node2 = xmlNewText(BAD_CAST AnsiString(nData).c_str());
                //XML_ADD_CHILD(node, node2);
                XML_NEW_TEXT_NODE_NUMBER(node2, nData, node);
                XML_ADD_CHILD(node, node2);
            }
            break;

        case VARIABLE_CLASS:
            DEBUG("           class");
            if (rc->is_buffer) {
                bin_write_char(rc, type);
            } else {
                if (!is_simple)
                    XML_SET_PROP(node, "type", "class");
            }
            Serialize(rc, class_data, XML_REF(node), is_simple, 1);
            break;

        case VARIABLE_ARRAY:
            DEBUG("           array");
            if (rc->is_buffer) {
                bin_write_char(rc, type);
            } else {
                if (is_simple != 2)
                    XML_SET_PROP(node, "type", "array");
            }
            SerializeArray(rc, variable_data, class_data, XML_REF(node), is_simple);
            break;

        case VARIABLE_DELEGATE:
            DEBUG("           delegate");
            DEBUG2("              class", (void *)szData);
            DEBUG2("              member", (void *)(SYS_INT)nData);
            if (rc->is_buffer) {
                bin_write_char(rc, type);
                int size = strlen(szData);
                bin_write_size(rc, size);
                bin_write(rc, szData, size);

                char *member = (char *)(SYS_INT)nData;
                size = strlen(member);
                bin_write_size(rc, size);
                bin_write(rc, member, size);
            } else {
                if (is_simple != 2)
                    XML_SET_PROP(node, "type", "delegate");
                XML_SET_PROP(node, "class", (szData ? szData : "%ERROR"));
                XML_SET_PROP(node, "member", (nData ? (char *)(SYS_INT)nData : "%ERROR"));
            }
            break;
    }
    DEBUG("           serialized");
}

//---------------------------------------------------------------------------
void Serialize(RefContainer *rc, void *pData, XML_NODE_REF parent, char is_simple, char is_simple_array_element) {
    XML_NODE node;

    char *class_name = 0;

    if (rc->is_buffer) {
        //XML_NULL_NODE(node);
        int ref_ptr = CheckBack(rc, pData);
        if (ref_ptr) {
            bin_write_int(rc, -ref_ptr);
            return;
        }
    } else
    if (!is_simple) {
        int ref_ptr = CheckBack(rc, pData);
        if (ref_ptr) {
            DEBUG("...backreference");
            node = XML_NEW_NODE("cyclic_reference", XML_DEREF(parent));
            XML_SET_PROP(node, "refID", AnsiString((long)ref_ptr).c_str());

            XML_ADD_CHILD(parent, node);
            return;
        }
    }

    DEBUG("Getting members count, classname");
    int members_count = LocalInvoker(INVOKE_GET_SERIAL_CLASS, pData, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);
    DEBUG("... success");

    if (rc->is_buffer) {
        bin_write_int(rc, rc->BACK_REF_COUNT);
        int size = strlen(class_name);
        bin_write_size(rc, size);
        bin_write(rc, class_name, size);
        rc->increment += members_count * (sizeof(NUMBER) + 2);
    } else
    if (is_simple) {
        node = XML_DEREF(parent);
        if (!node) {
            XML_NEW_ROOT(node, class_name, rc->doc);
            //xmlDocSetRootElement(rc->doc, node);
        }
        if ((parent) && (is_simple_array_element) && (is_simple != 2))
            XML_SET_PROP(node, "type", class_name);
    } else {
        if (!(XML_DEREF(parent))) {
            XML_NEW_ROOT(node, "object", rc->doc);
            //xmlDocSetRootElement(rc->doc, node);
        } else {
            node = XML_NEW_NODE("object", XML_DEREF(parent));
            XML_ADD_CHILD(parent, node);
        }
        XML_SET_PROP(node, "cycid", AnsiString((long)rc->BACK_REF_COUNT).c_str());

        DEBUG2(class_name, "start");
        XML_SET_PROP(node, "class", (class_name ? class_name : "%ERROR%"));
    }

    if (members_count > 0) {
        char   **members       = new char * [members_count];
        char   *flags          = new char[members_count];
        char   *access         = new char[members_count];
        char   *types          = new char[members_count];
        char   **szValues      = new char * [members_count];
        NUMBER *nValues        = new NUMBER[members_count];
        void   **class_data    = new void * [members_count];
        void   **variable_data = new void * [members_count];

        //memset(szValues,0,members_count * sizeof(char *));
        //memset(nValues,0,members_count * sizeof(NUMBER));

        DEBUG2(class_name, "invoke");
        int result = LocalInvoker(rc->no_defaults ? INVOKE_GET_SERIAL_CLASS_NO_DEFAULTS : INVOKE_GET_SERIAL_CLASS, pData, members_count, &class_name, members, flags, access, types, (const char **)szValues, nValues, class_data, variable_data);
        DEBUG2(class_name, "done invoke");

        if (result != 0) {
            rc->err_ser = (void *)"Error in serialization (bug ?)";
            return;
        }
        DEBUG2(class_name, "members loop");

#ifdef GO_DEBUG
        for (int i = 0; i < members_count; i++) {
            if ((flags[i] == 0) && (types[i] == VARIABLE_STRING)) {
                DEBUG3(class_name, "string member", i);
                DEBUG3(class_name, "string member", (void *)szValues[i]);
            }
        }
#endif

        for (int i = 0; i < members_count; i++) {
            if (flags[i] == 0) {
                if (rc->no_defaults) {
                    if (types[i] == VARIABLE_DELEGATE)
                        continue;
                    // skip arrays
                    if ((rc->no_defaults == 2) && (types[i] == VARIABLE_ARRAY))
                        continue;

                    // skip objects
                    if ((rc->no_defaults == 3) && (types[i] == VARIABLE_CLASS))
                        continue;

                    // skip arrays and objects
                    if ((rc->no_defaults == 4) && ((types[i] == VARIABLE_ARRAY) || (types[i] == VARIABLE_CLASS)))
                        continue;

                    if ((rc->no_defaults == 5) && (members[i]) && ((!strcmp(members[i], "__EVENTS_VECTOR")) || (!strcmp(members[i], "CHILDS")) || (!strcmp(members[i], "CHILD1")) || (!strcmp(members[i], "CHILD2")))) {
                        // skip this
                        continue;
                    }
                }
                DEBUG3(class_name, "serialize", members[i]);
                SerializeVariable(rc, members[i], types[i], szValues[i], nValues[i], class_data[i], variable_data[i], XML_REF(node), is_simple);
                if (rc->err_ser)
                    return;
            }
        }
        if (rc->is_buffer)
            bin_write_size(rc, 0);

        DEBUG2(class_name, "done members loop");

        delete[] members;
        delete[] flags;
        delete[] access;
        delete[] types;
        delete[] szValues;
        delete[] nValues;
        delete[] class_data;
        delete[] variable_data;

        DEBUG2(class_name, "delete");
    }
}

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SerializeObject CONCEPT_API_PARAMETERS {
    RESET_ERROR

    PARAMETERS_CHECK_MIN_MAX(1, 3, "SerializeObject: SerializeObject(object_to_serialize,filename=\"\",encoding=\"UTF-8\")");

    LOCAL_INIT;
    char *pData;
    char *filename = "";
#ifdef WITH_LIBXML2
    char *encoding = "UTF-8";
#else
    char *encoding = NULL;
#endif

    GET_ARRAY(0, pData);

    CHECK_CLASS("SerializeObject parameter error: object expected (first parameter)");
    if (PARAMETERS_COUNT > 1) {
        GET_CHECK_STRING(1, filename, "SerializeObject parameter error: filename must be of string type(STATIC STRING)");
    }
    if (PARAMETERS_COUNT > 2) {
        GET_CHECK_STRING(2, encoding, "SerializeObject parameter error: encoding must be of string type(STATIC STRING)");
    }

    RefContainer *rc = new RefContainer();
    rc->BACK_REF_COUNT = 0;
    rc->top_variable   = NULL;
    rc->is_buffer      = 0;
    rc->err_ser        = 0;
    rc->no_defaults    = 0;
#ifdef WITH_LIBXML2
    void *old_ctx = xmlGenericErrorContext;
    xmlSetGenericErrorFunc(0, MyGenericErrorFunc);

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    rc->doc = doc;
    Serialize(rc, pData, 0);
#else
    pugi::xml_document doc;
    rc->doc = &doc;
    pugi::xml_node doc_element = doc.document_element();
    Serialize(rc, pData, XML_REF(doc_element));
    pugi::xml_encoding t_encoding = pugi::encoding_utf8;
    if (encoding) {
        if (!strcasecmp(encoding, "UTF-16"))
            t_encoding = pugi::encoding_utf16;
        else
        if (!strcasecmp(encoding, "UTF-16LE"))
            t_encoding = pugi::encoding_utf16_le;
        else
        if (!strcasecmp(encoding, "UTF-16BE"))
            t_encoding = pugi::encoding_utf16_be;
        else
        if (!strcasecmp(encoding, "UTF-32"))
            t_encoding = pugi::encoding_utf32_be;
        else
        if (!strcasecmp(encoding, "UTF-32LE"))
            t_encoding = pugi::encoding_utf32_le;
        else
        if (!strcasecmp(encoding, "UTF-32BE"))
            t_encoding = pugi::encoding_utf32_be;
        else
        if (!strcasecmp(encoding, "UTF-16LE"))
            t_encoding = pugi::encoding_utf16_le;
        else
        if (!strcasecmp(encoding, "WCHAR"))
            t_encoding = pugi::encoding_wchar;
        else
        if (!strcasecmp(encoding, "LATIN1"))
            t_encoding = pugi::encoding_latin1;
    }
#endif
    //xmlDtdPtr dtd = xmlCreateIntSubset(doc, BAD_CAST "root", NULL, BAD_CAST "tree2.dtd");

    DEBUG("All done");
    int is_file = false;

    if ((filename) && (filename[0])) {
        is_file = true;
        DEBUG("XML Save file ...");
#ifdef WITH_LIBXML2
        xmlSaveFormatFileEnc(filename, doc, encoding, 1);
#else
        doc.save_file(filename, PUGIXML_TEXT("\t"), pugi::format_default, t_encoding);
#endif
        DEBUG("done");
    } else {
#ifdef WITH_LIBXML2
        xmlChar *mem;
        int     size;

        xmlDocDumpMemoryEnc(doc, &mem, &size, encoding);
        if (size < 0)
            size = 0;
#else
        SimpleWriter writer;
        doc.save(writer, PUGIXML_TEXT(""), pugi::format_raw, t_encoding);
        unsigned long size = writer.buffer.Length();
        char          *mem = writer.buffer.c_str();
#endif

        RETURN_BUFFER((char *)mem, size);

#ifdef WITH_LIBXML2
        xmlFree(mem);
#endif
        //free(mem);
    }


#ifdef WITH_LIBXML2
    DEBUG("Clean ...");
    xmlFreeDoc(doc);
    //xmlCleanupParser();
    xmlMemoryDump();
    DEBUG("done");
#endif
    if (is_file) {
        RETURN_NUMBER(0);
    }
#ifdef WITH_LIBXML2
    xmlSetGenericErrorFunc(old_ctx, xmlGenericError);
#endif
    void *err_ser = NULL;
    if (rc) {
        err_ser = rc->err_ser;
        delete rc;
    }
    return err_ser;
}
//---------------------------------------------------------------------------
int DoNode(RefContainer *rc, void *ConceptHandler, XML_NODE node, void *OwnerPTR = 0, void *obj_ptr = 0) {
    if (!node)
        return 0;

    XML_DECLARE_NODE(cur_node);

    AnsiString prop;
    AnsiString node_type;

    for (cur_node = node; cur_node; cur_node = XML_NEXT(cur_node)) {
        if (XML_IS_NODE(cur_node)) {
            node_type = XML_NODE_TYPE(cur_node);
            if (node_type == (char *)"member") {
                char *prop_ptr = XML_NODE_PROP(cur_node, "type");
                char *name_ptr = XML_NODE_PROP(cur_node, "name");
                if ((prop_ptr) && (name_ptr)) {
                    prop = (char *)prop_ptr;
                    if (prop == (char *)"number") {
                        AnsiString value    = XML_CONTENT(cur_node);
                        NUMBER     nValue   = value.ToFloat();
                        void       *handler = 0;
                        if (!IS_OK(LocalInvoker(INVOKE_GET_CLASS_VARIABLE, OwnerPTR, (char *)name_ptr, &handler))) {
                            rc->err_ser = (char *)"Error in member serialization. Member not defined ?";
                            return -1;
                        }
                        LocalInvoker(INVOKE_SET_CLASS_MEMBER, obj_ptr, (char *)name_ptr, (INTEGER)VARIABLE_NUMBER, 0, nValue);
                    } else
                    if (prop == (char *)"string") {
                        AnsiString value    = XML_CONTENT(cur_node);
                        void       *handler = 0;
                        DEBUG(name_ptr);
                        if (!IS_OK(LocalInvoker(INVOKE_GET_CLASS_VARIABLE, OwnerPTR, (char *)name_ptr, &handler))) {
                            rc->err_ser = (char *)"Error in member serialization. Member not defined ?";
                            return -1;
                        }
                        DEBUG2(name_ptr, "ok, set value :");
                        DEBUG(value.c_str());
                        LocalInvoker(INVOKE_SET_CLASS_MEMBER, obj_ptr, (char *)name_ptr, (INTEGER)VARIABLE_STRING, value.c_str(), (NUMBER)value.Length());
                        DEBUG2(name_ptr, "done value");
                    } else
                    if (prop == (char *)"class") {
                        void *handler = 0;
                        DEBUG("Expanding class member ...");
                        if (!IS_OK(LocalInvoker(INVOKE_GET_CLASS_VARIABLE, OwnerPTR, (char *)name_ptr, &handler))) {
                            rc->err_ser = (char *)"Error in member serialization. Member not defined ?";
                            return -1;
                        }
                        DoNode(rc, ConceptHandler, XML_FIRST_CHILD(cur_node), handler);
                    } else
                    if (prop == (char *)"array") {
                        void *handler = 0;
                        DEBUG("Expanding array member ...");
                        if (!IS_OK(LocalInvoker(INVOKE_GET_CLASS_VARIABLE, OwnerPTR, (char *)name_ptr, &handler))) {
                            rc->err_ser = (char *)"Error in member serialization. Member not defined ?";
                            return -1;
                        }
                        DoNode(rc, ConceptHandler, XML_FIRST_CHILD(cur_node), handler);
                    }
                }
                if (prop_ptr)
                    XML_FREE_PROP_STR(prop_ptr);
                if (name_ptr)
                    XML_FREE_PROP_STR(name_ptr);
            } else
            if (node_type == (char *)"element") {
                char    *key       = 0;
                void    *var_data  = OwnerPTR;
                void    *elem_data = 0;
                char    *prop_ptr  = XML_NODE_PROP(cur_node, "type");
                INTEGER index      = 0;

                key = XML_NODE_PROP(cur_node, "key");
                if ((key) && (key[0])) {
                    LocalInvoker(INVOKE_ARRAY_VARIABLE_BY_KEY, var_data, key, &elem_data);
                    //DoNode(cur_node->children, var_data, elem_data);
                    DEBUG2("Key: ", key);
                } else {
                    char *idx = XML_NODE_PROP(cur_node, "index");
                    if (idx)
                        index = TOINT(idx);//AnsiString(idx).ToInt();
                    else
                        index = -1;
                    XML_FREE_PROP_STR(idx);
                    if (index < 0)
                        index = 0;
                    LocalInvoker(INVOKE_ARRAY_VARIABLE, var_data, index, &elem_data);
                    DEBUG2("Index: ", index);
                    //DoNode(cur_node->children, elem_data);
                }
                XML_FREE_PROP_STR(key);
                if (prop_ptr) {
                    prop = prop_ptr;
                    if (prop == (char *)"number") {
                        AnsiString value  = XML_CONTENT(cur_node);
                        NUMBER     nValue = value.ToFloat();
                        LocalInvoker(INVOKE_SET_VARIABLE, elem_data, (INTEGER)VARIABLE_NUMBER, (char *)0, nValue);
                    } else
                    if (prop == (char *)"string") {
                        AnsiString value = XML_CONTENT(cur_node);
                        LocalInvoker(INVOKE_SET_VARIABLE, elem_data, (INTEGER)VARIABLE_STRING, value.c_str(), (NUMBER)value.Length());
                    } else
                    if (prop == (char *)"object") {
                        DEBUG("Object in array ...");
                        DoNode(rc, ConceptHandler, XML_FIRST_CHILD(cur_node), elem_data);
                    } else
                    if (prop == (char *)"array") {
                        DEBUG("Array in array ...");
                        DoNode(rc, ConceptHandler, XML_FIRST_CHILD(cur_node), elem_data);
                    }
                    XML_FREE_PROP_STR(prop_ptr);
                }
            } else
            if (node_type == (char *)"object") {
                char *class_name = 0;
                void *var_data   = OwnerPTR;

                class_name = XML_NODE_PROP(cur_node, "class");//(char *)xmlGetProp(cur_node, BAD_CAST "class");
                DEBUG2("Building for ", class_name);

                if (!IS_OK(LocalInvoker(INVOKE_CREATE_OBJECT, ConceptHandler, var_data, class_name))) {
                    rc->full_error  = (char *)"Unable to create class. Class ";
                    rc->full_error += class_name;
                    rc->full_error += (char *)" undefined.";
                    rc->err_ser     = (void *)rc->full_error.c_str();
                    return -1;
                }
                INTEGER type;
                char    *str = 0;
                NUMBER  nvalue;

                LocalInvoker(INVOKE_GET_VARIABLE, var_data, &type, &str, &nvalue);

                char *cyc_id_ptr = XML_NODE_PROP(cur_node, "cycid");//(char *)xmlGetProp(cur_node, BAD_CAST "cycid");
                int  cyc_id      = TOINT(cyc_id_ptr);
                if ((cyc_id > 0) && (cyc_id <= MAX_OBJECTS)) {
                    cyc_id--;
                    rc->BACK_REFERENCES[cyc_id] = str;
                    rc->BACK_REF_COUNT++;
                    rc->BACK_TYPES[cyc_id] = VARIABLE_CLASS;
                }

                DoNode(rc, ConceptHandler, XML_FIRST_CHILD(cur_node), var_data, (void *)str);
                XML_FREE_PROP_STR(class_name);
                XML_FREE_PROP_STR(cyc_id_ptr);
            } else
            if (node_type == (char *)"array") {
                void    *var_data = OwnerPTR;
                INTEGER type;
                char    *str = 0;
                NUMBER  nvalue;


                LocalInvoker(INVOKE_CREATE_ARRAY, var_data, 0);

                LocalInvoker(INVOKE_GET_VARIABLE, var_data, &type, &str, &nvalue);

                char *cyc_id_ptr = XML_NODE_PROP(cur_node, "cycid");//(char *)xmlGetProp(cur_node, BAD_CAST "cycid");
                int  cyc_id      = TOINT(cyc_id_ptr);
                if ((cyc_id > 0) && (cyc_id <= MAX_OBJECTS)) {
                    cyc_id--;
                    rc->BACK_REFERENCES[cyc_id] = str;
                    rc->BACK_REF_COUNT++;

                    rc->BACK_TYPES[cyc_id] = VARIABLE_ARRAY;
                }

                DoNode(rc, ConceptHandler, XML_FIRST_CHILD(cur_node), var_data, (void *)str);
                XML_FREE_PROP_STR(cyc_id_ptr);
            } else
            if (node_type == (char *)"cyclic_reference") {
                char *ref_id_ptr = XML_NODE_PROP(cur_node, "refID");//(char *)xmlGetProp(cur_node, BAD_CAST "refID");
                int  ref_id      = TOINT(ref_id_ptr) - 1;
                if ((ref_id >= 0) && (ref_id < rc->BACK_REF_COUNT)) {
                    DEBUG2("Valid cyclic reference: ", ref_id_ptr);
                    LocalInvoker(INVOKE_SET_VARIABLE, OwnerPTR, (INTEGER)rc->BACK_TYPES[ref_id], rc->BACK_REFERENCES[ref_id], (NUMBER)0);
                }
                XML_FREE_PROP_STR(ref_id_ptr);
            }
        }
    }
    return 1;
}

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_UnSerializeObject CONCEPT_API_PARAMETERS {
    RESET_ERROR

                                          PARAMETERS_CHECK_MIN_MAX(1, 3, "UnSerializeObject: UnSerializeObject(filename|buffer, is_raw_buffer=false, var error)");

    LOCAL_INIT;
    char   *filename = "";
    NUMBER _is_raw;
    NUMBER len;
    bool   is_raw = false;

    GET_CHECK_BUFFER(0, filename, len, "UnSerializeObject parameter error: filename must be of string type(STATIC STRING)");

    if (PARAMETERS_COUNT >= 2) {
        GET_CHECK_NUMBER(1, _is_raw, "UnSerializeObject parameter error: is_raw_buffer must be a number");
        is_raw = (bool)(int)_is_raw;
    }

    // setting the return to number, 0
    RETURN_NUMBER(0);
#ifdef WITH_LIBXML2
    LIBXML_TEST_VERSION

    void *old_ctx = xmlGenericErrorContext;
    xmlSetGenericErrorFunc(0, MyGenericErrorFunc);
    xmlDocPtr doc;
    if (is_raw) {
        doc = xmlReadMemory(filename, (int)len, "include.xml", NULL, 0);
        if (!doc)
            doc = xmlReadMemory(filename, (int)len, "include.xml", "UTF-8", 0);
    } else {
        doc = xmlReadFile(filename, NULL, 0);
        if (!doc)
            doc = xmlReadFile(filename, "UTF-8", 0);
    }

    if (!doc) {
        if (PARAMETERS_COUNT > 2) {
            SET_STRING(2, "UnSerializeObject failed to load the XML document");
        }
        xmlSetGenericErrorFunc(old_ctx, xmlGenericError);
        return 0;//return (void *)"UnSerializeObject failed to load the XML document";
    }

    xmlNodePtr node = xmlDocGetRootElement(doc);
#else
    pugi::xml_document     doc;
    pugi::xml_parse_result result;
    if (is_raw) {
        result = doc.load_buffer(filename, (int)len);
        if (result.status != pugi::status_ok)
            result = doc.load_buffer(filename, (int)len, pugi::parse_default, pugi::encoding_utf8);
    } else {
        result = doc.load_file(filename);
        if (result.status != pugi::status_ok)
            result = doc.load_file(filename, pugi::parse_default, pugi::encoding_utf8);
    }
    if (result.status != pugi::status_ok) {
        if (PARAMETERS_COUNT > 2) {
            char *description = (char *)result.description();
            SET_STRING(2, description);
        }
        return 0;
    }
    XML_NODE node = doc.root().first_child();
#endif
    RefContainer *rc = new RefContainer();
    rc->err_ser        = 0;
    rc->BACK_REF_COUNT = 0;
    rc->top_variable   = RESULT;
    rc->no_defaults    = 0;
#ifdef WITH_LIBXML2
    rc->doc = doc;
#else
    rc->doc = &doc;
#endif
    rc->err_ser   = 0;
    rc->is_buffer = 0;
    DoNode(rc, PARAMETERS->HANDLER, node, RESULT);
#ifdef WITH_LIBXML2
    xmlFreeDoc(doc);
    //xmlCleanupParser();
#endif

    if (PARAMETERS_COUNT > 2) {
        if (rc->err_ser) {
            SET_STRING(2, rc->err_ser);
        } else {
            SET_STRING(2, "");
        }
    }
#ifdef WITH_LIBXML2
    xmlSetGenericErrorFunc(old_ctx, xmlGenericError);
#endif
    if (rc)
        delete rc;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_CreateObject CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "CreateObject: CreateObject(szClassName)");

    LOCAL_INIT;
    char *class_name = 0;
    char *pData;

    GET_CHECK_STRING(0, class_name, "CreateObject parameter error: szClassName must be of string type(STATIC STRING)");

    if (!IS_OK(LocalInvoker(INVOKE_CREATE_OBJECT, PARAMETERS->HANDLER, RESULT, class_name))) {
        RETURN_NUMBER(0);
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_HasMember CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "HasMember: HasMember(Object,szMemberName)");

    LOCAL_INIT;
    char *membername = 0;
    char *pData;

    GET_ARRAY(0, pData);

    if (TYPE != VARIABLE_CLASS) {
        RETURN_NUMBER(0);
        return 0;
    }

    GET_CHECK_STRING(1, membername, "HasMember parameter error: szMemberName must be of string type(STATIC STRING)");

    int result = (Invoke(INVOKE_HAS_MEMBER, pData, membername) != 0);

    RETURN_NUMBER(result);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SetMember CONCEPT_API_PARAMETERS {
    RESET_ERROR

                                  PARAMETERS_CHECK(3, "SetMember: SetMember(Object,szMemberName,Value)");
    LOCAL_INIT;
    char *membername = 0;
    char *pData;

    char   *szData;
    NUMBER nData;

    GET_ARRAY(0, pData);

    if (TYPE != VARIABLE_CLASS) {
        RETURN_NUMBER(0);
        return 0;
    }

    GET_CHECK_STRING(1, membername, "SetMember parameter error: szMemberName must be of string type(STATIC STRING)");

    GET_BUFFER(2, szData, nData);

    RETURN_NUMBER(IS_OK(Invoke(INVOKE_SET_CLASS_MEMBER, pData, membername, TYPE, szData, nData)));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_GetMember CONCEPT_API_PARAMETERS {
    RESET_ERROR

                                  PARAMETERS_CHECK_MIN_MAX(3, 4, "GetMember: GetMember(Object,szMemberName,Value[, asdelegate=false])");
    LOCAL_INIT;
    char *membername = 0;
    char *pData;
    char as_delegate = 0;

    char   *szData = 0;
    NUMBER nData   = 0;

    GET_ARRAY(0, pData);

    if (TYPE != VARIABLE_CLASS) {
        RETURN_NUMBER(0);
        return 0;
    }

    GET_CHECK_STRING(1, membername, "GetMember parameter error: szMemberName must be of string type(STATIC STRING)");

    if (PARAMETERS->COUNT > 3) {
        NUMBER nVal;
        GET_CHECK_NUMBER(3, nVal, "GetMember parameter error: asdelegate must be of number type");
        if (nVal)
            as_delegate = 1;
    }

    TYPE = -1;
    if (as_delegate) {
        int is_ok = IS_OK(Invoke(INVOKE_CREATE_DELEGATE, LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], membername));
        RETURN_NUMBER(is_ok);
    } else {
        int is_ok = IS_OK(Invoke(INVOKE_GET_CLASS_MEMBER, pData, membername, &TYPE, &szData, &nData));
        RETURN_NUMBER(is_ok);
        if (is_ok)
            SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[2] - 1], TYPE, (char *)szData, nData);
        else {
            SET_NUMBER(2, 0);
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ObjectLinks CONCEPT_API_PARAMETERS {
    RESET_ERROR

                                    PARAMETERS_CHECK(1, "ObjectLinks: ObjectLinks(object|array|delegate)");
    LOCAL_INIT;

    RETURN_NUMBER((double)Invoke(INVOKE_OBJECT_LINKS, PARAMETER(0)));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_StaticLinks CONCEPT_API_PARAMETERS {
    RESET_ERROR

                                    PARAMETERS_CHECK(1, "StaticLinks: StaticLinks(any_variable_type)");
    LOCAL_INIT;

    RETURN_NUMBER((double)Invoke(INVOKE_VAR_LINKS, PARAMETER(0)));
    return 0;
}
//---------------------------------------------------------------------------
static AnsiString as_result;
CONCEPT_DLL_API CONCEPT__ASSERT CONCEPT_API_PARAMETERS {
    RESET_ERROR

                                PARAMETERS_CHECK_MIN_MAX(1, 2, "_ASSERT: _ASSERT(condition[, szErrMessage=\"\"])");
    LOCAL_INIT;

    as_result = "Assertion failed";

    char   *pData;
    NUMBER nLength;
    char   *err_data = 0;

    if (PARAMETERS_COUNT == 2) {
        GET_CHECK_STRING(1, err_data, "_ASSERT: szErrMessage should be of STATIC STRING type");
    }

    GET_BUFFER(0, pData, nLength);

    if (TYPE == VARIABLE_NUMBER) {
        if ((TYPE == VARIABLE_NUMBER) && (!nLength)) {
            if (err_data) {
                as_result += ": ";
                as_result += err_data;
            }
            RETURN_NUMBER(0);
            return (void *)as_result.c_str();
        } else {
            RETURN_NUMBER(1);
            return 0;
        }
    }

    RETURN_NUMBER(1);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__ClsPtr CONCEPT_API_PARAMETERS {
    RESET_ERROR
                                PARAMETERS_CHECK(1, "_ClsPtr: _ClsPtr(object)");

    LOCAL_INIT;

    char   *pData;
    NUMBER nLength;

    GET_BUFFER(0, pData, nLength);

    RETURN_NUMBER((long)pData);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__VarPtr CONCEPT_API_PARAMETERS {
    RESET_ERROR
                                PARAMETERS_CHECK(1, "_VarPtr: _VarPtr(variable)");

    LOCAL_INIT;

    RETURN_NUMBER((long)PARAMETER(0));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__KeySorted CONCEPT_API_PARAMETERS {
    RESET_ERROR
                                   PARAMETERS_CHECK(1, "_KeySorted: array _KeySorted(array)");

    LOCAL_INIT;

    char *arr  = 0;
    char *arr2 = 0;

    GET_CHECK_ARRAY(0, arr, "_KeySorted: paramter should be an array");

    if (IS_OK(Invoke(INVOKE_SORT_ARRAY_BY_KEYS, arr, &arr2))) {
        RETURN_ARRAY(arr2);
    } else
        return (void *)"_KeySorted: failed";

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__IsSet CONCEPT_API_PARAMETERS {
    RESET_ERROR
                               PARAMETERS_CHECK(2, "_IsSet: boolean _IsSet(array, nIndex|szKey)");

    LOCAL_INIT;

    char *arr  = 0;
    char *arr2 = 0;

    NUMBER len;

    INTEGER index;
    char    *key;

    GET_CHECK_ARRAY(0, arr, "_IsSet: paramter 1 should be an array");
    GET_BUFFER(1, key, len);

    index = (INTEGER)len;

    if (TYPE == VARIABLE_STRING)
        index = -1;
    else
    if (TYPE == VARIABLE_NUMBER)
        key = 0;
    else
        return (void *)"_IsSet: parameter 2 should be a number or a string";

    int result = Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, PARAMETER(0), index, key);

    if (IS_OK(result)) {
        RETURN_NUMBER(result);
    } else
        return (void *)"_IsSet: failed (may be not supported on your version. Please update.)";

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__GetKeys CONCEPT_API_PARAMETERS {
    RESET_ERROR
                                 PARAMETERS_CHECK(1, "_GetKeys: array _GetKeys(array)");

    LOCAL_INIT;

    char *arr  = 0;
    char *arr2 = 0;

    GET_CHECK_ARRAY(0, arr, "_GetKeys: paramter should be an array");

    char *key;
    int  count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));

    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < count; i++) {
        key = 0;
        Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(0), i, &key);
        if (key) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, VARIABLE_STRING, key, (NUMBER)0);
        } else {
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, VARIABLE_NUMBER, key, (NUMBER)0);
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ToXML, 1, 3)
    RESET_ERROR
    T_OBJECT(ToXML, 0)
    char *pData = PARAM(0);
#ifdef WITH_LIBXML2
    char *encoding = "UTF-8";
#else
    char *encoding = NULL;
#endif
    char is_simple = 1;
    int  format    = 1;

    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(ToXML, 1);
        if (PARAM_INT(1) != 0)
            is_simple = 2;
    }

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(ToXML, 2);
        format = PARAM_INT(2);
    }

    if (PARAMETERS_COUNT > 3) {
        T_STRING(ToXML, 3);
        encoding = PARAM(3);
    }

    RefContainer *rc = new RefContainer();
    rc->BACK_REF_COUNT = 0;
    rc->top_variable   = NULL;
    rc->is_buffer      = 0;
    rc->err_ser        = 0;
    rc->no_defaults    = 0;
#ifdef WITH_LIBXML2
    void *old_ctx = xmlGenericErrorContext;
    xmlSetGenericErrorFunc(0, MyGenericErrorFunc);

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    rc->doc = doc;
    Serialize(rc, pData, 0, is_simple);
#else
    pugi::xml_document doc;
    rc->doc = &doc;
    pugi::xml_node doc_element = doc.document_element();
    Serialize(rc, pData, XML_REF(doc_element), is_simple);
    pugi::xml_encoding t_encoding = pugi::encoding_utf8;
    if (encoding) {
        if (!strcasecmp(encoding, "UTF-16"))
            t_encoding = pugi::encoding_utf16;
        else
        if (!strcasecmp(encoding, "UTF-16LE"))
            t_encoding = pugi::encoding_utf16_le;
        else
        if (!strcasecmp(encoding, "UTF-16BE"))
            t_encoding = pugi::encoding_utf16_be;
        else
        if (!strcasecmp(encoding, "UTF-32"))
            t_encoding = pugi::encoding_utf32_be;
        else
        if (!strcasecmp(encoding, "UTF-32LE"))
            t_encoding = pugi::encoding_utf32_le;
        else
        if (!strcasecmp(encoding, "UTF-32BE"))
            t_encoding = pugi::encoding_utf32_be;
        else
        if (!strcasecmp(encoding, "UTF-16LE"))
            t_encoding = pugi::encoding_utf16_le;
        else
        if (!strcasecmp(encoding, "WCHAR"))
            t_encoding = pugi::encoding_wchar;
        else
        if (!strcasecmp(encoding, "LATIN1"))
            t_encoding = pugi::encoding_latin1;
    }
#endif
    DEBUG("All done");
    int is_file = false;
#ifdef WITH_LIBXML2
    xmlChar *mem = 0;
    int     size;

    if (format)
        xmlDocDumpFormatMemoryEnc(doc, &mem, &size, encoding, format);
    else
        xmlDocDumpMemoryEnc(doc, &mem, &size, encoding);

    if (size < 0)
        size = 0;

    RETURN_BUFFER((char *)mem, size);

    xmlFree(mem);
#else
    SimpleWriter writer;
    if (format)
        doc.save(writer, PUGIXML_TEXT("\t"), pugi::format_default, t_encoding);
    else
        doc.save(writer, PUGIXML_TEXT(""), pugi::format_raw, t_encoding);
    unsigned long size = writer.buffer.Length();
    char          *mem = writer.buffer.c_str();
    RETURN_BUFFER((char *)mem, size);
#endif
    if (is_file) {
        RETURN_NUMBER(0);
    }
#ifdef WITH_LIBXML2
    DEBUG("Clean ...");
    xmlFreeDoc(doc);
//xmlCleanupParser();
    xmlMemoryDump();
    DEBUG("done");
    xmlSetGenericErrorFunc(old_ctx, xmlGenericError);
#endif
    void *err_ser = NULL;
    if (rc) {
        err_ser = rc->err_ser;
        delete rc;
    }
    return err_ser;
END_IMPL
//---------------------------------------------------------------------------
void RecursiveNode(void *owner, INVOKE_CALL Invoke, pugi::xml_node xml_node) {
    pugi::xml_node child = xml_node.first_child();
    const pugi::xml_text text = xml_node.text();
    if ((!text) || (text.empty())) {
        void *list = NULL;
        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, owner, (char *)xml_node.name(), &list);
        if (list) {
            CREATE_ARRAY(list);
        }
        if (list) {
            INTEGER i = 0;
            for (pugi::xml_attribute attr = xml_node.first_attribute(); attr; attr = attr.next_attribute()) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, list, (char *)attr.name(), (INTEGER)VARIABLE_STRING, attr.value(), (NUMBER)0);
                i++;
            }

            if ((xml_node.empty()) || (!child))
                return;

            do {
                void *subarr = 0;
                Invoke(INVOKE_ARRAY_VARIABLE, list, i, &subarr);
                if (subarr) {
                    CREATE_ARRAY(subarr);
                    RecursiveNode(subarr, Invoke, child);
                }
                i++;
                child = child.next_sibling();
            } while (child);
        }
    } else {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, owner, (char *)xml_node.name(), (INTEGER)VARIABLE_STRING, text.get(), (NUMBER)0);
    }
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FromXML, 1, 2)
    T_STRING(FromXML, 0);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(PARAM(0));
    CREATE_ARRAY(RESULT);
    if (result) {
        pugi::xml_node xml_node = doc.first_child();
        if (xml_node)
            RecursiveNode(RESULT, Invoke, xml_node);
    } else {
        if (PARAMETERS_COUNT > 1) {
            SET_STRING(1, result.description());
        }
    }
END_IMPL
//---------------------------------------------------------------------------
void DoVariable(RefContainer *rc, char *member, int type, char *szData, NUMBER nData, void *class_data, void *variable_data, void *orig) {
    INVOKE_CALL Invoke = LocalInvoker;

    switch (type) {
        case VARIABLE_STRING:
        case VARIABLE_NUMBER:
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, variable_data, (char *)member, type, szData, nData);
            break;

        case VARIABLE_ARRAY:
            {
                void *subarr = 0;
                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, variable_data, (char *)member, &subarr);
                if (subarr) {
                    CREATE_ARRAY(subarr);
                    DoArray(rc, orig, subarr);
                }
            }
            break;

        case VARIABLE_DELEGATE:
            {
                INTEGER type1   = 0;
                char    *str1   = 0;
                NUMBER  nvalue1 = 0;
                if (IS_OK(Invoke(INVOKE_GET_VARIABLE, orig, &type1, &str1, &nvalue1)))
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, variable_data, (char *)member, type1, str1, nvalue1);
            }
            break;

        case VARIABLE_CLASS:
            {
                void *subarr = 0;
                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, variable_data, (char *)member, &subarr);
                if (subarr) {
                    CREATE_ARRAY(subarr);
                    DoObject(rc, (void *)class_data, subarr);
                }
            }
            break;
    }
}

//---------------------------------------------------------------------------
void DoArray(RefContainer *rc, void *pData, void *variable_data) {
    INVOKE_CALL Invoke  = LocalInvoker;
    int         ref_ptr = CheckBack(rc, pData);

    if (ref_ptr) {
        void *arr = rc->BACK_REFERENCES2[ref_ptr - 1];
        if (arr) {
            INTEGER type;
            char    *str = 0;
            NUMBER  nvalue;

            if (IS_OK(LocalInvoker(INVOKE_GET_VARIABLE, arr, &type, &str, &nvalue)))
                Invoke(INVOKE_SET_VARIABLE, variable_data, type, (char *)str, (NUMBER)nvalue);
        }
        return;
    } else {
        if ((rc->BACK_REF_COUNT > 0) && (rc->BACK_REF_COUNT <= MAX_OBJECTS))
            rc->BACK_REFERENCES2[rc->BACK_REF_COUNT - 1] = variable_data;
    }

    int count = LocalInvoker(INVOKE_GET_ARRAY_COUNT, pData);
    for (int i = 0; i < count; i++) {
        void    *newpData = 0;
        char    *key      = 0;
        INTEGER type      = 0;
        char    *szData   = 0;
        NUMBER  nData     = 0;
        LocalInvoker(INVOKE_ARRAY_VARIABLE, pData, i, &newpData);
        LocalInvoker(INVOKE_GET_ARRAY_KEY, pData, i, &key);
        if (newpData) {
            LocalInvoker(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            void *subarr = 0;
            if (key) {
                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, variable_data, (char *)key, &subarr);
            } else {
                Invoke(INVOKE_ARRAY_VARIABLE, variable_data, (INTEGER)i, &subarr);
            }
            if (subarr) {
                switch (type) {
                    case VARIABLE_STRING:
                    case VARIABLE_NUMBER:
                    case VARIABLE_DELEGATE:
                        Invoke(INVOKE_SET_VARIABLE, subarr, type, szData, nData);
                        break;

                    case VARIABLE_ARRAY:
                        {
                            void *subarr2 = 0;
                            if (key)
                                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, variable_data, (char *)key, &subarr2);
                            else
                                Invoke(INVOKE_ARRAY_VARIABLE, variable_data, (INTEGER)i, &subarr2);
                            if (subarr2) {
                                CREATE_ARRAY(subarr2);
                                DoArray(rc, newpData, subarr2);
                            }
                        }
                        break;

                    case VARIABLE_CLASS:
                        {
                            void *subarr2 = 0;
                            if (key)
                                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, variable_data, (char *)key, &subarr2);
                            else
                                Invoke(INVOKE_ARRAY_VARIABLE, variable_data, (INTEGER)i, &subarr2);
                            if (subarr2) {
                                CREATE_ARRAY(subarr2);
                                DoObject(rc, (void *)szData, subarr2);
                            }
                        }
                        break;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
void DoObject(RefContainer *rc, void *pData, void *parent, bool root_only) {
    INVOKE_CALL Invoke = LocalInvoker;
    char *class_name = 0;
    int  ref_ptr     = CheckBack(rc, pData);
    if (ref_ptr) {
        void *arr = rc->BACK_REFERENCES2[ref_ptr - 1];
        if (arr) {
            INTEGER type;
            char    *str = 0;
            NUMBER  nvalue;
            if (IS_OK(LocalInvoker(INVOKE_GET_VARIABLE, arr, &type, &str, &nvalue)))
                Invoke(INVOKE_SET_VARIABLE, parent, type, (char *)str, (NUMBER)nvalue);
        }
        return;
    } else {
        if ((rc->BACK_REF_COUNT > 0) && (rc->BACK_REF_COUNT <= MAX_OBJECTS))
            rc->BACK_REFERENCES2[rc->BACK_REF_COUNT - 1] = parent;
    }

    int members_count = LocalInvoker(INVOKE_GET_SERIAL_CLASS, pData, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);

    if (members_count > 0) {
        char   **members       = new char * [members_count];
        char   *flags          = new char[members_count];
        char   *access         = new char[members_count];
        char   *types          = new char[members_count];
        char   **szValues      = new char * [members_count];
        NUMBER *nValues        = new NUMBER[members_count];
        void   **class_data    = new void * [members_count];
        void   **variable_data = new void * [members_count];

        int result = LocalInvoker(rc->no_defaults ? INVOKE_GET_SERIAL_CLASS_NO_DEFAULTS : INVOKE_GET_SERIAL_CLASS, pData, members_count, &class_name, members, flags, access, types, (const char **)szValues, nValues, class_data, variable_data);

        if (result != 0) {
            rc->err_ser = (void *)"Error in serialization (bug ?)";
            return;
        }

        for (int i = 0; i < members_count; i++) {
            if (flags[i] == 0) {
                if (root_only) {
                    switch ((INTEGER)types[i]) {
                        case VARIABLE_STRING:
                        case VARIABLE_NUMBER:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, parent, members[i], (INTEGER)types[i], szValues[i], nValues[i]);
                            break;
                        default:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, parent, members[i], (INTEGER)types[i], class_data[i], nValues[i]);
                            break;
                    }
                } else
                    DoVariable(rc, members[i], types[i], szValues[i], nValues[i], class_data[i], parent, variable_data[i]);
            }
        }

        delete[] members;
        delete[] flags;
        delete[] access;
        delete[] types;
        delete[] szValues;
        delete[] nValues;
        delete[] class_data;
        delete[] variable_data;
    }
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ToArray, 1, 3)
    RESET_ERROR
    T_OBJECT(ToArray, 0)

    int no_defaults = 0;
    bool root_only = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(ToArray, 1)
        no_defaults = PARAM_INT(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(ToArray, 2)
        if (PARAM_INT(2))
            root_only = true;
    }

    CREATE_ARRAY(RESULT);

    RefContainer *rc = new RefContainer();
    rc->BACK_REF_COUNT = 0;
    rc->top_variable   = NULL;
    rc->is_buffer      = 0;
    rc->err_ser        = 0;
    rc->no_defaults    = no_defaults;
    DoObject(rc, PARAM(0), RESULT, root_only);
    delete rc;
END_IMPL
//---------------------------------------------------------------------------
int bin_write(RefContainer *rc, char *data, int d_size, int write_increment) {
    if ((d_size <= 0) || (!data))
        return 0;

    if (rc->offset + d_size > rc->size) {
        int blocks = 1;
        if (rc->increment) {
            int current_blocks = rc->size / write_increment;
            if (rc->increment > write_increment)
                blocks = rc->increment / write_increment + 1;

            if (blocks < current_blocks) {
                if (current_blocks < 1000)
                    blocks = current_blocks;
                else
                if (blocks < current_blocks / 2)
                    blocks = current_blocks / 2;
            }
            rc->increment = 0;
        }
        rc->size = ((rc->offset + d_size) / write_increment + blocks) * write_increment;
        rc->buf  = (char *)realloc(rc->buf, rc->size);
        if (!rc->buf) {
            rc->size = 0;
            rc->offset = 0;
        }
    }
    if (rc->buf) {
        memcpy(rc->buf + rc->offset, data, d_size);
        rc->offset += d_size;
        return d_size;
    }
    return -1;
}

//---------------------------------------------------------------------------
char *bin_read(RefContainer *rc, int d_size) {
    if (d_size <= 0)
        return NULL;

    if (rc->offset + d_size > rc->size)
        return NULL;

    char *res = rc->buf + rc->offset;
    rc->offset += d_size;
    return res;
}

//---------------------------------------------------------------------------
int bin_write_int(RefContainer *rc, int v) {
    int ref = htonl(v);

    return bin_write(rc, (char *)&ref, sizeof(ref));
}

//---------------------------------------------------------------------------
int bin_read_int(RefContainer *rc) {
    char *c = bin_read(rc, sizeof(int));

    if (!c)
        return 0;
    int ref = *(int *)c;
    return ntohl(ref);
}

//---------------------------------------------------------------------------
int bin_write_char(RefContainer *rc, char v) {
    return bin_write(rc, &v, 1);
}

//---------------------------------------------------------------------------
char bin_read_char(RefContainer *rc) {
    char *c = bin_read(rc, 1);

    if (!c)
        return 0;
    return c[0];
}

//---------------------------------------------------------------------------
int bin_write_double(RefContainer *rc, NUMBER v) {
    return bin_write(rc, (char *)&v, sizeof(NUMBER));
}

//---------------------------------------------------------------------------
double bin_read_double(RefContainer *rc) {
    char *c = bin_read(rc, sizeof(NUMBER));

    if (!c)
        return 0;
    return *(NUMBER *)c;
}

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
int bin_write_size(RefContainer *rc, unsigned long long v) {
    uint64_t tmp = v;
    char     buffer[0xF];
    char     *out_frame = buffer;
    int      size_len   = 0;

    if (tmp <= 0x7D) {
        out_frame[0] = tmp;
        size_len++;
    } else
    if (tmp < 0xFFFF) {
        out_frame[0] = 0x7E;
        out_frame++;
        *(unsigned short *)out_frame = htons((unsigned short)tmp);
        size_len += 3;
    } else {
        out_frame[0] = 0x7F;
        out_frame++;
        *(uint64_t *)out_frame = htonll2(tmp);
        size_len += 9;
    }
    bin_write(rc, buffer, size_len);
    return size_len;
}

//---------------------------------------------------------------------------
uint64_t bin_read_size(RefContainer *rc) {
    // to do
    char c    = bin_read_char(rc);
    char *buf = NULL;
    int  size = 0;

    switch (c) {
        case 0x7E:
            buf  = bin_read(rc, 2);
            size = 2;
            break;

        case 0x7F:
            buf  = bin_read(rc, 8);
            size = 8;
            break;

        default:
            if (c <= 0x7D)
                return c;
            return 0;
    }
    if (!buf)
        return 0;
    if (size == 2)
        return ntohs(*(unsigned short *)buf);
    else
        return htonll2(*(uint64_t *)buf);
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(BinarizeObject, 1, 2)
    char *szvar;
    void *var = LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1];

    int no_defaults = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(BinarizeObject, 1)
        no_defaults = PARAM_INT(1);
    }

    // need type
    GetVariable(var, &TYPE, &szvar, &nDUMMY_FILL);
    if ((TYPE != VARIABLE_CLASS) && (TYPE != VARIABLE_ARRAY))
        return (void *)"BinarizeObject: Parameter 1 must be an object or array";

    RefContainer rc_stack;
    RefContainer *rc = &rc_stack; // = new RefContainer();
    rc->BACK_REF_COUNT = 0;
    rc->top_variable   = NULL;
    rc->buf            = 0;
    rc->size           = 0;
    rc->offset         = 0;
    rc->doc            = 0;
    rc->err_ser        = 0;
    rc->is_buffer      = 1;
    rc->increment      = 0;
    rc->no_defaults    = no_defaults;

    bin_write_char(rc, TYPE);
    if (TYPE == VARIABLE_CLASS)
        Serialize(rc, szvar, 0);
    else
        SerializeArray(rc, var, szvar, NULL, 0);

    if ((rc->buf) && (rc->offset)) {
        RETURN_BUFFER(rc->buf, rc->offset);
    } else {
        RETURN_STRING("");
    }
    if (rc->buf)
        free(rc->buf);
    // delete rc;
END_IMPL
//---------------------------------------------------------------------------
int FilterContains(RefContainer *rc, AnsiString *name) {
    if ((rc->filters) && (rc->filters_count)) {
        for (int i = 0; i < rc->filters_count; i++) {
            char *ref = rc->filters[i];
            if ((ref) && (*name == ref)) {
                // don't use it again
                rc->filters[i] = NULL;
                rc->filters_used++;
                return 1;
            }
        }
        return 0;
    }
    return 1;
}

int DoBin(RefContainer *rc, void *ConceptHandler, void *OwnerPTR = 0, int dry_run = 0) {
    int type;

    do {
        type = bin_read_char(rc);
        // padding ?
    } while (type == 0x01);
    char       *temp = NULL;
    AnsiString buf;
    uint64_t   size   = 0;
    int        ref_id = 0;
    int        ref_dry_run;
    NUMBER     n;

    void *handler = 0;
    switch (type) {
        case VARIABLE_STRING:
            size = bin_read_size(rc);
            if (dry_run) {
                // skip it
                bin_read(rc, size);
            } else {
                temp = bin_read(rc, size);
                if ((size > 0) && (temp))
                    LocalInvoker(INVOKE_SET_VARIABLE, OwnerPTR, (INTEGER)VARIABLE_STRING, temp, (NUMBER)size);
                else
                    LocalInvoker(INVOKE_SET_VARIABLE, OwnerPTR, (INTEGER)VARIABLE_STRING, "", (NUMBER)0);
            }
            break;

        case VARIABLE_NUMBER:
            if (dry_run) {
                bin_read_double(rc);
            } else {
                n = bin_read_double(rc);
                LocalInvoker(INVOKE_SET_VARIABLE, OwnerPTR, (INTEGER)VARIABLE_NUMBER, (char *)0, n);
            }
            break;

        case VARIABLE_CLASS:
            ref_id = bin_read_int(rc);
            if (ref_id < 0) {
                if (!dry_run) {
                    ref_id *= -1;
                    if ((ref_id > 0) && (ref_id <= MAX_OBJECTS)) {
                        ref_id--;
                        LocalInvoker(INVOKE_SET_VARIABLE, OwnerPTR, (INTEGER)rc->BACK_TYPES[ref_id], rc->BACK_REFERENCES[ref_id], (NUMBER)0);
                    }
                }
            } else {
                size = bin_read_size(rc);
                temp = bin_read(rc, size);
                if ((size <= 0) || (!temp))
                    break;
                if (!dry_run) {
                    buf.LoadBuffer(temp, size);
                    if (!IS_OK(LocalInvoker(INVOKE_CREATE_OBJECT_NOCONSTRUCTOR, ConceptHandler, OwnerPTR, buf.c_str()))) {
                        rc->full_error  = (char *)"Unable to create class. Class ";
                        rc->full_error += buf.c_str();
                        rc->full_error += (char *)" undefined.";
                        rc->err_ser     = (void *)rc->full_error.c_str();
                        return -1;
                    }
                    if ((ref_id > 0) && (ref_id <= MAX_OBJECTS)) {
                        INTEGER type;
                        char    *str = 0;
                        NUMBER  nvalue;

                        LocalInvoker(INVOKE_GET_VARIABLE, OwnerPTR, &type, &str, &nvalue);

                        ref_id--;
                        rc->BACK_REFERENCES[ref_id] = str;
                        rc->BACK_REF_COUNT++;
                        rc->BACK_TYPES[ref_id] = VARIABLE_CLASS;
                    }
                }
                int msize = 0;
                do {
                    msize = bin_read_size(rc);
                    if (msize > 0) {
                        temp = bin_read(rc, msize);
                        if (!temp)
                            return -1;
                        ref_dry_run = dry_run;
                        if (!dry_run) {
                            buf.LoadBuffer(temp, msize);
                            if ((!rc->filters) || (FilterContains(rc, &buf))) {
                                if (!IS_OK(LocalInvoker(INVOKE_GET_CLASS_VARIABLE, OwnerPTR, (char *)buf.c_str(), &handler))) {
                                    rc->full_error = (char *)"Error in member serialization. Member not defined ?";
                                    rc->err_ser    = rc->full_error.c_str();
                                    return -1;
                                }
                                if (rc->filters)
                                    ref_dry_run = 0;
                            } else
                                ref_dry_run = 1;
                        }
                        if (DoBin(rc, ConceptHandler, handler, ref_dry_run) == -1)
                            return -1;
                        if ((ref_dry_run) && (rc->filters_used >= rc->filters_count))
                            return -1;
                    }
                } while (msize != 0);
            }
            break;

        case VARIABLE_ARRAY:
            ref_id = bin_read_int(rc);
            if (ref_id < 0) {
                if (!dry_run) {
                    ref_id *= -1;
                    if ((ref_id > 0) && (ref_id <= MAX_OBJECTS)) {
                        ref_id--;
                        LocalInvoker(INVOKE_SET_VARIABLE, OwnerPTR, (INTEGER)rc->BACK_TYPES[ref_id], rc->BACK_REFERENCES[ref_id], (NUMBER)0);
                    }
                }
            } else {
                if (!dry_run) {
                    LocalInvoker(INVOKE_CREATE_ARRAY, OwnerPTR, 0);
                    if ((ref_id > 0) && (ref_id <= MAX_OBJECTS)) {
                        INTEGER type;
                        char    *str = 0;
                        NUMBER  nvalue;

                        LocalInvoker(INVOKE_GET_VARIABLE, OwnerPTR, &type, &str, &nvalue);

                        ref_id--;
                        rc->BACK_REFERENCES[ref_id] = str;
                        rc->BACK_REF_COUNT++;
                        rc->BACK_TYPES[ref_id] = VARIABLE_ARRAY;
                    }
                }
                size = bin_read_size(rc);
                for (int i = 0; i < size; i++) {
                    uint64_t key_size = bin_read_size(rc);
                    ref_dry_run = dry_run;
                    if (key_size) {
                        temp = bin_read(rc, key_size);
                        if (!temp)
                            return -1;
                        if (dry_run) {
                            handler = NULL;
                        } else {
                            if (rc->filters)
                                buf.LoadBuffer(temp, key_size);
                            if ((!rc->filters) || (FilterContains(rc, &buf))) {
                                if ((!rc->filters) && (rc->offset < rc->size - 1)) {
                                    // add null_character at the end
                                    // ugly but faster when not using filters (no additional copying necessary to buf)
                                    char pushed_char = temp[key_size];
                                    temp[key_size] = 0;

                                    LocalInvoker(INVOKE_ARRAY_VARIABLE_BY_KEY, OwnerPTR, temp, &handler);
                                    // restore char
                                    temp[key_size] = pushed_char;
                                } else {
                                    if (!rc->filters)
                                        buf.LoadBuffer(temp, key_size);
                                    LocalInvoker(INVOKE_ARRAY_VARIABLE_BY_KEY, OwnerPTR, buf.c_str(), &handler);
                                }
                                if (rc->filters)
                                    ref_dry_run = 0;
                            } else
                                ref_dry_run = 1;
                        }
                        if (DoBin(rc, ConceptHandler, handler, ref_dry_run) == -1)
                            return -1;
                    } else {
                        if (rc->filters)
                            ref_dry_run = dry_run;

                        if (!ref_dry_run)
                            LocalInvoker(INVOKE_ARRAY_VARIABLE, OwnerPTR, i, &handler);
                        if (DoBin(rc, ConceptHandler, handler, ref_dry_run) == -1)
                            return -1;
                    }
                    if ((ref_dry_run) && (rc->filters_used >= rc->filters_count))
                        return -1;
                }
            }
            break;

        case VARIABLE_DELEGATE:
            // class
            size = bin_read_size(rc);
            temp = bin_read(rc, size);
            // member
            size = bin_read_size(rc);
            temp = bin_read(rc, size);
            break;
    }
    return 1;
}

//---------------------------------------------------------------------------
char **GetCharList2(void *arr, int *outcount, INVOKE_CALL _Invoke) {
    INTEGER type      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;
    char    **ret     = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    if (count <= 0) {
        *outcount = 0;
        return 0;
    }
    *outcount = count;
    ret       = (char **)malloc(sizeof(char *) * count);

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i] = szData;
            } else
                ret[i] = 0;
        }
    }
    return ret;
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(UnBinarizeObject, 1, 3)
    T_STRING(UnBinarizeObject, 0)

    RETURN_NUMBER(0);

    uint64_t offset = 0;

    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(UnBinarizeObject, 1)
        offset = (SYS_INT)PARAM(1);
        if ((offset < 0) || (offset >= PARAM_LEN(0)))
            return (void *)"UnBinarizeObject: invalid offset";
    }
    RefContainer rc_stack;
    RefContainer *rc = &rc_stack; // = new RefContainer();
    rc->BACK_REF_COUNT = 0;
    rc->top_variable   = RESULT;
    rc->doc            = NULL;
    rc->buf            = PARAM(0) + offset;
    rc->size           = PARAM_LEN(0) - offset;
    rc->offset         = 0;
    rc->is_buffer      = 1;
    rc->filters_set    = 0;
    rc->filters        = NULL;
    rc->filters_count  = 0;
    rc->filters_used   = 0;
    rc->no_defaults    = 0;
    if (PARAMETERS_COUNT > 2) {
        T_ARRAY(UnBinarizeObject, 2)
        rc->filters = GetCharList2(PARAMETER(2), &rc->filters_count, Invoke);
        if (rc->filters)
            rc->filter_set = 1;
    }

    DoBin(rc, PARAMETERS->HANDLER, RESULT);
    if (rc->filters)
        free(rc->filters);
    if (PARAMETERS_COUNT > 3) {
        if (rc->err_ser) {
            SET_STRING(3, rc->err_ser);
        } else {
            SET_STRING(3, "");
        }
    }
    // if (rc)
    //    delete rc;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ToSize, 1)
    T_NUMBER(ToSize, 0)

    uint64_t tmp = (uint64_t)PARAM(0);
    char buffer[0xF];
    char *out_frame = buffer;
    int  size_len   = 0;

    if (tmp <= 0x7D) {
        out_frame[0] = tmp;
        size_len++;
    } else
    if (tmp < 0xFFFF) {
        out_frame[0] = 0x7E;
        out_frame++;
        *(unsigned short *)out_frame = htons((unsigned short)tmp);
        size_len += 3;
    } else {
        out_frame[0] = 0x7F;
        out_frame++;
        *(uint64_t *)out_frame = htonll2(tmp);
        size_len += 9;
    }
    RETURN_BUFFER(buffer, size_len);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FromSize, 1, 2)
    T_STRING(FromSize, 0)

    char *buf_in = PARAM(0);
    int buf_len = PARAM_LEN(0);
    if (buf_len <= 0) {
        RETURN_NUMBER(-1);
        return 0;
    }

    char c    = buf_in[0];
    char *buf = NULL;
    int  size = 0;

    switch (c) {
        case 0x7E:
            if (buf_len < 3) {
                RETURN_NUMBER(-1);
                return 0;
            }
            buf  = buf_in + 1;
            size = 2;
            break;

        case 0x7F:
            if (buf_len < 9) {
                RETURN_NUMBER(-1);
                return 0;
            }
            buf  = buf_in + 1;
            size = 8;
            break;

        default:
            if (c <= 0x7D) {
                if (PARAMETERS_COUNT > 1) {
                    SET_NUMBER(1, 1);
                }
                RETURN_NUMBER(c);
                return 0;
            }
            RETURN_NUMBER(-1);
            return 0;
    }
    if (!buf) {
        RETURN_NUMBER(-1);
        return 0;
    }
    double res;
    if (size == 2)
        res = ntohs(*(unsigned short *)buf);
    else
        res = htonll2(*(uint64_t *)buf);
    if (PARAMETERS_COUNT > 1) {
        if (size > 1)
            size++;
        SET_NUMBER(1, size);
    }
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_VARIABLE_PARAMS(CreateObject2, 1)
    T_STRING(CreateObject2, 0)
    int cnt = PARAMETERS_COUNT;
    if (cnt > 1) {
        RETURN_NUMBER(0);
        INTEGER *PARAMS = (INTEGER *)malloc(sizeof(INTEGER) * cnt);
        for (int i = 1; i < cnt; i++)
            PARAMS[i - 1] = PARAMETERS->PARAM_INDEX[i];
        PARAMS[cnt - 1] = -1;

        Invoke(INVOKE_CREATE_OBJECT_3, PARAMETERS->HANDLER, RESULT, PARAM(0), LOCAL_CONTEXT, PARAMS);
        free(PARAMS);
    } else {
        if (!IS_OK(LocalInvoker(INVOKE_CREATE_OBJECT, PARAMETERS->HANDLER, RESULT, PARAM(0)))) {
            RETURN_NUMBER(0);
        }
    }
    return 0;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_VARIABLE_PARAMS(pack, 1)
    T_STRING(pack, 0)
    char *format = PARAM(0);
    int         len = PARAM_LEN(0);
    size_t      size;
    int         param_index = 1;
    int         buf_size    = PACK_BUFFER_INCREMENT;
    signed char *buf        = (signed char *)malloc(buf_size);
    int         buf_index   = 0;
    char        size_buf[11];
    int         size_pos;
    int         padding;
    int         used_size;
    size_t      tmp_len;
    for (int i = 0; i < len; i++) {
        char f = format[i];
        if (f == ':') {
            size_pos = -1;
            do {
                size_pos++;
                i++;
                if (i >= len)
                    break;
                size = format[i];
                size_buf[size_pos] = size;
            } while ((size >= '0') && (size <= '9') && (size_pos <= 10));
            size_buf[size_pos] = 0;
            i--;
            size_pos = atoi(size_buf);
            if (size_pos < 0) {
                PACK_PANIC;
            }
            if ((size_pos > 0) && (buf_index < size_pos)) {
                size_pos -= buf_index;
                PACK_ENSURE_BUFFER(size_pos)
                memset(&buf[buf_index], 0, size_pos);
                buf_index += size_pos;
            }
            continue;
        }
        if (param_index >= PARAMETERS_COUNT) {
            PACK_PANIC
        }
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], &TYPE, &szDUMMY_FILL, &nDUMMY_FILL);
        param_index++;
        switch (f) {
            case 'i':
            case 'I':
            case 'u':
            case 'U':
                if (TYPE == VARIABLE_STRING) {
                    nDUMMY_FILL = atof(szDUMMY_FILL);
                    TYPE        = VARIABLE_NUMBER;
                }
                if ((TYPE != VARIABLE_NUMBER) && (TYPE != VARIABLE_ARRAY))
                    PACK_PANIC
                        i++;
                CHECK_PACK_SIZE(i, len);
                size = format[i] - '0';
                if (size == 8) {
                    // valid data
                    if (TYPE == VARIABLE_ARRAY) {
                        PACK_ARRAY(1,
                            if ((f == 'i') || (f == 'I'))
                                buf[buf_index++] = (char)nDUMMY_FILL;
                            else
                                ((unsigned char *)buf)[buf_index++] = (unsigned char)nDUMMY_FILL;
                        );
                    } else {
                        PACK_ENSURE_BUFFER(1)
                        if ((f == 'i') || (f == 'I'))
                            buf[buf_index++] = (char)nDUMMY_FILL;
                        else
                            ((unsigned char *)buf)[buf_index++] = (unsigned char)nDUMMY_FILL;
                    }
                } else
                if (size == 1) {
                    i++;
                    CHECK_PACK_SIZE(i, len);
                    size = format[i] - '0';
                    if (size != 6) {
                        RAISE_PACK_ERROR(-3);
                    }
                    // valid data
                    if (TYPE == VARIABLE_ARRAY) {
                        PACK_ARRAY(2,
                            if (((f == 'i') && (is_little_endian)) || ((f == 'I') && (!is_little_endian))) {
                                *(int16_t *)(&buf[buf_index]) = (int16_t)nDUMMY_FILL;
                            } else
                            if (((f == 'i') && (!is_little_endian)) || ((f == 'I') && (is_little_endian))) {
                                *(int16_t *)(&buf[buf_index]) = swap_int16((int16_t)nDUMMY_FILL);
                            } else
                            if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                                *(uint16_t *)(&buf[buf_index]) = (uint16_t)nDUMMY_FILL;
                            } else
                                *(uint16_t *)(&buf[buf_index]) = swap_uint16((uint16_t)nDUMMY_FILL);
                            buf_index += 2;
                        );
                    } else {
                        PACK_ENSURE_BUFFER(2)
                        if (((f == 'i') && (is_little_endian)) || ((f == 'I') && (!is_little_endian))) {
                            *(int16_t *)(&buf[buf_index]) = (int16_t)nDUMMY_FILL;
                        } else
                        if (((f == 'i') && (!is_little_endian)) || ((f == 'I') && (is_little_endian))) {
                            *(int16_t *)(&buf[buf_index]) = swap_int16((int16_t)nDUMMY_FILL);
                        } else
                        if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                            *(uint16_t *)(&buf[buf_index]) = (uint16_t)nDUMMY_FILL;
                        } else
                            *(uint16_t *)(&buf[buf_index]) = swap_uint16((uint16_t)nDUMMY_FILL);
                        buf_index += 2;
                    }
                } else
                if (size == 3) {
                    i++;
                    CHECK_PACK_SIZE(i, len);
                    size = format[i] - '0';
                    if (size != 2) {
                        RAISE_PACK_ERROR(-3);
                    }
                    // valid data
                    if (TYPE == VARIABLE_ARRAY) {
                        PACK_ARRAY(4,
                            if (((f == 'i') && (is_little_endian)) || ((f == 'I') && (!is_little_endian))) {
                                *(int32_t *)(&buf[buf_index]) = (int32_t)nDUMMY_FILL;
                            } else
                            if (((f == 'i') && (!is_little_endian)) || ((f == 'I') && (is_little_endian))) {
                                *(int32_t *)(&buf[buf_index]) = swap_int32((int32_t)nDUMMY_FILL);
                            } else
                            if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                                *(uint32_t *)(&buf[buf_index]) = (uint32_t)nDUMMY_FILL;
                            } else
                                *(uint32_t *)(&buf[buf_index]) = swap_uint32((uint32_t)nDUMMY_FILL);
                            buf_index += 4;
                        );
                    } else {
                        PACK_ENSURE_BUFFER(4)
                        if (((f == 'i') && (is_little_endian)) || ((f == 'I') && (!is_little_endian))) {
                            *(int32_t *)(&buf[buf_index]) = (int32_t)nDUMMY_FILL;
                        } else
                        if (((f == 'i') && (!is_little_endian)) || ((f == 'I') && (is_little_endian))) {
                            *(int32_t *)(&buf[buf_index]) = swap_int32((int32_t)nDUMMY_FILL);
                        } else
                        if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                            *(uint32_t *)(&buf[buf_index]) = (uint32_t)nDUMMY_FILL;
                        } else
                            *(uint32_t *)(&buf[buf_index]) = swap_uint32((uint32_t)nDUMMY_FILL);
                        buf_index += 4;
                    }
                } else
                if (size == 6) {
                    i++;
                    CHECK_PACK_SIZE(i, len);
                    size = format[i] - '0';
                    if (size != 4) {
                        RAISE_PACK_ERROR(-3);
                    }
                    // valid data
                    if (TYPE == VARIABLE_ARRAY) {
                        PACK_ARRAY(8,
                            if (((f == 'i') && (is_little_endian)) || ((f == 'I') && (!is_little_endian))) {
                                *(int64_t *)(&buf[buf_index]) = (int64_t)nDUMMY_FILL;
                            } else
                            if (((f == 'i') && (!is_little_endian)) || ((f == 'I') && (is_little_endian))) {
                                *(int64_t *)(&buf[buf_index]) = swap_int64((int64_t)nDUMMY_FILL);
                            } else
                            if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                                *(uint64_t *)(&buf[buf_index]) = (uint64_t)nDUMMY_FILL;
                            } else
                                *(uint64_t *)(&buf[buf_index]) = swap_uint64((uint64_t)nDUMMY_FILL);
                            buf_index += 8;
                        );
                    } else {
                        PACK_ENSURE_BUFFER(8)
                        if (((f == 'i') && (is_little_endian)) || ((f == 'I') && (!is_little_endian))) {
                            *(int64_t *)(&buf[buf_index]) = (int64_t)nDUMMY_FILL;
                        } else
                        if (((f == 'i') && (!is_little_endian)) || ((f == 'I') && (is_little_endian))) {
                            *(int64_t *)(&buf[buf_index]) = swap_int64((int64_t)nDUMMY_FILL);
                        } else
                        if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                            *(uint64_t *)(&buf[buf_index]) = (uint64_t)nDUMMY_FILL;
                        } else
                            *(uint64_t *)(&buf[buf_index]) = swap_uint64((uint64_t)nDUMMY_FILL);
                        buf_index += 8;
                    }
                } else
                if (size == 2) {
                    i++;
                    CHECK_PACK_SIZE(i, len);
                    size = format[i] - '0';
                    if (size != 4) {
                        RAISE_PACK_ERROR(-3);
                    }
                    if (TYPE == VARIABLE_ARRAY) {
                        PACK_ARRAY(3,
                            unsigned int i = (unsigned int)nDUMMY_FILL;
                            if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                                buf[buf_index + 2] = i / 0x10000;
                                i %= 0x10000;
                                buf[buf_index + 1] = i / 0x100;
                                i %= 0x100;
                                buf[buf_index] = i;
                            } else {
                                buf[buf_index] = i / 0x10000;
                                i %= 0x10000;
                                buf[buf_index + 1] = i / 0x100;
                                i %= 0x100;
                                buf[buf_index + 2] = i;
                            }
                            buf_index += 3;
                        );
                    } else {
                        PACK_ENSURE_BUFFER(3)
                        unsigned int i = (unsigned int)nDUMMY_FILL;
                        if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                            buf[buf_index + 2] = i / 0x10000;
                            i %= 0x10000;
                            buf[buf_index + 1] = i / 0x100;
                            i %= 0x100;
                            buf[buf_index] = i;
                        } else {
                            buf[buf_index] = i / 0x10000;
                            i %= 0x10000;
                            buf[buf_index + 1] = i / 0x100;
                            i %= 0x100;
                            buf[buf_index + 2] = i;
                        }
                        buf_index += 3;
                    }
                } else {
                    RAISE_PACK_ERROR(-3)
                }
                break;

            case 'f':
                if (TYPE == VARIABLE_STRING) {
                    nDUMMY_FILL = atof(szDUMMY_FILL);
                    TYPE        = VARIABLE_NUMBER;
                } else
                if (TYPE == VARIABLE_ARRAY) {
                    PACK_ARRAY(4,
                        *(float *)(&buf[buf_index]) = (float)nDUMMY_FILL;
                        buf_index += 4;
                    )
                } else
                if (TYPE != VARIABLE_NUMBER)
                    PACK_PANIC
                    else {
                        PACK_ENSURE_BUFFER(4)
                        // valid data
                        * (float *)(&buf[buf_index]) = (float)nDUMMY_FILL;
                        buf_index += 4;
                    }
                break;

            case 'd':
                if (TYPE == VARIABLE_STRING) {
                    nDUMMY_FILL = atof(szDUMMY_FILL);
                    TYPE        = VARIABLE_NUMBER;
                } else
                if (TYPE == VARIABLE_ARRAY) {
                    PACK_ARRAY(8,
                               *(double *)(&buf[buf_index]) = (double)nDUMMY_FILL;
                               buf_index += 8;
                               )
                } else
                if (TYPE != VARIABLE_NUMBER) {
                    PACK_PANIC
                } else {
                    PACK_ENSURE_BUFFER(8)
                    // valid data
                    * (double *)(&buf[buf_index]) = (double)nDUMMY_FILL;
                    buf_index += 8;
                }
                break;

            case 's':
                // to do
                size_pos = 0;
                do {
                    i++;
                    if (i >= len)
                        break;
                    size = format[i];
                    if ((size < '0') || (size > '9'))
                        break;
                    size_buf[size_pos] = size;
                    size_pos++;
                } while (size_pos <= 10);

                size = 0;
                if (size_pos > 0) {
                    size_buf[size_pos] = 0;
                    size = atoi(size_buf);
                }
                i--;
                padding = 0;
                if (TYPE == VARIABLE_ARRAY) {
                    void *pData = LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index - 1] - 1];
                    int  count  = LocalInvoker(INVOKE_GET_ARRAY_COUNT, pData);
                    if (count) {
                        for (INTEGER j = 0; j < count; j++) {
                            void *elem_data;
                            LocalInvoker(INVOKE_ARRAY_VARIABLE, pData, j, &elem_data);
                            if (elem_data) {
                                LocalInvoker(INVOKE_GET_VARIABLE, elem_data, &TYPE, &szDUMMY_FILL, &nDUMMY_FILL);
                                if (TYPE == VARIABLE_STRING) {
                                    tmp_len = (size_t)nDUMMY_FILL;
                                    padding = 0;
                                    if (size > 0) {
                                        if (tmp_len >= size)
                                            tmp_len = size;
                                        else
                                            padding = size - tmp_len;
                                        PACK_ENSURE_BUFFER(size)
                                    } else {
                                        PACK_ENSURE_BUFFER(tmp_len)
                                    }
                                    memcpy(&buf[buf_index], szDUMMY_FILL, tmp_len);
                                    if (padding)
                                        memset(&buf[buf_index + tmp_len], 0, padding);
                                    buf_index += tmp_len + padding;
                                } else
                                if (TYPE == VARIABLE_NUMBER) {
                                    AnsiString tmp(nDUMMY_FILL);
                                    tmp_len = tmp.Length();
                                    padding = 0;

                                    /*if ((size > 0) && (tmp_len > size))
                                        tmp_len = size;
                                       PACK_ENSURE_BUFFER(tmp_len)*/
                                    if (size > 0) {
                                        if (tmp_len >= size)
                                            tmp_len = size;
                                        else
                                            padding = size - tmp_len;

                                        PACK_ENSURE_BUFFER(size)
                                    } else {
                                        PACK_ENSURE_BUFFER(tmp_len)
                                    }
                                    memcpy(&buf[buf_index], tmp.c_str(), tmp_len);
                                    if (padding)
                                        memset(&buf[buf_index + tmp_len], 0, padding);
                                    buf_index += tmp_len + padding;
                                }
                            }
                        }
                    }
                } else
                if (TYPE == VARIABLE_STRING) {
                    tmp_len = (size_t)nDUMMY_FILL;
                    if (size > 0) {
                        if (tmp_len >= size)
                            tmp_len = size;
                        else
                            padding = size - tmp_len;
                        PACK_ENSURE_BUFFER(size)
                    } else {
                        PACK_ENSURE_BUFFER(tmp_len)
                    }
                    memcpy(&buf[buf_index], szDUMMY_FILL, tmp_len);
                    if (padding)
                        memset(&buf[buf_index + tmp_len], 0, padding);
                    buf_index += tmp_len + padding;
                } else
                if (TYPE == VARIABLE_NUMBER) {
                    AnsiString tmp(nDUMMY_FILL);
                    int        tmp_len = tmp.Length();

                    if (size > 0) {
                        if (tmp_len >= size)
                            tmp_len = size;
                        else
                            padding = size - tmp_len;

                        PACK_ENSURE_BUFFER(size)
                    } else {
                        PACK_ENSURE_BUFFER(tmp_len)
                    }
                    memcpy(&buf[buf_index], tmp.c_str(), tmp_len);
                    if (padding)
                        memset(&buf[buf_index + tmp_len], 0, padding);
                    buf_index += tmp_len + padding;
                } else {
                    PACK_PANIC;
                }
                break;

            case '#':
                break;

            case '*':
                size_pos = 0;
                do {
                    i++;
                    if (i >= len)
                        break;
                    size = format[i];
                    if ((size < '0') || (size > '9'))
                        break;
                    size_buf[size_pos] = size;
                    size_pos++;
                } while (size_pos <= 10);

                size = 0;
                if (size_pos > 0) {
                    size_buf[size_pos] = 0;
                    size = atoi(size_buf);
                }
                if (size < 8)
                    size = 8;
                i--;
                used_size      = 0;
                buf[buf_index] = TYPE;
                if (TYPE == VARIABLE_ARRAY) {
                    void *pData = LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index - 1] - 1];
                    nDUMMY_FILL = LocalInvoker(INVOKE_GET_ARRAY_COUNT, pData);
                    PACK_ENSURE_BUFFER(size + 1)
                    // valid data
                    * (double *)(&buf[buf_index + 1]) = (double)nDUMMY_FILL;
                    used_size = 8;
                } else
                if (TYPE == VARIABLE_STRING) {
                    used_size = (int)nDUMMY_FILL;
                    if (used_size > size)
                        used_size = size;
                    memcpy(&buf[buf_index + 1], szDUMMY_FILL, used_size);
                } else
                if (TYPE == VARIABLE_NUMBER) {
                    PACK_ENSURE_BUFFER(9)
                    // valid data
                    * (double *)(&buf[buf_index + 1]) = (double)nDUMMY_FILL;
                    used_size = 8;
                }

                if (used_size < size)
                    memset(&buf[buf_index + used_size + 1], 0, size - used_size);

                buf_index += size + 1;
                break;

            default:
                // unknown format
                RAISE_PACK_ERROR(-1)
        }
    }
    RETURN_BUFFER((char *)buf, buf_index);
    free(buf);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(unpack, 2, 3)
    T_STRING(unpack, 0)
    T_STRING(unpack, 1)
    CREATE_ARRAY(RESULT);
    char        *format = PARAM(0);
    int         len     = PARAM_LEN(0);
    signed char *buffer = (signed char *)PARAM(1);
    signed char *orig   = buffer;
    int         in_len  = PARAM_LEN(1);
    INTEGER     index   = 0;
    int         size;
    char        size_buf[11];
    int         size_pos;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(unpack, 2)
        int offset = PARAM_INT(2);
        if (offset < 0)
            offset = 0;
        in_len -= offset;
        if (in_len <= 0)
            return 0;

        buffer += offset;
    }

    for (int i = 0; i < len; i++) {
        char f = format[i];
        switch (f) {
            case 'i':
            case 'I':
            case 'u':
            case 'U':
                i++;
                if (i >= len)
                    return 0;
                size = format[i] - '0';
                if (size == 8) {
                    if (in_len < 1)
                        return 0;

                    if ((f == 'i') || (f == 'I'))
                        nDUMMY_FILL = (char)*buffer;
                    else
                        nDUMMY_FILL = (unsigned char)*buffer;
                    buffer++;
                    in_len--;
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", (NUMBER)nDUMMY_FILL);
                    index++;
                } else
                if (size == 1) {
                    i++;
                    if (i >= len)
                        return 0;
                    size = format[i] - '0';
                    if ((size != 6) || (in_len < 2))
                        return 0;

                    if (((f == 'i') && (is_little_endian)) || ((f == 'I') && (!is_little_endian))) {
                        nDUMMY_FILL = *(int16_t *)buffer;
                    } else
                    if (((f == 'i') && (!is_little_endian)) || ((f == 'I') && (is_little_endian))) {
                        nDUMMY_FILL = swap_int16(*(int16_t *)buffer);
                    } else
                    if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                        nDUMMY_FILL = *(uint16_t *)buffer;
                    } else
                        nDUMMY_FILL = swap_uint16(*(uint16_t *)buffer);

                    buffer += 2;
                    in_len -= 2;
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", (NUMBER)nDUMMY_FILL);
                    index++;
                } else
                if (size == 3) {
                    i++;
                    if (i >= len)
                        return 0;
                    size = format[i] - '0';
                    if ((size != 2) || (in_len < 4))
                        return 0;

                    if (((f == 'i') && (is_little_endian)) || ((f == 'I') && (!is_little_endian))) {
                        nDUMMY_FILL = *(int32_t *)buffer;
                    } else
                    if (((f == 'i') && (!is_little_endian)) || ((f == 'I') && (is_little_endian))) {
                        nDUMMY_FILL = swap_int32(*(int32_t *)buffer);
                    } else
                    if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                        nDUMMY_FILL = *(uint32_t *)buffer;
                    } else
                        nDUMMY_FILL = swap_uint32(*(uint32_t *)buffer);

                    buffer += 4;
                    in_len -= 4;
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", (NUMBER)nDUMMY_FILL);
                    index++;
                } else
                if (size == 6) {
                    i++;
                    if (i >= len)
                        return 0;
                    size = format[i] - '0';
                    if ((size != 4) || (in_len < 8))
                        return 0;

                    if (((f == 'i') && (is_little_endian)) || ((f == 'I') && (!is_little_endian))) {
                        nDUMMY_FILL = *(int64_t *)buffer;
                    } else
                    if (((f == 'i') && (!is_little_endian)) || ((f == 'I') && (is_little_endian))) {
                        nDUMMY_FILL = swap_int64(*(int64_t *)buffer);
                    } else
                    if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian))) {
                        nDUMMY_FILL = *(uint64_t *)buffer;
                    } else
                        nDUMMY_FILL = swap_uint64(*(uint64_t *)buffer);

                    buffer += 8;
                    in_len -= 8;
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", (NUMBER)nDUMMY_FILL);
                    index++;
                } else
                if (size == 2) {
                    i++;
                    if (i >= len)
                        return 0;
                    size = format[i] - '0';
                    if ((size != 4) || (in_len < 3))
                        return 0;

                    if (((f == 'u') && (is_little_endian)) || ((f == 'U') && (!is_little_endian)))
                        nDUMMY_FILL = (unsigned char)buffer[2] * 0x10000 + (unsigned char)buffer[1] * 0x100 + (unsigned char)buffer[0];
                    else
                        nDUMMY_FILL = (unsigned char)buffer[0] * 0x10000 + (unsigned char)buffer[1] * 0x100 + (unsigned char)buffer[2];

                    buffer += 3;
                    in_len -= 3;
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", (NUMBER)nDUMMY_FILL);
                    index++;
                } else
                    return 0;
                break;

            case 'f':
                if (in_len < 4)
                    return 0;
                nDUMMY_FILL = *(float *)buffer;
                buffer     += 4;
                in_len     -= 4;
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", (NUMBER)nDUMMY_FILL);
                index++;
                break;

            case 'd':
                if (in_len < 8)
                    return 0;
                nDUMMY_FILL = *(double *)buffer;
                buffer     += 8;
                in_len     -= 8;
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", (NUMBER)nDUMMY_FILL);
                index++;
                break;

            case 's':
                size_pos = -1;
                do {
                    size_pos++;
                    i++;
                    if (i >= len)
                        break;
                    size = format[i];
                    size_buf[size_pos] = size;
                } while ((size >= '0') && (size <= '9') && (size_pos <= 10));
                size_buf[size_pos] = 0;
                i--;
                size = atoi(size_buf);
                if (size < 0)
                    return 0;
                if (size > in_len)
                    size = in_len;
                // a s0 size is a no-op
                if (size) {
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_STRING, buffer, (NUMBER)size);
                    buffer += size;
                    in_len -= size;
                } else
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_STRING, "", (NUMBER)0);
                index++;
                break;

            case '*':
                size_pos = -1;
                do {
                    size_pos++;
                    i++;
                    if (i >= len)
                        break;
                    size = format[i];
                    size_buf[size_pos] = size;
                } while ((size >= '0') && (size <= '9') && (size_pos <= 10));
                size_buf[size_pos] = 0;
                size = atoi(size_buf);
                i--;
                if (size < 8)
                    size = 8;
                TYPE = *buffer;
                if (in_len < size) {
                    in_len = 0;
                    i      = len;
                    break;
                }
                if (TYPE == VARIABLE_STRING) {
                    int size2 = size;
                    while ((size2) && (!buffer[size2]))
                        size2--;
                    if (size2)
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_STRING, buffer + 1, (NUMBER)size2);
                    else
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_STRING, "", (NUMBER)0);
                } else
                if ((TYPE == VARIABLE_NUMBER) || (TYPE == VARIABLE_ARRAY)) {
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", *(NUMBER *)(buffer + 1));
                } else
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", (NUMBER)0);
                size++;
                buffer += size;
                in_len -= size;
                index++;
                break;

            case ':':
                size_pos = -1;
                do {
                    size_pos++;
                    i++;
                    if (i >= len)
                        break;
                    size = format[i];
                    size_buf[size_pos] = size;
                } while ((size >= '0') && (size <= '9') && (size_pos <= 10));
                size_buf[size_pos] = 0;
                i--;
                size_pos = atoi(size_buf);
                if (size_pos < 0)
                    return 0;

                size = buffer - orig;
                if ((size_pos > 0) && (size < size_pos)) {
                    size_pos -= size;
                    buffer   += size_pos;
                    in_len   -= size_pos;
                }
                break;

            case '#':
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, VARIABLE_NUMBER, "", (NUMBER)0);
                index++;
                break;
        }
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(compress, 1, 3)
    T_STRING(compress, 0)
    mz_ulong max_len = compressBound(PARAM_LEN(0));
    int level = -1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(compress, 1);
        level = PARAM_INT(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(compress, 2)
        max_len = PARAM_INT(2);
    }
    char *out_buf;
    CORE_NEW(max_len + 1, out_buf);
    if (out_buf) {
        out_buf[max_len] = 0;
        int res;
        if (level >= 0)
            res = compress2((unsigned char *)out_buf, &max_len, (unsigned char *)PARAM(0), PARAM_LEN(0), level);
        else
            res = compress((unsigned char *)out_buf, &max_len, (unsigned char *)PARAM(0), PARAM_LEN(0));
        if (res == Z_OK) {
            SetVariable(RESULT, -1, out_buf, max_len);
        } else {
            CORE_DELETE(out_buf);
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(uncompress, 1, 2)
    T_STRING(uncompress, 0)
    mz_ulong max_len = PARAM_LEN(0) * 20;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(uncompress, 1)
        max_len = PARAM_INT(1);
    }
    char *out_buf;
    CORE_NEW(max_len + 1, out_buf);
    if (out_buf) {
        out_buf[max_len] = 0;
        int res = uncompress((unsigned char *)out_buf, &max_len, (unsigned char *)PARAM(0), PARAM_LEN(0));
        if (res == Z_OK) {
            SetVariable(RESULT, -1, out_buf, max_len);
        } else {
            CORE_DELETE(out_buf);
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__callstack, 0)
// 64k buffer
    char *call_stack = (char *)malloc(0x10000 * sizeof(char *));
    if (call_stack) {
        call_stack[0] = 0;
        Invoke(INVOKE_GET_CALLSTACK, PARAMETERS->HANDLER, &call_stack, (INTEGER)0x10000);
        RETURN_STRING(call_stack);
    } else {
        RETURN_STRING("");
    }
    free(call_stack);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__object, 1)
    T_STRING(__object, 0)
    if (!IS_OK(LocalInvoker(INVOKE_CREATE_OBJECT_NOCONSTRUCTOR, PARAMETERS->HANDLER, RESULT, PARAM(0)))) {
        RETURN_NUMBER(0);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(hpack, 1)
    T_STRING(hpack, 0)
    int len = PARAM_LEN(0);

    char *out_buf = NULL;
    const unsigned char *buffer = (const unsigned char *)PARAM(0);
    int max_len = len * 4;
    CORE_NEW(max_len + 5, out_buf);
    int bit_len = 0;
    if (out_buf) {
        char tmp[5];
        for (int i = 0; i < len; i++) {
            unsigned char index = buffer[i];
            unsigned int val = HuffTable[index].val;
            unsigned char bits = HuffTable[index].bits;

            int target_index = bit_len / 8;
            int target_offset = bit_len % 8;

            // this should never happen
            // but just to be sure
            if (target_index >= max_len)
                break;

            int available_bits = 8 - target_offset;
            bit_len += bits;
            int iteration = 0;
            do {
                if (!target_offset)
                    out_buf[target_index] = 0;

                if (bits <= available_bits) {
                    if (bits == available_bits)
                        out_buf[target_index] |= val;
                    else
                        out_buf[target_index] |= val << (available_bits - bits);
                    break;
                } else {
                    bits -= available_bits;
                    out_buf[target_index] |= val >> bits;
                    val &= (1 << bits) - 1;                   
                    target_index++;

                    target_offset = 0;
                    available_bits = 8;
                }
                iteration++;
            } while (true);
        }
        int len = bit_len / 8;
        int target_offset = bit_len % 8;
        if (target_offset) {
            out_buf[len] |= (1 << (8 - target_offset)) - 1;
            len++;
        }
        out_buf[len] = 0;
        SetVariable(RESULT, -1, out_buf, len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
unsigned int AddBits(unsigned int &hufvalue, const unsigned char *in_buffer, int bit_pos, int bits) {
    int bit_start = bit_pos / 8;
    int bit_offset = bit_pos % 8;

    while (bits > 0) {
        int remaining_bits = 8 - bit_offset;
        int tmp_bits = bits;
        if (tmp_bits > remaining_bits)
            tmp_bits = remaining_bits;
        bit_pos += tmp_bits;
        unsigned int temp = in_buffer[bit_start];

        if (bit_offset)
            temp &= (1 << remaining_bits) - 1;

        temp >>= (remaining_bits - tmp_bits);
        bits -= tmp_bits;
        if (bits) {
            bit_start++;
            bit_offset = 0;
            hufvalue |= temp << (bits);
        } else
            hufvalue |= temp;
    }
    return bit_pos;
}

int SlowLookup(const HuffChar *slow_values, unsigned int val) {
    while (slow_values->val) {
        if (slow_values->val == val)
            return slow_values->symbol;
        slow_values++;
    }
    return -1;
}

CONCEPT_FUNCTION_IMPL(hunpack, 1)
    T_STRING(hunpack, 0)
    int len = PARAM_LEN(0);
    int bit_len = len * 8;
    int bit_pos = 0;

    const unsigned char *in_buffer = (const unsigned char *)PARAM(0);
    char *out_buf = NULL;
    int max_len = len * 2;
    CORE_NEW(max_len + 1, out_buf);
    int data_len = 0;
    if (out_buf) {
        while (bit_pos < bit_len) {
            int idx = 0;
            int read_bits;

            do {
                unsigned int hufvalue = 0;
                read_bits = HuffLengths[idx++];
                if (!read_bits) {
                    // error
                    RETURN_STRING("");
                    CORE_DELETE(out_buf);
                    return 0;
                }
                if (bit_pos + read_bits > bit_len) {
                    AddBits(hufvalue, in_buffer, bit_pos, bit_len - bit_pos);
                    if (hufvalue != ((1 << (bit_len - bit_pos)) - 1)) {
                        
                        RETURN_STRING("");
                        CORE_DELETE(out_buf);
                        return 0;
                    }
                    bit_pos = bit_len;
                    // end of buffer;
                    break;
                }

                int temp_pos = AddBits(hufvalue, in_buffer, bit_pos, read_bits);
                if ((read_bits <= 8) && (hufvalue < 0x100)) {
                    unsigned int c = HuffQuickVals[hufvalue];
                    if ((c) && ((c >> 8) == read_bits)) {
                        out_buf[data_len++] = c & 0xFF;
                        bit_pos += read_bits;
                        break;
                    }
                }
                if (read_bits >= 10) {
                    const HuffChar *slow_values = slow_table[read_bits - 10];
                    if (slow_values) {
                        int c = SlowLookup(slow_values, hufvalue);
                        if (c >= 0) {
                            out_buf[data_len++] = c;
                            bit_pos += read_bits;
                            break;
                        }
                    }
                }
            } while (true);
            out_buf[data_len] = 0;
        }
        out_buf[data_len] = 0;
        SetVariable(RESULT, -1, out_buf, data_len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
