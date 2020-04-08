#include "uchat.h"

static int check_data(void *pass, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    for(int i = 0; argv[i]; i++)
        printf("argv[%i] = %s\n", i, argv[i]);
    printf("pass = %s\n", pass);
    if (argv[0] && strcmp(argv[0], (char *)pass) == 0)
        return 0;
    return 1;
}

static int check_socket(void* rep, int argc, char **argv, char **col_name) {
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

int mx_sign_in(int c_sock, char *login, char *pass) {
    char *command = malloc(1024);
    sqlite3 *db = NULL;
    int status = sqlite3_open(MX_PATH_TO_DB, &db);

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sprintf(command, "SELECT password FROM users WHERE login='%s'", login);
    printf("%s\n", command);
    if (sqlite3_exec(db, command, check_data, pass, 0) != SQLITE_OK) {
        write(c_sock, "Check your login or password\n", 29);
        printf("Check your login or password\n");
        sqlite3_close(db);
        return -1;
    }

    // set socket-> c_sock  in db
    if ((mx_update_socket(c_sock, login)) == -1)
        printf("Socket wasn`t update\n");
    
    write(c_sock, "login\0", 6);
    printf("Complete. Connecting server...\n");
    mx_strdel(&command);
    sqlite3_close(db);
    return 1;
}

int mx_find_sock_in_db(int c_sock, char *login) {
    printf("start mx_find_sock_in_db\n");
    char *command = malloc(1024);
    sqlite3 *db = NULL;
    int status = sqlite3_open(MX_PATH_TO_DB, &db);
    int rep = 0;
    (void)c_sock;
    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    sprintf(command, "SELECT socket FROM users WHERE login='%s'", login);
    printf("%s\n", command);
    if (sqlite3_exec(db, command, check_socket, &rep, NULL) != SQLITE_OK) {
        // write(c_sock, "user exist in UCHAT!\0", 21);
        printf("user exist in UCHAT!\n");
        return 1;
    }
    if (rep == 0) // if socket in table != 0
        return 1;
    printf("Complete. Next step is authorization!\n");
    mx_strdel(&command);
    sqlite3_close(db);
    return -1;
}

int mx_check_client(int client_sock, char *c_input) {
    printf ("start mx_sheck_client\nClient sock = %d\n", client_sock);
    // char data[256];
    char **log_pas = NULL;
    // int size = 0;

    // size = recv(client_sock, data, sizeof(data), 0);
    // printf(" recive [%d] from client1: [%s]\n", size, data);
    log_pas = mx_strsplit(c_input, ' ');
    printf("recieve data: [%s]\n", c_input);

    printf(" recive from client %s\n", log_pas[0]);
    printf(" recive from client %s\n", log_pas[1]);

    if ((mx_find_sock_in_db(client_sock,log_pas[0])) == 1) {
        printf("login in base\n");
        return 1;
    }
    else if ((mx_sign_in(client_sock, log_pas[0], log_pas[1])) == -1) {
            printf("not login\n");
            return -1;
        }
    else
        return 1;
}


