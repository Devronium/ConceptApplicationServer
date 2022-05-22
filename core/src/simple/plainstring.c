#include "plainstring.h"
#include <stdlib.h>
#include <string.h>

#define WITH_DTOA

#define PLAINSTRING_MALLOC      malloc
#define PLAINSTRING_REALLOC     realloc
#define PLAINSTRING_FREE        free

#define breakeven_point    12

static const char *null_string = "";
#define fast_memcpy(d, s, n)                                                                 \
    { register size_t nn = (size_t)(n);                                                      \
      if (nn >= breakeven_point) { memcpy((d), (s), nn); }                                   \
      else if (nn > 0) {                                                                     \
          register char *dd; register const char *ss;                                        \
          for (ss = (s), dd = (d); nn > 0; nn--) { *dd++ = *ss++; } } }

#define MEMCPY    fast_memcpy

void *plainstring_core_new(size_t size) {
    return PLAINSTRING_MALLOC(size);
}

void plainstring_core_free(void *ptr) {
    PLAINSTRING_FREE(ptr);
}

void plainstring_core_free(void *ptr);

void cstr_loaddouble(char *buffer, double d) {
#ifdef WITH_FPCONV
    buffer[0] = 0;
    int len = fpconv_dtoa(d, buffer);
    if (len > 0)
        buffer[len] = 0;
#else
#ifdef WITH_DTOA
    // 3 times faster
    buffer[0] = 0;
    dtoa_milo_c_wrapper(d, buffer);
#else
    sprintf(buffer, "%.15g", d);
    size_t len = strlen(buffer);
    if (((len > 1) && ((buffer [len - 1] == '.') || (buffer [len - 1] == ',')))) {
        buffer [--len] = 0;
    }
#endif
#endif
}

int plainstring_memory_length(const struct plainstring *this_string) {
    return this_string->DATA_SIZE;
}

void plainstring_init(struct plainstring *str) {
    if (str) {
        str->DATA = 0;
        str->DATA_SIZE = 0;
        str->LENGTH = 0;
        str->EXTRA_DATA = 0;
    }
}

struct plainstring *plainstring_new(void) {
    struct plainstring *str = (struct plainstring *)PLAINSTRING_MALLOC(sizeof(struct plainstring));
    plainstring_init(str);
    return str;
}

struct plainstring *plainstring_new_str(const char *value) {
    struct plainstring *str = plainstring_new();
    if (!str)
        return str;
    if ((value) && (value[0]))
        plainstring_set(str, value);
    return str;
}

struct plainstring *plainstring_new_char(char c) {
    struct plainstring *str = plainstring_new();
    if (!str)
        return str;
    plainstring_set_char(str, c);
    return str;
}

struct plainstring *plainstring_new_int(int i) {
    struct plainstring *str = plainstring_new();
    if (!str)
        return str;
    plainstring_set_int(str, i);
    return str;
}

struct plainstring *plainstring_new_double(double d) {
    struct plainstring *str = plainstring_new();
    if (!str)
        return str;
    plainstring_set_double(str, d);
    return str;
}

struct plainstring *plainstring_new_plainstring(const struct plainstring *ps) {
    struct plainstring *str = plainstring_new();
    if (!str)
        return str;
    if (ps)
        plainstring_loadbuffer(str, ps->DATA, ps->LENGTH);
    return str;
}

char plainstring_char(const struct plainstring *this_string, intptr_t index) {
    if ((this_string) && (this_string->DATA)) {
        if ((index >= 0) && (index < this_string->LENGTH))
            return this_string->DATA[index];
    }
    return 0;
}

void plainstring_char_plainstring(const struct plainstring *this_string, intptr_t index, struct plainstring *ps) {
    if (!ps)
        return;
    if ((this_string) && (this_string->DATA)) {
        if ((index >= 0) && (index < this_string->LENGTH)) {
            plainstring_loadbuffer(ps, &this_string->DATA[index], 1);
            return;
        }
    }
    if (ps->DATA) {
        PLAINSTRING_FREE(ps->DATA);
        ps->DATA = 0;
        ps->DATA_SIZE = 0;
        ps->LENGTH = 0;
    }
}

