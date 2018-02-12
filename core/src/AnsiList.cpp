#include "AnsiList.h"

#include "ConceptTypes.h"
#include "AnsiException.h"
#include "ClassCode.h"
#include "ClassMember.h"
#include "Array.h"
#include "TinyString.h"

#ifdef USE_DEPRECATED
 #include <string.h>
//    #include <stdlib.h>
#else
 #include <string.h>
//    #include <stdlib.h>
#endif

POOLED_IMPLEMENTATION(Node)
POOLED_IMPLEMENTATION(AnsiList)

AnsiList::AnsiList(char _ac) {
    First        = NULL;
    AutoClean    = _ac;
    count        = 0;
    LastIterator = 0;
    LastPos      = 0;
    Last         = 0;
}

void AnsiList::Add(void *data, signed char data_is_vector, signed char no_clean) {
    if (data_is_vector == DATA_EXCEPTION) {
        AddUniqueElement(data, data_is_vector, no_clean);
        return;
    }
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

void AnsiList::AddUniqueElement(void *data, signed char data_is_vector , signed char no_clean) {
    LastPos = 0;
    Node *Cursor = First;

    if (Cursor) {
        while (Cursor->_NextNode) {
            if (Cursor->data_type == data_is_vector) {
                void *DATA = Cursor->_DATA;
                switch (data_is_vector) {
                    case DATA_EXCEPTION:
                        if ((((AnsiException *)DATA)->GetID() == ((AnsiException *)data)->GetID()) &&
                            (((AnsiException *)DATA)->GetLine() == ((AnsiException *)data)->GetLine()) &&
                            (((AnsiException *)DATA)->GetFileName() == ((AnsiException *)data)->GetFileName())) {
                            delete ((AnsiException *)data);
                            return;
                        }
                }
            }
            Cursor = (Node *)Cursor->_NextNode;
        }

        void *DATA = Cursor->_DATA;
        switch (data_is_vector) {
            case DATA_EXCEPTION:
                if ((((AnsiException *)DATA)->GetID() == ((AnsiException *)data)->GetID()) &&
                    (((AnsiException *)DATA)->GetLine() == ((AnsiException *)data)->GetLine()) &&
                    (((AnsiException *)DATA)->GetFileName() == ((AnsiException *)data)->GetFileName())) {
                    delete ((AnsiException *)data);
                    return;
                }
        }
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

void AnsiList::AddUniquePointer(void *data, signed char data_is_vector , signed char no_clean) {
    LastPos = 0;
    Node *Cursor = First;

    if (Cursor) {
        while (Cursor->_NextNode) {
            if (Cursor->_DATA == data) {
                return;
            }
            Cursor = (Node *)Cursor->_NextNode;
        }

        if (Cursor->_DATA == data) {
            return;
        }

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

    if (!Cursor) {
        return;
    }

    while (Cursor) {
        Next = (Node *)Cursor->_NextNode;
        void *DATA = Cursor->_DATA;
        if ((AutoClean) && (!Cursor->no_clean)) {
            switch (Cursor->data_type) {
                case DATA_TINYSTRING:
                    delete (TinyString *)DATA;
                    DATA = 0;
                    break;

                case DATA_VAR_DESCRIPTOR:
                    delete (VariableDESCRIPTOR *)DATA;
                    DATA = 0;
                    break;

                case DATA_32_BIT:
                    delete (intptr_t *)DATA;
                    DATA = 0;
                    break;

                case DATA_ANALIZER_ELEMENT:
                    delete (AnalizerElement *)DATA;
                    DATA = 0;
                    break;

                case DATA_EXCEPTION:
                    delete (AnsiException *)DATA;
                    DATA = 0;
                    break;

                case DATA_OPTIMIZED_ELEMENT:
                    delete (OptimizedElement *)DATA;
                    DATA = 0;
                    break;

                case DATA_CLASS_CODE:
                    delete (ClassCode *)DATA;
                    DATA = 0;
                    break;

                case DATA_CLASS_MEMBER:
                    delete (ClassMember *)DATA;
                    DATA = 0;
                    break;

                case DATA_LIST:
                    delete (AnsiList *)DATA;
                    DATA = 0;
                    break;

                case DATA_COMPILED_CLASS:
                    delete_CompiledClass((struct CompiledClass *)DATA);
                    DATA = 0;
                    break;

                case DATA_ARRAY:
                    delete_Array((struct Array *)DATA);
                    DATA = 0;
                    break;

                case DATA_UNDEFINEDMEMBER:
                    delete (UndefinedMember *)DATA;
                    DATA = 0;
                    break;

                default:
                    //delete void * is undefined
                    break;
            }
        }
        delete Cursor;
        Cursor = Next;
    }
    First = NULL;
    count = 0;
}

void *AnsiList::Delete(int i) {
    Node *Cursor = First;
    Node *Prev   = NULL;
    void *DATA   = NULL;

    if (i >= count) {
        return DATA;
    }

    if ((LastPos) && (i > LastPos)) {
        i     -= LastPos;
        Cursor = LastIterator;
    } else
        LastPos = 0;

    while (Cursor) {
        if (!i) {
            DATA = Cursor->_DATA;
            count--;
            if (Cursor == Last) {
                Last = Prev;
            }

            if (Prev) {
                Prev->_NextNode = Cursor->_NextNode;
            } else {
                First = (Node *)Cursor->_NextNode;
            }

            if ((AutoClean) && (!Cursor->no_clean)) {
                switch (Cursor->data_type) {
                    case DATA_TINYSTRING:
                        delete (TinyString *)DATA;
                        DATA = 0;
                        break;

                    case DATA_VAR_DESCRIPTOR:
                        delete (VariableDESCRIPTOR *)DATA;
                        DATA = 0;
                        break;

                    case DATA_32_BIT:
                        delete (intptr_t *)DATA;
                        DATA = 0;
                        break;

                    case DATA_ANALIZER_ELEMENT:
                        delete (AnalizerElement *)DATA;
                        DATA = 0;
                        break;

                    case DATA_EXCEPTION:
                        delete (AnsiException *)DATA;
                        DATA = 0;
                        break;

                    case DATA_OPTIMIZED_ELEMENT:
                        delete (OptimizedElement *)DATA;
                        DATA = 0;
                        break;

                    case DATA_CLASS_CODE:
                        delete (ClassCode *)DATA;
                        DATA = 0;
                        break;

                    case DATA_CLASS_MEMBER:
                        delete (ClassMember *)DATA;
                        DATA = 0;
                        break;

                    case DATA_LIST:
                        delete (AnsiList *)DATA;
                        DATA = 0;
                        break;

                    case DATA_COMPILED_CLASS:
                        delete_CompiledClass((struct CompiledClass *)DATA);
                        DATA = 0;
                        break;

                    case DATA_ARRAY:
                        delete_Array((struct Array *)DATA);
                        DATA = 0;
                        break;

                    case DATA_UNDEFINEDMEMBER:
                        delete (UndefinedMember *)DATA;
                        DATA = 0;
                        break;

                    default:
                        // delete void * is udefined
                        break;
                }
            } else {
                DATA = Cursor->_DATA;
            }
            delete Cursor;
            return DATA;
        }
        i--;
        Prev   = Cursor;
        Cursor = (Node *)Cursor->_NextNode;
    }
    return DATA;
}

void *AnsiList::Remove(int i) {
    Node *Cursor = First;
    Node *Prev   = NULL;
    void *DATA   = NULL;
    int  idx     = i;

    if (i >= count) {
        return DATA;
    }

    if ((LastPos) && (i > LastPos)) {
        i     -= LastPos;
        Cursor = LastIterator;
    } else
        LastPos = 0;

    while (Cursor) {
        if (!i) {
            if (Cursor == Last) {
                Last = Prev;
            }
            count--;
            if (Prev) {
                Prev->_NextNode = Cursor->_NextNode;
            } else {
                First = (Node *)Cursor->_NextNode;
            }

            if (Prev) {
                LastPos      = idx > 0 ? idx - 1 : 0;
                LastIterator = Prev;
            } else
                LastPos = 0;
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

int AnsiList::Count() {
    return count;
}

void *AnsiList::Item(int i) {
    Node *Cursor;
    Node *Prec     = 0;
    int  lastindex = i;
    void *DATA     = NULL;

    if ((i >= count) || (i < 0)) {
        return DATA;
    }

    if ((i == count - 1) && (Last)) {
        return Last->_DATA;
    } else
    if ((LastPos) && (i >= LastPos)) {
        i     -= LastPos;
        Cursor = LastIterator;
    } else
        Cursor = First;

    while (Cursor) {
        if (!i) {
            if ((Prec) && (lastindex > 1)) {
                LastPos      = lastindex - 1;
                LastIterator = Prec;
            } else {
                LastPos      = lastindex;
                LastIterator = Cursor;
            }
            DATA = Cursor->_DATA;
            return DATA;
        }
        i--;
        Prec   = Cursor;
        Cursor = (Node *)Cursor->_NextNode;
    }
    return DATA;
}

void *AnsiList::operator[](int i) {
    return Item(i);
}

void AnsiList::Insert(void *data, int i, signed char data_type, signed char no_clean) {
    Node *Cursor = First;
    Node *Prec   = NULL;
    Node *NewNode;

    if (i < 0) {
        return;
    }

    if (i >= count) {
        Add(data, data_type, no_clean);
    } else {
        if ((LastPos) && (i > LastPos)) {
            i     -= LastPos;
            Cursor = LastIterator;
        } else
            LastPos = 0;

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

void AnsiList::ToStatic(void **ptr, int max_elements) {
    Node *Cursor = First;
    int index = 0;
    if (!ptr)
        return;
    while ((Cursor) && (max_elements > 0)) {
        ptr[index++] = Cursor->_DATA;
        max_elements--;
        Cursor = (Node *)Cursor->_NextNode;
    }
}

void AnsiList::GetFromList(AnsiList *other) {
    if (!other)
        return;
    if (other->First) {
        if (this->Last) {
            this->Last->_NextNode = other->First;
            this->Last            = other->Last;
            this->count          += other->count;
        } else {
            this->First = other->First;
            this->Last  = other->Last;
            this->count = other->count;
        }

        other->First        = NULL;
        other->count        = 0;
        other->LastIterator = 0;
        other->LastPos      = 0;
        other->Last         = 0;
    }
}

AnsiList::~AnsiList(void) {
    Clear();
}
