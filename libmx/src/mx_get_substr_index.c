#include "libmx.h"

int mx_get_substr_index(const char *str, const char *sub) {
    int i = 0;
    int len = 0;

    if (str && sub) {
        len = mx_strlen(sub);
        for (; *str != '\0'; i++) {
            if (*str == *sub)
                if (mx_strncmp(str, sub, len) == 0)
                    return i;
            str++;
        }
        if (!str)
            return -1;
    }
    return -2;
}
