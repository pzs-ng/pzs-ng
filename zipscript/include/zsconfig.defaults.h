/* =======================
 * =                     =
 * = ZSCONFIG.H.DEFAULTS =
 * =                     =
 * =======================
 * THIS IS THE WRONG FILE TO EDIT!
 * DO NOT CHANGE ANYTHING HERE - COPY THE NECESSARY SETTINGS INTO YOUR
 * ZSCONFIG.H FILE
 */

/* --:START:-- */

/*
 * ZSCONFIG.H - A GUIDE
 * ---------------------
 */

/* zsconfig.h can include a number of options. Starting with pzs-ng beta 4, the
 * settings in zsconfig.h are overrides, meaning that not all of these settings
 * need be in zsconfig.h. Only the ones you change should be put there.
 * This provide a very flexible configuration, and upgrades should be less of a
 * hassle, since new settings in the config is no longer a concern - they will
 * be used with default settings.
 *
 * Here follow a summary of all settings. I hope this will provide some idea on
 * what the different things does. The format is as follow:
 *
 * <define> <option(s)>
 * 	Description
 * 	Default: <value>
 *
 * To add a define, for instance sitepath_dir, put the following in you zsconfig.h
 *
 * #define sitepath_dir	"/mysite/"
 *
 * ie, add '#define" in front, and the option(s) behind.
 *
 * 
 * -------------------------------------------------------------------------------
 */


/*
 *
 *
 * General Settings : Paths
 * ========================
 *
 * Here follow some important path-variables that define the structure of your site.
 *
 *
 */

/* 
 * sitepath_dir <PATH>
 * 	This is used as a base for datacleaner, when it's run as a cscript.
 * 	It is normally set to /site/, which is the common location in which all
 * 	your site's public dirs reside.
 */
#ifndef sitepath_dir
#define sitepath_dir	"/site/"
#endif

/* 
 * log <PATH>
 * 	This setting should point to a file where all logging will occur. The
 * 	usual place is glftpd's glftpd.log. This file must be world read/
 * 	writable (+rw).
 */ 
#ifndef log
#define log		"/ftp-data/logs/glftpd.log"
#endif

/*
 * storage <PATH>
 * 	This is the place where the zipscript keeps a 'mirror' of your site's
 * 	setup. Here stats for racers and releases are stored. This storage
 * 	should not be accessible to your site's users, but must be world
 * 	read/writable (+rwx).
 */ 
#ifndef storage
#define storage		"/ftp-data/pzs-ng/"
#endif

/*
 * gl_userfiles <PATH>
 * 	This is the place where userfiles are stored. The files must be world
 * 	readable (+r).
 */
#ifndef gl_userfiles
#define gl_userfiles	"/ftp-data/users/"
#endif

/*
 * dupepath <PATH>
 * 	This is the location of glftpd's dupefile. The file must be world
 * 	read/writable (+rw), and so must the containing dir (+rwx).
 */
#ifndef dupepath
#define dupepath	"/ftp-data/logs/dupefile"
#endif

/*
 * group_dirs <PATHS>
 * 	This is the place(s) where your site's groups/affilliates have space
 * 	provided. Normally each (release-)group have a designated area
 * 	accessible only to the group's members, in which they upload a new
 * 	release and 'pre' it (release it) simultanously on several sites.
 * 	Uploads and races in these dirs will not be logged. Dirs are separated
 * 	with the space character.
 */
#ifndef group_dirs
#define group_dirs	"/site/groups/"
#endif

/*
 * zip_dirs <PATHS>
 * 	This variable should hold a list of dirs where you expect zip-releases.
 * 	Dirs listed here will be logged. Dirs are separated by a space
 * 	character.
 */
#ifndef zip_dirs
#define zip_dirs	"/site/test/ /site/incoming/0day/"
#endif

/*
 * sfv_dirs <PATHS>
 * 	Similar to zip_dirs, this holds a list of dirs you expect to get sfv-
 * 	releases. Typical releases that use sfv is anything that is not zipped,
 * 	like rar, mp3, avi etc. Dirs listed here will be logged. Dirs are
 * 	separated by a space character.
 */
#ifndef sfv_dirs
#define sfv_dirs	"/site/test/ /site/incoming/mp3/ /site/incoming/games/ /site/incoming/apps/ /site/incoming/musicvideos/ /site/incoming/requests/"
#endif

/*
 * nocheck_dirs <PATHS>
 * 	Normally dirs in group/zip/sfv dirs are checked for corruption and
 * 	completeness. The dirs listed in this varaible will override paths
 * 	inside the previous dirs, and not check the files uploaded.
 * 	Unlike the original zipscript-c, pzs-ng will only check dirs listed
 * 	in group/zip/sfv dirs, and not the entire site. Dirs are separated
 * 	by a space character.
 */
#ifndef nocheck_dirs
#define nocheck_dirs	"/site/private/"
#endif

/*
 * speedtest_dirs <PATHS>
 * 	You may wish to have a place to perform speedtests. If someone uploads
 * 	files to dirs listed in this variable, the file is auto-deleted and a
 * 	message is written to glftpd.log.
 */
#ifndef speedtest_dirs
#define speedtest_dirs	"/site/speedtest/"
#endif

/*
 * noforce_sfv_first_dirs <PATHS>
 * 	The option 'force_sfv_first' forces sfv to be uploaded first in a
 * 	release. In some dirs, this may not be desiarable. To the rescue come
 * 	this option, which allow you to enter dirs that will be excluded from
 * 	the force.
 */
#ifndef noforce_sfv_first_dirs
#define noforce_sfv_first_dirs	"/site/incoming/requests/"
#endif

/*
 * audio_nocheck_dirs <PATHS>
 * 	Audio-files can be checked against a list of allowed/denied genres,
 * 	years etc. Audio-files found in paths defined in this variable will
 * 	not be checked against these defines, thus allowing them. Dirs are
 * 	separated by a space character.
 */
#ifndef audio_nocheck_dirs
#define audio_nocheck_dirs	"/site/groups/ /site/incoming/requests/"
#endif

/*
 * allowed_types_exemption_dirs <PATHS>
 * 	In allowed_types we define filetypes that are allowed on site w/o
 * 	any checks. Define here paths where these filetypes should be checked
 * 	*if* an sfv is uploaded.
 */
#ifndef allowed_types_exemption_dirs
#define allowed_types_exemption_dirs	"/site/incoming/musicvideos/"
#endif

/*
 * check_for_missing_nfo_dirs <PATHS>
 * 	Normally each release has a .nfo included. It has info about the
 * 	release written by the release-group, and may contain vital info
 * 	like serials etc. The paths defined in this value will be checked
 * 	for missing nfo after the release is marked as 'complete'. Separate
 * 	dirs with a space character.
 */
#ifndef check_for_missing_nfo_dirs
#define check_for_missing_nfo_dirs	"/site/incoming/games/ /site/incoming/apps/"
#endif

/*
 * cleanupdirs <PATHS>
 * 	To keep your site tidy and free from stale/dead symlinks created by the
 * 	zipscript, you should put here a list of dirs to scan for dead links.
 * 	The list is not recursive, and does not take wildcards.
 */
#ifndef cleanupdirs
#define cleanupdirs	"/site/test/ /site/incoming/games/ /site/incoming/apps/ /site/incoming/musicvideos/"
#endif

/*
 * cleanupdirs_dated <PATHS>
 * 	To keep your site tidy and free from stale/dead symlinks created by the
 * 	zipscript, you should put here a list of dirs to scan for dead links.
 * 	The list is not recursive, and does not take wildcards. This list can
 * 	contain format cookies for dated directories, refer to 'man strftime'
 * 	for a list of available date cookies.
 */
