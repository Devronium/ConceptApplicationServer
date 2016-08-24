//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <gmp.h>
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(mpz_init, 0, 1)
    mpz_t * integ = (mpz_t *)malloc(sizeof(mpz_t));
    if (!integ)
        return (void *)"mpz_init: error allocating memory";
    mpz_init(*integ);

    if (PARAMETERS_COUNT > 0) {
        SET_NUMBER(0, (SYS_INT)integ);
    }
    RETURN_NUMBER((SYS_INT)integ)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(mpz_clear, 1)
    T_HANDLE(mpz_clear, 0)

    mpz_t * integ = (mpz_t *)PARAM_INT(0);
    mpz_clear(*integ);
    free(integ);

    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(mpz_inits, 1)
    T_NUMBER(mpz_inits, 0)

    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);
    long len = PARAM_INT(0);
    for (long i = 0; i < len; i++) {
        mpz_t *integ = (mpz_t *)malloc(sizeof(mpz_t));
        if (integ) {
            mpz_init(*integ);
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)integ);
        } else {
            return (void *)"mpz_inits: error allocating memory";
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(mpz_clears, 1)
    T_ARRAY(mpz_clears, 0)

    void *arr = PARAMETER(0);
    long len = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    for (INTEGER i = 0; i < len; i++) {
        char    *str = 0;
        NUMBER  nDummy;
        INTEGER type;

        Invoke(INVOKE_GET_ARRAY_ELEMENT, arr, i, &type, &str, &nDummy);
        mpz_t *integ = (mpz_t *)(SYS_INT)nDummy;
        if ((type == VARIABLE_NUMBER) && (integ)) {
            mpz_clear(*integ);
            free(integ);
            Invoke(INVOKE_SET_ARRAY_ELEMENT, arr, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)0);
        }
    }
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
//------------------------------------------------------------------------
// void mpz_set(mpz_t rop, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_set, 2)
    T_HANDLE(mpz_set, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_set, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    mpz_set(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_set_ui(mpz_t rop, unsigned long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_set_ui, 2)
    T_HANDLE(mpz_set_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_set_ui, 1) // unsigned long int op
    unsigned long int op = (unsigned long int)PARAM(1);

    mpz_set_ui(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_set_si(mpz_t rop, signed long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_set_si, 2)
    T_HANDLE(mpz_set_si, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_set_si, 1) // signed long int op
    signed long int op = (signed long int)PARAM(1);

    mpz_set_si(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_set_d(mpz_t rop, double op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_set_d, 2)
    T_HANDLE(mpz_set_d, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_set_d, 1) // double op
    double op = (double)PARAM(1);

    mpz_set_d(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_set_q(mpz_t rop, mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_set_q, 2)
    T_HANDLE(mpz_set_q, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_set_q, 1) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(1);

    mpz_set_q(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_set_f(mpz_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_set_f, 2)
    T_HANDLE(mpz_set_f, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_set_f, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpz_set_f(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpz_set_str(mpz_t rop, char* str, int base)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_set_str, 3)
    T_HANDLE(mpz_set_str, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_STRING(mpz_set_str, 1) // char* str
    char *str = (char *)PARAM(1);

    T_NUMBER(mpz_set_str, 2) // int base
    int base = (int)PARAM(2);

    RETURN_NUMBER(mpz_set_str(*rop, str, base))
END_IMPL
//------------------------------------------------------------------------
// void mpz_swap(mpz_t rop1, mpz_t rop2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_swap, 2)
    T_HANDLE(mpz_swap, 0) // mpz_t rop1
    mpz_t * rop1 = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_swap, 1) // mpz_t rop2
    mpz_t * rop2 = (mpz_t *)PARAM_INT(1);

    mpz_swap(*rop1, *rop2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_init_set(mpz_t rop, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_init_set, 2)
    mpz_t * rop = (mpz_t *)malloc(sizeof(mpz_t));
    if (!rop)
        return (void *)"mpz_init_set: error allocating memory";

    T_HANDLE(mpz_init_set, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    mpz_init_set(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_init_set_ui(mpz_t rop, unsigned long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_init_set_ui, 2)
    mpz_t * rop = (mpz_t *)malloc(sizeof(mpz_t));
    if (!rop)
        return (void *)"mpz_init_set_ui: error allocating memory";

    T_NUMBER(mpz_init_set_ui, 1) // unsigned long int op
    unsigned long int op = (unsigned long int)PARAM(1);

    mpz_init_set_ui(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_init_set_si(mpz_t rop, signed long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_init_set_si, 2)
    mpz_t * rop = (mpz_t *)malloc(sizeof(mpz_t));
    if (!rop)
        return (void *)"mpz_init_set_si: error allocating memory";

    T_NUMBER(mpz_init_set_si, 1) // signed long int op
    signed long int op = (signed long int)PARAM(1);

    mpz_init_set_si(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_init_set_d(mpz_t rop, double op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_init_set_d, 2)
    mpz_t * rop = (mpz_t *)malloc(sizeof(mpz_t));
    if (!rop)
        return (void *)"mpz_init_set_d: error allocating memory";

    T_NUMBER(mpz_init_set_d, 1) // double op
    double op = (double)PARAM(1);

    mpz_init_set_d(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpz_init_set_str(mpz_t rop, char* str, int base)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_init_set_str, 3)
    mpz_t * rop = (mpz_t *)malloc(sizeof(mpz_t));
    if (!rop)
        return (void *)"mpz_init_set_str: error allocating memory";

    T_STRING(mpz_init_set_str, 1) // char* str
    char *str = (char *)PARAM(1);

    T_NUMBER(mpz_init_set_str, 2) // int base
    int base = (int)PARAM(2);

    RETURN_NUMBER(mpz_init_set_str(*rop, str, base))
END_IMPL
//------------------------------------------------------------------------
// void mpz_add(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_add, 3)
    T_HANDLE(mpz_add, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_add, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_add, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_add(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_add_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_add_ui, 3)
    T_HANDLE(mpz_add_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_add_ui, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_add_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpz_add_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_sub(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_sub, 3)
    T_HANDLE(mpz_sub, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_sub, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_sub, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_sub(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_sub_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_sub_ui, 3)
    T_HANDLE(mpz_sub_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_sub_ui, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_sub_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpz_sub_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_ui_sub(mpz_t rop, unsigned long int op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_ui_sub, 3)
    T_HANDLE(mpz_ui_sub, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_ui_sub, 1) // unsigned long int op1
    unsigned long int op1 = (unsigned long int)PARAM(1);

    T_HANDLE(mpz_ui_sub, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_ui_sub(*rop, op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_mul(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_mul, 3)
    T_HANDLE(mpz_mul, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_mul, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_mul, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_mul(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_mul_si(mpz_t rop, mpz_t op1, long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_mul_si, 3)
    T_HANDLE(mpz_mul_si, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_mul_si, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_mul_si, 2) // long int op2
    long int op2 = (long int)PARAM(2);

    mpz_mul_si(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_mul_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_mul_ui, 3)
    T_HANDLE(mpz_mul_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_mul_ui, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_mul_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpz_mul_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_addmul(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_addmul, 3)
    T_HANDLE(mpz_addmul, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_addmul, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_addmul, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_addmul(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_addmul_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_addmul_ui, 3)
    T_HANDLE(mpz_addmul_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_addmul_ui, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_addmul_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpz_addmul_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_submul(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_submul, 3)
    T_HANDLE(mpz_submul, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_submul, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_submul, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_submul(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_submul_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_submul_ui, 3)
    T_HANDLE(mpz_submul_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_submul_ui, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_submul_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpz_submul_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_mul_2exp(mpz_t rop, mpz_t op1, mp_bitcnt_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_mul_2exp, 3)
    T_HANDLE(mpz_mul_2exp, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_mul_2exp, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_mul_2exp, 2) // mp_bitcnt_t op2
    mp_bitcnt_t op2 = (mp_bitcnt_t)PARAM(2);

    mpz_mul_2exp(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_neg(mpz_t rop, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_neg, 2)
    T_HANDLE(mpz_neg, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_neg, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    mpz_neg(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_abs(mpz_t rop, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_abs, 2)
    T_HANDLE(mpz_abs, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_abs, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    mpz_abs(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_cdiv_q(mpz_t q, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cdiv_q, 3)
    T_HANDLE(mpz_cdiv_q, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cdiv_q, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_cdiv_q, 2) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(2);

    mpz_cdiv_q(*q, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_cdiv_r(mpz_t r, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cdiv_r, 3)
    T_HANDLE(mpz_cdiv_r, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cdiv_r, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_cdiv_r, 2) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(2);

    mpz_cdiv_r(*r, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_cdiv_qr(mpz_t q, mpz_t r, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cdiv_qr, 4)
    T_HANDLE(mpz_cdiv_qr, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cdiv_qr, 1) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_cdiv_qr, 2) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(2);

    T_HANDLE(mpz_cdiv_qr, 3) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(3);

    mpz_cdiv_qr(*q, *r, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_cdiv_q_ui(mpz_t q, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cdiv_q_ui, 3)
    T_HANDLE(mpz_cdiv_q_ui, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cdiv_q_ui, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_cdiv_q_ui, 2) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_cdiv_q_ui(*q, *n, d))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_cdiv_r_ui(mpz_t r, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cdiv_r_ui, 3)
    T_HANDLE(mpz_cdiv_r_ui, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cdiv_r_ui, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_cdiv_r_ui, 2) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_cdiv_r_ui(*r, *n, d))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_cdiv_qr_ui(mpz_t q, mpz_t r, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cdiv_qr_ui, 4)
    T_HANDLE(mpz_cdiv_qr_ui, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cdiv_qr_ui, 1) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_cdiv_qr_ui, 2) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(2);

    T_NUMBER(mpz_cdiv_qr_ui, 3) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(3);

    RETURN_NUMBER(mpz_cdiv_qr_ui(*q, *r, *n, d))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_cdiv_ui(mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cdiv_ui, 2)
    T_HANDLE(mpz_cdiv_ui, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_cdiv_ui, 1) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(1);

    RETURN_NUMBER(mpz_cdiv_ui(*n, d))
END_IMPL
//------------------------------------------------------------------------
// void mpz_cdiv_q_2exp(mpz_t q, mpz_t n, mp_bitcnt_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cdiv_q_2exp, 3)
    T_HANDLE(mpz_cdiv_q_2exp, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cdiv_q_2exp, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_cdiv_q_2exp, 2) // mp_bitcnt_t b
    mp_bitcnt_t b = (mp_bitcnt_t)PARAM(2);

    mpz_cdiv_q_2exp(*q, *n, b);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_cdiv_r_2exp(mpz_t r, mpz_t n, mp_bitcnt_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cdiv_r_2exp, 3)
    T_HANDLE(mpz_cdiv_r_2exp, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cdiv_r_2exp, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_cdiv_r_2exp, 2) // mp_bitcnt_t b
    mp_bitcnt_t b = (mp_bitcnt_t)PARAM(2);

    mpz_cdiv_r_2exp(*r, *n, b);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_fdiv_q(mpz_t q, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fdiv_q, 3)
    T_HANDLE(mpz_fdiv_q, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_fdiv_q, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_fdiv_q, 2) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(2);

    mpz_fdiv_q(*q, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_fdiv_r(mpz_t r, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fdiv_r, 3)
    T_HANDLE(mpz_fdiv_r, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_fdiv_r, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_fdiv_r, 2) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(2);

    mpz_fdiv_r(*r, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_fdiv_qr(mpz_t q, mpz_t r, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fdiv_qr, 4)
    T_HANDLE(mpz_fdiv_qr, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_fdiv_qr, 1) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_fdiv_qr, 2) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(2);

    T_HANDLE(mpz_fdiv_qr, 3) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(3);

    mpz_fdiv_qr(*q, *r, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_fdiv_q_ui(mpz_t q, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fdiv_q_ui, 3)
    T_HANDLE(mpz_fdiv_q_ui, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_fdiv_q_ui, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_fdiv_q_ui, 2) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_fdiv_q_ui(*q, *n, d))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_fdiv_r_ui(mpz_t r, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fdiv_r_ui, 3)
    T_HANDLE(mpz_fdiv_r_ui, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_fdiv_r_ui, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_fdiv_r_ui, 2) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_fdiv_r_ui(*r, *n, d))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_fdiv_qr_ui(mpz_t q, mpz_t r, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fdiv_qr_ui, 4)
    T_HANDLE(mpz_fdiv_qr_ui, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_fdiv_qr_ui, 1) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_fdiv_qr_ui, 2) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(2);

    T_NUMBER(mpz_fdiv_qr_ui, 3) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(3);

    RETURN_NUMBER(mpz_fdiv_qr_ui(*q, *r, *n, d))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_fdiv_ui(mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fdiv_ui, 2)
    T_HANDLE(mpz_fdiv_ui, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_fdiv_ui, 1) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(1);

    RETURN_NUMBER(mpz_fdiv_ui(*n, d))
END_IMPL
//------------------------------------------------------------------------
// void mpz_fdiv_q_2exp(mpz_t q, mpz_t n, mp_bitcnt_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fdiv_q_2exp, 3)
    T_HANDLE(mpz_fdiv_q_2exp, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_fdiv_q_2exp, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_fdiv_q_2exp, 2) // mp_bitcnt_t b
    mp_bitcnt_t b = (mp_bitcnt_t)PARAM(2);

    mpz_fdiv_q_2exp(*q, *n, b);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_fdiv_r_2exp(mpz_t r, mpz_t n, mp_bitcnt_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fdiv_r_2exp, 3)
    T_HANDLE(mpz_fdiv_r_2exp, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_fdiv_r_2exp, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_fdiv_r_2exp, 2) // mp_bitcnt_t b
    mp_bitcnt_t b = (mp_bitcnt_t)PARAM(2);

    mpz_fdiv_r_2exp(*r, *n, b);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_tdiv_q(mpz_t q, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tdiv_q, 3)
    T_HANDLE(mpz_tdiv_q, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_tdiv_q, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_tdiv_q, 2) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(2);

    mpz_tdiv_q(*q, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_tdiv_r(mpz_t r, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tdiv_r, 3)
    T_HANDLE(mpz_tdiv_r, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_tdiv_r, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_tdiv_r, 2) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(2);

    mpz_tdiv_r(*r, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_tdiv_qr(mpz_t q, mpz_t r, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tdiv_qr, 4)
    T_HANDLE(mpz_tdiv_qr, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_tdiv_qr, 1) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_tdiv_qr, 2) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(2);

    T_HANDLE(mpz_tdiv_qr, 3) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(3);

    mpz_tdiv_qr(*q, *r, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_tdiv_q_ui(mpz_t q, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tdiv_q_ui, 3)
    T_HANDLE(mpz_tdiv_q_ui, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_tdiv_q_ui, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_tdiv_q_ui, 2) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_tdiv_q_ui(*q, *n, d))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_tdiv_r_ui(mpz_t r, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tdiv_r_ui, 3)
    T_HANDLE(mpz_tdiv_r_ui, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_tdiv_r_ui, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_tdiv_r_ui, 2) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_tdiv_r_ui(*r, *n, d))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_tdiv_qr_ui(mpz_t q, mpz_t r, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tdiv_qr_ui, 4)
    T_HANDLE(mpz_tdiv_qr_ui, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_tdiv_qr_ui, 1) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_tdiv_qr_ui, 2) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(2);

    T_NUMBER(mpz_tdiv_qr_ui, 3) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(3);

    RETURN_NUMBER(mpz_tdiv_qr_ui(*q, *r, *n, d))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_tdiv_ui(mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tdiv_ui, 2)
    T_HANDLE(mpz_tdiv_ui, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_tdiv_ui, 1) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(1);

    RETURN_NUMBER(mpz_tdiv_ui(*n, d))
END_IMPL
//------------------------------------------------------------------------
// void mpz_tdiv_q_2exp(mpz_t q, mpz_t n, mp_bitcnt_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tdiv_q_2exp, 3)
    T_HANDLE(mpz_tdiv_q_2exp, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_tdiv_q_2exp, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_tdiv_q_2exp, 2) // mp_bitcnt_t b
    mp_bitcnt_t b = (mp_bitcnt_t)PARAM(2);

    mpz_tdiv_q_2exp(*q, *n, b);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_tdiv_r_2exp(mpz_t r, mpz_t n, mp_bitcnt_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tdiv_r_2exp, 3)
    T_HANDLE(mpz_tdiv_r_2exp, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_tdiv_r_2exp, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_tdiv_r_2exp, 2) // mp_bitcnt_t b
    mp_bitcnt_t b = (mp_bitcnt_t)PARAM(2);

    mpz_tdiv_r_2exp(*r, *n, b);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_mod(mpz_t r, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_mod, 3)
    T_HANDLE(mpz_mod, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_mod, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_mod, 2) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(2);

    mpz_mod(*r, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_mod_ui(mpz_t r, mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_mod_ui, 3)
    T_HANDLE(mpz_mod_ui, 0) // mpz_t r
    mpz_t * r = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_mod_ui, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_mod_ui, 2) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_mod_ui(*r, *n, d))
END_IMPL
//------------------------------------------------------------------------
// void mpz_divexact(mpz_t q, mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_divexact, 3)
    T_HANDLE(mpz_divexact, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_divexact, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_divexact, 2) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(2);

    mpz_divexact(*q, *n, *d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_divexact_ui(mpz_t q, mpz_t n, unsigned long d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_divexact_ui, 3)
    T_HANDLE(mpz_divexact_ui, 0) // mpz_t q
    mpz_t * q = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_divexact_ui, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_divexact_ui, 2) // unsigned long d
    unsigned long d = (unsigned long)PARAM(2);

    mpz_divexact_ui(*q, *n, d);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpz_divisible_p(mpz_t n, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_divisible_p, 2)
    T_HANDLE(mpz_divisible_p, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_divisible_p, 1) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_divisible_p(*n, *d))
END_IMPL
//------------------------------------------------------------------------
// int mpz_divisible_ui_p(mpz_t n, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_divisible_ui_p, 2)
    T_HANDLE(mpz_divisible_ui_p, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_divisible_ui_p, 1) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(1);

    RETURN_NUMBER(mpz_divisible_ui_p(*n, d))
END_IMPL
//------------------------------------------------------------------------
// int mpz_divisible_2exp_p(mpz_t n, mp_bitcnt_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_divisible_2exp_p, 2)
    T_HANDLE(mpz_divisible_2exp_p, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_divisible_2exp_p, 1) // mp_bitcnt_t b
    mp_bitcnt_t b = (mp_bitcnt_t)PARAM(1);

    RETURN_NUMBER(mpz_divisible_2exp_p(*n, b))
END_IMPL
//------------------------------------------------------------------------
// int mpz_congruent_p(mpz_t n, mpz_t c, mpz_t d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_congruent_p, 3)
    T_HANDLE(mpz_congruent_p, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_congruent_p, 1) // mpz_t c
    mpz_t * c = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_congruent_p, 2) // mpz_t d
    mpz_t * d = (mpz_t *)PARAM_INT(2);

    RETURN_NUMBER(mpz_congruent_p(*n, *c, *d))
END_IMPL
//------------------------------------------------------------------------
// int mpz_congruent_ui_p(mpz_t n, unsigned long int c, unsigned long int d)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_congruent_ui_p, 3)
    T_HANDLE(mpz_congruent_ui_p, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_congruent_ui_p, 1) // unsigned long int c
    unsigned long int c = (unsigned long int)PARAM(1);

    T_NUMBER(mpz_congruent_ui_p, 2) // unsigned long int d
    unsigned long int d = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_congruent_ui_p(*n, c, d))
END_IMPL
//------------------------------------------------------------------------
// int mpz_congruent_2exp_p(mpz_t n, mpz_t c, mp_bitcnt_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_congruent_2exp_p, 3)
    T_HANDLE(mpz_congruent_2exp_p, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_congruent_2exp_p, 1) // mpz_t c
    mpz_t * c = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_congruent_2exp_p, 2) // mp_bitcnt_t b
    mp_bitcnt_t b = (mp_bitcnt_t)PARAM(2);

    RETURN_NUMBER(mpz_congruent_2exp_p(*n, *c, b))
END_IMPL
//------------------------------------------------------------------------
// void mpz_powm(mpz_t rop, mpz_t base, mpz_t exp, mpz_t mod)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_powm, 4)
    T_HANDLE(mpz_powm, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_powm, 1) // mpz_t base
    mpz_t * base = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_powm, 2) // mpz_t exp
    mpz_t * exp = (mpz_t *)PARAM_INT(2);

    T_HANDLE(mpz_powm, 3) // mpz_t mod
    mpz_t * mod = (mpz_t *)PARAM_INT(3);

    mpz_powm(*rop, *base, *exp, *mod);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_powm_ui(mpz_t rop, mpz_t base, unsigned long int exp, mpz_t mod)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_powm_ui, 4)
    T_HANDLE(mpz_powm_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_powm_ui, 1) // mpz_t base
    mpz_t * base = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_powm_ui, 2) // unsigned long int exp
    unsigned long int exp = (unsigned long int)PARAM(2);

    T_HANDLE(mpz_powm_ui, 3) // mpz_t mod
    mpz_t * mod = (mpz_t *)PARAM_INT(3);

    mpz_powm_ui(*rop, *base, exp, *mod);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_powm_sec(mpz_t rop, mpz_t base, mpz_t exp, mpz_t mod)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_powm_sec, 4)
    T_HANDLE(mpz_powm_sec, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_powm_sec, 1) // mpz_t base
    mpz_t * base = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_powm_sec, 2) // mpz_t exp
    mpz_t * exp = (mpz_t *)PARAM_INT(2);

    T_HANDLE(mpz_powm_sec, 3) // mpz_t mod
    mpz_t * mod = (mpz_t *)PARAM_INT(3);

    mpz_powm_sec(*rop, *base, *exp, *mod);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_pow_ui(mpz_t rop, mpz_t base, unsigned long int exp)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_pow_ui, 3)
    T_HANDLE(mpz_pow_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_pow_ui, 1) // mpz_t base
    mpz_t * base = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_pow_ui, 2) // unsigned long int exp
    unsigned long int exp = (unsigned long int)PARAM(2);

    mpz_pow_ui(*rop, *base, exp);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_ui_pow_ui(mpz_t rop, unsigned long int base, unsigned long int exp)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_ui_pow_ui, 3)
    T_HANDLE(mpz_ui_pow_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_ui_pow_ui, 1) // unsigned long int base
    unsigned long int base = (unsigned long int)PARAM(1);

    T_NUMBER(mpz_ui_pow_ui, 2) // unsigned long int exp
    unsigned long int exp = (unsigned long int)PARAM(2);

    mpz_ui_pow_ui(*rop, base, exp);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpz_root(mpz_t rop, mpz_t op, unsigned long int n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_root, 3)
    T_HANDLE(mpz_root, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_root, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_root, 2) // unsigned long int n
    unsigned long int n = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_root(*rop, *op, n))
END_IMPL
//------------------------------------------------------------------------
// void mpz_rootrem(mpz_t root, mpz_t rem, mpz_t u, unsigned long int n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_rootrem, 4)
    T_HANDLE(mpz_rootrem, 0) // mpz_t root
    mpz_t * root = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_rootrem, 1) // mpz_t rem
    mpz_t * rem = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_rootrem, 2) // mpz_t u
    mpz_t * u = (mpz_t *)PARAM_INT(2);

    T_NUMBER(mpz_rootrem, 3) // unsigned long int n
    unsigned long int n = (unsigned long int)PARAM(3);

    mpz_rootrem(*root, *rem, *u, n);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_sqrt(mpz_t rop, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_sqrt, 2)
    T_HANDLE(mpz_sqrt, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_sqrt, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    mpz_sqrt(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_sqrtrem(mpz_t rop1, mpz_t rop2, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_sqrtrem, 3)
    T_HANDLE(mpz_sqrtrem, 0) // mpz_t rop1
    mpz_t * rop1 = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_sqrtrem, 1) // mpz_t rop2
    mpz_t * rop2 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_sqrtrem, 2) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(2);

    mpz_sqrtrem(*rop1, *rop2, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpz_perfect_power_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_perfect_power_p, 1)
    T_HANDLE(mpz_perfect_power_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_perfect_power_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpz_perfect_square_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_perfect_square_p, 1)
    T_HANDLE(mpz_perfect_square_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_perfect_square_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpz_probab_prime_p(mpz_t n, int reps)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_probab_prime_p, 2)
    T_HANDLE(mpz_probab_prime_p, 0) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_probab_prime_p, 1) // int reps
    int reps = (int)PARAM(1);

    RETURN_NUMBER(mpz_probab_prime_p(*n, reps))
END_IMPL
//------------------------------------------------------------------------
// void mpz_nextprime(mpz_t rop, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_nextprime, 2)
    T_HANDLE(mpz_nextprime, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_nextprime, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    mpz_nextprime(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_gcd(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_gcd, 3)
    T_HANDLE(mpz_gcd, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_gcd, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_gcd, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_gcd(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_gcd_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_gcd_ui, 3)
    T_HANDLE(mpz_gcd_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_gcd_ui, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_gcd_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpz_gcd_ui(*rop, *op1, op2))
END_IMPL
//------------------------------------------------------------------------
// void mpz_gcdext(mpz_t g, mpz_t s, mpz_t t, mpz_t a, mpz_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_gcdext, 5)
    T_HANDLE(mpz_gcdext, 0) // mpz_t g
    mpz_t * g = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_gcdext, 1) // mpz_t s
    mpz_t * s = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_gcdext, 2) // mpz_t t
    mpz_t * t = (mpz_t *)PARAM_INT(2);

    T_HANDLE(mpz_gcdext, 3) // mpz_t a
    mpz_t * a = (mpz_t *)PARAM_INT(3);

    T_HANDLE(mpz_gcdext, 4) // mpz_t b
    mpz_t * b = (mpz_t *)PARAM_INT(4);

    mpz_gcdext(*g, *s, *t, *a, *b);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_lcm(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_lcm, 3)
    T_HANDLE(mpz_lcm, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_lcm, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_lcm, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_lcm(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_lcm_ui(mpz_t rop, mpz_t op1, unsigned long op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_lcm_ui, 3)
    T_HANDLE(mpz_lcm_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_lcm_ui, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_lcm_ui, 2) // unsigned long op2
    unsigned long op2 = (unsigned long)PARAM(2);

    mpz_lcm_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpz_invert(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_invert, 3)
    T_HANDLE(mpz_invert, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_invert, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_invert, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    RETURN_NUMBER(mpz_invert(*rop, *op1, *op2))
END_IMPL
//------------------------------------------------------------------------
// int mpz_jacobi(mpz_t a, mpz_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_jacobi, 2)
    T_HANDLE(mpz_jacobi, 0) // mpz_t a
    mpz_t * a = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_jacobi, 1) // mpz_t b
    mpz_t * b = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_jacobi(*a, *b))
END_IMPL
//------------------------------------------------------------------------
// int mpz_legendre(mpz_t a, mpz_t p)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_legendre, 2)
    T_HANDLE(mpz_legendre, 0) // mpz_t a
    mpz_t * a = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_legendre, 1) // mpz_t p
    mpz_t * p = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_legendre(*a, *p))
END_IMPL
//------------------------------------------------------------------------
// int mpz_kronecker(mpz_t a, mpz_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_kronecker, 2)
    T_HANDLE(mpz_kronecker, 0) // mpz_t a
    mpz_t * a = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_kronecker, 1) // mpz_t b
    mpz_t * b = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_kronecker(*a, *b))
END_IMPL
//------------------------------------------------------------------------
// int mpz_kronecker_si(mpz_t a, long b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_kronecker_si, 2)
    T_HANDLE(mpz_kronecker_si, 0) // mpz_t a
    mpz_t * a = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_kronecker_si, 1) // long b
    long b = (long)PARAM(1);

    RETURN_NUMBER(mpz_kronecker_si(*a, b))
END_IMPL
//------------------------------------------------------------------------
// int mpz_kronecker_ui(mpz_t a, unsigned long b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_kronecker_ui, 2)
    T_HANDLE(mpz_kronecker_ui, 0) // mpz_t a
    mpz_t * a = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_kronecker_ui, 1) // unsigned long b
    unsigned long b = (unsigned long)PARAM(1);

    RETURN_NUMBER(mpz_kronecker_ui(*a, b))
END_IMPL
//------------------------------------------------------------------------
// int mpz_si_kronecker(long a, mpz_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_si_kronecker, 2)
    T_NUMBER(mpz_si_kronecker, 0) // long a
    long a = (long)PARAM(0);

    T_HANDLE(mpz_si_kronecker, 1) // mpz_t b
    mpz_t * b = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_si_kronecker(a, *b))
END_IMPL
//------------------------------------------------------------------------
// int mpz_ui_kronecker(unsigned long a, mpz_t b)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_ui_kronecker, 2)
    T_NUMBER(mpz_ui_kronecker, 0) // unsigned long a
    unsigned long a = (unsigned long)PARAM(0);

    T_HANDLE(mpz_ui_kronecker, 1) // mpz_t b
    mpz_t * b = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_ui_kronecker(a, *b))
END_IMPL
//------------------------------------------------------------------------
// mp_bitcnt_t mpz_remove(mpz_t rop, mpz_t op, mpz_t f)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_remove, 3)
    T_HANDLE(mpz_remove, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_remove, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_remove, 2) // mpz_t f
    mpz_t * f = (mpz_t *)PARAM_INT(2);

    RETURN_NUMBER(mpz_remove(*rop, *op, *f))
END_IMPL
//------------------------------------------------------------------------
// void mpz_fac_ui(mpz_t rop, unsigned long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fac_ui, 2)
    T_HANDLE(mpz_fac_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_fac_ui, 1) // unsigned long int op
    unsigned long int op = (unsigned long int)PARAM(1);

    mpz_fac_ui(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_bin_ui(mpz_t rop, mpz_t n, unsigned long int k)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_bin_ui, 3)
    T_HANDLE(mpz_bin_ui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_bin_ui, 1) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_bin_ui, 2) // unsigned long int k
    unsigned long int k = (unsigned long int)PARAM(2);

    mpz_bin_ui(*rop, *n, k);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_bin_uiui(mpz_t rop, unsigned long int n, unsigned long int k)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_bin_uiui, 3)
    T_HANDLE(mpz_bin_uiui, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_bin_uiui, 1) // unsigned long int n
    unsigned long int n = (unsigned long int)PARAM(1);

    T_NUMBER(mpz_bin_uiui, 2) // unsigned long int k
    unsigned long int k = (unsigned long int)PARAM(2);

    mpz_bin_uiui(*rop, n, k);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_fib_ui(mpz_t fn, unsigned long int n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fib_ui, 2)
    T_HANDLE(mpz_fib_ui, 0) // mpz_t fn
    mpz_t * fn = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_fib_ui, 1) // unsigned long int n
    unsigned long int n = (unsigned long int)PARAM(1);

    mpz_fib_ui(*fn, n);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_fib2_ui(mpz_t fn, mpz_t fnsub1, unsigned long int n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fib2_ui, 3)
    T_HANDLE(mpz_fib2_ui, 0) // mpz_t fn
    mpz_t * fn = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_fib2_ui, 1) // mpz_t fnsub1
    mpz_t * fnsub1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_fib2_ui, 2) // unsigned long int n
    unsigned long int n = (unsigned long int)PARAM(2);

    mpz_fib2_ui(*fn, *fnsub1, n);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_lucnum_ui(mpz_t ln, unsigned long int n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_lucnum_ui, 2)
    T_HANDLE(mpz_lucnum_ui, 0) // mpz_t ln
    mpz_t * ln = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_lucnum_ui, 1) // unsigned long int n
    unsigned long int n = (unsigned long int)PARAM(1);

    mpz_lucnum_ui(*ln, n);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_lucnum2_ui(mpz_t ln, mpz_t lnsub1, unsigned long int n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_lucnum2_ui, 3)
    T_HANDLE(mpz_lucnum2_ui, 0) // mpz_t ln
    mpz_t * ln = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_lucnum2_ui, 1) // mpz_t lnsub1
    mpz_t * lnsub1 = (mpz_t *)PARAM_INT(1);

    T_NUMBER(mpz_lucnum2_ui, 2) // unsigned long int n
    unsigned long int n = (unsigned long int)PARAM(2);

    mpz_lucnum2_ui(*ln, *lnsub1, n);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpz_cmp(mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cmp, 2)
    T_HANDLE(mpz_cmp, 0) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cmp, 1) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_cmp(*op1, *op2))
