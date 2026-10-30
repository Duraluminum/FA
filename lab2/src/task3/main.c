#include <stdio.h>
#include "../../include/search.h"


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <substring> <file1> [files]\n", argv[0]);
        return 1;
    }

    SearchStatus status = find_in_files_array(argv[1], (size_t)(argc - 2), (const char *const *)(argv + 2));
    if (status != SEARCH_OK) {
        fprintf(stderr, "Error searching files: %d\n", status);
        return 1;
    }

    return 0;
}