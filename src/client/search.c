#include "uchat.h"

void close_result_callback (GtkWidget *widget, GdkEventButton *event, t_search *search) {
    (void)widget;
    (void)event;
    gtk_widget_hide(search->main_box);
    free(search);
}

void join_callback (GtkWidget *widget, t_all *data) {
    gtk_widget_hide(widget);
    //gtk_widget_set_can_focus (widget, FALSE);
    //gtk_button_set_label(GTK_BUTTON(widget), "Processing");
    json_object  *new_json = json_object_new_object();
    json_object  *room_data = json_object_new_object();

    json_object_object_add(new_json, "type", json_object_new_int(MX_JOIN_ROOM_TYPE));
    json_object_object_add(new_json, "room_id", json_object_new_int(data->room_id));
    json_object_object_add(new_json, "user_id", json_object_new_int(data->info->id));
    json_object_object_add(new_json, "login", json_object_new_string(data->info->login));
    json_object_object_add(room_data, "name", json_object_new_string(data->room_name));
    json_object_object_add(room_data, "room_id", json_object_new_int(data->room_id));
    json_object_object_add(new_json, "room_data", room_data);
    const char *json_str = json_object_to_json_string(new_json);
    tls_send(data->info->tls_client, json_str, strlen(json_str));
}

void mx_search_all_client(t_client_info *info, json_object *new_json) {
    info->data->search = (t_search *)malloc(sizeof(t_search));
    info->data->search->main_box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(info->data->search->main_box, 
                            gtk_widget_get_allocated_width (info->data->window), 
                            gtk_widget_get_allocated_height (info->data->window));
    gtk_fixed_put(GTK_FIXED(info->data->general_box),
                    info->data->search->main_box, 0, 0);
    GtkWidget *search_main = gtk_event_box_new();
    gtk_widget_set_size_request(search_main, 450, -1);
    gtk_box_pack_start (GTK_BOX (info->data->search->main_box), search_main, FALSE, FALSE, 0);
    gtk_widget_set_name (search_main, "profile");
    GtkWidget *v_box = gtk_box_new(FALSE, 5);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(v_box), GTK_ORIENTATION_VERTICAL);
    gtk_container_add (GTK_CONTAINER (search_main), v_box);
    //--rooms
    GtkWidget *title = gtk_label_new("Rooms:\n");
    gtk_box_pack_start (GTK_BOX (v_box), title, FALSE, FALSE, 0);
    GtkWidget *separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start (GTK_BOX (v_box), separator, FALSE, FALSE, 0);
    struct json_object *rooms;
    json_object_object_get_ex(new_json, "rooms", &rooms);
    int n_rooms = json_object_array_length(rooms);
    for (int i = 0; i < n_rooms; i++) {
        json_object *room_data = json_object_array_get_idx(rooms, i);
        const char *name = json_object_get_string(json_object_object_get(room_data, "name"));
        int id = json_object_get_int(json_object_object_get(room_data, "id"));
        int acces = json_object_get_int(json_object_object_get(room_data, "acces"));
        GtkWidget *h_box = gtk_box_new(FALSE, 5);
        gtk_box_pack_start (GTK_BOX (v_box), h_box, FALSE, FALSE, 0);
        GtkWidget *label = gtk_label_new(name);
        gtk_box_pack_start (GTK_BOX (h_box), label, FALSE, FALSE, 0);
        t_all *data = (t_all *)malloc(sizeof(t_all));
        data->info = info;
        data->room_id = id;
        data->room_name = strdup(json_object_get_string(json_object_object_get(room_data, "name")));
        t_room *room = mx_find_room(info->data->rooms, id);
        GtkWidget *button = NULL;
        if (room) {
            button = gtk_label_new("Already joined");
            gtk_box_pack_end (GTK_BOX (h_box), button, FALSE, FALSE, 0);
        } else {
            if (acces == 1) {
                button = gtk_button_new_with_label("Join room");
                g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (join_callback), data);
                gtk_box_pack_end (GTK_BOX (h_box), button, FALSE, FALSE, 0);
            }
            else {
                button = gtk_label_new("Private");
                gtk_box_pack_end (GTK_BOX (h_box), button, FALSE, FALSE, 0);
            }
        }
        separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
        gtk_box_pack_start (GTK_BOX (v_box), separator, FALSE, FALSE, 0);
    }
    //--users

    //--exit
    GtkWidget *search_exit = gtk_event_box_new();
    gtk_box_pack_start (GTK_BOX (info->data->search->main_box), search_exit, TRUE, TRUE, 0);
    gtk_widget_add_events (search_exit, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (search_exit), "button_press_event", G_CALLBACK (close_result_callback), info->data->search);
    gtk_widget_set_name (search_exit, "search_exit");
    gtk_widget_show_all(info->data->search->main_box);
}
