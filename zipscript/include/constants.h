#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_

#define VERSION		"0.7.3 BETA4 (PRE)"

#define F_IGNORED		254
#define F_BAD			255
#define F_NFO			253
#define F_DELETED		0
#define F_CHECKED		1
#define F_NOTCHECKED		2

#define TRUE			1
#define FALSE			0

#define DISABLED		NULL

#define FILE_MAX		256

#ifndef O_SYNC
# define O_SYNC O_FSYNC
#endif

#define video_announce_norace_complete_type	"COMPLETE_VIDEO"
#define zip_announce_norace_complete_type	"COMPLETE_ZIP"
#define rar_announce_norace_complete_type	"COMPLETE_RAR"
#define other_announce_norace_complete_type	"COMPLETE_OTHER"
#define audio_announce_norace_complete_type	"COMPLETE_AUDIO"
#define general_announce_norace_complete_type	"COMPLETE"
#define video_announce_race_complete_type	"COMPLETE_RACE_VIDEO"
#define zip_announce_race_complete_type		"COMPLETE_RACE_ZIP"
#define rar_announce_race_complete_type		"COMPLETE_RACE_RAR"
#define other_announce_race_complete_type	"COMPLETE_RACE_OTHER"
#define audio_announce_race_complete_type	"COMPLETE_RACE_AUDIO"
#define general_announce_race_complete_type	"COMPLETE_RACE"
#define audio_announce_vbr_update_type		"UPDATE_VBR"
#define audio_announce_cbr_update_type		"UPDATE_CBR"
#define rar_announce_update_type		"UPDATE_RAR"
#define zip_announce_update_type		"UPDATE_ZIP"
#define other_announce_update_type		"UPDATE_OTHER"
#define video_announce_update_type		"UPDATE_VIDEO"
#define general_announce_update_type		"UPDATE"
#define general_race_type			"RACE"
#define general_newleader_type			"NEWLEADER"
#define general_halfway_type			"HALFWAY"
#define norace_halfway_type			"HALFWAY_NORACE"
#define race_halfway_type			"HALFWAY_RACE"
#define general_doublesfv_type			"DOUBLESFV"
#define general_sfv_type			"SFV"
#define general_badgenre_type			"BADGENRE"
#define general_badyear_type			"BADYEAR"
#define general_badbitrate_type			"BADBITRATE"
#define general_incomplete_type			"INCOMPLETE"
#define stat_users_head_type			"STAT_USERS_HEAD"
#define stat_users_type				"STAT_USERS"
#define stat_groups_head_type			"STAT_GROUPS_HEAD"
#define stat_groups_type			"STAT_GROUPS"
#define stat_post_type				"STAT_POST"

#define deny_double_msg		"{%U} {%G} {%r} {%n}"
#define audio_cbr_warn_msg	"{%U} {%G} {%X}"
#define audio_year_warn_msg	"{%U} {%G} {%Y}"
#define audio_genre_warn_msg	"{%U} {%G} {%w}"

#define pre_stats		"dummy_pre_stats"
#define between_stats		"dummy_between_stats"
#define post_stats		"{%u} {%g} {%F} {%.1m} {%d} {%.0A} %C0 %c0 %L %l"
#define winner			"dummy_winner"
#define loser			"dummy_loser"

#ifdef _WITH_NOFORMAT
 #define user_top		"{%n} {%u} {%g} {%.1m} {%f} {%.1p} {%.0s} {%D} {%W} {%M} {%A}"
 #define user_info		"{%u} {%g} {%.1m} {%f} {%.1p} {%.0s}"
 #define group_top		"{%n} {%g} {%.1m} {%f} {%.1p} {%.0s}"
 #define group_info		"{%g} {%.1m} {%f} {%.1p} {%.0s}"
#else
 #define user_top		"{%n} {%-9u} {%-9g} {%5.1m} {%2f} {%5.1p} {%5.0s} {%2D} {%2W} {%2M} {%2A}"
 #define user_info		"{%u} {%g} {%.1m} {%f} {%.1p} {%.0s}"
 #define group_top		"{%n} {%-9g} {%.1m} {%f} {%.1p} {%5.0s}"
 #define group_info		"{%g} {%.1m} {%-2f} {%.1p} {%.0s}"
#endif

/* Special reformat cookies */
#define fastestfile		"{%u} {%g} {%.3F}" /* Put in %l cookie */
#define slowestfile		"{%u} {%g} {%.3S}" /* Put in %L cookie */
#define racersplit		" " /* Put between racers in the %R cookie - !!MUST BE ONLY ONE (1) CHAR!! */
#define racersmsg		"%u/%g" /* Put in %R cookie */
#define audio_vbr		"{%X} {%z} {%Q} {VBR} {%I}" /* Put in %j cookie */
#define audio_cbr		"{%X} {%z} {%Q} {CBR}"    /* Put in %j cookie */

/* Audio files - currently used only for mp3 (mpeg2 layer3), but mpeg1 & mpeg2.5 + layer1 & 2 can be added if needed */
#define audio_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M}"
#define audio_sfv		"{%r} {%f} {%U} {%G} {%n}"
#define audio_update		"{%U} {%G} {%f} {%.0S} {%w} {%Y} %j {%n} {%r}"
#define audio_halfway		"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define audio_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%n}"
#define audio_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%t} {%T}"
#define audio_norace_halfway	"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define audio_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%U} {%G} {%n}"

/* Video files - supports mpeg1/mpeg2 and various avi formats */
#define video_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M}"
#define video_sfv		"{%r} {%f} {%U} {%G} {%n}"
#define video_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%r}"
#define video_halfway		"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define video_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%n} {%u} {%g} {%f} {%M}"
#define video_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%t} {%T}"
#define video_norace_halfway	"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define video_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%U} {%G} {%n}"

/* RAR - allows usage of compression mode cookie (%Z) for rarred releases */
#define rar_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M}"
#define rar_sfv			"{%r} {%f} {%U} {%G} {%n}"
#define rar_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%r}"
#define rar_halfway		"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define rar_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%n} {%u} {%g} {%f} {%M}"
#define rar_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%t} {%T}"
#define rar_norace_halfway	"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define rar_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%U} {%G} {%n}"

/* Rest of the stuff inside sfv */
#define other_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M}"
#define other_sfv		"{%r} {%f} {%U} {%G} {%n}"
#define other_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%r}"
#define other_halfway		"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define other_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%n} {%u} {%g} {%f} {%M}"
#define other_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%t} {%T}"
#define other_norace_halfway	"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define other_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%U} {%G} {%n}"

/* Zipped files */
#define zip_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M}"
#define zip_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%r}"
#define zip_halfway		"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define zip_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%u} {%g} {%f} {%M}"
#define zip_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%t} {%T}"
#define zip_norace_halfway	"{%r} %C0 %c0 {%.0m} {%f} {%.1p} {%.0A} {%M} {%n} {%u} {%g}"
#define zip_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%U} {%G} {%n}"

/* Get user day/week/month/allup position */
#define get_user_stats		TRUE

#define show_user_info		TRUE
#define show_group_info		TRUE

/* Output of racestats binary */
#define stats_line		"{%r} {%F} {%f} {%u} {%g} %C0 %c0"

/* Output of postdel binary */
#define incompletemsg		"{%U} {%G} {%r}"

enum ReleaseTypes {
	RTYPE_NULL = 0,
	RTYPE_RAR = 1,
	RTYPE_OTHER = 2,
	RTYPE_AUDIO = 3,
	RTYPE_VIDEO = 4,
	RTYPE_INVALID,
};

#endif

