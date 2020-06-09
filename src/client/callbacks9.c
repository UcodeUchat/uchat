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
    json_object  *new_json = mx_js_n_o();
    json_object  *room_data = mx_js_n_o();
    const char *json_str = NULL;

    gtk_widget_set_sensitive (widget, FALSE);
    gtk_button_set_label(GTK_BUTTON(widget), "Processing");
    mx_js_o_o_add(new_json, "type", mx_js_n_int(MX_JOIN_ROOM_TYPE));
    mx_js_o_o_add(new_json, "room_id", mx_js_n_int(data->room_id));
    mx_js_o_o_add(new_json, "user_id", mx_js_n_int(data->info->id));
    mx_js_o_o_add(new_json, "login", mx_js_n_str(data->info->login));
    mx_js_o_o_add(room_data, "name", mx_js_n_str(data->room_name));
    mx_js_o_o_add(room_data, "room_id", mx_js_n_int(data->room_id));
    mx_js_o_o_add(new_json, "room_data", room_data);
    json_str = mx_js_o_to_js_str(new_json);
    tls_send(data->info->tls_client, json_str, strlen(json_str));
    if (data->info->data->search != NULL) {
        gtk_widget_hide(data->info->data->search->main_box);
        free(data->info->data->search);
        data->info->data->search = NULL;
    }
}
