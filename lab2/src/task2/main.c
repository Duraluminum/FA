#include <stdio.h>
#include "employees.h"


int main(int argc, char *argv[]) {
    FILE *input = NULL;
    FILE *output = NULL;
    Employee *employees = NULL;
    size_t count = 0;
    int descending = 0;
    EmployeeStatus status;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input> <flag> <output>.\n", argv[0]);
        return 1;
    }

    status = parse_sort_flag(argv[2], &descending);
    if (status != EMPLOYEE_OK) {
        fprintf(stderr, "Error parsing flag: %d\n", status);
        return 1;
    }

    input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return 1;
    }

    status = read_employees(input, &employees, &count);
    if (fclose(input) != 0 && status == EMPLOYEE_OK) status = EMPLOYEE_INPUT;
    if (status == EMPLOYEE_OK) status = sort_employees(employees, count, descending, 1e-9);
    if (status != EMPLOYEE_OK) {
        fprintf(stderr, "Error processing input: %d\n", status);
        free_employees(employees, count);
        return 1;
    }

    output = fopen(argv[3], "w");
    if (output == NULL) {
        fprintf(stderr, "Error opening output file\n");
        free_employees(employees, count);
        return 1;
    }

    status = write_employees(output, employees, count);
    if (fclose(output) != 0 && status == EMPLOYEE_OK) status = EMPLOYEE_OUTPUT;
    free_employees(employees, count);
    if (status != EMPLOYEE_OK) {
        fprintf(stderr, "Error writing output: %d\n", status);
        return 1;
    }

    return 0;
}