#ifndef _RACE_FILE_H_
#define _RACE_FILE_H_

unsigned long readsfv_file(struct LOCATIONS *, struct VARS *, int);
void delete_sfv_file(struct LOCATIONS *);
void readrace_file(struct LOCATIONS *, struct VARS *, struct USERINFO **, struct GROUPINFO **);
void maketempdir(struct LOCATIONS *);
void read_write_leader_file(struct LOCATIONS *, struct VARS *, struct USERINFO *);
void testfiles_file(struct LOCATIONS *, struct VARS *);
void copysfv_file(char *, char *, long);
void create_indexfile_file(struct LOCATIONS *, struct VARS *, char *);
short clear_file_file(struct LOCATIONS *, char *);
void writerace_file(struct LOCATIONS *, struct VARS *, unsigned long, unsigned char);

#endif

