#include "uchat.h"


void mx_load_room_history (t_all *data) {
    struct json_object *messages;
    mx_js_o_o_get_ex(data->room_data, "messages", &messages);
    int n_msg = json_object_array_length(messages);
    for (int j = 0; j < n_msg; j++) {
        json_object *msg_data = json_object_array_get_idx(messages, j);
        mx_append_message(data->info, data->room, msg_data);
    }
    gtk_adjustment_set_value(data->room->Adjust, 
                            gtk_adjustment_get_upper(data->room->Adjust) - 
                            gtk_adjustment_get_page_size(data->room->Adjust) + 2.0);
}

void mx_init_room (t_client_info *info ,t_room *room, int position, json_object *room_data) {
    char *name = strdup(mx_js_g_str(mx_js_o_o_get(room_data, "name")));
    int id = mx_js_g_int(mx_js_o_o_get(room_data, "room_id"));
    int access = mx_js_g_int(mx_js_o_o_get(room_data, "access"));

    if (access == 3)
        room->name = mx_replace_substr (name, info->login, "");
    else
        room->name = strdup(name);
    room->position = position;
    room->id = id;
    room->access = access;
    room->messages = NULL;
    room->next = NULL;
    free(name);
}

void mx_init_room_menu(t_room *room, t_all *data) {
    GtkWidget *history = NULL;
    GtkWidget *leave = NULL;

    room->room_menu  = gtk_menu_new ();
    history = gtk_menu_item_new_with_label ("Load history");
    gtk_widget_show(history);
    gtk_menu_shell_append (GTK_MENU_SHELL (room->room_menu), history);
    g_signal_connect (G_OBJECT (history), "activate", G_CALLBACK (mx_scroll_callback), data);
    
    if (room->id != 0 && room->access != 3) {
        leave = gtk_menu_item_new_with_label ("Leave room");
        gtk_widget_show(leave);
        gtk_menu_shell_append (GTK_MENU_SHELL (room->room_menu), leave);
        g_signal_connect (G_OBJECT (leave), "activate", G_CALLBACK (mx_leave_callback), data);
    }
}


