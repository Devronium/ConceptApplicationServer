// 100% PURE ANSI C++ String Class
// (c) 2005 RadGs Software
// Author : Suica Eduard

#ifndef __ANSISTRING_H
#define __ANSISTRING_H

//#define USE_DEPRECATED

#define MAX_DECIMALS    0xFF
#define BLOCK_SIZE      16

class AnsiString {
private:
    char *Data;
    long _DATA_SIZE;

    long _LENGTH;
public:
    AnsiString(void);
    AnsiString(char *value);
    AnsiString(char c);
    AnsiString(int i);
    AnsiString(long i);
    AnsiString(double d);
    AnsiString(const AnsiString& S);

    char operator[](unsigned long index);
    char operator[](int index);

    int operator==(char *str);
    int operator!=(char *str);

    int operator==(AnsiString S);
    int operator!=(AnsiString S);


    int operator <(char *str);
    int operator <=(char *str);
    int operator >(char *str);
    int operator >=(char *str);

    int operator <(AnsiString S);
    int operator <=(AnsiString S);
    int operator >(AnsiString S);
    int operator >=(AnsiString S);

    void operator=(char *value);
    void operator=(AnsiString S);
    void operator=(long i);
    void operator=(int i);
    void operator=(char c);
    void operator=(double d);

    void operator+=(char *value);
    void operator+=(AnsiString S);
    void operator+=(char c);

    operator char *();

    AnsiString operator+(AnsiString S);

    char *c_str();
    long ToInt();
    double ToFloat();

    int LoadFile(char *filename);
    int SaveFile(char *filename);
    void LoadBuffer(char *buffer, int size);
    void AddBuffer(char *S_Data, int S_Len);

    long Length();

    long Pos(AnsiString substr, int after = 0);

    ~AnsiString();
};
#endif
