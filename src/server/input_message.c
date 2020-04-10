#include "uchat.h"

int mx_input_message(t_server_info *info, t_package *package) {
    (void)info;
    printf("msg received from %s\n", package->login);
    return 0;
}
