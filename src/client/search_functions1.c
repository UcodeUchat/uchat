#include "uchat.h"

static void show_room_name (json_object *room_data, GtkWidget *h_box) {
    const char *name = name = mx_js_g_str(mx_js_o_o_get(room_data, "name"));
    GtkWidget *label = gtk_label_new(name);
    
    gtk_box_pack_start (GTK_BOX (h_box), label, FALSE, FALSE, 0);
}

static void show_room_access (int acces, GtkWidget *button, GtkWidget *h_box, t_all *data) {
    if (acces == 1) {
        button = gtk_button_new_with_label("Join room");
        g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (mx_join_callback), data);
        gtk_box_pack_end (GTK_BOX (h_box), button, FALSE, FALSE, 0);
    }
    else {
        button = gtk_label_new("Private");
        gtk_box_pack_end (GTK_BOX (h_box), button, FALSE, FALSE, 0);
    }
}

static void show_room_data (json_object *room_data, GtkWidget *h_box, t_client_info *info) {
    int id = mx_js_g_int(mx_js_o_o_get(room_data, "id"));
    int acces = mx_js_g_int(mx_js_o_o_get(room_data, "acces"));
    t_all *data = (t_all *)malloc(sizeof(t_all));
    t_room *room = mx_find_room(info->data->rooms, id);
    GtkWidget *button = NULL;

    data->info = info;
    data->room_id = id;
    data->room_name = strdup(mx_js_g_str(mx_js_o_o_get(room_data, "name")));
    if (room) {
        button = gtk_label_new ("Already joined");
        gtk_box_pack_end (GTK_BOX (h_box), button, FALSE, FALSE, 0);
    } 
    else {
        show_room_access (acces, button, h_box, data);
    }
}

void mx_show_rooms_result (GtkWidget *room_box, int n_rooms, 
                        json_object *rooms, t_client_info *info) {
    json_object *room_data = NULL;
    GtkWidget *h_box = NULL;
    GtkWidget *separator = NULL;

    for (int i = 0; i < n_rooms; i++) {
        room_data = json_object_array_get_idx(rooms, i);
        h_box = mx_init_search_h_box(room_box);

        show_room_name (room_data, h_box);
        show_room_data (room_data, h_box, info);
        separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
        gtk_box_pack_start (GTK_BOX (room_box), separator, FALSE, FALSE, 0);
    }
}
