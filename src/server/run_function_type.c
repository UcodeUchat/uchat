#include "uchat.h"

int mx_run_function_type(t_server_info *info, t_package *package) {
    int return_value = -1;
    int type = package->type;


    fprintf(stderr, "Я нахожусь здесь....\n");
    if (type == MX_MSG_TYPE)
        return_value = mx_process_message_in_server(info, package);
    else if (type == MX_FILE_TYPE)
        return_value = mx_process_file_in_server(info, package);
    else if (type == MX_AUTH_TYPE)
    	return_value = mx_authorization(info, package);
    // else if (type == MX_REG_TYPE)
    mx_registration(info, package);
    return return_value;
}
