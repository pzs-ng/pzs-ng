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
#define site_root	"/glftpd/"
#define log		"/ftp-data/logs/glftpd.log"
#define storage		"/ftp-data/zipscript/"
#define gl_userfiles	"/ftp-data/users/"

/* SHM area to use (for glftpd) - Only change if you know what you're doing.
 */
#define KEY		0x0000DEAD

/* Are you using GLFTPD 2?
 */
#define for_glftpd2	TRUE

/* On default, announcing is disabled everywhere, use zip/sfv paths to enable annouces and private paths to disable.
 * Don't be lazy! Be specific! And remember - pathnames are case sensitive.
 */
#define group_dirs	"/site/STAFF/ /site/PRE/"
#define zip_dirs	"/site/STAFF/ /site/Requests/ /site/PRE/ /site/GBA/ /site/COVERS/"
#define sfv_dirs	"/site/COVERS/ /site/DVDR/ /site/MP3/ /site/FR-APPS/ /site/US-APPS/ /site/PRE/ /site/Requests/ /site/FR-GAMES/ /site/US-GAMES/ /site/XBOX/ /site/STAFF/"
#define nocheck_dirs	""

/* Directories for cleanup to check for broken incomplete files
 * Use same % values as with date
 */
#define cleanupdirs	"/site/COVERS/ /site/DVDR/ /site/GBA/ /site/MP3/%Y-%m-%d/ /site/FR-APPS/ /site/US-APPS/ /site/PRE/ /site/Requests/ /site/FR-GAMES/ /site/US-GAMES/ /site/XBOX/ /site/STAFF/"

/* Creates a zero-byte file for every non-existing file defined in sfv?
 */
#define create_missing_files	TRUE

/* Defines type of status bar
 * 0 = file
 * 1 = directory
 */
#define status_bar_type		0

/* Defines type of incomplete pointer
 * 0 = file
 * 1 = symbolic link
 */
#define incompleteislink	1


/* If file has suid bit set, program changes uid and gid to these (0 leaves option uncompiled)
 */
#define program_uid		10
#define program_gid		200

/* These file types are allowed without any checks (types need to be also ignored)
 */
#define allowed_types		"jpg,avi,mpg,cue,m3u"

/* These file types are ignored from sfv
 * NOTE: , at the beginning ignores all files without extensions
 */
#define ignored_types		",diz,jpg,cue,m3u,nfo,sfv"

/* Special modes
 */
#define debug_mode		FALSE
#define benchmark_mode		FALSE

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

#define deny_double_sfv		TRUE
#define deny_double_msg		"10%U uploaded a second (dupe) SFV in %r (%n). Dumb Ass!"

/* Exclude non sfv directories (ie group/request directory)
 */
#define exclude_non_sfv_dirs	TRUE

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
#define audio_genre_path		"/site/iNDEX/Music.By.Genre/"
#define audio_artist_path		"/site/incoming/music.by.artist/"
#define audio_year_path			"/site/incoming/music.by.year/"

/* It is a good idea to prevent crap from filling your hds.
 * Ignore these settings if you do not plan to restrict your uploads.
 */
#define allowed_bitrates		"160,192"
#define allowed_years			"2003,2004,2005,2006"
#define banned_genres			"Christian Rap"
#define allowed_genres			"Rock"


/* Audio related symlinks - set to TRUE if you wish to sort/symlink releases, or FALSE if not.
 */
#define audio_genre_sort		TRUE
#define audio_year_sort			FALSE
#define audio_artist_sort		FALSE

/* Audio related checks for quality/type - here you enable/disable the restriction you defined earlier.
 * If warn is true, any banned files will not be deleted, but instead a warning message will be logged
 * to your glftpd.log.
 * Bitrate checking is useless on VBR encoded mp3 files, so it's recommended you leave this FALSE.
 */
