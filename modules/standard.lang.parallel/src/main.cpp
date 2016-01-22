//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"

#include <CL/cl.h>
#include <map>

struct FunctionCall {
    int              *len;
    char             *types;
    cl_mem           *mem;
    int              *realindex;
    int              parameters;
    size_t           worksize;
    cl_command_queue cw;
};

#ifndef CLK_LOCAL_MEM_FENCE
 #define     CLK_LOCAL_MEM_FENCE    (1 << 0)
#endif

#ifndef CLK_GLOBAL_MEM_FENCE
 #define     CLK_GLOBAL_MEM_FENCE    (1 << 1)
#endif

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(CL_DEVICE_TYPE_CPU)
    DEFINE_ECONSTANT(CL_DEVICE_TYPE_GPU)
    DEFINE_ECONSTANT(CL_DEVICE_TYPE_ACCELERATOR)
    DEFINE_ECONSTANT(CL_DEVICE_TYPE_DEFAULT)
    DEFINE_ECONSTANT(CL_DEVICE_TYPE_ALL)
    DEFINE_ECONSTANT(CLK_LOCAL_MEM_FENCE);
    DEFINE_ECONSTANT(CLK_GLOBAL_MEM_FENCE);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PPInit, 0, 1)
    cl_int clerror;
    cl_platform_id platform;
    cl_device_id   device;
    cl_uint        platforms, devices;
    cl_device_type device_type = CL_DEVICE_TYPE_ALL;

    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(0)
        device_type = (cl_device_type)PARAM_INT(0);
    }

    clerror = clGetPlatformIDs(1, &platform, &platforms);
    if (clerror) {
        RETURN_NUMBER(0)
        return 0;
    }
    clerror = clGetDeviceIDs(platform, device_type, 1, &device, &devices);
    if (clerror) {
        RETURN_NUMBER(0)
        return 0;
    }
    cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };

    cl_context context = clCreateContext(properties, 1, &device, NULL, NULL, &clerror);
    if (clerror) {
        RETURN_NUMBER(0)
        return 0;
    }
    cl_command_queue cq = clCreateCommandQueue(context, device, 0, &clerror);
