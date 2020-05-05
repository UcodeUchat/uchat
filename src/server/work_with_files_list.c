#include "uchat.h"

t_file_list *mx_new_file_list_elem(json_object *obj) {
    t_file_list *new_file = (t_file_list *)malloc(sizeof(t_file_list));
    char *full_filename = mx_strjoin(MX_SAVE_FOLDER_IN_SERVER,
        json_object_get_string(json_object_object_get(obj, "file_name")));

    mx_memset(new_file, 0, sizeof(t_file_list));
    new_file->file = fopen(full_filename, "w+");
    mx_strdel(&full_filename);
    if (new_file->file != NULL) {
        new_file->user_id = json_object_get_int(json_object_object_get(obj, "user_id"));
        new_file->file_size = json_object_get_int(json_object_object_get(obj, "file_size"));
        new_file->file_name = mx_strdup(json_object_get_string(json_object_object_get(obj, "file_name")));
    }
    else {
        free(new_file);
        return NULL;
    }
    return new_file;
}

void mx_push_file_elem_to_list(t_file_list **files_list, t_file_list *new) {
    t_file_list *tmp = *files_list;

    if (tmp == NULL)
        *files_list = new;
    else {
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new;
    }
}
