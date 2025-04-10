#include "file-handler.h"

FILE *file_open(const char *path, const bool create)
{
    return fopen(path, create ? "w" : "r");
}

void file_write(const char *str, FILE *file)
{
    fprintf(file, "%s", str);
}

void file_close(FILE *file)
{
    fclose(file);
}

