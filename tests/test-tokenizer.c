#include <stdio.h>
#include <stdlib.h>
#include "test-tokenizer.h"
#include "test.h"
#include "../src/tokenizer.h"

#define TEST_FILE_NAME "tokenizer_test_file.jack"

static FILE *test_file_handle = NULL;

static void test_tokenizing_symbols();
static void test_tokenizing_keywords();
static void test_tokenizing_identifiers();
static void test_tokenizing_int_literals();
static void test_tokenizing_str_literals();
static void test_tokenizing_comments();
static char *test_file_content;
static void prepare_test_file(const char *file_content);
static void erase_test_file();

void test_tokenizer()
{
    tst_suite_begin("Tokenizer");

    tst_unit("Symbols", test_tokenizing_symbols);
    tst_unit("Keywords", test_tokenizing_keywords);
    tst_unit("Identifiers", test_tokenizing_identifiers);
    tst_unit("Int literals", test_tokenizing_int_literals);
    tst_unit("Str literals", test_tokenizing_str_literals);
    tst_unit("Comments", test_tokenizing_comments);

    tst_suite_finish();
}

static void test_tokenizing_symbols()
{
    prepare_test_file("{]};.");
    tokenizer_start(test_file_handle); 

    Tokenizer_atom atom = tokenizer_next();
    
    tst_true(atom.type == TK_TYPE_SYMBOL);
    tst_true(atom.symbol == TK_SYMBOL_L_CURLY);
    tst_true(atom.value[0] == '{');
    free(atom.value);

    erase_test_file();
}

static void test_tokenizing_keywords()
{

}

static void test_tokenizing_identifiers()
{

}

static void test_tokenizing_int_literals()
{

}

static void test_tokenizing_str_literals()
{

}

static void test_tokenizing_comments()
{

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

