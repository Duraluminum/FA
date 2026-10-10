#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "flags.h"


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number> <flag>\n", argv[0]);
        return 1;
    }

    long long x = 0;
    Status status = parse_x(argv[1], &x);
    if (status != STATUS_OK) {
        fprintf(stderr, "Error parsing number: %d\n", status);
        return 1;
    }

    char flag = '\0';
    status = parse_flag(argv[2], &flag);
    if (status != STATUS_OK) {
        fprintf(stderr, "Error parsing flag: %d\n", status);
        return 1;
    }

    switch (flag) {
        case 'h': {
            int count = 0;
            status = multiples(x, &count);

            if (status != STATUS_OK) {
                fprintf(stderr, "Error in multiples: %d\n", status);
                return 1;
            }

            break;
        }
        case 'p': {
            bool is_prime = false;
            status = check_prime(x, &is_prime);

            if (status != STATUS_OK) {
                fprintf(stderr, "Error in check_prime: %d\n", status);
                return 1;
            }

            if (is_prime) printf("%lld is prime\n", x);
            else printf("%lld is composite\n", x);
            
            break;
        }
        case 's': {
            status = print_hex(x);

            if (status != STATUS_OK) {
                fprintf(stderr, "Error in print_hex: %d\n", status);
                return 1;
            }

            break;
        }
        case 'e': {
            status = print_pow_table(x);

            if (status != STATUS_OK) {
                fprintf(stderr, "Error in print_pow_table: %d\n", status);
                return 1;
            }

            break;
        }
        case 'a': {
            long long sum = 0;
            status = print_sum(x, &sum);

            if (status != STATUS_OK) {
                fprintf(stderr, "Error in print_sum: %d\n", status);
                return 1;
            }
            
            printf("Sum from 1 to %lld is %lld\n", x, sum);
            break;
        }
        case 'f': {
            unsigned long long factorial = 0;
            status = fact(x, &factorial);

            if (status != STATUS_OK) {
                fprintf(stderr, "Error in fact: %d\n", status);
                return 1;
            }

            printf("Factorial of %lld is %llu\n", x, factorial);
            break;
        }
        default:
            fprintf(stderr, "Unknown flag\n");
            return 1;
    }

    return 0;
}