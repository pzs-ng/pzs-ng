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

#include "racetypes.h"

#include "handlers.h"
/*#include "handle_sfv.h"
#include "handle_nfo.h"
#include "handle_zip.h"*/

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifdef _SunOS_
# include "scandir.h"
# include "strsep.h"
#endif

#define opendirs(x, y) x = opendir("."); y = opendir("..")
#define closedirs(x, y) closedir(x); closedir(y)

void read_envdata(GLOBAL *, GDATA *, UDATA *, struct stat *);
void check_filesize(GLOBAL *, const char *, struct stat *);
void lock_release(GLOBAL *);
void set_uid_gid(void);
void group_dir_users(GLOBAL *); /* call this something else */
int match_nocheck_dirs(GLOBAL *);

/* turn these two into one function */
int check_zerosize(GLOBAL *, MSG *);
int check_banned_file(GLOBAL *, MSG *);

int process_file(GLOBAL *, MSG *, char **, char *, int *, int *);
void check_release_type(GLOBAL *, MSG *, RACETYPE *, char *[2]);

void execute_script(char *, char *, char *);
int get_nfo_filetype(unsigned int);
void create_missing_missing_nfo_indicator(GLOBAL *, char *[2]);
void release_complete(GLOBAL *, MSG *, char *[2]);
void release_incomplete(GLOBAL *, MSG *, RACETYPE *);

