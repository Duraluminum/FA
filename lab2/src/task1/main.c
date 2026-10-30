#include <stdio.h>
#include <stdlib.h>
#include "flags3.h"


int main(int argc, char *argv[]) {
    char flag = '\0';
    Status status;
    char *result = NULL;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <flag> <string> [seed strings]\n", argv[0]);
        return 1;
    }

    status = parse_flag(argv[1], &flag);
    if (status != STATUS_OK) {
        fprintf(stderr, "Error parsing flag: %d\n", status);
        return 1;
    }

    if (flag != 'c' && argc != 3) {
        fprintf(stderr, "Error: this flag requires exactly 2 arguments\n");
        return 1;
    }

    if (flag == 'l') {
        size_t length;
        status = string_length(argv[2], &length);

        if (status == STATUS_OK) printf("Length: %zu\n", length);

    } else if (flag == 'r') {
        status = reverse_string(argv[2], &result);

    } else if (flag == 'u') {
        status = odd_upper_string(argv[2], &result);

    } else if (flag == 'n') {
        status = rearrange_string(argv[2], &result);

    } else {
        unsigned int seed;
        const char **strings;
        size_t count = (size_t)argc - 4;

        if (argc < 4) {
            fprintf(stderr, "Error: -c requires a seed\n");
            return 1;
        }

        status = parse_seed(argv[3], &seed);
        if (status != STATUS_OK) {
            fprintf(stderr, "Error parsing seed: %d\n", status);
            return 1;
        }

        if (count == (size_t)-1 || count + 1 > (size_t)-1 / sizeof(char *)) return 1;

        strings = malloc((count + 1) * sizeof(char *));
        if (strings == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            return 1;
        }

        strings[0] = argv[2];
        for (size_t i = 0; i < count; i++)
            strings[i + 1] = argv[i + 4];

        status = concatenate_strings(seed, count + 1, strings, &result);
        
        free(strings);
    }

    if (status != STATUS_OK) {
        fprintf(stderr, "Error processing string: %d\n", status);
        free(result);
        return 1;
    }

    if (flag != 'l')
        printf("Result: %s\n", result);

    free(result);
    return 0;
}