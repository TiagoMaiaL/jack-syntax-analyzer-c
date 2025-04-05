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

    tst_unit("Empty class", test_parsing_empty_class);
    tst_unit("Class with vars", test_parsing_class_with_vars);
    tst_unit("Class with empty funcs", test_parsing_class_with_empty_funcs);
    tst_unit("Func body", test_parsing_func_body);
    tst_unit("Expressions", test_parsing_expr);

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
    tst_true(class.vars.count == 0);
    tst_true(class.subroutines_count == 0);

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

    LL_Node *node;
    Parser_class_var_dec var;
    Parser_class_dec class = parser_parse(test_file_handle).class_dec;

    node = class.vars.head;
    var = *(Parser_class_var_dec *)node->data;

    tst_true(class.vars.count == 2);

    tst_true(strcmp(var.type_name, "int") == 0);
    tst_true(strcmp(var.vars_names[0], "count") == 0);
    tst_true(var.vars_count == 1);

    node = node->next;
    var = *(Parser_class_var_dec *)node->data;

    tst_true(strcmp(var.type_name, "bool") == 0);
    tst_true(strcmp(var.vars_names[0], "flag_1") == 0);
    tst_true(strcmp(var.vars_names[1], "flag_2") == 0);
    tst_true(strcmp(var.vars_names[2], "flag_3") == 0);
    tst_true(var.vars_count == 3);

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

    tst_true(class.subroutines_count == 3);

    tst_true(class.subroutines[0].scope == PARSER_FUNC_STATIC);
    tst_true(strcmp(class.subroutines[0].type_name, "void") == 0);
    tst_true(strcmp(class.subroutines[0].name, "foo") == 0);
    tst_true(class.subroutines[0].params_count == 2);
    tst_true(strcmp(class.subroutines[0].params[0].type_name, "int") == 0);
    tst_true(strcmp(class.subroutines[0].params[0].name, "arg1") == 0);
    tst_true(strcmp(class.subroutines[0].params[1].type_name, "int") == 0);
    tst_true(strcmp(class.subroutines[0].params[1].name, "arg2") == 0);
   
    tst_true(class.subroutines[1].scope == PARSER_FUNC_CONSTRUCTOR);
    tst_true(strcmp(class.subroutines[1].type_name, "Bar") == 0);
    tst_true(strcmp(class.subroutines[1].name, "new") == 0);
    tst_true(class.subroutines[1].params_count == 2);
    tst_true(strcmp(class.subroutines[1].params[0].type_name, "int") == 0);
    tst_true(strcmp(class.subroutines[1].params[0].name, "age") == 0);
    tst_true(strcmp(class.subroutines[1].params[1].type_name, "boolean") == 0);
    tst_true(strcmp(class.subroutines[1].params[1].name, "some_flag") == 0);

    tst_true(class.subroutines[2].scope == PARSER_FUNC_METHOD);
    tst_true(strcmp(class.subroutines[2].type_name, "int") == 0);
    tst_true(strcmp(class.subroutines[2].name, "test") == 0);
    tst_true(class.subroutines[2].params_count == 1);
    tst_true(strcmp(class.subroutines[2].params[0].type_name, "char") == 0);
    tst_true(strcmp(class.subroutines[2].params[0].name, "arg1") == 0);

    erase_test_file(test_file_handle, TEST_FILE_NAME);
}

void test_parsing_func_body()
{
     test_file_handle = prepare_test_file(
        TEST_FILE_NAME, 
        "class Bar {\n"
        "  method int test(char arg1) {\n"
        "    var int someInt;\n"
        "    var char char1, char_2, char__3;\n"
        "  }\n"
        "}"
    );

    Parser_class_dec class = parser_parse(test_file_handle).class_dec;
    Parser_subroutine_dec subroutine = class.subroutines[0];

    tst_true(subroutine.vars_count == 2);

    Parser_var_dec someInt = subroutine.vars[0];
    tst_true(strcmp(someInt.type_name, "int") == 0);
    tst_true(someInt.vars_count == 1);
    tst_true(strcmp(someInt.vars_names[0], "someInt") == 0);
    
    Parser_var_dec chars_listed = subroutine.vars[1];
    tst_true(strcmp(chars_listed.type_name, "char") == 0);
    tst_true(chars_listed.vars_count == 3);
    tst_true(strcmp(chars_listed.vars_names[0], "char1") == 0);
    tst_true(strcmp(chars_listed.vars_names[1], "char_2") == 0);
    tst_true(strcmp(chars_listed.vars_names[2], "char__3") == 0);

    erase_test_file(test_file_handle, TEST_FILE_NAME);
}

void test_parsing_expr()
{

}

