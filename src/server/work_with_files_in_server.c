#include "uchat.h"

int mx_add_new_file_server(t_file_list **input_files, t_package *package) {
    if (package->add_info > 0) {
        t_file_list *new_file = (t_file_list *)malloc(sizeof(t_file_list));
        char *full_filename = mx_strjoin(MX_SAVE_FOLDER_IN_SERVER, package->data);

        mx_memset(new_file, 0, sizeof(t_file_list));
        new_file->file = fopen(full_filename, "w+");
        mx_strdel(&full_filename);
        if (new_file->file != NULL) { // check for similar filename in files folder
            t_file_list *tmp = (*input_files);

            new_file->user_id = package->user_id;
            new_file->file_size = package->add_info;
            if (tmp == NULL)
                *input_files = new_file;
            else {
                while (tmp->next)
                    tmp = tmp->next;
                tmp->next = new_file;
            }
            new_file->package = package;
            return 0;
        }
        else
            mx_err_exit("Can't create new file\n");
    }
    return 1;
}

int mx_add_data_to_file_server(t_file_list **input_files, t_package *package) {
    t_file_list *tmp = (*input_files);

    while(tmp && tmp->user_id != package->user_id)
        tmp = tmp->next;
    if (tmp) {
        tmp->package_size += package->add_info;
        fwrite(package->data, 1, package->add_info, tmp->file);
        return 0;
    }
    else
        return 1;
}

int mx_final_file_input_server(t_server_info *info, t_package *package) {
    t_file_list *our_file_list = info->input_files;
    t_file_list *prev_elem = NULL;

    while(our_file_list && our_file_list->user_id != package->user_id) {
        prev_elem = our_file_list;
        our_file_list = our_file_list->next;
    }

    fclose(our_file_list->file);
    if (our_file_list->file_size == our_file_list->package_size) // Sucess file sending
        // add to db and take, msg id
        mx_send_package_to_all_in_room(info, our_file_list->package);
    else {
        char *full_name = mx_strjoin(MX_SAVE_FOLDER_IN_SERVER, our_file_list->package->data);

        fprintf(stderr, "Incorrect file_size: %d|%d, %s\n", our_file_list->package_size, our_file_list->file_size, full_name);
        remove(full_name);
        mx_strdel(&full_name);
    }

    if (prev_elem == NULL)
        info->input_files = our_file_list->next;
    else
        prev_elem->next = prev_elem->next->next;
    free(our_file_list->package);
    free(our_file_list);
    return 0;
}
