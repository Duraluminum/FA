#include <stdlib.h>
#include <string.h>
#include "bases2.h"


int parse_unsigned(const char *text, unsigned long long *value) {
    if (text == NULL || text[0] == '\0' || value == NULL) return 0;

    unsigned long long number = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] < '0' || text[i] > '9') return 0;

        unsigned int digit = (unsigned int)(text[i] - '0');
        if (number > (ULLONG_MAX - digit) / 10ULL) return 0;
        number = number * 10ULL + digit;
    }

    *value = number;
    return 1;
}

BaseStatus decimal_power(unsigned long long value, unsigned int r, char **result) {
    if (result == NULL) return BASE_ARGUMENT;
    if (r < 1 || r > 5) return BASE_VALUE;

    const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
    unsigned long long mask = ~(~0ULL << r);
    char buffer[66];
    size_t length = 0;

    if (value == 0) {
        buffer[length++] = '0';
    }

    while (value != 0) {
        unsigned int digit = (unsigned int)(value & mask);
        buffer[length++] = digits[digit];
        value >>= r;
    }

    char *text = malloc(length + 1);
    if (text == NULL) return BASE_MEMORY;

    for (size_t i = 0; i < length; i++)
        text[i] = buffer[length - i - 1];

    text[length] = '\0';
    *result = text;
    return BASE_OK;
}