#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "bases.h"

#define BLOCK_BASE 1000000000U
#define START_CAPACITY 4

static Status reserve(BigInt *number, size_t need) {
    uint32_t *temp;
    size_t capacity;

    if (number == NULL) return STATUS_ARGUMENT;
    if (need > SIZE_MAX / sizeof(uint32_t)) return STATUS_MEMORY;
    if (need <= number->capacity) return STATUS_OK;

    capacity = number->capacity ? number->capacity : START_CAPACITY;
    while (capacity < need) {
        if (capacity > SIZE_MAX / 2) return STATUS_MEMORY;
        capacity *= 2;
    }

    temp = realloc(number->digits, capacity * sizeof(uint32_t));
    if (temp == NULL) return STATUS_MEMORY;

    number->digits = temp;
    number->capacity = capacity;
    return STATUS_OK;
}

static void normalize(BigInt *number) {
    while (number->size > 1 && number->digits[number->size - 1] == 0) {
        number->size--;
    }
    if (number->size == 1 && number->digits[0] == 0) {
        number->sign = 0;
    }
}

static int char_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

static char value_char(int value) {
    if (value < 10) return (char)('0' + value);
    return (char)('A' + value - 10);
}

Status big_copy(BigInt *dest, const BigInt *src) {
    Status status;

    if (dest == NULL || src == NULL) return STATUS_ARGUMENT;
    if (dest == src) return STATUS_OK;

    status = reserve(dest, src->size);
    if (status != STATUS_OK) return status;

    memcpy(dest->digits, src->digits, src->size * sizeof(uint32_t));
    dest->size = src->size;
    dest->sign = src->sign;
    return STATUS_OK;
}

Status read_line(char **line) {
    char *result;
    size_t size = 0;
    size_t capacity = 16;
    int c;

    if (line == NULL) return STATUS_ARGUMENT;

    result = malloc(capacity);
    if (result == NULL) return STATUS_MEMORY;

    while (1) {
        c = getchar();
        if (c == EOF) {
            free(result);
            return STATUS_INPUT;
        }
        if (c == '\n') break;
        if (size + 1 >= capacity) {
            char *temp;
            if (capacity > SIZE_MAX / 2) {
                free(result);
                return STATUS_MEMORY;
            }
            capacity *= 2;
            temp = realloc(result, capacity);
            if (temp == NULL) {
                free(result);
                return STATUS_MEMORY;
            }
            result = temp;
        }
        result[size++] = (char)c;
    }
    result[size] = '\0';
    *line = result;
    return STATUS_OK;
}

Status parse_base(const char *line, int *base) {
    size_t i;
    int value = 0;

    if (line == NULL || base == NULL || line[0] == '\0') return STATUS_ARGUMENT;

    for (i = 0; line[i] != '\0'; i++) {
        if (line[i] < '0' || line[i] > '9') return STATUS_NUMBER;
        value = value * 10 + line[i] - '0';
        if (value > 36) return STATUS_NUMBER;
    }
    if (value < 2) return STATUS_NUMBER;

    *base = value;
    return STATUS_OK;
}

void big_init(BigInt *number) {
    if (number == NULL) return;
    number->sign = 0;
    number->size = 0;
    number->capacity = 0;
    number->digits = NULL;
}

void big_free(BigInt *number) {
    if (number == NULL) return;
    free(number->digits);
    number->digits = NULL;
    number->size = 0;
    number->capacity = 0;
    number->sign = 0;
}

Status big_from_string(const char *line, int base, BigInt *number) {
    size_t i = 0;
    int sign = 1;
    Status status;

    if (line == NULL || number == NULL || base < 2 || base > 36) return STATUS_ARGUMENT;
    if (line[0] == '\0') return STATUS_NUMBER;

    if (line[i] == '+' || line[i] == '-') {
        if (line[i] == '-') sign = -1;
        i++;
        if (line[i] == '\0') return STATUS_NUMBER;
    }

    status = reserve(number, 1);
    if (status != STATUS_OK) return status;

    number->size = 1;
    number->digits[0] = 0;
    number->sign = 1;

    for (; line[i] != '\0'; i++) {
        int digit = char_value(line[i]);
        uint64_t carry;
        size_t j;

        if (digit < 0 || digit >= base) return STATUS_NUMBER;

        carry = (uint64_t)digit;
        for (j = 0; j < number->size; j++) {
            uint64_t value = (uint64_t)number->digits[j] * base + carry;
            number->digits[j] = (uint32_t)(value % BLOCK_BASE);
            carry = value / BLOCK_BASE;
        }
        if (carry != 0) {
            status = reserve(number, number->size + 1);
            if (status != STATUS_OK) return status;
            number->digits[number->size++] = (uint32_t)carry;
        }
    }

    number->sign = sign;
    normalize(number);
    return STATUS_OK;
}

