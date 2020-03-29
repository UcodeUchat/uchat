#include "inc/uchat.h"

int main(int argc, char **argv) {
    t_info_client *info = NULL;

    mx_init_info_client(&info, argc, argv);
    mx_start_client2(info);
    return 0;
}
