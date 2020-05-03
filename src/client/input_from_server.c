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

    int rc;

    mx_print_tid("new thread");

    while(1) { // read all input from server
//.. read json
        char buf[2048];
        json_object *new_json;
//        char json_str;

        rc = tls_read(info->tls_client, buf, 2048);    // get json

////****
//        int rc = tls_read(info->tls_client, input_package, (ssize_t) MX_PACKAGE_SIZE);

        if (rc == -1)
            mx_err_exit("error recv\n");
        if (rc != 0) {

            new_json = json_tokener_parse(buf);
            mx_print_json_object(new_json, "mx_process_input_from_server");
            input_package = mx_json_to_package(new_json);
            printf("New_package! Type:%d\n", input_package->type);

//            json_object *new_json = mx_packege_to_json(input_package);
//            mx_print_json_object(new_json, "process_input_from_server");

            if (input_package->type == MX_FILE_SEND_TYPE)
                mx_process_file_in_client(info, input_package);
            //mx_print_curr_time();
            if (input_package->add_info == MX_AUTH_TYPE || input_package->add_info == MX_AUTH_TYPE_V ||
                input_package->add_info == MX_AUTH_TYPE_NV) {
                if ((*info).auth_client == 0){
                    fprintf(stderr, "ANSWER = [%d]\n", input_package->add_info);
                    if (input_package->add_info == 4) {
                        info->id = input_package->user_id;
                        (*info).auth_client = 1;
                    }
                    else
                        (*info).auth_client = 0;
                    fprintf(stderr, "(*info).auth_client = [%d]\n", (*info).auth_client);
                    (*info).responce = 1;
                }
            }
            else if (input_package->add_info == MX_REG_TYPE) {

            }
            else if (input_package->add_info == MX_MSG_TYPE) {
                t_room *room = mx_find_room(info->data->rooms, input_package->room_id);
                GtkWidget *h_box = gtk_box_new(FALSE, 0);
                gtk_box_pack_start (GTK_BOX (room->message_box), h_box, FALSE, FALSE, 0);
                gtk_widget_show(h_box);
                char *data = mx_strjoin(input_package->login, "\n");
                char *tmp = strdup(data);
                free(data);
                data = mx_strjoin(tmp, input_package->data);
                free(tmp);
                GtkWidget *button = gtk_label_new(data);
                free(data);
                gtk_widget_set_name(button, "message");
                sleep_ms(100);
                if (input_package->user_id == info->id) {
                    gtk_label_set_justify (GTK_LABEL (button), GTK_JUSTIFY_RIGHT);
                    gtk_box_pack_end (GTK_BOX (h_box), button, FALSE, FALSE, 0);
                    gtk_widget_show(button);
                    sleep_ms(100);
                    gtk_adjustment_set_value(room->Adjust, 
                                            gtk_adjustment_get_upper(room->Adjust) - 
                                            gtk_adjustment_get_page_size(room->Adjust));
                }
                else {
                    gtk_label_set_justify (GTK_LABEL (button), GTK_JUSTIFY_LEFT);
                    gtk_box_pack_start (GTK_BOX (h_box), button, FALSE, FALSE, 0);
                    gtk_widget_show(button);
                }
                
                t_room *head = info->data->rooms;
                while (head != NULL) {
                    if (head && head->position < room->position)
                        head->position = head->position + 1;
                    head = head->next;
                }
                room->position = 0;
                gtk_notebook_reorder_child(GTK_NOTEBOOK(info->data->notebook), room->room_box, 0);
            }         
        }
    }

    return NULL;
}
