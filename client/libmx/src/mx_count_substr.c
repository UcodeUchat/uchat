#include "libmx.h"

int mx_count_substr(const char *str, const char *sub) {
    int num_of_str = 0;
    int len = 0;

    if (!sub || !str)
        return -1;
    len = mx_strlen(sub);
    while (*str) {
        if (*str == *sub) {
            if (mx_strncmp(str, sub, len) == 0) {
                num_of_str++;
                str += mx_strlen(sub);
            }
            else
                str++;
        }
        else
            str++;
    }
    return num_of_str;
}
