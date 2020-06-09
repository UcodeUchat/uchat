#include "uchat.h"

void mx_show_login (t_client_info *info, json_object *new_json, GtkWidget *profile) {
    const char *login = json_object_get_string(json_object_object_get(new_json, "login"));
    GtkWidget *box = gtk_box_new(FALSE, 5);
    GtkWidget *login_box = gtk_box_new(FALSE, 5);

    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    mx_show_title(" Login", box);
    gtk_box_pack_start (GTK_BOX (box), login_box, FALSE, FALSE, 0);
    info->data->profile->login_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->profile->login_entry), 50);
    gtk_entry_set_text(GTK_ENTRY(info->data->profile->login_entry), login);
    gtk_box_pack_start (GTK_BOX (login_box), info->data->profile->login_entry, FALSE, FALSE, 0);
    gtk_widget_show(info->data->profile->login_entry);
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->login_entry), FALSE);
    gtk_widget_show(login_box);
    gtk_widget_show(box);
}

static void show_email_settings(t_client_info *info, GtkWidget *email_box) {
    info->data->profile->email_button1 = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/edit.png", 22, 22, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_button_set_image(GTK_BUTTON(info->data->profile->email_button1), image);
    gtk_box_pack_start (GTK_BOX (email_box), info->data->profile->email_button1, FALSE, FALSE, 0);
    gtk_widget_show(info->data->profile->email_button1);

    info->data->profile->email_button2 = gtk_button_new();
    pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/save.png", 22, 22, TRUE, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_button_set_image(GTK_BUTTON(info->data->profile->email_button2), image);
    gtk_box_pack_start (GTK_BOX (email_box), info->data->profile->email_button2, FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(info->data->profile->email_button2),
        "clicked", G_CALLBACK(mx_save_email_callback), info);
    g_signal_connect(G_OBJECT(info->data->profile->email_button1), 
        "clicked", G_CALLBACK(mx_edit_email_callback), info);
}

void mx_show_email (t_client_info *info, int id, json_object *new_json, GtkWidget *profile) {
    const char *email = json_object_get_string(json_object_object_get(new_json, "email"));
    GtkWidget *box = gtk_box_new(FALSE, 5);
    GtkWidget *email_box = gtk_box_new(FALSE, 5);

    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    mx_show_title(" Email", box);
    gtk_box_pack_start (GTK_BOX (box), email_box, FALSE, FALSE, 0);
    info->data->profile->email_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->profile->email_entry), 50);
    gtk_entry_set_text(GTK_ENTRY(info->data->profile->email_entry), email);
    gtk_box_pack_start (GTK_BOX (email_box), info->data->profile->email_entry, FALSE, FALSE, 0);
    gtk_widget_show(info->data->profile->email_entry);
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->email_entry), FALSE);
    if (id == info->id)
        show_email_settings(info, email_box);
    gtk_widget_show(email_box);
    gtk_widget_show(box);
}

void mx_show_id (t_client_info *info, int id, GtkWidget *profile) {
    GtkWidget *box = gtk_box_new(FALSE, 5);
    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    mx_show_title(" ID", box);

    GtkWidget *id_box = gtk_box_new(FALSE, 5);
    gtk_box_pack_start (GTK_BOX (box), id_box, FALSE, FALSE, 0);
    info->data->profile->id_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->profile->id_entry), 50);
    gtk_entry_set_text(GTK_ENTRY(info->data->profile->id_entry), mx_itoa(id));
    gtk_box_pack_start (GTK_BOX (id_box), info->data->profile->id_entry, FALSE, FALSE, 0);
    gtk_widget_show(info->data->profile->id_entry);
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->id_entry), FALSE);
    gtk_widget_show(id_box);
    gtk_widget_show(box);
}