END_IMPL
//------------------------------------------------------------------------
// int mpz_cmp_d(mpz_t op1, double op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cmp_d, 2)
    T_HANDLE(mpz_cmp_d, 0) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_cmp_d, 1) // double op2
    double op2 = (double)PARAM(1);

    RETURN_NUMBER(mpz_cmp_d(*op1, op2))
END_IMPL
//------------------------------------------------------------------------
// int mpz_cmp_si(mpz_t op1, signed long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cmp_si, 2)
    T_HANDLE(mpz_cmp_si, 0) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_cmp_si, 1) // signed long int op2
    signed long int op2 = (signed long int)PARAM(1);

    RETURN_NUMBER(mpz_cmp_si(*op1, op2))
END_IMPL
//------------------------------------------------------------------------
// int mpz_cmp_ui(mpz_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cmp_ui, 2)
    T_HANDLE(mpz_cmp_ui, 0) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_cmp_ui, 1) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(1);

    RETURN_NUMBER(mpz_cmp_ui(*op1, op2))
END_IMPL
//------------------------------------------------------------------------
// int mpz_cmpabs(mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cmpabs, 2)
    T_HANDLE(mpz_cmpabs, 0) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_cmpabs, 1) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_cmpabs(*op1, *op2))
END_IMPL
//------------------------------------------------------------------------
// int mpz_cmpabs_d(mpz_t op1, double op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cmpabs_d, 2)
    T_HANDLE(mpz_cmpabs_d, 0) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_cmpabs_d, 1) // double op2
    double op2 = (double)PARAM(1);

    RETURN_NUMBER(mpz_cmpabs_d(*op1, op2))
