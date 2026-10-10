#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "flags2.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <flag> <args>\n", argv[0]);
        return 1;
    }

    char flag = '\0';
    Status status = parse_flag(argv[1], &flag);
    if (status != STATUS_OK) {
        fprintf(stderr, "Error parsing flag: %d\n", status);
        return 1;
    }

    switch (flag) {
        case 'q': {
            if (argc != 6) {
                fprintf(stderr, "Error: -q requires eps and 3 coefficients\n");
                return 1;
            }

            double eps = 0.0;
            status = parse_double(argv[2], &eps);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error parsing epsilon: %d\n", status);
                return 1;
            }
            if (eps <= 0.0) {
                fprintf(stderr, "Error: epsilon must be positive\n");
                return 1;
            }

            double coeffs[3];
            for (int i = 0; i < 3; i++) {
                status = parse_double(argv[3 + i], &coeffs[i]);
                if (status != STATUS_OK) {
                    fprintf(stderr, "Error parsing coefficient %d: %d\n", i + 1, status);
                    return 1;
                }
            }

            double perms[6][3];
            int perm_count = 0;
            status = unique_permutations(coeffs[0], coeffs[1], coeffs[2], eps,
                                         perms, &perm_count);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error generating permutations: %d\n", status);
                return 1;
            }

            for (int i = 0; i < perm_count; i++) {
                double a = perms[i][0];
                double b = perms[i][1];
                double c = perms[i][2];

                printf("Equation %.6g*x^2 + %.6g*x + %.6g = 0: ", a, b, c);

                double roots[2];
                int root_count = 0;
                Status rs = solve_quadratic(a, b, c, eps, roots, &root_count);

                if (rs == STATUS_ERR_NO_REAL_ROOTS) {
                    printf("No real roots in eq.\n");
                } else if (rs != STATUS_OK) {
                    printf("Error: %d\n", rs);
                } else {
                    for (int j = 0; j < root_count; j++) {
                        printf("x%d = %.10g ", j + 1, roots[j]);
                    }
                    printf("\n");
                }
            }

            break;
        }

        case 'm': {
            if (argc != 4) {
                fprintf(stderr, "Error: -m requires 2 integer arguments\n");
                return 1;
            }

            long long a = 0, b = 0;
            status = parse_int(argv[2], &a);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error parsing first number: %d\n", status);
                return 1;
            }

            status = parse_int(argv[3], &b);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error parsing second number: %d\n", status);
                return 1;
            }

            int result = 0;
            status = check_is_multiple(a, b, &result);
            if (status == STATUS_ERR_ZERO) {
                fprintf(stderr, "Error: both numbers must be non-zero\n");
                return 1;
            }
            if (status != STATUS_OK) {
                fprintf(stderr, "Error checking multiple: %d\n", status);
                return 1;
            }

            if (result) {
                printf("%lld is a multiple of %lld\n", a, b);
            } else {
                printf("%lld is not a multiple of %lld\n", a, b);
            }

            break;
        }

        case 't': {
            if (argc != 6) {
                fprintf(stderr, "Error: -t requires eps and 3 real numbers.\n");
                return 1;
            }

            double eps = 0.0;
            status = parse_double(argv[2], &eps);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error parsing epsilon: %d\n", status);
                return 1;
            }
            if (eps <= 0.0) {
                fprintf(stderr, "Error: epsilon must be positive\n");
                return 1;
            }

            double a = 0.0, b = 0.0, c = 0.0;
            status = parse_double(argv[3], &a);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error parsing side 1: %d\n", status);
                return 1;
            }

            status = parse_double(argv[4], &b);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error parsing side 2: %d\n", status);
                return 1;
            }

            status = parse_double(argv[5], &c);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error parsing side 3: %d\n", status);
                return 1;
            }

            bool result = false;
            status = check_right_triangle(a, b, c, eps, &result);
            if (status == STATUS_ERR_NOT_POSITIVE) {
                fprintf(stderr, "Error: sides must be positive\n");
                return 1;
            }
            if (status == STATUS_ERR_OUT_OF_RANGE) {
                fprintf(stderr, "Error: sides are out of safe num range\n");
                return 1;
            }
            if (status != STATUS_OK) {
                fprintf(stderr, "Error: %d\n", status);
                return 1;
            }

            if (result) {
                printf("The numbers can be sides of a right triangle\n");
            } else {
                printf("The numbers cant be sides of a right triangle\n");
            }

            break;
        }

        default:
            fprintf(stderr, "Unknown flag\n");
            return 1;
    }

    return 0;
}