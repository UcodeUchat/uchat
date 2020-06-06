#include "uchat.h"

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

void focus_menu_callback(GtkWidget *widget, GdkEventButton *event, GtkWidget *menu) {
    (void)widget;
    (void)event;
    gtk_widget_show(menu);
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


void *play_sound_pthread(void *taken_info) {
    char *file = (char *)taken_info;
    (void)file;

//    mx_play_sound_file("./audio/moby.aif", "0", NULL);
    mx_play_sound_file(mx_strjoin("./Uchat_downloads/", file), "0", NULL);
    return 0;
}

void load_audio_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    pthread_t sound_play;
    pthread_attr_t attr;
    int tc;

    mx_load_file(mes);
    printf("%s\n", mx_strjoin("./Uchat_downloads/", mes->message->data));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // #
    tc = pthread_create(&sound_play, &attr, play_sound_pthread, mes->message->data);
    if (tc != 0)
        printf("pthread_create error = %s\n", strerror(tc));
}

void delete_callback (GtkWidget *widget, t_mes *mes) {
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    json_object  *new_json = json_object_new_object();
    const char *json_str = json_object_to_json_string(new_json);

    json_object_object_add(new_json, "type", json_object_new_int(MX_DELETE_MESSAGE_TYPE));
    json_object_object_add(new_json, "message_id", json_object_new_int(message->id));
    json_object_object_add(new_json, "room_id", json_object_new_int(mes->room->id));
    mx_print_json_object(new_json, "delete message");
    tls_send(mes->info->tls_client, json_str, strlen(json_str));
    (void)widget;
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

void choose_side (t_client_info *info, t_message *node, t_room *room, int order) {
    if (node->user_id == info->id) {
        gtk_box_pack_end(GTK_BOX (node->right_box), node->menu_event, FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX (node->login_box), node->login_event, FALSE, FALSE, 0);
        gtk_box_pack_end (GTK_BOX (node->h_box), node->general_box, FALSE, FALSE, 0);
        if (node->add_info != 2) {
            g_idle_add ((GSourceFunc)mx_show_widget, node->h_box);
            if (order == 1) {
                sleep_ms(50);
                g_idle_add ((GSourceFunc)move_bar, room);
            }
        }
    }
    else {
        gtk_box_pack_start (GTK_BOX (node->login_box), node->login_event, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (node->h_box), node->general_box, FALSE, FALSE, 0);
        if (node->add_info != 2)
            g_idle_add ((GSourceFunc)mx_show_widget, node->h_box);
    }
}

void simple_message (t_message *node, const char *message) {
    node->message_box =  gtk_box_new(FALSE, 0);
    gtk_widget_show(node->message_box);
    gtk_box_pack_start (GTK_BOX (node->central_box), node->message_box, FALSE, FALSE, 0);
    node->message_label = gtk_label_new(message);
    gtk_widget_show(node->message_label);
    gtk_box_pack_start (GTK_BOX (node->message_box), node->message_label, FALSE, FALSE, 0);
}

void file (t_mes *mes, t_message *node, const char *message) {
    GtkWidget *label = gtk_label_new(message + 20);

    node->message_box = gtk_event_box_new();
    gtk_widget_show(node->message_box);
    gtk_box_pack_start (GTK_BOX (node->central_box), node->message_box, FALSE, FALSE, 0);
    gtk_widget_show(label);
    gtk_container_add (GTK_CONTAINER (node->message_box), label);
    gtk_widget_set_name(node->message_box, "file");
    gtk_widget_add_events (node->message_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (node->message_box), "button_press_event", G_CALLBACK (file_callback), mes);
    g_signal_connect (G_OBJECT (node->message_box), "button_release_event", G_CALLBACK (file1_callback), mes);
    gtk_widget_add_events (node->message_box, GDK_ENTER_NOTIFY_MASK);
    g_signal_connect (G_OBJECT (node->message_box), "enter_notify_event", G_CALLBACK (file_notify_callback), mes);
    g_signal_connect (G_OBJECT (node->message_box), "leave_notify_event", G_CALLBACK (file_notify1_callback), mes);
}

void image (t_message *node) {
    node->message_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->message_box);
    gtk_box_pack_start (GTK_BOX (node->central_box), node->message_box, FALSE, FALSE, 0);
    node->image_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->image_box);
    gtk_box_pack_start (GTK_BOX (node->message_box), node->image_box, FALSE, FALSE, 0);
}

