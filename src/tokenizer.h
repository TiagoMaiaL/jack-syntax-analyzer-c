#include <stdbool.h>
#include <stdio.h>

typedef enum {
    CLASS,
    CONSTRUCTOR,
    FUNCTION,
    METHOD,
    FIELD,
    STATIC,
    VAR,
    INT,
    CHAR,
    BOOLEAN,
    VOID,
    TRUE,
    FALSE,
    NULL_VAL,
    THIS,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN
} Tokenizer_keyword;

typedef enum {
    L_CURLY,
    R_CURLY,
    L_PAREN,
    R_PAREN,
    L_BRACK,
    R_BRACK,
    DOT,
    COMMA,
    SEMICOLON,
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    AMPERSAND,
    VERT_BAR,
    LESS_TH,
    GREATER_TH,
    EQUAL,
    NOT
} Tokenizer_symbol;

typedef enum {
    KEYWORD,
    // TODO: define how to identify each symbol.
    SYMBOL,
    // Sequence of letters, digits, and underscores
    IDENTIFIER,
    // Either an int or string constant.
    CONSTANT
} Tokenizer_atom_type;

typedef struct {
    Tokenizer_atom_type type;
    char *value;
} Tokenizer_atom;

int tokenizer_start(FILE *handle);
Tokenizer_atom tokenizer_next();
bool tokenizer_finished();

