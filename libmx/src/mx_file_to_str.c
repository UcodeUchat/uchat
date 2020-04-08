#include "libmx.h"

static int mx_file_len(const char *file) {
    short fl = open(file, O_RDONLY);
    short sz = 0;
    int len = 0;
    char buf;

    sz = read(fl, &buf, 1);
    while (sz > 0) {
        sz = read(fl, &buf, 1);
        len++;
    }
    close(fl);
    return len;
}

char *mx_file_to_str(const char *file) {
    int fl = open(file, O_RDONLY);
        
    if (fl == -1) {
        close(fl);
        return NULL;
    }
    else {
        int sz = 0;
        int size = mx_file_len(file);
        char *newstr;

        if (size == 0)
            return NULL;
        newstr = mx_strnew(size);
        sz = read(fl, newstr, size);
        close(fl);
        return newstr;
    }
}
