#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test-tokenizer.h"
#include "test.h"
#include "../src/tokenizer.h"

#define TEST_FILE_NAME "tokenizer_test_file.jack"

static FILE *test_file_handle = NULL;

static void test_tokenizing_whitespace();
static void test_tokenizing_symbols();
static void test_tokenizing_keywords();
static void test_tokenizing_identifiers();
static void test_tokenizing_int_literals();
static void test_tokenizing_str_literals();
static void test_tokenizing_comments();
static void test_tokenizing_simple_file();
static char *test_file_content;
static void prepare_test_file(const char *file_content);
static void erase_test_file();

void test_tokenizer()
{
    tst_suite_begin("Tokenizer");

    tst_unit("Whitespace", test_tokenizing_whitespace);
    tst_unit("Symbols", test_tokenizing_symbols);
    tst_unit("Keywords", test_tokenizing_keywords);
    tst_unit("Identifiers", test_tokenizing_identifiers);
    tst_unit("Int literals", test_tokenizing_int_literals);
    tst_unit("Str literals", test_tokenizing_str_literals);
    tst_unit("Comments", test_tokenizing_comments);
    tst_unit("Simple file", test_tokenizing_simple_file);

    tst_suite_finish();
}

static void test_tokenizing_whitespace()
{
    prepare_test_file(" ");
    tokenizer_start(test_file_handle); 

    Tokenizer_atom atom = tokenizer_next();
 
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(atom.is_complete);
    tst_true(strcmp(atom.value, " ") == 0);
    free(atom.value);


    prepare_test_file(" \t  class");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(atom.is_complete);
    tst_true(strcmp(atom.value, " \t  ") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_KEYWORD);
    tst_true(strcmp(atom.value, "class") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(tokenizer_finished());

    erase_test_file();
}

static void test_tokenizing_symbols()
{
    prepare_test_file("{]};.");
    tokenizer_start(test_file_handle); 

    tst_false(tokenizer_finished());

    Tokenizer_atom atom = tokenizer_next();
 
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_L_CURLY);
    tst_true(atom.value[0] == '{');
    tst_true(atom.is_complete);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.symbol == TK_SYMBOL_R_BRACK);
    tst_true(atom.value[0] == ']');
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.symbol == TK_SYMBOL_R_CURLY);
    tst_true(atom.value[0] == '}');
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.symbol == TK_SYMBOL_SEMICOLON);
    tst_true(atom.value[0] == ';');
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.symbol == TK_SYMBOL_DOT);
    tst_true(atom.value[0] == '.');
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.symbol == TK_SYMBOL_UNDEFINED);
    tst_true(atom.keyword == TK_KEYWORD_UNDEFINED);
    tst_true(atom.type == TK_TYPE_UNDEFINED);
    tst_true(atom.value == NULL);

    tst_true(tokenizer_finished());

    erase_test_file();
}

static void test_tokenizing_keywords()
{
    prepare_test_file("null");
    tokenizer_start(test_file_handle); 

    Tokenizer_atom atom = tokenizer_next();

    tst_true(atom.keyword == TK_KEYWORD_NULL_VAL);
    tst_true(atom.type == TK_TYPE_KEYWORD);
    tst_true(strcmp(atom.value, "null") == 0);
    tst_true(atom.is_complete);

    prepare_test_file("int");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.keyword == TK_KEYWORD_INT);
    tst_true(atom.type == TK_TYPE_KEYWORD);
    tst_true(strcmp(atom.value, "int") == 0);

    prepare_test_file("int.");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.keyword == TK_KEYWORD_INT);
    tst_true(atom.type == TK_TYPE_KEYWORD);
    tst_true(strcmp(atom.value, "int") == 0);

    atom = tokenizer_next();
    tst_true(atom.symbol == TK_SYMBOL_DOT);
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(strcmp(atom.value, ".") == 0);

    atom = tokenizer_next();
    tst_true(tokenizer_finished());

    erase_test_file();
}

