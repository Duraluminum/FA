#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "mail.h"


static int read_text(const char *message, char *buffer, size_t size) {
    printf("%s", message);
    if (fgets(buffer, (int)size, stdin) == NULL) return 0;
    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') buffer[length - 1] = '\0';
    else if (!feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return 0;
    }

    return buffer[0] != '\0';
}

static int parse_positive(const char *text, unsigned int *value) {
    if (text == NULL || value == NULL || text[0] == '\0') return 0;

    unsigned long number = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] < '0' || text[i] > '9') return 0;
        unsigned int digit = (unsigned int)(text[i] - '0');
        if (number > (4294967295UL - digit) / 10UL) return 0;
        number = number * 10UL + digit;
    }

    if (number == 0) return 0;
    *value = (unsigned int)number;
    return 1;
}

static int read_number(const char *message, unsigned int *value) {
    char line[64];
    return read_text(message, line, sizeof(line)) && parse_positive(line, value);
}

static int read_mail(Mail *mail) {
    char line[256];
    memset(mail, 0, sizeof(Mail));
    if (!read_text("Recipient city: ", line, sizeof(line)) || string_create(line, &mail->recipient.city) != MAIL_OK) return 0;
    if (!read_text("Recipient street: ", line, sizeof(line)) || string_create(line, &mail->recipient.street) != MAIL_OK) return 0;
    if (!read_number("House number: ", &mail->recipient.house)) return 0;
    if (!read_text("Building (use - if absent): ", line, sizeof(line)) || string_create(line, &mail->recipient.building) != MAIL_OK) return 0;
    if (!read_number("Apartment number: ", &mail->recipient.apartment)) return 0;
    if (!read_text("6-digit postcode: ", line, sizeof(line)) || string_create(line, &mail->recipient.postcode) != MAIL_OK) return 0;
    if (!read_text("Weight (non-negative): ", line, sizeof(line))) return 0;

    char *end;
    mail->weight = strtod(line, &end);
    if (end == line || *end != '\0' || !isfinite(mail->weight) || mail->weight < 0.0) return 0;
    if (!read_text("14-digit postal ID: ", line, sizeof(line)) || string_create(line, &mail->id) != MAIL_OK) return 0;
    if (!read_text("Creation time (dd:MM:yyyy hh:mm:ss): ", line, sizeof(line)) || string_create(line, &mail->created_at) != MAIL_OK) return 0;
    if (!read_text("Delivery time / deadline (dd:MM:yyyy hh:mm:ss): ", line, sizeof(line)) || string_create(line, &mail->delivery_at) != MAIL_OK) return 0;
    if (!read_text("Delivered? (0/1): ", line, sizeof(line)) || (strcmp(line, "0") != 0 && strcmp(line, "1") != 0)) return 0;
    mail->delivered = line[0] == '1';

    return 1;
}

static void print_string_demo(void) {
    String first = {NULL, 0}, second = {NULL, 0}, copied = {NULL, 0};
    String *dynamic_copy = NULL;

    if (string_create("pear", &first) != MAIL_OK || string_create("plum", &second) != MAIL_OK ||
        string_copy(&first, &copied) != MAIL_OK || string_copy_new(&second, &dynamic_copy) != MAIL_OK) {
        string_delete(&first);
        string_delete(&second);
        string_delete(&copied);
        if (dynamic_copy != NULL) {
            string_delete(dynamic_copy);
            free(dynamic_copy);
        }

        printf("String demonstration could not be completed\n");
        return;
    }

    printf("String demo: order=%d, equal=%d\n", string_compare_order(&first, &second),
           string_compare_equal(&first, &second));

    if (string_concat(&copied, &second) == MAIL_OK)
        printf("Copy to existing String and concatenate: %s\n", copied.data);

    printf("Copy to new String: %s\n", dynamic_copy->data);

    string_delete(&first);
    string_delete(&second);
    string_delete(&copied);
    string_delete(dynamic_copy);
    free(dynamic_copy);
}

static void show_selected_mails(const Post *post, int expired) {
    const Mail **selected = NULL;
    size_t count = 0;
    MailStatus status = post_select_mails(post, expired, time(NULL), &selected, &count);
    if (status != MAIL_OK) {
        printf("Cannot select mail items: %d\n", (int)status);
        free(selected);
        return;
    }

    if (count == 0) printf("No matching mail items\n");
    for (size_t i = 0; i < count; i++) post_print_mail(selected[i]);
    free(selected);
}

