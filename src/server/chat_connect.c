#include "inc/uchat.h"

void mx_chat_connect(t_info *info, t_clients *client_info) {
    char *input = NULL;

    while(1) {
        write(client_info->fd, "1 - sign_in\t2 - sign_up\t3 - exit\n", 33);
        input = mx_read_client_input(client_info->fd);
        if (input == NULL || strcmp(input, "") == 0
            || strcmp(input, "exit") == 0 || input[0] == 51)
            write(client_info->fd, "exit\n", 5);
        else if (strcmp(input, "sign_in") == 0 || input[0] == 49)
            mx_sign_in(info, client_info);
        else if (strcmp(input, "sign_up") == 0 || input[0] == 50)
            mx_sign_up(info, client_info);
        mx_strdel(&input);
    }
    mx_strdel(&input);
}