int 
main(int argc, char **argv)
{
	GLOBAL		g; /* this motherfucker owns */
	MSG		msg;
	GDATA		gdata;
	UDATA		udata;
	RACETYPE	rtype;
	
	char		*fileext = NULL, *name_p, *temp_p = NULL;
	char		*_complete[2] = { 0 }; /* 0 = bar, 1 = announce */
	int		exit_value = EXIT_SUCCESS;
	int		no_check = FALSE;
	int		n = 0;
	int		deldir = 0;
	struct stat	fileinfo;

#if ( benchmark_mode == TRUE )
	struct timeval	bstart, bstop;
	d_log(1, "zipscript-c: Reading time for benchmark\n");
	gettimeofday(&bstart, (struct timezone *)0);
#endif

	if (argc != 4) {
		printf(" - - PZS-NG ZipScript-C v%s - -\n\nUsage: %s <filename> <path> <crc>\n\n", ng_version(), argv[0]);
		return EXIT_FAILURE;
	}

	if (!fileexists(argv[1])) {
		d_log(1, "zipscript-c: File %s does not exist. exiting.\n", argv[1]);
		return 1;
	}

#if ( debug_mode == TRUE && debug_announce == TRUE)
	printf("PZS-NG: Running in debug mode.\n");
#endif

	/*
	 * Adding version-number to head if .debug message 15.09.2004 - psxc
	 */
	d_log(1, "zipscript-c: Project-ZS Next Generation (pzs-ng) v%s debug log.\n", ng_version());

#ifdef _ALT_MAX
	d_log(1, "zipscript-c: PATH_MAX not found - using predefined settings! Please report to the devs!\n");
#endif

	umask(0666 & 000);

	set_uid_gid();

	bzero(&g, sizeof(GLOBAL));

	g.v.misc.write_log = TRUE;
	g.v.misc.in_sfvfile = FALSE;

	strlcpy(g.v.file.name, argv[1], NAME_MAX);
	strlcpy(g.l.path, argv[2], PATH_MAX);
	strlcpy(g.v.misc.current_path, g.l.path, PATH_MAX);
	d_log(1, "zipscript-c: Changing directory to %s\n", g.l.path);
	chdir(g.l.path);

	d_log(1, "zipscript-c: Reading data from environment variables\n");
	read_envdata(&g, &gdata, &udata, &fileinfo);
	g.v.file.speed *= 1024;

	d_log(1, "zipscript-c: Checking the file size of %s\n", g.v.file.name);
	check_filesize(&g, g.v.file.name, &fileinfo);

	d_log(1, "zipscript-c: Setting race times\n");
	if (g.v.file.size != 0)
		g.v.total.start_time = g.v.total.stop_time - (g.v.file.size / g.v.file.speed);
	else
		g.v.total.start_time = g.v.total.stop_time > (g.v.total.stop_time - 1) ? g.v.total.stop_time : (g.v.total.stop_time -1);
		
	if ((g.v.total.stop_time - g.v.total.start_time) < 1)
		g.v.total.stop_time = g.v.total.start_time + 1;

	n = (g.l.length_path = strlen(g.l.path)) + 1;

	d_log(1, "zipscript-c: Allocating memory for variables\n");
	g.l.race = ng_realloc2(g.l.race, n += 10 + (g.l.length_zipdatadir = sizeof(storage) - 1), 1, 1, 1);
	g.l.sfv = ng_realloc2(g.l.sfv, n, 1, 1, 1);
	g.l.leader = ng_realloc2(g.l.leader, n, 1, 1, 1);

	d_log(1, "zipscript-c: Copying data g.l into memory\n");
	sprintf(g.l.sfv, storage "/%s/sfvdata", g.l.path);
	sprintf(g.l.leader, storage "/%s/leader", g.l.path);
	sprintf(g.l.race, storage "/%s/racedata", g.l.path);
	g.v.user.pos = 0;
	snprintf(g.v.misc.old_leader, 24, "none");
	g.v.file.compression_method = '5';

	/* Get file extension */
	d_log(1, "zipscript-c: Parsing file extension from filename... (%s)\n", argv[1]);
	name_p = temp_p = find_last_of(argv[1], ".");
	
	if (*temp_p != '.') {
		d_log(1, "zipscript-c: Got: no extension\n");
		temp_p = name_p;
	} else {
		d_log(1, "zipscript-c: Got: %s\n", temp_p);
		temp_p++;
	}
	name_p++;

	fileext = ng_realloc(fileext, sizeof(name_p), 1, 1, &g.v, 1);
	memcpy(fileext, name_p, sizeof(name_p));
	
#if ( sfv_cleanup_lowercase == TRUE )
	d_log(1, "zipscript-c: Copying (lowercased version of) extension to memory\n");
	strtolower(fileext);
#else
	d_log(1, "zipscript-c: Copying (unchanged version of) extension to memory\n");
#endif

	d_log(1, "zipscript-c: Caching release name\n");
	getrelname(&g);

	d_log(1, "zipscript-c: Creating directory to store racedata in\n");
	maketempdir(g.l.path);

	d_log(1, "zipscript-c: Locking release\n");
	lock_release(&g);

	printf(zipscript_header);

	g.v.misc.nfofound = (int)findfileext(".", ".nfo");

	/* Hide users in group_dirs */
	group_dir_users(&g);
	
	/* No check directories */
	no_check = match_nocheck_dirs(&g);

	if (no_check == FALSE) {

#if (ignore_zero_size == FALSE )
		exit_value = check_zerosize(&g, &msg);
#endif

#if ( check_for_banned_files )
		exit_value = check_banned_file(&g, &msg);
#endif

		if (exit_value < 2) {
			d_log(1, "process_file: Verifying old racedata\n");
			if (!verify_racedata(g.l.race, &g.v))
				d_log(1, "process_file:   Failed to open racedata - assuming this is a new race.\n");
			exit_value = process_file(&g, &msg, argv, fileext, &no_check, &deldir);
		}

	}

	/*if (no_check == TRUE) {	// File was not checked
		
		printf(zipscript_any_ok);
		printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_skip));
		
		if (matchpath(speedtest_dirs, g.l.path)) {
			d_log(1, "zipscript-c: writing speedtest to channel\n");
			writelog(&g, convert(&g.v, g.ui, g.gi, speed_announce), speed_type);
			exit_value = 2;
		}
	} else */
	if (exit_value == EXIT_SUCCESS) { /* File was checked */

		check_release_type(&g, &msg, &rtype, _complete);
		
		if (g.v.total.users > 0) {
			d_log(1, "zipscript-c: Sorting race stats\n");
			sortstats(&g.v, g.ui, g.gi);

#if ( get_user_stats == TRUE )
			d_log(1, "zipscript-c: Reading day/week/month/all stats for racers\n");
			d_log(1, "zipscript-c: stat section: %i\n", g.v.section);
			get_stats(&g.v, g.ui);
#endif
			
			showstats(&g.v, g.ui, g.gi);

			if (!enable_files_ahead || ((g.v.total.users > 1 && g.ui[g.ui[0].pos].files >= (g.ui[g.ui[1].pos].files + newleader_files_ahead)) || g.v.total.users == 1)) {
				d_log(1, "zipscript-c: Writing current leader to file\n");
				read_write_leader(g.l.leader, &g.v, &g.ui[g.ui[0].pos]);
			}
			if (g.v.total.users > 1) {
				if (g.ui[g.v.user.pos].files == 1 && msg.race != NULL) {
					d_log(1, "zipscript-c: Writing RACE to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.race), rtype.race);
				}
				if (g.v.total.files >= min_newleader_files && ((g.v.total.size * g.v.total.files) >= (min_newleader_size * 1024 * 1024)) && strcmp(g.v.misc.old_leader, g.ui[g.ui[0].pos].name) && msg.newleader != NULL && g.ui[g.ui[0].pos].files >= (g.ui[g.ui[1].pos].files + newleader_files_ahead) && g.v.total.files_missing) {
					d_log(1, "zipscript-c: Writing NEWLEADER to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.newleader), rtype.newleader);
				}
			} else {
				if (g.ui[g.v.user.pos].files == 1 && g.v.total.files >= min_update_files && ((g.v.total.size * g.v.total.files) >= (min_update_size * 1024 * 1024)) && msg.update) {
					d_log(1, "zipscript-c: Writing UPDATE to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.update), rtype.update);
				}
			}
		}
		
		if (g.v.total.files_missing > 0)
			release_incomplete(&g, &msg, &rtype);
		else if ((g.v.total.files_missing == 0) && (g.v.total.files > 0))
			release_complete(&g, &msg, _complete);
		else {

			/* Release is at unknown state */
			g.v.total.files = -g.v.total.files_missing;
			g.v.total.files_missing = 0;
			printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_unknown));

		}

	} else {

		/* File is marked to be deleted */
		d_log(1, "zipscript-c: Logging file as bad\n");
		remove_from_race(g.l.race, g.v.file.name, &g.v);

		printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_error));
	}

