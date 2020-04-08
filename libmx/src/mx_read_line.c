#include "libmx.h"

int mx_read_line(char **lineptr, int buf_size, char delim, const int fd) { 
    if (lineptr && buf_size && delim && fd)
        return 1;
    else
        return 0;
}
