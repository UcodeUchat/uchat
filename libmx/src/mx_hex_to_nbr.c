#include "libmx.h"

unsigned long mx_hex_to_nbr(const char *hex) {
    int len = mx_strlen(hex) - 1;
    int val = 0;
    unsigned long nbr_res = 0;
    
    for (int i = 0; hex[i] != '\0'; i++) {
        if (hex[i] >= '0' && hex[i] <= '9')
            val = hex[i] - '0';
        else if (hex[i] >= 'a' && hex[i] <= 'f')
            val = hex[i] - 'a' + 10;
        else if (hex[i] >= 'A' && hex[i] <= 'F')
            val = hex[i] - 'A' + 10;
        else
            return 0;
        nbr_res += val * mx_pow(16, len);
        len--;
    }
    return nbr_res;
}
