#include "tokenizer.h"

const char *tokenizer_keyword_str[] = {
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

const char tokenizer_symbol_ch[] = {
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

FILE *source;

int tokenizer_start(FILE *handle)
{
    if (source == NULL)
        return -1; // TODO: Extract codes
                   // Into their own file.

    source = handle;
    return 0;
}

Tokenizer_atom tokenizer_next()
{
    Tokenizer_atom atom;
    // TODO: Initialize atom according to
    // chars being read from source.

    return atom;
}

bool tokenizer_finished()
{
    // TODO: Determine if end of file was 
    // reached.
    return false;
}

