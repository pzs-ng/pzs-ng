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
	char		fname[NAME_MAX],
			uname[24],
			group[24];
} RACEDATA;

/* this is put in sfvdata files */
typedef struct {
	unsigned int	crc32;
	char		fname[NAME_MAX];
} SFVDATA;

/* this is what we put in a special 'head' file for version control, lock etc */
typedef struct {
	short int	data_version;		// version control.
	short int	data_type;		// type of release.
	short int	data_in_use;		// which program currently holds the lock.
	short int	data_incrementor;	// a check to see if nothing else wants the lock.
	short int	data_queue;		// positions in queue.
	short int	data_qcurrent;		// current position in queue;
} HEADDATA;

extern unsigned int readsfv(const char *, struct VARS *, int);
extern void update_sfvdata(const char *, const unsigned int);
extern void sfvdata_to_sfv(const char *, const char *);
extern void delete_sfv(const char *);
extern void readrace(const char *, struct VARS *, struct USERINFO **, struct GROUPINFO **);
extern void maketempdir(char *);
extern void read_write_leader(const char *, struct VARS *, struct USERINFO *);
extern void testfiles(struct LOCATIONS *, struct VARS *, int);
extern int copysfv(const char *, const char *, struct VARS *);
extern void create_indexfile(const char *, struct VARS *, char *);
extern short clear_file(const char *, char *);
extern void writerace(const char *, struct VARS *, unsigned int, unsigned char);
extern void remove_from_race(const char *, const char *);
extern int verify_racedata(const char *);
extern int create_lock(struct VARS *, const char *, short int, short int, short int);
extern void remove_lock(struct VARS *);
extern int update_lock(struct VARS *, short int, short int);

#endif

