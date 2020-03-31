#include "inc/uchat.h"

static int check_data(void *times_exec, int argc, char **argv, char **column_name) {
    (void)argc;
    (void)argv;
    (void)column_name;
    *(int *)times_exec = 1;
    return 1;
}

static bool create_new_account(t_info *info, t_clients *client_info) {
    char *ask_db = NULL;

    if (strlen(client_info->password) <= 0 && strlen(client_info->password) >= 25) {
        write(client_info->fd, "Incorrect password. Must be: len(1-25)\n", 39);
        return 1;
    }
    ask_db = malloc(strlen(client_info->login) + strlen(client_info->password) + 50);
    sprintf(ask_db, "INSERT INTO accounts(login, password) VALUES('%s', '%s')",
            client_info->login, client_info->password);
    if (sqlite3_exec(info->db, ask_db, NULL, NULL, &(client_info->err_msg)) != SQLITE_OK) {
        write(client_info->fd, "Can't create a new account. Try again later\n", 44);
        mx_strdel(&ask_db);
        write(client_info->fd, "Error message: ", 15);
        write(client_info->fd, client_info->err_msg, mx_strlen(client_info->err_msg));
        write(client_info->fd, "\n", 1);
        return 1;
    }
    mx_strdel(&ask_db);
    return 0;
}

void mx_sign_up(t_info *info, t_clients *client_info) {
    char *command = NULL;
    int times_exec = 0;

    mx_clear_form(client_info);
    write(client_info->fd, "Enter your login: ", 18);
    client_info->login = mx_read_client_input(client_info->fd);
    write(client_info->fd, "Enter your password: ", 21);
    client_info->password = mx_read_client_input(client_info->fd);
    command = malloc(1024);
    sprintf(command, "SELECT id FROM accounts WHERE login='%s'", client_info->login);
    if (sqlite3_exec(info->db, command, check_data, &times_exec, NULL) != SQLITE_OK
        || times_exec == 1) {
        write(client_info->fd, "This login is exist, try another one\n", 39);
    }
    else
        create_new_account(info, client_info);
    mx_strdel(&command);
}
