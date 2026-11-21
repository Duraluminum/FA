#ifndef MAIL_H
#define MAIL_H

#include <stddef.h>
#include <time.h>


typedef enum {
    MAIL_OK = 0,
    MAIL_ARGUMENT,
    MAIL_VALUE,
    MAIL_MEMORY,
    MAIL_NOT_FOUND,
    MAIL_OVERFLOW
} MailStatus;

typedef struct {
    char *data;
    int length;
} String;

typedef struct {
    String city;
    String street;
    unsigned int house;
    String building;
    unsigned int apartment;
    String postcode;
} Address;

typedef struct {
    Address recipient;
    double weight;
    String id;
    String created_at;
    String delivery_at;
    int delivered;
} Mail;

typedef struct {
    Address *office;
    Mail *mails;
    size_t count;
    size_t capacity;
} Post;

void string_delete(String *text);
int string_compare_order(const String *first, const String *second);
int string_compare_equal(const String *first, const String *second);

MailStatus string_create(const char *text, String *result);
MailStatus string_copy(const String *source, String *destination);
MailStatus string_copy_new(const String *source, String **destination);
MailStatus string_concat(String *first, const String *second);
MailStatus post_create(Post *post, const Address *office);
MailStatus post_add_mail(Post *post, const Mail *mail);
MailStatus post_remove_mail(Post *post, const char *id);
MailStatus post_select_mails(const Post *post, int expired, time_t now, const Mail ***selected, size_t *count);
const Mail *post_find_mail(const Post *post, const char *id);

void post_free(Post *post);
void post_sort(Post *post);
void post_print_mail(const Mail *mail);
void mail_free(Mail *mail);
int valid_datetime(const char *text, time_t *value);


#endif