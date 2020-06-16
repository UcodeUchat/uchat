#include "uchat.h"

static void server_send (t_client_info *info) {
    json_object *new_json;
    const char *json_string = NULL;
    char *message = strdup(gtk_entry_get_text(
                            GTK_ENTRY(info->data->message_entry)));

    new_json = mx_js_n_o();
    mx_js_o_o_add(new_json, "type", mx_js_n_int(MX_EDIT_MESSAGE_TYPE));
    mx_js_o_o_add(new_json, "login", mx_js_n_str(info->login));
    mx_js_o_o_add(new_json, "data", mx_js_n_str(message));
    mx_js_o_o_add(new_json, "user_id", mx_js_n_int(info->id));
    mx_js_o_o_add(new_json, "room_id", mx_js_n_int(info->editing_room));
    mx_js_o_o_add(new_json, "message_id", mx_js_n_int(info->editing));
    mx_js_o_o_add(new_json, "add_info", mx_js_n_int(0));
    json_string = mx_js_o_to_js_str(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
    gtk_widget_hide(info->data->edit_button);
    info->editing = -1;
}

void mx_send_callback (GtkWidget *widget, t_client_info *info) {
    if (strcmp(gtk_entry_get_text(GTK_ENTRY(info->data->message_entry))
                ,"") != 0) {
        if (info->editing < 0) {
            int position = gtk_notebook_get_current_page(
                            GTK_NOTEBOOK(info->data->notebook));
            t_room *room = mx_find_room_position(info->data->rooms, position);
            info->data->current_room = room->id;
//            mx_process_message_in_client(info);
        }
        else {
            server_send(info);
        }
        gtk_entry_set_text(GTK_ENTRY(info->data->message_entry), "");
    }
    (void)widget;
}

void mx_item_callback (GtkWidget *widget, t_stik *stik) {
    json_object *new_json;
    const char *json_string = NULL;
    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(
                                        stik->info->data->notebook));
    t_room *room = mx_find_room_position(stik->info->data->rooms, position);

    stik->info->data->current_room = room->id;
    new_json = mx_js_n_o();
    mx_js_o_o_add(new_json, "type", mx_js_n_int(MX_MSG_TYPE));
    mx_js_o_o_add(new_json, "login", mx_js_n_str(stik->info->login));
    mx_js_o_o_add(new_json, "data", mx_js_n_str(stik->name));
    mx_js_o_o_add(new_json, "user_id", mx_js_n_int(stik->info->id));
    mx_js_o_o_add(new_json, "room_id", 
                    mx_js_n_int(stik->info->data->current_room));
    mx_js_o_o_add(new_json, "add_info", mx_js_n_int(3));
    json_string = mx_js_o_to_js_str(new_json);
    tls_send(stik->info->tls_client, json_string, strlen(json_string));
    (void)widget;
}

void mx_show_search_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show_all(info->data->search_box);
}
