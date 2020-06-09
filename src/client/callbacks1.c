#include "uchat.h"

static void *login_msg_thread (void *data) {
    t_client_info *n_data = (t_client_info *)data;

    sleep(3);
    gtk_widget_hide(n_data->data->login_msg);
    gtk_widget_hide(n_data->data->stop);
    return 0;
}

static void authentification(t_client_info *info) {
    json_object *js;
    const char *json_string = NULL;

    js = mx_js_n_o();
    mx_js_o_o_add(js, "type", mx_js_n_int(MX_AUTH_TYPE));
    mx_js_o_o_add(js, "login", mx_js_n_str(info->login));
    mx_js_o_o_add(js, "password", mx_js_n_str(info->password));
    mx_print_json_object(js, "login authentification");
    json_string = mx_js_o_to_js_str(js);
    tls_send(info->tls_client, json_string, strlen(json_string));
    while (info->responce == 0) {
        (void)info;
    }
    info->responce = 0;
}

void mx_enter_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    info->login = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->login_entry));
    info->password = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->password_entry));
    authentification(info);
    if (info->auth_client == 0) {
        pthread_cancel(info->data->login_msg_t);
        gtk_widget_show(info->data->login_msg);
        gtk_widget_show(info->data->stop);
        pthread_create(&info->data->login_msg_t, 0, login_msg_thread, info);
    }
    else if(info->auth_client == 1) {
        mx_init_general (info);
        mx_init_menu (info);
        mx_init_search (info);
        mx_init_create (info);
    }  
}

void mx_search_callback (GtkWidget *widget, t_client_info *info) {
    json_object *new_json;
    const char *json_string = NULL;

    gtk_widget_hide(info->data->search_box);
    new_json = mx_js_n_o();
    mx_js_o_o_add(new_json, "type", mx_js_n_int(MX_SEARCH_ALL_TYPE));
    mx_js_o_o_add(new_json, "user_id", mx_js_n_int(info->id));
    mx_js_o_o_add(new_json, "query", mx_js_n_str 
                            (gtk_entry_get_text(GTK_ENTRY(info->data->search_entry))));
    json_string = mx_js_o_to_js_str(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
    gtk_entry_set_text(GTK_ENTRY(info->data->search_entry), "");
    (void)widget;
}

void mx_show_search_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show_all(info->data->search_box);
}
