#ifndef _RACE_FILE_H_
#define _RACE_FILE_H_

#include <sys/stat.h>

extern unsigned int readsfv_file(struct LOCATIONS *, struct VARS *, int);
extern void delete_sfv_file(struct LOCATIONS *);
extern void readrace_file(struct LOCATIONS *, struct VARS *, struct USERINFO **, struct GROUPINFO **);
extern void maketempdir(char *);
extern void read_write_leader_file(struct LOCATIONS *, struct VARS *, struct USERINFO *);
extern void testfiles_file(struct LOCATIONS *, struct VARS *, int);
extern int copysfv_file(char *, char *, off_t);
extern void create_indexfile_file(struct LOCATIONS *, struct VARS *, char *);
extern short clear_file_file(struct LOCATIONS *, char *);
extern void writerace_file(struct LOCATIONS *, struct VARS *, unsigned int, unsigned char);

#endif
