#include <stdlib.h>
#include "tokenizer.h"

typedef enum {
    TK_IN_COMMENT,
    TK_IN_IDENTIFIER,
    TK_IN_CONSTANT,
    TK_DEFAULT,
    TK_FINISHED,
    TK_ERROR
} TKState;

static const char *keyword_strings[] = {
    "class",
    "constructor",
    "function",
    "method",
    "field",
    "static",
    "var",
    "int",
    "char",
    "boolean",
    "void",
    "true",
    "false",
    "null",
    "this",
    "let",
    "do",
    "if",
    "else",
    "while",
    "return"
};

static const char symbol_chars[] = {
    '{',
    '}',
    '(',
    ')',
    '[',
    ']',
    '.',
    ',',
    ';',
    '+',
    '-',
    '*',
    '/',
    '&',
    '|',
    '<',
    '>',
    '=',
    '~'
};

static FILE *source;
static TKState state;

static char get_char();
static char *get_word();
static Tokenizer_symbol get_symbol(char ch);
static Tokenizer_atom make_empty_atom();

void tokenizer_start(FILE *handle)
{
    source = handle;

    if (source == NULL) {
        state = TK_ERROR;
    } else {
        state = TK_DEFAULT;
    }
}

Tokenizer_atom tokenizer_next()
{
    if (state == TK_FINISHED) { 
        return make_empty_atom();
    }

    if (state == TK_ERROR) {
        // TODO: return null atom with error type.
    }

    Tokenizer_atom atom;
    char next_char;

    atom = make_empty_atom();
    next_char = get_char();

    if (state == TK_DEFAULT || state == TK_FINISHED) {
        if (get_symbol(next_char) != TK_SYMBOL_UNDEFINED) {
            char *value = malloc(sizeof(char) * 2);
            value[0] = next_char;
            value[1] = '\0';

            atom.value = value;
            atom.type = TK_TYPE_SYMBOL;
            atom.symbol = get_symbol(next_char);
            
        } else {
            // TODO: tokenize comments as well.
            // Get word, check if it's a keyword, identifier or constant.
            // TODO: initialize atom.
        }
    } 
           
    return atom;
}

bool tokenizer_finished()
{
    return state == TK_FINISHED;
}

static char get_char()
{
    printf("State = %d\n", state);

    char ch = fgetc(source);

    if (ch == EOF) {
        if (ferror(source) == 0) {
            state = TK_FINISHED;
        } else {
            state = TK_ERROR;
        }
    }

    return ch;
}

static char *get_word()
{
    return "";
}

static Tokenizer_symbol get_symbol(char ch) {
    for (int i = 0; i < TK_SYMBOLS_COUNT; i++) {
        if (ch == symbol_chars[i]) {
            return i;
        }
    }
    return TK_SYMBOL_UNDEFINED;
}

static Tokenizer_atom make_empty_atom()
{
    Tokenizer_atom atom;
    atom.value = NULL;
    atom.type = TK_TYPE_UNDEFINED;
    atom.symbol = TK_SYMBOL_UNDEFINED;
    atom.keyword = TK_KEYWORD_UNDEFINED;
    return atom;
}

