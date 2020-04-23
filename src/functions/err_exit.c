#include "uchat.h"

void mx_err_exit(const char *err_msg) {
    mx_printerr(err_msg);
    exit(1);
}
