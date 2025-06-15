#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id-table.h"
#include "hash-table.h"

static HT_Table *table = NULL;

void idt_init()
{
    if (table == NULL) {
        table = malloc(sizeof(HT_Table));
        *table = ht_make_empty_table();
    }
}

void idt_store_var(
    const char *key, 
    const char *class_name,
    int index, 
    IDT_Category category
) {
    idt_init();

    IDT_Entry *entry = malloc(sizeof(IDT_Entry));
    IDT_Var_Entry *var = malloc(sizeof(IDT_Var_Entry));
    var->category = category;
    var->key = (char *)key;
    var->index = index;
    var->class_name = (char *)class_name;
    entry->var = var;

    ht_store(key, (void *)entry, table);
}

void idt_store_subroutine(
    const char *key,
    const bool is_void
) {
    idt_init();

    IDT_Entry *entry = malloc(sizeof(IDT_Entry));
    IDT_Subroutine_Entry *subroutine = malloc(sizeof(IDT_Subroutine_Entry));
    subroutine->key = (char *)key;
    subroutine->is_void = (bool)is_void;
    entry->subroutine = subroutine;

    ht_store(key, (void *)entry, table);
}

IDT_Entry *idt_entry(const char *key) {
    void *data = ht_value(key, table);

    if (data == NULL) {
        return NULL;
    }

    return (IDT_Entry *)data;
}

