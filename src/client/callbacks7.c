#include "uchat.h"

static void server_send (t_client_info *info) {
    json_object *new_json;
    const char *json_string = NULL;
    char *message = strdup(gtk_entry_get_text(GTK_ENTRY(info->data->message_entry)));

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_EDIT_MESSAGE_TYPE));
    json_object_object_add(new_json, "login", json_object_new_string(info->login));
    json_object_object_add(new_json, "data", json_object_new_string (message));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    json_object_object_add(new_json, "room_id", json_object_new_int(info->editing_room));
    json_object_object_add(new_json, "message_id", json_object_new_int(info->editing));
    json_object_object_add(new_json, "add_info", json_object_new_int(0));
    json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
    gtk_widget_hide(info->data->edit_button);
    info->editing = -1;
}

void mx_send_callback (GtkWidget *widget, t_client_info *info) {
    if (strcmp(gtk_entry_get_text(GTK_ENTRY(info->data->message_entry))
                ,"") != 0) {
        if (info->editing < 0) {
            int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
            t_room *room = mx_find_room_position(info->data->rooms, position);
            info->data->current_room = room->id;
            mx_process_message_in_client(info);
        }
        else {
            server_send(info);
        }
        gtk_entry_set_text(GTK_ENTRY(info->data->message_entry), "");
    }
    (void)widget;
}

void mx_choose_file_callback(GtkWidget *widget, t_client_info *info) {
    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
    t_room *room = mx_find_room_position(info->data->rooms, position);

    info->data->current_room = room->id;
    mx_send_file_from_client(info, NULL);
    (void)widget;
}

void mx_choose_sticker_callback(GtkWidget *widget, GtkWidget *menu) {
    (void)widget;
    gtk_menu_popup_at_pointer (GTK_MENU(menu), NULL);
}

void mx_item_callback (GtkWidget *widget, t_stik *stik) {
    json_object *new_json;
    const char *json_string = NULL;
    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(stik->info->data->notebook));
    t_room *room = mx_find_room_position(stik->info->data->rooms, position);

    stik->info->data->current_room = room->id;
    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_MSG_TYPE));
    json_object_object_add(new_json, "login", json_object_new_string(stik->info->login));
    json_object_object_add(new_json, "data", json_object_new_string(stik->name));
    json_object_object_add(new_json, "user_id", json_object_new_int(stik->info->id));
    json_object_object_add(new_json, "room_id", json_object_new_int(stik->info->data->current_room));
    json_object_object_add(new_json, "add_info", json_object_new_int(3));
    json_string = json_object_to_json_string(new_json);
    tls_send(stik->info->tls_client, json_string, strlen(json_string));
    (void)widget;
}