END_IMPL
//------------------------------------------------------------------------
// int mpz_cmpabs_ui(mpz_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_cmpabs_ui, 2)
    T_HANDLE(mpz_cmpabs_ui, 0) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_cmpabs_ui, 1) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(1);

    RETURN_NUMBER(mpz_cmpabs_ui(*op1, op2))
END_IMPL
//------------------------------------------------------------------------
// int mpz_sgn(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_sgn, 1)
    T_HANDLE(mpz_sgn, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_sgn(*op))
END_IMPL
//------------------------------------------------------------------------
// void mpz_and(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_and, 3)
    T_HANDLE(mpz_and, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_and, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_and, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_and(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_ior(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_ior, 3)
    T_HANDLE(mpz_ior, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_ior, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_ior, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_ior(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_xor(mpz_t rop, mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_xor, 3)
    T_HANDLE(mpz_xor, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_xor, 1) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(1);

    T_HANDLE(mpz_xor, 2) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(2);

    mpz_xor(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_com(mpz_t rop, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_com, 2)
    T_HANDLE(mpz_com, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_com, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    mpz_com(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// mp_bitcnt_t mpz_popcount(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_popcount, 1)
    T_HANDLE(mpz_popcount, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_popcount(*op))
END_IMPL
//------------------------------------------------------------------------
// mp_bitcnt_t mpz_hamdist(mpz_t op1, mpz_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_hamdist, 2)
    T_HANDLE(mpz_hamdist, 0) // mpz_t op1
    mpz_t * op1 = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_hamdist, 1) // mpz_t op2
    mpz_t * op2 = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_hamdist(*op1, *op2))
END_IMPL
//------------------------------------------------------------------------
// mp_bitcnt_t mpz_scan0(mpz_t op, mp_bitcnt_t starting_bit)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_scan0, 2)
    T_HANDLE(mpz_scan0, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_scan0, 1) // mp_bitcnt_t starting_bit
    mp_bitcnt_t starting_bit = (mp_bitcnt_t)PARAM(1);

    RETURN_NUMBER(mpz_scan0(*op, starting_bit))
END_IMPL
//------------------------------------------------------------------------
// mp_bitcnt_t mpz_scan1(mpz_t op, mp_bitcnt_t starting_bit)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_scan1, 2)
    T_HANDLE(mpz_scan1, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_scan1, 1) // mp_bitcnt_t starting_bit
    mp_bitcnt_t starting_bit = (mp_bitcnt_t)PARAM(1);

    RETURN_NUMBER(mpz_scan1(*op, starting_bit))
END_IMPL
//------------------------------------------------------------------------
// void mpz_setbit(mpz_t rop, mp_bitcnt_t bit_index)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_setbit, 2)
    T_HANDLE(mpz_setbit, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_setbit, 1) // mp_bitcnt_t bit_index
    mp_bitcnt_t bit_index = (mp_bitcnt_t)PARAM(1);

    mpz_setbit(*rop, bit_index);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_clrbit(mpz_t rop, mp_bitcnt_t bit_index)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_clrbit, 2)
    T_HANDLE(mpz_clrbit, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_clrbit, 1) // mp_bitcnt_t bit_index
    mp_bitcnt_t bit_index = (mp_bitcnt_t)PARAM(1);

    mpz_clrbit(*rop, bit_index);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_combit(mpz_t rop, mp_bitcnt_t bit_index)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_combit, 2)
    T_HANDLE(mpz_combit, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_combit, 1) // mp_bitcnt_t bit_index
    mp_bitcnt_t bit_index = (mp_bitcnt_t)PARAM(1);

    mpz_combit(*rop, bit_index);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpz_tstbit(mpz_t op, mp_bitcnt_t bit_index)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_tstbit, 2)
    T_HANDLE(mpz_tstbit, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_tstbit, 1) // mp_bitcnt_t bit_index
    mp_bitcnt_t bit_index = (mp_bitcnt_t)PARAM(1);

    RETURN_NUMBER(mpz_tstbit(*op, bit_index))
END_IMPL
//------------------------------------------------------------------------
// size_t mpz_out_str(FILE* stream, int base, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_out_str, 3)
    T_HANDLE(mpz_out_str, 0) // FILE* stream
    FILE * stream = (FILE *)PARAM_INT(0);

    T_NUMBER(mpz_out_str, 1) // int base
    int base = (int)PARAM(1);

    T_HANDLE(mpz_out_str, 2) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(2);

    RETURN_NUMBER(mpz_out_str(stream, base, *op))
END_IMPL
//------------------------------------------------------------------------
// size_t mpz_inp_str(mpz_t rop, FILE* stream, int base)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_inp_str, 3)
    T_HANDLE(mpz_inp_str, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_inp_str, 1) // FILE* stream
    FILE * stream = (FILE *)PARAM_INT(1);

    T_NUMBER(mpz_inp_str, 2) // int base
    int base = (int)PARAM(2);

    RETURN_NUMBER(mpz_inp_str(*rop, stream, base))
END_IMPL
//------------------------------------------------------------------------
// size_t mpz_out_raw(FILE* stream, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_out_raw, 2)
    T_HANDLE(mpz_out_raw, 0) // FILE* stream
    FILE * stream = (FILE *)PARAM_INT(0);

    T_HANDLE(mpz_out_raw, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_out_raw(stream, *op))
END_IMPL
//------------------------------------------------------------------------
// size_t mpz_inp_raw(mpz_t rop, FILE* stream)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_inp_raw, 2)
    T_HANDLE(mpz_inp_raw, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_inp_raw, 1) // FILE* stream
    FILE * stream = (FILE *)PARAM_INT(1);

    RETURN_NUMBER(mpz_inp_raw(*rop, stream))
END_IMPL
//------------------------------------------------------------------------
// void mpz_urandomb(mpz_t rop, gmp_randstate_t state, mp_bitcnt_t n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_urandomb, 3)
    T_HANDLE(mpz_urandomb, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_urandomb, 1) // gmp_randstate_t state
    gmp_randstate_t * state = (gmp_randstate_t *)PARAM_INT(1);

    T_NUMBER(mpz_urandomb, 2) // mp_bitcnt_t n
    mp_bitcnt_t n = (mp_bitcnt_t)PARAM(2);

    mpz_urandomb(*rop, *state, n);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_urandomm(mpz_t rop, gmp_randstate_t state, mpz_t n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_urandomm, 3)
    T_HANDLE(mpz_urandomm, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_urandomm, 1) // gmp_randstate_t state
    gmp_randstate_t * state = (gmp_randstate_t *)PARAM_INT(1);

    T_HANDLE(mpz_urandomm, 2) // mpz_t n
    mpz_t * n = (mpz_t *)PARAM_INT(2);

    mpz_urandomm(*rop, *state, *n);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_rrandomb(mpz_t rop, gmp_randstate_t state, mp_bitcnt_t n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_rrandomb, 3)
    T_HANDLE(mpz_rrandomb, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpz_rrandomb, 1) // gmp_randstate_t state
    gmp_randstate_t * state = (gmp_randstate_t *)PARAM_INT(1);

    T_NUMBER(mpz_rrandomb, 2) // mp_bitcnt_t n
    mp_bitcnt_t n = (mp_bitcnt_t)PARAM(2);

    mpz_rrandomb(*rop, *state, n);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_random(mpz_t rop, mp_size_t max_size)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_random, 2)
    T_HANDLE(mpz_random, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_random, 1) // mp_size_t max_size
    mp_size_t max_size = (mp_size_t)PARAM(1);

    mpz_random(*rop, max_size);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_random2(mpz_t rop, mp_size_t max_size)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_random2, 2)
    T_HANDLE(mpz_random2, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_random2, 1) // mp_size_t max_size
    mp_size_t max_size = (mp_size_t)PARAM(1);

    mpz_random2(*rop, max_size);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpz_import(mpz_t rop, size_t count, int order, size_t size, int endian, size_t nails, const void* op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_import, 7)
    T_HANDLE(mpz_import, 0) // mpz_t rop
    mpz_t * rop = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_import, 1) // size_t count
    size_t count = (size_t)PARAM(1);

    T_NUMBER(mpz_import, 2) // int order
    int order = (int)PARAM(2);

    T_NUMBER(mpz_import, 3) // size_t size
    size_t size = (size_t)PARAM(3);

    T_NUMBER(mpz_import, 4) // int endian
    int endian = (int)PARAM(4);

    T_NUMBER(mpz_import, 5) // size_t nails
    size_t nails = (size_t)PARAM(5);

    T_STRING(mpz_import, 6) // const void* op
    const void *op = (const void *)PARAM(6);
    if (PARAM_LEN(6) < count * size)
        return (void *)"mpz_import: string is too short";

    mpz_import(*rop, count, order, size, endian, nails, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void* mpz_export(void* rop, size_t* countp, int order, size_t size, int endian, size_t nails, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_export, 5)
    T_NUMBER(mpz_export, 0) // int order
    int order = (int)PARAM(0);

    T_NUMBER(mpz_export, 1) // size_t size
    size_t size = (size_t)PARAM(1);

    T_NUMBER(mpz_export, 2) // int endian
    int endian = (int)PARAM(2);

    T_NUMBER(mpz_export, 3) // size_t nails
    size_t nails = (size_t)PARAM(3);

    T_HANDLE(mpz_export, 4) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(4);

    size_t count;
    mpz_export(NULL, &count, order, size, endian, nails, *op);
    char *p = (char *)malloc(count + 1);
    p[count] = 0;

    mpz_export(p, &size, order, size, endian, nails, *op);

    RETURN_BUFFER(p, size)
    delete[] p;
END_IMPL
//------------------------------------------------------------------------
// int mpz_fits_ulong_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fits_ulong_p, 1)
    T_HANDLE(mpz_fits_ulong_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_fits_ulong_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpz_fits_slong_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fits_slong_p, 1)
    T_HANDLE(mpz_fits_slong_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_fits_slong_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpz_fits_uint_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fits_uint_p, 1)
    T_HANDLE(mpz_fits_uint_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_fits_uint_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpz_fits_sint_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fits_sint_p, 1)
    T_HANDLE(mpz_fits_sint_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_fits_sint_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpz_fits_ushort_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fits_ushort_p, 1)
    T_HANDLE(mpz_fits_ushort_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_fits_ushort_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpz_fits_sshort_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_fits_sshort_p, 1)
    T_HANDLE(mpz_fits_sshort_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_fits_sshort_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpz_odd_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_odd_p, 1)
    T_HANDLE(mpz_odd_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_odd_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpz_even_p(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_even_p, 1)
    T_HANDLE(mpz_even_p, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_even_p(*op))
END_IMPL
//------------------------------------------------------------------------
// size_t mpz_sizeinbase(mpz_t op, int base)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_sizeinbase, 2)
    T_HANDLE(mpz_sizeinbase, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_sizeinbase, 1) // int base
    int base = (int)PARAM(1);

    RETURN_NUMBER(mpz_sizeinbase(*op, base))
END_IMPL
//------------------------------------------------------------------------
// mp_limb_t mpz_getlimbn(mpz_t op, mp_size_t n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_getlimbn, 2)
    T_HANDLE(mpz_getlimbn, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    T_NUMBER(mpz_getlimbn, 1) // mp_size_t n
    mp_size_t n = (mp_size_t)PARAM(1);

    RETURN_NUMBER(mpz_getlimbn(*op, n))
END_IMPL
//------------------------------------------------------------------------
// size_t mpz_size(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_size, 1)
    T_HANDLE(mpz_size, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_size(*op))
END_IMPL
//------------------------------------------------------------------------
// void gmp_randinit_default(gmp_randstate_t state)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(gmp_randinit_default, 0, 1)
    gmp_randstate_t * integ = (gmp_randstate_t *)malloc(sizeof(gmp_randstate_t));
    if (!integ)
        return (void *)"gmp_randinit_default: error allocating memory";

    gmp_randinit_default(*integ);
    if (PARAMETERS_COUNT > 0) {
        SET_NUMBER(0, (SYS_INT)integ);
    }
    RETURN_NUMBER((SYS_INT)integ)
END_IMPL
//------------------------------------------------------------------------
// void gmp_randinit_mt(gmp_randstate_t state)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(gmp_randinit_mt, 0, 1)
    gmp_randstate_t * integ = (gmp_randstate_t *)malloc(sizeof(gmp_randstate_t));
    if (!integ)
        return (void *)"gmp_randinit_mt: error allocating memory";

    gmp_randinit_mt(*integ);
    if (PARAMETERS_COUNT > 0) {
        SET_NUMBER(0, (SYS_INT)integ);
    }
    RETURN_NUMBER((SYS_INT)integ)
END_IMPL
//------------------------------------------------------------------------
// void gmp_randinit_lc_2exp(gmp_randstate_t state, mpz_t a, unsigned long c, mp_bitcnt_t m2exp)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gmp_randinit_lc_2exp, 4)
    T_HANDLE(gmp_randinit_lc_2exp, 1) // mpz_t a
    mpz_t * a = (mpz_t *)PARAM_INT(1);

    T_NUMBER(gmp_randinit_lc_2exp, 2) // unsigned long c
    unsigned long c = (unsigned long)PARAM(2);

    T_NUMBER(gmp_randinit_lc_2exp, 3) // mp_bitcnt_t m2exp
    mp_bitcnt_t m2exp = (mp_bitcnt_t)PARAM(3);

    gmp_randstate_t *state = (gmp_randstate_t *)malloc(sizeof(gmp_randstate_t));
    if (!state)
        return (void *)"gmp_randinit_lc_2exp: error allocating memory";

    gmp_randinit_lc_2exp(*state, *a, c, m2exp);

    SET_NUMBER(0, (SYS_INT)state);
    RETURN_NUMBER((SYS_INT)state)
END_IMPL
//------------------------------------------------------------------------
// int gmp_randinit_lc_2exp_size(gmp_randstate_t state, mp_bitcnt_t size)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gmp_randinit_lc_2exp_size, 2)
    T_NUMBER(gmp_randinit_lc_2exp_size, 1) // mp_bitcnt_t size
    mp_bitcnt_t size = (mp_bitcnt_t)PARAM(1);

    gmp_randstate_t *state = (gmp_randstate_t *)malloc(sizeof(gmp_randstate_t));
    if (!state)
        return (void *)"gmp_randinit_lc_2exp_size: error allocating memory";

    RETURN_NUMBER(gmp_randinit_lc_2exp_size(*state, size))
    SET_NUMBER(0, (SYS_INT)state);
END_IMPL
//------------------------------------------------------------------------
// void gmp_randinit_set(gmp_randstate_t rop, gmp_randstate_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gmp_randinit_set, 2)
    gmp_randstate_t * state = (gmp_randstate_t *)malloc(sizeof(gmp_randstate_t));
    if (!state)
        return (void *)"gmp_randinit_set: error allocating memory";

    T_HANDLE(gmp_randinit_set, 1) // gmp_randstate_t op
    gmp_randstate_t * op = (gmp_randstate_t *)PARAM_INT(1);

    gmp_randinit_set(*state, *op);
    SET_NUMBER(0, (SYS_INT)state)
    RETURN_NUMBER((SYS_INT)state)
END_IMPL
//------------------------------------------------------------------------
// void gmp_randclear(gmp_randstate_t state)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gmp_randclear, 1)
    T_HANDLE(gmp_randclear, 0) // gmp_randstate_t state
    gmp_randstate_t * state = (gmp_randstate_t *)PARAM_INT(0);

    gmp_randclear(*state);
    free(state);
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void gmp_randseed(gmp_randstate_t state, mpz_t seed)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gmp_randseed, 2)
    T_HANDLE(gmp_randseed, 0) // gmp_randstate_t state
    gmp_randstate_t * state = (gmp_randstate_t *)PARAM_INT(0);

    T_HANDLE(gmp_randseed, 1) // mpz_t seed
    mpz_t * seed = (mpz_t *)PARAM_INT(1);

    gmp_randseed(*state, *seed);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void gmp_randseed_ui(gmp_randstate_t state, unsigned long int seed)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gmp_randseed_ui, 2)
    T_HANDLE(gmp_randseed_ui, 0) // gmp_randstate_t state
    gmp_randstate_t * state = (gmp_randstate_t *)PARAM_INT(0);

    T_NUMBER(gmp_randseed_ui, 1) // unsigned long int seed
    unsigned long int seed = (unsigned long int)PARAM(1);

    gmp_randseed_ui(*state, seed);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// unsigned long gmp_urandomb_ui(gmp_randstate_t state, unsigned long n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gmp_urandomb_ui, 2)
    T_HANDLE(gmp_urandomb_ui, 0) // gmp_randstate_t state
    gmp_randstate_t * state = (gmp_randstate_t *)PARAM_INT(0);

    T_NUMBER(gmp_urandomb_ui, 1) // unsigned long n
    unsigned long n = (unsigned long)PARAM(1);

    RETURN_NUMBER(gmp_urandomb_ui(*state, n))
END_IMPL
//------------------------------------------------------------------------
// unsigned long gmp_urandomm_ui(gmp_randstate_t state, unsigned long n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(gmp_urandomm_ui, 2)
    T_HANDLE(gmp_urandomm_ui, 0) // gmp_randstate_t state
    gmp_randstate_t * state = (gmp_randstate_t *)PARAM_INT(0);

    T_NUMBER(gmp_urandomm_ui, 1) // unsigned long n
    unsigned long n = (unsigned long)PARAM(1);

    RETURN_NUMBER(gmp_urandomm_ui(*state, n))
END_IMPL
//------------------------------------------------------------------------
// unsigned long int mpz_get_ui(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_get_ui, 1)
    T_HANDLE(mpz_get_ui, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_get_ui(*op))
END_IMPL
//------------------------------------------------------------------------
// signed long int mpz_get_si(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_get_si, 1)
    T_HANDLE(mpz_get_si, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_get_si(*op))
END_IMPL
//------------------------------------------------------------------------
// double mpz_get_d(mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_get_d, 1)
    T_HANDLE(mpz_get_d, 0) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(0);

    RETURN_NUMBER(mpz_get_d(*op))
END_IMPL
//------------------------------------------------------------------------
// double mpz_get_d_2exp(signed long int* exp, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_get_d_2exp, 2)
    signed long int exp = 0;

    T_HANDLE(mpz_get_d_2exp, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    RETURN_NUMBER(mpz_get_d_2exp(&exp, *op))
    SET_NUMBER(0, exp)
END_IMPL
//------------------------------------------------------------------------
// char* mpz_get_str(char* str, int base, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpz_get_str, 2)
    T_NUMBER(mpz_get_str, 0) // int base
    int base = (int)PARAM(0);

    T_HANDLE(mpz_get_str, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    char *s = mpz_get_str(0, base, *op);
    if (s) {
        RETURN_STRING(s)
        free(s);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
// void mpq_init(mpq_t x)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(mpq_init, 0, 1)
    mpq_t * integ = (mpq_t *)malloc(sizeof(mpq_t));
    if (!integ)
        return (void *)"mpq_init: error allocating memory";
    mpq_init(*integ);

    if (PARAMETERS_COUNT > 0) {
        SET_NUMBER(0, (SYS_INT)integ);
    }
    RETURN_NUMBER((SYS_INT)integ)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(mpq_clear, 1)
    T_HANDLE(mpq_clear, 0)

    mpq_t * integ = (mpq_t *)PARAM_INT(0);
    mpq_clear(*integ);
    free(integ);

    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(mpq_inits, 1)
    T_NUMBER(mpq_inits, 0)

    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);
    long len = PARAM_INT(0);
    for (long i = 0; i < len; i++) {
        mpq_t *integ = (mpq_t *)malloc(sizeof(mpq_t));
        if (integ) {
            mpq_init(*integ);
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)integ);
        } else {
            return (void *)"mpq_inits: error allocating memory";
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(mpq_clears, 1)
    T_ARRAY(mpq_clears, 0)

    void *arr = PARAMETER(0);
    long len = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    for (INTEGER i = 0; i < len; i++) {
        char    *str = 0;
        NUMBER  nDummy;
        INTEGER type;

        Invoke(INVOKE_GET_ARRAY_ELEMENT, arr, i, &type, &str, &nDummy);
        mpq_t *integ = (mpq_t *)(SYS_INT)nDummy;
        if ((type == VARIABLE_NUMBER) && (integ)) {
            mpq_clear(*integ);
            free(integ);
            Invoke(INVOKE_SET_ARRAY_ELEMENT, arr, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)0);
        }
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_set(mpq_t rop, mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_set, 2)
    T_HANDLE(mpq_set, 0) // mpq_t rop
    mpq_t * rop = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_set, 1) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(1);

    mpq_set(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_set_z(mpq_t rop, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_set_z, 2)
    T_HANDLE(mpq_set_z, 0) // mpq_t rop
    mpq_t * rop = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_set_z, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    mpq_set_z(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpq_set_str(mpq_t rop, char* str, int base)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_set_str, 3)
    T_HANDLE(mpq_set_str, 0) // mpq_t rop
    mpq_t * rop = (mpq_t *)PARAM_INT(0);

    T_STRING(mpq_set_str, 1) // char* str
    char *str = (char *)PARAM(1);

    T_NUMBER(mpq_set_str, 2) // int base
    int base = (int)PARAM(2);

    RETURN_NUMBER(mpq_set_str(*rop, str, base))
END_IMPL
//------------------------------------------------------------------------
// void mpq_swap(mpq_t rop1, mpq_t rop2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_swap, 2)
    T_HANDLE(mpq_swap, 0) // mpq_t rop1
    mpq_t * rop1 = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_swap, 1) // mpq_t rop2
    mpq_t * rop2 = (mpq_t *)PARAM_INT(1);

    mpq_swap(*rop1, *rop2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// double mpq_get_d(mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_get_d, 1)
    T_HANDLE(mpq_get_d, 0) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(0);

    RETURN_NUMBER(mpq_get_d(*op))
END_IMPL
//------------------------------------------------------------------------
// void mpq_set_d(mpq_t rop, double op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_set_d, 2)
    T_HANDLE(mpq_set_d, 0) // mpq_t rop
    mpq_t * rop = (mpq_t *)PARAM_INT(0);

    T_NUMBER(mpq_set_d, 1) // double op
    double op = (double)PARAM(1);

    mpq_set_d(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_set_f(mpq_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_set_f, 2)
    T_HANDLE(mpq_set_f, 0) // mpq_t rop
    mpq_t * rop = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_set_f, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpq_set_f(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// char* mpq_get_str(char* str, int base, mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_get_str, 2)
    T_NUMBER(mpq_get_str, 0) // int base
    int base = (int)PARAM(0);

    T_HANDLE(mpq_get_str, 1) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(1);

    char *str = mpq_get_str(0, base, *op);
    if (str) {
        RETURN_STRING(str)
        free(str);
    } else {
        RETURN_STRING("")
    }
END_IMPL
//------------------------------------------------------------------------
// void mpq_add(mpq_t sum, mpq_t addend1, mpq_t addend2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_add, 3)
    T_HANDLE(mpq_add, 0) // mpq_t sum
    mpq_t * sum = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_add, 1) // mpq_t addend1
    mpq_t * addend1 = (mpq_t *)PARAM_INT(1);

    T_HANDLE(mpq_add, 2) // mpq_t addend2
    mpq_t * addend2 = (mpq_t *)PARAM_INT(2);

    mpq_add(*sum, *addend1, *addend2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_sub(mpq_t difference, mpq_t minuend, mpq_t subtrahend)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_sub, 3)
    T_HANDLE(mpq_sub, 0) // mpq_t difference
    mpq_t * difference = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_sub, 1) // mpq_t minuend
    mpq_t * minuend = (mpq_t *)PARAM_INT(1);

    T_HANDLE(mpq_sub, 2) // mpq_t subtrahend
    mpq_t * subtrahend = (mpq_t *)PARAM_INT(2);

    mpq_sub(*difference, *minuend, *subtrahend);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_mul(mpq_t product, mpq_t multiplier, mpq_t multiplicand)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_mul, 3)
    T_HANDLE(mpq_mul, 0) // mpq_t product
    mpq_t * product = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_mul, 1) // mpq_t multiplier
    mpq_t * multiplier = (mpq_t *)PARAM_INT(1);

    T_HANDLE(mpq_mul, 2) // mpq_t multiplicand
    mpq_t * multiplicand = (mpq_t *)PARAM_INT(2);

    mpq_mul(*product, *multiplier, *multiplicand);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_mul_2exp(mpq_t rop, mpq_t op1, mp_bitcnt_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_mul_2exp, 3)
    T_HANDLE(mpq_mul_2exp, 0) // mpq_t rop
    mpq_t * rop = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_mul_2exp, 1) // mpq_t op1
    mpq_t * op1 = (mpq_t *)PARAM_INT(1);

    T_NUMBER(mpq_mul_2exp, 2) // mp_bitcnt_t op2
    mp_bitcnt_t op2 = (mp_bitcnt_t)PARAM(2);

    mpq_mul_2exp(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_div(mpq_t quotient, mpq_t dividend, mpq_t divisor)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_div, 3)
    T_HANDLE(mpq_div, 0) // mpq_t quotient
    mpq_t * quotient = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_div, 1) // mpq_t dividend
    mpq_t * dividend = (mpq_t *)PARAM_INT(1);

    T_HANDLE(mpq_div, 2) // mpq_t divisor
    mpq_t * divisor = (mpq_t *)PARAM_INT(2);

    mpq_div(*quotient, *dividend, *divisor);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_div_2exp(mpq_t rop, mpq_t op1, mp_bitcnt_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_div_2exp, 3)
    T_HANDLE(mpq_div_2exp, 0) // mpq_t rop
    mpq_t * rop = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_div_2exp, 1) // mpq_t op1
    mpq_t * op1 = (mpq_t *)PARAM_INT(1);

    T_NUMBER(mpq_div_2exp, 2) // mp_bitcnt_t op2
    mp_bitcnt_t op2 = (mp_bitcnt_t)PARAM(2);

    mpq_div_2exp(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_neg(mpq_t negated_operand, mpq_t operand)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_neg, 2)
    T_HANDLE(mpq_neg, 0) // mpq_t negated_operand
    mpq_t * negated_operand = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_neg, 1) // mpq_t operand
    mpq_t * operand = (mpq_t *)PARAM_INT(1);

    mpq_neg(*negated_operand, *operand);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_abs(mpq_t rop, mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_abs, 2)
    T_HANDLE(mpq_abs, 0) // mpq_t rop
    mpq_t * rop = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_abs, 1) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(1);

    mpq_abs(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_inv(mpq_t inverted_number, mpq_t number)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_inv, 2)
    T_HANDLE(mpq_inv, 0) // mpq_t inverted_number
    mpq_t * inverted_number = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_inv, 1) // mpq_t number
    mpq_t * number = (mpq_t *)PARAM_INT(1);

    mpq_inv(*inverted_number, *number);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpq_cmp(mpq_t op1, mpq_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_cmp, 2)
    T_HANDLE(mpq_cmp, 0) // mpq_t op1
    mpq_t * op1 = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_cmp, 1) // mpq_t op2
    mpq_t * op2 = (mpq_t *)PARAM_INT(1);

    RETURN_NUMBER(mpq_cmp(*op1, *op2))
