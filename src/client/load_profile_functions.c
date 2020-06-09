#include "uchat.h"

static void save_settings_callback(GtkWidget *widget, t_client_info *info) {
    json_object *new_json;
    const char *json_string;

    (void)widget;
    new_json = mx_js_n_o();
    mx_js_o_o_add(new_json, "type", mx_js_n_int(MX_EDIT_PROFILE_TYPE));
    mx_js_o_o_add(new_json, "add_info", mx_js_n_int(1));
    mx_js_o_o_add(new_json, "user_id", mx_js_n_int(info->id));
    mx_js_o_o_add(new_json, "visual_n", mx_js_n_int(
        gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(info->data->profile->visual))));
    mx_js_o_o_add(new_json, "audio_n", mx_js_n_int(
        gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(info->data->profile->audio))));
    mx_js_o_o_add(new_json, "email_n", mx_js_n_int(
        gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(info->data->profile->email))));
    json_string = mx_js_o_to_js_str(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
}

static void init_check_boxes (t_client_info *info, json_object *new_json, GtkWidget *notifications_box) {
    int visual_n = mx_js_g_int(mx_js_o_o_get(new_json, "visual_n"));
    int audio_n = mx_js_g_int(mx_js_o_o_get(new_json, "audio_n"));
    int email_n = mx_js_g_int(mx_js_o_o_get(new_json, "email_n"));

    info->data->profile->visual = gtk_check_button_new_with_label ("Visual");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(info->data->profile->visual), visual_n);
    gtk_box_pack_start (GTK_BOX (notifications_box), info->data->profile->visual, FALSE, FALSE, 0);
    info->data->profile->audio = gtk_check_button_new_with_label ("Audio");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(info->data->profile->audio), audio_n);
    gtk_box_pack_start (GTK_BOX (notifications_box), info->data->profile->audio, FALSE, FALSE, 0);
    info->data->profile->email = gtk_check_button_new_with_label ("Email");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(info->data->profile->email), email_n);
    gtk_box_pack_start (GTK_BOX (notifications_box), info->data->profile->email, FALSE, FALSE, 0);
    info->data->profile->save = gtk_button_new_with_label("Save");
    g_signal_connect (G_OBJECT (info->data->profile->save), "clicked", G_CALLBACK (save_settings_callback), info);
    gtk_box_pack_start (GTK_BOX (notifications_box), info->data->profile->save, FALSE, FALSE, 0);
}

void mx_show_global_settings (t_client_info *info, json_object *new_json, GtkWidget *profile) {
    GtkWidget *box = gtk_box_new(FALSE, 5);
    GtkWidget *notifications_box = gtk_box_new(FALSE, 5);

    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(notifications_box), GTK_ORIENTATION_VERTICAL);
    mx_show_title(" Notifications", box);
    gtk_box_pack_start (GTK_BOX (box), notifications_box, FALSE, FALSE, 0);
    init_check_boxes(info, new_json, notifications_box);
    gtk_widget_show_all(box);
}

static void message_callback (GtkWidget *widget, t_client_info *info) {
    json_object *new_json;
    const char *json_string;

    (void)widget;
    new_json = mx_js_n_o();
    mx_js_o_o_add(new_json, "type", mx_js_n_int(MX_DIRECT_MESSAGE_TYPE));
    mx_js_o_o_add(new_json, "first_id", mx_js_n_int(info->id));
    mx_js_o_o_add(new_json, "second_id", mx_js_n_int(info->data->profile->id));
    json_string = mx_js_o_to_js_str(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
    gtk_widget_destroy(info->data->profile->main_box);
    free(info->data->profile);
    info->data->profile = NULL;
    gtk_widget_hide(info->data->menu);
    if (info->data->search != NULL) {
        gtk_widget_hide(info->data->search->main_box);
        free(info->data->search);
        info->data->search = NULL;
    }
}

void mx_show_etc (t_client_info *info, json_object *new_json, GtkWidget *profile) {
    GtkWidget *box = gtk_box_new(FALSE, 5);
    GtkWidget *activityes_box = gtk_box_new(FALSE, 5);

    (void)info;
    (void)new_json;
    gtk_box_pack_start (GTK_BOX (profile), box, FALSE, FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(activityes_box), GTK_ORIENTATION_VERTICAL);
    mx_show_title(" Activityes", box);
    gtk_box_pack_start (GTK_BOX (box), activityes_box, FALSE, FALSE, 0);
    GtkWidget *button = gtk_button_new_with_label("Message");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (message_callback), info);
    gtk_box_pack_start (GTK_BOX (activityes_box), button, FALSE, FALSE, 0);
    gtk_widget_show(button);
    gtk_widget_show(activityes_box);
    gtk_widget_show(box);
}
