#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_

#define F_IGNORED			254
#define F_BAD				255
#define F_NFO				253
#define F_MISSING			252
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

#define INCOMPLETE_NORMAL		3
#define INCOMPLETE_NFO			7
#define INCOMPLETE_SAMPLE		9
#define INCOMPLETE_SFV			15

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
#define general_resumesfv_type				"RESUMESFV"
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
#define bad_file_nonfo_type                             "BAD_FILE_NONFO"
#define bad_file_genre_type				"BAD_FILE_GENRE"
#define bad_file_year_type				"BAD_FILE_YEAR"
#define bad_file_bitrate_type				"BAD_FILE_BITRATE"
#define bad_file_disallowed_type			"BAD_FILE_DISALLOWED"
#define bad_file_nfo_type				"BAD_FILE_DUPENFO"
#define bad_file_zipnfo_type				"BAD_FILE_ZIPNFO"
#define bad_file_duperelease_type			"BAD_FILE_DUPERELEASE"
#define bad_file_speedtest_type				"ULTEST"
#define sample_announce_type				"SAMPLEDIR"

#define vbrnew						"vbr-new"
#define vbrold						"vbr-old"

/* This is just a shorthand to not have to repeat everything for each announce. */
#define video_info "\"width\": %D, \"height\": %E, \"fps\": %.3H, \"aspect\": %.2;, \"type_long\": \"%:\", \"type_short\": \"%,\", \"audio_freq\": %`, \"audio_channels\": %=, \"audio_name\": \"%>\", \"audio_codec\": \"%<\""

#define deny_double_msg		"{\"user\": \"%U\", \"group\": \"%G\", \"release\": \"%r\", \"filename\": \"%n\"}"
#define deny_resumesfv_msg	"{\"user\": \"%U\", \"group\": \"%G\", \"release\": \"%r\", \"filename\": \"%n\"}"
#define audio_cbr_warn_msg	"{\"user\": \"%U\", \"group\": \"%G\", \"bitrate\": %X}"
#define audio_year_warn_msg	"{\"user\": \"%U\", \"group\": \"%G\", \"year\": %Y}"
#define audio_genre_warn_msg	"{\"user\": \"%U\", \"group\": \"%G\", \"genre\": %w}"

#define sample_msg		"{\"user\": \"%U\", \"group\": \"%G\", " video_info "}"

#define pre_stats		"dummy_pre_stats"
#define between_stats		"dummy_between_stats"
#define post_stats		"{\"num_users\": \"%u\", \"num_groups\": \"%g\", \"num_files\": %F, \"mbytes\": %.1m, \"duration\": \"%d\", \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"slowest_user\": %l, \"fastest_user\": %L, \"winning_user\": %C0, \"winning_group\": %c0}"
#define winner			"dummy_winner"
#define loser			"dummy_loser"

