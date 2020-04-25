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

    while(1) { // read all input from server
        int rc = tls_read(info->tls_client, input_package, (ssize_t) MX_PACKAGE_SIZE);
        if (rc == -1)
            mx_err_exit("error recv\n");
        if (rc != 0) {
            //mx_print_curr_time();
            if ((*info).auth_client == 0){
                fprintf(stderr, "ANSWER = [%d]\n", input_package->add_info);
                if (input_package->add_info == 4)
                    (*info).auth_client = 1;
                else
                    (*info).auth_client = 0;
                fprintf(stderr, "(*info).auth_client = [%d]\n", (*info).auth_client);
                (*info).responce = 1;
            }
            else {
                // t_room *room = mx_find_room(info->data->rooms, input_package->room_id);
                // GtkWidget *box = gtk_box_new(FALSE, 0);
                // gtk_box_pack_start (GTK_BOX (room->message_box), box, FALSE, FALSE, 0);
                // gtk_widget_show(box);
                // GtkWidget *button = gtk_button_new_with_label(input_package->data);
                // gtk_box_pack_end (GTK_BOX (box), button, FALSE, FALSE, 0);
                // gtk_widget_show(button);
                // t_room *head = info->data->rooms;
                // while (head != NULL) {
                //     if (head && head->position < room->position)
                //         head->position = head->position + 1;
                //     head = head->next;
                // }
                // room->position = 0;
                // gtk_notebook_reorder_child(GTK_NOTEBOOK(info->data->notebook), room->room_box, 0);
            }         
        }
    }

    return NULL;
}
