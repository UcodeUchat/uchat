#include "libmx.h"

char *mx_replace_substr(const char *str,
                        const char *sub, const char *replace) {
    if (str && sub && replace) {
        int sub_len = mx_strlen(sub);
        int replace_len = mx_strlen(replace);
        int sub_num = mx_count_substr(str, sub);
        char *newstr = mx_strnew(mx_strlen(str) 
            - sub_len * sub_num + replace_len * sub_num);
        char *to = newstr;

        while (*str)
            if (mx_strncmp(str, sub, sub_len) == 0) {
                mx_strcat(newstr, replace);
                to += replace_len;
                str += sub_len;
            }
            else
                *to++ = *str++;
        return newstr;
    }
    return NULL;
}
