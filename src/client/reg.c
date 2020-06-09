#include "uchat.h"

static void init_reg_fail(t_client_info *info) {
    GtkWidget *box = gtk_box_new(FALSE, 0);

    info->data->register_msg = gtk_label_new("Your login or password is invalid");
    gtk_widget_set_name (info->data->register_msg, "auth_fail1");
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width 
                                (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 180);
    gtk_box_pack_start (GTK_BOX (box), info->data->register_msg, 
                        TRUE, FALSE, 0);
    gtk_widget_show (box);
}

static void init_reg_entry (t_client_info *info, GtkWidget **entry, 
                            char *placeholder, int heigth) {
    GtkWidget *box = gtk_box_new(FALSE, 0);

    *entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (*entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (*entry), placeholder);
    gtk_editable_select_region (GTK_EDITABLE (*entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (*entry)));
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width 
                                (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, heigth);
    gtk_box_pack_start (GTK_BOX (box), *entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (*entry);
    gtk_widget_set_name(*entry, "entry");
}

static void init_reg_button (t_client_info *info, char *placeholder, int heigth, 
                        void (*callback) (GtkWidget *widget, t_client_info *info) ) {
    GtkWidget *button = gtk_button_new_with_label(placeholder);
    GtkWidget *box = gtk_box_new(FALSE, 0);
    
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (callback), info);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width 
                                (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, heigth);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    gtk_widget_set_name(button, "entry");
}

void mx_init_reg(t_client_info *info) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/lr3.png", 400, 
                                                            320, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

    info->data->register_msg_t = NULL;
    info->data->register_box = gtk_fixed_new();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->register_box, 0, 0);
    mx_init_main_title(info, info->data->register_box);
    info->data->registration = (t_reg *)malloc(sizeof(t_reg));
    gtk_fixed_put (GTK_FIXED (info->data->register_box), image, 300, 150);
    gtk_widget_show(image);
    init_reg_fail(info);
    init_reg_entry(info, &info->data->registration->login_entry, "login", 200);
    init_reg_entry(info, &info->data->registration->password_entry, "password", 250);
    init_reg_entry(info, &info->data->registration->repeat_password_entry, 
                    "repeat password", 300);
    init_reg_button(info, "Register", 350, mx_send_data_callback);  
    init_reg_button(info, "Cancel", 390, mx_cancel_callback);
}
