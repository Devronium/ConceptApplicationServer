//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <string>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define META_MALLOC(v, n, t) \
    (v = (t *)malloc(((n) * sizeof(t))))
#define META_REALLOC(v, n, t) \
    (v = (t *)realloc((v), ((n) * sizeof(t))))
#define META_FREE(x)    free((x))

extern "C" {
    #include "utf8.h"
    #include "utf8proc.h"
}

typedef struct {
    char *str;
    int  length;
    int  bufsize;
    int  free_string_on_destroy;
}
metastring;

std::string UriDecode(const std::string& sSrc);
std::string UriEncode(const std::string& sSrc);

#define NULLCHAR    (char *)0

// Boyer-Moore fast string search ...
//----------------------------------------------------------------------------------------//
#include <limits.h>

//static size_t max(ssize_t a, ssize_t b) { return a>b ? a : b; }
#ifndef max
 #define max(a, b)    a > b ? a : b;
#endif

/* Returns a pointer to the first occurrence of "needle"
 * within "haystack", or NULL if not found.
 */
const unsigned char *memmem_boyermoore_simplified
    (const unsigned char *haystack, size_t hlen,
    const unsigned char *needle, size_t nlen) {
    size_t occ[UCHAR_MAX + 1];
    size_t a, b, hpos;

    if ((nlen > hlen) || (nlen <= 0) || !haystack || !needle) return NULL;

    /* Preprocess */
    /* Initialize the table to default value */
    for (a = 0; a < UCHAR_MAX + 1; ++a) occ[a] = nlen;

    /* Then populate it with the analysis of the needle */
    for (a = 0, b = nlen; a < nlen; ++a) occ[needle[a]] = --b;

    /* Start searching from the end of needle (this is not a typo) */
    for (hpos = nlen - 1; hpos < hlen; ) {
        /* Compare the needle backwards, and stop when first mismatch is found */
        size_t npos = nlen - 1;

        while (haystack[hpos] == needle[npos]) {
            if (npos == 0) return haystack + hpos;
            --hpos;
            --npos;
        }

        /* Find out how much ahead we can skip based
         * on the byte that was found
         */
        hpos += max(nlen - npos, occ[haystack[hpos]]);
    }
    return NULL;
}

//----------------------------------------------------------------------------------------//
int a_rtrim(char *bin, int len) {
    //AnsiString result;

    //int len=strlen(bin);
    if (len) {
        for (int i = len - 1; i >= 0; i--) {
            char c = bin[i];
            if ((c != ' ') && (c != '\t') && (c != '\n') && (c != '\r') && (c != '\0') && (c != '\x0B'))
                return len - i - 1;
        }
    }
    return -1;
}

int a_ltrim(char *bin, int len) {
    if (len) {
        for (int i = 0; i < len; i++) {
            char c = bin[i];
            if ((c != ' ') && (c != '\t') && (c != '\n') && (c != '\r') && (c != '\0') && (c != '\x0B'))
                return i;
        }
    }
    return -1;
}

//---------------------------------------------------------------------------
enum EXPR_EVAL_ERR {
    EEE_NO_ERROR       = 0,
    EEE_PARENTHESIS    = 1,
    EEE_WRONG_CHAR     = 2,
    EEE_DIVIDE_BY_ZERO = 3
};

typedef char   EVAL_CHAR;

// By Peter Kankowski (kankowski@narod.ru, http://www.strchr.com/expression_evaluator)
class ExprEval {
private:
    EXPR_EVAL_ERR _err;
    EVAL_CHAR     *_err_pos;
    int           _paren_count;

    // Parse a number or an expression in parenthesis
    double ParseAtom(EVAL_CHAR *& expr) {
        // Skip spaces
        while ((*expr == ' ') || (*expr == '\t'))
            expr++;

        // Handle the sign before parenthesis (or before number)
        bool negative = false;
        if (*expr == '-') {
            negative = true;
            expr++;
        }
        if (*expr == '+') {
            expr++;
        }

        // Check if there is parenthesis
        if (*expr == '(') {
            expr++;
            _paren_count++;
            double res = ParseSummands(expr);
            if (*expr != ')') {
                // Unmatched opening parenthesis
                _err     = EEE_PARENTHESIS;
                _err_pos = expr;
                return 0;
            }
            expr++;
            _paren_count--;
            return negative ? -res : res;
        }

        // It should be a number; convert it to double
        char   *end_ptr;
        double res = strtod(expr, &end_ptr);
        if (end_ptr == expr) {
            // Report error
            _err     = EEE_WRONG_CHAR;
            _err_pos = expr;
            return 0;
        }
        // Advance the pointer and return the result
        expr = end_ptr;
        return negative ? -res : res;
    }

    // Parse multiplication and division
    double ParseFactors(EVAL_CHAR *& expr) {
        double num1 = ParseAtom(expr);

        for ( ; ; ) {
            // Skip spaces
            while (*expr == ' ')
                expr++;
            // Save the operation and position
            EVAL_CHAR op   = *expr;
            EVAL_CHAR *pos = expr;
            if ((op != '/') && (op != '*') && (op != '%'))
                return num1;
            expr++;
            double num2 = ParseAtom(expr);
            // Perform the saved operation
            if ((op == '/') || (op == '%')) {
                // Handle division by zero
                if (num2 == 0) {
                    _err     = EEE_DIVIDE_BY_ZERO;
                    _err_pos = pos;
                    return 0;
                }
                if (op == '%')
                    num1 = (long)num1 % (long)num2;
                else
                    num1 /= num2;
            } else
                num1 *= num2;
        }
    }

    // Parse addition and subtraction
    double ParseSummands(EVAL_CHAR *& expr) {
        double num1 = ParseFactors(expr);

        for ( ; ; ) {
            // Skip spaces
            while ((*expr == ' ') || (*expr == '\t'))
                expr++;
            EVAL_CHAR op = *expr;
            if ((op != '-') && (op != '+'))
                return num1;
            expr++;
            double num2 = ParseFactors(expr);
            if (op == '-')
                num1 -= num2;
            else
                num1 += num2;
        }
    }

public:
    double Eval(EVAL_CHAR *expr) {
        _paren_count = 0;
        _err         = EEE_NO_ERROR;
        double res = ParseSummands(expr);
        // Now, expr should point to '\0', and _paren_count should be zero
        if ((_paren_count != 0) || (*expr == ')')) {
            _err     = EEE_PARENTHESIS;
            _err_pos = expr;
            return 0;
        }
        if (*expr != '\0') {
            _err     = EEE_WRONG_CHAR;
            _err_pos = expr;
            return 0;
        }
        return res;
    }

    EXPR_EVAL_ERR GetErr() {
        return _err;
    }

    EVAL_CHAR *GetErrPos() {
        return _err_pos;
    }
};
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(EEE_NO_ERROR)
    DEFINE_ECONSTANT(EEE_PARENTHESIS)
    DEFINE_ECONSTANT(EEE_WRONG_CHAR)
    DEFINE_ECONSTANT(EEE_DIVIDE_BY_ZERO)

    DEFINE_ECONSTANT(UTF8PROC_NULLTERM)
    DEFINE_ECONSTANT(UTF8PROC_STABLE)
    DEFINE_ECONSTANT(UTF8PROC_COMPAT)
    DEFINE_ECONSTANT(UTF8PROC_COMPOSE)
    DEFINE_ECONSTANT(UTF8PROC_DECOMPOSE)
    DEFINE_ECONSTANT(UTF8PROC_IGNORE)
    DEFINE_ECONSTANT(UTF8PROC_REJECTNA)
    DEFINE_ECONSTANT(UTF8PROC_NLF2LS)
    DEFINE_ECONSTANT(UTF8PROC_NLF2PS)
    DEFINE_ECONSTANT(UTF8PROC_NLF2LF)
    DEFINE_ECONSTANT(UTF8PROC_STRIPCC)
    DEFINE_ECONSTANT(UTF8PROC_CASEFOLD)
    DEFINE_ECONSTANT(UTF8PROC_CHARBOUND)
    DEFINE_ICONSTANT("UTF8PROC_GROUP", UTF8PROC_LUMP)
    DEFINE_ECONSTANT(UTF8PROC_STRIPMARK)
    DEFINE_ECONSTANT(UTF8PROC_STRIPNA)
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_StrReplace CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "StrReplace takes 3 parameters : (String_to_look, replace_source, replace_with);");
    LOCAL_INIT;

    char       *string;
    double     len_st;
    char       *replace;
    double     len_rep;
    char       *with;
    double     len_with;
    AnsiString result;

    GET_CHECK_BUFFER(0, string, len_st, "StrReplace : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_BUFFER(1, replace, len_rep, "StrReplace : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_BUFFER(2, with, len_with, "StrReplace : parameter 3 should be a string (STATIC STRING)");

    result.LoadBuffer(string, (size_t)len_st);

    int delta = 0;
    if (len_rep) {
        int position = 0;
        size_t after    = 0;
        do {
            const char *szParam0 = result.c_str();
            size_t len0      = result.Length() - delta;

            if (after > len0)
                break;
            const unsigned char *ptr = memmem_boyermoore_simplified((const unsigned char *)szParam0 + after,
                                                                    (size_t)len0 - after,
                                                                    (const unsigned char *)replace,
                                                                    (size_t)len_rep
                                                                    );
            if (ptr) {
                position = (uintptr_t)ptr - (uintptr_t)szParam0;

                int len_temp = len0 - position - (size_t)len_rep;

                if (len_with > len_rep) {
                    AnsiString tmp;
                    tmp.LoadBuffer(szParam0, position);

                    if ((size_t)len_with)
                        tmp.AddBuffer(with, (size_t)len_with);

                    if (len_temp)
                        tmp.AddBuffer((char *)ptr + (long)len_rep, len_temp);

                    result = tmp;
                } else {
                    int delta2 = (int)len_rep - (int)len_with;
                    memcpy((void *)ptr, (void *)with, (size_t)len_with);
                    if (delta2) {
                        int  _def_len = len0 - position - delta2;
                        char *ptr2    = (char *)ptr;
                        for (int i = (int)len_with; i < _def_len; i++)
                            ptr2[i] = ptr2[i + delta2];

                        delta += delta2;
                    }
                }
                after = position + (size_t)len_with;
            } else
                break;
        } while (1);
    }

    int len = result.Length() - delta;
    if (len) {
        RETURN_BUFFER(result.c_str(), len);
    } else {
        RETURN_STRING("");
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_NumberToBin CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "NumberToBin takes one parameter : number to convert;");
    LOCAL_INIT;

    AnsiString    result;
    AnsiString    temp;
    NUMBER        nr;
    unsigned long number_long;
    char          rest;

    GET_CHECK_NUMBER(0, nr, "NumberToBin : parameter 1 should be a number (STATIC NUMBER)");
    number_long = (unsigned long)nr;
    do {
        rest         = number_long % 2;
        number_long /= 2;
        temp         = (char)('0' + rest);
        result       = temp + result;
    } while (number_long);

    RETURN_STRING(result.c_str());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_BinToNumber CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "BinToNumber takes one parameter : string to convert;");
    LOCAL_INIT;

    double result = 0;
    char   *bin;
    GET_CHECK_STRING(0, bin, "BinToNumber : parameter 1 should be a string (STATIC STRING)");

    AnsiString temp = bin;
    int        len  = temp.Length();
    for (int i = 0; i < len; i++) {
        if (temp[i] != '0')
            result += pow(2, len - i - 1);
    }
    RETURN_NUMBER(result);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_RepeatString CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "RepeatString takes 2 parameters : fill_string, length;");
    LOCAL_INIT;

    char   *fill_string;
    NUMBER times;
    GET_CHECK_STRING(0, fill_string, "RepeatString : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, times, "RepeatString : parameter 2 should be a number (STATIC NUMBER)");

    int len = (int)times;
    if (len) {
        int len2 = strlen(fill_string);
        if (!len2) {
            RETURN_STRING("");
            return 0;
        }

        char *buffer = new char[len * len2 + 1];
        buffer[len * len2] = 0;

        char *ptr = buffer;
        for (int i = 0; i < len; i++) {
            memcpy(ptr, fill_string, len2);
            ptr += len2;
        }

        RETURN_STRING(buffer);
        delete[] buffer;
    } else
        RETURN_STRING("");
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SubStr CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 4, "SubStr takes 2 to 4 parameters : string, start[, length][, experimental_link_buffer];");
    LOCAL_INIT;

    //AnsiString result;
    char   *fill_string;
    NUMBER start;
    NUMBER len;
    NUMBER mfill_len;
    NUMBER link_buffer;
    GET_CHECK_BUFFER(0, fill_string, mfill_len, "SubStr : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, start, "SubStr : parameter 2 should be a number (STATIC NUMBER)");
    if (PARAMETERS_COUNT > 2) {
        GET_CHECK_NUMBER(2, len, "SubStr : parameter 3 should be a number (STATIC NUMBER)");
    } else
        len = mfill_len;

    if (PARAMETERS_COUNT > 3) {
        GET_CHECK_NUMBER(3, link_buffer, "SubStr : parameter 4 should be a number (STATIC NUMBER)");
    } else
        link_buffer = 0;

    if (start < 0)
        start = 0;

    if (len < 0)
        len = 0;

    int end      = (int)start + (int)len;
    int fill_len = (int)mfill_len;

    if (!fill_len) {
        RETURN_STRING("");
        return 0;
    }


    if (end > fill_len)
        end = fill_len;

    char *result = fill_string + (INTEGER)start;

    if (end - start <= 0) {
        RETURN_STRING("");
    } else
    if ((int)link_buffer) {
        SetVariable(RESULT, -2, result, end - start);
    } else {
        RETURN_BUFFER(result, end - start);
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_String CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "String takes one parameter : string_buffer");
    LOCAL_INIT;

    char       *fill_string;
    NUMBER     len;
    GET_CHECK_BUFFER(0, fill_string, len, "String : parameter 1 should be a string (STATIC STRING)");

    if (((int)len) && (fill_string)) {
        RETURN_STRING(fill_string);
    } else {
        RETURN_STRING("");
    }
    return 0;
}
//---------------------------------------------------------------------------
int getbytes(unsigned long val, char *buf) {
    if (val == 0) {
        buf[0] = 0;
        return 0;
    }

    int i;
    for (i = 3; i >= 0; i--) {
        buf[i] = val % 0x100;
        val   /= 0x100;
        if (!val)
            break;
    }
    if (i < 0)
        i = 0;
    return i;
}

CONCEPT_DLL_API CONCEPT_chr CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "chr takes one parameter : character index");
    LOCAL_INIT;

    NUMBER ord;
    char   vl[5];
    vl[1] = 0;

    GET_CHECK_NUMBER(0, ord, "chr : parameter 1 should be a number (STATIC NUMBER)");

    if ((ord > 255) || (ord < -255)) {
        int offset = getbytes((unsigned long)ord, vl);
        RETURN_BUFFER(vl + offset, 4 - offset);
    } else {
        vl[0] = (char)ord;
        RETURN_BUFFER(vl, 1);
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ord CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ord takes one parameter : character");
    LOCAL_INIT;
    char   *str;
    NUMBER len;

    GET_CHECK_BUFFER(0, str, len, "ord : parameter 1 should be a string (STATIC STRING)");

    if (len != 1)
        return (char *)"ord: parameter 1 should be a character (a string with 1 byte length)";


    RETURN_NUMBER((double)(unsigned char)str[0]);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_NumberToHex CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "NumberToHex takes one parameter : number to convert;");
    LOCAL_INIT;
    char hex[] = "0123456789abcdef";

    AnsiString    result;
    AnsiString    temp;
    NUMBER        nr;
    unsigned long number_long;
    int           rest;

    GET_CHECK_NUMBER(0, nr, "NumberToHex : parameter 1 should be a number (STATIC NUMBER)");
    number_long = (unsigned long)nr;
    do {
        rest         = number_long % 16;
        number_long /= 16;
        temp         = hex[rest];
        result       = temp + result;
    } while (number_long);

    RETURN_STRING(result.c_str());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_HexToNumber CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "HexToNumber takes one parameter : string to convert;");
    LOCAL_INIT;

    double result = 0;
    char   *bin;
    GET_CHECK_STRING(0, bin, "HexToNumber : parameter 1 should be a string (STATIC STRING)");

    AnsiString temp = bin;
    int        len  = temp.Length();
    for (int i = 0; i < len; i++) {
        if (temp[i] != '0')
            switch (temp[i]) {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    result += (temp[i] - '0') * pow(16, len - i - 1);
                    break;

                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                    result += (10 + temp[i] - 'a') * pow(16, len - i - 1);
                    break;

                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                    result += (10 + temp[i] - 'A') * pow(16, len - i - 1);
                    break;
            }
    }
    RETURN_NUMBER(result);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(HexToString, 1)
    T_STRING(HexToString, 0)
    unsigned char *str = (unsigned char *)PARAM(0);
    int len = PARAM_LEN(0);
    if (len > 1) {
        int           idx  = 0;
        unsigned char *res = NULL;
        CORE_NEW(len / 2 + 1, res);

        for (int i = 0; i < len; i += 2) {
            unsigned char c1 = str[i];
            unsigned char c2 = str[i + 1];
            if ((c1 >= 'a') && (c1 <= 'f'))
                c1 = 10 + c1 - 'a';
            else
            if ((c1 >= 'A') && (c1 <= 'F'))
                c1 = 10 + c1 - 'A';
            else
            if ((c1 >= '0') && (c1 <= '9'))
                c1 -= '0';
            else
                break;

            if ((c2 >= 'a') && (c2 <= 'f'))
                c2 = 10 + c2 - 'a';
            else
            if ((c2 >= 'A') && (c2 <= 'F'))
                c2 = 10 + c2 - 'A';
            else
            if ((c2 >= '0') && (c2 <= '9'))
                c2 -= '0';
            else
                break;

            unsigned char c = c1 * 16 + c2;
            res[idx++] = c;
        }
        res[idx] = 0;
        SetVariable(RESULT, -1, (char *)res, idx);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(StringToHex, 1)
    T_STRING(StringToHex, 0)
    unsigned char *str = (unsigned char *)PARAM(0);
    int len = PARAM_LEN(0);
    if (len > 0) {
        int           idx  = 0;
        unsigned char *res = NULL;
        CORE_NEW(len * 2 + 1, res);

        for (int i = 0; i < len; i++) {
            unsigned char c  = str[i];
            unsigned char c1 = c / 16;
            unsigned char c2 = c % 16;
            if (c1 < 10)
                res[idx++] = '0' + c1;
            else
                res[idx++] = 'a' + c1 - 10;

            if (c2 < 10)
                res[idx++] = '0' + c2;
            else
                res[idx++] = 'a' + c2 - 10;
        }
        res[idx] = 0;
        SetVariable(RESULT, -1, (char *)res, idx);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_NumberToOct CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "NumberToOct takes one parameter : number to convert;");
    LOCAL_INIT;

    AnsiString    result;
    AnsiString    temp;
    NUMBER        nr;
    unsigned long number_long;
    int           rest;

    GET_CHECK_NUMBER(0, nr, "NumberToOct : parameter 1 should be a number (STATIC NUMBER)");
    number_long = (unsigned long)nr;
    do {
        rest         = number_long % 8;
        number_long /= 8;
        temp         = (char)('0' + rest);
        result       = temp + result;
    } while (number_long);

    RETURN_STRING(result.c_str());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_OctToNumber CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "OctToNumber takes one parameter : string to convert;");
    LOCAL_INIT;

    double result = 0;
    char   *bin;
    GET_CHECK_STRING(0, bin, "OctToNumber : parameter 1 should be a string (STATIC STRING)");

    AnsiString temp = bin;
    int        len  = temp.Length();
    for (int i = 0; i < len; i++) {
        if (temp[i] != '0')
            result += (temp[i] - '0') * pow(8, len - i - 1);
    }
    RETURN_NUMBER(result);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ltrim CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ltrim takes one parameter : string to trim left;");
    LOCAL_INIT;

    char *bin;
    GET_CHECK_STRING(0, bin, "ltrim : parameter 1 should be a string (STATIC STRING)");

    int len = strlen(bin);
    if (len) {
        int left = a_ltrim(bin, len);
        if (left < 0) {
            RETURN_STRING("");
            return 0;
        }
        RETURN_BUFFER(bin + left, len - left);
    } else
        RETURN_STRING("");
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_rtrim CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "rtrim takes one parameter : string to trim right;");
    LOCAL_INIT;

    char   *bin = 0;
    NUMBER nDummyLen;

    GET_CHECK_BUFFER(0, bin, nDummyLen, "rtrim : parameter 1 should be a string (STATIC STRING)");

    int len = (INTEGER)nDummyLen;
    if (len) {
        int right = a_rtrim(bin, len);
        if (right < 0) {
            RETURN_STRING("");
            return 0;
        }
        RETURN_BUFFER(bin, len - right);
    } else
        RETURN_STRING("");
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_trim CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "trim takes one parameter : string to trim;");
    LOCAL_INIT;

    char   *bin;
    NUMBER blen;
    GET_CHECK_BUFFER(0, bin, blen, "trim : parameter 1 should be a string (STATIC STRING)");

    if ((!(INTEGER)blen) || (!bin)) {
        RETURN_STRING("");
        return 0;
    }
    INTEGER len = (INTEGER)blen;
    if (len) {
        int right = a_rtrim(bin, len);
        int left  = 0;
        if (right < 0) {
            RETURN_STRING("");
            return 0;
        }
        left = a_ltrim(bin, len - right);

        if (left < 0)
            left = 0;

        int f_len = len - left - right;
        if (f_len) {
            RETURN_BUFFER(bin + left, f_len);
        } else {
            RETURN_STRING("");
        }
    } else
        RETURN_STRING("");
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_Ext CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "Ext takes one parameter");
    LOCAL_INIT;

    char   *bin;
    NUMBER blen;
    GET_CHECK_BUFFER(0, bin, blen, "Ext : parameter 1 should be a string (STATIC STRING)");

    if (blen) {
        int        has_ext = 0;
        for (int i = (int)blen - 1; i >= 0; i--) {
            if (bin[i] == '.') {
                if (i != blen - 1)
                    has_ext = i + 1;
                break;
            }
        }
        if (has_ext) {
            RETURN_BUFFER(bin + has_ext, blen - has_ext);
        } else {
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ToUpper CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ToUpper takes one parameter");
    LOCAL_INIT;

    char   *bin;
    NUMBER nDummyLen = 0;
    GET_CHECK_BUFFER(0, bin, nDummyLen, "ToUpper : parameter 1 should be a string (STATIC STRING)");

    INTEGER blen = (INTEGER)nDummyLen;
    if (blen) {
        char *target = 0;
        CORE_NEW(blen + 1, target);
        target[blen] = 0;

        if (!target) {
            RETURN_STRING("");
            return 0;
        }

        for (int i = 0; i < blen; i++)
            target[i] = toupper(bin[i]);

        SetVariable(RESULT, -1, target, blen);
    } else
        RETURN_STRING("");
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ToLower CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ToLower takes one parameter");
    LOCAL_INIT;

    char   *bin;
    NUMBER nDummyLen = 0;
    GET_CHECK_BUFFER(0, bin, nDummyLen, "ToLower : parameter 1 should be a string (STATIC STRING)");

    INTEGER blen = (INTEGER)nDummyLen;
    if (blen) {
        char *target = 0;
        CORE_NEW(blen + 1, target);
        target[blen] = 0;

        if (!target) {
            RETURN_STRING("");
            return 0;
        }

        for (int i = 0; i < blen; i++)
            target[i] = tolower(bin[i]);

        SetVariable(RESULT, -1, target, blen);
    } else
        RETURN_STRING("");
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ToHTML CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ToHTML takes one parameter");
    LOCAL_INIT;

    char   *bin;
    NUMBER blen;
    GET_CHECK_BUFFER(0, bin, blen, "ToHTML : parameter 1 should be a string (STATIC STRING)");

    if (blen) {
        AnsiString ext;
        for (int i = 0; i < blen; i++) {
            switch (bin[i]) {
                case ' ':
                    ext += "&nbsp;";
                    break;

                case '<':
                    ext += "&lt;";
                    break;

                case '>':
                    ext += "&gt;";
                    break;

                case '&':
                    ext += "&amp;";
                    break;

                default:
                    ext += bin[i];
            }
        }
        RETURN_STRING(ext.c_str());
    } else
        RETURN_STRING("");
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_Pos CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "Pos takes 2 parameters");
    LOCAL_INIT;

    char   *szParam0;
    char   *szParam1;
    double len0;
    double len1;

    GET_CHECK_BUFFER(0, szParam0, len0, "Pos : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_BUFFER(1, szParam1, len1, "Pos : parameter 2 should be a string (STATIC STRING)");

    const unsigned char *ptr = memmem_boyermoore_simplified((const unsigned char *)szParam0,
                                                            (size_t)len0,
                                                            (const unsigned char *)szParam1,
                                                            (size_t)len1
                                                            );
    int pos;
    if (ptr) {
        pos = (uintptr_t)ptr - (uintptr_t)szParam0 + 1;
    } else
        pos = 0;
    RETURN_NUMBER(pos);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_StrSplit CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 3, "StrSplit takes 2(or 3) parameters: string, separator string[, boolean put_empty_strings=0]");
    LOCAL_INIT;

    char   *szParam0 = NULL;
    char   *szParam1 = NULL;
    double put_empty = 0;

    GET_CHECK_STRING(0, szParam0, "StrSplit : parameter 1 should be a string (STATIC STRING)");
    SYS_INT len0 = (INTEGER)nDUMMY_FILL;
    GET_CHECK_STRING(1, szParam1, "StrSplit : parameter 2 should be a string (STATIC STRING)");
    SYS_INT len1 = (INTEGER)nDUMMY_FILL;
    if (PARAMETERS_COUNT == 3) {
        GET_CHECK_NUMBER(2, put_empty, "StrSplit : parameter 3 should be a number (STATIC NUMBER)");
    }

    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, RESULT)))
        return (void *)"Failed to INVOKE_CREATE_ARRAY";
    if (len0 <= 0)
        return 0;

    if (len1 <= 0) {
        if (!IS_OK(Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)0, (INTEGER)VARIABLE_STRING, szParam0, (double)len0)))
            return (void *)"Failed to INVOKE_SET_ARRAY_ELEMENT";
        return 0;
    }

    SYS_INT len_sep = (int)len1;
    const char *target = szParam0;
    SYS_INT target_len = len0;
    const char *pos;
    if (len_sep == 1)
        pos = (const char *)memchr(target, *szParam1, target_len);
    else
        pos = strstr(target, szParam1);
    int index = 0;
    while (pos) {
        SYS_INT str_len = pos - target;
        if (str_len > 0) {
            if (!IS_OK(Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index++, (INTEGER)VARIABLE_STRING, target, (double)str_len)))
                return (void *)"Failed to INVOKE_SET_ARRAY_ELEMENT";
        } else if ((int)put_empty) {
            if (!IS_OK(Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index++, (INTEGER)VARIABLE_STRING, "", (double)0)))
                return (void *)"Failed to INVOKE_SET_ARRAY_ELEMENT";
        }
        target = pos + len_sep;
        target_len -= str_len + len_sep;
        if (target_len <= 0)
            break;
        // pos = strstr(target, szParam1);
        if (len_sep == 1)
            pos = (const char *)memchr(target, *szParam1, target_len);
        else
            pos = strstr(target, szParam1);
    }
    if ((target_len > 0) || ((int)put_empty)) {
        if (target_len <= 0) {
            target_len = 0;
            target = "";
        }
        if (!IS_OK(Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, (INTEGER)VARIABLE_STRING, target, (double)target_len)))
            return (void *)"Failed to INVOKE_SET_ARRAY_ELEMENT";
    }

    return 0;
}
//---------------------------------------------------------------------------
NUMBER atof2(const char *ptr, SYS_INT len) {
    char buf[0x100];
    if (len <= 0)
        return 0;
    if (len >= (SYS_INT)sizeof(buf))
        len = (SYS_INT)sizeof(buf) - 1;
    memmove(buf, ptr, len);
    buf[len] = 0;
    return atof(buf);
}

