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
void test_parsing_func_body_with_vars();
void test_parsing_func_body_with_statements();
void test_parsing_expr();

void test_parser()
{
    tst_suite_begin("Parser");

    tst_unit("Empty class", test_parsing_empty_class);
    tst_unit("Class with vars", test_parsing_class_with_vars);
    tst_unit("Class with empty funcs", test_parsing_class_with_empty_funcs);
    tst_unit("Func body with vars", test_parsing_func_body_with_vars);
    tst_unit("Func body with statements", test_parsing_func_body_with_statements);
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
    tst_true(class.subroutines.count == 0);

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
    tst_true(strcmp((char *)var.names.head->data, "count") == 0);
    tst_true(var.names.count == 1);

    node = node->next;
    var = *(Parser_class_var_dec *)node->data;

    tst_true(strcmp(var.type_name, "bool") == 0);
    tst_true(strcmp((char *)var.names.head->data, "flag_1") == 0);
    tst_true(strcmp((char *)var.names.head->next->data, "flag_2") == 0);
    tst_true(strcmp((char *)var.names.tail->data, "flag_3") == 0);
    tst_true(var.names.count == 3);

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
    LL_Node *subroutine_node;
    Parser_subroutine_dec subroutine;
    Parser_param param;

    tst_true(class.subroutines.count == 3);

    subroutine_node = class.subroutines.head;
    subroutine = *(Parser_subroutine_dec *)subroutine_node->data;

    tst_true(subroutine.scope == PARSER_FUNC_STATIC);
    tst_true(strcmp(subroutine.type_name, "void") == 0);
    tst_true(strcmp(subroutine.name, "foo") == 0);
    tst_true(subroutine.params.count == 2);

    param = *(Parser_param *)subroutine.params.head->data;
    tst_true(strcmp(param.type_name, "int") == 0);
    tst_true(strcmp(param.name, "arg1") == 0);

    param = *(Parser_param *)subroutine.params.tail->data;
    tst_true(strcmp(param.type_name, "int") == 0);
    tst_true(strcmp(param.name, "arg2") == 0);

   
    subroutine_node = class.subroutines.head->next;
    subroutine = *(Parser_subroutine_dec *)subroutine_node->data;

    tst_true(subroutine.scope == PARSER_FUNC_CONSTRUCTOR);
    tst_true(strcmp(subroutine.type_name, "Bar") == 0);
    tst_true(strcmp(subroutine.name, "new") == 0);
    tst_true(subroutine.params.count == 2);

    param = *(Parser_param *)subroutine.params.head->data;
    tst_true(strcmp(param.type_name, "int") == 0);
    tst_true(strcmp(param.name, "age") == 0);

    param = *(Parser_param *)subroutine.params.tail->data;
    tst_true(strcmp(param.type_name, "boolean") == 0);
    tst_true(strcmp(param.name, "some_flag") == 0);


    subroutine_node = class.subroutines.tail;
    subroutine = *(Parser_subroutine_dec *)subroutine_node->data;

    tst_true(subroutine.scope == PARSER_FUNC_METHOD);
    tst_true(strcmp(subroutine.type_name, "int") == 0);
    tst_true(strcmp(subroutine.name, "test") == 0);
    tst_true(subroutine.params.count == 1);

    param = *(Parser_param *)subroutine.params.tail->data;
    tst_true(strcmp(param.type_name, "char") == 0);
    tst_true(strcmp(param.name, "arg1") == 0);

    erase_test_file(test_file_handle, TEST_FILE_NAME);
}

void test_parsing_func_body_with_vars()
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
    Parser_subroutine_dec subroutine = *(Parser_subroutine_dec *)class.subroutines.head->data;

    tst_true(subroutine.vars.count == 2);

    Parser_var_dec some_int = *(Parser_var_dec *)subroutine.vars.head->data;
    tst_true(strcmp(some_int.type_name, "int") == 0);
    tst_true(some_int.names.count == 1);
    tst_true(strcmp((char *)some_int.names.head->data, "someInt") == 0);
    
    Parser_var_dec chars_listed = *(Parser_var_dec *)subroutine.vars.tail->data;
    tst_true(strcmp(chars_listed.type_name, "char") == 0);
    tst_true(chars_listed.names.count == 3);
    tst_true(strcmp((char *)chars_listed.names.head->data, "char1") == 0);
    tst_true(strcmp((char *)chars_listed.names.head->next->data, "char_2") == 0);
    tst_true(strcmp((char *)chars_listed.names.tail->data, "char__3") == 0);

    erase_test_file(test_file_handle, TEST_FILE_NAME);
}