// %T: %u_racer_position %u_racer_name %g_racer_name %u_racer_mbytes %u_racer_files %u_racer_percent %u_racer_avgspeed %u_racer_dayup %u_racer_wkup %u_racer_monthup %u_racer_allup
// %t: %g_racer_position %g_racer_name %g_racer_mbytes %g_racer_files %g_racer_percent %g_racer_avgspeed
#ifdef _WITH_NOFORMAT
#define user_top		"{\"position\": %n, \"user\": \"%u\", \"group\": \"%g\", \"mbytes\": %.1m, \"files\": %f, \"percent\": %.1p, \"speed\": %.0s, \"dayup\": %D, \"weekup\": %W, \"monthup\": %M, \"allup\": %A}" /* Put in %T cookie (convert_user) */
#define user_info		"{\"user\": \"%u\", \"group\": \"%g\", \"mbytes\": %.1m, \"files\": %f, \"percent\": %.1p, \"speed\": %.0s}" /* Put in %C[num]-cookie (convert_user) */
#define group_top		"{\"position\": %n, \"group\": \"%g\", \"mbytes\": %.1m, \"files\": %f, \"percent\": %.1p, \"speed\": %.0s}" /* Put in %t cookie (convert_group) */
#define group_info		"{\"group\": \"%g\", \"mbytes\": %.1m, \"files\": %f, \"percent\": %.1p, \"speed\": %.0s}" /* Put in %c[num]-cookie (convert_group) */
#else
#define user_top		"{\"position\": %n, \"user\": \"%-9u\", \"group\": \"%-9g\", \"mbytes\": %5.1m, \"files\": %2f, \"percent\": %5.1p, \"speed\": %5.0s, \"dayup\": %2D, \"weekup\": %2W, \"monthup\": %2M, \"allup\": %2A}" /* Put in %T cookie (convert_user) */
#define user_info		"{\"user\": \"%u\", \"group\": \"%g\", \"mbytes\": %.1m, \"files\": %f, \"percent\": %.1p, \"speed\": %.0s}" /* Put in %C[num]-cookie (convert_user) */
#define group_top		"{\"position\": %n, \"group\": \"%-9g\", \"mbytes\": %.1m, \"files\": %f, \"percent\": %.1p, \"speed\": %5.0s}" /* Put in %t cookie (convert_group) */
#define group_info		"{\"group\": \"%g\", \"mbytes\": %.1m, \"files\": %-2f, \"percent\": %.1p, \"speed\": %5.0s}"
#endif

/* Special reformat cookies */
#define fastestfile		"{\"user\": \"%u\", \"group\": \"%g\", \"speed\": \"%.2F\"}"	/* Put in %L cookie */
#define slowestfile		"{\"user\": \"%u\", \"group\": \"%g\", \"speed\": \"%.2S\"}"	/* Put in %l cookie */

#define racersplit_prefix	"" /* Put after the last racer in the %R & %T/t cookie */
#define racersplit_postfix	"" /* Put before the first racer in the %R & %T/t cookie */
#define racersplit		", "	/* Put between racers in the %R & %T/t cookie */
#define racersmsg		"{\"user\": \"%u\", \"group\": \"%g\"}"	/* Put in %R cookie */

#define audio_info              "\"genre\": \"%w\", \"year\": %Y, \"bitrate\": %X, \"sampling\": %z, \"mode\": \"%Q\", \"artist\": \"%x\", \"album\": \"%W\", \"title\": \"%y\", \"codec\": \"%h\", \"layer\": \"%q\"" /* Helper define only, not used in zipscript directly. */
#define audio_vbr		"{" audio_info ", \"audio\": \"VBR\", \"a_stream\": \"%I\", \"a_version\": \"%i\", \"vbr_oldnew\": \"%@\", \"vbr_quality\": %_, \"vbr_minimum_bitrate\": \"%/\", \"vbr_noiseshaping\": \"%\\\", \"vbr_steremode\": \"%(\", \"vbr_unwise\": \"%)\", \"vbr_source\": \"%|\"}"	/* Put in %j cookie */
#define audio_cbr		"{" audio_info "}"			/* Put in %j cookie */

/*
 * Audio files - currently used only for flac & mp3 (mpeg2 layer3),
 * but mpeg1 & mpeg2.5 + layer1 & 2 can be added if needed
 */