CONCEPT_DLL_API CONCEPT_StrNumberSplit CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 3, "StrNumberSplit takes 2(or 3) parameters: string, separator string[, boolean put_empty_strings=0]");
    LOCAL_INIT;

    char   *szParam0;
    char   *szParam1;
    double put_empty = 0;

    GET_CHECK_STRING(0, szParam0, "StrNumberSplit : parameter 1 should be a string (STATIC STRING)");
    SYS_INT len0 = (INTEGER)nDUMMY_FILL;
    GET_CHECK_STRING(1, szParam1, "StrNumberSplit : parameter 2 should be a string (STATIC STRING)");
    SYS_INT len1 = (INTEGER)nDUMMY_FILL;
    if (PARAMETERS_COUNT == 3) {
        GET_CHECK_NUMBER(2, put_empty, "StrNumberSplit : parameter 3 should be a number (STATIC NUMBER)");
    }

    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, RESULT)))
        return (void *)"Failed to INVOKE_CREATE_ARRAY";

    if (len1 <= 0) {
        if (!IS_OK(Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)0, (INTEGER)VARIABLE_NUMBER, "", (double)atof2(szParam0, len0))))
            return (void *)"Failed to INVOKE_SET_ARRAY_ELEMENT";
        return 0;
    }

    SYS_INT len_sep = (int)len1;
    const char *target = szParam0;
    SYS_INT target_len = len0;
    const char *pos;
    if (len_sep == 1)
        pos = (const char *)memchr(target, *szParam1, target_len);
    else
        pos = strstr(target, szParam1);
    int index = 0;
    while (pos) {
        SYS_INT str_len = pos - target;
        if (str_len > 0) {
            if (!IS_OK(Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index++, (INTEGER)VARIABLE_NUMBER, "", (double)atof2(target, str_len))))
                return (void *)"Failed to INVOKE_SET_ARRAY_ELEMENT";
        } else if ((int)put_empty) {
            if (!IS_OK(Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index++, (INTEGER)VARIABLE_NUMBER, "", (double)0)))
                return (void *)"Failed to INVOKE_SET_ARRAY_ELEMENT";
        }
        target = pos + len_sep;
        target_len -= str_len + len_sep;
        if (target_len <= 0)
            break;
        // pos = strstr(target, szParam1);
        if (len_sep == 1)
            pos = (const char *)memchr(target, *szParam1, target_len);
        else
            pos = strstr(target, szParam1);
    }
    if ((target_len > 0) || ((int)put_empty)) {
        if (target_len <= 0) {
            target_len = 0;
            target = "0";
        }
        if (!IS_OK(Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index, (INTEGER)VARIABLE_NUMBER, "", (double)atof2(target, target_len))))
            return (void *)"Failed to INVOKE_SET_ARRAY_ELEMENT";
    }


    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_StrFrom CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "StrFrom takes 2 parameters : string, start;");
    LOCAL_INIT;

    char       *fill_string;
    NUMBER     start;
    NUMBER     mfill_len;

    GET_CHECK_BUFFER(0, fill_string, mfill_len, "StrFrom : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(1, start, "StrFrom : parameter 2 should be a number (STATIC NUMBER)");

    if (start < 0)
        start = 0;

    if (mfill_len < 0)
        mfill_len = 0;

    if (start >= mfill_len) {
        RETURN_STRING("");
        return 0;
    }

    RETURN_BUFFER(fill_string + (INTEGER)start, mfill_len - start);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(URIEncode, 1)
    T_STRING(URIEncode, 0)
    std::string in((const char *)PARAM(0), PARAM_LEN(0));

    std::string out = UriEncode(in);

    RETURN_BUFFER(out.c_str(), out.length())
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(URIDecode, 1)
    T_STRING(URIDecode, 0)
    std::string in((const char *)PARAM(0), PARAM_LEN(0));

    std::string out = UriDecode(in);

    RETURN_BUFFER(out.c_str(), out.length())
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8ToWide, 1)
    T_STRING(UTF8ToWide, 0)
    if (PARAM_LEN(0) <= 0) {
        RETURN_STRING("");
        return 0;
    }

    int          len    = PARAM_LEN(0);
    int          new_sz = len + 1;
    unsigned int *buf   = new unsigned int[new_sz];//size len*4 bytes
    buf[0] = 0;
    new_sz = u8_toucs(buf, new_sz, PARAM(0), len);
    RETURN_BUFFER((char *)buf, new_sz * sizeof(unsigned int));
    delete[] buf;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(WideToUTF8, 1)
    T_STRING(WideToUTF8, 0)
    if (PARAM_LEN(0) <= 0) {
        RETURN_STRING("");
        return 0;
    }

    int  len    = (PARAM_LEN(0)) / sizeof(unsigned int);
    int  new_sz = len + 1;
    char *buf   = new char[new_sz];
    buf[0] = 0;
    new_sz = u8_toutf8(buf, new_sz, (unsigned int *)PARAM(0), len);
    RETURN_BUFFER((char *)buf, new_sz * sizeof(unsigned int));
    delete[] buf;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8Offset, 2)
    T_STRING(UTF8Offset, 0)
    T_NUMBER(UTF8Offset, 1)
    if (PARAM_LEN(0) <= 0) {
        RETURN_NUMBER(-1);
        return 0;
    }

    RETURN_NUMBER(u8_offset(PARAM(0), PARAM_INT(1), PARAM_LEN(0)));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8Charnum, 2)
    T_STRING(UTF8Charnum, 0)
    T_NUMBER(UTF8Charnum, 1)

    if (PARAM_LEN(0) <= 0) {
        RETURN_NUMBER(-1);
        return 0;
    }

    RETURN_NUMBER(u8_charnum(PARAM(0), PARAM_INT(1)));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8NextChar, 2)
    T_STRING(UTF8NextChar, 0)
    T_NUMBER(UTF8NextChar, 1)

    int i = PARAM_INT(1);
    RETURN_NUMBER(u8_nextchar(PARAM(0), &i, PARAM_LEN(0)));
    SET_NUMBER(1, i);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8Next, 2)
    T_STRING(UTF8Next, 0)
    T_NUMBER(UTF8Next, 1)

    int i = PARAM_INT(1);
    u8_inc(PARAM(0), &i);
    RETURN_NUMBER(0);
    SET_NUMBER(1, i);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8Prev, 2)
    T_STRING(UTF8Prev, 0)
    T_NUMBER(UTF8Prev, 1)

    int i = PARAM_INT(1);
    u8_dec(PARAM(0), &i);
    RETURN_NUMBER(0);
    SET_NUMBER(1, i);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8Escape, 1)
    T_STRING(UTF8Escape, 0)
    if (PARAM_LEN(0) <= 0) {
        RETURN_STRING("");
        return 0;
    }

    int  sz   = (PARAM_LEN(0)) * 16 + 1; //(max 4 bytes per char, 3 for digit and one for slash
    char *buf = new char[sz];
    buf[0] = 0;
    int len = u8_escape(buf, sz, PARAM(0), 0, PARAM_LEN(0));
    RETURN_BUFFER(buf, len);
    delete[] buf;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8Unescape, 1)
    T_STRING(UTF8Unescape, 0)
    if (PARAM_LEN(0) <= 0) {
        RETURN_STRING("");
        return 0;
    }

    int  sz   = (PARAM_LEN(0)) * 4 + 1;
    char *buf = new char[sz];
    buf[0] = 0;
    int len = u8_unescape(buf, sz, PARAM(0));
    RETURN_BUFFER(buf, len);
    delete[] buf;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8Length, 1)
    T_STRING(UTF8Length, 0)

    if (PARAM_LEN(0) > 0) {
        RETURN_NUMBER((NUMBER)u8_strlen(PARAM(0), PARAM_LEN(0)));
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//---------------------------------------------------------------------------
int toLower(char *str, int length, char *out, bool turkic = false) {
    int offs       = 0;
    int out_offset = 0;

    while (length > 0) {
        short new_char[4] = { -1, -1, -1, -1 };
        short old_char[4] = { -1, -1, -1, -1 };
        int   m           = length < 4 ? length : 4;
        for (int i = 0; i < m; i++)
            old_char[i] = (unsigned char)str[offs + i];
        char new_char_len = 0;
        char old_char_len = 0;
        if (turkic) {
            if ((old_char[0] == 0x49)) {               // LATIN CAPITAL LETTER I
                new_char[0]  = 0xc4;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xb0)) {               // LATIN CAPITAL LETTER I WITH DOT ABOVE
                new_char[0]  = 0x69;
                new_char_len = 1;
                old_char_len = 2;
            }
        }
        if (!old_char_len) {
            if ((old_char[0] == 0x41)) {               // LATIN CAPITAL LETTER A
                new_char[0]  = 0x61;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x42)) {               // LATIN CAPITAL LETTER B
                new_char[0]  = 0x62;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x43)) {               // LATIN CAPITAL LETTER C
                new_char[0]  = 0x63;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x44)) {               // LATIN CAPITAL LETTER D
                new_char[0]  = 0x64;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x45)) {               // LATIN CAPITAL LETTER E
                new_char[0]  = 0x65;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x46)) {               // LATIN CAPITAL LETTER F
                new_char[0]  = 0x66;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x47)) {               // LATIN CAPITAL LETTER G
                new_char[0]  = 0x67;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x48)) {               // LATIN CAPITAL LETTER H
                new_char[0]  = 0x68;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x49)) {               // LATIN CAPITAL LETTER I
                new_char[0]  = 0x69;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x4a)) {               // LATIN CAPITAL LETTER J
                new_char[0]  = 0x6a;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x4b)) {               // LATIN CAPITAL LETTER K
                new_char[0]  = 0x6b;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x4c)) {               // LATIN CAPITAL LETTER L
                new_char[0]  = 0x6c;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x4d)) {               // LATIN CAPITAL LETTER M
                new_char[0]  = 0x6d;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x4e)) {               // LATIN CAPITAL LETTER N
                new_char[0]  = 0x6e;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x4f)) {               // LATIN CAPITAL LETTER O
                new_char[0]  = 0x6f;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x50)) {               // LATIN CAPITAL LETTER P
                new_char[0]  = 0x70;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x51)) {               // LATIN CAPITAL LETTER Q
                new_char[0]  = 0x71;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x52)) {               // LATIN CAPITAL LETTER R
                new_char[0]  = 0x72;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x53)) {               // LATIN CAPITAL LETTER S
                new_char[0]  = 0x73;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x54)) {               // LATIN CAPITAL LETTER T
                new_char[0]  = 0x74;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x55)) {               // LATIN CAPITAL LETTER U
                new_char[0]  = 0x75;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x56)) {               // LATIN CAPITAL LETTER V
                new_char[0]  = 0x76;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x57)) {               // LATIN CAPITAL LETTER W
                new_char[0]  = 0x77;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x58)) {               // LATIN CAPITAL LETTER X
                new_char[0]  = 0x78;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x59)) {               // LATIN CAPITAL LETTER Y
                new_char[0]  = 0x79;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x5a)) {               // LATIN CAPITAL LETTER Z
                new_char[0]  = 0x7a;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0xc2) && (old_char[1] == 0xb5)) {               // MICRO SIGN
                new_char[0]  = 0xce;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x80)) {               // LATIN CAPITAL LETTER A WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x81)) {               // LATIN CAPITAL LETTER A WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x82)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x83)) {               // LATIN CAPITAL LETTER A WITH TILDE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x84)) {               // LATIN CAPITAL LETTER A WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x85)) {               // LATIN CAPITAL LETTER A WITH RING ABOVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x86)) {               // LATIN CAPITAL LETTER AE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x87)) {               // LATIN CAPITAL LETTER C WITH CEDILLA
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x88)) {               // LATIN CAPITAL LETTER E WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x89)) {               // LATIN CAPITAL LETTER E WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x8a)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x8b)) {               // LATIN CAPITAL LETTER E WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x8c)) {               // LATIN CAPITAL LETTER I WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x8d)) {               // LATIN CAPITAL LETTER I WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x8e)) {               // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x8f)) {               // LATIN CAPITAL LETTER I WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x90)) {               // LATIN CAPITAL LETTER ETH
                new_char[0]  = 0xc3;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x91)) {               // LATIN CAPITAL LETTER N WITH TILDE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x92)) {               // LATIN CAPITAL LETTER O WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x93)) {               // LATIN CAPITAL LETTER O WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x94)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x95)) {               // LATIN CAPITAL LETTER O WITH TILDE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x96)) {               // LATIN CAPITAL LETTER O WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x98)) {               // LATIN CAPITAL LETTER O WITH STROKE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x99)) {               // LATIN CAPITAL LETTER U WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x9a)) {               // LATIN CAPITAL LETTER U WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x9b)) {               // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x9c)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x9d)) {               // LATIN CAPITAL LETTER Y WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x9e)) {               // LATIN CAPITAL LETTER THORN
                new_char[0]  = 0xc3;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x80)) {               // LATIN CAPITAL LETTER A WITH MACRON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x82)) {               // LATIN CAPITAL LETTER A WITH BREVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x84)) {               // LATIN CAPITAL LETTER A WITH OGONEK
                new_char[0]  = 0xc4;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x86)) {               // LATIN CAPITAL LETTER C WITH ACUTE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x88)) {               // LATIN CAPITAL LETTER C WITH CIRCUMFLEX
                new_char[0]  = 0xc4;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x8a)) {               // LATIN CAPITAL LETTER C WITH DOT ABOVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x8c)) {               // LATIN CAPITAL LETTER C WITH CARON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x8e)) {               // LATIN CAPITAL LETTER D WITH CARON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x90)) {               // LATIN CAPITAL LETTER D WITH STROKE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x92)) {               // LATIN CAPITAL LETTER E WITH MACRON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x94)) {               // LATIN CAPITAL LETTER E WITH BREVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x96)) {               // LATIN CAPITAL LETTER E WITH DOT ABOVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x98)) {               // LATIN CAPITAL LETTER E WITH OGONEK
                new_char[0]  = 0xc4;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x9a)) {               // LATIN CAPITAL LETTER E WITH CARON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x9c)) {               // LATIN CAPITAL LETTER G WITH CIRCUMFLEX
                new_char[0]  = 0xc4;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x9e)) {               // LATIN CAPITAL LETTER G WITH BREVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa0)) {               // LATIN CAPITAL LETTER G WITH DOT ABOVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa2)) {               // LATIN CAPITAL LETTER G WITH CEDILLA
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa4)) {               // LATIN CAPITAL LETTER H WITH CIRCUMFLEX
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa6)) {               // LATIN CAPITAL LETTER H WITH STROKE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa8)) {               // LATIN CAPITAL LETTER I WITH TILDE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xaa)) {               // LATIN CAPITAL LETTER I WITH MACRON
                new_char[0]  = 0xc4;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xac)) {               // LATIN CAPITAL LETTER I WITH BREVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xae)) {               // LATIN CAPITAL LETTER I WITH OGONEK
                new_char[0]  = 0xc4;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xb2)) {               // LATIN CAPITAL LIGATURE IJ
                new_char[0]  = 0xc4;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xb4)) {               // LATIN CAPITAL LETTER J WITH CIRCUMFLEX
                new_char[0]  = 0xc4;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xb6)) {               // LATIN CAPITAL LETTER K WITH CEDILLA
                new_char[0]  = 0xc4;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xb9)) {               // LATIN CAPITAL LETTER L WITH ACUTE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xbb)) {               // LATIN CAPITAL LETTER L WITH CEDILLA
                new_char[0]  = 0xc4;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xbd)) {               // LATIN CAPITAL LETTER L WITH CARON
                new_char[0]  = 0xc4;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xbf)) {               // LATIN CAPITAL LETTER L WITH MIDDLE DOT
                new_char[0]  = 0xc5;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x81)) {               // LATIN CAPITAL LETTER L WITH STROKE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x83)) {               // LATIN CAPITAL LETTER N WITH ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x85)) {               // LATIN CAPITAL LETTER N WITH CEDILLA
                new_char[0]  = 0xc5;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x87)) {               // LATIN CAPITAL LETTER N WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x8a)) {               // LATIN CAPITAL LETTER ENG
                new_char[0]  = 0xc5;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x8c)) {               // LATIN CAPITAL LETTER O WITH MACRON
                new_char[0]  = 0xc5;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x8e)) {               // LATIN CAPITAL LETTER O WITH BREVE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x90)) {               // LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x92)) {               // LATIN CAPITAL LIGATURE OE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x94)) {               // LATIN CAPITAL LETTER R WITH ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x96)) {               // LATIN CAPITAL LETTER R WITH CEDILLA
                new_char[0]  = 0xc5;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x98)) {               // LATIN CAPITAL LETTER R WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x9a)) {               // LATIN CAPITAL LETTER S WITH ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x9c)) {               // LATIN CAPITAL LETTER S WITH CIRCUMFLEX
                new_char[0]  = 0xc5;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x9e)) {               // LATIN CAPITAL LETTER S WITH CEDILLA
                new_char[0]  = 0xc5;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa0)) {               // LATIN CAPITAL LETTER S WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa2)) {               // LATIN CAPITAL LETTER T WITH CEDILLA
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa4)) {               // LATIN CAPITAL LETTER T WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa6)) {               // LATIN CAPITAL LETTER T WITH STROKE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa8)) {               // LATIN CAPITAL LETTER U WITH TILDE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xaa)) {               // LATIN CAPITAL LETTER U WITH MACRON
                new_char[0]  = 0xc5;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xac)) {               // LATIN CAPITAL LETTER U WITH BREVE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xae)) {               // LATIN CAPITAL LETTER U WITH RING ABOVE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb0)) {               // LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb2)) {               // LATIN CAPITAL LETTER U WITH OGONEK
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb4)) {               // LATIN CAPITAL LETTER W WITH CIRCUMFLEX
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb6)) {               // LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb8)) {               // LATIN CAPITAL LETTER Y WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb9)) {               // LATIN CAPITAL LETTER Z WITH ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xbb)) {               // LATIN CAPITAL LETTER Z WITH DOT ABOVE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xbd)) {               // LATIN CAPITAL LETTER Z WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xbf)) {               // LATIN SMALL LETTER LONG S
                new_char[0]  = 0x73;
                new_char_len = 1;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x81)) {               // LATIN CAPITAL LETTER B WITH HOOK
                new_char[0]  = 0xc9;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x82)) {               // LATIN CAPITAL LETTER B WITH TOPBAR
                new_char[0]  = 0xc6;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x84)) {               // LATIN CAPITAL LETTER TONE SIX
                new_char[0]  = 0xc6;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x86)) {               // LATIN CAPITAL LETTER OPEN O
                new_char[0]  = 0xc9;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x87)) {               // LATIN CAPITAL LETTER C WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x89)) {               // LATIN CAPITAL LETTER AFRICAN D
                new_char[0]  = 0xc9;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x8a)) {               // LATIN CAPITAL LETTER D WITH HOOK
                new_char[0]  = 0xc9;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x8b)) {               // LATIN CAPITAL LETTER D WITH TOPBAR
                new_char[0]  = 0xc6;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x8e)) {               // LATIN CAPITAL LETTER REVERSED E
                new_char[0]  = 0xc7;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x8f)) {               // LATIN CAPITAL LETTER SCHWA
                new_char[0]  = 0xc9;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x90)) {               // LATIN CAPITAL LETTER OPEN E
                new_char[0]  = 0xc9;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x91)) {               // LATIN CAPITAL LETTER F WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x93)) {               // LATIN CAPITAL LETTER G WITH HOOK
                new_char[0]  = 0xc9;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x94)) {               // LATIN CAPITAL LETTER GAMMA
                new_char[0]  = 0xc9;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x96)) {               // LATIN CAPITAL LETTER IOTA
                new_char[0]  = 0xc9;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x97)) {               // LATIN CAPITAL LETTER I WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x98)) {               // LATIN CAPITAL LETTER K WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x9c)) {               // LATIN CAPITAL LETTER TURNED M
                new_char[0]  = 0xc9;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x9d)) {               // LATIN CAPITAL LETTER N WITH LEFT HOOK
                new_char[0]  = 0xc9;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x9f)) {               // LATIN CAPITAL LETTER O WITH MIDDLE TILDE
                new_char[0]  = 0xc9;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa0)) {               // LATIN CAPITAL LETTER O WITH HORN
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa2)) {               // LATIN CAPITAL LETTER OI
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa4)) {               // LATIN CAPITAL LETTER P WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa6)) {               // LATIN LETTER YR
                new_char[0]  = 0xca;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa7)) {               // LATIN CAPITAL LETTER TONE TWO
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa9)) {               // LATIN CAPITAL LETTER ESH
                new_char[0]  = 0xca;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xac)) {               // LATIN CAPITAL LETTER T WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xae)) {               // LATIN CAPITAL LETTER T WITH RETROFLEX HOOK
                new_char[0]  = 0xca;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xaf)) {               // LATIN CAPITAL LETTER U WITH HORN
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb1)) {               // LATIN CAPITAL LETTER UPSILON
                new_char[0]  = 0xca;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb2)) {               // LATIN CAPITAL LETTER V WITH HOOK
                new_char[0]  = 0xca;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb3)) {               // LATIN CAPITAL LETTER Y WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb5)) {               // LATIN CAPITAL LETTER Z WITH STROKE
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb7)) {               // LATIN CAPITAL LETTER EZH
                new_char[0]  = 0xca;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb8)) {               // LATIN CAPITAL LETTER EZH REVERSED
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xbc)) {               // LATIN CAPITAL LETTER TONE FIVE
                new_char[0]  = 0xc6;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x84)) {               // LATIN CAPITAL LETTER DZ WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x85)) {               // LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x87)) {               // LATIN CAPITAL LETTER LJ
                new_char[0]  = 0xc7;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x88)) {               // LATIN CAPITAL LETTER L WITH SMALL LETTER J
                new_char[0]  = 0xc7;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x8a)) {               // LATIN CAPITAL LETTER NJ
                new_char[0]  = 0xc7;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x8b)) {               // LATIN CAPITAL LETTER N WITH SMALL LETTER J
                new_char[0]  = 0xc7;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x8d)) {               // LATIN CAPITAL LETTER A WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x8f)) {               // LATIN CAPITAL LETTER I WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x91)) {               // LATIN CAPITAL LETTER O WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x93)) {               // LATIN CAPITAL LETTER U WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x95)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x97)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x99)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x9b)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
                new_char[0]  = 0xc7;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x9e)) {               // LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa0)) {               // LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa2)) {               // LATIN CAPITAL LETTER AE WITH MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa4)) {               // LATIN CAPITAL LETTER G WITH STROKE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa6)) {               // LATIN CAPITAL LETTER G WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa8)) {               // LATIN CAPITAL LETTER K WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xaa)) {               // LATIN CAPITAL LETTER O WITH OGONEK
                new_char[0]  = 0xc7;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xac)) {               // LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xae)) {               // LATIN CAPITAL LETTER EZH WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb1)) {               // LATIN CAPITAL LETTER DZ
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb2)) {               // LATIN CAPITAL LETTER D WITH SMALL LETTER Z
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb4)) {               // LATIN CAPITAL LETTER G WITH ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb6)) {               // LATIN CAPITAL LETTER HWAIR
                new_char[0]  = 0xc6;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb7)) {               // LATIN CAPITAL LETTER WYNN
                new_char[0]  = 0xc6;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb8)) {               // LATIN CAPITAL LETTER N WITH GRAVE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xba)) {               // LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xbc)) {               // LATIN CAPITAL LETTER AE WITH ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xbe)) {               // LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x80)) {               // LATIN CAPITAL LETTER A WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x82)) {               // LATIN CAPITAL LETTER A WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x84)) {               // LATIN CAPITAL LETTER E WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x86)) {               // LATIN CAPITAL LETTER E WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x88)) {               // LATIN CAPITAL LETTER I WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x8a)) {               // LATIN CAPITAL LETTER I WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x8c)) {               // LATIN CAPITAL LETTER O WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x8e)) {               // LATIN CAPITAL LETTER O WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x90)) {               // LATIN CAPITAL LETTER R WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x92)) {               // LATIN CAPITAL LETTER R WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x94)) {               // LATIN CAPITAL LETTER U WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x96)) {               // LATIN CAPITAL LETTER U WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x98)) {               // LATIN CAPITAL LETTER S WITH COMMA BELOW
                new_char[0]  = 0xc8;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x9a)) {               // LATIN CAPITAL LETTER T WITH COMMA BELOW
                new_char[0]  = 0xc8;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x9c)) {               // LATIN CAPITAL LETTER YOGH
                new_char[0]  = 0xc8;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x9e)) {               // LATIN CAPITAL LETTER H WITH CARON
                new_char[0]  = 0xc8;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xa0)) {               // LATIN CAPITAL LETTER N WITH LONG RIGHT LEG
                new_char[0]  = 0xc6;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xa2)) {               // LATIN CAPITAL LETTER OU
                new_char[0]  = 0xc8;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xa4)) {               // LATIN CAPITAL LETTER Z WITH HOOK
                new_char[0]  = 0xc8;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xa6)) {               // LATIN CAPITAL LETTER A WITH DOT ABOVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xa8)) {               // LATIN CAPITAL LETTER E WITH CEDILLA
                new_char[0]  = 0xc8;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xaa)) {               // LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
                new_char[0]  = 0xc8;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xac)) {               // LATIN CAPITAL LETTER O WITH TILDE AND MACRON
                new_char[0]  = 0xc8;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xae)) {               // LATIN CAPITAL LETTER O WITH DOT ABOVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xb0)) {               // LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
                new_char[0]  = 0xc8;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xb2)) {               // LATIN CAPITAL LETTER Y WITH MACRON
                new_char[0]  = 0xc8;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xba)) {               // LATIN CAPITAL LETTER A WITH STROKE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xbb)) {               // LATIN CAPITAL LETTER C WITH STROKE
                new_char[0]  = 0xc8;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xbd)) {               // LATIN CAPITAL LETTER L WITH BAR
                new_char[0]  = 0xc6;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xbe)) {               // LATIN CAPITAL LETTER T WITH DIAGONAL STROKE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x81)) {               // LATIN CAPITAL LETTER GLOTTAL STOP
                new_char[0]  = 0xc9;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x83)) {               // LATIN CAPITAL LETTER B WITH STROKE
                new_char[0]  = 0xc6;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x84)) {               // LATIN CAPITAL LETTER U BAR
                new_char[0]  = 0xca;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x85)) {               // LATIN CAPITAL LETTER TURNED V
                new_char[0]  = 0xca;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x86)) {               // LATIN CAPITAL LETTER E WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x88)) {               // LATIN CAPITAL LETTER J WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x8a)) {               // LATIN CAPITAL LETTER SMALL Q WITH HOOK TAIL
                new_char[0]  = 0xc9;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x8c)) {               // LATIN CAPITAL LETTER R WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x8e)) {               // LATIN CAPITAL LETTER Y WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0x85)) {               // COMBINING GREEK YPOGEGRAMMENI
                new_char[0]  = 0xce;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0xb0)) {               // GREEK CAPITAL LETTER HETA
                new_char[0]  = 0xcd;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0xb2)) {               // GREEK CAPITAL LETTER ARCHAIC SAMPI
                new_char[0]  = 0xcd;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0xb6)) {               // GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA
                new_char[0]  = 0xcd;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x86)) {               // GREEK CAPITAL LETTER ALPHA WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x88)) {               // GREEK CAPITAL LETTER EPSILON WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x89)) {               // GREEK CAPITAL LETTER ETA WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x8a)) {               // GREEK CAPITAL LETTER IOTA WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x8c)) {               // GREEK CAPITAL LETTER OMICRON WITH TONOS
                new_char[0]  = 0xcf;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x8e)) {               // GREEK CAPITAL LETTER UPSILON WITH TONOS
                new_char[0]  = 0xcf;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x8f)) {               // GREEK CAPITAL LETTER OMEGA WITH TONOS
                new_char[0]  = 0xcf;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x91)) {               // GREEK CAPITAL LETTER ALPHA
                new_char[0]  = 0xce;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x92)) {               // GREEK CAPITAL LETTER BETA
                new_char[0]  = 0xce;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x93)) {               // GREEK CAPITAL LETTER GAMMA
                new_char[0]  = 0xce;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x94)) {               // GREEK CAPITAL LETTER DELTA
                new_char[0]  = 0xce;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x95)) {               // GREEK CAPITAL LETTER EPSILON
                new_char[0]  = 0xce;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x96)) {               // GREEK CAPITAL LETTER ZETA
                new_char[0]  = 0xce;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x97)) {               // GREEK CAPITAL LETTER ETA
                new_char[0]  = 0xce;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x98)) {               // GREEK CAPITAL LETTER THETA
                new_char[0]  = 0xce;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x99)) {               // GREEK CAPITAL LETTER IOTA
                new_char[0]  = 0xce;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x9a)) {               // GREEK CAPITAL LETTER KAPPA
                new_char[0]  = 0xce;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x9b)) {               // GREEK CAPITAL LETTER LAMDA
                new_char[0]  = 0xce;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x9c)) {               // GREEK CAPITAL LETTER MU
                new_char[0]  = 0xce;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x9d)) {               // GREEK CAPITAL LETTER NU
                new_char[0]  = 0xce;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x9e)) {               // GREEK CAPITAL LETTER XI
                new_char[0]  = 0xce;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0x9f)) {               // GREEK CAPITAL LETTER OMICRON
                new_char[0]  = 0xce;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xa0)) {               // GREEK CAPITAL LETTER PI
                new_char[0]  = 0xcf;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xa1)) {               // GREEK CAPITAL LETTER RHO
                new_char[0]  = 0xcf;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xa3)) {               // GREEK CAPITAL LETTER SIGMA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xa4)) {               // GREEK CAPITAL LETTER TAU
                new_char[0]  = 0xcf;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xa5)) {               // GREEK CAPITAL LETTER UPSILON
                new_char[0]  = 0xcf;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xa6)) {               // GREEK CAPITAL LETTER PHI
                new_char[0]  = 0xcf;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xa7)) {               // GREEK CAPITAL LETTER CHI
                new_char[0]  = 0xcf;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xa8)) {               // GREEK CAPITAL LETTER PSI
                new_char[0]  = 0xcf;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xa9)) {               // GREEK CAPITAL LETTER OMEGA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xaa)) {               // GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xab)) {               // GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x82)) {               // GREEK SMALL LETTER FINAL SIGMA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x8f)) {               // GREEK CAPITAL KAI SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x90)) {               // GREEK BETA SYMBOL
                new_char[0]  = 0xce;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x91)) {               // GREEK THETA SYMBOL
                new_char[0]  = 0xce;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x95)) {               // GREEK PHI SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x96)) {               // GREEK PI SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x98)) {               // GREEK LETTER ARCHAIC KOPPA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x9a)) {               // GREEK LETTER STIGMA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x9c)) {               // GREEK LETTER DIGAMMA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x9e)) {               // GREEK LETTER KOPPA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa0)) {               // GREEK LETTER SAMPI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa2)) {               // COPTIC CAPITAL LETTER SHEI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa4)) {               // COPTIC CAPITAL LETTER FEI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa6)) {               // COPTIC CAPITAL LETTER KHEI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa8)) {               // COPTIC CAPITAL LETTER HORI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xaa)) {               // COPTIC CAPITAL LETTER GANGIA
                new_char[0]  = 0xcf;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xac)) {               // COPTIC CAPITAL LETTER SHIMA
                new_char[0]  = 0xcf;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xae)) {               // COPTIC CAPITAL LETTER DEI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xb0)) {               // GREEK KAPPA SYMBOL
                new_char[0]  = 0xce;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xb1)) {               // GREEK RHO SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xb4)) {               // GREEK CAPITAL THETA SYMBOL
                new_char[0]  = 0xce;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xb5)) {               // GREEK LUNATE EPSILON SYMBOL
                new_char[0]  = 0xce;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xb7)) {               // GREEK CAPITAL LETTER SHO
                new_char[0]  = 0xcf;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xb9)) {               // GREEK CAPITAL LUNATE SIGMA SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xba)) {               // GREEK CAPITAL LETTER SAN
                new_char[0]  = 0xcf;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xbd)) {               // GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL
                new_char[0]  = 0xcd;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xbe)) {               // GREEK CAPITAL DOTTED LUNATE SIGMA SYMBOL
                new_char[0]  = 0xcd;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xbf)) {               // GREEK CAPITAL REVERSED DOTTED LUNATE SIGMA SYMBOL
                new_char[0]  = 0xcd;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x80)) {               // CYRILLIC CAPITAL LETTER IE WITH GRAVE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x81)) {               // CYRILLIC CAPITAL LETTER IO
                new_char[0]  = 0xd1;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x82)) {               // CYRILLIC CAPITAL LETTER DJE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x83)) {               // CYRILLIC CAPITAL LETTER GJE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x84)) {               // CYRILLIC CAPITAL LETTER UKRAINIAN IE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x85)) {               // CYRILLIC CAPITAL LETTER DZE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x86)) {               // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
                new_char[0]  = 0xd1;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x87)) {               // CYRILLIC CAPITAL LETTER YI
                new_char[0]  = 0xd1;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x88)) {               // CYRILLIC CAPITAL LETTER JE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x89)) {               // CYRILLIC CAPITAL LETTER LJE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x8a)) {               // CYRILLIC CAPITAL LETTER NJE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x8b)) {               // CYRILLIC CAPITAL LETTER TSHE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x8c)) {               // CYRILLIC CAPITAL LETTER KJE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x8d)) {               // CYRILLIC CAPITAL LETTER I WITH GRAVE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x8e)) {               // CYRILLIC CAPITAL LETTER SHORT U
                new_char[0]  = 0xd1;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x8f)) {               // CYRILLIC CAPITAL LETTER DZHE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x90)) {               // CYRILLIC CAPITAL LETTER A
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x91)) {               // CYRILLIC CAPITAL LETTER BE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x92)) {               // CYRILLIC CAPITAL LETTER VE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x93)) {               // CYRILLIC CAPITAL LETTER GHE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x94)) {               // CYRILLIC CAPITAL LETTER DE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x95)) {               // CYRILLIC CAPITAL LETTER IE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x96)) {               // CYRILLIC CAPITAL LETTER ZHE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x97)) {               // CYRILLIC CAPITAL LETTER ZE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x98)) {               // CYRILLIC CAPITAL LETTER I
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x99)) {               // CYRILLIC CAPITAL LETTER SHORT I
                new_char[0]  = 0xd0;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x9a)) {               // CYRILLIC CAPITAL LETTER KA
                new_char[0]  = 0xd0;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x9b)) {               // CYRILLIC CAPITAL LETTER EL
                new_char[0]  = 0xd0;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x9c)) {               // CYRILLIC CAPITAL LETTER EM
                new_char[0]  = 0xd0;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x9d)) {               // CYRILLIC CAPITAL LETTER EN
                new_char[0]  = 0xd0;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x9e)) {               // CYRILLIC CAPITAL LETTER O
                new_char[0]  = 0xd0;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0x9f)) {               // CYRILLIC CAPITAL LETTER PE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa0)) {               // CYRILLIC CAPITAL LETTER ER
                new_char[0]  = 0xd1;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa1)) {               // CYRILLIC CAPITAL LETTER ES
                new_char[0]  = 0xd1;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa2)) {               // CYRILLIC CAPITAL LETTER TE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa3)) {               // CYRILLIC CAPITAL LETTER U
                new_char[0]  = 0xd1;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa4)) {               // CYRILLIC CAPITAL LETTER EF
                new_char[0]  = 0xd1;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa5)) {               // CYRILLIC CAPITAL LETTER HA
                new_char[0]  = 0xd1;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa6)) {               // CYRILLIC CAPITAL LETTER TSE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa7)) {               // CYRILLIC CAPITAL LETTER CHE
                new_char[0]  = 0xd1;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa8)) {               // CYRILLIC CAPITAL LETTER SHA
                new_char[0]  = 0xd1;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xa9)) {               // CYRILLIC CAPITAL LETTER SHCHA
                new_char[0]  = 0xd1;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xaa)) {               // CYRILLIC CAPITAL LETTER HARD SIGN
                new_char[0]  = 0xd1;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xab)) {               // CYRILLIC CAPITAL LETTER YERU
                new_char[0]  = 0xd1;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xac)) {               // CYRILLIC CAPITAL LETTER SOFT SIGN
                new_char[0]  = 0xd1;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xad)) {               // CYRILLIC CAPITAL LETTER E
                new_char[0]  = 0xd1;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xae)) {               // CYRILLIC CAPITAL LETTER YU
                new_char[0]  = 0xd1;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xaf)) {               // CYRILLIC CAPITAL LETTER YA
                new_char[0]  = 0xd1;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa0)) {               // CYRILLIC CAPITAL LETTER OMEGA
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa2)) {               // CYRILLIC CAPITAL LETTER YAT
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa4)) {               // CYRILLIC CAPITAL LETTER IOTIFIED E
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa6)) {               // CYRILLIC CAPITAL LETTER LITTLE YUS
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa8)) {               // CYRILLIC CAPITAL LETTER IOTIFIED LITTLE YUS
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xaa)) {               // CYRILLIC CAPITAL LETTER BIG YUS
                new_char[0]  = 0xd1;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xac)) {               // CYRILLIC CAPITAL LETTER IOTIFIED BIG YUS
                new_char[0]  = 0xd1;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xae)) {               // CYRILLIC CAPITAL LETTER KSI
                new_char[0]  = 0xd1;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb0)) {               // CYRILLIC CAPITAL LETTER PSI
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb2)) {               // CYRILLIC CAPITAL LETTER FITA
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb4)) {               // CYRILLIC CAPITAL LETTER IZHITSA
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb6)) {               // CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb8)) {               // CYRILLIC CAPITAL LETTER UK
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xba)) {               // CYRILLIC CAPITAL LETTER ROUND OMEGA
                new_char[0]  = 0xd1;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xbc)) {               // CYRILLIC CAPITAL LETTER OMEGA WITH TITLO
                new_char[0]  = 0xd1;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xbe)) {               // CYRILLIC CAPITAL LETTER OT
                new_char[0]  = 0xd1;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x80)) {               // CYRILLIC CAPITAL LETTER KOPPA
                new_char[0]  = 0xd2;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x8a)) {               // CYRILLIC CAPITAL LETTER SHORT I WITH TAIL
                new_char[0]  = 0xd2;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x8c)) {               // CYRILLIC CAPITAL LETTER SEMISOFT SIGN
                new_char[0]  = 0xd2;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x8e)) {               // CYRILLIC CAPITAL LETTER ER WITH TICK
                new_char[0]  = 0xd2;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x90)) {               // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
                new_char[0]  = 0xd2;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x92)) {               // CYRILLIC CAPITAL LETTER GHE WITH STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x94)) {               // CYRILLIC CAPITAL LETTER GHE WITH MIDDLE HOOK
                new_char[0]  = 0xd2;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x96)) {               // CYRILLIC CAPITAL LETTER ZHE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x98)) {               // CYRILLIC CAPITAL LETTER ZE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x9a)) {               // CYRILLIC CAPITAL LETTER KA WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x9c)) {               // CYRILLIC CAPITAL LETTER KA WITH VERTICAL STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x9e)) {               // CYRILLIC CAPITAL LETTER KA WITH STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa0)) {               // CYRILLIC CAPITAL LETTER BASHKIR KA
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa2)) {               // CYRILLIC CAPITAL LETTER EN WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa4)) {               // CYRILLIC CAPITAL LIGATURE EN GHE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa6)) {               // CYRILLIC CAPITAL LETTER PE WITH MIDDLE HOOK
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa8)) {               // CYRILLIC CAPITAL LETTER ABKHASIAN HA
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xaa)) {               // CYRILLIC CAPITAL LETTER ES WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xac)) {               // CYRILLIC CAPITAL LETTER TE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xae)) {               // CYRILLIC CAPITAL LETTER STRAIGHT U
                new_char[0]  = 0xd2;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb0)) {               // CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb2)) {               // CYRILLIC CAPITAL LETTER HA WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb4)) {               // CYRILLIC CAPITAL LIGATURE TE TSE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb6)) {               // CYRILLIC CAPITAL LETTER CHE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb8)) {               // CYRILLIC CAPITAL LETTER CHE WITH VERTICAL STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xba)) {               // CYRILLIC CAPITAL LETTER SHHA
                new_char[0]  = 0xd2;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xbc)) {               // CYRILLIC CAPITAL LETTER ABKHASIAN CHE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xbe)) {               // CYRILLIC CAPITAL LETTER ABKHASIAN CHE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x80)) {               // CYRILLIC LETTER PALOCHKA
                new_char[0]  = 0xd3;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x81)) {               // CYRILLIC CAPITAL LETTER ZHE WITH BREVE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x83)) {               // CYRILLIC CAPITAL LETTER KA WITH HOOK
                new_char[0]  = 0xd3;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x85)) {               // CYRILLIC CAPITAL LETTER EL WITH TAIL
                new_char[0]  = 0xd3;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x87)) {               // CYRILLIC CAPITAL LETTER EN WITH HOOK
                new_char[0]  = 0xd3;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x89)) {               // CYRILLIC CAPITAL LETTER EN WITH TAIL
                new_char[0]  = 0xd3;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x8b)) {               // CYRILLIC CAPITAL LETTER KHAKASSIAN CHE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x8d)) {               // CYRILLIC CAPITAL LETTER EM WITH TAIL
                new_char[0]  = 0xd3;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x90)) {               // CYRILLIC CAPITAL LETTER A WITH BREVE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x92)) {               // CYRILLIC CAPITAL LETTER A WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x94)) {               // CYRILLIC CAPITAL LIGATURE A IE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x96)) {               // CYRILLIC CAPITAL LETTER IE WITH BREVE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x98)) {               // CYRILLIC CAPITAL LETTER SCHWA
                new_char[0]  = 0xd3;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x9a)) {               // CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x9c)) {               // CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x9e)) {               // CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa0)) {               // CYRILLIC CAPITAL LETTER ABKHASIAN DZE
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa2)) {               // CYRILLIC CAPITAL LETTER I WITH MACRON
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa4)) {               // CYRILLIC CAPITAL LETTER I WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa6)) {               // CYRILLIC CAPITAL LETTER O WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa8)) {               // CYRILLIC CAPITAL LETTER BARRED O
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xaa)) {               // CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xac)) {               // CYRILLIC CAPITAL LETTER E WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xae)) {               // CYRILLIC CAPITAL LETTER U WITH MACRON
                new_char[0]  = 0xd3;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb0)) {               // CYRILLIC CAPITAL LETTER U WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb2)) {               // CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb4)) {               // CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb6)) {               // CYRILLIC CAPITAL LETTER GHE WITH DESCENDER
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb8)) {               // CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xba)) {               // CYRILLIC CAPITAL LETTER GHE WITH STROKE AND HOOK
                new_char[0]  = 0xd3;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xbc)) {               // CYRILLIC CAPITAL LETTER HA WITH HOOK
                new_char[0]  = 0xd3;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xbe)) {               // CYRILLIC CAPITAL LETTER HA WITH STROKE
                new_char[0]  = 0xd3;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x80)) {               // CYRILLIC CAPITAL LETTER KOMI DE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x82)) {               // CYRILLIC CAPITAL LETTER KOMI DJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x84)) {               // CYRILLIC CAPITAL LETTER KOMI ZJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x86)) {               // CYRILLIC CAPITAL LETTER KOMI DZJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x88)) {               // CYRILLIC CAPITAL LETTER KOMI LJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x8a)) {               // CYRILLIC CAPITAL LETTER KOMI NJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x8c)) {               // CYRILLIC CAPITAL LETTER KOMI SJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x8e)) {               // CYRILLIC CAPITAL LETTER KOMI TJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x90)) {               // CYRILLIC CAPITAL LETTER REVERSED ZE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x92)) {               // CYRILLIC CAPITAL LETTER EL WITH HOOK
                new_char[0]  = 0xd4;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x94)) {               // CYRILLIC CAPITAL LETTER LHA
                new_char[0]  = 0xd4;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x96)) {               // CYRILLIC CAPITAL LETTER RHA
                new_char[0]  = 0xd4;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x98)) {               // CYRILLIC CAPITAL LETTER YAE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x9a)) {               // CYRILLIC CAPITAL LETTER QA
                new_char[0]  = 0xd4;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x9c)) {               // CYRILLIC CAPITAL LETTER WE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x9e)) {               // CYRILLIC CAPITAL LETTER ALEUT KA
                new_char[0]  = 0xd4;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xa0)) {               // CYRILLIC CAPITAL LETTER EL WITH MIDDLE HOOK
                new_char[0]  = 0xd4;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xa2)) {               // CYRILLIC CAPITAL LETTER EN WITH MIDDLE HOOK
                new_char[0]  = 0xd4;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xa4)) {               // CYRILLIC CAPITAL LETTER PE WITH DESCENDER
                new_char[0]  = 0xd4;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xa6)) {               // CYRILLIC CAPITAL LETTER SHHA WITH DESCENDER
                new_char[0]  = 0xd4;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xb1)) {               // ARMENIAN CAPITAL LETTER AYB
                new_char[0]  = 0xd5;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xb2)) {               // ARMENIAN CAPITAL LETTER BEN
                new_char[0]  = 0xd5;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xb3)) {               // ARMENIAN CAPITAL LETTER GIM
                new_char[0]  = 0xd5;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xb4)) {               // ARMENIAN CAPITAL LETTER DA
                new_char[0]  = 0xd5;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xb5)) {               // ARMENIAN CAPITAL LETTER ECH
                new_char[0]  = 0xd5;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xb6)) {               // ARMENIAN CAPITAL LETTER ZA
                new_char[0]  = 0xd5;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xb7)) {               // ARMENIAN CAPITAL LETTER EH
                new_char[0]  = 0xd5;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xb8)) {               // ARMENIAN CAPITAL LETTER ET
                new_char[0]  = 0xd5;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xb9)) {               // ARMENIAN CAPITAL LETTER TO
                new_char[0]  = 0xd5;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xba)) {               // ARMENIAN CAPITAL LETTER ZHE
                new_char[0]  = 0xd5;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xbb)) {               // ARMENIAN CAPITAL LETTER INI
                new_char[0]  = 0xd5;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xbc)) {               // ARMENIAN CAPITAL LETTER LIWN
                new_char[0]  = 0xd5;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xbd)) {               // ARMENIAN CAPITAL LETTER XEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xbe)) {               // ARMENIAN CAPITAL LETTER CA
                new_char[0]  = 0xd5;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xbf)) {               // ARMENIAN CAPITAL LETTER KEN
                new_char[0]  = 0xd5;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x80)) {               // ARMENIAN CAPITAL LETTER HO
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x81)) {               // ARMENIAN CAPITAL LETTER JA
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x82)) {               // ARMENIAN CAPITAL LETTER GHAD
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x83)) {               // ARMENIAN CAPITAL LETTER CHEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x84)) {               // ARMENIAN CAPITAL LETTER MEN
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x85)) {               // ARMENIAN CAPITAL LETTER YI
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x86)) {               // ARMENIAN CAPITAL LETTER NOW
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x87)) {               // ARMENIAN CAPITAL LETTER SHA
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x88)) {               // ARMENIAN CAPITAL LETTER VO
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x89)) {               // ARMENIAN CAPITAL LETTER CHA
                new_char[0]  = 0xd5;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x8a)) {               // ARMENIAN CAPITAL LETTER PEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x8b)) {               // ARMENIAN CAPITAL LETTER JHEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x8c)) {               // ARMENIAN CAPITAL LETTER RA
                new_char[0]  = 0xd5;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x8d)) {               // ARMENIAN CAPITAL LETTER SEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x8e)) {               // ARMENIAN CAPITAL LETTER VEW
                new_char[0]  = 0xd5;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x8f)) {               // ARMENIAN CAPITAL LETTER TIWN
                new_char[0]  = 0xd5;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x90)) {               // ARMENIAN CAPITAL LETTER REH
                new_char[0]  = 0xd6;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x91)) {               // ARMENIAN CAPITAL LETTER CO
                new_char[0]  = 0xd6;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x92)) {               // ARMENIAN CAPITAL LETTER YIWN
                new_char[0]  = 0xd6;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x93)) {               // ARMENIAN CAPITAL LETTER PIWR
                new_char[0]  = 0xd6;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x94)) {               // ARMENIAN CAPITAL LETTER KEH
                new_char[0]  = 0xd6;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x95)) {               // ARMENIAN CAPITAL LETTER OH
                new_char[0]  = 0xd6;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0x96)) {               // ARMENIAN CAPITAL LETTER FEH
                new_char[0]  = 0xd6;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa0)) {               // GEORGIAN CAPITAL LETTER AN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa1)) {               // GEORGIAN CAPITAL LETTER BAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa2)) {               // GEORGIAN CAPITAL LETTER GAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa3)) {               // GEORGIAN CAPITAL LETTER DON
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa4)) {               // GEORGIAN CAPITAL LETTER EN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa5)) {               // GEORGIAN CAPITAL LETTER VIN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa6)) {               // GEORGIAN CAPITAL LETTER ZEN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa7)) {               // GEORGIAN CAPITAL LETTER TAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa8)) {               // GEORGIAN CAPITAL LETTER IN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xa9)) {               // GEORGIAN CAPITAL LETTER KAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xaa)) {               // GEORGIAN CAPITAL LETTER LAS
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xab)) {               // GEORGIAN CAPITAL LETTER MAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xac)) {               // GEORGIAN CAPITAL LETTER NAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xad)) {               // GEORGIAN CAPITAL LETTER ON
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xae)) {               // GEORGIAN CAPITAL LETTER PAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xaf)) {               // GEORGIAN CAPITAL LETTER ZHAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb0)) {               // GEORGIAN CAPITAL LETTER RAE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb1)) {               // GEORGIAN CAPITAL LETTER SAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb2)) {               // GEORGIAN CAPITAL LETTER TAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb3)) {               // GEORGIAN CAPITAL LETTER UN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb4)) {               // GEORGIAN CAPITAL LETTER PHAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb5)) {               // GEORGIAN CAPITAL LETTER KHAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb6)) {               // GEORGIAN CAPITAL LETTER GHAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb7)) {               // GEORGIAN CAPITAL LETTER QAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb8)) {               // GEORGIAN CAPITAL LETTER SHIN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xb9)) {               // GEORGIAN CAPITAL LETTER CHIN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xba)) {               // GEORGIAN CAPITAL LETTER CAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xbb)) {               // GEORGIAN CAPITAL LETTER JIL
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xbc)) {               // GEORGIAN CAPITAL LETTER CIL
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xbd)) {               // GEORGIAN CAPITAL LETTER CHAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xbe)) {               // GEORGIAN CAPITAL LETTER XAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x82) && (old_char[2] == 0xbf)) {               // GEORGIAN CAPITAL LETTER JHAN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x83) && (old_char[2] == 0x80)) {               // GEORGIAN CAPITAL LETTER HAE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x83) && (old_char[2] == 0x81)) {               // GEORGIAN CAPITAL LETTER HE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x83) && (old_char[2] == 0x82)) {               // GEORGIAN CAPITAL LETTER HIE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x83) && (old_char[2] == 0x83)) {               // GEORGIAN CAPITAL LETTER WE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x83) && (old_char[2] == 0x84)) {               // GEORGIAN CAPITAL LETTER HAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x83) && (old_char[2] == 0x85)) {               // GEORGIAN CAPITAL LETTER HOE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x83) && (old_char[2] == 0x87)) {               // GEORGIAN CAPITAL LETTER YN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0x83) && (old_char[2] == 0x8d)) {               // GEORGIAN CAPITAL LETTER AEN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb4;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x80)) {               // LATIN CAPITAL LETTER A WITH RING BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x82)) {               // LATIN CAPITAL LETTER B WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x84)) {               // LATIN CAPITAL LETTER B WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x86)) {               // LATIN CAPITAL LETTER B WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x88)) {               // LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x8a)) {               // LATIN CAPITAL LETTER D WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x8c)) {               // LATIN CAPITAL LETTER D WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x8e)) {               // LATIN CAPITAL LETTER D WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x90)) {               // LATIN CAPITAL LETTER D WITH CEDILLA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x92)) {               // LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x94)) {               // LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x96)) {               // LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x98)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x9a)) {               // LATIN CAPITAL LETTER E WITH TILDE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x9c)) {               // LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x9e)) {               // LATIN CAPITAL LETTER F WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa0)) {               // LATIN CAPITAL LETTER G WITH MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa2)) {               // LATIN CAPITAL LETTER H WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa4)) {               // LATIN CAPITAL LETTER H WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa6)) {               // LATIN CAPITAL LETTER H WITH DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa8)) {               // LATIN CAPITAL LETTER H WITH CEDILLA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xaa)) {               // LATIN CAPITAL LETTER H WITH BREVE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xac)) {               // LATIN CAPITAL LETTER I WITH TILDE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xae)) {               // LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb0)) {               // LATIN CAPITAL LETTER K WITH ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb2)) {               // LATIN CAPITAL LETTER K WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb4)) {               // LATIN CAPITAL LETTER K WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb6)) {               // LATIN CAPITAL LETTER L WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb8)) {               // LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xba)) {               // LATIN CAPITAL LETTER L WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xbc)) {               // LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xbe)) {               // LATIN CAPITAL LETTER M WITH ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x80)) {               // LATIN CAPITAL LETTER M WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x82)) {               // LATIN CAPITAL LETTER M WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x84)) {               // LATIN CAPITAL LETTER N WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x86)) {               // LATIN CAPITAL LETTER N WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x88)) {               // LATIN CAPITAL LETTER N WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x8a)) {               // LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x8c)) {               // LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x8e)) {               // LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x90)) {               // LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x92)) {               // LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x94)) {               // LATIN CAPITAL LETTER P WITH ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x96)) {               // LATIN CAPITAL LETTER P WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x98)) {               // LATIN CAPITAL LETTER R WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x9a)) {               // LATIN CAPITAL LETTER R WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x9c)) {               // LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x9e)) {               // LATIN CAPITAL LETTER R WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa0)) {               // LATIN CAPITAL LETTER S WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa2)) {               // LATIN CAPITAL LETTER S WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa4)) {               // LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa6)) {               // LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa8)) {               // LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xaa)) {               // LATIN CAPITAL LETTER T WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xac)) {               // LATIN CAPITAL LETTER T WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xae)) {               // LATIN CAPITAL LETTER T WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb0)) {               // LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb2)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb4)) {               // LATIN CAPITAL LETTER U WITH TILDE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb6)) {               // LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb8)) {               // LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xba)) {               // LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xbc)) {               // LATIN CAPITAL LETTER V WITH TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xbe)) {               // LATIN CAPITAL LETTER V WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x80)) {               // LATIN CAPITAL LETTER W WITH GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x82)) {               // LATIN CAPITAL LETTER W WITH ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x84)) {               // LATIN CAPITAL LETTER W WITH DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x86)) {               // LATIN CAPITAL LETTER W WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x88)) {               // LATIN CAPITAL LETTER W WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x8a)) {               // LATIN CAPITAL LETTER X WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x8c)) {               // LATIN CAPITAL LETTER X WITH DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x8e)) {               // LATIN CAPITAL LETTER Y WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x90)) {               // LATIN CAPITAL LETTER Z WITH CIRCUMFLEX
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x92)) {               // LATIN CAPITAL LETTER Z WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x94)) {               // LATIN CAPITAL LETTER Z WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x9b)) {               // LATIN SMALL LETTER LONG S WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa0)) {               // LATIN CAPITAL LETTER A WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa2)) {               // LATIN CAPITAL LETTER A WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa4)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa6)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa8)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xaa)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xac)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xae)) {               // LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb0)) {               // LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb2)) {               // LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb4)) {               // LATIN CAPITAL LETTER A WITH BREVE AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb6)) {               // LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb8)) {               // LATIN CAPITAL LETTER E WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xba)) {               // LATIN CAPITAL LETTER E WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xbc)) {               // LATIN CAPITAL LETTER E WITH TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xbe)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x80)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x82)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x84)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x86)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x88)) {               // LATIN CAPITAL LETTER I WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x8a)) {               // LATIN CAPITAL LETTER I WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x8c)) {               // LATIN CAPITAL LETTER O WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x8e)) {               // LATIN CAPITAL LETTER O WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x90)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x92)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x94)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x96)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x98)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x9a)) {               // LATIN CAPITAL LETTER O WITH HORN AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x9c)) {               // LATIN CAPITAL LETTER O WITH HORN AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x9e)) {               // LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa0)) {               // LATIN CAPITAL LETTER O WITH HORN AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa2)) {               // LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa4)) {               // LATIN CAPITAL LETTER U WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa6)) {               // LATIN CAPITAL LETTER U WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa8)) {               // LATIN CAPITAL LETTER U WITH HORN AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xaa)) {               // LATIN CAPITAL LETTER U WITH HORN AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xac)) {               // LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xae)) {               // LATIN CAPITAL LETTER U WITH HORN AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb0)) {               // LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb2)) {               // LATIN CAPITAL LETTER Y WITH GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb4)) {               // LATIN CAPITAL LETTER Y WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb6)) {               // LATIN CAPITAL LETTER Y WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb8)) {               // LATIN CAPITAL LETTER Y WITH TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xba)) {               // LATIN CAPITAL LETTER MIDDLE-WELSH LL
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xbc)) {               // LATIN CAPITAL LETTER MIDDLE-WELSH V
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xbe)) {               // LATIN CAPITAL LETTER Y WITH LOOP
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x88)) {               // GREEK CAPITAL LETTER ALPHA WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x89)) {               // GREEK CAPITAL LETTER ALPHA WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x8a)) {               // GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x8b)) {               // GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x8c)) {               // GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x8d)) {               // GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x8e)) {               // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x8f)) {               // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x98)) {               // GREEK CAPITAL LETTER EPSILON WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x99)) {               // GREEK CAPITAL LETTER EPSILON WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x9a)) {               // GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x9b)) {               // GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x9c)) {               // GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x9d)) {               // GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa8)) {               // GREEK CAPITAL LETTER ETA WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa9)) {               // GREEK CAPITAL LETTER ETA WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xaa)) {               // GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xab)) {               // GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xac)) {               // GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xad)) {               // GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xae)) {               // GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xaf)) {               // GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb8)) {               // GREEK CAPITAL LETTER IOTA WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb9)) {               // GREEK CAPITAL LETTER IOTA WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xba)) {               // GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xbb)) {               // GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xbc)) {               // GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xbd)) {               // GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xbe)) {               // GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xbf)) {               // GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x88)) {               // GREEK CAPITAL LETTER OMICRON WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x89)) {               // GREEK CAPITAL LETTER OMICRON WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x8a)) {               // GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x8b)) {               // GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x8c)) {               // GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x8d)) {               // GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x99)) {               // GREEK CAPITAL LETTER UPSILON WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x9b)) {               // GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x9d)) {               // GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x9f)) {               // GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa8)) {               // GREEK CAPITAL LETTER OMEGA WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa9)) {               // GREEK CAPITAL LETTER OMEGA WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xaa)) {               // GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xab)) {               // GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xac)) {               // GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xad)) {               // GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xae)) {               // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xaf)) {               // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xb8)) {               // GREEK CAPITAL LETTER ALPHA WITH VRACHY
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xb9)) {               // GREEK CAPITAL LETTER ALPHA WITH MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xba)) {               // GREEK CAPITAL LETTER ALPHA WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xbb)) {               // GREEK CAPITAL LETTER ALPHA WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xbe)) {               // GREEK PROSGEGRAMMENI
                new_char[0]  = 0xce;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x88)) {               // GREEK CAPITAL LETTER EPSILON WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x89)) {               // GREEK CAPITAL LETTER EPSILON WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x8a)) {               // GREEK CAPITAL LETTER ETA WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x8b)) {               // GREEK CAPITAL LETTER ETA WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x98)) {               // GREEK CAPITAL LETTER IOTA WITH VRACHY
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x99)) {               // GREEK CAPITAL LETTER IOTA WITH MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x9a)) {               // GREEK CAPITAL LETTER IOTA WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x9b)) {               // GREEK CAPITAL LETTER IOTA WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xa8)) {               // GREEK CAPITAL LETTER UPSILON WITH VRACHY
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xa9)) {               // GREEK CAPITAL LETTER UPSILON WITH MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xaa)) {               // GREEK CAPITAL LETTER UPSILON WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xab)) {               // GREEK CAPITAL LETTER UPSILON WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xac)) {               // GREEK CAPITAL LETTER RHO WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xb8)) {               // GREEK CAPITAL LETTER OMICRON WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xb9)) {               // GREEK CAPITAL LETTER OMICRON WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xba)) {               // GREEK CAPITAL LETTER OMEGA WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xbb)) {               // GREEK CAPITAL LETTER OMEGA WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x84) && (old_char[2] == 0xa6)) {               // OHM SIGN
                new_char[0]  = 0xcf;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x84) && (old_char[2] == 0xaa)) {               // KELVIN SIGN
                new_char[0]  = 0x6b;
                new_char_len = 1;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x84) && (old_char[2] == 0xab)) {               // ANGSTROM SIGN
                new_char[0]  = 0xc3;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x84) && (old_char[2] == 0xb2)) {               // TURNED CAPITAL F
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa0)) {               // ROMAN NUMERAL ONE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa1)) {               // ROMAN NUMERAL TWO
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa2)) {               // ROMAN NUMERAL THREE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa3)) {               // ROMAN NUMERAL FOUR
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa4)) {               // ROMAN NUMERAL FIVE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa5)) {               // ROMAN NUMERAL SIX
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa6)) {               // ROMAN NUMERAL SEVEN
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa7)) {               // ROMAN NUMERAL EIGHT
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa8)) {               // ROMAN NUMERAL NINE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xa9)) {               // ROMAN NUMERAL TEN
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xaa)) {               // ROMAN NUMERAL ELEVEN
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xab)) {               // ROMAN NUMERAL TWELVE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xac)) {               // ROMAN NUMERAL FIFTY
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xad)) {               // ROMAN NUMERAL ONE HUNDRED
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xae)) {               // ROMAN NUMERAL FIVE HUNDRED
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xaf)) {               // ROMAN NUMERAL ONE THOUSAND
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x86) && (old_char[2] == 0x83)) {               // ROMAN NUMERAL REVERSED ONE HUNDRED
                new_char[0]  = 0xe2;
                new_char[1]  = 0x86;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xb6)) {               // CIRCLED LATIN CAPITAL LETTER A
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xb7)) {               // CIRCLED LATIN CAPITAL LETTER B
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xb8)) {               // CIRCLED LATIN CAPITAL LETTER C
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xb9)) {               // CIRCLED LATIN CAPITAL LETTER D
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xba)) {               // CIRCLED LATIN CAPITAL LETTER E
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xbb)) {               // CIRCLED LATIN CAPITAL LETTER F
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xbc)) {               // CIRCLED LATIN CAPITAL LETTER G
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xbd)) {               // CIRCLED LATIN CAPITAL LETTER H
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xbe)) {               // CIRCLED LATIN CAPITAL LETTER I
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x92) && (old_char[2] == 0xbf)) {               // CIRCLED LATIN CAPITAL LETTER J
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x80)) {               // CIRCLED LATIN CAPITAL LETTER K
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x81)) {               // CIRCLED LATIN CAPITAL LETTER L
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x82)) {               // CIRCLED LATIN CAPITAL LETTER M
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x83)) {               // CIRCLED LATIN CAPITAL LETTER N
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x84)) {               // CIRCLED LATIN CAPITAL LETTER O
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x85)) {               // CIRCLED LATIN CAPITAL LETTER P
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x86)) {               // CIRCLED LATIN CAPITAL LETTER Q
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x87)) {               // CIRCLED LATIN CAPITAL LETTER R
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x88)) {               // CIRCLED LATIN CAPITAL LETTER S
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x89)) {               // CIRCLED LATIN CAPITAL LETTER T
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x8a)) {               // CIRCLED LATIN CAPITAL LETTER U
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x8b)) {               // CIRCLED LATIN CAPITAL LETTER V
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x8c)) {               // CIRCLED LATIN CAPITAL LETTER W
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x8d)) {               // CIRCLED LATIN CAPITAL LETTER X
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x8e)) {               // CIRCLED LATIN CAPITAL LETTER Y
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x8f)) {               // CIRCLED LATIN CAPITAL LETTER Z
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x80)) {               // GLAGOLITIC CAPITAL LETTER AZU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x81)) {               // GLAGOLITIC CAPITAL LETTER BUKY
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x82)) {               // GLAGOLITIC CAPITAL LETTER VEDE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x83)) {               // GLAGOLITIC CAPITAL LETTER GLAGOLI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x84)) {               // GLAGOLITIC CAPITAL LETTER DOBRO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x85)) {               // GLAGOLITIC CAPITAL LETTER YESTU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x86)) {               // GLAGOLITIC CAPITAL LETTER ZHIVETE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x87)) {               // GLAGOLITIC CAPITAL LETTER DZELO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x88)) {               // GLAGOLITIC CAPITAL LETTER ZEMLJA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x89)) {               // GLAGOLITIC CAPITAL LETTER IZHE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x8a)) {               // GLAGOLITIC CAPITAL LETTER INITIAL IZHE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x8b)) {               // GLAGOLITIC CAPITAL LETTER I
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x8c)) {               // GLAGOLITIC CAPITAL LETTER DJERVI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x8d)) {               // GLAGOLITIC CAPITAL LETTER KAKO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x8e)) {               // GLAGOLITIC CAPITAL LETTER LJUDIJE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x8f)) {               // GLAGOLITIC CAPITAL LETTER MYSLITE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x90)) {               // GLAGOLITIC CAPITAL LETTER NASHI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x91)) {               // GLAGOLITIC CAPITAL LETTER ONU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x92)) {               // GLAGOLITIC CAPITAL LETTER POKOJI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x93)) {               // GLAGOLITIC CAPITAL LETTER RITSI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x94)) {               // GLAGOLITIC CAPITAL LETTER SLOVO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x95)) {               // GLAGOLITIC CAPITAL LETTER TVRIDO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x96)) {               // GLAGOLITIC CAPITAL LETTER UKU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x97)) {               // GLAGOLITIC CAPITAL LETTER FRITU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x98)) {               // GLAGOLITIC CAPITAL LETTER HERU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x99)) {               // GLAGOLITIC CAPITAL LETTER OTU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x9a)) {               // GLAGOLITIC CAPITAL LETTER PE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x9b)) {               // GLAGOLITIC CAPITAL LETTER SHTA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x9c)) {               // GLAGOLITIC CAPITAL LETTER TSI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x9d)) {               // GLAGOLITIC CAPITAL LETTER CHRIVI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x9e)) {               // GLAGOLITIC CAPITAL LETTER SHA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0x9f)) {               // GLAGOLITIC CAPITAL LETTER YERU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa0)) {               // GLAGOLITIC CAPITAL LETTER YERI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa1)) {               // GLAGOLITIC CAPITAL LETTER YATI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa2)) {               // GLAGOLITIC CAPITAL LETTER SPIDERY HA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa3)) {               // GLAGOLITIC CAPITAL LETTER YU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa4)) {               // GLAGOLITIC CAPITAL LETTER SMALL YUS
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa5)) {               // GLAGOLITIC CAPITAL LETTER SMALL YUS WITH TAIL
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa6)) {               // GLAGOLITIC CAPITAL LETTER YO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa7)) {               // GLAGOLITIC CAPITAL LETTER IOTATED SMALL YUS
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa8)) {               // GLAGOLITIC CAPITAL LETTER BIG YUS
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xa9)) {               // GLAGOLITIC CAPITAL LETTER IOTATED BIG YUS
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xaa)) {               // GLAGOLITIC CAPITAL LETTER FITA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xab)) {               // GLAGOLITIC CAPITAL LETTER IZHITSA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xac)) {               // GLAGOLITIC CAPITAL LETTER SHTAPIC
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xad)) {               // GLAGOLITIC CAPITAL LETTER TROKUTASTI A
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xae)) {               // GLAGOLITIC CAPITAL LETTER LATINATE MYSLITE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa0)) {               // LATIN CAPITAL LETTER L WITH DOUBLE BAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa2)) {               // LATIN CAPITAL LETTER L WITH MIDDLE TILDE
                new_char[0]  = 0xc9;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa3)) {               // LATIN CAPITAL LETTER P WITH STROKE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb5;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa4)) {               // LATIN CAPITAL LETTER R WITH TAIL
                new_char[0]  = 0xc9;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa7)) {               // LATIN CAPITAL LETTER H WITH DESCENDER
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa9)) {               // LATIN CAPITAL LETTER K WITH DESCENDER
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xab)) {               // LATIN CAPITAL LETTER Z WITH DESCENDER
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xad)) {               // LATIN CAPITAL LETTER ALPHA
                new_char[0]  = 0xc9;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xae)) {               // LATIN CAPITAL LETTER M WITH HOOK
                new_char[0]  = 0xc9;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xaf)) {               // LATIN CAPITAL LETTER TURNED A
                new_char[0]  = 0xc9;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xb0)) {               // LATIN CAPITAL LETTER TURNED ALPHA
                new_char[0]  = 0xc9;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xb2)) {               // LATIN CAPITAL LETTER W WITH HOOK
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xb5)) {               // LATIN CAPITAL LETTER HALF H
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xbe)) {               // LATIN CAPITAL LETTER S WITH SWASH TAIL
                new_char[0]  = 0xc8;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xbf)) {               // LATIN CAPITAL LETTER Z WITH SWASH TAIL
                new_char[0]  = 0xc9;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x80)) {               // COPTIC CAPITAL LETTER ALFA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x82)) {               // COPTIC CAPITAL LETTER VIDA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x84)) {               // COPTIC CAPITAL LETTER GAMMA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x86)) {               // COPTIC CAPITAL LETTER DALDA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x88)) {               // COPTIC CAPITAL LETTER EIE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x8a)) {               // COPTIC CAPITAL LETTER SOU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x8c)) {               // COPTIC CAPITAL LETTER ZATA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x8e)) {               // COPTIC CAPITAL LETTER HATE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x90)) {               // COPTIC CAPITAL LETTER THETHE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x92)) {               // COPTIC CAPITAL LETTER IAUDA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x94)) {               // COPTIC CAPITAL LETTER KAPA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x96)) {               // COPTIC CAPITAL LETTER LAULA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x98)) {               // COPTIC CAPITAL LETTER MI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x9a)) {               // COPTIC CAPITAL LETTER NI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x9c)) {               // COPTIC CAPITAL LETTER KSI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x9e)) {               // COPTIC CAPITAL LETTER O
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa0)) {               // COPTIC CAPITAL LETTER PI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa2)) {               // COPTIC CAPITAL LETTER RO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa4)) {               // COPTIC CAPITAL LETTER SIMA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa6)) {               // COPTIC CAPITAL LETTER TAU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa8)) {               // COPTIC CAPITAL LETTER UA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xaa)) {               // COPTIC CAPITAL LETTER FI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xac)) {               // COPTIC CAPITAL LETTER KHI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xae)) {               // COPTIC CAPITAL LETTER PSI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb0)) {               // COPTIC CAPITAL LETTER OOU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb2)) {               // COPTIC CAPITAL LETTER DIALECT-P ALEF
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb4)) {               // COPTIC CAPITAL LETTER OLD COPTIC AIN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb6)) {               // COPTIC CAPITAL LETTER CRYPTOGRAMMIC EIE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb8)) {               // COPTIC CAPITAL LETTER DIALECT-P KAPA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xba)) {               // COPTIC CAPITAL LETTER DIALECT-P NI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xbc)) {               // COPTIC CAPITAL LETTER CRYPTOGRAMMIC NI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xbe)) {               // COPTIC CAPITAL LETTER OLD COPTIC OOU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x80)) {               // COPTIC CAPITAL LETTER SAMPI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x82)) {               // COPTIC CAPITAL LETTER CROSSED SHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x84)) {               // COPTIC CAPITAL LETTER OLD COPTIC SHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x86)) {               // COPTIC CAPITAL LETTER OLD COPTIC ESH
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x88)) {               // COPTIC CAPITAL LETTER AKHMIMIC KHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x8a)) {               // COPTIC CAPITAL LETTER DIALECT-P HORI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x8c)) {               // COPTIC CAPITAL LETTER OLD COPTIC HORI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x8e)) {               // COPTIC CAPITAL LETTER OLD COPTIC HA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x90)) {               // COPTIC CAPITAL LETTER L-SHAPED HA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x92)) {               // COPTIC CAPITAL LETTER OLD COPTIC HEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x94)) {               // COPTIC CAPITAL LETTER OLD COPTIC HAT
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x96)) {               // COPTIC CAPITAL LETTER OLD COPTIC GANGIA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x98)) {               // COPTIC CAPITAL LETTER OLD COPTIC DJA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x9a)) {               // COPTIC CAPITAL LETTER OLD COPTIC SHIMA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x9c)) {               // COPTIC CAPITAL LETTER OLD NUBIAN SHIMA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x9e)) {               // COPTIC CAPITAL LETTER OLD NUBIAN NGI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xa0)) {               // COPTIC CAPITAL LETTER OLD NUBIAN NYI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xa2)) {               // COPTIC CAPITAL LETTER OLD NUBIAN WAU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xab)) {               // COPTIC CAPITAL LETTER CRYPTOGRAMMIC SHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xad)) {               // COPTIC CAPITAL LETTER CRYPTOGRAMMIC GANGIA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xb2)) {               // COPTIC CAPITAL LETTER BOHAIRIC KHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x80)) {               // CYRILLIC CAPITAL LETTER ZEMLYA
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x82)) {               // CYRILLIC CAPITAL LETTER DZELO
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x84)) {               // CYRILLIC CAPITAL LETTER REVERSED DZE
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x86)) {               // CYRILLIC CAPITAL LETTER IOTA
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x88)) {               // CYRILLIC CAPITAL LETTER DJERV
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x8a)) {               // CYRILLIC CAPITAL LETTER MONOGRAPH UK
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x8c)) {               // CYRILLIC CAPITAL LETTER BROAD OMEGA
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x8e)) {               // CYRILLIC CAPITAL LETTER NEUTRAL YER
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x90)) {               // CYRILLIC CAPITAL LETTER YERU WITH BACK YER
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x92)) {               // CYRILLIC CAPITAL LETTER IOTIFIED YAT
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x94)) {               // CYRILLIC CAPITAL LETTER REVERSED YU
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x96)) {               // CYRILLIC CAPITAL LETTER IOTIFIED A
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x98)) {               // CYRILLIC CAPITAL LETTER CLOSED LITTLE YUS
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x9a)) {               // CYRILLIC CAPITAL LETTER BLENDED YUS
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x9c)) {               // CYRILLIC CAPITAL LETTER IOTIFIED CLOSED LITTLE YUS
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x9e)) {               // CYRILLIC CAPITAL LETTER YN
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa0)) {               // CYRILLIC CAPITAL LETTER REVERSED TSE
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa2)) {               // CYRILLIC CAPITAL LETTER SOFT DE
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa4)) {               // CYRILLIC CAPITAL LETTER SOFT EL
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa6)) {               // CYRILLIC CAPITAL LETTER SOFT EM
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa8)) {               // CYRILLIC CAPITAL LETTER MONOCULAR O
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xaa)) {               // CYRILLIC CAPITAL LETTER BINOCULAR O
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xac)) {               // CYRILLIC CAPITAL LETTER DOUBLE MONOCULAR O
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x80)) {               // CYRILLIC CAPITAL LETTER DWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x82)) {               // CYRILLIC CAPITAL LETTER DZWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x84)) {               // CYRILLIC CAPITAL LETTER ZHWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x86)) {               // CYRILLIC CAPITAL LETTER CCHE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x88)) {               // CYRILLIC CAPITAL LETTER DZZE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x8a)) {               // CYRILLIC CAPITAL LETTER TE WITH MIDDLE HOOK
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x8c)) {               // CYRILLIC CAPITAL LETTER TWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x8e)) {               // CYRILLIC CAPITAL LETTER TSWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x90)) {               // CYRILLIC CAPITAL LETTER TSSE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x92)) {               // CYRILLIC CAPITAL LETTER TCHE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x94)) {               // CYRILLIC CAPITAL LETTER HWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x96)) {               // CYRILLIC CAPITAL LETTER SHWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xa2)) {               // LATIN CAPITAL LETTER EGYPTOLOGICAL ALEF
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xa4)) {               // LATIN CAPITAL LETTER EGYPTOLOGICAL AIN
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xa6)) {               // LATIN CAPITAL LETTER HENG
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xa8)) {               // LATIN CAPITAL LETTER TZ
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xaa)) {               // LATIN CAPITAL LETTER TRESILLO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xac)) {               // LATIN CAPITAL LETTER CUATRILLO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xae)) {               // LATIN CAPITAL LETTER CUATRILLO WITH COMMA
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xb2)) {               // LATIN CAPITAL LETTER AA
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xb4)) {               // LATIN CAPITAL LETTER AO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xb6)) {               // LATIN CAPITAL LETTER AU
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xb8)) {               // LATIN CAPITAL LETTER AV
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xba)) {               // LATIN CAPITAL LETTER AV WITH HORIZONTAL BAR
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xbc)) {               // LATIN CAPITAL LETTER AY
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xbe)) {               // LATIN CAPITAL LETTER REVERSED C WITH DOT
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x80)) {               // LATIN CAPITAL LETTER K WITH STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x82)) {               // LATIN CAPITAL LETTER K WITH DIAGONAL STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x84)) {               // LATIN CAPITAL LETTER K WITH STROKE AND DIAGONAL STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x86)) {               // LATIN CAPITAL LETTER BROKEN L
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x88)) {               // LATIN CAPITAL LETTER L WITH HIGH STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x8a)) {               // LATIN CAPITAL LETTER O WITH LONG STROKE OVERLAY
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x8c)) {               // LATIN CAPITAL LETTER O WITH LOOP
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x8e)) {               // LATIN CAPITAL LETTER OO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x90)) {               // LATIN CAPITAL LETTER P WITH STROKE THROUGH DESCENDER
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x92)) {               // LATIN CAPITAL LETTER P WITH FLOURISH
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x94)) {               // LATIN CAPITAL LETTER P WITH SQUIRREL TAIL
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x96)) {               // LATIN CAPITAL LETTER Q WITH STROKE THROUGH DESCENDER
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x98)) {               // LATIN CAPITAL LETTER Q WITH DIAGONAL STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x9a)) {               // LATIN CAPITAL LETTER R ROTUNDA
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x9c)) {               // LATIN CAPITAL LETTER RUM ROTUNDA
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x9e)) {               // LATIN CAPITAL LETTER V WITH DIAGONAL STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa0)) {               // LATIN CAPITAL LETTER VY
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa2)) {               // LATIN CAPITAL LETTER VISIGOTHIC Z
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa4)) {               // LATIN CAPITAL LETTER THORN WITH STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa6)) {               // LATIN CAPITAL LETTER THORN WITH STROKE THROUGH DESCENDER
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa8)) {               // LATIN CAPITAL LETTER VEND
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xaa)) {               // LATIN CAPITAL LETTER ET
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xac)) {               // LATIN CAPITAL LETTER IS
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xae)) {               // LATIN CAPITAL LETTER CON
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xb9)) {               // LATIN CAPITAL LETTER INSULAR D
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xbb)) {               // LATIN CAPITAL LETTER INSULAR F
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xbd)) {               // LATIN CAPITAL LETTER INSULAR G
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb5;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xbe)) {               // LATIN CAPITAL LETTER TURNED INSULAR G
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x80)) {               // LATIN CAPITAL LETTER TURNED L
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x82)) {               // LATIN CAPITAL LETTER INSULAR R
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x84)) {               // LATIN CAPITAL LETTER INSULAR S
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x86)) {               // LATIN CAPITAL LETTER INSULAR T
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x8b)) {               // LATIN CAPITAL LETTER SALTILLO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x8d)) {               // LATIN CAPITAL LETTER TURNED H
                new_char[0]  = 0xc9;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x90)) {               // LATIN CAPITAL LETTER N WITH DESCENDER
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x92)) {               // LATIN CAPITAL LETTER C WITH BAR
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa0)) {               // LATIN CAPITAL LETTER G WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa2)) {               // LATIN CAPITAL LETTER K WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa4)) {               // LATIN CAPITAL LETTER N WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa6)) {               // LATIN CAPITAL LETTER R WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa8)) {               // LATIN CAPITAL LETTER S WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xaa)) {               // LATIN CAPITAL LETTER H WITH HOOK
                new_char[0]  = 0xc9;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xa1)) {               // FULLWIDTH LATIN CAPITAL LETTER A
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xa2)) {               // FULLWIDTH LATIN CAPITAL LETTER B
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xa3)) {               // FULLWIDTH LATIN CAPITAL LETTER C
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xa4)) {               // FULLWIDTH LATIN CAPITAL LETTER D
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xa5)) {               // FULLWIDTH LATIN CAPITAL LETTER E
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xa6)) {               // FULLWIDTH LATIN CAPITAL LETTER F
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xa7)) {               // FULLWIDTH LATIN CAPITAL LETTER G
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xa8)) {               // FULLWIDTH LATIN CAPITAL LETTER H
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xa9)) {               // FULLWIDTH LATIN CAPITAL LETTER I
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xaa)) {               // FULLWIDTH LATIN CAPITAL LETTER J
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xab)) {               // FULLWIDTH LATIN CAPITAL LETTER K
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xac)) {               // FULLWIDTH LATIN CAPITAL LETTER L
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xad)) {               // FULLWIDTH LATIN CAPITAL LETTER M
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xae)) {               // FULLWIDTH LATIN CAPITAL LETTER N
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xaf)) {               // FULLWIDTH LATIN CAPITAL LETTER O
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb0)) {               // FULLWIDTH LATIN CAPITAL LETTER P
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb1)) {               // FULLWIDTH LATIN CAPITAL LETTER Q
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb2)) {               // FULLWIDTH LATIN CAPITAL LETTER R
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb3)) {               // FULLWIDTH LATIN CAPITAL LETTER S
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb4)) {               // FULLWIDTH LATIN CAPITAL LETTER T
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb5)) {               // FULLWIDTH LATIN CAPITAL LETTER U
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb6)) {               // FULLWIDTH LATIN CAPITAL LETTER V
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb7)) {               // FULLWIDTH LATIN CAPITAL LETTER W
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb8)) {               // FULLWIDTH LATIN CAPITAL LETTER X
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xb9)) {               // FULLWIDTH LATIN CAPITAL LETTER Y
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbc) && (old_char[2] == 0xba)) {               // FULLWIDTH LATIN CAPITAL LETTER Z
                new_char[0]  = 0xef;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x80)) {               // DESERET CAPITAL LETTER LONG I
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa8;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x81)) {               // DESERET CAPITAL LETTER LONG E
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa9;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x82)) {               // DESERET CAPITAL LETTER LONG A
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xaa;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x83)) {               // DESERET CAPITAL LETTER LONG AH
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xab;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x84)) {               // DESERET CAPITAL LETTER LONG O
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xac;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x85)) {               // DESERET CAPITAL LETTER LONG OO
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xad;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x86)) {               // DESERET CAPITAL LETTER SHORT I
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xae;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x87)) {               // DESERET CAPITAL LETTER SHORT E
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xaf;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x88)) {               // DESERET CAPITAL LETTER SHORT A
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb0;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x89)) {               // DESERET CAPITAL LETTER SHORT AH
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb1;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x8a)) {               // DESERET CAPITAL LETTER SHORT O
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb2;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x8b)) {               // DESERET CAPITAL LETTER SHORT OO
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb3;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x8c)) {               // DESERET CAPITAL LETTER AY
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb4;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x8d)) {               // DESERET CAPITAL LETTER OW
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb5;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x8e)) {               // DESERET CAPITAL LETTER WU
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb6;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x8f)) {               // DESERET CAPITAL LETTER YEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb7;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x90)) {               // DESERET CAPITAL LETTER H
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb8;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x91)) {               // DESERET CAPITAL LETTER PEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xb9;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x92)) {               // DESERET CAPITAL LETTER BEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xba;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x93)) {               // DESERET CAPITAL LETTER TEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xbb;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x94)) {               // DESERET CAPITAL LETTER DEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xbc;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x95)) {               // DESERET CAPITAL LETTER CHEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xbd;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x96)) {               // DESERET CAPITAL LETTER JEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xbe;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x97)) {               // DESERET CAPITAL LETTER KAY
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xbf;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x98)) {               // DESERET CAPITAL LETTER GAY
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x80;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x99)) {               // DESERET CAPITAL LETTER EF
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x81;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x9a)) {               // DESERET CAPITAL LETTER VEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x82;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x9b)) {               // DESERET CAPITAL LETTER ETH
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x83;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x9c)) {               // DESERET CAPITAL LETTER THEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x84;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x9d)) {               // DESERET CAPITAL LETTER ES
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x85;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x9e)) {               // DESERET CAPITAL LETTER ZEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x86;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0x9f)) {               // DESERET CAPITAL LETTER ESH
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x87;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa0)) {               // DESERET CAPITAL LETTER ZHEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x88;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa1)) {               // DESERET CAPITAL LETTER ER
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x89;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa2)) {               // DESERET CAPITAL LETTER EL
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x8a;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa3)) {               // DESERET CAPITAL LETTER EM
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x8b;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa4)) {               // DESERET CAPITAL LETTER EN
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x8c;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa5)) {               // DESERET CAPITAL LETTER ENG
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x8d;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa6)) {               // DESERET CAPITAL LETTER OI
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x8e;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa7)) {               // DESERET CAPITAL LETTER EW
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x91;
                new_char[3]  = 0x8f;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x9e)) {                // LATIN CAPITAL LETTER SHARP S
                new_char[0]  = 0xc3;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x88)) {                // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x89)) {                // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x8a)) {                // GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x8b)) {                // GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x8c)) {                // GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x8d)) {                // GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x8e)) {                // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x8f)) {                // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x98)) {                // GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x99)) {                // GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x9a)) {                // GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x9b)) {                // GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x9c)) {                // GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x9d)) {                // GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x9e)) {                // GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x9f)) {                // GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa8)) {                // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa9)) {                // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xaa)) {                // GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xab)) {                // GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xac)) {                // GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xad)) {                // GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xae)) {                // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xaf)) {                // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xbc)) {                // GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x8c)) {                // GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xbc)) {                // GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            }
        }

        if (!old_char_len) {
            int pre_offs = offs;
            //(isutf(str[++offs]) && (length--)) || (isutf(str[++offs]) && (length--)) || (isutf(str[++offs]) && (length--)) || ((++offs) && (length--));
            (void)(isutf(str[++offs]) || isutf(str[++offs]) ||
                   isutf(str[++offs]) || ++offs);

            old_char_len = offs - pre_offs;

            /*if (old_char_len==1) {
                new_char[0]=tolower(str[pre_offs]);
                new_char_len=1;
               } else {*/
            for (int j = 0; j < old_char_len; j++)
                new_char[j] = str[pre_offs + j];

            new_char_len = old_char_len;
            //}
        } else
            offs += old_char_len;

        for (int j = 0; j < new_char_len; j++)
            out[out_offset++] = new_char[j];
        length -= old_char_len;
    }
    out[out_offset] = 0;
    return out_offset;
}

