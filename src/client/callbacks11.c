#include "uchat.h"

void mx_focus_menu_callback(GtkWidget *widget, 
                    GdkEventButton *event, GtkWidget *menu) {
    (void)widget;
    (void)event;
    gtk_widget_show(menu);
}

void mx_open_menu_callback(GtkWidget *widget, 
                    GdkEventButton *event, GtkWidget *menu) {
    (void)widget;
    (void)event;
    gtk_menu_popup_at_pointer (GTK_MENU(menu), NULL);
}

void mx_load_profile_callback(GtkWidget *widget, t_mes *mes) {
    (void)widget;
    mx_load_profile_client(mes->info, mes->user_id);
}

void mx_record_callback (GtkWidget *widget, t_client_info *info) {
    char *audio_file = mx_record_audio();
    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
    t_room *room = mx_find_room_position(info->data->rooms, position);
    
    info->data->current_room = room->id;
    printf("new %s\n", audio_file);
    mx_send_file_from_client(info, audio_file);
    (void)widget;
}
