#include <stdio.h>
#include <stdlib.h>
#include "bases2.h"


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <decimal_number> <r: 1..5>\n", argv[0]);
        return 1;
    }

    unsigned long long value, r_val;
    if (!parse_unsigned(argv[1], &value) || !parse_unsigned(argv[2], &r_val) || r_val < 1 || r_val > 5) {
        fprintf(stderr, "Invalid arguments. Usage: %s <decimal_number> <r: 1..5>\n", argv[0]);
        return 1;
    }

    char *result = NULL;
    BaseStatus status = decimal_power(value, (unsigned int)r_val, &result);
    if (status != BASE_OK) {
        fprintf(stderr, "Conversion error: %d\n", status);
        return 1;
    }

    printf("Base 2^%llu: %s\n", r_val, result);
    free(result);
    return 0;
}