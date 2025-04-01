#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "test-parser.h"
#include "utils.h"
#include "../src/parser.h"

#define TEST_FILE_NAME "parser_test_file.jack"

static FILE *test_file_handle = NULL;

void test_parsing_empty_class();
void test_parsing_class_with_vars();
void test_parsing_class_with_empty_funcs();
void test_parsing_func_body();
void test_parsing_expr();

void test_parser()
{
    tst_suite_begin("Parser");

    tst_unit("Empty class", *test_parsing_empty_class);
    tst_unit("Class with vars", *test_parsing_class_with_vars);
    tst_unit("Class with empty funcs", *test_parsing_class_with_empty_funcs);
    tst_unit("Func body", *test_parsing_func_body);
    tst_unit("Expressions", *test_parsing_expr);

    tst_suite_finish();
}

void test_parsing_empty_class()
{
    test_file_handle = prepare_test_file(
        TEST_FILE_NAME, 
        "class Main {}"
    );

    Parser_class_dec class = parser_parse(test_file_handle).class_dec;
    tst_true(strcmp(class.name, "Main") == 0);
    tst_true(class.vars_count == 0);
    tst_true(class.funcs_count == 0);

    erase_test_file(test_file_handle, TEST_FILE_NAME);
}

void test_parsing_class_with_vars()
{

}

void test_parsing_class_with_empty_funcs()
{

}

void test_parsing_func_body()
{

}

void test_parsing_expr()
{

}

