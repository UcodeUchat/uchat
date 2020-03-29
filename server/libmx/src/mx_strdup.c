#include "libmx.h"

char *mx_strdup(const char *s1) {
    char *newstr = mx_strnew(mx_strlen(s1));

    if (newstr == NULL)
        return NULL;
    return newstr ? mx_strcpy(newstr, s1) : NULL;
}
