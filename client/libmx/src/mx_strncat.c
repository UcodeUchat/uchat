#include "libmx.h"

char *mx_strncat(char *restrict s1, const char *restrict s2, int len) {
	int size = mx_strlen (s1) + mx_strlen (s2);
	short j = 0;

	for (int i = mx_strlen(s1); i <= size && len--; i++)
		s1[i] = s2[j++];
	return s1;
}
