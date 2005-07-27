#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_

#include "zsconfig.h"
#include "zsconfig.defaults.h"

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

#define PROGTYPE_ZIPSCRIPT		2
#define PROGTYPE_POSTDEL		4
#define PROGTYPE_CLEANUP		8
#define PROGTYPE_DATACLEANER		16
#define PROGTYPE_RESCAN			32

#define video_announce_norace_complete_type		"COMPLETE_VIDEO"
#define zip_announce_norace_complete_type		"COMPLETE_ZIP"
#define rar_announce_norace_complete_type		"COMPLETE_RAR"
#define other_announce_norace_complete_type		"COMPLETE_OTHER"
#define audio_announce_norace_complete_type		"COMPLETE_AUDIO"
#define audio_vbr_announce_norace_complete_type		"COMPLETE_AUDIO_VBR"
#define audio_cbr_announce_norace_complete_type		"COMPLETE_AUDIO_CBR"
#define general_announce_norace_complete_type		"COMPLETE"
#define video_announce_race_complete_type		"COMPLETE_RACE_VIDEO"
#define zip_announce_race_complete_type			"COMPLETE_RACE_ZIP"
#define rar_announce_race_complete_type			"COMPLETE_RACE_RAR"
#define other_announce_race_complete_type		"COMPLETE_RACE_OTHER"
#define audio_announce_race_complete_type		"COMPLETE_RACE_AUDIO"
#define audio_vbr_announce_race_complete_type		"COMPLETE_RACE_AUDIO_VBR"
#define audio_cbr_announce_race_complete_type		"COMPLETE_RACE_AUDIO_CBR"
#define general_announce_race_complete_type		"COMPLETE_RACE"
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
#define video_announce_norace_halfway_type		"HALFWAY_VIDEO"
#define zip_announce_norace_halfway_type		"HALFWAY_ZIP"
#define rar_announce_norace_halfway_type		"HALFWAY_RAR"
#define other_announce_norace_halfway_type		"HALFWAY_OTHER"
#define audio_announce_norace_halfway_type		"HALFWAY_AUDIO"
#define general_announce_norace_halfway_type		"HALFWAY"
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
#define general_badpreset_type				"BADPRESET"
#define general_incomplete_type				"INCOMPLETE"
#define bad_file_crc_type				"BAD_FILE_CRC"
#define bad_file_0size_type				"BAD_FILE_0SIZE"
#define bad_file_zip_type				"BAD_FILE_ZIP"
#define bad_file_wrongdir_type				"BAD_FILE_WRONGDIR"
#define bad_file_sfv_type				"BAD_FILE_SFV"
#define bad_file_nosfv_type				"BAD_FILE_NOSFV"
#define bad_file_genre_type				"BAD_FILE_GENRE"
#define bad_file_year_type				"BAD_FILE_YEAR"
#define bad_file_bitrate_type				"BAD_FILE_BITRATE"
#define bad_file_vbr_preset_type			"BAD_FILE_VBR_PRESET"
#define bad_file_disallowed_type			"BAD_FILE_DISALLOWED"
#define bad_file_nfo_type				"BAD_FILE_DUPENFO"
#define bad_file_password_type				"BAD_FILE_PASSWORD"
#define bad_file_nfodenied_type				"BAD_FILE_NFODENIED"
#define speed_type					"SPEEDTEST"

#define deny_double_msg			"{%U} {%G} {%n}"
#define audio_cbr_warn_msg		"{%U} {%G} {%X}"
#define audio_year_warn_msg		"{%U} {%G} {%Y}"
#define audio_genre_warn_msg		"{%U} {%G} {%w}"
#define audio_vbr_preset_warn_msg	"{%U} {%G} {%I}"

#define post_stats		"{%u} {%g} {%F} {%.1m} {%d} {%.0A} {%.0a} %C0 %c0 %l %L"
#define winner			"dummy_winner"
#define loser			"dummy_loser"

#ifdef _WITH_NOFORMAT
#define user_top		"{%n} {%u} {%g} {%K} {%.1m} {%f} {%.1p} {%.0s} {%D} {%W} {%M} {%A}"
#define user_info		"{%u} {%g} {%K} {%.1m} {%f} {%.1p} {%.0s}"
#define group_top		"{%n} {%g} {%.1m} {%f} {%.1p} {%.0s}"
#define group_info		"{%g} {%.1m} {%f} {%.1p} {%.0s}"
#else
#define user_top		"{%n} {%-9u} {%-9g} {%-15K} {%5.1m} {%2f} {%5.1p} {%5.0s} {%2D} {%2W} {%2M} {%2A}"
#define user_info		"{%u} {%g} {%-15K} {%.1m} {%f} {%.1p} {%.0s}"
#define group_top		"{%n} {%-9g} {%.1m} {%f} {%.1p} {%5.0s}"
#define group_info		"{%g} {%.1m} {%-2f} {%.1p} {%.0s}"
#endif

