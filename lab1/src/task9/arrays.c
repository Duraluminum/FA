#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "arrays.h"


Status parse_int(const char *str, int *out) {
    if (str == NULL || out == NULL) return STATUS_INVALID_ARGUMENT;
    if (*str == '\0') return STATUS_INVALID_VALUE;

    int sign = 1;
    size_t i = 0;
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }
    if (str[i] == '\0') return STATUS_INVALID_VALUE;

    long long val = 0;
    long long limit = (sign == 1) ? INT_MAX : -(long long)INT_MIN;

    for (; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') return STATUS_INVALID_VALUE;
        int digit = str[i] - '0';
        if (val > (limit - digit) / 10) return STATUS_RANGE_ERROR;
        val = val * 10 + digit;
    }

    *out = (int)(val * sign);
    return STATUS_OK;
}

Status generate_random_int(int min, int max, int *result) {
    if (result == NULL) return STATUS_INVALID_ARGUMENT;
    if (min > max) return STATUS_INVALID_VALUE;

    const uint64_t random_space = 1ULL << 60;
    uint64_t range = (uint64_t)((long long)max - (long long)min) + 1ULL;
    uint64_t limit = random_space - random_space % range;
    uint64_t random_value;

    do {
        random_value = 0;
        for (int i = 0; i < 4; i++) {
            int part = rand() % 32768;
            random_value = (random_value << 15) | (uint64_t)part;
        }
    } while (random_value >= limit);

    *result = (int)((long long)min + (long long)(random_value % range));
    return STATUS_OK;
}

Status fill_array(int *array, size_t size, int min, int max) {
    if (array == NULL) return STATUS_INVALID_ARGUMENT;
    if (size == 0) return STATUS_INVALID_VALUE;
    if (min > max) return STATUS_INVALID_VALUE;

    for (size_t i = 0; i < size; i++) {
        Status status = generate_random_int(min, max, &array[i]);
        if (status != STATUS_OK) return status;
    }
    return STATUS_OK;
}

Status find_min_max(const int *array, size_t size, size_t *min_index, size_t *max_index) {
    if (array == NULL || min_index == NULL || max_index == NULL) return STATUS_INVALID_ARGUMENT;
    if (size == 0) return STATUS_INVALID_VALUE;

    *min_index = 0;
    *max_index = 0;
    for (size_t i = 1; i < size; i++) {
        if (array[i] < array[*min_index]) *min_index = i;
        if (array[i] > array[*max_index]) *max_index = i;
    }
    return STATUS_OK;
}

Status swap_elements(int *array, size_t size, size_t first_index, size_t second_index) {
    if (array == NULL) return STATUS_INVALID_ARGUMENT;
    if (size == 0) return STATUS_INVALID_VALUE;
    if (first_index >= size || second_index >= size) return STATUS_INVALID_ARGUMENT;
    if (first_index == second_index) return STATUS_OK;

    int temp = array[first_index];
    array[first_index] = array[second_index];
    array[second_index] = temp;
    return STATUS_OK;
}

Status create_random_array(int **array, size_t *size) {
    if (array == NULL || size == NULL) return STATUS_INVALID_ARGUMENT;

    int random_size;
    Status status = generate_random_int(MIN_ARRAY_SIZE, MAX_ARRAY_SIZE, &random_size);
    if (status != STATUS_OK) return status;

    if ((size_t)random_size > CUSTOM_SIZE_MAX / sizeof(int)) return STATUS_RANGE_ERROR;

    int *new_array = malloc((size_t)random_size * sizeof(int));
    if (new_array == NULL) return STATUS_MEMORY_ERROR;

    status = fill_array(new_array, (size_t)random_size, MIN_VALUE, MAX_VALUE);
    if (status != STATUS_OK) {
        free(new_array);
        return status;
    }

    *array = new_array;
    *size = (size_t)random_size;
    return STATUS_OK;
}

Status find_nearest(const int *array, size_t size, int value, int *nearest) {
    if (array == NULL || nearest == NULL) return STATUS_INVALID_ARGUMENT;
    if (size == 0) return STATUS_INVALID_VALUE;

    *nearest = array[0];
    long long best_distance = (long long)value - (long long)array[0];
    if (best_distance < 0) best_distance = -best_distance;

    for (size_t i = 1; i < size; i++) {
        long long current_distance = (long long)value - (long long)array[i];
        if (current_distance < 0) current_distance = -current_distance;

        if (current_distance < best_distance) {
            best_distance = current_distance;
            *nearest = array[i];
        }
    }
    return STATUS_OK;
}

Status create_array_c(const int *array_a, size_t size_a, const int *array_b, size_t size_b, int **array_c) {
    if (array_a == NULL || array_b == NULL || array_c == NULL) return STATUS_INVALID_ARGUMENT;
    if (size_a == 0 || size_b == 0) return STATUS_INVALID_VALUE;
    if (size_a > CUSTOM_SIZE_MAX / sizeof(int)) return STATUS_RANGE_ERROR;

    int *new_array = malloc(size_a * sizeof(int));
    if (new_array == NULL) return STATUS_MEMORY_ERROR;

    for (size_t i = 0; i < size_a; i++) {
        int nearest;
        Status status = find_nearest(array_b, size_b, array_a[i], &nearest);
        if (status != STATUS_OK) {
            free(new_array);
            return status;
        }
        new_array[i] = array_a[i] + nearest;
    }

    *array_c = new_array;
    return STATUS_OK;
}

Status print_array(const int *array, size_t size) {
    if (array == NULL) return STATUS_INVALID_ARGUMENT;
    if (size == 0) return STATUS_INVALID_VALUE;

    for (size_t i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    return STATUS_OK;
}