#ifndef cleanupdirs_dated
#define cleanupdirs_dated	"/site/incoming/0day/%m%d/ /site/incoming/mp3/%m%d/"
#endif

/*
 *
 *
 * General Settings : Files
 * ========================
 *
 * PZS-NG depend on some external files. Here follow a list.
 *
 *
 */

/*
 * GROUPFILE <PATH>
 * 	This is the location of glftpd's groupfile.
 */
#ifndef GROUPFILE
#define GROUPFILE	"/etc/group"
#endif

/*
 * PASSWDFILE <PATH>
 * 	This is the location of glftpd's passwd file.
 */
#ifndef PASSWDFILE
#define PASSWDFILE	"/etc/passwd"
#endif

/*
 * unzip_bin <PATH>
 * 	This is the location of the unzip binary needed to verify zip-releases.
 */
#ifndef unzip_bin
#define unzip_bin	"/bin/unzip"
#endif

/*
 * zip_bin <PATH>
 * 	This is the location of the zip binary needed to clean zip-releases for
 * 	unwanted files.
 */
#ifndef zip_bin
#define zip_bin		"/bin/zip"
#endif

/*
 * banned_filelist <PATH>
 * 	You can ban certain files on site, based on name. The chck is not
 * 	case-senistive, and wildcards may be used.
 * 	Put here the path to the text-file with a list of the files you do not
 * 	want on your site.
 * 	For more info, or upgraded lists, see http://project-sl.swecheck.net
 * 	Please note - chars like [ and ] are used in the wildcard scheme - if you
 * 	search for such chars in the filenames, you need to double-escape them,
 * 	ie \\[ and \\] and \\( etc.
 */
#ifndef banned_filelist
#define banned_filelist	"/ftp-data/misc/banned_filelist.txt"
#endif

/*
 *
 *
 * General Settings : Site
 * =======================
 *
 * Your site have some specific settings.
 *
 *
 */


/*
 * subdir_list <STRING>
 * 	This is a list of what is defined as subdirs. Dirs matching these
 * 	patterns will be considered subdirs, and follow a different pattern
 * 	regarding symlinks and nfo-rules etc.
 * 	Put the char '?' where a number or char may or may not be, but *only*
 * 	at the end of a dirname. Ie, cd?? will match cdA, cd12, but not cd123.
 * 	Do not put '?' at the beginning or in the middle - it will not work.
 * 	The string is not case-sensitive (dVd == DvD).
 */
#ifndef subdir_list
#define subdir_list	"cd??,disc??,disk??,dvd??,extra?,sub?,subtitle?,vobsub?,sample?,subpack?"
#endif

/*
 * gl_sections <STRING>
 * 	Here you should put a list of all sections on your site. Sections is
 * 	not the same as dirs, but is what you have defined as sections in
 * 	glftpd.conf. See glftpd's docs for more info on sections.
 * 	First section is normally named 'DEFAULT'. The order in which you put
 * 	the names of the sections are important. Separate sections with a
 * 	space character.
 */
#ifndef gl_sections
#define gl_sections	"DEFAULT"
#endif

/*
 * short_sitename <STRING>
 * 	Insert here the name of your site, short format. This will be used on
 * 	progressbars, completebars etc. This can be used in a cookie (%Z).
 */
#ifndef short_sitename
#define short_sitename	"NG"
#endif

/*
 * program_uid <NUMBER>
 * 	Some may want to run zipscript-c as a special user, which will be
 * 	set as 'owner' of links, missing files etc. Setting this to anything
 * 	but 0 will force the zipscript to change uid to the number you put
 * 	here. In order for this to work, zipscript-c needs the suid bit set.
 */
#ifndef program_uid
#define program_uid		0
#endif

/*
 * program_gid <NUMBER>
 * 	Same as with program_uid, only this is with the group.
 */
#ifndef program_gid
#define program_gid		0
#endif

/*
 * chmod_completebar <TRUE|FALSE>
 * 	When TRUE, the permissions on the completebar is changed to 0222. This is
 * 	to prevent it from being transfered on download/fxp.
 * 	Note that this will not happen in group-dirs.
 * 	Default: FALSE
 */
#ifndef chmod_completebar
#define	chmod_completebar		FALSE
#endif

/*
 *
 *
 * General Settings : Cross-type
 * =============================
 *
 * Below follow a list of settings that control the behaviour of PZS-NG
 *
 *
 */

/*
 * strict_path_match <TRUE|FALSE>
 * 	This setting is used to enforce the filetype (zip/sfv) based on path.
 * 	When set to TRUE, zip files is not allowed in sfv dirs, and the other
 * 	way around. group dirs are not affected, as they can be both.
 */
#ifndef strict_path_match
#define strict_path_match		FALSE
#endif

/*
 * check_for_banned_files <TRUE|FALSE>
 * 	Setting this to TRUE will check the filename against the list defined in
 * 	banned_filelist - if found, the file is marked as bad.
 */
#ifndef check_for_banned_files
#define check_for_banned_files		TRUE
#endif

/*
 * check_for_missing_nfo_filetypes <STRING|DISABLED>
 * 	Same as with check_for_missing_nfo_dirs above, but this is for
 * 	filetypes. Filetypes is not the same as file extensions, so the only
 * 	5 valid filetypes you can use here is:
 * 	rar,other,audio,video,zip
 * 	That's it - no others.
 */
#ifndef check_for_missing_nfo_filetypes
#define check_for_missing_nfo_filetypes	DISABLED
#endif

/*
 * create_missing_files <TRUE|FALSE>
 * 	To easily see what files are missing in a release, the zipscript can
 * 	create a 0-byte file of each missing file (filename-missing). This
 * 	file will be removed when the file is uploaded and found free of
 * 	errors.
 */
#ifndef create_missing_files
#define create_missing_files	TRUE
#endif

/*
 * status_bar_type <0|1>
 * 	In each dir there will be a statusbar. Here you define if you wish
 * 	to make it as a file (0) or as a dir (1).
 */
#ifndef status_bar_type
#define status_bar_type		1
#endif

/*
 * incompleteislink <0|1>
 * 	While a release is incomplete, a link will be created pointing to
 * 	the releasedir. You can define if it will be a file (0) or a
 * 	symbolic link (1).
 */
#ifndef incompleteislink
#define incompleteislink	1
#endif

/*
 * userellink <0|1>
 * 	With symbolic links you have an option of it being absolute (0) or
 * 	relative (1).
 */
#ifndef userellink
#define userellink		1
#endif

/*
 * allowed_types <STRING>
 * 	This variable holds a comma-separated list of extensions. All files
 * 	with any of these extensions are allowed and not checked.
 */
#ifndef allowed_types
#define allowed_types		"jpg,cue,m3u,mpg,mpeg,avi,txt,mov,vob"
#endif

/*
 * ignored_types <STRING>
 * 	Files found with an extension listed in this variable are denied on
 * 	upload, and ignored on rescan/postdel/etc. The list is comma-separated.
 * 	Defining a file with no extension can be done by starting the list
 * 	with a comma.
 */
#ifndef ignored_types
#define ignored_types		",diz,debug,message,imdb,html,url"
#endif

/*
 * mark_file_as_bad <TRUE|FALSE>
 * 	Normally files marked as bad are deleted. You can change this by setting
 * 	the following value to TRUE - instead of the file being deleted, it will
 * 	be renamed <filename>.bad. Bad files are ignored on rescan.
 */
