#include <stdio.h>
#include "linked-list.h"
#include "tokenizer.h"

// TODO: Create a dynamic array.
#define PARSER_MAX_C_VARS       50
#define PARSER_MAX_C_FUNCS      50
#define PARSER_MAX_VARS_LISTED  10
#define PARSER_MAX_PARAMS       10
#define PARSER_MAX_STATEMENTS   50
#define PARSER_MAX_VARS         50

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
} Parser_subroutine_dec;

typedef struct {
    char *name;
    LL_List vars;
    LL_List subroutines;
} Parser_class_dec;

typedef struct {
    Parser_class_dec class_dec;
} Parser_jack_syntax; 

typedef struct {
    // TODO: Add property for operator
    // TODO: Add property for linked list of terms 
} Parser_term;

typedef struct {
    // TODO: Add main properties
    // term
    Parser_term *term;
} Parser_expression;

typedef struct {
    
} Parser_do_statement;

typedef struct {

} Parser_let_statement;

typedef struct {

} Parser_while_statement;

typedef struct {

} Parser_return_statement;

typedef struct {

} Parser_if_statement;

typedef struct {
    // TODO: add properties
    // do_statement
    // let_statement
    // if_statement
    // while_statement
    // return statement
} Parser_statement;

Parser_jack_syntax parser_parse(FILE *source);
