#ifndef _PROCESSFUNCS_H_
#define _PROCESSFUNCS_H_

#include "objects.h"
#include "racetypes.h"
#include "zsfunctions.h"

extern void read_envdata(GLOBAL *, GDATA *, UDATA *, struct stat *);
extern void check_filesize(GLOBAL *, const char *, struct stat *);
extern void set_uid_gid(void);
extern void group_dir_users(GLOBAL *); /* call this something else */
extern int match_nocheck_dirs(GLOBAL *);

/* turn these two into one function */
extern int check_zerosize(GLOBAL *, MSG *);
extern int check_banned_file(GLOBAL *, MSG *);

extern int process_file(GLOBAL *, MSG *, char **, char *, int *);
extern void check_release_type(GLOBAL *, MSG *, RACETYPE *, char *[2]);

extern void execute_script(char *, char *, char *);
extern int get_nfo_filetype(unsigned int);
extern void create_missing_missing_nfo_indicator(GLOBAL *, char *[2]);
extern void release_complete(GLOBAL *, MSG *, char *[2]);
extern void release_incomplete(GLOBAL *, MSG *, RACETYPE *);

#endif /* _PROCESSFUNCS_H_ */

