#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifndef HAVE_STRLCPY
# include "strl/strl.h"
#endif

#include "helpfunctions.h"
#include "zsfunctions.h"
#include "race-file.h"
#include "objects.h"
#include "macros.h"

#include "constants.h"

#include "errors.h"
#include "multimedia.h"
#include "convert.h"
#include "dizreader.h"
#include "stats.h"
#include "complete.h"
#include "crc.h"
#include "ng-version.h"
//#include "avi.h"
//#include "audio.h"
//#include "video.h"
//#include "avinfo.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifdef _SunOS_
#include "scandir.h"
#include "strsep.h"
#endif

/* Store various message strings here */
typedef struct _MSG {
	char *complete;
	char *update;
	char *race;
	char *sfv;
	char *newleader;
	char *halfway;
	char *error;
} MSG;

int handle_zip(GLOBAL *, MSG *, DIR *);
int handle_sfv(GLOBAL *, MSG *, DIR *);
int handle_nfo(GLOBAL *, MSG *, DIR *);
int handle_sfv32(GLOBAL *, MSG *, DIR *, char **, char *);
void read_envdata(GLOBAL *, GDATA *, UDATA *, struct stat *);
void check_filesize(GLOBAL *, const char *, struct stat *);
void lock_release(GLOBAL *, DIR *, DIR *);

