#include "libmx.h"

int mx_strncmp(const char *s1, const char *s2, int size) {
	size--;
	while ((*s1 && *s2) && size--) {
		if (*s1 != *s2)
			break;
		s1++;
		s2++;
	}
	return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}