#if ( enable_accept_script == TRUE )
	if (exit_value == EXIT_SUCCESS)
		execute_script(accept_script, g.v.file.name, "accept");
#endif

#if ( enable_nfo_script == TRUE )
	 if ( !g.v.misc.nfofound && findfileext(".", ".nfo"))
		execute_script(nfo_script, g.v.file.name, "nfo");
#endif

	if ((findfileext(".", ".nfo") || (findfileext("..", ".nfo"))) && (g.l.nfo_incomplete)) {
		d_log(1, "zipscript-c: Removing missing-nfo indicator (if any)\n");
		remove_nfo_indicator(&g);
	}

#if ( del_banned_release )
	if (deldir) {
		d_log(1, "zipscript-c: del_banned_release is set - removing entire dir.\n");
		move_progress_bar(1, &g.v, g.ui, g.gi);
		if (g.l.incomplete)
			unlink(g.l.incomplete);
		del_releasedir(".", g.l.path);
	}
#endif

#if ( enable_banned_script )
	if (deldir)
		execute_script(banned_script, g.v.file.name, "banned");
#endif

	d_log(1, "zipscript-c: Releasing memory and removing lock\n");
	remove_lock(&g.v);

	if (fileexists(".delme"))
		unlink(".delme");

	buffer_groups(&gdata, GROUPFILE, 1);
	buffer_users(&udata, PASSWDFILE, 1);
	ng_free(fileext);
	ng_free(g.l.race);
	ng_free(g.l.sfv);
	ng_free(g.l.leader);