int 
main(int argc, char **argv)
{
	GLOBAL		g; /* this motherfucker owns */
	MSG		msg;
	GDATA		gdata;
	UDATA		udata;

	DIR		*dir, *parent;
	
	char           *fileext = NULL, *name_p, *temp_p = NULL;
	char           *target = 0;
	char           *update_type = 0;
	char           *complete_bar = 0;
	int				exit_value = EXIT_SUCCESS;
	int				no_check = FALSE;
	char	       *race_type = 0;
	char	       *newleader_type = 0;
	char	       *race_halfway_type = 0;
	char	       *norace_halfway_type = 0;
	char	       *inc_point[2];
	char           *complete_announce = 0;
	int		cnt, n = 0;
#if ( enable_complete_script || enable_accept_script )
	int		nfofound = 0;
#endif
#if ( del_banned_release || enable_banned_script )
	int		deldir = 0;
#endif
	struct stat	fileinfo;

#if ( benchmark_mode == TRUE )
	struct timeval	bstart, bstop;
	d_log("zipscript-c: Reading time for benchmark\n");
	gettimeofday(&bstart, (struct timezone *)0);
#endif

#if ( debug_mode == TRUE && debug_announce == TRUE)
	printf("PZS-NG: Running in debug mode.\n");
#endif

	/*
	 * Adding version-number to head if .debug message 15.09.2004 - psxc
	 */
	d_log("zipscript-c: Project-ZS Next Generation (pzs-ng) v%s debug log.\n", ng_version());

#ifdef _ALT_MAX
	d_log("zipscript-c: PATH_MAX not found - using predefined settings! Please report to the devs!\n");
#endif

	umask(0666 & 000);

#if ( program_uid > 0 )
	d_log("zipscript-c: Trying to change effective gid\n");
	setegid(program_gid);
	d_log("zipscript-c: Trying to change effective uid\n");
	seteuid(program_uid);
#endif

	if (argc != 4) {
		printf(" - - PZS-NG ZipScript-C v%s - -\n\nUsage: %s <filename> <path> <crc>\n\n", ng_version(), argv[0]);
		exit(1);
	}
	d_log("zipscript-c: Clearing arrays\n");
	bzero(&g.v.total, sizeof(struct race_total));
	g.v.misc.slowest_user[0] = 30000;
	g.v.misc.fastest_user[0] = g.v.misc.release_type = RTYPE_NULL;

	/* gettimeofday(&g.v.transfer_stop, (struct timezone *)0 ); */

	g.v.misc.write_log = TRUE;
	g.v.misc.in_sfvfile = FALSE;

	strlcpy(g.v.file.name, argv[1], NAME_MAX);
	strlcpy(g.l.path, argv[2], PATH_MAX);
	strlcpy(g.v.misc.current_path, g.l.path, PATH_MAX);
	d_log("zipscript-c: Changing directory to %s\n", g.l.path);
	chdir(g.l.path);

	d_log("zipscript-c: Reading data from environment variables\n");
	read_envdata(&g, &gdata, &udata, &fileinfo);
	g.v.file.speed *= 1024;

	d_log("zipscript-c: Checking the file size of %s\n", g.v.file.name);
	check_filesize(&g, g.v.file.name, &fileinfo);

	d_log("zipscript-c: Setting race times\n");
	if (g.v.file.size != 0)
		g.v.total.start_time = g.v.total.stop_time - ((unsigned int)(g.v.file.size) / g.v.file.speed);
	else
		g.v.total.start_time = g.v.total.stop_time > (g.v.total.stop_time - 1) ? g.v.total.stop_time : (g.v.total.stop_time -1);
	if ((int)(g.v.total.stop_time - g.v.total.start_time) < 1)
		g.v.total.stop_time = g.v.total.start_time + 1;

	n = (g.l.length_path = (int)strlen(g.l.path)) + 1;

	d_log("zipscript-c: Allocating memory for variables\n");
	g.l.race = ng_realloc2(g.l.race, n += 10 + (g.l.length_zipdatadir = sizeof(storage) - 1), 1, 1, 1);
	g.l.sfv = ng_realloc2(g.l.sfv, n, 1, 1, 1);
	g.l.leader = ng_realloc2(g.l.leader, n, 1, 1, 1);
	target = ng_realloc2(target, n + 256, 1, 1, 1);
	g.ui = ng_realloc2(g.ui, sizeof(struct USERINFO *) * 30, 1, 1, 1);
	g.gi = ng_realloc2(g.gi, sizeof(struct GROUPINFO *) * 30, 1, 1, 1);

	d_log("zipscript-c: Copying data g.l into memory\n");
	sprintf(g.l.sfv, storage "/%s/sfvdata", g.l.path);
	sprintf(g.l.leader, storage "/%s/leader", g.l.path);
	sprintf(g.l.race, storage "/%s/racedata", g.l.path);
	g.v.user.pos = 0;
	sprintf(g.v.misc.old_leader, "none");
	g.v.file.compression_method = '5';

	/* Get file extension */

	d_log("zipscript-c: Parsing file extension from filename... (%s)\n", argv[1]);
	for (temp_p = name_p = argv[1]; *name_p != 0; name_p++) {
		if (*name_p == '.') {
			temp_p = name_p;
		}
	}
	
	if (*temp_p != '.') {
		d_log("zipscript-c: Got: no extension\n");
		temp_p = name_p;
	} else {
		d_log("zipscript-c: Got: %s\n", temp_p);
		temp_p++;
	}
	name_p++;

#if ( sfv_cleanup_lowercase == TRUE )
	d_log("zipscript-c: Copying (lowercased version of) extension to memory\n");
#else
	d_log("zipscript-c: Copying (unchanged version of) extension to memory\n");
#endif
	fileext = ng_realloc(fileext, name_p - temp_p, 1, 1, &g.v, 1);
	memcpy(fileext, temp_p, name_p - temp_p);
#if ( sfv_cleanup_lowercase == TRUE )
	strtolower(fileext);
#endif
	d_log("zipscript-c: Reading directory structure\n");
	dir = opendir(".");
	parent = opendir("..");

	d_log("zipscript-c: Caching release name\n");
	getrelname(&g);

	d_log("zipscript-c: Creating directory to store racedata in\n");
	maketempdir(g.l.path);

	d_log("zipscript-c: Locking release\n");
	lock_release(&g, dir, parent);

	printf(zipscript_header);

	/* Hide users in group_dirs */
	if (matchpath(group_dirs, g.l.path) && (hide_group_uploaders == TRUE)) {
		d_log("zipscript-c: Hiding user in group-dir:\n");
		if ((int)strlen(hide_gname) > 0) {
			snprintf(g.v.user.group, 18, "%s", hide_gname);
			d_log("zipscript-c:    Changing groupname\n");
		}
		if ((int)strlen(hide_uname) > 0) {
			snprintf(g.v.user.name, 18, "%s", hide_uname);
			d_log("zipscript-c:    Changing username\n");
		}
		if ((int)strlen(hide_uname) == 0) {
			d_log("zipscript-c:    Making username = groupname\n");
			snprintf(g.v.user.name, 18, "%s", g.v.user.group);
		}
	}
	/* Empty file recieved */
#if (ignore_zero_size == FALSE )
	if (g.v.file.size == 0) {
		d_log("zipscript-c: File seems to be 0\n");
		sprintf(g.v.misc.error_msg, EMPTY_FILE);
		mark_as_bad(g.v.file.name);
		msg.error = convert(&g.v, g.ui, g.gi, bad_file_msg);
		if (exit_value < 2)
			writelog(&g, msg.error, bad_file_0size_type);
		exit_value = 2;
	}
#endif
	/* No check directories */
	if (matchpath(nocheck_dirs, g.l.path) || matchpath(speedtest_dirs, g.l.path) || (!matchpath(zip_dirs, g.l.path) && !matchpath(sfv_dirs, g.l.path) && !matchpath(group_dirs, g.l.path))) {
		d_log("zipscript-c: Directory matched with nocheck_dirs, or is not among sfv/zip/group dirs\n");
		d_log("zipscript-c:   - nocheck_dirs  : '%s'\n", nocheck_dirs);
		d_log("zipscript-c:   - speedtest_dirs: '%s'\n", speedtest_dirs);
		d_log("zipscript-c:   - zip_dirs      : '%s'\n", zip_dirs);
		d_log("zipscript-c:   - sfv_dirs      : '%s'\n", sfv_dirs);
		d_log("zipscript-c:   - group_dirs    : '%s'\n", group_dirs);
		d_log("zipscript-c:   - current path  : '%s'\n", g.l.path);
		no_check = TRUE;
	} else {
		/* Process file */
		d_log("zipscript-c: Verifying old racedata\n");
		if (!verify_racedata(g.l.race, &g.v))
			d_log("zipscript-c:   Failed to open racedata - assuming this is a new race.\n");

		switch (get_filetype(&g, fileext)) {
		
		case 0:	/* ZIP CHECK */
			exit_value = handle_zip(&g, &msg, dir);
			break;

		case 1:	/* SFV CHECK */
			exit_value = handle_sfv(&g, &msg, dir);
			break;
			
		case 2:	/* NFO CHECK */
			exit_value = handle_nfo(&g, &msg, dir);
			break;
		
		case 3:	/* SFV BASED CRC-32 CHECK */
			exit_value = handle_sfv32(&g, &msg, dir, argv, fileext);
			break;

		case 4:	/* ACCEPTED FILE */

			d_log("zipscript-c: File type: NO CHECK\n");
			no_check = TRUE;
			break;
			/* END OF ACCEPTED FILE CHECK */

		case 255:	/* UNKNOWN - WE DELETE THESE, SINCE IT WAS
				 * ALSO IGNORED */
			d_log("zipscript-c: File type: UNKNOWN [ignored in sfv]\n");
			sprintf(g.v.misc.error_msg, UNKNOWN_FILE, fileext);
			mark_as_bad(g.v.file.name);
			msg.error = convert(&g.v, g.ui, g.gi, bad_file_msg);
			if (exit_value < 2)
				writelog(&g, msg.error, bad_file_disallowed_type);
			exit_value = 2;
			break;
			/* END OF UNKNOWN CHECK */
		}
	}

	if (no_check == TRUE) {	/* File was not checked */
		printf(zipscript_any_ok);
		printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_skip));
		if (matchpath(speedtest_dirs, g.l.path)) {
			d_log("zipscript-c: writing speedtest to channel\n");
			writelog(&g, convert(&g.v, g.ui, g.gi, speed_announce), speed_type);
			exit_value = 2;
		}

	} else if (exit_value == EXIT_SUCCESS) {	/* File was checked */

		switch (g.v.misc.release_type) {
			case RTYPE_RAR:
				race_type = rar_announce_race_type;
				newleader_type = rar_announce_newleader_type;
				update_type = rar_announce_update_type;
				norace_halfway_type = rar_announce_norace_halfway_type;
				race_halfway_type = rar_announce_race_halfway_type;
				complete_bar = rar_completebar;
				msg.complete = CHOOSE(g.v.total.users, rar_complete, rar_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
				break;	/* rar */
			case RTYPE_OTHER:
				race_type = other_announce_race_type;
				newleader_type = other_announce_newleader_type;
				update_type = other_announce_update_type;
				norace_halfway_type = other_announce_norace_halfway_type;
				race_halfway_type = other_announce_race_halfway_type;
				complete_bar = other_completebar;
				msg.complete = CHOOSE(g.v.total.users, other_complete, other_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, other_announce_race_complete_type, other_announce_norace_complete_type);
				break;	/* other */
			case RTYPE_AUDIO:
				race_type = audio_announce_race_type;
				newleader_type = audio_announce_newleader_type;
				if (!g.v.audio.is_vbr)
					update_type = audio_announce_cbr_update_type;
				else
					update_type = audio_announce_vbr_update_type;
				norace_halfway_type = audio_announce_norace_halfway_type;
				race_halfway_type = audio_announce_race_halfway_type;
				complete_bar = audio_completebar;
				msg.complete = CHOOSE(g.v.total.users, audio_complete, audio_norace_complete);
				if (!g.v.audio.is_vbr) {
					complete_announce = CHOOSE(g.v.total.users, audio_cbr_announce_race_complete_type, audio_cbr_announce_norace_complete_type);
				} else {
					complete_announce = CHOOSE(g.v.total.users, audio_vbr_announce_race_complete_type, audio_vbr_announce_norace_complete_type);
				}
				break;	/* audio */
			case RTYPE_VIDEO:
				race_type = video_announce_race_type;
				newleader_type = video_announce_newleader_type;
				update_type = video_announce_update_type;
				norace_halfway_type = video_announce_norace_halfway_type;
				race_halfway_type = video_announce_race_halfway_type;
				complete_bar = video_completebar;
				msg.complete = CHOOSE(g.v.total.users, video_complete, video_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, video_announce_race_complete_type, video_announce_norace_complete_type);
				break;	/* video */
			case RTYPE_NULL:
				race_type = zip_announce_race_type;
				newleader_type = zip_announce_newleader_type;
				update_type = zip_announce_update_type;
				norace_halfway_type = zip_announce_norace_halfway_type;
				race_halfway_type = zip_announce_race_halfway_type;
				complete_bar = zip_completebar;
				msg.complete = CHOOSE(g.v.total.users, zip_complete, zip_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, zip_announce_race_complete_type, zip_announce_norace_complete_type);
				break;	/* zip */
			default:
				race_type = rar_announce_race_type;
				newleader_type = rar_announce_newleader_type;
				update_type = rar_announce_update_type;
				norace_halfway_type = rar_announce_norace_halfway_type;
				race_halfway_type = rar_announce_race_halfway_type;
				complete_bar = rar_completebar;
				msg.complete = CHOOSE(g.v.total.users, rar_complete, rar_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
				d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (3:%d)\n", g.v.misc.release_type);
				break;	/* rar */
//			}
		}
		if (g.v.total.users > 0) {
			d_log("zipscript-c: Sorting race stats\n");
			sortstats(&g.v, g.ui, g.gi);
#if ( get_user_stats == TRUE )
			d_log("zipscript-c: Reading day/week/month/all stats for racers\n");
			d_log("zipscript-c: stat section: %i\n", g.v.section);
			get_stats(&g.v, g.ui);
#endif
			d_log("zipscript-c: Printing on-site race info\n");
			showstats(&g.v, g.ui, g.gi);

			if (!enable_files_ahead || ((g.v.total.users > 1 && g.ui[g.ui[0]->pos]->files >= (g.ui[g.ui[1]->pos]->files + newleader_files_ahead)) || g.v.total.users == 1)) {
				d_log("zipscript-c: Writing current leader to file\n");
				read_write_leader(g.l.leader, &g.v, g.ui[g.ui[0]->pos]);
			}
			if (g.v.total.users > 1) {
				if (g.ui[g.v.user.pos]->files == 1 && msg.race != NULL) {
					d_log("zipscript-c: Writing RACE to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.race), race_type);
				}
				if (g.v.total.files >= min_newleader_files && ((g.v.total.size * g.v.total.files) >= (min_newleader_size * 1024 * 1024)) && strcmp(g.v.misc.old_leader, g.ui[g.ui[0]->pos]->name) && msg.newleader != NULL && g.ui[g.ui[0]->pos]->files >= (g.ui[g.ui[1]->pos]->files + newleader_files_ahead) && g.v.total.files_missing) {
					d_log("zipscript-c: Writing NEWLEADER to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.newleader), newleader_type);
				}
			} else {
				if (g.ui[g.v.user.pos]->files == 1 && g.v.total.files >= min_update_files && ((g.v.total.size * g.v.total.files) >= (min_update_size * 1024 * 1024)) && msg.update) {
					d_log("zipscript-c: Writing UPDATE to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.update), update_type);
				}
			}
		}
		if (g.v.total.files_missing > 0) {
			if (g.v.total.files_missing == g.v.total.files >> 1 && g.v.total.files >= min_halfway_files && ((g.v.total.size * g.v.total.files) >= (min_halfway_size * 1024 * 1024)) && msg.halfway != NULL) {
				d_log("zipscript-c: Writing HALFWAY to %s\n", log);
				writelog(&g, convert(&g.v, g.ui, g.gi, msg.halfway), (g.v.total.users > 1 ? race_halfway_type : norace_halfway_type));
			}
			d_log("zipscript-c: Caching progress bar\n");
			buffer_progress_bar(&g.v);

			if (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs) {
				d_log("zipscript-c: Creating incomplete indicator:\n", g.l.incomplete);
				d_log("zipscript-c:    name: '%s', incomplete: '%s', path: '%s'\n", g.v.misc.release_name, g.l.incomplete, g.l.path);
				create_incomplete();
			}

			d_log("zipscript-c: Creating/moving progress bar\n");
			move_progress_bar(0, &g.v, g.ui, g.gi);
			printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_ok));
		} else if ((g.v.total.files_missing == 0) && (g.v.total.files > 0)) {
			/* Release is complete */
			d_log("zipscript-c: Caching progress bar\n");
			buffer_progress_bar(&g.v);
			printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_ok));
			d_log("zipscript-c: Setting complete pointers\n");
			if ( g.v.misc.release_type == RTYPE_AUDIO ) {
				d_log("zipscript-c: Symlinking audio\n");
				if (!strncasecmp(g.l.link_target, "VA", 2) && (g.l.link_target[2] == '-' || g.l.link_target[2] == '_'))
					memcpy(g.v.audio.id3_artist, "VA", 3);

				if (!matchpath(group_dirs, g.l.path)) {
#if ( audio_genre_sort == TRUE )
					d_log("zipscript-c:   Sorting mp3 by genre (%s)\n", g.v.audio.id3_genre);
					createlink(audio_genre_path, g.v.audio.id3_genre, g.l.link_source, g.l.link_target);
#endif
#if ( audio_artist_sort == TRUE )
					d_log("zipscript-c:   Sorting mp3 by artist\n");
					if (*g.v.audio.id3_artist) {
						d_log("zipscript-c:     - artist: %s\n", g.v.audio.id3_artist);
						if (memcmp(g.v.audio.id3_artist, "VA", 3)) {
							temp_p = ng_realloc(temp_p, 2, 1, 1, &g.v, 1);
							snprintf(temp_p, 2, "%c", toupper(*g.v.audio.id3_artist));
							createlink(audio_artist_path, temp_p, g.l.link_source, g.l.link_target);
							ng_free(temp_p);
						} else
							createlink(audio_artist_path, "VA", g.l.link_source, g.l.link_target);
					}
#endif
#if ( audio_year_sort == TRUE )
					d_log("zipscript-c:   Sorting mp3 by year (%s)\n", g.v.audio.id3_year);
					if (!*g.v.audio.id3_year != 0)
						createlink(audio_year_path, g.v.audio.id3_year, g.l.link_source, g.l.link_target);
#endif
#if ( audio_group_sort == TRUE )
					d_log("zipscript-c:   Sorting mp3 by group\n");
					temp_p = remove_pattern(g.l.link_target, "*-", RP_LONG_LEFT);
					temp_p = remove_pattern(temp_p, "_", RP_SHORT_LEFT);
					n = (int)strlen(temp_p);
					if (n > 0 && n < 15) {
						d_log("zipscript-c:   - Valid groupname found: %s (%i)\n", temp_p, n);
						createlink(audio_group_path, temp_p, g.l.link_source, g.l.link_target);
					}
#endif
				}
#if ( create_m3u == TRUE )
				if (findfileext(dir, ".sfv")) {
					d_log("zipscript-c: Creating m3u\n");
					cnt = sprintf(target, findfileext(dir, ".sfv"));
					strlcpy(target + cnt - 3, "m3u", 4);
					create_indexfile(g.l.race, &g.v, target);
				} else
					d_log("zipscript-c: Cannot create m3u, sfv is missing\n");
#endif
			}
			if (complete_bar) {
				d_log("zipscript-c: Removing old complete bar, if any\n");
				removecomplete();
			}

			d_log("zipscript-c: Removing incomplete indicator (%s)\n", g.l.incomplete);
			complete(&g);

			if (msg.complete != NULL) {
				d_log("zipscript-c: Writing COMPLETE and STATS to %s\n", log);
				writelog(&g, convert(&g.v, g.ui, g.gi, msg.complete), complete_announce);
			}
			if (complete_bar) {
				d_log("zipscript-c: Creating complete bar\n");
				createstatusbar(convert(&g.v, g.ui, g.gi, complete_bar));
#if (chmod_completebar)
				if (!matchpath(group_dirs, g.l.path))
					chmod(convert(&g.v, g.ui, g.gi, complete_bar), 0222);
				else
					d_log("zipscript-c: we are in a group_dir - will not chmod the complete bar.\n");
#endif
			}

#if ( enable_complete_script == TRUE )
			nfofound = (int)findfileext(dir, ".nfo");
			if (!fileexists(complete_script))
				d_log("zipscript-c: Warning - complete_script (%s) - file does not exists\n", complete_script);
			d_log("zipscript-c: Executing complete script\n");
			sprintf(target, complete_script " \"%s\"", g.v.file.name);
			if (execute(target))
				d_log("zipscript-c: Failed to execute complete_script: %s\n", strerror(errno));

#if ( enable_nfo_script == TRUE )
			if (!nfofound && findfileext(dir, ".nfo")) {
				if (!fileexists(nfo_script))
					d_log("zipscript-c: Warning - nfo_script (%s) - file does not exists\n", nfo_script);
				d_log("zipscript-c: Executing nfo script (%s)\n", nfo_script);
				sprintf(target, nfo_script " \"%s\"", g.v.file.name);
				if (execute(target))
					d_log("zipscript-c: Failed to execute nfo_script: %s\n", strerror(errno));
			}
#endif
#endif
			if (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs) {
				/* Creating no-nfo link if needed. */
				n = 0;
				if (check_for_missing_nfo_filetypes) {
					switch (g.v.misc.release_type) {
						case RTYPE_RAR:
							if (strcomp(check_for_missing_nfo_filetypes, "rar"))
								n = 1;
							break;
						case RTYPE_OTHER:
							if (strcomp(check_for_missing_nfo_filetypes, "other"))
								n = 1;
							break;
						case RTYPE_AUDIO:
							if (strcomp(check_for_missing_nfo_filetypes, "audio"))
								n = 1;
							break;
						case RTYPE_VIDEO:
							if (strcomp(check_for_missing_nfo_filetypes, "video"))
								n = 1;
							break;
						case RTYPE_NULL:
							if (strcomp(check_for_missing_nfo_filetypes, "zip"))
								n = 1;
							break;
					}
				}
				if ((g.l.nfo_incomplete) && (!findfileext(dir, ".nfo")) && (matchpath(check_for_missing_nfo_dirs, g.l.path) || n) ) {
					if (!g.l.in_cd_dir) {
						d_log("zipscript-c: Creating missing-nfo indicator %s.\n", g.l.nfo_incomplete);
						create_incomplete_nfo();
					} else if (!findfileextparent(parent, ".nfo")) {
						d_log("zipscript-c: Creating missing-nfo indicator (base) %s.\n", g.l.nfo_incomplete);
						/* This is not pretty, but should be functional. */
						if ((inc_point[0] = find_last_of(g.l.path, "/")) != g.l.path)
							*inc_point[0] = '\0';
						if ((inc_point[1] = find_last_of(g.v.misc.release_name, "/")) != g.v.misc.release_name)
							*inc_point[1] = '\0';
						create_incomplete_nfo();
						if (*inc_point[0] == '\0')
							*inc_point[0] = '/';
						if (*inc_point[1] == '\0')
							*inc_point[1] = '/';
					}
				}
			}
		} else {

			/* Release is at unknown state */

			g.v.total.files = -g.v.total.files_missing;
			g.v.total.files_missing = 0;
			printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_unknown));
		}
	} else {
		/* File is marked to be deleted */

		d_log("zipscript-c: Logging file as bad\n");
		remove_from_race(g.l.race, g.v.file.name, &g.v);
		//writerace(g.l.race, &g.v, 0, F_BAD);

		printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_error));
	}
