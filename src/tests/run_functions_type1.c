#include "uchat.h"

/*
static void add_room(t_client_info *info, json_object *new_json) {
    t_room *head = info->data->rooms;
    json_object *room_data = NULL;

    while (head != NULL) {
        head->position = head->position + 1;
        head = head->next;
    }  
    json_object_deep_copy(mx_js_o_o_get(new_json, "room_data"), &room_data, NULL);
    mx_push_room(info, room_data, 0);
}

*/
void mx_join_room_client(t_client_info *info, json_object *new_json) {
    int user_id = mx_js_g_int(mx_js_o_o_get(new_json, "user_id"));
    int room_id = mx_js_g_int(mx_js_o_o_get(new_json, "room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    if (user_id == info->id) {
        if (room == NULL) {
//            add_room (info, new_json);
        }
    }
    else {
        if (room != NULL) {
            mx_show_gif ("img/welcome.gif", " joined room", room, new_json);
        }
    }
}

int mx_notebook_detach(t_note *note) {
    int pos = gtk_notebook_page_num (GTK_NOTEBOOK(note->notebook),
                       note->box);
    gtk_notebook_remove_page (GTK_NOTEBOOK(note->notebook), pos);
    return 0;
}


void mx_edit_profile_client(t_client_info *info, json_object *new_json) {
    int confirmation = mx_js_g_int(mx_js_o_o_get(new_json, "confirmation"));

    if (confirmation) {
        info->audio = mx_js_g_int(mx_js_o_o_get(new_json, "audio_n"));
        info->visual = mx_js_g_int(mx_js_o_o_get(new_json, "visual_n"));
    }
}

void mx_show_gif(char *gif, char *event, t_room *room, json_object *new_json) {
    GtkWidget *h_box = gtk_box_new(FALSE, 5);
    char *label = mx_strjoin(mx_js_g_str(
                            mx_js_o_o_get(new_json, "login")), event);
    GtkWidget *login = gtk_label_new(label);
    GtkWidget *image_box = gtk_box_new(FALSE, 0);
    GtkWidget *image = gtk_image_new_from_file(gif);

    gtk_orientable_set_orientation (GTK_ORIENTABLE(h_box), 
                                    GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start (GTK_BOX (room->message_box), h_box, FALSE, FALSE, 0);
    gtk_widget_set_halign (login, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (h_box), login, FALSE, FALSE, 0);
    gtk_widget_show(login);
    gtk_widget_set_halign (image_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (h_box), image_box, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (image_box), image, FALSE, FALSE, 0);
    gtk_widget_show(image);
    gtk_widget_show(image_box);
    g_idle_add ((GSourceFunc)mx_show_widget, h_box);
}
