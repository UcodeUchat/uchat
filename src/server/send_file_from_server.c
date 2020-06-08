#include "uchat.h"

int is_file_exist(char *file_name, t_file_tmp *vars) {
    struct stat buffer;
    char *full_file_name = mx_strjoin(MX_SAVE_FOLDER_IN_SERVER, file_name);
    int exist = stat(full_file_name, &buffer);

    if (exist == MX_OK) {
        vars->file_name = full_file_name;
        vars->size = buffer.st_size;
        return MX_OK;
    }
    else {
        mx_file_is_not_exist(vars);
        free(vars);
        mx_strdel(&full_file_name);
        return 1;
    }
}

void start_sending(FILE *file, t_file_tmp *vars) {
    json_object *send_obj = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);
    json_object *data = json_object_new_string("");
    const char *json_string;
    int readed = 1;
    char buffer[2048];
    char *extention = strdup(vars->file_name);

    while (mx_get_char_index(extention, '.') >= 0) {
        char *tmp = strdup(extention + mx_get_char_index(extention, '.') + 1);
        free(extention);
        extention = strdup(tmp);
        free(tmp); 
    }
    if (strcmp(extention, "jpg") == 0 || strcmp(extention, "png") == 0 || strcmp(extention, "gif") == 0)
        json_object_object_add(send_obj, "add_info", json_object_new_int(2));
    else
        json_object_object_add(send_obj, "add_info", json_object_new_int(1));
    json_object_object_add(send_obj, "piece", json_object_new_int(1));
    json_object_object_add(send_obj, "file_size", json_object_new_int(vars->size));
    json_object_object_add(send_obj, "file_id", json_object_new_int(vars->file_id));
    json_object_object_add(send_obj, "file_name", json_object_new_string(vars->file_name + strlen(MX_SAVE_FOLDER_IN_SERVER)));
    json_object_object_add(send_obj, "room_id", json_object_new_int(vars->room_id));
    json_string = json_object_to_json_string(send_obj);
    mx_save_send(vars->mutex, vars->tls, json_string, strlen(json_string));
    json_object_object_del(send_obj, "file_size");
    json_object_set_int(json_object_object_get(send_obj, "piece"), 2);
    json_object_object_add(send_obj, "data", data);
    while(readed > 0 && !feof(file)) {
        readed = fread(buffer, 1, sizeof(buffer), file);
        json_object_set_string_len(data, buffer, readed);
        feof(file) ? json_object_set_int(json_object_object_get(send_obj, "piece"), 3) : 0;
        json_string = json_object_to_json_string(send_obj);
        mx_save_send(vars->mutex, vars->tls, json_string, strlen(json_string));
    }
    json_object_put(send_obj);
}

int mx_send_file_from_server(t_server_info *info, t_socket_list *csl) {
    if (mx_check_is_object_valid(csl->obj) == MX_OK) {
        char *file = mx_check_file_in_db_user_access(info, csl->obj);

        if (file != NULL) {
            t_file_tmp *vars = mx_set_variables(csl);
            pthread_t thread_input;
            pthread_attr_t attr;

            mx_print_json_object(csl->obj, "data");
            if (is_file_exist(file, vars) == MX_OK) {
                pthread_attr_init(&attr);
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                if (pthread_create(&thread_input, &attr, mx_sd_fl, vars) == 0)
                    return 0;
                mx_strdel(&vars->file_name);
                free(vars);
            }
        }
    }
    return 1;
}
