#ifndef _ZSFUNCTIONS_H_
#define _ZSFUNCTIONS_H_

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <regex.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>
#include <unistd.h>
#ifndef PATH_MAX
# define _LIMITS_H_
# include <sys/syslimits.h>
#endif

#include "objects.h"
#include "macros.h"
#include "constants.h"

#define createzerofile(filename) fclose(fopen(filename, "a+"))

/* Creates status bar file */
#if ( status_bar_type == 0 )
# define createstatusbar(bardata) createzerofile(bardata);
#endif
#if ( status_bar_type == 1 )
# define createstatusbar(bardata) mkdir(bardata, 0777);
#endif
#if ( status_bar_type == 2 )
# define createstatusbar(bardata)
#endif

extern struct USERINFO  **userI;
extern struct GROUPINFO **groupI;
extern struct VARS      raceI;
extern struct dirent	**dirlist;
extern unsigned int     direntries;


#if ( debug_mode == TRUE )
#include <stdarg.h>
#endif

extern void d_log(char *, ...);

extern void create_missing(char *, short);
extern char * findfileext(char *);
extern int findfileextcount(char *);
extern unsigned int hexstrtodec(unsigned char *);
#ifdef __linux__
extern int selector (const struct dirent *);
extern int selector2 (const struct dirent *);
#else
extern int selector (struct dirent *);
extern int selector2 (struct dirent *);
#endif

extern void rescandir();
extern void rescandir2();
extern void strtolower(char *);
extern char israr(char *);
extern char isvideo(char *);
extern void buffer_progress_bar(struct VARS *);
extern void move_progress_bar(short delete, struct VARS *);
extern short findfile(char *);
extern void removecomplete();
extern short matchpath(char *, char *);
extern short strcomp(char *, char *);
extern short fileexists(char *);
extern void createlink(char *, char *, char *, char *);
extern void readsfv_ffile(char *, off_t buf_bytes);
extern void get_rar_info(char *);
extern int execute(char *);
extern char *abs2rel(const char *, const char *, char *, const size_t);

#endif
