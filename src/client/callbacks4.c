#include "uchat.h"

void mx_close_creation_callback (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    gtk_widget_hide(info->data->create_room->main_box);
}

void mx_close_creation_callback1 (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_hide(info->data->create_room->main_box);
}

static void init_room_json (json_object *new_json, t_client_info *info) {
    json_object  *room_data = json_object_new_object();
    json_object *messages = json_object_new_array();

    json_object_object_add(new_json, "type", json_object_new_int(MX_CREATE_ROOM_TYPE));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    json_object_object_add(new_json, "name", json_object_new_string 
                            (gtk_entry_get_text(GTK_ENTRY(info->data->create_room->name_entry))));
    json_object_object_add(new_json, "acces", json_object_new_int 
                            (gtk_combo_box_get_active (GTK_COMBO_BOX(info->data->create_room->selection)) + 1));
    json_object_object_add(room_data, "messages", messages);
    json_object_object_add(room_data, "name", json_object_new_string 
                            (gtk_entry_get_text(GTK_ENTRY(info->data->create_room->name_entry))));
    json_object_object_add(new_json, "room_data", room_data);
}

void mx_create_room_callback (GtkWidget *widget, t_client_info *info) {
    if (strcmp ("", gtk_entry_get_text(
                GTK_ENTRY(info->data->create_room->name_entry))) != 0) {
        json_object *new_json = json_object_new_object();
        const char *json_string = NULL;

        gtk_widget_hide(info->data->create_room->main_box);
        init_room_json(new_json, info);
        json_string = json_object_to_json_string(new_json);
        tls_send(info->tls_client, json_string, strlen(json_string));
        gtk_entry_set_text(GTK_ENTRY(info->data->create_room->name_entry), "");
        gtk_widget_hide(info->data->menu);
    }
    (void)widget;
}

void mx_delete_acc_callback(GtkWidget *widget, GtkWidget *answer_menu) {
    (void)widget;
    gtk_menu_popup_at_pointer (GTK_MENU(answer_menu), NULL);
}