//---------------------------------------------------------------------------
int toUpper(char *str, int length, char *out, bool turkic = false) {
    int offs       = 0;
    int out_offset = 0;

    while (length > 0) {
        short new_char[4] = { -1, -1, -1, -1 };
        short old_char[4] = { -1, -1, -1, -1 };
        int   m           = length < 4 ? length : 4;
        for (int i = 0; i < m; i++)
            old_char[i] = (unsigned char)str[offs + i];
        char new_char_len = 0;
        char old_char_len = 0;
        if (turkic) {
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xb1)) {               // LATIN CAPITAL LETTER I
                new_char[0]  = 0x49;
                new_char_len = 1;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0x69)) {               // LATIN CAPITAL LETTER I WITH DOT ABOVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 1;
            }
        }
        if (!old_char_len) {
            if ((old_char[0] == 0x61)) {               // LATIN CAPITAL LETTER A
                new_char[0]  = 0x41;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x62)) {               // LATIN CAPITAL LETTER B
                new_char[0]  = 0x42;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x63)) {               // LATIN CAPITAL LETTER C
                new_char[0]  = 0x43;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x64)) {               // LATIN CAPITAL LETTER D
                new_char[0]  = 0x44;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x65)) {               // LATIN CAPITAL LETTER E
                new_char[0]  = 0x45;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x66)) {               // LATIN CAPITAL LETTER F
                new_char[0]  = 0x46;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x67)) {               // LATIN CAPITAL LETTER G
                new_char[0]  = 0x47;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x68)) {               // LATIN CAPITAL LETTER H
                new_char[0]  = 0x48;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x69)) {               // LATIN CAPITAL LETTER I
                new_char[0]  = 0x49;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x6a)) {               // LATIN CAPITAL LETTER J
                new_char[0]  = 0x4a;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x6b)) {               // LATIN CAPITAL LETTER K
                new_char[0]  = 0x4b;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x6c)) {               // LATIN CAPITAL LETTER L
                new_char[0]  = 0x4c;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x6d)) {               // LATIN CAPITAL LETTER M
                new_char[0]  = 0x4d;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x6e)) {               // LATIN CAPITAL LETTER N
                new_char[0]  = 0x4e;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x6f)) {               // LATIN CAPITAL LETTER O
                new_char[0]  = 0x4f;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x70)) {               // LATIN CAPITAL LETTER P
                new_char[0]  = 0x50;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x71)) {               // LATIN CAPITAL LETTER Q
                new_char[0]  = 0x51;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x72)) {               // LATIN CAPITAL LETTER R
                new_char[0]  = 0x52;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x73)) {               // LATIN CAPITAL LETTER S
                new_char[0]  = 0x53;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x74)) {               // LATIN CAPITAL LETTER T
                new_char[0]  = 0x54;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x75)) {               // LATIN CAPITAL LETTER U
                new_char[0]  = 0x55;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x76)) {               // LATIN CAPITAL LETTER V
                new_char[0]  = 0x56;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x77)) {               // LATIN CAPITAL LETTER W
                new_char[0]  = 0x57;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x78)) {               // LATIN CAPITAL LETTER X
                new_char[0]  = 0x58;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x79)) {               // LATIN CAPITAL LETTER Y
                new_char[0]  = 0x59;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0x7a)) {               // LATIN CAPITAL LETTER Z
                new_char[0]  = 0x5a;
                new_char_len = 1;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xbc)) {               // MICRO SIGN
                new_char[0]  = 0xc2;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa0)) {               // LATIN CAPITAL LETTER A WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa1)) {               // LATIN CAPITAL LETTER A WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa2)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa3)) {               // LATIN CAPITAL LETTER A WITH TILDE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa4)) {               // LATIN CAPITAL LETTER A WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa5)) {               // LATIN CAPITAL LETTER A WITH RING ABOVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa6)) {               // LATIN CAPITAL LETTER AE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa7)) {               // LATIN CAPITAL LETTER C WITH CEDILLA
                new_char[0]  = 0xc3;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa8)) {               // LATIN CAPITAL LETTER E WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa9)) {               // LATIN CAPITAL LETTER E WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xaa)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xab)) {               // LATIN CAPITAL LETTER E WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xac)) {               // LATIN CAPITAL LETTER I WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xad)) {               // LATIN CAPITAL LETTER I WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xae)) {               // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xaf)) {               // LATIN CAPITAL LETTER I WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xb0)) {               // LATIN CAPITAL LETTER ETH
                new_char[0]  = 0xc3;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xb1)) {               // LATIN CAPITAL LETTER N WITH TILDE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xb2)) {               // LATIN CAPITAL LETTER O WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xb3)) {               // LATIN CAPITAL LETTER O WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xb4)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xb5)) {               // LATIN CAPITAL LETTER O WITH TILDE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xb6)) {               // LATIN CAPITAL LETTER O WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xb8)) {               // LATIN CAPITAL LETTER O WITH STROKE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xb9)) {               // LATIN CAPITAL LETTER U WITH GRAVE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xba)) {               // LATIN CAPITAL LETTER U WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xbb)) {               // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
                new_char[0]  = 0xc3;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xbc)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS
                new_char[0]  = 0xc3;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xbd)) {               // LATIN CAPITAL LETTER Y WITH ACUTE
                new_char[0]  = 0xc3;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xbe)) {               // LATIN CAPITAL LETTER THORN
                new_char[0]  = 0xc3;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x81)) {               // LATIN CAPITAL LETTER A WITH MACRON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x83)) {               // LATIN CAPITAL LETTER A WITH BREVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x85)) {               // LATIN CAPITAL LETTER A WITH OGONEK
                new_char[0]  = 0xc4;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x87)) {               // LATIN CAPITAL LETTER C WITH ACUTE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x89)) {               // LATIN CAPITAL LETTER C WITH CIRCUMFLEX
                new_char[0]  = 0xc4;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x8b)) {               // LATIN CAPITAL LETTER C WITH DOT ABOVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x8d)) {               // LATIN CAPITAL LETTER C WITH CARON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x8f)) {               // LATIN CAPITAL LETTER D WITH CARON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x91)) {               // LATIN CAPITAL LETTER D WITH STROKE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x93)) {               // LATIN CAPITAL LETTER E WITH MACRON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x95)) {               // LATIN CAPITAL LETTER E WITH BREVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x97)) {               // LATIN CAPITAL LETTER E WITH DOT ABOVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x99)) {               // LATIN CAPITAL LETTER E WITH OGONEK
                new_char[0]  = 0xc4;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x9b)) {               // LATIN CAPITAL LETTER E WITH CARON
                new_char[0]  = 0xc4;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x9d)) {               // LATIN CAPITAL LETTER G WITH CIRCUMFLEX
                new_char[0]  = 0xc4;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0x9f)) {               // LATIN CAPITAL LETTER G WITH BREVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa1)) {               // LATIN CAPITAL LETTER G WITH DOT ABOVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa3)) {               // LATIN CAPITAL LETTER G WITH CEDILLA
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa5)) {               // LATIN CAPITAL LETTER H WITH CIRCUMFLEX
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa7)) {               // LATIN CAPITAL LETTER H WITH STROKE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xa9)) {               // LATIN CAPITAL LETTER I WITH TILDE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xab)) {               // LATIN CAPITAL LETTER I WITH MACRON
                new_char[0]  = 0xc4;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xad)) {               // LATIN CAPITAL LETTER I WITH BREVE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xaf)) {               // LATIN CAPITAL LETTER I WITH OGONEK
                new_char[0]  = 0xc4;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xb3)) {               // LATIN CAPITAL LIGATURE IJ
                new_char[0]  = 0xc4;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xb5)) {               // LATIN CAPITAL LETTER J WITH CIRCUMFLEX
                new_char[0]  = 0xc4;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xb7)) {               // LATIN CAPITAL LETTER K WITH CEDILLA
                new_char[0]  = 0xc4;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xba)) {               // LATIN CAPITAL LETTER L WITH ACUTE
                new_char[0]  = 0xc4;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xbc)) {               // LATIN CAPITAL LETTER L WITH CEDILLA
                new_char[0]  = 0xc4;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc4) && (old_char[1] == 0xbe)) {               // LATIN CAPITAL LETTER L WITH CARON
                new_char[0]  = 0xc4;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x80)) {               // LATIN CAPITAL LETTER L WITH MIDDLE DOT
                new_char[0]  = 0xc4;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x82)) {               // LATIN CAPITAL LETTER L WITH STROKE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x84)) {               // LATIN CAPITAL LETTER N WITH ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x86)) {               // LATIN CAPITAL LETTER N WITH CEDILLA
                new_char[0]  = 0xc5;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x88)) {               // LATIN CAPITAL LETTER N WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x8b)) {               // LATIN CAPITAL LETTER ENG
                new_char[0]  = 0xc5;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x8d)) {               // LATIN CAPITAL LETTER O WITH MACRON
                new_char[0]  = 0xc5;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x8f)) {               // LATIN CAPITAL LETTER O WITH BREVE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x91)) {               // LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x93)) {               // LATIN CAPITAL LIGATURE OE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x95)) {               // LATIN CAPITAL LETTER R WITH ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x97)) {               // LATIN CAPITAL LETTER R WITH CEDILLA
                new_char[0]  = 0xc5;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x99)) {               // LATIN CAPITAL LETTER R WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x9b)) {               // LATIN CAPITAL LETTER S WITH ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x9d)) {               // LATIN CAPITAL LETTER S WITH CIRCUMFLEX
                new_char[0]  = 0xc5;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0x9f)) {               // LATIN CAPITAL LETTER S WITH CEDILLA
                new_char[0]  = 0xc5;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa1)) {               // LATIN CAPITAL LETTER S WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa3)) {               // LATIN CAPITAL LETTER T WITH CEDILLA
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa5)) {               // LATIN CAPITAL LETTER T WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa7)) {               // LATIN CAPITAL LETTER T WITH STROKE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xa9)) {               // LATIN CAPITAL LETTER U WITH TILDE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xab)) {               // LATIN CAPITAL LETTER U WITH MACRON
                new_char[0]  = 0xc5;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xad)) {               // LATIN CAPITAL LETTER U WITH BREVE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xaf)) {               // LATIN CAPITAL LETTER U WITH RING ABOVE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb1)) {               // LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb3)) {               // LATIN CAPITAL LETTER U WITH OGONEK
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb5)) {               // LATIN CAPITAL LETTER W WITH CIRCUMFLEX
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xb7)) {               // LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xbf)) {               // LATIN CAPITAL LETTER Y WITH DIAERESIS
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xba)) {               // LATIN CAPITAL LETTER Z WITH ACUTE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xbc)) {               // LATIN CAPITAL LETTER Z WITH DOT ABOVE
                new_char[0]  = 0xc5;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc5) && (old_char[1] == 0xbe)) {               // LATIN CAPITAL LETTER Z WITH CARON
                new_char[0]  = 0xc5;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0x73)) {               // LATIN SMALL LETTER LONG S
                new_char[0]  = 0xc5;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x93)) {               // LATIN CAPITAL LETTER B WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x83)) {               // LATIN CAPITAL LETTER B WITH TOPBAR
                new_char[0]  = 0xc6;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x85)) {               // LATIN CAPITAL LETTER TONE SIX
                new_char[0]  = 0xc6;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x94)) {               // LATIN CAPITAL LETTER OPEN O
                new_char[0]  = 0xc6;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x88)) {               // LATIN CAPITAL LETTER C WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x96)) {               // LATIN CAPITAL LETTER AFRICAN D
                new_char[0]  = 0xc6;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x97)) {               // LATIN CAPITAL LETTER D WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x8c)) {               // LATIN CAPITAL LETTER D WITH TOPBAR
                new_char[0]  = 0xc6;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x9d)) {               // LATIN CAPITAL LETTER REVERSED E
                new_char[0]  = 0xc6;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x99)) {               // LATIN CAPITAL LETTER SCHWA
                new_char[0]  = 0xc6;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x9b)) {               // LATIN CAPITAL LETTER OPEN E
                new_char[0]  = 0xc6;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x92)) {               // LATIN CAPITAL LETTER F WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xa0)) {               // LATIN CAPITAL LETTER G WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xa3)) {               // LATIN CAPITAL LETTER GAMMA
                new_char[0]  = 0xc6;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xa9)) {               // LATIN CAPITAL LETTER IOTA
                new_char[0]  = 0xc6;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xa8)) {               // LATIN CAPITAL LETTER I WITH STROKE
                new_char[0]  = 0xc6;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x99)) {               // LATIN CAPITAL LETTER K WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xaf)) {               // LATIN CAPITAL LETTER TURNED M
                new_char[0]  = 0xc6;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xb2)) {               // LATIN CAPITAL LETTER N WITH LEFT HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xb5)) {               // LATIN CAPITAL LETTER O WITH MIDDLE TILDE
                new_char[0]  = 0xc6;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa1)) {               // LATIN CAPITAL LETTER O WITH HORN
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa3)) {               // LATIN CAPITAL LETTER OI
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa5)) {               // LATIN CAPITAL LETTER P WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xca) && (old_char[1] == 0x80)) {               // LATIN LETTER YR
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xa8)) {               // LATIN CAPITAL LETTER TONE TWO
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xca) && (old_char[1] == 0x83)) {               // LATIN CAPITAL LETTER ESH
                new_char[0]  = 0xc6;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xad)) {               // LATIN CAPITAL LETTER T WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xca) && (old_char[1] == 0x88)) {               // LATIN CAPITAL LETTER T WITH RETROFLEX HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb0)) {               // LATIN CAPITAL LETTER U WITH HORN
                new_char[0]  = 0xc6;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xca) && (old_char[1] == 0x8a)) {               // LATIN CAPITAL LETTER UPSILON
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xca) && (old_char[1] == 0x8b)) {               // LATIN CAPITAL LETTER V WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb4)) {               // LATIN CAPITAL LETTER Y WITH HOOK
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb6)) {               // LATIN CAPITAL LETTER Z WITH STROKE
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xca) && (old_char[1] == 0x92)) {               // LATIN CAPITAL LETTER EZH
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xb9)) {               // LATIN CAPITAL LETTER EZH REVERSED
                new_char[0]  = 0xc6;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xbd)) {               // LATIN CAPITAL LETTER TONE FIVE
                new_char[0]  = 0xc6;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x86)) {               // LATIN CAPITAL LETTER DZ WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x86)) {               // LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x89)) {               // LATIN CAPITAL LETTER LJ
                new_char[0]  = 0xc7;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x89)) {               // LATIN CAPITAL LETTER L WITH SMALL LETTER J
                new_char[0]  = 0xc7;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x8c)) {               // LATIN CAPITAL LETTER NJ
                new_char[0]  = 0xc7;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x8c)) {               // LATIN CAPITAL LETTER N WITH SMALL LETTER J
                new_char[0]  = 0xc7;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x8e)) {               // LATIN CAPITAL LETTER A WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x90)) {               // LATIN CAPITAL LETTER I WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x92)) {               // LATIN CAPITAL LETTER O WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x94)) {               // LATIN CAPITAL LETTER U WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x96)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x98)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x9a)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x9c)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
                new_char[0]  = 0xc7;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0x9f)) {               // LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa1)) {               // LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa3)) {               // LATIN CAPITAL LETTER AE WITH MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa5)) {               // LATIN CAPITAL LETTER G WITH STROKE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa7)) {               // LATIN CAPITAL LETTER G WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xa9)) {               // LATIN CAPITAL LETTER K WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xab)) {               // LATIN CAPITAL LETTER O WITH OGONEK
                new_char[0]  = 0xc7;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xad)) {               // LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xaf)) {               // LATIN CAPITAL LETTER EZH WITH CARON
                new_char[0]  = 0xc7;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb3)) {               // LATIN CAPITAL LETTER DZ
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb3)) {               // LATIN CAPITAL LETTER D WITH SMALL LETTER Z
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb5)) {               // LATIN CAPITAL LETTER G WITH ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x95)) {               // LATIN CAPITAL LETTER HWAIR
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0xbf)) {               // LATIN CAPITAL LETTER WYNN
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xb9)) {               // LATIN CAPITAL LETTER N WITH GRAVE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xbb)) {               // LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xbd)) {               // LATIN CAPITAL LETTER AE WITH ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc7) && (old_char[1] == 0xbf)) {               // LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
                new_char[0]  = 0xc7;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x81)) {               // LATIN CAPITAL LETTER A WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x83)) {               // LATIN CAPITAL LETTER A WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x85)) {               // LATIN CAPITAL LETTER E WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x87)) {               // LATIN CAPITAL LETTER E WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x89)) {               // LATIN CAPITAL LETTER I WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x8b)) {               // LATIN CAPITAL LETTER I WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x8d)) {               // LATIN CAPITAL LETTER O WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x8f)) {               // LATIN CAPITAL LETTER O WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x91)) {               // LATIN CAPITAL LETTER R WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x93)) {               // LATIN CAPITAL LETTER R WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x95)) {               // LATIN CAPITAL LETTER U WITH DOUBLE GRAVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x97)) {               // LATIN CAPITAL LETTER U WITH INVERTED BREVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x99)) {               // LATIN CAPITAL LETTER S WITH COMMA BELOW
                new_char[0]  = 0xc8;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x9b)) {               // LATIN CAPITAL LETTER T WITH COMMA BELOW
                new_char[0]  = 0xc8;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x9d)) {               // LATIN CAPITAL LETTER YOGH
                new_char[0]  = 0xc8;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0x9f)) {               // LATIN CAPITAL LETTER H WITH CARON
                new_char[0]  = 0xc8;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x9e)) {               // LATIN CAPITAL LETTER N WITH LONG RIGHT LEG
                new_char[0]  = 0xc8;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xa3)) {               // LATIN CAPITAL LETTER OU
                new_char[0]  = 0xc8;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xa5)) {               // LATIN CAPITAL LETTER Z WITH HOOK
                new_char[0]  = 0xc8;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xa7)) {               // LATIN CAPITAL LETTER A WITH DOT ABOVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xa9)) {               // LATIN CAPITAL LETTER E WITH CEDILLA
                new_char[0]  = 0xc8;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xab)) {               // LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
                new_char[0]  = 0xc8;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xad)) {               // LATIN CAPITAL LETTER O WITH TILDE AND MACRON
                new_char[0]  = 0xc8;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xaf)) {               // LATIN CAPITAL LETTER O WITH DOT ABOVE
                new_char[0]  = 0xc8;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xb1)) {               // LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
                new_char[0]  = 0xc8;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xb3)) {               // LATIN CAPITAL LETTER Y WITH MACRON
                new_char[0]  = 0xc8;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa5)) {               // LATIN CAPITAL LETTER A WITH STROKE
                new_char[0]  = 0xc8;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xbc)) {               // LATIN CAPITAL LETTER C WITH STROKE
                new_char[0]  = 0xc8;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x9a)) {               // LATIN CAPITAL LETTER L WITH BAR
                new_char[0]  = 0xc8;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa6)) {               // LATIN CAPITAL LETTER T WITH DIAGONAL STROKE
                new_char[0]  = 0xc8;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x82)) {               // LATIN CAPITAL LETTER GLOTTAL STOP
                new_char[0]  = 0xc9;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc6) && (old_char[1] == 0x80)) {               // LATIN CAPITAL LETTER B WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xca) && (old_char[1] == 0x89)) {               // LATIN CAPITAL LETTER U BAR
                new_char[0]  = 0xc9;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xca) && (old_char[1] == 0x8c)) {               // LATIN CAPITAL LETTER TURNED V
                new_char[0]  = 0xc9;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x87)) {               // LATIN CAPITAL LETTER E WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x89)) {               // LATIN CAPITAL LETTER J WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x8b)) {               // LATIN CAPITAL LETTER SMALL Q WITH HOOK TAIL
                new_char[0]  = 0xc9;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x8d)) {               // LATIN CAPITAL LETTER R WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x8f)) {               // LATIN CAPITAL LETTER Y WITH STROKE
                new_char[0]  = 0xc9;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb9)) {               // COMBINING GREEK YPOGEGRAMMENI
                new_char[0]  = 0xcd;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0xb1)) {               // GREEK CAPITAL LETTER HETA
                new_char[0]  = 0xcd;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0xb3)) {               // GREEK CAPITAL LETTER ARCHAIC SAMPI
                new_char[0]  = 0xcd;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0xb7)) {               // GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA
                new_char[0]  = 0xcd;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xac)) {               // GREEK CAPITAL LETTER ALPHA WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xad)) {               // GREEK CAPITAL LETTER EPSILON WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xae)) {               // GREEK CAPITAL LETTER ETA WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xaf)) {               // GREEK CAPITAL LETTER IOTA WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x8c)) {               // GREEK CAPITAL LETTER OMICRON WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x8d)) {               // GREEK CAPITAL LETTER UPSILON WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x8e)) {               // GREEK CAPITAL LETTER OMEGA WITH TONOS
                new_char[0]  = 0xce;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb1)) {               // GREEK CAPITAL LETTER ALPHA
                new_char[0]  = 0xce;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb2)) {               // GREEK CAPITAL LETTER BETA
                new_char[0]  = 0xce;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb3)) {               // GREEK CAPITAL LETTER GAMMA
                new_char[0]  = 0xce;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb4)) {               // GREEK CAPITAL LETTER DELTA
                new_char[0]  = 0xce;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb5)) {               // GREEK CAPITAL LETTER EPSILON
                new_char[0]  = 0xce;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb6)) {               // GREEK CAPITAL LETTER ZETA
                new_char[0]  = 0xce;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb7)) {               // GREEK CAPITAL LETTER ETA
                new_char[0]  = 0xce;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb8)) {               // GREEK CAPITAL LETTER THETA
                new_char[0]  = 0xce;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb9)) {               // GREEK CAPITAL LETTER IOTA
                new_char[0]  = 0xce;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xba)) {               // GREEK CAPITAL LETTER KAPPA
                new_char[0]  = 0xce;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xbb)) {               // GREEK CAPITAL LETTER LAMDA
                new_char[0]  = 0xce;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xbc)) {               // GREEK CAPITAL LETTER MU
                new_char[0]  = 0xce;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xbd)) {               // GREEK CAPITAL LETTER NU
                new_char[0]  = 0xce;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xbe)) {               // GREEK CAPITAL LETTER XI
                new_char[0]  = 0xce;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xbf)) {               // GREEK CAPITAL LETTER OMICRON
                new_char[0]  = 0xce;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x80)) {               // GREEK CAPITAL LETTER PI
                new_char[0]  = 0xce;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x81)) {               // GREEK CAPITAL LETTER RHO
                new_char[0]  = 0xce;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x83)) {               // GREEK CAPITAL LETTER SIGMA
                new_char[0]  = 0xce;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x84)) {               // GREEK CAPITAL LETTER TAU
                new_char[0]  = 0xce;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x85)) {               // GREEK CAPITAL LETTER UPSILON
                new_char[0]  = 0xce;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x86)) {               // GREEK CAPITAL LETTER PHI
                new_char[0]  = 0xce;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x87)) {               // GREEK CAPITAL LETTER CHI
                new_char[0]  = 0xce;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x88)) {               // GREEK CAPITAL LETTER PSI
                new_char[0]  = 0xce;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x89)) {               // GREEK CAPITAL LETTER OMEGA
                new_char[0]  = 0xce;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x8a)) {               // GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
                new_char[0]  = 0xce;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x8b)) {               // GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
                new_char[0]  = 0xce;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x83)) {               // GREEK SMALL LETTER FINAL SIGMA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x97)) {               // GREEK CAPITAL KAI SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb2)) {               // GREEK BETA SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb8)) {               // GREEK THETA SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x86)) {               // GREEK PHI SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x80)) {               // GREEK PI SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x99)) {               // GREEK LETTER ARCHAIC KOPPA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x9b)) {               // GREEK LETTER STIGMA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x9d)) {               // GREEK LETTER DIGAMMA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x9f)) {               // GREEK LETTER KOPPA
                new_char[0]  = 0xcf;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa1)) {               // GREEK LETTER SAMPI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa3)) {               // COPTIC CAPITAL LETTER SHEI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa5)) {               // COPTIC CAPITAL LETTER FEI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa7)) {               // COPTIC CAPITAL LETTER KHEI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xa9)) {               // COPTIC CAPITAL LETTER HORI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xab)) {               // COPTIC CAPITAL LETTER GANGIA
                new_char[0]  = 0xcf;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xad)) {               // COPTIC CAPITAL LETTER SHIMA
                new_char[0]  = 0xcf;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xaf)) {               // COPTIC CAPITAL LETTER DEI
                new_char[0]  = 0xcf;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xba)) {               // GREEK KAPPA SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x81)) {               // GREEK RHO SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb8)) {               // GREEK CAPITAL THETA SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb5)) {               // GREEK LUNATE EPSILON SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xb8)) {               // GREEK CAPITAL LETTER SHO
                new_char[0]  = 0xcf;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xb2)) {               // GREEK CAPITAL LUNATE SIGMA SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0xbb)) {               // GREEK CAPITAL LETTER SAN
                new_char[0]  = 0xcf;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0xbb)) {               // GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0xbc)) {               // GREEK CAPITAL DOTTED LUNATE SIGMA SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xcd) && (old_char[1] == 0xbd)) {               // GREEK CAPITAL REVERSED DOTTED LUNATE SIGMA SYMBOL
                new_char[0]  = 0xcf;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x90)) {               // CYRILLIC CAPITAL LETTER IE WITH GRAVE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x91)) {               // CYRILLIC CAPITAL LETTER IO
                new_char[0]  = 0xd0;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x92)) {               // CYRILLIC CAPITAL LETTER DJE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x93)) {               // CYRILLIC CAPITAL LETTER GJE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x94)) {               // CYRILLIC CAPITAL LETTER UKRAINIAN IE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x95)) {               // CYRILLIC CAPITAL LETTER DZE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x96)) {               // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
                new_char[0]  = 0xd0;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x97)) {               // CYRILLIC CAPITAL LETTER YI
                new_char[0]  = 0xd0;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x98)) {               // CYRILLIC CAPITAL LETTER JE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x99)) {               // CYRILLIC CAPITAL LETTER LJE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x9a)) {               // CYRILLIC CAPITAL LETTER NJE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x9b)) {               // CYRILLIC CAPITAL LETTER TSHE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x9c)) {               // CYRILLIC CAPITAL LETTER KJE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x9d)) {               // CYRILLIC CAPITAL LETTER I WITH GRAVE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x9e)) {               // CYRILLIC CAPITAL LETTER SHORT U
                new_char[0]  = 0xd0;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x9f)) {               // CYRILLIC CAPITAL LETTER DZHE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb0)) {               // CYRILLIC CAPITAL LETTER A
                new_char[0]  = 0xd0;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb1)) {               // CYRILLIC CAPITAL LETTER BE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb2)) {               // CYRILLIC CAPITAL LETTER VE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb3)) {               // CYRILLIC CAPITAL LETTER GHE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb4)) {               // CYRILLIC CAPITAL LETTER DE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb5)) {               // CYRILLIC CAPITAL LETTER IE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb6)) {               // CYRILLIC CAPITAL LETTER ZHE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb7)) {               // CYRILLIC CAPITAL LETTER ZE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x97;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb8)) {               // CYRILLIC CAPITAL LETTER I
                new_char[0]  = 0xd0;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xb9)) {               // CYRILLIC CAPITAL LETTER SHORT I
                new_char[0]  = 0xd0;
                new_char[1]  = 0x99;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xba)) {               // CYRILLIC CAPITAL LETTER KA
                new_char[0]  = 0xd0;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xbb)) {               // CYRILLIC CAPITAL LETTER EL
                new_char[0]  = 0xd0;
                new_char[1]  = 0x9b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xbc)) {               // CYRILLIC CAPITAL LETTER EM
                new_char[0]  = 0xd0;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xbd)) {               // CYRILLIC CAPITAL LETTER EN
                new_char[0]  = 0xd0;
                new_char[1]  = 0x9d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xbe)) {               // CYRILLIC CAPITAL LETTER O
                new_char[0]  = 0xd0;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd0) && (old_char[1] == 0xbf)) {               // CYRILLIC CAPITAL LETTER PE
                new_char[0]  = 0xd0;
                new_char[1]  = 0x9f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x80)) {               // CYRILLIC CAPITAL LETTER ER
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x81)) {               // CYRILLIC CAPITAL LETTER ES
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x82)) {               // CYRILLIC CAPITAL LETTER TE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x83)) {               // CYRILLIC CAPITAL LETTER U
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x84)) {               // CYRILLIC CAPITAL LETTER EF
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x85)) {               // CYRILLIC CAPITAL LETTER HA
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x86)) {               // CYRILLIC CAPITAL LETTER TSE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x87)) {               // CYRILLIC CAPITAL LETTER CHE
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x88)) {               // CYRILLIC CAPITAL LETTER SHA
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x89)) {               // CYRILLIC CAPITAL LETTER SHCHA
                new_char[0]  = 0xd0;
                new_char[1]  = 0xa9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x8a)) {               // CYRILLIC CAPITAL LETTER HARD SIGN
                new_char[0]  = 0xd0;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x8b)) {               // CYRILLIC CAPITAL LETTER YERU
                new_char[0]  = 0xd0;
                new_char[1]  = 0xab;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x8c)) {               // CYRILLIC CAPITAL LETTER SOFT SIGN
                new_char[0]  = 0xd0;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x8d)) {               // CYRILLIC CAPITAL LETTER E
                new_char[0]  = 0xd0;
                new_char[1]  = 0xad;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x8e)) {               // CYRILLIC CAPITAL LETTER YU
                new_char[0]  = 0xd0;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0x8f)) {               // CYRILLIC CAPITAL LETTER YA
                new_char[0]  = 0xd0;
                new_char[1]  = 0xaf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa1)) {               // CYRILLIC CAPITAL LETTER OMEGA
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa3)) {               // CYRILLIC CAPITAL LETTER YAT
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa5)) {               // CYRILLIC CAPITAL LETTER IOTIFIED E
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa7)) {               // CYRILLIC CAPITAL LETTER LITTLE YUS
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xa9)) {               // CYRILLIC CAPITAL LETTER IOTIFIED LITTLE YUS
                new_char[0]  = 0xd1;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xab)) {               // CYRILLIC CAPITAL LETTER BIG YUS
                new_char[0]  = 0xd1;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xad)) {               // CYRILLIC CAPITAL LETTER IOTIFIED BIG YUS
                new_char[0]  = 0xd1;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xaf)) {               // CYRILLIC CAPITAL LETTER KSI
                new_char[0]  = 0xd1;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb1)) {               // CYRILLIC CAPITAL LETTER PSI
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb3)) {               // CYRILLIC CAPITAL LETTER FITA
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb5)) {               // CYRILLIC CAPITAL LETTER IZHITSA
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb7)) {               // CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xb9)) {               // CYRILLIC CAPITAL LETTER UK
                new_char[0]  = 0xd1;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xbb)) {               // CYRILLIC CAPITAL LETTER ROUND OMEGA
                new_char[0]  = 0xd1;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xbd)) {               // CYRILLIC CAPITAL LETTER OMEGA WITH TITLO
                new_char[0]  = 0xd1;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd1) && (old_char[1] == 0xbf)) {               // CYRILLIC CAPITAL LETTER OT
                new_char[0]  = 0xd1;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x81)) {               // CYRILLIC CAPITAL LETTER KOPPA
                new_char[0]  = 0xd2;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x8b)) {               // CYRILLIC CAPITAL LETTER SHORT I WITH TAIL
                new_char[0]  = 0xd2;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x8d)) {               // CYRILLIC CAPITAL LETTER SEMISOFT SIGN
                new_char[0]  = 0xd2;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x8f)) {               // CYRILLIC CAPITAL LETTER ER WITH TICK
                new_char[0]  = 0xd2;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x91)) {               // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
                new_char[0]  = 0xd2;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x93)) {               // CYRILLIC CAPITAL LETTER GHE WITH STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x95)) {               // CYRILLIC CAPITAL LETTER GHE WITH MIDDLE HOOK
                new_char[0]  = 0xd2;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x97)) {               // CYRILLIC CAPITAL LETTER ZHE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x99)) {               // CYRILLIC CAPITAL LETTER ZE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x9b)) {               // CYRILLIC CAPITAL LETTER KA WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x9d)) {               // CYRILLIC CAPITAL LETTER KA WITH VERTICAL STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0x9f)) {               // CYRILLIC CAPITAL LETTER KA WITH STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa1)) {               // CYRILLIC CAPITAL LETTER BASHKIR KA
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa3)) {               // CYRILLIC CAPITAL LETTER EN WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa5)) {               // CYRILLIC CAPITAL LIGATURE EN GHE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa7)) {               // CYRILLIC CAPITAL LETTER PE WITH MIDDLE HOOK
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xa9)) {               // CYRILLIC CAPITAL LETTER ABKHASIAN HA
                new_char[0]  = 0xd2;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xab)) {               // CYRILLIC CAPITAL LETTER ES WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xad)) {               // CYRILLIC CAPITAL LETTER TE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xaf)) {               // CYRILLIC CAPITAL LETTER STRAIGHT U
                new_char[0]  = 0xd2;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb1)) {               // CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb3)) {               // CYRILLIC CAPITAL LETTER HA WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb5)) {               // CYRILLIC CAPITAL LIGATURE TE TSE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb7)) {               // CYRILLIC CAPITAL LETTER CHE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xb9)) {               // CYRILLIC CAPITAL LETTER CHE WITH VERTICAL STROKE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xbb)) {               // CYRILLIC CAPITAL LETTER SHHA
                new_char[0]  = 0xd2;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xbd)) {               // CYRILLIC CAPITAL LETTER ABKHASIAN CHE
                new_char[0]  = 0xd2;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd2) && (old_char[1] == 0xbf)) {               // CYRILLIC CAPITAL LETTER ABKHASIAN CHE WITH DESCENDER
                new_char[0]  = 0xd2;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x8f)) {               // CYRILLIC LETTER PALOCHKA
                new_char[0]  = 0xd3;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x82)) {               // CYRILLIC CAPITAL LETTER ZHE WITH BREVE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x84)) {               // CYRILLIC CAPITAL LETTER KA WITH HOOK
                new_char[0]  = 0xd3;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x86)) {               // CYRILLIC CAPITAL LETTER EL WITH TAIL
                new_char[0]  = 0xd3;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x88)) {               // CYRILLIC CAPITAL LETTER EN WITH HOOK
                new_char[0]  = 0xd3;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x8a)) {               // CYRILLIC CAPITAL LETTER EN WITH TAIL
                new_char[0]  = 0xd3;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x8c)) {               // CYRILLIC CAPITAL LETTER KHAKASSIAN CHE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x8e)) {               // CYRILLIC CAPITAL LETTER EM WITH TAIL
                new_char[0]  = 0xd3;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x91)) {               // CYRILLIC CAPITAL LETTER A WITH BREVE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x93)) {               // CYRILLIC CAPITAL LETTER A WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x95)) {               // CYRILLIC CAPITAL LIGATURE A IE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x97)) {               // CYRILLIC CAPITAL LETTER IE WITH BREVE
                new_char[0]  = 0xd3;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x99)) {               // CYRILLIC CAPITAL LETTER SCHWA
                new_char[0]  = 0xd3;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x9b)) {               // CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x9d)) {               // CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0x9f)) {               // CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa1)) {               // CYRILLIC CAPITAL LETTER ABKHASIAN DZE
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa3)) {               // CYRILLIC CAPITAL LETTER I WITH MACRON
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa5)) {               // CYRILLIC CAPITAL LETTER I WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa7)) {               // CYRILLIC CAPITAL LETTER O WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xa9)) {               // CYRILLIC CAPITAL LETTER BARRED O
                new_char[0]  = 0xd3;
                new_char[1]  = 0xa8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xab)) {               // CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xaa;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xad)) {               // CYRILLIC CAPITAL LETTER E WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xac;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xaf)) {               // CYRILLIC CAPITAL LETTER U WITH MACRON
                new_char[0]  = 0xd3;
                new_char[1]  = 0xae;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb1)) {               // CYRILLIC CAPITAL LETTER U WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb3)) {               // CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb5)) {               // CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb7)) {               // CYRILLIC CAPITAL LETTER GHE WITH DESCENDER
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xb9)) {               // CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
                new_char[0]  = 0xd3;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xbb)) {               // CYRILLIC CAPITAL LETTER GHE WITH STROKE AND HOOK
                new_char[0]  = 0xd3;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xbd)) {               // CYRILLIC CAPITAL LETTER HA WITH HOOK
                new_char[0]  = 0xd3;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd3) && (old_char[1] == 0xbf)) {               // CYRILLIC CAPITAL LETTER HA WITH STROKE
                new_char[0]  = 0xd3;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x81)) {               // CYRILLIC CAPITAL LETTER KOMI DE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x83)) {               // CYRILLIC CAPITAL LETTER KOMI DJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x85)) {               // CYRILLIC CAPITAL LETTER KOMI ZJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x87)) {               // CYRILLIC CAPITAL LETTER KOMI DZJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x89)) {               // CYRILLIC CAPITAL LETTER KOMI LJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x8b)) {               // CYRILLIC CAPITAL LETTER KOMI NJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x8d)) {               // CYRILLIC CAPITAL LETTER KOMI SJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x8f)) {               // CYRILLIC CAPITAL LETTER KOMI TJE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x91)) {               // CYRILLIC CAPITAL LETTER REVERSED ZE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x93)) {               // CYRILLIC CAPITAL LETTER EL WITH HOOK
                new_char[0]  = 0xd4;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x95)) {               // CYRILLIC CAPITAL LETTER LHA
                new_char[0]  = 0xd4;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x97)) {               // CYRILLIC CAPITAL LETTER RHA
                new_char[0]  = 0xd4;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x99)) {               // CYRILLIC CAPITAL LETTER YAE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x98;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x9b)) {               // CYRILLIC CAPITAL LETTER QA
                new_char[0]  = 0xd4;
                new_char[1]  = 0x9a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x9d)) {               // CYRILLIC CAPITAL LETTER WE
                new_char[0]  = 0xd4;
                new_char[1]  = 0x9c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0x9f)) {               // CYRILLIC CAPITAL LETTER ALEUT KA
                new_char[0]  = 0xd4;
                new_char[1]  = 0x9e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xa1)) {               // CYRILLIC CAPITAL LETTER EL WITH MIDDLE HOOK
                new_char[0]  = 0xd4;
                new_char[1]  = 0xa0;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xa3)) {               // CYRILLIC CAPITAL LETTER EN WITH MIDDLE HOOK
                new_char[0]  = 0xd4;
                new_char[1]  = 0xa2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xa5)) {               // CYRILLIC CAPITAL LETTER PE WITH DESCENDER
                new_char[0]  = 0xd4;
                new_char[1]  = 0xa4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd4) && (old_char[1] == 0xa7)) {               // CYRILLIC CAPITAL LETTER SHHA WITH DESCENDER
                new_char[0]  = 0xd4;
                new_char[1]  = 0xa6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xa1)) {               // ARMENIAN CAPITAL LETTER AYB
                new_char[0]  = 0xd4;
                new_char[1]  = 0xb1;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xa2)) {               // ARMENIAN CAPITAL LETTER BEN
                new_char[0]  = 0xd4;
                new_char[1]  = 0xb2;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xa3)) {               // ARMENIAN CAPITAL LETTER GIM
                new_char[0]  = 0xd4;
                new_char[1]  = 0xb3;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xa4)) {               // ARMENIAN CAPITAL LETTER DA
                new_char[0]  = 0xd4;
                new_char[1]  = 0xb4;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xa5)) {               // ARMENIAN CAPITAL LETTER ECH
                new_char[0]  = 0xd4;
                new_char[1]  = 0xb5;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xa6)) {               // ARMENIAN CAPITAL LETTER ZA
                new_char[0]  = 0xd4;
                new_char[1]  = 0xb6;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xa7)) {               // ARMENIAN CAPITAL LETTER EH
                new_char[0]  = 0xd4;
                new_char[1]  = 0xb7;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xa8)) {               // ARMENIAN CAPITAL LETTER ET
                new_char[0]  = 0xd4;
                new_char[1]  = 0xb8;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xa9)) {               // ARMENIAN CAPITAL LETTER TO
                new_char[0]  = 0xd4;
                new_char[1]  = 0xb9;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xaa)) {               // ARMENIAN CAPITAL LETTER ZHE
                new_char[0]  = 0xd4;
                new_char[1]  = 0xba;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xab)) {               // ARMENIAN CAPITAL LETTER INI
                new_char[0]  = 0xd4;
                new_char[1]  = 0xbb;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xac)) {               // ARMENIAN CAPITAL LETTER LIWN
                new_char[0]  = 0xd4;
                new_char[1]  = 0xbc;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xad)) {               // ARMENIAN CAPITAL LETTER XEH
                new_char[0]  = 0xd4;
                new_char[1]  = 0xbd;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xae)) {               // ARMENIAN CAPITAL LETTER CA
                new_char[0]  = 0xd4;
                new_char[1]  = 0xbe;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xaf)) {               // ARMENIAN CAPITAL LETTER KEN
                new_char[0]  = 0xd4;
                new_char[1]  = 0xbf;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb0)) {               // ARMENIAN CAPITAL LETTER HO
                new_char[0]  = 0xd5;
                new_char[1]  = 0x80;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb1)) {               // ARMENIAN CAPITAL LETTER JA
                new_char[0]  = 0xd5;
                new_char[1]  = 0x81;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb2)) {               // ARMENIAN CAPITAL LETTER GHAD
                new_char[0]  = 0xd5;
                new_char[1]  = 0x82;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb3)) {               // ARMENIAN CAPITAL LETTER CHEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0x83;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb4)) {               // ARMENIAN CAPITAL LETTER MEN
                new_char[0]  = 0xd5;
                new_char[1]  = 0x84;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb5)) {               // ARMENIAN CAPITAL LETTER YI
                new_char[0]  = 0xd5;
                new_char[1]  = 0x85;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb6)) {               // ARMENIAN CAPITAL LETTER NOW
                new_char[0]  = 0xd5;
                new_char[1]  = 0x86;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb7)) {               // ARMENIAN CAPITAL LETTER SHA
                new_char[0]  = 0xd5;
                new_char[1]  = 0x87;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb8)) {               // ARMENIAN CAPITAL LETTER VO
                new_char[0]  = 0xd5;
                new_char[1]  = 0x88;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xb9)) {               // ARMENIAN CAPITAL LETTER CHA
                new_char[0]  = 0xd5;
                new_char[1]  = 0x89;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xba)) {               // ARMENIAN CAPITAL LETTER PEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0x8a;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xbb)) {               // ARMENIAN CAPITAL LETTER JHEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0x8b;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xbc)) {               // ARMENIAN CAPITAL LETTER RA
                new_char[0]  = 0xd5;
                new_char[1]  = 0x8c;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xbd)) {               // ARMENIAN CAPITAL LETTER SEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0x8d;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xbe)) {               // ARMENIAN CAPITAL LETTER VEW
                new_char[0]  = 0xd5;
                new_char[1]  = 0x8e;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd5) && (old_char[1] == 0xbf)) {               // ARMENIAN CAPITAL LETTER TIWN
                new_char[0]  = 0xd5;
                new_char[1]  = 0x8f;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd6) && (old_char[1] == 0x80)) {               // ARMENIAN CAPITAL LETTER REH
                new_char[0]  = 0xd5;
                new_char[1]  = 0x90;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd6) && (old_char[1] == 0x81)) {               // ARMENIAN CAPITAL LETTER CO
                new_char[0]  = 0xd5;
                new_char[1]  = 0x91;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd6) && (old_char[1] == 0x82)) {               // ARMENIAN CAPITAL LETTER YIWN
                new_char[0]  = 0xd5;
                new_char[1]  = 0x92;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd6) && (old_char[1] == 0x83)) {               // ARMENIAN CAPITAL LETTER PIWR
                new_char[0]  = 0xd5;
                new_char[1]  = 0x93;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd6) && (old_char[1] == 0x84)) {               // ARMENIAN CAPITAL LETTER KEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0x94;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd6) && (old_char[1] == 0x85)) {               // ARMENIAN CAPITAL LETTER OH
                new_char[0]  = 0xd5;
                new_char[1]  = 0x95;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xd6) && (old_char[1] == 0x86)) {               // ARMENIAN CAPITAL LETTER FEH
                new_char[0]  = 0xd5;
                new_char[1]  = 0x96;
                new_char_len = 2;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x80)) {               // GEORGIAN CAPITAL LETTER AN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x81)) {               // GEORGIAN CAPITAL LETTER BAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x82)) {               // GEORGIAN CAPITAL LETTER GAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x83)) {               // GEORGIAN CAPITAL LETTER DON
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x84)) {               // GEORGIAN CAPITAL LETTER EN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x85)) {               // GEORGIAN CAPITAL LETTER VIN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x86)) {               // GEORGIAN CAPITAL LETTER ZEN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x87)) {               // GEORGIAN CAPITAL LETTER TAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x88)) {               // GEORGIAN CAPITAL LETTER IN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x89)) {               // GEORGIAN CAPITAL LETTER KAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x8a)) {               // GEORGIAN CAPITAL LETTER LAS
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x8b)) {               // GEORGIAN CAPITAL LETTER MAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x8c)) {               // GEORGIAN CAPITAL LETTER NAR
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x8d)) {               // GEORGIAN CAPITAL LETTER ON
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x8e)) {               // GEORGIAN CAPITAL LETTER PAR
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x8f)) {               // GEORGIAN CAPITAL LETTER ZHAR
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x90)) {               // GEORGIAN CAPITAL LETTER RAE
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x91)) {               // GEORGIAN CAPITAL LETTER SAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x92)) {               // GEORGIAN CAPITAL LETTER TAR
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x93)) {               // GEORGIAN CAPITAL LETTER UN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x94)) {               // GEORGIAN CAPITAL LETTER PHAR
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x95)) {               // GEORGIAN CAPITAL LETTER KHAR
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x96)) {               // GEORGIAN CAPITAL LETTER GHAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x97)) {               // GEORGIAN CAPITAL LETTER QAR
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x98)) {               // GEORGIAN CAPITAL LETTER SHIN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x99)) {               // GEORGIAN CAPITAL LETTER CHIN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x9a)) {               // GEORGIAN CAPITAL LETTER CAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x9b)) {               // GEORGIAN CAPITAL LETTER JIL
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x9c)) {               // GEORGIAN CAPITAL LETTER CIL
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x9d)) {               // GEORGIAN CAPITAL LETTER CHAR
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x9e)) {               // GEORGIAN CAPITAL LETTER XAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0x9f)) {               // GEORGIAN CAPITAL LETTER JHAN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x82;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0xa0)) {               // GEORGIAN CAPITAL LETTER HAE
                new_char[0]  = 0xe1;
                new_char[1]  = 0x83;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0xa1)) {               // GEORGIAN CAPITAL LETTER HE
                new_char[0]  = 0xe1;
                new_char[1]  = 0x83;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0xa2)) {               // GEORGIAN CAPITAL LETTER HIE
                new_char[0]  = 0xe1;
                new_char[1]  = 0x83;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0xa3)) {               // GEORGIAN CAPITAL LETTER WE
                new_char[0]  = 0xe1;
                new_char[1]  = 0x83;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0xa4)) {               // GEORGIAN CAPITAL LETTER HAR
                new_char[0]  = 0xe1;
                new_char[1]  = 0x83;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0xa5)) {               // GEORGIAN CAPITAL LETTER HOE
                new_char[0]  = 0xe1;
                new_char[1]  = 0x83;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0xa7)) {               // GEORGIAN CAPITAL LETTER YN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x83;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb4) && (old_char[2] == 0xad)) {               // GEORGIAN CAPITAL LETTER AEN
                new_char[0]  = 0xe1;
                new_char[1]  = 0x83;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x81)) {               // LATIN CAPITAL LETTER A WITH RING BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x83)) {               // LATIN CAPITAL LETTER B WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x85)) {               // LATIN CAPITAL LETTER B WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x87)) {               // LATIN CAPITAL LETTER B WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x89)) {               // LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x8b)) {               // LATIN CAPITAL LETTER D WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x8d)) {               // LATIN CAPITAL LETTER D WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x8f)) {               // LATIN CAPITAL LETTER D WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x91)) {               // LATIN CAPITAL LETTER D WITH CEDILLA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x93)) {               // LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x95)) {               // LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x97)) {               // LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x99)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x9b)) {               // LATIN CAPITAL LETTER E WITH TILDE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x9d)) {               // LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0x9f)) {               // LATIN CAPITAL LETTER F WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa1)) {               // LATIN CAPITAL LETTER G WITH MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa3)) {               // LATIN CAPITAL LETTER H WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa5)) {               // LATIN CAPITAL LETTER H WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa7)) {               // LATIN CAPITAL LETTER H WITH DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xa9)) {               // LATIN CAPITAL LETTER H WITH CEDILLA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xab)) {               // LATIN CAPITAL LETTER H WITH BREVE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xad)) {               // LATIN CAPITAL LETTER I WITH TILDE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xaf)) {               // LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb1)) {               // LATIN CAPITAL LETTER K WITH ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb3)) {               // LATIN CAPITAL LETTER K WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb5)) {               // LATIN CAPITAL LETTER K WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb7)) {               // LATIN CAPITAL LETTER L WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xb9)) {               // LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xbb)) {               // LATIN CAPITAL LETTER L WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xbd)) {               // LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb8) && (old_char[2] == 0xbf)) {               // LATIN CAPITAL LETTER M WITH ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb8;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x81)) {               // LATIN CAPITAL LETTER M WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x83)) {               // LATIN CAPITAL LETTER M WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x85)) {               // LATIN CAPITAL LETTER N WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x87)) {               // LATIN CAPITAL LETTER N WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x89)) {               // LATIN CAPITAL LETTER N WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x8b)) {               // LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x8d)) {               // LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x8f)) {               // LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x91)) {               // LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x93)) {               // LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x95)) {               // LATIN CAPITAL LETTER P WITH ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x97)) {               // LATIN CAPITAL LETTER P WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x99)) {               // LATIN CAPITAL LETTER R WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x9b)) {               // LATIN CAPITAL LETTER R WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x9d)) {               // LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0x9f)) {               // LATIN CAPITAL LETTER R WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa1)) {               // LATIN CAPITAL LETTER S WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa3)) {               // LATIN CAPITAL LETTER S WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa5)) {               // LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa7)) {               // LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa9)) {               // LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xab)) {               // LATIN CAPITAL LETTER T WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xad)) {               // LATIN CAPITAL LETTER T WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xaf)) {               // LATIN CAPITAL LETTER T WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb1)) {               // LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb3)) {               // LATIN CAPITAL LETTER U WITH DIAERESIS BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb5)) {               // LATIN CAPITAL LETTER U WITH TILDE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb7)) {               // LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xb9)) {               // LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xbb)) {               // LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xbd)) {               // LATIN CAPITAL LETTER V WITH TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xbf)) {               // LATIN CAPITAL LETTER V WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xb9;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x81)) {               // LATIN CAPITAL LETTER W WITH GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x83)) {               // LATIN CAPITAL LETTER W WITH ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x85)) {               // LATIN CAPITAL LETTER W WITH DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x87)) {               // LATIN CAPITAL LETTER W WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x89)) {               // LATIN CAPITAL LETTER W WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x8b)) {               // LATIN CAPITAL LETTER X WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x8d)) {               // LATIN CAPITAL LETTER X WITH DIAERESIS
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x8f)) {               // LATIN CAPITAL LETTER Y WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x91)) {               // LATIN CAPITAL LETTER Z WITH CIRCUMFLEX
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x93)) {               // LATIN CAPITAL LETTER Z WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0x95)) {               // LATIN CAPITAL LETTER Z WITH LINE BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb9) && (old_char[2] == 0xa1)) {               // LATIN SMALL LETTER LONG S WITH DOT ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa1)) {               // LATIN CAPITAL LETTER A WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa3)) {               // LATIN CAPITAL LETTER A WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa5)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa7)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xa9)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xab)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xad)) {               // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xaf)) {               // LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb1)) {               // LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb3)) {               // LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb5)) {               // LATIN CAPITAL LETTER A WITH BREVE AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb7)) {               // LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xb9)) {               // LATIN CAPITAL LETTER E WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xbb)) {               // LATIN CAPITAL LETTER E WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xbd)) {               // LATIN CAPITAL LETTER E WITH TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xba) && (old_char[2] == 0xbf)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x81)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x83)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x85)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x87)) {               // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x89)) {               // LATIN CAPITAL LETTER I WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x8b)) {               // LATIN CAPITAL LETTER I WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x8d)) {               // LATIN CAPITAL LETTER O WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x8f)) {               // LATIN CAPITAL LETTER O WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x91)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x93)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x95)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x97)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x99)) {               // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x9b)) {               // LATIN CAPITAL LETTER O WITH HORN AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x9d)) {               // LATIN CAPITAL LETTER O WITH HORN AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0x9f)) {               // LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa1)) {               // LATIN CAPITAL LETTER O WITH HORN AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa3)) {               // LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa5)) {               // LATIN CAPITAL LETTER U WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa7)) {               // LATIN CAPITAL LETTER U WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xa9)) {               // LATIN CAPITAL LETTER U WITH HORN AND ACUTE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xab)) {               // LATIN CAPITAL LETTER U WITH HORN AND GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xad)) {               // LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xaf)) {               // LATIN CAPITAL LETTER U WITH HORN AND TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb1)) {               // LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb3)) {               // LATIN CAPITAL LETTER Y WITH GRAVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb5)) {               // LATIN CAPITAL LETTER Y WITH DOT BELOW
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb7)) {               // LATIN CAPITAL LETTER Y WITH HOOK ABOVE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xb9)) {               // LATIN CAPITAL LETTER Y WITH TILDE
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xbb)) {               // LATIN CAPITAL LETTER MIDDLE-WELSH LL
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xbd)) {               // LATIN CAPITAL LETTER MIDDLE-WELSH V
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbb) && (old_char[2] == 0xbf)) {               // LATIN CAPITAL LETTER Y WITH LOOP
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbb;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x80)) {               // GREEK CAPITAL LETTER ALPHA WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x81)) {               // GREEK CAPITAL LETTER ALPHA WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x82)) {               // GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x83)) {               // GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x84)) {               // GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x85)) {               // GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x86)) {               // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x87)) {               // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x90)) {               // GREEK CAPITAL LETTER EPSILON WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x91)) {               // GREEK CAPITAL LETTER EPSILON WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x92)) {               // GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x93)) {               // GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x94)) {               // GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0x95)) {               // GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa0)) {               // GREEK CAPITAL LETTER ETA WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa1)) {               // GREEK CAPITAL LETTER ETA WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa2)) {               // GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa3)) {               // GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa4)) {               // GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa5)) {               // GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa6)) {               // GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xa7)) {               // GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb0)) {               // GREEK CAPITAL LETTER IOTA WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb1)) {               // GREEK CAPITAL LETTER IOTA WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb2)) {               // GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb3)) {               // GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb4)) {               // GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb5)) {               // GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb6)) {               // GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbc) && (old_char[2] == 0xb7)) {               // GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x80)) {               // GREEK CAPITAL LETTER OMICRON WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x81)) {               // GREEK CAPITAL LETTER OMICRON WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x82)) {               // GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x83)) {               // GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x84)) {               // GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x85)) {               // GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x91)) {               // GREEK CAPITAL LETTER UPSILON WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x93)) {               // GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x95)) {               // GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0x97)) {               // GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa0)) {               // GREEK CAPITAL LETTER OMEGA WITH PSILI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa1)) {               // GREEK CAPITAL LETTER OMEGA WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa2)) {               // GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa3)) {               // GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa4)) {               // GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa5)) {               // GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa6)) {               // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xa7)) {               // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbd;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xb0)) {               // GREEK CAPITAL LETTER ALPHA WITH VRACHY
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xb1)) {               // GREEK CAPITAL LETTER ALPHA WITH MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb0)) {               // GREEK CAPITAL LETTER ALPHA WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb1)) {               // GREEK CAPITAL LETTER ALPHA WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xce) && (old_char[1] == 0xb9)) {               // GREEK PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb2)) {               // GREEK CAPITAL LETTER EPSILON WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb3)) {               // GREEK CAPITAL LETTER EPSILON WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb4)) {               // GREEK CAPITAL LETTER ETA WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb5)) {               // GREEK CAPITAL LETTER ETA WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x90)) {               // GREEK CAPITAL LETTER IOTA WITH VRACHY
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x91)) {               // GREEK CAPITAL LETTER IOTA WITH MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb6)) {               // GREEK CAPITAL LETTER IOTA WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb7)) {               // GREEK CAPITAL LETTER IOTA WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xa0)) {               // GREEK CAPITAL LETTER UPSILON WITH VRACHY
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xa1)) {               // GREEK CAPITAL LETTER UPSILON WITH MACRON
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xba)) {               // GREEK CAPITAL LETTER UPSILON WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xbb)) {               // GREEK CAPITAL LETTER UPSILON WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xa5)) {               // GREEK CAPITAL LETTER RHO WITH DASIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb8)) {               // GREEK CAPITAL LETTER OMICRON WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xb9)) {               // GREEK CAPITAL LETTER OMICRON WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xbc)) {               // GREEK CAPITAL LETTER OMEGA WITH VARIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbd) && (old_char[2] == 0xbd)) {               // GREEK CAPITAL LETTER OMEGA WITH OXIA
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xcf) && (old_char[1] == 0x89)) {               // OHM SIGN
                new_char[0]  = 0xe2;
                new_char[1]  = 0x84;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0x6b)) {               // KELVIN SIGN
                new_char[0]  = 0xe2;
                new_char[1]  = 0x84;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 1;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0xa5)) {               // ANGSTROM SIGN
                new_char[0]  = 0xe2;
                new_char[1]  = 0x84;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0x8e)) {               // TURNED CAPITAL F
                new_char[0]  = 0xe2;
                new_char[1]  = 0x84;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb0)) {               // ROMAN NUMERAL ONE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb1)) {               // ROMAN NUMERAL TWO
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb2)) {               // ROMAN NUMERAL THREE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb3)) {               // ROMAN NUMERAL FOUR
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb4)) {               // ROMAN NUMERAL FIVE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb5)) {               // ROMAN NUMERAL SIX
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb6)) {               // ROMAN NUMERAL SEVEN
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb7)) {               // ROMAN NUMERAL EIGHT
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb8)) {               // ROMAN NUMERAL NINE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xb9)) {               // ROMAN NUMERAL TEN
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xba)) {               // ROMAN NUMERAL ELEVEN
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xbb)) {               // ROMAN NUMERAL TWELVE
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xbc)) {               // ROMAN NUMERAL FIFTY
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xbd)) {               // ROMAN NUMERAL ONE HUNDRED
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xbe)) {               // ROMAN NUMERAL FIVE HUNDRED
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x85) && (old_char[2] == 0xbf)) {               // ROMAN NUMERAL ONE THOUSAND
                new_char[0]  = 0xe2;
                new_char[1]  = 0x85;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x86) && (old_char[2] == 0x84)) {               // ROMAN NUMERAL REVERSED ONE HUNDRED
                new_char[0]  = 0xe2;
                new_char[1]  = 0x86;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x90)) {               // CIRCLED LATIN CAPITAL LETTER A
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x91)) {               // CIRCLED LATIN CAPITAL LETTER B
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x92)) {               // CIRCLED LATIN CAPITAL LETTER C
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x93)) {               // CIRCLED LATIN CAPITAL LETTER D
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x94)) {               // CIRCLED LATIN CAPITAL LETTER E
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x95)) {               // CIRCLED LATIN CAPITAL LETTER F
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x96)) {               // CIRCLED LATIN CAPITAL LETTER G
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x97)) {               // CIRCLED LATIN CAPITAL LETTER H
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x98)) {               // CIRCLED LATIN CAPITAL LETTER I
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x99)) {               // CIRCLED LATIN CAPITAL LETTER J
                new_char[0]  = 0xe2;
                new_char[1]  = 0x92;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x9a)) {               // CIRCLED LATIN CAPITAL LETTER K
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x9b)) {               // CIRCLED LATIN CAPITAL LETTER L
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x9c)) {               // CIRCLED LATIN CAPITAL LETTER M
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x9d)) {               // CIRCLED LATIN CAPITAL LETTER N
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x9e)) {               // CIRCLED LATIN CAPITAL LETTER O
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0x9f)) {               // CIRCLED LATIN CAPITAL LETTER P
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa0)) {               // CIRCLED LATIN CAPITAL LETTER Q
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa1)) {               // CIRCLED LATIN CAPITAL LETTER R
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa2)) {               // CIRCLED LATIN CAPITAL LETTER S
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa3)) {               // CIRCLED LATIN CAPITAL LETTER T
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa4)) {               // CIRCLED LATIN CAPITAL LETTER U
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa5)) {               // CIRCLED LATIN CAPITAL LETTER V
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa6)) {               // CIRCLED LATIN CAPITAL LETTER W
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa7)) {               // CIRCLED LATIN CAPITAL LETTER X
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa8)) {               // CIRCLED LATIN CAPITAL LETTER Y
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0x93) && (old_char[2] == 0xa9)) {               // CIRCLED LATIN CAPITAL LETTER Z
                new_char[0]  = 0xe2;
                new_char[1]  = 0x93;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb0)) {               // GLAGOLITIC CAPITAL LETTER AZU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb1)) {               // GLAGOLITIC CAPITAL LETTER BUKY
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x81;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb2)) {               // GLAGOLITIC CAPITAL LETTER VEDE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb3)) {               // GLAGOLITIC CAPITAL LETTER GLAGOLI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x83;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb4)) {               // GLAGOLITIC CAPITAL LETTER DOBRO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb5)) {               // GLAGOLITIC CAPITAL LETTER YESTU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x85;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb6)) {               // GLAGOLITIC CAPITAL LETTER ZHIVETE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb7)) {               // GLAGOLITIC CAPITAL LETTER DZELO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x87;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb8)) {               // GLAGOLITIC CAPITAL LETTER ZEMLJA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xb9)) {               // GLAGOLITIC CAPITAL LETTER IZHE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xba)) {               // GLAGOLITIC CAPITAL LETTER INITIAL IZHE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xbb)) {               // GLAGOLITIC CAPITAL LETTER I
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xbc)) {               // GLAGOLITIC CAPITAL LETTER DJERVI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xbd)) {               // GLAGOLITIC CAPITAL LETTER KAKO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xbe)) {               // GLAGOLITIC CAPITAL LETTER LJUDIJE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb0) && (old_char[2] == 0xbf)) {               // GLAGOLITIC CAPITAL LETTER MYSLITE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x80)) {               // GLAGOLITIC CAPITAL LETTER NASHI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x81)) {               // GLAGOLITIC CAPITAL LETTER ONU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x91;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x82)) {               // GLAGOLITIC CAPITAL LETTER POKOJI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x83)) {               // GLAGOLITIC CAPITAL LETTER RITSI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x93;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x84)) {               // GLAGOLITIC CAPITAL LETTER SLOVO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x85)) {               // GLAGOLITIC CAPITAL LETTER TVRIDO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x95;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x86)) {               // GLAGOLITIC CAPITAL LETTER UKU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x87)) {               // GLAGOLITIC CAPITAL LETTER FRITU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x97;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x88)) {               // GLAGOLITIC CAPITAL LETTER HERU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x89)) {               // GLAGOLITIC CAPITAL LETTER OTU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x8a)) {               // GLAGOLITIC CAPITAL LETTER PE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x8b)) {               // GLAGOLITIC CAPITAL LETTER SHTA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x8c)) {               // GLAGOLITIC CAPITAL LETTER TSI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x8d)) {               // GLAGOLITIC CAPITAL LETTER CHRIVI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x8e)) {               // GLAGOLITIC CAPITAL LETTER SHA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x8f)) {               // GLAGOLITIC CAPITAL LETTER YERU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x90)) {               // GLAGOLITIC CAPITAL LETTER YERI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x91)) {               // GLAGOLITIC CAPITAL LETTER YATI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x92)) {               // GLAGOLITIC CAPITAL LETTER SPIDERY HA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x93)) {               // GLAGOLITIC CAPITAL LETTER YU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x94)) {               // GLAGOLITIC CAPITAL LETTER SMALL YUS
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x95)) {               // GLAGOLITIC CAPITAL LETTER SMALL YUS WITH TAIL
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x96)) {               // GLAGOLITIC CAPITAL LETTER YO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x97)) {               // GLAGOLITIC CAPITAL LETTER IOTATED SMALL YUS
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x98)) {               // GLAGOLITIC CAPITAL LETTER BIG YUS
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x99)) {               // GLAGOLITIC CAPITAL LETTER IOTATED BIG YUS
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x9a)) {               // GLAGOLITIC CAPITAL LETTER FITA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x9b)) {               // GLAGOLITIC CAPITAL LETTER IZHITSA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x9c)) {               // GLAGOLITIC CAPITAL LETTER SHTAPIC
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x9d)) {               // GLAGOLITIC CAPITAL LETTER TROKUTASTI A
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0x9e)) {               // GLAGOLITIC CAPITAL LETTER LATINATE MYSLITE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb0;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa1)) {               // LATIN CAPITAL LETTER L WITH DOUBLE BAR
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xab)) {               // LATIN CAPITAL LETTER L WITH MIDDLE TILDE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb5) && (old_char[2] == 0xbd)) {               // LATIN CAPITAL LETTER P WITH STROKE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xbd)) {               // LATIN CAPITAL LETTER R WITH TAIL
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xa8)) {               // LATIN CAPITAL LETTER H WITH DESCENDER
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xaa)) {               // LATIN CAPITAL LETTER K WITH DESCENDER
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xac)) {               // LATIN CAPITAL LETTER Z WITH DESCENDER
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x91)) {               // LATIN CAPITAL LETTER ALPHA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xb1)) {               // LATIN CAPITAL LETTER M WITH HOOK
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x90)) {               // LATIN CAPITAL LETTER TURNED A
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x92)) {               // LATIN CAPITAL LETTER TURNED ALPHA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xb3)) {               // LATIN CAPITAL LETTER W WITH HOOK
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb1) && (old_char[2] == 0xb6)) {               // LATIN CAPITAL LETTER HALF H
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xc8) && (old_char[1] == 0xbf)) {               // LATIN CAPITAL LETTER S WITH SWASH TAIL
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0x80)) {               // LATIN CAPITAL LETTER Z WITH SWASH TAIL
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb1;
                new_char[2]  = 0xbf;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x81)) {               // COPTIC CAPITAL LETTER ALFA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x83)) {               // COPTIC CAPITAL LETTER VIDA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x85)) {               // COPTIC CAPITAL LETTER GAMMA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x87)) {               // COPTIC CAPITAL LETTER DALDA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x89)) {               // COPTIC CAPITAL LETTER EIE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x8b)) {               // COPTIC CAPITAL LETTER SOU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x8d)) {               // COPTIC CAPITAL LETTER ZATA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x8f)) {               // COPTIC CAPITAL LETTER HATE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x91)) {               // COPTIC CAPITAL LETTER THETHE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x93)) {               // COPTIC CAPITAL LETTER IAUDA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x95)) {               // COPTIC CAPITAL LETTER KAPA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x97)) {               // COPTIC CAPITAL LETTER LAULA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x99)) {               // COPTIC CAPITAL LETTER MI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x9b)) {               // COPTIC CAPITAL LETTER NI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x9d)) {               // COPTIC CAPITAL LETTER KSI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0x9f)) {               // COPTIC CAPITAL LETTER O
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa1)) {               // COPTIC CAPITAL LETTER PI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa3)) {               // COPTIC CAPITAL LETTER RO
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa5)) {               // COPTIC CAPITAL LETTER SIMA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa7)) {               // COPTIC CAPITAL LETTER TAU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xa9)) {               // COPTIC CAPITAL LETTER UA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xab)) {               // COPTIC CAPITAL LETTER FI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xad)) {               // COPTIC CAPITAL LETTER KHI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xaf)) {               // COPTIC CAPITAL LETTER PSI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb1)) {               // COPTIC CAPITAL LETTER OOU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb3)) {               // COPTIC CAPITAL LETTER DIALECT-P ALEF
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb5)) {               // COPTIC CAPITAL LETTER OLD COPTIC AIN
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb7)) {               // COPTIC CAPITAL LETTER CRYPTOGRAMMIC EIE
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xb9)) {               // COPTIC CAPITAL LETTER DIALECT-P KAPA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xbb)) {               // COPTIC CAPITAL LETTER DIALECT-P NI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xbd)) {               // COPTIC CAPITAL LETTER CRYPTOGRAMMIC NI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb2) && (old_char[2] == 0xbf)) {               // COPTIC CAPITAL LETTER OLD COPTIC OOU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb2;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x81)) {               // COPTIC CAPITAL LETTER SAMPI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x83)) {               // COPTIC CAPITAL LETTER CROSSED SHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x85)) {               // COPTIC CAPITAL LETTER OLD COPTIC SHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x87)) {               // COPTIC CAPITAL LETTER OLD COPTIC ESH
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x89)) {               // COPTIC CAPITAL LETTER AKHMIMIC KHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x8b)) {               // COPTIC CAPITAL LETTER DIALECT-P HORI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x8d)) {               // COPTIC CAPITAL LETTER OLD COPTIC HORI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x8f)) {               // COPTIC CAPITAL LETTER OLD COPTIC HA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x91)) {               // COPTIC CAPITAL LETTER L-SHAPED HA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x93)) {               // COPTIC CAPITAL LETTER OLD COPTIC HEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x95)) {               // COPTIC CAPITAL LETTER OLD COPTIC HAT
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x97)) {               // COPTIC CAPITAL LETTER OLD COPTIC GANGIA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x99)) {               // COPTIC CAPITAL LETTER OLD COPTIC DJA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x9b)) {               // COPTIC CAPITAL LETTER OLD COPTIC SHIMA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x9d)) {               // COPTIC CAPITAL LETTER OLD NUBIAN SHIMA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0x9f)) {               // COPTIC CAPITAL LETTER OLD NUBIAN NGI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xa1)) {               // COPTIC CAPITAL LETTER OLD NUBIAN NYI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xa3)) {               // COPTIC CAPITAL LETTER OLD NUBIAN WAU
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xac)) {               // COPTIC CAPITAL LETTER CRYPTOGRAMMIC SHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xae)) {               // COPTIC CAPITAL LETTER CRYPTOGRAMMIC GANGIA
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe2) && (old_char[1] == 0xb3) && (old_char[2] == 0xb3)) {               // COPTIC CAPITAL LETTER BOHAIRIC KHEI
                new_char[0]  = 0xe2;
                new_char[1]  = 0xb3;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x81)) {               // CYRILLIC CAPITAL LETTER ZEMLYA
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x83)) {               // CYRILLIC CAPITAL LETTER DZELO
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x85)) {               // CYRILLIC CAPITAL LETTER REVERSED DZE
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x87)) {               // CYRILLIC CAPITAL LETTER IOTA
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x89)) {               // CYRILLIC CAPITAL LETTER DJERV
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x8b)) {               // CYRILLIC CAPITAL LETTER MONOGRAPH UK
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x8d)) {               // CYRILLIC CAPITAL LETTER BROAD OMEGA
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x8f)) {               // CYRILLIC CAPITAL LETTER NEUTRAL YER
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x91)) {               // CYRILLIC CAPITAL LETTER YERU WITH BACK YER
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x93)) {               // CYRILLIC CAPITAL LETTER IOTIFIED YAT
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x95)) {               // CYRILLIC CAPITAL LETTER REVERSED YU
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x97)) {               // CYRILLIC CAPITAL LETTER IOTIFIED A
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x99)) {               // CYRILLIC CAPITAL LETTER CLOSED LITTLE YUS
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x9b)) {               // CYRILLIC CAPITAL LETTER BLENDED YUS
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x9d)) {               // CYRILLIC CAPITAL LETTER IOTIFIED CLOSED LITTLE YUS
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0x9f)) {               // CYRILLIC CAPITAL LETTER YN
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa1)) {               // CYRILLIC CAPITAL LETTER REVERSED TSE
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa3)) {               // CYRILLIC CAPITAL LETTER SOFT DE
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa5)) {               // CYRILLIC CAPITAL LETTER SOFT EL
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa7)) {               // CYRILLIC CAPITAL LETTER SOFT EM
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xa9)) {               // CYRILLIC CAPITAL LETTER MONOCULAR O
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xab)) {               // CYRILLIC CAPITAL LETTER BINOCULAR O
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x99) && (old_char[2] == 0xad)) {               // CYRILLIC CAPITAL LETTER DOUBLE MONOCULAR O
                new_char[0]  = 0xea;
                new_char[1]  = 0x99;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x81)) {               // CYRILLIC CAPITAL LETTER DWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x83)) {               // CYRILLIC CAPITAL LETTER DZWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x85)) {               // CYRILLIC CAPITAL LETTER ZHWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x87)) {               // CYRILLIC CAPITAL LETTER CCHE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x89)) {               // CYRILLIC CAPITAL LETTER DZZE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x8b)) {               // CYRILLIC CAPITAL LETTER TE WITH MIDDLE HOOK
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x8d)) {               // CYRILLIC CAPITAL LETTER TWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x8f)) {               // CYRILLIC CAPITAL LETTER TSWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x91)) {               // CYRILLIC CAPITAL LETTER TSSE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x93)) {               // CYRILLIC CAPITAL LETTER TCHE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x95)) {               // CYRILLIC CAPITAL LETTER HWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9a) && (old_char[2] == 0x97)) {               // CYRILLIC CAPITAL LETTER SHWE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9a;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xa3)) {               // LATIN CAPITAL LETTER EGYPTOLOGICAL ALEF
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xa5)) {               // LATIN CAPITAL LETTER EGYPTOLOGICAL AIN
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xa7)) {               // LATIN CAPITAL LETTER HENG
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xa9)) {               // LATIN CAPITAL LETTER TZ
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xab)) {               // LATIN CAPITAL LETTER TRESILLO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xad)) {               // LATIN CAPITAL LETTER CUATRILLO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xaf)) {               // LATIN CAPITAL LETTER CUATRILLO WITH COMMA
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xb3)) {               // LATIN CAPITAL LETTER AA
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xb5)) {               // LATIN CAPITAL LETTER AO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xb7)) {               // LATIN CAPITAL LETTER AU
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xb9)) {               // LATIN CAPITAL LETTER AV
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xbb)) {               // LATIN CAPITAL LETTER AV WITH HORIZONTAL BAR
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xbd)) {               // LATIN CAPITAL LETTER AY
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9c) && (old_char[2] == 0xbf)) {               // LATIN CAPITAL LETTER REVERSED C WITH DOT
                new_char[0]  = 0xea;
                new_char[1]  = 0x9c;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x81)) {               // LATIN CAPITAL LETTER K WITH STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x83)) {               // LATIN CAPITAL LETTER K WITH DIAGONAL STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x85)) {               // LATIN CAPITAL LETTER K WITH STROKE AND DIAGONAL STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x87)) {               // LATIN CAPITAL LETTER BROKEN L
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x89)) {               // LATIN CAPITAL LETTER L WITH HIGH STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x8b)) {               // LATIN CAPITAL LETTER O WITH LONG STROKE OVERLAY
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x8d)) {               // LATIN CAPITAL LETTER O WITH LOOP
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x8f)) {               // LATIN CAPITAL LETTER OO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x91)) {               // LATIN CAPITAL LETTER P WITH STROKE THROUGH DESCENDER
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x93)) {               // LATIN CAPITAL LETTER P WITH FLOURISH
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x95)) {               // LATIN CAPITAL LETTER P WITH SQUIRREL TAIL
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x94;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x97)) {               // LATIN CAPITAL LETTER Q WITH STROKE THROUGH DESCENDER
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x96;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x99)) {               // LATIN CAPITAL LETTER Q WITH DIAGONAL STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x9b)) {               // LATIN CAPITAL LETTER R ROTUNDA
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x9d)) {               // LATIN CAPITAL LETTER RUM ROTUNDA
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0x9f)) {               // LATIN CAPITAL LETTER V WITH DIAGONAL STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa1)) {               // LATIN CAPITAL LETTER VY
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa3)) {               // LATIN CAPITAL LETTER VISIGOTHIC Z
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa5)) {               // LATIN CAPITAL LETTER THORN WITH STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa7)) {               // LATIN CAPITAL LETTER THORN WITH STROKE THROUGH DESCENDER
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xa9)) {               // LATIN CAPITAL LETTER VEND
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xab)) {               // LATIN CAPITAL LETTER ET
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xad)) {               // LATIN CAPITAL LETTER IS
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xaf)) {               // LATIN CAPITAL LETTER CON
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xba)) {               // LATIN CAPITAL LETTER INSULAR D
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xbc)) {               // LATIN CAPITAL LETTER INSULAR F
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xbb;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xb5) && (old_char[2] == 0xb9)) {               // LATIN CAPITAL LETTER INSULAR G
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xbd;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9d) && (old_char[2] == 0xbf)) {               // LATIN CAPITAL LETTER TURNED INSULAR G
                new_char[0]  = 0xea;
                new_char[1]  = 0x9d;
                new_char[2]  = 0xbe;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x81)) {               // LATIN CAPITAL LETTER TURNED L
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x80;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x83)) {               // LATIN CAPITAL LETTER INSULAR R
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x82;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x85)) {               // LATIN CAPITAL LETTER INSULAR S
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x84;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x87)) {               // LATIN CAPITAL LETTER INSULAR T
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x86;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x8c)) {               // LATIN CAPITAL LETTER SALTILLO
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xa5)) {               // LATIN CAPITAL LETTER TURNED H
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x91)) {               // LATIN CAPITAL LETTER N WITH DESCENDER
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x90;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0x93)) {               // LATIN CAPITAL LETTER C WITH BAR
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0x92;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa1)) {               // LATIN CAPITAL LETTER G WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa3)) {               // LATIN CAPITAL LETTER K WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa5)) {               // LATIN CAPITAL LETTER N WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa7)) {               // LATIN CAPITAL LETTER R WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xea) && (old_char[1] == 0x9e) && (old_char[2] == 0xa9)) {               // LATIN CAPITAL LETTER S WITH OBLIQUE STROKE
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xc9) && (old_char[1] == 0xa6)) {               // LATIN CAPITAL LETTER H WITH HOOK
                new_char[0]  = 0xea;
                new_char[1]  = 0x9e;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x81)) {               // FULLWIDTH LATIN CAPITAL LETTER A
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x82)) {               // FULLWIDTH LATIN CAPITAL LETTER B
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x83)) {               // FULLWIDTH LATIN CAPITAL LETTER C
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x84)) {               // FULLWIDTH LATIN CAPITAL LETTER D
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x85)) {               // FULLWIDTH LATIN CAPITAL LETTER E
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x86)) {               // FULLWIDTH LATIN CAPITAL LETTER F
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x87)) {               // FULLWIDTH LATIN CAPITAL LETTER G
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x88)) {               // FULLWIDTH LATIN CAPITAL LETTER H
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x89)) {               // FULLWIDTH LATIN CAPITAL LETTER I
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x8a)) {               // FULLWIDTH LATIN CAPITAL LETTER J
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x8b)) {               // FULLWIDTH LATIN CAPITAL LETTER K
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x8c)) {               // FULLWIDTH LATIN CAPITAL LETTER L
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x8d)) {               // FULLWIDTH LATIN CAPITAL LETTER M
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x8e)) {               // FULLWIDTH LATIN CAPITAL LETTER N
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x8f)) {               // FULLWIDTH LATIN CAPITAL LETTER O
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x90)) {               // FULLWIDTH LATIN CAPITAL LETTER P
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb0;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x91)) {               // FULLWIDTH LATIN CAPITAL LETTER Q
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb1;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x92)) {               // FULLWIDTH LATIN CAPITAL LETTER R
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb2;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x93)) {               // FULLWIDTH LATIN CAPITAL LETTER S
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb3;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x94)) {               // FULLWIDTH LATIN CAPITAL LETTER T
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb4;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x95)) {               // FULLWIDTH LATIN CAPITAL LETTER U
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb5;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x96)) {               // FULLWIDTH LATIN CAPITAL LETTER V
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb6;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x97)) {               // FULLWIDTH LATIN CAPITAL LETTER W
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb7;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x98)) {               // FULLWIDTH LATIN CAPITAL LETTER X
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x99)) {               // FULLWIDTH LATIN CAPITAL LETTER Y
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xb9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xef) && (old_char[1] == 0xbd) && (old_char[2] == 0x9a)) {               // FULLWIDTH LATIN CAPITAL LETTER Z
                new_char[0]  = 0xef;
                new_char[1]  = 0xbc;
                new_char[2]  = 0xba;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa8)) {               // DESERET CAPITAL LETTER LONG I
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x80;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xa9)) {               // DESERET CAPITAL LETTER LONG E
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x81;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xaa)) {               // DESERET CAPITAL LETTER LONG A
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x82;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xab)) {               // DESERET CAPITAL LETTER LONG AH
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x83;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xac)) {               // DESERET CAPITAL LETTER LONG O
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x84;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xad)) {               // DESERET CAPITAL LETTER LONG OO
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x85;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xae)) {               // DESERET CAPITAL LETTER SHORT I
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x86;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xaf)) {               // DESERET CAPITAL LETTER SHORT E
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x87;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb0)) {               // DESERET CAPITAL LETTER SHORT A
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x88;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb1)) {               // DESERET CAPITAL LETTER SHORT AH
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x89;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb2)) {               // DESERET CAPITAL LETTER SHORT O
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x8a;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb3)) {               // DESERET CAPITAL LETTER SHORT OO
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x8b;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb4)) {               // DESERET CAPITAL LETTER AY
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x8c;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb5)) {               // DESERET CAPITAL LETTER OW
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x8d;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb6)) {               // DESERET CAPITAL LETTER WU
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x8e;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb7)) {               // DESERET CAPITAL LETTER YEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x8f;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb8)) {               // DESERET CAPITAL LETTER H
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x90;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xb9)) {               // DESERET CAPITAL LETTER PEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x91;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xba)) {               // DESERET CAPITAL LETTER BEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x92;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xbb)) {               // DESERET CAPITAL LETTER TEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x93;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xbc)) {               // DESERET CAPITAL LETTER DEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x94;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xbd)) {               // DESERET CAPITAL LETTER CHEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x95;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xbe)) {               // DESERET CAPITAL LETTER JEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x96;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x90) && (old_char[3] == 0xbf)) {               // DESERET CAPITAL LETTER KAY
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x97;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x80)) {               // DESERET CAPITAL LETTER GAY
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x98;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x81)) {               // DESERET CAPITAL LETTER EF
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x99;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x82)) {               // DESERET CAPITAL LETTER VEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x9a;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x83)) {               // DESERET CAPITAL LETTER ETH
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x9b;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x84)) {               // DESERET CAPITAL LETTER THEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x9c;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x85)) {               // DESERET CAPITAL LETTER ES
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x9d;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x86)) {               // DESERET CAPITAL LETTER ZEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x9e;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x87)) {               // DESERET CAPITAL LETTER ESH
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0x9f;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x88)) {               // DESERET CAPITAL LETTER ZHEE
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa0;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x89)) {               // DESERET CAPITAL LETTER ER
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa1;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x8a)) {               // DESERET CAPITAL LETTER EL
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa2;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x8b)) {               // DESERET CAPITAL LETTER EM
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa3;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x8c)) {               // DESERET CAPITAL LETTER EN
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa4;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x8d)) {               // DESERET CAPITAL LETTER ENG
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa5;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x8e)) {               // DESERET CAPITAL LETTER OI
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa6;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xf0) && (old_char[1] == 0x90) && (old_char[2] == 0x91) && (old_char[3] == 0x8f)) {               // DESERET CAPITAL LETTER EW
                new_char[0]  = 0xf0;
                new_char[1]  = 0x90;
                new_char[2]  = 0x90;
                new_char[3]  = 0xa7;
                new_char_len = 4;
                old_char_len = 4;
            } else
            if ((old_char[0] == 0xc3) && (old_char[1] == 0x9f)) {                // LATIN CAPITAL LETTER SHARP S
                new_char[0]  = 0xe1;
                new_char[1]  = 0xba;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 2;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x80)) {                // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x88;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x81)) {                // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x89;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x82)) {                // GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x8a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x83)) {                // GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x8b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x84)) {                // GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x85)) {                // GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x8d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x86)) {                // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x8e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x87)) {                // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x8f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x90)) {                // GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x98;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x91)) {                // GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x99;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x92)) {                // GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x9a;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x93)) {                // GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x9b;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x94)) {                // GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x9c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x95)) {                // GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x9d;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x96)) {                // GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x9e;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0x97)) {                // GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0x9f;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa0)) {                // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa8;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa1)) {                // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xa9;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa2)) {                // GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xaa;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa3)) {                // GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xab;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa4)) {                // GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xac;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa5)) {                // GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xad;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa6)) {                // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xae;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xa7)) {                // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xaf;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbe) && (old_char[2] == 0xb3)) {                // GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbe;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0x83)) {                // GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0x8c;
                new_char_len = 3;
                old_char_len = 3;
            } else
            if ((old_char[0] == 0xe1) && (old_char[1] == 0xbf) && (old_char[2] == 0xb3)) {                // GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
                new_char[0]  = 0xe1;
                new_char[1]  = 0xbf;
                new_char[2]  = 0xbc;
                new_char_len = 3;
                old_char_len = 3;
            }
        }

        if (!old_char_len) {
            int pre_offs = offs;
            //(isutf(str[++offs]) && (length--)) || (isutf(str[++offs]) && (length--)) || (isutf(str[++offs]) && (length--)) || ((++offs) && (length--));
            (void)(isutf(str[++offs]) || isutf(str[++offs]) ||
                   isutf(str[++offs]) || ++offs);

            old_char_len = offs - pre_offs;

            for (int j = 0; j < old_char_len; j++)
                new_char[j] = str[pre_offs + j];

            new_char_len = old_char_len;
        } else
            offs += old_char_len;

        for (int j = 0; j < new_char_len; j++)
            out[out_offset++] = new_char[j];
        length -= old_char_len;
    }
    out[out_offset] = 0;
    return out_offset;
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(UTF8ToLower, 1, 2)
    T_STRING(UTF8ToLower, 0)
    bool turkic = false;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(UTF8ToLower, 1)
        turkic = (bool)PARAM_INT(1);
    }

    INTEGER blen = PARAM_LEN(0);
    if (blen) {
        char *target = 0;
        CORE_NEW(blen * 3 + 1, target);
        target[blen] = 0;

        if (!target) {
            RETURN_STRING("");
            return 0;
        }
        int len = toLower(PARAM(0), blen, target, turkic);
        SetVariable(RESULT, -1, target, len);
    } else
        RETURN_STRING("");
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(UTF8ToUpper, 1, 2)
    T_STRING(UTF8ToUpper, 0)
    bool turkic = false;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(UTF8ToUpper, 1)
        turkic = (bool)PARAM_INT(1);
    }
    INTEGER blen = PARAM_LEN(0);
    if (blen) {
        char *target = 0;
        CORE_NEW(blen * 3 + 1, target);
        target[blen] = 0;

        if (!target) {
            RETURN_STRING("");
            return 0;
        }
        int len = toUpper(PARAM(0), blen, target, turkic);
        SetVariable(RESULT, -1, target, len);
    } else
        RETURN_STRING("");
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Soundex, 1)
    T_STRING(Soundex, 0)
    char *in = PARAM(0);
    static int code[] =
    { 0, 1, 2, 3, 0, 1, 2, 0, 0, 2, 2, 4, 5, 5, 0, 1, 2, 6, 2, 3, 0, 1, 0, 2, 0, 2 };
