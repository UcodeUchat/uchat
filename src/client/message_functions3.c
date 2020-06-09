#include "uchat.h"


static void file_callback(GtkWidget *widget, GdkEventButton *event, 
                            t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    gtk_widget_set_name(widget, "file_pressed");
    if (mes->message != NULL)
        mx_load_file(mes);
}

static void file1_callback(GtkWidget *widget, GdkEventButton *event, 
                            t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    gtk_widget_set_name(widget, "file_hover");
}

static void file_notify_callback(GtkWidget *widget, GdkEventButton *event, 
                                t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    t_message *message = mx_find_message(mes->room->messages, mes->id);

    if (message != NULL) {
        gtk_widget_show(message->menu);
        gtk_widget_set_name(widget, "file_hover");
    }  
}

static void file_notify1_callback(GtkWidget *widget, GdkEventButton *event, 
                                    t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    gtk_widget_set_name(widget, "file");
}

void mx_file (t_mes *mes, t_message *node, const char *message) {
    GtkWidget *label = gtk_label_new(message + 20);

    node->message_box = gtk_event_box_new();
    gtk_widget_show(node->message_box);
    gtk_box_pack_start (GTK_BOX (node->central_box), node->message_box, FALSE, FALSE, 0);
    gtk_widget_show(label);
    gtk_container_add (GTK_CONTAINER (node->message_box), label);
    gtk_widget_set_name(node->message_box, "file");
    gtk_widget_add_events (node->message_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (node->message_box), "button_press_event", 
        G_CALLBACK (file_callback), mes);
    g_signal_connect (G_OBJECT (node->message_box), "button_release_event", 
        G_CALLBACK (file1_callback), mes);
    gtk_widget_add_events (node->message_box, GDK_ENTER_NOTIFY_MASK);
    g_signal_connect (G_OBJECT (node->message_box), "enter_notify_event", 
        G_CALLBACK (file_notify_callback), mes);
    g_signal_connect (G_OBJECT (node->message_box), "leave_notify_event", 
        G_CALLBACK (file_notify1_callback), mes);
}
