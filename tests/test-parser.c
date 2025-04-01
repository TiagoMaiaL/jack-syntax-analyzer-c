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
    test_file_handle = prepare_test_file(
        TEST_FILE_NAME, 
        "class Main {\n"
        "  static int count;\n"
        "  field bool flag_1, flag_2, flag_3;\n"
        "}"
    );

    Parser_class_dec class = parser_parse(test_file_handle).class_dec;

    tst_true(class.vars_count == 2);
    tst_true(strcmp(class.vars[0].type_name, "int") == 0);
    tst_true(strcmp(class.vars[0].vars_names[0], "count") == 0);
    tst_true(class.vars[0].vars_count == 1);

    tst_true(strcmp(class.vars[1].type_name, "bool") == 0);
    tst_true(strcmp(class.vars[1].vars_names[0], "flag_1") == 0);
    tst_true(strcmp(class.vars[1].vars_names[1], "flag_2") == 0);
    tst_true(strcmp(class.vars[1].vars_names[2], "flag_3") == 0);
    tst_true(class.vars[1].vars_count == 3);

    erase_test_file(test_file_handle, TEST_FILE_NAME);
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

