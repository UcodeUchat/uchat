#include "uchat.h"

GtkWidget *mx_init_content_box (GtkWidget *v_box, char *title) {
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


GtkWidget *mx_init_search_h_box (GtkWidget *room_box) {
    GtkWidget *h_box = gtk_box_new(FALSE, 5);

    gtk_widget_set_size_request(h_box, -1, 41);
    gtk_widget_set_valign (h_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (room_box), h_box, FALSE, FALSE, 0);
    return h_box;
}