#if ( enable_accept_script == TRUE )
	if (exit_value == EXIT_SUCCESS) {
		nfofound = (int)findfileext(dir, ".nfo");
		if (!fileexists(accept_script)) {
			d_log("zipscript-c: Warning - accept_script (%s) - file does not exists\n", accept_script);
		}
		d_log("zipscript-c: Executing accept script\n");
		sprintf(target, accept_script " \"%s\"", g.v.file.name);
		if (execute(target) != 0)
			d_log("zipscript-c: Failed to execute accept_script: %s\n", strerror(errno));

#if ( enable_nfo_script == TRUE )
		if (!nfofound && findfileext(dir, ".nfo")) {
			if (!fileexists(nfo_script)) {
				d_log("zipscript-c: Warning - nfo_script (%s) - file does not exists\n", nfo_script);
			}
			d_log("zipscript-c: Executing nfo script (%s)\n", nfo_script);
			sprintf(target, nfo_script " \"%s\"", g.v.file.name);
			if (execute(target) != 0)
				d_log("zipscript-c: Failed to execute nfo_script: %s\n", strerror(errno));
		}
#endif
	}
#endif
	if ((findfileext(dir, ".nfo") || (findfileextparent(parent, ".nfo"))) && (g.l.nfo_incomplete)) {
		d_log("zipscript-c: Removing missing-nfo indicator (if any)\n");
		remove_nfo_indicator(&g);
	}

