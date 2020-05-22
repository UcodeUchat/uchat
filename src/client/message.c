#include "uchat.h"

typedef struct s_all {
    t_room *room;
    GtkWidget *widget;
}               t_all;

void focus_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    if (message != NULL)
        gtk_widget_show(message->menu);
}

void focus1_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    if (message != NULL)
        gtk_widget_hide(message->menu);
}

void focus_menu_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    if (message != NULL)
        gtk_widget_show(message->menu);
}

void open_menu_callback(GtkWidget *widget, GdkEventButton *event, GtkWidget *menu) {
    (void)widget;
    (void)event;
    gtk_menu_popup_at_pointer (GTK_MENU(menu), NULL);
}

void load_profile_callback(GtkWidget *widget, t_mes *mes) {
    (void)widget;
    mx_load_profile_client(mes->info, mes->user_id);
}

void delete_callback (GtkWidget *widget, t_mes *mes) {
    (void)widget;
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    json_object  *new_json = json_object_new_object();

    json_object_object_add(new_json, "type", json_object_new_int(MX_DELETE_MESSAGE_TYPE));
    json_object_object_add(new_json, "message_id", json_object_new_int(message->id));
    json_object_object_add(new_json, "room_id", json_object_new_int(mes->room->id));
    mx_print_json_object(new_json, "delete message");
    const char *json_str = json_object_to_json_string(new_json);
    tls_send(mes->info->tls_client, json_str, strlen(json_str));
}

void edit_callback (GtkWidget *widget, t_mes *mes) {
    (void)widget;
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    if (message != NULL) {
        gtk_entry_set_text(GTK_ENTRY(mes->info->data->message_entry), message->data);
        gtk_widget_show(mes->info->data->edit_button);
        mes->info->editing_room = mes->room->id;
        mes->info->editing = message->id;
    }
}

void file_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    gtk_widget_set_name(widget, "file_pressed");
    mes->message = mx_find_message(mes->room->messages, mes->id);
    if (mes->message != NULL)
        mx_load_file(mes);
}

void file1_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    gtk_widget_set_name(widget, "file_hover");
}

void file_notify_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    if (message != NULL) {
        gtk_widget_show(message->menu);
        gtk_widget_set_name(widget, "file_hover");
    }  
}

void file_notify1_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    gtk_widget_set_name(widget, "file");
}

int mx_show_widget(GtkWidget *widget) {
    gtk_widget_show(widget);
    return 0;
}

int mx_destroy_widget(GtkWidget *widget) {
    gtk_widget_destroy(widget);
    return 0;
}


int move_bar(t_room *room) {
    gtk_adjustment_set_value(room->Adjust, 
                            gtk_adjustment_get_upper(room->Adjust) - 
                            gtk_adjustment_get_page_size(room->Adjust) + 2.0);
    return 0;
}

