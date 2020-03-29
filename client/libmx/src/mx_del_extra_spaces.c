#include "libmx.h"

static int mx_size_without_extra_spaces(const char *str) {
    int size = 0;
    
    while (*str) {
        while (mx_isspace(*str) && mx_isspace(*(str + 1)))
            str++;
        size++;
        str++;
    }
    return size;
}

char *mx_del_extra_spaces(const char *str) {
    if (str) {
        char *trim_str = mx_strtrim(str);
        char *newstr = mx_strnew(mx_size_without_extra_spaces(trim_str));
        char *finalstr = newstr;

        for (int i = 0; trim_str[i];) {
            if (mx_isspace(trim_str[i])) {
                while (mx_isspace(trim_str[i]))
                    ++i;
                *newstr++ = ' ';
            }
            else {
                *newstr = trim_str[i++];
                newstr++;
            }
        }
        free(trim_str);
        return finalstr;
    }
    return NULL;
}
