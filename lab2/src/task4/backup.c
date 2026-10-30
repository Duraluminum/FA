#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include "backup.h"

#define BACKUP_MAX_DP_CELLS 10000000U


int parse_unsigned(const char *text, unsigned long long *value) {
    if (text == NULL || text[0] == '\0' || value == NULL) return 0;

    unsigned long long number = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] < '0' || text[i] > '9') return 0;

        unsigned int digit;
        digit = (unsigned int)(text[i] - '0');
        if (number > (ULLONG_MAX - digit) / 10ULL) return 0;
        number = number * 10ULL + digit;
    }

    *value = number;
    return 1;
}

int read_token(char *token, size_t size) {
    int ch;
    size_t length = 0;
    
    do {
        ch = getchar();
    } while (ch != EOF && isspace((unsigned char)ch));

    if (ch == EOF) return 0;

    while (ch != EOF && !isspace((unsigned char)ch)) {
        if (length + 1 >= size) {
            while ((ch = getchar()) != EOF && !isspace((unsigned char)ch)) {}
            return -1;
        }
        
        token[length++] = (char)ch;
        ch = getchar();
    }

    token[length] = '\0';
    return 1;
}

int read_dynamic_token(char **token) {
    int ch;
    size_t length = 0, capacity = 32;

    char *text = malloc(capacity);
    if (text == NULL) return -1;
    
    do {
        ch = getchar();
    } while (ch != EOF && isspace((unsigned char)ch));

    if (ch == EOF) {
        free(text);
        return 0;
    }

    while (ch != EOF && !isspace((unsigned char)ch)) {
        if (length + 1 >= capacity) {
            if (capacity > SIZE_MAX / 2) {
                free(text);
                return -1;
            }
            
            size_t next;
            char *new_text;
            next = capacity * 2;
            new_text = realloc(text, next);
            if (new_text == NULL) {
                free(text);
                return -1;
            }

            text = new_text;
            capacity = next;
        }

        text[length++] = (char)ch;
        ch = getchar();
    }

    text[length] = '\0';
    *token = text;

    return 1;
}

static char *copy_name(const char *name) {
    size_t length = strlen(name);
    if (length == SIZE_MAX) return NULL;

    char *copy;
    copy = malloc(length + 1);
    if (copy != NULL) memcpy(copy, name, length + 1);

    return copy;
}

static int compare_name_pointers(const void *first, const void *second) {
    const char *const *name_a = first;
    const char *const *name_b = second;

    return strcmp(*name_a, *name_b);
}

BackupStatus choose_files(const BackupFile *files, size_t count, size_t capacity, BackupResult *result) {
    if (files == NULL || result == NULL || count == 0) return BACKUP_ARGUMENT;
    if (capacity == SIZE_MAX || count == SIZE_MAX) return BACKUP_MEMORY;

    size_t rows, columns, cells, i, j;
    BackupFile *chosen = NULL;
    const char **names = NULL;
    size_t chosen_count = 0, total_size = 0;
    unsigned long long total_importance = 0;
    unsigned long long *dp;
    unsigned char *selected;

    rows = count + 1;
    columns = capacity + 1;
    if (rows > SIZE_MAX / columns) return BACKUP_MEMORY;
    
    cells = rows * columns;
    if (cells > SIZE_MAX / sizeof(unsigned long long) || cells > BACKUP_MAX_DP_CELLS) return BACKUP_MEMORY;

    dp = calloc(cells, sizeof(unsigned long long));
    selected = calloc(count, sizeof(unsigned char));
    if (dp == NULL || selected == NULL) {
        free(dp);
        free(selected);
        return BACKUP_MEMORY;
    }

    if (count > SIZE_MAX / sizeof(char *)) {
        free(dp);
        free(selected);
        return BACKUP_MEMORY;
    }

    names = malloc(count * sizeof(char *));
    if (names == NULL) {
        free(dp);
        free(selected);
        return BACKUP_MEMORY;
    }

    for (i = 0; i < count; i++) {
        if (files[i].name == NULL || files[i].name[0] == '\0' || files[i].size == 0) {
            free(names);
            free(dp);
            free(selected);
            return BACKUP_VALUE;
        }

        names[i] = files[i].name;
    }

    qsort(names, count, sizeof(char *), compare_name_pointers);
    for (i = 1; i < count; i++) {
        if (strcmp(names[i - 1], names[i]) == 0) {
            free(names);
            free(dp);
            free(selected);
            return BACKUP_VALUE;
        }
    }

    free(names);
    for (i = 1; i <= count; i++) {
        size_t size = files[i - 1].size;
        unsigned long long importance = files[i - 1].importance;

        for (j = 0; j <= capacity; j++) {
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

    j = capacity;
    for (i = count; i > 0; i--) {
        if (dp[i * columns + j] != dp[(i - 1) * columns + j]) {
            selected[i - 1] = 1;
            j -= files[i - 1].size;
            chosen_count++;
            total_size += files[i - 1].size;

            if (files[i - 1].importance > ULLONG_MAX - total_importance) {
                free(dp);
                free(selected);
                return BACKUP_OVERFLOW;
            }

            total_importance += files[i - 1].importance;
        }
    }

    if (chosen_count > 0) {
        chosen = malloc(chosen_count * sizeof(BackupFile));
        if (chosen == NULL) {
            free(dp);
            free(selected);
            return BACKUP_MEMORY;
        }

        chosen_count = 0;
        for (i = 0; i < count; i++) {
            if (selected[i]) {
                chosen[chosen_count] = files[i];
                chosen[chosen_count].name = copy_name(files[i].name);
                if (chosen[chosen_count].name == NULL) {
                    free_backup_files(chosen, chosen_count);
                    free(dp);
                    free(selected);
                    return BACKUP_MEMORY;
                }
                
                chosen_count++;
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