void sticker (t_message *node, const char *message) {
    GtkWidget *stick_box =  gtk_box_new(FALSE, 0);
    GdkPixbuf *item_pixbuf = gdk_pixbuf_new_from_file_at_scale (message, 200, 200, TRUE, NULL);
    GtkWidget *item_image = gtk_image_new_from_pixbuf(item_pixbuf);

    gtk_widget_show(stick_box);
    gtk_box_pack_start (GTK_BOX (node->central_box), stick_box, FALSE, FALSE, 0);
    gtk_widget_show(item_image);
    gtk_box_pack_start (GTK_BOX (stick_box), item_image, FALSE, FALSE, 0);
}

void audio (t_mes *mes, t_message *node, const char *message) {
    GtkWidget *label2 = gtk_label_new(message + 20);

    node->message_box = gtk_event_box_new();
    gtk_widget_show(node->message_box);
    gtk_box_pack_start (GTK_BOX (node->central_box), node->message_box, FALSE, FALSE, 0);
    gtk_widget_show(label2);
    gtk_container_add (GTK_CONTAINER (node->message_box), label2);
    gtk_widget_set_name(node->message_box, "file");
    gtk_widget_add_events (node->message_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (node->message_box), "button_press_event", G_CALLBACK (load_audio_callback), mes);
}

void choose_type (t_mes *mes, t_message *node, const char *message) {
    if (node->add_info == 0)
        simple_message (node, message);
    else if (node->add_info == 1) 
        file (mes, node, message);
    else if (node->add_info == 2)
        image (node);
    else if (node->add_info == 3)
        sticker (node, message);
    else if (node->add_info == 4)
        audio (mes, node, message);
}

void create_item (GtkWidget *menu, t_mes *mes, char *title, 
            void (*callback) (GtkWidget *widget, t_mes *mes)) {
    GtkWidget *item = gtk_menu_item_new_with_label(title);

    gtk_widget_show(item);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (callback), mes);
}

void init_message_menu (t_message *node, t_mes *mes) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/options.png", 20, 40, TRUE, NULL);
    GtkWidget *menu  = gtk_menu_new ();

    node->menu_event = gtk_event_box_new();
    node->menu = gtk_image_new_from_pixbuf(pixbuf);
    gtk_container_add (GTK_CONTAINER (node->menu_event), node->menu);
    create_item (menu, mes, "Delete", delete_callback);
    if (node->add_info == 0)
        create_item (menu, mes, "Edit", edit_callback);
    gtk_widget_add_events (node->menu_event, GDK_ENTER_NOTIFY_MASK);
    gtk_widget_add_events (node->menu_event, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (node->menu_event), "enter_notify_event", G_CALLBACK (focus_menu_callback), node->menu);
    g_signal_connect (G_OBJECT (node->menu_event), "button_press_event", G_CALLBACK (open_menu_callback), G_OBJECT (menu));
    gtk_widget_show(node->menu_event);
}

void init_message_login (t_message *node, t_mes *mes, const char *login) {
    GtkWidget *login_menu  = gtk_menu_new ();
    GtkWidget *view = gtk_menu_item_new_with_label("View profile");
    GtkWidget *label = gtk_label_new(login);

    node->login_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->login_box);
    node->login_event = gtk_event_box_new();
    gtk_widget_show(view);
    gtk_menu_shell_append (GTK_MENU_SHELL (login_menu), view);
    g_signal_connect (G_OBJECT (view), "activate", G_CALLBACK (load_profile_callback), mes);
    gtk_widget_add_events (node->login_event, GDK_ENTER_NOTIFY_MASK);
    gtk_widget_add_events (node->login_event, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (node->login_event), "enter_notify_event", 
                        G_CALLBACK (focus_callback), mes);
    g_signal_connect (G_OBJECT (node->login_event), "button_press_event", 
                        G_CALLBACK (open_menu_callback), G_OBJECT(login_menu));
    gtk_widget_set_name (label, "login");
    gtk_container_add(GTK_CONTAINER(node->login_event), label);
    gtk_box_pack_start(GTK_BOX (node->central_box), node->login_box, FALSE, FALSE, 0);
    gtk_widget_show(label);
    gtk_widget_show(node->login_event);
}

