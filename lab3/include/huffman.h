#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>


typedef enum {
    HUFFMAN_OK = 0,
    HUFFMAN_ARGUMENT,
    HUFFMAN_OPEN,
    HUFFMAN_INPUT,
    HUFFMAN_MEMORY,
    HUFFMAN_OVERFLOW
} HuffmanStatus;

typedef struct HuffmanNode {
    uint64_t frequency;
    unsigned char value;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

typedef struct {
    uint64_t frequencies[256];
    char codes[256][257];
    uint64_t original_bytes;
    uint64_t encoded_bits;
    uint64_t encoded_bytes;
    double saving_percent;
} HuffmanResult;

HuffmanStatus analyze_file(const char *path, HuffmanResult *result);
void print_huffman_result(const HuffmanResult *result);


#endif