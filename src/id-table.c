#include <stdlib.h>
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

void idt_store(const char *id, int index, IDT_Category category)
{
    idt_init();

    IDT_Entry *entry = malloc(sizeof(IDT_Entry));
    entry->category = category;
    entry->id = (char *)id;

    ht_store(id, (void *)entry, table);
}

IDT_Entry *idt_entry(const char *id)
{
    void *data = ht_value(id, table);

    if (data == NULL) {
        return NULL;
    }

    return (IDT_Entry *)data;
}

