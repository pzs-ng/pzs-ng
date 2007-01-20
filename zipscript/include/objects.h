#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <sys/time.h>
#ifdef _WITH_SS5
#include "constants.ss5.h"
#else
#include "constants.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef PATH_MAX
 #define _LIMITS_H_
 #if defined(_SunOS_)
  #include <syslimits.h>
 #elif defined(_BSD_)
  #include <sys/syslimits.h>
 #else
  #include <limits.h>
  #include <syslimits.h>
 #endif
#endif

#ifndef PATH_MAX
 #define PATH_MAX 1024
 #define NAME_MAX 255
 #define _ALT_MAX
#endif

#if NAME_MAX%4
 #define NAMEMAX NAME_MAX+4-NAME_MAX%4
#else 
 #define NAMEMAX NAME_MAX
#endif

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

struct USERINFO {
	char		name[24];	/* Username */
	off_t		bytes;	/* Bytes uploaded */
	double		speed;	/* Time spent uploading (secs) */
	unsigned int	files;	/* Files uploaded */
	unsigned int	pos;	/* User position */
	unsigned int	group;	/* Primary group number */

	unsigned long long	dayup;	/* Day up stats */
	unsigned long long	wkup;	/* Week */
	unsigned long long	monthup;/* Month */
	unsigned long long	allup;	/* Alltime */
};

struct GROUPINFO {
	char		name[24];	/* Groupname */
	off_t		bytes;	/* Bytes uploaded */
	double		speed;	/* Time spent uploading (secs) */
	unsigned int	files;	/* Files uploaded */
	unsigned int	pos;	/* Group position */
	unsigned int	users;	/* Users in group; */
};

struct audio {
	char		id3_artist[31];
	char		id3_title [31];
	char		id3_album [31];
	char		id3_year  [5];
	char		bitrate   [5];
	char		samplingrate[6];
	char           *id3_genre;
	char           *layer;
	char           *codec;
	char           *channelmode;
	char		vbr_version_string[10];
	char		vbr_preset[15];
	int		is_vbr;
	char		vbr_oldnew[1];
};

struct video {
	int		height;
	int		width;
	char	       *fps;
};

struct VIDEO {
	int		height;
	int		width;
	double		fps;
	char	       *vids;
	char	       *fourcc;
	long		hz;
	int		ch;
	char	       *audio;
	char	       *audiotype;
};

struct current_user {
	char		name      [24];
	char		group     [24];
	char		tagline   [255];
	short int	pos;
};

struct current_file {
	char		name[NAME_MAX];
	char		unlink[NAME_MAX];
	unsigned int	speed;
	off_t		size;
	char		compression_method;
};

struct race_total {
	unsigned int	start_time;
	unsigned int	stop_time;
	unsigned char	users;
	unsigned char	groups;
	int		files;
	int		files_missing;
	int		files_bad;
	unsigned char	nfo_present;
	double		speed;
	off_t		size;
	off_t		bad_size;
};

struct misc {
	char		old_leader[24];
	char            release_name[PATH_MAX];
	char	        current_path[PATH_MAX];
	char		racer_list[1024];
	char		total_racer_list[1024];
	char		top_messages[2][1024];
	char		error_msg [80];
	char		progress_bar[15];
	int		release_type;
	unsigned char	write_log;
	long		fastest_user[2];
	long		slowest_user[2];
};

struct VARS {
	struct current_user user;
	struct current_file file;
	struct race_total total;
	struct misc	misc;
	struct audio	audio;
	struct video	video;
	unsigned char	section;
	char		sectionname[128];
	char		headpath[PATH_MAX];
	unsigned int	data_incrementor;
	unsigned int	data_in_use;
	unsigned int	data_queue;
	unsigned int	data_type;
	char		id3_artist[31];
	char		id3_genre[31];
};

struct LOCATIONS {
	char		*leader;
	char		*nfo_incomplete;
	char		*incomplete;
	char		*sfv;
	char		*sfvbackup;
	char		*sfv_incomplete;
	char		*race;
	char		link_target[PATH_MAX];
	char		link_source[PATH_MAX];
	char		path[PATH_MAX];
	char		basepath[PATH_MAX];
	unsigned int	length_path;
	unsigned int	length_zipdatadir;
	unsigned int	in_cd_dir;
};

/* passing this around is a lot easier than passing
 * a lot of other shit around */
typedef struct {
	struct USERINFO		**ui;
	struct GROUPINFO	**gi;
	struct VARS		v;
	struct LOCATIONS	l;
} GLOBAL; /* reconsider this name */

/* sfv_version - must be > 5. Should not be any need to add a version
 * for racedata - if either sfv_data or racedata changes, they both
 * should be removed */
#define sfv_version	16

#endif