static int parse_choice(const char *text, int *choice) {
    if (text == NULL || choice == NULL || text[0] == '\0') return 0;

    char *end;
    long value = strtol(text, &end, 10);
    if (end == text) return 0;
    while (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t') end++;
    if (*end != '\0' || value < 0 || value > 6) return 0;
    *choice = (int)value;

    return 1;
}


int main(void) {
    print_string_demo();

    Address office;
    memset(&office, 0, sizeof(Address));
    char line[256];
    printf("\nCurrent post office address\n");
    if (!read_text("City: ", line, sizeof(line)) || string_create(line, &office.city) != MAIL_OK ||
        !read_text("Street: ", line, sizeof(line)) || string_create(line, &office.street) != MAIL_OK ||
        !read_number("House number: ", &office.house) ||
        !read_text("Building: ", line, sizeof(line)) || string_create(line, &office.building) != MAIL_OK ||
        !read_text("Postcode (6 digits): ", line, sizeof(line)) || strlen(line) != 6) {
        fprintf(stderr, "Invalid post office address\n");
        string_delete(&office.city);
        string_delete(&office.street);
        string_delete(&office.building);
        string_delete(&office.postcode);
        return 1;
    }

    if (string_create(line, &office.postcode) != MAIL_OK) {
        fprintf(stderr, "Invalid post office postcode\n");
        string_delete(&office.city);
        string_delete(&office.street);
        string_delete(&office.building);
        return 1;
    }

    for (size_t i = 0; i < 6; i++) {
        if (line[i] < '0' || line[i] > '9') {
            fprintf(stderr, "Invalid post office postcode\n");
            string_delete(&office.city);
            string_delete(&office.street);
            string_delete(&office.building);
            string_delete(&office.postcode);
            return 1;
        }
    }

    Post post;
    MailStatus status = post_create(&post, &office);
    string_delete(&office.city);
    string_delete(&office.street);
    string_delete(&office.building);
    string_delete(&office.postcode);
    if (status != MAIL_OK) {
        fprintf(stderr, "Cannot create post office: %d\n", (int)status);
        return 1;
    }

    for (;;) {
        printf("\n1 Add mail\n2 Remove mail by ID\n3 Find mail by ID\n4 Sort mails\n5 Show delivered\n6 Show expired undelivered\n0 Exit\nChoice: ");
        if (fgets(line, sizeof(line), stdin) == NULL) break;

        int choice;
        if (!parse_choice(line, &choice)) {
            printf("Invalid menu item\n");
            continue;
        }

        switch (choice) {
            case 0:
                break;

            case 1: {
                Mail mail;
                if (!read_mail(&mail)) {
                    fprintf(stderr, "Invalid mail data\n");
                    mail_free(&mail);
                    break;
                }

                status = post_add_mail(&post, &mail);
                mail_free(&mail);
                if (status != MAIL_OK) fprintf(stderr, "Cannot add mail: %d\n", (int)status);
                else printf("Mail added\n");
                break;
            }

            case 2:
                if (!read_text("Postal ID: ", line, sizeof(line))) break;
                status = post_remove_mail(&post, line);
                if (status != MAIL_OK) fprintf(stderr, "Cannot remove mail: %d\n", (int)status);
                else printf("Mail removed\n");
                break;

            case 3: {
                if (!read_text("Postal ID: ", line, sizeof(line))) break;
                const Mail *mail = post_find_mail(&post, line);
                if (mail == NULL) printf("Mail not found\n");
                else {
                    printf("Post office: %s, %s, house %u\n", post.office->city.data,
                           post.office->street.data, post.office->house);
                    post_print_mail(mail);
                }

                break;
            }

            case 4:
                post_sort(&post);
                printf("Mails sorted by recipient postcode and ID\n");
                break;

            case 5:
                show_selected_mails(&post, 0);
                break;

            case 6:
                show_selected_mails(&post, 1);
                break;

            default:
                printf("Unknown menu item\n");
                break;
        }

        if (choice == 0) break;
    }

    post_free(&post);
    return 0;
}