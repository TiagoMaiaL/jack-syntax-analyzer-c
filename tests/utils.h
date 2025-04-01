#ifndef TEST_UTILS
#define TEST_UTILS

#include <stdio.h>
#include <stdlib.h>

FILE *prepare_test_file(const char *file_name, const char *file_content);
void erase_test_file(FILE *file_handle, const char *file_name);

#endif
