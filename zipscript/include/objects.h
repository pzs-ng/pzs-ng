#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <sys/time.h>
#include "constants.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../conf/zsconfig.h"

struct USERINFO {
  char				name[24];	/* Username */
  off_t				bytes;		/* Bytes uploaded */
  double			speed;		/* Time spent uploading (secs) */
  unsigned char			files;		/* Files uploaded */
  unsigned char			pos;		/* User position */
  unsigned char			group;		/* Primary group number */

  unsigned			dayup;		/* Day up stats */
  unsigned			wkup;		/* Week */
  unsigned			monthup;	/* Month */
  unsigned			allup;		/* Alltime */
};



struct GROUPINFO {
  char				name[24];	/* Groupname */
  off_t				bytes;		/* Bytes uploaded */
  double			speed;		/* Time spent uploading (secs) */
  unsigned char			files;		/* Files uploaded */
  unsigned char			pos;		/* Group position */
  unsigned char			users;		/* Users in group; */
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
  unsigned char			*fps;
};

struct current_user {
  char				name[24];
  char				group[24];
  char				tagline[255];
  short				pos;
};


struct current_file {
  char				*name;
  unsigned int			speed;
  off_t				size;
  char				compression_method;
};


struct race_total {
  unsigned int			start_time;
  unsigned int			stop_time;
  unsigned char			users;
  unsigned char			groups;
  unsigned char			files;
  short				files_missing;
  unsigned char			files_bad;
  unsigned char			nfo_present;
  double				speed;
  off_t				size;
  off_t				bad_size;
};


struct misc {
  char				old_leader[24];
  char				*release_name;
  char				racer_list[1024];
  char				top_messages[2][1024];
  char				error_msg[80];
  char				progress_bar[15];
  short				release_type;
  unsigned char			write_log;
  short				fastest_user[2];
  short				slowest_user[2];
};



struct VARS {
  struct current_user		user;
  struct current_file		file;
  struct race_total		total;
  struct misc			misc;
  struct audio			audio;
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

#endif
