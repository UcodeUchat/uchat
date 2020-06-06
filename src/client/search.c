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
    json_object  *new_json = json_object_new_object();
    json_object  *room_data = json_object_new_object();
    const char *json_str = NULL;

    gtk_widget_set_sensitive (widget, FALSE);
    gtk_button_set_label(GTK_BUTTON(widget), "Processing");
    json_object_object_add(new_json, "type", json_object_new_int(MX_JOIN_ROOM_TYPE));
    json_object_object_add(new_json, "room_id", json_object_new_int(data->room_id));
    json_object_object_add(new_json, "user_id", json_object_new_int(data->info->id));
    json_object_object_add(new_json, "login", json_object_new_string(data->info->login));
    json_object_object_add(room_data, "name", json_object_new_string(data->room_name));
    json_object_object_add(room_data, "room_id", json_object_new_int(data->room_id));
    json_object_object_add(new_json, "room_data", room_data);
    json_str = json_object_to_json_string(new_json);
    tls_send(data->info->tls_client, json_str, strlen(json_str));
    if (data->info->data->search != NULL) {
        gtk_widget_hide(data->info->data->search->main_box);
        free(data->info->data->search);
        data->info->data->search = NULL;
    }
}

GtkWidget *init_content_box (GtkWidget *v_box, char *title) {
    GtkWidget *h_box = gtk_box_new(FALSE, 5);
    GtkWidget *label = gtk_label_new(title);
    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    GtkWidget *room_box = gtk_box_new(FALSE, 5);

    gtk_widget_set_size_request(h_box, -1, 35);
    gtk_box_pack_start (GTK_BOX (v_box), h_box, FALSE, FALSE, 0);
    gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign (h_box, GTK_ALIGN_START);
    gtk_widget_set_valign (label, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (h_box), label, FALSE, FALSE, 0);

    gtk_widget_set_size_request(scrolled_window, -1, 150);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolled_window),
                                GTK_POLICY_AUTOMATIC,
                                GTK_POLICY_ALWAYS);
    gtk_box_pack_start (GTK_BOX (v_box), scrolled_window, FALSE, FALSE, 0);
    gtk_widget_set_name(scrolled_window, "rooms_w");
    gtk_orientable_set_orientation (GTK_ORIENTABLE(room_box), GTK_ORIENTATION_VERTICAL);
    gtk_container_add (GTK_CONTAINER (scrolled_window), room_box);
    return room_box;
}

void show_empty_result (GtkWidget *room_box, char *title) {
    GtkWidget *h_box = gtk_box_new(FALSE, 5);
    GtkWidget *empty_label = gtk_label_new(title);

    gtk_widget_set_size_request(h_box, -1, 41);
    gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (room_box), h_box, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (h_box), empty_label, FALSE, FALSE, 0); 
}

GtkWidget *init_h_box (GtkWidget *room_box) {
    GtkWidget *h_box = gtk_box_new(FALSE, 5);

    gtk_widget_set_size_request(h_box, -1, 41);
    gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (room_box), h_box, FALSE, FALSE, 0);
    return h_box;
}

void show_room_name (json_object *room_data, GtkWidget *h_box) {
    const char *name = name = json_object_get_string(json_object_object_get(room_data, "name"));
    GtkWidget *label = gtk_label_new(name);
    
    gtk_box_pack_start (GTK_BOX (h_box), label, FALSE, FALSE, 0);
}

