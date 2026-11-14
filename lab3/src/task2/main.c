#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include "conference.h"


static int parse_unsigned(const char *text, unsigned long long *value) {
    if (text == NULL || value == NULL || text[0] == '\0') return 0;

    unsigned long long number = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] < '0' || text[i] > '9') return 0;

        unsigned int digit = (unsigned int)(text[i] - '0');
        if (number > (ULLONG_MAX - digit) / 10ULL) return 0;
        number = number * 10ULL + digit;
    }

    *value = number;
    return 1;
}

static int parse_time(const char *text, int *minutes) {
    if (text == NULL || minutes == NULL || strlen(text) != 5 || text[2] != ':') return 0;
    if (text[0] < '0' || text[0] > '9' || text[1] < '0' || text[1] > '9' || text[3] < '0' || text[3] > '9' || text[4] < '0' || text[4] > '9') return 0;

    int hours = (text[0] - '0') * 10 + text[1] - '0';
    int mins = (text[3] - '0') * 10 + text[4] - '0';
    if (hours > 23 || mins > 59) return 0;

    *minutes = hours * 60 + mins;
    return 1;
}

static int compare_ids(const void *first, const void *second) {
    const unsigned long long *a = (const unsigned long long *)first;
    const unsigned long long *b = (const unsigned long long *)second;

    return *a < *b ? -1 : *a > *b ? 1 : 0;
}

int main(void) {
    char n_text[64], k_text[64];
    unsigned long long n_value, k_value;
    if (scanf("%63s %63s", n_text, k_text) != 2 || !parse_unsigned(n_text, &n_value) || !parse_unsigned(k_text, &k_value) ||
        n_value == 0 || k_value == 0 || n_value > 1000000 || n_value > SIZE_MAX || k_value > SIZE_MAX) {
        fprintf(stderr, "Invalid conferences or rooms count\n");
        return 1;
    }

    size_t n = (size_t)n_value;
    size_t k = (size_t)k_value;
    Talk *talks = (Talk *)malloc(n * sizeof(Talk));
    unsigned long long *ids = (unsigned long long *)malloc(n * sizeof(unsigned long long));
    if (talks == NULL || ids == NULL) {
        free(talks);
        free(ids);
        return 1;
    }

    int valid = 1;
    for (size_t i = 0; i < n; i++) {
        char id_text[64], start[16], finish[16];
        if (scanf("%63s %15s %15s", id_text, start, finish) != 3 || !parse_unsigned(id_text, &talks[i].id) || talks[i].id == 0 ||
            !parse_time(start, &talks[i].start) || !parse_time(finish, &talks[i].finish) || talks[i].start >= talks[i].finish) {
            valid = 0;
            break;
        }

        ids[i] = talks[i].id;
    }

    if (valid) {
        qsort(ids, n, sizeof(unsigned long long), compare_ids);
        for (size_t i = 1; i < n; i++) {
            if (ids[i - 1] == ids[i]) valid = 0;
        }
    }

    size_t answer = 0;
    ConferenceStatus status = valid ? max_conf(talks, n, k, &answer) : CONF_VALUE;
    free(ids);
    free(talks);
    
    if (status == CONF_ARGUMENT) {
        fprintf(stderr, "Invalid argument\n");
        return 1;
    }
    if (status == CONF_MEMORY) {
        fprintf(stderr, "Memory error\n");
        return 1;
    }
    if (status == CONF_VALUE) {
        fprintf(stderr, "Invalid value\n");
        return 1;
    }

    printf("%zu\n", answer);
    return 0;
}