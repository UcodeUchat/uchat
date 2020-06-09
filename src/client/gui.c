#include "uchat.h"

t_room *mx_find_room_position(t_room *rooms, int position) {
   t_room *head = rooms;
   t_room *node = NULL;

    while (head != NULL) {
        if (head->position == position) {
            node = head;
            break;
        }
        head = head->next;
    }
    return node;
}

void mx_sleep_ms (int milliseconds) {
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

void mx_init_main_title(t_client_info *info, GtkWidget *screen)  {
    GtkWidget *table = gtk_grid_new();
    GtkWidget *title1 = gtk_label_new("Ucode");
    GtkWidget *title2 = gtk_label_new("chat");
    GtkWidget *box = gtk_box_new(FALSE, 0);
    GtkWidget *event_box = gtk_event_box_new();

    gtk_widget_set_name(title1, "title1");
    gtk_grid_attach (GTK_GRID (table), title1, 0, 0, 1, 1);
    gtk_widget_set_name(title2, "title2");
    gtk_grid_attach (GTK_GRID (table), title2, 1, 0, 1, 1);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width 
                                    (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (screen), box, 0, 125);
    gtk_container_add (GTK_CONTAINER (event_box), table);
    gtk_widget_add_events (event_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (event_box), "button_press_event", 
        G_CALLBACK (mx_url_callback), NULL);
    gtk_box_pack_start (GTK_BOX (box), event_box, TRUE, FALSE, 0);
    gtk_widget_show_all(box);
}

void init_window(t_client_info *info) {
    GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_path (provider,"my_style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                               GTK_STYLE_PROVIDER(provider),
                               GTK_STYLE_PROVIDER_PRIORITY_USER);
    info->data = (t_data *)malloc(sizeof(t_data));
    info->data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(info->data->window, "back"); 
    gtk_window_set_resizable (GTK_WINDOW (info->data->window), FALSE);
    gtk_widget_set_size_request (GTK_WIDGET (info->data->window), 1000, 630);
    gtk_widget_show (info->data->window);
    gtk_window_set_title (GTK_WINDOW (info->data->window), "Sign in");
    g_signal_connect (G_OBJECT (info->data->window), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect_swapped (G_OBJECT (info->data->window), "delete_event",
                              G_CALLBACK (gtk_widget_destroy), 
                              G_OBJECT (info->data->window));
    info->data->main_box = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (info->data->window), info->data->main_box);
    gtk_widget_show (info->data->main_box);
}

int mx_login (t_client_info *info) {
    init_window(info);
    mx_init_reg(info);
    mx_init_login(info);

    gtk_main();
    return 0;
}
