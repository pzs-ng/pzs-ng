#ifndef _HELPFUNCTIONS_H
#define _HELPFUNCTIONS_H

#include <fcntl.h>

extern char *find_last_of(char *, const char *);
extern char *find_first_of(char *, const char *);
extern void strip_whitespaces(char *);
extern void xlock(struct flock *, int, short);
extern void xunlock(struct flock *, int);

#endif