#if ( benchmark_mode == TRUE )
	gettimeofday(&bstop, (struct timezone *)0);
	printf("Checks completed in %0.6f seconds\n", ((bstop.tv_sec - bstart.tv_sec) + (bstop.tv_usec - bstart.tv_usec) / 1000000.));
	d_log(1, "zipscript-c: Checks completed in %0.6f seconds\n", ((bstop.tv_sec - bstart.tv_sec) + (bstop.tv_usec - bstart.tv_usec) / 1000000.));
#endif

#if ( sleep_on_bad > 0 && sleep_on_bad < 1001 )
	if (exit_value == 2) {
		if (sleep_on_bad == 117) {
			exit_value = 126;
		} else {
			exit_value = sleep_on_bad + 10;
		}
		d_log(1, "zipscript-c: Sleeping for %d seconds.\n", sleep_on_bad);
	}
#endif
	d_log(1, "zipscript-c: Exit %d\n", exit_value);
	return exit_value;
}

void
read_envdata(GLOBAL *g, GDATA *gdata, UDATA *udata, struct stat *fileinfo)
{
	
	int		n;
	char		*temp_p = NULL, *temp_p_free = NULL;

	buffer_groups(gdata, GROUPFILE, 0);
	buffer_users(udata, PASSWDFILE, 0);
	
	if ((getenv("USER") == NULL) || (getenv("GROUP") == NULL) ||
	    (getenv("TAGLINE") == NULL) || (getenv("SPEED") ==NULL) ||
	    (getenv("SECTION") == NULL)) {
		d_log(1, "read_envdata: We are running from shell, falling back to default values for $USER, $GROUP, $TAGLINE, $SECTION and $SPEED\n");
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

		d_log(1, "read_envdata: Reading section from env (%s)\n", getenv("SECTION"));
		snprintf(g->v.sectionname, 127, getenv("SECTION"));
		g->v.section = 0;
		temp_p_free = temp_p = strdup((const char *)gl_sections);	/* temp_p_free is needed since temp_p is modified by strsep */
		if (temp_p == NULL) {
			d_log(1, "read_envdata: Can't allocate memory for sections\n");
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
		d_log(1, "check_filesize: Failed to stat file: %s\n", strerror(errno));
		g->v.file.size = 0;
		g->v.total.stop_time = 0;
	} else {
		g->v.file.size = fileinfo->st_size;
		d_log(1, "check_filesize: File size was: %d\n", g->v.file.size);
		g->v.total.stop_time = fileinfo->st_mtime;
	}

}

void
lock_release(GLOBAL *g)
{

	int	m, n;

	while (1) {
	
		if ((m = create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 3, 0))) {
			d_log(1, "lock_release: Failed to lock release.\n");
			
			if (m == 1) {
				d_log(1, "lock_release: version mismatch. Exiting.\n");
				printf("Error. You need to rm -fR ftp-data/pzs-ng/* before the zipscript will work.\n");
				exit(EXIT_FAILURE);
			}
			
			if (m == PROGTYPE_RESCAN) {
				d_log(1, "lock_release: Detected rescan running - will try to make it quit.\n");
				update_lock(&g->v, 0, 0);
			}

			if (m == PROGTYPE_POSTDEL) {
				n = (signed int)g->v.lock.data_incrementor;
				d_log(1, "lock_release: Detected postdel running - sleeping for one second.\n");
				if (!create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 0, g->v.lock.data_queue))
					break;
				usleep(1000000);
				if (!create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 0, g->v.lock.data_queue))
					break;
				if ( n == (signed int)g->v.lock.data_incrementor) {
					d_log(1, "lock_release: Failed to get lock. Forcing unlock.\n");
					if (create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 2, g->v.lock.data_queue)) {
						d_log(1, "lock_release: Failed to force a lock.\n");
						d_log(1, "lock_release: Exiting with error.\n");
						exit(EXIT_FAILURE);
					}
					break;
				}

			} else {
			
				for ( n = 0; n <= max_seconds_wait_for_lock * 10; n++) {
					d_log(1, "lock_release: sleeping for .1 second before trying to get a lock.\n");
					usleep(100000);
					if (!(m = create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 0, g->v.lock.data_queue)))
						break;	
				}
				
				if (n >= max_seconds_wait_for_lock * 10) {
					if (m == PROGTYPE_RESCAN) {
						d_log(1, "lock_release: Failed to get lock. Forcing unlock.\n");
						if (create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 2, g->v.lock.data_queue))
							d_log(1, "lock_release: Failed to force a lock.\n");
					} else
						d_log(1, "lock_release: Failed to get a lock.\n");
					if (!g->v.lock.data_in_use && !ignore_lock_timeout) {
						d_log(1, "lock_release: Exiting with error.\n");
						exit(EXIT_FAILURE);
					}
				}
			}

		}
		
		usleep(10000);
		
		if (update_lock(&g->v, 1, 0) != -1)
			break;
			
	}

}