// safe to do this, context seems to be reference-counted
    clReleaseContext(context);
    if (clerror) {
        RETURN_NUMBER(0)
        return 0;
    }
    RETURN_NUMBER((SYS_INT)cq);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PPDone, 1)
    T_NUMBER(0)
    cl_command_queue context = (cl_command_queue)(SYS_INT)PARAM(0);
    if (context) {
        clReleaseCommandQueue(context);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PPCompile, 2, 4)
    T_HANDLE(0)
    T_STRING(1)

    cl_command_queue cw = (cl_command_queue)(SYS_INT)PARAM(0);
    cl_context context = 0;
    clGetCommandQueueInfo(cw, CL_QUEUE_CONTEXT, sizeof(context), &context, NULL);
    if (!context) {
        if (PARAMETERS_COUNT > 3) {
            SET_STRING(3, "");
        }
        RETURN_NUMBER(0);
        return 0;
    }
    const char *srcptr = PARAM(1);
    size_t     srcsize = PARAM_LEN(1);

    cl_int     clerror;
    cl_program prog     = clCreateProgramWithSource(context, 1, &srcptr, &srcsize, &clerror);
    char       *options = "";
    if (PARAMETERS_COUNT > 2) {
        T_STRING(2)
        options = PARAM(2);
    }
    clerror = clBuildProgram(prog, 0, NULL, options, NULL, NULL);
    if (clerror != CL_SUCCESS) {
        if (PARAMETERS_COUNT > 3) {
            cl_device_id device = 0;
            clGetCommandQueueInfo(cw, CL_QUEUE_DEVICE, sizeof(device), &device, NULL);

            char buffer[0x2FFF];
            buffer[0] = 0;
            clGetProgramBuildInfo(prog, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
            SET_STRING(3, buffer);
        }
        clReleaseProgram(prog);
        RETURN_NUMBER(0);
    } else {
        if (PARAMETERS_COUNT > 3) {
            SET_STRING(3, "");
        }
        RETURN_NUMBER((SYS_INT)prog);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PPKernel, 2)
    T_HANDLE(0)
    T_STRING(1)

    cl_int clerror;
    cl_program prog     = (cl_program)(SYS_INT)PARAM(0);
    cl_kernel  k_kernel = clCreateKernel(prog, PARAM(1), &clerror);
    if (clerror) {
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER((SYS_INT)k_kernel);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PPReleaseKernel, 1)
    T_NUMBER(0)
    cl_kernel k_kernel = (cl_kernel)(SYS_INT)PARAM(0);
    if (k_kernel) {
        clReleaseKernel(k_kernel);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PPRelease, 1)
    T_NUMBER(0)
    cl_program k_kernel = (cl_program)(SYS_INT)PARAM(0);
    if (k_kernel) {
        clReleaseProgram(k_kernel);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PPGo, 4)
    T_HANDLE(0)
    T_HANDLE(1)
    T_ARRAY(2)
    T_ARRAY(3)

    cl_command_queue cw = (cl_command_queue)(SYS_INT)PARAM(0);
    cl_context context = 0;
    clGetCommandQueueInfo(cw, CL_QUEUE_CONTEXT, sizeof(context), &context, NULL);

    cl_int    clerror;
    cl_kernel k_kernel = (cl_kernel)(SYS_INT)PARAM(1);

    int len = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));

    cl_uint parameters = 0;
    clGetKernelInfo(k_kernel, CL_KERNEL_NUM_ARGS, sizeof(parameters), &parameters, NULL);

    if (parameters != len)
        return (void *)"PPGo: kenel function takes different number of parameters than received";

    char   *_empty_str   = "";
    size_t max_work_size = 0;
    cl_mem *mem          = 0;
    int    *lengths      = 0;
    char   *types        = 0;
    if (len) {
        mem     = (cl_mem *)malloc(sizeof(cl_mem) * len);
        lengths = (int *)malloc(sizeof(int) * len);
        types   = (char *)malloc(sizeof(char) * len);
    }
    for (INTEGER i = 0; i < len; i++) {
        char    *str = 0;
        NUMBER  nDummy;
        INTEGER type;

        char    *str2 = 0;
        NUMBER  nDummy2;
        INTEGER type2;

        int len2 = 0;

        Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(2), i, &type, &str, &nDummy);
        Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(3), i, &type2, &str2, &nDummy2);
        if (type2 == VARIABLE_NUMBER)
            len2 = (int)nDummy2;
        mem[i]     = 0;
        types[i]   = type;
        lengths[i] = 0;
        if (type == VARIABLE_NUMBER) {
            if (len2 <= 0) {
                float f = nDummy;
                mem[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float), NULL, &clerror);
                clSetKernelArg(k_kernel, i, sizeof(cl_mem), &mem[i]);
                clEnqueueWriteBuffer(cw, mem[i], CL_FALSE, 0, sizeof(f), &f, 0, NULL, NULL);
                if (len2 < 0)
                    lengths[i] = 1;
            } else {
                mem[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float), NULL, &clerror);
                clSetKernelArg(k_kernel, i, sizeof(cl_mem), &mem[i]);
                lengths[i] = 1;
            }
            if (max_work_size < 1)
                max_work_size = 1;
        } else
        if (type == VARIABLE_STRING) {
            if (len2 <= 0) {
                // include the zero character
                int len_str = (int)nDummy + 1;
                if (!str)
                    str = _empty_str;
                mem[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, len_str, NULL, &clerror);
                clSetKernelArg(k_kernel, i, sizeof(cl_mem), &mem[i]);
                clEnqueueWriteBuffer(cw, mem[i], CL_FALSE, 0, len_str, str, 0, NULL, NULL);
                if (max_work_size < len_str - 1)
                    max_work_size = len_str - 1;
                if (len2 < 0)
                    lengths[i] = len_str;
            } else {
                // is out parameter
                mem[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, len2, NULL, &clerror);
                clSetKernelArg(k_kernel, i, sizeof(cl_mem), &mem[i]);
                if (max_work_size < len2)
                    max_work_size = len2;
                lengths[i] = len2;
            }
        } else
        if (type == VARIABLE_ARRAY) {
            if (len2 <= 0) {
                void *arr = 0;
                Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &arr);
                if (arr) {
                    int   len_arr = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
                    float *f      = (float *)malloc(sizeof(float) * (len_arr + 1));
                    f[len_arr] = 0;
                    for (int j = 0; j < len_arr; j++) {
                        f[j] = 0;

                        char    *str3 = 0;
                        NUMBER  nDummy3;
                        INTEGER type3;
                        Invoke(INVOKE_GET_ARRAY_ELEMENT, arr, j, &type3, &str3, &nDummy3);
                        if (type3 == VARIABLE_NUMBER) {
                            f[j] = (float)nDummy3;
                        } else
                        if (type3 == VARIABLE_STRING) {
                            AnsiString temp(str3);
                            f[j] = (float)temp.ToFloat();
                        } else
                            f[j] = 0;
                    }
                    mem[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * len_arr, NULL, &clerror);
                    clSetKernelArg(k_kernel, i, sizeof(cl_mem), &mem[i]);
                    clEnqueueWriteBuffer(cw, mem[i], CL_FALSE, 0, sizeof(float) * len_arr, f, 0, NULL, NULL);
                    if (max_work_size < len_arr)
                        max_work_size = len_arr;
                    free(f);
                    f = 0;
                    if (len2 < 0)
                        lengths[i] = len_arr;
                }
            } else {
                mem[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * len2, NULL, &clerror);
                clSetKernelArg(k_kernel, i, sizeof(cl_mem), &mem[i]);
                if (max_work_size < len2)
                    max_work_size = len2;
                lengths[i] = len2;
            }
        }
    }
    clerror = clEnqueueNDRangeKernel(cw, k_kernel, 1, NULL, &max_work_size, NULL, 0, NULL, NULL);

    FunctionCall *call = (FunctionCall *)malloc(sizeof(FunctionCall));
    call->parameters = len;
    call->mem        = mem;
    call->realindex  = 0;
    call->len        = lengths;
    call->cw         = cw;
    call->types      = types;
    call->worksize   = max_work_size;
    RETURN_NUMBER((SYS_INT)call);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PPFinish, 1)
    T_HANDLE(0)

    cl_int clerror;
    FunctionCall *call = (FunctionCall *)(SYS_INT)PARAM(0);

    CREATE_ARRAY(RESULT);

    int  len       = call->parameters;
    void **buffers = 0;
    if (len) {
        buffers = (void **)malloc(sizeof(void *) * len);
        for (int i = 0; i < len; i++) {
            buffers[i] = 0;
            if (call->len[i] > 0) {
                int element_size = 1;
                if ((call->types[i] == VARIABLE_NUMBER) || (call->types[i] == VARIABLE_ARRAY))
                    element_size = sizeof(float);

                buffers[i] = malloc(call->len[i] * element_size);
                clerror    = clEnqueueReadBuffer(call->cw, call->mem[i], CL_FALSE, 0, call->len[i] * element_size, buffers[i], 0, NULL, NULL);
            }
        }
    }
    clerror = clFinish(call->cw);

    int len2 = 0;
    for (INTEGER i = 0; i < len; i++) {
        cl_mem  m = call->mem[i];
        float   *arr;
        void    *arr2 = 0;
        INTEGER idx   = i;
        if (call->realindex)
            idx = call->realindex[i];
        if (call->len[i] > 0) {
            switch (call->types[i]) {
                case VARIABLE_NUMBER:
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)idx, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)0);
                    break;

                case VARIABLE_STRING:
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)idx, (INTEGER)VARIABLE_STRING, buffers[i], (NUMBER)call->len[i]);
                    break;

                case VARIABLE_ARRAY:
                    arr  = (float *)buffers[i];
                    len2 = call->len[i];
                    Invoke(INVOKE_ARRAY_VARIABLE, RESULT, idx, &arr2);
                    CREATE_ARRAY(arr2);
                    for (INTEGER j = 0; j < len2; j++) {
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, arr2, (INTEGER)j, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)arr[j]);
                    }
                    break;
            }
        }
        if (m)
            clReleaseMemObject(m);
        void *buf = buffers[i];
        if (buf)
            free(buf);
    }
    if (buffers)
        free(buffers);
    if (call->mem)
        free(call->mem);
    if (call->len)
        free(call->len);
    if (call->types)
        free(call->types);
    if (call->realindex)
        free(call->realindex);
    free(call);
    SET_NUMBER(0, 0)
END_IMPL
//------------------------------------------------------------------------
int EstimateLength(TreeContainer *CODE, int codeLen, TreeVD *DATA, int dataLen, int varindex) {
    varindex++;
    int max = 0;
    for (int i = 0; i < codeLen; i++) {
        TreeContainer *OE = &CODE[i];
        if ((OE->Operator_TYPE == TYPE_OPERATOR) && (OE->Operator_ID == KEY_INDEX_OPEN) && (OE->OperandLeft_ID == varindex)) {
            TreeVD *VD = &DATA[OE->OperandRight_ID - 1];
            int    val = (int)VD->nValue + 1;
            if (val > max)
                max = val;
        }
    }
    return max;
}

int CheckArray(TreeContainer *CODE, int codeLen, TreeVD *DATA, int dataLen, int varindex) {
    varindex++;
    for (int i = 0; i < codeLen; i++) {
        TreeContainer *OE = &CODE[i];
        if (OE->Operator_TYPE == TYPE_OPERATOR) {
            if ((OE->Operator_ID == KEY_NEW) && (OE->OperandLeft_ID == -1) && (OE->Result_ID == varindex))
                return VARIABLE_ARRAY;
        }
    }
    return VARIABLE_NUMBER;
}

