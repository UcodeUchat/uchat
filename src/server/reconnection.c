#include "uchat.h"

int check_user(void *id, int argc, char **argv, char **col_name) {
    (void)col_name;
    if (argc > 0) {
        *((int *)id) = atoi(argv[0]);
        return 0;
    }
    return 1;
}

int mx_reconnection(t_server_info *info, t_socket_list *csl) {
    int id = -1;
    const char *login = NULL;
    const char *password = NULL;
    char command[1024];

    login = json_object_get_string(json_object_object_get(csl->obj,"login"));
    password = json_object_get_string(json_object_object_get(csl->obj,
        "password"));
    command[sprintf(command, "SELECT id FROM users WHERE login='%s' \
                    AND password='%s'", login, password)] = 0;
    if (sqlite3_exec(info->db, command, check_user, &id, NULL)
        == SQLITE_OK && id != -1) {
        command[sprintf(command, "UPDATE users SET socket='%d' WHERE id='%d'",
                csl->socket, id)] = 0;
        sqlite3_exec(info->db, command, NULL, NULL, NULL);
        return 0;
    }
    return -1;
}