void
set_uid_gid(void)
{

	if ( program_uid > 0 ) {
		d_log(1, "set_uid_gid: Trying to change effective uid/gid\n");
		setegid(program_gid);
		seteuid(program_uid);
	} else if (!geteuid()) {
		d_log(1, "set_uid_gid: +s mode detected - trying to change effective uid/gid to !root\n");
		if (setegid(getgid()) == -1)
			d_log(1, "set_uid_gid: failed to change gid: %s\n", strerror(errno));
		if (seteuid(getuid()) == -1)
			d_log(1, "set_uid_gid: failed to change uid: %s\n", strerror(errno));
	}

}

void
group_dir_users(GLOBAL *g)
{

	if (matchpath(group_dirs, g->l.path) && (hide_group_uploaders == TRUE)) {
		d_log(1, "group_dir_users: Hiding user in group-dir:\n");
		if (strlen(hide_gname) > 0) {
			snprintf(g->v.user.group, 18, "%s", hide_gname);
			d_log(1, "group_dir_users:    Changing groupname\n");
		}
		if (strlen(hide_uname) > 0) {
			snprintf(g->v.user.name, 18, "%s", hide_uname);
			d_log(1, "group_dir_users:    Changing username\n");
		}
		if (strlen(hide_uname) == 0) {
			d_log(1, "group_dir_users:    Making username = groupname\n");
			snprintf(g->v.user.name, 18, "%s", g->v.user.group);
		}
	}

}

int
match_nocheck_dirs(GLOBAL *g)
{

	if (matchpath(nocheck_dirs, g->l.path) || matchpath(speedtest_dirs, g->l.path) ||
		(!matchpath(zip_dirs, g->l.path) && !matchpath(sfv_dirs, g->l.path) &&
		!matchpath(group_dirs, g->l.path))) {
		
		d_log(1, "match_nocheck_dirs: Directory matched with nocheck_dirs, or is not among sfv/zip/group dirs\n");
		d_log(1, "match_nocheck_dirs:   - nocheck_dirs  : '%s'\n", nocheck_dirs);
		d_log(1, "match_nocheck_dirs:   - speedtest_dirs: '%s'\n", speedtest_dirs);
		d_log(1, "match_nocheck_dirs:   - zip_dirs      : '%s'\n", zip_dirs);
		d_log(1, "match_nocheck_dirs:   - sfv_dirs      : '%s'\n", sfv_dirs);
		d_log(1, "match_nocheck_dirs:   - group_dirs    : '%s'\n", group_dirs);
		d_log(1, "match_nocheck_dirs:   - current path  : '%s'\n", g->l.path);
		
		return TRUE;

	} else
		return FALSE;

}

int
check_zerosize(GLOBAL *g, MSG *msg)
{
	
	if (g->v.file.size == 0) {
		d_log(1, "check_zerosize: File seems to be 0 bytes\n");
		sprintf(g->v.misc.error_msg, EMPTY_FILE);
		mark_as_bad(g->v.file.name);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		writelog(g, msg->error, bad_file_0size_type);
		return 2;
	} else
		return EXIT_SUCCESS;

}

