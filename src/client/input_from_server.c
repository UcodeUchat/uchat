#include "uchat.h"

t_room *mx_find_room(t_room *rooms, int id) {
   t_room *head = rooms;
   t_room *node = NULL;

    while (head != NULL) {
        if (head->id == id) {
            node = head;
            break;
        }
        head = head->next;
    }
    return node;
}

void *mx_process_input_from_server(void *taken_info) {
    t_client_info *info = (t_client_info *)taken_info;
    t_package *input_package = malloc(MX_PACKAGE_SIZE);

    (void)info;
    (void)input_package;
    mx_print_tid("new thread");

//    while(1) { // read all input from server
        int rc = tls_read(info->tls_client, input_package, (ssize_t) MX_PACKAGE_SIZE);
        if (rc == -1)
            mx_err_exit("error recv\n");
        if (rc != 0) {
            //mx_print_curr_time();
            //printf("input: piece.%d, type.%d, login.%s\n", input_package->piece, input_package->type,
                   //input_package->login);
            GtkTreeIter iter;
//            info->data->current_room = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
            t_room *room = mx_find_room(info->data->rooms, input_package->room_id);
            gtk_list_store_append(GTK_LIST_STORE(room->list), &iter);
            gtk_list_store_set(GTK_LIST_STORE(room->list), &iter, 0, input_package->login, 1, input_package->data, -1);
            t_room *head = info->data->rooms;
            while (head != NULL) {
                if (head && head->position < room->position)
                    head->position = head->position + 1;
                head = head->next;
            }
            room->position = 0;
            gtk_notebook_reorder_child(GTK_NOTEBOOK(info->data->notebook), room->room_box, 0);
        }
//    }

    return NULL;
}
