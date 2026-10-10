#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stddef.h>

#define CUSTOM_SIZE_MAX ((size_t)(~(size_t)0))

#define ARRAY_SIZE 20
#define MIN_ARRAY_SIZE 10
#define MAX_ARRAY_SIZE 10000
#define MIN_VALUE -1000
#define MAX_VALUE 1000


typedef enum {
    STATUS_OK,
    STATUS_INVALID_ARGUMENT,
    STATUS_INVALID_VALUE,
    STATUS_RANGE_ERROR,
    STATUS_MEMORY_ERROR
} Status;

Status parse_int(const char *string, int *value);
Status generate_random_int(int min, int max, int *result);
Status fill_array(int *array, size_t size, int min, int max);
Status find_min_max(const int *array, size_t size, size_t *min_index, size_t *max_index);
Status swap_elements(int *array, size_t size, size_t first_index, size_t second_index);
Status create_random_array(int **array, size_t *size);
Status find_nearest(const int *array, size_t size, int value, int *nearest);
Status create_array_c(const int *array_a, size_t size_a, const int *array_b, size_t size_b, int **array_c);
Status print_array(const int *array, size_t size);


#endif