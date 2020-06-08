#include "uchat.h"


int mx_add(t_mes *mes) {
    gtk_box_pack_start (GTK_BOX (mes->room->message_box), 
        mes->message->h_box, FALSE, FALSE, 0);;
    return 0;
}

int mx_reorder(t_mes *mes) {
    gtk_box_reorder_child (GTK_BOX (mes->room->message_box), mes->message->h_box, 0);
    return 0;
}
