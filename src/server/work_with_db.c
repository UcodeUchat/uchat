#include "uchat.h"

static int check_data(void *js, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0] && strcmp(argv[0], json_object_get_string(\
                            json_object_object_get(js, "password"))) == 0) {
        json_object_object_add((struct json_object*) js, "user_id",\
                                json_object_new_int(atoi(argv[1])));
        json_object_object_add((struct json_object*) js, "visual",\
                                json_object_new_int(atoi(argv[2])));
        json_object_object_add((struct json_object*) js, "audio",\
                                json_object_new_int(atoi(argv[3])));
        return 0;
    }
    return 1;
}

static int check_socket(void *rep, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (atoi(argv[0]) == 0) {
        (*(int *)rep)++;
        return 0;
    }
    return 1;
}

int mx_sign_in(t_server_info *i, json_object *js, int sock) {
    char *command = malloc(1024);

    sprintf(command, "SELECT password, users.id, visual, audio FROM users,\
            user_notifications WHERE users.login='%s' \
            and user_notifications.user_id=users.id", \
        json_object_get_string(json_object_object_get(js, "login")));
    if (sqlite3_exec(i->db, command, check_data, js, 0) != SQLITE_OK)
        return -1;
    if ((mx_update_socket(i, sock, json_object_get_string(\
         json_object_object_get(js, "login")))) == -1)
        printf("Socket wasn`t update\n");
    mx_strdel(&command);
    return 1;
}

int mx_find_sock_in_db(t_server_info *i, const char *login) {
    char *command = malloc(1024);
    int rep = 0;

    sprintf(command, "SELECT socket FROM users WHERE login='%s'", login);
    if (sqlite3_exec(i->db, command, check_socket, &rep, NULL) != SQLITE_OK)
        return -1;
    if (rep == 0) // if socket in table != 0
        return -1;
    mx_strdel(&command);
    return 1;
}

int mx_check_client(t_server_info *info, json_object *js, int sock) {
    if ((mx_find_sock_in_db(info, json_object_get_string(\
                            json_object_object_get(js, "login")))) == 1) {
            if ((mx_sign_in(info, js, sock)) == -1) {
                return -1;
        }
    }
    else
        return -1;
    return 1;
}
