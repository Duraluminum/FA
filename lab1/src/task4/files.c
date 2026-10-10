#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "files.h"


Status parse_flag(const char *arg, char *out, int *has_n) {
    if (!arg || !out || !has_n) return STATUS_ERR_NULL;
    if (arg[0] != '-' && arg[0] != '/') return STATUS_ERR_UNKNOWN_FLAG;
    if (!arg[1]) return STATUS_ERR_UNKNOWN_FLAG;

    if (arg[1] == 'n') {
        *has_n = 1;
        if (!arg[2] || arg[3]) return STATUS_ERR_UNKNOWN_FLAG;
        *out = arg[2];
    } else {
        *has_n = 0;
        if (arg[2]) return STATUS_ERR_UNKNOWN_FLAG;
        *out = arg[1];
    }

    if (*out != 'd' && *out != 'i' && *out != 's' && *out != 'a') {
        return STATUS_ERR_UNKNOWN_FLAG;
    }

    return STATUS_OK;
}

char *make_file_name(const char *in_name) {
    if (!in_name) return NULL;

    const char *pfx = "out_";
    size_t len = strlen(pfx) + strlen(in_name) + 1;
    char *buf = (char *)malloc(len);
    if (!buf) return NULL;

    snprintf(buf, len, "%s%s", pfx, in_name);
    return buf;
}

char *my_strdup(const char *s) {
    if (!s) return NULL;

    size_t len = strlen(s) + 1;
    char *d = (char *)malloc(len);
    if (!d) return NULL;

    memcpy(d, s, len);
    return d;
}

Status proc_d(FILE *in, FILE *out) {
    if (!in || !out) return STATUS_ERR_NULL;

    int c;
    while ((c = fgetc(in)) != EOF) {
        if (!isdigit(c)) {
            if (fputc(c, out) == EOF) return STATUS_ERR_IO;
        }
    }

    return ferror(in) ? STATUS_ERR_IO : STATUS_OK;
}

Status proc_i(FILE *in, FILE *out) {
    if (!in || !out) return STATUS_ERR_NULL;

    char buf[4096];

    while (fgets(buf, sizeof(buf), in)) {
        int cnt = 0;
        size_t len = strlen(buf);
        
        for (size_t i = 0; i < len; ++i) {
            char c = buf[i];

            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                cnt++;
            }
        }

        if (buf[len - 1] == '\n' || feof(in)) {
            if (fprintf(out, "%d\n", cnt) < 0) return STATUS_ERR_IO;
        }
    }

    return ferror(in) ? STATUS_ERR_IO : STATUS_OK;
}

Status proc_s(FILE *in, FILE *out) {
    if (!in || !out) return STATUS_ERR_NULL;

    char buf[4096];

    while (fgets(buf, sizeof(buf), in)) {
        int cnt = 0;
        size_t len = strlen(buf);

        for (size_t i = 0; i < len; ++i) {
            char c = buf[i];

            if (c == '\n' || c == '\r') continue;

            bool is_lat = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
            bool is_dig = (c >= '0' && c <= '9');

            if (!is_lat && !is_dig && c != ' ') {
                cnt++;
            }
        }

        if (len > 0 && (buf[len - 1] == '\n' || feof(in))) {
            if (fprintf(out, "%d\n", cnt) < 0) {
                return STATUS_ERR_IO;
            }
        }
    }

    return ferror(in) ? STATUS_ERR_IO : STATUS_OK;
}

Status proc_a(FILE *in, FILE *out) {
    if (!in || !out) return STATUS_ERR_NULL;

    int c;
    while ((c = fgetc(in)) != EOF) {
        if (c >= '0' && c <= '9') {
            if (fputc(c, out) == EOF) return STATUS_ERR_IO;
        } else {
            if (fprintf(out, "%X", c) < 0) return STATUS_ERR_IO;
        }
    }

    return ferror(in) ? STATUS_ERR_IO : STATUS_OK;
}

void print_err(Status s) {
    const char *messages[] = {
        "",
        "Invalid args",
        "Cant open input",
        "Cant open output",
        "Memory error",
        "I/O error",
        "unknown flag"
    };

    if (s >= 0 && s <= STATUS_ERR_UNKNOWN_FLAG) {
        fprintf(stderr, "Error: %s\n", messages[s]);
    } else {
        fprintf(stderr, "Error: Unknown error\n");
    }
}