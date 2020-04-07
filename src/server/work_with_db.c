#include "uchat.h"

static int check_data(void *pass, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    for(int i = 0; argv[i]; i++)
        printf("argv[%i] = %s\n", i, argv[i]);
    if (argv[0] && strcmp(argv[0], (char *)pass) == 0)
        return 0;
    return 1;
}

static int check_socket(void* c_sock, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;

    for(int i = 0; argv[i]; i++)
        printf("argv[%i] = %s\n", i, argv[i]);
    if (argv[0] == c_sock)
        return 0;
    return 1;
}

int mx_sign_in(int c_sock, char *login, char *pass) {
    char *command = malloc(1024);
    sqlite3 *db = NULL;
    int status = sqlite3_open(MX_PATH_TO_DB, &db);

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    sprintf(command, "SELECT password FROM users WHERE login='%s'", login);
    printf("%s\n", command);
    if (sqlite3_exec(db, command, check_data, pass, NULL) != SQLITE_OK) {
        write(c_sock, "Check your login or password\n", 29);
        printf("Check your login or password\n");
        return -1;
    }

    // set socket-> c_sock  in db

    write(c_sock, "login", 6);
    printf("Complete. Connecting server...\n");
    mx_strdel(&command);
    return 1;
}

int mx_find_sock_in_db(int c_sock, char *login) {
    printf("\vstart mx_find_sock_in_db\n");
    char *command = malloc(1024);
    sqlite3 *db = NULL;
    int status = sqlite3_open(MX_PATH_TO_DB, &db);

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    sprintf(command, "SELECT sock FROM users WHERE login='%s'", login);
    printf("%s\n", command);
    if (sqlite3_exec(db, command, check_socket, &c_sock, NULL) != SQLITE_OK) {
//        write(c_sock, "input your login or passwor\n", 29);
        printf("input your login or password\n");
        return -1;
    }
    write(c_sock, "login", 6);
    printf("Complete. Connecting server...\n");
    mx_strdel(&command);
    return 1;
}




int mx_check_client(int client_sock) {
    printf ("start mx_sheck_client\tClient sock = %d\n", client_sock);
    char data[256];
    char **log_pas = NULL;
    int size = 0;

    size = read(client_sock, data, sizeof(data));
    data[size] = '\0';
//    size = recv(client_sock, data, strlen(data), 0);
    printf(" recive %d from client1: %s\n", size, data);
    log_pas = mx_strsplit(data, ' ');
    printf("recieve data: %s\n", data);
    printf(" recive from client %s\n", log_pas[0]);
    printf(" recive from client %s\n", log_pas[1]);

    if ((mx_find_sock_in_db(client_sock,log_pas[0])) == 1) {
        write(client_sock, "login", 6);
        printf("login in base\n");
        return 1;
    }

    if ((mx_sign_in(client_sock, log_pas[0], log_pas[1])) == -1) {
        write(client_sock, "not login", 10);
        printf("not login\n");
        return -1;
    }
    return 1;
}

