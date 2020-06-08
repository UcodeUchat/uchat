#include "uchat.h"


static void create_item (GtkWidget *menu, t_mes *mes, char *title, 
            void (*callback) (GtkWidget *widget, t_mes *mes)) {
    GtkWidget *item = gtk_menu_item_new_with_label(title);

    gtk_widget_show(item);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (callback), mes);
}

void mx_init_message_menu (t_message *node, t_mes *mes) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/options.png", 20, 40, TRUE, NULL);
    GtkWidget *menu  = gtk_menu_new ();

    node->menu_event = gtk_event_box_new();
    node->menu = gtk_image_new_from_pixbuf(pixbuf);
    gtk_container_add (GTK_CONTAINER (node->menu_event), node->menu);
    create_item (menu, mes, "Delete", mx_delete_callback);
    if (node->add_info == 0)
        create_item (menu, mes, "Edit", mx_edit_callback);
    gtk_widget_add_events (node->menu_event, GDK_ENTER_NOTIFY_MASK);
    gtk_widget_add_events (node->menu_event, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (node->menu_event), "enter_notify_event", G_CALLBACK (mx_focus_menu_callback), node->menu);
    g_signal_connect (G_OBJECT (node->menu_event), "button_press_event", G_CALLBACK (mx_open_menu_callback), G_OBJECT (menu));
    gtk_widget_show(node->menu_event);
}


void mx_init_message_login (t_message *node, t_mes *mes, const char *login) {
    GtkWidget *login_menu  = gtk_menu_new ();
    GtkWidget *view = gtk_menu_item_new_with_label("View profile");
    GtkWidget *label = gtk_label_new(login);

    node->login_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->login_box);
    node->login_event = gtk_event_box_new();
    gtk_widget_show(view);
    gtk_menu_shell_append (GTK_MENU_SHELL (login_menu), view);
    g_signal_connect (G_OBJECT (view), "activate", G_CALLBACK (mx_load_profile_callback), mes);
    gtk_widget_add_events (node->login_event, GDK_ENTER_NOTIFY_MASK);
    gtk_widget_add_events (node->login_event, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (node->login_event), "enter_notify_event", 
                        G_CALLBACK (mx_focus_callback), mes);
    g_signal_connect (G_OBJECT (node->login_event), "button_press_event", 
                        G_CALLBACK (mx_open_menu_callback), G_OBJECT(login_menu));
    gtk_widget_set_name (label, "login");
    gtk_container_add(GTK_CONTAINER(node->login_event), label);
    gtk_box_pack_start(GTK_BOX (node->central_box), node->login_box, FALSE, FALSE, 0);
    gtk_widget_show(label);
    gtk_widget_show(node->login_event);
}
