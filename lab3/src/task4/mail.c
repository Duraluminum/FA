#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include "mail.h"


static MailStatus address_copy(const Address *source, Address *destination) {
    if (source == NULL || destination == NULL) return MAIL_ARGUMENT;
    memset(destination, 0, sizeof(Address));

    if (string_copy(&source->city, &destination->city) != MAIL_OK ||
        string_copy(&source->street, &destination->street) != MAIL_OK ||
        string_copy(&source->building, &destination->building) != MAIL_OK ||
        string_copy(&source->postcode, &destination->postcode) != MAIL_OK) {
        string_delete(&destination->city);
        string_delete(&destination->street);
        string_delete(&destination->building);
        string_delete(&destination->postcode);
        return MAIL_MEMORY;
    }

    destination->house = source->house;
    destination->apartment = source->apartment;
    return MAIL_OK;
}

static int digits_only(const char *text, size_t length) {
    if (text == NULL || strlen(text) != length) return 0;
    for (size_t i = 0; i < length; i++)
        if (text[i] < '0' || text[i] > '9') return 0;
    return 1;
}

static int date_compare(const String *first, const String *second) {
    time_t a, b;
    valid_datetime(first->data, &a);
    valid_datetime(second->data, &b);
    if (a < b) return -1;
    if (a > b) return 1;

    return 0;
}

static int compare_mail(const void *first, const void *second) {
    const Mail *a = first;
    const Mail *b = second;
    int cmp = string_compare_equal(&a->recipient.postcode, &b->recipient.postcode);
    if (cmp != 0) return cmp;

    return string_compare_equal(&a->id, &b->id);
}

static int compare_created(const void *first, const void *second) {
    const Mail *a = *(const Mail *const *)first;
    const Mail *b = *(const Mail *const *)second;
    return date_compare(&a->created_at, &b->created_at);
}


MailStatus string_create(const char *text, String *result) {
    if (text == NULL || result == NULL) return MAIL_ARGUMENT;

    size_t length = strlen(text);
    if (length > (size_t)INT_MAX) return MAIL_OVERFLOW;

    char *copy = malloc(length + 1);
    if (copy == NULL) return MAIL_MEMORY;
    memcpy(copy, text, length + 1);

    result->data = copy;
    result->length = (int)length;
    return MAIL_OK;
}

void string_delete(String *text) {
    if (text == NULL) return;
    free(text->data);
    text->data = NULL;
    text->length = 0;
}

int string_compare_order(const String *first, const String *second) {
    if (first == NULL || second == NULL || first->data == NULL || second->data == NULL) return 0;
    if (first->length < second->length) return -1;
    if (first->length > second->length) return 1;
    return strcmp(first->data, second->data);
}

int string_compare_equal(const String *first, const String *second) {
    if (first == NULL || second == NULL || first->data == NULL || second->data == NULL) return 0;
    return strcmp(first->data, second->data);
}

MailStatus string_copy(const String *source, String *destination) {
    if (source == NULL || destination == NULL || source->data == NULL) return MAIL_ARGUMENT;
    String copy = {NULL, 0};
    MailStatus status = string_create(source->data, &copy);
    if (status != MAIL_OK) return status;
    string_delete(destination);
    *destination = copy;

    return MAIL_OK;
}

MailStatus string_copy_new(const String *source, String **destination) {
    if (source == NULL || destination == NULL || source->data == NULL || *destination != NULL) return MAIL_ARGUMENT;

    String *copy = malloc(sizeof(String));
    if (copy == NULL) return MAIL_MEMORY;

    copy->data = NULL;
    copy->length = 0;
    MailStatus status = string_copy(source, copy);
    if (status != MAIL_OK) {
        free(copy);
        return status;
    }

    *destination = copy;
    return MAIL_OK;
}

MailStatus string_concat(String *first, const String *second) {
    if (first == NULL || second == NULL || first->data == NULL || second->data == NULL || first->length < 0 || second->length < 0) return MAIL_ARGUMENT;
    if (second->length > INT_MAX - first->length) return MAIL_OVERFLOW;

    int old_length = first->length;
    size_t length = (size_t)old_length + (size_t)second->length;
    char *joined = realloc(first->data, length + 1);
    if (joined == NULL) return MAIL_MEMORY;

    if (first == second) memmove(joined + old_length, joined, (size_t)old_length + 1);
    else memcpy(joined + old_length, second->data, (size_t)second->length + 1);

    first->data = joined;
    first->length = (int)length;

    return MAIL_OK;
}

