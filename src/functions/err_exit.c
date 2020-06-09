#include "uchat.h"

void mx_err_exit(const char *err_msg) {
    mx_printerr(err_msg);
    exit(1);
}

int mx_err_return(const char *err_msg) {
    mx_printerr(err_msg);
    return 1;
}


int mx_err_return2(const char *err_msg, const char *err_msg2) {
    if (err_msg)
        mx_printerr(err_msg);
    if (err_msg2)
        mx_printerr(err_msg2);
    mx_printerr("\n");
    return 1;
}

int mx_err_return3(const char *err_msg, const char *err_msg2, int value) {
    if (err_msg)
        mx_printerr(err_msg);
    if (err_msg2)
        mx_printerr(err_msg2);
    mx_printerr("\n");
    return value;
}
