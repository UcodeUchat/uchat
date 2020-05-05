#include "uchat.h"

int mx_save_file_in_server(t_server_info *info, t_socket_list *csl) {
    (void)info;
    printf("mx_save_file_in_server\n");
    printf("%s\n", json_object_to_json_string(csl->obj));
    return 0;
}
