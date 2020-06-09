#include "uchat.h"

static int move_bar(t_room *room) {
    gtk_adjustment_set_value(room->Adjust, 
                            gtk_adjustment_get_upper(room->Adjust) - 
                            gtk_adjustment_get_page_size(room->Adjust) + 2.0);
    return 0;
}

void mx_choose_side (t_client_info *info, t_message *node, 
                    t_room *room, int order) {
    if (node->user_id == info->id) {
        gtk_box_pack_end(GTK_BOX (node->right_box), node->menu_event, 0, 0, 0);
        gtk_box_pack_end(GTK_BOX (node->login_box), node->login_event, 
                        0, 0, 0);
        gtk_box_pack_end (GTK_BOX (node->h_box), node->general_box, 0, 0, 0);
        g_idle_add ((GSourceFunc)mx_show_widget, node->h_box);
        if (order == 1) {
            mx_sleep_ms(50);
            g_idle_add ((GSourceFunc)move_bar, room);
        }
    }
    else {
        gtk_box_pack_start (GTK_BOX (node->login_box), node->login_event, 
                            0, 0, 0);
        gtk_box_pack_start (GTK_BOX (node->h_box), node->general_box, 0, 0, 0);
        if (node->add_info != 2)
            g_idle_add ((GSourceFunc)mx_show_widget, node->h_box);
    }
}

void mx_simple_message (t_message *node, const char *message) {
    node->message_box =  gtk_box_new(FALSE, 0);
    gtk_widget_show(node->message_box);
    gtk_box_pack_start (GTK_BOX (node->central_box), node->message_box, 
                        FALSE, FALSE, 0);
    node->message_label = gtk_label_new(message);
    gtk_widget_show(node->message_label);
    gtk_box_pack_start (GTK_BOX (node->message_box), node->message_label, 
                        FALSE, FALSE, 0);
}