int plainstring_equals(const struct plainstring *this_string, const char *str) {
    if ((!str) || (!str[0])) {
        if ((!this_string) || (!this_string->LENGTH))
            return 1;
        return 0;
    }
    if (!this_string)
        return 0;
    return !strcmp(this_string->DATA ? this_string->DATA : null_string, str ? str : null_string);
}

int plainstring_not_equals(const struct plainstring *this_string, const char *str) {
    return !plainstring_equals(this_string, str);
}

int plainstring_equals_double(const struct plainstring *this_string, double d) {
    char buffer [MAX_DECIMALS];
    if (!this_string)
        return 0;
    cstr_loaddouble(buffer, d);
    return plainstring_equals(this_string, buffer);
}

int plainstring_not_equals_double(const struct plainstring *this_string, double d) {
    return (!plainstring_equals_double(this_string, d));
}

int plainstring_equals_plainstring(const struct plainstring *this_string, const struct plainstring *ps) {
    if ((!this_string) || (!this_string->LENGTH)) {
        if ((!ps) || (!ps->LENGTH))
            return 1;
        if (!this_string)
            return 0;
    }

    if ((!ps) || (this_string->LENGTH != ps->LENGTH))
        return 0;

    if (!this_string->LENGTH)
        return 1;

    return !memcmp(this_string->DATA, ps->DATA, this_string->LENGTH);
}

int plainstring_not_equals_plainstring(const struct plainstring *this_string, const struct plainstring *ps) {
    return !plainstring_equals_plainstring(this_string, ps);
}

int plainstring_less_plainstring(const struct plainstring *this_string, const struct plainstring *ps) {
    intptr_t len = ps->LENGTH < this_string->LENGTH ? ps->LENGTH : this_string->LENGTH;

    if (len) {
        int res = memcmp(this_string->DATA, ps->DATA, len);
        if (res < 0)
            return 1;
        if ((res == 0) && (this_string->LENGTH < ps->LENGTH))
            return 1;
    } else
    if (ps->LENGTH)
        return 1;
    return 0;
}

int plainstring_lessequal_plainstring(const struct plainstring *this_string, const struct plainstring *ps) {
    intptr_t len = ps->LENGTH < this_string->LENGTH ? ps->LENGTH : this_string->LENGTH;

    if (len) {
        int res = memcmp(this_string->DATA, ps->DATA, len);
        if (res < 0)
            return 1;
        if ((res == 0) && (this_string->LENGTH <= ps->LENGTH))
            return 1;
    } else
    if ((ps->LENGTH) || (this_string->LENGTH == ps->LENGTH))
        return 1;
    return 0;
}

int plainstring_greater_plainstring(const struct plainstring *this_string, const struct plainstring *ps) {
    intptr_t len = ps->LENGTH < this_string->LENGTH ? ps->LENGTH : this_string->LENGTH;

    if (len) {
        int res = memcmp(this_string->DATA, ps->DATA, len);
        if (res > 0)
            return 1;
        if ((res == 0) && (this_string->LENGTH > ps->LENGTH))
            return 1;
    } else
    if (this_string->LENGTH)
        return 1;
    return 0;
}

int plainstring_greaterequal_plainstring(const struct plainstring *this_string, const struct plainstring *ps) {
    intptr_t len = ps->LENGTH < this_string->LENGTH ? ps->LENGTH : this_string->LENGTH;

    if (len) {
        int res = memcmp(this_string->DATA, ps->DATA, len);
        if (res > 0)
            return 1;
        if ((res == 0) && (this_string->LENGTH >= ps->LENGTH))
            return 1;
    } else
    if ((this_string->LENGTH) || (this_string->LENGTH == ps->LENGTH))
        return 1;
    return 0;
}

int plainstring_less(const struct plainstring *this_string, const char *str) {
    if ((this_string->DATA) && (str))
        return strcmp(this_string->DATA, str) < 0;

    const char *ref_Data = this_string->DATA;
    if (!str)
        str = null_string;
    if (!ref_Data)
        ref_Data = null_string;

    return strcmp(ref_Data, str) < 0;
}

