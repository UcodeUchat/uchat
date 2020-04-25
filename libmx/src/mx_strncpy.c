#include "libmx.h"

char *mx_strncpy(char *dst, const char *src, int len) {
	char *to = dst;

	while (*src && len--)
		*to++ = *src++;
	while (len-- > 0)
		*to++ = '\0';
	return dst;
}
