#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "brackets.h"


static int read_line(FILE *file, char **line) {
    if (file == NULL || line == NULL) return -1;
    size_t used = 0, capacity = 128;
    char *text = (char *)malloc(capacity);
    if (text == NULL) return -1;

    int symbol;
    while ((symbol = fgetc(file)) != EOF && symbol != '\n') {
        if (used + 1 >= capacity) {
            if (capacity > SIZE_MAX / 2) {
                free(text);
                return -1;
            }

            size_t next = capacity * 2;
            char *new_text = (char *)realloc(text, next);
            if (new_text == NULL) {
                free(text);
                return -1;
            }

            text = new_text;
            capacity = next;
        }

        text[used++] = (char)symbol;
    }

    if (symbol == EOF && used == 0) {
        free(text);
        return 0;
    }

    text[used] = '\0';
    *line = text;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Cannot open input file\n");
        return 1;
    }
    
    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        fclose(input);
        fprintf(stderr, "Cannot open output file\n");
        return 1;
    }

    int result = 0, line_status;
    char *line = NULL;
    while ((line_status = read_line(input, &line)) == 1) {
        TreeNode *root = NULL;
        TreeStatus status = parse_tree(line, &root);
        free(line);
        line = NULL;

        if (status != TREE_OK) {
            fprintf(stderr, "Invalid expression\n");
            result = 1;
            break;
        }

        print_tree(output, root, 0);
        fputc('\n', output);
        free_tree(root);
    }

    free(line);
    if (line_status < 0) result = 1;
    if (fclose(input) != 0) result = 1;
    if (fclose(output) != 0) result = 1;

    return result;
}