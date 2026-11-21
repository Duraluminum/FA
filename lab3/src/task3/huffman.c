#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "huffman.h"


static void heap_push(HuffmanNode **heap, size_t *size, HuffmanNode *node) {
    size_t position = (*size)++;
    while (position > 0) {
        size_t parent = (position - 1) / 2;
        if (heap[parent]->frequency <= node->frequency) break;
        heap[position] = heap[parent];
        position = parent;
    }

    heap[position] = node;
}

static HuffmanNode *heap_pop(HuffmanNode **heap, size_t *size) {
    HuffmanNode *result = heap[0];
    HuffmanNode *last = heap[--(*size)];
    size_t position = 0;

    while (position * 2 + 1 < *size) {
        size_t child = position * 2 + 1;
        if (child + 1 < *size && heap[child + 1]->frequency < heap[child]->frequency) child++;
        if (last->frequency <= heap[child]->frequency) break;
        heap[position] = heap[child];
        position = child;
    }

    if (*size != 0) heap[position] = last;
    return result;
}

static void make_codes(const HuffmanNode *node, char *path, size_t depth, char codes[256][257]) {
    if (node == NULL) return;
    if (node->left == NULL && node->right == NULL) {
        if (depth == 0) {
            codes[node->value][0] = '0';
            codes[node->value][1] = '\0';
        } else {
            path[depth] = '\0';
            for (size_t i = 0; i <= depth; i++) codes[node->value][i] = path[i];
        }

        return;
    }

    if (node->left != NULL) {
        path[depth] = '0';
        make_codes(node->left, path, depth + 1, codes);
    }

    if (node->right != NULL) {
        path[depth] = '1';
        make_codes(node->right, path, depth + 1, codes);
    }
}


HuffmanStatus analyze_file(const char *path, HuffmanResult *result) {
    if (path == NULL || path[0] == '\0' || result == NULL) return HUFFMAN_ARGUMENT;
    memset(result, 0, sizeof(HuffmanResult));

    FILE *file = fopen(path, "rb");
    if (file == NULL) return HUFFMAN_OPEN;

    uint64_t *frequencies = result->frequencies;
    uint64_t file_size = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (frequencies[(unsigned char)ch] == UINT64_MAX || file_size == UINT64_MAX) {
            fclose(file);
            return HUFFMAN_OVERFLOW;
        }

        frequencies[(unsigned char)ch]++;
        file_size++;
    }

    if (!feof(file)) {
        fclose(file);
        return HUFFMAN_INPUT;
    }

    if (fclose(file) != 0) return HUFFMAN_INPUT;

    char (*codes)[257] = result->codes;
    HuffmanNode *nodes = calloc(511, sizeof(HuffmanNode));
    HuffmanNode *heap[511];
    if (nodes == NULL) return HUFFMAN_MEMORY;

    size_t heap_size = 0, node_count = 0;
    for (size_t i = 0; i < 256; i++) {
        if (frequencies[i] == 0) continue;
        nodes[node_count].value = (unsigned char)i;
        nodes[node_count].frequency = frequencies[i];
        heap_push(heap, &heap_size, &nodes[node_count]);
        node_count++;
    }

    while (heap_size > 1) {
        HuffmanNode *left = heap_pop(heap, &heap_size);
        HuffmanNode *right = heap_pop(heap, &heap_size);
        if (UINT64_MAX - left->frequency < right->frequency) {
            free(nodes);
            return HUFFMAN_OVERFLOW;
        }

        nodes[node_count].frequency = left->frequency + right->frequency;
        nodes[node_count].left = left;
        nodes[node_count].right = right;
        heap_push(heap, &heap_size, &nodes[node_count]);
        node_count++;
    }

    if (heap_size == 1) {
        char path_buffer[257];
        make_codes(heap[0], path_buffer, 0, codes);
    }

    uint64_t encoded_bits = 0;
    for (size_t i = 0; i < 256; i++) {
        if (frequencies[i] == 0) continue;
        size_t code_length = 0;
        while (codes[i][code_length] != '\0') code_length++;
        if (code_length != 0 && frequencies[i] > (UINT64_MAX - encoded_bits) / code_length) {
            free(nodes);
            return HUFFMAN_OVERFLOW;
        }
        
        encoded_bits += frequencies[i] * code_length;
    }

    result->original_bytes = file_size;
    result->encoded_bits = encoded_bits;
    result->encoded_bytes = encoded_bits / 8 + (encoded_bits % 8 != 0);
    result->saving_percent = file_size == 0 ? 0.0 : 100.0 * (1.0 - (double)result->encoded_bytes / (double)file_size);

    free(nodes);
    return HUFFMAN_OK;
}

void print_huffman_result(const HuffmanResult *result) {
    if (result == NULL) return;
    printf("Byte\tFrequency\tCode\n");
    for (size_t i = 0; i < 256; i++)
        if (result->frequencies[i] != 0)
            printf("%zu\t%llu\t\t%s\n", i, (unsigned long long)result->frequencies[i], result->codes[i]);

    printf("Original size: %llu  bytes\n", (unsigned long long)result->original_bytes);
    printf("Encoded size: %llu  bits\n", (unsigned long long)result->encoded_bits);
    printf("Encoded size: %llu  bytes\n", (unsigned long long)result->encoded_bytes);
    printf("Saving: %.2f%%\n", result->saving_percent);
}