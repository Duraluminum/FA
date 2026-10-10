#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>
#include "flags2.h"


Status parse_flag(const char* const arg, char* const out) {
    if (!arg || !out) return STATUS_ERR_NULL;
    if (arg[0] != '/' && arg[0] != '-') return STATUS_ERR_UNKNOWN_FLAG;
    if (strlen(arg) != 2) return STATUS_ERR_UNKNOWN_FLAG;

    const char f = arg[1];
    if (f != 'q' && f != 'm' && f != 't') {
        return STATUS_ERR_UNKNOWN_FLAG;
    }

    *out = f;
    return STATUS_OK;
}

Status parse_int(const char *str, long long *out) {
    if (str == NULL || out == NULL) return STATUS_ERR_NULL;
    if (*str == '\0') return STATUS_ERR_EMPTY_STRING;
    
    int sign = 1;
    const char *p = str;
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    if (*p == '\0') return STATUS_ERR_NOT_A_NUMBER;

    long long value = 0;
    for (; *p != '\0'; p++) {
        if (*p < '0' || *p > '9') return STATUS_ERR_NOT_A_NUMBER;
        
        int digit = *p - '0';
        if (value > (LLONG_MAX - digit) / 10) return STATUS_ERR_TOO_LARGE;
        
        value = value * 10 + digit;
    }

    *out = sign * value;
    return STATUS_OK;
}

Status parse_double(const char *str, double *out) {
    if (str == NULL || out == NULL) return STATUS_ERR_NULL;
    if (*str == '\0') return STATUS_ERR_EMPTY_STRING;

    int sign = 1;
    const char *p = str;
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    
    if (*p == '\0') return STATUS_ERR_NOT_A_NUMBER;
    
    double value = 0.0;
    bool has_digit = false;
    for (; *p >= '0' && *p <= '9'; p++) {
        value = value * 10.0 + (*p - '0');
        has_digit = true;
    }

    if (*p == '.') {
        p++;
        double factor = 0.1;
        for (; *p >= '0' && *p <= '9'; p++) {
            value += (*p - '0') * factor;
            factor *= 0.1;
            has_digit = true;
        }
    }

    if (!has_digit) return STATUS_ERR_NOT_A_NUMBER;
    if (*p != '\0') return STATUS_ERR_NOT_A_NUMBER;
    if (!isfinite(value)) return STATUS_ERR_TOO_LARGE;
    
    *out = sign * value;
    return STATUS_OK;
}

Status check_is_multiple(long long a, long long b, int *result) {
    if (result == NULL) return STATUS_ERR_NULL;
    if (a == 0 || b == 0) return STATUS_ERR_ZERO;
    
    *result = (a % b == 0) ? 1 : 0;
    return STATUS_OK;
}

Status solve_quadratic(double a, double b, double c, double eps, double roots[2], int *count) {
    if (roots == NULL || count == NULL) return STATUS_ERR_NULL;
    if (eps <= 0.0) return STATUS_ERR_NOT_POSITIVE;

    if (fabs(a) < eps) {
        if (fabs(b) < eps) {
            *count = 0;
            return STATUS_ERR_NO_REAL_ROOTS;
        }
        roots[0] = -c / b;
        *count = 1;

        return STATUS_OK;
    }

    double d = b * b - 4.0 * a * c;

    if (d < -eps) {
        *count = 0;
        return STATUS_ERR_NO_REAL_ROOTS;
    }

    if (fabs(d) < eps) {
        roots[0] = -b / (2.0 * a);
        *count = 1;
        return STATUS_OK;
    }

    double sqrt_d = sqrt(d);
    roots[0] = (-b - sqrt_d) / (2.0 * a);
    roots[1] = (-b + sqrt_d) / (2.0 * a);
    *count = 2;
    return STATUS_OK;
}

static int triples_equal(double a1, double b1, double c1, double a2, double b2, double c2, double eps) {
    return fabs(a1 - a2) < eps && fabs(b1 - b2) < eps && fabs(c1 - c2) < eps;
}

Status unique_permutations(double a, double b, double c, double eps, double permutations[6][3], int *count) {
    if (permutations == NULL || count == NULL) return STATUS_ERR_NULL;

    double all[6][3] = {
        { a, b, c }, { a, c, b },
        { b, a, c }, { b, c, a },
        { c, a, b }, { c, b, a }
    };

    int n = 0;
    for (int i = 0; i < 6; i++) {
        bool duplicate = false;
        for (int j = 0; j < n; j++) {
            if (triples_equal(all[i][0], all[i][1], all[i][2], permutations[j][0], permutations[j][1], permutations[j][2], eps)) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            permutations[n][0] = all[i][0];
            permutations[n][1] = all[i][1];
            permutations[n][2] = all[i][2];
            n++;
        }
    }

    *count = n;
    return STATUS_OK;
}

Status check_right_triangle(double a, double b, double c, double eps, bool *result) {
    if (result == NULL) return STATUS_ERR_NULL;
    if (eps <= 0.0) return STATUS_ERR_NOT_POSITIVE;
    if (a <= eps || b <= eps || c <= eps) 
        return STATUS_ERR_NOT_POSITIVE;
    
    const double MAX_SAFE = sqrt(DBL_MAX);
    const double MIN_SAFE = sqrt(DBL_MIN);
    if (a > MAX_SAFE || b > MAX_SAFE || c > MAX_SAFE ||
        a < MIN_SAFE || b < MIN_SAFE || c < MIN_SAFE)
        return STATUS_ERR_OUT_OF_RANGE;

    double x = a, y = b, z = c;
    double tmp;
    if (x > y) { tmp = x; x = y; y = tmp; }
    if (y > z) { tmp = y; y = z; z = tmp; }
    if (x > y) { tmp = x; x = y; y = tmp; }

    double catets = x * x + y * y;
    double hypoth = z * z;

    *result = (fabs(catets - hypoth) < eps) ? true : false;
    return STATUS_OK;
}