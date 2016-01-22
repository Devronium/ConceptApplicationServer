#include "AnsiList.h"

#include "AnsiString.h"
#include "TemplateElement.h"
#include "Templatizer.h"

#ifdef USE_DEPRECATED
 #include <string.h>
//    #include <stdlib.h>
#else
 #include <string.h>
//    #include <stdlib.h>
#endif

AnsiList::AnsiList(int _ac) {
    First        = NULL;
    AutoClean    = _ac;
    count        = 0;
    LastIterator = 0;
    LastPos      = 0;
    Last         = 0;
}

void AnsiList::Add(void *data, char data_is_vector, char no_clean) {
    LastPos = 0;
    Node *Cursor  = First;
    Node *newNode = new Node;

    newNode->_DATA     = data;
    newNode->_NextNode = NULL;
    newNode->data_type = data_is_vector;
    newNode->no_clean  = no_clean;

    if (Last) {
        Last->_NextNode = newNode;
    } else {
        First = newNode;
    }
    Last = newNode;
    count++;
}

void AnsiList::AddUniquePointer(void *data, char data_is_vector /*=0*/, char no_clean) {
    LastPos = 0;
    Node *Cursor = First;

    if (Cursor) {
        while (Cursor->_NextNode) {
            if (Cursor->_DATA == data)
                return;
            Cursor = (Node *)Cursor->_NextNode;
        }

        if (Cursor->_DATA == data)
            return;

        Node *newNode = new Node;

        newNode->_DATA     = data;
        newNode->_NextNode = NULL;
        newNode->data_type = data_is_vector;
        newNode->no_clean  = no_clean;
        Cursor->_NextNode  = (void *)newNode;
        Last = newNode;
    } else {
        Node *newNode = new Node;

        newNode->_DATA     = data;
        newNode->_NextNode = NULL;
        newNode->data_type = data_is_vector;
        newNode->no_clean  = no_clean;
        First = newNode;
        Last  = newNode;
    }
    count++;
}

void AnsiList::Clear() {
    LastPos = 0;
    Node *Cursor = First;
    Node *Next;
    Last = 0;

    if (!Cursor)
        return;

    while (Cursor) {
        Next = (Node *)Cursor->_NextNode;
        void *DATA = Cursor->_DATA;
        if ((AutoClean) && (!Cursor->no_clean)) {
            switch (Cursor->data_type) {
                case DATA_STRING:
                    delete (AnsiString *)DATA;
                    DATA = 0;
                    break;

                case DATA_32_BIT:
                    delete (long *)DATA;
                    DATA = 0;
                    break;

                case DATA_ELEMENT:
                    delete (CTemplateElement *)DATA;
                    DATA = 0;
                    break;

                case DATA_VARIABLE:
                    delete (TemplateVariable *)DATA;
                    DATA = 0;
                    break;

                case DATA_TEMPLATIZER:
                    delete (CTemplatizer *)DATA;
                    DATA = 0;
                    break;

                default:
                    delete DATA;
            }
        }
        delete Cursor;
        Cursor = Next;
    }
    First = NULL;
    count = 0;
}

int AnsiList::Delete(int i) {
    LastPos = 0;
    Node *Cursor = First;
    Node *Prev   = NULL;
    void *DATA   = NULL;
    if (i >= count)
        return 0;

    while (Cursor) {
        if (!i) {
            DATA = Cursor->_DATA;
            count--;
            if (Cursor == Last)
                Last = Prev;

            if (Prev)
                Prev->_NextNode = Cursor->_NextNode;
            else
                First = (Node *)Cursor->_NextNode;

            if ((AutoClean) && (!Cursor->no_clean)) {
                switch (Cursor->data_type) {
                    case DATA_STRING:
                        delete (AnsiString *)DATA;
                        DATA = 0;
                        break;

                    case DATA_32_BIT:
                        delete (long *)DATA;
                        DATA = 0;
                        break;

                    case DATA_ELEMENT:
                        delete (CTemplateElement *)DATA;
                        DATA = 0;
                        break;

                    case DATA_VARIABLE:
                        delete (TemplateVariable *)DATA;
                        DATA = 0;
                        break;

                    case DATA_TEMPLATIZER:
                        delete (CTemplatizer *)DATA;
                        DATA = 0;
                        break;

                    default:
                        delete DATA;
                }
            } else
                DATA = Cursor->_DATA;
            delete Cursor;
            return 1;
        }
        i--;
        Prev   = Cursor;
        Cursor = (Node *)Cursor->_NextNode;
    }
    return 0;
}

