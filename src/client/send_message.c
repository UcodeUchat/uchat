#include "uchat.h"

void mx_send_message(t_client_info *info) {
    t_package *package = mx_create_new_package(); // that must be NULL
    // with mem_copy need to copy data from 1-st argument in this function
    package->user_id = 1;
    strncat(package->login, info->login, sizeof(package->login) - 1);
    strncat(package->password, info->password, sizeof(package->password) - 1);
    //
    package->type = MX_MSG_TYPE;
    while(1) {
        printf("We are wainting for your msg\n");
        mx_get_input(package->data);
        printf("sizeof(package) = %zu\n", sizeof(package));
        write(info->socket, package, MX_PACKAGE_SIZE);
        mx_memset(package->data, 0, sizeof(package->data));
    }
}
