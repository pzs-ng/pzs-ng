#include <string.h>
#include "helpfunctions.h"

/* find the first occurance of any of delim in name */
char *
find_first_of(char *name, const char *delim)
{
	char		*n_ptr;
	const char	*d_ptr;

	for (n_ptr = name; *n_ptr; n_ptr++) {
		for (d_ptr = delim; *d_ptr; d_ptr++) {
			if (*n_ptr == *d_ptr)
				return n_ptr;
		}
	}
	
	return NULL;
}

/* find the last occurance of any of delim in name */
char *
find_last_of(char *name, const char *delim)
{
	char		*n_ptr, *result = NULL;
	const char	*d_ptr;

	for (n_ptr = name; *n_ptr; n_ptr++) {
		for (d_ptr = delim; *d_ptr; d_ptr++) {
			if (*n_ptr == *d_ptr)
				result = n_ptr;
		}
	}
	
	return result;
}

/* remove trailing whitespaces and newline stuff */
void
strip_whitespaces(char *s)
{
	while (s[strlen(s)-1] == ' '  ||
	       s[strlen(s)-1] == '\t' ||
	       s[strlen(s)-1] == '\n' ||
	       s[strlen(s)-1] == '\r')
		s[strlen(s)-1] = '\0';
}