int AnsiList::DeleteByData(void *data) {
    LastPos = 0;
    Node *Cursor = First;
    Node *Prev   = NULL;
    void *DATA   = NULL;

    while (Cursor) {
        DATA = Cursor->_DATA;
        if (data == DATA) {
            count--;
            if (Cursor == Last)
                Last = Prev;

            if (Prev)
                Prev->_NextNode = Cursor->_NextNode;
            else
                First = (Node *)Cursor->_NextNode;

            if ((AutoClean) && (!Cursor->no_clean)) {
                switch (Cursor->data_type) {
                    case DATA_STRING:
                        delete (AnsiString *)DATA;
                        DATA = 0;
                        break;

                    case DATA_32_BIT:
                        delete (long *)DATA;
                        DATA = 0;
                        break;

                    case DATA_ELEMENT:
                        delete (CTemplateElement *)DATA;
                        DATA = 0;
                        break;

                    case DATA_VARIABLE:
                        delete (TemplateVariable *)DATA;
                        DATA = 0;
                        break;

                    case DATA_TEMPLATIZER:
                        delete (CTemplatizer *)DATA;
                        DATA = 0;
                        break;

                    default:
                        delete DATA;
                }
            } else
                DATA = Cursor->_DATA;
            delete Cursor;
            return 1;
        }
        Prev   = Cursor;
        Cursor = (Node *)Cursor->_NextNode;
    }
    return 0;
}

// la fel ca Delete, numai ca nu tzine cont de autoclean (ac)
void *AnsiList::Remove(int i) {
    LastPos = 0;
    Node *Cursor = First;
    Node *Prev   = NULL;
    void *DATA   = NULL;
    if (i >= count)
        return DATA;

    while (Cursor) {
        if (!i) {
            if (Cursor == Last)
                Last = Prev;
            count--;
            if (Prev)
                Prev->_NextNode = Cursor->_NextNode;
            else
                First = (Node *)Cursor->_NextNode;

            DATA = Cursor->_DATA;
            delete Cursor;
            return DATA;
        }
        i--;
        Prev   = Cursor;
        Cursor = (Node *)Cursor->_NextNode;
    }
    return DATA;
}

long AnsiList::Count() {
    return count;
}

void *AnsiList::Item(int i) {
    Node *Cursor   = First;
    int  lastindex = i;
    void *DATA     = NULL;

    if ((i >= count) || (i < 0))
        return DATA;

    if ((LastPos) && (i >= LastPos)) {
        i     -= LastPos;
        Cursor = LastIterator;
    }

    while (Cursor) {
        if (!i) {
            LastPos      = lastindex;
            LastIterator = Cursor;
            DATA         = Cursor->_DATA;
            return DATA;
        }
        i--;
        Cursor = (Node *)Cursor->_NextNode;
    }
    return DATA;
}

void *AnsiList::operator[](int i) {
    return Item(i);
}

void AnsiList::Insert(void *data, int i, char data_type, char no_clean) {
    LastPos = 0;
    Node *Cursor = First;
    Node *Prec   = NULL;
    Node *NewNode;
    if (i < 0)
        return;

    if (i >= count)
        Add(data, data_type, no_clean);
    else {
        while (Cursor) {
            if (!i) {
                NewNode            = new Node;
                NewNode->data_type = data_type;
                NewNode->no_clean  = no_clean;
                NewNode->_DATA     = data;
                if (!Prec) {
                    NewNode->_NextNode = (void *)Cursor;
                    First = NewNode;
                } else {
                    NewNode->_NextNode = Prec->_NextNode;
                    Prec->_NextNode    = (void *)NewNode;
                }
                count++;
                return;
            }
            i--;
            Prec   = Cursor;
            Cursor = (Node *)Cursor->_NextNode;
        }
    }
}

AnsiList::~AnsiList(void) {
    Clear();
}