#if ( disable_audio_race == FALSE )
#define audio_race		"{\"user\": \"%U\", \"group\": \"%G\", \"competitors\": [%R], \"release\": \"%r\", \"speed\": %.0S, \"filename\": \"%n\", \"duration\": \"%d\", \"percent\": %.1p, \"num_users\": %u, \"num_groups\": %g, \"num_files\": %F, \"expected_files\": %f, \"missing_files\": %M, \"tagline\": \"%K\", \"racers\": [%B], \"eta\": \"%$\"}"
#else
#define audio_race		DISABLED
#endif
#if ( disable_audio_sfv == FALSE )
#define audio_sfv		"{\"release\": \"%r\", \"expected_files\": %f, \"user\": \"%U\", \"group\": \"%G\", \"filename\": \"%n\", \"tagline\": \"%K\"}"
#else
#define audio_sfv		DISABLED
#endif
#if ( disable_audio_update == FALSE )
#define audio_update		"{\"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"speed\": %.0S, \"estimated_size\": %.1e, \"filename\": \"%n\", \"release\": \"%r\", \"tagline\": \"%K\", \"audio_info\": %j, \"eta\": \"%$\"}"
#else
#define audio_update		DISABLED
#endif
#if ( disable_audio_halfway == FALSE )
#define audio_halfway		"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"racers\": [%B], \"eta\": \"%$\"}"
#else
#define audio_halfway		DISABLED
#endif
#if ( disable_audio_newleader == FALSE )
#define audio_newleader		"{\"user\": \"%U\", \"group\": \"%G\", \"speed\": %.0S, \"duration\": \"%d\", \"num_files\": %F, \"percent\": %.1p, \"total_mbytes\": %.0m, \"release\": \"%r\", \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"expected_files\": %f, \"missing_files\": %M, \"winning_user\": %C0, \"winning_group\": %c0, \"tagline\": \"%K\", \"competitors\": [%R], \"eta\": \"%$\"}"
#else
#define audio_newleader		DISABLED
#endif
#if ( disable_audio_complete == FALSE )
#define audio_complete		"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"num_users\": %u, \"num_groups\": %g, \"audio_info\": %j, \"slowest_user\": %l, \"fastest_user\": %L, \"winning_user\": %C0, \"winning_group\": %c0, \"filename\": \"%n\", \"racers\": [%B], \"top_groups\": [%t], \"top_users\": [%T]}"
#else
#define audio_complete		DISABLED
#endif
#if ( disable_audio_norace_halfway == FALSE )
#define audio_norace_halfway	"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"tagline\": \"%K\", \"eta\": \"%$\"}"
#else
#define audio_norace_halfway	DISABLED
#endif
#if ( disable_audio_norace_complete == FALSE )
#define audio_norace_complete	"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"user\": \"%U\", \"group\": \"%G\", \"audio_info\": %j, \"filename\": \"%n\", \"tagline\": \"%K\", \"top_groups\": [%t], \"top_users\": [%T]}"
#else
#define audio_norace_complete	DISABLED
#endif

/* Video files - supports mpeg1/mpeg2 and various avi formats */

#if ( disable_video_race == FALSE )
#define video_race		"{\"user\": \"%U\", \"group\": \"%G\", \"competitors\": [%R], \"release\": \"%r\", \"speed\": %.0S, \"filename\": \"%n\", \"duration\": \"%d\", \"percent\": %.1p, \"num_users\": %u, \"num_groups\": %g, \"num_files\": %F, \"expected_files\": %f, \"missing_files\": %M, \"tagline\": \"%K\", \"racers\": [%B], \"eta\": \"%$\", \"video_info\": {" video_info "}}"
#else
#define video_race		DISABLED
#endif
#if ( disable_video_sfv == FALSE )
#define video_sfv		"{\"release\": \"%r\", \"expected_files\": %f, \"user\": \"%U\", \"group\": \"%G\", \"filename\": \"%n\", \"tagline\": \"%K\"}"
#else
#define video_sfv		DISABLED
#endif
#if ( disable_video_update == FALSE )
#define video_update		"{\"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"speed\": %.0S, \"estimated_size\": %.1e, \"filename\": \"%n\", \"release\": \"%r\", \"tagline\": \"%K\", \"eta\": \"%$\", \"video_info\": {" video_info "}}"
#else
#define video_update		DISABLED
#endif
#if ( disable_video_halfway == FALSE )
#define video_halfway		"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"racers\": [%B], \"eta\": \"%$\", \"video_info\": {" video_info "}}"
#else
#define video_halfway		DISABLED
#endif
#if ( disable_video_newleader == FALSE )
#define video_newleader		"{\"user\": \"%U\", \"group\": \"%G\", \"speed\": %.0S, \"duration\": \"%d\", \"num_files\": %F, \"percent\": %.1p, \"total_mbytes\": %.0m, \"release\": \"%r\", \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"expected_files\": %f, \"missing_files\": %M, \"winning_user\": %C0, \"winning_group\": %c0, \"tagline\": \"%K\", \"competitors\": [%R], \"eta\": \"%$\", \"video_info\": {" video_info "}}"
#else
#define video_newleader		DISABLED
#endif
#if ( disable_video_complete == FALSE )
#define video_complete		"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"num_users\": %u, \"num_groups\": %g, \"slowest_user\": %l, \"fastest_user\": %L, \"winning_user\": %C0, \"winning_group\": %c0, \"filename\": \"%n\", \"racers\": [%B], \"top_groups\": [%t], \"top_users\": [%T], \"video_info\": {" video_info "}}"
#else
#define video_complete		DISABLED
#endif
#if ( disable_video_norace_halfway == FALSE )
#define video_norace_halfway	"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"tagline\": \"%K\", \"eta\": \"%$\", \"video_info\": {" video_info "}}"
#else
#define video_norace_halfway	DISABLED
#endif
#if ( disable_video_norace_complete == FALSE )
#define video_norace_complete	"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"user\": \"%U\", \"group\": \"%G\", \"filename\": \"%n\", \"tagline\": \"%K\", \"top_groups\": [%t], \"top_users\": [%T], \"video_info\": {" video_info "}}"
#else
#define video_norace_complete	DISABLED
#endif

