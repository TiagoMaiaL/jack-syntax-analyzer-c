#include <stdlib.h>
#include "id-table.h"
#include "hash-table.h"

typedef struct {
    char *id;
    IDT_Type type;
    IDT_Scope scope;
} IDT_Entry;

static HT_Table *table = NULL;

void idt_init()
{
    if (table == NULL) {
        table = malloc(sizeof(HT_Table));
        *table = ht_make_empty_table();
    }
}

void idt_store(const char *id, IDT_Type type, IDT_Scope scope)
{
    idt_init();

    IDT_Entry *entry = malloc(sizeof(IDT_Entry));
    entry->type = type;
    entry->scope = scope;
    entry->id = id;

    ht_store(id, (void *)entry, table);
}

