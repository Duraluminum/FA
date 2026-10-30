#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <limits.h>
#include "backup.h"


int main(void) {
    char token[256];
    unsigned long long raw_n, raw_capacity;
    size_t n, capacity;
    BackupFile *files = NULL;
    BackupResult result = {NULL, 0, 0, 0};
    BackupStatus status;
    
    int read_status;
    read_status = read_token(token, sizeof(token));
    if (read_status != 1 || !parse_unsigned(token, &raw_n) || raw_n == 0 || raw_n > SIZE_MAX) {
        fprintf(stderr, "Invalid number of files\n");
        return 1;
    }

    read_status = read_token(token, sizeof(token));
    if (read_status != 1 || !parse_unsigned(token, &raw_capacity) || raw_capacity > SIZE_MAX) {
        fprintf(stderr, "Invalid capacity\n");
        return 1;
    }

    n = (size_t)raw_n;
    capacity = (size_t)raw_capacity;
    if (n > SIZE_MAX / sizeof(BackupFile) || n > 1000000U) {
        fprintf(stderr, "Too many files\n");
        return 1;
    }

    files = calloc(n, sizeof(BackupFile));
    if (files == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        unsigned long long raw_size, importance;

        read_status = read_dynamic_token(&files[i].name);
        if (read_status != 1 || files[i].name[0] == '\0') {
            fprintf(stderr, "Invalid filename\n");
            free_backup_files(files, n);
            return 1;
        }

        read_status = read_token(token, sizeof(token));
        if (read_status != 1 || !parse_unsigned(token, &raw_size) || raw_size == 0 || raw_size > SIZE_MAX) {
            fprintf(stderr, "Invalid file size\n");
            free_backup_files(files, n);
            return 1;
        }

        read_status = read_token(token, sizeof(token));
        if (read_status != 1 || !parse_unsigned(token, &importance)) {
            fprintf(stderr, "Invalid file importance\n");
            free_backup_files(files, n);
            return 1;
        }
        
        files[i].size = (size_t)raw_size;
        files[i].importance = importance;
    }

    read_status = read_token(token, sizeof(token));
    if (read_status != 0) {
        fprintf(stderr, "Unexpected extra input\n");
        free_backup_files(files, n);
        return 1;
    }

    status = choose_files(files, n, capacity, &result);
    if (status != BACKUP_OK) {
        fprintf(stderr, "Error selecting files: %d\n", status);
        free_backup_files(files, n);
        return 1;
    }

    printf("TOTAL_SIZE %zu\n", result.total_size);
    printf("TOTAL_IMPORTANCE %llu\n", result.total_importance);
    printf("FILES %zu\n", result.count);

    for (size_t i = 0; i < result.count; i++)
        printf("%s\n", result.files[i].name);
    
    free_backup_result(&result);
    free_backup_files(files, n);

    return 0;
}