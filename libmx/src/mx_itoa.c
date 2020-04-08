#include "libmx.h"

char *mx_itoa(int number) {
    char *num_to_str = mx_strnew(mx_digits_num(number));
    long num = number;
    bool sym = 0;
    short i = 0;
    
    if (number == 0)
        *num_to_str = '0';
    else if (num < 0) {
        num = -num;
        sym = 1;
    }
    while (num) {
        num_to_str[i] = num % 10 + '0';
        num /= 10;
        i++;
    }
    if (sym)
        num_to_str[i] = '-';
    mx_str_reverse(num_to_str);
    return num_to_str;
}
