#include "uchat.h"

void mx_show_exit (t_client_info *info,  GtkWidget *profile) {
    GtkWidget *close_box = gtk_box_new(FALSE, 0);
    gtk_box_pack_start (GTK_BOX (profile), close_box, FALSE, FALSE, 0);
    GtkWidget *exit_button = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/cancel.png", 22, 22, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_button_set_image(GTK_BUTTON(exit_button), image);
    g_signal_connect(G_OBJECT(exit_button), "clicked", G_CALLBACK(mx_close_profile_callback), info);
    gtk_box_pack_end (GTK_BOX (close_box),exit_button, FALSE, FALSE, 0);
    gtk_widget_show(exit_button);
    gtk_widget_show(close_box);
}

static void show_name_settings(t_client_info *info, GtkWidget *name_box) {
    info->data->profile->name_button1 = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/edit.png", 22, 22, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_button_set_image(GTK_BUTTON(info->data->profile->name_button1), image);
    gtk_box_pack_start (GTK_BOX (name_box), info->data->profile->name_button1, FALSE, FALSE, 0);
    gtk_widget_show(info->data->profile->name_button1);

    info->data->profile->name_button2 = gtk_button_new();
    pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/save.png", 22, 22, TRUE, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_button_set_image(GTK_BUTTON(info->data->profile->name_button2), image);
    gtk_box_pack_start (GTK_BOX (name_box), info->data->profile->name_button2, FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(info->data->profile->name_button2), 
        "clicked", G_CALLBACK(mx_save_name_callback), info);
    g_signal_connect(G_OBJECT(info->data->profile->name_button1), 
        "clicked", G_CALLBACK(mx_edit_name_callback), info);
}

void mx_show_name (t_client_info *info, int id, json_object *new_json, GtkWidget *profile) {
    const char *name = json_object_get_string(json_object_object_get(new_json, "name"));
    GtkWidget *box = gtk_box_new(FALSE, 5);
    GtkWidget *name_box = gtk_box_new(FALSE, 5);

    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    mx_show_title(" Name", box);
    gtk_box_pack_start (GTK_BOX (box), name_box, FALSE, FALSE, 0);
    info->data->profile->name_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->profile->name_entry), 50);
    gtk_entry_set_text(GTK_ENTRY(info->data->profile->name_entry), name);
    gtk_box_pack_start (GTK_BOX (name_box), info->data->profile->name_entry, FALSE, FALSE, 0);
    gtk_widget_show(info->data->profile->name_entry);
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->name_entry), FALSE);
    if (id == info->id)
        show_name_settings(info, name_box);
    gtk_widget_show(name_box);
    gtk_widget_show(box);
}