#define	audio_bitrate_check		FALSE
#define audio_bitrate_warn		TRUE
#define audio_bitrate_warn_msg		"10%U/%G has started an illegal upload %r (bitrate: %Wkbps). Naughty boy!"
#define audio_year_check		TRUE
#define audio_year_warn			TRUE
#define audio_year_warn_msg		"10%U/%G has started an illegal upload %r (year: %Y). Naughty boy!"
#define audio_banned_genre_check	TRUE
#define audio_allowed_genre_check	FALSE
#define audio_genre_warn		TRUE
#define audio_genre_warn_msg		"10%U/%G has started an illegal upload %r (genre: %w). Naughty boy!"

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
#define zip_complete_type		1
#define zip_norace_complete_type	1
#define rar_complete_type		1
#define rar_norace_complete_type	1
#define audio_complete_type		1
#define audio_norace_complete_type	1
#define video_complete_type		1
#define video_norace_complete_type	1
#define other_complete_type		1
#define other_norace_complete_type	1


/* General output settings and cookies
 */
#define pre_stats		"Top Users: "
#define between_stats		"Top Groups: "
#define post_stats		DISABLED

#define user_top		"%n. 10%u/%-10g [%.1mM/%fF/%.1p%%/%.0sKB/s]"
#define user_info		"10%u/%g [%.1mM/%fF/%.1p%%/%.0sKB/s]"
#define group_top		"%n. %g [%.1mM/%fF/%.1p%%/%.0sKB/s]"
#define group_info		"%g [%.1mM/%fF/%.1p%%/%.0sKB/s]"
#define fastestfile		"Fastest upload by 10%u/%g at %B%.0F%BKB/S."	/* Put in %l cookie */
#define slowestfile		"Slowest upload by 10%u/%g at %B%.0S%BKB/S."	/* Put in %L cookie */


/* Get list of competitors (all racers excluding current user) - stored in %R cookie
 */
#define get_competitor_list	TRUE
#define racersmsg		"10%u(%g)"					/* Put in %R cookie */


/* Audio files - currently used only for mp3 (mpeg2 layer3), but mpeg1 & mpeg2.5 + layer1 & 2 can be added if needed
*/
#define audio_vbr		"%XKbps VBR (%I-%Q)"		/* Put in %j cookie */
#define audio_cbr		"%XKbps CBR (%Q)"		/* Put in %j cookie */


#define audio_race		"10%U is racing [ %R ] on %r @ %B%.0S%BkB/s."
#define audio_sfv		DISABLED
#define audio_update		"Got MP3 info for %r. Expecting %B%f%B File(s) of %B%w%B from %B%Y%B. First File upped at %.0S kB/s - %j"
#define audio_halfway		DISABLED
#define audio_newleader		"10%C0 takes the lead in %r !"
#define audio_complete		"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. %l %L"
#define audio_norace_halfway	DISABLED
#define audio_norace_complete	"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. %l %L"

/* Video files - supports mpeg1/mpeg2 and various avi formats
 */
#define video_race		"10%U is racing [ %R ] on %r @ %B%.0S%BkB/s."
#define video_sfv		"Got SFV for %r. Expecting %B%f%B File(s)."
#define video_update		"10%U/%G was the first to upload file in %r at %.0S kB/s. Expecting %B%.1e%BmB."
#define video_halfway		"%r is in halfway. Leading uploader is 10%C0. Leading group is %c0."
#define video_newleader		"10%C0 takes the lead in %r !"
#define video_complete		"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. Compression method: m%B%Z%B. %l %L"
#define video_norace_halfway	"%r is in halfway. Leading uploader is 10%C0. Leading group is %B%c0%B."
#define video_norace_complete	"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. %l %L"

/* RAR - allows usage of compression mode cookie for rarred releases
 */
#define rar_race		"10%U is racing [ %R ] on %r @ %B%.0S%BkB/s."
#define rar_sfv			"Got SFV for %r. Expecting %B%f%B File(s)."
#define rar_update		"10%U/%G was the first to upload file in %r at %.0S kB/s. Expecting %B%.1e%BmB."
#define rar_halfway		"%r is in halfway. Leading uploader is 10%C0. Leading group is %c0."
#define rar_newleader		"10%C0 takes the lead in %r !"
#define rar_complete		"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. Compression method: m%B%Z%B. %l %L"
#define rar_norace_halfway	"%r is in halfway. Leading uploader is 10%C0. Leading group is %B%c0%B."
#define rar_norace_complete	"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. %l %L"

/* Rest of the stuff inside sfv
 */