int valid_datetime(const char *text, time_t *value) {
    if (text == NULL || value == NULL || strlen(text) != 19) return 0;
    if (text[2] != ':' || text[5] != ':' || text[10] != ' ' || text[13] != ':' || text[16] != ':') return 0;

    int parts[6] = {0};
    const int positions[6] = {0, 3, 6, 11, 14, 17};
    const int lengths[6] = {2, 2, 4, 2, 2, 2};
    for (size_t i = 0; i < 6; i++) {
        for (int j = 0; j < lengths[i]; j++) {
            char ch = text[positions[i] + j];
            if (ch < '0' || ch > '9') return 0;
            parts[i] = parts[i] * 10 + ch - '0';
        }
    }

    struct tm date;
    memset(&date, 0, sizeof(date));
    date.tm_mday = parts[0];
    date.tm_mon = parts[1] - 1;
    date.tm_year = parts[2] - 1900;
    date.tm_hour = parts[3];
    date.tm_min = parts[4];
    date.tm_sec = parts[5];
    date.tm_isdst = -1;
    if (date.tm_year < -1899 || date.tm_mon < 0 || date.tm_mon > 11 || date.tm_hour > 23 || date.tm_min > 59 || date.tm_sec > 59) return 0;

    int day = date.tm_mday, month = date.tm_mon, year = date.tm_year;
    time_t timestamp = mktime(&date);
    if (timestamp == (time_t)-1) return 0;
    struct tm *check = localtime(&timestamp);
    if (check == NULL || check->tm_mday != day || check->tm_mon != month || check->tm_year != year ||
        check->tm_hour != parts[3] || check->tm_min != parts[4] || check->tm_sec != parts[5]) return 0;

    *value = timestamp;
    return 1;
}

MailStatus post_create(Post *post, const Address *office) {
    if (post == NULL || office == NULL) return MAIL_ARGUMENT;
    memset(post, 0, sizeof(Post));
    post->office = malloc(sizeof(Address));
    if (post->office == NULL) return MAIL_MEMORY;
    if (address_copy(office, post->office) != MAIL_OK) {
        free(post->office);
        post->office = NULL;
        return MAIL_MEMORY;
    }

    return MAIL_OK;
}

void mail_free(Mail *mail) {
    if (mail == NULL) return;
    string_delete(&mail->recipient.city);
    string_delete(&mail->recipient.street);
    string_delete(&mail->recipient.building);
    string_delete(&mail->recipient.postcode);
    string_delete(&mail->id);
    string_delete(&mail->created_at);
    string_delete(&mail->delivery_at);
    memset(mail, 0, sizeof(Mail));
}

void post_free(Post *post) {
    if (post == NULL) return;
    
    for (size_t i = 0; i < post->count; i++) mail_free(&post->mails[i]);
    free(post->mails);
    if (post->office != NULL) {
        string_delete(&post->office->city);
        string_delete(&post->office->street);
        string_delete(&post->office->building);
        string_delete(&post->office->postcode);
        free(post->office);
    }

    memset(post, 0, sizeof(Post));
}

