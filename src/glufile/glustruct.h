#ifndef __GLUSTRUCTS_H
#define __GLUSTRUCTS_H

typedef struct _added {
	int time;
	char uname[24];
} ADDED;

typedef struct _section {
	int files;
	long kilobytes;
	int seconds;
} SECTION;

typedef struct _group {
	char gname[16];
	short gadmin; 
} GROUP;

typedef struct _ip {
	char *ident;
	char ip[16];
} IP;

/* A nifty solution to keep track of how many
   elements are allocated
*/
typedef struct _aip {
	int n;
	IP *ips;
} AIP;

typedef struct _agroup {
	int n;
	GROUP *grps;
} AGROUP;

typedef struct _aint {
	int n;
	int *val;
} AINT;

typedef struct _asection {
	int n;
	SECTION *s;
} ASECTION;

/* All information from a userfile
   is finally stored in one of these
*/
typedef struct _gluser {
	
	short read;
	
	char *user; 
	char *homedir;
	
	ADDED added;

	int expires;
	
	/* reconsider this */
	int general[5];
	
	int logins[4];

	/* 0 unset, 1 set */
	short flags[22];

	char *tagline;
	char *dir;

	AINT ratio;
	AINT credits;
	AINT time;
	AINT timeframe;

	/* 0 allup
	   1 alldn
	   2 wkup
	   3 wkdn
	   4 dayup
	   5 daydn
	   6 monthup
	   7 monthdn
	   8 nuke
	*/
	ASECTION sections[9];

	AIP ip;

	AGROUP groups;
	AGROUP private;
	
} GLUSER;

#endif /* __GLUSTRUCTS_H */

