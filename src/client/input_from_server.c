#include "uchat.h"

t_message *mx_find_message(t_message *messages, int id) {
   t_message *head = messages;
   t_message *node = NULL;

    while (head != NULL) {
        if (head->id == id) {
            node = head;
            break;
        }
        head = head->next;
    }
    return node;
}

static void additional_act1 (t_client_info *info, t_room *room, 
                            json_object *new_json, t_message *tmp) {
    int id = json_object_get_int(json_object_object_get(new_json, "id"));
    int add_info = json_object_get_int(json_object_object_get(new_json, "add_info"));
    if (add_info == 2) {
        t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
        mes->info = info;
        mes->room = room;
        mes->id = id;
        mes->message = tmp;
        mx_load_file(mes);
    }
}

void mx_append_message(t_client_info *info, t_room *room, 
                        json_object *new_json) {
    t_message *tmp;
    t_message **list = &room->messages;

    if (!list)
        return;
    tmp = mx_create_message(info, room, new_json, 2);  // Create new
    if (!tmp)
        return;
    if (*list == NULL) {  // Find Null-node
        *list = tmp;
    }
    else {
        tmp->next = *list;
        *list = tmp;
    }
    additional_act1 (info, room, new_json, tmp);
}

void pop_message_id(t_message *messages, int id) {
    t_message *head = messages;

    while (head->next != NULL) {
        if (head->next->id == id) {
            head->next = head->next->next;
            break;
        }
        head = head->next;
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

int notebook_reorder(t_note *note) {
    gtk_notebook_reorder_child(GTK_NOTEBOOK(note->notebook), 
        note->box, note->position);
    return 0;
}

void input_message(t_client_info *info, json_object *new_json) {
    int room_id = json_object_get_int(json_object_object_get(new_json, "room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    mx_push_message(info, room, new_json);
    t_room *head = info->data->rooms;
    while (head != NULL) {
        if (head && head->position < room->position) {
            head->position = head->position + 1;
        }
        head = head->next;
    }
    room->position = 0;
    t_note *note = (t_note *)malloc(sizeof(t_note));
    note->notebook = info->data->notebook;
    note->box = room->room_box;
    note->position = 0;
    g_idle_add ((GSourceFunc)notebook_reorder, note);
}

void load_history(t_client_info *info, json_object *new_json) {
    int room_id = json_object_get_int(json_object_object_get(new_json, "room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);           
    struct json_object *messages;

    json_object_object_get_ex(new_json, "messages", &messages);
    int n_msg = json_object_array_length(messages);
    for (int i = 0; i < n_msg; i++) {
        json_object *msg_data = json_object_array_get_idx(messages, i);
        mx_append_message(info, room, msg_data);
    }
    info->can_load = 1;
}

void delete_message(t_client_info *info, json_object *new_json) { 
    int room_id = json_object_get_int(json_object_object_get(new_json, "room_id"));
    int message_id = json_object_get_int(json_object_object_get(new_json, "message_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    if (message_id >= room->messages->id) {
        mx_sleep_ms(100);
        t_message *message = mx_find_message(room->messages, message_id);
        g_idle_add ((GSourceFunc)mx_destroy_widget, message->h_box);
        pop_message_id(room->messages, message_id);
    }
}

void edit_message(t_client_info *info, json_object *new_json) { 
    int room_id = json_object_get_int(json_object_object_get(new_json, "room_id"));
    int message_id = json_object_get_int(json_object_object_get(new_json, "message_id"));
    const char *data = json_object_get_string(json_object_object_get(new_json, "data"));
    t_room *room = mx_find_room(info->data->rooms, room_id);
    t_message *node = NULL;

    if (room && message_id >= room->messages->id) {
        mx_sleep_ms(50);
        node = mx_find_message(room->messages, message_id);
        if (node) {
            free(node->data);
            node->data = strdup(data);
            g_idle_add ((GSourceFunc)mx_destroy_widget, node->message_label);
            node->message_label = gtk_label_new(data);
            gtk_box_pack_start (GTK_BOX (node->message_box), 
                node->message_label, FALSE, FALSE, 0);
            g_idle_add ((GSourceFunc)mx_show_widget, node->message_label);
        }
    }
}

void input_authentification(t_client_info *info, json_object *new_json) {
    int type = json_object_get_int(json_object_object_get(new_json, "type"));
    int user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
    int visual = json_object_get_int(json_object_object_get(new_json, "visual"));
    int audio = json_object_get_int(json_object_object_get(new_json, "audio"));

    if ((*info).auth_client == 0) {
        if (type == 4) {
            info->id = user_id;
            info->visual = visual;
            info->audio = audio;
            (*info).auth_client = 1;
            json_object_put(info->rooms);
            json_object *rooms = NULL;
            json_object_deep_copy(json_object_object_get(new_json, "rooms"), &rooms, NULL);
            info->rooms = rooms;
        }
        else
            (*info).auth_client = 0;
        (*info).responce = 1;
    }
}


void pop_room_id(t_room **rooms, int id) {
    t_room *head = *rooms;

    if (head->id == id) {
        *rooms = head->next;
    }
    else {    
        while (head->next != NULL) {
            if (head->next->id == id) {
                head->next = head->next->next;
                break;
            }
            head = head->next;
        }
    }
}

void show_gif(char *gif, char *event, t_room *room, json_object *new_json) {
    GtkWidget *h_box = gtk_box_new(FALSE, 5);
    char *label = mx_strjoin(json_object_get_string(
                            json_object_object_get(new_json, "login")), event);
    GtkWidget *login = gtk_label_new(label);
    GtkWidget *image_box = gtk_box_new(FALSE, 0);
    GtkWidget *image = gtk_image_new_from_file(gif);

    gtk_orientable_set_orientation (GTK_ORIENTABLE(h_box), 
                                    GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start (GTK_BOX (room->message_box), h_box, FALSE, FALSE, 0);
    gtk_widget_set_halign (login, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (h_box), login, FALSE, FALSE, 0);
    gtk_widget_show(login);
    gtk_widget_set_halign (image_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (h_box), image_box, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (image_box), image, FALSE, FALSE, 0);
    gtk_widget_show(image);
    gtk_widget_show(image_box);
    g_idle_add ((GSourceFunc)mx_show_widget, h_box);
}

void add_room(t_client_info *info, json_object *new_json) {
    t_room *head = info->data->rooms;
    json_object *room_data = NULL;

    while (head != NULL) {
        head->position = head->position + 1;
        head = head->next;
    }  
    json_object_deep_copy(json_object_object_get(new_json, "room_data"), &room_data, NULL);
    mx_push_room(info, room_data, 0);
}

void join_room(t_client_info *info, json_object *new_json) {
    int user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(new_json, "room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    if (user_id == info->id) {
        if (room == NULL) {
            add_room (info, new_json);
        }
    }
    else {
        if (room != NULL) {
            show_gif ("img/welcome.gif", " joined room", room, new_json);
        }
    }
}

int mx_notebook_detach(t_note *note) {
    int pos = gtk_notebook_page_num (GTK_NOTEBOOK(note->notebook),
                       note->box);
    gtk_notebook_remove_page (GTK_NOTEBOOK(note->notebook), pos);
    return 0;
}

void exit_room(t_client_info *info, t_room *room, int room_id) {
    t_room *head = info->data->rooms;
    t_note *note = (t_note *)malloc(sizeof(t_note));

    while (head != NULL) {
        if (head && head->position > room->position)
            head->position = head->position - 1;
        head = head->next;
    }
    note->notebook = info->data->notebook;
    note->box = room->room_box;
    g_idle_add ((GSourceFunc)mx_notebook_detach, note);
    pop_room_id(&info->data->rooms, room_id);
}

void leave_room(t_client_info *info, json_object *new_json) {
    int user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(new_json, "room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    if (room != NULL) {
        if (user_id == info->id) {
            exit_room (info, room, room_id);
        }
        else {
            show_gif ("img/leave.gif", " lrgt room", room, new_json);
        }
    }
}

void edit_profile(t_client_info *info, json_object *new_json) {
    int confirmation = json_object_get_int(json_object_object_get(new_json, "confirmation"));

    if (confirmation) {
        info->audio = json_object_get_int(json_object_object_get(new_json, "audio_n"));
        info->visual = json_object_get_int(json_object_object_get(new_json, "visual_n"));
    }
}

void init_direect_message(t_client_info *info, json_object *new_json) {
    t_room *head = info->data->rooms;
    json_object *room_data = NULL;

    while (head != NULL) {
        head->position = head->position + 1;
        head = head->next;
    }
    json_object_deep_copy(json_object_object_get(new_json, "room_data"), &room_data, NULL);
    mx_push_room(info, room_data, 0);
}

void direct_message(t_client_info *info, json_object *new_json) {
    int exist = json_object_get_int(json_object_object_get(new_json, 
                                                            "exist"));
    int room_id = json_object_get_int(json_object_object_get(new_json, 
                                                            "room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    if (exist && room != NULL) {
        gtk_notebook_set_current_page (GTK_NOTEBOOK(info->data->notebook), 
                                        room->position);
    }
    else if (!exist && room == NULL) { 
            init_direect_message (info, new_json);
    }
}

void part1 (int type, json_object *obj, t_client_info *info) {
    if (type == MX_MSG_TYPE)
        input_message(info, obj);
    else if (type == MX_FILE_DOWNLOAD_TYPE) 
        mx_save_file_in_client(info, obj);
    else if (type == MX_AUTH_TYPE_V || type == MX_AUTH_TYPE_NV) 
        input_authentification(info, obj);
    else if (type == MX_LOAD_MORE_TYPE)
        load_history(info, obj);
    else if (type == MX_DELETE_MESSAGE_TYPE)
        delete_message(info, obj);
    else if (type == MX_EDIT_MESSAGE_TYPE)
        edit_message(info, obj);
    else if (type == MX_LOAD_PROFILE_TYPE)
        mx_load_user_profile(info, obj);
}

void part2 (int type, json_object *obj, t_client_info *info) {
    if (type == MX_LEAVE_ROOM_TYPE)
        leave_room(info, obj);
    else if (type == MX_JOIN_ROOM_TYPE)
        join_room(info, obj);
    else if (type == MX_CREATE_ROOM_TYPE)
        join_room(info, obj);
    else if (type == MX_EDIT_PROFILE_TYPE)
        edit_profile(info, obj);
    else if (type == MX_SEARCH_ALL_TYPE)
        mx_search_all_client(info, obj);
    else if (type == MX_DIRECT_MESSAGE_TYPE)
        direct_message(info, obj);
}

int mx_run_function_type_in_client(t_client_info *info, json_object *obj) {
    int type = json_object_get_int(json_object_object_get(obj, "type"));
    // if (type != MX_FILE_DOWNLOAD_TYPE) mx_print_json_object(obj, "mx_process_input_from_server");

    part1(type, obj, info);
    part2(type, obj, info);
    return 0;
}

void *mx_process_input_from_server(void *taken_info) {
    t_client_info *info = (t_client_info *)taken_info;
    int rc;
    char buffer[2048];
    json_tokener *tok = json_tokener_new();
    enum json_tokener_error jerr;
    json_object *new_json;

    while (1) { // read all input from server
        rc = tls_read(info->tls_client, buffer, sizeof(buffer));    // get json
        if (rc == -1) {
            if (mx_reconnect_client(info))
                break;
        }
        else if (rc != 0) {
            new_json = json_tokener_parse_ex(tok, buffer, rc);
            jerr = json_tokener_get_error(tok);
            if (jerr == json_tokener_success) {
                mx_run_function_type_in_client(info, new_json);
            }
            else if (jerr != json_tokener_continue) {
                fprintf(stderr, "Error: %s\n", json_tokener_error_desc(jerr));
            }
            json_object_put(new_json);
        }
    }
    return NULL;
}
