#include "uchat.h"

bool pick_file_to_send(FILE **file, t_client_info *info, t_package *package) {
    (void)info;
    (void)package;
    // need to write a function for picking a file
    struct stat file_stat;
    char file_name[128] = "download.jpeg";
    if ((*file = fopen(file_name, "r")) != NULL) {
        stat(file_name, &file_stat);
        package->add_info = file_stat.st_size;
        strncat(package->data, file_name, MX_MAX_DATA_SIZE);
        return 0;
    }
    //
    return 1;
}

void mx_send_file_from_client(t_client_info *info) {
    t_package *package = mx_create_new_package();
    FILE *file_to_send;

    package->piece = 1;
    package->type = MX_FILE_TYPE;
    package->room_id = 0; // take that from info
    package->user_id = 0; // take that from info
    if (pick_file_to_send(&file_to_send, info, package) == 0) {
        int num_bytes = 1;

        write(info->socket, package, MX_PACKAGE_SIZE);
        mx_memset(package->data, 0, MX_MAX_DATA_SIZE);
        while(num_bytes > 0 && !feof(file_to_send)) {
            num_bytes = fread(package->data, 1, MX_MAX_DATA_SIZE, file_to_send);
            package->piece = feof(file_to_send) ? 3 : 2;
            write(info->socket, package, MX_PACKAGE_SIZE);
            mx_memset(package->data, 0, MX_MAX_DATA_SIZE);
        }
        fclose(file_to_send);
    }
}
