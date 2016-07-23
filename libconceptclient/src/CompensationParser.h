#ifndef __COMPENSATION_PARSER_H
#define __COMPENSATION_PARSER_H

#define VARIABLE_NUMBER    0
#define VARIABLE_STRING    1
#define MAX_PARAMS         0xF
#define MAX_BLOCKS         0x20
#define DATA_BLOCK         16
#define CODE_BLOCK         16
#define FUNC_BLOCK         16

typedef int (*PROTOCOL_MESSAGE_CB)(AnsiString& Sender, int MSG_ID, AnsiString& Target, AnsiString& Value, void *UserData);

#define PROPERTY_CODE                           \
    SetProperty(Sender, OE, context, OE->left); \
    if (err) {                                  \
        f->DestroyContext(context);             \
        return;                                 \
    }

#define PROPERTY_CODE_RESULT                      \
    SetProperty(Sender, OE, context, OE->result); \
    if (err) {                                    \
        f->DestroyContext(context);               \
        return;                                   \
    }

#include "AnsiString.h"
#include "Codes.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    AnsiString value;
    double     nval;
    char       type;
    void       *CodeRef;
} DataElement;

typedef struct {
    int        result;
    int        op;
    int        left;
    int        right;
    int        rid;

    AnsiString right_def;
    AnsiString set_def;
    AnsiString get_def;
    short      parameters[MAX_PARAMS];
} CodeElement;

class Function {
private:
    DataElement **data;
    int         dataindex;
public:
    CodeElement **code;
    int         codeindex;

    AnsiString Name;
    int        RID;
    short      parameters[MAX_PARAMS];

    Function() {
        code          = 0;
        data          = 0;
        dataindex     = 0;
        codeindex     = 0;
        RID           = -1;
        parameters[0] = -1;
    }

    void EnsureIndex(int index) {
        while (index >= dataindex) {
            if (dataindex % DATA_BLOCK == 0)
                data = (DataElement **)realloc(data, (dataindex + DATA_BLOCK) * sizeof(DataElement *));
            if (data) {
                DataElement *d = new DataElement;
                d->type           = VARIABLE_NUMBER;
                d->nval           = 0;
                d->CodeRef        = NULL;
                data[dataindex++] = d;
            }
        }
    }

    void AddData(char *name, char *val) {
        int index = 0;

        if (name) {
            while ((name[0]) && ((name[0] < '0') || (name[0] > '9')))
                name++;
            index = atoi(name);
        } else
            return;

        while (index >= dataindex) {
            if (dataindex % DATA_BLOCK == 0)
                data = (DataElement **)realloc(data, (dataindex + DATA_BLOCK) * sizeof(DataElement *));
            if (data) {
                DataElement *d = new DataElement;
                if ((index == dataindex) && ((val) && (val[0] == '"'))) {
                    AnsiString tmp(val);
                    StripString(&tmp, d->value);
                    d->type = VARIABLE_STRING;
                } else {
                    if (index == dataindex)
                        d->nval = atof(val);
                    else
                        d->nval = 0;
                    d->type = VARIABLE_NUMBER;
                }
                data[dataindex++] = d;
            }
        }
    }

    void AddCode(CodeElement *c) {
        if (codeindex % CODE_BLOCK == 0)
            code = (CodeElement **)realloc(code, (codeindex + CODE_BLOCK) * sizeof(CodeElement *));
        if (code)
            code[codeindex++] = c;
    }

    DataElement *CreateContext() {
        DataElement *ctx = NULL;

        if (data) {
            ctx = new DataElement[dataindex];
            for (int i = 0; i < dataindex; i++) {
                ctx[i].nval = data[i]->nval;
                ctx[i].type = data[i]->type;
                if (ctx[i].type == VARIABLE_STRING)
                    ctx[i].value = data[i]->value;
                ctx[i].CodeRef = NULL;
            }
        }
        return ctx;
    }

    void DestroyContext(DataElement *ctx) {
        if (ctx)
            delete[] ctx;
    }

    ~Function() {
        if (data) {
            for (int i = 0; i < dataindex; i++)
                delete data[i];
            free(data);
        }
        if (code) {
            for (int i = 0; i < codeindex; i++)
                delete code[i];
            free(code);
        }
    }
};

class CompensationParser {
private:
    PROTOCOL_MESSAGE_CB send_message;
    PROTOCOL_MESSAGE_CB wait_message;
    void *UserData;

    Function    **FList;
    int         f_index;
    DataElement return_value;
    bool        err;

    void BuildVariable(Function *f, char *data, int& position, int len) {
        AnsiString param;
        AnsiString name;
        char       in_name = 1;

        while (++position < len) {
            char c = data[position];
            switch (c) {
                case '\r':
                    break;

                case '\n':
                    position--;
                    if (!in_name)
                        f->AddData(name.c_str(), param.c_str());
                    return;

                case ' ':
                case '\t':
                    if ((in_name) && (param.c_str())) {
                        name    = param;
                        param   = "";
                        in_name = 0;
                        break;
                    }

                default:
                    param += c;
                    break;
            }
        }
        if (!in_name)
            f->AddData(name.c_str(), param.c_str());
    }

