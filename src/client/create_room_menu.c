#include "uchat.h"

static void init_create_entry (t_client_info *info, GtkWidget *box) {
    GtkWidget *box1 = gtk_box_new(FALSE, 0);

    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    info->data->create_room->name_entry = gtk_entry_new();
    gtk_widget_set_size_request(info->data->create_room->name_entry, 200, -1);
    gtk_entry_set_max_length (GTK_ENTRY (info->data->create_room->name_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->create_room->name_entry), "Write room name");
    gtk_editable_select_region (GTK_EDITABLE (info->data->create_room->name_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->create_room->name_entry)));
    gtk_box_pack_start (GTK_BOX (box1), info->data->create_room->name_entry, TRUE, FALSE, 0);
    gtk_widget_show(info->data->create_room->name_entry);
    gtk_widget_show(box1);
}

static void init_create_combo_box (t_client_info *info, GtkWidget *box) {
    GtkWidget *box1 = gtk_box_new(FALSE, 0);

    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    info->data->create_room->selection = gtk_combo_box_text_new();
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(info->data->create_room->selection), "0", "Public");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(info->data->create_room->selection), "1", "Private");
    gtk_combo_box_set_active (GTK_COMBO_BOX(info->data->create_room->selection), 0);
    gtk_box_pack_start (GTK_BOX (box1), info->data->create_room->selection, TRUE, FALSE, 0);
    gtk_widget_set_size_request(info->data->create_room->selection, 200, -1);
    gtk_widget_show(info->data->create_room->selection);
    gtk_widget_show(box1);
}

static void init_create_buttons (t_client_info *info, GtkWidget *box) {
    GtkWidget *box1 = gtk_box_new(FALSE, 0);

    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    info->data->create_room->create_button = gtk_button_new_with_label("OK");
    g_signal_connect(G_OBJECT(info->data->create_room->create_button), "clicked", G_CALLBACK(mx_create_room_callback), info);
    gtk_box_pack_start (GTK_BOX (box1), info->data->create_room->create_button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(info->data->create_room->create_button, 100, -1);
    gtk_widget_show(info->data->create_room->create_button);
    info->data->create_room->cancel_button = gtk_button_new_with_label("Cancel");
    g_signal_connect(G_OBJECT(info->data->create_room->cancel_button), "clicked", G_CALLBACK(mx_close_creation_callback1), info);
    gtk_box_pack_start (GTK_BOX (box1), info->data->create_room->cancel_button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(info->data->create_room->cancel_button, 100, -1);
    gtk_widget_show(info->data->create_room->cancel_button);
    gtk_widget_show(box1);
}

void mx_init_create (t_client_info *info) { 
    GtkWidget *main_box = NULL;
    GtkWidget *fixed = NULL;
    GtkWidget *exit_box = NULL;
    GtkWidget *box = NULL;

    info->data->create_room = (t_create *)malloc(sizeof(t_create));
    info->data->create_room->main_box = gtk_box_new(FALSE, 0);
    gtk_fixed_put(GTK_FIXED(info->data->general_box), info->data->create_room->main_box, 0, 0);
    gtk_widget_set_size_request(info->data->create_room->main_box, 
                                gtk_widget_get_allocated_width (info->data->window), 
                                gtk_widget_get_allocated_height (info->data->window));
    main_box = mx_init_menu_main_box(info, info->data->create_room->main_box, "profile");
    fixed = mx_init_menu_fixed (main_box);
    box = mx_init_menu_box (fixed, 250);
    init_create_entry (info, box);
    init_create_combo_box (info, box);
    init_create_buttons (info, box);
    exit_box = mx_init_menu_exit_box (info, info->data->create_room->main_box, 
                                    mx_close_creation_callback);
}
