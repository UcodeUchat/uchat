#include "uchat.h"

bool pick_file_to_send(FILE **file, t_client_info *info, t_package *package) {
    (void)info;
    (void)package;
    // need to write a function for picking a file
    struct stat file_stat;
    char file_name[256] = {0};

    scanf("%s", file_name);
    printf("pick_file_to_send 1\n");
    if ((*file = fopen(file_name, "r")) != NULL) {
        if (ftrylockfile(*file) == 0) {
            printf("pick_file_to_send 2+\n");
            stat(file_name, &file_stat);
            package->add_info = file_stat.st_size;
            strncat(package->data, file_name, MX_MAX_DATA_SIZE);
            return 0;
        }
        else
            fprintf(stderr, "Can't send this file, something using it!\n");
    }
    return 1;
}

void mx_send_file_from_client(t_client_info *info) {
    t_package *package = mx_create_new_package();
    FILE *file_to_send;

    printf("mx_send_file \n");
    package->piece = 1;
    package->type = MX_FILE_SEND_TYPE;
    package->room_id = 0; // take that from info
    package->user_id = info->id; // take that from info
    if (pick_file_to_send(&file_to_send, info, package) == 0) {
        int num_bytes = 1;

        tls_write(info->tls_client, package, MX_PACKAGE_SIZE);
        mx_memset(package->data, 0, MX_MAX_DATA_SIZE);
        while(num_bytes > 0 && !feof(file_to_send)) {
            sleep(1);
            num_bytes = fread(package->data, 1, MX_MAX_DATA_SIZE, file_to_send);
            package->piece = feof(file_to_send) ? 3 : 2;
            package->add_info = num_bytes;
            tls_write(info->tls_client, package, MX_PACKAGE_SIZE);
            mx_memset(package->data, 0, MX_MAX_DATA_SIZE);
        }
        funlockfile(file_to_send);
        fclose(file_to_send);
    }
}
