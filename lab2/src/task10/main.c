#include <stdio.h>
#include <stdlib.h>
#include "polynomial.h"


int main(void) {
    double *res = NULL;
    Status status;
    int deg;

    deg = 2;
    status = calc_poly(0.0001, 2.0, &res, deg, 1.0, -3.0, 2.0);
    if (status == STATUS_OK) {
        printf("Test 1 (deg %d, a = 2.0):\n", deg);

        for (int i = 0; i <= deg; i++) {
            printf("g%d = %g\n", i, res[i]);
        }

        free(res);
        res = NULL;
    }

    deg = 2;
    status = calc_poly(0.0001, 0.0, &res, deg, 1.0, -3.0, 2.0);
    if (status == STATUS_OK) {
        printf("\nTest 3 (deg %d, a = 0.0):\n", deg);

        for (int i = 0; i <= deg; i++) {
            printf("g%d = %g\n", i, res[i]);
        }

        free(res);
        res = NULL;
    }

    deg = 3;
    status = calc_poly(0.0001, 2.44, &res, deg, 1.0, -5.0, 8.0, -4.0);
    if (status == STATUS_OK) {
        printf("\nTest 4 (deg %d, a = 2.44):\n", deg);

        for (int i = 0; i <= deg; i++) {
            printf("g%d = %g\n", i, res[i]);
        }

        free(res);
        res = NULL;
    }

    return 0;
}