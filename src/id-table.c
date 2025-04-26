#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id-table.h"
#include "hash-table.h"

static HT_Table *table = NULL;

static char *unique_id_key(
    const char *id, 
    const char *scope_id
);

void idt_init()
{
    if (table == NULL) {
        table = malloc(sizeof(HT_Table));
        *table = ht_make_empty_table();
    }
}

void idt_store(
    const char *id, 
    const char *scope_id, 
    int index, 
    IDT_Category category
) {
    idt_init();

    char *key = unique_id_key(id, scope_id);

    IDT_Entry *entry = malloc(sizeof(IDT_Entry));
    entry->category = category;
    entry->id = (char *)id;
    entry->key = (char *)key;
    entry->index = index;

    ht_store(key, (void *)entry, table);
}

IDT_Entry *idt_entry(
    const char *id, 
    const char *scope_id
) {
    void *data = ht_value(
        unique_id_key(id, scope_id), 
        table
    );

    if (data == NULL) {
        return NULL;
    }

    return (IDT_Entry *)data;
}

static char *unique_id_key(
    const char *id, 
    const char *scope_id
) {
    int len = 0;
    len += strlen(id);
    len += strlen(scope_id);
    len += 2; // separator + \0

    char *buffer = malloc(sizeof(char) * len);
    sprintf(buffer, "%s$%s", id, scope_id);

    return buffer;
}