/* RAR - allows usage of compression mode cookie (%J) for rarred releases */
#if ( disable_rar_race == FALSE )
#define rar_race		"{\"user\": \"%U\", \"group\": \"%G\", \"competitors\": [%R], \"release\": \"%r\", \"speed\": %.0S, \"filename\": \"%n\", \"duration\": \"%d\", \"percent\": %.1p, \"num_users\": %u, \"num_groups\": %g, \"num_files\": %F, \"expected_files\": %f, \"missing_files\": %M, \"tagline\": \"%K\", \"racers\": [%B], \"compression\": \"%J\", \"eta\": \"%$\"}"
#else
#define rar_race		DISABLED
#endif
#if ( disable_rar_sfv == FALSE )
#define rar_sfv			"{\"release\": \"%r\", \"expected_files\": %f, \"user\": \"%U\", \"group\": \"%G\", \"filename\": \"%n\", \"tagline\": \"%K\"}"
#else
#define rar_sfv			DISABLED
#endif
#if ( disable_rar_update == FALSE )
#define rar_update		"{\"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"speed\": %.0S, \"estimated_size\": %.1e, \"filename\": \"%n\", \"release\": \"%r\", \"tagline\": \"%K\", \"compression\": \"%J\", \"eta\": \"%$\"}"
#else
#define rar_update		DISABLED
#endif
#if ( disable_rar_halfway == FALSE )
#define rar_halfway		"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"racers\": [%B], \"compression\": \"%J\", \"eta\": \"%$\"}"
#else
#define rar_halfway		DISABLED
#endif
#if ( disable_rar_newleader == FALSE )
#define rar_newleader		"{\"user\": \"%U\", \"group\": \"%G\", \"speed\": %.0S, \"duration\": \"%d\", \"num_files\": %F, \"percent\": %.1p, \"total_mbytes\": %.0m, \"release\": \"%r\", \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"expected_files\": %f, \"missing_files\": %M, \"winning_user\": %C0, \"winning_group\": %c0, \"tagline\": \"%K\", \"competitors\": [%R], \"compression\": \"%J\", \"eta\": \"%$\"}"
#else
#define rar_newleader		DISABLED
#endif
#if ( disable_rar_complete == FALSE )
#define rar_complete		"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"num_users\": %u, \"num_groups\": %g, \"slowest_user\": %l, \"fastest_user\": %L, \"winning_user\": %C0, \"winning_group\": %c0, \"filename\": \"%n\", \"racers\": [%B], \"compression\": \"%J\", \"top_groups\": [%t], \"top_users\": [%T]}"
#else
#define rar_complete		DISABLED
#endif
#if ( disable_rar_norace_halfway == FALSE )
#define rar_norace_halfway	"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"tagline\": \"%K\", \"compression\": \"%J\", \"eta\": \"%$\"}"
#else
#define rar_norace_halfway	DISABLED
#endif
#if ( disable_rar_norace_complete == FALSE )
#define rar_norace_complete	"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"user\": \"%U\", \"group\": \"%G\", \"filename\": \"%n\", \"tagline\": \"%K\", \"compression\": \"%J\", \"top_groups\": [%t], \"top_users\": [%T]}"
#else
#define rar_norace_complete	DISABLED
#endif

