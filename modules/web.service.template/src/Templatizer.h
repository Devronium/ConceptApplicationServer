#ifndef _TEMPLATIZER_H
#define _TEMPLATIZER_H

#include "AnsiString.h"
#include "AnsiList.h"
#include "stdlibrary.h"
#include "TemplateElement.h"


class TemplateVariable {
    friend class CTemplatizer;
private:
    AnsiString  name;
    void        *data;
    void        *work_variable;
    INVOKE_CALL Invoke;

    void *popped_for;

    char   *szData;
    int    type;
    double nData;

    bool in_for;
    bool obj;
    int  index;

    unsigned short line;
    AnsiString     filename;

public:
    TemplateVariable(AnsiString NAME, void *DATA, INVOKE_CALL INV, unsigned short line, AnsiString filename = "") {
        this->line     = line;
        this->filename = filename;
        name           = NAME;
        data           = DATA;
        work_variable  = 0;
        Invoke         = INV;
        type           = -1;
        szData         = 0;
        nData          = 0;
        //offset=0;
        in_for     = 0;
        index      = 0;
        popped_for = 0;
        obj        = false;
        Invoke(INVOKE_LOCK_VARIABLE, data);
    }

    /*void SetOffset(int off) {
        offset=off;
       }

       int GetOffset() {
        return offset;
       }*/

    AnsiString GetString() {
        return szData;
    }

    double GetNumber() {
        return nData;
    }

    bool is_popped(AnsiList *A, AnsiList *B) {
        if (!A)
            return false;

        /*if (!B)
            return false;*/
        if (A->Count() != B->Count())
            return false;
        int cnt = A->Count();
        for (int i = 0; i < cnt; i++) {
            if (*(AnsiString *)A->Item(i) != *(AnsiString *)B->Item(i))
                return false;
        }
        return true;
    }

    int QueryType(AnsiList *members, AnsiString *errors) {
        void    *_data = this->data;
        INTEGER type   = 0;

        obj = false;
        if ((members) && (!is_popped((AnsiList *)popped_for, members))) {
            int count = members->Count();
            for (int i = 0; i < count; i++) {
                AnsiString *S = (AnsiString *)members->Item(i);
                const char *mname = S->c_str();
                type = mname[0];
                mname++;
                void *_data_member = 0;

                if (type == '.') {
                    obj = true;
                    if (in_for) {
                        if (Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, _data, index, 0))
                            Invoke(INVOKE_ARRAY_VARIABLE, _data, index, &_data);
                        else {
                            *errors += (char *)"No such index in this array: ";
                            *errors += AnsiString((long)index);
                            ADD_INFO_ERROR(*errors);
                            return 0;
                        }
                    }

                    Invoke(INVOKE_GET_CLASS_VARIABLE, _data, mname, &_data_member);
                    if (!_data_member) {
                        if (errors) {
                            *errors += (char *)"No such member in objective variable : ";
                            *errors += mname;
                            ADD_INFO_ERROR(*errors);
                        }
                        break;
                    }
                } else {
                    if ((mname[0] == '\"') || (mname[0] == '\'')) {
                        AnsiString mn = mname + 1;
                        if (mn.Length()) {
                            char c = mn.c_str()[mn.Length() - 1];
                            ((char *)mn.c_str())[mn.Length() - 1] = 0;
                            if ((c == '\"') || (c == '\'')) {
                                if (Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, _data, (long)-1, mn.c_str()))
                                    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, _data, mn.c_str(), &_data_member);
                                else {
                                    *errors += (char *)"No such key in this array: ";
                                    *errors += mn;
                                    ADD_INFO_ERROR(*errors);
                                    return 0;
                                }
                            } else
                                _data_member = 0;
                        }
                    } else
                        Invoke(INVOKE_ARRAY_VARIABLE, _data, AnsiString(mname).ToInt(), &_data_member);
                    if (!_data_member) {
                        if (errors) {
                            *errors += (char *)"No such element in this array : ";
                            *errors += mname;
                            ADD_INFO_ERROR(*errors);
                        }
                        break;
                    }
                }
                _data = _data_member;
            }
        }

        if ((in_for) && (type != '.')) {
            Invoke(INVOKE_GET_ARRAY_ELEMENT, _data, index, &type, &szData, &nData);
        } else {
            Invoke(INVOKE_GET_VARIABLE, _data, &type, &szData, &nData);
            work_variable = _data;

            /*if (type==VARIABLE_ARRAY) {
                if (in_for) {
                    Invoke(INVOKE_GET_ARRAY_ELEMENT, _data, index, &type, &szData, &nData);
                } else {
                    work_variable=_data;
                }
               }*/
        }
        return type;
    }

    /*int QueryType(long index) {
        Invoke(INVOKE_GET_ARRAY_ELEMENT, data, index, &type, &szData, &nData);
        return type;
       }*/

    void BeginFor() {
        in_for = true;
        index  = 0;
    }

    void Next() {
        index++;
    }

    void EndFor() {
        index  = 0;
        in_for = false;
    }

    int GetArrayCount() {
        if (obj)
            return Invoke(INVOKE_GET_ARRAY_COUNT, work_variable);
        else
            return Invoke(INVOKE_GET_ARRAY_COUNT, data);
    }

    AnsiString GetArrayKey() {
        AnsiString result;
        char       *key = 0;

        //Invoke(INVOKE_GET_ARRAY_KEY,data,index,&key);
        Invoke(INVOKE_GET_ARRAY_KEY, work_variable, index, &key);
        if (key)
            result = key;
        return result;
    }

    void PopData(void *d) {
        if (Invoke(INVOKE_ARRAY_VARIABLE, data, index, &data) < 0) {
            Invoke(INVOKE_ARRAY_VARIABLE, work_variable, index, &data);
            popped_for = d;
        }
        index = 0;
    }

    void *GetClass() {
        return (void *)szData;
    }

    ~TemplateVariable() {
        if (data)
            Invoke(INVOKE_FREE_VARIABLE, data);
    }
};


class CTemplatizer {
    friend class TemplateVariable;
private:
    //AnsiString TEMPLATE;
    AnsiList    Variables;
    AnsiList    Template;
    INVOKE_CALL Invoke;
    signed char go_back;
    AnsiString  errors;

    AnsiString     filename;
    unsigned short line;

    int FindVariable(AnsiString name);
    int FindVariable(void *data);
    int FindVariable(AnsiString name, void *data);
    char *ParseElement(char *data, AnsiList *Owner, CTemplateElement *EO);
    char *Literal(char *data, AnsiList *Owner);

public:
    char *GetComparator(char *data, CTemplateElement *te);

    CTemplatizer(INVOKE_CALL INV);
    int BindVariable(AnsiString name, void *data);
    int UnbindVariable(AnsiString name);
    int UnbindVariable(void *data);
    long Compile(char *data, AnsiList *Owner = 0, CTemplateElement *EO = 0);
    AnsiString Execute(AnsiList *Target = 0, CTemplateElement *Owner = 0);
    AnsiString GetErrors();
};
#endif // _TEMPLATIZER_H
