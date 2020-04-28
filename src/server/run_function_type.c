#include "uchat.h"

void mx_send_package_to_all_in_room(t_server_info *info, t_package *package) {
    int *sockets_online = mx_get_users_sock_in_room(&info, package->room_id);
    struct tls *tls_socket = NULL;

    for (int i = 0; i < info->wdb->i; i++) {
        tls_socket = mx_find_tls_socket(info->socket_list, sockets_online[i]);
        if (tls_socket)
            tls_write(tls_socket, package, MX_PACKAGE_SIZE);
    }
}

int mx_run_function_type(t_server_info *info, t_package *package) {
    int return_value = -1;
    int type = package->type;

    // fprintf(stderr, "Я нахожусь здесь....\n");
    fprintf(stderr, "type = [%d]\n", type);
    if (type == MX_MSG_TYPE)
        return_value = mx_process_message_in_server(info, package);
    else if (type == MX_FILE_TYPE)
        return_value = mx_process_file_in_server(info, package);
    else if (type == MX_AUTH_TYPE)
    	return_value = mx_authorization(info, package);
    else if (type == MX_REG_TYPE)
        mx_registration(info, package);
    return return_value;
}
