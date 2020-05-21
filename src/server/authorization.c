#include "uchat.h"


static int search_email(void *data, int argc, char **argv, char **col_name) {
    (void)col_name;
    (void)argc;
    char **email = (char **)data;
    if (argv[0]) {
        if (mx_atoi(argv[0]) == 1 && argv[1] != NULL) {
            *email = strdup(argv[1]);
            return 0;
        } 
    }
    return 1;
}

void email_notify(t_server_info *i, json_object *js) {
    int id = json_object_get_int(json_object_object_get(js, "user_id"));
    char *command = malloc(1024);
    char *email = NULL;
    sprintf(command, "SELECT user_notifications.email, users.email FROM user_notifications, users \
            WHERE  user_notifications.user_id='%d' and users.id='%d'", id, id);
    if (sqlite3_exec(i->db, command, search_email, &email, 0) == SQLITE_OK) {
        printf("%s\n", email);
        mx_send_mail(email, "Someone logged in your account in Uchat");
        mx_strdel(&email);
    }
    else {
        printf("fail\n");
    }
    mx_strdel(&command); 
}

int mx_authorization(t_server_info *i, t_socket_list *csl, json_object *js) {
    const char *json_string = NULL;
    int valid = mx_check_client(i, js, csl->socket);
	
    if (valid == 1) { // вошел
        json_object_set_int(json_object_object_get(js, "type"), MX_AUTH_TYPE_V);
        json_object_object_add(js, "room_id", json_object_new_int(0));
        //получаем лист комнат для одного юзера
        mx_get_rooms(i, js);
        mx_print_json_object(js, "auth");
        json_string = json_object_to_json_string(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
        email_notify(i, js);        
	}
	else { //не вошел
        json_object_set_int(json_object_object_get(js, "type"), MX_AUTH_TYPE_NV);
        json_string = json_object_to_json_string(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
	}
	return 1;
}

static int search_data(void *p, int argc, char **argv, char **col_name) {
    (void)p;
    (void)col_name;
    (void)argv;
    if (argc > 0)
        return 1;
    return 0;
}

int mx_search_in_db(t_server_info *i, const char *l, const char *pa) {
    char *command = malloc(1024);
    (void)pa;
    sprintf(command, "SELECT login FROM users WHERE login='%s'", l);
    if (sqlite3_exec(i->db, command, search_data, NULL, 0) != SQLITE_OK)
        return -1;
    mx_strdel(&command);
    return 1;
}

static int get_user_id(void *p, int argc, char **argv, char **col_name) {
    int *id = (int *)p;

    (void)col_name;
    if (argc > 1 || !argv[0])
        return 1;
    *id = atoi(argv[0]);
    return 0;
}

int mx_add_to_db(t_server_info *i, const char *l, const char *pa) {
    char *command = malloc(1024);
    char *command1 = malloc(1024);
    char *command2 = malloc(1024);
    int user_id = -1;
    
    sprintf(command, "insert into users (socket, login, password, access)\
                values (0,'%s', '%s', 1);\nSELECT last_insert_rowid()", l, pa);
    if (sqlite3_exec(i->db, command, get_user_id, &user_id, 0) != SQLITE_OK)
        return -1;
    mx_strdel(&command);
    // add user in general room
    sprintf(command1, "insert into room_user (room_id, user_id)\
                values (0,%d);", user_id);
    if (sqlite3_exec(i->db, command1, NULL, NULL, 0) != SQLITE_OK)
        return -1;
    mx_strdel(&command1);
    // add notifications
    sprintf(command2, "insert into user_notifications (user_id, visual, audio, email)\
                values (%d, 0, 0, 0);", user_id);
    if (sqlite3_exec(i->db, command2, NULL, NULL, 0) != SQLITE_OK)
        return -1;
    mx_strdel(&command2);
    return 1;
}

int mx_registration(t_server_info *i, t_socket_list *csl, json_object *js) {
    const char *json_string = NULL;

    if (mx_search_in_db(i, json_object_get_string(json_object_object_get(js, "login")),\
        json_object_get_string(json_object_object_get(js, "password"))) == -1) {
    	json_object_set_int(json_object_object_get(js, "add_info"), MX_AUTH_TYPE_NV);
    }
    else{
    	mx_add_to_db(i, json_object_get_string(json_object_object_get(js, "login")),\
        json_object_get_string(json_object_object_get(js, "password")));
    	json_object_set_int(json_object_object_get(js, "add_info"), MX_AUTH_TYPE_V);
    }
    json_string = json_object_to_json_string(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
    return 1;
}