int plainstring_lessequal(const struct plainstring *this_string, const char *str) {
    if ((this_string->DATA) && (str))
        return strcmp(this_string->DATA, str) <= 0;

    const char *ref_Data = this_string->DATA;
    if (!str)
        str = null_string;
    if (!ref_Data)
        ref_Data = null_string;

    return strcmp(ref_Data, str) <= 0;
}

int plainstring_greater(const struct plainstring *this_string, const char *str) {
    if ((this_string->DATA) && (str))
        return strcmp(this_string->DATA, str) > 0;

    const char *ref_Data = this_string->DATA;
    if (!str)
        str = null_string;
    if (!ref_Data)
        ref_Data = null_string;

    return strcmp(ref_Data, str) > 0;
}

int plainstring_greaterequal(const struct plainstring *this_string, const char *str) {
    if ((this_string->DATA) && (str))
        return strcmp(this_string->DATA, str) >= 0;

    const char *ref_Data = this_string->DATA;
    if (!str)
        str = null_string;
    if (!ref_Data)
        ref_Data = null_string;

    return strcmp(ref_Data, str) >= 0;
}

int plainstring_less_double(const struct plainstring *this_string, double d) {
    char buffer [MAX_DECIMALS];
    cstr_loaddouble(buffer, d);
    return plainstring_less(this_string, buffer);
}

int plainstring_lessequal_double(const struct plainstring *this_string, double d) {
    char buffer [MAX_DECIMALS];
    cstr_loaddouble(buffer, d);
    return plainstring_lessequal(this_string, buffer);
}

int plainstring_greater_double(const struct plainstring *this_string, double d) {
    char buffer [MAX_DECIMALS];
    cstr_loaddouble(buffer, d);
    return plainstring_greater(this_string, buffer);
}

int plainstring_greaterequal_double(const struct plainstring *this_string, double d) {
    char buffer [MAX_DECIMALS];
    cstr_loaddouble(buffer, d);
    return plainstring_greaterequal(this_string, buffer);
}

