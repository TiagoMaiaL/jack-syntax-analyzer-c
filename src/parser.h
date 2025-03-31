#include <stdio.h>
#include "tokenizer.h"

#define PARSER_MAX_C_VARS 100
#define PARSER_MAX_C_FUNCS 100

typedef enum {
    PARSER_VAR_STATIC,
    PARSER_VAR_FIELD,
    PARSER_VAR_LOCAL
} Parser_var_scope;

typedef struct {
    Parser_var_scope scope;
    char *type_name;
    char *name; // TODO: Allow for multiple identifiers.
} Parser_class_var_dec;

typedef struct {

} Parser_subroutine_dec;

typedef struct {
    Tokenizer_atom identifier;
    short var_count;
    Parser_class_var_dec vars[PARSER_MAX_C_VARS];
    short funcs_count;
    Parser_subroutine_dec funcs[PARSER_MAX_C_FUNCS];
} Parser_class_dec;

typedef struct {
    Parser_class_dec class_dec;
} Parser_jack_syntax; 

typedef struct {

} Parser_params_list;

typedef struct {

} Parser_var_dec;

typedef struct {

} Parser_statement;

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

} Parser_expression;

Parser_jack_syntax parser_parse(FILE *source);