int
check_banned_file(GLOBAL *g, MSG *msg)
{
	
	if (filebanned_match(g->v.file.name)) {
		d_log(1, "check_banned_file: Banned file detected (%s)\n", g->v.file.name);
		sprintf(g->v.misc.error_msg, BANNED_FILE);
		mark_as_bad(g->v.file.name);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		writelog(g, msg->error, bad_file_disallowed_type);
		return 2;
	} else
		return EXIT_SUCCESS;

}

/* fuck you, arguments */
int
process_file(GLOBAL *g, MSG *msg, char **argv, char *fileext, int *no_check, int *deldir)
{
	
	short type;
	handler_t *handler;
	HANDLER_ARGS ha;
	
	type = get_filetype(fileext);
	handler = get_handler(type);
		
	ha.g = g;
	ha.msg = msg;
	ha.argv = argv;
	ha.fileext = fileext;
	ha.deldir = deldir;
	
	return handler(&ha);
	
}

void
check_release_type(GLOBAL *g, MSG *msg, RACETYPE *rtype, char *_complete[2])
{

	switch (g->v.misc.release_type) {
		case RTYPE_RAR:
			set_rtype_msg(rtype, &__racetype_rar);
			_complete[0] = rar_completebar;
			_complete[1] = CHOOSE(g->v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, rar_complete, rar_norace_complete);
			break;	/* rar */
		case RTYPE_OTHER:
			set_rtype_msg(rtype, &__racetype_other);
			_complete[0] = other_completebar;
			_complete[1] = CHOOSE(g->v.total.users, other_announce_race_complete_type, other_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, other_complete, other_norace_complete);
			break;	/* other */
		case RTYPE_AUDIO:
			_complete[0] = audio_completebar;
			if (!g->v.audio.is_vbr) {
				set_rtype_msg(rtype, &__racetype_audio_vbr);
				_complete[1] = CHOOSE(g->v.total.users, audio_cbr_announce_race_complete_type, audio_cbr_announce_norace_complete_type);
			} else {
				set_rtype_msg(rtype, &__racetype_audio_cbr);
				_complete[1] = CHOOSE(g->v.total.users, audio_vbr_announce_race_complete_type, audio_vbr_announce_norace_complete_type);
			}
			msg->complete = CHOOSE(g->v.total.users, audio_complete, audio_norace_complete);
			break;	/* audio */
		case RTYPE_VIDEO:
			set_rtype_msg(rtype, &__racetype_video);
			_complete[0] = video_completebar;
			_complete[1] = CHOOSE(g->v.total.users, video_announce_race_complete_type, video_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, video_complete, video_norace_complete);
			break;	/* video */
		case RTYPE_NULL:
			set_rtype_msg(rtype, &__racetype_zip);
			_complete[0] = zip_completebar;
			_complete[1] = CHOOSE(g->v.total.users, zip_announce_race_complete_type, zip_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, zip_complete, zip_norace_complete);
			break;	/* zip */
		default:
			set_rtype_msg(rtype, &__racetype_rar);
			_complete[0] = rar_completebar;
			_complete[1] = CHOOSE(g->v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, rar_complete, rar_norace_complete);
			d_log(1, "check_release_type: WARNING! Not a known release type - Contact the authors! (3:%d)\n", g->v.misc.release_type);
			break;	/* rar */
	}

}

void
execute_script(char *script, char *arg, char *type)
{

	static char target[PATH_MAX];
	
	if (!fileexists(script))
		d_log(1, "execute_script: Warning - %s_script (%s) - file does not exists\n", type, script);

	d_log(1, "execute_script: Executing %s script\n", type);
	
	snprintf(target, PATH_MAX, "%s \"%s\"", script, arg);
	
	if (execute(target) != 0)
		d_log(1, "execute_script: Failed to execute %s_script: %s\n", type, strerror(errno));
		
}

