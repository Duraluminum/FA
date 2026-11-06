#ifndef BASES2_H
#define BASES2_H

#include <stddef.h>


typedef enum {
    BASE_OK = 0,
    BASE_ARGUMENT,
    BASE_VALUE,
    BASE_MEMORY
} BaseStatus;

int parse_unsigned(const char *text, unsigned long long *value);
BaseStatus decimal_power(unsigned long long value, unsigned int r, char **result);


#endif