#ifndef ID_TABLE
#define ID_TABLE

typedef enum {
    IDT_STATIC,
    IDT_LOCAL,
    IDT_FIELD,
    IDT_PARAM
} IDT_Category;

typedef struct {
    char *key;
    int index;
    IDT_Category category;
} IDT_Entry;

void idt_store(const char *key, int index, IDT_Category category);
IDT_Entry *idt_entry(const char *key);

#endif
