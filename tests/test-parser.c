#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "test-parser.h"
#include "utils.h"

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

