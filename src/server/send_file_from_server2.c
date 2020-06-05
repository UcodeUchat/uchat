#include "uchat.h"

t_file_tmp *set_variables(t_socket_list *csl) {
    t_file_tmp *new = malloc(sizeof(t_file_tmp));

    new->tls = csl->tls_socket;
    new->mutex = &csl->mutex;
    new->file_id = json_object_get_int(json_object_object_get(csl->obj,\
                                        "file_id"));
    new->room_id = json_object_get_int(json_object_object_get(csl->obj,\
                                        "room_id"));
    return new;
}

int check_is_object_valid(json_object *obj) {
    if (json_object_object_get(obj, "room_id")
        && json_object_object_get(obj, "user_id")
        && json_object_object_get(obj, "file_id"))
        return MX_OK;
    else
        return 1;
}

int res(void *arg, int argc, char **argv, char **col_name) {
    (void)col_name;
    (void)argc;
    (void)arg;
    if (argv[0])
        return 0;
    return 1;
}

int get_result(void *arg, int argc, char **argv, char **col_name) {
    json_object *obj = (json_object *)arg;

    (void)col_name;
    (void)argc;
    if (argv[0]) {
        json_object_object_add(obj, "name", json_object_new_string(argv[3]));
        return MX_OK;
    }
    return 1;
}

char *check_file_in_db_and_user_access(t_server_info *info, json_object *obj) {
    int file_id = json_object_get_int(json_object_object_get(obj, "file_id"));
    char command[1024];

    command[sprintf(command, "select * from msg_history where id='%d'",
                    file_id)] = '\0';
    if (sqlite3_exec(info->db, command, get_result, obj, NULL) != SQLITE_OK) {
		return NULL;
    }
    command[sprintf(command, "select * from room_user where user_id='%d' & room_id='%d'", json_object_get_int(json_object_object_get(obj, "user_id")), json_object_get_int(json_object_object_get(obj, "room_id")))] = '\0';
    if (sqlite3_exec(info->db, command, res, NULL, NULL) != SQLITE_OK) {
		return NULL;
    }
    return strdup(json_object_get_string(json_object_object_get(obj, "name")));
}
