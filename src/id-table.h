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
    // TODO: Add a class name property
    int index;
    IDT_Category category;
} IDT_Entry;

void idt_store(const char *id, const char *scope_id, int index, IDT_Category category);
IDT_Entry *idt_entry(const char *id, const char *scope_id);
char *idt_category_name(IDT_Category category);

#endif
