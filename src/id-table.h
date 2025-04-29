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
    char *id;
    int index;
    IDT_Category category;
} IDT_Entry;

void idt_store(const char *id, const char *scope_id, int index, IDT_Category category);
IDT_Entry *idt_entry(const char *id, const char *scope_id);

#endif
