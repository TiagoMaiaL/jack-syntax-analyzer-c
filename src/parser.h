#include <stdio.h>
#include "tokenizer.h"

typedef struct {
    Tokenizer_atom identifier;
} Parser_class_dec;

typedef struct {
    Parser_class_dec class_dec;
} Parser_jack_syntax; 

typedef struct {

} Parser_class_var_dec;

typedef struct {

} Parser_subroutine_dec;

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

