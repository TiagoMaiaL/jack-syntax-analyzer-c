#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static Tokenizer_atom current_atom;

static Parser_class_dec parse_class_dec();
static void parse_class_vars_dec(Parser_class_dec *class);
static void parse_subroutines(Parser_class_dec *class);
static void parse_params_list(Parser_subroutine_dec *subroutine);
static void parse_var_decs(Parser_subroutine_dec *subroutine);
static void parse_statements(Parser_subroutine_dec *subroutine);
static void parse_do(Parser_subroutine_dec *subroutine);
static void parse_let();
static void parse_while();
static void parse_return();
static void parse_if();
static Parser_statement make_empty_statement();
static void parse_expression_list();
static void parse_expression();
static void parse_term();
static void parse_subroutine_call(Parser_do_statement *do_statement);
static Parser_expression make_empty_expression();
static Parser_term *make_empty_term();
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
    class_dec.vars = ll_make_empty_list();
    class_dec.subroutines = ll_make_empty_list();

    consume_atom(); 
    expect(current_atom.type == TK_TYPE_IDENTIFIER, "Class name expected"); 
    class_dec.name = current_atom.value;

    consume_atom();
    expect(current_atom.symbol == TK_SYMBOL_L_CURLY, "'{' symbol expected");
    free(current_atom.value);
    
    parse_class_vars_dec(&class_dec);
    parse_subroutines(&class_dec);

    consume_atom();
    expect(current_atom.symbol == TK_SYMBOL_R_CURLY, "'}' symbol expected");
    free(current_atom.value);

    return class_dec;
}

static void parse_class_vars_dec(Parser_class_dec *class)
{
    bool has_var_decs;

    Tokenizer_atom peak = peak_atom();
    has_var_decs = peak.keyword == TK_KEYWORD_STATIC || 
                   peak.keyword == TK_KEYWORD_FIELD;
    free(peak.value);

    if (!has_var_decs) {
        return;
    }

    Parser_class_var_dec var_dec;
    var_dec.names = ll_make_empty_list();

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

    LL_Node *name_node = ll_make_node(sizeof(char));
    name_node->data = (void *)current_atom.value;
    ll_append(name_node, &var_dec.names);
    
    consume_atom(); 
    while (current_atom.symbol == TK_SYMBOL_COMMA) {
        consume_atom();

        expect(
            current_atom.type == TK_TYPE_IDENTIFIER,
            "Expected variable name in declaration"
        );

        name_node = ll_make_node(sizeof(char));
        name_node->data = (void *)current_atom.value;
        ll_append(name_node, &var_dec.names);

        consume_atom();
        free(current_atom.value);
    }

    expect(
        current_atom.symbol == TK_SYMBOL_SEMICOLON,
        "Expected ';' at end of variable declaration."
    );

    LL_Node *var_node = ll_make_node(sizeof(Parser_class_var_dec));
    *(Parser_class_var_dec *)var_node->data = var_dec;
    ll_append(var_node, &class->vars);

    parse_class_vars_dec(class);
}

static void parse_subroutines(Parser_class_dec *class)
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
    subroutine.params = ll_make_empty_list();
    subroutine.vars = ll_make_empty_list();
    subroutine.statements = ll_make_empty_list();

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

    parse_var_decs(&subroutine);
    parse_statements(&subroutine);

    consume_atom();
    expect(
        current_atom.symbol == TK_SYMBOL_R_CURLY,
        "Expected right curly brace '}' at end of "
        "subroutine's body declaration."
    );
    free(current_atom.value);

    LL_Node *node = ll_make_node(sizeof(Parser_subroutine_dec));
    *(Parser_subroutine_dec *)node->data = subroutine;
    ll_append(node, &class->subroutines);

    parse_subroutines(class);
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
        
        LL_Node *param_node = ll_make_node(sizeof(Parser_param));
        *(Parser_param *)param_node->data = param;
        ll_append(param_node, &subroutine->params);

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

