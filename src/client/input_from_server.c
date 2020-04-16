#include "uchat.h"

void *mx_process_input_from_server(void *taken_info) {
    t_client_info *info = (t_client_info *)taken_info;
    t_package *input_package = malloc(MX_PACKAGE_SIZE);

    (void)info;
    (void)input_package;
    mx_print_tid("new thread");
    while(1) { // read all input from server
        int rc = recv(info->socket, input_package, MX_PACKAGE_SIZE, MSG_WAITALL);
        if (rc == -1)
            mx_err_exit("error recv\n");
        if (rc != 0) {
            mx_print_curr_time();
            printf("input: piece.%d, type.%d, login.%s\n", input_package->piece, input_package->type,
                   input_package->login);
            printf("%s\n", input_package->data);
        }
    }
    return NULL;
}
