#ifndef __ANSILIST_H
#define __ANSILIST_H

#include "ConceptPools.h"

#define DATA_HANDLER              -1
#define DATA_STRING               0x01
#define DATA_VAR_DESCRIPTOR       0x02
#define DATA_VECTOR               0x03
#define DATA_32_BIT               0x04
#define DATA_ANALIZER_ELEMENT     0x05
#define DATA_EXCEPTION            0x06
#define DATA_OPTIMIZED_ELEMENT    0x07
#define DATA_CLASS_CODE           0x08
#define DATA_CLASS_MEMBER         0x09
#define DATA_LIST                 0x0A
#define DATA_COMPILED_CLASS       0x0B
#define DATA_ARRAY                0x0C
#define DATA_UNDEFINEDMEMBER      0x0D
#define DATA_TINYSTRING           0x0E

struct Node {
    void        *_DATA;
    void        *_NextNode;
    signed char data_type;
    signed char no_clean;

    POOLED(Node);
};

class AnsiList {
private:
    Node *First;
    Node *Last;
    char AutoClean;
    int  count;

    Node *LastIterator;
    int  LastPos;
public:
    POOLED(AnsiList)

    AnsiList(char _ac = 1);
    void Add(void *data, signed char data_is_vector , signed char no_clean = 0);
    void AddUniqueElement(void *data, signed char data_is_vector , signed char no_clean = 0);
    void AddUniquePointer(void *data, signed char data_is_vector , signed char no_clean = 0);

    void *operator[](int i);
    void *Item(int i);
    void *Delete(int i);
    void *Remove(int i);
    void ToStatic(void **ptr, int max_elements);
    void Clear();
    void Insert(void *data, int i, signed char data_type, signed char no_clean = 0);
    int Count();
    void GetFromList(AnsiList *other);

    ~AnsiList(void);
};
#endif
