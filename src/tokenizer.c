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
static void tokenize_symbol(Tokenizer_atom *atom, char ch);
static void tokenize_comment(Tokenizer_atom *atom);
static void tokenize_identifier(Tokenizer_atom *atom);
static void tokenize_int_literal(Tokenizer_atom *atom);
static void tokenize_str_literal(Tokenizer_atom *atom);

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
    Tokenizer_atom atom = make_empty_atom();

    if (state == TK_ERROR) {
        atom.type = TK_TYPE_ERROR;
        return atom;
    }

    char next_char = get_char();

    if (state == TK_FINISHED) {
        return atom;
    }

    if (state == TK_DEFAULT) {
        Tokenizer_symbol symbol = get_symbol(next_char);

        if (symbol != TK_SYMBOL_UNDEFINED) {
            if (symbol == TK_SYMBOL_SLASH /* && (peak() == '/' || peak() == '*') */) {
                // TODO: tokenize comments as well.

            } else {
                tokenize_symbol(&atom, next_char);
            }
                        
        } else {
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

static void tokenize_symbol(Tokenizer_atom *atom, char ch)
{
    char *value = malloc(sizeof(char) * 2);
    value[0] = ch;
    value[1] = '\0';

    atom->value = value;
    atom->type = TK_TYPE_SYMBOL;
    atom->symbol = get_symbol(ch);
}

static void tokenize_comment(Tokenizer_atom *atom)
{

}

static void tokenize_identifier(Tokenizer_atom *atom)
{

}

static void tokenize_int_literal(Tokenizer_atom *atom)
{

}

static void tokenize_str_literal(Tokenizer_atom *atom)
{

}

static char get_char()
{
    if (state == TK_FINISHED) {
        return EOF;
    }

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