t_message *mx_create_message(t_client_info *info, t_room *room, json_object *new_json, int order) {
    t_message *node =  (t_message *)malloc(sizeof(t_message));
    int id = json_object_get_int(json_object_object_get(new_json, "id"));
    int user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
    int add_info = json_object_get_int(json_object_object_get(new_json, "add_info"));
    const char *login = json_object_get_string(json_object_object_get(new_json, "login"));
    const char *message = json_object_get_string(json_object_object_get(new_json, "data"));

    node->id = id;
    node->data = strdup(message);

    t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
    mes->info = info;
    mes->room = room;
    mes->id = id;
    mes->user_id = user_id;
    node->h_box = gtk_box_new(FALSE, 0);
    gtk_box_pack_start (GTK_BOX (room->message_box), node->h_box, FALSE, FALSE, 0);
    if(order == 2)
        gtk_box_reorder_child (GTK_BOX (room->message_box), node->h_box, 0);
    GtkWidget *general_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(general_box);
    //-events
    GtkWidget *event = gtk_event_box_new();
    gtk_widget_show(event);
    gtk_widget_add_events (event, GDK_ENTER_NOTIFY_MASK);
    g_signal_connect (G_OBJECT (event), "enter_notify_event", G_CALLBACK (focus_callback), mes);
    g_signal_connect (G_OBJECT (event), "leave_notify_event", G_CALLBACK (focus1_callback), mes);
    gtk_box_pack_start (GTK_BOX (general_box), event, FALSE, FALSE, 0);
    //--
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_show(box);
    gtk_container_add (GTK_CONTAINER (event), box);
    gtk_widget_set_name(general_box, "message");
    gtk_container_set_border_width(GTK_CONTAINER(box), 1);

    GtkWidget *left_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(left_box);
    gtk_widget_set_size_request(left_box, 10, -1);
    gtk_box_pack_start(GTK_BOX (box), left_box, FALSE, FALSE, 0);
    GtkWidget *main_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(main_box);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(main_box), GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX (box), main_box, FALSE, FALSE, 0);
    GtkWidget *right_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(right_box);
    gtk_widget_set_size_request(right_box, 15, -1);
    gtk_box_pack_start(GTK_BOX (box), right_box, FALSE, FALSE, 0);
    //-menu
    GtkWidget *menu_event = gtk_event_box_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/options.png", 20, 40, TRUE, NULL);
    node->menu = gtk_image_new_from_pixbuf(pixbuf);
    gtk_container_add (GTK_CONTAINER (menu_event), node->menu);
    GtkWidget *menu  = gtk_menu_new ();
    //--items
    GtkWidget *delete = gtk_menu_item_new_with_label("Delete");
    gtk_widget_show(delete);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), delete);
    g_signal_connect (G_OBJECT (delete), "activate", G_CALLBACK (delete_callback), mes);

    if (add_info == 0) {
        GtkWidget *edit = gtk_menu_item_new_with_label("Edit");
        gtk_widget_show(edit);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), edit);
        g_signal_connect (G_OBJECT (edit), "activate", G_CALLBACK (edit_callback), mes);
    }
    //--
    gtk_widget_add_events (menu_event, GDK_ENTER_NOTIFY_MASK);
    gtk_widget_add_events (menu_event, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (menu_event), "enter_notify_event", G_CALLBACK (focus_menu_callback), mes);
    g_signal_connect (G_OBJECT (menu_event), "button_press_event", G_CALLBACK (open_menu_callback), G_OBJECT (menu));
    gtk_widget_show(menu_event);
    //--
    GtkWidget *box1 = gtk_box_new(FALSE, 0);
    gtk_widget_show(box1);
    GtkWidget *login_event = gtk_event_box_new();
    //--
    GtkWidget *login_menu  = gtk_menu_new ();
    GtkWidget *view = gtk_menu_item_new_with_label("View profile");
    gtk_widget_show(view);
    gtk_menu_shell_append (GTK_MENU_SHELL (login_menu), view);
    g_signal_connect (G_OBJECT (view), "activate", G_CALLBACK (load_profile_callback), mes);
    //--
    gtk_widget_add_events (login_event, GDK_ENTER_NOTIFY_MASK);
    gtk_widget_add_events (login_event, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (login_event), "enter_notify_event", 
                        G_CALLBACK (focus_callback), mes);
    g_signal_connect (G_OBJECT (login_event), "button_press_event", 
                        G_CALLBACK (open_menu_callback), G_OBJECT(login_menu));
    GtkWidget *label1 = gtk_label_new(login);
    gtk_container_add(GTK_CONTAINER(login_event), label1);
    gtk_box_pack_start(GTK_BOX (main_box), box1, FALSE, FALSE, 0);
    gtk_widget_show(label1);
    gtk_widget_show(login_event);
    
    if (add_info == 0) {
        node->message_box =  gtk_box_new(FALSE, 0);
        gtk_widget_show(node->message_box);
        gtk_box_pack_start (GTK_BOX (main_box), node->message_box, FALSE, FALSE, 0);
        node->message_label = gtk_label_new(message);
        gtk_widget_show(node->message_label);
        gtk_box_pack_start (GTK_BOX (node->message_box), node->message_label, FALSE, FALSE, 0);
    }
    else if (add_info == 1) {
        node->message_box = gtk_event_box_new();
        gtk_widget_show(node->message_box);
        gtk_box_pack_start (GTK_BOX (main_box), node->message_box, FALSE, FALSE, 0);
        GtkWidget *label2 = gtk_label_new(message + 20);
        gtk_widget_show(label2);
        gtk_container_add (GTK_CONTAINER (node->message_box), label2);
        gtk_widget_set_name(node->message_box, "file");
        gtk_widget_add_events (node->message_box, GDK_BUTTON_PRESS_MASK);
        g_signal_connect (G_OBJECT (node->message_box), "button_press_event", G_CALLBACK (file_callback), mes);
        g_signal_connect (G_OBJECT (node->message_box), "button_release_event", G_CALLBACK (file1_callback), mes);
        gtk_widget_add_events (node->message_box, GDK_ENTER_NOTIFY_MASK);
        g_signal_connect (G_OBJECT (node->message_box), "enter_notify_event", G_CALLBACK (file_notify_callback), mes);
        g_signal_connect (G_OBJECT (node->message_box), "leave_notify_event", G_CALLBACK (file_notify1_callback), mes);
    }
    else if (add_info == 2) {
        node->message_box = gtk_box_new(FALSE, 0);
        gtk_widget_show(node->message_box);
        gtk_box_pack_start (GTK_BOX (main_box), node->message_box, FALSE, FALSE, 0);
        node->image_box = gtk_box_new(FALSE, 0);
        gtk_widget_show(node->image_box);
        gtk_box_pack_start (GTK_BOX (node->message_box), node->image_box, FALSE, FALSE, 0);
    }
    else if (add_info == 3) {
        GtkWidget *box2 =  gtk_box_new(FALSE, 0);
        gtk_widget_show(box2);
        gtk_box_pack_start (GTK_BOX (main_box), box2, FALSE, FALSE, 0);
        //char *path = mx_strjoin("stickers/", message);
        GdkPixbuf *item_pixbuf = gdk_pixbuf_new_from_file_at_scale (message, 200, 200, TRUE, NULL);
        GtkWidget *item_image = gtk_image_new_from_pixbuf(item_pixbuf);;
        gtk_widget_show(item_image);
        gtk_box_pack_start (GTK_BOX (box2), item_image, FALSE, FALSE, 0);
    }

    if (user_id == info->id) {
        gtk_box_pack_end(GTK_BOX (right_box), menu_event, FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX (box1), login_event, FALSE, FALSE, 0);
        gtk_box_pack_end (GTK_BOX (node->h_box), general_box, FALSE, FALSE, 0);
        if (add_info != 2) {
            g_idle_add ((GSourceFunc)mx_show_widget, node->h_box);
            if (order == 1) {
                sleep_ms(25);
                g_idle_add ((GSourceFunc)move_bar, room);
            }
        }
    }
    else {
        gtk_box_pack_start (GTK_BOX (box1), login_event, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (node->h_box), general_box, FALSE, FALSE, 0);
        if (add_info != 2)
            g_idle_add ((GSourceFunc)mx_show_widget, node->h_box);
    }
    node->next = NULL;
    return node;
}

void mx_push_message(t_client_info *info, t_room *room, json_object *new_json) {
    t_message *tmp;
    t_message *p;
    t_message **list = &room->messages;

    if (!list)
        return;
    tmp = mx_create_message(info, room, new_json, 1);  // Create new
    if (!tmp)
        return;
    p = *list;
    if (*list == NULL) {  // Find Null-node
        *list = tmp;
    }
    else {
        while (p->next != NULL)  // Find Null-node
            p = p->next;
        p->next = tmp;
    }
    int id = json_object_get_int(json_object_object_get(new_json, "id"));
    int add_info = json_object_get_int(json_object_object_get(new_json, "add_info"));
    if (add_info == 2) {
        t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
        mes->info = info;
        mes->room = room;
        mes->id = id;
        mx_load_file(mes);
    }
}
