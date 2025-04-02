#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static Tokenizer_atom current_atom;

static Parser_class_dec parse_class_dec();
static void parse_class_vars_dec(Parser_class_dec *class, short var_i);
static void parse_subroutines(Parser_class_dec *class, short func_i);
static void parse_params_list(Parser_subroutine_dec *subroutine);
static void parse_var_decs(Parser_subroutine_dec *subroutine, short var_i);
static void parse_statements(Parser_subroutine_dec *subroutine);
static void parse_do();
static void parse_let();
static void parse_while();
static void parse_return();
static void parse_if();
static void parse_expression();
static void parse_term();
static void parse_expression_list();
static Tokenizer_atom consume_atom();
static Tokenizer_atom peak_atom();
static void expect(bool expression, char *failure_msg);
static bool is_type(Tokenizer_atom atom);
static void exit_parsing(char *msg);

Parser_jack_syntax parser_parse(FILE *source) {
    tokenizer_start(source);

    Parser_jack_syntax jack_syntax;
    jack_syntax.class_dec = parse_class_dec();

    return jack_syntax;
}

static Parser_class_dec parse_class_dec()
{
    consume_atom();
    expect(current_atom.keyword == TK_KEYWORD_CLASS, "'class' keyword expected");
    free(current_atom.value);

    Parser_class_dec class_dec;
    class_dec.vars_count = 0;
    class_dec.subroutines_count = 0; 

    consume_atom(); 
    expect(current_atom.type == TK_TYPE_IDENTIFIER, "Class name expected"); 
    class_dec.name = current_atom.value;

    consume_atom();
    expect(current_atom.symbol == TK_SYMBOL_L_CURLY, "'{' symbol expected");
    free(current_atom.value);
    
    parse_class_vars_dec(&class_dec, 0);
    parse_subroutines(&class_dec, 0);

    consume_atom();
    expect(current_atom.symbol == TK_SYMBOL_R_CURLY, "'}' symbol expected");
    free(current_atom.value);

    return class_dec;
}

static void parse_class_vars_dec(Parser_class_dec *class, short var_i)
{
    // TODO: Unit test tokenizer peaking.
    bool has_var_decs;

    Tokenizer_atom peak = peak_atom();
    has_var_decs = peak.keyword == TK_KEYWORD_STATIC || 
                   peak.keyword == TK_KEYWORD_FIELD;
    free(peak.value);

    if (!has_var_decs) {
        return;
    }

    Parser_class_var_dec var_dec;
    var_dec.vars_count = 0;

    consume_atom();
    if (current_atom.keyword == TK_KEYWORD_STATIC) {
        var_dec.scope = PARSER_VAR_STATIC;
    } else if (current_atom.keyword == TK_KEYWORD_FIELD) {
        var_dec.scope = PARSER_VAR_FIELD;
    } else {
        exit_parsing("Expected a valid scope for the variable declaration");
    }
    free(current_atom.value);

    consume_atom();
    expect(
        is_type(current_atom),
        "Expected type in variable declaration"
    );
    var_dec.type_name = current_atom.value;

    consume_atom();
    expect(
        current_atom.value != NULL && 
        current_atom.type == TK_TYPE_IDENTIFIER,
        "Expected variable name in declaration"
    );
    var_dec.vars_names[0] = current_atom.value;
    var_dec.vars_count++;
    
    consume_atom(); 
    while (current_atom.symbol == TK_SYMBOL_COMMA) {
        consume_atom();

        expect(
            current_atom.type == TK_TYPE_IDENTIFIER,
            "Expected variable name in declaration"
        );
        var_dec.vars_names[var_dec.vars_count] = current_atom.value;
        var_dec.vars_count++;

        consume_atom();
        free(current_atom.value);
    }

    expect(
        current_atom.symbol == TK_SYMBOL_SEMICOLON,
        "Expected ';' at end of variable declaration."
    );

    class->vars[var_i] = var_dec;
    var_i++;
    class->vars_count = var_i;

    parse_class_vars_dec(class, var_i);
}

static void parse_subroutines(Parser_class_dec *class, short func_i)
{
    bool has_func_decs;

    Tokenizer_atom peak = peak_atom();
    has_func_decs = peak.keyword == TK_KEYWORD_FUNCTION || 
                    peak.keyword == TK_KEYWORD_CONSTRUCTOR ||
                    peak.keyword == TK_KEYWORD_METHOD;
    free(peak.value);

    if (!has_func_decs) {
        return;
    }

    Parser_subroutine_dec subroutine;
    subroutine.params_count = 0;
    subroutine.vars_count = 0;

    consume_atom();
    if (current_atom.keyword == TK_KEYWORD_FUNCTION) {
        subroutine.scope = PARSER_FUNC_STATIC;
    } else if (current_atom.keyword == TK_KEYWORD_CONSTRUCTOR) {
        subroutine.scope = PARSER_FUNC_CONSTRUCTOR;
    } else if (current_atom.keyword == TK_KEYWORD_METHOD) {
        subroutine.scope = PARSER_FUNC_METHOD;
    } else {
        exit_parsing("Undefined scope for function declaration");
    }
    free(current_atom.value);

    consume_atom();
    expect(
        is_type(current_atom),
        "Expected return type in subroutine declaration"
    );
    subroutine.type_name = current_atom.value;

    consume_atom();
    expect(
        current_atom.type == TK_TYPE_IDENTIFIER,
        "Expected subroutine name in declaration"
    );
    subroutine.name = current_atom.value; 

    parse_params_list(&subroutine);

    consume_atom();
    expect(
        current_atom.symbol == TK_SYMBOL_L_CURLY,
        "Expected left curly brace '{' at beginning of "
        "subroutine's body declaration."
    );
    free(current_atom.value);

    parse_var_decs(&subroutine, 0);
    parse_statements(&subroutine);

    consume_atom();
    expect(
        current_atom.symbol == TK_SYMBOL_R_CURLY,
        "Expected right curly brace '}' at end of "
        "subroutine's body declaration."
    );
    free(current_atom.value);

    class->subroutines[func_i] = subroutine;
    func_i++;
    class->subroutines_count = func_i;
    
    parse_subroutines(class, func_i);
}