#ifndef mark_file_as_bad
#define mark_file_as_bad		FALSE
#endif

/*
 * allow_file_resume <TRUE|FALSE>
 * 	Broken transfers may be a problem on some sites. Glftpd2 and pzs-ng
 * 	allow you to resume such files by setting this to TRUE.
 */
#ifndef allow_file_resume
#define allow_file_resume		FALSE
#endif

/*
 * sleep_on_bad <NUMBER>
 * 	Whenever a bad file is encountered, you can add a delay for the user
 * 	between 1 and 1000 seconds. Put the number of seconds glftpd should
 * 	sleep. Setting this to 0 ( or >1000) will disable this feature.
 */
#ifndef sleep_on_bad
#define sleep_on_bad			0
#endif

/*
 * test_for_password <TRUE|FALSE>
 * 	Some rarfiles have password protection. If you wish to check for such
 * 	files, set this to TRUE.
 */
#ifndef test_for_password
#define test_for_password		TRUE
#endif

/*
 *
 *
 * General Settings : Groupdirs
 * ============================
 *
 * Groupdir specific settings.
 *
 *
 */

/*
 * create_incomplete_links_in_group_dirs <TRUE|FALSE>
 * 	Should incomplete indicators be created in groupdirs? With the default
 * 	settings this should be no problem, but if you change the location of
 * 	the links, it may very well be.
 */
#ifndef create_incomplete_links_in_group_dirs
#define create_incomplete_links_in_group_dirs	TRUE
#endif

/*
 * write_complete_message_in_group_dirs <TRUE|FALSE>
 * 	Should a .message file be created in the group-dirs?
 */
#ifndef write_complete_message_in_group_dirs
#define write_complete_message_in_group_dirs	TRUE
#endif

/*
 * hide_group_uploaders <TRUE|FALSE>
 * 	If you wish to hide the names of the member of a releasegroup in the
 * 	.message file, this option will 'hide' the uploader.
 */
#ifndef hide_group_uploaders
#define hide_group_uploaders	TRUE
#endif

/*
 * hide_gname <STRING>
 * 	Set here something you wish to use instead of the groupname when
 * 	hiding users. Setting this empty will not change groupname.
 */
#ifndef hide_gname
#define hide_gname		""
#endif

/*
 * hide_uname <STRING>
 * 	Set here something you wish to use instead of the username when
 * 	hiding users. Setting this empty will make the username the same
 * 	as groupname.
 */
#ifndef hide_uname
#define hide_uname		""
#endif

/*
 * custom_group_dirs_complete_message <STRING|DISABLED>
 * 	If you rather would like to put in a predefined message in the
 * 	.message in group-dirs, define here what that message should be.
 * 	Setting this variable to DISABLED disables it.
 */
#ifndef custom_group_dirs_complete_message
#define custom_group_dirs_complete_message	"... Looks like this is a pre. :)"
#endif

/*
 *
 *
 * Audio Related Settings
 * ======================
 *
 * The following is more or less audio related. Ignore if you have no audio
 * releases on your site.
 *
 *
 */

/*
 * audio_genre_sort <TRUE|FALSE>
 * 	If you wish to sort audio/mp3 releases by genre, set this to TRUE.
 */
#ifndef audio_genre_sort
#define audio_genre_sort	FALSE
#endif

/*
 * audio_genre_path <PATH>
 * 	If you choose to sort audio/mp3 releases by genre, this is the place
 * 	where the symlinks will be created. This dir must be created in order
 * 	for the sorting to work - the zipscript will not create it for you.
 */
#ifndef audio_genre_path
#define audio_genre_path	"/site/incoming/music.by.genre/"
#endif

/*
 * audio_artist_sort <TRUE|FALSE>
 * 	If you wish to sort audio/mp3 releases by artist, set this to TRUE.
 */
#ifndef audio_artist_sort
#define audio_artist_sort	FALSE
#endif

/*
 * audio_artist_path <PATH>
 * 	If you choose to sort audio/mp3 releases by artist, this is the place
 * 	where the symlinks will be created. This dir must be created in order
 * 	for the sorting to work - the zipscript will not create it for you.
 */
#ifndef audio_artist_path
#define audio_artist_path	"/site/incoming/music.by.artist/"
#endif

/*
 * audio_year_sort <TRUE|FALSE>
 * 	If you wish to sort audio/mp3 releases by year, set this to TRUE.
 */
#ifndef audio_year_sort
#define audio_year_sort		FALSE
#endif

/*
 * audio_year_path <PATH>
 * 	If you choose to sort audio/mp3 releases by year, this is the place
 * 	where the symlinks will be created. This dir must be created in order
 * 	for the sorting to work - the zipscript will not create it for you.
 */
#ifndef audio_year_path
#define audio_year_path		"/site/incoming/music.by.year/"
#endif

/*
 * audio_group_sort <TRUE|FALSE>
 * 	If you wish to sort aduio/mp3 releases by group, set this to TRUE.
 */
#ifndef audio_group_sort
#define audio_group_sort	FALSE
#endif

/*
 * audio_group_path <PATH>
 * 	If you choose to sort audio/mp3 releases by group, this is the place
 * 	where the symlinks will be created. This dir must be created in order
 * 	for the sorting to work - the zipscript will not create it for you.
 */
#ifndef audio_group_path
#define audio_group_path	"/site/incoming/music.by.group/"
#endif

/*
 * allowed_constant_bitrates <STRING>
 * 	You can restrict uploaded audio/mp3 releases several ways. One is by
 * 	bitrate. Only CBR (Constant Bit Rate) mp3-files is of interest this
 * 	way. The list you define should be comma-separated.
 */
#ifndef allowed_constant_bitrates
#define allowed_constant_bitrates	"160,192"
#endif

/*
 * audio_cbr_check <TRUE|FALSE>
 * 	Should the zipscript check for allowed bitrate on cbr audio/mp3
 * 	releases? Set to TRUE if yes.
 */
#ifndef audio_cbr_check
#define audio_cbr_check		TRUE
#endif

/*
 * audio_cbr_warn <TRUE|FALSE>
 * 	If the audio-release is not in the list of allowed cbr bitrates, should
 * 	the zipscript log a warning message instead of deleting the file? Set
 * 	to TRUE if yes.
 */
#ifndef audio_cbr_warn
#define audio_cbr_warn		TRUE
#endif

/*
 * allowed_years <STRING>
 * 	Another way to restrict mp3-releases is by year. Define here a list
 * 	of years allowed on site. The list should be comma-separated.
 */
#ifndef allowed_years
#define allowed_years		"2000,2001,2002,2003,2004,2005"
#endif

/*
 * audio_year_check <TRUE|FALSE>
 * 	Should the zipscript check for allowed years on audio/mp3 releases? Set
 * 	to TRUE if yes.
 */
#ifndef audio_year_check
#define audio_year_check	TRUE
#endif

/*
 * audio_year_warn <TRUE|FALSE>
 * 	If the audio-release is not in the list of allowed years, should the
 * 	zipscript log a warning message instead of deleting the file? Set to
 * 	TRUE if yes.
 */
#ifndef audio_year_warn
#define audio_year_warn		TRUE
#endif

/*
 * banned_genres <STRING>
 * 	You can also restrict by genre. Use this variable to define genres
 * 	not allowed on your site. Genres are separated by a comma.
 */
#ifndef banned_genres
#define banned_genres		"Christian Rap,Christian Gangsta Rap,Contemporary Christian,Christian Rock"
#endif

/*
 * allowed_genres <STRING>
 * 	Instead of listing all denied genres, you can list all allowed genres.
 * 	Put here a comma-separated list of allowed genres.
 */