static void parse_var_decs(Parser_subroutine_dec *subroutine)
{
    Tokenizer_atom peak = peak_atom();
    free(peak.value);

    if (peak.keyword != TK_KEYWORD_VAR) {
        return;
    }

    consume_atom();
    free(current_atom.value);
    
    Parser_var_dec var;
    var.names = ll_make_empty_list();

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
        LL_Node *name_node = ll_make_node(sizeof(char));
        name_node->data = (void *)current_atom.value;
        ll_append(name_node, &var.names);

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

    LL_Node *var_node = ll_make_node(sizeof(Parser_var_dec));
    *(Parser_var_dec *)var_node->data = var;
    ll_append(var_node, &subroutine->vars);

    parse_var_decs(subroutine);
}

static void parse_statements(Parser_subroutine_dec *subroutine)
{
    Tokenizer_atom peak = peak_atom();
    free(peak.value);

    if (peak.keyword == TK_KEYWORD_LET) {
    
    } else if (peak.keyword == TK_KEYWORD_IF) {

    } else if (peak.keyword == TK_KEYWORD_ELSE) {

    } else if (peak.keyword == TK_KEYWORD_WHILE) {

    } else if (peak.keyword == TK_KEYWORD_DO) {
        parse_do(subroutine);

    } else if (peak.keyword == TK_KEYWORD_RETURN) {


    } else {
        return;
    }

    parse_statements(subroutine);
}

static void parse_do(Parser_subroutine_dec *subroutine)
{
    consume_atom();
    free(current_atom.value);
    expect(
        current_atom.keyword == TK_KEYWORD_DO,
        "Expected 'do' keyword at beginning of statement"
    );

    Parser_do_statement *do_statement = malloc(sizeof(Parser_do_statement));
    parse_subroutine_call(do_statement);

    Parser_statement statement = make_empty_statement();
    statement.do_statement = do_statement;

    consume_atom();
    free(current_atom.value);
    expect(
        current_atom.symbol == TK_SYMBOL_SEMICOLON,
        "Expected ';' at end of statement."
    );

    LL_Node *statement_node = ll_make_node(sizeof(Parser_statement));
    *(Parser_statement *)statement_node->data = statement;
    ll_append(statement_node, &subroutine->statements);
}

static void parse_subroutine_call(Parser_do_statement *do_statement)
{
    char *instance_var_name = NULL;
    char *subroutine_name = NULL;

    consume_atom();
    expect(
        current_atom.type == TK_TYPE_IDENTIFIER,
        "Expected name of subroutine or "
        "instance in function call"
    );

    Tokenizer_atom peak = peak_atom();
    free(peak.value);

    if (peak.symbol == TK_SYMBOL_DOT) {
        instance_var_name = current_atom.value;

        consume_atom();
        free(current_atom.value);

        consume_atom();
        expect(
            current_atom.type == TK_TYPE_IDENTIFIER,
            "Expected name of method in "
            "method call"
        );

        subroutine_name = current_atom.value;

    } else {
        subroutine_name = current_atom.value;
    }

    consume_atom();
    free(current_atom.value);
    expect(
        current_atom.symbol == TK_SYMBOL_L_PAREN,
        "Expected '(' in subroutine call"
    );

    LL_List expressions = ll_make_empty_list();
    //TODO: parse_expressions_list(&expressions);

    consume_atom();
    free(current_atom.value);
    expect(
        current_atom.symbol == TK_SYMBOL_R_PAREN,
        "Expected ')' in subroutine call"
    );

    Parser_term_subroutine_call subroutine_call;
    subroutine_call.instance_var_name = instance_var_name;
    subroutine_call.subroutine_name = subroutine_name;
    subroutine_call.param_expressions = expressions;

    do_statement->subroutine_call = subroutine_call;
}

static Parser_statement make_empty_statement()
{
    Parser_statement statement;
    statement.do_statement = NULL;
    statement.let_statement = NULL;
    statement.if_statement = NULL;
    statement.while_statement = NULL;
    statement.return_statement = NULL;
    return statement;
}

static Parser_expression make_empty_expression()
{
    Parser_expression expression;
    expression.term = NULL;
    expression.op = PARSER_TERM_OP_UNDEFINED;
    expression.right_term = NULL;
    return expression;
}

static Parser_term *make_empty_term()
{
    Parser_term *term = malloc(sizeof(Parser_term));
    term->integer = NULL;
    term->string = NULL;
    term->keyword_value = PARSER_TERM_KEYWORD_UNDEFINED;
    term->var_usage = NULL;
    term->subroutine_call = NULL;
    term->parenthesized_expression = NULL;
    term->sub_term = NULL;
    return term;
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

