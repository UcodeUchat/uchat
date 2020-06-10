#include "uchat.h"

int mx_delete_message (t_server_info *info,
                       t_socket_list *csl, json_object *js) {
    int msg_id = mx_js_g_int(mx_js_o_o_get(js, "message_id"));
    char command[1024];

    (void)csl;
    sprintf(command, "DELETE FROM msg_history where id = %d;", msg_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        mx_send_json_to_all_in_room(info, js);
    }
    return 1;
}