static void test_tokenizing_identifiers()
{
    prepare_test_file("nullable");
    tokenizer_start(test_file_handle); 

    Tokenizer_atom atom = tokenizer_next();

    tst_true(atom.type == TK_TYPE_IDENTIFIER);
    tst_true(strcmp(atom.value, "nullable") == 0);
    tst_true(atom.is_complete);


    prepare_test_file("_asdf");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_IDENTIFIER);
    tst_true(strcmp(atom.value, "_asdf") == 0);
    tst_true(atom.is_complete);


    prepare_test_file("__7__123");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_IDENTIFIER);
    tst_true(strcmp(atom.value, "__7__123") == 0);
    tst_true(atom.is_complete);


    prepare_test_file("__7__;");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_IDENTIFIER);
    tst_true(strcmp(atom.value, "__7__") == 0);
    tst_true(atom.is_complete);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_SEMICOLON);
    tst_true(strcmp(atom.value, ";") == 0);
    tst_true(atom.is_complete);


    atom = tokenizer_next();
    tst_true(tokenizer_finished());

    erase_test_file();
}

static void test_tokenizing_int_literals()
{
    prepare_test_file("1234");
    tokenizer_start(test_file_handle); 

    Tokenizer_atom atom = tokenizer_next();

    tst_true(atom.type == TK_TYPE_INT_CONSTANT);
    tst_true(strcmp(atom.value, "1234") == 0);
    tst_true(atom.is_complete);


    prepare_test_file("0");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();

    tst_true(atom.type == TK_TYPE_INT_CONSTANT);
    tst_true(strcmp(atom.value, "0") == 0);
    tst_true(atom.is_complete);

    prepare_test_file("33");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();

    tst_true(atom.type == TK_TYPE_INT_CONSTANT);
    tst_true(strcmp(atom.value, "33") == 0);
    tst_true(atom.is_complete);

    prepare_test_file("x=143+59219;");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_IDENTIFIER);
    tst_true(strcmp(atom.value, "x") == 0);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_EQUAL);
    tst_true(strcmp(atom.value, "=") == 0);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_INT_CONSTANT);
    tst_true(strcmp(atom.value, "143") == 0);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_PLUS);
    tst_true(strcmp(atom.value, "+") == 0);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_INT_CONSTANT);
    tst_true(strcmp(atom.value, "59219") == 0);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_SEMICOLON);
    tst_true(strcmp(atom.value, ";") == 0);


    atom = tokenizer_next();
    tst_true(tokenizer_finished());

    erase_test_file();
}

static void test_tokenizing_str_literals()
{
    prepare_test_file("\"\"");
    tokenizer_start(test_file_handle); 

    Tokenizer_atom atom = tokenizer_next();

    tst_true(atom.type == TK_TYPE_STR_CONSTANT);
    tst_true(strcmp(atom.value, "\"\"") == 0);
    tst_true(atom.is_complete);


    prepare_test_file("\"testing str literal\"");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_STR_CONSTANT);
    tst_true(strcmp(atom.value, "\"testing str literal\"") == 0);
    tst_true(atom.is_complete);


    prepare_test_file("\"asdf;NULL,!+=asdfkjh:::121313__\"");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_STR_CONSTANT);
    tst_true(strcmp(atom.value, "\"asdf;NULL,!+=asdfkjh:::121313__\"") == 0);
    tst_true(atom.is_complete);


    prepare_test_file("x=\"test\";");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_IDENTIFIER);
    tst_true(strcmp(atom.value, "x") == 0);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(strcmp(atom.value, "=") == 0);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_STR_CONSTANT);
    tst_true(strcmp(atom.value, "\"test\"") == 0);
    tst_true(atom.is_complete);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(strcmp(atom.value, ";") == 0);


    prepare_test_file("\"asdf\n");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_STR_CONSTANT);
    tst_true(!atom.is_complete);
    tst_true(strcmp(atom.value, "\"asdf") == 0);

    prepare_test_file("\"asdf");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_STR_CONSTANT);
    tst_true(!atom.is_complete);
    tst_true(strcmp(atom.value, "\"asdf") == 0);


    atom = tokenizer_next();
    tst_true(tokenizer_finished());

    erase_test_file();
}

