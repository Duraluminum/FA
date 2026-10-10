#ifndef FLAGS_H
#define FLAGS_H

#include <stdbool.h>


typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_NULL,
    STATUS_ERR_EMPTY_STR,
    STATUS_ERR_NOT_A_NUMBER,
    STATUS_ERR_NOT_NATURAL,
    STATUS_ERR_TOO_LARGE,
    STATUS_ERR_OVERFLOW,
    STATUS_ERR_RANGE,
    STATUS_ERR_UNKNOWN_FLAG
} Status;

Status parse_x(const char *str, long long *out);
Status parse_flag(const char* const arg, char* const out);
Status multiples(long long x, int *count);
Status check_prime(long long x, bool *is_prime);
Status print_hex(long long x);
Status print_pow_table(long long x);
Status print_sum(long long x, long long *sum);
Status fact(long long x, unsigned long long *fact);


#endif