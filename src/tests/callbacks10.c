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
    json_object  *new_json = mx_js_n_o();
    const char *json_str = NULL;

    mx_js_o_o_add(new_json, "type", mx_js_n_int(MX_DELETE_MESSAGE_TYPE));
    mx_js_o_o_add(new_json, "message_id", mx_js_n_int(message->id));
    mx_js_o_o_add(new_json, "room_id", mx_js_n_int(mes->room->id));
    mx_print_json_object(new_json, "delete message");
    json_str = mx_js_o_to_js_str(new_json);
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
