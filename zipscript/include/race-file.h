#ifndef _RACE_FILE_H_
#define _RACE_FILE_H_

#include <sys/stat.h>
#include "objects.h"

/* this is what we write to racedata files */
typedef struct {
	unsigned int	crc32,
			speed;
	off_t		size;
	time_t		start_time;
	unsigned char	status;
	char		fname[PATH_MAX],
			uname[24],
			group[24];
} RACEDATA;

/* IMPLEMENT THIS */
/* this is put in sfvdata files */
typedef struct {
	unsigned int	crc32;
	char		fname[PATH_MAX];
} SFVDATA;

extern unsigned int readsfv(const char *, struct VARS *, int);
extern void delete_sfv(const char *);
extern void readrace(const char *, struct VARS *, struct USERINFO **, struct GROUPINFO **);
extern void maketempdir(char *);
extern void read_write_leader(const char *, struct VARS *, struct USERINFO *);
extern void testfiles(struct LOCATIONS *, struct VARS *, int);
extern int copysfv(const char *, const char *);
extern void create_indexfile(const char *, struct VARS *, char *);
extern short clear_file(const char *, char *);
extern void writerace(const char *, struct VARS *, unsigned int, unsigned char);

#endif
