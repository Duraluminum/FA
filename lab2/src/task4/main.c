#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "backup.h"


int main(void) {
    char *line = NULL, *tokens[4];
    unsigned long long raw_n, raw_capacity, raw_size, importance;
    size_t n, capacity, token_count;
    BackupFile *files = NULL;
    BackupResult result = {NULL, 0, 0, 0};
    BackupStatus status;
    int read_status;

    read_status = read_line(&line);
    if (read_status != 1 || (token_count = split_line(line, tokens, 2)) != 2 ||
        !parse_unsigned(tokens[0], &raw_n) || raw_n == 0 || raw_n > SIZE_MAX || raw_n > 1000000U) {
        fprintf(stderr, "Invalid first line\n");
        free(line);
        return 1;
    }

    if (!parse_unsigned(tokens[1], &raw_capacity) || raw_capacity > SIZE_MAX) {
        fprintf(stderr, "Invalid capacity\n");
        free(line);
        return 1;
    }

    n = (size_t)raw_n;
    capacity = (size_t)raw_capacity;
    free(line);
    line = NULL;

    files = calloc(n, sizeof(BackupFile));
    if (files == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        read_status = read_line(&line);
        if (read_status != 1 || split_line(line, tokens, 3) != 3) {
            fprintf(stderr, "Invalid file description\n");
            free(line);
            free_backup_files(files, n);
            return 1;
        }

        files[i].name = copy_name(tokens[0]);
        if (files[i].name == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            free(line);
            free_backup_files(files, n);
            return 1;
        }

        for (size_t j = 0; j < i; j++) {
            if (strcmp(files[i].name, files[j].name) == 0) {
                fprintf(stderr, "Duplicate filename\n");
                free(line);
                free_backup_files(files, n);
                return 1;
            }
        }

        if (!parse_unsigned(tokens[1], &raw_size) || raw_size == 0 || raw_size > SIZE_MAX) {
            fprintf(stderr, "Invalid file size\n");
            free(line);
            free_backup_files(files, n);
            return 1;
        }

        if (!parse_unsigned(tokens[2], &importance)) {
            fprintf(stderr, "Invalid file importance\n");
            free(line);
            free_backup_files(files, n);
            return 1;
        }

        files[i].size = (size_t)raw_size;
        files[i].importance = importance;
        free(line);
        line = NULL;
    }

    status = choose_files(files, n, capacity, &result);
    if (status != BACKUP_OK) {
        fprintf(stderr, "Error selecting files: %d\n", status);
        free_backup_files(files, n);
        return 1;
    }

    if (printf("TOTAL_SIZE %zu\n", result.total_size) < 0 ||
            printf("TOTAL_IMPORTANCE %llu\n", result.total_importance) < 0 ||
            printf("FILES %zu\n", result.count) < 0) {

        fprintf(stderr, "Output error\n");
        free_backup_result(&result);
        free_backup_files(files, n);
        
        return 1;
    }

    for (size_t i = 0; i < result.count; i++) {
        if (printf("%s\n", result.files[i].name) < 0) {
            fprintf(stderr, "Output error\n");
            free_backup_result(&result);
            free_backup_files(files, n);
            return 1;
        }
    }

    free_backup_result(&result);
    free_backup_files(files, n);
    return 0;
}