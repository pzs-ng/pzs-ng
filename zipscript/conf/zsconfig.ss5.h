/* zsconfig.h - ZipScript-C config file
 *
 * Please do not change settings you dont understand!
 * 
 * READ THE COOKIE FILE FIRST! DO NOT EDIT IN WINDOWS!
 */



/*
 *	General Settings
 *	================
 */

/* These should be changed to match your glftpd installation.
 */
#define log		"/ftp-data/logs/glftpd.log"
#define storage		"/tmp/zipscript/"
#define gl_userfiles	"/ftp-data/users/"
#define dupepath	"/ftp-data/logs/dupefile"

/* On default, announcing is disabled everywhere, use zip/sfv paths to enable annouces and private paths to disable.
 * Don't be lazy! Be specific! And remember - pathnames are case sensitive.
 */
#define group_dirs	"/site/groups/"
#define zip_dirs	"/site/incoming/0day/"
#define sfv_dirs	"/site/incoming/mp3/ /site/incoming/games/ /site/incoming/apps/ /site/incoming/musicvideos/ /site/incoming/requests/"
#define no_message_dirs	"/site/groups/ /site/requests/"
#define nocheck_dirs	"/site/private/"
#define audio_nocheck	"/site/groups/ /site/incoming/requests/"

#define GROUPFILE	"/etc/group"
#define PASSWDFILE	"/etc/passwd"

/* Directories for cleanup to check for broken incomplete files
 * Use same % values as with date
 */
#define cleanupdirs	"/site/incoming/games/ /site/incoming/apps/ /site/incoming/0day/%m%d/ /site/incoming/mp3/%m%d/"

/* Hide name of uploaders in the .message-file in group-dirs? Setting this
 * to TRUE will convert the username to the groupname of the user.
 */
#define hide_group_uploaders	TRUE

/* Creates a zero-byte file for every non-existing file defined in sfv?
 */
#define create_missing_files	TRUE

/* Defines type of status bar
 * 0 = file
 * 1 = directory
 */
#define status_bar_type		1

/* Defines type of incomplete pointer
 * 0 = file
 * 1 = symbolic link
 */
#define incompleteislink	1

/* Defines symbolic link type
 * 0 = absolute path
 * 1 = relative path
 */
#define userellink		1


/* If file has suid bit set, program changes uid and gid to these (0 leaves option uncompiled)
 */
#define program_uid		20
#define program_gid		200

/* These file types are allowed without any checks (types need to be also ignored)
 */
#define allowed_types		"jpg,avi,mpg,cue,m3u"

/* These file types are ignored from sfv
 * NOTE: , at the beginning ignores all files without extensions
 */
#define ignored_types		",diz,jpg,cue,m3u,nfo,sfv,debug"

/* Special modes
 */
#define debug_mode		TRUE
#define benchmark_mode		TRUE

/* Get user day/week/month/allup position
 */
#define get_user_stats		TRUE

#define show_user_info		TRUE
#define show_group_info		TRUE


/*
 *	SFV Settings
 *	============
 */

/* Removes duplicate entries and comments from the sfv - some lame zipscripts seem to add duplicate entries :p
 */
#define sfv_dupecheck		TRUE

/* Writes a plain sfv - removing all comments etc
 */

#define sfv_cleanup		TRUE

/* Should new sfv files be denied in dir? If not, the new sfv will be checked and denied/allowed depending on the
 * content of the sfv.
 */

#define deny_double_sfv		FALSE
#define deny_double_msg		"User %B%U%B/%G uploaded a second (dupe) SFV in %B%r%B (%n). Naughty boy!"

/* Exclude non sfv directories (ie group/request directory)
 */
/*#define exclude_non_sfv_dirs	TRUE*/

/* Force sfv to be uploaded first, before accepting any files
 */
#define force_sfv_first		FALSE


/*
 *	MP3 Settings
 *	============
 */

/* These directories will contain symlinks to your audio releases, based on genre, year and artist.
   Ignore these paths if you have no plans on sorting your releases.
 */
#define audio_genre_path		"/site/incoming/music.by.genre/"
#define audio_artist_path		"/site/incoming/music.by.artist/"
#define audio_year_path			"/site/incoming/music.by.year/"
#define audio_group_path		"/site/incoming/music.by.group/"

