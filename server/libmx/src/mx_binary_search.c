#include "libmx.h"

int mx_binary_search(char **arr, int size, const char *s, int *count) {
    int left = 0;
    int right = size - 1;
    int mid = size / 2;

    *count = 0;
    while (left <= right) {
        mid = left + (right - left) / 2;
        ++*count;
        if (mx_strcmp(arr[mid], s) == 0)
            return mid;
        else if (mx_strcmp(arr[mid], s) < 0)
            left = mid + 1;
        else
            right = mid - 1;
    }
    *count = 0;
    return -1;
}
