#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "big_sum.h"


int main(int argc, char *argv[]) {
    int base;
    char *result = NULL;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <base> <number1> [number2 ...]\n", argv[0]);
        return 1;
    }

    if (!parse_base(argv[1], &base)) {
        fprintf(stderr, "Invalid base\n");
        return 1;
    }

    SumStatus status = sum_base_array(base, (size_t)argc - 2, (const char *const *)(argv + 2), &result);

    if (status != SUM_OK) {
        fprintf(stderr, "Error summing numbers: %d\n", status);
        free(result);
        return 1;
    }

    printf("Sum: %s\n", result);
    free(result);

    return 0;
}