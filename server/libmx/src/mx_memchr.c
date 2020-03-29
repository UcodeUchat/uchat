#include "libmx.h"

void *mx_memchr(const void *s, int c, size_t n) {
    const unsigned char *str = s;

    while (*str && n--) {
        if (*str == c)
            return (void *)str;
        else
            str++;
    }
    return NULL;
}
