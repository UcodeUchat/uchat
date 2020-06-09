#include "uchat.h"

static void send_record_thread (void *data) {
    t_client_info *info = (t_client_info *)data;

    printf("new record %s\n", info->record_file);
    printf("start send record\n");
    mx_send_file_from_client(info, info->record_file);
}


static void *record_thread (void *data) {
    t_client_info *info = (t_client_info *)data;

    info->record_file = strdup(mx_record_audio());
    pthread_cleanup_push(send_record_thread, info);
    if (data)
        pthread_exit((void *) 1);

    pthread_cleanup_pop(0);
    pthread_exit((void *) 0);
}

void mx_record_callback (GtkWidget *widget, t_client_info *info) {
    pthread_t thread_record;
    pthread_attr_t attr;
    int tc;

    info->record_file = NULL;
    pthread_attr_init(&attr);
//    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // #
    tc = pthread_create(&thread_record, &attr, record_thread, info);
    if (tc != 0)
        printf("pthread_create error = %s\n", strerror(tc));

    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
    t_room *room = mx_find_room_position(info->data->rooms, position);
    info->data->current_room = room->id;

//    while(info->record_file == NULL) { ; }  // wait record
//    printf("new %s\n", info->record_file);
//    mx_send_file_from_client(info, info->record_file);
    (void)widget;
}

void mx_choose_file_callback(GtkWidget *widget, t_client_info *info) {
    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
    t_room *room = mx_find_room_position(info->data->rooms, position);

    info->data->current_room = room->id;
    mx_send_file_from_client(info, NULL);
    (void)widget;
}

void mx_choose_sticker_callback(GtkWidget *widget, GtkWidget *menu) {
    (void)widget;
    gtk_menu_popup_at_pointer (GTK_MENU(menu), NULL);
}
