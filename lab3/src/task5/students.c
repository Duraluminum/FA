#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include "students.h"


static char *copy_text(const char *text) {
    size_t length = strlen(text);
    char *copy = malloc(length + 1);
    if (copy != NULL) memcpy(copy, text, length + 1);
    return copy;
}

static int parse_unsigned(const char *text, unsigned long long *value) {
    if (text == NULL || text[0] == '\0' || value == NULL) return 0;

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

static int is_latin_word(const char *text) {
    if (text == NULL || text[0] == '\0') return 0;

    for (size_t i = 0; text[i] != '\0'; i++)
        if (!((text[i] >= 'A' && text[i] <= 'Z') || (text[i] >= 'a' && text[i] <= 'z'))) return 0;

    return 1;
}

static size_t split_tokens(char *line, char **tokens, size_t limit) {
    size_t count = 0;
    char *p = line;
    while (*p != '\0') {
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') break;
        if (count == limit) return limit + 1;
        tokens[count++] = p;
        while (*p != '\0' && !isspace((unsigned char)*p)) p++;
        if (*p != '\0') *p++ = '\0';
    }

    return count;
}

static void student_free(Student *student) {
    if (student == NULL) return;
    free(student->name);
    free(student->surname);
    free(student->group);
    free(student->grades);
    memset(student, 0, sizeof(Student));
}

static double student_average(const Student *student) {
    unsigned int total = 0;
    for (size_t i = 0; i < EXAM_COUNT; i++) total += student->grades[i];
    return (double)total / (double)EXAM_COUNT;
}

static int compare_id(const void *first, const void *second) {
    const Student *a = first;
    const Student *b = second;
    if (a->id < b->id) return -1;
    if (a->id > b->id) return 1;

    return 0;
}

static int compare_surname(const void *first, const void *second) {
    const Student *a = first;
    const Student *b = second;
    int cmp = strcmp(a->surname, b->surname);
    if (cmp != 0) return cmp;
    return strcmp(a->name, b->name);
}

static int compare_name(const void *first, const void *second) {
    const Student *a = first;
    const Student *b = second;
    int cmp = strcmp(a->name, b->name);
    if (cmp != 0) return cmp;
    return strcmp(a->surname, b->surname);
}

static int compare_group(const void *first, const void *second) {
    const Student *a = first;
    const Student *b = second;
    int cmp = strcmp(a->group, b->group);
    if (cmp != 0) return cmp;
    return compare_surname(first, second);
}


StudentStatus read_students(FILE *file, Student **students, size_t *count) {
    if (file == NULL || students == NULL || count == NULL) return STUDENT_ARGUMENT;

    *students = NULL;
    *count = 0;
    Student *items = NULL;
    size_t used = 0, capacity = 0;
    char line[1024];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strchr(line, '\n') == NULL && !feof(file)) {
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
            free_students(items, used);
            return STUDENT_VALUE;
        }

        char *tokens[10];
        size_t token_count = split_tokens(line, tokens, 10);
        if (token_count == 0) continue;
        if (token_count != 9) {
            free_students(items, used);
            return STUDENT_VALUE;
        }

        Student item;
        memset(&item, 0, sizeof(Student));
        if (!parse_unsigned(tokens[0], &item.id) || !is_latin_word(tokens[1]) || !is_latin_word(tokens[2]) || tokens[3][0] == '\0') {
            free_students(items, used);
            return STUDENT_VALUE;
        }

        item.name = copy_text(tokens[1]);
        item.surname = copy_text(tokens[2]);
        item.group = copy_text(tokens[3]);
        item.grades = malloc(EXAM_COUNT * sizeof(unsigned char));
        if (item.name == NULL || item.surname == NULL || item.group == NULL || item.grades == NULL) {
            student_free(&item);
            free_students(items, used);
            return STUDENT_MEMORY;
        }

        int valid = 1;
        for (size_t i = 0; i < EXAM_COUNT; i++) {
            unsigned long long grade;
            if (!parse_unsigned(tokens[i + 4], &grade) || grade > UCHAR_MAX) valid = 0;
            else item.grades[i] = (unsigned char)grade;
        }

        if (!valid) {
            student_free(&item);
            free_students(items, used);
            return STUDENT_VALUE;
        }

        if (used == capacity) {
            size_t next = capacity == 0 ? 8 : capacity * 2;
            if (next < capacity || next > (size_t)-1 / sizeof(Student)) {
                student_free(&item);
                free_students(items, used);
                return STUDENT_MEMORY;
            }

            Student *new_items = realloc(items, next * sizeof(Student));
            if (new_items == NULL) {
                student_free(&item);
                free_students(items, used);
                return STUDENT_MEMORY;
            }

            items = new_items;
            capacity = next;
        }

        items[used++] = item;
    }

    if (!feof(file)) {
        free_students(items, used);
        return STUDENT_INPUT;
    }

    *students = items;
    *count = used;
    return STUDENT_OK;
}

