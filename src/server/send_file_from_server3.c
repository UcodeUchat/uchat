#include "uchat.h"

void *send_file(void *arg) {
    t_file_tmp *vars = (t_file_tmp *)arg;
    FILE *file;
        
    if ((file = fopen(vars->file_name, "r")) != NULL) {
        start_sending(file, vars);
        fclose(file);
    }
    mx_strdel(&vars->file_name);
    free(vars);
    return NULL;
}

void file_is_not_exist(t_file_tmp *vars) {
    const char *json_string;
    json_object *obj_for_sending = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);

    json_object_object_add(obj_for_sending, "file_id",
        json_object_new_int(vars->file_id));
    json_object_object_add(obj_for_sending, "piece", json_object_new_int(-1));
    json_string = json_object_to_json_string(obj_for_sending);
    mx_save_send(vars->mutex, vars->tls, json_string, strlen(json_string));
    json_object_put(obj_for_sending);
}

