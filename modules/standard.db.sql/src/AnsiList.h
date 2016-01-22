#ifndef __ANSILIST_H
#define __ANSILIST_H

#define DATA_STRING        0x01
#define DATA_32_BIT        0x02
#define DATA_CONNECTION    0x03
#define DATA_STATAMENT     0x04
#define DATA_BUFFER        0x05
#define DATA_PARAMETER     0x06

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
    int DeleteByData(void *);
    void *Remove(int i);
    void Clear();
    void Insert(void *data, int i, char data_type, char no_clean = 0);
    long Count();

    ~AnsiList(void);
};
#endif
