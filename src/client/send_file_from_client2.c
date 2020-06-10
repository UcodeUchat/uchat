#include "uchat.h"

static GtkWidget *out_gtk_file_chooser_dialog_new(t_client_info *info) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new ("Save File",
                        GTK_WINDOW(info->data->window),
                        GTK_FILE_CHOOSER_ACTION_OPEN,
                        ("_Cancel"),
                        GTK_RESPONSE_CANCEL,
                        ("_OK"),
                        GTK_RESPONSE_ACCEPT,
                        NULL);
    return dialog;
}

static int set_vars(GtkWidget **dialog, gint *resp, char **file_name, char **p) {
    if ((*resp = gtk_dialog_run(GTK_DIALOG(*dialog)))
                                ==  GTK_RESPONSE_ACCEPT) {
        *p = strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(*dialog)));
        *file_name = strdup(*p);
        while (mx_get_char_index(*file_name, '/') >= 0) {
            char *tmp = strdup(*file_name + mx_get_char_index(*file_name,
                                                              '/') + 1);

            free(*file_name);
            *file_name = strdup(tmp);
            free(tmp); 
        }
        gtk_widget_destroy(*dialog);
        return 0;
    }
    else
        gtk_widget_destroy(*dialog);
    return 1;
}

bool mx_pick_file_to_send(t_client_info *info, FILE **file, json_object **js) {
    struct stat s_stat;
    char *file_path;
    char *file_name;
    GtkWidget *dialog = out_gtk_file_chooser_dialog_new(info);
    gint resp;

    gtk_widget_show_all(dialog);
    if (set_vars(&dialog, &resp, &file_name, &file_path) == MX_OK) {
        if (stat(file_path, &s_stat) != MX_OK)
            return 1;
        if ((s_stat.st_size > 0 && s_stat.st_size <= MX_MAX_FILE_SIZE)
            && (!(s_stat.st_mode & S_IFDIR) && (s_stat.st_mode & S_IFREG)))
            if ((*file = fopen(file_path, "r")) != NULL) {
                *js = mx_create_basic_json_object(MX_FILE_SEND_TYPE);
                mx_js_o_o_add(*js, "file_name", mx_js_n_str(file_name));
                mx_js_o_o_add(*js, "file_size", mx_js_n_int(s_stat.st_size));
                return 0;
            }
    }
    return 1;
}
