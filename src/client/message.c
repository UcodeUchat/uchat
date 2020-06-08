#include "uchat.h"

void load_audio_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    
    printf("mes----->%s\n", mes->message->data);
    mx_play_sound_file("./audio/moby.aif", "0", "5");
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

static void choose_type (t_mes *mes, t_message *node, const char *message) {
    if (node->add_info == 0)
        mx_simple_message (node, message);
    else if (node->add_info == 1) 
        mx_file (mes, node, message);
    else if (node->add_info == 2)
        mx_image (node);
    else if (node->add_info == 3)
        mx_sticker (node, message);
    else if (node->add_info == 4)
        audio (mes, node, message);
}

t_message *mx_create_message (t_client_info *info, t_room *room, 
                json_object *new_json, int order) {
    t_message *node =  (t_message *)malloc(sizeof(t_message)); 
    t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
    const char *login = json_object_get_string(json_object_object_get(new_json, "login"));
    const char *message = json_object_get_string(json_object_object_get(new_json, "data"));

    mx_init_message(node, room, new_json, order);
    mx_init_message_data(info, room, new_json, mes);
    mx_init_main_message(node, mes);
    mx_init_message_menu(node, mes);
    mx_init_message_login(node, mes, login);
    mes->message = node;
    choose_type(mes, node, message);
    mx_choose_side (info, node, room, order);

    node->next = NULL;
    return node;
}

static void *sound_thread (void *data) {
    (void)data;
    mx_play_sound_file("audio/moby.aif", "0", "5");
    return 0;
}

static void additional_act (t_client_info *info, t_room *room, json_object *new_json) {
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
