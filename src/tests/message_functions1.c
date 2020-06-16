#include "uchat.h"

static void init_main_message_part2 (t_message *node) {
    node->left_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->left_box);
    gtk_widget_set_size_request(node->left_box, 10, -1);
    gtk_box_pack_start(GTK_BOX (node->main_box), node->left_box, 
        FALSE, FALSE, 0);
    node->central_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->central_box);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(node->central_box), 
        GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX (node->main_box), node->central_box, 
        FALSE, FALSE, 0);
    node->right_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->right_box);
    gtk_widget_set_size_request(node->right_box, 15, -1);
    gtk_box_pack_start(GTK_BOX (node->main_box), node->right_box, 
        FALSE, FALSE, 0);
}

void mx_init_main_message (t_message *node, t_mes *mes) {
    node->general_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->general_box);
    node->event = gtk_event_box_new();
    gtk_widget_show(node->event);
    gtk_widget_add_events (node->event, GDK_ENTER_NOTIFY_MASK);
    g_signal_connect (G_OBJECT (node->event), "enter_notify_event", 
        G_CALLBACK (mx_focus_callback), mes);
    g_signal_connect (G_OBJECT (node->event), "leave_notify_event", 
        G_CALLBACK (mx_focus1_callback), mes);
    gtk_box_pack_start (GTK_BOX (node->general_box), 
        node->event, FALSE, FALSE, 0);
    node->main_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->main_box);
    gtk_container_add (GTK_CONTAINER (node->event), node->main_box);
    gtk_widget_set_name(node->general_box, "message");
    gtk_container_set_border_width(GTK_CONTAINER(node->main_box), 1);
    init_main_message_part2(node);
}

void mx_init_message (t_message *node, t_room *room, 
                        json_object *new_json, int order) {
    node->id = mx_js_g_int(mx_js_o_o_get(new_json, "id"));
    node->user_id = mx_js_g_int(
                        mx_js_o_o_get(new_json, "user_id"));
    node->add_info = mx_js_g_int(
                        mx_js_o_o_get(new_json, "add_info"));
    node->data = strdup (mx_js_g_str(
                        mx_js_o_o_get(new_json, "data")));
    node->h_box = gtk_box_new(FALSE, 0);
    t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
    mes->room = room;
    mes->message = node;
    mes->audio = mx_init_struct_audio();
//    mx_init_struct_audio2(mes);
    g_idle_add ((GSourceFunc)mx_add, mes);
    if (order == 2)
        g_idle_add ((GSourceFunc)mx_reorder, mes);
}

void mx_init_message_data (t_client_info *info, t_room *room, 
                            json_object *new_json, t_mes *mes) {
    mes->info = info;
    mes->room = room;
    mes->id = mx_js_g_int(mx_js_o_o_get(new_json, "id"));
    mes->user_id = mx_js_g_int(mx_js_o_o_get(new_json, "user_id"));
}
