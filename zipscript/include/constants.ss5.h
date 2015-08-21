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

#define PROGTYPE_ZIPSCRIPT              2
#define PROGTYPE_POSTDEL                4
#define PROGTYPE_CLEANUP                8
#define PROGTYPE_DATACLEANER            16
#define PROGTYPE_RESCAN                 32

#define INCOMPLETE_NORMAL		3
#define INCOMPLETE_NFO			7
#define INCOMPLETE_SAMPLE		9
#define INCOMPLETE_SFV			15

#ifndef O_SYNC
#define O_SYNC O_FSYNC
#endif

#define video_announce_norace_complete_type		"COMPLETE"
#define zip_announce_norace_complete_type		"COMPLETE"
#define rar_announce_norace_complete_type		"COMPLETE"
#define other_announce_norace_complete_type		"COMPLETE"
#define audio_vbr_announce_norace_complete_type		"COMPLETE"
#define audio_cbr_announce_norace_complete_type		"COMPLETE"
#define general_announce_norace_complete_type		"COMPLETE"
#define video_announce_race_complete_type		"COMPLETE"
#define zip_announce_race_complete_type			"COMPLETE"
#define rar_announce_race_complete_type			"COMPLETE"
#define other_announce_race_complete_type		"COMPLETE"
#define audio_vbr_announce_race_complete_type		"COMPLETE"
#define audio_cbr_announce_race_complete_type		"COMPLETE"
#define general_announce_race_complete_type		"COMPLETE"
#define video_announce_stat_race_complete_type		"COMPLETE"
#define zip_announce_stat_race_complete_type		"COMPLETE"
#define rar_announce_stat_race_complete_type		"COMPLETE"
#define other_announce_stat_race_complete_type		"COMPLETE"
#define audio_vbr_announce_stat_race_complete_type	"COMPLETE+ID3"
#define audio_cbr_announce_stat_race_complete_type	"COMPLETE+ID3"
#define general_announce_stat_race_complete_type	"COMPLETE"
#define video_announce_one_race_complete_type		"COMPLETE"
#define zip_announce_one_race_complete_type		"COMPLETE"
#define rar_announce_one_race_complete_type		"COMPLETE"
#define other_announce_one_race_complete_type		"COMPLETE"
#define audio_vbr_announce_one_race_complete_type	"COMPLETE+ID3"
#define audio_cbr_announce_one_race_complete_type	"COMPLETE+ID3"
#define general_announce_one_race_complete_type		"COMPLETE"
#define audio_announce_vbr_update_type			"UPDATE"
#define audio_announce_cbr_update_type			"UPDATE"
#define rar_announce_update_type			"UPDATE"
#define zip_announce_update_type			"UPDATE"
#define other_announce_update_type			"UPDATE"
#define video_announce_update_type			"UPDATE"
#define general_announce_update_type			"UPDATE"
#define audio_announce_race_type			"RACE"
#define rar_announce_race_type				"RACE"
#define zip_announce_race_type				"RACE"
#define other_announce_race_type			"RACE"
#define video_announce_race_type			"RACE"
#define general_announce_race_type			"RACE"
#define audio_announce_newleader_type			"NEWLEADER"
#define rar_announce_newleader_type			"NEWLEADER"
#define zip_announce_newleader_type			"NEWLEADER"
#define other_announce_newleader_type			"NEWLEADER"
#define video_announce_newleader_type			"NEWLEADER"
#define general_announce_newleader_type			"NEWLEADER"
#define audio_announce_norace_halfway_type		"HALFWAY"
#define rar_announce_norace_halfway_type		"HALFWAY"
#define zip_announce_norace_halfway_type		"HALFWAY"
#define other_announce_norace_halfway_type		"HALFWAY"
#define video_announce_norace_halfway_type		"HALFWAY"
#define general_announce_norace_halfway_type		"HALFWAY"
#define audio_announce_race_halfway_type		"HALFWAY"
#define rar_announce_race_halfway_type			"HALFWAY"
#define zip_announce_race_halfway_type			"HALFWAY"
#define other_announce_race_halfway_type		"HALFWAY"
#define video_announce_race_halfway_type		"HALFWAY"
#define general_announce_race_halfway_type		"HALFWAY"
#define general_doublesfv_type				"DOUBLESFV"
#define general_resumesfv_type				"DOUBLESFV"
#define audio_announce_sfv_type				"SFV"
#define rar_announce_sfv_type				"SFV"
#define other_announce_sfv_type				"SFV"
#define video_announce_sfv_type				"SFV"
#define general_announce_sfv_type			"SFV"
#define general_badgenre_type				"BADGENRE"
#define general_badyear_type				"BADYEAR"
#define general_badbitrate_type				"BADBITRATE"
#define general_incomplete_type				"INCOMPLETE"
#define stat_users_type					"USERSTATS"
#define stat_groups_type				"GROUPSTATS"
#define stat_post_type					"POSTSTATS"
#define	bad_file_crc_type				"BAD_FILE_CRC"
#define	bad_file_0size_type				"BAD_FILE_0SIZE"
#define	bad_file_zip_type				"BAD_FILE_ZIP"
#define	bad_file_wrongdir_type				"BAD_FILE_WRONGDIR"
#define	bad_file_sfv_type				"BAD_FILE_SFV"
#define	bad_file_nosfv_type				"BAD_FILE_NOSFV"
#define bad_file_nonfo_type                             "BAD_FILE_NONFO"
#define	bad_file_genre_type				"BAD_FILE_GENRE"
#define	bad_file_year_type				"BAD_FILE_YEAR"	
#define	bad_file_bitrate_type				"BAD_FILE_BITRATE"
#define	bad_file_disallowed_type			"BAD_FILE_DISALLOWED"
#define bad_file_nfo_type				"BAD_FILE_DUPENFO"
#define bad_file_zipnfo_type				"BAD_FILE_ZIPNFO"
#define bad_file_duperelease_type			"BAD_FILE_DUPERELEASE"
#define bad_file_speedtest_type				"ULTEST"
#define sample_announce_type				"SAMPLEDIR"

