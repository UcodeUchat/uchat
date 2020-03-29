#include "libmx.h"

int mx_list_size(t_list *list) {
    int size = 0;

    if (list) {
        t_list *temp = list;
        
        while (temp) {
            temp = temp->next;
            size++;
        }
    }
    return size;
}
