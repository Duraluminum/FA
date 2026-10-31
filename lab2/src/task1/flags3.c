#include <stdlib.h>
#include <limits.h>
#include "flags3.h"


static size_t my_strlen(const char *str) {
    size_t length = 0;
    while (str[length] != '\0')
        length++;

    return length;
}

static int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static int is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static char to_upper(char c) {
    if (c >= 'a' && c <= 'z') return (char)(c - 'a' + 'A');

    return c;
}

Status parse_flag(const char *arg, char *out) {
    if (arg == NULL || out == NULL) return STATUS_ERR_NULL;
    if ((arg[0] != '-' && arg[0] != '/') || arg[1] == '\0' || arg[2] != '\0') return STATUS_ERR_UNKNOWN_FLAG;
    if (arg[1] != 'l' && arg[1] != 'r' && arg[1] != 'u' && arg[1] != 'n' && arg[1] != 'c') return STATUS_ERR_UNKNOWN_FLAG;

    *out = arg[1];
    return STATUS_OK;
}

Status parse_seed(const char *str, unsigned int *seed) {
    if (str == NULL || seed == NULL) return STATUS_ERR_NULL;
    if (str[0] == '\0') return STATUS_ERR_EMPTY_STR;

    unsigned int value = 0;
    for (size_t i = 0; str[i] != '\0'; i++) {
        unsigned int digit;
        if (!is_digit(str[i])) return STATUS_ERR_NOT_A_NUMBER;

        digit = (unsigned int)(str[i] - '0');
        if (value > (UINT_MAX - digit) / 10U) return STATUS_ERR_TOO_LARGE;
        
        value = value * 10U + digit;
    }

    *seed = value;
    return STATUS_OK;
}

Status string_length(const char *str, size_t *length) {
    if (str == NULL || length == NULL) return STATUS_ERR_NULL;

    *length = my_strlen(str);
    return STATUS_OK;
}

Status reverse_string(const char *str, char **result) {
    if (str == NULL || result == NULL) return STATUS_ERR_NULL;
    
    size_t length;
    char *text;
    length = my_strlen(str);
    if (length == (size_t)-1) return STATUS_ERR_TOO_LARGE;

    text = malloc(length + 1);
    if (text == NULL) return STATUS_ERR_MEMORY;

    for (size_t i = 0; i < length; i++)
        text[i] = str[length - i-1];
    text[length] = '\0';

    *result = text;
    return STATUS_OK;
}

Status odd_upper_string(const char *str, char **result) {
    if (str == NULL || result == NULL) return STATUS_ERR_NULL;

    size_t length;
    char *text;
    length = my_strlen(str);
    if (length == (size_t)-1) return STATUS_ERR_TOO_LARGE;

    text = malloc(length + 1);
    if (text == NULL) return STATUS_ERR_MEMORY;

    for (size_t i = 0; i < length; i++)
        text[i] = i % 2 == 1 ? to_upper(str[i]) : str[i];
    text[length] = '\0';

    *result = text;
    return STATUS_OK;
}

Status rearrange_string(const char *str, char **result) {
    if (str == NULL || result == NULL) return STATUS_ERR_NULL;

    size_t length, pos = 0;
    char *text;
    length = my_strlen(str);
    if (length == (size_t)-1) return STATUS_ERR_TOO_LARGE;

    text = malloc(length + 1);
    if (text == NULL) return STATUS_ERR_MEMORY;

    for (size_t i = 0; i < length; i++) if (is_digit(str[i])) text[pos++] = str[i];
    for (size_t i = 0; i < length; i++) if (is_letter(str[i])) text[pos++] = str[i];
    for (size_t i = 0; i < length; i++) if (!is_digit(str[i]) && !is_letter(str[i])) text[pos++] = str[i];
    text[pos] = '\0';

    *result = text;
    return STATUS_OK;
}

Status concatenate_strings(unsigned int seed, size_t count, const char *const *strings, char **result) {
    if (strings == NULL || result == NULL) return STATUS_ERR_NULL;
    if (count == 0 || count > (size_t)-1 / sizeof(size_t)) return STATUS_ERR_RANGE;

    size_t *order;
    order = malloc(count * sizeof(size_t));
    if (order == NULL) return STATUS_ERR_MEMORY;

    size_t total = 0, pos = 0;
    for (size_t i = 0; i < count; i++) {
        size_t length;

        if (strings[i] == NULL) {
            free(order);
            return STATUS_ERR_NULL;
        }

        length = my_strlen(strings[i]);
        if (total > (size_t)-1 - length) {
            free(order);
            return STATUS_ERR_TOO_LARGE;
        }

        total += length;
        order[i] = i;
    }

    if (total == (size_t)-1) {
        free(order);
        return STATUS_ERR_TOO_LARGE;
    }

    char *text;
    text = malloc(total + 1);
    if (text == NULL) {
        free(order);
        return STATUS_ERR_MEMORY;
    }

    srand(seed);
    for (size_t i = count; i > 1; i--) {
        size_t j = (size_t)rand() % i;
        
        size_t temp = order[i-1];
        order[i-1] = order[j];
        order[j] = temp;
    }

    for (size_t i = 0; i < count; i++) {
        for (size_t j = 0; strings[order[i]][j] != '\0'; j++)
            text[pos++] = strings[order[i]][j];
    }

    text[pos] = '\0';
    
    free(order);
    *result = text;
    return STATUS_OK;
}