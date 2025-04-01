#include <stdlib.h>
#include "utils.h"

FILE *prepare_test_file(const char *file_name, const char *file_content)
{
    FILE *file_handle = fopen(file_name, "wa");

    if (fputs(file_content, file_handle) == EOF) {
        abort();
    }
    fclose(file_handle);

    fopen(file_name, "r");

    return file_handle;
}

void erase_test_file(FILE *file_handle, const char *file_name)
{
    if (file_handle != NULL) {
        fclose(file_handle);
    }
    remove(file_name);
}