END_IMPL
//------------------------------------------------------------------------
// int mpq_cmp_ui(mpq_t op1, unsigned long int num2, unsigned long int den2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_cmp_ui, 3)
    T_HANDLE(mpq_cmp_ui, 0) // mpq_t op1
    mpq_t * op1 = (mpq_t *)PARAM_INT(0);

    T_NUMBER(mpq_cmp_ui, 1) // unsigned long int num2
    unsigned long int num2 = (unsigned long int)PARAM(1);

    T_NUMBER(mpq_cmp_ui, 2) // unsigned long int den2
    unsigned long int den2 = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpq_cmp_ui(*op1, num2, den2))
END_IMPL
//------------------------------------------------------------------------
// int mpq_cmp_si(mpq_t op1, long int num2, unsigned long int den2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_cmp_si, 3)
    T_HANDLE(mpq_cmp_si, 0) // mpq_t op1
    mpq_t * op1 = (mpq_t *)PARAM_INT(0);

    T_NUMBER(mpq_cmp_si, 1) // long int num2
    long int num2 = (long int)PARAM(1);

    T_NUMBER(mpq_cmp_si, 2) // unsigned long int den2
    unsigned long int den2 = (unsigned long int)PARAM(2);

    RETURN_NUMBER(mpq_cmp_si(*op1, num2, den2))
