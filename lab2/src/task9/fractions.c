#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "fractions.h"


static unsigned long long gcd(unsigned long long a, unsigned long long b) {
    while (b != 0) {
        unsigned long long temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}

static int is_prime(unsigned int n) {
    if (n < 2) return 0;

    for (unsigned int i = 2; i <= n / i; i++)
        if (n % i == 0) return 0;

    return 1;
}

static int has_finite_representation(unsigned long long p, unsigned long long q, unsigned int k) {
    unsigned long long g = gcd(p, q);
    if (g > 1) {
        p /= g;
        q /= g;
    }

    if (q == 1) return 1;

    unsigned int temp_k = k;
    for (unsigned int i = 2; i <= temp_k; i++) {
        if (temp_k % i == 0 && is_prime(i)) {
            while (q % i == 0) q /= i;

            while (temp_k % i == 0) temp_k /= i;
        }
    }

    return q == 1;
}

static Status check_fraction_str(const char *str, unsigned int base) {
    if (str == NULL) return STATUS_ERR_NULL;
    
    const char *slash = str;
    while (*slash != '\0' && *slash != '/')
        slash++;
    
    unsigned long long p = 0;
    unsigned long long q = 1;
    if (*slash == '/') {
        const char *s = str;
        if (*s < '0' || *s > '9') return STATUS_ERR_BAD_NUMBER;

        while (*s >= '0' && *s <= '9') {
            p = p * 10 + (unsigned int)(*s - '0');
            s++;
        }

        while (*s == ' ') s++;
        if (*s != '/') return STATUS_ERR_BAD_NUMBER;

        s++;
        while (*s == ' ') s++;

        if (*s < '0' || *s > '9') return STATUS_ERR_BAD_NUMBER;

        q = 0;
        while (*s >= '0' && *s <= '9') {
            q = q * 10 + (unsigned int)(*s - '0');
            s++;
        }

        while (*s == ' ') s++;
        if (*s != '\0') return STATUS_ERR_BAD_NUMBER;

        if (q == 0) return STATUS_ERR_BAD_NUMBER;
    } else {
        const char *frac = str;
        if (str[0] == '0' && str[1] == '.') {
            frac = str + 2;
        } else if (str[0] == '.') {
            frac = str + 1;
        } else {
            return STATUS_ERR_BAD_NUMBER;
        }

        if (*frac == '\0') return STATUS_ERR_BAD_NUMBER;

        p = 0;
        q = 1;
        int has_digit = 0;
        for (const char *s = frac; *s != '\0'; s++) {
            if (*s < '0' || *s > '9') return STATUS_ERR_BAD_NUMBER;

            unsigned int digit = (unsigned int)(*s - '0');
            p = p * 10 + digit;
            q *= 10;
            has_digit = 1;

            unsigned long long g = gcd(p, q);
            if (g > 1) {
                p /= g;
                q /= g;
            }
        }

        if (!has_digit) return STATUS_ERR_BAD_NUMBER;
    }

    if (p >= q) return STATUS_ERR_RANGE;

    if (has_finite_representation(p, q, base)) {
        printf("Fraction %s in base %u is finite\n", str, base);
    } else {
        printf("Fraction %s in base %u is infinite\n", str, base);
    }

    return STATUS_OK;
}

Status process_fractions(unsigned int base, int count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        const char *str = va_arg(args, const char *);
        Status status = check_fraction_str(str, base);

        if (status != STATUS_OK) {
            fprintf(stderr, "Error: invalid fraction '%s'; code: %d\n", str, (int)status);
        }
    }
    va_end(args);

    return STATUS_OK;
}