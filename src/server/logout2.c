#include "uchat.h"

static int search_rooms(void *array, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        json_object *room = mx_js_n_o();
        json_object *id = mx_js_n_int(atoi(argv[0]));
        json_object *name = mx_js_n_str(argv[1]);
        json_object *acces = mx_js_n_int(atoi(argv[2]));

        mx_js_o_o_add(room, "id", id);
        mx_js_o_o_add(room, "name", name);
        mx_js_o_o_add(room, "acces", acces);
        json_object_array_add((struct json_object *)array, room);
    }
    return 0;
}

static int search_users(void *array, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        json_object *user = mx_js_n_o();
        json_object *id = mx_js_n_int(atoi(argv[0]));
        json_object *login = mx_js_n_str(argv[2]);

        mx_js_o_o_add(user, "id", id);
        mx_js_o_o_add(user, "login", login);
        json_object_array_add((struct json_object *)array, user);
    }
    return 0;
}

static int sql_exec(t_server_info *info, const char **query,
                    json_object **array_rooms, json_object **array_users) {
    char cmd[1024];

    if (strcmp(*query, "All") == 0)
        sprintf(cmd, "SELECT * FROM rooms;");
    else
        sprintf(cmd, "SELECT * FROM rooms WHERE name LIKE '%%%s%%' \
                AND NOT access=3;", *query);
    if (sqlite3_exec(info->db, cmd, search_rooms, *array_rooms, NULL) != 0)
        return 0;
    if (strcmp(*query, "All") == 0)
        sprintf(cmd, "SELECT * FROM users;");
    else
        sprintf(cmd, "SELECT * FROM users WHERE login LIKE '%%%s%%';", *query);
    if (sqlite3_exec(info->db, cmd, search_users, *array_users, NULL) != 0)
        return 0;
    return 1;
}

int mx_search_all (t_server_info *info, t_socket_list *csl, json_object *js) {
    const char *query = mx_js_g_str(mx_js_o_o_get(js, "query"));
    const char *json_str = NULL;
    json_object *array_rooms = json_object_new_array();
    json_object *array_users = json_object_new_array();

    mx_js_o_o_add(js, "rooms", array_rooms);
    mx_js_o_o_add(js, "users", array_users);
    if (sql_exec(info, &query, &array_rooms, &array_users) == 0)
        return 0;
    json_str = mx_js_o_to_js_str(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_str, strlen(json_str));
    return 1;
}
