#include <string.h>
#include "test.h"
#include "test-id-table.h"
#include "../src/id-table.h"

void test_id_table_usage();

void test_id_table()
{
    tst_suite_begin("Id table");
    tst_unit("Identifier table usage", test_id_table_usage);
    tst_suite_finish();
}

#include <stdio.h>

void test_id_table_usage()
{
    IDT_Entry *entry = NULL;

    idt_store("test", "some_func", 0, IDT_LOCAL);
    entry = idt_entry("test", "some_func");
    tst_true(entry != NULL);
    tst_true(entry->category == IDT_LOCAL);
    tst_true(strcmp(entry->key, "test$some_func") == 0);
    tst_true(strcmp(entry->id, "test") == 0);

    idt_store("tets", "func_some", 0, IDT_PARAM);
    entry = idt_entry("tets", "func_some");
    tst_true(entry != NULL);
    tst_true(entry->category == IDT_PARAM);
    tst_true(strcmp(entry->key, "tets$func_some") == 0);
    tst_true(strcmp(entry->id, "tets") == 0);
    
    idt_store("testing", "class", 1, IDT_FIELD);
    entry = idt_entry("testing", "class");
    tst_true(entry != NULL);
    tst_true(entry->index == 1);
    tst_true(entry->category == IDT_FIELD);
    tst_true(strcmp(entry->key, "testing$class") == 0);
    tst_true(strcmp(entry->id, "testing") == 0);

    tst_true(idt_entry("asdf", "") == NULL);
}

