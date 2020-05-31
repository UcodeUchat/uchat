#include "uchat.h"

void close_result_callback (GtkWidget *widget, GdkEventButton *event, t_search *search) {
    (void)widget;
    (void)event;
    if (search != NULL) {
        gtk_widget_hide(search->main_box);
        free(search);
        search = NULL;
    }
}

void join_callback (GtkWidget *widget, t_all *data) {
    gtk_button_set_relief (GTK_BUTTON(widget),  GTK_RELIEF_NONE);
    gtk_widget_set_sensitive (widget, FALSE);
    gtk_button_set_label(GTK_BUTTON(widget), "Processing");
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

void init_rooms (t_client_info *info, json_object *new_json, GtkWidget *v_box) {
    GtkWidget *h_box = gtk_box_new(FALSE, 5);
    gtk_widget_set_size_request(h_box, -1, 35);
    gtk_box_pack_start (GTK_BOX (v_box), h_box, FALSE, FALSE, 0);
    gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign (h_box, GTK_ALIGN_START);
    GtkWidget *title = gtk_label_new("Rooms:\n");
    gtk_widget_set_valign (title, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (h_box), title, FALSE, FALSE, 0);

    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request(scrolled_window, -1, 150);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolled_window),
                                GTK_POLICY_AUTOMATIC,
                                GTK_POLICY_ALWAYS);
    gtk_box_pack_start (GTK_BOX (v_box), scrolled_window, FALSE, FALSE, 0);
    gtk_widget_set_name(scrolled_window, "rooms_w");
    GtkWidget *room_box = gtk_box_new(FALSE, 5);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(room_box), GTK_ORIENTATION_VERTICAL);
    gtk_container_add (GTK_CONTAINER (scrolled_window), room_box);

    struct json_object *rooms;
    json_object_object_get_ex(new_json, "rooms", &rooms);
    int n_rooms = json_object_array_length(rooms);
    if (!n_rooms) {
        h_box = gtk_box_new(FALSE, 5);
        gtk_widget_set_size_request(h_box, -1, 41);
        gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
        gtk_box_pack_start (GTK_BOX (room_box), h_box, FALSE, FALSE, 0);
        GtkWidget *empty_label = gtk_label_new("No rooms found");
        gtk_box_pack_start (GTK_BOX (h_box), empty_label, FALSE, FALSE, 0); 
    }
    for (int i = 0; i < n_rooms; i++) {
        json_object *room_data = json_object_array_get_idx(rooms, i);
        const char *name = json_object_get_string(json_object_object_get(room_data, "name"));
        int id = json_object_get_int(json_object_object_get(room_data, "id"));
        int acces = json_object_get_int(json_object_object_get(room_data, "acces"));
        h_box = gtk_box_new(FALSE, 5);
        gtk_widget_set_size_request(h_box, -1, 41);
        gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
        gtk_box_pack_start (GTK_BOX (room_box), h_box, FALSE, FALSE, 0);
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
        GtkWidget *separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
        gtk_box_pack_start (GTK_BOX (room_box), separator, FALSE, FALSE, 0);
    }
}

void open_menu_callback1(GtkWidget *widget, GdkEventButton *event, GtkWidget *menu) {
    (void)widget;
    (void)event;
    gtk_menu_popup_at_pointer (GTK_MENU(menu), NULL);
}

void load_profile_callback1(GtkWidget *widget, t_mes *mes) {
    (void)widget;
    mx_load_profile_client(mes->info, mes->user_id);
}

