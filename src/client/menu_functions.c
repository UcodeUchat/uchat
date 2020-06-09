#include "uchat.h"

GtkWidget *mx_init_menu_fixed (GtkWidget *main_box) {
    GtkWidget *fixed = gtk_fixed_new();

    gtk_container_add(GTK_CONTAINER(main_box), fixed);
    gtk_widget_show(fixed);
    return fixed;
}

GtkWidget *mx_init_menu_box (GtkWidget *fixed, int size) {
    GtkWidget *box = gtk_box_new(FALSE, 10);
    
    gtk_widget_set_size_request(box, size, -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), 
                                    GTK_ORIENTATION_VERTICAL);
    gtk_fixed_put (GTK_FIXED (fixed), box, 0, 10);
    gtk_widget_show (box);
    return box;
}

GtkWidget *mx_init_menu_main_box (t_client_info *info, 
                                    GtkWidget *parent, char *style) {
    GtkWidget *main_box = gtk_event_box_new();

    (void) info;
    gtk_box_pack_start (GTK_BOX (parent), main_box, FALSE, FALSE, 0);
    gtk_widget_set_size_request(main_box, 150, -1);
    gtk_widget_set_name (main_box, style);
    gtk_widget_show(main_box);
    return main_box;
}

GtkWidget *mx_init_menu_exit_box (t_client_info *info, GtkWidget *parent,
                        void (*callback) (GtkWidget *widget, GdkEventButton *event, 
                                            t_client_info *info)) {
    GtkWidget *exit_box = gtk_event_box_new();

    gtk_box_pack_start (GTK_BOX (parent), exit_box, TRUE, TRUE, 0);
    gtk_widget_realize (exit_box);
    gtk_widget_add_events (exit_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (exit_box), "button_press_event", 
                        G_CALLBACK (callback), info);
    gtk_widget_set_name (exit_box, "menu_exit");
    gtk_widget_show(exit_box);
    return exit_box;
}
