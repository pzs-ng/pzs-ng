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

	while (*s == ' ' || *s == '\t')
		s++;

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

/* Create a .lock file for "path", which is
 * the full path to the file.
 *
 * returns -1 on failure, 0 on success
 */
int
xlock(const char *path)
{
	static int status, cnt = 0;
	static struct stat sb;
	static char *lockfile;

	if (stat(path, &sb) == -1) {
		d_log(1, "xlock(%s): stat(%s, %p): %s\n",
				path, path, &sb, strerror(errno));
		return -1;
	}

	if (sb.st_nlink == 1) {
		
		/* the length of ".lock" is 5, and \0 is 1 => 6 */
		lockfile = malloc(strlen(path)+6);
		sprintf(lockfile, "%s.lock", path);

		/* remove .lock file if it exists and is older than
		 * max_age_for_lock_file second(s) */
		if (stat(lockfile, &sb) != -1)
			if (time(NULL)-sb.st_mtime > max_age_for_lock_file)
				xunlock(path);
	
		do {
			
			status = link(path, lockfile);
			
			/* .lock file created */
			if (status == 0)
				break;

			usleep(100);
			cnt++;

		} while (status == -1 && errno == EEXIST &&
				cnt < 100);
		
		free(lockfile);
			
		if (status != 0) {
			d_log(1, "xlock(%s): link(%s, %s.lock): %s\n",
					path, path, path, strerror(errno));
			return -1;
		}
	
	}
	
	return 0;
}

/* remove .lock file for "path".
 *
 * to the caller it doesn't matter
 */
int
xunlock(const char *path)
{
	int status = 0;
	char *lockfile;
	
	lockfile = malloc(strlen(path)+6);
	sprintf(lockfile, "%s.lock", path);

	status = unlink(lockfile);

	free(lockfile);
		
	if (status == -1)
		d_log(1, "xunlock(%s): unlink(%s.lock): %s\n",
				path, path, strerror(errno));
	
	return status;
}

FILE *
xfopen(const char *path, const char *mode)
{
	FILE *fp = NULL;

	if ((fp = fopen(path, mode)) != NULL) {
		if (xlock(path) == -1) {
			fclose(fp);
			return NULL;
		}
	} else
		d_log(1, "xfopen(%s, %s): fopen(): %s\n",
				path, mode, strerror(errno));
	
	return fp;
}

int
xfclose(const char *path, FILE *fp)
{
	int status;

	status = fclose(fp);
	
	if (status == 0)
		xunlock(path);
	else
		d_log(1, "xfclose(%s, %p): fclose(): %s\n",
				path, fp, strerror(errno));
	
	return status;
}