/* It is a good idea to prevent crap from filling your hds.
 * Ignore these settings if you do not plan to restrict your uploads.
 */
#define allowed_constant_bitrates	"160,192"
#define allowed_years			"2000,2001,2002,2003,2004"
#define banned_genres			"Christian Rap,R&B"
#define allowed_genres			"Rock"

/* Audio related symlinks - set to TRUE if you wish to sort/symlink releases, or FALSE if not.
 */
#define audio_genre_sort		TRUE
#define audio_year_sort			FALSE
#define audio_artist_sort		TRUE
#define audio_group_sort		TRUE

/* Audio related checks for quality/type - here you enable/disable the restriction you defined earlier.
 * If warn is true, any banned files will not be deleted, but instead a warning message will be logged
 * to your glftpd.log.
 * Bitrate checking is useless on VBR encoded mp3 files, so it's recommended you leave this FALSE.
 */
#define	audio_cbr_check			FALSE
#define audio_cbr_warn			TRUE
#define audio_cbr_warn_msg		"%B%U%B/%G has started an illegal upload %B%r%B (bitrate: %Wkbps). Naughty boy!"
#define audio_year_check		TRUE
#define audio_year_warn			TRUE
#define audio_year_warn_msg		"%B%U%B/%G has started an illegal upload %B%r%B (year: %Y). Naughty boy!"
#define audio_banned_genre_check	TRUE
#define audio_allowed_genre_check	FALSE
#define audio_genre_warn		TRUE
#define audio_genre_warn_msg		"%B%U%B/%G has started an illegal upload %B%r%B (genre: %w). Naughty boy!"

/* Create m3u file on release complete for audio release?
 */
#define create_m3u			TRUE


/*
 *	Race Settings
 *	=============
 *	NOTE: You may disable announces by typing DISABLED instead of cookie string
 *	(do not use DISABLED with message etc files!!!)
 */

/* New leader needs this number of files more than current leader to be counted as "newleader"
 * (reduces "spam" in channel)
 */
#define enable_files_ahead	TRUE
#define newleader_files_ahead	2

/* If minium is not reached - announcement will be disabled. The number entered represents
 * the minimum number of files in the release.
 */
#define min_newleader_files	1
#define min_update_files	1
#define min_halfway_files	10

/* Limit number of users/groups to display in stats.
 */
#define max_users_in_top	7
#define max_groups_in_top	7


/* Enable/disable norace announces (will be used instead of normal announces, if there is only one user in race)
 */
#define	announce_norace		TRUE

/* Defines how stats are written into log
 * 0 = complete msg only
 * 1 = n line mode (complete, pre_stats, user top, between stats, group top, post stats)
 */
/*#define zip_complete_type		0
#define zip_norace_complete_type	0
#define rar_complete_type		1
#define rar_norace_complete_type	0
#define audio_complete_type		1
#define audio_norace_complete_type	0
#define video_complete_type		1
#define video_norace_complete_type	0
#define other_complete_type		1
#define other_norace_complete_type	0*/


/* General output settings and cookies
 */
/*#define pre_stats		"UserTop: "
#define between_stats		"GroupTop: "
#define post_stats		DISABLED

#define user_top		"%n. %B%u%B/%-10g [%.1mM/%fF/%.1p%%/%.0sKB/s]"
#define user_info		"%B%u%B/%g [%.1mM/%fF/%.1p%%/%.0sKB/s]"
#define group_top		"%n. %B%g%B [%.1mM/%fF/%.1p%%/%.0sKB/s]"
#define group_info		"%B%g%B [%.1mM/%fF/%.1p%%/%.0sKB/s]"
#define fastestfile		"Fastest upload by %B%u%B/%g at %B%.3F%BKB/S."*/		/* Put in %l cookie */
/*#define slowestfile		"Slowest upload by %B%u%B/%g at %B%.3S%BKB/S."*/		/* Put in %L cookie */

/* Get list of competitors (all racers excluding current user) - stored in %R cookie
 */
/*#define get_competitor_list	TRUE
#define racersmsg		"%B%u%B(%g)"*/					/* Put in %R cookie */

/* Audio files - currently used only for mp3 (mpeg2 layer3), but mpeg1 & mpeg2.5 + layer1 & 2 can be added if needed
 */
