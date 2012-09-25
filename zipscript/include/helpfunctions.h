#ifndef _HELPFUNCTIONS_H
#define _HELPFUNCTIONS_H

#include <fcntl.h>

#define WHITESPACE_STR  " \f\n\r\t\v"
#define BAD_STR  "\b\f\n\r\t\v"

extern char *find_last_of(char *, const char *);
extern char *find_first_of(char *, const char *);
extern void tailstrip_chars(char *, char *);
extern char *prestrip_chars(char *, char *);
extern void strip_chars(char *, char*);
extern void xlock(struct flock *, int, short);
extern void xunlock(struct flock *, int);

#endif

