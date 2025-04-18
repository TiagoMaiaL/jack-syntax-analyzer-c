#ifndef ID_TABLE
#define ID_TABLE

typedef enum {
    IDT_STATIC,
    IDT_LOCAL
} IDT_Scope;

typedef enum {
    IDT_VAR,
    IDT_FUNC
} IDT_Type;

void idt_store(const char *id, IDT_Type type, IDT_scope scope);

#endif