#if ( del_banned_release || enable_banned_script )
	if (deldir) {
		if (enable_banned_script) {
			sprintf(target, banned_script " \"%s\"", g.v.file.name);
			if (execute(target) != 0)
				d_log("zipscript-c: Failed to execute banned_script: %s\n", strerror(errno));
		}
		if (del_banned_release) {
			d_log("zipscript-c: del_banned_release is set - removing entire dir.\n");
			move_progress_bar(1, &g.v, g.ui, g.gi);
			if (g.l.incomplete)
				unlink(g.l.incomplete);
			rewinddir(dir);
			del_releasedir(dir, g.l.path);
		}
	}
#endif

	d_log("zipscript-c: Releasing memory and removing lock\n");
	closedir(dir);
	closedir(parent);
	remove_lock(&g.v);

	if (fileexists(".delme"))
		unlink(".delme");

	buffer_groups(&gdata, GROUPFILE, 1);
	buffer_users(&udata, PASSWDFILE, 1);
	updatestats_free(&g);
	ng_free(fileext);
	ng_free(target);
	ng_free(g.l.race);
	ng_free(g.l.sfv);
	ng_free(g.l.leader);

#if ( benchmark_mode == TRUE )
	gettimeofday(&bstop, (struct timezone *)0);
	printf("Checks completed in %0.6f seconds\n", ((bstop.tv_sec - bstart.tv_sec) + (bstop.tv_usec - bstart.tv_usec) / 1000000.));
	d_log("zipscript-c: Checks completed in %0.6f seconds\n", ((bstop.tv_sec - bstart.tv_sec) + (bstop.tv_usec - bstart.tv_usec) / 1000000.));
#endif

#if ( sleep_on_bad > 0 && sleep_on_bad < 1001 )
	if (exit_value == 2) {
		if (sleep_on_bad == 117) {
			exit_value = 126;
		} else {
			exit_value = sleep_on_bad + 10;
		}
		d_log("zipscript-c: Sleeping for %d seconds.\n", sleep_on_bad);
	}
#endif
	d_log("zipscript-c: Exit %d\n", exit_value);
	return exit_value;
}

int
handle_zip(GLOBAL *g, MSG *msg, DIR *dir) {

	//char           *error_msg = 0;
	unsigned char	exit_value = EXIT_SUCCESS;
	char			target[strlen(unzip_bin) + 10 + NAME_MAX];
	char 			*fileext;
	long		loc;
	struct dirent	*dp;

	d_log("zipscript-c: File type is: ZIP\n");
	d_log("zipscript-c: Testing file integrity with %s\n", unzip_bin);
	if (!fileexists(unzip_bin)) {
		d_log("zipscript-c: ERROR! Not able to check zip-files - %s does not exists!\n", unzip_bin);
		sprintf(g->v.misc.error_msg, BAD_ZIP);
		mark_as_bad(g->v.file.name);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_zip_type);
		exit_value = 2;
		return exit_value;
	} else {
#if (test_for_password || extract_nfo)
		if ((!findfileextcount(dir, ".nfo") || findfileextcount(dir, ".zip") == 1) && !mkdir(".unzipped", 0777)) {
			sprintf(target, "%s -qqjo \"%s\" -d .unzipped", unzip_bin, g->v.file.name);
		} else
			sprintf(target, "%s -qqt \"%s\"", unzip_bin, g->v.file.name);
#else
		sprintf(target, "%s -qqt \"%s\"", unzip_bin, g->v.file.name);
#endif
		if (execute(target) != 0) {
			d_log("zipscript-c: Integrity check failed (#%d): %s\n", errno, strerror(errno));
			sprintf(g->v.misc.error_msg, BAD_ZIP);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_zip_type);
			exit_value = 2;
			return exit_value;
		}
#if (test_for_password || extract_nfo)
			if (!findfileextcount(dir, ".nfo") || findfileextcount(dir, ".zip") == 1)
				check_zipfile(".unzipped");
