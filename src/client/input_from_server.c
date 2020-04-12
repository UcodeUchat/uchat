#include "uchat.h"

void *mx_process_input_from_server(void *taken_info) {
    t_client_info *info = (t_client_info *)taken_info;
    t_package *input_package = malloc(MX_PACKAGE_SIZE);

    (void)info;
    (void)input_package;
    while(1) { // read all input from server
        recv(info->socket, input_package, MX_PACKAGE_SIZE, MSG_WAITALL);
        printf("input: piece.%d, type.%d, login.%s\n", input_package->piece, input_package->type, input_package->login);
    }
    return NULL;
}
