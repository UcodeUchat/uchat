#include "uchat.h"

int mx_run_function_type(t_server_info *info, t_package *package) {
    int return_value = -1;
    int type = package->type;

    if (type == 1)
        return_value = mx_input_message(info, package);
    else if (type == 2)
        return_value = mx_input_file(info, package);
    return return_value;
}
