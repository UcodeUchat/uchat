#include "libmx.h"

static int sort(char **arr, int left, int right) {
    int l = left;
    int r = right;
    char *mid = arr[left + (right - left) / 2];

    while (l <= r) {
        for (; mx_strlen(arr[l]) < mx_strlen(mid); l++);
        for (; mx_strlen(arr[r]) > mx_strlen(mid); r--);

        if (l <= r) {
            if (mx_strlen(arr[l]) > mx_strlen(arr[r])) {
                mx_swap_str(&arr[l], &arr[r]);
            }
            l++;
            r--;
        }
    }
    return 1 + (l < right ? sort(arr, l, right) : 0) 
    + (left < r ? sort(arr, left, r) : 0);
}

int mx_quicksort(char **arr, int left, int right) {
    return arr ? sort(arr, left, right) : -1;
}
