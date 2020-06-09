#include "uchat.h"

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
    tc = pthread_create(&sound_play, &attr, play_sound_pthread, \
                        mes->message->data);
    if (tc != 0)
        printf("pthread_create error = %s\n", strerror(tc));
}

void mx_pause_cb(GtkWidget *widget, t_mes *mes) {
    (void)widget;
    (void)mes;
    fprintf(stderr, "pause\n");
}

void mx_play_cb(GtkWidget *widget, t_mes *mes) {
    (void)widget;
    (void)mes;
    fprintf(stderr, "play\n");
}

GtkWidget *mx_make_button(t_mes *mes, char *name, 
                            void(*callback)(GtkWidget *widget, t_mes *mes) ) {
    GtkWidget *button1 = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale (name, 20, 20, TRUE,\
                                                            NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

    gtk_button_set_image(GTK_BUTTON(button1), image);
    gtk_widget_show(image);
    g_signal_connect(G_OBJECT(button1), "clicked", G_CALLBACK(callback), mes);
    return button1;
}

void audio (t_mes *mes, t_message *node) {
    GtkWidget *box_all = gtk_box_new(FALSE, 0);
    GtkWidget *v_mess = gtk_label_new("Voice message:");
    GtkWidget *box_butt = gtk_box_new(FALSE, 0);
    GtkWidget *b_pause = mx_make_button(mes, "img/pause.png", mx_pause_cb);
    GtkWidget *b_play = mx_make_button(mes, "img/play.png", mx_play_cb);

    gtk_orientable_set_orientation(GTK_ORIENTABLE(box_all),\
                                    GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX(node->central_box), box_all, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_all), v_mess, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_all), box_butt, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_butt), b_pause, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_butt), b_play, FALSE, FALSE, 0);
    gtk_widget_show_all(box_all);
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
        audio (mes, node);
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
    mx_play_sound_file("audio/message_receive.aiff", "0", "1");
//    mx_play_sound_file("audio/moby.aif", "0", "3");
    return 0;
}

static void additional_act (t_client_info *info, t_room *room, 
                            json_object *new_json, t_message *tmp) {
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
        mes->message = tmp;
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
    additional_act (info, room, new_json, tmp);
}