static void parse_params_list(Parser_subroutine_dec *subroutine)
{
    consume_atom();
    expect(
        current_atom.symbol == TK_SYMBOL_L_PAREN,
        "Expected opening parenthesis for parameter list " 
        "'(' in subroutine declaration"
    );
    free(current_atom.value);

    consume_atom();
    while (is_type(current_atom)) {
        Parser_param param;
        param.type_name = current_atom.value;

        consume_atom();
        expect(
            current_atom.type == TK_TYPE_IDENTIFIER,
            "Expected parameter name in function declaration"
        );
        param.name = current_atom.value;

        subroutine->params[subroutine->params_count] = param;
        subroutine->params_count++;

        consume_atom();
        if (current_atom.symbol == TK_SYMBOL_COMMA) {
            free(current_atom.value);
            consume_atom();
        }
    }

    expect(
        current_atom.symbol == TK_SYMBOL_R_PAREN,
        "Expected closing parenthesis ')' at "
        "end of parameter list in subroutine declaration"
    );
    free(current_atom.value);
}

static void parse_var_decs(Parser_subroutine_dec *subroutine, short var_i)
{
    Tokenizer_atom peak = peak_atom();
    free(peak.value);

    if (peak.keyword != TK_KEYWORD_VAR) {
        return;
    }

    consume_atom();
    free(current_atom.value);
    
    Parser_var_dec var;
    var.vars_count = 0;

    consume_atom();
    expect(
        is_type(current_atom) && 
        current_atom.keyword != TK_KEYWORD_VOID,
        "Expected variable type in declaration"
    );
    var.type_name = current_atom.value;

    consume_atom();
    expect(
        current_atom.type == TK_TYPE_IDENTIFIER,
        "Expected variable name in declaration"
    );
    while (current_atom.type == TK_TYPE_IDENTIFIER) {
        var.vars_names[var.vars_count] = current_atom.value;
        var.vars_count++;

        consume_atom();

        if (current_atom.symbol == TK_SYMBOL_COMMA) {
            free(current_atom.value);
            consume_atom();
        }
    }

    expect(
        current_atom.symbol == TK_SYMBOL_SEMICOLON,
        "Expected semicolon ';' at end of variable declaration"
    );
    free(current_atom.value);

    subroutine->vars_count++;
    subroutine->vars[var_i] = var;
    var_i++;

    parse_var_decs(subroutine, var_i);
}

static void parse_statements(Parser_subroutine_dec *subroutine)
{

}

static Tokenizer_atom consume_atom()
{
    Tokenizer_atom atom;
    bool should_skip = true;

    while(should_skip) {
        atom = tokenizer_next();
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

static Tokenizer_atom peak_atom()
{
    Tokenizer_atom atom;
    bool should_skip = true;

    while(should_skip) {
        atom = tokenizer_peak();
        should_skip = atom.type == TK_TYPE_COMMENT || 
                      atom.type == TK_TYPE_WHITESPACE;
        if (should_skip)
            tokenizer_next();
    }

    return atom;
}

#define EXPECT_FAIL_MSG "Unexpected token"

static void expect(bool expression, char *failure_msg)
{
    int len = strlen(EXPECT_FAIL_MSG) + 1; // "%s "
    len += strlen(current_atom.value) + 3; // "'%s'."
    len += strlen(failure_msg) + 1;        // "' %s\n'"
    len += 1; // '\0'

    char error_output[len];
    error_output[0] = '\0';

    sprintf(
        error_output, 
        "%s '%s'. %s\n", 
        EXPECT_FAIL_MSG, 
        current_atom.value, 
        failure_msg
    );

    if (!expression) {
        exit_parsing(error_output);
    }
}

static bool is_type(Tokenizer_atom atom)
{
    return atom.value != NULL && (
        atom.keyword == TK_KEYWORD_INT ||
        atom.keyword == TK_KEYWORD_CHAR ||
        atom.keyword == TK_KEYWORD_BOOLEAN ||
        atom.keyword == TK_KEYWORD_VOID ||
        atom.type == TK_TYPE_IDENTIFIER
    );
}

static void exit_parsing(char *msg)
{
    printf(
        "Line %d, column %d\n", 
        tokenizer_get_line(), 
        tokenizer_get_column()
    );
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

