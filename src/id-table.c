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

void idt_store(
    const char *key, 
    const char *class_name,
    int index, 
    IDT_Category category
) {
    idt_init();

    IDT_Entry *entry = malloc(sizeof(IDT_Entry));
    entry->category = category;
    entry->key = (char *)key;
    entry->index = index;
    entry->class_name = (char *)class_name;

    ht_store(key, (void *)entry, table);
}

IDT_Entry *idt_entry(const char *key) {
    void *data = ht_value(key, table);

    if (data == NULL) {
        return NULL;
    }

    return (IDT_Entry *)data;
}

