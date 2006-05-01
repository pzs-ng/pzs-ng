#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <sys/time.h>
#include "zsconstants.h"
#include "constants.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*#include "zsconfig.h"
#include "zsconfig.defaults.h"*/

struct USERINFO {
	char		name[24];	/* Username */
	off_t		bytes;		/* Bytes uploaded */
	double		speed;		/* Time spent uploading (secs) */
	unsigned char	files,		/* Files uploaded */
			pos,		/* User position */
			group;		/* Primary group number */

	unsigned int	dayup,		/* Day up stats */
			wkup,		/* Week */
			monthup,	/* Month */
			allup;		/* Alltime */
	char		tagline[64];	/* Tagline */
};

struct GROUPINFO {
	char		name[24];	/* Groupname */
	off_t		bytes;	/* Bytes uploaded */
	double		speed;	/* Time spent uploading (secs) */
	unsigned char	files,	/* Files uploaded */
			pos,	/* Group position */
			users;	/* Users in group; */
};

struct audio {
	char		id3_artist[31],
			id3_title [31],
			id3_album [31],
			id3_year  [5],
			bitrate   [5],
			samplingrate[6],
		       *id3_genre,
		       *layer,
		       *codec,
		       *channelmode,
			vbr_version_string[10],
			vbr_preset[15];
	int		is_vbr,
			id3_genrenum;
};

struct video {
	int		height,
			width;
	char	       *fps;
};

struct MULTIMEDIA {
	int		height,
			width;
	double		fps;
	char		vids[100],
			fourcc[100];
	long		hz;
	int		ch;
	char		audio[100];
	int		audiotype;
};

struct current_user {
	char		name      [24],
			group     [24],
			tagline   [64];
	short int	pos;
};

struct current_file {
	char		name[NAME_MAX];
	unsigned int	speed;
	off_t		size;
	char		compression_method;
};

struct race_total {
	unsigned int	start_time,
			stop_time;
	unsigned char	users,
			groups;
	int		files,
			files_missing,
			files_bad;
	unsigned char	nfo_present;
	double		speed;
	off_t		size,
			bad_size;
};

struct misc {
	char		old_leader[24],
			release_name[PATH_MAX],
		        current_path[PATH_MAX],
			racer_list[1024],
			total_racer_list[1024],
			top_messages[2][1024],
			error_msg [80],
			progress_bar[15];
	unsigned int	release_type,
			sfv_match,
			in_sfvfile;
	unsigned char	write_log;
	long		fastest_user[2],
			slowest_user[2];
	int		nfofound;
};

struct LOCK {
	char		headpath[PATH_MAX];
	unsigned int	data_incrementor,
			data_in_use,
			data_queue,
			data_type;
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

struct RACEINFO {
	
};

struct LOCATIONS {
	char		*leader,
			*nfo_incomplete,
			*incomplete,
			*race,
			sfv[PATH_MAX+1],
			link_target[PATH_MAX+1],
			link_source[PATH_MAX+1],
			*path;
	unsigned int	length_path,
			length_zipdatadir,
			in_cd_dir;
};

/* passing this around is a lot easier than passing
 * a lot of other shit around */
typedef struct {
	//struct USERINFO		**ui;
	//struct GROUPINFO	**gi;
	struct USERINFO		ui[30];
	struct GROUPINFO	gi[30];
	struct VARS		v;
	struct LOCATIONS	l;
	struct MULTIMEDIA	m;
} GLOBAL; /* reconsider this name */

/* sfv_version - must be > 5. Should not be any need to add a version
 * for racedata - if either sfv_data or racedata changes, they both
 * should be removed */
#define sfv_version	18

/* Store various message strings here */
typedef struct _msg {
	char *complete;
	char *update;
	char *race;
	char *sfv;
	char *newleader;
	char *halfway;
	char *error;
} MSG;

#endif
