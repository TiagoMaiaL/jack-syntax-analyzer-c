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
static void parse_let(Parser_subroutine_dec *subroutine);
static void parse_if(Parser_subroutine_dec *subroutine);
static void parse_while(Parser_subroutine_dec *subroutine);
static void parse_return(Parser_subroutine_dec *subroutine);
static Parser_statement make_empty_statement();

static Parser_expression parse_expression();
static void parse_expression_list();
static Parser_term parse_term();
static Parser_term_subroutine_call parse_subroutine_call(char *identifier);
static LL_List parse_expressions_list();
static bool is_expression_keyword(Tokenizer_keyword keyword);
static Parser_term_keyword_constant get_keyword_value(Tokenizer_keyword keyword);
static bool is_operator(Tokenizer_symbol symbol);
static bool is_unary_operator(Tokenizer_symbol symbol);
static Parser_term_var_usage parse_var_usage();
static Parser_term_operator get_operator(Tokenizer_symbol symbol);
static Parser_expression make_empty_expression();
static Parser_term make_empty_term();

static Tokenizer_atom consume_atom();
static Tokenizer_atom peek_atom();
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

    Tokenizer_atom peek = peek_atom();
    has_var_decs = peek.keyword == TK_KEYWORD_STATIC || 
                   peek.keyword == TK_KEYWORD_FIELD;
    free(peek.value);

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

    Tokenizer_atom peek = peek_atom();
    has_func_decs = peek.keyword == TK_KEYWORD_FUNCTION || 
                    peek.keyword == TK_KEYWORD_CONSTRUCTOR ||
                    peek.keyword == TK_KEYWORD_METHOD;
    free(peek.value);

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
    Tokenizer_atom peek = peek_atom();
    free(peek.value);

    if (peek.keyword != TK_KEYWORD_VAR) {
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
    Tokenizer_atom peek = peek_atom();
    free(peek.value);

    if (peek.keyword == TK_KEYWORD_LET) {
        parse_let(subroutine);    

    } else if (peek.keyword == TK_KEYWORD_IF) {

    } else if (peek.keyword == TK_KEYWORD_WHILE) {

    } else if (peek.keyword == TK_KEYWORD_DO) {
        parse_do(subroutine);

    } else if (peek.keyword == TK_KEYWORD_RETURN) {
        parse_return(subroutine);

    } else {
        return;
    }

    parse_statements(subroutine);
}

