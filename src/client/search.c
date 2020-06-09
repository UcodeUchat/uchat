#include "uchat.h"

static void close_result_callback (GtkWidget *widget, GdkEventButton *event, t_search *search) {
    (void)widget;
    (void)event;
    if (search != NULL) {
        gtk_widget_hide(search->main_box);
        free(search);
        search = NULL;
    }
}

static void init_rooms (t_client_info *info, json_object *new_json, GtkWidget *v_box) {
    struct json_object *rooms;
    int n_rooms;
    GtkWidget *room_box = mx_init_content_box(v_box, "Rooms:\n");

    mx_js_o_o_get_ex(new_json, "rooms", &rooms);
    n_rooms = json_object_array_length(rooms);
    if (!n_rooms)
        mx_show_empty_result(room_box, "No users found");
    else
        mx_show_rooms_result(room_box, n_rooms, rooms, info);
}

static void init_users (t_client_info *info, json_object *new_json, GtkWidget *v_box) {
    struct json_object *users;
    int n_users;
    GtkWidget *room_box = mx_init_content_box(v_box, "Users:\n");

    mx_js_o_o_get_ex(new_json, "users", &users);
    n_users = json_object_array_length(users);
    if (!n_users)
        mx_show_empty_result(room_box, "No users found");
    else
        mx_show_users_result (room_box, n_users, users, info);
}

static void init_search_exit(t_client_info *info) {
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