int
get_nfo_filetype(unsigned int type)
{

	switch (type) {
		case RTYPE_RAR:
			if (strcomp(check_for_missing_nfo_filetypes, "rar"))
				return 1;
		case RTYPE_OTHER:
			if (strcomp(check_for_missing_nfo_filetypes, "other"))
				return 1;
		case RTYPE_AUDIO:
			if (strcomp(check_for_missing_nfo_filetypes, "audio"))
				return 1;
		case RTYPE_VIDEO:
			if (strcomp(check_for_missing_nfo_filetypes, "video"))
				return 1;
		case RTYPE_NULL:
			if (strcomp(check_for_missing_nfo_filetypes, "zip"))
				return 1;
	}

	return 0;
}

void
create_missing_missing_nfo_indicator(GLOBAL *g, char *inc_point[2])
{

	if (!g->l.in_cd_dir) {
		d_log(1, "create_missing_missing_nfo_ind: Creating missing-nfo indicator %s\n", g->l.nfo_incomplete);
		create_incomplete_nfo2();
	} else if (!findfileext("..", ".nfo")) {
		d_log(1, "create_missing_missing_nfo_ind: Creating missing-nfo indicator (base) %s\n", g->l.nfo_incomplete);
		/* This is not pretty, but should be functional. */
		if ((inc_point[0] = find_last_of(g->l.path, "/")) != g->l.path)
			*inc_point[0] = '\0';
		if ((inc_point[1] = find_last_of(g->v.misc.release_name, "/")) != g->v.misc.release_name)
			*inc_point[1] = '\0';
		create_incomplete_nfo2();
		if (*inc_point[0] == '\0')
			*inc_point[0] = '/';
		if (*inc_point[1] == '\0')
			*inc_point[1] = '/';
	}
}

