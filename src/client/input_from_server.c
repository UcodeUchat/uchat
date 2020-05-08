#include "uchat.h"

t_message *create_message(t_client_info *info, t_room *room, json_object *new_json) {
    t_message *node =  (t_message *)malloc(sizeof(t_message));
    int id = json_object_get_int(json_object_object_get(new_json, "id"));
    int user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
    const char *login = json_object_get_string(json_object_object_get(new_json, "login"));
    const char *message = json_object_get_string(json_object_object_get(new_json, "data"));

    node->id = id;
    node->h_box = gtk_box_new(FALSE, 0);
    gtk_box_pack_start (GTK_BOX (room->message_box), node->h_box, FALSE, FALSE, 0);
    gtk_widget_show(node->h_box);
    char *data = mx_strjoin(login, "\n");
    char *tmp = strdup(data);
    free(data);
    data = mx_strjoin(tmp, message);
    free(tmp);
    GtkWidget *button = gtk_label_new(data);
    free(data);
    sleep_ms(100);
    if (user_id == info->id) {
        gtk_label_set_justify (GTK_LABEL (button), GTK_JUSTIFY_RIGHT);
        gtk_box_pack_end (GTK_BOX (node->h_box), button, FALSE, FALSE, 0);
        gtk_widget_show(button);
        sleep_ms(100);
        gtk_adjustment_set_value(room->Adjust, 
                                gtk_adjustment_get_upper(room->Adjust) - 
                                gtk_adjustment_get_page_size(room->Adjust));
    }
    else {
        gtk_label_set_justify (GTK_LABEL (button), GTK_JUSTIFY_LEFT);
        gtk_box_pack_start (GTK_BOX (node->h_box), button, FALSE, FALSE, 0);
        gtk_widget_show(button);
    }
    gtk_widget_set_name(button, "message");
    
    node->next = NULL;
    return node;
}

void push_message(t_client_info *info, t_room *room, json_object *new_json) {
    t_message *tmp;
    t_message *p;
    t_message **list = &room->messages;

    if (!list)
        return;
    tmp = create_message(info, room, new_json);  // Create new
    if (!tmp)
        return;
    p = *list;
    if (*list == NULL) {  // Find Null-node
        *list = tmp;
        return;
    }
    else {
        while (p->next != NULL)  // Find Null-node
            p = p->next;
        p->next = tmp;
    }
}

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

void input_message(t_client_info *info, json_object *new_json) {
    int room_id = json_object_get_int(json_object_object_get(new_json, "room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    push_message(info, room, new_json);           
    t_room *head = info->data->rooms;
    while (head != NULL) {
        if (head && head->position < room->position)
            head->position = head->position + 1;
        head = head->next;
    }
    room->position = 0;
    gtk_notebook_reorder_child(GTK_NOTEBOOK(info->data->notebook), room->room_box, 0);
}

void input_authentification(t_client_info *info, json_object *new_json) {
    int type = json_object_get_int(json_object_object_get(new_json, "type"));
    int user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));

    if ((*info).auth_client == 0){
        fprintf(stderr, "ANSWER = [%d]\n", type);
        if (type == 4) {
            info->id = user_id;
            (*info).auth_client = 1;
            json_object_object_get_ex(new_json, "rooms", &info->rooms);
        }
        else{
            (*info).auth_client = 0;
        }
        fprintf(stderr, "(*info).auth_client = [%d]\n", (*info).auth_client);
        (*info).responce = 1;
    }
}

void *mx_process_input_from_server(void *taken_info) {
    t_client_info *info = (t_client_info *)taken_info;

    while (1) { // read all input from server
        int rc;
        char buf[2048];
        json_object *new_json;
        int type;

        rc = tls_read(info->tls_client, buf, 2048);    // get json
        if (rc == -1)
            mx_err_exit("error recv\n");
        if (rc != 0) {
            new_json = json_tokener_parse(buf);
            type = json_object_get_int(json_object_object_get(new_json, "type"));
            mx_print_json_object(new_json, "mx_process_input_from_server");
            printf("New_package! Type:%d\n", type);
            if (type == MX_FILE_DOWNLOAD_TYPE) {
                mx_save_file_in_client(info, new_json);
            }
            else if (type == MX_AUTH_TYPE_V || type == MX_AUTH_TYPE_NV) {
                input_authentification(info, new_json);
            }
            else if (type == MX_REG_TYPE_V || type == MX_REG_TYPE_NV) {
                //input_registration(info, new_json);
            }
            else if (type == MX_MSG_TYPE) {
                input_message(info, new_json);
            }         
        }
    }

    return NULL;
}
