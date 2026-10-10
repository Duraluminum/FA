#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bases.h"

static Status print_number(const char *name, const BigInt *number, int input_base) {
    const int bases[] = {9, 18, 27, 36};
    char *text;
    size_t i;
    Status status;

    status = big_to_base(number, input_base, &text);
    if (status != STATUS_OK) return status;
    if (printf("%s: %s\n", name, text) < 0) {
        free(text);
        return STATUS_OUTPUT;
    }
    free(text);

    for (i = 0; i < 4; i++) {
        status = big_to_base(number, bases[i], &text);
        if (status != STATUS_OK) return status;
        if (printf("base %d: %s\n", bases[i], text) < 0) {
            free(text);
            return STATUS_OUTPUT;
        }
        free(text);
    }
    return STATUS_OK;
}

int main(void) {
    char *line = NULL;
    int base;
    int has_numbers = 0;
    Status status;
    BigInt number;
    BigInt sum;
    BigInt maximum;

    big_init(&number);
    big_init(&sum);
    big_init(&maximum);

    printf("Enter base [2..36]: ");
    status = read_line(&line);
    if (status == STATUS_OK) status = parse_base(line, &base);
    free(line);
    line = NULL;

    if (status != STATUS_OK) {
        printf("Invalid base\n");
        big_free(&number);
        big_free(&sum);
        big_free(&maximum);
        return 1;
    }

    printf("Enter numbers (Stop to finish):\n");
    while (1) {
        status = read_line(&line);
        if (status != STATUS_OK) break;
        if (strcmp(line, "Stop") == 0) {
            free(line);
            line = NULL;
            break;
        }

        status = big_from_string(line, base, &number);
        free(line);
        line = NULL;

        if (status == STATUS_NUMBER) {
            printf("Invalid number\n");
            continue;
        }
        if (status != STATUS_OK) break;

        if (!has_numbers) {
            status = big_copy(&maximum, &number);
            if (status != STATUS_OK) break;
            status = big_copy(&sum, &number);
            if (status != STATUS_OK) break;
            has_numbers = 1;
        } else {
            if (compare_abs(&number, &maximum) > 0) {
                status = big_copy(&maximum, &number);
                if (status != STATUS_OK) break;
            }
            status = big_add(&sum, &number);
            if (status != STATUS_OK) break;
        }
    }

    if (status == STATUS_OK && has_numbers) {
        status = print_number("Maximum by absolute value", &maximum, base);
        if (status == STATUS_OK) status = print_number("Sum", &sum, base);
    } else if (status == STATUS_OK) {
        printf("No numbers entered.\n");
    }

    big_free(&number);
    big_free(&sum);
    big_free(&maximum);
    free(line);

    return status == STATUS_OK ? 0 : 1;
}