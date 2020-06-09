#include "uchat.h"

void static pop_message_id(t_message *messages, int id) {
    t_message *head = messages;

    while (head->next != NULL) {
        if (head->next->id == id) {
            head->next = head->next->next;
            break;
        }
        head = head->next;
    }
}

static int notebook_reorder(t_note *note) {
    gtk_notebook_reorder_child(GTK_NOTEBOOK(note->notebook), 
        note->box, note->position);
    return 0;
}

void mx_delete_message_client(t_client_info *info, json_object *new_json) { 
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

void mx_input_message(t_client_info *info, json_object *new_json) {
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