/* a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z */
    char   key[5];
    register char ch;
    register int  last;
    register int  count;

/* Set up default key, complete with trailing '0's */
    strcpy(key, "Z000");

/* Advance to the first letter.  If none present,
    return default key */
    if (!in) {
        RETURN_STRING("");
        return 0;
    }
    while (*in != '\0' && !isalpha(*in))
        ++in;
    if (*in == '\0') {
        RETURN_STRING(key);
        //return key;
        return 0;
    }

/* Pull out the first letter, uppercase it, and
    set up for main loop */
    key[0] = toupper(*in);
    last   = code[key[0] - 'A'];
    ++in;

/* Scan rest of string, stop at end of string or
    when the key is full */
    for (count = 1; count < 4 && *in != '\0'; ++in) {
        /* If non-alpha, ignore the character altogether */
        if (isalpha(*in)) {
            ch = tolower(*in);
            /* Fold together adjacent letters sharing the same code */
            if (last != code[ch - 'a']) {
                last = code[ch - 'a'];
                /* Ignore code==0 letters except as separators */
                if (last != 0)
                    key[count++] = '0' + last;
            }
        }
    }
    RETURN_STRING(key);
END_IMPL
//---------------------------------------------------------------------------
void phonetic(char *name, char *metaph, int metalen) {
    char *VOWELS = (char *)"AEIOU",
         *FRONTV = (char *)"EIY",     /* special cases for letters in FRONT of these */
         *VARSON = (char *)"CSPTG",   /* variable sound--those modified by adding an "h"    */
         *DOUBLE = (char *)".";       /* let these double letters through */

    char *excpPAIR = (char *)"AGKPW", /* exceptions "ae-", "gn-", "kn-", "pn-", "wr-" */
         *nextLTR  = (char *)"ENNNR";
    char *chrptr, *chrptr1;

    int ii, jj, silent, hard, Lng, lastChr;

    char curLtr, prevLtr, nextLtr, nextLtr2, nextLtr3;

    int vowelAfter, vowelBefore, frontvAfter;

    char wname[60];
    char *ename = wname;

    jj = 0;
    for (ii = 0; name[ii] != '\0'; ii++) {
        if (isalpha(name[ii])) {
            ename[jj] = toupper(name[ii]);
            jj++;
        }
    }
    ename[jj] = '\0';

    if (strlen(ename) == 0) return;

    /* if ae, gn, kn, pn, wr then drop the first letter */
    if ((chrptr = strchr(excpPAIR, ename[0])) != NULLCHAR) {
        chrptr1 = nextLTR + (chrptr - excpPAIR);
        if (*chrptr1 == ename[1]) strcpy(ename, &ename[1]);
    }
    /* change x to s */
    if (ename[0] == 'X') ename[0] = 'S';
    /* get rid of the "h" in "wh" */
    if (strncmp(ename, "WH", 2) == 0) strcpy(&ename[1], &ename[2]);

    Lng     = strlen(ename);
    lastChr = Lng - 1;      /* index to last character in string makes code easier*/

    /* Remove an S from the end of the string */
    if (ename[lastChr] == 'S') {
        ename[lastChr] = '\0';
        Lng            = strlen(ename);
        lastChr        = Lng - 1;
    }

    for (ii = 0; ((strlen(metaph) < metalen) && (ii < Lng)); ii++) {
        curLtr = ename[ii];

        vowelBefore = (0);
        prevLtr     = ' ';
        if (ii > 0) {
            prevLtr = ename[ii - 1];
            if (strchr(VOWELS, prevLtr) != NULLCHAR) vowelBefore = (1);
        }
        /* if first letter is a vowel KEEP it */
        if ((ii == 0) && (strchr(VOWELS, curLtr) != NULLCHAR)) {
            strncat(metaph, &curLtr, 1);
            continue;
        }

        vowelAfter  = (0);
        frontvAfter = (0);
        nextLtr     = ' ';
        if (ii < lastChr) {
            nextLtr = ename[ii + 1];
            if (strchr(VOWELS, nextLtr) != NULLCHAR) vowelAfter = (1);
            if (strchr(FRONTV, nextLtr) != NULLCHAR) frontvAfter = (1);
        }
        /* skip double letters except ones in list */
        if ((curLtr == nextLtr) && (strchr(DOUBLE, nextLtr) == NULLCHAR)) continue;

        nextLtr2 = ' ';
        if (ii < (lastChr - 1)) nextLtr2 = ename[ii + 2];

        nextLtr3 = ' ';
        if (ii < (lastChr - 2)) nextLtr3 = ename[ii + 3];

        switch (curLtr) {
            case 'B':
                silent = (0);
                if ((ii == lastChr) && (prevLtr == 'M')) silent = (1);
                if (!silent) strncat(metaph, &curLtr, 1);
                break;

            /*silent -sci-,-sce-,-scy-;  sci-, etc OK*/
            case 'C':
                if (!((ii > 1) && (prevLtr == 'S') && frontvAfter)) {
                    if ((ii > 0) && (nextLtr == 'I') && (nextLtr2 == 'A'))
                        strcat(metaph, "X");
                    else
                    if (frontvAfter)
                        strcat(metaph, "S");
                    else
                    if ((ii > 1) && (prevLtr == 'S') && (nextLtr == 'H'))
                        strcat(metaph, "K");
                    else
                    if (nextLtr == 'H')
                        if ((ii == 0) && (strchr(VOWELS, nextLtr2) == NULLCHAR))
                            strcat(metaph, "K");
                        else
                            strcat(metaph, "X");
                    else
                    if (prevLtr == 'C')
                        strcat(metaph, "C");
                    else
                        strcat(metaph, "K");
                }
                break;

            case 'D':
                if ((nextLtr == 'G') && (strchr(FRONTV, nextLtr2) != NULLCHAR))
                    strcat(metaph, "J");
                else
                    strcat(metaph, "T");
                break;

            case 'G':
                silent = (0);
                /* SILENT -gh- except for -gh and no vowel after h */
                if (((ii < (lastChr - 1)) && (nextLtr == 'H')) &&
                    (strchr(VOWELS, nextLtr2) == NULLCHAR))
                    silent = (1);

                if ((ii == (lastChr - 3)) &&
                    (nextLtr == 'N') && (nextLtr2 == 'E') && (nextLtr3 == 'D'))
                    silent = (1);
                else
                if ((ii == (lastChr - 1)) && (nextLtr == 'N')) silent = (1);

                if ((prevLtr == 'D') && frontvAfter) silent = (1);

                if (prevLtr == 'G')
                    hard = (1);
                else
                    hard = (0);

                if (!silent) {
                    if (frontvAfter && (!hard))
                        strcat(metaph, "J");
                    else
                        strcat(metaph, "K");
                }
                break;

            case 'H':
                silent = (0);
                if (strchr(VARSON, prevLtr) != NULLCHAR) silent = (1);

                if (vowelBefore && !vowelAfter) silent = (1);

                if (!silent) strncat(metaph, &curLtr, 1);
                break;

            case 'F':
            case 'J':
            case 'L':
            case 'M':
            case 'N':
            case 'R':
                strncat(metaph, &curLtr, 1);
                break;

            case 'K':
                if (prevLtr != 'C') strncat(metaph, &curLtr, 1);
                break;

            case 'P':
                if (nextLtr == 'H')
                    strcat(metaph, "F");
                else
                    strcat(metaph, "P");
                break;

            case 'Q':
                strcat(metaph, "K");
                break;

            case 'S':
                if ((ii > 1) && (nextLtr == 'I') &&
                    ((nextLtr2 == 'O') || (nextLtr2 == 'A')))
                    strcat(metaph, "X");
                else
                if (nextLtr == 'H')
                    strcat(metaph, "X");
                else
                    strcat(metaph, "S");
                break;

            case 'T':
                if ((ii > 1) && (nextLtr == 'I') &&
                    ((nextLtr2 == 'O') || (nextLtr2 == 'A')))
                    strcat(metaph, "X");
                else
                if (nextLtr == 'H')             /* The=0, Tho=T, Withrow=0 */
                    if ((ii > 0) || (strchr(VOWELS, nextLtr2) != NULLCHAR))
                        strcat(metaph, "0");
                    else
                        strcat(metaph, "T");
                else
                if (!((ii < (lastChr - 2)) && (nextLtr == 'C') && (nextLtr2 == 'H')))
                    strcat(metaph, "T");
                break;

            case 'V':
                strcat(metaph, "F");
                break;

            case 'W':
            case 'Y':
                if ((ii < lastChr) && vowelAfter) strncat(metaph, &curLtr, 1);
                break;

            case 'X':
                strcat(metaph, "KS");
                break;

            case 'Z':
                strcat(metaph, "S");
                break;
        }
    }

    /*  DON'T DO THIS NOW, REMOVING "S" IN BEGINNING HAS the same effect
        with plurals, in addition imbedded S's in the Metaphone are included
        Lng = strlen(metaph);
        lastChr = Lng -1;
        if ( metaph[lastChr] == 'S' && Lng >= 3 ) metaph[lastChr] = '\0';
     */

    return;
}

