#include "uchat.h"

static void *play_sound_pthread(void *mes) {
    t_mes *tmp = (t_mes *)mes;

    mx_play_sound_file(tmp, "0", NULL);
    tmp->info->cant_play = 1;
    return 0;
}

static void stop_cb(GtkWidget *widget, t_mes *mes) {
    (void)widget;
    fprintf(stderr, "pause\n");
    mes->audio->pause = 1;
    mes->audio->play = 0;
}

static void play_cb(GtkWidget *widget, t_mes *mes) {
    (void)widget;
    pthread_t sound_play;
    int tc;

    if (mes->audio->play == 0 && mes->info->cant_play == 1) {
        mes->info->cant_play = 0;
        mes->audio->pause = 0;
        mes->audio->play = 1;
        mx_load_file(mes);
        tc = pthread_create(&sound_play, NULL, play_sound_pthread, mes);
        if (tc != 0)
            printf("pthread_create error = %s\n", strerror(tc));
    }
    mes->audio->play = 0;
}

static GtkWidget *make_button(t_mes *mes, char *name, 
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

void mx_audio (t_mes *mes, t_message *node) {
    GtkWidget *box_all = gtk_box_new(FALSE, 0);
    GtkWidget *v_mess = gtk_label_new("Voice message:");
    GtkWidget *box_butt = gtk_box_new(FALSE, 0);
    GtkWidget *b_pause = make_button(mes, "img/stop.png", stop_cb);
    GtkWidget *b_play = make_button(mes, "img/play.png", play_cb);

    gtk_orientable_set_orientation(GTK_ORIENTABLE(box_all),\
                                    GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX(node->central_box), box_all, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_all), v_mess, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_all), box_butt, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_butt), b_pause, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_butt), b_play, FALSE, FALSE, 0);
    gtk_widget_show_all(box_all);
}
