#include "uchat.h"

void mx_edit_message_client(t_client_info *info, json_object *new_json) { 
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

void mx_input_authentification(t_client_info *info, json_object *new_json) {
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

void mx_load_history_client(t_client_info *info, json_object *new_json) {
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
