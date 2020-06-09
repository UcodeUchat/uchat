#include "uchat.h"

static char *get_name(char *path) {
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

static bool open_file_to_send(FILE **file, json_object **obj, char *path) {
    struct stat f_stat;

    if (stat(path, &f_stat) != MX_OK)
        return 1;
    if (f_stat.st_size > 0 && f_stat.st_size <= MX_MAX_FILE_SIZE)
        if (!(f_stat.st_mode & S_IFDIR) && (f_stat.st_mode & S_IFREG))
            if ((*file = fopen(path, "r")) != NULL) {
                char *file_name = get_name(path);

                *obj = mx_create_basic_json_object(MX_FILE_SEND_TYPE);
                mx_js_o_o_add(*obj, "file_name", mx_js_n_str(file_name));
                mx_js_o_o_add(*obj, "file_size", mx_js_n_int(f_stat.st_size));
                mx_strdel(&file_name);
                return 0;
            }
    return 1;
}

static bool pick_file_method(t_client_info *info, char *file_name,
                             FILE **file, json_object **send_obj) {
    if (file_name != NULL) {
        printf("file name->%s\n", file_name);
        if (open_file_to_send(file, send_obj, file_name) != 0) {
            printf("exit from function_98\n");
            return 1;
        }
    }
    else {
        if (mx_pick_file_to_send(info, file, send_obj) != 0) {
            printf("pick\n");
            return 1;
        }
    }
    return 0;
}

static void send_file_data(t_client_info *info, FILE **file,
                           json_object **data, json_object **send_obj) {
    int readed = 1;
    char buffer[1024];
    const char *json_string;

    while(readed > 0 && !feof(*file)) {
        readed = fread(buffer, 1, sizeof(buffer), *file);
        json_object_set_string_len(*data, buffer, readed);
        if (feof(*file)) {
            mx_js_s_int(mx_js_o_o_get(*send_obj, "piece"), 3);
            mx_js_o_o_add(*send_obj, "login", mx_js_n_str(info->login));
        }
        json_string = mx_js_o_to_js_str(*send_obj);
        tls_send(info->tls_client, json_string, strlen(json_string));
    }
    fclose(*file);
}

void mx_send_file_from_client(t_client_info *info, char *file_name) {
    FILE *file;
    json_object *send_obj = NULL;
    const char *json_string;
    json_object *data;

    if (pick_file_method(info, file_name, &file, &send_obj) != MX_OK)
        return;
    mx_js_o_o_add(send_obj, "piece", mx_js_n_int(1));
    mx_js_o_o_add(send_obj, "user_id", mx_js_n_int(info->id));
    mx_js_o_o_add(send_obj, "room_id", mx_js_n_int(info->data->current_room));
    json_string = mx_js_o_to_js_str(send_obj);
    tls_send(info->tls_client, json_string, strlen(json_string));
    json_object_object_del(send_obj, "file_name");
    json_object_object_del(send_obj, "file_size");
    mx_js_s_int(mx_js_o_o_get(send_obj, "piece"), 2);
    data = mx_js_n_str("");
    mx_js_o_o_add(send_obj, "data", data);
    send_file_data(info, &file, &data, &send_obj);
    mx_send_empty_json(info->tls_client);
    json_object_put(send_obj);
}
