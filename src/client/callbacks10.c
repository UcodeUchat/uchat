#include "uchat.h"

void mx_focus_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    
    (void)widget;
    (void)event;
    if (message != NULL)
        gtk_widget_show(message->menu);
}

void mx_focus1_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    t_message *message = mx_find_message(mes->room->messages, mes->id);

    (void)widget;
    (void)event;
    if (message != NULL)
        gtk_widget_hide(message->menu);
}

void mx_delete_callback (GtkWidget *widget, t_mes *mes) {
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    json_object  *new_json = json_object_new_object();
    const char *json_str = NULL;

    json_object_object_add(new_json, "type", json_object_new_int(MX_DELETE_MESSAGE_TYPE));
    json_object_object_add(new_json, "message_id", json_object_new_int(message->id));
    json_object_object_add(new_json, "room_id", json_object_new_int(mes->room->id));
    mx_print_json_object(new_json, "delete message");
    json_str = json_object_to_json_string(new_json);
    tls_send(mes->info->tls_client, json_str, strlen(json_str));
    (void)widget;
}

void mx_edit_callback (GtkWidget *widget, t_mes *mes) {
    t_message *message = mx_find_message(mes->room->messages, mes->id);

    (void)widget;
    if (message != NULL) {
        gtk_entry_set_text(GTK_ENTRY(mes->info->data->message_entry), message->data);
        gtk_widget_show(mes->info->data->edit_button);
        mes->info->editing_room = mes->room->id;
        mes->info->editing = message->id;
    }
}