#endif
	}
	d_log("zipscript-c: Integrity ok\n");
	printf(zipscript_zip_ok);

	if ((matchpath(zip_dirs, g->l.path)) || (matchpath(group_dirs, g->l.path))  ) {
		d_log("zipscript-c: Directory matched with zip_dirs/group_dirs\n");
	} else {
		d_log("zipscript-c: WARNING! Directory did not match with zip_dirs/group_dirs\n");
		if (strict_path_match == TRUE) {
			d_log("zipscript-c: Strict mode on - exiting\n");
			sprintf(g->v.misc.error_msg, UNKNOWN_FILE, fileext);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_wrongdir_type);
			exit_value = 2;
			return exit_value;
		}
	}
	if (!fileexists("file_id.diz")) {
		d_log("zipscript-c: file_id.diz does not exist, trying to extract it from %s\n", g->v.file.name);
		sprintf(target, "%s -qqjnCLL \"%s\" file_id.diz 2>.delme", unzip_bin, g->v.file.name);
		if (execute(target) != 0)
			d_log("zipscript-c: No file_id.diz found (#%d): %s\n", errno, strerror(errno));
		else {
			if ((loc = findfile(dir, "file_id.diz.bad"))) {
				seekdir(dir, loc);
				dp = readdir(dir);
				unlink(dp->d_name);
			}
			chmod("file_id.diz", 0666);
		}
	}
	d_log("zipscript-c: Reading diskcount from diz:\n");
	g->v.total.files = read_diz("file_id.diz");
	d_log("zipscript-c:    Expecting %d files.\n", g->v.total.files);

	if (g->v.total.files == 0) {
		d_log("zipscript-c:    Could not get diskcount from diz.\n");
		g->v.total.files = 1;
		unlink("file_id.diz");
	}
	g->v.total.files_missing = g->v.total.files;

	d_log("zipscript-c: Storing new race data\n");
	writerace(g->l.race, &g->v, 0, F_CHECKED);
	d_log("zipscript-c: Reading race data from file to memory\n");
	readrace(g->l.race, &g->v, g->ui, g->gi);
	if (g->v.total.files_missing < 0) {
		d_log("zipscript-c: There seems to be more files in zip than we expected\n");
		g->v.total.files -= g->v.total.files_missing;
		g->v.total.files_missing = 0;
		g->v.misc.write_log = FALSE;
	}
	d_log("zipscript-c: Setting message pointers\n");
	msg->race = zip_race;
	msg->update = zip_update;
	msg->halfway = CHOOSE2(g->v.total.users, zip_halfway, zip_norace_halfway);
	msg->newleader = zip_newleader;

	return exit_value;
}

int
handle_sfv(GLOBAL *g, MSG *msg, DIR *dir) {

	char			*fileext;
	unsigned char	exit_value = EXIT_SUCCESS;
	struct dirent	*dp;
	int		cnt, cnt2;
	char	       *ext = 0;
	char	       *sfv_type = 0;

	d_log("zipscript-c: File type is: SFV\n");
	if ((matchpath(sfv_dirs, g->l.path)) || (matchpath(group_dirs, g->l.path))  ) {
		d_log("zipscript-c: Directory matched with sfv_dirs/group_dirs\n");
	} else {
		d_log("zipscript-c: WARNING! Directory did not match with sfv_dirs/group_dirs\n");
		if (strict_path_match == TRUE) {
			d_log("zipscript-c: Strict mode on - exiting\n");
			sprintf(g->v.misc.error_msg, UNKNOWN_FILE, fileext);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_wrongdir_type);
			exit_value = 2;
			return exit_value;
		}
	}

	if (fileexists(g->l.sfv)) {
		if (deny_double_sfv == TRUE && findfileextcount(dir, ".sfv") > 1 && sfv_compare_size(".sfv", g->v.file.size) > 0) {
			d_log("zipscript-c: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g->v.file.size));
			d_log("zipscript-c: No double sfv allowed\n");
			msg->error = convert(&g->v, g->ui, g->gi, deny_double_msg);
			writelog(g, msg->error, general_doublesfv_type);
			sprintf(g->v.misc.error_msg, DOUBLE_SFV);
			mark_as_bad(g->v.file.name);
			exit_value = 2;
			return exit_value;
		} else if (findfileextcount(dir, ".sfv") > 1 && sfv_compare_size(".sfv", g->v.file.size) > 0) {
			d_log("zipscript-c: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g->v.file.size));
			d_log("zipscript-c: Reading remainders of old sfv\n");
			readsfv(g->l.sfv, &g->v, 1);
			cnt = g->v.total.files - g->v.total.files_missing;
			cnt2 = g->v.total.files;
			g->v.total.files_missing = g->v.total.files = 0;
			//readsfv_ffile(g->v.file.name, g->v.file.size, g->v);
			readsfv_ffile(&g->v);
			if ((g->v.total.files <= cnt2) || (g->v.total.files != (cnt + g->v.total.files_missing))) {
				d_log("zipscript-c: Old sfv seems to match with more files than current one\n");
				strlcpy(g->v.misc.error_msg, "SFV does not match with files!", 80);
				msg->error = convert(&g->v, g->ui, g->gi, deny_double_msg);
				writelog(g, msg->error, general_doublesfv_type);
				sprintf(g->v.misc.error_msg, DOUBLE_SFV);
				mark_as_bad(g->v.file.name);
				exit_value = 2;
				return exit_value;
			}
			g->v.total.files = g->v.total.files_missing = 0;
		} else {
			d_log("zipscript-c: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g->v.file.size));
			d_log("zipscript-c: Hmm.. Seems the old .sfv was deleted. Allowing new one.\n");
			unlink(g->l.race);
			unlink(g->l.sfv);
			rewinddir(dir);
			while ((dp = readdir(dir))) {
				cnt = cnt2 = (int)strlen(dp->d_name);
				ext = dp->d_name;
				while (ext[cnt] != '-' && cnt > 0)
					cnt--;
				if (ext[cnt] != '-')
					cnt = cnt2;
				else
					cnt++;
				ext += cnt;
				if (!strncmp(ext, "missing", 7))
					unlink(dp->d_name);
			}
		}
	}
	d_log("zipscript-c: Parsing sfv and creating sfv data\n");
	if (copysfv(g->v.file.name, g->l.sfv, &g->v, g->l.path, 0)) {
		d_log("zipscript-c: Found invalid entries in SFV.\n");
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_sfv_type);
		mark_as_bad(g->v.file.name);
		exit_value = 2;
		sprintf(g->v.misc.error_msg, EMPTY_SFV);
		unlink(g->l.race);
		unlink(g->l.sfv);

		rewinddir(dir);
		while ((dp = readdir(dir))) {
			cnt = cnt2 = (int)strlen(dp->d_name);
			ext = dp->d_name;
			while (ext[cnt] != '-' && cnt > 0)
				cnt--;
			if (ext[cnt] != '-')
				cnt = cnt2;
			else
				cnt++;
			ext += cnt;
			if (!strncmp(ext, "missing", 7))
				unlink(dp->d_name);
		}

		return exit_value;
	}

#if (use_partial_on_noforce == TRUE)
	if ( (force_sfv_first == FALSE) || matchpartialpath(noforce_sfv_first_dirs, g->l.path))
#else
	if ( (force_sfv_first == FALSE) || matchpath(noforce_sfv_first_dirs, g->l.path))
#endif
	{
		if (fileexists(g->l.race)) {
			d_log("zipscript-c: Testing files marked as untested\n");
			testfiles(&g->l, &g->v, 0);
		}
	}
	d_log("zipscript-c: Reading file count from SFV\n");
	readsfv(g->l.sfv, &g->v, 0);

#if (smart_sfv_write && sfv_cleanup)
	d_log("zipscript-c: Rewriting sfv file according to smart_sfv_write\n");
	sfvdata_to_sfv(g->l.sfv, findfileext(dir, ".sfv"));
#endif
	
	if (g->v.total.files == 0) {
		d_log("zipscript-c: SFV seems to have no files of accepted types, or has errors.\n");
		sprintf(g->v.misc.error_msg, EMPTY_SFV);
		mark_as_bad(g->v.file.name);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_sfv_type);
		exit_value = 2;
		return exit_value;
	}
	printf(zipscript_sfv_ok);
	if (fileexists(g->l.race)) {
		d_log("zipscript-c: Reading race data from file to memory\n");
		readrace(g->l.race, &g->v, g->ui, g->gi);
	}
	if (del_completebar) {
		d_log("zipscript-c: Making sure that release is not marked as complete\n");
		removecomplete();
	}
	d_log("zipscript-c: Setting message pointers\n");
	sfv_type = general_announce_sfv_type;
	switch (g->v.misc.release_type) {
	case RTYPE_RAR:
		msg->sfv = rar_sfv;
		sfv_type = rar_announce_sfv_type;
		break;
	case RTYPE_OTHER:
		msg->sfv = other_sfv;
		sfv_type = other_announce_sfv_type;
		break;
	case RTYPE_AUDIO:
		msg->sfv = audio_sfv;
		sfv_type = audio_announce_sfv_type;
		break;
	case RTYPE_VIDEO:
		msg->sfv = video_sfv;
		sfv_type = video_announce_sfv_type;
		break;
	default :
		msg->sfv = rar_sfv;
		sfv_type = rar_announce_sfv_type;
		d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (1:%d)\n", g->v.misc.release_type);
		break;
	}

	if (!msg->sfv)
		d_log("zipscript-c: Something's messed up - msg->sfv not set!\n");

	msg->halfway = msg->newleader = msg->race = msg->update = NULL;

	if (g->v.total.files_missing > 0) {
		if (msg->sfv != NULL) {
			d_log("zipscript-c: Writing SFV message to %s\n", log);
			writelog(g, convert(&g->v, g->ui, g->gi, msg->sfv), sfv_type);
		}
	} else {
		if (g->v.misc.release_type == RTYPE_AUDIO) {
			d_log("zipscript-c: Reading audio info for completebar\n");
			get_mpeg_audio_info(findfileext(dir, ".mp3"), &g->v.audio);
		}
	}

	return exit_value;
}

