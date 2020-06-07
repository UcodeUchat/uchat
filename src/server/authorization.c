#include "uchat.h"

static int search_email(void *data, int argc, char **argv, char **col_name) {
    char **email = (char **)data;

    (void)col_name;
    if (argc > 0) {
        if (atoi(argv[0]) == 1 && argv[1] != NULL) {
            *email = strdup(argv[1]);
            return 0;
        } 
    }
    return 1;
}

static int search_name(void *p, int argc, char **argv, char **col_name) {
    char **login = (char **)p;

    (void)col_name;
    (void)argc;
    *login = argv[2] ? strdup(argv[2]) : NULL;
    return 0;
}

static char *get_user_name(t_server_info *i, json_object *js) {
    char ask[1024];
    char *login = NULL;

    ask[sprintf(ask, "SELECT * FROM users WHERE login='%s'",
        json_object_get_string(json_object_object_get(js, "login")))] = '\0';
    if (sqlite3_exec(i->db, ask, search_name, &login, 0) != SQLITE_OK)
        return NULL;
    return login;
}

static int get_user_id(void *p, int argc, char **argv, char **col_name) {
    int *id = (int *)p;

    (void)col_name;
    if (argc > 1 || !argv[0])
        return 1;
    *id = atoi(argv[0]);
    return 0;
}

void mx_email_notify(t_server_info *i, json_object *js) {
    int id = json_object_get_int(json_object_object_get(js, "user_id"));
    char *login = get_user_name(i, js);
    char *command = malloc(1024);
    char *email = NULL;

    sprintf(command, "SELECT user_notifications.email, \
            users.email FROM user_notifications, users \
            WHERE  user_notifications.user_id='%d' and users.id='%d'", id, id);
    if (sqlite3_exec(i->db, command, search_email, &email, 0) == SQLITE_OK) {
        printf("%s\n", email);
        mx_send_mail(login, email, "Someone logged in your account in Uchat");
        mx_strdel(&email);
    }
    else
        fprintf(stderr, "email is not exist\n");
    mx_strdel(&login);
    mx_strdel(&command);
}

int mx_add_to_db(t_server_info *i, const char *l, const char *pa, int us_id) {
    char command[1024];

    sprintf(command, "insert into users (socket, login, password, access)\
                values (0,'%s', '%s', 1);\nSELECT last_insert_rowid()", l, pa);
    if (sqlite3_exec(i->db, command, get_user_id, &us_id, 0) != SQLITE_OK)
        return -1;
    sprintf(command, "insert into room_user (room_id, user_id)\
                values (0,%d);", us_id);
    if (sqlite3_exec(i->db, command, NULL, NULL, 0) != SQLITE_OK)
        return -1;
    sprintf(command, "insert into user_notifications (user_id, visual, audio,\
            email) values (%d, 0, 0, 0);", us_id);
    if (sqlite3_exec(i->db, command, NULL, NULL, 0) != SQLITE_OK)
        return -1;
    return 1;
}

int mx_registration(t_server_info *i, t_socket_list *csl, json_object *js) {
    const char *json_string = NULL;

    if (mx_search_in_db(i, json_object_get_string(json_object_object_get(js, \
        "login")), json_object_get_string(json_object_object_get(js,\
        "password"))) == -1) {
        json_object_set_int(json_object_object_get(js, "add_info"),\
                                                    MX_AUTH_TYPE_NV);
    }
    else{
        mx_add_to_db(i, json_object_get_string(json_object_object_get(js,\
        "login")), json_object_get_string(json_object_object_get(js,\
        "password")), -1);
        json_object_set_int(json_object_object_get(js, "add_info"),\
                                                    MX_AUTH_TYPE_V);
    }
    json_string = json_object_to_json_string(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string,\
                 strlen(json_string));
    return 1;
}
