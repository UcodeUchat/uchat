#include "uchat.h"

void mx_close_creation_callback (GtkWidget *widget, 
                GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    gtk_widget_hide(info->data->create_room->main_box);
}

void mx_close_creation_callback1 (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_hide(info->data->create_room->main_box);
}

static void init_room_json (json_object *new_json, t_client_info *info) {
    json_object  *room_data = mx_js_n_o();
    json_object *messages = json_object_new_array();

    mx_js_o_o_add(new_json, "type", mx_js_n_int(MX_CREATE_ROOM_TYPE));
    mx_js_o_o_add(new_json, "user_id", mx_js_n_int(info->id));
    mx_js_o_o_add(new_json, "name", mx_js_n_str (gtk_entry_get_text
                        (GTK_ENTRY(info->data->create_room->name_entry))));
    mx_js_o_o_add(new_json, "acces", mx_js_n_int (gtk_combo_box_get_active 
                        (GTK_COMBO_BOX(info->data->create_room->selection)) + 1));
    mx_js_o_o_add(room_data, "messages", messages);
    mx_js_o_o_add(room_data, "name", mx_js_n_str(gtk_entry_get_text
                        (GTK_ENTRY(info->data->create_room->name_entry))));
    mx_js_o_o_add(new_json, "room_data", room_data);
}

void mx_create_room_callback (GtkWidget *widget, t_client_info *info) {
    if (strcmp ("", gtk_entry_get_text
                (GTK_ENTRY(info->data->create_room->name_entry))) != 0) {
        json_object *new_json = mx_js_n_o();
        const char *json_string = NULL;

        gtk_widget_hide(info->data->create_room->main_box);
        init_room_json(new_json, info);
        json_string = mx_js_o_to_js_str(new_json);
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
