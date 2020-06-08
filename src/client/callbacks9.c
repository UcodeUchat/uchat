#include "uchat.h"

void mx_close_profile_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_destroy(info->data->profile->main_box);
    free(info->data->profile);
    info->data->profile = NULL;
}

void mx_close_profile_callback1 (GtkWidget *widget, 
	GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    gtk_widget_destroy(info->data->profile->main_box);
    free(info->data->profile);
    info->data->profile = NULL;
}

void mx_join_callback (GtkWidget *widget, t_all *data) {
    json_object  *new_json = json_object_new_object();
    json_object  *room_data = json_object_new_object();
    const char *json_str = NULL;

    gtk_widget_set_sensitive (widget, FALSE);
    gtk_button_set_label(GTK_BUTTON(widget), "Processing");
    json_object_object_add(new_json, "type", json_object_new_int(MX_JOIN_ROOM_TYPE));
    json_object_object_add(new_json, "room_id", json_object_new_int(data->room_id));
    json_object_object_add(new_json, "user_id", json_object_new_int(data->info->id));
    json_object_object_add(new_json, "login", json_object_new_string(data->info->login));
    json_object_object_add(room_data, "name", json_object_new_string(data->room_name));
    json_object_object_add(room_data, "room_id", json_object_new_int(data->room_id));
    json_object_object_add(new_json, "room_data", room_data);
    json_str = json_object_to_json_string(new_json);
    tls_send(data->info->tls_client, json_str, strlen(json_str));
    if (data->info->data->search != NULL) {
        gtk_widget_hide(data->info->data->search->main_box);
        free(data->info->data->search);
        data->info->data->search = NULL;
    }
}
