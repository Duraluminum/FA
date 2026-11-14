#include <stdlib.h>
#include <stdint.h>
#include "conference.h"


static int compare_talks(const void *first, const void *second) {
    const Talk *a = (const Talk *)first;
    const Talk *b = (const Talk *)second;
    if (a->finish < b->finish) return -1;
    if (a->finish > b->finish) return 1;
    if (a->start < b->start) return -1;
    if (a->start > b->start) return 1;

    return 0;
}

ConferenceStatus max_conf(const Talk *talks, size_t count, size_t rooms, size_t *result) {
    if ((talks == NULL && count != 0) || result == NULL) return CONF_ARGUMENT;
    if (rooms == 0 && count != 0) return CONF_VALUE;

    *result = 0;
    if (count == 0) return CONF_OK;
    if (rooms > count) rooms = count;
    if (count > SIZE_MAX / sizeof(Talk) || rooms > SIZE_MAX / sizeof(int)) return CONF_MEMORY;

    Talk *sorted = (Talk *)malloc(count * sizeof(Talk));
    int *finish_times = (int *)malloc(rooms * sizeof(int));
    if (sorted == NULL || finish_times == NULL) {
        free(sorted);
        free(finish_times);
        return CONF_MEMORY;
    }

    for (size_t i = 0; i < count; i++) sorted[i] = talks[i];
    qsort(sorted, count, sizeof(Talk), compare_talks);
    
    for (size_t i = 0; i < rooms; i++) finish_times[i] = -1;

    for (size_t i = 0; i < count; i++) {
        size_t selected = rooms;
        int latest_finish = -1;
        for (size_t j = 0; j < rooms; j++) {
            if (finish_times[j] <= sorted[i].start && finish_times[j] >= latest_finish) {
                latest_finish = finish_times[j];
                selected = j;
            }
        }
        
        if (selected != rooms) {
            finish_times[selected] = sorted[i].finish;
            (*result)++;
        }
    }

    free(sorted);
    free(finish_times);
    return CONF_OK;
}