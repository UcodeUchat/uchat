#include "uchat.h"

static void edit_data (char *command, json_object *js, int user_id) {
    const char *column = mx_js_g_str(mx_js_o_o_get(js, "column"));
    const char *data = mx_js_g_str(mx_js_o_o_get(js, "data"));

    sprintf(command, "UPDATE users SET %s='%s' where id='%d';", column, data, user_id);
}

static void edit_notifications (char *command, json_object *js, int user_id) {
    int visual = mx_js_g_int(mx_js_o_o_get(js, "visual_n"));
        int audio = mx_js_g_int(mx_js_o_o_get(js, "audio_n"));
        int email = mx_js_g_int(mx_js_o_o_get(js, "email_n"));

        sprintf(command, "UPDATE user_notifications SET visual='%d', \
            audio='%d', email='%d' where user_id='%d';",
            visual, audio, email, user_id);
}

int mx_edit_profile (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
    int add_info = mx_js_g_int(mx_js_o_o_get(js, "add_info"));
    char command[1024];
    const char *json_str = NULL;

    if (!add_info) {
        edit_data(command, js, user_id);
    }
    else {
        edit_notifications(command, js, user_id);
    }
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) 
        mx_js_o_o_add(js, "confirmation", mx_js_n_int(1));
    else 
        mx_js_o_o_add(js, "confirmation", mx_js_n_int(0));
    json_str = mx_js_o_to_js_str(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_str, strlen(json_str));
    return 1;
}
