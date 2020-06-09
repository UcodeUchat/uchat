#include "uchat.h"

int mx_show_widget(GtkWidget *widget) {
    gtk_widget_show(widget);
    return 0;
}

int mx_destroy_widget(GtkWidget *widget) {
    gtk_widget_destroy(widget);
    return 0;
}

void mx_image (t_message *node) {
    node->message_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->message_box);
    gtk_box_pack_start (GTK_BOX (node->central_box), node->message_box, FALSE, FALSE, 0);
    node->image_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->image_box);
    gtk_box_pack_start (GTK_BOX (node->message_box), node->image_box, FALSE, FALSE, 0);
}

void mx_sticker (t_message *node, const char *message) {
    GtkWidget *stick_box =  gtk_box_new(FALSE, 0);
    GdkPixbuf *item_pixbuf = gdk_pixbuf_new_from_file_at_scale (message, 200, 200, TRUE, NULL);
    GtkWidget *item_image = gtk_image_new_from_pixbuf(item_pixbuf);

    gtk_widget_show(stick_box);
    gtk_box_pack_start (GTK_BOX (node->central_box), stick_box, FALSE, FALSE, 0);
    gtk_widget_show(item_image);
    gtk_box_pack_start (GTK_BOX (stick_box), item_image, FALSE, FALSE, 0);
}
