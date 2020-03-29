#include "libmx.h"

static int count_word_len(const char *s, char c) {
	int num = 0;

	for (; *s && *s != c; ++s)
		++num;
	return num;
}

char **mx_strsplit(const char *s, char c) {
	if (s) {
		char **words = (char **)malloc(sizeof(char*)
		* (mx_count_words(s, c) + 1));
		int symbols = 0;
		int word_index = 0;
		
		while (*s) {
			for (; *s && *s == c; ++s);
			symbols = count_word_len(s, c);
			if (symbols) {
				words[word_index] = mx_strnew(symbols);
				mx_strncpy(words[word_index++], s, symbols);
			}
			s += symbols;
			*s ? s++ : 0;
		}
		words[word_index] = NULL;
		return words;
	}
	return NULL;
}