    int VariableIndex(Function *f, char *name) {
        int index = -1;

        if (name) {
            while ((name[0]) && ((name[0] < '0') || (name[0] > '9')))
                name++;
            index = atoi(name);
        }
        if (index >= 0)
            f->EnsureIndex(index);
        return index;
    }

    AnsiString EntryPoint(char *data, int& position, int len) {
        char       in_name = 1;
        AnsiString res;

        while (++position < len) {
            char c = data[position];
            switch (c) {
                case '(':
                case ')':
                case '\r':
                case ' ':
                case '\t':
                case ',':
                case ';':
                    if (res.Length())
                        in_name = 0;
                    break;

                case '\n':
                    position--;
                    return res;
                    break;

                default:
                    if (in_name)
                        res += c;
            }
        }
        return res;
    }

    void BuildStatement(Function *f, char *data, int& position, int len, int line) {
        AnsiString params[MAX_BLOCKS];
        int        param_index = 0;
        char       not_out     = 1;
        short      param_start = 0;
        int        param_pos   = 0;
        char       has_params  = 0;

        while ((not_out) && (position < len)) {
            char c = data[position++];
            switch (c) {
                case '(':
                    if (!has_params)
                        has_params = param_index + 1;
                    if (((param_index == 2) && (params[param_index].Length())) || ((param_index > 0) && (params[param_index - 1] != (char *)"=")) || ((param_index == 0) && (params[param_index].Length())))
                        param_start = param_index + 1;

                case ')':
                case '\r':
                case ' ':
                case '\t':
                case ',':
                case ';':
                    if (params[param_index].Length()) {
                        if (param_index < MAX_BLOCKS - 1)
                            param_index++;
                    }
                    break;

                case '\n':
                    position--;
                    not_out = 0;
                    break;

                case ']':
                case '[':
                    if (params[param_index].Length()) {
                        param_index++;
                        position--;
                        break;
                    }

                default:
                    params[param_index] += c;
                    break;
            }
        }
        if (params[param_index].Length())
            param_index++;

        if ((param_index > 1) && (params[0] == (char *)"this"))
            f->RID = params[1].ToInt();
        else {
            int start = 0;

            CodeElement *c = new CodeElement;
            c->result        = -1;
            c->op            = -1;
            c->left          = -1;
            c->right         = -1;
            c->parameters[0] = -1;
            c->rid           = -1;

            if (params[0] == (char *)"if") {
                c->op   = KEY_OPTIMIZED_IF;
                c->left = this->VariableIndex(f, params[1].c_str());
                if (params[2] == (char *)"goto")
                    c->right = params[3].ToInt();
            } else
            if (params[0] == (char *)"goto") {
                c->op    = KEY_OPTIMIZED_GOTO;
                c->right = params[1].ToInt();
            } else
            if (params[0] == (char *)"return") {
                c->op = KEY_OPTIMIZED_RETURN;
                if (params[1].Length())
                    c->right = this->VariableIndex(f, params[1].c_str());
            } else
            if (params[0] == (char *)"echo") {
                c->op = KEY_OPTIMIZED_ECHO;
                if (params[1].Length())
                    c->right = this->VariableIndex(f, params[1].c_str());
            } else
            if (params[0] == (char *)"try") {
                c->op    = KEY_OPTIMIZED_TRY_CATCH;
                c->right = VariableIndex(f, params[1].c_str());
                if (params[2] == (char *)"goto")
                    c->left = params[3].ToInt();
            } else
            if (params[0] == (char *)"throw") {
                c->op = KEY_OPTIMIZED_THROW;
            } else
            if (params[0] == (char *)"endthrow") {
                c->op = KEY_OPTIMIZED_END_CATCH;
            } else
            if (params[0] == (char *)"trap") {
                c->op = KEY_OPTIMIZED_DEBUG_TRAP;
            } else {
                if ((param_index > 2) && (params[1] == (char *)"=")) {
                    c->result = VariableIndex(f, params[0].c_str());
                    start     = 2;
                }
                for (int i = start; i < param_index; i++) {
                    if (params[i] == (char *)"->") {
                        c->set_def = params[++i];
                    } else
                    if (params[i] == (char *)"<-") {
                        c->get_def = params[++i];
                    } else
                    if (params[i] == (char *)"this") {
                        if (i == start)
                            c->left = 0;
                    } else
                    if (params[i][0] == '#') {
                        AnsiString temp(params[i].c_str() + 1);
                        c->rid = temp.ToInt();
                    } else
                    if ((param_start) && (i >= param_start)) {
                        int index = 0;
                        if (params[i] != (char *)"this")
                            index = VariableIndex(f, params[i]);

                        if (param_pos < MAX_PARAMS)
                            c->parameters[param_pos++] = index;
                    } else {
                        if (params[i] == (char *)"del")
                            params[i] = "delete";
                        int type = GetType(params[i]);
                        if (type == TYPE_OPERATOR) {
                            c->op = GetID(params[i]);
                        } else
                        if (((i == param_start) && (i != start)) || (c->op == KEY_SEL) || (c->op == KEY_DLL_CALL) || (c->op == KEY_NEW) || (params[i].Pos(".") > 0) || (params[i].Pos("::") > 0)) {
                            c->right_def = params[i];
                        } else {
                            if (i == start)
                                c->left = VariableIndex(f, params[i]);
                            else
                                c->right = VariableIndex(f, params[i]);
                        }
                    }
                }
                c->parameters[param_pos] = -1;
            }

            if ((c->op == -1) && (start == 2) && (param_index == 3) && (!has_params)) {
                c->op = -2;
            } else
            if ((c->op == -1) && (has_params))
                if (!c->right_def.Length()) {
                    c->right_def = params[has_params - 1];
                }
            f->AddCode(c);
        }
    }