#ifndef allowed_genres
#define allowed_genres		"Top 40,Pop Funk,Rock,Pop"
#endif

/*
 * audio_banned_genre_check <TRUE|FALSE>
 * 	Should the audio/mp3 release be checked against your defined genres?
 * 	Only audio_banned_genre_check OR audio_allowed_genre_check should be
 * 	set to TRUE - not both.
 */
#ifndef audio_banned_genre_check
#define audio_banned_genre_check	TRUE
#endif

/*
 * audio_allowed_genre_check <TRUE|FALSE>
 * 	Should the audio/mp3 release be checked against your defined genres?
 * 	Only audio_banned_genre_check OR audio_allowed_genre_check should be
 * 	set to TRUE - not both.
 */
#ifndef audio_allowed_genre_check
#define audio_allowed_genre_check	FALSE
#endif

/*
 * audio_genre_warn <TRUE|FALSE>
 * 	If the audio-release is not part of allowed genres, should the
 * 	zipscript log a warning message instead of deleting the file? Set to
 * 	TRUE if yes.
 */
#ifndef audio_genre_warn
#define audio_genre_warn		TRUE
#endif

/*
 * audio_allowed_vbr_preset_check <TRUE|FALSE>
 * 	VBR mp3 releases are encoded using certain presets. If you wish to
 * 	enforce restrictions based on the preset, you can use this setting.
 */
#ifndef audio_allowed_vbr_preset_check
#define audio_allowed_vbr_preset_check	FALSE
#endif

/*
 * allowed_vbr_presets <STRING>
 * 	Enter here a comma separated list of presets you allow on site.
 * 	It is recommended you keep NA in the list, since the the zipscript may
 * 	fail to find the preset.
 */
#ifndef allowed_vbr_presets
#define allowed_vbr_presets		"APS,NA"
#endif

/*
 * audio_vbr_preset_warn <TRUE|FALSE>
 * 	If the audio-release is not part of allowed presets, should the
 * 	zipscript log a warning message instead of deleting the file? Set to
 * 	TRUE if yes.
 */
#ifndef audio_vbr_preset_warn
#define audio_vbr_preset_warn		TRUE
#endif

/*
 * del_banned_release <TRUE|FALSE>
 * 	If you want to make absolutely sure the banned release does not take up
 * 	more bandwith or space than necessary, you can choose to completely
 * 	remove the releasedir.
 * 	NOTE: This will override the *_warn setting - ie, you will get a warning
 * 	announce, but the file along with the entire dir will be removed.
 */
#ifndef del_banned_release
#define del_banned_release	FALSE
#endif

/*
 * create_m3u <TRUE|FALSE>
 * 	If you wish to create .m3u files on complete mp3-releases, set this to
 * 	TRUE.
 */
#ifndef create_m3u
#define create_m3u			TRUE
#endif

/*
 *
 *
 * NFO Related Settings
 * ====================
 *
 * The following is nfo related.
 *
 *
 */

/*
 * extract_nfo <TRUE|FALSE>
 * 	Should the zipscript extract the "real" nfo from a zip?
 */
#ifndef extract_nfo
#define extract_nfo			TRUE
#endif

/*
 * deny_nfo_upload_in_zip <TRUE|FALSE>
 * 	Should we deny nfo uploads in zip dirs? Should really only be used in
 * 	conjuntion with extract_nfo.
 */
#ifndef deny_nfo_upload_in_zip
#define deny_nfo_upload_in_zip		FALSE
#endif

/*
 * deny_double_nfo <TRUE|FALSE>
 * 	This is usually done (better) with a 3rd party app, but ... You can
 * 	deny a nfo upload if one already exists in the same dir.
 */
#ifndef deny_double_nfo
#define deny_double_nfo		FALSE
#endif

/*
 * deny_double_nfo_warn <TRUE|FALSE>
 * 	If you rather want the nfo to stay, to make sure the correct nfo is in
 * 	the dir, you can set this to TRUE - a msg will be logged in channel,
 * 	allowing the uploader (or and admin) to find out which of the two
 * 	nfo files are the correct one.
 * 	Note: deny_double_nfo needs to be set to TRUE for this option to take
 * 	effect.
 */
#ifndef deny_double_nfo_warn
#define deny_double_nfo_warn	TRUE
#endif

/*
 *
 *
 * SFV Related Settings
 * ====================
 *
 * The following is sfv related.
 *
 *
 */

/*
 * use_partial_on_noforce <TRUE|FALSE>
 * 	Usually all pathmatches are full paths. You can change this on what
 * 	you put in noforce_sfv_first_dirs to make it match parts of a dir,
 * 	like 'sample' or something. Please be careful to include a slash after
 * 	the dirs if this setting is on - like '/sample/', to minimize the
 * 	number of possible wrong hits.
 */
#ifndef use_partial_on_noforce
#define use_partial_on_noforce		FALSE
#endif


/*
 * sfv_calc_single_fname <TRUE|FALSE>
 * 	Setting this to TRUE enables the zipscript to calculate a crc checksum
 * 	for filenames listed in the sfv, which do not have a corresponding crc.
 * 	Normally only broken sfv's should have a missing crc after a filename.
 */
#ifndef sfv_calc_single_fname
#define	sfv_calc_single_fname	FALSE
#endif

/*
 * allow_files_not_in_sfv <TRUE|FALSE>
 * 	Should we allow files uploaded that is not in the sfv? This is only
 * 	valid on upload (not rescan etc), and is normally not used. Please
 * 	note that this is a override option - you may be not like everything
 * 	this option allows on your site...
 */
#ifndef allow_files_not_in_sfv
#define allow_files_not_in_sfv	FALSE
#endif

/*
 * strict_sfv_check <TRUE|FALSE>
 * 	Whenever "bad" entries are found in the sfv, they are usually ignored.
 * 	Setting this settingto true will mark the entire sfv as bad if such
 * 	entries are found, forcing the user to upload a valid sfv file.
 * 	Bad entries consist of lines containing the NULL char, or lines without
 * 	a valid crc code.
 */
#ifndef strict_sfv_check
#define strict_sfv_check	FALSE
#endif

/*
 * sfv_dupecheck <TRUE|FALSE>
 * 	Setting this option to TRUE will remove duplicate entries within the
 * 	uploaded sfv file.
 */
#ifndef sfv_dupecheck
#define sfv_dupecheck		TRUE
#endif

/*
 * sfv_cleanup <TRUE|FALSE>
 * 	Setting this to true will cleanup the uploaded sfv, removing comments
 * 	etc. from it. A copy of the original sfv will be kept in the storage-
 * 	dir. If you need to rescan, the original sfv will be used, and a new
 * 	cleaned version will be written in the releasedir.
 * 	Please note that if the sfv is removed from the releasedir, the backup
 * 	sfv is also removed.
 */
#ifndef sfv_cleanup
#define sfv_cleanup		TRUE
#endif

/*
 * sfv_cleanup_lowercase <TRUE|FALSE>
 * 	If sfv_cleanup is set to TRUE, this setting will also convert all
 * 	entries in the sfv to lowercase.
 * 	If sfv_cleanup is set to FALSE, this setting will only make -missing
 * 	pointers and the internal sfv database lowercased.
 * 	Either way, if set to TRUE you in effect turn winblows mode on, and the
 * 	zipscript will not separate case on filenames.
 */
#ifndef sfv_cleanup_lowercase
#define sfv_cleanup_lowercase	TRUE
#endif