CONCEPT_FUNCTION_IMPL(Metaphone, 1)
    T_STRING(Metaphone, 0)
    char *name = PARAM(0);
    char metaph[0xFFF];

    metaph[0] = '\0';
    phonetic(name, metaph, 0xFFF);

    RETURN_STRING(metaph);
END_IMPL
//---------------------------------------------------------------------------
metastring *NewMetaString(const char *init_str) {
    metastring *s;
    const char empty_string[] = "";

    META_MALLOC(s, 1, metastring);
    if (!s)
        return NULL;

    if (init_str == NULL)
        init_str = empty_string;
    s->length = strlen(init_str);
    /* preallocate a bit more for potential growth */
    s->bufsize = s->length + 7;

    META_MALLOC(s->str, s->bufsize, char);
    if (!s->str)
        return NULL;

    strncpy(s->str, init_str, s->length + 1);
    s->free_string_on_destroy = 1;

    return s;
}

void DestroyMetaString(metastring *s) {
    if (s == NULL)
        return;

    if (s->free_string_on_destroy && (s->str != NULL))
        META_FREE(s->str);

    META_FREE(s);
}

void IncreaseBuffer(metastring *s, int chars_needed) {
    META_REALLOC(s->str, (s->bufsize + chars_needed + 10), char);
    if (!s->str)
        return;
    s->bufsize = s->bufsize + chars_needed + 10;
}

