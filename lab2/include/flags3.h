#ifndef FLAGS_H
#define FLAGS_H

#include <stddef.h>


typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_NULL,
    STATUS_ERR_EMPTY_STR,
    STATUS_ERR_NOT_A_NUMBER,
    STATUS_ERR_NOT_NATURAL,
    STATUS_ERR_TOO_LARGE,
    STATUS_ERR_OVERFLOW,
    STATUS_ERR_RANGE,
    STATUS_ERR_UNKNOWN_FLAG,
    STATUS_ERR_MEMORY
} Status;

Status parse_flag(const char *arg, char *out);
Status parse_seed(const char *str, unsigned int *seed);
Status string_length(const char *str, size_t *length);
Status reverse_string(const char *str, char **result);
Status odd_upper_string(const char *str, char **result);
Status rearrange_string(const char *str, char **result);
Status concatenate_strings(unsigned int seed, size_t count, const char *const *strings, char **result);


#endif