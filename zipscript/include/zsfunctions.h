#ifndef _ZSFUNCTIONS_H_
#define _ZSFUNCTIONS_H_

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#ifdef	USE_HSREGEX
 #include "hsregex.h"
#else
 #include <regex.h>
#endif
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>
#include <unistd.h>
#ifndef PATH_MAX
#define _LIMITS_H_
 #ifdef _SunOS_
  #include <syslimits.h>
 #else
  #include <sys/syslimits.h>
 #endif
#endif

#include <config.h>

#include "objects.h"
#include "macros.h"
#ifdef _WITH_SS5
#include "constants.ss5.h"
#else
#include "constants.h"
#endif

#define createzerofile(filename) fclose(fopen(filename, "a+"))

/* Creates status bar file */
#if ( status_bar_type == 0 )
#define createstatusbar(bardata) createzerofile(bardata);
#endif
#if ( status_bar_type == 1 )
#define createstatusbar(bardata) mkdir(bardata, 0777);
#endif
#if ( status_bar_type == 2 )
#define createstatusbar(bardata)
#endif

extern struct USERINFO **userI;
extern struct GROUPINFO **groupI;
extern struct VARS raceI;
extern struct dirent **dirlist;
extern unsigned int direntries;


#if ( debug_mode == TRUE )
#include <stdarg.h>
#endif

extern void	d_log(char *,...);

extern void	create_missing(char *, short);
extern char    *findfileext(char *);
extern char    *findfileextparent(char *);
extern int	findfileextcount(char *);
extern unsigned int hexstrtodec(unsigned char *);
#if defined(__linux__)
extern int	selector(const struct dirent *);
#elif defined(__NetBSD__)
extern int	selector(const struct dirent *);
#else
extern int	selector(struct dirent *);
#endif

extern void	rescandir();
extern void	temprescandir(int);
extern void	rescanparent();
extern void	temprescanparent(int);
extern void	del_releasedir(char *);
extern void	strtolower(char *);
extern void	unlink_missing(char *);
extern char	israr(char *);
extern char	isvideo(char *);
extern void	buffer_progress_bar(struct VARS *);
extern void	move_progress_bar(unsigned char, struct VARS *);
extern short	findfile(char *);
extern char    *findfilename(char *);
extern void	removedotfiles();
extern void	removecomplete();
extern short	matchpath(char *, char *);
extern short	matchpartialpath(char *, char *);
extern short	strcomp(char *, char *);
extern short	subcomp(char *);
extern short	fileexists(char *);
extern void	createlink(char *, char *, char *, char *);
extern void	readsfv_ffile(char *, off_t);
extern void	get_rar_info(char *);
extern int	execute(char *);
extern char    *abs2rel(const char *, const char *, char *, const size_t);
extern char    *get_g_name(int);
extern char    *get_u_name(int);
extern int	buffer_groups(char *, int);
extern int	buffer_users(char *, int);
extern unsigned long sfv_compare_size(char *, unsigned long);
extern void	mark_as_bad(char *);
#endif
