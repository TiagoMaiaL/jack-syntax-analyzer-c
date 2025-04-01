#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "test-parser.h"

#define TEST_FILE_NAME "parser_test_file.jack"

static FILE *test_file_handle = NULL;

static void prepare_test_file(const char *file_content);
static void erase_test_file();

void test_parser()
{
    tst_suite_begin("Parser");

    //tst_unit("Empty class", );

    tst_suite_finish();
}

// TODO: Add a testing utils file with the functions below

static void prepare_test_file(const char *file_content)
{
    test_file_handle = fopen(TEST_FILE_NAME, "wa");

    if (fputs(file_content, test_file_handle) == EOF) {
        abort();
    }
    fclose(test_file_handle);

    fopen(TEST_FILE_NAME, "r");
}

static void erase_test_file()
{
    if (test_file_handle != NULL) {
        fclose(test_file_handle);
    }
    remove(TEST_FILE_NAME);
}

