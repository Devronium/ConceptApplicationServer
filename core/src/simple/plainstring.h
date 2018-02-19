#ifndef __PLAINSTRING_H
#define __PLAINSTRING_H

#include <stdint.h>
#include <stdio.h>
#include "../cacheio.h"

#define MAX_DECIMALS              0xFF
#define BLOCK_SIZE                16 

#define SERIALIZE_8BIT_LENGTH     1
#define SERIALIZE_16BIT_LENGTH    2
#define SERIALIZE_32BIT_LENGTH    4

struct plainstring {
    char *DATA;
    intptr_t DATA_SIZE;
    intptr_t LENGTH;
    uintptr_t EXTRA_DATA;
};

int plainstring_memory_length(const struct plainstring *this_string);

void plainstring_init(struct plainstring *str);
struct plainstring *plainstring_new(void);
struct plainstring *plainstring_new_str(const char *value);
struct plainstring *plainstring_new_char(char c);
struct plainstring *plainstring_new_int(int i);
struct plainstring *plainstring_new_double(double d);
struct plainstring *plainstring_new_plainstring(const struct plainstring *ps);
char plainstring_char(const struct plainstring *this_string, intptr_t index);
void plainstring_char_plainstring(const struct plainstring *this_string, intptr_t index, struct plainstring *ps);

int plainstring_equals(const struct plainstring *this_string, const char *str);
int plainstring_not_equals(const struct plainstring *this_string, const char *str);

int plainstring_equals_double(const struct plainstring *this_string, double d);
int plainstring_not_equals_double(const struct plainstring *this_string, double d);

int plainstring_equals_plainstring(const struct plainstring *this_string, const struct plainstring *ps);
int plainstring_not_equals_plainstring(const struct plainstring *this_string, const struct plainstring *ps);

int plainstring_less_plainstring(const struct plainstring *this_string, const struct plainstring *ps);
int plainstring_lessequal_plainstring(const struct plainstring *this_string, const struct plainstring *ps);

int plainstring_greater_plainstring(const struct plainstring *this_string, const struct plainstring *ps);
int plainstring_greaterequal_plainstring(const struct plainstring *this_string, const struct plainstring *ps);

int plainstring_less_double(const struct plainstring *this_string, double d);
int plainstring_lessequal_double(const struct plainstring *this_string, double d);

int plainstring_greater_double(const struct plainstring *this_string, double d);
int plainstring_greaterequal_double(const struct plainstring *this_string, double d);

int plainstring_less(const struct plainstring *this_string, const char *str);
int plainstring_lessequal(const struct plainstring *this_string, const char *str);

int plainstring_greater(const struct plainstring *this_string, const char *str);
int plainstring_greaterequal(const struct plainstring *this_string, const char *str);

void plainstring_set(struct plainstring *this_string, const char *value);
void plainstring_set_double(struct plainstring *this_string, double d);
void plainstring_set_int(struct plainstring *this_string, int i);
void plainstring_set_char(struct plainstring *this_string, char c);
void plainstring_set_plainstring(struct plainstring *this_string, const struct plainstring *ps);

void plainstring_add(struct plainstring *this_string, const char *value);
void plainstring_add_char(struct plainstring *this_string, char c);
void plainstring_add_plainstring(struct plainstring *this_string, const struct plainstring *ps);
void plainstring_add_double(struct plainstring *this_string, double d);
void plainstring_add_int(struct plainstring *this_string, int value);

const char *plainstring_c_str(const struct plainstring *this_string);

struct plainstring *plainstring_sum(struct plainstring *this_string, const struct plainstring *other);
struct plainstring *plainstring_sum_str(struct plainstring *this_string, const char *other);

intptr_t plainstring_int(const struct plainstring *this_string);
double plainstring_float(const struct plainstring *this_string);

int plainstring_loadfile(struct plainstring *this_string, const char *filename);
int plainstring_savefile(const struct plainstring *this_string, const char *filename);

void plainstring_loadbuffer(struct plainstring *this_string, const char *buffer, int size);
void plainstring_addbuffer(struct plainstring *this_string, const char *buffer, int size);
void plainstring_linkbuffer(struct plainstring *this_string, char *buffer, int size);

void plainstring_increasebuffer(struct plainstring *this_string, int size);
void plainstring_sum_of_2(struct plainstring *this_string, const struct plainstring *s1, const struct plainstring *s2);
void plainstring_asg(struct plainstring *this_string, const struct plainstring *s);
void plainstring_replace_char_with_string(struct plainstring *this_string, const struct plainstring *s, intptr_t index);
intptr_t plainstring_len(const struct plainstring *this_string);
intptr_t plainstring_find(const struct plainstring *this_string, const struct plainstring *substr);

int plainstring_computesharedsize(struct concept_FILE *in, int type);

void plainstring_deinit(struct plainstring *this_string);
void plainstring_delete(struct plainstring *this_string);

void cstr_loaddouble(char *buffer, double d);

#endif