#define deny_double_msg		"{%U} {%G} {%r} {%n}"
#define deny_resumesfv_msg	"{%U} {%G} {%r} {%n}"
#define audio_cbr_warn_msg	"{%U} {%G} {%X}"
#define audio_year_warn_msg	"{%U} {%G} {%Y}"
#define audio_genre_warn_msg	"{%U} {%G} {%w}"

#define sample_msg		"{%u} {%g} {%D} {%E} {%.3H} {%.2;} {%:} {%,} {%`} {%=} {%>} {%<}"

#define pre_stats		"UserTop: "
#define between_stats		"GroupTop: "
#define post_stats		DISABLED
#define winner			"Winner: "
#define loser			"Loser: "
#define vbrnew			"vbr-new"
#define vbrold			"vbr-old"

#define user_top		"%n/%u/%g/%.1m/%f/%.1p/%.0s"
#define user_info		"%u/%g/%.1m/%f/%.1p/%.0s"
#define group_top		"%n/SS/%g/%.1m/%f/%.1p/%.0s"
#define group_info		"SS/%g/%.1m/%f/%.1p/%.0s"

/* Special reformat cookies */
#define fastestfile		"{%u} {%g} {%.2F}"	/* Put in %L cookie */
#define slowestfile		"{%u} {%g} {%.2S}"	/* Put in %l cookie */

#define racersplit		" "	/* Put between racers in the %R */
#define racersplit_prefix	""      /* Put before the first racer in the %R cookie */
#define racersplit_postfix	""	/* Put after the last racer in the %R cookie */
#define racersmsg		"%u/%g"	/* Put in %R cookie */

#define audio_vbr		"VBR %i %I"	/* Put in %j cookie */
#define audio_cbr		"CBR"	/* Put in %j cookie */

/*
 * Audio files - currently used only for flac & mp3 (mpeg2 layer3),
 * but mpeg1 & mpeg2.5 + layer1 & 2 can be added if needed
 */
#define audio_race		"%U %G %.0S {%R}"
#define audio_sfv		"%f"
#define audio_update		"{%.1e} {%w} {%Y} {%X} {%j}"
#define audio_halfway		"{%C0} {%c0}"
#define audio_newleader		"{%C0}"
#define audio_complete		"{%U} {%G} {%.0m} {%d} {%.0A} {%j}"
#define audio_norace_halfway	"{%C0} {%c0}"
#define audio_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A} {%j}"

/* Video files - supports mpeg1/mpeg2 and various avi formats */
#define video_race		"%U %G %.0S {%R}"
#define video_sfv		"%f"
#define video_update		"%.1e %f"
#define video_halfway		"{%C0} {%c0}"
#define video_newleader		"{%C0}"
#define video_complete		"{%U} {%G} {%.0m} {%d} {%.0A}"
#define video_norace_halfway	"{%C0} {%c0}"
#define video_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A}"

/* RAR - allows usage of compression mode cookie (%Z) for rarred releases */
#define rar_race		"%U %G %.0S {%R}"
#define rar_sfv			"%f"
#define rar_update		"%.1e %f"
#define rar_halfway		"{%C0} {%c0}"
#define rar_newleader		"{%C0}"
#define rar_complete		"{%U} {%G} {%.0m} {%d} {%.0A}"
#define rar_norace_halfway	"{%C0} {%c0}"
#define rar_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A}"


/* Rest of the stuff inside sfv */
#define other_race		"%U %G %.0S {%R}"
#define other_sfv		"%f"
#define other_update		"%.1e %f"
#define other_halfway		"{%C0} {%c0}"
#define other_newleader		"{%C0}"
#define other_complete		"{%U} {%G} {%.0m} {%d} {%.0A}"
#define other_norace_halfway	"{%C0} {%c0}"
#define other_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A}"

/* Zipped files */
#define zip_race		"%U %G %.0S {%R}"
#define zip_update		"%.1e %f"
#define zip_halfway		"{%C0} {%c0}"
#define zip_newleader		"{%C0}"
#define zip_complete		"{%U} {%G} {%.0m} {%d} {%.0A}"
#define zip_norace_halfway	"{%C0} {%c0}"
#define zip_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A}"

/* Output of racestats binary */
#define stats_line		"%r/%F/%f%/%u/%g/%C0"

/* Output of postdel binary */
#define incompletemsg		"%U/%G/%r"
#define bad_file_msg		"%r/%U/%G/%n"
#define speedtest_msg		"%U/%G/%.2s/%.1m/%.1n"

enum ReleaseTypes {
	RTYPE_NULL = 0,
	RTYPE_RAR = 1,
	RTYPE_OTHER = 2,
	RTYPE_AUDIO = 3,
	RTYPE_VIDEO = 4,
	RTYPE_INVALID,
};

#endif
