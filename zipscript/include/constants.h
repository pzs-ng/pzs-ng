#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_

#define F_IGNORED			254
#define F_BAD				255
#define F_NFO				253
#define F_DELETED			0
#define F_CHECKED			1
#define F_NOTCHECKED			2

#define TRUE				1
#define FALSE				0

#define DISABLED			NULL

#define FILE_MAX			256
#define MAXIMUM_FILES_IN_RELEASE	1024

#ifndef O_SYNC
#define O_SYNC O_FSYNC
#endif

#define video_announce_norace_complete_type		"COMPLETE_VIDEO"
#define zip_announce_norace_complete_type		"COMPLETE_ZIP"
#define rar_announce_norace_complete_type		"COMPLETE_RAR"
#define other_announce_norace_complete_type		"COMPLETE_OTHER"
#define audio_announce_norace_complete_type		"COMPLETE_AUDIO"
#define audio_vbr_announce_norace_complete_type		"COMPLETE_AUDIO_VBR"
#define audio_cbr_announce_norace_complete_type		"COMPLETE_AUDIO_CBR"
#define general_announce_norace_complete_type		"COMPLETE"
#define video_announce_race_complete_type		"COMPLETE_RACE_VIDEO"
#define video_announce_one_race_complete_type		"COMPLETE_STAT_RACE_VIDEO"
#define zip_announce_race_complete_type			"COMPLETE_RACE_ZIP"
#define zip_announce_one_race_complete_type		"COMPLETE_STAT_RACE_ZIP"
#define rar_announce_race_complete_type			"COMPLETE_RACE_RAR"
#define rar_announce_one_race_complete_type		"COMPLETE_STAT_RACE_RAR"
#define other_announce_race_complete_type		"COMPLETE_RACE_OTHER"
#define other_announce_one_race_complete_type		"COMPLETE_STAT_RACE_OTHER"
#define audio_announce_race_complete_type		"COMPLETE_RACE_AUDIO"
#define audio_announce_one_race_complete_type		"COMPLETE_STAT_RACE_AUDIO"
#define audio_vbr_announce_race_complete_type		"COMPLETE_RACE_AUDIO_VBR"
#define audio_vbr_announce_one_race_complete_type	"COMPLETE_STAT_RACE_AUDIO_VBR"
#define audio_cbr_announce_race_complete_type		"COMPLETE_RACE_AUDIO_CBR"
#define audio_cbr_announce_one_race_complete_type	"COMPLETE_STAT_RACE_AUDIO_CBR"
#define general_announce_race_complete_type		"COMPLETE_RACE"
#define general_announce_one_race_complete_type		"COMPLETE_STAT_RACE"
#define audio_announce_vbr_update_type			"UPDATE_VBR"
#define audio_announce_cbr_update_type			"UPDATE_CBR"
#define rar_announce_update_type			"UPDATE_RAR"
#define zip_announce_update_type			"UPDATE_ZIP"
#define other_announce_update_type			"UPDATE_OTHER"
#define video_announce_update_type			"UPDATE_VIDEO"
#define general_announce_update_type			"UPDATE"
#define video_announce_race_type			"RACE_VIDEO"
#define zip_announce_race_type				"RACE_ZIP"
#define rar_announce_race_type				"RACE_RAR"
#define other_announce_race_type			"RACE_OTHER"
#define audio_announce_race_type			"RACE_AUDIO"
#define general_announce_race_type			"RACE"
#define video_announce_newleader_type			"NEWLEADER_VIDEO"
#define zip_announce_newleader_type			"NEWLEADER_ZIP"
#define rar_announce_newleader_type			"NEWLEADER_RAR"
#define other_announce_newleader_type			"NEWLEADER_OTHER"
#define audio_announce_newleader_type			"NEWLEADER_AUDIO"
#define general_announce_newleader_type			"NEWLEADER"
#define video_announce_norace_halfway_type		"HALFWAY_NORACE_VIDEO"
#define zip_announce_norace_halfway_type		"HALFWAY_NORACE_ZIP"
#define rar_announce_norace_halfway_type		"HALFWAY_NORACE_RAR"
#define other_announce_norace_halfway_type		"HALFWAY_NORACE_OTHER"
#define audio_announce_norace_halfway_type		"HALFWAY_NORACE_AUDIO"
#define general_announce_norace_halfway_type		"HALFWAY_NORACE"
#define video_announce_race_halfway_type		"HALFWAY_RACE_VIDEO"
#define zip_announce_race_halfway_type			"HALFWAY_RACE_ZIP"
#define rar_announce_race_halfway_type			"HALFWAY_RACE_RAR"
#define other_announce_race_halfway_type		"HALFWAY_RACE_OTHER"
#define audio_announce_race_halfway_type		"HALFWAY_RACE_AUDIO"
#define general_announce_race_halfway_type		"HALFWAY_RACE"
#define video_announce_sfv_type				"SFV_VIDEO"
#define rar_announce_sfv_type				"SFV_RAR"
#define other_announce_sfv_type				"SFV_OTHER"
#define audio_announce_sfv_type				"SFV_AUDIO"
#define general_announce_sfv_type			"SFV"
#define general_doublesfv_type				"DOUBLESFV"
#define general_badgenre_type				"BADGENRE"
#define general_badyear_type				"BADYEAR"
#define general_badbitrate_type				"BADBITRATE"
#define general_incomplete_type				"INCOMPLETE"
#define stat_users_head_type				"STAT_USERS_HEAD"
#define stat_users_type					"STAT_USERS"
#define stat_groups_head_type				"STAT_GROUPS_HEAD"
#define stat_groups_type				"STAT_GROUPS"
#define stat_post_type					"STAT_POST"
//#define bad_file_msg_type				"BAD"
#define bad_file_crc_type				"BAD_FILE_CRC"
#define bad_file_0size_type				"BAD_FILE_0SIZE"
#define bad_file_zip_type				"BAD_FILE_ZIP"
#define bad_file_wrongdir_type				"BAD_FILE_WRONGDIR"
#define bad_file_sfv_type				"BAD_FILE_SFV"
#define bad_file_nosfv_type				"BAD_FILE_NOSFV"
#define bad_file_genre_type				"BAD_FILE_GENRE"
#define bad_file_year_type				"BAD_FILE_YEAR"
#define bad_file_bitrate_type				"BAD_FILE_BITRATE"
#define bad_file_disallowed_type			"BAD_FILE_DISALLOWED"
#define bad_file_nfo_type				"BAD_FILE_DUPENFO"


