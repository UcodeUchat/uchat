#include "libmx.h"

void mx_push_front(t_list **list, void *data) {
    t_list *new_node = NULL;

    if (list && (new_node = mx_create_node(data)) != NULL) {
        new_node->next = *list;
        *list = new_node;
    }
}