void free_students(Student *students, size_t count) {
    if (students == NULL) return;
    for (size_t i = 0; i < count; i++) student_free(&students[i]);
    free(students);
}

static void print_student(const Student *student) {
    printf("%llu %s %s %s | grades:", student->id, student->name, student->surname, student->group);
    for (size_t i = 0; i < EXAM_COUNT; i++) printf(" %u", (unsigned int)student->grades[i]);
    printf(" | average: %.2f\n", student_average(student));
}

void print_students(const Student *students, size_t count) {
    if (students == NULL && count != 0) return;
    for (size_t i = 0; i < count; i++) print_student(&students[i]);
}

void sort_students(Student *students, size_t count, int field) {
    if (students == NULL || count < 2) return;
    int (*compare)(const void *, const void *) = NULL;
    if (field == 1) compare = compare_id;
    else if (field == 2) compare = compare_surname;
    else if (field == 3) compare = compare_name;
    else if (field == 4) compare = compare_group;
    if (compare != NULL) qsort(students, count, sizeof(Student), compare);
}

StudentStatus find_students(const Student *students, size_t count, int field, const char *value, const Student ***matches, size_t *match_count) {
    if ((students == NULL && count != 0) || value == NULL || matches == NULL || match_count == NULL ||
        field < 1 || field > 4) return STUDENT_ARGUMENT;

    *matches = NULL;
    *match_count = 0;
    if (((field == 2 || field == 3) && !is_latin_word(value)) ||
        (field == 4 && value[0] == '\0')) return STUDENT_VALUE;

    unsigned long long id = 0;
    if (field == 1 && !parse_unsigned(value, &id)) return STUDENT_VALUE;
    if (count == 0) return STUDENT_OK;
    if (count > (size_t)-1 / sizeof(Student *)) return STUDENT_MEMORY;

    const Student **result = malloc(count * sizeof(Student *));
    if (result == NULL) return STUDENT_MEMORY;

    size_t found = 0;
    for (size_t i = 0; i < count; i++) {
        int match = 0;
        if (field == 1) match = students[i].id == id;
        else if (field == 2) match = strcmp(students[i].surname, value) == 0;
        else if (field == 3) match = strcmp(students[i].name, value) == 0;
        else if (field == 4) match = strcmp(students[i].group, value) == 0;
        if (match) result[found++] = &students[i];
    }

    if (found == 0) free(result);
    else *matches = result;
    *match_count = found;

    return STUDENT_OK;
}

void print_student_matches(const Student *const *matches, size_t match_count) {
    if (matches == NULL && match_count != 0) return;
    for (size_t i = 0; i < match_count; i++) print_student(matches[i]);
}

StudentStatus find_above_average(const Student *students, size_t count, double eps, const Student ***matches, size_t *match_count, double *average) {
    if ((students == NULL && count != 0) || matches == NULL || match_count == NULL || average == NULL || !isfinite(eps) || eps <= 0.0) return STUDENT_ARGUMENT;

    *matches = NULL;
    *match_count = 0;
    if (count == 0) return STUDENT_VALUE;
    if (count > (size_t)-1 / sizeof(Student *)) return STUDENT_MEMORY;

    long double total = 0.0L;
    for (size_t i = 0; i < count; i++)
        for (size_t j = 0; j < EXAM_COUNT; j++) total += students[i].grades[j];
    *average = (double)(total / ((long double)count * EXAM_COUNT));

    const Student **result = malloc(count * sizeof(Student *));
    if (result == NULL) return STUDENT_MEMORY;
    size_t found = 0;
    for (size_t i = 0; i < count; i++)
        if (student_average(&students[i]) > *average + eps) result[found++] = &students[i];

    if (found == 0) free(result);
    else *matches = result;
    *match_count = found;

    return STUDENT_OK;
}

StudentStatus write_student_trace(FILE *file, const Student *student) {
    if (file == NULL || student == NULL || student->name == NULL || student->surname == NULL ||
        student->group == NULL || student->grades == NULL) return STUDENT_ARGUMENT;

    if (fprintf(file, "Student %llu: %s %s, group %s, average grade %.2f\n",
                student->id, student->surname, student->name, student->group, student_average(student)) < 0) return STUDENT_OUTPUT;

    return STUDENT_OK;
}

StudentStatus write_above_average(FILE *file, const Student *const *matches, size_t match_count, double average) {
    if (file == NULL || (matches == NULL && match_count != 0) || !isfinite(average)) return STUDENT_ARGUMENT;
    if (fprintf(file, "Students with average grade above global average %.2f:\n", average) < 0)
        return STUDENT_OUTPUT;

    for (size_t i = 0; i < match_count; i++)
        if (fprintf(file, "%s %s\n", matches[i]->surname, matches[i]->name) < 0) return STUDENT_OUTPUT;

    return STUDENT_OK;
}