int compare_abs(const BigInt *a, const BigInt *b) {
    size_t i;

    if (a == NULL || b == NULL) return 0;
    if (a->size != b->size) return a->size > b->size ? 1 : -1;

    i = a->size;
    while (i > 0) {
        i--;
        if (a->digits[i] != b->digits[i]) {
            return a->digits[i] > b->digits[i] ? 1 : -1;
        }
    }
    return 0;
}

static Status add_abs(BigInt *a, const BigInt *b) {
    size_t i;
    size_t max_size;
    uint64_t carry = 0;
    Status status;

    max_size = a->size > b->size ? a->size : b->size;
    if (max_size == SIZE_MAX) return STATUS_MEMORY;

    status = reserve(a, max_size + 1);
    if (status != STATUS_OK) return status;

    for (i = 0; i < max_size; i++) {
        uint64_t value = carry;
        if (i < a->size) value += a->digits[i];
        if (i < b->size) value += b->digits[i];
        a->digits[i] = (uint32_t)(value % BLOCK_BASE);
        carry = value / BLOCK_BASE;
    }

    a->size = max_size;
    if (carry != 0) a->digits[a->size++] = (uint32_t)carry;

    return STATUS_OK;
}

static void subtract_abs(BigInt *a, const BigInt *b) {
    size_t i;
    int64_t borrow = 0;

    for (i = 0; i < a->size; i++) {
        int64_t value = (int64_t)a->digits[i] - (i < b->size ? b->digits[i] : 0) - borrow;
        if (value < 0) {
            value += BLOCK_BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }
        a->digits[i] = (uint32_t)value;
    }
    normalize(a);
}

Status big_add(BigInt *a, const BigInt *b) {
    int cmp;
    BigInt temp;
    Status status;

    if (a == NULL || b == NULL) return STATUS_ARGUMENT;
    if (b->sign == 0) return STATUS_OK;
    if (a->sign == 0) return big_copy(a, b);

    if (a->sign == b->sign) return add_abs(a, b);

    cmp = compare_abs(a, b);
    if (cmp == 0) {
        a->size = 1;
        a->digits[0] = 0;
        a->sign = 0;
        return STATUS_OK;
    }

    if (cmp > 0) {
        subtract_abs(a, b);
        return STATUS_OK;
    }

    big_init(&temp);
    status = big_copy(&temp, b);
    if (status == STATUS_OK) subtract_abs(&temp, a);
    if (status == STATUS_OK) {
        status = big_copy(a, &temp);
        if (status == STATUS_OK) a->sign = b->sign;
    }
    big_free(&temp);
    return status;
}

static uint32_t divide_small(BigInt *number, uint32_t divisor) {
    size_t i = number->size;
    uint64_t remainder = 0;

    while (i > 0) {
        uint64_t value;
        i--;
        value = remainder * BLOCK_BASE + number->digits[i];
        number->digits[i] = (uint32_t)(value / divisor);
        remainder = value % divisor;
    }
    normalize(number);
    return (uint32_t)remainder;
}

Status big_to_base(const BigInt *number, int base, char **result) {
    BigInt temp;
    char *text;
    size_t size = 0;
    size_t capacity = 16;
    Status status;

    if (number == NULL || result == NULL || base < 2 || base > 36) return STATUS_ARGUMENT;

    text = malloc(capacity);
    if (text == NULL) return STATUS_MEMORY;

    big_init(&temp);
    status = big_copy(&temp, number);
    if (status != STATUS_OK) {
        free(text);
        return status;
    }

    if (temp.sign == 0) {
        text[0] = '0';
        text[1] = '\0';
        big_free(&temp);
        *result = text;
        return STATUS_OK;
    }

    while (temp.sign != 0) {
        uint32_t remainder;
        if (size + 1 >= capacity) {
            char *new_text;
            if (capacity > SIZE_MAX / 2) {
                free(text);
                big_free(&temp);
                return STATUS_MEMORY;
            }
            capacity *= 2;
            new_text = realloc(text, capacity);
            if (new_text == NULL) {
                free(text);
                big_free(&temp);
                return STATUS_MEMORY;
            }
            text = new_text;
        }
        remainder = divide_small(&temp, (uint32_t)base);
        text[size++] = value_char((int)remainder);
    }

    if (number->sign < 0) {
        if (size + 1 >= capacity) {
            char *new_text;
            if (capacity > SIZE_MAX / 2) {
                free(text);
                big_free(&temp);
                return STATUS_MEMORY;
            }
            capacity *= 2;
            new_text = realloc(text, capacity);
            if (new_text == NULL) {
                free(text);
                big_free(&temp);
                return STATUS_MEMORY;
            }
            text = new_text;
        }
        text[size++] = '-';
    }

    text[size] = '\0';

    {
        size_t left = 0;
        size_t right = size - 1;
        while (left < right) {
            char c = text[left];
            text[left] = text[right];
            text[right] = c;
            left++;
            right--;
        }
    }

    big_free(&temp);
    *result = text;
    return STATUS_OK;
}