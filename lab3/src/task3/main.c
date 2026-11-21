#include <stdio.h>
#include "huffman.h"


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    HuffmanResult result;
    HuffmanStatus status = analyze_file(argv[1], &result);
    if (status != HUFFMAN_OK) {
        fprintf(stderr, "Huffman error: %d\n", (int)status);
        return 1;
    }

    print_huffman_result(&result);

    return 0;
}