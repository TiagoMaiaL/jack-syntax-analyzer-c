#include "file-handler.h"

FILE *file_open(const char *path)
{
    return fopen(path, "r");
}

void write(const char *str, FILE *file)
{
    fprintf(file, "%s", str);
}

