#include "libmx.h"

char *mx_strstr(const char *haystack, const char *needle) {
	int symb = mx_strlen(needle);
	
	if (!symb)
		return (char *)haystack;
	while (*haystack) {
		if (*haystack == *needle) {
			if (mx_strncmp(haystack, needle, symb) == 0)
				return (char *)haystack;
		}
		haystack++;
	}
	return 0;
}