static void parse_let(Parser_subroutine_dec *subroutine)
{
    Parser_let_statement let_stmt;

    consume_atom();
    free(current_atom.value);
    expect(
        current_atom.keyword == TK_KEYWORD_LET,
        "Expected let keyword in variable assignemnt"
    );

    consume_atom();
    expect(
        current_atom.type == TK_TYPE_IDENTIFIER,
        "Expected variable name in assignment"
    );

    let_stmt.var_name = current_atom.value;

    consume_atom();
    free(current_atom.value);

    if (current_atom.symbol == TK_SYMBOL_L_BRACK) {
        let_stmt.subscript = parse_expression();
        
        consume_atom();
        free(current_atom.value);
        expect(
            current_atom.symbol == TK_SYMBOL_R_BRACK,
            "Expected ']' at end of array subscript "
            "in variable assignemnt"
        );

        consume_atom();
        free(current_atom.value);
    }

    expect(
        current_atom.symbol == TK_SYMBOL_EQUAL,
        "Expected '=' in variable assignment"
    );
    
    let_stmt.value = parse_expression();

    consume_atom();
    free(current_atom.value);
    expect(
        current_atom.symbol == TK_SYMBOL_SEMICOLON,
        "Expected ';' in variable assignment"
    );

    Parser_statement stmt = make_empty_statement();
    stmt.let_statement = malloc(sizeof(Parser_let_statement));
    *stmt.let_statement = let_stmt;

    LL_Node *node = ll_make_node(sizeof(Parser_statement));
    *(Parser_statement *)node->data = stmt;
    ll_append(node, &subroutine->statements);
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
    do_statement->subroutine_call = parse_subroutine_call(NULL);

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

static void parse_return(Parser_subroutine_dec *subroutine)
{
    consume_atom();
    free(current_atom.value);
    expect(
        current_atom.keyword == TK_KEYWORD_RETURN,
        "Expected 'return' kewyord"
    );

    Parser_return_statement return_stmt;
    return_stmt.has_expr = false;

    Tokenizer_atom peek = peek_atom();
    free(peek.value);

    if (peek.symbol != TK_SYMBOL_SEMICOLON) {
        return_stmt.expression = parse_expression();
        return_stmt.has_expr = true;
    }

    consume_atom();
    expect(
        current_atom.symbol == TK_SYMBOL_SEMICOLON,
        "Expected ';' at end of return statement"
    );

    Parser_statement statement = make_empty_statement();
    statement.return_statement = malloc(sizeof(Parser_return_statement));
    *statement.return_statement = return_stmt;

    LL_Node *statement_node = ll_make_node(sizeof(Parser_statement));
    *(Parser_statement *)statement_node->data = statement;
    ll_append(statement_node, &subroutine->statements);
}

static Parser_expression parse_expression()
{
    Parser_expression expr = make_empty_expression();

    Parser_term term = parse_term();
    LL_Node *node = ll_make_node(sizeof(Parser_term));
    *(Parser_term *)node->data = term;
    ll_append(node, &expr.terms);

    Tokenizer_atom peek = peek_atom();
    free(peek.value);

    while (is_operator(peek.symbol)) {
        consume_atom();
        free(current_atom.value);

        Parser_term_operator op;
        op = get_operator(current_atom.symbol);
        node = ll_make_node(sizeof(Parser_term_operator));
        *(Parser_term_operator *)node->data = op;
        ll_append(node, &expr.operators);

        Parser_term term = parse_term();
        LL_Node *node = ll_make_node(sizeof(Parser_term));
        *(Parser_term *)node->data = term;
        ll_append(node, &expr.terms);

        peek = peek_atom();
        free(peek.value);
    }

    return expr;
}

static Parser_term parse_term()
{
    Parser_term term = make_empty_term();

    consume_atom();

    if (current_atom.type == TK_TYPE_INT_CONSTANT) {
        term.integer = current_atom.value;

    } else if (current_atom.type == TK_TYPE_STR_CONSTANT) {
        term.string = current_atom.value;

    } else if (is_expression_keyword(current_atom.keyword)) {
        term.keyword_value = get_keyword_value(current_atom.keyword);

    } else if (current_atom.type == TK_TYPE_IDENTIFIER) {
        Tokenizer_atom peek = peek_atom();
        free(peek.value);
        
        if (peek.symbol == TK_SYMBOL_L_PAREN || peek.symbol == TK_SYMBOL_DOT) {
            term.subroutine_call = malloc(sizeof(Parser_term_subroutine_call));
            *term.subroutine_call = parse_subroutine_call(current_atom.value);

        } else {
            term.var_usage = malloc(sizeof(Parser_term_var_usage));
            *term.var_usage = parse_var_usage();
        }
    } else if (current_atom.symbol == TK_SYMBOL_L_PAREN) {
        free(current_atom.value);

        term.parenthesized_expression = malloc(sizeof(Parser_expression));
        *term.parenthesized_expression = parse_expression();

        consume_atom();
        free(current_atom.value);
        expect(
            current_atom.symbol == TK_SYMBOL_R_PAREN,
            "Expected ')' at end of expression."
        );

    } else if (is_unary_operator(current_atom.symbol)) {
        Parser_sub_term *sub_term = malloc(sizeof(Parser_sub_term));
        sub_term->unary_op = get_operator(current_atom.symbol);
        sub_term->term = parse_term();
        term.sub_term = sub_term;
        
    } else {
        exit_parsing("Expected start of expression term.");
    }
               
    return term;
}

static bool is_expression_keyword(Tokenizer_keyword keyword)
{
    return keyword == TK_KEYWORD_TRUE    ||
        keyword == TK_KEYWORD_FALSE      ||
        keyword == TK_KEYWORD_NULL_VAL   ||
        keyword == TK_KEYWORD_THIS;
}

static Parser_term_keyword_constant get_keyword_value(Tokenizer_keyword keyword)
{
    if (current_atom.keyword == TK_KEYWORD_TRUE) {
        return PARSER_TERM_KEYWORD_TRUE;

    } else if (current_atom.keyword == TK_KEYWORD_FALSE) {
        return PARSER_TERM_KEYWORD_FALSE;

    } else if (current_atom.keyword == TK_KEYWORD_NULL_VAL) {
        return PARSER_TERM_KEYWORD_NULL;

    } else if (current_atom.keyword == TK_KEYWORD_THIS) {
        return PARSER_TERM_KEYWORD_THIS;

    } else {
        return PARSER_TERM_KEYWORD_UNDEFINED;
    }
}

static bool is_operator(Tokenizer_symbol symbol)
{
    return symbol == TK_SYMBOL_PLUS     ||
        symbol == TK_SYMBOL_MINUS       ||
        symbol == TK_SYMBOL_ASTERISK    ||
        symbol == TK_SYMBOL_SLASH       ||
        symbol == TK_SYMBOL_AMPERSAND   ||
        symbol == TK_SYMBOL_VERT_BAR    ||
        symbol == TK_SYMBOL_LESS_TH     ||
        symbol == TK_SYMBOL_GREATER_TH  ||
        symbol == TK_SYMBOL_EQUAL;
}

static bool is_unary_operator(Tokenizer_symbol symbol)
{
    return symbol == TK_SYMBOL_MINUS || symbol == TK_SYMBOL_NOT;
}

static Parser_term_operator get_operator(Tokenizer_symbol symbol)
{
    if (symbol == TK_SYMBOL_PLUS) {
        return PARSER_TERM_OP_ADDITION;

    } else if (symbol == TK_SYMBOL_MINUS) {
        return PARSER_TERM_OP_SUBTRACTION;

    } else if (symbol == TK_SYMBOL_ASTERISK) {
        return PARSER_TERM_OP_MULTIPLICATION;

    } else if (symbol == TK_SYMBOL_SLASH) {
        return PARSER_TERM_OP_DIVISION;

    } else if (symbol == TK_SYMBOL_AMPERSAND) {
        return PARSER_TERM_OP_AND;

    } else if (symbol == TK_SYMBOL_VERT_BAR) {
        return PARSER_TERM_OP_OR;

    } else if (symbol == TK_SYMBOL_LESS_TH) {
        return PARSER_TERM_OP_LESSER;

    } else if (symbol == TK_SYMBOL_GREATER_TH) {
        return PARSER_TERM_OP_GREATER;

    } else if (symbol == TK_SYMBOL_EQUAL) {
        return PARSER_TERM_OP_ASSIGN;

    } else if (symbol == TK_SYMBOL_NOT) {
        return PARSER_TERM_OP_NOT;

    } else {
        return PARSER_TERM_OP_UNDEFINED;
    }
}

static Parser_term_subroutine_call parse_subroutine_call(char *identifier)
{
    char *instance_var_name = NULL;
    char *subroutine_name = NULL;

    if (identifier == NULL) {
        consume_atom();
        expect(
            current_atom.type == TK_TYPE_IDENTIFIER,
            "Expected name of subroutine or "
            "instance in function call"
        );
        identifier = current_atom.value;
    }

    Tokenizer_atom peek = peek_atom();
    free(peek.value);

    if (peek.symbol == TK_SYMBOL_DOT) {
        instance_var_name = identifier;

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

    LL_List expressions = parse_expressions_list();

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

    return subroutine_call;
}

static LL_List parse_expressions_list()
{
    LL_List exprs = ll_make_empty_list();

    Tokenizer_atom peek = peek_atom();
    free(peek.value);

    if (peek.symbol == TK_SYMBOL_R_PAREN) {
        return exprs;
    }
    
    Parser_expression expr = parse_expression();
    LL_Node *node = ll_make_node(sizeof(Parser_expression));
    *(Parser_expression *)node->data = expr;
    ll_append(node, &exprs);

    peek = peek_atom();
    free(peek.value);

    while (peek.symbol == TK_SYMBOL_COMMA) {
        expr = parse_expression();
        node = ll_make_node(sizeof(Parser_expression));
        *(Parser_expression *)node->data = expr;
        ll_append(node, &exprs);

        peek = peek_atom();
        free(peek.value);
    }

    return exprs;
}

static Parser_term_var_usage parse_var_usage()
{
    expect(
        current_atom.type = TK_TYPE_IDENTIFIER,
        "Expected variable name in expression"
    );

    Parser_term_var_usage var_usage;
    var_usage.var_name = current_atom.value;
    var_usage.expression = NULL;

    Tokenizer_atom peek = peek_atom();
    free(peek.value);

    if (peek.symbol == TK_SYMBOL_L_BRACK) {
        consume_atom();
        free(current_atom.value);

        var_usage.expression = malloc(sizeof(Parser_expression));
        *var_usage.expression = parse_expression();
    }

    return var_usage;
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
    expression.terms = ll_make_empty_list();
    expression.operators = ll_make_empty_list();
    return expression;
}

static Parser_term make_empty_term()
{
    Parser_term term;
    term.integer = NULL;
    term.string = NULL;
    term.keyword_value = PARSER_TERM_KEYWORD_UNDEFINED;
    term.var_usage = NULL;
    term.subroutine_call = NULL;
    term.parenthesized_expression = NULL;
    term.sub_term = NULL;
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

static Tokenizer_atom peek_atom()
{
    Tokenizer_atom atom;
    bool should_skip = true;

    while(should_skip) {
        atom = tokenizer_peek();
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