void MakeUpper(metastring *s) {
    char *i;

    for (i = s->str; *i; i++) {
        *i = toupper(*i);
    }
}

int IsVowel(metastring *s, int pos) {
    char c;

    if ((pos < 0) || (pos >= s->length))
        return 0;

    c = *(s->str + pos);
    if ((c == 'A') || (c == 'E') || (c == 'I') || (c == 'O') ||
        (c == 'U') || (c == 'Y'))
        return 1;

    return 0;
}

int SlavoGermanic(metastring *s) {
    if ((char *)strstr(s->str, "W"))
        return 1;
    else if ((char *)strstr(s->str, "K"))
        return 1;
    else if ((char *)strstr(s->str, "CZ"))
        return 1;
    else if ((char *)strstr(s->str, "WITZ"))
        return 1;
    else
        return 0;
}

int GetLength(metastring *s) {
    return s->length;
}

char GetAt(metastring *s, int pos) {
    if ((pos < 0) || (pos >= s->length))
        return '\0';

    return (char)*(s->str + pos);
}

void SetAt(metastring *s, int pos, char c) {
    if ((pos < 0) || (pos >= s->length))
        return;

    *(s->str + pos) = c;
}

/*
   Caveats: the START value is 0 based
 */
int StringAt(metastring *s, int start, int length, ...) {
    char    *test;
    char    *pos;
    va_list ap;

    if ((start < 0) || (start >= s->length))
        return 0;

    pos = (s->str + start);
    va_start(ap, length);

    do {
        test = va_arg(ap, char *);
        if (*test && (strncmp(pos, test, length) == 0))
            return 1;
    } while (strcmp(test, ""));

    va_end(ap);

    return 0;
}