#define deny_double_msg		"{%U} {%G} {%r} {%n}"
#define audio_cbr_warn_msg	"{%U} {%G} {%X}"
#define audio_year_warn_msg	"{%U} {%G} {%Y}"
#define audio_genre_warn_msg	"{%U} {%G} {%w}"

#define pre_stats		"dummy_pre_stats"
#define between_stats		"dummy_between_stats"
#define post_stats		"{%u} {%g} {%F} {%.1m} {%d} {%.0A} {%.0a} %C0 %c0 %l %L"
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
#define fastestfile		"{%u} {%g} {%.3F}"	/* Put in %l cookie */
#define slowestfile		"{%u} {%g} {%.3S}"	/* Put in %L cookie */
#ifndef racersplit
#define racersplit		""	/* Put between racers in the %R cookie */
#endif
#ifndef racersplit_prior
#define racersplit_prior	""
#endif
#ifndef racersmsg
#define racersmsg		"{%u} {%g}"	/* Put in %R cookie */
#endif
#define audio_vbr		"{%w} {%Y} {%X} {%z} {%Q} {VBR} {%I} {%i} {%x} {%W} {%y} {%h} {%q}"	/* Put in %j cookie */
#define audio_cbr		"{%w} {%Y} {%X} {%z} {%Q} {CBR} {%x} {%W} {%y} {%h} {%q}"		/* Put in %j cookie */

/*
 * Audio files - currently used only for mp3 (mpeg2 layer3), but mpeg1 &
 * mpeg2.5 + layer1 & 2 can be added if needed
 */
#if ( disable_audio_race == FALSE )
#define audio_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%K} {%B} {%$}"
#else
#define audio_race		DISABLED
#endif
#if ( disable_audio_sfv == FALSE )
#define audio_sfv		"{%r} {%f} {%U} {%G} {%n} {%K}"
#else
#define audio_sfv		DISABLED
#endif
#if ( disable_audio_update == FALSE )
#define audio_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%r} {%K} %j {%$}"
#else
#define audio_update		DISABLED
#endif
#if ( disable_audio_halfway == FALSE )
#define audio_halfway		"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%B} {%$}"
#else
#define audio_halfway		DISABLED
#endif
#if ( disable_audio_newleader == FALSE )
#define audio_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%K} {%R} {%$}"
#else
#define audio_newleader		DISABLED
#endif
#if ( disable_audio_complete == FALSE )
#define audio_complete		"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %j %l %L %C0 %c0 {%n} {%B} {%t} {%T}"
#else
#define audio_complete		DISABLED
#endif
#if ( disable_audio_norace_halfway == FALSE )
#define audio_norace_halfway	"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%$}"
#else
#define audio_norace_halfway	DISABLED
#endif
#if ( disable_audio_norace_complete == FALSE )
#define audio_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} %j {%n} {%K} %t %T"
#else
#define audio_norace_complete	DISABLED
#endif

/* Video files - supports mpeg1/mpeg2 and various avi formats */
#if ( disable_video_race == FALSE )
#define video_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%K} {%B} {%$}"
#else
#define video_race		DISABLED
#endif
#if ( disable_video_sfv == FALSE )
#define video_sfv		"{%r} {%f} {%U} {%G} {%n} {%K}"
#else
#define video_sfv		DISABLED
#endif
#if ( disable_video_update == FALSE )
#define video_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%r} {%K} {%$}"
#else
#define video_update		DISABLED
#endif
#if ( disable_video_halfway == FALSE )
#define video_halfway		"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%B} {%$}"
#else
#define video_halfway		DISABLED
#endif
#if ( disable_video_newleader == FALSE )
#define video_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%K} {%R} {%$}"
#else
#define video_newleader		DISABLED
#endif
#if ( disable_video_complete == FALSE )
#define video_complete		"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%B} {%t} {%T}"
#else
#define video_complete		DISABLED
#endif
#if ( disable_video_norace_halfway == FALSE )
#define video_norace_halfway	"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%$}"
#else
#define video_norace_halfway	DISABLED
#endif
#if ( disable_video_norace_complete == FALSE )
#define video_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} {%n} {%K} %t %T"
#else
#define video_norace_complete	DISABLED
#endif

