#include <stdio.h>
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
    short vars_count;
    char* vars_names[PARSER_MAX_VARS_LISTED];
} Parser_class_var_dec;

typedef struct {
    char *type_name;
    short vars_count;
    char* vars_names[PARSER_MAX_VARS_LISTED];
} Parser_var_dec;

typedef struct {
    char *type_name;
    char *name;
} Parser_param;

typedef enum {
    PARSER_FUNC_STATIC,
    PARSER_FUNC_CONSTRUCTOR,
    PARSER_FUNC_METHOD
} Parser_func_scope; // TODO: rename to Parser_subroutine_scope.

typedef struct {
    Parser_func_scope scope;
    char *type_name;
    char *name;
    short params_count;
    Parser_param params[PARSER_MAX_PARAMS];
    short vars_count;
    Parser_var_dec vars[PARSER_MAX_VARS];
} Parser_subroutine_dec;

typedef struct {
    char *name;
    short vars_count;
    Parser_class_var_dec vars[PARSER_MAX_C_VARS];
    short subroutines_count;
    Parser_subroutine_dec subroutines[PARSER_MAX_C_FUNCS];
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
