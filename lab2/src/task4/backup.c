#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <limits.h>
#include "backup.h"

#define MAX_DP_CELLS 10000000U


int parse_unsigned(const char *text, unsigned long long *value) {
    if (text == NULL || text[0] == '\0' || value == NULL) return 0;

    unsigned long long number = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] < '0' || text[i] > '9') return 0;

        unsigned int digit = (unsigned int)(text[i] - '0');
        if (number > (ULLONG_MAX - digit) / 10ULL) return 0;
        number = number * 10ULL + digit;
    }

    *value = number;
    return 1;
}

int read_line(char **line) {
    if (line == NULL) return -2;

    size_t used = 0, capacity = 128;
    char *text = malloc(capacity);
    if (text == NULL) return -1;

    int ch;
    while ((ch = getchar()) != EOF && ch != '\n') {
        if (ch == '\0') {
            free(text);
            return -2;
        }

        if (used + 1 >= capacity) {
            if (capacity > SIZE_MAX / 2) {
                free(text);
                return -1;
            }

            size_t next = capacity * 2;
            char *new_text = realloc(text, next);
            if (new_text == NULL) {
                free(text);
                return -1;
            }

            text = new_text;
            capacity = next;
        }

        text[used++] = (char)ch;
    }

    if (ch == EOF && ferror(stdin)) {
        free(text);
        return -2;
    }

    if (ch == EOF && used == 0) {
        free(text);
        return 0;
    }

    text[used] = '\0';
    *line = text;
    return 1;
}

size_t split_line(char *line, char **tokens, size_t limit) {
    size_t count = 0;
    char *p = line;

    while (*p != '\0') {
        while (*p != '\0' && isspace((unsigned char)*p)) p++;
        if (*p == '\0') break;
        if (count == limit) return limit + 1;

        tokens[count++] = p;
        while (*p != '\0' && !isspace((unsigned char)*p)) p++;

        if (*p != '\0') *p++ = '\0';
    }

    return count;
}

char *copy_name(const char *name) {
    size_t length = strlen(name);
    char *copy = malloc(length + 1);
    if (copy != NULL) memcpy(copy, name, length + 1);

    return copy;
}

BackupStatus choose_files(const BackupFile *files, size_t count, size_t capacity, BackupResult *result) {
    if (count == SIZE_MAX || capacity == SIZE_MAX || count > SIZE_MAX / sizeof(BackupFile)) return BACKUP_MEMORY;
    if (files == NULL || count == 0) return BACKUP_ARGUMENT;
    if (result == NULL) return BACKUP_ARGUMENT;

    result->files = NULL;
    result->count = 0;
    result->total_size = 0;
    result->total_importance = 0;

    size_t rows = count + 1, columns = capacity + 1;
    if (rows > SIZE_MAX / columns) return BACKUP_MEMORY;

    size_t cells = rows * columns;
    if (cells > SIZE_MAX / sizeof(unsigned long long) || cells > MAX_DP_CELLS) return BACKUP_MEMORY;

    for (size_t i = 0; i < count; i++) {
        if (files[i].name == NULL || files[i].name[0] == '\0' || files[i].size == 0)
            return BACKUP_VALUE;
        
        for (size_t j = 0; j < i; j++)
            if (strcmp(files[i].name, files[j].name) == 0) return BACKUP_VALUE;
    }

    unsigned long long *dp = calloc(cells, sizeof(unsigned long long));
    unsigned char *selected = calloc(count, sizeof(unsigned char));
    if (dp == NULL || selected == NULL) {
        free(dp);
        free(selected);
        return BACKUP_MEMORY;
    }

    for (size_t i = 1; i <= count; i++) {
        size_t size = files[i - 1].size;
        unsigned long long importance = files[i - 1].importance;

        for (size_t j = 0; j <= capacity; j++) {
            unsigned long long without = dp[(i - 1) * columns + j];
            unsigned long long with = 0;

            if (size <= j) {
                unsigned long long previous = dp[(i - 1) * columns + j - size];
                if (importance > ULLONG_MAX - previous) {
                    free(dp);
                    free(selected);
                    return BACKUP_OVERFLOW;
                }

                with = previous + importance;
            }

            dp[i * columns + j] = size <= j && with > without ? with : without;
        }
    }

    size_t chosen_count = 0, total_size = 0, j = capacity;
    unsigned long long total_importance = 0;

    for (size_t i = count; i > 0; i--) {
        if (dp[i * columns + j] != dp[(i - 1) * columns + j]) {
            selected[i - 1] = 1;
            chosen_count++;

            total_size += files[i - 1].size;
            total_importance += files[i - 1].importance;
            j -= files[i - 1].size;
        }
    }

    BackupFile *chosen = NULL;
    if (chosen_count > 0) {
        chosen = calloc(chosen_count, sizeof(BackupFile));
        if (chosen == NULL) {
            free(dp);
            free(selected);
            return BACKUP_MEMORY;
        }

        size_t index = 0;
        for (size_t i = 0; i < count; i++) {
            if (selected[i]) {
                chosen[index] = files[i];
                chosen[index].name = copy_name(files[i].name);
                if (chosen[index].name == NULL) {
                    free_backup_files(chosen, index);
                    free(dp);
                    free(selected);
                    return BACKUP_MEMORY;
                }

                index++;
            }
        }
    }

    free(dp);
    free(selected);
    result->files = chosen;
    result->count = chosen_count;
    result->total_size = total_size;
    result->total_importance = total_importance;
    
    return BACKUP_OK;
}

void free_backup_files(BackupFile *files, size_t count) {
    if (files == NULL) return;

    for (size_t i = 0; i < count; i++)
        free(files[i].name);

    free(files);
}

void free_backup_result(BackupResult *result) {
    if (result == NULL) return;

    free_backup_files(result->files, result->count);
    result->files = NULL;
    result->count = 0;
    result->total_size = 0;
    result->total_importance = 0;
}