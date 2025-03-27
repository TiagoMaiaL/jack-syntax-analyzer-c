#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

typedef enum {
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

static bool tokenize_symbol(Tokenizer_atom *atom);
static Tokenizer_symbol get_symbol(char ch);

static bool tokenize_comment(Tokenizer_atom *atom);
static bool is_in_comment_start(char ch);

static bool tokenize_keyword(Tokenizer_atom *atom);
static Tokenizer_keyword get_keyword(char **val_ref);

static bool tokenize_identifier(Tokenizer_atom *atom);
static char *get_identifier();

static bool tokenize_int_constant(Tokenizer_atom *atom);
static char *get_int_constant();

static bool tokenize_str_constant(Tokenizer_atom *atom);
static char *get_str_constant();

static char peak();
static char get_char();
static void seek_back(int amount);
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

    if (tokenize_symbol(&atom)) {
        return atom;
    }

    if (tokenize_comment(&atom)) {
        return atom;
    }

    if (tokenize_keyword(&atom)) {
        return atom;
    }

    if (tokenize_identifier(&atom)) {
        return atom;
    }

    if (tokenize_int_constant(&atom)) {
        return atom;
    }

    if (tokenize_str_constant(&atom)) {
        return atom;
    }

    // TODO: Treat other unexpected kinds of tokens
    
    return atom;
}

bool tokenizer_finished()
{
    return state == TK_FINISHED;
}

static bool tokenize_symbol(Tokenizer_atom *atom)
{
    if (state == TK_ERROR) {
        atom->type = TK_TYPE_ERROR;
        return false;
    }

    char ch = get_char();

    if (state == TK_FINISHED) {
        return false;
    }
   
    Tokenizer_symbol symbol = get_symbol(ch);

    if (symbol == TK_SYMBOL_UNDEFINED || is_in_comment_start(ch)) {
        seek_back(1);
        return false;
    }

    char *value = malloc(sizeof(char) * 2);
    value[0] = ch;
    value[1] = '\0';

    atom->value = value;
    atom->type = TK_TYPE_SYMBOL;
    atom->symbol = get_symbol(ch);

    return true;
}

static Tokenizer_symbol get_symbol(char ch) {
    for (int i = 0; i < TK_SYMBOLS_COUNT; i++) {
        if (ch == symbol_chars[i]) {
            return i;
        }
    }
    return TK_SYMBOL_UNDEFINED;
}

static bool tokenize_comment(Tokenizer_atom *atom)
{
    char ch;
    bool is_comment_start;
    bool is_line_comment;
    int comment_len;

    ch = get_char();

    if (state == TK_FINISHED) {
        return false;
    }
    
    if (state == TK_ERROR) {
        atom->type = TK_TYPE_ERROR;
        return false;
    }

    if (!is_in_comment_start(ch)) {
        seek_back(1);
        return false;
    }

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

    return true;
}

static bool is_in_comment_start(char ch)
{
    char next_ch = peak();
    return ch == '/' && (next_ch == '/' || next_ch == '*');
}

static bool tokenize_keyword(Tokenizer_atom *atom)
{
    char *value = NULL;
    Tokenizer_keyword keyword = get_keyword(&value);

    if (keyword == TK_KEYWORD_UNDEFINED) {
        return false;
    }

    atom->type = TK_TYPE_KEYWORD;
    atom->keyword = keyword;
    atom->value = value;

    return true;
}

static Tokenizer_keyword get_keyword(char **val_ref)
{
    if (state == TK_ERROR || state == TK_FINISHED) {
        return TK_KEYWORD_UNDEFINED;
    }

    char ch;
    int keyword_len = 0;
    char *keyword_value = NULL;

    while ((ch = get_char()) != EOF && isalpha(ch)) {
        keyword_len++;
    }

    if (ch != EOF && !isalpha(ch)) {
        seek_back(1);
    }

    if (keyword_len == 0) {
        return TK_KEYWORD_UNDEFINED;
    }

    seek_back(keyword_len);

    keyword_value = malloc(sizeof(char) * (keyword_len + 1));
    for (int i = 0; i < keyword_len; i++) {
        keyword_value[i] = get_char();
    }
    keyword_value[keyword_len + 1] = '\0';

    *val_ref = keyword_value;

    for (int i = 0; i < TK_KEYWORDS_COUNT; i++) {
        if (strcmp(keyword_strs[i], keyword_value) == 0) {
            return i;
        }
    }

    seek_back(keyword_len);

    return TK_KEYWORD_UNDEFINED;
}

static bool tokenize_identifier(Tokenizer_atom *atom)
{
    char *identifier = get_identifier();

    if (identifier == NULL) {
        return false;
    }

    atom->type = TK_TYPE_IDENTIFIER;
    atom->value = identifier;

    return true;
}

static char *get_identifier()
{
    if (state == TK_ERROR) {
        return NULL;
    }

    char *value;
    char ch;
    int len;

    value = NULL;
    ch = get_char();

    if (state == TK_FINISHED) {
        return NULL;
    }

    if (!isalpha(ch) && ch != '_') {
        seek_back(1);
        return NULL;
    }

    len = 1;

    while (isalnum(ch = get_char()) || ch == '_') {
        len++;
    }

    if (!isalnum(ch) && ch != '_' && ch != EOF) {
        seek_back(1);
    }

    seek_back(len);

    value = malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        value[i] = get_char();
    }
    value[len + 1] = '\0';

    return value;
}

static bool tokenize_int_constant(Tokenizer_atom *atom)
{
    char *integer = get_int_constant();

    if (integer == NULL) {
        if (state == TK_ERROR) {
            atom->type = TK_TYPE_ERROR;
        }
        return false;
    }

    atom->type = TK_TYPE_INT_CONSTANT;
    atom->value = integer;

    return true;
}

static char *get_int_constant()
{
    char ch;
    int len;
    char *value;

    ch = get_char();

    if (state == TK_FINISHED || state == TK_ERROR) {
        return NULL;
    }

    if (!isdigit(ch)) {
        seek_back(1);
        return NULL;
    }

    len = 1;

    while (isdigit(ch = get_char())) {
        len++;
    }

    seek_back(ch != EOF ? len + 1 : len);
    value = malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        value[i] = get_char();
    }
    value[len] = '\0';

    return value;
}

static bool tokenize_str_constant(Tokenizer_atom *atom)
{
    char *str_literal = get_str_constant();

    if (str_literal == NULL) {
        return false;
    }

    atom->type = TK_TYPE_STR_CONSTANT;
    atom->value = str_literal;

    return true;
}

static char *get_str_constant()
{
    char ch;
    int len;
    char *value;

    ch = get_char();

    if (state == TK_FINISHED || state == TK_ERROR) {
        return NULL;
    }

    if (ch != '\"') {
        seek_back(1);
        return NULL;
    }

    len = 1;

    while ((ch = get_char()) != '\n' && ch != '\"' && ch != EOF) {
        len++;
    }

    if (ch == '\"') {
        len++;
    }

    if (ch == '\n' || (ch == EOF && state != TK_ERROR)) {
        // TODO: Handle incomplete string state.
    }

    seek_back(ch != '\"' ? len + 1 : len);
    value = malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        value[i] = get_char();
    }
    value[len] = '\0';

    return value;
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

static void seek_back(int amount)
{
    if (source == NULL || state == TK_ERROR) {
        return;
    }

    fseek(source, -amount, SEEK_CUR);

    if (state == TK_FINISHED) {
        state = TK_DEFAULT;
    }

    state = TK_DEFAULT;
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

