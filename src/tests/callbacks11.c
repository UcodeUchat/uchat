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
