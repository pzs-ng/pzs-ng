#ifndef _RACE_FILE_H_
#define _RACE_FILE_H_

#include <sys/stat.h>
#include "objects.h"

/* this is what we write to racedata files */
typedef struct {
	unsigned int	crc32,
			speed;
	unsigned long	bitrate;
	off_t		size;
	time_t		start_time;
	unsigned char	status;
	unsigned char	dummy;
	char		fname[NAMEMAX],
			uname[24],
			group[24],
			tagline[64];
} RACEDATA;

/* this is put in sfvdata files */
typedef struct {
	unsigned int	crc32;
	char		fname[NAMEMAX];
	short int	fmatch;
} SFVDATA;

/* this is what we put in a special 'head' file for version control, lock etc */
typedef struct {
	short int	data_version,		// version control.
			data_type,		// type of release.
			data_in_use,		// which program currently holds the lock.
			data_incrementor,	// a check to see if nothing else wants the lock.
			data_queue,		// positions in queue.
			data_qcurrent,		// current position in queue.
			data_pid;		// the pid of the process holding the lock.
} HEADDATA;

extern unsigned int readsfv(const char *, struct VARS *, int);
extern void update_sfvdata(const char *, const char *, const unsigned int);
extern void sfvdata_to_sfv(const char *, const char *);
extern void delete_sfv(const char *, struct VARS *);
extern void readrace(const char *, struct VARS *, struct USERINFO **, struct GROUPINFO **);
extern void maketempdir(char *);
extern void read_write_leader(const char *, struct VARS *, struct USERINFO *);
extern void testfiles(struct LOCATIONS *, struct VARS *, int);
extern int copysfv(const char *, const char *, struct VARS *);
extern void create_indexfile(const char *, struct VARS *, char *);
extern short clear_file(const char *, char *);
extern short match_file(char *, char *);
extern void writerace(const char *, struct VARS *, unsigned int, unsigned char);
extern void write_bitrate_in_race(const char *, struct VARS *);
extern int read_bitrate_in_race(const char *, struct VARS *);
extern void remove_from_race(const char *, const char *);
extern int verify_racedata(const char *);
extern int create_lock(struct VARS *, const char *, short int, short int, short int);
extern void remove_lock(struct VARS *);
extern int update_lock(struct VARS *, short int, short int);

#endif

