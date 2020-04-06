#include "libmx.h"

void mx_strdel(char **str) {
    if ((str != NULL && *str != NULL)) {
        if (malloc_size(*str))
            free(*str);
        *str = NULL;
    }
}