void init_users (t_client_info *info, json_object *new_json, GtkWidget *v_box) {
    GtkWidget *h_box = gtk_box_new(FALSE, 5);
    gtk_widget_set_size_request(h_box, -1, 35);
    gtk_box_pack_start (GTK_BOX (v_box), h_box, FALSE, FALSE, 0);
    gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign (h_box, GTK_ALIGN_START);
    GtkWidget *title = gtk_label_new("Users:\n");
    gtk_widget_set_valign (title, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (h_box), title, FALSE, FALSE, 0);

    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request(scrolled_window, -1, 150);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolled_window),
                                GTK_POLICY_AUTOMATIC,
                                GTK_POLICY_ALWAYS);
    gtk_box_pack_start (GTK_BOX (v_box), scrolled_window, FALSE, FALSE, 0);
    gtk_widget_set_name(scrolled_window, "rooms_w");
    GtkWidget *room_box = gtk_box_new(FALSE, 5);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(room_box), GTK_ORIENTATION_VERTICAL);
    gtk_container_add (GTK_CONTAINER (scrolled_window), room_box);

    struct json_object *users;
    json_object_object_get_ex(new_json, "users", &users);
    int n_users = json_object_array_length(users);
    if (!n_users) {
        h_box = gtk_box_new(FALSE, 5);
        gtk_widget_set_size_request(h_box, -1, 41);
        gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
        gtk_box_pack_start (GTK_BOX (room_box), h_box, FALSE, FALSE, 0);
        GtkWidget *empty_label = gtk_label_new("No users found");
        gtk_box_pack_start (GTK_BOX (h_box), empty_label, FALSE, FALSE, 0); 
    }
    for (int i = 0; i < n_users; i++) {
        json_object *user_data = json_object_array_get_idx(users, i);
        const char *login = json_object_get_string(json_object_object_get(user_data, "login"));
        int id = json_object_get_int(json_object_object_get(user_data, "id"));
        if (id != info->id) {
            h_box = gtk_box_new(FALSE, 5);
            gtk_widget_set_size_request(h_box, -1, 41);
            gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
            gtk_box_pack_start (GTK_BOX (room_box), h_box, FALSE, FALSE, 0);

            GtkWidget *login_event = gtk_event_box_new();
            //--
            GtkWidget *login_menu  = gtk_menu_new ();
            GtkWidget *view = gtk_menu_item_new_with_label("View profile");
            gtk_widget_show(view);
            gtk_menu_shell_append (GTK_MENU_SHELL (login_menu), view);
            t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
            mes->info = info;
            mes->user_id = id;
            g_signal_connect (G_OBJECT (view), "activate", G_CALLBACK (load_profile_callback1), mes);
            //--
            gtk_widget_add_events (login_event, GDK_BUTTON_PRESS_MASK);
            g_signal_connect (G_OBJECT (login_event), "button_press_event", 
                                G_CALLBACK (open_menu_callback1), G_OBJECT(login_menu));
            GtkWidget *label = gtk_label_new(login);
            gtk_container_add(GTK_CONTAINER(login_event), label);
            gtk_box_pack_start(GTK_BOX (h_box), login_event, FALSE, FALSE, 0);

            GtkWidget *separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
            gtk_box_pack_start (GTK_BOX (room_box), separator, FALSE, FALSE, 0);
        }
    }
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
    gtk_widget_set_size_request(search_main, 300, -1);
    gtk_box_pack_start (GTK_BOX (info->data->search->main_box), search_main, FALSE, FALSE, 0);
    gtk_widget_set_name (search_main, "profile");
    GtkWidget *v_box = gtk_box_new(FALSE, 1);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(v_box), GTK_ORIENTATION_VERTICAL);
    gtk_container_add (GTK_CONTAINER (search_main), v_box);
    //--rooms
    init_rooms (info, new_json, v_box);
    //--users
    init_users (info, new_json, v_box);
    //--exit
    GtkWidget *search_exit = gtk_event_box_new();
    gtk_box_pack_start (GTK_BOX (info->data->search->main_box), search_exit, TRUE, TRUE, 0);
    gtk_widget_add_events (search_exit, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (search_exit), "button_press_event", G_CALLBACK (close_result_callback), info->data->search);
    gtk_widget_set_name (search_exit, "search_exit");
    gtk_widget_show_all(info->data->search->main_box);
}
