#include "libmx.h"

void *mx_memmove(void *dst, const void *src, size_t len) {
    char *copy = mx_strdup(src);

    for (size_t i = 0; i < len; ++i)
        ((char *)dst)[i] = copy[i];
    free(copy);
    return dst;
}
