#ifndef FLAGS_H
#define FLAGS_H

#include <stdbool.h>


typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_NULL,
    STATUS_ERR_EMPTY_STRING,
    STATUS_ERR_NOT_A_NUMBER,
    STATUS_ERR_TOO_LARGE,
    STATUS_ERR_NOT_POSITIVE,
    STATUS_ERR_ZERO,
    STATUS_ERR_OUT_OF_RANGE,
    STATUS_ERR_UNKNOWN_FLAG,
    STATUS_ERR_NO_REAL_ROOTS,
    STATUS_ERR_BAD_ARG_COUNT
} Status;

Status parse_flag(const char* const arg, char* const out);

Status parse_int(const char *str, long long *out);
Status parse_double(const char *str, double *out);
Status check_is_multiple(long long a, long long b, int *result);
Status solve_quadratic(double a, double b, double c, double eps, double roots[2], int *count);
Status unique_permutations(double a, double b, double c, double eps, double permutations[6][3], int *count);
Status check_right_triangle(double a, double b, double c, double eps, bool *result);


#endif