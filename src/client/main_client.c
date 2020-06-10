#include "uchat.h"

static void create_download_folder() {
    struct stat st;// = {0};

    if (stat(MX_SAVE_FOLDER_IN_CLIENT, &st) == -1) {
        mkdir(MX_SAVE_FOLDER_IN_CLIENT, 0700);
    }
}

static void init_info(t_client_info *info, int argc, char **argv) {
    (*info).argc = argc;
    (*info).argv = argv;
    (*info).ip = argv[1];
    (*info).port = (uint16_t) atoi(argv[2]);
    (*info).tls_client = NULL;
    (*info).responce = 0;
    (*info).can_record = 1;
}

int main(int argc, char **argv) {
    t_client_info *info = NULL;

    if (argc != 3) {
        mx_printerr("usage: uchat [ip_adress] [port]\n");
        return -1;
    }
    gtk_init (&argc, &argv);
    info = (t_client_info *)malloc(sizeof(t_client_info));
    memset(info, 0, sizeof(t_client_info));
    init_info(info, argc, argv);
    create_download_folder();
    if (mx_start_client(info)) {
        printf("error = %s\n", strerror(errno));
        // info clean and close
        return -1;
    }
    printf("exit client\n");
    return 0;
}
