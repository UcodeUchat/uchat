#include "uchat.h"

static int validation (char *login, char *password, char *repeat_password) {
    if (!login || strlen(login) < 6) {
        printf("Твой логин хуйня\n");
        return 0;
    }
    if (!password || strlen(password) < 6) {
        printf("Твой пароль хуйня\n");
        return 0;
    }
    if (strcmp(password, repeat_password) != 0) {
        printf("Пароли не совпадают\n");
        return 0;
    }
    return 1;
}

static void *register_msg_thread (void *data) {
    t_client_info *n_data = (t_client_info *)data;

    sleep(3);
    gtk_widget_hide(n_data->data->register_msg);
    return 0;
}

static void sever_register (char *login, char *password, t_client_info *info) {
    json_object *json_obj = mx_create_basic_json_object(MX_REG_TYPE);
    const char *json_str;

    json_object_object_add(json_obj, "login", json_object_new_string(login));
    json_object_object_add(json_obj, "password", json_object_new_string(password));
    json_str = json_object_to_json_string(json_obj);
    tls_send(info->tls_client, json_str, strlen(json_str));
    gtk_widget_hide(info->data->register_box);
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->repeat_password_entry), "");
    gtk_widget_show(info->data->login_box);
}

void mx_send_data_callback (GtkWidget *widget, t_client_info *info) {
    char *login = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->login_entry));
    char *password = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->password_entry));
    char *repeat = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->repeat_password_entry));

    if (validation(login, password, repeat)) {
        sever_register (login, password, info);
    }
    else {
        pthread_cancel(info->data->register_msg_t);
        gtk_widget_show(info->data->register_msg);
        pthread_create(&info->data->register_msg_t, 0, register_msg_thread, info);
    }
    (void)widget;
}

void mx_cancel_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Sign in");
    gtk_widget_hide(info->data->register_box);
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->repeat_password_entry), "");
    gtk_widget_show(info->data->login_box);
}
