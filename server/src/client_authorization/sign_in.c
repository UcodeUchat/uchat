#include "../../inc/uchat.h"

static int check_data(void *client_info, int argc, char **argv,
                       char **column_name) {
    if (argv[0] && strcmp(argv[0], ((t_clients *)client_info)->password) == 0)
        return 0;
    return 1;
}

void mx_sign_in(t_info *info, t_clients *client_info) {
    char *command = NULL;

    mx_clear_form(client_info);
    write(client_info->fd, "Enter your login: ", 18);
    client_info->login = mx_read_client_input(client_info->fd);
    write(client_info->fd, "Enter your password: ", 21);
    client_info->password = mx_read_client_input(client_info->fd);
    command = malloc(1024);
    sprintf(command, "SELECT password FROM accounts WHERE login='%s'",
            client_info->login);
    if (sqlite3_exec(info->db, command, check_data, client_info, NULL) != SQLITE_OK)
        write(client_info->fd, "Check your login or password\n", 29);
    else if (mx_user_online(info, client_info->login) == NULL) {
        write(client_info->fd, "Connection...\n", 14);
        mx_chat_menu(info, client_info);
    }
    else
        write(client_info->fd, "This user is already online\n", 28);
    mx_strdel(&command);
}
