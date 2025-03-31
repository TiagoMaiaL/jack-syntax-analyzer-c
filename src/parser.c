#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static Tokenizer_atom current_atom;

static Parser_class_dec parse_class_dec();
static void parse_class_body(Parser_class_dec *class);
static void parse_class_var_dec();
static void parse_subroutine();
static void parse_param_list();
static void parse_var_dec();
static void parse_statements();
static void parse_do();
static void parse_let();
static void parse_while();
static void parse_return();
static void parse_if();
static void parse_expression();
static void parse_term();
static void parse_expression_list();
static Tokenizer_atom consume_atom(char *failure_msg);
static void expect(bool expression, char *failure_msg);
static void exit_parsing(char *msg);

Parser_jack_syntax parser_parse(FILE *source) {
    tokenizer_start(source);

    Parser_jack_syntax jack_syntax;
    jack_syntax.class_dec = parse_class_dec();
    return jack_syntax;
}

static Parser_class_dec parse_class_dec()
{
    consume_atom("");
    expect(current_atom.keyword == TK_KEYWORD_CLASS, "'class' keyword expected");

    Parser_class_dec class_dec;

    consume_atom("");
    expect(current_atom.keyword == TK_TYPE_IDENTIFIER, "'class' identifier expected");
    class_dec.identifier = current_atom;
    
    // TODO: Parse class body:
    // static vars
    // static funcs
    // constructor
    // fields
    // methods

    return class_dec;
}

static Tokenizer_atom consume_atom(char *failure_msg)
{
    Tokenizer_atom atom;
    bool should_skip = true;

    while(should_skip) {
        Tokenizer_atom atom = tokenizer_next();
        should_skip = atom.type == TK_TYPE_COMMENT || 
                      atom.type == TK_TYPE_WHITESPACE;
    }
    
    if (atom.type == TK_TYPE_ERROR) {
        exit_parsing("Failure while getting next token from text.");
    }

    if (tokenizer_finished()) {
        exit_parsing("There are no tokens left to be consumed.");
    }

    if (atom.type == TK_TYPE_UNDEFINED) {
        exit_parsing("Unexpected kind of text not allowed.");
    }

    current_atom = atom;
    return current_atom;
}

#define EXPECT_FAIL_MSG     "Unexpected token: "
#define EXPECT_FAIL_MSG_LEN 18

static void expect(bool expression, char *failure_msg)
{
    int strlen = 0;

    for (int i = 0;; i++) {
        if (failure_msg[i] == '\0') {
            break;
        }
    }
    strlen += EXPECT_FAIL_MSG_LEN + 1;

    char error_output[strlen];
    strcat(error_output, EXPECT_FAIL_MSG);
    strcat(error_output, failure_msg);
    error_output[EXPECT_FAIL_MSG_LEN - 1] = '\0';

    if (!expression) {
        exit_parsing(error_output);
    }
}

static void exit_parsing(char *msg)
{
    // TODO: Print line and column numbers.
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

