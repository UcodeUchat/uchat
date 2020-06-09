#include "uchat.h"

void mx_menu_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show(info->data->menu);

}

void mx_edit_cancel_callback (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    info->editing = -1;
    gtk_entry_set_text(GTK_ENTRY(info->data->message_entry), "");
    gtk_widget_hide(info->data->edit_button);
}

void mx_load_profile_client(t_client_info *info, int id) {
    json_object *new_json;

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_LOAD_PROFILE_TYPE));
    json_object_object_add(new_json, "id", json_object_new_int(id));
    const char *json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
    gtk_widget_hide(info->data->menu);
}

void mx_profile_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    mx_load_profile_client(info, info->id);
}

void mx_create_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show(info->data->create_room->main_box);
}
