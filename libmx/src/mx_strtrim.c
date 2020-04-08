#include "libmx.h"

char *mx_strtrim(const char *str) {
	if (str) {
		int len = mx_strlen(str);
		int start = 0;
		int end = 0;
		char *res;

		for (; str[start] && mx_isspace(str[start]); start++);
		for (int j = len - 1; str[j] && mx_isspace(str[j]); j--, end++);

		if (start + end >= len)
			return mx_strnew(0);
		res = mx_strnew(len - start - end);
		mx_strncpy(res, str + start, len - start - end);
		return res;
	}
	return NULL;
}
