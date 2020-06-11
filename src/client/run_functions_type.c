#include "uchat.h"

static void init_direect_message(t_client_info *info, json_object *new_json) {
    t_room *head = info->data->rooms;
    json_object *room_data = NULL;

    while (head != NULL) {
        head->position = head->position + 1;
        head = head->next;
    }
    json_object_deep_copy(mx_js_o_o_get(new_json, "room_data"), &room_data, NULL);
    mx_push_room(info, room_data, 0);
}

void mx_direct_message_client(t_client_info *info, json_object *new_json) {
    int exist = mx_js_g_int(mx_js_o_o_get(new_json,"exist"));
    int room_id = mx_js_g_int(mx_js_o_o_get(new_json,"room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    if (exist && room != NULL) {
        gtk_notebook_set_current_page (GTK_NOTEBOOK(info->data->notebook), 
                                        room->position);
    }
    else if (!exist && room == NULL) { 
        init_direect_message (info, new_json);
    }
}

static void pop_room_id(t_room **rooms, int id) {
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

static void exit_room(t_client_info *info, t_room *room, int room_id) {
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

void mx_leave_room_client(t_client_info *info, json_object *new_json) {
    int user_id = mx_js_g_int(mx_js_o_o_get(new_json, "user_id"));
    int room_id = mx_js_g_int(mx_js_o_o_get(new_json, "room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    if (room != NULL) {
        if (user_id == info->id) {
            exit_room (info, room, room_id);
        }
        else {
            mx_show_gif ("img/leave.gif", " leave room", room, new_json);
        }
    }
}
