#ifndef __GLUSERFILE_H
#define __GLUSERFILE_H

#include "glustruct.h"

/* uncomment the line below and comment this one if
   bzero isn't present on your system */
#include <string.h>
#ifdef _SunOS_
# include <strings.h>
#endif
#define gluser_reset(x) bzero(x, sizeof(GLUSER))
/* #define gluser_reset(x) _gluser_reset(x) */

int getint(char *, int *, int);
char *strip_whitespaces(char *);
AINT *strtointarray(AINT *, char *);

int gluser_read(GLUSER *, char *);
void gluser_init(GLUSER *);
void gluser_free(GLUSER *);

void _gluser_reset(GLUSER *);

void ufile_general(int *, char *);
void ufile_logins(int *, char *);
void ufile_timeframe(AINT *, char *);
void ufile_flags(short *, char *);
void ufile_added(ADDED *, char *);
void ufile_credits(AINT *, char *);
void ufile_ratio(AINT *, char *);
int ufile_ip(AIP *, char *);
void ufile_time(AINT *, char *);
void ufile_section(ASECTION *, char *);
int ufile_group(AGROUP *, char *);
void ufile_private(AGROUP *, char *);

#endif /* __GLUSERFILE_H */

