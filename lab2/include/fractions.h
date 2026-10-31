#ifndef FRACTIONS_H
#define FRACTIONS_H


typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_NULL,
    STATUS_ERR_BAD_BASE,
    STATUS_ERR_BAD_NUMBER,
    STATUS_ERR_RANGE
} Status;

Status process_fractions(unsigned int base, int count, ...);


#endif