void test_parsing_func_body_with_statements()
{
    Parser_class_dec class;
    Parser_subroutine_dec subroutine;

    Parser_statement stmt;
    Parser_do_statement do_stmt;
    Parser_let_statement let_stmt;
    Parser_if_statement if_stmt;
    Parser_while_statement while_stmt;
    Parser_return_statement return_stmt;

    Parser_expression expr;
    Parser_term term;
    Parser_term_subroutine_call call;

    test_file_handle = prepare_test_file(
        TEST_FILE_NAME, 
        "class Bar {\n"
        "  method int test(char arg1) {\n"
        "    do funcCall();\n"
        "    do instance.methodCall();\n"
        "  }\n"
        "}"
    );

    class = parser_parse(test_file_handle).class_dec;
    subroutine = *(Parser_subroutine_dec *)class.subroutines.head->data;

    tst_true(subroutine.statements.count == 2);

    stmt = *(Parser_statement *)subroutine.statements.head->data;
    do_stmt = *stmt.do_statement;
    call = do_stmt.subroutine_call;

    tst_true(call.instance_var_name == NULL);
    tst_true(strcmp(call.subroutine_name, "funcCall") == 0);

    stmt = *(Parser_statement *)subroutine.statements.tail->data;
    do_stmt = *stmt.do_statement;
    call = do_stmt.subroutine_call;

    tst_true(strcmp(call.instance_var_name, "instance") == 0);
    tst_true(strcmp(call.subroutine_name, "methodCall") == 0);
    

    test_file_handle = prepare_test_file(
        TEST_FILE_NAME, 
        "class Foo {\n"
        "  method int some_method() {\n"
        "    let x = func_call();\n"
        "    let y[1324] = call();\n"
        "  }\n"
        "}"
    );

    class = parser_parse(test_file_handle).class_dec;
    subroutine = *(Parser_subroutine_dec *)class.subroutines.head->data;

    stmt = *(Parser_statement *)subroutine.statements.head->data;
    let_stmt = *stmt.let_statement;
    expr = let_stmt.value;
    term = *(Parser_term *)expr.terms.head->data;

    tst_true(subroutine.statements.count == 2);

    tst_true(strcmp(let_stmt.var_name, "x") == 0);
    tst_true(term.subroutine_call != NULL);
    tst_true(term.subroutine_call->instance_var_name == NULL);
    tst_true(strcmp(term.subroutine_call->subroutine_name, "func_call") == 0);


    stmt = *(Parser_statement *)subroutine.statements.tail->data;
    let_stmt = *stmt.let_statement;
    expr = let_stmt.value;
    term = *(Parser_term *)expr.terms.head->data;

    tst_true(strcmp(let_stmt.var_name, "y") == 0);
    tst_true(term.subroutine_call != NULL);
    tst_true(term.subroutine_call->instance_var_name == NULL);
    tst_true(strcmp(term.subroutine_call->subroutine_name, "call") == 0);

    expr = let_stmt.subscript;
    term = *(Parser_term *)expr.terms.head->data;
    tst_true(term.integer != NULL);
    tst_true(strcmp(term.integer, "1324") == 0);


    test_file_handle = prepare_test_file(
        TEST_FILE_NAME, 
        "class Foo {\n"
        "  method int some_method() {\n"
        "    if (true | false & var_name | 123) {\n"
        "      do test.something();\n"
        "    }\n"
        "    return 1000 + 2000 * 12 / 1;\n"
        "  }\n"
        "}"
    );

    class = parser_parse(test_file_handle).class_dec;
    subroutine = *(Parser_subroutine_dec *)class.subroutines.head->data;

    tst_true(subroutine.statements.count == 2);

    stmt = *(Parser_statement *)subroutine.statements.head->data;
    if_stmt = *stmt.if_statement;
    expr = if_stmt.conditional;

    tst_true(expr.terms.count == 4);
    tst_true(expr.operators.count == 3);

    term = *(Parser_term *)expr.terms.head->data;
    tst_true(term.keyword_value == PARSER_TERM_KEYWORD_TRUE);
    term = *(Parser_term *)expr.terms.head->next->data;
    tst_true(term.keyword_value == PARSER_TERM_KEYWORD_FALSE);
    term = *(Parser_term *)expr.terms.head->next->next->data;
    tst_true(strcmp(term.var_usage->var_name, "var_name") == 0);
    term = *(Parser_term *)expr.terms.tail->data;
    tst_true(strcmp(term.integer, "123") == 0);

    Parser_term_operator op = *(Parser_term_operator *)expr.operators.head->data;
    tst_true(op == PARSER_TERM_OP_OR);
    op = *(Parser_term_operator *)expr.operators.head->next->data;
    tst_true(op == PARSER_TERM_OP_AND);
    op = *(Parser_term_operator *)expr.operators.tail->data;
    tst_true(op == PARSER_TERM_OP_OR);

    Parser_statement do_wrapper = *(Parser_statement *)if_stmt.conditional_statements.head->data;
    call = do_wrapper.do_statement->subroutine_call;

    tst_true(strcmp(call.instance_var_name, "test") == 0);
    tst_true(strcmp(call.subroutine_name, "something") == 0);


    stmt = *(Parser_statement *)subroutine.statements.tail->data;

    tst_true(stmt.return_statement != NULL);

    return_stmt = *stmt.return_statement;
    expr = return_stmt.expression;
    
    tst_true(return_stmt.has_expr);
    tst_true(expr.terms.count == 4);
    tst_true(expr.operators.count == 3);

    term = *(Parser_term *)expr.terms.head->data;
    tst_true(strcmp(term.integer, "1000") == 0);
    term = *(Parser_term *)expr.terms.head->next->data;
    tst_true(strcmp(term.integer, "2000") == 0);
    term = *(Parser_term *)expr.terms.head->next->next->data;
    tst_true(strcmp(term.integer, "12") == 0);
    term = *(Parser_term *)expr.terms.tail->data;
    tst_true(strcmp(term.integer, "1") == 0);

    op = *(Parser_term_operator *)expr.operators.head->data;
    tst_true(op == PARSER_TERM_OP_ADDITION);
    op = *(Parser_term_operator *)expr.operators.head->next->data;
    tst_true(op == PARSER_TERM_OP_MULTIPLICATION);
    op = *(Parser_term_operator *)expr.operators.tail->data;
    tst_true(op == PARSER_TERM_OP_DIVISION);

    erase_test_file(test_file_handle, TEST_FILE_NAME);
}

void test_parsing_expr()
{

}

