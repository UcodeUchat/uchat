#include "uchat.h"

static int check_data(void *pass, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    // for(int i = 0; argv[i]; i++)
    //     printf("argv[%i] = %s\n", i, argv[i]);
    // printf("pass = %s\n", pass);
    if (argv[0] && strcmp(argv[0], (char *)pass) == 0)
        return 0;
    return 1;
}

static int check_socket(void* rep, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;

    // for(int i = 0; argv[i]; i++)
        // printf("argv[%i] = %s\n", i, argv[i]);
    if (atoi(argv[0]) == 0) {
        (*(int *)rep)++;
        return 0;
    }
    // user cosket != 0
    return 1;
}

int mx_sign_in(t_server_info *i, int c_sock, char *login, char *pass) {
    char *command = malloc(1024);
    int check = 0;

    sprintf(command, "SELECT password FROM users WHERE login='%s'", login);
    printf("%s\n", command);
    if ((sqlite3_exec(i->db, command, check_data, pass, 0) != SQLITE_OK) && ++(check)) {
        // write(c_sock, "Check your login or password\n", 29);
        printf("Check your login or password\n");
        return -1;
    }
    fprintf(stderr, "2check = [%d]\n", check);
    if (check == 0)
        return -1;
    // set socket-> c_sock  in db
    if ((mx_update_socket(i, c_sock, login)) == -1)
        printf("Socket wasn`t update\n");
    // write(c_sock, "login\0", 6);
    // printf("Complete. Connecting server...\n");
    mx_strdel(&command);
    return 1;
}

int mx_find_sock_in_db(t_server_info *i, char *login) {
    // printf("start mx_find_sock_in_db\n");
    char *command = malloc(1024);
    int rep = 0;
    int check = 0;

    sprintf(command, "SELECT socket FROM users WHERE login='%s'", login);
    printf("%s\n", command);
    if (sqlite3_exec(i->db, command, check_socket, &rep, NULL) != SQLITE_OK && ++(check)) {
        // write(c_sock, "user exist in UCHAT!\0", 21);
        printf("user exist in UCHAT!\n");
        return 1;
    }
    fprintf(stderr, "1check = [%d]\n", check);
    if (check == 0)
        return -1;
    if (rep == 0) // if socket in table != 0
        return 1;
    // printf("Complete. Next step is authorization!\n");
    mx_strdel(&command);
    return -1;
}

int mx_check_client(t_server_info *info, t_package *p) {
    // printf ("start mx_sheck_client\nClient sock = %d\n", p->client_sock);

    if ((mx_find_sock_in_db(info, p->login)) == 1) {
        printf("login in base\n");
        return 1;
    }
    else if ((mx_sign_in(info, p->client_sock, p->login, p->password)) == -1) {
            printf("not login\n");
            return -1;
        }
    else
        return 1;
}


