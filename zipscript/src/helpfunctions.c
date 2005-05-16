#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "zsfunctions.h"
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
	
	return n_ptr;
}

/* find the last occurance of any of delim in name */
char *
find_last_of(char *name, const char *delim)
{
	char		*n_ptr, *result;
	const char	*d_ptr;

	for (result = n_ptr = name; *n_ptr; n_ptr++) {
		for (d_ptr = delim; *d_ptr; d_ptr++) {
			if (*n_ptr == *d_ptr)
				result = n_ptr;
		}
	}
	
	return result;
}

/* remove trailing whitespaces and newline stuff */
char *
strip_whitespaces(char *s)
{
	size_t len;

	if (s && *s) {
		len = strlen(s) - 1;
		while (s[len] == ' '  ||
			s[len] == '\t' ||
			s[len] == '\n' ||
			s[len] == '\r') {
				s[len] = '\0';
				len--;
			}
	}

	return s;
}

/* remove preceding whitespaces and newline stuff */
char *
prestrip_whitespaces(char *s)
{
	if (s)
		while (*s == ' '  ||
	           *s == '\t' ||
		   *s == '\n' ||
	           *s == '\r')
			s++;
	return s;
}

/* exclusive write lock */
void
xlock(struct flock *fl, int fd, short type)
{
	fl->l_start = fl->l_len = fl->l_pid = 0;
	fl->l_type = type;
	fl->l_whence = SEEK_SET;
	
	if (fcntl(fd, F_SETLKW, fl) == -1) {
		d_log(1, "xlock: fcntl: %s\n", strerror(errno));
	}
}	

/* unlock exclusive lock */
void
xunlock(struct flock *fl, int fd)
{
	fl->l_type = F_UNLCK;
	if (fcntl(fd, F_SETLK, fl) == -1) {
		d_log(1, "xunlock: fcntl: %s\n", strerror(errno));
	}
}

