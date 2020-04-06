#include "uchat.h"

static void init_db(sqlite3 *db) {
    int status = sqlite3_open(MX_PATH_TO_DB, &db);

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
}

static int check_data(void *password, int argc, char **argv,
                       char **column_name) {
    (void)argc;
    (void)column_name;
    printf("argv[0] = %s\n", argv[0]);
    if (argv[0] && strcmp(argv[0], (char *)password) == 0)
        return 0;
    return 1;
}

int mx_sign_in(int c_sock, char *login, char *password) {
    char *command = malloc(1024);
    sqlite3 *db = NULL;

    init_db(db);
    sprintf(command, "SELECT password FROM users WHERE login='%s'", login);
    if (sqlite3_exec(db, command, check_data, password, NULL) != SQLITE_OK) {
        write(c_sock, "Check your login or password\n", 29);
        return -1;
    }
    write(c_sock, "Complete. Connecting server...\n", 31);
    mx_strdel(&command);
    return 1;
}

int mx_check_client(int client_sock) {
    printf ("start mx_sheck_client\n");
    char *data = mx_strnew(256);
//    char **log_pas = NULL;
    int size = 0;

    size = read(client_sock, data, sizeof(data));
//    size = recv(client_sock, data, strlen(data), 0);
    printf(" recive %d from client1: %s\n", size, data);
//    log_pas = mx_strsplit(data, ' ');
    printf("recieve data: %s\n", data);
//    printf(" recive from client %s\n", log_pas[0]);
//    printf(" recive from client %s\n", log_pas[1]);

    if ((mx_sign_in(client_sock, "snikolayen", "123123")) == -1) {
        write(client_sock, "not login\n", 10);
        return -1;
    }
    write(client_sock, "login\n", 6);
    return 1;
}

