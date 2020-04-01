#include "uchat.h"

int main(int argc, char **argv) {
    t_info_client *info = NULL;

    if (argc < 3) {
        mx_printerr("usage: uchat [ip_adress] [port]\n");
        return -1;
    }

    info = (t_info_client *)malloc(sizeof(t_info_client));
    memset(info, 0, sizeof(t_info_client));
    (*info).argc = argc;
    (*info).argv = argv;
    (*info).ip = argv[1];
    (*info).port = (uint16_t) atoi(argv[2]);
    if (mx_start_client(info)) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    return 0;
}
