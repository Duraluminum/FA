#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "flags.h"

#define CUSTOM_LLONG_MAX ((long long)((~(unsigned long long)0) >> 1))


Status parse_x(const char *str, long long *out) {
    if(str == NULL || out == NULL) return STATUS_ERR_NULL;
    if(*str == '\0') return STATUS_ERR_EMPTY_STR;
    
    long long val = 0;
    int has_digit = 0;
    for (const char *p = str; *p != '\0'; p++) {
        if (*p < '0' || *p > '9') return STATUS_ERR_NOT_A_NUMBER;

        int digit = *p - '0';
        has_digit = 1;
        if (val > (CUSTOM_LLONG_MAX - digit) / 10) return STATUS_ERR_TOO_LARGE;

        val = val * 10 + digit;
    }

    if(!has_digit) return STATUS_ERR_NOT_A_NUMBER;
    if(val < 1) return STATUS_ERR_NOT_NATURAL;

    *out = val;
    return STATUS_OK;
}

Status parse_flag(const char* const arg, char* const out) {
    if (!arg || !out) return STATUS_ERR_NULL;
    if (arg[0] != '/' && arg[0] != '-') return STATUS_ERR_UNKNOWN_FLAG;
    if (strlen(arg) != 2) return STATUS_ERR_UNKNOWN_FLAG;

    const char f = arg[1];
    if (f != 'h' && f != 'p' && f != 's' && f != 'e' && f != 'a' && f != 'f') {
        return STATUS_ERR_UNKNOWN_FLAG;
    }

    *out = f;
    return STATUS_OK;
}

Status multiples(long long x, int *count) {
    if (count == NULL) return STATUS_ERR_NULL;
    if (x < 1) return STATUS_ERR_NOT_NATURAL;

    int cnt = 0;
    if (x > 100) {
        printf("No multiples of %lld in the range of 100.\n", x);
        *count = 0;
        return STATUS_OK;
    }

    for (long long n = x; n <= 100; n += x) {
        printf("%lld ", n);
        cnt++;
    }

    if (cnt == 0) {
        printf("No multiples of %lld in the range of 100.\n", x);
    } else {
        printf("\n");
    }

    *count = cnt;
    return STATUS_OK;
}

Status check_prime(long long x, bool *is_prime) {
    if (is_prime == NULL) return STATUS_ERR_NULL;
    if (x < 1) return STATUS_ERR_NOT_NATURAL;
    if (x < 2) {
        *is_prime = false;
        return STATUS_OK;
    }

    for (long long i = 2; i <= x / i; i++) {
        if (x % i == 0) {
            *is_prime = 0;
            return STATUS_OK;
        }
    }

    *is_prime = 1;
    return STATUS_OK;
}

Status print_hex(long long x) {
    if (x < 0) return STATUS_ERR_NOT_NATURAL;
    if (x == 0) {
        printf("0\n");
        return STATUS_OK;
    }

    char buf[32];
    int ind = 0;
    unsigned long long val = (unsigned long long)x;
    while (val > 0) {
        int digit = (int)(val % 16);
        buf[ind++] = "0123456789ABCDEF"[digit];
        val /= 16;
    }

    for (int i = ind - 1; i >= 0; i--) {
        printf("%c", buf[i]);
        if (i > 0) printf(" ");
    }
    printf("\n");

    return STATUS_OK;
}

Status print_pow_table(long long x) {
    if (x < 1) return STATUS_ERR_NOT_NATURAL;
    if (x > 10) return STATUS_ERR_RANGE;

    for (int base = 1; base <= 10; base++) {
        printf("base %d: ", base);

        long long val = 1;
        for (long long pow = 1; pow <= x; pow++) {
            if (val > CUSTOM_LLONG_MAX / base) return STATUS_ERR_OVERFLOW;

            val *= base;
            printf("%lld ", val);
        }
        printf("\n");
    }

    return STATUS_OK;
}

Status print_sum(long long x, long long *sum) {
    if (sum == NULL) return STATUS_ERR_NULL;
    if (x < 1) return STATUS_ERR_NOT_NATURAL;
    if (x > 4000000000LL) return STATUS_ERR_OVERFLOW;

    long long a = x;
    long long b = x + 1;
    if (a % 2 == 0) a /= 2;
    else b /= 2;

    *sum = a * b;
    return STATUS_OK;
}

Status fact(long long x, unsigned long long *fact) {
    if (fact == NULL) return STATUS_ERR_NULL;
    if (x < 1) return STATUS_ERR_NOT_NATURAL;
    if (x >= 21) return STATUS_ERR_OVERFLOW;

    unsigned long long val = 1;
    for (long long i = 2; i <= x; i++) {
        val *= (unsigned long long)i;
    }

    *fact = val;
    return STATUS_OK;
}