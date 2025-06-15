#ifndef ID_TABLE
#define ID_TABLE

#include <stdbool.h>

typedef enum {
    IDT_STATIC,
    IDT_LOCAL,
    IDT_FIELD,
    IDT_PARAM
} IDT_Category;

typedef struct {
    char *key;
    char *class_name;
    int index;
    IDT_Category category;
} IDT_Var_Entry;

typedef struct {
    char *key;
    bool is_void;
} IDT_Subroutine_Entry;

typedef struct {
    IDT_Var_Entry *var;
    IDT_Subroutine_Entry *subroutine;
} IDT_Entry;

void idt_store_var(
    const char *key, 
    const char *class_name, 
    int index, 
    IDT_Category category
);
void idt_store_subroutine(
    const char *key,
    const bool is_void
);
IDT_Entry *idt_entry(const char *key);

#endif