int CheckArrayParam(TreeContainer *CODE, int codeLen, TreeVD *DATA, int dataLen, int varindex) {
    varindex++;
    for (int i = 0; i < codeLen; i++) {
        TreeContainer *OE = &CODE[i];
        if (OE->Operator_TYPE == TYPE_OPERATOR) {
            if ((OE->Operator_ID == KEY_INDEX_OPEN) && (OE->OperandLeft_ID == varindex))
                return VARIABLE_ARRAY;
        }
    }
    return VARIABLE_NUMBER;
}

int CheckArrayRecursive(TreeContainer *CODE, int codeLen, TreeVD *DATA, int dataLen, int varindex) {
    varindex++;
    for (int i = 0; i < codeLen; i++) {
        TreeContainer *OE = &CODE[i];
        if ((OE->Operator_TYPE == TYPE_OPERATOR) && ((OE->Result_ID == varindex) || (OE->OperandLeft_ID == varindex))) {
            if ((OE->Operator_ID == KEY_ASG) || (OE->Operator_ID == KEY_BY_REF)) {
                if (DATA[OE->OperandRight_ID - 1].TYPE == VARIABLE_STRING)
                    return VARIABLE_STRING;
                if (CheckArray(CODE, codeLen, DATA, dataLen, OE->OperandRight_ID - 1) == VARIABLE_ARRAY)
                    return VARIABLE_ARRAY;
            }
        }
    }
    return VARIABLE_NUMBER;
}

int EstimateLengthRecursive(TreeContainer *CODE, int codeLen, TreeVD *DATA, int dataLen, int varindex) {
    int len = 0;

    if (DATA[varindex].TYPE == VARIABLE_STRING)
        return DATA[varindex].Length;

    varindex++;
    for (int i = 0; i < codeLen; i++) {
        TreeContainer *OE = &CODE[i];
        if ((OE->Operator_TYPE == TYPE_OPERATOR) && ((OE->Result_ID == varindex) || (OE->OperandLeft_ID == varindex))) {
            if ((OE->Operator_ID == KEY_ASG) || (OE->Operator_ID == KEY_BY_REF)) {
                if (DATA[OE->OperandRight_ID - 1].TYPE == VARIABLE_STRING) {
                    int len_s = DATA[OE->OperandRight_ID - 1].Length;
                    if (len_s > len)
                        len = len_s;
                }

                int len2 = EstimateLength(CODE, codeLen, DATA, dataLen, OE->OperandRight_ID - 1);
                if (len2 > len)
                    len = len2;
            }
        }
    }
    return len;
}

int EstimateLengthRecursive2(TreeContainer *CODE, int codeLen, TreeVD *DATA, int dataLen, int varindex) {
    int len = 0;

    if (DATA[varindex].TYPE == VARIABLE_STRING)
        return DATA[varindex].Length;

    varindex++;
    for (int i = 0; i < codeLen; i++) {
        TreeContainer *OE = &CODE[i];
        if ((OE->Operator_TYPE == TYPE_OPERATOR) && ((OE->Result_ID == varindex) || (OE->OperandRight_ID == varindex))) {
            if ((OE->Operator_ID == KEY_ASG) || (OE->Operator_ID == KEY_BY_REF)) {
                if (DATA[OE->OperandLeft_ID - 1].TYPE == VARIABLE_STRING) {
                    int len_s = DATA[OE->OperandLeft_ID - 1].Length;
                    if (len_s > len)
                        len = len_s;
                }

                int len2 = EstimateLength(CODE, codeLen, DATA, dataLen, OE->OperandLeft_ID - 1);
                if (len2 > len)
                    len = len2;
            }
        }
    }
    return len;
}

AnsiString DoCString(char *value, int len) {
    AnsiString res;

    for (int i = 0; i < len; i++) {
        char c = value[i];
        switch (c) {
            case '\a':
                res += (char *)"\\a";
                break;

            case '\b':
                res += (char *)"\\b";
                break;

            case '\f':
                res += (char *)"\\f";
                break;

            case '\n':
                res += (char *)"\\n";
                break;

            case '\r':
                res += (char *)"\\r";
                break;

            case '\t':
                res += (char *)"\\t";
                break;

            case '\v':
                res += (char *)"\\v";
                break;

            case '\\':
                res += (char *)"\\\\";
                break;

            case '\"':
                res += (char *)"\\\"";
                break;

            case '\0':
                res += (char *)"\\0";
                break;

            default:
                res += c;
        }
    }
    return res;
}