#define other_race		"10%U is racing [ %R ] on %r @ %B%.0S%BkB/s."
#define other_sfv		"Got SFV for %r. Expecting %B%f%B File(s)."
#define other_update		"10%U/%G was the first to upload file in %r at %.0S kB/s. Expecting %B%.1e%BmB."
#define other_halfway		"%r is in halfway. Leading uploader is 10%C0. Leading group is %c0."
#define other_newleader		"10%C0 takes the lead in %r !"
#define other_complete		"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. Compression method: m%B%Z%B. %l %L"
#define other_norace_halfway	"%r is in halfway. Leading uploader is 10%C0. Leading group is %B%c0%B."
#define other_norace_complete	"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. %l %L"

/* Zipped files
 */
#define zip_race		"10%U is racing [ %R ] on %r @ %B%.0S%bkB/s."
#define zip_update		"Got DiZ for %r. Expecting %B%.1e%BmB in %B%f%B File(s). First File upped at %.0S kB/s"
#define zip_halfway		DISABLED
#define zip_newleader		"10%C0 takes the lead in %r !"
#define zip_complete		"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. %l %L Lucky7: %T"
#define zip_norace_halfway	DISABLED
#define zip_norace_complete	"%r [ %B%.0m%BmB in %B%f%BF at %B%.0A%BkB/s - %d ]. %l %L"



/*
 *	Output variables used on site (.message file) 
 *	=============================================
 */

/* Write .message file on complete
 */
#define write_complete_message	TRUE

/* Various Output
 */
#define message_header		"\n                          __          __\n                  _______/  |______ _/  |_  ______\n                 /  ___/\\   __\\__  \\\\   __\\/  ___/ \n                 \\___ \\  |  |  / __ \\|  |  \\___ \\ \n                /____  > |__| (____  /__| /____  >\n                     \\/            \\/          \\/ \n"
#define message_user_header	"\n.....................................................................\n"  \
				".                                                                   .\n"  \
				".  [+]   Racers   [+]                        size   #    Speed      .\n"  \
				".                                                                   .\n"
#define message_user_body	". %3n %-34U %7.1mM %3fF %5.0sKBs     .\n"
#define message_user_footer	".....................................................................\n"  \
				".                                                                   .\n"
#define message_group_header	".  [+]   Groups   [+]                                               .\n"  \
				".                                                                   .\n"
#define message_group_body	". %3n %-34g %7.1mM %3fF %5.0sKBs     .\n"
#define message_group_footer	".....................................................................\n"

#define message_mp3 		".                                                                   .\n"  \
				".  [+]   I D 3 - T A G   [+]                                        .\n"  \
				".                                                                   .\n"  \
				".  Artist: %-34x                       .\n"  \
				".  Album : %-34W                       .\n"  \
				".  Genre : %-15w Encoding: %-30j .\n"  \
				".  Tracks: %-15f Year: %-34Y .\n"  \
				".                                                                   .\n"  \
				".....................................................................\n"
#define message_footer		".                                                                   .\n"  \
				".  [+]   Infos   [+]                                                .\n"  \
				".                                                                   .\n"  \
				".  Release size:       %5.0m MB                                     .\n"  \
				".  Global race speed: %6.0A kB/s                                   .\n"  \
				".                                                                   .\n"  \
				".....................................................................\n"

#define video_completebar	"[PZS] - ( %.0mM %fF @ %0.A kBps - COMPLETE ) - [PZS]"
#define audio_completebar	"[PZS] - ( %.0mM %fF @ %0.A kBps - COMPLETE - %w %Y ) - [PZS]"
#define rar_completebar		"[PZS] - ( %.0mM %fF @ %0.A kBps - COMPLETE ) - [PZS]"
#define other_completebar	"[PZS] - ( %.0mM %fF @ %0.A kBps - COMPLETE ) - [PZS]"
#define zip_completebar		"[PZS] - ( %.0mM %fF @ %0.A kBps - COMPLETE ) - [PZS]"
#define del_completebar		"\\[PZS] - \\( .*F - COMPLETE.*) - \\[PZS]"


#define zipscript_header	 "....................................................\n"  \
				 ".                                                  .\n"  \
				 ".                [+]    PZS    [+]                 .\n"  \
				 "....................................................\n"
#define zipscript_sfv_ok	 ".                                                  .\n"  \
				 ". [+]  SFV-file: oK!  [+]                          .\n"  \
				 ".                                                  .\n"
