#include "libmx.h"
#include <stdio.h>

char *mx_strcat(char *restrict s1, const char *restrict s2) {
    int s1_size = strlen(s1);
    int size = strlen(s1) + strlen(s2);
    int j = 0;

    for (int i = s1_size; i <= size; i++, j++)
        s1[i] = s2[j];
    return s1;
}
