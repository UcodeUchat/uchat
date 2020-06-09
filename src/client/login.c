#include "uchat.h"

static void init_auth_fail(t_client_info *info, GtkWidget *screen) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/stop2.png", 600, 520, TRUE, NULL);
    info->data->stop = gtk_image_new_from_pixbuf(pixbuf);
    gtk_fixed_put(GTK_FIXED(screen), info->data->stop, 176, 106);

    info->data->login_msg = gtk_label_new("Your login or password is invalid");
    gtk_widget_set_name (info->data->login_msg, "auth_fail1");
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (screen), box, 0, 180);
    gtk_box_pack_start (GTK_BOX (box), info->data->login_msg, TRUE, FALSE, 0);
    gtk_widget_show (box);
}

static void init_login_entry (t_client_info *info, GtkWidget **entry, char *placeholder, int heigth) {
    *entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (*entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (*entry), placeholder);
    gtk_editable_select_region (GTK_EDITABLE (*entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (*entry)));
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, heigth);
    gtk_box_pack_start (GTK_BOX (box), *entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (*entry);
    gtk_widget_set_name(*entry, "entry");
}

static void init_login_button (t_client_info *info, char *placeholder, int heigth,
                               void (*callback) (GtkWidget *widget, t_client_info *info) ) {
    GtkWidget *button = gtk_button_new_with_label(placeholder);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (callback), info);
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, heigth);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    gtk_widget_set_name(button, "entry");
}

void mx_init_login(t_client_info *info) {
    info->data->login_msg_t = NULL;
    info->editing = -1;
    info->data->login_box = gtk_fixed_new ();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->login_box, 0, 0);
    init_auth_fail(info, info->data->login_box);
    mx_init_main_title(info, info->data->login_box);
    init_login_entry(info, &info->data->login_entry, "login", 200);
    init_login_entry(info, &info->data->password_entry, "password", 250);
    gtk_entry_set_visibility (GTK_ENTRY(info->data->password_entry), FALSE);
    init_login_button(info, "Sign in", 300, mx_enter_callback);
    init_login_button(info, "Registration", 340, mx_reg_callback);
    gtk_widget_show(info->data->login_box);
}