void show_room_access (int acces, GtkWidget *button, GtkWidget *h_box, t_all *data) {
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

void show_room_data (json_object *room_data, GtkWidget *h_box, t_client_info *info) {
    int id = json_object_get_int(json_object_object_get(room_data, "id"));
    int acces = json_object_get_int(json_object_object_get(room_data, "acces"));
    t_all *data = (t_all *)malloc(sizeof(t_all));
    t_room *room = mx_find_room(info->data->rooms, id);
    GtkWidget *button = NULL;

    data->info = info;
    data->room_id = id;
    data->room_name = strdup(json_object_get_string(json_object_object_get(room_data, "name")));
    if (room) {
        button = gtk_label_new ("Already joined");
        gtk_box_pack_end (GTK_BOX (h_box), button, FALSE, FALSE, 0);
    } 
    else {
        show_room_access (acces, button, h_box, data);
    }
}

void show_rooms_result (GtkWidget *room_box, int n_rooms, 
                        json_object *rooms, t_client_info *info) {
    json_object *room_data = NULL;
    GtkWidget *h_box = NULL;
    GtkWidget *separator = NULL;

    for (int i = 0; i < n_rooms; i++) {
        room_data = json_object_array_get_idx(rooms, i);
        h_box = init_h_box(room_box);

        show_room_name (room_data, h_box);
        show_room_data (room_data, h_box, info);
        separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
        gtk_box_pack_start (GTK_BOX (room_box), separator, FALSE, FALSE, 0);
    }
}

void init_rooms (t_client_info *info, json_object *new_json, GtkWidget *v_box) {
    struct json_object *rooms;
    int n_rooms;
    GtkWidget *room_box = init_content_box(v_box, "Rooms:\n");

    json_object_object_get_ex(new_json, "rooms", &rooms);
    n_rooms = json_object_array_length(rooms);
    if (!n_rooms)
        show_empty_result(room_box, "No users found");
    else
        show_rooms_result(room_box, n_rooms, rooms, info);
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

GtkWidget *init_login_event (GtkWidget *h_box, t_client_info *info, int id,  const char *login) {
    GtkWidget *login_event = gtk_event_box_new();
    GtkWidget *login_menu  = gtk_menu_new ();
    GtkWidget *view = gtk_menu_item_new_with_label("View profile");
    GtkWidget *label = gtk_label_new(login);
    t_mes *mes = (t_mes *)malloc(sizeof(t_mes));

    gtk_widget_show(view);
    gtk_menu_shell_append (GTK_MENU_SHELL (login_menu), view);
    mes->info = info;
    mes->user_id = id;
    g_signal_connect (G_OBJECT (view), "activate", G_CALLBACK (load_profile_callback1), mes);
    gtk_widget_add_events (login_event, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (login_event), "button_press_event", 
                        G_CALLBACK (open_menu_callback1), G_OBJECT(login_menu));
    gtk_container_add(GTK_CONTAINER(login_event), label);
    gtk_box_pack_start(GTK_BOX (h_box), login_event, FALSE, FALSE, 0);
    return login_event;
}


void show_users_result (GtkWidget *room_box, int n_users, 
                        json_object *users, t_client_info *info) {
    json_object *user_data = NULL;
    const char *login;
    int id = 0;
    GtkWidget *h_box = NULL;
    GtkWidget *login_event = NULL;
    GtkWidget *separator = NULL;

    for (int i = 0; i < n_users; i++) {
        user_data = json_object_array_get_idx(users, i);
        login = json_object_get_string(json_object_object_get(user_data, "login"));
        id = json_object_get_int(json_object_object_get(user_data, "id"));

        if (id != info->id) {
            h_box = init_h_box(room_box);
            login_event = init_login_event(h_box, info, id, login);
            separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
            gtk_box_pack_start (GTK_BOX (room_box), separator, FALSE, FALSE, 0);
        }
    }
}

void init_users (t_client_info *info, json_object *new_json, GtkWidget *v_box) {
    struct json_object *users;
    int n_users;
    GtkWidget *room_box = init_content_box(v_box, "Users:\n");

    json_object_object_get_ex(new_json, "users", &users);
    n_users = json_object_array_length(users);
    if (!n_users)
        show_empty_result(room_box, "No users found");
    else
        show_users_result (room_box, n_users, users, info);
}

void init_search_exit(t_client_info *info) {
    GtkWidget *search_exit = gtk_event_box_new();

    gtk_box_pack_start (GTK_BOX (info->data->search->main_box), 
                    search_exit, TRUE, TRUE, 0);
    gtk_widget_add_events (search_exit, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (search_exit), "button_press_event", 
                    G_CALLBACK (close_result_callback), info->data->search);
    gtk_widget_set_name (search_exit, "search_exit");
}

void mx_search_all_client(t_client_info *info, json_object *new_json) {
    GtkWidget *search_main = gtk_event_box_new();
    GtkWidget *v_box = gtk_box_new(FALSE, 1);

    info->data->search = (t_search *)malloc(sizeof(t_search));
    info->data->search->main_box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(info->data->search->main_box, 
                            gtk_widget_get_allocated_width (info->data->window), 
                            gtk_widget_get_allocated_height (info->data->window));
    gtk_fixed_put(GTK_FIXED(info->data->general_box),
                    info->data->search->main_box, 0, 0);
    gtk_widget_set_size_request(search_main, 300, -1);
    gtk_box_pack_start (GTK_BOX (info->data->search->main_box), 
                        search_main, FALSE, FALSE, 0);
    gtk_widget_set_name (search_main, "profile");
    gtk_orientable_set_orientation (GTK_ORIENTABLE(v_box), GTK_ORIENTATION_VERTICAL);
    gtk_container_add (GTK_CONTAINER (search_main), v_box);
    init_rooms (info, new_json, v_box);
    init_users (info, new_json, v_box);
    init_search_exit(info);
    gtk_widget_show_all(info->data->search->main_box);
}
