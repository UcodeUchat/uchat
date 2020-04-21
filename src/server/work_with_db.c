#include "uchat.h"

static int check_data(void *p, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    t_package *p1 = (t_package *)p;
    for(int i = 0; argv[i]; i++)
        printf("argv[%i] = %s\n", i, argv[i]);
    printf("pass = %s\n", p1->password);
    if (argv[0] && strcmp(argv[0], p1->password) == 0){
        p1->type = 100;
        return 0;
    }
    return 1;
}

static int check_socket(void *rep, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;

    for(int i = 0; argv[i]; i++)
        printf("argv[%i] = %s\n", i, argv[i]);
    if (atoi(argv[0]) == 0) {
        (*(int *)rep)++;
        return 0;
    }
    // user cosket != 0
    return 1;
}

int mx_sign_in(t_server_info *i, t_package *p) {
    char *command = malloc(1024);

    sprintf(command, "SELECT password FROM users WHERE login='%s'", p->login);
    printf("%s\n", command);
    if (sqlite3_exec(i->db, command, check_data, p, 0) != SQLITE_OK) {
        printf("Check your login or password\n");
        return -1;
    }
    fprintf(stderr, "type = [%d]\n", p->type);

    if ((mx_update_socket(i, p->client_sock, p->login)) == -1)
        printf("Socket wasn`t update\n");
    mx_strdel(&command);
    return 1;
}

int mx_find_sock_in_db(t_server_info *i, char *login) {
    // printf("start mx_find_sock_in_db\n");
    char *command = malloc(1024);
    int rep = 0;

    sprintf(command, "SELECT socket FROM users WHERE login='%s'", login);
    printf("%s\n", command);
    if (sqlite3_exec(i->db, command, check_socket, &rep, NULL) != SQLITE_OK){
        printf("user exist in UCHAT!\n");
        return -1;
    }
    fprintf(stderr, "rep = [%d]\n", rep);

    if (rep == 0) // if socket in table != 0
        return -1;
    printf("Complete. Next step is authorization!\n");
    mx_strdel(&command);
    return 1;
}

int mx_check_client(t_server_info *info, t_package *p) {
    // printf ("start mx_sheck_client\nClient sock = %d\n", p->client_sock);

    if ((mx_find_sock_in_db(info, p->login)) == 1) {
        printf("login is not in base\n");
            if ((mx_sign_in(info, p)) == -1) {
                printf("not login\n");
                return -1;
        }
    }
    else
        return -1;
    return 1;
}

