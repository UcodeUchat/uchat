#include "uchat.h"

static void update_profile (char *type, char *data, t_client_info *info) {
    json_object *new_json;
    const char *json_string;

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_EDIT_PROFILE_TYPE));
    json_object_object_add(new_json, "add_info", json_object_new_int(0));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    json_object_object_add(new_json, "column", json_object_new_string(type));
    json_object_object_add(new_json, "data", json_object_new_string (data));
    json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
}

void mx_edit_name_callback (GtkWidget *widget, t_client_info *info) {
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->name_entry), TRUE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->name_button2);
}

void mx_save_name_callback (GtkWidget *widget, t_client_info *info) {
    char *name = strdup(gtk_entry_get_text(GTK_ENTRY(info->data->profile->name_entry)));
    
    update_profile("name", name, info);
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->name_entry), FALSE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->name_button1);
}

void mx_edit_email_callback (GtkWidget *widget, t_client_info *info) {
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->email_entry), TRUE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->email_button2);
}

void mx_save_email_callback (GtkWidget *widget, t_client_info *info) {
    char *email = strdup(gtk_entry_get_text(GTK_ENTRY(info->data->profile->email_entry)));
 
    update_profile("email", email, info);
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->email_entry), FALSE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->email_button1);
}
