#include <sys/time.h>
#include "constants.h"

struct USERINFO {
 char				name[24];	/* Username */
 unsigned long long	bytes;		/* Bytes uploaded */
 double				speed;		/* Time spent uploading (secs) */
 unsigned char		files;		/* Files uploaded */
 unsigned char		pos;		/* User position */
 unsigned char		group;		/* Primary group number */

 unsigned			dayup;		/* Day up stats */
 unsigned			wkup;		/* Week */
 unsigned			monthup;	/* Month */
 unsigned			allup;		/* Alltime */
};



struct GROUPINFO {
 char				name[24];	/* Groupname */
 unsigned long long	bytes;		/* Bytes uploaded */
 double				speed;		/* Time spent uploading (secs) */
 unsigned char		files;		/* Files uploaded */
 unsigned char		pos;		/* Group position */
 unsigned char		users;		/* Users in group; */
};



struct ONLINE {
 char				tagline[64];	/* The users tagline */
 char   			username[24];	/* The username of the user */
 char				status[256];	/* The status of the user, idle, RETR, etc */
 char				host[256];	/* The host the user is comming from (with ident) */
 char				currentdir[256];/* The users current dir (fullpath) */
 long				groupid;	/* The groupid of the users primary group */
 time_t				login_time;	/* The login time since the epoch (man 2 time) */
 struct timeval			tstart;		/* Replacement for last_update. */
 unsigned long			bytes_xfer;	/* Bytes transferred this far. */
 pid_t				procid;		/* The processor id of the process */
};



struct audio {
 char				id3_artist[31];
 char				id3_title[31];
 char				id3_album[31];
 char				id3_year[5];
 char				bitrate[5];
 char				samplingrate[6];
 char				*id3_genre;
 char				*layer;
 char				*codec;
 char				*channelmode;
 char				vbr_version_string[10];
 char				vbr_preset[15];
 int			 	is_vbr;
};



struct video {
 int				height;
 int				width;
 unsigned char		*fps;
};

struct current_user {
 char				*name;
 char				group[24];
 char				*tagline;
 short				pos;
};


struct current_file {
 char				*name;
 long				speed;
 unsigned long long	size;
 char				compression_method;
};


struct race_total {
 unsigned char		users;
 unsigned char		groups;
 unsigned char		files;
 short				files_missing;
 unsigned char		files_bad;
 unsigned char		nfo_present;
 double				speed;
 unsigned long long	size;
 unsigned long long	bad_size;
};


struct misc {
 char				old_leader[24];
 char				*release_name;
 char				racer_list[1024];
 char				top_messages[2][1024];
 char				error_msg[80];
 char				progress_bar[15];
 short				release_type;
 unsigned char		write_log;
 short				fastest_user[2];
 short				slowest_user[2];
};



struct VARS {
 struct	timeval			transfer_start;
 struct timeval			transfer_stop;
 struct	current_user	user;
 struct current_file	file;
 struct race_total		total;
 struct misc			misc;
 struct	audio			audio;
 struct video			video;
};


struct LOCATIONS {
 char				*leader;
 char				*incomplete;
 char				*sfv;
 char				*race;
 char				*link_target;
 char				*link_source;
 char				*path;
 unsigned			length_path;
 unsigned			length_zipdatadir;
};


#define F_IGNORED		254
#define F_BAD			255
#define F_NFO			253
#define F_DELETED		0
#define F_CHECKED		1
#define F_NOTCHECKED	2

#define TRUE			1
#define FALSE			0

#define DISABLED		NULL

#define FILE_MAX		256

#ifndef O_SYNC
# define O_SYNC O_FSYNC
#endif
#ifndef alloca
# define m_alloc(x) malloc(x)
# define m_free(p) free(p)
#else
# define m_alloc(x) alloca(x)
# define m_free(p)
#endif
