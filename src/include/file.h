#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

char *get_file_path(const char *type);
bool file_exists(const char *filename);
char **read_file(const char *filename, int *lines_read);
int write_file(const char *filename, const char *content, const char *mode);

#endif /* FILE_H */