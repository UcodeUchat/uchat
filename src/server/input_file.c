#include "uchat.h"

int mx_input_file(t_server_info *info, t_package *package) {
    (void)info;
    printf("file received from %s\n", package->login);
    return 0;
}