void init_main_message2 (t_message *node) {
    node->left_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->left_box);
    gtk_widget_set_size_request(node->left_box, 10, -1);
    gtk_box_pack_start(GTK_BOX (node->main_box), node->left_box, FALSE, FALSE, 0);
    node->central_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->central_box);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(node->central_box), GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX (node->main_box), node->central_box, FALSE, FALSE, 0);
    node->right_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->right_box);
    gtk_widget_set_size_request(node->right_box, 15, -1);
    gtk_box_pack_start(GTK_BOX (node->main_box), node->right_box, FALSE, FALSE, 0);
}

void init_main_message (t_message *node, t_mes *mes) {
    node->general_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->general_box);
    node->event = gtk_event_box_new();
    gtk_widget_show(node->event);
    gtk_widget_add_events (node->event, GDK_ENTER_NOTIFY_MASK);
    g_signal_connect (G_OBJECT (node->event), "enter_notify_event", G_CALLBACK (focus_callback), mes);
    g_signal_connect (G_OBJECT (node->event), "leave_notify_event", G_CALLBACK (focus1_callback), mes);
    gtk_box_pack_start (GTK_BOX (node->general_box), node->event, FALSE, FALSE, 0);
    node->main_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(node->main_box);
    gtk_container_add (GTK_CONTAINER (node->event), node->main_box);
    gtk_widget_set_name(node->general_box, "message");
    gtk_container_set_border_width(GTK_CONTAINER(node->main_box), 1);
    init_main_message2(node);
}

void init_message (t_message *node, t_room *room, json_object *new_json, int order) {
    node->id = json_object_get_int(json_object_object_get(new_json, "id"));
    node->user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
    node->add_info = json_object_get_int(json_object_object_get(new_json, "add_info"));
    node->data = strdup (json_object_get_string(json_object_object_get(new_json, "data")));
    node->h_box = gtk_box_new(FALSE, 0);
    gtk_box_pack_start (GTK_BOX (room->message_box), node->h_box, FALSE, FALSE, 0);
    if (order == 2)
        gtk_box_reorder_child (GTK_BOX (room->message_box), node->h_box, 0);
}

void init_message_data (t_client_info *info, t_room *room, json_object *new_json, t_mes *mes) {
    mes->info = info;
    mes->room = room;
    mes->id = json_object_get_int(json_object_object_get(new_json, "id"));;
    mes->user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
}

t_message *mx_create_message (t_client_info *info, t_room *room, json_object *new_json, int order) {
    t_message *node =  (t_message *)malloc(sizeof(t_message)); 
    t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
    const char *login = json_object_get_string(json_object_object_get(new_json, "login"));
    const char *message = json_object_get_string(json_object_object_get(new_json, "data"));

    init_message(node, room, new_json, order);
    init_message_data(info, room, new_json, mes);
    init_main_message(node, mes);
    init_message_menu(node, mes);
    init_message_login(node, mes, login);
    mes->message = node;
    choose_type(mes, node, message);
    choose_side (info, node, room, order);

    node->next = NULL;
    return node;
}

void *sound_thread (void *data) {
    (void)data;
    mx_play_sound_file("audio/moby.aif", "0", "5");
    return 0;
}

void additional_act (t_client_info *info, t_room *room, json_object *new_json) {
    int id = json_object_get_int(json_object_object_get(new_json, "id"));
    int add_info = json_object_get_int(json_object_object_get(new_json, "add_info"));
    int user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));

    if (info->audio == 1) {
        if (user_id != info->id) {
            pthread_t sound_t = NULL;
            pthread_create(&sound_t, 0, sound_thread, NULL);
        }
    }
    if (add_info == 2) {
        t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
        mes->info = info;
        mes->room = room;
        mes->id = id;
        mx_load_file(mes);
    }
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
    additional_act (info, room, new_json);
}
