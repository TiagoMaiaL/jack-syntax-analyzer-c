#include "file-handler.h"

FILE* file_open(const char *path)
{
    return fopen(path, "r");
}

