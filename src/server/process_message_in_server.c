#include "uchat.h"

int mx_process_message_in_server(t_server_info *info, t_package *package) {
    printf("msg from %s, piece=%d\n", package->login, package->piece);

    // save to db before sending
    mx_send_package_to_all_in_room(info, package);
    return 0;
}
