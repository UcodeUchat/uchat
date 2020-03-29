#include "libmx.h"

short mx_digits_num(long long num) {
    int sum = 0;
    
    if (num == 0)
        return 1;
    if (num < 0) {
        num = -num;
        sum++;
    }
    while (num) {
        sum++;
        num /= 10;
    }
    return sum;
}
