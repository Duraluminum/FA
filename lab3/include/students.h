#ifndef STUDENTS_H
#define STUDENTS_H

#include <stddef.h>
#include <stdio.h>

#define EXAM_COUNT 5


typedef enum {
    STUDENT_OK = 0,
    STUDENT_ARGUMENT,
    STUDENT_INPUT,
    STUDENT_OUTPUT,
    STUDENT_MEMORY,
    STUDENT_VALUE
} StudentStatus;

typedef struct {
    unsigned long long id;
    char *name;
    char *surname;
    char *group;
    unsigned char *grades;
} Student;

void free_students(Student *students, size_t count);
void print_students(const Student *students, size_t count);
void sort_students(Student *students, size_t count, int field);
void print_student_matches(const Student *const *matches, size_t match_count);

StudentStatus read_students(FILE *file, Student **students, size_t *count);
StudentStatus find_students(const Student *students, size_t count, int field, const char *value, const Student ***matches, size_t *match_count);
StudentStatus find_above_average(const Student *students, size_t count, double eps, const Student ***matches, size_t *match_count, double *average);
StudentStatus write_student_trace(FILE *file, const Student *student);
StudentStatus write_above_average(FILE *file, const Student *const *matches, size_t match_count, double average);


#endif