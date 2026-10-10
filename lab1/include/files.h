#ifndef FILES_H
#define FILES_H

#include <stdio.h>


typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_NULL,
    STATUS_ERR_OPEN_IN,
    STATUS_ERR_OPEN_OUT,
    STATUS_ERR_MEM,
    STATUS_ERR_IO,
    STATUS_ERR_UNKNOWN_FLAG
} Status;

Status parse_flag(const char *arg, char *out, int *has_n);

char *make_file_name(const char *in_name);
char *my_strdup(const char *s);

Status proc_d(FILE *in, FILE *out);
Status proc_i(FILE *in, FILE *out);
Status proc_s(FILE *in, FILE *out);
Status proc_a(FILE *in, FILE *out);

void print_err(Status s);


#endif