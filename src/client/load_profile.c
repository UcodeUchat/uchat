#include "uchat.h"

void mx_show_title (const char *title, GtkWidget *box) {
    GtkWidget *title_box = gtk_box_new(FALSE, 5);
    GtkWidget *label = gtk_label_new(title);

    gtk_box_pack_start (GTK_BOX (box), title_box, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (title_box), label, FALSE, FALSE, 0);
    gtk_widget_show(label);
    gtk_widget_show(title_box);
}

static void init_profile (t_client_info *info, GtkWidget *box, int id, json_object *new_json) {
    GtkWidget *profile = gtk_box_new(FALSE, 5);

    gtk_box_pack_start (GTK_BOX (box), profile, FALSE, FALSE, 0);
    gtk_widget_set_name(profile, "profile");
    gtk_fixed_put(GTK_FIXED(info->data->general_box), 
        info->data->profile->main_box, 0, 0);
    gtk_widget_set_size_request(info->data->profile->main_box, 
                            gtk_widget_get_allocated_width (info->data->window), 
                            gtk_widget_get_allocated_height (info->data->window));
    gtk_orientable_set_orientation (GTK_ORIENTABLE(profile), 
                                    GTK_ORIENTATION_VERTICAL);
    mx_show_exit(info, profile);
    mx_show_login(info, new_json, profile);
    mx_show_id(info, id, profile);
    mx_show_name(info, id, new_json, profile);
    mx_show_email(info, id, new_json, profile);
    gtk_widget_show(profile);
    id == info->id ? mx_show_global_settings(info, new_json, profile) : 
                     mx_show_etc(info, new_json, profile);
}

static void init_close_event (t_client_info *info, GtkWidget *box) {
    GtkWidget *close_event = gtk_event_box_new();

    gtk_widget_set_name (close_event, "menu_exit");
    gtk_widget_add_events (close_event, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (close_event), "button_press_event", 
                        G_CALLBACK (mx_close_profile_callback1), info);
    gtk_box_pack_start (GTK_BOX (box), close_event, TRUE, TRUE, 0);
    gtk_widget_show(close_event);
}

void mx_load_user_profile(t_client_info *info, json_object *new_json) {
    int id = mx_js_g_int(mx_js_o_o_get(new_json, "id"));
    GtkWidget *box = gtk_box_new(FALSE, 0);

    if (info->data->profile != NULL) {
        g_idle_add ((GSourceFunc)mx_destroy_widget, info->data->profile->main_box);
        free(info->data->profile);
        info->data->profile = NULL;
    }
    info->data->profile = (t_prof *)malloc(sizeof(t_prof));
    info->data->profile->id = id;
    info->data->profile->main_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(info->data->profile->main_box),box);
    gtk_widget_show(box);
    init_profile(info, box, id ,new_json);
    init_close_event(info, box);
    g_idle_add ((GSourceFunc)mx_show_widget, info->data->profile->main_box);
}