unsigned int murmur_hash(const void *key, long len) {
    if (!key)
        return 0;
    const unsigned int  m     = 0x5bd1e995;
    const int           r     = 24;
    unsigned int        seed  = 0x7870AAFF;
    const unsigned char *data = (const unsigned char *)key;
    //int len = strlen((const char *)data);
    if (!len)
        return 0;

    unsigned int h = seed ^ len;

    while (len >= 4) {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len  -= 4;
    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;

        case 2:
            h ^= data[1] << 8;

        case 1:
            h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

AnsiString GenerateCode(void *DELEGATE, INVOKE_CALL Invoke, AnsiString& err, std::map<unsigned int, int> *mapped_func, int is_kernel = 1, AnsiString *cl_funcname = NULL, char *parameters_def = NULL, int *param_def_index = NULL) {
    AnsiString code;
    AnsiString func;

    TreeContainer *CODE   = 0;
    INTEGER       codeLen = 0;

    TreeVD  *DATA   = 0;
    INTEGER dataLen = 0;

    char *class_name  = 0;
    char *member_name = 0;

    Invoke(INVOKE_GET_DELEGATE_NAMES, DELEGATE, &class_name, &member_name);

    unsigned int hash   = murmur_hash(member_name, strlen(member_name));
    int          exists = (*mapped_func)[hash];
    if (exists)
        return code;
    (*mapped_func)[hash] = 1;

    int params = Invoke(INVOKE_COUNT_DELEGATE_PARAMS, DELEGATE);
    if (is_kernel) {
        if (params < 1) {
            err = "PPConcept: the given delegate function must take at least one number parameter (workindex)";
            return code;
        }
        code = "__kernel void ";
    } else
        code = "float ";
    code += class_name;
    code += "_";
    AnsiString mname(member_name);
    int        len = mname.Length();
    for (int i = 0; i < len; i++) {
        if (member_name[i] == '#')
            mname.c_str()[i] = '_';
    }
    code += mname;
    code += "(";

    if (cl_funcname) {
        *cl_funcname  = class_name;
        *cl_funcname += "_";
        *cl_funcname += mname;
    }
    if (param_def_index)
        *param_def_index = 0;
    AnsiString decl;
    int        res = Invoke(INVOKE_DELEGATE_BYTECODE, DELEGATE, &CODE, &codeLen, &DATA, &dataLen);
    if ((DATA) && (CODE)) {
        if ((params < dataLen) && (dataLen > 0)) {
            // VD[0] is this
            if (is_kernel) {
                TreeVD *VD1 = &DATA[1];
                if (VD1->TYPE != VARIABLE_NUMBER) {
                    Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                    err = "PPConcept: the given delegate function must have the first parameter (workindex) defined as number";
                    return code;
                }
                if (VD1->IsRef) {
                    Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                    err = "PPConcept: the workindex parameter should not be a reference";
                    return code;
                }

                /*TreeVD *VD2 = &DATA[2];
                   if (VD1->TYPE!=VARIABLE_NUMBER) {
                    Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                    err="PPConcept: the given delegate function must have the second parameter (localindex) defined as number";
                    return code;
                   }
                   if (VD1->IsRef) {
                    Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                    err="PPConcept: the localindex parameter should not be a reference";
                    return code;
                   }*/
            }
            char *int_params = (char *)malloc(dataLen);
            memset(int_params, 0, dataLen);

            int *refs = (int *)malloc(sizeof(int) * dataLen);
            memset(refs, 0, sizeof(int) * dataLen);

            int *coderefs = (int *)malloc(sizeof(int) * codeLen);
            memset(coderefs, 0, sizeof(int) * codeLen);

            int *used = (int *)malloc(sizeof(int) * dataLen);
            memset(used, 0, sizeof(int) * dataLen);

            for (int i = 0; i < codeLen; i++) {
                TreeContainer *OE = &CODE[i];
                if (OE->Operator_TYPE == TYPE_OPERATOR) {
                    switch (OE->Operator_ID) {
                        case KEY_ASG:
                        case KEY_BY_REF:
                        case KEY_ASU:
                        case KEY_ADI:
                        case KEY_AMU:
                        case KEY_MUL:
                        case KEY_ADV:
                        case KEY_ARE:
                        case KEY_AAN:
                        case KEY_AXO:
                        case KEY_AOR:
                        case KEY_ASL:
                        case KEY_ASR:
                        case KEY_INC:
                        case KEY_INC_LEFT:
                        case KEY_DEC:
                        case KEY_DEC_LEFT:
                            if (refs[OE->OperandLeft_ID - 1])
                                coderefs[i] = refs[OE->OperandLeft_ID - 1];
                            break;
                    }

                    switch (OE->Operator_ID) {
                        case KEY_INC:
                        case KEY_INC_LEFT:
                        case KEY_DEC:
                        case KEY_DEC_LEFT:
                            int_params[OE->OperandLeft_ID - 1] = 1;
                            break;

                        case KEY_XOR:
                        case KEY_BAN:
                        case KEY_BOR:
                        case KEY_OR:
                        case KEY_AND:
                        case KEY_SHL:
                        case KEY_SHR:
                        case KEY_ASL:
                        case KEY_ASR:
                        case KEY_AAN:
                        case KEY_AXO:
                        case KEY_AOR:
                            int_params[OE->Result_ID - 1]       = 1;
                            int_params[OE->OperandLeft_ID - 1]  = 1;
                            int_params[OE->OperandRight_ID - 1] = 1;
                            break;

                        case KEY_INDEX_OPEN:
                            int_params[OE->OperandRight_ID - 1] = 1;
                            refs[OE->Result_ID - 1]             = i + 1;
                            break;

                        case KEY_COM:
                            int_params[OE->OperandLeft_ID - 1] = 1;
                            int_params[OE->Result_ID - 1]      = 1;
                            break;

                        case KEY_LES:
                        case KEY_LEQ:
                        case KEY_GRE:
                        case KEY_GEQ:
                        case KEY_LENGTH:
                            int_params[OE->Result_ID - 1] = 1;
                            break;
                    }
                }

                if (OE->Operator_TYPE == TYPE_OPERATOR) {
                    switch (OE->Operator_ID) {
                        case KEY_DELETE:
                        case KEY_TYPE_OF:
                        case KEY_CLASS_NAME:
                        case KEY_LENGTH:
                            used[OE->OperandLeft_ID - 1]++;
                            break;

                        case KEY_ASG:
                        case KEY_BY_REF:
                        case KEY_ASU:
                        case KEY_ADI:
                        case KEY_AMU:
                        case KEY_MUL:
                        case KEY_ADV:
                        case KEY_DIV:
                        case KEY_SUM:
                        case KEY_DIF:
                        case KEY_LES:
                        case KEY_LEQ:
                        case KEY_GRE:
                        case KEY_GEQ:
                        case KEY_EQU:
                        case KEY_NEQ:
                        case KEY_CND_NULL:
                        case KEY_REM:
                        case KEY_ARE:
                        case KEY_BOR:
                        case KEY_BAN:
                        case KEY_AAN:
                        case KEY_AND:
                        case KEY_AXO:
                        case KEY_AOR:
                        case KEY_ASL:
                        case KEY_ASR:
                        case KEY_SHL:
                        case KEY_SHR:
                        case KEY_XOR:
                        case KEY_OR:
                        case KEY_INDEX_OPEN:
                            used[OE->OperandRight_ID - 1]++;
                            used[OE->OperandLeft_ID - 1]++;
                            break;

                        case KEY_INC:
                        case KEY_DEC:
                        case KEY_INC_LEFT:
                        case KEY_DEC_LEFT:
                        case KEY_POZ:
                        case KEY_VALUE:
                        case KEY_NOT:
                        case KEY_NEG:
                        case KEY_COM:
                            used[OE->OperandLeft_ID - 1]++;
                            break;

                        case KEY_DLL_CALL:
                        case KEY_SEL:
                            if ((OE->Function) && (OE->Function[0])) {
                                for (int k = 0; k < OE->ParametersCount; k++)
                                    used[OE->Parameters[k] - 1]++;
                                break;
                            }
                            break;
                    }
                } else
                if (OE->Operator_TYPE == TYPE_OPTIMIZED_KEYWORD) {
                    switch (OE->Operator_ID) {
                        case KEY_OPTIMIZED_THROW:
                        case KEY_OPTIMIZED_RETURN:
                        case KEY_OPTIMIZED_ECHO:
                        case KEY_OPTIMIZED_IF:
                            used[OE->OperandRight_ID - 1]++;
                            break;
                    }
                }
            }

            AnsiString type_temp;
            for (int i = is_kernel; i < params; i++) {
                TreeVD *VD = &DATA[i + 1];
                if ((VD->TYPE == VARIABLE_NUMBER) || (VD->TYPE == -VARIABLE_NUMBER)) {
                    if (CheckArrayParam(CODE, codeLen, DATA, dataLen, i + 1) == VARIABLE_ARRAY)
                        VD->TYPE = VARIABLE_ARRAY;
                }
                switch (VD->TYPE) {
                    case VARIABLE_NUMBER:
                    case -VARIABLE_NUMBER:
                        if (i > is_kernel)
                            code += ", ";
                        type_temp = "float";
                        if (int_params[i + 1])
                            type_temp = "int";

                        code += type_temp;
                        if (is_kernel) {
                            if (VD->IsRef) {
                                if (parameters_def)
                                    parameters_def[(*param_def_index)++] = 'n';
                                code += "__global ";
                            } else {
                                if (parameters_def)
                                    parameters_def[(*param_def_index)++] = 'N';
                                code += "__global const ";
                            }
                            code += "* ref_var";

                            code += AnsiString((long)i + 1);
                            decl += "\tfloat var";
                            decl += AnsiString((long)i + 1);
                            decl += " = *ref_var";
                            decl += AnsiString((long)i + 1);
                            decl += ";\n";
                        } else {
                            if (VD->IsRef)
                                code += " var";
                            else
                                code += " var";
                            code += AnsiString((long)i + 1);
                        }
                        break;

                    case -VARIABLE_ARRAY:
                    case VARIABLE_ARRAY:
                        if (i > is_kernel)
                            code += ", ";
                        if (is_kernel) {
                            if (VD->IsRef) {
                                if (parameters_def)
                                    parameters_def[(*param_def_index)++] = 'a';
                                code += "__global float* var";
                            } else {
                                if (parameters_def)
                                    parameters_def[(*param_def_index)++] = 'A';
                                code += "__global const float* var";
                            }
                            code += AnsiString((long)i + 1);
                            code += ", __global float *length_var";
                            code += AnsiString((long)i + 1);
                        } else {
                            code += "__global float* var";
                            code += AnsiString((long)i + 1);
                            code += ", float length_var";
                            code += AnsiString((long)i + 1);
                        }
                        break;

                    case VARIABLE_STRING:
                    case -VARIABLE_STRING:
                        if (i > is_kernel)
                            code += ", ";
                        if (is_kernel) {
                            if (VD->IsRef) {
                                if (parameters_def)
                                    parameters_def[(*param_def_index)++] = 's';
                                code += "__global char* var";
                            } else {
                                if (parameters_def)
                                    parameters_def[(*param_def_index)++] = 'S';
                                code += "__global const char* var";
                            }
                            code += AnsiString((long)i + 1);
                            code += ", __global float *length_var";
                            code += AnsiString((long)i + 1);
                        } else {
                            code += "__global char* var";
                            code += AnsiString((long)i + 1);
                            code += ", float length_var";
                            code += AnsiString((long)i + 1);
                        }
                        break;

                    default:
                        free(refs);
                        free(coderefs);
                        free(int_params);
                        free(used);
                        Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                        err = "PPConcept: unsupported parameter type";
                        return code;
                }
            }
            if (is_kernel) {
                decl += "\tconst uint var1 = get_global_id(0);\n";
                //decl+="\tconst uint var2 = get_local_id(0);\n";
            }
            int estimate_len;
            for (int i = params + 1; i < dataLen; i++) {
                TreeVD *VD    = &DATA[i];
                bool   is_ref = false;
                if (VD->TYPE == VARIABLE_NUMBER) {
                    VD->TYPE = CheckArray(CODE, codeLen, DATA, dataLen, i);
                    if (VD->TYPE == VARIABLE_NUMBER) {
                        VD->TYPE = CheckArrayRecursive(CODE, codeLen, DATA, dataLen, i);
                        is_ref   = true;
                    }
                } else
                if (int_params[i])
                    VD->TYPE = VARIABLE_NUMBER;

                switch (VD->TYPE) {
                    case VARIABLE_NUMBER:
                    case -VARIABLE_NUMBER:
                        if (int_params[i])
                            decl += "\tint var";
                        else
                            decl += "\tfloat var";
                        decl += AnsiString((long)i);
                        decl += " = ";
                        decl += AnsiString(VD->nValue);
                        decl += ";\n";
                        break;

                    case -VARIABLE_ARRAY:
                    case VARIABLE_ARRAY:
                        if (is_ref) {
                            decl += "\tfloat *var";
                            decl += AnsiString((long)i);
                            decl += " = 0;\n";
                        } else {
                            decl        += "\tfloat var";
                            decl        += AnsiString((long)i);
                            estimate_len = EstimateLength(CODE, codeLen, DATA, dataLen, i);
                            if (!estimate_len)
                                estimate_len = EstimateLengthRecursive2(CODE, codeLen, DATA, dataLen, i);

                            if (!estimate_len) {
                                free(refs);
                                free(coderefs);
                                free(int_params);
                                free(used);
                                Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                                err = "PPConcept: unable to estimate array length. Please add a static reference, for example array[1000]";
                                return code;
                            }
                            decl += "[";
                            decl += AnsiString((long)estimate_len);
                            decl += "];\n";
                        }
                        break;

                    case VARIABLE_STRING:
                    case -VARIABLE_STRING:
                        if (is_ref) {
                            decl += "\tchar *var";
                            decl += AnsiString((long)i);
                            decl += " = 0;\n";
                        } else {
                            if (VD->Length > 0) {
                                decl += "\tchar var";
                                decl += AnsiString((long)i);
                                decl += "[] = \"";
                                decl += DoCString(VD->Value, VD->Length);
                                decl += "\";\n";
                            } else {
                                decl += "\tchar var";
                                decl += AnsiString((long)i);
                                decl += " = 0;\n";
                            }
                        }
                        break;

                    default:
                        free(refs);
                        free(coderefs);
                        free(int_params);
                        free(used);
                        Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                        err = "PPConcept: unsupported variable type";
                        return code;
                }
            }
            //decl+="\tbarrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);\n";

            for (int i = 0; i < codeLen; i++) {
                TreeContainer *OE = &CODE[i];
                decl += "l_";
                decl += AnsiString(i);
                decl += ":";
                if (OE->Operator_TYPE == TYPE_OPTIMIZED_KEYWORD) {
                    if (OE->Operator_ID == KEY_OPTIMIZED_IF) {
                        decl += "\tif (!(var";
                        decl += AnsiString((long)OE->OperandRight_ID - 1);
                        if (OE->OperandReserved_ID >= codeLen) {
                            decl += ")) return;";
                        } else {
                            decl += ")) goto l_";
                            decl += AnsiString((long)OE->OperandReserved_ID);
                            decl += ";";
                        }
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_GOTO) {
                        if (OE->OperandReserved_ID >= codeLen) {
                            decl += "\treturn;";
                        } else {
                            decl += "\tgoto l_";
                            decl += AnsiString((long)OE->OperandReserved_ID);
                            decl += ";";
                        }
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_RETURN) {
                        if (is_kernel)
                            decl += "\treturn;";
                        else {
                            decl += "\treturn var";
                            decl += AnsiString((long)OE->OperandRight_ID - 1);
                            decl += ";";
                        }
                    } else
                    if (OE->Operator_ID == KEY_OPTIMIZED_ECHO) {
                        if (DATA[OE->OperandRight_ID - 1].TYPE == VARIABLE_STRING)
                            decl += "\tprintf(\"%s\", ";
                        else
                        if (int_params[OE->OperandRight_ID - 1])
                            decl += "\tprintf(\"%i\", (int)";
                        else
                            decl += "\tprintf(\"%f\", (float)";
                        decl += AnsiString((long)OE->OperandRight_ID - 1);
                        decl += ");";
                    } else {
                        free(refs);
                        free(coderefs);
                        free(int_params);
                        free(used);
                        Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                        err = "PPConcept: debugger and exceptions are not supported in parallel functions";
                        return code;
                    }
                } else
                if (OE->Operator_TYPE == TYPE_OPERATOR) {
                    decl += "\t";
                    switch (OE->Operator_ID) {
                        case KEY_ASG:
                        case KEY_BY_REF:
                            if (used[OE->Result_ID - 1]) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "var";
                            //if (coderefs[i]) {
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " = var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1])
                                decl += ");";
                            else
                                decl += ";";
                            break;

                        case KEY_INC:
                            if (used[OE->Result_ID - 1]) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ";
                            }
                            //if (coderefs[i]) {
                            if (coderefs[i]) {
                                if (used[OE->Result_ID - 1])
                                    decl += "(";
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += "var";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]+=1";
                                if (used[OE->Result_ID - 1])
                                    decl += ")";
                            } else {
                                decl += "++var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            }
                            decl += ";";
                            break;

                        case KEY_INC_LEFT:
                            if (used[OE->Result_ID - 1]) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ";
                                if (coderefs[i]) {
                                    TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                    decl += "var";
                                    decl += AnsiString(OE2->OperandLeft_ID - 1);
                                    decl += "[var";
                                    decl += AnsiString(OE2->OperandRight_ID - 1);
                                    decl += "];";
                                }
                            }
                            decl += "var";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]+=1;";
                            } else {
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += "++;";
                            }
                            break;

                        case KEY_DEC:
                            if (used[OE->Result_ID - 1]) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ";
                            }
                            if (coderefs[i]) {
                                if (used[OE->Result_ID - 1])
                                    decl += "(";
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += "var";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]-=1";
                                if (used[OE->Result_ID - 1])
                                    decl += ")";
                            } else {
                                decl += "--var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            }
                            decl += ";";
                            break;

                        case KEY_DEC_LEFT:
                            if (used[OE->Result_ID - 1]) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ";
                                if (coderefs[i]) {
                                    TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                    decl += "var";
                                    decl += AnsiString(OE2->OperandLeft_ID - 1);
                                    decl += "[var";
                                    decl += AnsiString(OE2->OperandRight_ID - 1);
                                    decl += "];";
                                }
                            }
                            decl += "var";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]-=1;";
                            } else {
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += "--;";
                            }
                            break;

                        case KEY_INDEX_OPEN:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += "];";
                            }
                            break;

                        case KEY_ASU:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "var";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " += var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1] > 1)
                                decl += ");";
                            else
                                decl += ";";
                            break;

                        case KEY_LES:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " < var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ");";
                            }
                            break;

                        case KEY_LEQ:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " <= var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ");";
                            }
                            break;

                        case KEY_GRE:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " > var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ");";
                            }
                            break;

                        case KEY_GEQ:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " >= var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ");";
                            }
                            break;

                        case KEY_SUM:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " + var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_DIF:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " + var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_ADI:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "var";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " -= var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1] > 1)
                                decl += ");";
                            else
                                decl += ";";
                            break;

                        case KEY_AMU:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "var";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " *= var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1] > 1)
                                decl += ");";
                            else
                                decl += ";";
                            break;

                        case KEY_ADV:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (";
                            }
                            decl += "var";
                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "]";
                            } else
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " /= var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            if (used[OE->Result_ID - 1] > 1)
                                decl += ");";
                            else
                                decl += ";";
                            break;

                        case KEY_MUL:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " * var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_DIV:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " / var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_EQU:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " == var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ");";
                            }
                            break;

                        case KEY_NEQ:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " != var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ");";
                            }
                            break;

                        case KEY_CND_NULL:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " ? var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " : var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ");";
                            }
                            break;

                        case KEY_REM:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (int)var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " % (int)var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_ARE:
                            decl += "var";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = (";
                            decl += "(int)var";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " % (int)var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            decl += ");";

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " var";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_BOR:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " || var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_BAN:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " && var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_AAN:
                            decl += "var";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = (var";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " &= (int)var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            decl += ");";

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " var";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_AND:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (int)var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " & (int)var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_AXO:
                            decl += "var";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = (var";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " ^= (int)var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            decl += ");";

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " var";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_AOR:
                            decl += "var";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = (var";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " |= (int)var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            decl += ");";

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " var";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_ASL:
                            decl += "var";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = (var";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " <<= (int)var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            decl += ");";

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " var";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_ASR:
                            decl += "var";
                            decl += AnsiString(OE->Result_ID - 1);
                            decl += " = (var";
                            decl += AnsiString(OE->OperandLeft_ID - 1);
                            decl += " >>= (int)var";
                            decl += AnsiString(OE->OperandRight_ID - 1);
                            decl += ");";

                            if (coderefs[i]) {
                                TreeContainer *OE2 = &CODE[coderefs[i] - 1];
                                decl += " var";
                                decl += AnsiString(OE2->OperandLeft_ID - 1);
                                decl += "[var";
                                decl += AnsiString(OE2->OperandRight_ID - 1);
                                decl += "] = ";
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_SHL:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (int)var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " << (int)var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_SHR:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (int)var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " >> (int)var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_XOR:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (int)var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " ^ (int)var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_OR:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = (int)var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += " | (int)var";
                                decl += AnsiString(OE->OperandRight_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_POZ:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_NOT:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = !var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_NEG:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = -(int)var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_COM:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ~(int)var";
                                decl += AnsiString(OE->OperandLeft_ID - 1);
                                decl += ";";
                            }
                            break;

                        case KEY_LENGTH:
                            if (used[OE->Result_ID - 1] > 0) {
                                decl += "var";
                                decl += AnsiString(OE->Result_ID - 1);
                                decl += " = ";
                                if (OE->OperandLeft_ID <= params) {
                                    decl += "*length_var";
                                    decl += AnsiString(OE->OperandLeft_ID - 1);
                                } else {
                                    if (!estimate_len)
                                        estimate_len = EstimateLength(CODE, codeLen, DATA, dataLen, OE->OperandLeft_ID - 1);
                                    if (!estimate_len)
                                        estimate_len = EstimateLengthRecursive(CODE, codeLen, DATA, dataLen, OE->OperandLeft_ID - 1);
                                    if (!estimate_len) {
                                        free(refs);
                                        free(coderefs);
                                        free(int_params);
                                        free(used);
                                        Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                                        err = "PPConcept: unable to estimate array length";
                                        return code;
                                    }
                                    decl += AnsiString((long)estimate_len);
                                }
                                decl += ";";
                            }
                            break;

                        case KEY_SEL:
                            if ((OE->OperandLeft_ID == 1) && (OE->Function)) {
                                if (OE->ParametersCount > -1) {
                                    void *DELEGATE2 = 0;
                                    CREATE_VARIABLE(DELEGATE2);
                                    Invoke(INVOKE_CREATE_DELEGATE, DELEGATE, DELEGATE2, OE->Function);
                                    func += GenerateCode(DELEGATE2, Invoke, err, mapped_func, 0);
                                    FREE_VARIABLE(DELEGATE2);
                                    if (err.Length())
                                        return err;
                                    if (used[OE->Result_ID - 1] > 0) {
                                        decl += "var";
                                        decl += AnsiString(OE->Result_ID - 1);
                                        decl += " = ";
                                    }
                                    decl += class_name;
                                    decl += "_";
                                    decl += OE->Function;
                                    decl += "(";
                                    for (int k = 0; k < OE->ParametersCount; k++) {
                                        if (k)
                                            decl += ", ";
                                        decl += "var";
                                        int param = OE->Parameters[k] - 1;
                                        decl += AnsiString((long)param);
                                        if (param <= params) {
                                            if ((DATA[param].TYPE == VARIABLE_ARRAY) || (DATA[param].TYPE == VARIABLE_STRING) || (DATA[param].TYPE == -VARIABLE_ARRAY) || (DATA[param].TYPE == -VARIABLE_STRING)) {
                                                decl += ", ";
                                                if (is_kernel)
                                                    decl += "*length_var";
                                                else
                                                    decl += "length_var";
                                                decl += AnsiString((long)param);
                                            }
                                        } else {
                                            int elen = EstimateLength(CODE, codeLen, DATA, dataLen, param);
                                            if (!elen)
                                                elen = EstimateLengthRecursive(CODE, codeLen, DATA, dataLen, param);
                                            if (elen) {
                                                decl += ", ";
                                                decl += AnsiString((long)elen);
                                            }
                                        }
                                    }
                                    decl += ");";
                                } else {
                                    void    *VARIABLE = 0;
                                    char    *str4     = 0;
                                    NUMBER  nDummy4;
                                    INTEGER type4 = 0;

                                    char    *str5 = 0;
                                    NUMBER  nDummy5;
                                    INTEGER type5 = 0;

                                    Invoke(INVOKE_GET_VARIABLE, DELEGATE, &type4, &str4, &nDummy4);
                                    if (type4 == VARIABLE_DELEGATE)
                                        Invoke(INVOKE_GET_CLASS_MEMBER, str4, OE->Function, &type5, &str5, &nDummy5);

                                    if (type5 == VARIABLE_NUMBER) {
                                        if (used[OE->Result_ID - 1] > 0) {
                                            decl += "var";
                                            decl += AnsiString(OE->Result_ID - 1);
                                            decl += " = ";
                                            decl += AnsiString(nDummy5);
                                            decl += ";";
                                        }
                                    } else {
                                        free(refs);
                                        free(coderefs);
                                        free(int_params);
                                        free(used);
                                        Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                                        err = "PPConcept: only numeric class members may be referenced";
                                        return code;
                                    }
                                }
                            } else {
                                free(refs);
                                free(coderefs);
                                free(int_params);
                                free(used);
                                Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                                err = "PPConcept: only the functions declared in the current class may be called";
                                return code;
                            }
                            break;

                        case KEY_DLL_CALL:
                            // ignore profiler
                            if ((OE->OperandLeft_ID == -2) && (OE->Function) && (OE->Function[0]) && (strcmp(OE->Function, "__profile"))) {
                                if (used[OE->Result_ID - 1] > 0) {
                                    decl += "var";
                                    decl += AnsiString(OE->Result_ID - 1);
                                    decl += " = ";
                                }
                                if (!strcmp(OE->Function, "ord"))
                                    decl += "(float)";
                                else
                                if (!strcmp(OE->Function, "chr"))
                                    decl += "(char)";
                                else
                                if ((!strcmp(OE->Function, "PPNative")) && (OE->ParametersCount == 1)) {
                                    decl += DATA[OE->Parameters[0] - 1].Value;
                                    break;
                                } else
                                    decl += OE->Function;
                                decl += "(";
                                for (int k = 0; k < OE->ParametersCount; k++) {
                                    if (k)
                                        decl += ", ";
                                    decl += "var";
                                    decl += AnsiString(OE->Parameters[k] - 1);
                                }
                                decl += ");";
                                break;
                            }

                        case KEY_NEW:
                            if (OE->OperandLeft_ID == -1) {
                                // array creation
                                break;
                            }

                        case KEY_VALUE:
                        default:
                            free(refs);
                            free(coderefs);
                            free(int_params);
                            free(used);
                            Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                            err = "PPConcept: unsupported operator encountered (new Class, value, classof, ., ::, ->, typeof...)";
                            return code;
                    }
                } else {
                    free(refs);
                    free(coderefs);
                    free(int_params);
                    free(used);
                    Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
                    err = "PPConcept: unsupported code element";
                    return code;
                }
                decl += "\n";
            }
            free(int_params);
            free(coderefs);
            free(refs);
            free(used);
        }
        Invoke(INVOKE_BYTECODE_FREE, CODE, DATA);
    }

    code += ") {\n";
    code += decl;
    code += "}\n\n";

    return func + code;
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PPConcept, 1, 3)
    T_DELEGATE(0)

    static AnsiString err;
    AnsiString cl_funcname;

    std::map<unsigned int, int> mapped_functions;
    char       parameters[0xFFF];
    int        param_len = 0;
    AnsiString code      = GenerateCode(PARAMETER(0), Invoke, err, &mapped_functions, 1, &cl_funcname, parameters, &param_len);
    parameters[param_len] = 0;
    if (PARAMETERS_COUNT > 1) {
        SET_STRING(1, cl_funcname.c_str());
    }
    if (PARAMETERS_COUNT > 2) {
        SET_STRING(2, parameters);
    }
    if (err.Length()) {
        RETURN_STRING("");
        return (void *)err.c_str();
    }
    RETURN_STRING(code.c_str());
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PPConceptGo, 4)
    T_HANDLE(0)
    T_HANDLE(1)
    T_ARRAY(2)
    T_STRING(3)

    cl_command_queue cw = (cl_command_queue)(SYS_INT)PARAM(0);
    cl_context context = 0;
    clGetCommandQueueInfo(cw, CL_QUEUE_CONTEXT, sizeof(context), &context, NULL);

    cl_int    clerror;
    cl_kernel k_kernel = (cl_kernel)(SYS_INT)PARAM(1);

    int len = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));

    cl_uint parameters = PARAM_LEN(3);

    if (parameters != len)
        return (void *)"PPConceptGo: kenel function takes different number of parameters than received";

    char   *_empty_str   = "";
    size_t max_work_size = 0;
    cl_mem *mem          = 0;
    int    *lengths      = 0;
    int    *realindex    = 0;
    char   *types        = 0;
    if (len) {
        mem       = (cl_mem *)malloc(sizeof(cl_mem) * len * 2);
        lengths   = (int *)malloc(sizeof(int) * len * 2);
        types     = (char *)malloc(sizeof(char) * len * 2);
        realindex = (int *)malloc(sizeof(int) * len * 2);

        memset(mem, 0, sizeof(cl_mem) * len * 2);
        memset(lengths, 0, sizeof(int) * len * 2);
        memset(types, 0, sizeof(char) * len * 2);
        memset(realindex, 0, sizeof(int) * len * 2);
    }
    char *params = PARAM(3);
    int  idx     = 0;

    for (INTEGER i = 0; i < len; i++) {
        char    *str = 0;
        NUMBER  nDummy;
        INTEGER type;

        Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(2), i, &type, &str, &nDummy);
        char    *str2 = 0;
        NUMBER  nDummy2;
        INTEGER type2;

        int len2 = 0;
        switch (params[i]) {
            case 'A':
                if (type != VARIABLE_ARRAY) {
                    if (mem) {
                        free(mem);
                        free(lengths);
                        free(types);
                    }
                    return (void *)"PPConceptGo: invalid parameter received (expected array)";
                }
                break;

            case 'a':
                if (type != VARIABLE_ARRAY) {
                    if (mem) {
                        free(mem);
                        free(lengths);
                        free(types);
                    }
                    return (void *)"PPConceptGo: invalid parameter received (expected array reference)";
                } else {
                    void *arr = 0;
                    Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &arr);
                    if (arr)
                        len2 = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
                }
                break;

            case 'S':
                if (type != VARIABLE_STRING) {
                    if (mem) {
                        free(mem);
                        free(lengths);
                        free(types);
                    }
                    return (void *)"PPConceptGo: invalid parameter received (expected string)";
                }
                break;

            case 's':
                if (type != VARIABLE_STRING) {
                    if (mem) {
                        free(mem);
                        free(lengths);
                        free(types);
                    }
                    return (void *)"PPConceptGo: invalid parameter received (expected string reference)";
                }
                len2 = (int)nDummy;
                break;

            case 'N':
                if (type != VARIABLE_NUMBER) {
                    if (mem) {
                        free(mem);
                        free(lengths);
                        free(types);
                    }
                    return (void *)"PPConceptGo: invalid parameter received (expected number)";
                }
                break;

            case 'n':
                if (type != VARIABLE_NUMBER) {
                    if (mem) {
                        free(mem);
                        free(lengths);
                        free(types);
                    }
                    return (void *)"PPConceptGo: invalid parameter received (expected number reference)";
                }
                break;
        }

        mem[idx]       = 0;
        types[idx]     = type;
        lengths[idx]   = 0;
        realindex[idx] = i;
        if (type == VARIABLE_NUMBER) {
            if (len2 <= 0) {
                float f = nDummy;
                mem[idx] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float), NULL, &clerror);
                clSetKernelArg(k_kernel, idx, sizeof(cl_mem), &mem[idx]);
                clEnqueueWriteBuffer(cw, mem[idx], CL_FALSE, 0, sizeof(f), &f, 0, NULL, NULL);
                if (len2 < 0)
                    lengths[idx] = 1;
            } else {
                mem[idx] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float), NULL, &clerror);
                clSetKernelArg(k_kernel, idx, sizeof(cl_mem), &mem[idx]);
                lengths[idx] = 1;
            }
            if (max_work_size < 1)
                max_work_size = 1;
        } else
        if (type == VARIABLE_STRING) {
            if (len2 <= 0) {
                // include the zero character
                int len_str = (int)nDummy + 1;
                if (!str)
                    str = _empty_str;
                mem[idx] = clCreateBuffer(context, CL_MEM_READ_ONLY, len_str, NULL, &clerror);
                clSetKernelArg(k_kernel, idx, sizeof(cl_mem), &mem[idx]);
                clEnqueueWriteBuffer(cw, mem[idx], CL_FALSE, 0, len_str, str, 0, NULL, NULL);
                if (max_work_size < len_str - 1)
                    max_work_size = len_str - 1;
                //if (len2<0)
                //    lengths[idx]=len_str;
            } else {
                // is out parameter
                mem[idx] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, len2, NULL, &clerror);
                clSetKernelArg(k_kernel, idx, sizeof(cl_mem), &mem[idx]);
                if (max_work_size < len2)
                    max_work_size = len2;
                lengths[idx] = len2;
            }
            idx++;
            float f = nDummy;
            types[idx] = VARIABLE_NUMBER;
            mem[idx]   = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float), NULL, &clerror);
            clSetKernelArg(k_kernel, idx, sizeof(cl_mem), &mem[idx]);
            clEnqueueWriteBuffer(cw, mem[idx], CL_FALSE, 0, sizeof(f), &f, 0, NULL, NULL);
            //lengths[idx]=0;
        } else
        if (type == VARIABLE_ARRAY) {
            if (len2 <= 0) {
                void *arr = 0;
                Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &arr);
                if (arr) {
                    int   len_arr = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
                    float *f      = (float *)malloc(sizeof(float) * (len_arr + 1));
                    f[len_arr] = 0;
                    for (int j = 0; j < len_arr; j++) {
                        f[j] = 0;

                        char    *str3 = 0;
                        NUMBER  nDummy3;
                        INTEGER type3;
                        Invoke(INVOKE_GET_ARRAY_ELEMENT, arr, j, &type3, &str3, &nDummy3);
                        if (type3 == VARIABLE_NUMBER) {
                            f[j] = (float)nDummy3;
                        } else
                        if (type3 == VARIABLE_STRING) {
                            AnsiString temp(str3);
                            f[j] = (float)temp.ToFloat();
                        } else
                            f[j] = 0;
                    }
                    mem[idx] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * len_arr, NULL, &clerror);
                    clSetKernelArg(k_kernel, idx, sizeof(cl_mem), &mem[idx]);
                    clEnqueueWriteBuffer(cw, mem[idx], CL_FALSE, 0, sizeof(float) * len_arr, f, 0, NULL, NULL);
                    if (max_work_size < len_arr)
                        max_work_size = len_arr;
                    free(f);
                    //if (len2<0)
                    //    lengths[idx]=len_arr;
                }
            } else {
                mem[idx] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * len2, NULL, &clerror);
                clSetKernelArg(k_kernel, idx, sizeof(cl_mem), &mem[idx]);
                if (max_work_size < len2)
                    max_work_size = len2;
                lengths[idx] = len2;
            }
            idx++;
            float f;
            if (len2 <= 0) {
                void *arr = 0;
                Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &arr);
                if (arr)
                    f = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
            } else
                f = len2;
            types[idx] = VARIABLE_NUMBER;
            mem[idx]   = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float), NULL, &clerror);
            clSetKernelArg(k_kernel, idx, sizeof(cl_mem), &mem[idx]);
            clEnqueueWriteBuffer(cw, mem[idx], CL_FALSE, 0, sizeof(f), &f, 0, NULL, NULL);
            //lengths[idx]=0;
        }
        idx++;
    }
    clerror = clEnqueueNDRangeKernel(cw, k_kernel, 1, NULL, &max_work_size, NULL, 0, NULL, NULL);

    FunctionCall *call = (FunctionCall *)malloc(sizeof(FunctionCall));
    call->parameters = idx;
    call->mem        = mem;
    call->len        = lengths;
    call->realindex  = realindex;
    call->cw         = cw;
    call->types      = types;
    call->worksize   = max_work_size;
    RETURN_NUMBER((SYS_INT)call);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(get_global_id, 1)
    T_NUMBER(0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(get_local_id, 1)
    T_NUMBER(0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(get_global_size, 1)
    T_NUMBER(0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(get_local_size, 1)
    T_NUMBER(0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(barrier, 1)
    T_NUMBER(0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PPNative, 1)
    T_STRING(0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(get_work_dim, 0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(get_num_groups, 1)
    T_NUMBER(0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(get_group_id, 1)
    T_NUMBER(0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(get_global_offset, 1)
    T_NUMBER(0)
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