MailStatus post_add_mail(Post *post, const Mail *mail) {
    if (post == NULL || mail == NULL || post->office == NULL) return MAIL_ARGUMENT;
    if (mail->weight < 0.0 || !isfinite(mail->weight)) return MAIL_VALUE;
    if (mail->recipient.city.data == NULL || mail->recipient.city.length == 0 ||
        mail->recipient.street.data == NULL || mail->recipient.street.length == 0 ||
        mail->recipient.building.data == NULL || mail->recipient.postcode.data == NULL ||
        mail->id.data == NULL || mail->created_at.data == NULL || mail->delivery_at.data == NULL ||
        (mail->delivered != 0 && mail->delivered != 1)) return MAIL_VALUE;
    if (!digits_only(mail->recipient.postcode.data, 6) || !digits_only(mail->id.data, 14) ||
        mail->recipient.house == 0 || mail->recipient.apartment == 0) return MAIL_VALUE;

    time_t created, delivery;
    if (!valid_datetime(mail->created_at.data, &created) || !valid_datetime(mail->delivery_at.data, &delivery) || delivery < created) return MAIL_VALUE;
    if (post_find_mail(post, mail->id.data) != NULL) return MAIL_VALUE;

    if (post->count == post->capacity) {
        size_t next = post->capacity == 0 ? 4 : post->capacity * 2;
        if (next < post->capacity || next > (size_t)-1 / sizeof(Mail)) return MAIL_OVERFLOW;
        Mail *new_mails = realloc(post->mails, next * sizeof(Mail));
        if (new_mails == NULL) return MAIL_MEMORY;
        post->mails = new_mails;
        post->capacity = next;
    }

    Mail copy;
    memset(&copy, 0, sizeof(Mail));
    if (address_copy(&mail->recipient, &copy.recipient) != MAIL_OK ||
        string_copy(&mail->id, &copy.id) != MAIL_OK ||
        string_copy(&mail->created_at, &copy.created_at) != MAIL_OK ||
        string_copy(&mail->delivery_at, &copy.delivery_at) != MAIL_OK) {
        mail_free(&copy);
        return MAIL_MEMORY;
    }

    copy.weight = mail->weight;
    copy.delivered = mail->delivered;
    post->mails[post->count++] = copy;
    return MAIL_OK;
}

MailStatus post_remove_mail(Post *post, const char *id) {
    if (post == NULL || id == NULL) return MAIL_ARGUMENT;
    if (!digits_only(id, 14)) return MAIL_VALUE;

    size_t index = post->count;
    for (size_t i = 0; i < post->count; i++)
        if (strcmp(post->mails[i].id.data, id) == 0) index = i;
    if (index == post->count) return MAIL_NOT_FOUND;

    mail_free(&post->mails[index]);
    for (size_t i = index; i + 1 < post->count; i++) post->mails[i] = post->mails[i + 1];
    post->count--;

    return MAIL_OK;
}

const Mail *post_find_mail(const Post *post, const char *id) {
    if (post == NULL || id == NULL) return NULL;
    for (size_t i = 0; i < post->count; i++)
        if (post->mails[i].id.data != NULL && strcmp(post->mails[i].id.data, id) == 0) return &post->mails[i];

    return NULL;
}

void post_sort(Post *post) {
    if (post != NULL && post->count > 1) qsort(post->mails, post->count, sizeof(Mail), compare_mail);
}

void post_print_mail(const Mail *mail) {
    if (mail == NULL || mail->id.data == NULL) return;

    printf("ID: %s\nRecipient: %s, %s, house %u, building %s, apartment %u, postcode %s\n",
           mail->id.data, mail->recipient.city.data, mail->recipient.street.data,
           mail->recipient.house, mail->recipient.building.data, mail->recipient.apartment, mail->recipient.postcode.data);

    printf("Weight: %.3f kg\nCreated: %s\nDelivery time / deadline: %s\nStatus: %s\n\n",
           mail->weight, mail->created_at.data, mail->delivery_at.data, mail->delivered ? "delivered" : "not delivered");
}

MailStatus post_select_mails(const Post *post, int expired, time_t now, const Mail ***selected, size_t *count) {
    if (post == NULL || selected == NULL || count == NULL || (expired != 0 && expired != 1))
        return MAIL_ARGUMENT;

    *selected = NULL;
    *count = 0;
    if (post->count == 0) return MAIL_OK;
    if (post->count > (size_t)-1 / sizeof(Mail *)) return MAIL_OVERFLOW;

    const Mail **result = malloc(post->count * sizeof(Mail *));
    if (result == NULL) return MAIL_MEMORY;

    size_t used = 0;
    for (size_t i = 0; i < post->count; i++) {
        time_t deadline;
        if (!valid_datetime(post->mails[i].delivery_at.data, &deadline)) {
            free(result);
            return MAIL_VALUE;
        }
        
        if ((!expired && post->mails[i].delivered) ||
            (expired && !post->mails[i].delivered && deadline <= now)) result[used++] = &post->mails[i];
    }

    qsort(result, used, sizeof(Mail *), compare_created);
    if (used == 0) free(result);
    else *selected = result;
    *count = used;
    
    return MAIL_OK;
}