/*
 * sfv_cleanup_crlf <TRUE|FALSE>
 * 	If sfv_cleanup is set to TRUE, this option defines if you use windows
 * 	or *nix version of linefeeds. The recommended option is to leave this
 * 	to TRUE, using windows style (<cr><lf>).
 */
#ifndef sfv_cleanup_crlf
#define sfv_cleanup_crlf	TRUE
#endif

/*
 * smart_sfv_write <TRUE|FALSE>
 * 	If the original sfv had bad entries, rewrite it to only contain the
 * 	good ones. If false marked bad files are uploaded, the sfv is also
 * 	updated.
 */
#ifndef smart_sfv_write
#define smart_sfv_write		FALSE
#endif

/*
 * deny_double_sfv <TRUE|FALSE>
 * 	With sfv releases, there should be only one sfv file allowed. This can
 * 	be done two ways - either by force or by being 'smart'. Setting this
 * 	option to TRUE will deny any new sfv in the dir, if a sfv already has
 * 	been uploaded. Setting it to FALSE will make the zipscript check the
 * 	new sfv first, compare it to the old sfv file and make a decision
 * 	based on the content of the sfv. If the new sfv contains more files
 * 	than the old one, and they match with what is already uploaded in the
 * 	dir, it will be allowed and used. If it does not, it will be denied.
 */
#ifndef deny_double_sfv
#define deny_double_sfv		FALSE
#endif


/*
 * exclude_non_sfv_dirs <TRUE|FALSE>
 * 	Applies only to audio/mp3 releases - NEEDS MORE INFO HERE!!!!!
 */
#ifndef exclude_non_sfv_dirs
#define exclude_non_sfv_dirs	TRUE
#endif

/*
 * force_sfv_first <TRUE|FALSE>
 * 	In sfv_dirs, you can either allow files to be uploaded by default and
 * 	checked later when the sfv arrives, or you can force the uploader to
 * 	first provide a sfv before any other filetype is allowed. Setting this
 * 	to TRUE will do the latter.
 */
#ifndef force_sfv_first
#define force_sfv_first		FALSE
#endif

/*
 *
 *
 * Debug Related Settings
 * ======================
 *
 * The following is debug related.
 *
 *
 */

/*
 * debug_mode <TRUE|FALSE>
 * 	If something seems weird/doesn't work, you should debug. Setting this
 * 	variable to TRUE will turn on debugging. A file named .debug will be
 * 	created, normally in the same dir as the upload-dir.
 */
#ifndef debug_mode
#define debug_mode		FALSE
#endif

/*
 * debug_level <INTEGER>
 *  This specifies the verbosity of the debug messages printed to .debug.
 *  A higher value means higher verbosity.
 */
#ifndef debug_level
#define debug_level		1
#endif

/*
 * debug_altlog <TRUE|FALSE>
 * 	If debug is on, you can choose wether you wish to have the .debug file
 * 	in the releasedir, or in the mirrored tree created in 'storage'. Please
 * 	note that if you choose to put the debug file in the 'storage' path,
 * 	some debug-info may be lost. Setting this to TRUE will use the
 * 	'storage' path, while FALSE will use the upload-dir.
 */
#ifndef debug_altlog
#define debug_altlog		FALSE
#endif

/*
 * debug_announce <TRUE|FALSE>
 * 	If this variable is set to TRUE, and debug_mode is set to TRUE the
 * 	zipscript will announce to the uploader that the script is running in
 * 	debug mode. The announce is done at the very beginning of the script,
 * 	which means that it should give some output no matter what, if it
 * 	manages to start. Good to find out if the script has actually been
 * 	executed.
 */
#ifndef debug_announce
#define debug_announce		FALSE
#endif

/*
 * benchmark_mode <TRUE|FALSE>
 * 	This option will show the user uploading an estimate of how long it
 * 	took the zipscript to scan the uploaded file.
 */
#ifndef benchmark_mode
#define benchmark_mode		TRUE
#endif

/*
 * ignore_zero_size <TRUE|FALSE>
 * 	This option is for debugging purposes only. Normally files of size
 * 	0 bytes are logged as bad, meaning glftpd will delete them. This
 * 	option makes it possible to allow such files.
 */
#ifndef ignore_zero_size
#define ignore_zero_size	FALSE
#endif

/*
 * remove_dot_debug_on_delete <TRUE|FALSE>
 * 	If you have debug mode on, users may or may not be able to see the
 * 	hidden files in the dir, and glftpd may be set up to deny the user
 * 	to delete other people's files (the .debug may be owned by a different
 * 	racer). Setting this to TRUE will try to remove the .debug file if
 * 	the dir itself is removed. Previously, people had to have access to
 * 	the wipe-command to be able to do this.
 */
#ifndef remove_dot_debug_on_delete
#define remove_dot_debug_on_delete	TRUE
#endif

/*
 *
 *
 * Zip Related Settings
 * ====================
 *
 * The following is zip related.
 *
 *
 */

/*
 * zip_clean <TRUE|FALSE>
 * 	If you wish to clean zipfiles (remove files based on the list given in
 * 	banned_filelist), set this to TRUE.
 */
#ifndef zip_clean
#define zip_clean		TRUE
#endif

/*
 *
 *
 * Lock Related Settings
 * =====================
 *
 * The following is lock related.
 *
 *
 */

/*
 * max_age_for_lock_file <NUMBER>
 * 	When critical data is read from and written to, the zipscript needs
 * 	to make sure things like multiple simultaneous writes to files corrupt
 * 	them. To prevent this, a file.lock is created, and while that file exists
 * 	no new process tries to read or write from the file. However, if a
 * 	process crashes while accessing a data file, the .lock file will still
 * 	be in the file system. This setting determines how old, in seconds, the
 * 	.lock file can be before it's ignored and deleted by a new process.
 */
#ifndef max_age_for_lock_file
#define max_age_for_lock_file	2
#endif

/*
 *
 *
 * Cleanup Related Settings
 * ========================
 *
 *
 */

/*
 * days_back_cleanup <NUMBER>
 * 	This setting defines how many days backward should be scanned on
 * 	cleanup. 0=disabled, 1=today only, 2=yesterday and today etc.
 */
#ifndef days_back_cleanup
#define days_back_cleanup	2
#endif

/*
 * always_scan_audio_syms <TRUE|FALSE>
 * 	On cleanup symlinks are removed. However, if cleanup is used as a
 * 	cscript, only current dir will be checked. You can make sure to
 * 	clean up broken audio-symlinks as well, by setting this variable to
 * 	TRUE. Because audio archives can get huge, this is not on by default.
 */
#ifndef always_scan_audio_syms
#define always_scan_audio_syms	FALSE
#endif

/*
 *
 *
 * Postdel Related Settings
 * ========================
 *
 *
 */

/*
 * remove_dot_files_on_delete <TRUE|FALSE>
 * 	If your site hides hidden files (files starting with a dot (''.''),
 * 	your users may have problems deleting dirs completely. Setting this
 * 	option to TRUE will remove all hidden files, when all other files of
 * 	the release is deleted.
 */
#ifndef remove_dot_files_on_delete
#define remove_dot_files_on_delete	FALSE
#endif


/*
 *
 *
 * Rescan Related Settings
 * =======================
 *
 *
 */

/*
 * ignore_zero_size_on_rescan <TRUE|FALSE>
 * 	Sometimes a release is approved, which usually is not according to
 * 	site rules. One of the ways to approve a dir is to create a 0-sized
 * 	file in the dir, with the word APPROVED or ALLOWED somewhere.
 * 	This setting will ignore all zero-sized files on rescan.
 */
