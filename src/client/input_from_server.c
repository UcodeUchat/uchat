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
    // t_package *input_package = malloc(MX_PACKAGE_SIZE);
    // mx_print_tid("new thread");

    while(1) { // read all input from server
        int rc;
        char buf[2048];
        json_object *new_json;
        int type;
        int user_id;

        rc = tls_read(info->tls_client, buf, 2048);    // get json
        if (rc == -1)
            mx_err_exit("error recv\n");
        if (rc != 0) {

            new_json = json_tokener_parse(buf);
            type = json_object_get_int(json_object_object_get(new_json, "type"));
            user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
            mx_print_json_object(new_json, "mx_process_input_from_server");
            printf("New_package! Type:%d\n", type);
            // if (input_package->type == MX_FILE_SEND_TYPE)
            //     mx_process_file_in_client(info, input_package);
            if (type == MX_AUTH_TYPE || type == MX_AUTH_TYPE_V ||
                type == MX_AUTH_TYPE_NV) {
                if ((*info).auth_client == 0){
                    fprintf(stderr, "ANSWER = [%d]\n", type);
                    if (type == 4) {
                        info->id = user_id;
                        (*info).auth_client = 1;
                    }
                    else
                        (*info).auth_client = 0;
                    fprintf(stderr, "(*info).auth_client = [%d]\n", (*info).auth_client);
                    (*info).responce = 1;
                }
            }
            // else if (input_package->type == MX_REG_TYPE_V || input_package->type == MX_REG_TYPE_NV) {

            // }
            // else if (input_package->type == MX_MSG_TYPE) {
            //     t_room *room = mx_find_room(info->data->rooms, input_package->room_id);
            //     GtkWidget *h_box = gtk_box_new(FALSE, 0);
            //     gtk_box_pack_start (GTK_BOX (room->message_box), h_box, FALSE, FALSE, 0);
            //     gtk_widget_show(h_box);
            //     char *data = mx_strjoin(input_package->login, "\n");
            //     char *tmp = strdup(data);
            //     free(data);
            //     data = mx_strjoin(tmp, input_package->data);
            //     free(tmp);
            //     GtkWidget *button = gtk_label_new(data);
            //     free(data);
            //     sleep_ms(100);
            //     if (input_package->user_id == info->id) {
            //         gtk_label_set_justify (GTK_LABEL (button), GTK_JUSTIFY_RIGHT);
            //         gtk_box_pack_end (GTK_BOX (h_box), button, FALSE, FALSE, 0);
            //         gtk_widget_show(button);
            //         sleep_ms(100);
            //         gtk_adjustment_set_value(room->Adjust, 
            //                                 gtk_adjustment_get_upper(room->Adjust) - 
            //                                 gtk_adjustment_get_page_size(room->Adjust));
            //     }
            //     else {
            //         gtk_label_set_justify (GTK_LABEL (button), GTK_JUSTIFY_LEFT);
            //         gtk_box_pack_start (GTK_BOX (h_box), button, FALSE, FALSE, 0);
            //         gtk_widget_show(button);
            //     }
            //     gtk_widget_set_name(button, "message");
                
            //     t_room *head = info->data->rooms;
            //     while (head != NULL) {
            //         if (head && head->position < room->position)
            //             head->position = head->position + 1;
            //         head = head->next;
            //     }
            //     room->position = 0;
            //     gtk_notebook_reorder_child(GTK_NOTEBOOK(info->data->notebook), room->room_box, 0);
            // }         
        }
    }

    return NULL;
}
