#include "uchat.h"

void mx_url_callback (GtkWidget *widget, GdkEventButton *event, void *data) {
    (void)widget;
    (void)event;
    (void)data;
    system("open https://ucode.world");
}

void mx_reg_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;

    gtk_window_set_title(GTK_WINDOW(info->data->window), "Registration");
    gtk_widget_hide(info->data->login_box);
    gtk_widget_show (info->data->register_box);
}

void mx_close_menu_callback (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    gtk_widget_hide(info->data->menu);
}

static void delete_account(t_client_info *info) {
    json_object *new_json;

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_DELETE_ACCOUNT_TYPE));
    json_object_object_add(new_json, "login", json_object_new_string(info->login));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    mx_print_json_object(new_json, "delete_account");
    const char *json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
}

void mx_yep_callback(GtkWidget *widget, t_client_info *info) {
    (void)widget;
    info->auth_client = 0;
    delete_account(info);
    mx_logout_client(info);
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Login");
    gtk_entry_set_text(GTK_ENTRY(info->data->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->password_entry), "");
    gtk_widget_destroy(info->data->general_box);
    gtk_widget_show (info->data->login_box);
}