static void test_tokenizing_comments()
{
    // one-line comment
    prepare_test_file("// asdf asdf\n");
    tokenizer_start(test_file_handle); 

    Tokenizer_atom atom = tokenizer_next();
 
    tst_true(atom.type == TK_TYPE_COMMENT);
    tst_true(strcmp(atom.value, "// asdf asdf\n") == 0);
    free(atom.value);

    // symbol + one-line comment + symbol
    prepare_test_file(";// asdf asdf\n.");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_COMMENT);
    tst_true(strcmp(atom.value, "// asdf asdf\n") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    free(atom.value);

    // block comment (inline)
    prepare_test_file("/* asdf */");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();

    tst_true(atom.type == TK_TYPE_COMMENT);
    tst_true(strcmp(atom.value, "/* asdf */") == 0);
    free(atom.value);

    // block comment (multi-line)
    prepare_test_file("/* asdf\nasdf\nasdf */");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();

    tst_true(atom.type == TK_TYPE_COMMENT);
    tst_true(strcmp(atom.value, "/* asdf\nasdf\nasdf */") == 0);
    free(atom.value);

    // symbol + block comment + symbol
    prepare_test_file("./* asdf\nasdf*//");
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_COMMENT);
    tst_true(strcmp(atom.value, "/* asdf\nasdf*/") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(strcmp(atom.value, "/") == 0);
    free(atom.value);
    
    // unfinished block comment
    prepare_test_file("/* asdf");
    tokenizer_start(test_file_handle); 
   
    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_COMMENT);
    tst_true(!atom.is_complete);
    tst_true(strcmp(atom.value, "/* asdf") == 0);
    free(atom.value);


    atom = tokenizer_next();
    tst_true(tokenizer_finished());

    erase_test_file();
}

static void test_tokenizing_simple_file()
{
    char *source_code = "class Main\n"
                        "{\n"
                        "\tfunction void main()\n"
                        "\t{\n"
                        "\t\treturn;\n"
                        "\t}\n"
                        "}";
    Tokenizer_atom atom;

    prepare_test_file(source_code);
    tokenizer_start(test_file_handle); 

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_KEYWORD);
    tst_true(atom.keyword == TK_KEYWORD_CLASS);
    tst_true(strcmp(atom.value, "class") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(strcmp(atom.value, " ") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_IDENTIFIER);
    tst_true(strcmp(atom.value, "Main") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(strcmp(atom.value, "\n") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_L_CURLY);
    tst_true(strcmp(atom.value, "{") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(strcmp(atom.value, "\n\t") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_KEYWORD);
    tst_true(atom.keyword == TK_KEYWORD_FUNCTION);
    tst_true(strcmp(atom.value, "function") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(strcmp(atom.value, " ") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_KEYWORD);
    tst_true(atom.keyword == TK_KEYWORD_VOID);
    tst_true(strcmp(atom.value, "void") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(strcmp(atom.value, " ") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_IDENTIFIER);
    tst_true(strcmp(atom.value, "main") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_L_PAREN);
    tst_true(strcmp(atom.value, "(") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_R_PAREN);
    tst_true(strcmp(atom.value, ")") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(strcmp(atom.value, "\n\t") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_L_CURLY);
    tst_true(strcmp(atom.value, "{") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(strcmp(atom.value, "\n\t\t") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_KEYWORD);
    tst_true(atom.keyword == TK_KEYWORD_RETURN);
    tst_true(strcmp(atom.value, "return") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_SEMICOLON);
    tst_true(strcmp(atom.value, ";") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(strcmp(atom.value, "\n\t") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_R_CURLY);
    tst_true(strcmp(atom.value, "}") == 0);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_WHITESPACE);
    tst_true(strcmp(atom.value, "\n") == 0);
    free(atom.value);

    atom = tokenizer_next();
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_R_CURLY);
    tst_true(strcmp(atom.value, "}") == 0);

    atom = tokenizer_next();
    tst_true(tokenizer_finished());

    erase_test_file();
}

static void prepare_test_file(const char *file_content)
{
    test_file_handle = fopen(TEST_FILE_NAME, "wa");

    if (fputs(file_content, test_file_handle) == EOF) {
        abort();
    }
    fclose(test_file_handle);

    fopen(TEST_FILE_NAME, "r");
}

static void erase_test_file()
{
    if (test_file_handle != NULL) {
        fclose(test_file_handle);
    }
    remove(TEST_FILE_NAME);
}

