#ifndef _ZSFUNCTIONS_H_
#define _ZSFUNCTIONS_H_

#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <regex.h>
#ifndef PATH_MAX
# define _LIMITS_H_
# include <sys/syslimits.h>
#endif

#include "objects.h"

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


#if ( debug_mode == TRUE )
#include <stdarg.h>

extern void d_log(char *, ...);
#else
# define d_log
#endif


extern void create_missing(char *, short);
extern char * findfileext(char *);
extern int findfileextcount(char *);
extern unsigned long hexstrtodec(unsigned char *);
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
extern void readsfv_ffile(char *, long buf_bytes);
extern void get_rar_info(char *);
extern int execute(char *);

