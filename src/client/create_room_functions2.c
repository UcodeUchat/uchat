#include "uchat.h"

void mx_init_room_data (t_client_info *info, t_room *room, 
                        json_object *room_data, t_all *data) {
    data->info = info;
    data->room = room;
    data->room_data = room_data;
}

void mx_init_room_box (t_room *room) {
    room->room_box = gtk_box_new(FALSE, 0);
    gtk_widget_set_name(room->room_box, "mesage_box");
    gtk_orientable_set_orientation (GTK_ORIENTABLE(room->room_box), 
                                    GTK_ORIENTATION_VERTICAL);
    gtk_widget_show(room->room_box);
}

void mx_init_room_header (t_room *room) {
    GtkWidget *full_name = gtk_label_new(room->name);
    
    room->header = gtk_event_box_new();
    gtk_widget_set_size_request(room->header, -1, 40);
    gtk_widget_add_events (room->header, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (room->header), "button_press_event", 
                    G_CALLBACK (mx_room_menu_callback), room->room_menu);
    gtk_widget_set_name (full_name, "title");
    gtk_container_add (GTK_CONTAINER (room->header), full_name);
    gtk_widget_show(full_name);
    gtk_box_pack_start (GTK_BOX (room->room_box), room->header, FALSE, FALSE, 0);
    gtk_widget_show(room->header);
}

void mx_init_room_window (t_room *room) {
    GtkWidget *ptrVscrollBar = NULL;

    room->scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_box_pack_start (GTK_BOX (room->room_box), 
                        room->scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show(room->scrolled_window);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(room->scrolled_window),
                                GTK_POLICY_NEVER,
                                GTK_POLICY_AUTOMATIC);
    room->Adjust = gtk_scrolled_window_get_vadjustment(
        GTK_SCROLLED_WINDOW(room->scrolled_window));
    ptrVscrollBar = gtk_scrolled_window_get_vscrollbar(
        GTK_SCROLLED_WINDOW(room->scrolled_window));
    gtk_widget_set_name (ptrVscrollBar, "bar");
}

void mx_init_room_messsage_box (t_room *room) {
    room->message_box = gtk_box_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(room->message_box), 5);
    gtk_container_add(GTK_CONTAINER(room->scrolled_window), room->message_box);
    gtk_widget_show(room->message_box);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(room->message_box), 
                                    GTK_ORIENTATION_VERTICAL);
}
