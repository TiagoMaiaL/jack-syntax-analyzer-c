#include <string.h>
#include <stdlib.h>
#include "hash-table.h"

typedef struct {
    char *key;
    void *data;
} HT_Entry;

int ht_hash(const char *key)
{
    int hash = 0;
    int i = 0;
    char ch;

    while ((ch = key[i]) != '\0') {
        hash += ch;
        i++;
    }

    return hash % HT_MAX_COUNT;
}

HT_Table ht_make_empty_table()
{
    HT_hash_table table;

    for (short i = 0; i < HT_MAX_COUNT; i++) {
        table.values[i] = LL_make_empty_list();
    }

    return table;
}

int ht_store(const char *key, const void *data, HT_Table table)
{
    short index = ht_hash(key);
    LL_Node *node = ht_node(key, table);

    HT_Entry entry;
    entry.key = key;
    entry.data = data;

    if (node == NULL) {
        node = ll_make_node(sizeof(HT_Entry);
        *(HT_Entry *)node->data = entry;

        LL_List *list = &table[index];
        ll_append(node, list);

    } else {
        *(HT_Entry *)node->data = entry;
    }
}

LL_Node *ht_node(const char *key, const HT_Table table)
{
    short index = ht_hash(key);
    LL_List list = table.values[index];
    
    if (list.head == NULL) {
        return NULL;
    }

    LL_Node *node = list.head;

    while (node != NULL) {
        HT_Entry entry = *(HT_Entry *)node->data;
        if (strcmp(key, entry.key) == 0) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

