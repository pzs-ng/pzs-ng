#ifndef _RACE_FILE_H_
#define _RACE_FILE_H_

#include <sys/stat.h>

extern unsigned int readsfv(struct LOCATIONS *, struct VARS *, int);
extern void delete_sfv(struct LOCATIONS *);
extern void readrace(struct LOCATIONS *, struct VARS *, struct USERINFO **, struct GROUPINFO **);
extern void maketempdir(char *);
extern void read_write_leader(struct LOCATIONS *, struct VARS *, struct USERINFO *);
extern void testfiles(struct LOCATIONS *, struct VARS *, int);
extern int copysfv(char *, char *, off_t);
extern void create_indexfile(struct LOCATIONS *, struct VARS *, char *);
extern short clear_file(struct LOCATIONS *, char *);
extern void writerace(struct LOCATIONS *, struct VARS *, unsigned int, unsigned char);

#endif