#define zipscript_any_ok	 ".                                                  .\n"  \
				 ". [+]  File: ok!      [+]                          .\n"  \
				 ".                                                  .\n"
#define zipscript_SFV_ok	 ".                                                  .\n"  \
				 ". [+]  CRC-Check: oK! [+]                          .\n"  \
				 ".                                                  .\n"
#define zipscript_SFV_skip	 ".                                                  .\n"  \
				 ". [+]  CRC-Check: SKIPPED!  [+]                    .\n"  \
				 ".                                                  .\n"
#define zipscript_zip_ok	 ".                                                  .\n"  \
				 ". [+]  ZiP integrity: oK!  [+]                     .\n"  \
				 ".                                                  .\n"
#define realtime_mp3_info	 "....................................................\n" \
				 ".                                                  .\n" \
				 ". [+]  ID3-TAG  [+]                                .\n" \
				 ".                                                  .\n" \
				 ". Title    : %-37y .\n" \
				 ". Artist   : %-37x .\n" \
				 ". Album    : %-37W .\n" \
				 ". Genre    : %-37w .\n" \
				 ". Year     : %-37Y .\n" \
				 ". Encoding : %-37j .\n"
#define realtime_user_header	 "....................................................\n" \
				 ".                                                  .\n" \
				 ". [+] Top Racers [+]                               .\n" \
				 ".                                                  .\n"
#define realtime_user_body	 ". %2n. %-17u %8.1mmB %2fF %7.0skB/s .\n"
#define realtime_user_footer	 ""
#define realtime_group_header	 "....................................................\n" \
				 ".                                                  .\n" \
				 ". [+] Top Groups [+]                               .\n" \
				 ".                                                  .\n"
#define realtime_group_body	 ". %2n. %-17g %8.1mmB %2fF %7.0skB/s .\n"
#define realtime_group_footer	 ""
#define zipscript_footer_unknown "..[ - NO - SFV - ]........................[%2F/??]...\n"
#define zipscript_footer_ok	 "....................................................\n" \
				 ".                                                  .\n" \
				 ". [+] Progression : [%V]      [%2F/%-2f]  .\n"  \
				 ".                                                  .\n"  \
				 "....................................................\n"
#define zipscript_footer_error	 ". - %-46v .\n" \
				 "....................................................\n"
#define zipscript_footer_skip	 "....................................................\n"

#define sfv_comment		 ""

/* How Indicators look
 */
#define incomplete_cd_indicator		"../../(iNCOMPLETE)-(%0)-%1"
#define incomplete_indicator		"../(iNCOMPLETE)-%0"

#define incomplete_nonfo_cd_indicator	"../../(iNCOMPLETE)-(no nfo)-(%0)-%1"
#define incomplete_nonfo_indicator	"../(iNCOMPLETE)-(no nfo)-%0"

#define progressmeter			"[%V] - %3.0p%% Complete - [PZS]"
#define del_progressmeter		"\\[.*] - ...% Complete - \\[PZS]"


/*
 *	Various settings
 *	================
 */

/* Enable/disable accept script (accept script is run, if file is ok)
 */
#define enable_accept_script	FALSE
#define accept_script		"/bin/imdb_parse.sh"
#define enable_accept_script2	FALSE
#define accept_script2		"/bin/imdb_parse2.sh"

/* Enable/disable nfo script (only run on .nfo files)
 */
#define enable_nfo_script	FALSE
#define nfo_script		"/bin/psxc-imdb.sh"
#define enable_nfo_script2	FALSE
#define nfo_script2		"/bin/psxc-imdb2.sh"

/* Enable/disable complete script (complete script is run, if release is complete)
 */
#define enable_complete_script	FALSE
#define complete_script		"/bin/nfo_copy.sh"
#define enable_complete_script2	FALSE
#define complete_script2	"/bin/nfo_copy2.sh"

/*
 *	Other binary settings
 *	=====================
 */

/* Output of racestats binary
 */
#define stats_line		"%r: %B%F%B of %B%f%B files uploaded by %B%u%B racer(s) from %B%g%B group(s). Leading uploader: %C0."

/* Output of postdel binary
 */
#define incompletemsg		"10%U deletes file from %r. Release is now %Bincomplete%B!"