void MetaphAdd(metastring *s, const char *new_str) {
    int add_length;

    if (new_str == NULL)
        return;

    add_length = strlen(new_str);
    if ((s->length + add_length) > (s->bufsize - 1)) {
        IncreaseBuffer(s, add_length);
    }

    strcat(s->str, new_str);
    s->length += add_length;
}

void DoubleMetaphone(char *str, int length, char **codes) {
    metastring *original;
    metastring *primary;
    metastring *secondary;
    int        current;
    int        last;

    current = 0;
    /* we need the real length and last prior to padding */
    //length  = strlen(str);
    last     = length - 1;
    original = NewMetaString(str);
    /* Pad original so we can index beyond end */
    MetaphAdd(original, "     ");

    primary   = NewMetaString("");
    secondary = NewMetaString("");
    primary->free_string_on_destroy   = 0;
    secondary->free_string_on_destroy = 0;

    MakeUpper(original);

    /* skip these when at start of word */
    if (StringAt(original, 0, 2, "GN", "KN", "PN", "WR", "PS", ""))
        current += 1;

    /* Initial 'X' is pronounced 'Z' e.g. 'Xavier' */
    if (GetAt(original, 0) == 'X') {
        MetaphAdd(primary, "S");        /* 'Z' maps to 'S' */
        MetaphAdd(secondary, "S");
        current += 1;
    }

    /* main loop */
    while ((primary->length < 4) || (secondary->length < 4)) {
        if (current >= length)
            break;

        switch (GetAt(original, current)) {
            case 'A':
            case 'E':
            case 'I':
            case 'O':
            case 'U':
            case 'Y':
                if (current == 0) {
                    /* all init vowels now map to 'A' */
                    MetaphAdd(primary, "A");
                    MetaphAdd(secondary, "A");
                }
                current += 1;
                break;

            case 'B':

                /* "-mb", e.g", "dumb", already skipped over... */
                MetaphAdd(primary, "P");
                MetaphAdd(secondary, "P");

                if (GetAt(original, current + 1) == 'B')
                    current += 2;
                else
                    current += 1;
                break;

            case '\xC7':
                MetaphAdd(primary, "S");
                MetaphAdd(secondary, "S");
                current += 1;
                break;

            case 'C':
                /* various germanic */
                if ((current > 1) &&
                    !IsVowel(original, current - 2) &&
                    StringAt(original, (current - 1), 3, "ACH", "") &&
                    ((GetAt(original, current + 2) != 'I') &&
                     ((GetAt(original, current + 2) != 'E') ||
                      StringAt(original, (current - 2), 6, "BACHER",
                               "MACHER", "")))) {
                    MetaphAdd(primary, "K");
                    MetaphAdd(secondary, "K");
                    current += 2;
                    break;
                }

                /* special case 'caesar' */
                if ((current == 0) &&
                    StringAt(original, current, 6, "CAESAR", "")) {
                    MetaphAdd(primary, "S");
                    MetaphAdd(secondary, "S");
                    current += 2;
                    break;
                }

                /* italian 'chianti' */
                if (StringAt(original, current, 4, "CHIA", "")) {
                    MetaphAdd(primary, "K");
                    MetaphAdd(secondary, "K");
                    current += 2;
                    break;
                }

                if (StringAt(original, current, 2, "CH", "")) {
                    /* find 'michael' */
                    if ((current > 0) &&
                        StringAt(original, current, 4, "CHAE", "")) {
                        MetaphAdd(primary, "K");
                        MetaphAdd(secondary, "X");
                        current += 2;
                        break;
                    }

                    /* greek roots e.g. 'chemistry', 'chorus' */
                    if ((current == 0) &&
                        (StringAt(original, (current + 1), 5, "HARAC", "HARIS", "") ||
                         StringAt(original, (current + 1), 3, "HOR",
                                  "HYM", "HIA", "HEM", "")) &&
                        !StringAt(original, 0, 5, "CHORE", "")) {
                        MetaphAdd(primary, "K");
                        MetaphAdd(secondary, "K");
                        current += 2;
                        break;
                    }

                    /* germanic, greek, or otherwise 'ch' for 'kh' sound */
                    if (
                        (StringAt(original, 0, 4, "VAN ", "VON ", "") ||
                         StringAt(original, 0, 3, "SCH", ""))
                        /*  'architect but not 'arch', 'orchestra', 'orchid' */
                        || StringAt(original, (current - 2), 6, "ORCHES",
                                    "ARCHIT", "ORCHID", "") ||
                        StringAt(original, (current + 2), 1, "T", "S",
                                 "") ||
                        ((StringAt(original, (current - 1), 1, "A", "O", "U", "E", "") ||
                          (current == 0))
                         /* e.g., 'wachtler', 'wechsler', but not 'tichner' */
                         && StringAt(original, (current + 2), 1, "L", "R",
                                     "N", "M", "B", "H", "F", "V", "W", " ", ""))) {
                        MetaphAdd(primary, "K");
                        MetaphAdd(secondary, "K");
                    } else {
                        if (current > 0) {
                            if (StringAt(original, 0, 2, "MC", "")) {
                                /* e.g., "McHugh" */
                                MetaphAdd(primary, "K");
                                MetaphAdd(secondary, "K");
                            } else {
                                MetaphAdd(primary, "X");
                                MetaphAdd(secondary, "K");
                            }
                        } else {
                            MetaphAdd(primary, "X");
                            MetaphAdd(secondary, "X");
                        }
                    }
                    current += 2;
                    break;
                }
                /* e.g, 'czerny' */
                if (StringAt(original, current, 2, "CZ", "") &&
                    !StringAt(original, (current - 2), 4, "WICZ", "")) {
                    MetaphAdd(primary, "S");
                    MetaphAdd(secondary, "X");
                    current += 2;
                    break;
                }

                /* e.g., 'focaccia' */
                if (StringAt(original, (current + 1), 3, "CIA", "")) {
                    MetaphAdd(primary, "X");
                    MetaphAdd(secondary, "X");
                    current += 3;
                    break;
                }

                /* double 'C', but not if e.g. 'McClellan' */
                if (StringAt(original, current, 2, "CC", "") &&
                    !((current == 1) && (GetAt(original, 0) == 'M'))) {
                    /* 'bellocchio' but not 'bacchus' */
                    if (StringAt(original, (current + 2), 1, "I", "E", "H", "") &&
                        !StringAt(original, (current + 2), 2, "HU", "")) {
                        /* 'accident', 'accede' 'succeed' */
                        if (
                            ((current == 1) &&
                             (GetAt(original, current - 1) == 'A')) ||
                            StringAt(original, (current - 1), 5, "UCCEE",
                                     "UCCES", "")) {
                            MetaphAdd(primary, "KS");
                            MetaphAdd(secondary, "KS");
                            /* 'bacci', 'bertucci', other italian */
                        } else {
                            MetaphAdd(primary, "X");
                            MetaphAdd(secondary, "X");
                        }
                        current += 3;
                        break;
                    } else {    /* Pierce's rule */
                        MetaphAdd(primary, "K");
                        MetaphAdd(secondary, "K");
                        current += 2;
                        break;
                    }
                }

                if (StringAt(original, current, 2, "CK", "CG", "CQ", "")) {
                    MetaphAdd(primary, "K");
                    MetaphAdd(secondary, "K");
                    current += 2;
                    break;
                }

                if (StringAt(original, current, 2, "CI", "CE", "CY", "")) {
                    /* italian vs. english */
                    if (StringAt
                            (original, current, 3, "CIO", "CIE", "CIA", "")) {
                        MetaphAdd(primary, "S");
                        MetaphAdd(secondary, "X");
                    } else {
                        MetaphAdd(primary, "S");
                        MetaphAdd(secondary, "S");
                    }
                    current += 2;
                    break;
                }

                /* else */
                MetaphAdd(primary, "K");
                MetaphAdd(secondary, "K");

                /* name sent in 'mac caffrey', 'mac gregor */
                if (StringAt(original, (current + 1), 2, " C", " Q", " G", ""))
                    current += 3;
                else
                if (StringAt(original, (current + 1), 1, "C", "K", "Q", "") &&
                    !StringAt(original, (current + 1), 2, "CE", "CI", ""))
                    current += 2;
                else
                    current += 1;
                break;

            case 'D':
                if (StringAt(original, current, 2, "DG", "")) {
                    if (StringAt(original, (current + 2), 1, "I", "E", "Y", "")) {
                        /* e.g. 'edge' */
                        MetaphAdd(primary, "J");
                        MetaphAdd(secondary, "J");
                        current += 3;
                        break;
                    } else {
                        /* e.g. 'edgar' */
                        MetaphAdd(primary, "TK");
                        MetaphAdd(secondary, "TK");
                        current += 2;
                        break;
                    }
                }

                if (StringAt(original, current, 2, "DT", "DD", "")) {
                    MetaphAdd(primary, "T");
                    MetaphAdd(secondary, "T");
                    current += 2;
                    break;
                }

                /* else */
                MetaphAdd(primary, "T");
                MetaphAdd(secondary, "T");
                current += 1;
                break;

            case 'F':
                if (GetAt(original, current + 1) == 'F')
                    current += 2;
                else
                    current += 1;
                MetaphAdd(primary, "F");
                MetaphAdd(secondary, "F");
                break;

            case 'G':
                if (GetAt(original, current + 1) == 'H') {
                    if ((current > 0) && !IsVowel(original, current - 1)) {
                        MetaphAdd(primary, "K");
                        MetaphAdd(secondary, "K");
                        current += 2;
                        break;
                    }

                    if (current < 3) {
                        /* 'ghislane', ghiradelli */
                        if (current == 0) {
                            if (GetAt(original, current + 2) == 'I') {
                                MetaphAdd(primary, "J");
                                MetaphAdd(secondary, "J");
                            } else {
                                MetaphAdd(primary, "K");
                                MetaphAdd(secondary, "K");
                            }
                            current += 2;
                            break;
                        }
                    }
                    /* Parker's rule (with some further refinements) - e.g., 'hugh' */
                    if (
                        ((current > 1) &&
                         StringAt(original, (current - 2), 1, "B", "H", "D", ""))
                        /* e.g., 'bough' */
                        || ((current > 2) &&
                            StringAt(original, (current - 3), 1, "B", "H", "D", ""))
                        /* e.g., 'broughton' */
                        || ((current > 3) &&
                            StringAt(original, (current - 4), 1, "B", "H", ""))) {
                        current += 2;
                        break;
                    } else {
                        /* e.g., 'laugh', 'McLaughlin', 'cough', 'gough', 'rough', 'tough' */
                        if ((current > 2) &&
                            (GetAt(original, current - 1) == 'U') &&
                            StringAt(original, (current - 3), 1, "C",
                                     "G", "L", "R", "T", "")) {
                            MetaphAdd(primary, "F");
                            MetaphAdd(secondary, "F");
                        } else if ((current > 0) &&
                                   (GetAt(original, current - 1) != 'I')) {
                            MetaphAdd(primary, "K");
                            MetaphAdd(secondary, "K");
                        }

                        current += 2;
                        break;
                    }
                }

                if (GetAt(original, current + 1) == 'N') {
                    if ((current == 1) && IsVowel(original, 0) &&
                        !SlavoGermanic(original)) {
                        MetaphAdd(primary, "KN");
                        MetaphAdd(secondary, "N");
                    } else
                    /* not e.g. 'cagney' */
                    if (!StringAt(original, (current + 2), 2, "EY", "") &&
                        (GetAt(original, current + 1) != 'Y') &&
                        !SlavoGermanic(original)) {
                        MetaphAdd(primary, "N");
                        MetaphAdd(secondary, "KN");
                    } else {
                        MetaphAdd(primary, "KN");
                        MetaphAdd(secondary, "KN");
                    }
                    current += 2;
                    break;
                }

                /* 'tagliaro' */
                if (StringAt(original, (current + 1), 2, "LI", "") &&
                    !SlavoGermanic(original)) {
                    MetaphAdd(primary, "KL");
                    MetaphAdd(secondary, "L");
                    current += 2;
                    break;
                }

                /* -ges-,-gep-,-gel-, -gie- at beginning */
                if ((current == 0) &&
                    ((GetAt(original, current + 1) == 'Y') ||
                     StringAt(original, (current + 1), 2, "ES", "EP",
                              "EB", "EL", "EY", "IB", "IL", "IN", "IE",
                              "EI", "ER", ""))) {
                    MetaphAdd(primary, "K");
                    MetaphAdd(secondary, "J");
                    current += 2;
                    break;
                }

                /*  -ger-,  -gy- */
                if (
                    (StringAt(original, (current + 1), 2, "ER", "") ||
                     (GetAt(original, current + 1) == 'Y')) &&
                    !StringAt(original, 0, 6, "DANGER", "RANGER", "MANGER", "") &&
                    !StringAt(original, (current - 1), 1, "E", "I", "") &&
                    !StringAt(original, (current - 1), 3, "RGY", "OGY",
                              "")) {
                    MetaphAdd(primary, "K");
                    MetaphAdd(secondary, "J");
                    current += 2;
                    break;
                }

                /*  italian e.g, 'biaggi' */
                if (StringAt(original, (current + 1), 1, "E", "I", "Y", "") ||
                    StringAt(original, (current - 1), 4, "AGGI", "OGGI", "")) {
                    /* obvious germanic */
                    if (
                        (StringAt(original, 0, 4, "VAN ", "VON ", "") ||
                         StringAt(original, 0, 3, "SCH", "")) ||
                        StringAt(original, (current + 1), 2, "ET", "")) {
                        MetaphAdd(primary, "K");
                        MetaphAdd(secondary, "K");
                    } else {
                        /* always soft if french ending */
                        if (StringAt
                                (original, (current + 1), 4, "IER ", "")) {
                            MetaphAdd(primary, "J");
                            MetaphAdd(secondary, "J");
                        } else {
                            MetaphAdd(primary, "J");
                            MetaphAdd(secondary, "K");
                        }
                    }
                    current += 2;
                    break;
                }

                if (GetAt(original, current + 1) == 'G')
                    current += 2;
                else
                    current += 1;
                MetaphAdd(primary, "K");
                MetaphAdd(secondary, "K");
                break;

            case 'H':
                /* only keep if first & before vowel or btw. 2 vowels */
                if (((current == 0) || IsVowel(original, current - 1)) &&
                    IsVowel(original, current + 1)) {
                    MetaphAdd(primary, "H");
                    MetaphAdd(secondary, "H");
                    current += 2;
                } else          /* also takes care of 'HH' */
                    current += 1;
                break;

            case 'J':
                /* obvious spanish, 'jose', 'san jacinto' */
                if (StringAt(original, current, 4, "JOSE", "") ||
                    StringAt(original, 0, 4, "SAN ", "")) {
                    if (((current == 0) &&
                         (GetAt(original, current + 4) == ' ')) ||
                        StringAt(original, 0, 4, "SAN ", "")) {
                        MetaphAdd(primary, "H");
                        MetaphAdd(secondary, "H");
                    } else {
                        MetaphAdd(primary, "J");
                        MetaphAdd(secondary, "H");
                    }
                    current += 1;
                    break;
                }

                if ((current == 0) &&
                    !StringAt(original, current, 4, "JOSE", "")) {
                    MetaphAdd(primary, "J");    /* Yankelovich/Jankelowicz */
                    MetaphAdd(secondary, "A");
                } else {
                    /* spanish pron. of e.g. 'bajador' */
                    if (IsVowel(original, current - 1) &&
                        !SlavoGermanic(original) &&
                        ((GetAt(original, current + 1) == 'A') ||
                         (GetAt(original, current + 1) == 'O'))) {
                        MetaphAdd(primary, "J");
                        MetaphAdd(secondary, "H");
                    } else {
                        if (current == last) {
                            MetaphAdd(primary, "J");
                            MetaphAdd(secondary, "");
                        } else {
                            if (!StringAt(original, (current + 1), 1, "L", "T",
                                          "K", "S", "N", "M", "B", "Z", "") &&
                                !StringAt(original, (current - 1), 1,
                                          "S", "K", "L", "")) {
                                MetaphAdd(primary, "J");
                                MetaphAdd(secondary, "J");
                            }
                        }
                    }
                }

                if (GetAt(original, current + 1) == 'J')        /* it could happen! */
                    current += 2;
                else
                    current += 1;
                break;

            case 'K':
                if (GetAt(original, current + 1) == 'K')
                    current += 2;
                else
                    current += 1;
                MetaphAdd(primary, "K");
                MetaphAdd(secondary, "K");
                break;

            case 'L':
                if (GetAt(original, current + 1) == 'L') {
                    /* spanish e.g. 'cabrillo', 'gallegos' */
                    if (((current == (length - 3)) &&
                         StringAt(original, (current - 1), 4, "ILLO",
                                  "ILLA", "ALLE", "")) ||
                        ((StringAt(original, (last - 1), 2, "AS", "OS", "") ||
                          StringAt(original, last, 1, "A", "O", "")) &&
                         StringAt(original, (current - 1), 4, "ALLE", ""))) {
                        MetaphAdd(primary, "L");
                        MetaphAdd(secondary, "");
                        current += 2;
                        break;
                    }
                    current += 2;
                } else
                    current += 1;
                MetaphAdd(primary, "L");
                MetaphAdd(secondary, "L");
                break;

            case 'M':
                if ((StringAt(original, (current - 1), 3, "UMB", "") &&
                     (((current + 1) == last) ||
                      StringAt(original, (current + 2), 2, "ER", "")))
                    /* 'dumb','thumb' */
                    || (GetAt(original, current + 1) == 'M'))
                    current += 2;
                else
                    current += 1;
                MetaphAdd(primary, "M");
                MetaphAdd(secondary, "M");
                break;

            case 'N':
                if (GetAt(original, current + 1) == 'N')
                    current += 2;
                else
                    current += 1;
                MetaphAdd(primary, "N");
                MetaphAdd(secondary, "N");
                break;

            case '\xD1':
                current += 1;
                MetaphAdd(primary, "N");
                MetaphAdd(secondary, "N");
                break;

            case 'P':
                if (GetAt(original, current + 1) == 'H') {
                    MetaphAdd(primary, "F");
                    MetaphAdd(secondary, "F");
                    current += 2;
                    break;
                }

                /* also account for "campbell", "raspberry" */
                if (StringAt(original, (current + 1), 1, "P", "B", ""))
                    current += 2;
                else
                    current += 1;
                MetaphAdd(primary, "P");
                MetaphAdd(secondary, "P");
                break;

            case 'Q':
                if (GetAt(original, current + 1) == 'Q')
                    current += 2;
                else
                    current += 1;
                MetaphAdd(primary, "K");
                MetaphAdd(secondary, "K");
                break;

            case 'R':
                /* french e.g. 'rogier', but exclude 'hochmeier' */
                if ((current == last) &&
                    !SlavoGermanic(original) &&
                    StringAt(original, (current - 2), 2, "IE", "") &&
                    !StringAt(original, (current - 4), 2, "ME", "MA", "")) {
                    MetaphAdd(primary, "");
                    MetaphAdd(secondary, "R");
                } else {
                    MetaphAdd(primary, "R");
                    MetaphAdd(secondary, "R");
                }

                if (GetAt(original, current + 1) == 'R')
                    current += 2;
                else
                    current += 1;
                break;

            case 'S':
                /* special cases 'island', 'isle', 'carlisle', 'carlysle' */
                if (StringAt(original, (current - 1), 3, "ISL", "YSL", "")) {
                    current += 1;
                    break;
                }

                /* special case 'sugar-' */
                if ((current == 0) &&
                    StringAt(original, current, 5, "SUGAR", "")) {
                    MetaphAdd(primary, "X");
                    MetaphAdd(secondary, "S");
                    current += 1;
                    break;
                }

                if (StringAt(original, current, 2, "SH", "")) {
                    /* germanic */
                    if (StringAt
                            (original, (current + 1), 4, "HEIM", "HOEK", "HOLM",
                            "HOLZ", "")) {
                        MetaphAdd(primary, "S");
                        MetaphAdd(secondary, "S");
                    } else {
                        MetaphAdd(primary, "X");
                        MetaphAdd(secondary, "X");
                    }
                    current += 2;
                    break;
                }

                /* italian & armenian */
                if (StringAt(original, current, 3, "SIO", "SIA", "") ||
                    StringAt(original, current, 4, "SIAN", "")) {
                    if (!SlavoGermanic(original)) {
                        MetaphAdd(primary, "S");
                        MetaphAdd(secondary, "X");
                    } else {
                        MetaphAdd(primary, "S");
                        MetaphAdd(secondary, "S");
                    }
                    current += 3;
                    break;
                }

                /* german & anglicisations, e.g. 'smith' match 'schmidt', 'snider' match 'schneider'
                   also, -sz- in slavic language altho in hungarian it is pronounced 's' */
                if (((current == 0) &&
                     StringAt(original, (current + 1), 1, "M", "N", "L", "W", "")) ||
                    StringAt(original, (current + 1), 1, "Z", "")) {
                    MetaphAdd(primary, "S");
                    MetaphAdd(secondary, "X");
                    if (StringAt(original, (current + 1), 1, "Z", ""))
                        current += 2;
                    else
                        current += 1;
                    break;
                }

                if (StringAt(original, current, 2, "SC", "")) {
                    /* Schlesinger's rule */
                    if (GetAt(original, current + 2) == 'H') {
                        /* dutch origin, e.g. 'school', 'schooner' */
                        if (StringAt(original, (current + 3), 2, "OO", "ER", "EN",
                                     "UY", "ED", "EM", "")) {
                            /* 'schermerhorn', 'schenker' */
                            if (StringAt(original, (current + 3), 2, "ER", "EN", "")) {
                                MetaphAdd(primary, "X");
                                MetaphAdd(secondary, "SK");
                            } else {
                                MetaphAdd(primary, "SK");
                                MetaphAdd(secondary, "SK");
                            }
                            current += 3;
                            break;
                        } else {
                            if ((current == 0) && !IsVowel(original, 3) &&
                                (GetAt(original, 3) != 'W')) {
                                MetaphAdd(primary, "X");
                                MetaphAdd(secondary, "S");
                            } else {
                                MetaphAdd(primary, "X");
                                MetaphAdd(secondary, "X");
                            }
                            current += 3;
                            break;
                        }
                    }

                    if (StringAt(original, (current + 2), 1, "I", "E", "Y", "")) {
                        MetaphAdd(primary, "S");
                        MetaphAdd(secondary, "S");
                        current += 3;
                        break;
                    }
                    /* else */
                    MetaphAdd(primary, "SK");
                    MetaphAdd(secondary, "SK");
                    current += 3;
                    break;
                }

                /* french e.g. 'resnais', 'artois' */
                if ((current == last) &&
                    StringAt(original, (current - 2), 2, "AI", "OI", "")) {
                    MetaphAdd(primary, "");
                    MetaphAdd(secondary, "S");
                } else {
                    MetaphAdd(primary, "S");
                    MetaphAdd(secondary, "S");
                }

                if (StringAt(original, (current + 1), 1, "S", "Z", ""))
                    current += 2;
                else
                    current += 1;
                break;

            case 'T':
                if (StringAt(original, current, 4, "TION", "")) {
                    MetaphAdd(primary, "X");
                    MetaphAdd(secondary, "X");
                    current += 3;
                    break;
                }

                if (StringAt(original, current, 3, "TIA", "TCH", "")) {
                    MetaphAdd(primary, "X");
                    MetaphAdd(secondary, "X");
                    current += 3;
                    break;
                }

                if (StringAt(original, current, 2, "TH", "") ||
                    StringAt(original, current, 3, "TTH", "")) {
                    /* special case 'thomas', 'thames' or germanic */
                    if (StringAt(original, (current + 2), 2, "OM", "AM", "") ||
                        StringAt(original, 0, 4, "VAN ", "VON ", "") ||
                        StringAt(original, 0, 3, "SCH", "")) {
                        MetaphAdd(primary, "T");
                        MetaphAdd(secondary, "T");
                    } else {
                        MetaphAdd(primary, "0");     /* yes, zero */
                        MetaphAdd(secondary, "T");
                    }
                    current += 2;
                    break;
                }

                if (StringAt(original, (current + 1), 1, "T", "D", ""))
                    current += 2;
                else
                    current += 1;
                MetaphAdd(primary, "T");
                MetaphAdd(secondary, "T");
                break;

            case 'V':
                if (GetAt(original, current + 1) == 'V')
                    current += 2;
                else
                    current += 1;
                MetaphAdd(primary, "F");
                MetaphAdd(secondary, "F");
                break;

            case 'W':
                /* can also be in middle of word */
                if (StringAt(original, current, 2, "WR", "")) {
                    MetaphAdd(primary, "R");
                    MetaphAdd(secondary, "R");
                    current += 2;
                    break;
                }

                if ((current == 0) &&
                    (IsVowel(original, current + 1) ||
                     StringAt(original, current, 2, "WH", ""))) {
                    /* Wasserman should match Vasserman */
                    if (IsVowel(original, current + 1)) {
                        MetaphAdd(primary, "A");
                        MetaphAdd(secondary, "F");
                    } else {
                        /* need Uomo to match Womo */
                        MetaphAdd(primary, "A");
                        MetaphAdd(secondary, "A");
                    }
                }

                /* Arnow should match Arnoff */
                if (((current == last) && IsVowel(original, current - 1)) ||
                    StringAt(original, (current - 1), 5, "EWSKI", "EWSKY",
                             "OWSKI", "OWSKY", "") ||
                    StringAt(original, 0, 3, "SCH", "")) {
                    MetaphAdd(primary, "");
                    MetaphAdd(secondary, "F");
                    current += 1;
                    break;
                }

                /* polish e.g. 'filipowicz' */
                if (StringAt(original, current, 4, "WICZ", "WITZ", "")) {
                    MetaphAdd(primary, "TS");
                    MetaphAdd(secondary, "FX");
                    current += 4;
                    break;
                }

                /* else skip it */
                current += 1;
                break;

            case 'X':
                /* french e.g. breaux */
                if (!((current == last) &&
                      (StringAt(original, (current - 3), 3, "IAU", "EAU", "") ||
                       StringAt(original, (current - 2), 2, "AU", "OU", "")))) {
                    MetaphAdd(primary, "KS");
                    MetaphAdd(secondary, "KS");
                }


                if (StringAt(original, (current + 1), 1, "C", "X", ""))
                    current += 2;
                else
                    current += 1;
                break;

            case 'Z':
                /* chinese pinyin e.g. 'zhao' */
                if (GetAt(original, current + 1) == 'H') {
                    MetaphAdd(primary, "J");
                    MetaphAdd(secondary, "J");
                    current += 2;
                    break;
                } else if (StringAt(original, (current + 1), 2, "ZO", "ZI", "ZA", "") ||
                           (SlavoGermanic(original) &&
                            ((current > 0) &&
                             (GetAt(original, current - 1) != 'T')))) {
                    MetaphAdd(primary, "S");
                    MetaphAdd(secondary, "TS");
                } else {
                    MetaphAdd(primary, "S");
                    MetaphAdd(secondary, "S");
                }

                if (GetAt(original, current + 1) == 'Z')
                    current += 2;
                else
                    current += 1;
                break;

            default:
                current += 1;
        }

        /* printf("PRIMARY: %s\n", primary->str);
           printf("SECONDARY: %s\n", secondary->str);  */
    }


    if (primary->length > 4)
        SetAt(primary, 4, '\0');

    if (secondary->length > 4)
        SetAt(secondary, 4, '\0');

    *codes   = primary->str;
    *++codes = secondary->str;

    DestroyMetaString(original);
    DestroyMetaString(primary);
    DestroyMetaString(secondary);
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(DoubleMetaphone, 1, 2)
    T_STRING(DoubleMetaphone, 0)
    char *name = PARAM(0);
    char *codes[2];
    codes[0] = 0;
    codes[1] = 0;

    DoubleMetaphone(name, PARAM_LEN(0), codes);
    if (PARAMETERS_COUNT > 1) {
        if (codes[1]) {
            SET_STRING(2, codes[1]);
        } else {
            SET_STRING(2, "");
        }
    }
    if (codes[0]) {
        RETURN_STRING(codes[0]);
        META_FREE(codes[0]);
    } else {
        RETURN_STRING("");
    }
    if (codes[1]) {
        META_FREE(codes[1]);
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(calc, 1, 2)
    T_STRING(calc, 0)
    double result = 0;
    ExprEval eval;
    double   res = eval.Eval(PARAM(0));
    if (eval.GetErr() != EEE_NO_ERROR) {
        result = res;
        if (PARAMETERS_COUNT > 1) {
            SET_NUMBER(1, eval.GetErr());
        }
    } else {
        if (PARAMETERS_COUNT > 1) {
            SET_NUMBER(1, 0);
        }
        result = res;
    }
    RETURN_NUMBER(result);
END_IMPL
//---------------------------------------------------------------------------
int ucs2_to_utf8(int ucs2, char **utf8, char *utf8return) {  /* Convert UCS-2 to UTF-8 */
    static unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
    const unsigned long byteMask         = 0xBF;
    const unsigned long byteMark         = 0x80;
    int utf8len = 0;
    int i       = 0;

    if (ucs2 < 0x80) {
        utf8len = 1;
    } else if (ucs2 < 0x800) {
        utf8len = 2;
    } else
    /* This is always true for UCS-2 but would be needed for UCS-4*/
    /* When ucs2 is USHORT this gives compiler warnings. */
    if (ucs2 <= 0xffff) {
        utf8len = 3;
    } else if (ucs2 < 0x200000) {
        utf8len = 4;
    } else if (ucs2 < 0x4000000) {
        utf8len = 5;
    } else if (ucs2 <= 0x7FFFFFFFUL) {  /* 31 bits = max for UCS4 */
        utf8len = 6;
    } else {
        utf8len = 2;
        ucs2    = 0xFFFD;               /* Replacement for invalid char */
    }
    i = utf8len;                        /* index into utf8return */
    utf8return[i--] = 0;                /* Null terminate the string */

    switch (utf8len) {                  /* code falls through cases! */
        case 6:
            utf8return[i--] = (ucs2 | byteMark) & byteMask;
            ucs2          >>= 6;

        case 5:
            utf8return[i--] = (ucs2 | byteMark) & byteMask;
            ucs2          >>= 6;

        case 4:
            utf8return[i--] = (ucs2 | byteMark) & byteMask;
            ucs2          >>= 6;

        case 3:
            utf8return[i--] = (ucs2 | byteMark) & byteMask;
            ucs2          >>= 6;

        case 2:
            utf8return[i--] = (ucs2 | byteMark) & byteMask;
            ucs2          >>= 6;

        case 1:
            utf8return[i--] = ucs2 | firstByteMark[utf8len];
    }
    *utf8 = utf8return;
    return utf8len;
}

CONCEPT_FUNCTION_IMPL(U_, 1)
    T_NUMBER(U_, 0)

    char *utf;
    char utf8buffer[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int len = ucs2_to_utf8(PARAM_INT(0), &utf, utf8buffer);
    if (len > 0) {
        RETURN_BUFFER(utf, len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(UTF8Map, 2)
    T_STRING(UTF8Map, 0)
    T_NUMBER(UTF8Map, 1)

    utf8proc_uint8_t *dstptr = 0;
    utf8proc_ssize_t len = utf8proc_map((utf8proc_uint8_t *)PARAM(0), (utf8proc_ssize_t)PARAM_LEN(0), &dstptr, (utf8proc_option_t)PARAM_INT(1));
    if (dstptr) {
        if (len > 0) {
            RETURN_BUFFER((const char *)dstptr, len);
        } else {
            RETURN_STRING("");
        }
        free(dstptr);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(StrMove, 2)
    T_STRING(StrMove, 0)
    T_NUMBER(StrMove, 1)

    int from = PARAM_INT(1);
    if (from < 0)
        from = 0;
    if (from > PARAM_LEN(0))
        from = PARAM_LEN(0);

    if (from == 0) {
        RETURN_STRING("");
    } else
    if (from == PARAM_LEN(0)) {
        RETURN_BUFFER(PARAM(0), PARAM_LEN(0));
        SET_STRING(0, "");
    } else {
        RETURN_BUFFER(PARAM(0), from);

        int len = PARAM_LEN(0) - from; 
        memmove(PARAM(0), PARAM(0) + from, len);

        Invoke(INVOKE_RESIZE_STRING, PARAMETER(0), (intptr_t)len);
    }
END_IMPL
//---------------------------------------------------------------------------
void setArrayElement(void *RESULT, INVOKE_CALL Invoke, const char *key, int key_len, const char *value, int value_len) {
    const char *no_key = "::";
    const char *empty_value = "";
    char key_buffer[0x100];

    int do_exit = 0;
    while ((!do_exit) && (value_len > 0)) {
        switch (value[value_len - 1]) {
            case ' ':
            case '\r':
            case '\n':
            case '\t':
            case '\x0B':
                value_len --;
                break;
            default:
                do_exit = 1;
        }
    }

    do_exit = 0;
    while ((!do_exit) && (key_len > 0)) {
        switch (key[key_len - 1]) {
            case ' ':
            case '\r':
            case '\n':
            case '\t':
            case '\x0B':
                key_len --;
                break;
            default:
                do_exit = 1;
        }
    }

    if ((key_len <= 0) && (value_len <= 0))
        return;

    if (key_len >= sizeof(key_buffer))
        key_len = sizeof(key_buffer);

    if ((!key) || (key_len <= 0)) {
        key = no_key;
    } else {
        int i;
        for (i = 0; i < key_len; i ++)
            key_buffer[i] = tolower(key[i]);
        key_buffer[key_len] = 0;
        key = key_buffer;
    }

    if (value_len <= 0) {
        value = empty_value;
        value_len = 0;
    }

    void *var = NULL;
    
    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, key, &var);

    char *szData;
    INTEGER type;
    NUMBER nData;

    Invoke(INVOKE_GET_VARIABLE, var, &type, &szData, &nData);

    char *buf = NULL;
    int len;

    switch (type) {
        case VARIABLE_STRING:
            len = (int)nData;
            if (len > 0) {
                buf = (char *)malloc(len + 1);
                if (buf) {
                    memcpy(buf, szData, len);
                    buf[len] = 0;
                }
            }

            CREATE_ARRAY(var);
            if (buf) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, var, (INTEGER)0, (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)len);
                free(buf);
            } else {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, var, (INTEGER)0, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
            }
            // no break here
        case VARIABLE_ARRAY:
            Invoke(INVOKE_SET_ARRAY_ELEMENT, var, (INTEGER)Invoke(INVOKE_GET_ARRAY_COUNT, var), (INTEGER)VARIABLE_STRING, (char *)value, (NUMBER)value_len);
            break;
        default:
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_STRING, (char *)value, (NUMBER)value_len);
            break;
    }
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(StrKeyValue, 1, 3)
    T_STRING(StrKeyValue, 0)
    int skip_lines = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(StrKeyValue, 1)
        skip_lines = PARAM_INT(1);
        if (skip_lines < 0)
            skip_lines = 0;
    }
    int as_array = 0;
    if (PARAMETERS_COUNT > 2) {
        if (skip_lines > 1) {
            CREATE_ARRAY(PARAMETER(2));
            as_array = 1;
        } else {
            SET_STRING(2, "");
        }
    }

    CREATE_ARRAY(RESULT);

    char *str = PARAM(0);
    int len = PARAM_LEN(0);
    int in_key = 0;
    int in_val = 0;
    int key_len = 0;
    char *key = str;
    if ((str) && (len > 0)) {
        int i;
        for (i = 0; i < len; i ++) {
            char ch = str[i];
            switch (ch) {
                case '\n':
                    // new line
                    if (!in_val) {
                        int line_len = (str - key) + i - 1;
                        if ((PARAMETERS_COUNT > 2) && (skip_lines)) {
                            if (line_len > 0) {
                                if (as_array) {
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(2), (INTEGER)Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2)), (INTEGER)VARIABLE_STRING, (char *)key, (NUMBER)line_len);
                                } else {
                                    SET_BUFFER(2, key, line_len);
                                }
                            }
                        } else {
                            setArrayElement(RESULT, Invoke, NULL, 0, key, key_len);
                        }
                    } else {
                        setArrayElement(RESULT, Invoke, key, key_len, str + in_val, i - in_val - 1);
                    }
                    if (skip_lines)
                        skip_lines --;
                    in_key = i + 1;
                    key = str + in_key;
                    in_val = 0;

                    if ((i < len - 1) && ((str[i + 1] == '\r') || (str[i + 1] == '\n')))
                        return 0;
                    break;
                case ':':
                    if ((!skip_lines) && (!in_val)) {
                        key_len = (str - key) + i;
                        in_val = i + 1;
                        in_key = 0;
                    }
                    break;
                case ' ':
                case '\t':
                case '\x0B':
                    if ((in_key) && (in_key == i) && (!in_val))
                        in_key ++;
                    else
                    if ((in_val) && (in_val == i))
                        in_val ++;
                    break;
            }
        }
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PhraseSplit, 1, 2)
    T_STRING(PhraseSplit, 0)

    int min_len = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(PhraseSplit, 1)
        min_len = PARAM_INT(1);
    }

    CREATE_ARRAY(RESULT);
    char *str = PARAM(0);
    int len = PARAM_LEN(0);
    int start = 0;
    INTEGER index = 0;
    int str_len;
    for (int i = 0; i < len; i ++) {
        switch (str[i]) {
            case '.':
            case '!':
            case '?':
                if (i + 1 < len) {
                    switch (str[i + 1]) {
                        case '.':
                        case '!':
                        case '?':
                            continue;
                    }
                }
                str_len = i + 1 - start;
                if (min_len > 0) {
                    if (u8_strlen(str + start, str_len) < min_len)
                        continue;
                }
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index++, (INTEGER)VARIABLE_STRING, str + start, (NUMBER)str_len);
                start = i + 1;
                while ((i + 1 < len) && ((str[i + 1] == ' ') || (str[i + 1] == '\t') || (str[i + 1] == '\r'))) {
                    i ++;
                    start ++;
                }
                break;
        }
    }
    if (start != len)
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index++, (INTEGER)VARIABLE_STRING, str + start, (NUMBER)(len + 1 - start));

