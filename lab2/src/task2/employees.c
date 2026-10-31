#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include "employees.h"


static char *copy_text(const char *text) {
    size_t length = strlen(text);
    if (length == SIZE_MAX) return NULL;

    char *copy;
    copy = malloc(length + 1);
    if (copy != NULL)
        memcpy(copy, text, length + 1);

    return copy;
}

static int is_latin_word(const char *word) {
    if (word[0] == '\0') return 0;

    for (size_t i = 0; word[i] != '\0'; i++) {
        if (!((word[i] >= 'A' && word[i] <= 'Z') || (word[i] >= 'a' && word[i] <= 'z'))) return 0;
    }

    return 1;
}

static int parse_unsigned(const char *text, unsigned long long *value) {
    if (text == NULL || text[0] == '\0' || value == NULL) return 0;
    
    unsigned long long number = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] < '0' || text[i] > '9') return 0;

        unsigned int digit;
        digit = (unsigned int)(text[i] - '0');
        if (number > (ULLONG_MAX - digit) / 10ULL) return 0;

        number = number * 10ULL + digit;
    }

    *value = number;
    return 1;
}

static int read_line(FILE *file, char **line) {
    size_t used = 0, capacity = 128;
    char *text = malloc(capacity);
    if (text == NULL) return -1;

    int ch;
    while ((ch = fgetc(file)) != EOF && ch != '\n') {
        if (ch == '\0') {
            free(text);
            return -3;
        }

        if (used + 1 >= capacity) {
            if (capacity > SIZE_MAX / 2) {
                free(text);
                return -1;
            }
            
            size_t next;
            char *new_text;
            next = capacity * 2;
            new_text = realloc(text, next);
            if (new_text == NULL) {
                free(text);
                return -1;
            }

            text = new_text;
            capacity = next;
        }

        text[used++] = (char)ch;
    }

    if (ch == EOF && used == 0) {
        free(text);
        return ferror(file) ? -2 : 0;
    }

    text[used] = '\0';
    *line = text;
    return 1;
}

static int compare_employee(const Employee *a, const Employee *b, double eps) {
    if (fabs(a->salary - b->salary) > eps) return a->salary < b->salary ? -1 : 1;
    
    int cmp;

    cmp = strcmp(a->surname, b->surname);
    if (cmp != 0) return cmp;
    
    cmp = strcmp(a->name, b->name);
    if (cmp != 0) return cmp;
    
    if (a->id < b->id) return -1;
    if (a->id > b->id) return 1;

    return 0;
}

typedef struct {
    Employee employee;
    double eps;
} EmployeeSortItem;

static int qsort_compare(const void *first, const void *second) {
    const EmployeeSortItem *a = first;
    const EmployeeSortItem *b = second;
    return compare_employee(&a->employee, &b->employee, a->eps);
}

EmployeeStatus parse_sort_flag(const char *arg, int *descending) {
    if (arg == NULL || descending == NULL) return EMPLOYEE_ARGUMENT;
    if ((arg[0] != '-' && arg[0] != '/') || arg[1] == '\0' || arg[2] != '\0') return EMPLOYEE_VALUE;

    if (arg[1] == 'a') *descending = 0;
    else if (arg[1] == 'd') *descending = 1;
    else return EMPLOYEE_VALUE;

    return EMPLOYEE_OK;
}

void free_employees(Employee *employees, size_t count) {
    if (employees == NULL) return;

    for (size_t i = 0; i < count; i++) {
        free(employees[i].name);
        free(employees[i].surname);
    }

    free(employees);
}

