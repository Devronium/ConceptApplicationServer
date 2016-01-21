#include "DoubleList.h"
#include "ConceptTypes.h"
#include "AnsiList.h"

POOLED_IMPLEMENTATION(DoubleNode)
POOLED_IMPLEMENTATION(DoubleList)

#define DD_ABS(X)    ((X < 0) ? -X : X)

DoubleList::DoubleList() {
    First        = 0;
    count        = 0;
    LastIterator = 0;
    LastPos      = 0;
    Last         = 0;
}

DoubleNode *DoubleList::GetNode(int index, char is_remove) {
    DoubleNode *iterator = 0;

    if ((index >= count) || (index < 0))
        return iterator;

    int i;
    int delta1  = count - index;
    int tdelta2 = index - LastPos;
    int delta2  = DD_ABS(tdelta2);

    int op_type = 0;
    if (index > delta1) {
        op_type = 2;
        if ((LastPos) && (delta1 > delta2))
            op_type = 1;
    } else {
        if ((LastPos) && (index > delta2))
            op_type = 1;
    }

    switch (op_type) {
        case 0:
            i        = index;
            iterator = First;
            while (--i >= 0)
                iterator = (DoubleNode *)iterator->_NextNode;
            break;

        case 1:
            i        = delta2;
            iterator = LastIterator;
            if (tdelta2 < 0) {
                while (--i >= 0)
                    iterator = (DoubleNode *)iterator->_PrevNode;
            } else {
                while (--i >= 0)
                    iterator = (DoubleNode *)iterator->_NextNode;
            }
            break;

        case 2:
            i        = delta1;
            iterator = Last;
            while (--i > 0)
                iterator = (DoubleNode *)iterator->_PrevNode;
            break;
    }

    if (is_remove) {
        if (iterator->_PrevNode) {
            LastIterator = (DoubleNode *)iterator->_PrevNode;
            LastPos      = index - 1;
        } else
        if (iterator->_NextNode) {
            LastIterator = (DoubleNode *)iterator->_NextNode;
            LastPos      = index + 1;
        } else {
            LastPos      = 0;
            LastIterator = 0;
        }
    } else {
        LastIterator = iterator;
        LastPos      = index;
    }
    return iterator;
}

void DoubleList::Add(void *data, char data_is_vector) {
    DoubleNode *newNode = new DoubleNode;

    newNode->_DATA     = data;
    newNode->type      = data_is_vector;
    newNode->_NextNode = NULL;
    if (Last) {
        Last->_NextNode    = newNode;
        newNode->_PrevNode = Last;
    } else {
        First = newNode;
        newNode->_PrevNode = 0;
    }
    Last = newNode;
    count++;
}

void DoubleList::Clear() {
    LastPos = 0;
    DoubleNode *Cursor = First;
    DoubleNode *Next;
    Last = 0;

    if (!Cursor) {
        return;
    }

    while (Cursor) {
        Next = (DoubleNode *)Cursor->_NextNode;
        void *DATA = Cursor->_DATA;
        if (Cursor->type == DATA_ANALIZER_ELEMENT)
            delete (AnalizerElement *)DATA;
        else
        if (Cursor->type == DATA_VAR_DESCRIPTOR)
            delete (VariableDESCRIPTOR *)DATA;
        delete Cursor;
        Cursor = Next;
    }
    First = NULL;
    count = 0;
}

void *DoubleList::Delete(int i) {
    void       *DATA = 0;
    DoubleNode *Node = GetNode(i, 1);

    if (Node) {
        DoubleNode *Next = (DoubleNode *)Node->_NextNode;
        DoubleNode *Prev = (DoubleNode *)Node->_PrevNode;

        if (Prev)
            Prev->_NextNode = Next;
        else
            First = Next;

        if (Next)
            Next->_PrevNode = Prev;
        else
            Last = Prev;
        DATA = Node->_DATA;
        count--;
        if (Node->type == DATA_ANALIZER_ELEMENT)
            delete (AnalizerElement *)DATA;
        else
        if (Node->type == DATA_VAR_DESCRIPTOR)
            delete (VariableDESCRIPTOR *)DATA;

        delete Node;
    }
    return DATA;
}

void *DoubleList::Remove(int i) {
    void       *DATA = 0;
    DoubleNode *Node = GetNode(i, 1);

    if (Node) {
        DoubleNode *Next = (DoubleNode *)Node->_NextNode;
        DoubleNode *Prev = (DoubleNode *)Node->_PrevNode;

        if (Prev)
            Prev->_NextNode = Next;
        else
            First = Next;

        if (Next)
            Next->_PrevNode = Prev;
        else
            Last = Prev;
        DATA = Node->_DATA;
        count--;

        delete Node;
    }
    return DATA;
}

int DoubleList::Count() {
    return count;
}

void *DoubleList::Item(int i) {
    DoubleNode *Cursor = GetNode(i);
    void       *DATA   = NULL;

    if (Cursor)
        DATA = Cursor->_DATA;

    return DATA;
}

void *DoubleList::operator[](int i) {
    return Item(i);
}

void DoubleList::Insert(void *data, int i, char data_type) {
    if (i < 0)
        return;

    if (i >= count) {
        Add(data, data_type);
    } else {
        DoubleNode *Temp = GetNode(i, 1);
        if (!Temp)
            return;
        DoubleNode *Prec = (DoubleNode *)Temp->_PrevNode;

        DoubleNode *NewNode = new DoubleNode;
        NewNode->type  = data_type;
        NewNode->_DATA = data;

        NewNode->_NextNode = Temp;
        Temp->_PrevNode    = NewNode;
        NewNode->_PrevNode = Prec;

        if (Prec) {
            Prec->_NextNode = NewNode;
        } else {
            First = NewNode;
        }

        if (!NewNode->_NextNode)
            Last = NewNode;

        count++;
        return;
    }
}

DoubleList::~DoubleList(void) {
    Clear();
}