    Function *ParseFunction(char *data, int& position, int len) {
        int        line           = 0;
        Function   *f             = new Function();
        char       params         = 0;
        char       statement_line = 0;
        AnsiString param;

        while (++position < len) {
            char c = data[position];
            if (!line) {
                if ((c == '(') || (c == ')') || (c == ' ') || (c == '\t') || (c == '\r') || (c == ',')) {
                    if ((params) && (params < MAX_PARAMS)) {
                        // check if parameter is by type (discard the type)
                        if ((c == ' ') || (c == '\t'))
                            param = (char *)"";
                        else {
                            if (param.Length()) {
                                f->parameters[params - 1] = VariableIndex(f, param);
                                f->parameters[params]     = -1;
                                params++;
                            }
                        }
                        param = (char *)"";
                    } else
                        params++;
                } else {
                    if (params) {
                        param += c;
                    } else
                        f->Name += c;

                    if (c == '\n')
                        line++;
                }
            } else {
                switch (c) {
                    case '$':
                        this->BuildVariable(f, data, position, len);
                        break;

                    case '\r':
                        break;

                    case '\n':
                        line++;
                        break;

                    case '@':
                    case '#':
                        position--;
                        return f;

                    default:
                        this->BuildStatement(f, data, position, len, statement_line++);
                        break;
                }
            }
        }
        return f;
    }

    Function *Find(AnsiString& fname) {
        for (int i = 0; i < f_index; i++) {
            Function *f = FList[i];
            if ((f) && (f->Name == fname))
                return f;
        }
        return NULL;
    }

    void SetProperty(int Sender, CodeElement *OE, DataElement *context, int element) {
        if (element < 0)
            return;
        CodeElement *OE2 = (CodeElement *)context[element].CodeRef;
        if ((!OE2) || (!OE2->set_def.Length()))
            return;

        Function *f2 = this->Find(OE2->set_def);
        if (f2) {
            short params[2];
            params[0] = element;
            params[1] = -1;
            if (OE2->rid >= 0)
                Sender = OE2->rid;
            else
                while ((OE2) && (OE2->left >= 0)) {
                    OE2 = (CodeElement *)context[OE2->left].CodeRef;
                    if (OE2) {
                        Sender = OE2->rid;
                        break;
                    }
                }
            this->Execute(f2, Sender, AnsiString(), params, context);
        }
    }

