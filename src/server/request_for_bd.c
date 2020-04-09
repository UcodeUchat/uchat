#include "uchat.h"

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

int mx_update_socket(int client_sock, char *login) {
    char *command = malloc(1024);
    sqlite3 *db = NULL;
    int status = sqlite3_open(MX_PATH_TO_DB, &db);

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sprintf(command, "UPDATE users SET socket='%d' WHERE login='%s'",
    		client_sock, login);
    printf("%s\n", command);
    if (sqlite3_exec(db, command, 0, 0, NULL) != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }
    sprintf(command, "SELECT * FROM users WHERE login='%s'", login);
    printf("%s\n", command);
    int count1 = 0;
    if (sqlite3_exec(db, command, print_users, &count1, NULL) != SQLITE_OK) {
        write(client_sock, "Check your login or password\n", 29);
        printf("Check your login or password\n");
        sqlite3_close(db);
        return -1;
    }
    return 1;
}

int mx_drop_socket(int client_sock) {
    printf("start set socket to 0\n");
    char *command = malloc(1024);
    sqlite3 *db = NULL;
    int status = sqlite3_open(MX_PATH_TO_DB, &db);

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sprintf(command, "UPDATE users SET socket=0 WHERE socket=%d", client_sock);
    printf("%s\n", command);
    if (sqlite3_exec(db, command, 0, 0, NULL) != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }
    sprintf(command, "SELECT * FROM users");
    printf("%s\n", command);
    int count1 = 0;
    if (sqlite3_exec(db, command, print_users, &count1, NULL) != SQLITE_OK) {
        // write(client_sock, "Check your login or password\n", 29);
        // printf("Check your login or password\n");
        sqlite3_close(db);
        return -1;
    }
    return 1;
}



