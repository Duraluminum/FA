#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "polynomial.h"


static void horner_shift(double *g, size_t n, double a) {
    for (size_t k = 0; k < n; k++) {
        for (size_t i = 1; i < n - k; i++) {
            g[i] = g[i] + g[i - 1] * a;
        }
    }

    for (size_t i = 0; i < n / 2; i++) {
        double temp = g[i];
        g[i] = g[n - 1 - i];
        g[n - 1 - i] = temp;
    }
}

Status calc_poly(const double eps, const double a, double **out, const int deg, ...) {
    if (out == NULL || deg < 0 || eps <= 0.0) return STATUS_ERR_BAD_DEGREE;

    size_t n = (size_t)deg + 1;
    double *g = malloc(n * sizeof(double));
    if (g == NULL) return STATUS_ERR_NO_MEMORY;

    va_list args;
    va_start(args, deg);

    for (size_t i = 0; i < n; i++) {
        g[n - 1 - i] = va_arg(args, double);
    }
    va_end(args);

    horner_shift(g, n, a);

    for (size_t i = 0; i < n; i++) {
        if (fabs(g[i]) < eps) {
            g[i] = 0.0;
        }
    }

    *out = g;
    return STATUS_OK;
}