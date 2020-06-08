#include "uchat.h"

void mx_scroll_callback (GtkWidget *widget, t_all *data) {
    (void)widget;
    if (gtk_adjustment_get_value(data->room->Adjust) == 
        gtk_adjustment_get_lower(data->room->Adjust) && data->info->can_load == 1 
        && data->room->messages != NULL) {
        json_object  *new_json = json_object_new_object();

        data->info->can_load = 0;
        json_object_object_add(new_json, "type", json_object_new_int(MX_LOAD_MORE_TYPE));
        json_object_object_add(new_json, "room_id", json_object_new_int(data->room->id));
        json_object_object_add(new_json, "last_id", json_object_new_int(data->room->messages->id));
        mx_print_json_object(new_json, "load 15 more");
        const char *json_str = json_object_to_json_string(new_json);
        tls_send(data->info->tls_client, json_str, strlen(json_str));
    }
}


void mx_leave_callback (GtkWidget *widget, t_all *data) {
    json_object  *new_json = json_object_new_object();
    const char *json_str = NULL;

    json_object_object_add(new_json, "type", json_object_new_int(MX_LEAVE_ROOM_TYPE));
    json_object_object_add(new_json, "room_id", json_object_new_int(data->room->id));
    json_object_object_add(new_json, "access", json_object_new_int(data->room->access));
    json_object_object_add(new_json, "user_id", json_object_new_int(data->info->id));
    json_object_object_add(new_json, "login", json_object_new_string(data->info->login));
    json_str = json_object_to_json_string(new_json);
    tls_send(data->info->tls_client, json_str, strlen(json_str));
    (void)widget;
}

void mx_room_menu_callback(GtkWidget *widget, GdkEventButton *event, GtkWidget *menu) {
    (void)widget;
    (void)event;
    gtk_menu_popup_at_pointer (GTK_MENU(menu), NULL);
}

void mx_logout_client(t_client_info *info) {
    json_object *new_json;
    const char *json_string = NULL;

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_LOGOUT_TYPE));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    mx_print_json_object(new_json, "logout");
    json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
}


void mx_logout_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    info->auth_client = 0;
    mx_logout_client(info);
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Login");
    gtk_entry_set_text(GTK_ENTRY(info->data->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->password_entry), "");
    gtk_widget_destroy(info->data->general_box);
    gtk_widget_show (info->data->login_box);
}
