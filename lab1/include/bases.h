#ifndef BASES_H
#define BASES_H

#include <stddef.h>
#include <stdint.h>


typedef enum {
    STATUS_OK,
    STATUS_ARGUMENT,
    STATUS_MEMORY,
    STATUS_INPUT,
    STATUS_NUMBER,
    STATUS_OUTPUT
} Status;

typedef struct {
    int sign;
    size_t size;
    size_t capacity;
    uint32_t *digits;
} BigInt;

Status read_line(char **line);
Status parse_base(const char *line, int *base);
void big_init(BigInt *number);
void big_free(BigInt *number);
Status big_copy(BigInt *dest, const BigInt *src);
Status big_from_string(const char *line, int base, BigInt *number);
int compare_abs(const BigInt *a, const BigInt *b);
Status big_add(BigInt *a, const BigInt *b);
Status big_to_base(const BigInt *number, int base, char **result);


#endif