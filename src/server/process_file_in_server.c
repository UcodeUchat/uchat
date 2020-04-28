#include "uchat.h"

void set_file_name(t_package *package) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *file_name_in_package = mx_strdup(package->data);

    sprintf(package->data, "%d_%02d_%02d_%02d_%02d_%02d_%s",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec, file_name_in_package);
    mx_strdel(&file_name_in_package);
}

int mx_process_file_in_server(t_server_info *info, t_package *package) {
    printf("PIECE:%d, TYPE:%d, DATA-SIZE:%d\n", package->piece, package->type, mx_strlen(package->data));

    if (package->piece == 1) {
        set_file_name(package);
        printf("1=%d\n", mx_add_new_file_server(&(info->input_files), package) ? 1 : 0);
    }
    else if (package->piece == 2 || package->piece == 3) {
        printf("2=%d\n", mx_add_data_to_file_server(&(info->input_files), package) ? 1 : 0);
        if (package->piece == 3)
            printf("3=%d\n", mx_final_file_input_server(info, package) ? 1 : 0);
    }

    return 0;
}
