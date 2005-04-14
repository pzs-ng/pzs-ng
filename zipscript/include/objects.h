#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <sys/time.h>
#include "constants.h"
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


#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

struct USERINFO {
	char		name[24];	/* Username */
	off_t		bytes;		/* Bytes uploaded */
	double		speed;		/* Time spent uploading (secs) */
	unsigned char	files;		/* Files uploaded */
	unsigned char	pos;		/* User position */
	unsigned char	group;		/* Primary group number */

	unsigned int	dayup;		/* Day up stats */
	unsigned int	wkup;		/* Week */
	unsigned int	monthup;	/* Month */
	unsigned int	allup;		/* Alltime */
	char		tagline[64];	/* Tagline */
};

struct GROUPINFO {
	char		name[24];	/* Groupname */
	off_t		bytes;	/* Bytes uploaded */
	double		speed;	/* Time spent uploading (secs) */
	unsigned char	files;	/* Files uploaded */
	unsigned char	pos;	/* Group position */
	unsigned char	users;	/* Users in group; */
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
};

struct video {
	int		height;
	int		width;
	char	       *fps;
};

struct avi {
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
	char		tagline   [64];
	short int	pos;
};

struct current_file {
	char		name[NAME_MAX];
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
	short int	release_type;
	short int	sfv_match;
	short int	in_sfvfile;
	unsigned char	write_log;
	long		fastest_user[2];
	long		slowest_user[2];
};

struct LOCK {
	char		headpath[PATH_MAX];
	short int	data_incrementor;
	short int	data_in_use;
	short int	data_queue;
	short int	data_type;
};
	
struct VARS {
	struct current_user user;
	struct current_file file;
	struct race_total total;
	struct misc	misc;
	struct audio	audio;
	struct video	video;
	struct LOCK	lock;
	unsigned char	section;
	char		sectionname[128];
	char		avi[1024];
};

struct LOCATIONS {
	char		*leader;
	char		*nfo_incomplete;
	char		*incomplete;
	char		*sfv;
	char		*race;
	char		link_target[PATH_MAX];
	char		link_source[PATH_MAX];
	char		path[PATH_MAX];
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
#define sfv_version	17

#endif
