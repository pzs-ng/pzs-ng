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

/* First Version:	????????	?
 * Modified:		20120926	Sked
 * Description:	remove trailing given chars, modifying the string
 */
void
tailstrip_chars(char *s, char *dels)
{
	if (s && *s) {
		char *p = s + strlen(s) - 1;
		while (s-1 != p && strchr(dels, (int)*p) != NULL)
			--p;
		*(++p) = '\0';
	}
}

/* First Version:	????????	?
 * Modified:		20120926	Sked
 * Description:	remove prefixing given chars, retunring the result
 */
char *
prestrip_chars(char *s, char *dels)
{
	if (s)
		while (*s && strchr(dels, (int)*s) != NULL)
			++s;
	return s;
}

/* First Version:	20120925	Sked
 * Description:	Removes all given characters from a string, modifying it
 */
void
strip_chars(char *s, char *dels)
{
	if (s != NULL) {
		char *p = s;
		while (*s) {
			if (strchr(dels, (int)*s) != NULL) {
				while (*p && strchr(dels, (int)*p) != NULL)
					++p;
			} else {
				++s;
				++p;
			}
			*s = *p;
		}
	}
}

/* exclusive write lock */
void
xlock(struct flock *fl, int fd, short type)
{
	fl->l_start = fl->l_len = fl->l_pid = 0;
	fl->l_type = type;
	fl->l_whence = SEEK_SET;
	
	if (fcntl(fd, F_SETLKW, fl) == -1) {
		d_log("xlock: fcntl: %s\n", strerror(errno));
	}
}	

/* unlock exclusive lock */
void
xunlock(struct flock *fl, int fd)
{
	fl->l_type = F_UNLCK;
	if (fcntl(fd, F_SETLK, fl) == -1) {
		d_log("xunlock: fcntl: %s\n", strerror(errno));
	}
}

