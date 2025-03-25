#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

typedef enum {
    TK_IN_COMMENT,
    TK_IN_IDENTIFIER,
    TK_IN_CONSTANT,
    TK_DEFAULT,
    TK_FINISHED,
    TK_ERROR
} TKState;

static const char *keyword_strs[] = {
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

static void tokenize_symbol(Tokenizer_atom *atom, char ch);
static Tokenizer_symbol get_symbol(char ch);

static void tokenize_comment(Tokenizer_atom *atom);
static bool is_comment_start(Tokenizer_symbol symbol);

static void tokenize_keyword(Tokenizer_atom *atom);
static Tokenizer_keyword get_keyword(char *val_ref);

static void tokenize_identifier(Tokenizer_atom *atom);
static void tokenize_int_literal(Tokenizer_atom *atom);
static void tokenize_str_literal(Tokenizer_atom *atom);

static char peak();
static char get_char();
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
            if (is_comment_start(symbol)) {
                tokenize_comment(&atom);
            } else {
                tokenize_symbol(&atom, next_char);
            }
        } else {
            if (get_keyword(NULL) != TK_KEYWORD_UNDEFINED) {
                tokenize_keyword(&atom);
            }
            // TODO: Check for identifiers
            // TODO: Check for int constants
            // TODO: Check for str constants
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

static Tokenizer_symbol get_symbol(char ch) {
    for (int i = 0; i < TK_SYMBOLS_COUNT; i++) {
        if (ch == symbol_chars[i]) {
            return i;
        }
    }
    return TK_SYMBOL_UNDEFINED;
}

static void tokenize_comment(Tokenizer_atom *atom)
{
    char ch;
    bool is_line_comment;
    int comment_len;

    ch = get_char();
    is_line_comment = ch == '/';
    comment_len = 2; // / + (/ or *)

    while ((ch = get_char()) != EOF) {
        comment_len += 1;

        if (is_line_comment && ch == '\n') {
            break;
        }

        if (!is_line_comment && (ch == '*' && peak() == '/')) {
            comment_len += 1; // account for peak().
            break;
        }
    }

    char *value = malloc(sizeof(char) * (comment_len + 1));

    if (is_line_comment) {
        fseek(source, -comment_len, SEEK_CUR);
    } else {
        fseek(source, -(comment_len - 1), SEEK_CUR);
    }

    for (int i = 0; i < comment_len; i++) {
        value[i] = get_char();
    }
    value[comment_len] = '\0';
    
    atom->type = TK_TYPE_COMMENT;
    atom->value = value;
}

static bool is_comment_start(Tokenizer_symbol symbol)
{
    char ch = peak();
    return symbol == TK_SYMBOL_SLASH && (ch == '/' || ch == '*');
}

static void tokenize_keyword(Tokenizer_atom *atom)
{
    char *value = NULL;
    Tokenizer_keyword keyword = get_keyword(value);

    atom->type = TK_TYPE_KEYWORD;
    atom->keyword = keyword;
    atom->value = value;
}

static Tokenizer_keyword get_keyword(char *val_ref)
{
    fseek(source, -1, SEEK_CUR);
    
    int keyword_len = 0;
    char ch = get_char();
    char *keyword_value = NULL;

    while (isalpha(ch)) {
        keyword_len++;
    }

    fseek(source, -keyword_len, SEEK_CUR);

    if (keyword_len == 0) {
        return TK_KEYWORD_UNDEFINED;
    }

    keyword_value = malloc(sizeof(char) * (keyword_len + 1));
    for (int i = 0; i < keyword_len; i++) {
        keyword_value[i] = get_char();
    }
    keyword_value[keyword_len + 1] = '\0';

    val_ref = keyword_value;

    for (int i = 0; i < TK_KEYWORDS_COUNT; i++) {
        if (strcmp(keyword_strs[i], keyword_value) == 0) {
            return i;
        }
    }

    return TK_KEYWORD_UNDEFINED;
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

static char peak()
{
    if (state == TK_FINISHED) {
        return EOF;
    }

    char ch = fgetc(source);

    if (ch != EOF) {
        fseek(source, -1, SEEK_CUR);
    }

    return ch;
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

static Tokenizer_atom make_empty_atom()
{
    Tokenizer_atom atom;
    atom.value = NULL;
    atom.type = TK_TYPE_UNDEFINED;
    atom.symbol = TK_SYMBOL_UNDEFINED;
    atom.keyword = TK_KEYWORD_UNDEFINED;
    return atom;
}

