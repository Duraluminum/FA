#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "students.h"


static int read_line(const char *message, char *buffer, size_t size) {
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

static int parse_choice(const char *text, int *choice) {
    if (text == NULL || choice == NULL || text[0] == '\0') return 0;
    char *end;
    long value = strtol(text, &end, 10);
    if (end == text) return 0;
    while (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t') end++;
    if (*end != '\0' || value < 0 || value > 5) return 0;
    *choice = (int)value;

    return 1;
}


int main(int argc, char **argv) {
    if (argc != 3 || argv[1][0] == '\0' || argv[2][0] == '\0' || strcmp(argv[1], argv[2]) == 0) {
        fprintf(stderr, "Usage: %s <students_file> <trace_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Cannot open students file\n");
        return 1;
    }

    Student *students = NULL;
    size_t count = 0;
    StudentStatus status = read_students(input, &students, &count);
    if (fclose(input) != 0 && status == STUDENT_OK) status = STUDENT_INPUT;
    if (status != STUDENT_OK || count == 0) {
        fprintf(stderr, count == 0 ? "No students in file\n" : "Cannot read students: %d\n", (int)status);
        free_students(students, count);
        return 1;
    }

    FILE *trace = fopen(argv[2], "w");
    if (trace == NULL) {
        fprintf(stderr, "Cannot open trace file\n");
        free_students(students, count);
        return 1;
    }

    char line[256];
    for (;;) {
        printf("\n1 Search students\n2 Sort students\n3 Write student by ID to trace\n4 Write above-average students to trace\n5 Show all students\n0 Exit\nChoice: ");
        if (fgets(line, sizeof(line), stdin) == NULL) break;

        int choice;
        if (!parse_choice(line, &choice)) {
            printf("Invalid menu item\n");
            continue;
        }

        switch (choice) {
            case 0:
                break;

            case 1:
            case 2: {
                printf("Field: 1 ID, 2 surname, 3 name, 4 group\n");
                if (fgets(line, sizeof(line), stdin) == NULL) {
                    choice = 0;
                    break;
                }

                int field;
                if (!parse_choice(line, &field) || field < 1 || field > 4) {
                    printf("Invalid field\n");
                    break;
                }

                if (choice == 1) {
                    if (!read_line("Search value: ", line, sizeof(line))) break;
                    const Student **matches = NULL;
                    size_t match_count = 0;
                    status = find_students(students, count, field, line, &matches, &match_count);
                    if (status != STUDENT_OK) printf("Invalid search value or memory error\n");
                    else if (match_count == 0) printf("No students found\n");
                    else print_student_matches(matches, match_count);
                    free(matches);
                } else {
                    sort_students(students, count, field);
                    print_students(students, count);
                }

                break;
            }

            case 3: {
                if (!read_line("Student ID: ", line, sizeof(line))) {
                    printf("Invalid ID\n");
                    break;
                }

                const Student **matches = NULL;
                size_t match_count = 0;
                status = find_students(students, count, 1, line, &matches, &match_count);
                if (status != STUDENT_OK) printf("Invalid ID or memory error\n");
                else if (match_count == 0) printf("Student not found\n");
                else {
                    int output_error = 0;
                    for (size_t i = 0; i < match_count; i++)
                        if (write_student_trace(trace, matches[i]) != STUDENT_OK) output_error = 1;
                    if (output_error || fflush(trace) != 0) printf("Trace write error\n");
                    else printf("%zu student record(s) written to trace file\n", match_count);
                }

                free(matches);
                break;
            }

            case 4: {
                const Student **matches = NULL;
                size_t match_count = 0;
                double average = 0.0;
                status = find_above_average(students, count, 1.0e-9, &matches, &match_count, &average);
                if (status == STUDENT_OK) status = write_above_average(trace, matches, match_count, average);
                free(matches);

                if (status != STUDENT_OK || fflush(trace) != 0) printf("Trace write error\n");
                else printf("Above-average students written to trace file\n");

                break;
            }

            case 5:
                print_students(students, count);
                break;

            default:
                printf("Unknown menu item\n");
                break;
        }

        if (choice == 0) break;
    }

    int close_error = fclose(trace);
    free_students(students, count);
    if (close_error != 0) {
        fprintf(stderr, "Trace close error\n");
        return 1;
    }

    return 0;
}