#ifndef ignore_zero_sized_on_rescan
#define ignore_zero_sized_on_rescan	FALSE
#endif

/*
 * rescan_nocheck_dirs_allowed <TRUE|FALSE>
 * 	If you need to check a dir placed in a nocheck_dirs area, you must
 * 	set this variable to TRUE. Setting this to FALSE will render the rescan
 * 	option dead in nocheck_dirs areas.
 */
#ifndef rescan_nocheck_dirs_allowed
#define rescan_nocheck_dirs_allowed	TRUE
#endif

/*
 * mark_empty_dirs_as_incomplete_on_rescan <TRUE|FALSE>
 * 	If you wish to mark an empty dir as incomplete, set this to TRUE. Empty
 * 	dirs that are rescanned will then be marked as incomplete
 */
#ifndef mark_empty_dirs_as_incomplete_on_rescan
#define mark_empty_dirs_as_incomplete_on_rescan	FALSE
#endif

/*
 *
 *
 * ng-chown Related Settings
 * =========================
 *
 *
 */

/*
 * allow_uid_change_in_ng_chown <TRUE|FALSE>
 * 	ng-chown is a binary that changes the uid/gid of files dirs. In
 * 	order for it to work you have to enable a few things. This variable
 * 	will let ng-chown change the uid of files/dirs. ng-chown need to be
 * 	chmod'ed +s for it to work too.
 */
#ifndef allow_uid_change_in_ng_chown
#define allow_uid_change_in_ng_chown	FALSE
#endif

/*
 * allow_gid_change_in_ng_chown <TRUE|FALSE>
 * 	Same as with allow_uid_change_in_ng_chown, only with gid.
 */
#ifndef allow_gid_change_in_ng_chown
#define allow_gid_change_in_ng_chown	FALSE
#endif

/*
 * allow_dir_chown_in_ng_chown <TRUE|FALSE>
 * 	In ng-chown you have to specify a dir or a file to chmod. This setting
 * 	allows/denies to change the ownership of that file/dir.
 */
#ifndef allow_files_chown_in_ng_chown
#define allow_files_chown_in_ng_chown	FALSE
#endif

/*
 * allow_files_chown_in_ng_chown <TRUE|FALSE>
 * 	In ng-chown you have to specify a dir or a file to chmod. If you specify
 * 	a dir, this setting will chown the files in that dir, but not the dir
 * 	itself.
 */
#ifndef allow_dir_chown_in_ng_chown
#define allow_dir_chown_in_ng_chown	FALSE
#endif

/*
 * change_spaces_to_underscore_in_ng_chown <TRUE|FALSE>
 * 	Some ftp-clients have trouble reading the names of uid/gid if they have
 * 	spaces in them. This will search for a uid/gid with a underscore, if
 * 	the name (genre) given has a space inside. It is recommended to *NOT*
 * 	change this value (from TRUE).
 */
#ifndef change_spaces_to_underscore_in_ng_chown
#define change_spaces_to_underscore_in_ng_chown	TRUE
#endif

/*
 *
 *
 * Announce : Settings
 * ===================
 *
 *
 */

/*
 * enable_files_ahead <TRUE|FALSE>
 * 	To limit the amount of logging on races, you can choose to only report
 * 	a new leader in a race if a racer is leading with a certain amount of
 * 	files. Set to TRUE to enable this feature.
 */
#ifndef enable_files_ahead
#define enable_files_ahead	TRUE
#endif

/*
 * newleader_files_ahead <NUMBER>
 * 	Define here how many files ahead a racer must be before the zipscript
 * 	announces him as new leader.
 */
#ifndef newleader_files_ahead
#define newleader_files_ahead	2
#endif

/*
 * min_newleader_files <NUMBER>
 * 	Define here the minimum number of files in the RELEASE to announce a
 * 	new leader in a race.
 */
#ifndef min_newleader_files
#define min_newleader_files	1
#endif

/*
 * min_update_files <NUMBER>
 * 	Define here the minimum number of files in the RELEASE to announce an
 * 	update message.
 */
#ifndef min_update_files
#define min_update_files	2
#endif

/*
 * min_halfway_files <NUMBER>
 * 	Define here the minimum number of files in the RELEASE to announce a
 * 	halfway message.
 */
#ifndef min_halfway_files
#define min_halfway_files	10
#endif

/* * min_newleader_size <NUMBER>
 * 	Define here the minimum estimated total size (in MB) of RELEASE to
 * 	announce a new leader in a race.
 */
#ifndef min_newleader_size
#define min_newleader_size	10
#endif

/*
 * min_update_size <NUMBER>
 * 	Define here the minimum estimated total size (in MB) of RELEASE to
 * 	announce an update message.
 */
#ifndef min_update_size
#define min_update_size		1
#endif

/*
 * min_halfway_size <NUMBER>
 * 	Define here the minimum estimated total size (in MB) of RELEASE to
 * 	announce a halfway message.
 */
#ifndef min_halfway_size
#define min_halfway_size	10
#endif

/*
 * max_users_in_top <NUMBER>
 * 	Put here the maximum number of users to be logged in a race, when a
 * 	release is complete.
 */
#ifndef max_users_in_top
#define max_users_in_top	7
#endif

/*
 * max_groups_in_top <NUMBER>
 * 	Put here the maximum number of groups to be logged in a race, when a
 * 	release is complete.
 */
#ifndef max_groups_in_top
#define max_groups_in_top	7
#endif

/*
 * announce_norace <TRUE|FALSE>
 * 	If this variable is set to FALSE, announces with only one racer will
 * 	not be announced.
 */
#ifndef announce_norace
#define announce_norace		TRUE
#endif

/*
 * get_competitor_list <TRUE|FALSE>
 * 	This setting, if set to TRUE, will put a list of all racers
 * 	excluding the current user into a special cookie.
 */
#ifndef get_competitor_list
#define get_competitor_list	TRUE
#endif

/*
 * get_user_stats <TRUE|FALSE>
 * 	If this is set to true, the zipscript will try to fetch stats from the
 * 	racers userfiles.
 */
#ifndef get_user_stats
#define get_user_stats		TRUE
#endif

/*
 * show_stats_from_pos2_only <TRUE|FALSE>
 * 	If this is set to true, only racers in position 2 and below is shown
 * 	in the %T and %t cookies - this affects the LOOP variables in the bot.
 */
#ifndef show_stats_from_pos2_only
#define show_stats_from_pos2_only	FALSE
#endif

/*
 * show_user_info <TRUE|FALSE>
 * 	If you wish to print stat info after a file is uploaded to the racer,
 * 	set this to true. It will display racers (usernames) and stats on the
 * 	current race in the client's ftp program.
 */
#ifndef show_user_info
#define show_user_info		TRUE
#endif

/*
 * show_group_info <TRUE|FALSE>
 * 	If you wish to print stat info after a file is uploaded to the racer,
 * 	set this to true. It will display racers (groupnames) and stats on the
 * 	current race in the client's ftp program.
 */
#ifndef show_group_info
#define show_group_info		TRUE
#endif

/*
 * write_complete_message <TRUE|FALSE>
 * 	Should a file be created in the releasedir, containing race-info etc?
 */
#ifndef write_complete_message
#define write_complete_message	TRUE
#endif

/*
 * message_file_name <STRING>
 * 	What name should the complete_message file have?
 */
#ifndef message_file_name
#define message_file_name	".message"
#endif

/*
 *
 *
 * Announce : Output
 * =================
 *
 *
 */
 
 /*
 * message_header <STRING|DISABLED>
 * 	Define here what should be put in the top of the complete_message.
 */
