#include "uchat.h"

t_file_list *create_new_file_list(char *name, int id) {
    t_file_list *new = malloc(sizeof(t_file_list));
    char *full_name = mx_strjoin(MX_SAVE_FOLDER_IN_CLIENT, name);

    if ((new->file = fopen(full_name, "w+")) != NULL) {
        new->file_name = full_name;
        new->id = id;
        new->next = NULL;
    }
    else {
        fprintf(stderr, "fopen for new file error\n");
        free(new);
        new = NULL;
        mx_strdel(&full_name);
    }
    return new;
}

t_file_list *mx_find_file_in_list(t_file_list *list, int id) {
    t_file_list *tmp = list;

    while (tmp) {
        if (tmp->id == id)
            return tmp;
        else
            tmp = tmp->next;
    }
    return NULL;
}

int mx_add_file_to_list_in_client(t_file_list **list, char *name, int id) {
    t_file_list *tmp = *list;

    if (mx_find_file_in_list(*list, id) == NULL) {
        t_file_list *new = create_new_file_list(name, id);

        if (new != NULL) {
            if (*list) {
                while(tmp->next)
                    tmp = tmp->next;
                tmp->next = new;
            }
            else
                *list = new;
            return 0;
        }
    }
    else
        fprintf(stderr, "file exist in file list\n");
    return 1;
}

void mx_pop_file_list_in_client(t_file_list **list, int id) {
    t_file_list *tmp = *list;

    if ((*list)->id == id) {
        *list = (*list)->next;
        mx_strdel(&(tmp->file_name));
        free(tmp);
    }
    else {
        while(tmp->next) {
            if (tmp->next->id == id) {
                t_file_list *tmp_del = tmp->next;

                tmp->next = tmp->next->next;
                mx_strdel(&tmp_del->file_name);
                free(tmp_del);
                break;
            }
            tmp = tmp->next;
        }
    }
}
