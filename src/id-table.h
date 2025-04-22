#ifndef ID_TABLE
#define ID_TABLE

typedef enum {
    IDT_STATIC,
    IDT_LOCAL,
    IDT_FIELD,
    IDT_PARAM
} IDT_Scope;

typedef struct {
    char *id;
    int index;
    IDT_Scope scope;
} IDT_Entry;

void idt_store(const char *id, int index, IDT_Scope scope);

IDT_Entry *idt_entry(const char *id);

#endif