/* Special reformat cookies */
#define fastestfile		"{%u} {%g} {%K} {%.3F}" /* Put in %l cookie */
#define slowestfile		"{%u} {%g} {%K} {%.3S}" /* Put in %L cookie */
#ifndef racersplit
#define racersplit		"" /* Put between racers in the %R cookie */
#endif
#ifndef racersplit_prior
#define racersplit_prior	""
#endif
#ifndef racersmsg
#define racersmsg		"{%u} {%g} {%K}" /* Put in %R cookie */
#endif
#define audio_vbr		"{%w} {%Y} {%X} {%z} {%Q} {VBR} {%I} {%i} {%x} {%W} {%y} {%h} {%q}" /* Put in %j cookie */
#define audio_cbr		"{%w} {%Y} {%X} {%z} {%Q} {CBR} {%x} {%W} {%y} {%h} {%q}" /* Put in %j cookie */

/* Complete cookies (race) */
#define	audio_complete		"{%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %j %l %L %C0 %c0 {%n} {%B} {%t} {%T}"
#define	other_complete		"{%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%B} {%t} {%T}"
#define	rar_complete		"{%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%B} {%J} {%t} {%T}"
#define	video_complete		"{%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%B} {%t} {%T}"
#define	zip_complete		"{%.0m} {%f} {%.0A} {%.0a} {%d} {%u} {%g} %l %L %C0 %c0 {%n} {%B} {%t} {%T}"

/* Complete cookies (norace) */
#define	audio_norace_complete	"{%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} {%K} %j {%n} %t %T"
#define	other_norace_complete	"{%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} {%K} {%n} %t %T"
#define	rar_norace_complete	"{%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} {%K} {%n} {%J} %t %T"
#define	video_norace_complete	"{%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} {%K} {%n} %t %T"
#define	zip_norace_complete	"{%.0m} {%f} {%.0A} {%.0a} {%d} {%U} {%G} {%K} {%n} %t %T"

/* Halfway cookies (race) */
#define	audio_halfway		"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%B} {%$}"
#define	other_halfway		"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%B} {%$}"
#define	rar_halfway		"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%B} {%J} {%$}"
#define	video_halfway		"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%B} {%$}"
#define	zip_halfway		"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%B} {%$}"

/* Halfway cookies (norace) */
#define	audio_norace_halfway	"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%$}"
#define	other_norace_halfway	"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%$}"
#define	rar_norace_halfway	"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%J} {%$}"
#define	video_norace_halfway	"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%$}"
#define	zip_norace_halfway	"%C0 %c0 {%.0m} {%F} {%.1p} {%.0A} {%.0a} {%M} {%n} {%u} {%g} {%U} {%G} {%f} {%K} {%$}"

/* Update cookies */
#define	audio_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%K} %j {%$}"
#define	other_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%K} {%$}"
#define	rar_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%K} {%J} {%$}"
#define	video_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%K} {%$}"
#define	zip_update		"{%U} {%G} {%f} {%.0S} {%.1e} {%n} {%K} {%$}"

/* New leader cookies */
#define	audio_newleader		"{%U} {%G} {%K} {%.0S} {%d} {%F} {%.1p} {%.0m} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%R} {%$}"
#define	other_newleader		"{%U} {%G} {%K} {%.0S} {%d} {%F} {%.1p} {%.0m} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%R} {%$}"
#define	rar_newleader		"{%U} {%G} {%K} {%.0S} {%d} {%F} {%.1p} {%.0m} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%R} {%J} {%$}"
#define	video_newleader		"{%U} {%G} {%K} {%.0S} {%d} {%F} {%.1p} {%.0m} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%R} {%$}"
#define	zip_newleader		"{%U} {%G} {%K} {%.0S} {%d} {%F} {%.1p} {%.0m} {%n} {%u} {%g} {%f} {%M} %C0 %c0 {%R} {%$}"

/* Race	cookies	*/
#define	audio_race		"{%U} {%G} {%K} {%R} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%B} {%$}"
#define	other_race		"{%U} {%G} {%K} {%R} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%B} {%$}"
#define	rar_race		"{%U} {%G} {%K} {%R} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%B} {%J} {%$}"
#define	video_race		"{%U} {%G} {%K} {%R} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%B} {%$}"
#define	zip_race		"{%U} {%G} {%K} {%R} {%.0S} {%n} {%d} {%.1p} {%u} {%g} {%F} {%f} {%M} {%B} {%$}"

/* SFV cookies */
#define	audio_sfv		"{%f} {%U} {%G} {%n} {%K}"
#define	other_sfv		"{%f} {%U} {%G} {%n} {%K}"
#define	rar_sfv			"{%f} {%U} {%G} {%n} {%K}"
#define	video_sfv		"{%f} {%U} {%G} {%n} {%K}"

/* Speed test */
#define speed_announce		"{%U} {%G} {%K} {%.0S} {%.2/}"

/* Output of racestats binary */
#define stats_line		"{%F} {%f} {%u} {%g} %C0 %c0"

/* Output of postdel binary */
#define incompletemsg		"{%U} {%G}"

#define bad_file_msg		"{%U} {%G} {%n}"

enum ReleaseTypes {
	RTYPE_NULL = 0,
	RTYPE_RAR = 1,
	RTYPE_OTHER = 2,
	RTYPE_AUDIO = 3,
	RTYPE_VIDEO = 4,
	RTYPE_INVALID,
};

#endif