/* RAR - allows usage of compression mode cookie (%J) for rarred releases */
#if ( disable_rar_race == FALSE )
#define rar_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%K} {%B} {%J} {%$}"
#else
#define rar_race		DISABLED
#endif
#if ( disable_rar_sfv == FALSE )
#define rar_sfv			"{%r} {%f} {%U} {%G} {%n} {%K}"
#else
#define rar_sfv			DISABLED
#endif
#if ( disable_rar_update == FALSE )
#define rar_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%r} {%K} {%J} {%$}"
#else
#define rar_update		DISABLED
#endif
#if ( disable_rar_halfway == FALSE )
#define rar_halfway		"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%B} {%J} {%$}"
#else
#define rar_halfway		DISABLED
#endif
#if ( disable_rar_newleader == FALSE )
#define rar_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%K} {%R} {%J} {%$}"
#else
#define rar_newleader		DISABLED
#endif
#if ( disable_rar_complete == FALSE )
#define rar_complete		"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%B} {%J} {%t} {%T}"
#else
#define rar_complete		DISABLED
#endif
#if ( disable_rar_norace_halfway == FALSE )
#define rar_norace_halfway	"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%J} {%$}"
#else
#define rar_norace_halfway	DISABLED
#endif
#if ( disable_rar_norace_complete == FALSE )
#define rar_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} {%n} {%K} {%J} %t %T"
#else
#define rar_norace_complete	DISABLED
#endif

/* Rest of the stuff inside sfv */
#if ( disable_other_race == FALSE )
#define other_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%K} {%B} {%$}"
#else
#define other_race		DISABLED
#endif
#if ( disable_other_sfv == FALSE )
#define other_sfv		"{%r} {%f} {%U} {%G} {%n} {%K}"
#else
#define other_sfv		DISABLED
#endif
#if ( disable_other_update == FALSE )
#define other_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%r} {%K} {%$}"
#else
#define other_update		DISABLED
#endif
#if ( disable_other_halfway == FALSE )
#define other_halfway		"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%B} {%$}"
#else
#define other_halfway		DISABLED
#endif
#if ( disable_other_newleader == FALSE )
#define other_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%K} {%R} {%$}"
#else
#define other_newleader		DISABLED
#endif
#if ( disable_other_complete == FALSE )
#define other_complete		"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%B} {%t} {%T}"
#else
#define other_complete		DISABLED
#endif
#if ( disable_other_norace_halfway == FALSE )
#define other_norace_halfway	"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%$}"
#else
#define other_norace_halfway	DISABLED
#endif
#if ( disable_other_norace_complete == FALSE )
#define other_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} {%n} {%K} %t %T"
#else
#define other_norace_complete	DISABLED
#endif

/* Zipped files */
#if ( disable_zip_race == FALSE )
#define zip_race		"{%U} {%G} {%R} {%r} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%K} {%B} {%$}"
#else
#define zip_race		DISABLED
#endif
#if ( disable_zip_update == FALSE )
#define zip_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%r} {%K} {%$}"
#else
#define zip_update		DISABLED
#endif
#if ( disable_zip_halfway == FALSE )
#define zip_halfway		"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%B} {%$}"
#else
#define zip_halfway		DISABLED
#endif
#if ( disable_zip_newleader == FALSE )
#define zip_newleader		"{%U} {%G} {%.0S} {%d} {%F} {%.1p} {%.0m} {%r} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%K} {%R} {%$}"
#else
#define zip_newleader		DISABLED
#endif
#if ( disable_zip_complete == FALSE )
#define zip_complete		"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%B} {%t} {%T}"
#else
#define zip_complete		DISABLED
#endif
#if ( disable_zip_norace_halfway == FALSE )
#define zip_norace_halfway	"{%r} %C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%$}"
#else
#define zip_norace_halfway	DISABLED
#endif
#if ( disable_zip_norace_complete == FALSE )
#define zip_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} {%n} {%K} %t %T"
#else
#define zip_norace_complete	DISABLED
#endif

/* Output of racestats binary */
#define stats_line		"{%r} {%F} {%f} {%u} {%g} %C0 %c0"

/* Output of postdel binary */
#define incompletemsg		"{%U} {%G} {%r}"

#define bad_file_msg		"{%r} {%U} {%G} {%f}"

enum ReleaseTypes {
	RTYPE_NULL = 0,
	RTYPE_RAR = 1,
	RTYPE_OTHER = 2,
	RTYPE_AUDIO = 3,
	RTYPE_VIDEO = 4,
	RTYPE_INVALID,
};

#endif
