#include "uchat.h"

int mx_process_message_in_server(t_server_info *info, t_package *package) {
    (void)info;
    printf("msg from %s, piece=%d\n", package->login, package->piece);
    write(package->client_sock, package, MX_PACKAGE_SIZE);
    return 0;
}