/* Rest of the stuff inside sfv */
#if ( disable_other_race == FALSE )
#define other_race		"{\"user\": \"%U\", \"group\": \"%G\", \"competitors\": [%R], \"release\": \"%r\", \"speed\": %.0S, \"filename\": \"%n\", \"duration\": \"%d\", \"percent\": %.1p, \"num_users\": %u, \"num_groups\": %g, \"num_files\": %F, \"expected_files\": %f, \"missing_files\": %M, \"tagline\": \"%K\", \"racers\": [%B], \"eta\": \"%$\"}"
#else
#define other_race		DISABLED
#endif
#if ( disable_other_sfv == FALSE )
#define other_sfv		"{\"release\": \"%r\", \"expected_files\": %f, \"user\": \"%U\", \"group\": \"%G\", \"filename\": \"%n\", \"tagline\": \"%K\"}"
#else
#define other_sfv		DISABLED
#endif
#if ( disable_other_update == FALSE )
#define other_update		"{\"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"speed\": %.0S, \"estimated_size\": %.1e, \"filename\": \"%n\", \"release\": \"%r\", \"tagline\": \"%K\", \"eta\": \"%$\"}"
#else
#define other_update		DISABLED
#endif
#if ( disable_other_halfway == FALSE )
#define other_halfway		"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"racers\": [%B], \"eta\": \"%$\"}"
#else
#define other_halfway		DISABLED
#endif
#if ( disable_other_newleader == FALSE )
#define other_newleader		"{\"user\": \"%U\", \"group\": \"%G\", \"speed\": %.0S, \"duration\": \"%d\", \"num_files\": %F, \"percent\": %.1p, \"total_mbytes\": %.0m, \"release\": \"%r\", \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"expected_files\": %f, \"missing_files\": %M, \"winning_user\": %C0, \"winning_group\": %c0, \"tagline\": \"%K\", \"competitors\": [%R], \"eta\": \"%$\"}"
#else
#define other_newleader		DISABLED
#endif
#if ( disable_other_complete == FALSE )
#define other_complete		"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"num_users\": %u, \"num_groups\": %g, \"slowest_user\": %l, \"fastest_user\": %L, \"winning_user\": %C0, \"winning_group\": %c0, \"filename\": \"%n\", \"racers\": [%B], \"top_groups\": [%t], \"top_users\": [%T]}"
#else
#define other_complete		DISABLED
#endif
#if ( disable_other_norace_halfway == FALSE )
#define other_norace_halfway	"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"tagline\": \"%K\", \"eta\": \"%$\"}"
#else
#define other_norace_halfway	DISABLED
#endif
#if ( disable_other_norace_complete == FALSE )
#define other_norace_complete	"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"user\": \"%U\", \"group\": \"%G\", \"filename\": \"%n\", \"tagline\": \"%K\", \"top_groups\": [%t], \"top_users\": [%T]}"
#else
#define other_norace_complete	DISABLED
#endif