    void Execute(Function *f, int Sender, AnsiString EventData, short *params = NULL, DataElement *caller_context = NULL) {
        return_value.type = VARIABLE_NUMBER;
        return_value.nval = 0;
        DataElement *context = f->CreateContext();
        int         pidx     = 0;
        int         p        = f->parameters[pidx];

        while (p >= 0) {
            if ((params) && (caller_context)) {
                int p2 = params[pidx];
                if (p2 >= 0) {
                    context[p].type = caller_context[p2].type;
                    //context[p].CodeRef = caller_context[p2].CodeRef;
                    if (caller_context[p2].type == VARIABLE_STRING)
                        context[p].value = caller_context[p2].value;
                    else
                        context[p].nval = caller_context[p2].nval;
                } else {
                    // incorrect number of parameters
                    err = true;
                    f->DestroyContext(context);
                    return;
                }
            } else {
                if (pidx == 0) {
                    context[p].type  = VARIABLE_STRING;
                    context[p].value = AnsiString((long)Sender);
                } else
                if (pidx == 1) {
                    context[p].type  = VARIABLE_STRING;
                    context[p].value = EventData;
                } else {
                    // to many parameters
                    err = true;
                    f->DestroyContext(context);
                    return;
                }
            }
            p = f->parameters[++pidx];
        }
        if (context) {
            int INSTRUCTION_POINTER = 0;
            while ((INSTRUCTION_POINTER < f->codeindex) && (INSTRUCTION_POINTER >= 0)) {
                CodeElement *OE = f->code[INSTRUCTION_POINTER++];
                switch (OE->op) {
                    case KEY_OPTIMIZED_IF:
                        if (OE->left >= 0) {
                            if (context[OE->left].type == VARIABLE_NUMBER) {
                                if (!context[OE->left].nval)
                                    INSTRUCTION_POINTER = OE->right;
                            } else
                            if (context[OE->left].type == VARIABLE_STRING) {
                                if (!context[OE->left].value.Length())
                                    INSTRUCTION_POINTER = OE->right;
                            }
                        } else
                            INSTRUCTION_POINTER = OE->right;
                        break;

                    case KEY_OPTIMIZED_GOTO:
                        INSTRUCTION_POINTER = OE->right;
                        break;

                    case KEY_OPTIMIZED_RETURN:
                        if (OE->right >= 0) {
                            return_value.type = context[OE->right].type;
                            if (return_value.type == VARIABLE_STRING)
                                return_value.value = context[OE->right].value;
                            else
                                return_value.nval = context[OE->right].nval;
                        }
                        f->DestroyContext(context);
                        return;

                    case KEY_OPTIMIZED_ECHO:
                        if (OE->right >= 0) {
                            if (context[OE->right].type == VARIABLE_STRING)
                                fprintf(stderr, "echo: \"%s\"\n", context[OE->right].value.c_str());
                            else
                                fprintf(stderr, "echo: %f\n", context[OE->right].nval);
                        }
                        break;

                    case KEY_OPTIMIZED_THROW:
                    case KEY_OPTIMIZED_TRY_CATCH:
                    case KEY_OPTIMIZED_END_CATCH:
                        // not supported on client
                        err = true;
                        f->DestroyContext(context);
                        return;

                    case KEY_OPTIMIZED_DEBUG_TRAP:
                        // just ignore it
                        break;

                    // unary operators
                    case KEY_DELETE:
                        if (OE->right >= 0) {
                            context[OE->right].type = VARIABLE_NUMBER;
                            context[OE->right].nval = 0;
                        }
                        break;

                    case KEY_TYPE_OF:
                        if (OE->result >= 0) {
                            context[OE->result].type = VARIABLE_STRING;
                            switch (context[OE->right].type) {
                                case VARIABLE_NUMBER:
                                    context[OE->result].value = "numeric";
                                    break;

                                case VARIABLE_STRING:
                                    context[OE->result].value = "string";
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_LENGTH:
                        if (OE->result >= 0) {
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->right].type) {
                                case VARIABLE_STRING:
                                    context[OE->result].nval = context[OE->right].value.Length();
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_VALUE:
                        if (OE->result >= 0) {
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->right].type) {
                                case VARIABLE_NUMBER:
                                    context[OE->result].nval = context[OE->right].nval;
                                    break;

                                case VARIABLE_STRING:
                                    context[OE->result].nval = context[OE->right].value.ToFloat();
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_NOT:
                        if (OE->result >= 0) {
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->right].type) {
                                case VARIABLE_NUMBER:
                                    context[OE->result].nval = context[OE->right].nval ? 0 : 1;
                                    break;

                                case VARIABLE_STRING:
                                    context[OE->result].nval = context[OE->right].value.Length() ? 0 : 1;
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_COM:
                        if (OE->result >= 0) {
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->right].type) {
                                case VARIABLE_NUMBER:
                                    context[OE->result].nval = ~(int)context[OE->right].nval;
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_POZ:
                        if (OE->result >= 0) {
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->right].type) {
                                case VARIABLE_NUMBER:
                                    context[OE->result].nval = context[OE->right].nval;
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                        }
                        break;

                    case KEY_NEG:
                        if (OE->result >= 0) {
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->right].type) {
                                case VARIABLE_NUMBER:
                                    context[OE->result].nval = -context[OE->right].nval;
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_INC:
                        if (OE->left >= 0) {
                            if (context[OE->left].type != VARIABLE_NUMBER) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            if (OE->result >= 0) {
                                context[OE->result].type = VARIABLE_NUMBER;
                                context[OE->result].nval = context[OE->left].nval;
                                PROPERTY_CODE_RESULT
                            }
                            context[OE->left].nval += 1;
                        } else
                        if (OE->right >= 0) {
                            if (context[OE->right].type != VARIABLE_NUMBER) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->right].nval += 1;
                            if (OE->result >= 0) {
                                context[OE->result].type = VARIABLE_NUMBER;
                                context[OE->result].nval = context[OE->right].nval;
                                PROPERTY_CODE_RESULT
                            }
                        } else {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        break;

                    case KEY_DEC:
                        if (OE->left >= 0) {
                            if (context[OE->left].type != VARIABLE_NUMBER) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            if (OE->result >= 0) {
                                context[OE->result].type = VARIABLE_NUMBER;
                                context[OE->result].nval = context[OE->left].nval;
                                PROPERTY_CODE_RESULT
                            }
                            context[OE->left].nval -= 1;
                        } else
                        if (OE->right >= 0) {
                            if (context[OE->right].type != VARIABLE_NUMBER) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->right].nval -= 1;
                            if (OE->result >= 0) {
                                context[OE->result].type = VARIABLE_NUMBER;
                                context[OE->result].nval = context[OE->right].nval;
                                PROPERTY_CODE_RESULT
                            }
                        } else {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        break;

                    // binary operators
                    // compare

                    case KEY_LES:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->left].type) {
                                case VARIABLE_NUMBER:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].nval < context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].nval < context[OE->right].value.ToFloat())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                case VARIABLE_STRING:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].value < AnsiString(context[OE->right].nval))
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].value < context[OE->right].value)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_LEQ:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->left].type) {
                                case VARIABLE_NUMBER:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].nval <= context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].nval <= context[OE->right].value.ToFloat())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                case VARIABLE_STRING:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].value <= AnsiString(context[OE->right].nval))
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].value <= context[OE->right].value)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_GRE:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->left].type) {
                                case VARIABLE_NUMBER:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].nval > context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].nval > context[OE->right].value.ToFloat())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                case VARIABLE_STRING:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].value > AnsiString(context[OE->right].nval))
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].value > context[OE->right].value)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_GEQ:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->left].type) {
                                case VARIABLE_NUMBER:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].nval >= context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].nval >= context[OE->right].value.ToFloat())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                case VARIABLE_STRING:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].value >= AnsiString(context[OE->right].nval))
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].value >= context[OE->right].value)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_NEQ:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->left].type) {
                                case VARIABLE_NUMBER:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].nval != context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].nval != context[OE->right].value.ToFloat())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                case VARIABLE_STRING:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].value != AnsiString(context[OE->right].nval))
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].value != context[OE->right].value)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_EQU:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->left].type) {
                                case VARIABLE_NUMBER:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].nval == context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].nval == context[OE->right].value.ToFloat())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                case VARIABLE_STRING:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].value == AnsiString(context[OE->right].nval))
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].value == context[OE->right].value)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_BAN:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->left].type) {
                                case VARIABLE_NUMBER:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].nval && context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].nval && context[OE->right].value.Length())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                case VARIABLE_STRING:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].value.Length() && context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].value.Length() && context[OE->right].value.Length())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_BOR:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            switch (context[OE->left].type) {
                                case VARIABLE_NUMBER:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].nval || context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].nval || context[OE->right].value.Length())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                case VARIABLE_STRING:
                                    switch (context[OE->right].type) {
                                        case VARIABLE_NUMBER:
                                            if (context[OE->left].value.Length() || context[OE->right].nval)
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        case VARIABLE_STRING:
                                            if (context[OE->left].value.Length() || context[OE->right].value.Length())
                                                context[OE->result].nval = 1;
                                            else
                                                context[OE->result].nval = 0;
                                            break;

                                        default:
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                    }
                                    break;

                                default:
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_SHL:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            if ((context[OE->left].type == VARIABLE_NUMBER) && (context[OE->right].type == VARIABLE_NUMBER)) {
                                context[OE->result].nval = (long)context[OE->left].nval << (long)context[OE->right].nval;
                            } else {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_SHR:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            if ((context[OE->left].type == VARIABLE_NUMBER) && (context[OE->right].type == VARIABLE_NUMBER)) {
                                context[OE->result].nval = (long)context[OE->left].nval >> (long)context[OE->right].nval;
                            } else {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_AND:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            if ((context[OE->left].type == VARIABLE_NUMBER) && (context[OE->right].type == VARIABLE_NUMBER)) {
                                context[OE->result].nval = (long)context[OE->left].nval & (long)context[OE->right].nval;
                            } else {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_XOR:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            if ((context[OE->left].type == VARIABLE_NUMBER) && (context[OE->right].type == VARIABLE_NUMBER)) {
                                context[OE->result].nval = (long)context[OE->left].nval ^ (long)context[OE->right].nval;
                            } else {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_OR:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            if ((context[OE->left].type == VARIABLE_NUMBER) && (context[OE->right].type == VARIABLE_NUMBER)) {
                                context[OE->result].nval = (long)context[OE->left].nval | (long)context[OE->right].nval;
                            } else {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_ASG:
                    case KEY_BY_REF:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = context[OE->right].type;
                        switch (context[OE->right].type) {
                            case VARIABLE_NUMBER:
                                context[OE->left].nval = context[OE->right].nval;
                                break;

                            case VARIABLE_STRING:
                                context[OE->left].value = context[OE->right].value;
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_ASU:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        switch (context[OE->left].type) {
                            context[OE->result].type = VARIABLE_NUMBER;

                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_NUMBER:
                                        context[OE->left].nval += context[OE->right].nval;
                                        break;

                                    case VARIABLE_STRING:
                                        context[OE->left].nval += context[OE->right].value.ToFloat();
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            case VARIABLE_STRING:
                                context[OE->result].type = VARIABLE_STRING;
                                switch (context[OE->right].type) {
                                    case VARIABLE_NUMBER:
                                        context[OE->left].value += AnsiString(context[OE->right].nval);
                                        break;

                                    case VARIABLE_STRING:
                                        context[OE->left].value += context[OE->right].value;
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_ADI:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = VARIABLE_NUMBER;
                        switch (context[OE->left].type) {
                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_NUMBER:
                                        context[OE->left].nval -= context[OE->right].nval;
                                        break;

                                    case VARIABLE_STRING:
                                        context[OE->left].nval -= context[OE->right].value.ToFloat();
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_ADV:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = VARIABLE_NUMBER;
                        switch (context[OE->left].type) {
                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_STRING:
                                        context[OE->right].nval = context[OE->right].value.ToFloat();

                                    case VARIABLE_NUMBER:
                                        if (context[OE->right].nval)
                                            context[OE->left].nval /= context[OE->right].nval;
                                        else {
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                        }
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_ARE:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = VARIABLE_NUMBER;
                        switch (context[OE->left].type) {
                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_STRING:
                                        context[OE->right].nval = context[OE->right].value.ToFloat();

                                    case VARIABLE_NUMBER:
                                        if ((long)context[OE->right].nval)
                                            context[OE->left].nval = (long)context[OE->left].nval % (long)context[OE->right].nval;
                                        else {
                                            err = true;
                                            f->DestroyContext(context);
                                            return;
                                        }
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_AMU:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = VARIABLE_NUMBER;
                        switch (context[OE->left].type) {
                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_NUMBER:
                                        context[OE->left].nval *= context[OE->right].nval;
                                        break;

                                    case VARIABLE_STRING:
                                        context[OE->left].nval *= context[OE->right].value.ToFloat();
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_AAN:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = VARIABLE_NUMBER;
                        switch (context[OE->left].type) {
                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_STRING:
                                        context[OE->right].nval = context[OE->right].value.ToFloat();

                                    case VARIABLE_NUMBER:
                                        context[OE->left].nval = (long)context[OE->left].nval & (long)context[OE->right].nval;
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_AXO:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = VARIABLE_NUMBER;
                        switch (context[OE->left].type) {
                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_STRING:
                                        context[OE->right].nval = context[OE->right].value.ToFloat();

                                    case VARIABLE_NUMBER:
                                        context[OE->left].nval = (long)context[OE->left].nval ^ (long)context[OE->right].nval;
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_AOR:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = VARIABLE_NUMBER;
                        switch (context[OE->left].type) {
                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_STRING:
                                        context[OE->right].nval = context[OE->right].value.ToFloat();

                                    case VARIABLE_NUMBER:
                                        context[OE->left].nval = (long)context[OE->left].nval | (long)context[OE->right].nval;
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_ASL:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = VARIABLE_NUMBER;
                        switch (context[OE->left].type) {
                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_STRING:
                                        context[OE->right].nval = context[OE->right].value.ToFloat();

                                    case VARIABLE_NUMBER:
                                        context[OE->left].nval = (long)context[OE->left].nval << (long)context[OE->right].nval;
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_ASR:
                        if ((OE->right < 0) || (OE->left < 0)) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                        context[OE->result].type = VARIABLE_NUMBER;
                        switch (context[OE->left].type) {
                            case VARIABLE_NUMBER:
                                switch (context[OE->right].type) {
                                    case VARIABLE_STRING:
                                        context[OE->right].nval = context[OE->right].value.ToFloat();

                                    case VARIABLE_NUMBER:
                                        context[OE->left].nval = (long)context[OE->left].nval >> (long)context[OE->right].nval;
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                                break;

                            default:
                                err = true;
                                f->DestroyContext(context);
                                return;
                        }
                        if (OE->result >= 0)
                            context[OE->result].nval = context[OE->left].nval;
                        PROPERTY_CODE
                        break;

                    case KEY_DIV:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            if (context[OE->right].type == VARIABLE_STRING)
                                context[OE->right].nval = context[OE->right].value.ToFloat();

                            if ((context[OE->left].type == VARIABLE_NUMBER) && ((context[OE->right].type == VARIABLE_NUMBER) || (context[OE->right].type == VARIABLE_STRING)) && (context[OE->right].nval)) {
                                context[OE->result].nval = context[OE->left].nval / context[OE->right].nval;
                            } else {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_REM:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            if (context[OE->right].type == VARIABLE_STRING)
                                context[OE->right].nval = context[OE->right].value.ToFloat();

                            if ((context[OE->left].type == VARIABLE_NUMBER) && ((context[OE->right].type == VARIABLE_NUMBER) || (context[OE->right].type == VARIABLE_STRING)) && ((long)context[OE->right].nval)) {
                                context[OE->result].nval = (long)context[OE->left].nval % (long)context[OE->right].nval;
                            } else {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_MUL:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            context[OE->result].type = VARIABLE_NUMBER;
                            if ((context[OE->left].type == VARIABLE_NUMBER) && (context[OE->right].type == VARIABLE_NUMBER)) {
                                context[OE->result].nval = context[OE->left].nval * context[OE->right].nval;
                            } else
                            if ((context[OE->left].type == VARIABLE_NUMBER) && (context[OE->right].type == VARIABLE_STRING)) {
                                context[OE->result].nval = context[OE->left].nval * context[OE->right].value.ToFloat();
                            } else {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_DIF:
                        if (OE->result >= 0) {
                            context[OE->result].type = VARIABLE_NUMBER;
                            if ((OE->right < 0) || (OE->left < 0)) {
                                if ((OE->right >= 0) && (context[OE->right].type == VARIABLE_NUMBER)) {
                                    context[OE->result].nval = -context[OE->right].nval;
                                } else {
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                                }
                            } else {
                                context[OE->result].type = VARIABLE_NUMBER;
                                if ((context[OE->left].type == VARIABLE_NUMBER) && (context[OE->right].type == VARIABLE_NUMBER)) {
                                    context[OE->result].nval = context[OE->left].nval - context[OE->right].nval;
                                } else
                                if ((context[OE->left].type == VARIABLE_NUMBER) && (context[OE->right].type == VARIABLE_STRING)) {
                                    context[OE->result].nval = context[OE->left].nval - context[OE->right].value.ToFloat();
                                } else {
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                                }
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_SUM:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0)) {
                                if ((OE->right < 0) || (context[OE->right].type != VARIABLE_NUMBER)) {
                                    err = true;
                                    f->DestroyContext(context);
                                    return;
                                }
                            } else {
                                switch (context[OE->left].type) {
                                    case VARIABLE_NUMBER:
                                        context[OE->result].type = VARIABLE_NUMBER;
                                        switch (context[OE->right].type) {
                                            case VARIABLE_NUMBER:
                                                context[OE->result].nval = context[OE->left].nval + context[OE->right].nval;
                                                break;

                                            case VARIABLE_STRING:
                                                context[OE->result].nval = context[OE->left].nval + context[OE->right].value.ToFloat();
                                                break;

                                            default:
                                                err = true;
                                                f->DestroyContext(context);
                                                return;
                                        }
                                        break;

                                    case VARIABLE_STRING:
                                        context[OE->result].type = VARIABLE_STRING;
                                        switch (context[OE->right].type) {
                                            case VARIABLE_NUMBER:
                                                context[OE->result].value = context[OE->left].value + AnsiString(context[OE->right].nval);
                                                break;

                                            case VARIABLE_STRING:
                                                context[OE->result].value = context[OE->left].value + context[OE->right].value;
                                                break;

                                            default:
                                                err = true;
                                                f->DestroyContext(context);
                                                return;
                                        }
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_INDEX_OPEN:
                    case KEY_INDEX_CLOSE:
                        if (OE->result >= 0) {
                            if ((OE->right < 0) || (OE->left < 0) || (context[OE->left].type != VARIABLE_STRING)) {
                                err = true;
                                f->DestroyContext(context);
                                return;
                            } else {
                                context[OE->result].type = VARIABLE_STRING;
                                switch (context[OE->right].type) {
                                    case VARIABLE_NUMBER:
                                        context[OE->result].value = context[OE->left].value[(int)context[OE->right].nval];
                                        break;

                                    case VARIABLE_STRING:
                                        context[OE->result].value = context[OE->left].value[(int)context[OE->right].value.ToInt()];
                                        break;

                                    default:
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                }
                            }
                            PROPERTY_CODE_RESULT
                        }
                        break;

                    case KEY_SEL:
                        if (OE->result >= 0)
                            context[OE->result].CodeRef = OE;
                        if (OE->get_def.Length()) {
                            Function *f2     = this->Find(OE->get_def);
                            int      Sender2 = Sender;
                            if ((OE->left >= 0) && (context[OE->left].CodeRef)) {
                                CodeElement *d2 = (CodeElement *)context[OE->left].CodeRef;
                                if (d2->rid >= 0)
                                    Sender2 = d2->rid;
                            }
                            if (f2) {
                                this->Execute(f2, Sender2, AnsiString(), OE->parameters, context);
                                if ((!err) && (OE->result >= 0)) {
                                    context[OE->result].type = return_value.type;
                                    if (return_value.type == VARIABLE_STRING)
                                        context[OE->result].value = return_value.value;
                                    else
                                        context[OE->result].nval = return_value.nval;
                                    PROPERTY_CODE_RESULT
                                }
                            } else
                                err = true;
                        } else
                        if ((OE->right_def.Length()) && (!OE->set_def.Length())) {
                            if (OE->rid == -1) {
                                if (OE->result >= 0) {
                                    if ((OE->left == 0) && ((OE->right_def == (char *)"RID") || (OE->right_def == (char *)"REMOTE_ID"))) {
                                        context[OE->result].type = VARIABLE_NUMBER;
                                        context[OE->result].nval = Sender;
                                        PROPERTY_CODE_RESULT
                                    } else
                                    if ((OE->left == 0) && (OE->right_def == (char *)"REMOTE_ID_STR")) {
                                        context[OE->result].type  = VARIABLE_STRING;
                                        context[OE->result].value = AnsiString((long)Sender);
                                        PROPERTY_CODE_RESULT
                                    } else
                                    if (!IgnoreErrors)
                                        err = true;
                                }
                            }
                        }
                        if (err) {
                            f->DestroyContext(context);
                            return;
                        }
                        break;

                    // just a simple assignment
                    case -2:
                        if ((OE->left >= 0) && (OE->result >= 0)) {
                            context[OE->result].type = context[OE->left].type;
                            if (context[OE->result].type == VARIABLE_STRING)
                                context[OE->result].value = context[OE->left].value;
                            else
                                context[OE->result].nval = context[OE->left].nval;
                            PROPERTY_CODE_RESULT
                            break;
                        }

                    case -1:
                        if (OE->right_def.Length()) {
                            Function *f2 = this->Find(OE->right_def);
                            if (f2) {
                                this->Execute(f2, Sender, AnsiString(), OE->parameters, context);
                                if ((!err) && (OE->result >= 0)) {
                                    context[OE->result].type = return_value.type;
                                    if (return_value.type == VARIABLE_STRING)
                                        context[OE->result].value = return_value.value;
                                    else
                                        context[OE->result].nval = return_value.nval;
                                    PROPERTY_CODE_RESULT
                                }
                            } else
                            if (OE->right_def == (char *)"IsClient") {
                                if (OE->result >= 0) {
                                    context[OE->result].type = VARIABLE_NUMBER;
                                    context[OE->result].nval = 1;
                                }
                            } else
                            if (OE->right_def == (char *)"send_message") {
                                if (this->send_message) {
                                    AnsiString Sender;
                                    AnsiString Target;
                                    AnsiString Value;
                                    int        MSG_ID;

                                    if ((OE->parameters[0] < 0) || (OE->parameters[1] < 0) || (OE->parameters[2] < 0) || (OE->parameters[3] < 0)) {
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                    }
                                    if (context[OE->parameters[0]].type == VARIABLE_STRING)
                                        Sender = context[OE->parameters[0]].value;
                                    else
                                        Sender = AnsiString(context[OE->parameters[0]].nval);

                                    if (context[OE->parameters[1]].type == VARIABLE_STRING)
                                        MSG_ID = context[OE->parameters[1]].value.ToInt();
                                    else
                                        MSG_ID = (int)context[OE->parameters[1]].nval;

                                    if (context[OE->parameters[2]].type == VARIABLE_STRING)
                                        Target = context[OE->parameters[2]].value;
                                    else
                                        Target = AnsiString(context[OE->parameters[2]].nval);

                                    if (context[OE->parameters[3]].type == VARIABLE_STRING)
                                        Value = context[OE->parameters[3]].value;
                                    else
                                        Value = AnsiString(context[OE->parameters[3]].nval);

                                    int res = this->send_message(Sender, MSG_ID, Target, Value, UserData);
                                    if (OE->result >= 0) {
                                        context[OE->result].type = VARIABLE_NUMBER;
                                        context[OE->result].nval = res;
                                    }
                                }
                            } else
                            if (OE->right_def == (char *)"wait_message_ID") {
                                if (this->wait_message) {
                                    AnsiString Sender;
                                    AnsiString Target;
                                    AnsiString Value;
                                    int        MSG_ID;

                                    if ((OE->parameters[0] < 0) || (OE->parameters[1] < 0) || (OE->parameters[2] < 0) || (OE->parameters[3] < 0)) {
                                        err = true;
                                        f->DestroyContext(context);
                                        return;
                                    }
                                    if (context[OE->parameters[0]].type == VARIABLE_STRING)
                                        Sender = context[OE->parameters[0]].value;
                                    else
                                        Sender = AnsiString(context[OE->parameters[0]].nval);

                                    if (context[OE->parameters[1]].type == VARIABLE_STRING)
                                        MSG_ID = context[OE->parameters[1]].value.ToInt();
                                    else
                                        MSG_ID = (int)context[OE->parameters[1]].nval;

                                    if (context[OE->parameters[2]].type == VARIABLE_STRING)
                                        Target = context[OE->parameters[2]].value;
                                    else
                                        Target = AnsiString(context[OE->parameters[2]].nval);

                                    if (context[OE->parameters[3]].type == VARIABLE_STRING)
                                        Value = context[OE->parameters[3]].value;
                                    else
                                        Value = AnsiString(context[OE->parameters[3]].nval);

                                    int res = this->wait_message(Sender, MSG_ID, Target, Value, UserData);
                                    if (OE->result >= 0) {
                                        context[OE->result].type = VARIABLE_NUMBER;
                                        context[OE->result].nval = res;
                                    }
                                    context[OE->parameters[0]].type  = VARIABLE_STRING;
                                    context[OE->parameters[0]].value = Sender;
                                    context[OE->parameters[1]].type  = VARIABLE_NUMBER;
                                    context[OE->parameters[1]].nval  = MSG_ID;
                                    context[OE->parameters[2]].type  = VARIABLE_STRING;
                                    context[OE->parameters[2]].value = Target;
                                    context[OE->parameters[3]].type  = VARIABLE_STRING;
                                    context[OE->parameters[3]].value = Value;
                                }
                            } else
                            if (!IgnoreErrors) {
                                err = true;
                            }
                            if (err) {
                                f->DestroyContext(context);
                                return;
                            }
                            break;
                        }

                    default:
                        fprintf(stderr, "Unsupported operator 0x%x in %s, instruction pointer %i\n", OE->op, f->Name.c_str(), INSTRUCTION_POINTER);
                        if (!IgnoreErrors) {
                            err = true;
                            f->DestroyContext(context);
                            return;
                        }
                }
            }
            f->DestroyContext(context);
        }
    }

public:
    bool IgnoreErrors;

    CompensationParser(PROTOCOL_MESSAGE_CB send, PROTOCOL_MESSAGE_CB wait, void *userdata = NULL) {
        FList        = NULL;
        f_index      = 0;
        send_message = send;
        wait_message = wait;
        UserData     = userdata;
        IgnoreErrors = true;
    }

    ~CompensationParser() {
        if (FList) {
            for (int i = 0; i < f_index; i++)
                delete FList[i];
            free(FList);
        }
    }

    void SetCallbacks(PROTOCOL_MESSAGE_CB send, PROTOCOL_MESSAGE_CB wait, void *userdata = NULL) {
        send_message = send;
        wait_message = wait;
        UserData     = userdata;
    }

    void AddFunction(Function *f) {
        if (f_index % FUNC_BLOCK == 0)
            FList = (Function **)realloc(FList, (f_index + FUNC_BLOCK) * sizeof(Function *));
        if (FList)
            FList[f_index++] = f;
    }

    AnsiString Parse(char *data, int len) {
        AnsiString entry_point;

        for (int i = 0; i < len; i++) {
            char c = data[i];
            switch (c) {
                case '#':
                    entry_point = EntryPoint(data, i, len);
                    break;

                case '@':
                    AddFunction(this->ParseFunction(data, i, len));
                    break;
            }
        }
        return entry_point;
    }

    void NotifyEvent(AnsiString fname, int Sender, AnsiString EventData) {
        err = false;
        Function *f = this->Find(fname);
        if (f)
            Execute(f, Sender, EventData);
    }
};
#endif