void
release_complete(GLOBAL *g, MSG *msg, char *_complete[2])
{

	int	n = 0, cnt;
	char	*inc_point[2];
	char	target[NAME_MAX];
	char	*temp_p = 0;

	d_log(1, "release_complete: Caching progress bar\n");
	buffer_progress_bar(&g->v);
	
	printf("%s", convert(&g->v, g->ui, g->gi, zipscript_footer_ok));
	
	d_log(1, "release_complete: Setting complete pointers\n");
	if ( g->v.misc.release_type == RTYPE_AUDIO ) {
		
		d_log(1, "release_complete: Symlinking audio\n");
		if (!strncasecmp(g->l.link_target, "VA", 2) && (g->l.link_target[2] == '-' || g->l.link_target[2] == '_'))
			memcpy(g->v.audio.id3_artist, "VA", 3);

		if (!matchpath(group_dirs, g->l.path)) {

#if ( audio_genre_sort == TRUE )
			d_log(1, "release_complete:   Sorting mp3 by genre (%s)\n", g->v.audio.id3_genre);
			if (g->v.audio.id3_genre)
				createlink(audio_genre_path, g->v.audio.id3_genre, g->l.link_source, g->l.link_target);
#endif

#if ( audio_artist_sort == TRUE )
			d_log(1, "release_complete:   Sorting mp3 by artist\n");
			if (g->v.audio.id3_artist) {
				d_log(1, "release_complete:     - artist: %s\n", g->v.audio.id3_artist);
				if (memcmp(g->v.audio.id3_artist, "VA", 3)) {
					temp_p = ng_realloc(temp_p, 2, 1, 1, &g->v, 1);
					snprintf(temp_p, 2, "%c", toupper(*g->v.audio.id3_artist));
					createlink(audio_artist_path, temp_p, g->l.link_source, g->l.link_target);
					ng_free(temp_p);
				} else
					createlink(audio_artist_path, "VA", g->l.link_source, g->l.link_target);
			}
#endif

#if ( audio_year_sort == TRUE )
			d_log(1, "release_complete:   Sorting mp3 by year (%s)\n", g->v.audio.id3_year);
			if (g->v.audio.id3_year)
				createlink(audio_year_path, g->v.audio.id3_year, g->l.link_source, g->l.link_target);
#endif

#if ( audio_group_sort == TRUE )
			d_log(1, "release_complete:   Sorting mp3 by group\n");
			temp_p = remove_pattern(g->l.link_target, "*-", RP_LONG_LEFT);
			temp_p = remove_pattern(temp_p, "_", RP_SHORT_LEFT);
			n = strlen(temp_p);
			if (n > 0 && n < 15) {
				d_log(1, "release_complete:   - Valid groupname found: %s (%i)\n", temp_p, n);
				createlink(audio_group_path, temp_p, g->l.link_source, g->l.link_target);
			}
#endif

		}
		
#if ( create_m3u == TRUE )
		if (findfileext(".", ".sfv")) {
			d_log(1, "release_complete: Creating m3u\n");
			cnt = sprintf(target, findfileext(".", ".sfv"));
			strlcpy(target + cnt - 3, "m3u", 4);
			create_indexfile(g->l.race, &g->v, target);
		} else
			d_log(1, "release_complete: Cannot create m3u, sfv is missing\n");
#endif

	}
	
	if (_complete[0]) {
		d_log(1, "release_complete: Removing old complete bar, if any\n");
		removecomplete();
	}

	d_log(1, "release_complete: Removing incomplete indicator (%s)\n", g->l.incomplete);
	complete(g);

	if (msg->complete != NULL) {
		d_log(1, "release_complete: Writing COMPLETE and STATS to %s\n", log);
		writelog(g, convert(&g->v, g->ui, g->gi, msg->complete), _complete[1]);
	}
	
	if (_complete[0]) {
		d_log(1, "release_complete: Creating complete bar\n");
		createstatusbar(convert(&g->v, g->ui, g->gi, _complete[0]));
		
#if (chmod_completebar)
		if (!matchpath(group_dirs, g->l.path))
			chmod(convert(&g->v, g->ui, g->gi, _complete[0]), 0222);
		else
			d_log(1, "release_complete: we are in a group_dir - will not chmod the complete bar.\n");
#endif

	}

#if ( enable_complete_script == TRUE )
	execute_script(complete_script, g->v.file.name, "complete");
#endif

	if (!matchpath(group_dirs, g->l.path) || create_incomplete_links_in_group_dirs) {
		/* Creating no-nfo link if needed. */
		n = 0;
		
		if (check_for_missing_nfo_filetypes)
			n = get_nfo_filetype(g->v.misc.release_type);

		if ((g->l.nfo_incomplete) && (!findfileext(".", ".nfo")) &&
			(matchpath(check_for_missing_nfo_dirs, g->l.path) || n)) {
			
			create_missing_missing_nfo_indicator(g, inc_point);
		}

	}

}

void
release_incomplete(GLOBAL *g, MSG *msg, RACETYPE *rtype)
{

	if (g->v.total.files_missing == g->v.total.files >> 1 && g->v.total.files >= min_halfway_files &&
	   ((g->v.total.size * g->v.total.files) >= (min_halfway_size * 1024 * 1024)) && msg->halfway != NULL) {
		d_log(1, "release_incomplete: Writing HALFWAY to %s\n", log);
		writelog(g, convert(&g->v, g->ui, g->gi, msg->halfway), (g->v.total.users > 1 ? rtype->race_halfway : rtype->norace_halfway));
	}
	
	d_log(1, "release_incomplete: Caching progress bar\n");
	buffer_progress_bar(&g->v);

	if (!matchpath(group_dirs, g->l.path) || create_incomplete_links_in_group_dirs) {
		d_log(1, "release_incomplete: Creating incomplete indicator:\n", g->l.incomplete);
		d_log(1, "release_incomplete:    name: '%s', incomplete: '%s', path: '%s'\n", g->v.misc.release_name, g->l.incomplete, g->l.path);
		create_incomplete2();
	}

	d_log(1, "release_incomplete: Creating/moving progress bar\n");
	move_progress_bar(0, &g->v, g->ui, g->gi);
	printf("%s", convert(&g->v, g->ui, g->gi, zipscript_footer_ok));

}