END_IMPL
//------------------------------------------------------------------------
// int mpq_sgn(mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_sgn, 1)
    T_HANDLE(mpq_sgn, 0) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(0);

    RETURN_NUMBER(mpq_sgn(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpq_equal(mpq_t op1, mpq_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_equal, 2)
    T_HANDLE(mpq_equal, 0) // mpq_t op1
    mpq_t * op1 = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_equal, 1) // mpq_t op2
    mpq_t * op2 = (mpq_t *)PARAM_INT(1);

    RETURN_NUMBER(mpq_equal(*op1, *op2))
END_IMPL
//------------------------------------------------------------------------
// mpz_t mpq_numref(mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_numref, 1)
    T_HANDLE(mpq_numref, 0) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(0);

    RETURN_NUMBER((SYS_INT)mpq_numref(*op))
END_IMPL
//------------------------------------------------------------------------
// mpz_t mpq_denref(mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_denref, 1)
    T_HANDLE(mpq_denref, 0) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(0);

    RETURN_NUMBER((SYS_INT)mpq_denref(*op))
END_IMPL
//------------------------------------------------------------------------
// void mpq_get_num(mpz_t numerator, mpq_t rational)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_get_num, 2)
    T_HANDLE(mpq_get_num, 0) // mpz_t numerator
    mpz_t * numerator = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpq_get_num, 1) // mpq_t rational
    mpq_t * rational = (mpq_t *)PARAM_INT(1);

    mpq_get_num(*numerator, *rational);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_get_den(mpz_t denominator, mpq_t rational)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_get_den, 2)
    T_HANDLE(mpq_get_den, 0) // mpz_t denominator
    mpz_t * denominator = (mpz_t *)PARAM_INT(0);

    T_HANDLE(mpq_get_den, 1) // mpq_t rational
    mpq_t * rational = (mpq_t *)PARAM_INT(1);

    mpq_get_den(*denominator, *rational);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_set_num(mpq_t rational, mpz_t numerator)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_set_num, 2)
    T_HANDLE(mpq_set_num, 0) // mpq_t rational
    mpq_t * rational = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_set_num, 1) // mpz_t numerator
    mpz_t * numerator = (mpz_t *)PARAM_INT(1);

    mpq_set_num(*rational, *numerator);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpq_set_den(mpq_t rational, mpz_t denominator)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_set_den, 2)
    T_HANDLE(mpq_set_den, 0) // mpq_t rational
    mpq_t * rational = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_set_den, 1) // mpz_t denominator
    mpz_t * denominator = (mpz_t *)PARAM_INT(1);

    mpq_set_den(*rational, *denominator);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// size_t mpq_out_str(FILE* stream, int base, mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_out_str, 3)
    T_HANDLE(mpq_out_str, 0) // FILE* stream
    FILE * stream = (FILE *)PARAM_INT(0);

    T_NUMBER(mpq_out_str, 1) // int base
    int base = (int)PARAM(1);

    T_HANDLE(mpq_out_str, 2) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(2);

    RETURN_NUMBER(mpq_out_str(stream, base, *op))
