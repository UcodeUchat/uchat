#include "uchat.h"

int mx_process_file_in_server(t_server_info *info, t_package *package) {
    (void)info;
    printf("PIECE:%d, TYPE:%d, DATA-SIZE:%d\n", package->piece, package->type, mx_strlen(package->data));
    // send to all needed users
    write(package->client_sock, package, MX_PACKAGE_SIZE);
    //
    return 0;
}
