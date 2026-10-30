#ifndef SEARCH_H
#define SEARCH_H

#include <stddef.h>


typedef enum {
    SEARCH_OK = 0,
    SEARCH_ARGUMENT,
    SEARCH_EMPTY,
    SEARCH_OPEN,
    SEARCH_INPUT,
    SEARCH_MEMORY,
    SEARCH_OUTPUT
} SearchStatus;

SearchStatus find_in_files_array(const char *substring, size_t count, const char *const *paths);


#endif