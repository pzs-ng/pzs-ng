#ifndef _RACE_FILE_H_
#define _RACE_FILE_H_

#include <sys/stat.h>

extern unsigned int readsfv(const char *, struct VARS *, int);
extern void delete_sfv(const char *);
extern void readrace(const char *, struct VARS *, struct USERINFO **, struct GROUPINFO **);
extern void maketempdir(char *);
extern void read_write_leader(const char *, struct VARS *, struct USERINFO *);
extern void testfiles(struct LOCATIONS *, struct VARS *, int);
extern int copysfv(char *, char *, off_t);
extern void create_indexfile(const char *, struct VARS *, char *);
extern short clear_file(const char *, char *);
extern void writerace(const char *, struct VARS *, unsigned int, unsigned char);

#endif
