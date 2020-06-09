#include "uchat.h"

char *get_name(char *path) {
    char *extention = strdup(path);
    char *tmp = NULL;
    while (mx_get_char_index(extention, '/') >= 0) {
        tmp = strdup(extention + mx_get_char_index(extention, '/') + 1);
        free(extention);
        extention = strdup(tmp);
        free(tmp);
    }
    return extention;
}

bool pick_file_to_send(t_client_info *info, FILE **file, json_object **obj) {
    struct stat file_stat;
    char *file_path;
    char *file_name;
    GtkWidget *dialog = gtk_file_chooser_dialog_new ("Save File",
                                      GTK_WINDOW(info->data->window),
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_OK"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);
    gtk_widget_show_all(dialog);
    gint resp = gtk_dialog_run(GTK_DIALOG(dialog));
    if (resp ==  GTK_RESPONSE_ACCEPT) {
        file_path = strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
        file_name = strdup(file_path);
        while (mx_get_char_index(file_name, '/') >= 0) {
            char *tmp = strdup(file_name + mx_get_char_index(file_name, '/') + 1);
            free(file_name);
            file_name = strdup(tmp);
            free(tmp); 
        }
        gtk_widget_destroy(dialog);
        if (stat(file_path, &file_stat) != MX_OK)
            return 1;
        if (file_stat.st_size > 0 && file_stat.st_size <= MX_MAX_FILE_SIZE) {
            if (!(file_stat.st_mode & S_IFDIR) && (file_stat.st_mode & S_IFREG)) {
                if ((*file = fopen(file_path, "r")) != NULL) {
                    *obj = mx_create_basic_json_object(MX_FILE_SEND_TYPE);
                    mx_js_o_o_add(*obj, "file_name", mx_js_n_str(file_name));
                    mx_js_o_o_add(*obj, "file_size", mx_js_n_int(file_stat.st_size));
                    return 0;
                }
                else
                    fprintf(stderr, "Хм, під час відкривання файлу щось пішло не так\n");
            }
            else
                fprintf(stderr, "Йой, непідтримуваний тип файлу\n");
        }
        else
            fprintf(stderr, "Схоже, що твій файл надто великий\nМаксимальний розмір файлу %d\n", MX_MAX_FILE_SIZE);
    }
    else {
        gtk_widget_destroy(dialog);
    }
    return 1;
}
static bool open_file_to_send(FILE **file, json_object **obj, char *path) {
    struct stat file_stat;

    if (stat(path, &file_stat) != MX_OK)
        return 1;
    if (file_stat.st_size > 0 && file_stat.st_size <= MX_MAX_FILE_SIZE) {
        if (!(file_stat.st_mode & S_IFDIR) && (file_stat.st_mode & S_IFREG)) {
            if ((*file = fopen(path, "r")) != NULL) {
                char *file_name = get_name(path);

                *obj = mx_create_basic_json_object(MX_FILE_SEND_TYPE);
                mx_js_o_o_add(*obj, "file_name", mx_js_n_str(file_name));
                mx_js_o_o_add(*obj, "file_size", mx_js_n_int(file_stat.st_size));
                mx_strdel(&file_name);
                return 0;
            }
            else
                fprintf(stderr, "Хм, під час відкривання файлу щось пішло не так\n");
        }
        else
            fprintf(stderr, "Йой, непідтримуваний тип файлу\n");
    }
    else
        fprintf(stderr, "Схоже, що твій файл надто великий\nМаксимальний розмір файлу %d\n", MX_MAX_FILE_SIZE);
    return 1;
}


// struct stat file_stat;
// stat(file_path, &file_stat);
// printf("S_IFMT:%d, S_IFSOCK:%d, S_IFLNK:%d, S_IFREG:%d, S_IFBLK:%d, S_IFDIR:%d, S_IFCHR:%d, S_IFIFO:%d\n",
// file_stat.st_mode & S_IFMT ? 1 : 0,
// file_stat.st_mode & S_IFSOCK ? 1 : 0,
// file_stat.st_mode & S_IFLNK ? 1 : 0,
// file_stat.st_mode & S_IFREG ? 1 : 0,
// file_stat.st_mode & S_IFBLK ? 1 : 0,
// file_stat.st_mode & S_IFDIR ? 1 : 0,
// file_stat.st_mode & S_IFCHR ? 1 : 0,
// file_stat.st_mode & S_IFIFO ? 1 : 0);

void mx_send_file_from_client(t_client_info *info, char *file_name) {
    FILE *file;
    json_object *send_obj = NULL;

    if (file_name != NULL) {
        printf("file name->%s\n", file_name);
        if (open_file_to_send(&file, &send_obj, file_name) != 0) {
            printf("exit from function_98\n");
            return;
        }
    }
    else {
        if (pick_file_to_send(info, &file, &send_obj) != 0) {
            printf("pick\n");
            return;
        }
    }
    int readed = 1;
    const char *json_string;
    char buffer[1024];
    json_object *data;
    mx_js_o_o_add(send_obj, "piece", mx_js_n_int(1));
    mx_js_o_o_add(send_obj, "user_id", mx_js_n_int(info->id));
    mx_js_o_o_add(send_obj, "room_id", mx_js_n_int(info->data->current_room)); // need to know
    json_string = mx_js_o_to_js_str(send_obj);
    tls_send(info->tls_client, json_string, strlen(json_string));
    json_object_object_del(send_obj, "file_name");
    json_object_object_del(send_obj, "file_size");
    mx_js_s_int(mx_js_o_o_get(send_obj, "piece"), 2);
    data = mx_js_n_str("");
    mx_js_o_o_add(send_obj, "data", data);
    while(readed > 0 && !feof(file)) {
//        printf("while %d\n", readed);
        readed = fread(buffer, 1, sizeof(buffer), file);
        json_object_set_string_len(data, buffer, readed);
        if (feof(file)) {
            mx_js_s_int(mx_js_o_o_get(send_obj, "piece"), 3);
            mx_js_o_o_add(send_obj, "login", mx_js_n_str(info->login));
        }
        json_string = mx_js_o_to_js_str(send_obj);
        tls_send(info->tls_client, json_string, strlen(json_string));
        // mx_print_json_object(send_obj, "send aiff");
    }
    mx_send_empty_json(info->tls_client);
    json_object_put(send_obj);
    // unlock file
    fclose(file);
//    }
    printf("exit from function\n");
}