/*#define audio_vbr		"Encoded @ %XKbps VBR (%I) in %zHz %Q."*/		/* Put in %j cookie */
/*#define audio_cbr		"Encoded @ %XKbps CBR in %zHz %Q."*/		/* Put in %j cookie */

/*#define audio_race		"%B%U%B is racing [ %R ] on %B%r%B @ %B%.0S%BkB/s."
#define audio_sfv		"Got SFV for %B%r%B. Expecting %B%f%BF."
#define audio_update		"Got MP3 info for %B%r%B. Expecting %B%f%BF of %B%w%B from %B%Y%B. %j"
#define audio_halfway		"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define audio_newleader		"%C0 takes the lead in %B%r%B!"
#define audio_complete		"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). %l %L"
#define audio_norace_halfway	"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define audio_norace_complete	"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). %l %L"*/

/* Video files - supports mpeg1/mpeg2 and various avi formats
 */
/*#define video_race		"%B%U%B is racing [ %R ] on %B%r%B @ %B%.0S%BkB/s."
#define video_sfv		"Got SFV for %B%r%B. Expecting %B%f%BF."
#define video_update		"%B%U%B/%G was the first to upload file in %B%r%B. Expecting %B%.1e%BmB."
#define video_halfway		"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define video_newleader		"%C0 takes the lead in %B%r%B!"
#define video_complete		"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). Compression method: m%B%Z%B. %l %L"
#define video_norace_halfway	"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define video_norace_complete	"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). %l %L"*/

/* RAR - allows usage of compression mode cookie for rarred releases
 */
/*#define rar_race		"%B%U%B is racing [ %R ] on %B%r%B @ %B%.0S%BkB/s."
#define rar_sfv			"Got SFV for %B%r%B. Expecting %B%f%BF."
#define rar_update		"%B%U%B/%G was the first to upload file in %B%r%B. Expecting %B%.1e%BmB."
#define rar_halfway		"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define rar_newleader		"%C0 takes the lead in %B%r%B!"
#define rar_complete		"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). Compression method: m%B%Z%B. %l %L"
#define rar_norace_halfway	"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define rar_norace_complete	"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). %l %L"*/

/* Rest of the stuff inside sfv
 */
/*#define other_race		"%B%U%B is racing [ %R ] on %B%r%B @ %B%.0S%BkB/s."
#define other_sfv		"Got SFV for %B%r%B. Expecting %B%f%BF."
#define other_update		"%B%U%B/%G was the first to upload file in %B%r%B. Expecting %B%.1e%BmB."
#define other_halfway		"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define other_newleader		"%C0 takes the lead in %B%r%B!"
#define other_complete		"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). %l %L"
#define other_norace_halfway	"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define other_norace_complete	"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). %l %L"*/

/* Zipped files
 */
/*#define zip_race		"%B%U%B is racing [ %R ] on %B%r%B @ %B%.0S%BkB/s."
#define zip_update		"Got DiZ for %B%r%B. Expecting %B%.1e%BmB in %B%f%BF"
#define zip_halfway		"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define zip_newleader		"%C0 takes the lead in %B%r%B!"
#define zip_complete		"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). %l %L Lucky7: %T"
#define zip_norace_halfway	"%B%r%B is in halfway. Leading uploader is %C0. Leading group is %c0."
#define zip_norace_complete	"%B%r%B [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ] was completed by %B%u%B racer(s) from %B%g%B group(s). %l %L"*/



/*
 *	Output variables used on site (.message file)
 *	=============================================
 */

/* Write .message file on complete
 */
#define write_complete_message	TRUE

/* What the .message file should be called
 */
#define message_file_name	".message"

/* Various Output
 */
#define message_header		""
#define message_user_header	"\n                           U S E R T O P\n" \
				"+--------------------------------------------------------------------+\n"
#define message_user_body	"%3n %-34U %7.1mM %3fF %5.1p%% %5.0sKBs\n"
#define message_user_footer	"+--------------------------------------------------------------------+\n" \
				"%3u Total %36.1mM %3fF 100.0%% %5.0aKBs\n"

#define message_group_header	"\n\n                          G R O U P T O P\n" \
				"+--------------------------------------------------------------------+\n"