#ifndef message_header
#define message_header		".--== PZS-NG v1.1 ====-----------------------------------------------.\n"
#endif

/*
 * message_user_header <STRING|DISABLED>
 * 	Define here what should be put above a list of racers (users).
 */
#ifndef message_user_header
#define message_user_header	"|                          U S E R T O P                             |\n+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
#endif

/*
 * message_user_body <STRING|DISABLED>
 * 	Define here the body of the list of racers (users).
 */
#ifndef message_user_body
#define message_user_body	"| %3n %-29U %7.1mM %4fF %5.1p%% %7.0sKBs |\n"
#endif

/*
 * message_user_footer <STRING|DISABLED>
 * 	Define here the footer of the list of racers (users).
 */
#ifndef message_user_footer
#define message_user_footer	"+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n| %3u Total %31.1mM %4fF 100.0%% %7.0aKBs |\n"
#endif

/*
 * message_group_header <STRING|DISABLED>
 * 	Define here what should be put above a list of racers (groups).
 */
#ifndef message_group_header
#define message_group_header	"|                                                                    |\n|                          G R O U P T O P                           |\n+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
#endif

/*
 * message_group_body <STRING|DISABLED>
 * 	Define here the body of the list of racers (groups).
 */
#ifndef message_group_body
#define message_group_body	"| %3n %-29g %7.1mM %4fF %5.1p%% %7.0sKBs |\n"
#endif

/*
 * message_group_footer <STRING|DISABLED>
 * 	Define here the footer of the list of racers (groups).
 */
#ifndef message_group_footer
#define message_group_footer	"+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n| %3g Total %31.1mM %4fF 100.0%% %7.0aKBs |\n"
#endif

/*
 * message_mp3 <STRING|DISABLED>
 * 	Define here a special complete_message for mp3-releases.
 */
#ifndef message_mp3
#define message_mp3		"|                                                                    |\n|                                                                    |\n|                           I D 3 - T A G                            |\n+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n| Artist : %-57x |\n| Album  : %-57W |\n| Genre  : %-34w Rate  : %3Xkbit/s      |\n| Tracks : %-34f Year  : %-14Y |\n| Encoder: %-34i Preset: %-14I |\n"
#endif

/*
 * message_footer <STRING|DISABLED>
 * 	Define here what should be put at the bottom of the complete_message
 * 	file.
 */
#ifndef message_footer
#define message_footer		"`-------------------------------------------------------===========--'\n"
#endif

/*
 * video_completebar <STRING|DISABLED>
 * 	When a release is complete, a 'complete bar' will be created. Define
 * 	here how it should look like on video releases.
 */
#ifndef video_completebar
#define video_completebar	"[%Z] - ( %.0mM %fF - COMPLETE ) - [%Z]"
#endif

/*
 * audio_completebar <STRING|DISABLED>
 * 	When a release is complete, a 'complete bar' will be created. Define
 * 	here how it should look like on audio releases.
 */
#ifndef audio_completebar
#define audio_completebar	"[%Z] - ( %.0mM %fF - COMPLETE - %w %Y ) - [%Z]"
#endif

/*
 * rar_completebar <STRING|DISABLED>
 * 	When a release is complete, a 'complete bar' will be created. Define
 * 	here how it should look like on rar releases.
 */
#ifndef rar_completebar
#define rar_completebar		"[%Z] - ( %.0mM %fF - COMPLETE ) - [%Z]"
#endif

/*
 * zip_completebar <STRING|DISABLED>
 * 	When a release is complete, a 'complete bar' will be created. Define
 * 	here how it should look like on zip releases.
 */
#ifndef zip_completebar
#define zip_completebar		"[%Z] - ( %.0mM %fF - COMPLETE ) - [%Z]"
#endif

/*
 * other_completebar <STRING|DISABLED>
 * 	When a release is complete, a 'complete bar' will be created. Define
 * 	here how it should look like on other releases.
 */
#ifndef other_completebar
#define other_completebar	"[%Z] - ( %.0mM %fF - COMPLETE ) - [%Z]"
#endif

/*
 * del_completebar <STRING|DISABLED>
 * 	This setting is what the zipscript will search for (and delete) when
 * 	it updates the complete bar.
 */
#ifndef del_completebar
#define del_completebar		"^\\[.*] - \\( .*F - COMPLETE.*) - \\[.*]$"
#endif

/*
 * zipscript_header <STRING>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown as the head of that message.
 */
#ifndef zipscript_header
#define zipscript_header	".-== PZS-NG Zipscript-C ==-------------------------.\n"
#endif

/*
 * zipscript_sfv_ok <STRING>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown as the body of that message on sfv-file-
 * 	upload.
 */
#ifndef zipscript_sfv_ok
#define zipscript_sfv_ok	"| + SFV-file: oK!                                  |\n"
#endif

/*
 * zipscript_any_ok <STRING>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown as the body of that message on any ok
 * 	file.
 */
#ifndef zipscript_any_ok
#define zipscript_any_ok	"| + File: ok!                                      |\n"
#endif

/*
 * zipscript_SFV_ok <STRING>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown as the body of that message on sfv-
 * 	checked file.
 */
#ifndef zipscript_SFV_ok
#define zipscript_SFV_ok	"| + CRC-Check: oK!                                 |\n"
#endif

/*
 * zipscript_SFV_skip <STRING>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown as the body of that message when sfv
 * 	has not yet been uploaded.
 */
#ifndef zipscript_SFV_skip
#define zipscript_SFV_skip	"| + CRC-Check: SKIPPED!                            |\n"
#endif

/*
 * zipscript_zip_ok <STRING>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown as the body of that message on zipfile
 * 	upload.
 */
#ifndef zipscript_zip_ok
#define zipscript_zip_ok	"| + ZiP integrity: oK!                             |\n"
#endif

/*
 * realtime_mp3_info <STRING|DISABLED>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown as the body of that message on mp3-file
 * 	upload.
 */
#ifndef realtime_mp3_info
#define realtime_mp3_info	"+-=[ID3tag]=------------------------===------------+\n| Title  : %-39y |\n| Artist : %-39x |\n| Album  : %-39W |\n| Genre  : %-39w |\n| Year   : %-39Y |\n"
#endif

/*
 * realtime_user_header <STRING|DISABLED>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the body of that message, head of
 * 	user-stats.
 */
#ifndef realtime_user_header
#define realtime_user_header	"+-=[UserTop]=-------------------===----------------+\n"
#endif

/*
 * realtime_user_body <STRING|DISABLED>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the body of that message, body of
 * 	user-stats.
 */
#ifndef realtime_user_body
#define realtime_user_body	"| %2n. %-15u %8.1mmB %4fF %7.0skB/s |\n"
#endif

/*
 * realtime_user_footer <STRING|DISABLED>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the body of that message, footer of
 * 	user-stats.
 */
#ifndef realtime_user_footer
#define realtime_user_footer	DISABLED
#endif

/*
 * realtime_group_header <STRING|DISABLED>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the body of that message, head of
 * 	group-stats.
 */
#ifndef realtime_group_header
#define realtime_group_header	"+-=[GroupTop]=--------------------===--------------+\n"
#endif

/*
 * realtime_group_body <STRING|DISABLED>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the body of that message, body of
 * 	group-stats.
 */
#ifndef realtime_group_body
#define realtime_group_body	"| %2n. %-15g %8.1mmB %4fF %7.0skB/s |\n"
#endif

/*
 * realtime_group_footer <STRING|DISABLED>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the body of that message, footer of
 * 	group-stats.
 */
