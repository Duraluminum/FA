#ifndef BACKUP_H
#define BACKUP_H

#include <stddef.h>


typedef enum {
    BACKUP_OK = 0,
    BACKUP_ARGUMENT,
    BACKUP_INPUT,
    BACKUP_MEMORY,
    BACKUP_VALUE,
    BACKUP_OVERFLOW,
    BACKUP_OUTPUT
} BackupStatus;

typedef struct {
    char *name;
    size_t size;
    unsigned long long importance;
} BackupFile;

typedef struct {
    BackupFile *files;
    size_t count;
    size_t total_size;
    unsigned long long total_importance;
} BackupResult;

int parse_unsigned(const char *text, unsigned long long *value);
int read_line(char **line);
size_t split_line(char *line, char **tokens, size_t limit);
char *copy_name(const char *name);
BackupStatus choose_files(const BackupFile *files, size_t count, size_t capacity, BackupResult *result);
void free_backup_files(BackupFile *files, size_t count);
void free_backup_result(BackupResult *result);


#endif