void plainstring_set(struct plainstring *this_string, const char *value) {
    intptr_t len;

    if (this_string->DATA == value)
        return;
    if (this_string->DATA)
        this_string->DATA[0] = 0;

    this_string->LENGTH = 0;
    if (value) {
        len     = strlen(value);
        this_string->LENGTH = len;
        if (len) {
            if (len + 1 >= this_string->DATA_SIZE) {
                this_string->DATA_SIZE  = ((len + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
                char *orig_data         = this_string->DATA;
                this_string->DATA       = (char *)PLAINSTRING_REALLOC(this_string->DATA, this_string->DATA_SIZE);
                if (!this_string->DATA) {
                    this_string->DATA_SIZE = 0;
                    this_string->LENGTH = 0;
                    PLAINSTRING_FREE(orig_data);
                }
            }
            if (this_string->DATA) {
                MEMCPY(this_string->DATA, value, len + 1);
            }
        }
    }
}

void plainstring_set_double(struct plainstring *this_string, double d) {
    char buffer [MAX_DECIMALS];
    cstr_loaddouble(buffer, d);
    plainstring_set(this_string, buffer);
}

void plainstring_set_int(struct plainstring *this_string, int i) {
    char buffer[MAX_DECIMALS];
    sprintf(buffer, "%i", i);
    plainstring_set(this_string, buffer);
}

void plainstring_set_char(struct plainstring *this_string, char c) {
    plainstring_loadbuffer(this_string, &c, 1);
}

void plainstring_set_plainstring(struct plainstring *this_string, const struct plainstring *ps) {
    if (!ps) {
        plainstring_set(this_string, null_string);
        return;
    }
    if (this_string->DATA == ps->DATA)
        return;

    if (this_string->DATA) {
        if (this_string->DATA_SIZE - ps->LENGTH > BLOCK_SIZE) {
            PLAINSTRING_FREE(this_string->DATA);
            this_string->DATA = 0;
            this_string->DATA_SIZE = 0;
        } else
            this_string->DATA[0] = 0;
    }
    this_string->LENGTH = 0;

    if (ps->LENGTH) {
        this_string->LENGTH = ps->LENGTH;
        if (ps->LENGTH + 1 >= this_string->DATA_SIZE) {

            PLAINSTRING_FREE(this_string->DATA);
            this_string->DATA_SIZE = ((ps->LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            this_string->DATA = (char *)PLAINSTRING_MALLOC(this_string->DATA_SIZE);
        }
        MEMCPY(this_string->DATA, ps->DATA, ps->LENGTH);
        this_string->DATA[this_string->LENGTH] = 0;
    }
}

void plainstring_add(struct plainstring *this_string, const char *value) {
    size_t len;
    size_t len_value;

    if ((this_string->DATA) && (value)) {
        len       = this_string->LENGTH;
        len_value = strlen(value);
        this_string->LENGTH   = len + len_value;
        if (this_string->DATA_SIZE < this_string->LENGTH + 1) {
            this_string->DATA_SIZE = ((this_string->LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            char *orig_data = this_string->DATA;
            this_string->DATA = (char *)PLAINSTRING_REALLOC(this_string->DATA, this_string->DATA_SIZE);
            if (!this_string->DATA) {
                this_string->DATA_SIZE = 0;
                this_string->LENGTH = 0;
                PLAINSTRING_FREE(orig_data);
                return;
            }
            MEMCPY(this_string->DATA + len, value, len_value + 1);
        } else {
            MEMCPY(this_string->DATA + len, value, len_value + 1);
        }
    } else
    if (value)
        plainstring_set(this_string, value);
}

void plainstring_add_char(struct plainstring *this_string, char c) {
    if (this_string->LENGTH + 2 < this_string->DATA_SIZE) {
        this_string->DATA[this_string->LENGTH++] = c;
        this_string->DATA[this_string->LENGTH]   = 0;
    } else {
        this_string->DATA_SIZE += BLOCK_SIZE;
        this_string->DATA = (char *)PLAINSTRING_REALLOC(this_string->DATA, this_string->DATA_SIZE);
        this_string->DATA[this_string->LENGTH++] = c;
        this_string->DATA[this_string->LENGTH] = 0;
    }
}

void plainstring_add_plainstring(struct plainstring *this_string, const struct plainstring *ps) {
    if ((ps) && (ps->DATA) && (ps->LENGTH))
        plainstring_addbuffer(this_string, ps->DATA, ps->LENGTH);
}

void plainstring_add_double(struct plainstring *this_string, double d) {
    char buffer [MAX_DECIMALS];
    cstr_loaddouble(buffer, d);
    plainstring_add(this_string, buffer);
}

void plainstring_add_int(struct plainstring *this_string, int value) {
    char buffer[MAX_DECIMALS];
    sprintf(buffer, "%i", value);
    plainstring_add(this_string, buffer);
}

const char *plainstring_c_str(const struct plainstring *this_string) {
    if (this_string->LENGTH)
        return this_string->DATA;
    return null_string;
}

struct plainstring *plainstring_sum(struct plainstring *this_string, const struct plainstring *other) {
    struct plainstring *str = plainstring_new_plainstring(this_string);
    if ((str) && (other))
        plainstring_add_plainstring(str, other);
    return str;
}

struct plainstring *plainstring_sum_str(struct plainstring *this_string, const char *other) {
    struct plainstring *str = plainstring_new_plainstring(this_string);
    if ((str) && (other))
        plainstring_add(str, other);
    return str;
}

intptr_t plainstring_int(const struct plainstring *this_string) {
    if ((this_string) && (this_string->DATA) && (this_string->LENGTH))
        return atol(this_string->DATA);

    return 0;
}

double plainstring_float(const struct plainstring *this_string) {
    if ((this_string) && (this_string->DATA) && (this_string->LENGTH))
        return atof(this_string->DATA);

    return 0;
}

int plainstring_loadfile(struct plainstring *this_string, const char *filename) {
    FILE     *in;
    intptr_t size;

    in = fopen(filename, "rb");
    if (in) {
        fseek(in, 0, SEEK_END);
        size = ftell(in);
        fseek(in, 0, SEEK_SET);
        PLAINSTRING_FREE(this_string->DATA);

        this_string->DATA_SIZE  = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
        this_string->DATA        = (char *)PLAINSTRING_MALLOC(this_string->DATA_SIZE);
        this_string->DATA [size] = 0;
        fread(this_string->DATA, size, 1, in);
        fclose(in);
        this_string->LENGTH = size;
        return 0;
    }
    return -1;

}

int plainstring_savefile(const struct plainstring *this_string, const char *filename) {
    FILE   *in;
    size_t len;
    int    ret = -1;

    in = fopen(filename, "wb");
    if (in) {
        if (this_string->DATA) {
            len = this_string->LENGTH;
            if (fwrite(this_string->DATA, 1, len, in) == len)
                ret = 0;
        } else {
            ret = 0;
        }
        fclose(in);
        return ret;
    }
    return ret;
}

void plainstring_loadbuffer(struct plainstring *this_string, const char *buffer, int size) {
    void *PLAINSTRING_FREE_after = NULL;
    if ((this_string->DATA) && (buffer == this_string->DATA))
        PLAINSTRING_FREE_after = this_string->DATA;
    else
        PLAINSTRING_FREE(this_string->DATA);
    if (size < 0)
        size = 0;

    this_string->DATA = 0;
    this_string->LENGTH = size;
    if (!size) {
        this_string->DATA_SIZE = 0;
        return;
    }
    this_string->DATA_SIZE = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
    this_string->DATA = (char *)PLAINSTRING_MALLOC(this_string->DATA_SIZE);
    if (this_string->DATA) {
        MEMCPY(this_string->DATA, buffer, size);
        this_string->DATA[size] = 0;
    } else {
        this_string->DATA_SIZE = 0;
        this_string->LENGTH = 0;
    }
    PLAINSTRING_FREE(PLAINSTRING_FREE_after);
}

void plainstring_addbuffer(struct plainstring *this_string, const char *buffer, int size) {
    size_t len;

    if ((!buffer) || (!size))
        return;

    if (this_string->DATA) {
        len       = this_string->LENGTH;
        this_string->LENGTH   += size;
        if (this_string->DATA_SIZE < this_string->LENGTH + 1) {
            this_string->DATA_SIZE = ((this_string->LENGTH + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;

            char *orig_data = this_string->DATA;
            this_string->DATA = (char *)PLAINSTRING_REALLOC(this_string->DATA, this_string->DATA_SIZE);
            if (this_string->DATA) {
                MEMCPY(this_string->DATA + len, buffer, size);
                this_string->DATA[this_string->LENGTH] = 0;
            } else {
                PLAINSTRING_FREE(orig_data);
                this_string->LENGTH    = 0;
                this_string->DATA_SIZE = 0;
            }
        } else {
            MEMCPY(this_string->DATA + len, buffer, size);
            this_string->DATA[this_string->LENGTH] = 0;
        }
    } else {
        plainstring_loadbuffer(this_string, buffer, size);
    }
}

void plainstring_linkbuffer(struct plainstring *this_string, char *buffer, int size) {
    PLAINSTRING_FREE(this_string->DATA);
    if (size < 0)
        size = 0;

    this_string->LENGTH = size;
    this_string->DATA_SIZE = size;
    if (size)
        this_string->DATA = buffer;
    else
        this_string->DATA = 0;
}

void plainstring_increasebuffer(struct plainstring *this_string, int size) {
    if (size <= 0)
        return;

    int new_len = this_string->LENGTH + size + 1;

    if (this_string->DATA_SIZE < new_len) {
        int mod_size = new_len / BLOCK_SIZE;

        if ((mod_size >= 0x2000) && (this_string->DATA_SIZE > 0x20000))
            new_len = (new_len / BLOCK_SIZE + mod_size / 3) * BLOCK_SIZE;

        this_string->DATA_SIZE = (new_len / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;

        char *orig_data = this_string->DATA;
        this_string->DATA = (char *)PLAINSTRING_REALLOC(this_string->DATA, this_string->DATA_SIZE);
        if (!this_string->DATA) {
            this_string->DATA_SIZE = ((this_string->LENGTH + size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            this_string->DATA = (char *)PLAINSTRING_REALLOC(orig_data, this_string->DATA_SIZE);
            if (!this_string->DATA) {
                this_string->DATA_SIZE = 0;
                this_string->LENGTH = 0;
                PLAINSTRING_FREE(orig_data);
            }
        }
    }
}

void plainstring_sum_of_2(struct plainstring *this_string, const struct plainstring *s1, const struct plainstring *s2) {
    plainstring_add_plainstring(this_string, s1);
    plainstring_add_plainstring(this_string, s2);
}

void plainstring_asg(struct plainstring *this_string, const struct plainstring *s) {
    if (!s)
        return;
    int size = s->LENGTH;

    if ((size < this_string->LENGTH) || (!this_string->DATA)) {
        PLAINSTRING_FREE(this_string->DATA);

        this_string->DATA = 0;
        this_string->LENGTH = 0;
        if (!size) {
            this_string->DATA_SIZE = 0;
            return;
        }
        this_string->DATA_SIZE = ((size + 1) / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
        this_string->DATA = (char *)PLAINSTRING_MALLOC(this_string->DATA_SIZE);
    } else
        plainstring_increasebuffer(this_string, size - this_string->LENGTH);

    if (size)
        MEMCPY(this_string->DATA, s->DATA, size);
    this_string->LENGTH = size;
    this_string->DATA[size] = 0;
}

void plainstring_replace_char_with_string(struct plainstring *this_string, const struct plainstring *s, intptr_t index) {
    if ((!this_string) || (!s) || (index < 0) || (index >= this_string->LENGTH))
        return;

    int slen = s->LENGTH;
    if (slen == 1) {
        this_string->DATA[index] = s->DATA[0];
    } else {
        uintptr_t len = this_string->LENGTH + s->LENGTH;
        const char *ptr = s->DATA;
        uintptr_t ptr_len = s->LENGTH;
        struct plainstring temp;
        plainstring_init(&temp);
        if (s->DATA == this_string->DATA) {
            plainstring_set_plainstring(&temp, s);
            ptr = temp.DATA;
        }
        if (len > (uintptr_t)this_string->DATA_SIZE) {
            this_string->DATA_SIZE = (len / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
            char *orig_data        = this_string->DATA;
            this_string->DATA      = (char *)PLAINSTRING_REALLOC(this_string->DATA, this_string->DATA_SIZE);
            if (!this_string->DATA) {
                this_string->DATA_SIZE = 0;
                this_string->LENGTH = 0;
                PLAINSTRING_FREE(orig_data);
            }
        }
        if (this_string->DATA) {
            memmove(this_string->DATA + index + ptr_len, this_string->DATA + index + 1, this_string->LENGTH - index - 1);
            memmove(this_string->DATA + index, ptr, ptr_len);
            this_string->LENGTH       = len - 1;
            this_string->DATA[this_string->LENGTH] = 0;
        }
        plainstring_deinit(&temp);
    }
}

intptr_t plainstring_len(const struct plainstring *this_string) {
    return this_string->LENGTH;
}

intptr_t plainstring_find(const struct plainstring *this_string, const struct plainstring *substr) {
    intptr_t result = -1;
    if (substr) {
        char *ptr = strstr(this_string->DATA, substr->DATA);
        if (ptr)
            result = ptr - this_string->DATA + 1;
    }
    return result;
}

int plainstring_computesharedsize(struct concept_FILE *infile, int type) {
    int            len;
    unsigned char  uClen = 0;
    unsigned short uSlen = 0;

    switch (type) {
        case SERIALIZE_8BIT_LENGTH:
            FREAD_FAIL(&uClen, sizeof(uClen), 1, infile);
            len = uClen;
            break;

        case SERIALIZE_16BIT_LENGTH:
            FREAD_INT_FAIL(&uSlen, sizeof(uSlen), 1, infile);
            len = uSlen;
            break;

        default:
            FREAD_INT_FAIL(&len, sizeof(len), 1, infile);
            break;
    }
    SKIP(len, infile);
#ifdef ARM_ADJUST_SIZE
    return ARM_ADJUST_SIZE(len + 1);
#else
    return len + 1;
#endif
}

void plainstring_delete(struct plainstring *this_string) {
    plainstring_deinit(this_string);
    PLAINSTRING_FREE(this_string);
}

void plainstring_deinit(struct plainstring *this_string) {
    PLAINSTRING_FREE(this_string->DATA);
}
