#include "../inc/uchat.h"

int main(int argc, char **argv) {
    t_info *info = NULL;

    mx_init_info(&info, argc, argv);
    mx_start_client(info);
    return 0;
}
