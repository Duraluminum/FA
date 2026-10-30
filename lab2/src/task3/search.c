#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../include/search.h"


static size_t my_strlen(const char *str) {
    size_t length = 0;
    while (str[length] != '\0')
        length++;

    return length;
}

static SearchStatus read_file(FILE *file, char **data, size_t *length) {
    size_t used = 0, capacity = 1024;
    char *buffer = malloc(capacity);
    if (buffer == NULL) return SEARCH_MEMORY;

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (used + 1 >= capacity) {
            size_t next;
            char *new_buffer;
            if (capacity > SIZE_MAX / 2) {
                free(buffer);
                return SEARCH_MEMORY;
            }

            next = capacity * 2;
            
            new_buffer = realloc(buffer, next);
            if (new_buffer == NULL) {
                free(buffer);
                return SEARCH_MEMORY;
            }

            buffer = new_buffer;
            capacity = next;
        }

        buffer[used++] = (char)ch;
    }

    if (ferror(file)) {
        free(buffer);
        return SEARCH_INPUT;
    }

    buffer[used] = '\0';
    *data = buffer;
    *length = used;
    return SEARCH_OK;
}

static int matches_at(const char *data, size_t data_length, size_t position, const char *needle, size_t needle_length) {
    if (position > data_length || needle_length > data_length - position) return 0;

    for (size_t i = 0; i < needle_length; i++)
        if (data[position + i] != needle[i]) return 0;

    return 1;
}

SearchStatus find_in_files_array(const char *substring, size_t count, const char *const *paths) {
    if (substring == NULL || paths == NULL || count == 0) return SEARCH_ARGUMENT;

    size_t needle_length, file_index;
    needle_length = my_strlen(substring);
    if (needle_length == 0) return SEARCH_EMPTY;

    SearchStatus result = SEARCH_OK;
    for (file_index = 0; file_index < count; file_index++) {
        FILE *file;
        char *data = NULL;
        size_t length = 0, position, line = 1, column = 1;
        SearchStatus status;
        if (paths[file_index] == NULL || paths[file_index][0] == '\0') return SEARCH_ARGUMENT;

        file = fopen(paths[file_index], "r");
        if (file == NULL) {
            fprintf(stderr, "Error opening file: %s\n", paths[file_index]);
            result = SEARCH_OPEN;
            continue;
        }

        status = read_file(file, &data, &length);
        if (fclose(file) != 0 && status == SEARCH_OK) status = SEARCH_INPUT;
        if (status != SEARCH_OK) {
            fprintf(stderr, "Error reading file: %s\n", paths[file_index]);
            free(data);
            result = status;
            continue;
        }

        printf("File: %s\n", paths[file_index]);
        for (position = 0; position < length; position++) {
            if (matches_at(data, length, position, substring, needle_length)) {
                if (printf("Line %zu, character %zu\n", line, column) < 0) result = SEARCH_OUTPUT;
            }

            if (data[position] == '\n') {
                line++;
                column = 1;

            } else {
                column++;
            }
        }

        free(data);
    }

    return result;
}