END_IMPL
//-------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(LeftOf, 2, 3)
    T_STRING(LeftOf, 0)
    T_STRING(LeftOf, 1)
    const char *default_value = "";
    if (PARAMETERS_COUNT > 2) {
        T_STRING(LeftOf, 2)
        default_value = PARAM(2);
    }

    const char* sep = PARAM(1);
    INTEGER len_sep = PARAM_LEN(1);

    if ((len_sep == 0) || (PARAM_LEN(0) == 0)) {
        RETURN_STRING(default_value);
        return 0;
    }

    const char* pos;
    if (len_sep == 1)
        pos = (const char*)memchr(PARAM(0), *sep, PARAM_LEN(0));
    else
        pos = strstr(PARAM(0), sep);

    if (pos) {
        INTEGER buf_len = pos - PARAM(0);
        RETURN_BUFFER(PARAM(0), buf_len);
    } else {
        RETURN_STRING(default_value);
    }
END_IMPL
//-------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(RightOf, 2, 3)
    T_STRING(RightOf, 0)
    T_STRING(RightOf, 1)
    const char *default_value = "";
    if (PARAMETERS_COUNT > 2) {
        T_STRING(RightOf, 2)
        default_value = PARAM(2);
    }

    const char* sep = PARAM(1);
    INTEGER len_sep = PARAM_LEN(1);

    if ((len_sep == 0) || (PARAM_LEN(0) == 0)) {
        RETURN_STRING(default_value);
        return 0;
    }

    const char* pos;
    if (len_sep == 1)
        pos = (const char*)memchr(PARAM(0), *sep, PARAM_LEN(0));
    else
        pos = strstr(PARAM(0), sep);

    if (pos) {
        RETURN_STRING(pos + len_sep);
    } else {
        RETURN_STRING(default_value);
    }
END_IMPL
//-------------------------------
CONCEPT_FUNCTION_IMPL(EnsureBuffer, 2)
    T_STRING(EnsureBuffer, 0);
    T_NUMBER(EnsureBuffer, 1);

    int err = Invoke(INVOKE_ENSURE_STRING_BUFFER, PARAMETER(0), (intptr_t)PARAM(1));

    RETURN_NUMBER(err);
END_IMPL
//-------------------------------
#define LEVENSHTEIN_MIN(a, b)   (a) < (b) ? (a) : (b)
CONCEPT_FUNCTION_IMPL(LevenshteinDistance, 2)
    T_STRING(LevenshteinDistance, 0);
    T_STRING(LevenshteinDistance, 1);

    int n = (PARAM(0)) ? u8_strlen(PARAM(0), PARAM_LEN(0)) + 1 : 0;
    int m = (PARAM(1)) ? u8_strlen(PARAM(1), PARAM_LEN(1)) + 1 : 0;

    if (n < 2) {
        RETURN_NUMBER(m);
        return 0;
    }
    if (m < 2) {
        RETURN_NUMBER(n);
        return 0;
    }

    if (n > 64)
        n = 64;
    if (m > 64)
        m = 64;

    // String matrix
    int d[64][64];
    int i, j;

    for (i = 0; i < n; i++)
        d[i][0] = i;
    for (j = 0; j < m; j++)
        d[0][j] = j;

    int idx_0 = 0;
    for (i = 1; i < n; i++) {
        // Current char in string s
        int s0 = u8_nextchar(PARAM(0), &idx_0, PARAM_LEN(0));

        int idx_1 = 0;
        for (j = 1; j < m; j++) {
            int jm1 = j - 1;
            int im1 = i - 1;

            int s1 = u8_nextchar(PARAM(1), &idx_1, PARAM_LEN(1));

            int a = d[im1][j] + 1;
            int b = d[i][jm1] + 1;
            int c = d[im1][jm1] + (s1 != s0);

            d[i][j] = (a < b) ? LEVENSHTEIN_MIN(a, c) : LEVENSHTEIN_MIN(b, c);
        }
    }

    RETURN_NUMBER(d[n - 1][m - 1]);
END_IMPL
//-------------------------------
