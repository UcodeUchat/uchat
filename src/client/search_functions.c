#include "uchat.h"

static void open_menu_callback1(GtkWidget *widget, GdkEventButton *event, GtkWidget *menu) {
    (void)widget;
    (void)event;
    gtk_menu_popup_at_pointer (GTK_MENU(menu), NULL);
}

static void load_profile_callback1(GtkWidget *widget, t_mes *mes) {
    (void)widget;
    mx_load_profile_client(mes->info, mes->user_id);
}

static GtkWidget *init_login_event (GtkWidget *h_box, t_client_info *info, int id,  const char *login) {
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

void mx_show_empty_result (GtkWidget *room_box, char *title) {
    GtkWidget *h_box = gtk_box_new(FALSE, 5);
    GtkWidget *empty_label = gtk_label_new(title);

    gtk_widget_set_size_request(h_box, -1, 41);
    gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (room_box), h_box, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (h_box), empty_label, FALSE, FALSE, 0); 
}


void mx_show_users_result (GtkWidget *room_box, int n_users, 
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
            h_box = mx_init_search_h_box(room_box);
            login_event = init_login_event(h_box, info, id, login);
            separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
            gtk_box_pack_start (GTK_BOX (room_box), separator, FALSE, FALSE, 0);
        }
    }
}
