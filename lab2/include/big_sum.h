#ifndef BIG_SUM_H
#define BIG_SUM_H

#include <stddef.h>


typedef enum {
    SUM_OK = 0,
    SUM_ARGUMENT,
    SUM_VALUE,
    SUM_MEMORY
} SumStatus;

int parse_base(const char *text, int *base);

SumStatus add_base_strings(const char *first, const char *second, int base, char **result);
SumStatus sum_base_array(int base, size_t count, const char *const *numbers, char **result);


#endif