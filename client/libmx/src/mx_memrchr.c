#include "libmx.h"
#include <stdio.h>

void *mx_memrchr(const void *s, int c, size_t n) {
    const unsigned char *str = (unsigned char *)s + n;

    while ((str != s)) {
        if (*str == c)
            return (void *)str;
        else
            str--;
    }
    if (*str == c)
        return (void *)str;
    return NULL;
}