END_IMPL
//------------------------------------------------------------------------
// size_t mpq_inp_str(mpq_t rop, FILE* stream, int base)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpq_inp_str, 3)
    T_HANDLE(mpq_inp_str, 0) // mpq_t rop
    mpq_t * rop = (mpq_t *)PARAM_INT(0);

    T_HANDLE(mpq_inp_str, 1) // FILE* stream
    FILE * stream = (FILE *)PARAM_INT(1);

    T_NUMBER(mpq_inp_str, 2) // int base
    int base = (int)PARAM(2);

    RETURN_NUMBER(mpq_inp_str(*rop, stream, base))
END_IMPL

//------------------------------------------------------------------------
// void mpf_set_default_prec(mp_bitcnt_t prec)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set_default_prec, 1)
    T_NUMBER(mpf_set_default_prec, 0) // mp_bitcnt_t prec
    mp_bitcnt_t prec = (mp_bitcnt_t)PARAM(0);

    mpf_set_default_prec(prec);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// mp_bitcnt_t mpf_get_default_prec(void)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_get_default_prec, 0)

    RETURN_NUMBER(mpf_get_default_prec())
END_IMPL
//------------------------------------------------------------------------
// void mpf_init(mpf_t x)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(mpf_init, 0, 1)
    mpf_t * integ = (mpf_t *)malloc(sizeof(mpf_t));
    if (!integ)
        return (void *)"mpf_init: error allocating memory";
    mpf_init(*integ);

    if (PARAMETERS_COUNT > 0) {
        SET_NUMBER(0, (SYS_INT)integ);
    }
    RETURN_NUMBER((SYS_INT)integ)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(mpf_clear, 1)
    T_HANDLE(mpf_clear, 0)

    mpf_t * integ = (mpf_t *)PARAM_INT(0);
    mpf_clear(*integ);
    free(integ);

    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(mpf_inits, 1)
    T_NUMBER(mpf_inits, 0)

    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);
    long len = PARAM_INT(0);
    for (long i = 0; i < len; i++) {
        mpf_t *integ = (mpf_t *)malloc(sizeof(mpf_t));
        if (integ) {
            mpf_init(*integ);
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)integ);
        } else {
            return (void *)"mpf_inits: error allocating memory";
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(mpf_clears, 1)
    T_ARRAY(mpf_clears, 0)

    void *arr = PARAMETER(0);
    long len = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    for (INTEGER i = 0; i < len; i++) {
        char    *str = 0;
        NUMBER  nDummy;
        INTEGER type;

        Invoke(INVOKE_GET_ARRAY_ELEMENT, arr, i, &type, &str, &nDummy);
        mpf_t *integ = (mpf_t *)(SYS_INT)nDummy;
        if ((type == VARIABLE_NUMBER) && (integ)) {
            mpf_clear(*integ);
            free(integ);
            Invoke(INVOKE_SET_ARRAY_ELEMENT, arr, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)0);
        }
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_init2(mpf_t x, mp_bitcnt_t prec)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_init2, 2)
    T_NUMBER(mpf_init2, 1) // mp_bitcnt_t prec
    mp_bitcnt_t prec = (mp_bitcnt_t)PARAM(1);

    mpf_t *integ = (mpf_t *)malloc(sizeof(mpf_t));
    if (!integ)
        return (void *)"mpf_init: error allocating memory";
    mpf_init2(*integ, prec);

    SET_NUMBER(0, (SYS_INT)integ);
    RETURN_NUMBER((SYS_INT)integ)
