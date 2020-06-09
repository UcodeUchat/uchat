#include "uchat.h"

static void init_edit_button (t_client_info *info, char *i_name, GtkWidget *fixed_message) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale (i_name, 20, 20, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

    info->data->edit_button = gtk_event_box_new ();
    gtk_widget_add_events (info->data->edit_button, GDK_BUTTON_PRESS_MASK);
    gtk_container_add (GTK_CONTAINER (info->data->edit_button), image);
    gtk_widget_show(image);
    g_signal_connect(G_OBJECT(info->data->edit_button), "button_press_event", G_CALLBACK(mx_edit_cancel_callback), info);
    gtk_fixed_put(GTK_FIXED(fixed_message), info->data->edit_button, 640, 7);
}

void mx_init_message_box (t_client_info *info, GtkWidget *box, 
                    void (*callback) (GtkWidget *widget, t_client_info *info)) {
    GtkWidget *fixed_message = gtk_fixed_new();

    info->data->message_entry = gtk_entry_new ();
    gtk_entry_set_placeholder_text(GTK_ENTRY (info->data->message_entry), "Write something");
    gtk_entry_set_max_length(GTK_ENTRY (info->data->message_entry), 100);
    gtk_editable_select_region(GTK_EDITABLE (info->data->message_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->message_entry)));
    g_signal_connect(G_OBJECT(info->data->message_entry),"activate", G_CALLBACK(callback), info);
    gtk_box_pack_start (GTK_BOX (box), fixed_message, TRUE, TRUE, 0);
    gtk_fixed_put(GTK_FIXED(fixed_message), info->data->message_entry, 0, 0);
    gtk_widget_set_size_request(info->data->message_entry, 670, -1);
    gtk_widget_set_name(info->data->message_entry, "entry");
    gtk_widget_show(info->data->message_entry);
    gtk_widget_show(fixed_message);
    init_edit_button (info, "img/cancel.png", fixed_message);
}

void mx_init_general_button_text (t_client_info *info, char *text, GtkWidget *box,
                        void (*callback) (GtkWidget *widget, t_client_info *info)) {
    GtkWidget *button = gtk_button_new_with_label(text);

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(callback), info);
    gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 0);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
}

void mx_init_general_button (t_client_info *info, char *i_name, GtkWidget *box,
                        void (*callback) (GtkWidget *widget, t_client_info *info)) {
    GtkWidget *button = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale (i_name, 20, 20, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

    gtk_button_set_image(GTK_BUTTON(button), image);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK (callback), info);
    gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 0);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
}

void mx_init_notebook (t_client_info *info, GtkWidget *box) {
    int n_rooms = json_object_array_length(info->rooms);

    info->data->notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK (info->data->notebook), GTK_POS_LEFT);
    gtk_widget_set_size_request(info->data->notebook, gtk_widget_get_allocated_width (info->data->window) -  20, 550);
    gtk_box_pack_start (GTK_BOX (box), info->data->notebook, TRUE, FALSE, 0);
    gtk_widget_show (info->data->notebook);
    for (int i = 0; i < n_rooms; i++) {
        json_object *room_data = json_object_array_get_idx(info->rooms, i);
        mx_push_room(info, room_data, i);   
    }
}

