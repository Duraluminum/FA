#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "big_sum.h"


static int digit_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

static char digit_char(int value) {
    return value < 10 ? (char)('0' + value) : (char)('a' + value - 10);
}

static SumStatus validate_number(const char *number, int base) {
    if (number == NULL || number[0] == '\0') return SUM_VALUE;

    for (size_t i = 0; number[i] != '\0'; i++) {
        int digit = digit_value(number[i]);
        if (digit < 0 || digit >= base) return SUM_VALUE;
    }

    return SUM_OK;
}

int parse_base(const char *text, int *base) {
    if (text == NULL || text[0] == '\0' || text[0] == '-') return 0;

    char *end;
    long value;
    value = strtol(text, &end, 10);
    if (*end != '\0' || end == text || value < 2 || value > 36) return 0;

    *base = (int)value;
    return 1;
}

SumStatus add_base_strings(const char *first, const char *second, int base, char **result) {
    if (first == NULL || second == NULL || result == NULL) return SUM_ARGUMENT;
    if (base < 2 || base > 36) return SUM_ARGUMENT;

    size_t length_first, length_second, capacity, used = 0;
    char *reversed, *text;
    int carry = 0;
    SumStatus status;

    status = validate_number(first, base);
    if (status != SUM_OK) return status;
    status = validate_number(second, base);
    if (status != SUM_OK) return status;

    length_first = strlen(first);
    length_second = strlen(second);
    if (length_first > SIZE_MAX - length_second - 2) return SUM_MEMORY;

    capacity = length_first > length_second ? length_first : length_second;
    if (capacity > SIZE_MAX - 2) return SUM_MEMORY;

    reversed = malloc(capacity + 2);
    if (reversed == NULL) return SUM_MEMORY;

    size_t i = 0, j = 0;
    while (i < length_first || j < length_second || carry != 0) {
        int value = carry;

        if (i < length_first) value += digit_value(first[length_first - i - 1]);
        if (j < length_second) value += digit_value(second[length_second - j - 1]);

        reversed[used++] = digit_char(value % base);
        carry = value / base;
        i++, j++;
    }

    while (used > 1 && reversed[used - 1] == '0') used--;

    text = malloc(used + 1);
    if (text == NULL) {
        free(reversed);
        return SUM_MEMORY;
    }

    for (i = 0; i < used; i++)
        text[i] = reversed[used - i - 1];
    
    text[used] = '\0';
    free(reversed);
    *result = text;

    return SUM_OK;
}

SumStatus sum_base_array(int base, size_t count, const char *const *numbers, char **result) {
    if (numbers == NULL || result == NULL || count == 0 || base < 2 || base > 36) return SUM_ARGUMENT;

    char *sum;
    sum = malloc(2);
    if (sum == NULL) return SUM_MEMORY;
    sum[0] = '0';
    sum[1] = '\0';

    SumStatus status;
    for (size_t i = 0; i < count; i++) {
        char *next = NULL;
        status = add_base_strings(sum, numbers[i], base, &next);
        free(sum);

        if (status != SUM_OK) return status;
        sum = next;
    }

    *result = sum;
    return SUM_OK;
}