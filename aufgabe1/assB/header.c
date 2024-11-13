#include "header.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

bool is_string_numeric(const char *str) {
	if (*str == '\0')
		return false;

	while (*str) {
		if (!isdigit((unsigned char) *str))
			return false;
		str++;
	}

	return true;
}