int
handle_nfo(GLOBAL *g, MSG *msg, DIR *dir) {
	
	unsigned char	no_check = FALSE;
	unsigned char	exit_value = EXIT_SUCCESS;

#if ( enable_nfo_script)
	char           target[1024];
#endif

	no_check = TRUE;
	d_log("zipscript-c: File type is: NFO\n");
#if ( deny_double_nfo )
	if (findfileextcount(dir, ".nfo") > 1) {
		d_log("zipscript-c: Looks like there already is a nfo uploaded. Denying this one.\n");
		sprintf(g->v.misc.error_msg, DUPE_NFO);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_nfo_type);
		exit_value = 2;
		return exit_value;
	}
#endif
	//writerace(g->l.race, &g->v, 0, F_NFO);

#if ( enable_nfo_script )
	if (!fileexists(nfo_script)) {
		d_log("zipscript-c: Warning - nfo_script (%s) - file does not exists\n", nfo_script);
	}
	d_log("zipscript-c: Executing nfo script (%s)\n", nfo_script);
	sprintf(target, nfo_script " \"%s\"", g->v.file.name);
	if (execute(target) != 0)
		d_log("zipscript-c: Failed to execute nfo_script: %s\n", strerror(errno));
#endif
	return exit_value;
}

int
handle_sfv32(GLOBAL *g, MSG *msg, DIR *dir, char **argv, char *fileext) {
	
	unsigned int	crc, s_crc = 0;
	unsigned char	no_check = FALSE;
	char           *target = 0;
	unsigned char	exit_value = EXIT_SUCCESS;

	d_log("zipscript-c: File type is: ANY\n");

	check_rarfile(g->v.file.name);

	d_log("zipscript-c: Converting crc (%s) from string to integer\n", argv[3]);
	crc = hexstrtodec(argv[3]);
	if (crc == 0) {
		d_log("zipscript-c: We did not get crc from ftp daemon, calculating crc for %s now.\n", g->v.file.name);
		crc = calc_crc32(g->v.file.name);
	}
	if (fileexists(g->l.sfv)) {
		s_crc = readsfv(g->l.sfv, &g->v, 0);

#if (sfv_calc_single_fname == TRUE)
		if (s_crc == 0 && g->v.misc.in_sfvfile == TRUE) {
			d_log("zipscript-c: CRC in SFV is 0 - trying to calculate it from the file\n");
			s_crc = calc_crc32(g->v.file.name);
			update_sfvdata(g->l.sfv, g->v.file.name, s_crc);
			if (smart_sfv_write && sfv_cleanup)
				sfvdata_to_sfv(g->l.sfv, findfileext(dir, ".sfv"));
		}
#endif
		if (!g->v.misc.sfv_match && g->v.misc.in_sfvfile == TRUE) {
			update_sfvdata(g->l.sfv, g->v.file.name, s_crc);
#if (smart_sfv_write && sfv_cleanup)
			sfvdata_to_sfv(g->l.sfv, findfileext(dir, ".sfv"));
#endif
		}

		d_log("zipscript-c: DEBUG: crc: %X - s_crc: %X - match:%d\n", crc, s_crc, g->v.misc.sfv_match);
		if (s_crc != crc && !(s_crc == 0 && g->v.misc.sfv_match)) {
			if (s_crc == 0 && !g->v.misc.sfv_match) {
				if (!strcomp(allowed_types, fileext)) {
#if (allow_files_not_in_sfv == TRUE)
					d_log("zipscript-c: Filename was not found in the SFV, but allowing anyway\n");
					no_check = TRUE;
					return exit_value;
#endif
					d_log("zipscript-c: Filename was not found in the SFV\n");
					strlcpy(g->v.misc.error_msg, NOT_IN_SFV, 80);
				} else {
					d_log("zipscript-c: filetype is part of allowed_types.\n");
					no_check = TRUE;
					return exit_value;
				}
			} else {
				d_log("zipscript-c: CRC-32 check failed\n");
				if (!hexstrtodec(argv[3]) && allow_file_resume) {
					d_log("zipscript-c: Broken xfer detected - allowing file.\n");
					no_check = TRUE;
					msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
					writelog(g, msg->error, bad_file_crc_type);
					if (enable_unduper_script == TRUE) {
						if (!fileexists(unduper_script)) {
							d_log("zipscript-c: Warning - undupe script (%s) does not exist.\n", unduper_script);
						}
						sprintf(target, unduper_script " \"%s\"", g->v.file.name);
						if (execute(target) == 0)
							d_log("zipscript-c: undupe of %s successful.\n", g->v.file.name);
						else
							d_log("zipscript-c: undupe of %s failed.\n", g->v.file.name);
					}
					return exit_value;
				}
				strlcpy(g->v.misc.error_msg, BAD_CRC, 80);
			}
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_crc_type);
			exit_value = 2;
			return exit_value;
		}
		if (s_crc == 0 && g->v.misc.sfv_match && g->v.misc.in_sfvfile == TRUE) {
			d_log("zipscript-c: File was found in sfv, but had no crc.\n");
			s_crc = crc;
			update_sfvdata(g->l.sfv, g->v.file.name, s_crc);
#if (smart_sfv_write && sfv_cleanup)
			sfvdata_to_sfv(g->l.sfv, findfileext(dir, ".sfv"));
#endif

		}
#if (sfv_cleanup_lowercase == TRUE)
		if (check_dupefile(dir, g->v.file.name)) {
			d_log("zipscript-c: dupe detected - same file, different case already exists.\n");
			strlcpy(g->v.misc.error_msg, DOUBLE_SFV, 80);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_crc_type);
			exit_value = 2;
			return exit_value;
		}
