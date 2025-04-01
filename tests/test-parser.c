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
     test_file_handle = prepare_test_file(
        TEST_FILE_NAME, 
        "class Bar {\n"
        "  function void foo(int arg1, int arg2) {\n"
        "  }\n"
        "  constructor Bar new(int age, boolean some_flag) {\n"
        "  }\n"
        "  method int test(char arg1) {\n"
        "  }\n"
        "}"
    );

    Parser_class_dec class = parser_parse(test_file_handle).class_dec;

    tst_true(class.funcs_count == 3);

    tst_true(class.funcs[0].scope == PARSER_FUNC_STATIC);
    tst_true(strcmp(class.funcs[0].type_name, "void") == 0);
    tst_true(strcmp(class.funcs[0].name, "foo") == 0);
    tst_true(class.funcs[0].params_count == 2);
    tst_true(strcmp(class.funcs[0].params[0].type_name, "int") == 0);
    tst_true(strcmp(class.funcs[0].params[0].name, "arg1") == 0);
    tst_true(strcmp(class.funcs[0].params[1].type_name, "int") == 0);
    tst_true(strcmp(class.funcs[0].params[1].name, "arg2") == 0);
   
    tst_true(class.funcs[1].scope == PARSER_FUNC_CONSTRUCTOR);
    tst_true(strcmp(class.funcs[1].type_name, "Bar") == 0);
    tst_true(strcmp(class.funcs[1].name, "new") == 0);
    tst_true(class.funcs[1].params_count == 2);
    tst_true(strcmp(class.funcs[1].params[0].type_name, "int") == 0);
    tst_true(strcmp(class.funcs[1].params[0].name, "age") == 0);
    tst_true(strcmp(class.funcs[1].params[1].type_name, "boolean") == 0);
    tst_true(strcmp(class.funcs[1].params[1].name, "some_flag") == 0);

    tst_true(class.funcs[2].scope == PARSER_FUNC_METHOD);
    tst_true(strcmp(class.funcs[2].type_name, "int") == 0);
    tst_true(strcmp(class.funcs[2].name, "test") == 0);
    tst_true(class.funcs[2].params_count == 1);
    tst_true(strcmp(class.funcs[2].params[0].type_name, "char") == 0);
    tst_true(strcmp(class.funcs[2].params[0].name, "arg1") == 0);

    erase_test_file(test_file_handle, TEST_FILE_NAME);
}

void test_parsing_func_body()
{

}

void test_parsing_expr()
{

}

