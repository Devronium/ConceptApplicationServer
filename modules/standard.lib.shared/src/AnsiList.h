#ifndef __ANSILIST_H
#define __ANSILIST_H

#define DATA_SH_DESC    1

struct Node {
    void *_DATA;
    void *_NextNode;
    char data_type;
    char no_clean;
};

class AnsiList
{
private:
    Node *First;
    Node *Last;
    int  AutoClean;
    long count;

    Node *LastIterator;
    long LastPos;
public:
    AnsiList(int _ac = 1);
    void Add(void *data, char data_is_vector /*=0*/, char no_clean = 0);
    void AddUniquePointer(void *data, char data_is_vector /*=0*/, char no_clean = 0);

    void *operator[](int i);
    void *Item(int i);
    int Delete(int i);
    void *Remove(int i);
    void Clear();
    void Insert(void *data, int i, char data_type, char no_clean = 0);
    long Count();

    ~AnsiList(void);
};
#endif
