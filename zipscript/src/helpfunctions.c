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
void
strip_whitespaces(char *s)
{
	if (s)
		while (s[strlen(s)-1] == ' '  ||
	           s[strlen(s)-1] == '\t' ||
			   s[strlen(s)-1] == '\n' ||
	           s[strlen(s)-1] == '\r')
			s[strlen(s)-1] = '\0';
}

/* exclusive write lock */
void
xlock(struct flock *fl, int fd)
{
	fl->l_start = fl->l_len = fl->l_pid = 0;
	fl->l_type = F_WRLCK;
	fl->l_whence = SEEK_SET;
	
	if (fcntl(fd, F_SETLKW, fl) == -1) {
		d_log("fcntl: %s", strerror(errno));
	}
}	

/* unlock exclusive lock */
void
xunlock(struct flock *fl, int fd)
{
	fl->l_type = F_UNLCK;
	if (fcntl(fd, F_SETLK, fl) == -1) {
		d_log("fcntl: %s", strerror(errno));
	}
}

