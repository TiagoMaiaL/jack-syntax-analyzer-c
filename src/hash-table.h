#ifndef HT_HASH_TABLE
#define HT_HASH_TABLE

#include <stdbool.h>
#include "linked-list.h"

#define HT_MAX_COUNT 500

typedef struct {
    LL_List values[HT_MAX_COUNT];
} HT_Table;

HT_Table ht_make_empty_table();
void ht_store(const char *key, const void *data, HT_Table table);
void *ht_value(const char *key, const HT_Table table);

#endif