#endif
		printf(zipscript_SFV_ok);
		d_log("zipscript-c: Storing new race data\n");
		writerace(g->l.race, &g->v, crc, F_CHECKED);
	} else {
#if ( force_sfv_first == TRUE )
#if (use_partial_on_noforce == TRUE)
		if (!matchpartialpath(noforce_sfv_first_dirs, g->l.path) && !matchpath(zip_dirs, g->l.path)) {
#else
		if (!matchpath(noforce_sfv_first_dirs, g->l.path) && !matchpath(zip_dirs, g->l.path)) {
#endif
			d_log("zipscript-c: SFV needs to be uploaded first\n");
			strlcpy(g->v.misc.error_msg, SFV_FIRST, 80);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_nosfv_type);
			exit_value = 2;
			return exit_value;
		} else if (matchpath(zip_dirs, g->l.path) && (!fileexists(g->l.sfv))) {
			d_log("zipscript-c: This looks like a file uploaded the wrong place - Not allowing it.\n");
			strlcpy(g->v.misc.error_msg, SFV_FIRST, 80);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_nosfv_type);
			exit_value = 2;
			return exit_value;
		} else {
			d_log("zipscript-c: path matched with noforce_sfv_first or zip_dirs - allowing file.\n");
			printf(zipscript_SFV_skip);
			d_log("zipscript-c: Storing new race data\n");
			writerace(g->l.race, &g->v, crc, F_NOTCHECKED);
		}
#else
		d_log("zipscript-c: Could not check file yet - SFV is not present\n");
		printf(zipscript_SFV_skip);
		d_log("zipscript-c: Storing new race data\n");
		writerace(g->l.race, &g->v, crc, F_NOTCHECKED);