#ifndef realtime_group_footer
#define realtime_group_footer	DISABLED
#endif

/*
 * zipscript_footer_unknown <STRING>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the footer of that message, on uploads
 * 	where no sfv is uploaded yet.
 */
#ifndef zipscript_footer_unknown
#define zipscript_footer_unknown	"`-[ - NO - SFV - ]----------------------[%3F/???]--'\n"
#endif

/*
 * zipscript_footer_ok <STRING>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the footer of that message, on uploads
 * 	where the file is verified ok.
 */
#ifndef zipscript_footer_ok
#define zipscript_footer_ok	"`-[%V]----------------------[%3F/%-3f]--'\n"
#endif

/*
 * zipscript_footer_error <STRING>
 * 	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the footer of that message, on uploads
 * 	where the file has error.
 */
#ifndef zipscript_footer_error
#define zipscript_footer_error	"| - %-46v |\n`-----------------------------------========-------'\n"
#endif

/*
 * zipscript_footer_skip <STRING>
 *   	When files are uploaded, some output is shown to the racer uploading.
 * 	Put here what should be shown in the footer of that message, on uploads
 * 	where the file has skipped checking.
 */
#ifndef zipscript_footer_skip
#define zipscript_footer_skip	"`--------------------------------=====-------------'\n"
#endif

/*
 *
 *
 * Announce : Indicators
 * =====================
 *
 *
 */

/*
 * incomplete_cd_indicator <STRING>
 * 	On upload an indicator is created after each file, to show still
 * 	incomplete releases. Put here how that indicator will look.
 * 	This variable defines indicators on multi-cd releases.
 */
#ifndef incomplete_cd_indicator
#define incomplete_cd_indicator		"../../(incomplete)-(%0)-%1"
#endif

/*
 * incomplete_indicator <STRING>
 * 	On upload an indicator is created after each file, to show still
 * 	incomplete releases. Put here how that indicator will look.
 * 	This variable defines indicators on normal releases.
 */
#ifndef incomplete_indicator
#define incomplete_indicator		"../(incomplete)-%0"
#endif

/*
 * incomplete_base_nfo_indicator <STRING>
 * 	On upload an indicator is created after each file, to show still
 * 	incomplete releases. Put here how that indicator will look.
 * 	This variable defines indicators on missing nfo in multi-cd releases.
 */
#ifndef incomplete_base_nfo_indicator
#define incomplete_base_nfo_indicator	"../../(no-nfo)-%1"
#endif

/*
 * incomplete_nfo_indicator <STRING>
 * 	On upload an indicator is created after each file, to show still
 * 	incomplete releases. Put here how that indicator will look.
 * 	This variable defines indicators on missing nfo in normal releases.
 */
#ifndef incomplete_nfo_indicator
#define incomplete_nfo_indicator	"../(no-nfo)-%0"
#endif

/*
 * progressmeter <STRING>
 * 	On upload a complete-bar is created after each file, to show how
 * 	complete the release is. Put here how that indicator will look.
 */
#ifndef progressmeter
#define progressmeter			"[%V] - %3.0p%% Complete - [%Z]"
#endif

/*
 * del_progressmeter <STRING>
 * 	On upload a complete-bar is created after each file, to show how
 * 	complete the release is. Put here what to search for when updating
 * 	that bar - the file(s) matching this will be deleted.
 */
#ifndef del_progressmeter
#define del_progressmeter		"^\\[.*] - .*%% Complete - \\[.*]$"
#endif

/*
 * charbar_missing <CHAR>
 * 	The progressbar consist of two chars, which together marks the progress
 * 	of the release. Put here the char to be used to mark missing files.
 */
#ifndef charbar_missing
#define charbar_missing			":"
#endif

/*
 * charbar_filled <CHAR>
 * 	The progressbar consist of two chars, which together marks the progress
 * 	of the release. Put here the char to be used to mark uploaded files.
 */
#ifndef charbar_filled
#define charbar_filled			"#"
#endif

/*
 *
 *
 * Event Related Settings
 * ======================
 *
 *
 */

/*
 * enable_accept_script <TRUE|FALSE>
 * 	An external script can be run after a file is verified and ok. Put
 * 	this to TRUE to run the external script.
 */
#ifndef enable_accept_script
#define enable_accept_script	FALSE
#endif

/*
 * accept_script <PATH>
 * 	Put here the path to the external script which should be run after
 * 	a file is verified and ok.
 */
#ifndef accept_script
#define accept_script		"/bin/imdb_parse.sh"
#endif

/*
 * enable_nfo_script <TRUE|FALSE>
 * 	An external script can be run after a nfo upload. Put this to TRUE to
 * 	run the external script.
 *	The script will also be executed if an nfo is extracted.
 */
#ifndef enable_nfo_script
#define enable_nfo_script	FALSE
#endif

/*
 * nfo_script <PATH>
 * 	Put here the path to the external script which should be run after a
 * 	nfo upload.
 */
#ifndef nfo_script
#define nfo_script		"/bin/psxc-imdb.sh"
#endif

/*
 * enable_complete_script <TRUE|FALSE>
 * 	An external script can be run after a release is complete. Put this to
 * 	TRUE to run the external script.
 */
#ifndef enable_complete_script
#define enable_complete_script	FALSE
#endif

/*
 * complete_script <PATH>
 * 	Put here the path to the external script which should be run after a
 * 	release is marked complete.
 */
#ifndef complete_script
#define complete_script		"/bin/nfo_copy.sh"
#endif

/*
 * enable_mp3_script <TRUE|FALSE>
 * 	An external script can be run after a mp3-file is uploaded. Put this to
 * 	TRUE to run the external script.
 */
#ifndef enable_mp3_script
#define enable_mp3_script	FALSE
#endif

/*
 * mp3_script <PATH>
 * 	Put here the path to the external script which should be run after
 * 	first uploaded mp3 in an audio release.
 */
#ifndef mp3_script
#define mp3_script		"/bin/ng-chown"
#endif

/*
 * mp3_script_cookies <STRING>
 * 	Put here what should be used as args for the external script after
 * 	first uploaded mp3 in an audio release.
 */
#ifndef mp3_script_cookies
#define mp3_script_cookies	"0 0 0 1 0 1 - \"%w\" \"%?\""
#endif

/*
 * enable_banned_script <TRUE|FALSE>
 * 	An external script can be run after a file is marked as banned (usually
 * 	mp3-files, based on genre/year/etc). Put this to TRUE to run the
 * 	external script.
 */
#ifndef enable_banned_script
#define enable_banned_script	FALSE
#endif

/*
 * banned_script <PATH>
 * 	Put here the path to the external script which should be run if a
 * 	banned file is found.
 */
#ifndef banned_script
#define banned_script		"/bin/mp3-ban.sh"
#endif

/*
 * enable_unduper_script <TRUE|FALSE>
 * 	If you allow sfv to be uploaded after other files, some files may be
 * 	marked as bad when the sfv is read and compared to the crc of the file.
 * 	The file is then deleted, and a new one needs be uploaded. In order
 * 	for that to happen, the file needs to be unduped, or it will be denied
 * 	by glftpd. Set this variable to TRUE to run an external script after
 * 	a file has been marked as bad, and which undupes it.
 */
#ifndef enable_unduper_script
#define enable_unduper_script	TRUE
#endif

/*
 * unduper_script <PATH>
 * 	Enter the name of the script doing the actual unduping of the file.
 */
#ifndef unduper_script
#define unduper_script		"/bin/ng-undupe"
#endif

/* --:END:-- */