#define message_group_body	"%3n %-34g %7.1mM %3fF %5.1p%% %5.0sKBs\n"
#define message_group_footer	"+--------------------------------------------------------------------+\n" \
				"%3g Total %36.1mM %3fF 100.0%% %5.0aKBs\n"

#define message_mp3 		"\n\n                            I D 3 - T A G\n" \
				"+--------------------------------------------------------------------+\n" \
				"  Artist : %x\n" \
				"  Album  : %W\n" \
				"  Genre  : %-34w Rate  : %Xkbit/sec\n" \
				"  Tracks : %-34f Year  : %Y\n" \
				"  Encoder: %-34i Preset: %I\n" \
				"+--------------------------------------------------------------------+\n"
#define message_footer		""

#define video_completebar	"[dS] - ( %.0mM %fF - COMPLETE ) - [dS]"
#define audio_completebar	"[dS] - ( %.0mM %fF - COMPLETE - %w %Y ) - [dS]"
#define rar_completebar		"[dS] - ( %.0mM %fF - COMPLETE ) - [dS]"
#define other_completebar	"[dS] - ( %.0mM %fF - COMPLETE ) - [dS]"
#define zip_completebar		"[dS] - ( %.0mM %fF - COMPLETE ) - [dS]"
#define del_completebar		"\\[dS] - \\( .*F - COMPLETE.*) - \\[dS]"


#define zipscript_header	 ".----=== ZipScript-C ===--,d1'--=== (C) 2004 ===---.\n"
#define zipscript_sfv_ok	 "| + SFV-file: oK!                                  |\n"
#define zipscript_any_ok	 "| + File: ok!                                      |\n"
#define zipscript_SFV_ok	 "| + CRC-Check: oK!                                 |\n"
#define zipscript_SFV_skip	 "| + CRC-Check: SKIPPED!                            |\n"
#define zipscript_zip_ok	 "| + ZiP integrity: oK!                             |\n"
#define realtime_mp3_info	 "+-=[ID3tag]=------------------------===------------+\n" \
				 "| Title  : %-39y |\n" \
				 "| Artist : %-39x |\n" \
				 "| Album  : %-39W |\n" \
				 "| Genre  : %-39w |\n" \
				 "| Year   : %-39Y |\n"
#define realtime_user_header	 "+-=[UserTop]=-------------------===----------------+\n"
#define realtime_user_body	 "| %2n. %-17u %8.1mmB %2fF %7.0skB/s |\n"
#define realtime_user_footer	 ""
#define realtime_group_header	 "+-=[GroupTop]=--------------------===--------------+\n"
#define realtime_group_body	 "| %2n. %-17g %8.1mmB %2fF %7.0skB/s |\n"
#define realtime_group_footer	 ""
#define zipscript_footer_unknown "`-[ - NO - SFV - ]------------------------[%2F/??]--'\n"
#define zipscript_footer_ok	 "`-[%V]------------------------[%2F/%-2f]--'\n"
#define zipscript_footer_error	 "| - %-46v |\n" \
				 "`-----------------------------------========-------'\n"
#define zipscript_footer_skip	 "`--------------------------------=====-------------'\n"

#define sfv_comment		 "; sfv raped by zipscript-c on the mighty ,dS'\r\n" \
				 "; \r\n"

/* How Indicators look
 */
#define incomplete_cd_indicator		"../../(incomplete)-(%0)-%1"
#define incomplete_indicator		"../(incomplete)-%0"

#define progressmeter			"[%V] - %3.0p%% Complete - [dS]"
#define del_progressmeter		"\\[.*] - ...% Complete - \\[dS]"


/*
 *	Various settings
 *	================
 */

/* Enable/disable accept script (accept script is run, if file is ok)
 */
#define enable_accept_script	FALSE
#define accept_script		"/bin/imdb_parse.sh"

/* Enable/disable nfo script (only run on .nfo files)
 */
#define enable_nfo_script	FALSE
#define nfo_script		"/bin/psxc-imdb.sh"

/* Enable/disable complete script (complete script is run, if release is complete)
 */
#define enable_complete_script	FALSE
#define complete_script		"/bin/nfo_copy.sh"

/* Enable/disable mp3 script (run only once, after first mp3-file is uploaded)
 */
#define enable_mp3_script	TRUE
#define mp3_script		"/bin/mp3-genres.sh"

