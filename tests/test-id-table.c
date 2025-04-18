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

    idt_store("test", IDT_VAR, IDT_LOCAL);
    entry = idt_entry("test");

    tst_true(entry != NULL);
    tst_true(entry->type == IDT_VAR);
    tst_true(entry->scope == IDT_LOCAL);
    tst_true(strcmp(entry->id, "test") == 0);

    idt_store("tets", IDT_FUNC, IDT_STATIC);
    entry = idt_entry("tets");
    tst_true(entry != NULL);
    tst_true(entry->type == IDT_FUNC);
    tst_true(entry->scope == IDT_STATIC);
    tst_true(strcmp(entry->id, "tets") == 0);
    
    idt_store("tets", IDT_FUNC, IDT_LOCAL);
    entry = idt_entry("tets");
    tst_true(entry != NULL);
    tst_true(entry->type == IDT_FUNC);
    tst_true(entry->scope == IDT_LOCAL);
    tst_true(strcmp(entry->id, "tets") == 0);


    tst_true(idt_entry("asdf") == NULL);
}

