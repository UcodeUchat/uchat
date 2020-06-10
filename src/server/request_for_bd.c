#include "uchat.h"

int mx_update_socket(t_server_info *i, int client_sock, const char *login) {
    char command[1024];

    sprintf(command, "UPDATE users SET socket='%d' WHERE login='%s'",
    		client_sock, login);
    if (sqlite3_exec(i->db, command, 0, 0, NULL) != SQLITE_OK)
        return -1;
    return 1;
}

int mx_drop_socket(t_server_info *i, int client_sock) {
    char *command = malloc(1024);

    sprintf(command, "UPDATE users SET socket=0 WHERE socket=%d", client_sock);
    if (sqlite3_exec(i->db, command, 0, 0, NULL) != SQLITE_OK) {
        return -1;
    }
    mx_strdel(&command);
    return 1;
}

int mx_delete_acc(t_server_info *i, json_object *j) {
    int user_id = mx_js_g_int(mx_js_o_o_get(j, "user_id"));
    char *command = malloc(1024);

    sprintf(command, "DELETE FROM users WHERE users.id=%d", user_id);
    if (sqlite3_exec(i->db, command, 0, 0, NULL) != SQLITE_OK) {
        mx_strdel(&command);
        return -1;
    }
    sprintf(command, "DELETE FROM room_user WHERE room_user.user_id=%d",
            user_id);
    if (sqlite3_exec(i->db, command, 0, 0, NULL) != SQLITE_OK) {
        mx_strdel(&command);
        return -1;
    }
    mx_strdel(&command);
    return 1;
}