#endif
	}

	d_log("zipscript-c: Reading race data from file to memory\n");
	readrace(g->l.race, &g->v, g->ui, g->gi);

	d_log("zipscript-c: Setting pointers\n");
	if (g->v.misc.release_type == RTYPE_NULL) {
		if (israr(fileext))
			g->v.misc.release_type = RTYPE_RAR;	/* .RAR / .R?? */
		else if (isvideo(fileext))
			g->v.misc.release_type = RTYPE_VIDEO;	/* AVI/MPEG */
		else if (!memcmp(fileext, "mp3", 4))
			g->v.misc.release_type = RTYPE_AUDIO;	/* MP3 */
		else
			g->v.misc.release_type = RTYPE_OTHER;	/* OTHER FILE */
	}
	switch (g->v.misc.release_type) {
	case RTYPE_RAR:
		get_rar_info(&g->v);
		msg->race = rar_race;
		msg->update = rar_update;
		msg->halfway = CHOOSE2(g->v.total.users, rar_halfway, rar_norace_halfway);
		msg->newleader = rar_newleader;
		break;
	case RTYPE_OTHER:
		msg->race = other_race;
		msg->update = other_update;
		msg->halfway = CHOOSE2(g->v.total.users, other_halfway, other_norace_halfway);
		msg->newleader = other_newleader;
		break;
	case RTYPE_AUDIO:
		msg->race = audio_race;
		msg->update = audio_update;
		msg->halfway = CHOOSE2(g->v.total.users, audio_halfway, audio_norace_halfway);
		msg->newleader = audio_newleader;
		d_log("zipscript-c: Trying to read audio header and tags\n");
		get_mpeg_audio_info(g->v.file.name, &g->v.audio);
		write_bitrate_in_race(g->l.race, &g->v);
		sprintf(g->v.audio.bitrate, "%i", read_bitrate_in_race(g->l.race, &g->v));
			if ((enable_mp3_script == TRUE) && (g->ui[g->v.user.pos]->files == 1)) {
				if (!fileexists(mp3_script)) {
					d_log("zipscript-c: Warning -  mp3_script (%s) - file does not exists\n", mp3_script);
				}
				d_log("zipscript-c: Executing mp3 script (%s %s)\n", mp3_script, convert(&g->v, g->ui, g->gi, mp3_script_cookies));
				sprintf(target, "%s %s", mp3_script, convert(&g->v, g->ui, g->gi, mp3_script_cookies));
				if (execute(target) != 0)
					d_log("zipscript-c: Failed to execute mp3_script: %s\n", strerror(errno));
			}
			if (!matchpath(audio_nocheck_dirs, g->l.path)) {
#if ( audio_banned_genre_check )
				if (strcomp(banned_genres, g->v.audio.id3_genre)) {
					d_log("zipscript-c: File is from banned genre\n");
					sprintf(g->v.misc.error_msg, BANNED_GENRE, g->v.audio.id3_genre);
					if (audio_genre_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log("zipscript-c: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_genre_warn_msg);
							writelog(g, msg->error, general_badgenre_type);
						} else
							d_log("zipscript-c: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_genre_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#elif ( audio_allowed_genre_check == TRUE )
				if (!strcomp(allowed_genres, g->v.audio.id3_genre)) {
					d_log("zipscript-c: File is not in allowed genre\n");
					sprintf(g->v.misc.error_msg, BANNED_GENRE, g->v.audio.id3_genre);
					if (audio_genre_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log("zipscript-c: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_genre_warn_msg);
							writelog(g, msg->error, general_badgenre_type);
						} else
							d_log("zipscript-c: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_genre_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#endif
#if ( audio_year_check == TRUE )
				if (!strcomp(allowed_years, g->v.audio.id3_year)) {
					d_log("zipscript-c: File is from banned year\n");
					sprintf(g->v.misc.error_msg, BANNED_YEAR, g->v.audio.id3_year);
					if (audio_year_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log("zipscript-c: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_year_warn_msg);
							writelog(g, msg->error, general_badyear_type);
						} else
							d_log("zipscript-c: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_year_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#endif
#if ( audio_cbr_check == TRUE )
				if (g->v.audio.is_vbr == 0) {
					if (!strcomp(allowed_constant_bitrates, g->v.audio.bitrate)) {
						d_log("zipscript-c: File is encoded using banned bitrate\n");
						sprintf(g->v.misc.error_msg, BANNED_BITRATE, g->v.audio.bitrate);
						if (audio_cbr_warn == TRUE) {
							if (g->ui[g->v.user.pos]->files == 1) {
								d_log("zipscript-c: warn on - logging to logfile\n");
								msg->error = convert(&g->v, g->ui, g->gi, audio_cbr_warn_msg);
								writelog(g, msg->error, general_badbitrate_type);
							} else
								d_log("zipscript-c: warn on - have already logged to logfile\n");
						} else {
							mark_as_bad(g->v.file.name);
							msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
							if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
								writelog(g, msg->error, bad_file_bitrate_type);
							exit_value = 2;
						}
#if ( del_banned_release || enable_banned_script )
						deldir = 1;
						exit_value = 2;
#endif
						break;
					}
				}
#endif
#if ( audio_allowed_vbr_preset_check == TRUE )
				if (g->v.audio.is_vbr && strlen(g->v.audio.vbr_preset) && !strcomp(allowed_vbr_presets, g->v.audio.vbr_preset)) {
					d_log("zipscript-c: File is not in allowed vbr preset list (%s)\n", g->v.audio.vbr_preset);
					sprintf(g->v.misc.error_msg, BANNED_PRESET, g->v.audio.vbr_preset);
					if (audio_vbr_preset_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log("zipscript-c: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_vbr_preset_warn_msg);
							writelog(g, msg->error, general_badpreset_type);
						} else
							d_log("zipscript-c: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_vbr_preset_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#endif
			} else
				d_log("zipscript-c: user is in a no audio check dir - skipping checks.\n");

		if (realtime_mp3_info != DISABLED) {
			d_log("zipscript-c: Printing realtime_mp3_info.\n");
			printf("%s", convert(&g->v, g->ui, g->gi, realtime_mp3_info));
		}
		break;
	case RTYPE_VIDEO:
		d_log("zipscript-c: Trying to read video header\n");
		if (!memcmp(fileext, "avi", 3)) {
			bzero(&g->m, sizeof(struct MULTIMEDIA));
			avinfo(g->v.file.name, &g->m);
			d_log("zipscript-c: avinfo: video - %dx%d * %.0f of %s (%s).\n", g->m.height, g->m.width, g->m.fps, g->m.vids, g->m.audio);
			avi_video(g->v.file.name, &g->v.video);
		}
		else
			mpeg_video(g->v.file.name, &g->v.video);
		msg->race = video_race;
		msg->update = video_update;
		msg->halfway = CHOOSE2(g->v.total.users, video_halfway, video_norace_halfway);
		msg->newleader = video_newleader;
		break;
	default:
		get_rar_info(&g->v);
		msg->race = rar_race;
		msg->update = rar_update;
		msg->halfway = CHOOSE2(g->v.total.users, rar_halfway, rar_norace_halfway);
		msg->newleader = rar_newleader;
		d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (2:%d)\n", g->v.misc.release_type);
		break;
	}

	if (!msg->race)
		d_log("zipscript-c: Something's messed up - msg->race not set!\n");

	if (exit_value == EXIT_SUCCESS) {
		d_log("zipscript-c: Removing missing indicator\n");
		unlink_missing(g->v.file.name);
		/*
		sprintf(target, "%s-missing", g->v.file.name);
#if (sfv_cleanup_lowercase == TRUE)
		  strtolower(target);
#endif
		if (target)
			unlink(target);
		*/
	}

	return exit_value;
}

void
read_envdata(GLOBAL *g, GDATA *gdata, UDATA *udata, struct stat *fileinfo)
{
	
	int			n;
	char		*temp_p = NULL, *temp_p_free = NULL;

	buffer_groups(gdata, GROUPFILE, 0);
	buffer_users(udata, PASSWDFILE, 0);
	
	if ((getenv("USER") == NULL) || (getenv("GROUP") == NULL) || (getenv("TAGLINE") == NULL) || (getenv("SPEED") ==NULL) || (getenv("SECTION") == NULL)) {
		d_log("zipscript-c: We are running from shell, falling back to default values for $USER, $GROUP, $TAGLINE, $SECTION and $SPEED\n");
		/*
		 * strcpy(g->v.user.name, "Unknown");
		 * strcpy(g->v.user.group, "NoGroup");
		 */

		fileinfo->st_uid = geteuid();
		fileinfo->st_gid = getegid();
		strlcpy(g->v.user.name, get_u_name(udata, fileinfo->st_uid), 24);
		strlcpy(g->v.user.group, get_g_name(gdata, fileinfo->st_gid), 24);
		memcpy(g->v.user.tagline, "No Tagline Set", 15);
		g->v.file.speed = 2005;
		g->v.section = 0;
		sprintf(g->v.sectionname, "DEFAULT");
	} else {
		sprintf(g->v.user.name, getenv("USER"));
		sprintf(g->v.user.group, getenv("GROUP"));
		if (!(int)strlen(g->v.user.group))
			memcpy(g->v.user.group, "NoGroup", 8);
		sprintf(g->v.user.tagline, getenv("TAGLINE"));
		if (!(int)strlen(g->v.user.tagline))
			memcpy(g->v.user.tagline, "No Tagline Set", 15);
		g->v.file.speed = (unsigned int)strtol(getenv("SPEED"), NULL, 0);
		if (!g->v.file.speed)
			g->v.file.speed = 1;

#if (debug_announce == TRUE)
		printf("DEBUG: Speed: %dkb/s (%skb/s)\n",  g->v.file.speed, getenv("SPEED"));
#endif

		d_log("zipscript-c: Reading section from env (%s)\n", getenv("SECTION"));
		snprintf(g->v.sectionname, 127, getenv("SECTION"));
		g->v.section = 0;
		temp_p_free = temp_p = strdup((const char *)gl_sections);	/* temp_p_free is needed since temp_p is modified by strsep */
		if ((temp_p) == NULL) {
			d_log("zipscript-c: Can't allocate memory for sections\n");
		} else {
			n = 0;
			while (temp_p) {
				if (!strcmp(strsep(&temp_p, " "), getenv("SECTION"))) {
					g->v.section = (unsigned char)n;
					break;
				} else
					n++;
			}
			ng_free(temp_p_free);
		}
	}
}

void
check_filesize(GLOBAL *g, const char *filename, struct stat *fileinfo)
{

	if (stat(filename, fileinfo)) {
		d_log("zipscript-c: Failed to stat file: %s\n", strerror(errno));
		g->v.file.size = 0;
		g->v.total.stop_time = 0;
	} else {
		g->v.file.size = fileinfo->st_size;
		d_log("zipscript-c: File size was: %d\n", g->v.file.size);
		g->v.total.stop_time = fileinfo->st_mtime;
	}

}

void
lock_release(GLOBAL *g, DIR *dir, DIR *parent)
{

	int		m, n;

	while (1) {
	
		if ((m = create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 3, 0))) {
			d_log("zipscript-c: Failed to lock release.\n");
			
			if (m == 1) {
				d_log("zipscript-c: version mismatch. Exiting.\n");
				printf("Error. You need to rm -fR ftp-data/pzs-ng/* before zipscript-c will work.\n");
				exit(EXIT_FAILURE);
			}
			
			if (m == PROGTYPE_RESCAN) {
				d_log("zipscript-c: Detected rescan running - will try to make it quit.\n");
				update_lock(&g->v, 0, 0);
			}

			if (m == PROGTYPE_POSTDEL) {
				n = (signed int)g->v.lock.data_incrementor;
				d_log("zipscript-c: Detected postdel running - sleeping for one second.\n");
				if (!create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 0, g->v.lock.data_queue))
					break;
				usleep(1000000);
				if (!create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 0, g->v.lock.data_queue))
					break;
				if ( n == (signed int)g->v.lock.data_incrementor) {
					d_log("zipscript-c: Failed to get lock. Forcing unlock.\n");
					if (create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 2, g->v.lock.data_queue)) {
						d_log("zipscript-c: Failed to force a lock.\n");
						d_log("zipscript-c: Exiting with error.\n");
						exit(EXIT_FAILURE);
					}
					break;
				}

			} else {
			
				for ( n = 0; n <= max_seconds_wait_for_lock * 10; n++) {
					d_log("zipscript-c: sleeping for .1 second before trying to get a lock.\n");
					usleep(100000);
					if (!(m = create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 0, g->v.lock.data_queue)))
						break;	
				}
				
				if (n >= max_seconds_wait_for_lock * 10) {
					if (m == PROGTYPE_RESCAN) {
						d_log("zipscript-c: Failed to get lock. Forcing unlock.\n");
						if (create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 2, g->v.lock.data_queue))
							d_log("zipscript-c: Failed to force a lock.\n");
					} else
						d_log("zipscript-c: Failed to get a lock.\n");
					if (!g->v.lock.data_in_use && !ignore_lock_timeout) {
						d_log("zipscript-c: Exiting with error.\n");
						exit(EXIT_FAILURE);
					}
				}
			}

			rewinddir(dir);
			rewinddir(parent);
		}
		
		usleep(10000);
		
		if (update_lock(&g->v, 1, 0) != -1)
			break;
			
	}

}