/* Zipped files */
#if ( disable_zip_race == FALSE )
#define zip_race		"{\"user\": \"%U\", \"group\": \"%G\", \"competitors\": [%R], \"release\": \"%r\", \"speed\": %.0S, \"filename\": \"%n\", \"duration\": \"%d\", \"percent\": %.1p, \"num_users\": %u, \"num_groups\": %g, \"num_files\": %F, \"expected_files\": %f, \"missing_files\": %M, \"tagline\": \"%K\", \"racers\": [%B], \"eta\": \"%$\"}"
#else
#define zip_race		DISABLED
#endif
#if ( disable_zip_update == FALSE )
#define zip_update		"{\"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"speed\": %.0S, \"estimated_size\": %.1e, \"filename\": \"%n\", \"release\": \"%r\", \"tagline\": \"%K\", \"eta\": \"%$\"}"
#else
#define zip_update		DISABLED
#endif
#if ( disable_zip_halfway == FALSE )
#define zip_halfway		"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"racers\": [%B], \"eta\": \"%$\"}"
#else
#define zip_halfway		DISABLED
#endif
#if ( disable_zip_newleader == FALSE )
#define zip_newleader		"{\"user\": \"%U\", \"group\": \"%G\", \"speed\": %.0S, \"duration\": \"%d\", \"num_files\": %F, \"percent\": %.1p, \"total_mbytes\": %.0m, \"release\": \"%r\", \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"expected_files\": %f, \"missing_files\": %M, \"winning_user\": %C0, \"winning_group\": %c0, \"tagline\": \"%K\", \"competitors\": [%R], \"eta\": \"%$\"}"
#else
#define zip_newleader		DISABLED
#endif
#if ( disable_zip_complete == FALSE )
#define zip_complete		"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"num_users\": %u, \"num_groups\": %g, \"slowest_user\": %l, \"fastest_user\": %L, \"winning_user\": %C0, \"winning_group\": %c0, \"filename\": \"%n\", \"racers\": [%B], \"top_groups\": [%t], \"top_users\": [%T]}"
#else
#define zip_complete		DISABLED
#endif
#if ( disable_zip_norace_halfway == FALSE )
#define zip_norace_halfway	"{\"release\": \"%r\", \"winning_user\": %C0, \"winning_group\": %c0, \"total_mbytes\": %.0m, \"num_files\": %F, \"percent\": %.1p, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"missing_files\": %M, \"filename\": \"%n\", \"num_users\": %u, \"num_groups\": %g, \"user\": \"%U\", \"group\": \"%G\", \"expected_files\": %f, \"tagline\": \"%K\", \"eta\": \"%$\"}"
#else
#define zip_norace_halfway	DISABLED
#endif
#if ( disable_zip_norace_complete == FALSE )
#define zip_norace_complete	"{\"release\": \"%r\", \"total_mbytes\": %.0m, \"expected_files\": %f, \"average_speed\": %.0A, \"average_speed_or_dontknow\": %.0a, \"duration\": \"%d\", \"epoch\": \"%&\", \"user\": \"%U\", \"group\": \"%G\", \"filename\": \"%n\", \"tagline\": \"%K\", \"top_groups\": [%t], \"top_users\": [%T]}"
#else
#define zip_norace_complete	DISABLED
#endif

/* Output of racestats binary */
#define stats_line		"{\"release\": \"%r\", \"num_files\": %F, \"expected_files\": %f, \"num_users\": %u, \"num_groups\": %g, \"winning_user\": %C0, \"winning_group\": %c0}"

/* Output of postdel binary */
#define incompletemsg		"{\"user\": \"%U\", \"group\": \"%G\", \"release\": \"%r\"}"

#define bad_file_msg		"{\"release\": \"%r\", \"user\": \"%U\", \"group\": \"%G\", \"filename\": \"%n\"}"
#define speedtest_msg		"{\"user\": \"%U\", \"group\": \"%G\", \"speed_mbit\": %.2s, \"speed_mbyte\": %.2#, \"mibytes\": %.1m, \"mbytes\": %.1N}"

enum ReleaseTypes {
	RTYPE_NULL = 0,
	RTYPE_RAR = 1,
	RTYPE_OTHER = 2,
	RTYPE_AUDIO = 3,
	RTYPE_VIDEO = 4,
	RTYPE_INVALID,
};

#endif
