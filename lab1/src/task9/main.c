#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arrays.h"


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <a> <b>\n", argv[0]);
        return 1;
    }

    int a, b;
    Status status;

    status = parse_int(argv[1], &a);
    if (status != STATUS_OK) {
        printf("Invalid value of a\n");
        return 1;
    }

    status = parse_int(argv[2], &b);
    if (status != STATUS_OK) {
        printf("Invalid value of b\n");
        return 1;
    }

    if (a > b) {
        printf("Error: a must be less than or equal to b\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    int array[ARRAY_SIZE];
    status = fill_array(array, ARRAY_SIZE, a, b);
    if (status != STATUS_OK) {
        printf("Error while filling the fixed array\n");
        return 1;
    }

    printf("Part 1\n");
    printf("Original array:\n");
    status = print_array(array, ARRAY_SIZE);
    if (status != STATUS_OK) {
        printf("Error while printing the array\n");
        return 1;
    }

    size_t min_index, max_index;
    status = find_min_max(array, ARRAY_SIZE, &min_index, &max_index);
    if (status != STATUS_OK) {
        printf("Error while finding minimum and maximum\n");
        return 1;
    }

    status = swap_elements(array, ARRAY_SIZE, min_index, max_index);
    if (status != STATUS_OK) {
        printf("Error while swapping elements\n");
        return 1;
    }

    printf("Array after swapping minimum and maximum:\n");
    status = print_array(array, ARRAY_SIZE);
    if (status != STATUS_OK) {
        printf("Error while printing the array\n");
        return 1;
    }

    printf("\nPart 2\n");
    int *array_a = NULL;
    int *array_b = NULL;
    int *array_c = NULL;
    size_t size_a = 0;
    size_t size_b = 0;

    status = create_random_array(&array_a, &size_a);
    if (status != STATUS_OK) {
        printf("Error while creating array A\n");
        return 1;
    }

    status = create_random_array(&array_b, &size_b);
    if (status != STATUS_OK) {
        printf("Error while creating array B\n");
        free(array_a);
        return 1;
    }

    status = create_array_c(array_a, size_a, array_b, size_b, &array_c);
    if (status != STATUS_OK) {
        printf("Error while creating array C\n");
        free(array_a);
        free(array_b);
        return 1;
    }

    printf("Size of A: %zu\n", size_a);
    printf("Size of B: %zu\n", size_b);
    printf("Size of C: %zu\n", size_a);

    printf("\nA:\n");
    status = print_array(array_a, size_a);
    if (status != STATUS_OK) {
        printf("Error while printing A\n");
        free(array_a);
        free(array_b);
        free(array_c);
        return 1;
    }

    printf("\nB:\n");
    status = print_array(array_b, size_b);
    if (status != STATUS_OK) {
        printf("Error while printing B\n");
        free(array_a);
        free(array_b);
        free(array_c);
        return 1;
    }

    printf("\nC:\n");
    status = print_array(array_c, size_a);
    if (status != STATUS_OK) {
        printf("Error while printing C\n");
        free(array_a);
        free(array_b);
        free(array_c);
        return 1;
    }

    free(array_a);
    free(array_b);
    free(array_c);
    return 0;
}