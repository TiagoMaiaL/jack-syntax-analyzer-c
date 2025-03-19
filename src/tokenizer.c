#include "tokenizer.h"

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

typedef enum {
    IN_COMMENT,
    IN_IDENTIFIER,
    IN_CONSTANT
    DEFAULT
    FINISHED
    ERROR
} State;

static State state;

static FILE *source;

int tokenizer_start(FILE *handle)
{
    if (source == NULL)
        return -1; // TODO: Extract codes
                   // Into their own file.

    source = handle;
    state = DEFAULT;

    return 0;
}

static char get_char()
{
    char ch = fgetch(source);

    if (ch == EOF) {
        if (ferror(source) == 0) {
            state = FINISHED;
        } else {
            state = ERROR;
        }
    }

    return ch;
}

static char *get_word()
{
    return "";
}

Tokenizer_atom tokenizer_next()
{
    if (state == FINISHED) { 
        // TODO: return null atom.
    }

    if (state == ERROR) {
        // TODO: return null atom with error type.
    }

    Tokenizer_atom atom;
    char next_char;

    next_char = get_char();

    if (state == DEFAULT || state == FINISHED) {
        if (is_symbol(next_char)) {
            // If it's a symbol, return it right away.
            // TODO: Initialize atom.
            
        } else {
            // Get word, check if it's a keyword, identifier or constant.
            // TODO: initialize atom.
        }
    } 
           
    return atom;
}

bool tokenizer_finished()
{
    return STATE == FINISHED;
}

