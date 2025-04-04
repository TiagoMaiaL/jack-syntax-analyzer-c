#include <stdlib.h>
#include "linked-list.h"

LL_List ll_make_empty_list()
{
    LL_List list;
    list.head = NULL;
    list.tail = NULL;
    list.count = 0;
    return list;
}

LL_List ll_make_list(size_t head_data_size)
{
    LL_Node *node = ll_make_node(head_data_size);
    LL_List list = ll_make_empty_list();
    list.head = node;
    list.tail = node;
    list.count = 1;
    return list;
}

LL_Node *ll_make_node(size_t data_size)
{
    LL_Node *node;

    node = malloc(sizeof(LL_Node));
    node->data = malloc(data_size);
    node->next = NULL;

    return node;
}

LL_Node *ll_tail(LL_Node *node)
{
    while (node->next != NULL) {
        node = node->next;
    }
    return node;
}

void ll_append(LL_Node *node, LL_List *list)
{
    list->tail->next = node;
    list->tail = node;
    list->count++;
}

void ll_free(LL_List *list)
{
    LL_Node *node;
    LL_Node *next;

    node = list->head;

    while (node->next != NULL) {
        next = node->next;
        free(node);
        node = next;
    }

    free(node);

    list->head = NULL;
    list->tail = NULL;
}

