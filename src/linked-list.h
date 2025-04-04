#ifndef LL_LIST
#define LL_LIST

typedef struct LL_List LL_List;
typedef struct LL_Node LL_Node;

struct LL_List {
    LL_Node *head;
    LL_Node *tail;
};

struct LL_Node {
    void *data;
    LL_Node *next;
};

LL_List ll_make_empty_list();
LL_List ll_make_list(size_t head_data_size);
LL_Node *ll_make_node(size_t data_size);
LL_Node *ll_tail(LL_Node *node);

void ll_append(LL_Node *node, LL_List *list);
void ll_free(LL_List *list);

#endif
