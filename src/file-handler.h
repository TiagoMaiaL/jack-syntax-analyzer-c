#include <stdbool.h>
#include <stdio.h>

FILE *file_open(const char *path, const bool create);
void file_write(const char *str, FILE *file);
void file_close(FILE *file);

