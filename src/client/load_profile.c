#include "uchat.h"

void update_profile (char *type, char *data, t_client_info *info) {
    json_object *new_json;
    const char *json_string;

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_EDIT_PROFILE_TYPE));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    json_object_object_add(new_json, "column", json_object_new_string(type));
    json_object_object_add(new_json, "data", json_object_new_string (data));
    json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
}

void close_profile_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_destroy(info->data->profile->main_box);
    free(info->data->profile);
    info->data->profile = NULL;
}

void close_profile_callback1 (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    gtk_widget_destroy(info->data->profile->main_box);
    free(info->data->profile);
    info->data->profile = NULL;
}

void edit_name_callback (GtkWidget *widget, t_client_info *info) {
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->name_entry), TRUE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->name_button2);
}

void save_name_callback (GtkWidget *widget, t_client_info *info) {
    char *name = strdup(gtk_entry_get_text(GTK_ENTRY(info->data->profile->name_entry)));
    
    update_profile("name", name, info);
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->name_entry), FALSE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->name_button1);
}

void show_title (const char *title, GtkWidget *box) {
    GtkWidget *title_box = gtk_box_new(FALSE, 5);
    gtk_box_pack_start (GTK_BOX (box), title_box, FALSE, FALSE, 0);
    GtkWidget *label = gtk_label_new(title);
    gtk_box_pack_start (GTK_BOX (title_box), label, FALSE, FALSE, 0);
    gtk_widget_show(label);
    gtk_widget_show(title_box);
}

void edit_email_callback (GtkWidget *widget, t_client_info *info) {
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->email_entry), TRUE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->email_button2);
}

void save_email_callback (GtkWidget *widget, t_client_info *info) {
    char *email = strdup(gtk_entry_get_text(GTK_ENTRY(info->data->profile->email_entry)));
    
    update_profile("email", email, info);
    gtk_editable_set_editable (GTK_EDITABLE (info->data->profile->email_entry), FALSE);
    gtk_widget_hide(widget);
    gtk_widget_show(info->data->profile->email_button1);
}

void show_exit (t_client_info *info,  GtkWidget *profile) {
    GtkWidget *close_box = gtk_box_new(FALSE, 0);
    gtk_box_pack_start (GTK_BOX (profile), close_box, FALSE, FALSE, 0);
    GtkWidget *exit_button = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/cancel.png", 22, 22, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_button_set_image(GTK_BUTTON(exit_button), image);
    g_signal_connect(G_OBJECT(exit_button), "clicked", G_CALLBACK(close_profile_callback), info);
    gtk_box_pack_end (GTK_BOX (close_box),exit_button, FALSE, FALSE, 0);
    gtk_widget_show(exit_button);
    gtk_widget_show(close_box);
}

void show_name_settings(t_client_info *info, GtkWidget *name_box) {
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
        "clicked", G_CALLBACK(save_name_callback), info);
    g_signal_connect(G_OBJECT(info->data->profile->name_button1), 
        "clicked", G_CALLBACK(edit_name_callback), info);
}

void show_name (t_client_info *info, int id, json_object *new_json, GtkWidget *profile) {
    const char *name = json_object_get_string(json_object_object_get(new_json, "name"));
    GtkWidget *box = gtk_box_new(FALSE, 5);
    GtkWidget *name_box = gtk_box_new(FALSE, 5);

    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    show_title(" Name", box);
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


void show_login (t_client_info *info, json_object *new_json, GtkWidget *profile) {
    const char *login = json_object_get_string(json_object_object_get(new_json, "login"));
    GtkWidget *box = gtk_box_new(FALSE, 5);
    GtkWidget *login_box = gtk_box_new(FALSE, 5);

    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    show_title(" Login", box);
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

void show_email_settings(t_client_info *info, GtkWidget *email_box) {
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
        "clicked", G_CALLBACK(save_email_callback), info);
    g_signal_connect(G_OBJECT(info->data->profile->email_button1), 
        "clicked", G_CALLBACK(edit_email_callback), info);
}

void show_email (t_client_info *info, int id, json_object *new_json, GtkWidget *profile) {
    const char *email = json_object_get_string(json_object_object_get(new_json, "email"));
    GtkWidget *box = gtk_box_new(FALSE, 5);
    GtkWidget *email_box = gtk_box_new(FALSE, 5);

    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    show_title(" Email", box);
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

void show_id (t_client_info *info, int id, GtkWidget *profile) {
    GtkWidget *box = gtk_box_new(FALSE, 5);
    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    show_title(" ID", box);

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

void mx_load_user_profile(t_client_info *info, json_object *new_json) {
    if (info->data->profile != NULL) {
        g_idle_add ((GSourceFunc)mx_destroy_widget, info->data->profile->main_box);
        free(info->data->profile);
        info->data->profile = NULL;
    }
    info->data->profile = (t_prof *)malloc(sizeof(t_prof));
    int id = json_object_get_int(json_object_object_get(new_json, "id"));
    info->data->profile->main_box = gtk_event_box_new();
    GtkWidget *box = gtk_box_new(FALSE, 0);
    GtkWidget *profile = gtk_box_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(info->data->profile->main_box),box);
    gtk_box_pack_start (GTK_BOX (box), profile, FALSE, FALSE, 0);
    gtk_widget_set_name(profile, "profile");
    gtk_fixed_put(GTK_FIXED(info->data->general_box), 
    	info->data->profile->main_box, 0, 0);
    gtk_widget_set_size_request(info->data->profile->main_box, 
    						gtk_widget_get_allocated_width (info->data->window), 
    						gtk_widget_get_allocated_height (info->data->window));
    gtk_orientable_set_orientation (GTK_ORIENTABLE(profile), 
    								GTK_ORIENTATION_VERTICAL);
    show_exit(info, profile);
    show_login(info, new_json, profile);
    show_id(info, id, profile);
    show_name(info, id, new_json, profile);
    show_email(info, id, new_json, profile);
    GtkWidget *close_event = gtk_event_box_new();
    gtk_widget_set_name (close_event, "menu_exit");
    gtk_widget_add_events (close_event, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (close_event), "button_press_event", 
    					G_CALLBACK (close_profile_callback1), info);
    gtk_box_pack_start (GTK_BOX (box), close_event, TRUE, TRUE, 0);
    gtk_widget_show(profile);
    gtk_widget_show(close_event);
    gtk_widget_show(box);
    g_idle_add ((GSourceFunc)mx_show_widget, info->data->profile->main_box);
}