EmployeeStatus read_employees(FILE *file, Employee **employees, size_t *count) {
    if (file == NULL || employees == NULL || count == NULL) return EMPLOYEE_ARGUMENT;

    Employee *items = NULL;
    size_t used = 0, capacity = 0;
    char *line = NULL;
    int line_status;
    while ((line_status = read_line(file, &line)) == 1) {
        char *tokens[5];
        size_t token_count = 0;
        char *p = line;
        char *end;
        unsigned long long id;
        double salary;
        Employee item;

        while (*p != '\0') {
            while (isspace((unsigned char)*p)) p++;
            if (*p == '\0') break;
            if (token_count == 5) break;

            tokens[token_count++] = p;
            while (*p != '\0' && !isspace((unsigned char)*p)) p++;

            if (*p != '\0') *p++ = '\0';
        }

        if (token_count == 0) {
            free(line);
            line = NULL;
            continue;
        }

        if (token_count != 4 || !parse_unsigned(tokens[0], &id) || !is_latin_word(tokens[1]) || !is_latin_word(tokens[2])) {
            free(line);
            free_employees(items, used);
            return EMPLOYEE_VALUE;
        }

        salary = strtod(tokens[3], &end);
        if (end == tokens[3] || *end != '\0' || !isfinite(salary) || salary < 0.0) {
            free(line);
            free_employees(items, used);
            return EMPLOYEE_VALUE;
        }

        item.id = id;
        item.name = copy_text(tokens[1]);
        item.surname = copy_text(tokens[2]);
        item.salary = salary;
        if (item.name == NULL || item.surname == NULL) {
            free(item.name);
            free(item.surname);
            free(line);
            free_employees(items, used);
            return EMPLOYEE_MEMORY;
        }

        if (used == capacity) {
            size_t next = capacity == 0 ? 8 : capacity * 2;
            Employee *new_items;

            if (next < capacity || next > SIZE_MAX / sizeof(Employee)) {
                free(item.name);
                free(item.surname);
                free(line);
                free_employees(items, used);
                return EMPLOYEE_MEMORY;
            }

            new_items = realloc(items, next * sizeof(Employee));
            if (new_items == NULL) {
                free(item.name);
                free(item.surname);
                free(line);
                free_employees(items, used);
                return EMPLOYEE_MEMORY;
            }

            items = new_items;
            capacity = next;
        }

        items[used++] = item;
        free(line);
        line = NULL;
    }

    free(line);
    if (line_status < 0 || ferror(file)) {
        free_employees(items, used);
        return line_status == -1 ? EMPLOYEE_MEMORY : EMPLOYEE_INPUT;
    }

    *employees = items;
    *count = used;
    return EMPLOYEE_OK;
}

EmployeeStatus sort_employees(Employee *employees, size_t count, int descending, double eps) {
    if ((employees == NULL && count != 0) || eps < 0.0) return EMPLOYEE_ARGUMENT;
    if (count < 2) return EMPLOYEE_OK;
    if (count > SIZE_MAX / sizeof(EmployeeSortItem)) return EMPLOYEE_MEMORY;

    EmployeeSortItem *items;
    items = malloc(count * sizeof(EmployeeSortItem));
    if (items == NULL) return EMPLOYEE_MEMORY;

    for (size_t i = 0; i < count; i++) {
        items[i].employee = employees[i];
        items[i].eps = eps;
    }

    qsort(items, count, sizeof(EmployeeSortItem), qsort_compare);
    for (size_t i = 0; i < count; i++) {
        size_t source = descending ? count - i - 1 : i;
        employees[i] = items[source].employee;
    }

    free(items);
    return EMPLOYEE_OK;
}

EmployeeStatus write_employees(FILE *file, const Employee *employees, size_t count) {
    if (file == NULL || (employees == NULL && count != 0)) return EMPLOYEE_ARGUMENT;

    for (size_t i = 0; i < count; i++) {
        if (fprintf(file, "%llu %s %s %.10g\n", employees[i].id, employees[i].name,
                    employees[i].surname, employees[i].salary) < 0) return EMPLOYEE_OUTPUT;
    }

    return ferror(file) ? EMPLOYEE_OUTPUT : EMPLOYEE_OK;
}