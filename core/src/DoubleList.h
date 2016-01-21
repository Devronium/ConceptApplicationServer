#ifndef __DOUBLELIST_H
#define __DOUBLELIST_H

#include "ConceptPools.h"

struct DoubleNode {
    void *_DATA;
    void *_NextNode;
    void *_PrevNode;

    char type;
    POOLED(DoubleNode);
};

class DoubleList {
private:
    DoubleNode *First;
    DoubleNode *Last;
    int        count;

    DoubleNode *LastIterator;
    int        LastPos;

    DoubleNode *GetNode(int index, char is_remove = 0);

public:
    POOLED(DoubleList)

    DoubleList();
    void Add(void *data, char data_is_vector );
    void *operator[](int i);
    void *Item(int i);
    void *Delete(int i);
    void *Remove(int i);
    void Insert(void *data, int i, char data_type);
    int Count();
    void Clear();

    ~DoubleList(void);
};
#endif

