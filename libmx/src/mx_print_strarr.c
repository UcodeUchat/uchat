#include "libmx.h"

void mx_print_strarr(char **arr, const char *delim) {
    if (arr && delim) {
        while (*arr) {
            mx_printstr(*arr);
            if (*(arr + 1))
                write(1, delim, mx_strlen(delim));
            arr++;
        }
        write (1, "\n", 1);
    }
}