END_IMPL
//------------------------------------------------------------------------
// mp_bitcnt_t mpf_get_prec(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_get_prec, 1)
    T_HANDLE(mpf_get_prec, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_get_prec(*op))
END_IMPL
//------------------------------------------------------------------------
// void mpf_set_prec(mpf_t rop, mp_bitcnt_t prec)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set_prec, 2)
    T_HANDLE(mpf_set_prec, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_set_prec, 1) // mp_bitcnt_t prec
    mp_bitcnt_t prec = (mp_bitcnt_t)PARAM(1);

    mpf_set_prec(*rop, prec);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_set_prec_raw(mpf_t rop, mp_bitcnt_t prec)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set_prec_raw, 2)
    T_HANDLE(mpf_set_prec_raw, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_set_prec_raw, 1) // mp_bitcnt_t prec
    mp_bitcnt_t prec = (mp_bitcnt_t)PARAM(1);

    mpf_set_prec_raw(*rop, prec);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_set(mpf_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set, 2)
    T_HANDLE(mpf_set, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_set, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpf_set(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_set_ui(mpf_t rop, unsigned long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set_ui, 2)
    T_HANDLE(mpf_set_ui, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_set_ui, 1) // unsigned long int op
    unsigned long int op = (unsigned long int)PARAM(1);

    mpf_set_ui(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_set_si(mpf_t rop, signed long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set_si, 2)
    T_HANDLE(mpf_set_si, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_set_si, 1) // signed long int op
    signed long int op = (signed long int)PARAM(1);

    mpf_set_si(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_set_d(mpf_t rop, double op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set_d, 2)
    T_HANDLE(mpf_set_d, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_set_d, 1) // double op
    double op = (double)PARAM(1);

    mpf_set_d(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_set_z(mpf_t rop, mpz_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set_z, 2)
    T_HANDLE(mpf_set_z, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_set_z, 1) // mpz_t op
    mpz_t * op = (mpz_t *)PARAM_INT(1);

    mpf_set_z(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_set_q(mpf_t rop, mpq_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set_q, 2)
    T_HANDLE(mpf_set_q, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_set_q, 1) // mpq_t op
    mpq_t * op = (mpq_t *)PARAM_INT(1);

    mpf_set_q(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpf_set_str(mpf_t rop, char* str, int base)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_set_str, 3)
    T_HANDLE(mpf_set_str, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_STRING(mpf_set_str, 1) // char* str
    char *str = (char *)PARAM(1);

    T_NUMBER(mpf_set_str, 2) // int base
    int base = (int)PARAM(2);

    RETURN_NUMBER(mpf_set_str(*rop, str, base))
END_IMPL
//------------------------------------------------------------------------
// void mpf_swap(mpf_t rop1, mpf_t rop2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_swap, 2)
    T_HANDLE(mpf_swap, 0) // mpf_t rop1
    mpf_t * rop1 = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_swap, 1) // mpf_t rop2
    mpf_t * rop2 = (mpf_t *)PARAM_INT(1);

    mpf_swap(*rop1, *rop2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_init_set(mpf_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_init_set, 2)
    mpf_t * rop = (mpf_t *)malloc(sizeof(mpf_t));
    if (!rop)
        return (void *)"mpf_init_set: error allocating memory";

    T_HANDLE(mpf_init_set, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpf_init_set(*rop, *op);
    RETURN_NUMBER((SYS_INT)rop)
END_IMPL
//------------------------------------------------------------------------
// void mpf_init_set_ui(mpf_t rop, unsigned long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_init_set_ui, 2)
    mpf_t * rop = (mpf_t *)malloc(sizeof(mpf_t));
    if (!rop)
        return (void *)"mpf_init_set_ui: error allocating memory";

    T_NUMBER(mpf_init_set_ui, 1) // unsigned long int op
    unsigned long int op = (unsigned long int)PARAM(1);

    mpf_init_set_ui(*rop, op);
    RETURN_NUMBER((SYS_INT)rop)
END_IMPL
//------------------------------------------------------------------------
// void mpf_init_set_si(mpf_t rop, signed long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_init_set_si, 2)
    mpf_t * rop = (mpf_t *)malloc(sizeof(mpf_t));
    if (!rop)
        return (void *)"mpf_init_set_si: error allocating memory";

    T_NUMBER(mpf_init_set_si, 1) // signed long int op
    signed long int op = (signed long int)PARAM(1);

    mpf_init_set_si(*rop, op);
    RETURN_NUMBER((SYS_INT)rop)
END_IMPL
//------------------------------------------------------------------------
// void mpf_init_set_d(mpf_t rop, double op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_init_set_d, 2)
    mpf_t * rop = (mpf_t *)malloc(sizeof(mpf_t));
    if (!rop)
        return (void *)"mpf_init_set: error allocating memory";

    T_NUMBER(mpf_init_set_d, 1) // double op
    double op = (double)PARAM(1);

    mpf_init_set_d(*rop, op);
    RETURN_NUMBER((SYS_INT)rop)
END_IMPL
//------------------------------------------------------------------------
// int mpf_init_set_str(mpf_t rop, char* str, int base)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_init_set_str, 3)
    mpf_t * rop = (mpf_t *)malloc(sizeof(mpf_t));
    if (!rop)
        return (void *)"mpf_init_set: error allocating memory";

    T_STRING(mpf_init_set_str, 1) // char* str
    char *str = (char *)PARAM(1);

    T_NUMBER(mpf_init_set_str, 2) // int base
    int base = (int)PARAM(2);

    RETURN_NUMBER(mpf_init_set_str(*rop, str, base))
END_IMPL
//------------------------------------------------------------------------
// double mpf_get_d(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_get_d, 1)
    T_HANDLE(mpf_get_d, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_get_d(*op))
END_IMPL
//------------------------------------------------------------------------
// double mpf_get_d_2exp(signed long int* exp, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_get_d_2exp, 2)
    T_HANDLE(mpf_get_d_2exp, 0) // signed long int* exp
    signed long int **exp = (signed long int **)PARAM_INT(0);

    T_HANDLE(mpf_get_d_2exp, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    RETURN_NUMBER(mpf_get_d_2exp(*exp, *op))
END_IMPL
//------------------------------------------------------------------------
// long mpf_get_si(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_get_si, 1)
    T_HANDLE(mpf_get_si, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_get_si(*op))
END_IMPL
//------------------------------------------------------------------------
// unsigned long mpf_get_ui(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_get_ui, 1)
    T_HANDLE(mpf_get_ui, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_get_ui(*op))
END_IMPL
//------------------------------------------------------------------------
// char* mpf_get_str(char* str, mp_exp_t* expptr, int base, size_t n_digits, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_get_str, 4)
    mp_exp_t expptr;

    T_NUMBER(mpf_get_str, 1) // int base
    int base = (int)PARAM(1);

    T_NUMBER(mpf_get_str, 2) // size_t n_digits
    size_t n_digits = (size_t)PARAM(2);

    T_HANDLE(mpf_get_str, 3) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(3);

    char *str = mpf_get_str(0, &expptr, base, n_digits, *op);
    SET_NUMBER(0, expptr);
    if (str) {
        RETURN_STRING(str)
        free(str);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
// void mpf_add(mpf_t rop, mpf_t op1, mpf_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_add, 3)
    T_HANDLE(mpf_add, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_add, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_HANDLE(mpf_add, 2) // mpf_t op2
    mpf_t * op2 = (mpf_t *)PARAM_INT(2);

    mpf_add(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_add_ui(mpf_t rop, mpf_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_add_ui, 3)
    T_HANDLE(mpf_add_ui, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_add_ui, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_NUMBER(mpf_add_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpf_add_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_sub(mpf_t rop, mpf_t op1, mpf_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_sub, 3)
    T_HANDLE(mpf_sub, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_sub, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_HANDLE(mpf_sub, 2) // mpf_t op2
    mpf_t * op2 = (mpf_t *)PARAM_INT(2);

    mpf_sub(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_ui_sub(mpf_t rop, unsigned long int op1, mpf_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_ui_sub, 3)
    T_HANDLE(mpf_ui_sub, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_ui_sub, 1) // unsigned long int op1
    unsigned long int op1 = (unsigned long int)PARAM(1);

    T_HANDLE(mpf_ui_sub, 2) // mpf_t op2
    mpf_t * op2 = (mpf_t *)PARAM_INT(2);

    mpf_ui_sub(*rop, op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_sub_ui(mpf_t rop, mpf_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_sub_ui, 3)
    T_HANDLE(mpf_sub_ui, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_sub_ui, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_NUMBER(mpf_sub_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpf_sub_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_mul(mpf_t rop, mpf_t op1, mpf_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_mul, 3)
    T_HANDLE(mpf_mul, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_mul, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_HANDLE(mpf_mul, 2) // mpf_t op2
    mpf_t * op2 = (mpf_t *)PARAM_INT(2);

    mpf_mul(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_mul_ui(mpf_t rop, mpf_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_mul_ui, 3)
    T_HANDLE(mpf_mul_ui, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_mul_ui, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_NUMBER(mpf_mul_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpf_mul_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_div(mpf_t rop, mpf_t op1, mpf_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_div, 3)
    T_HANDLE(mpf_div, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_div, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_HANDLE(mpf_div, 2) // mpf_t op2
    mpf_t * op2 = (mpf_t *)PARAM_INT(2);

    mpf_div(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_ui_div(mpf_t rop, unsigned long int op1, mpf_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_ui_div, 3)
    T_HANDLE(mpf_ui_div, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_ui_div, 1) // unsigned long int op1
    unsigned long int op1 = (unsigned long int)PARAM(1);

    T_HANDLE(mpf_ui_div, 2) // mpf_t op2
    mpf_t * op2 = (mpf_t *)PARAM_INT(2);

    mpf_ui_div(*rop, op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_div_ui(mpf_t rop, mpf_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_div_ui, 3)
    T_HANDLE(mpf_div_ui, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_div_ui, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_NUMBER(mpf_div_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpf_div_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_sqrt(mpf_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_sqrt, 2)
    T_HANDLE(mpf_sqrt, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_sqrt, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpf_sqrt(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_sqrt_ui(mpf_t rop, unsigned long int op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_sqrt_ui, 2)
    T_HANDLE(mpf_sqrt_ui, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_sqrt_ui, 1) // unsigned long int op
    unsigned long int op = (unsigned long int)PARAM(1);

    mpf_sqrt_ui(*rop, op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_pow_ui(mpf_t rop, mpf_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_pow_ui, 3)
    T_HANDLE(mpf_pow_ui, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_pow_ui, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_NUMBER(mpf_pow_ui, 2) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(2);

    mpf_pow_ui(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_neg(mpf_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_neg, 2)
    T_HANDLE(mpf_neg, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_neg, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpf_neg(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_abs(mpf_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_abs, 2)
    T_HANDLE(mpf_abs, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_abs, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpf_abs(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_mul_2exp(mpf_t rop, mpf_t op1, mp_bitcnt_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_mul_2exp, 3)
    T_HANDLE(mpf_mul_2exp, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_mul_2exp, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_NUMBER(mpf_mul_2exp, 2) // mp_bitcnt_t op2
    mp_bitcnt_t op2 = (mp_bitcnt_t)PARAM(2);

    mpf_mul_2exp(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_div_2exp(mpf_t rop, mpf_t op1, mp_bitcnt_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_div_2exp, 3)
    T_HANDLE(mpf_div_2exp, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_div_2exp, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_NUMBER(mpf_div_2exp, 2) // mp_bitcnt_t op2
    mp_bitcnt_t op2 = (mp_bitcnt_t)PARAM(2);

    mpf_div_2exp(*rop, *op1, op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpf_cmp(mpf_t op1, mpf_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_cmp, 2)
    T_HANDLE(mpf_cmp, 0) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_cmp, 1) // mpf_t op2
    mpf_t * op2 = (mpf_t *)PARAM_INT(1);

    RETURN_NUMBER(mpf_cmp(*op1, *op2))
END_IMPL
//------------------------------------------------------------------------
// int mpf_cmp_d(mpf_t op1, double op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_cmp_d, 2)
    T_HANDLE(mpf_cmp_d, 0) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_cmp_d, 1) // double op2
    double op2 = (double)PARAM(1);

    RETURN_NUMBER(mpf_cmp_d(*op1, op2))
END_IMPL
//------------------------------------------------------------------------
// int mpf_cmp_ui(mpf_t op1, unsigned long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_cmp_ui, 2)
    T_HANDLE(mpf_cmp_ui, 0) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_cmp_ui, 1) // unsigned long int op2
    unsigned long int op2 = (unsigned long int)PARAM(1);

    RETURN_NUMBER(mpf_cmp_ui(*op1, op2))
END_IMPL
//------------------------------------------------------------------------
// int mpf_cmp_si(mpf_t op1, signed long int op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_cmp_si, 2)
    T_HANDLE(mpf_cmp_si, 0) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_cmp_si, 1) // signed long int op2
    signed long int op2 = (signed long int)PARAM(1);

    RETURN_NUMBER(mpf_cmp_si(*op1, op2))
END_IMPL
//------------------------------------------------------------------------
// int mpf_eq(mpf_t op1, mpf_t op2, mp_bitcnt_t op3)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_eq, 3)
    T_HANDLE(mpf_eq, 0) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_eq, 1) // mpf_t op2
    mpf_t * op2 = (mpf_t *)PARAM_INT(1);

    T_NUMBER(mpf_eq, 2) // mp_bitcnt_t op3
    mp_bitcnt_t op3 = (mp_bitcnt_t)PARAM(2);

    RETURN_NUMBER(mpf_eq(*op1, *op2, op3))
END_IMPL
//------------------------------------------------------------------------
// void mpf_reldiff(mpf_t rop, mpf_t op1, mpf_t op2)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_reldiff, 3)
    T_HANDLE(mpf_reldiff, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_reldiff, 1) // mpf_t op1
    mpf_t * op1 = (mpf_t *)PARAM_INT(1);

    T_HANDLE(mpf_reldiff, 2) // mpf_t op2
    mpf_t * op2 = (mpf_t *)PARAM_INT(2);

    mpf_reldiff(*rop, *op1, *op2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpf_sgn(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_sgn, 1)
    T_HANDLE(mpf_sgn, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_sgn(*op))
END_IMPL
//------------------------------------------------------------------------
// size_t mpf_out_str(FILE* stream, int base, size_t n_digits, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_out_str, 4)
    T_HANDLE(mpf_out_str, 0) // FILE* stream
    FILE * stream = (FILE *)PARAM_INT(0);

    T_NUMBER(mpf_out_str, 1) // int base
    int base = (int)PARAM(1);

    T_NUMBER(mpf_out_str, 2) // size_t n_digits
    size_t n_digits = (size_t)PARAM(2);

    T_HANDLE(mpf_out_str, 3) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(3);

    RETURN_NUMBER(mpf_out_str(stream, base, n_digits, *op))
END_IMPL
//------------------------------------------------------------------------
// size_t mpf_inp_str(mpf_t rop, FILE* stream, int base)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_inp_str, 3)
    T_HANDLE(mpf_inp_str, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_inp_str, 1) // FILE* stream
    FILE * stream = (FILE *)PARAM_INT(1);

    T_NUMBER(mpf_inp_str, 2) // int base
    int base = (int)PARAM(2);

    RETURN_NUMBER(mpf_inp_str(*rop, stream, base))
END_IMPL
//------------------------------------------------------------------------
// void mpf_ceil(mpf_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_ceil, 2)
    T_HANDLE(mpf_ceil, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_ceil, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpf_ceil(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_floor(mpf_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_floor, 2)
    T_HANDLE(mpf_floor, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_floor, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpf_floor(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_trunc(mpf_t rop, mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_trunc, 2)
    T_HANDLE(mpf_trunc, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_trunc, 1) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(1);

    mpf_trunc(*rop, *op);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int mpf_integer_p(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_integer_p, 1)
    T_HANDLE(mpf_integer_p, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_integer_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpf_fits_ulong_p(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_fits_ulong_p, 1)
    T_HANDLE(mpf_fits_ulong_p, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_fits_ulong_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpf_fits_slong_p(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_fits_slong_p, 1)
    T_HANDLE(mpf_fits_slong_p, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_fits_slong_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpf_fits_uint_p(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_fits_uint_p, 1)
    T_HANDLE(mpf_fits_uint_p, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_fits_uint_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpf_fits_sint_p(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_fits_sint_p, 1)
    T_HANDLE(mpf_fits_sint_p, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_fits_sint_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpf_fits_ushort_p(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_fits_ushort_p, 1)
    T_HANDLE(mpf_fits_ushort_p, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_fits_ushort_p(*op))
END_IMPL
//------------------------------------------------------------------------
// int mpf_fits_sshort_p(mpf_t op)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_fits_sshort_p, 1)
    T_HANDLE(mpf_fits_sshort_p, 0) // mpf_t op
    mpf_t * op = (mpf_t *)PARAM_INT(0);

    RETURN_NUMBER(mpf_fits_sshort_p(*op))
END_IMPL
//------------------------------------------------------------------------
// void mpf_urandomb(mpf_t rop, gmp_randstate_t state, mp_bitcnt_t nbits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_urandomb, 3)
    T_HANDLE(mpf_urandomb, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_HANDLE(mpf_urandomb, 1) // gmp_randstate_t state
    gmp_randstate_t * state = (gmp_randstate_t *)PARAM_INT(1);

    T_NUMBER(mpf_urandomb, 2) // mp_bitcnt_t nbits
    mp_bitcnt_t nbits = (mp_bitcnt_t)PARAM(2);

    mpf_urandomb(*rop, *state, nbits);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void mpf_random2(mpf_t rop, mp_size_t max_size, mp_exp_t exp)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(mpf_random2, 3)
    T_HANDLE(mpf_random2, 0) // mpf_t rop
    mpf_t * rop = (mpf_t *)PARAM_INT(0);

    T_NUMBER(mpf_random2, 1) // mp_size_t max_size
    mp_size_t max_size = (mp_size_t)PARAM(1);

    T_NUMBER(mpf_random2, 2) // mp_exp_t exp
    mp_exp_t exp = (mp_exp_t)PARAM(2);

    mpf_random2(*rop, max_size, exp);
    RETURN_NUMBER(0)
END_IMPL

