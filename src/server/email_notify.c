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
        mx_js_g_str(mx_js_o_o_get(js, "login")))] = '\0';
    if (sqlite3_exec(i->db, ask, search_name, &login, 0) != SQLITE_OK)
        return NULL;
    return login;
}

void mx_email_notify(t_server_info *i, json_object *js) {
    int id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
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