/* Enable/disable unduper script (runs if sfv is uploaded after files, and files
 *                                are marked as bad and deleted)
 */
#define enable_unduper_script	TRUE
#define unduper_script		"/bin/pzsng-undupe"

/*
 *	Other binary settings
 *	=====================
 */

/* Output of racestats binary
 */
/*#define stats_line		"%B%r%B: %B%F%B of %B%f%B files uploaded by %B%u%B racer(s) from %B%g%B group(s). Leading uploader: %C0."*/

/* Output of postdel binary
 */
/*#define incompletemsg		"%B%U%B/%G deletes file from %B%r%B. Release is now %Bincomplete%B!"*/





/* #--#--#--#-- REQUIRED BY SITESTAT DO NOT MODIFY --#--#--#--#--# */
/* #--#--#--#-- REQUIRED BY SITESTAT DO NOT MODIFY --#--#--#--#--# */
/* #--#--#--#-- REQUIRED BY SITESTAT DO NOT MODIFY --#--#--#--#--# */

#define exclude_non_sfv_dirs		FALSE

#define zip_complete_type		0
#define zip_norace_complete_type	1
#define rar_complete_type		1
#define rar_norace_complete_type	1
#define audio_complete_type		1
#define audio_norace_complete_type	1
#define video_complete_type		1
#define video_norace_complete_type	1
#define other_complete_type		1
#define other_norace_complete_type	1

#define pre_stats		"UserTop: "
#define between_stats		"GroupTop: "
#define post_stats		DISABLED

#define user_top		"%n/%u/%g/%.1m/%f/%.1p/%.0s"
#define user_info		"%u/%g/%.1m/%f/%.1p/%.0s"
#define group_top		"%n/SS/%g/%.1m/%f/%.1p/%.0s"
#define group_info		"SS/%g/%.1m/%f/%.1p/%.0s"
#define fastestfile		""
#define slowestfile		""

#define get_competitor_list	TRUE
#define racersmsg		"%u/%g"

#define audio_vbr		"%w %Y VBR %i %I"
#define audio_cbr		"%w %Y %X"

#define audio_race		"%U %G %.0S {%R}"
#define audio_sfv		"%f"
#define audio_update		"{%.1e} {%w} {%Y} {%X} {%j}"
#define audio_halfway		"{%C0} {%c0}"
#define audio_newleader		"{%C0}"
#define audio_complete		"{%U} {%G} {%.0m} {%d} {%.0A}"
#define audio_norace_halfway	"{%C0} {%c0}"
#define audio_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A}"

#define video_race		"%U %G %.0S {%R}"
#define video_sfv		"%f"
#define video_update		"%.1e %f"
#define video_halfway		"{%C0} {%c0}"
#define video_newleader		"{%C0}"
#define video_complete		"{%U} {%G} {%.0m} {%d} {%.0A}"
#define video_norace_halfway	"{%C0} {%c0}"
#define video_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A}"

#define rar_race		"%U %G %.0S {%R}"
#define rar_sfv			"%f"
#define rar_update		"%.1e %f"
#define rar_halfway		"{%C0} {%c0}"
#define rar_newleader		"{%C0}"
#define rar_complete		"{%U} {%G} {%.0m} {%d} {%.0A}"
#define rar_norace_halfway	"{%C0} {%c0}"
#define rar_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A}"

#define other_race		"%U %G %.0S {%R}"
#define other_sfv		"%f"
#define other_update		"%.1e %f"
#define other_halfway		"{%C0} {%c0}"
#define other_newleader		"{%C0}"
#define other_complete		"{%U} {%G} {%.0m} {%d} {%.0A}"
#define other_norace_halfway	"{%C0} {%c0}"
#define other_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A}"

#define zip_race		"%U %G %.0S {%R}"
#define zip_update		"%.1e %f"
#define zip_halfway		"{%C0} {%c0}"
#define zip_newleader		"{%C0}"
#define zip_complete		"{%U} {%G} {%.0m} {%d} {%.0A}"
#define zip_norace_halfway	"{%C0} {%c0}"
#define zip_norace_complete	"{%U} {%G} {%.0m} {%d} {%.0A}"

#define stats_line		"%r/%F/%f%/%u/%g/%C0"

#define incompletemsg		"%U/%G/%r"
