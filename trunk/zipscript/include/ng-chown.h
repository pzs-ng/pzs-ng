#ifndef NG_CHOWN_H
#define NG_CHOWN_H

#include <sys/types.h>

int 
#if defined(__linux__)
selector3(const struct dirent *d);
#elif defined(__NetBSD__)
selector3 (const struct dirent *d);
#else
selector3(struct dirent *d);
#endif

short int matchpath2(char *, char *);
int myscandir(char *, char *);
int myscan(int, int, int, int, int, int, int, char *, char *);
uid_t get_gluid(char *, char *);
gid_t get_glgid(char *, char *);
void *ng_realloc3(void *, int);
void *ng_free3(void *);
#endif

