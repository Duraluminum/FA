#ifndef EMPLOYEES_H
#define EMPLOYEES_H

#include <stddef.h>
#include <stdio.h>


typedef enum {
    EMPLOYEE_OK = 0,
    EMPLOYEE_ARGUMENT,
    EMPLOYEE_INPUT,
    EMPLOYEE_OUTPUT,
    EMPLOYEE_MEMORY,
    EMPLOYEE_VALUE
} EmployeeStatus;

typedef struct {
    unsigned long long id;
    char *name;
    char *surname;
    double salary;
} Employee;

EmployeeStatus parse_sort_flag(const char *arg, int *descending);
EmployeeStatus read_employees(FILE *file, Employee **employees, size_t *count);
EmployeeStatus sort_employees(Employee *employees, size_t count, int descending, double eps);
EmployeeStatus write_employees(FILE *file, const Employee *employees, size_t count);
void free_employees(Employee *employees, size_t count);


#endif