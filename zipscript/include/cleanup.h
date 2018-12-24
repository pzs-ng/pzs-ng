#ifndef CLEANUP_H
#define CLEANUP_H

void scandirectory(char *, int);
char *replace_cookies(char *);
void incomplete_cleanup(char *, char *, int);
void cleanup(char *, char *, int, char *);
int checklink(char *, char *, int);
int regcomp_error(int, regex_t *, char *);
short matchpath(char *, char *);

#endif
