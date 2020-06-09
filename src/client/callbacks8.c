#include "uchat.h"

static void update_profile (char *type, char *data, t_client_info *info) {
    json_object *new_json;
    const char *json_string;

    new_json = mx_js_n_o();
    mx_js_o_o_add(new_json, "type", mx_js_n_int(MX_EDIT_PROFILE_TYPE));
    mx_js_o_o_add(new_json, "add_info", mx_js_n_int(0));
    mx_js_o_o_add(new_json, "user_id", mx_js_n_int(info->id));
    mx_js_o_o_add(new_json, "column", mx_js_n_str(type));
    mx_js_o_o_add(new_json, "data", mx_js_n_str (data));
    json_string = mx_js_o_to_js_str(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
}

void mx_edit_name_callback (GtkWidget *widget, t_client_info *info) {
    gtk_editable_set_editable (GTK_EDITABLE 
            (info->data->profile->name_entry), TRUE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->name_button2);
}

void mx_save_name_callback (GtkWidget *widget, t_client_info *info) {
    char *name = strdup(gtk_entry_get_text
        (GTK_ENTRY(info->data->profile->name_entry)));
    
    update_profile("name", name, info);
    gtk_editable_set_editable
            (GTK_EDITABLE (info->data->profile->name_entry), FALSE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->name_button1);
}

void mx_edit_email_callback (GtkWidget *widget, t_client_info *info) {
    gtk_editable_set_editable
            (GTK_EDITABLE (info->data->profile->email_entry), TRUE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->email_button2);
}

void mx_save_email_callback (GtkWidget *widget, t_client_info *info) {
    char *email = strdup(gtk_entry_get_text
        (GTK_ENTRY(info->data->profile->email_entry)));
 
    update_profile("email", email, info);
    gtk_editable_set_editable (GTK_EDITABLE
                    (info->data->profile->email_entry), FALSE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->email_button1);
}
