#include "uchat.h"

static void init_vars_general(t_client_info *info) {
    info->data->profile = NULL;
    info->can_load = 1;
    info->data->rooms = NULL;
    info->data->current_room = 0;
}

static GtkWidget *init_bottom_box (t_client_info *info) {
    GtkWidget *box = gtk_box_new(FALSE, 10);

    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window) - 20, -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->general_box), box, 10, 570);
    gtk_widget_show (box);
    return box;
}

static GtkWidget *init_top_box (t_client_info *info) {
    GtkWidget *box = gtk_box_new(FALSE, 0);

    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->general_box), box, 0, 10);
    gtk_widget_show (box);
    return box;
}

void mx_init_general (t_client_info *info) {
    GtkWidget *box = NULL;

    info->data->general_box = gtk_fixed_new();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->general_box, 0, 0);
    init_vars_general(info);
    box = init_bottom_box(info);
    mx_init_general_button (info ,"img/a.png", box, mx_menu_callback);
    mx_init_message_box (info, box, mx_send_callback);
    mx_init_general_button_text (info ,"Send", box, mx_send_callback);
    mx_init_general_button (info ,"img/rec.png", box, mx_record_callback);
    mx_init_general_button (info ,"img/search.png", box, mx_show_search_callback);
    mx_init_general_button (info ,"img/file.png", box, mx_choose_file_callback);
    mx_init_stickers(info, box);
    box = init_top_box(info);
    mx_init_notebook (info, box);
    gtk_widget_hide(info->data->login_box);
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Uchat");
    gtk_widget_show(info->data->general_box);
}

