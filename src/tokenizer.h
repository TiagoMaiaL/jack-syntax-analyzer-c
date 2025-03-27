#include <stdbool.h>
#include <stdio.h>

#define TK_KEYWORDS_COUNT   21
#define TK_SYMBOLS_COUNT    19

typedef enum {
    TK_KEYWORD_CLASS,
    TK_KEYWORD_CONSTRUCTOR,
    TK_KEYWORD_FUNCTION,
    TK_KEYWORD_METHOD,
    TK_KEYWORD_FIELD,
    TK_KEYWORD_STATIC,
    TK_KEYWORD_VAR,
    TK_KEYWORD_INT,
    TK_KEYWORD_CHAR,
    TK_KEYWORD_BOOLEAN,
    TK_KEYWORD_VOID,
    TK_KEYWORD_TRUE,
    TK_KEYWORD_FALSE,
    TK_KEYWORD_NULL_VAL,
    TK_KEYWORD_THIS,
    TK_KEYWORD_LET,
    TK_KEYWORD_DO,
    TK_KEYWORD_IF,
    TK_KEYWORD_ELSE,
    TK_KEYWORD_WHILE,
    TK_KEYWORD_RETURN,
    TK_KEYWORD_UNDEFINED
} Tokenizer_keyword;

typedef enum {
    TK_SYMBOL_L_CURLY,
    TK_SYMBOL_R_CURLY,
    TK_SYMBOL_L_PAREN,
    TK_SYMBOL_R_PAREN,
    TK_SYMBOL_L_BRACK,
    TK_SYMBOL_R_BRACK,
    TK_SYMBOL_DOT,
    TK_SYMBOL_COMMA,
    TK_SYMBOL_SEMICOLON,
    TK_SYMBOL_PLUS,
    TK_SYMBOL_MINUS,
    TK_SYMBOL_ASTERISK,
    TK_SYMBOL_SLASH,
    TK_SYMBOL_AMPERSAND,
    TK_SYMBOL_VERT_BAR,
    TK_SYMBOL_LESS_TH,
    TK_SYMBOL_GREATER_TH,
    TK_SYMBOL_EQUAL,
    TK_SYMBOL_NOT,
    TK_SYMBOL_UNDEFINED
} Tokenizer_symbol;

typedef enum {
    TK_TYPE_SYMBOL,
    TK_TYPE_COMMENT,
    TK_TYPE_KEYWORD,
    TK_TYPE_IDENTIFIER,
    TK_TYPE_INT_CONSTANT,
    TK_TYPE_STR_CONSTANT,
    TK_TYPE_ERROR,
    TK_TYPE_UNDEFINED
} Tokenizer_atom_type;

typedef struct {
    Tokenizer_atom_type type;
    Tokenizer_symbol symbol;
    Tokenizer_keyword keyword;
    char *value;
} Tokenizer_atom;

void tokenizer_start(FILE *handle);
Tokenizer_atom tokenizer_next();
bool tokenizer_finished();

