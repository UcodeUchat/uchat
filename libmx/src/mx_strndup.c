#include "libmx.h"

static size_t mx_check_len(const char *s1, size_t n) {
	size_t sz = 0;
	
	while (s1[sz++]);
	return sz - 1 > n ? sz - 1: n;
}

char *mx_strndup(const char *s1, size_t n) {
	char *newstr = s1 ? mx_strnew(mx_check_len(s1, n)) : NULL;

	return newstr ? mx_strncpy(newstr, s1, n) : NULL;
}
