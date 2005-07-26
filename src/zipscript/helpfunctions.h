#ifndef _HELPFUNCTIONS_H
#define _HELPFUNCTIONS_H

#include <fcntl.h>

extern char *find_last_of(char *, const char *);
extern char *find_first_of(char *, const char *);
extern char *strip_whitespaces(char *);
extern char *prestrip_whitespaces(char *);
extern int xlock(const char *);
extern int xunlock(const char *);
extern FILE *xfopen(const char *, const char *);
extern int xfclose(const char *, FILE *);

#endif /* _HELPFUNCTIONS_H */

