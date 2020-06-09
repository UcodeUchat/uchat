#include "uchat.h"

void *mx_sd_fl(void *arg) {
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

void mx_file_is_not_exist(t_file_tmp *vars) {
    const char *json_string;
    json_object *obj_json = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);

    mx_js_o_o_add(obj_json, "file_id",
        mx_js_n_int(vars->file_id));
    mx_js_o_o_add(obj_json, "piece", mx_js_n_int(-1));
    json_string = mx_js_o_to_js_str(obj_json);
    mx_save_send(vars->mutex, vars->tls, json_string, strlen(json_string));
    json_object_put(obj_json);
}
