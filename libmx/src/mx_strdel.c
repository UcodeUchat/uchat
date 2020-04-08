#include "libmx.h"

void mx_strdel(char **str) {
    if (str && *str && malloc_size(*str)) {
        free(*str);
        *str = NULL;
    }
}
