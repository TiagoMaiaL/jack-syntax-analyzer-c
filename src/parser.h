#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdio.h>
#include "linked-list.h"
#include "tokenizer.h"

typedef enum {
    PARSER_VAR_STATIC,
    PARSER_VAR_FIELD,
} Parser_class_var_scope;

typedef struct {
    Parser_class_var_scope scope;
    char *type_name;
    LL_List names;
} Parser_class_var_dec;

typedef struct {
    char *type_name;
    LL_List names;
} Parser_var_dec;

typedef struct {
    char *type_name;
    char *name;
} Parser_param;

typedef enum {
    PARSER_FUNC_STATIC,
    PARSER_FUNC_CONSTRUCTOR,
    PARSER_FUNC_METHOD
} Parser_subroutine_scope;

typedef struct {
    Parser_subroutine_scope scope;
    char *type_name;
    char *name;
    LL_List params;
    LL_List vars;
    LL_List statements;
} Parser_subroutine_dec;

typedef struct {
    char *name;
    LL_List vars;
    LL_List subroutines;
} Parser_class_dec;

typedef struct {
    Parser_class_dec class_dec;
} Parser_jack_syntax; 

typedef struct Parser_expression Parser_expression;
typedef struct Parser_term Parser_term;
typedef struct Parser_sub_term Parser_sub_term;

typedef enum {
    PARSER_TERM_OP_ADDITION,
    PARSER_TERM_OP_SUBTRACTION,
    PARSER_TERM_OP_MULTIPLICATION,
    PARSER_TERM_OP_DIVISION,
    PARSER_TERM_OP_AND,
    PARSER_TERM_OP_OR,
    PARSER_TERM_OP_LESSER,
    PARSER_TERM_OP_GREATER,
    PARSER_TERM_OP_ASSIGN,
    PARSER_TERM_OP_NOT,
    PARSER_TERM_OP_UNDEFINED
} Parser_term_operator;

typedef enum {
    PARSER_TERM_KEYWORD_TRUE,
    PARSER_TERM_KEYWORD_FALSE,
    PARSER_TERM_KEYWORD_NULL,
    PARSER_TERM_KEYWORD_THIS,
    PARSER_TERM_KEYWORD_UNDEFINED
} Parser_term_keyword_constant;

typedef struct {
    char *var_name;
    Parser_expression *subscript;
} Parser_term_var_usage;

typedef struct {
    char *instance_var_name;
    char *subroutine_name;
    LL_List param_expressions;
} Parser_term_subroutine_call;

struct Parser_term {
    char *integer;
    char *string;
    Parser_term_keyword_constant keyword_value;
    Parser_term_var_usage *var_usage;
    Parser_term_subroutine_call *subroutine_call;
    Parser_expression *parenthesized_expression;
    Parser_sub_term *sub_term;
};

struct Parser_sub_term {
    Parser_term_operator unary_op;
    Parser_term term;
};

struct Parser_expression {
    LL_List terms;
    LL_List operators;
};

typedef struct {
    Parser_term_subroutine_call subroutine_call;
} Parser_do_statement;

typedef struct {
    char *var_name;
    bool has_subscript;
    Parser_expression subscript;
    Parser_expression value;
} Parser_let_statement;

typedef struct {
    Parser_expression conditional;
    LL_List conditional_statements;
    bool has_else;
    LL_List else_statements;
} Parser_if_statement;

typedef struct {
    Parser_expression conditional;
    LL_List statements;
} Parser_while_statement;

typedef struct {
    bool has_expr;
    Parser_expression expression;
} Parser_return_statement;

typedef struct {
    Parser_do_statement *do_statement;
    Parser_let_statement *let_statement;
    Parser_if_statement *if_statement;
    Parser_while_statement *while_statement;
    Parser_return_statement *return_statement;
} Parser_statement;

Parser_jack_syntax parser_parse(FILE *source);
char *parser_unique_var_key(
    const char *class_name, 
    const char *func_name, 
    const char *name
);
void parser_free(Parser_jack_syntax ast);

#endif
