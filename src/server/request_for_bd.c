#include "uchat.h"
// удалить ее перед сдачей
static int print_users(void *status, int argc, char **argv, char **in) {
    (void)argc;
    if (*(int *)status == 0) {
        for (int i = 0; i < 5; i++) {
            printf("%s", in[i]);
            if (i < 2 || i == 3)
                printf("\t");
            if (i == 2)
                printf("\t\t");
        }
        printf("\n");
        (*(int *)status)++;
    }
    for (int i = 0; argv[i]; i++) {
        printf("%s", argv[i]);
        if (strlen(argv[i]) <= 8 && i >=2)
            printf("\t\t");
        else 
            printf("\t");
    }
    printf("\n");
    return 0;
}

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
    char *command1 = malloc(1024);
    int count1 = 0;

    sprintf(command, "UPDATE users SET socket=0 WHERE socket=%d", client_sock);
    if (sqlite3_exec(i->db, command, 0, 0, NULL) != SQLITE_OK) {
        return -1;
    }
    mx_strdel(&command);
    //удалить перед сдачей
    sprintf(command1, "SELECT * FROM users");
    if (sqlite3_exec(i->db, command1, print_users, &count1, NULL) != SQLITE_OK) {
        return -1;
    }
    mx_strdel(&command1);
    // -----
    return 1;
}

int mx_delete_acc(t_server_info *i, json_object *j) {
    int user_id = json_object_get_int(json_object_object_get(j, "user_id"));
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
