#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <stddef.h>


typedef enum {
    CONF_OK = 0,
    CONF_ARGUMENT,
    CONF_MEMORY,
    CONF_VALUE
} ConferenceStatus;

typedef struct {
    unsigned long long id;
    int start;
    int finish;
} Talk;

ConferenceStatus max_conf(const Talk *talks, size_t count, size_t rooms, size_t *result);


#endif