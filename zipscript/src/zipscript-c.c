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
typedef struct _msg {
	char *complete;
	char *update;
	char *race;
	char *sfv;
	char *newleader;
	char *halfway;
	char *error;
} MSG;

/* Race type strings go in here */
typedef struct _racetype {
	char *race;
	char *newleader;
	char *update;
	char *race_halfway;
	char *norace_halfway;
} RACETYPE;

int handle_zip(GLOBAL *, MSG *, DIR *);
int handle_sfv(GLOBAL *, MSG *, DIR *);
int handle_nfo(GLOBAL *, MSG *, DIR *);
int handle_sfv32(GLOBAL *, MSG *, DIR *, char **, char *, int *);
void read_envdata(GLOBAL *, GDATA *, UDATA *, struct stat *);
void check_filesize(GLOBAL *, const char *, struct stat *);
void lock_release(GLOBAL *, DIR *, DIR *);
void set_uid_gid(void);
void group_dir_users(GLOBAL *); /* call this something else */
int match_nocheck_dirs(GLOBAL *);

/* turn these two into one function */
int check_zerosize(GLOBAL *, MSG *);
int check_banned_file(GLOBAL *, MSG *);

int process_file(GLOBAL *, MSG *, DIR *, char **, char *, char **, int *, int *);
void check_release_type(GLOBAL *, MSG *, RACETYPE *, char *[2]);

void execute_script(char *, char *, char *);
int get_nfo_filetype(unsigned int);
void create_missing_missing_nfo_ind(GLOBAL *, char *[2], DIR *);
void release_complete(GLOBAL *, MSG *, DIR *, DIR *, char *[2]);
void release_incomplete(GLOBAL *, MSG *, RACETYPE *);

int 
main(int argc, char **argv)
{
	GLOBAL		g; /* this motherfucker owns */
	MSG		msg;
	GDATA		gdata;
	UDATA		udata;
	RACETYPE	rtype;
	DIR		*dir, *parent;
	
	char           *fileext = NULL, *name_p, *temp_p = NULL;
	char           *target = 0;
	char			*_complete[2] = { 0 }; /* 0 = bar, 1 = announce */
	int				exit_value = EXIT_SUCCESS;
	int				no_check = FALSE;
	int		n = 0;
	char	*nfofound = 0;
//#if ( del_banned_release || enable_banned_script )
	int		deldir = 0;
//#endif
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
	target = ng_realloc2(target, n + 256, 1, 1, 1);
	g.ui = ng_realloc2(g.ui, sizeof(struct USERINFO *) * 30, 1, 1, 1);
	g.gi = ng_realloc2(g.gi, sizeof(struct GROUPINFO *) * 30, 1, 1, 1);

	d_log(1, "zipscript-c: Copying data g.l into memory\n");
	sprintf(g.l.sfv, storage "/%s/sfvdata", g.l.path);
	sprintf(g.l.leader, storage "/%s/leader", g.l.path);
	sprintf(g.l.race, storage "/%s/racedata", g.l.path);
	g.v.user.pos = 0;
	sprintf(g.v.misc.old_leader, "none");
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

	d_log(1, "zipscript-c: Reading directory structure\n");
	dir = opendir(".");
	parent = opendir("..");

	d_log(1, "zipscript-c: Caching release name\n");
	getrelname(&g);

	d_log(1, "zipscript-c: Creating directory to store racedata in\n");
	maketempdir(g.l.path);

	d_log(1, "zipscript-c: Locking release\n");
	lock_release(&g, dir, parent);

	printf(zipscript_header);

	nfofound = findfileext(dir, ".nfo");

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

		if (exit_value < 2)
			exit_value = process_file(&g, &msg, dir, argv, fileext, &nfofound, &no_check, &deldir);

	}

	if (no_check == TRUE) {	/* File was not checked */
		
		printf(zipscript_any_ok);
		printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_skip));
		
		if (matchpath(speedtest_dirs, g.l.path)) {
			d_log(1, "zipscript-c: writing speedtest to channel\n");
			writelog(&g, convert(&g.v, g.ui, g.gi, speed_announce), speed_type);
			exit_value = 2;
		}

	} else if (exit_value == EXIT_SUCCESS) { /* File was checked */

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

			if (!enable_files_ahead || ((g.v.total.users > 1 && g.ui[g.ui[0]->pos]->files >= (g.ui[g.ui[1]->pos]->files + newleader_files_ahead)) || g.v.total.users == 1)) {
				d_log(1, "zipscript-c: Writing current leader to file\n");
				read_write_leader(g.l.leader, &g.v, g.ui[g.ui[0]->pos]);
			}
			if (g.v.total.users > 1) {
				if (g.ui[g.v.user.pos]->files == 1 && msg.race != NULL) {
					d_log(1, "zipscript-c: Writing RACE to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.race), rtype.race);
				}
				if (g.v.total.files >= min_newleader_files && ((g.v.total.size * g.v.total.files) >= (min_newleader_size * 1024 * 1024)) && strcmp(g.v.misc.old_leader, g.ui[g.ui[0]->pos]->name) && msg.newleader != NULL && g.ui[g.ui[0]->pos]->files >= (g.ui[g.ui[1]->pos]->files + newleader_files_ahead) && g.v.total.files_missing) {
					d_log(1, "zipscript-c: Writing NEWLEADER to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.newleader), rtype.newleader);
				}
			} else {
				if (g.ui[g.v.user.pos]->files == 1 && g.v.total.files >= min_update_files && ((g.v.total.size * g.v.total.files) >= (min_update_size * 1024 * 1024)) && msg.update) {
					d_log(1, "zipscript-c: Writing UPDATE to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.update), rtype.update);
				}
			}
		}
		
		if (g.v.total.files_missing > 0)
			release_incomplete(&g, &msg, &rtype);
		else if ((g.v.total.files_missing == 0) && (g.v.total.files > 0))
			release_complete(&g, &msg, dir, parent, _complete);
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
	if (!nfofound && findfileext(dir, ".nfo"))
		execute_script(nfo_script, g.v.file.name, "nfo");
#endif

	if ((findfileext(dir, ".nfo") || (findfileextparent(parent, ".nfo"))) && (g.l.nfo_incomplete)) {
		d_log(1, "zipscript-c: Removing missing-nfo indicator (if any)\n");
		remove_nfo_indicator(&g);
	}

#if ( del_banned_release )
	if (deldir) {
		d_log(1, "zipscript-c: del_banned_release is set - removing entire dir.\n");
		move_progress_bar(1, &g.v, g.ui, g.gi);
		if (g.l.incomplete)
			unlink(g.l.incomplete);
		rewinddir(dir);
		del_releasedir(dir, g.l.path);
	}
#endif

#if ( enable_banned_script )
	if (deldir)
		execute_script(banned_script, g.v.file.name, "banned");
#endif

	d_log(1, "zipscript-c: Releasing memory and removing lock\n");
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

int
handle_zip(GLOBAL *g, MSG *msg, DIR *dir) {

	//char           *error_msg = 0;
	unsigned char	exit_value = EXIT_SUCCESS;
	char			target[strlen(unzip_bin) + 10 + NAME_MAX];
	char 			*fileext;
	long		loc;
	struct dirent	*dp;

	d_log(1, "zipscript-c: File type is: ZIP\n");
	d_log(1, "zipscript-c: Testing file integrity with %s\n", unzip_bin);
	if (!fileexists(unzip_bin)) {
		d_log(1, "zipscript-c: ERROR! Not able to check zip-files - %s does not exists!\n", unzip_bin);
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
			d_log(1, "zipscript-c: Integrity check failed (#%d): %s\n", errno, strerror(errno));
			sprintf(g->v.misc.error_msg, BAD_ZIP);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_zip_type);
			exit_value = 2;
			return exit_value;
		}
#if (test_for_password || extract_nfo || zip_clean)
			if (!findfileextcount(dir, ".nfo") || findfileextcount(dir, ".zip") == 1) {
				if (check_zipfile(".unzipped", g->v.file.name)) {
					d_log(1, "handle_zip: File is password protected.\n");
					sprintf(g->v.misc.error_msg, PASSWORD_PROTECTED);
					msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
					if (exit_value < 2)
						writelog(g, msg->error, bad_file_password_type);
					exit_value = 2;
					return exit_value;
				}
			}
#endif
	}
	d_log(1, "zipscript-c: Integrity ok\n");
	printf(zipscript_zip_ok);

	if ((matchpath(zip_dirs, g->l.path)) || (matchpath(group_dirs, g->l.path))  ) {
		d_log(1, "zipscript-c: Directory matched with zip_dirs/group_dirs\n");
	} else {
		d_log(1, "zipscript-c: WARNING! Directory did not match with zip_dirs/group_dirs\n");
		if (strict_path_match == TRUE) {
			d_log(1, "zipscript-c: Strict mode on - exiting\n");
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
		d_log(1, "zipscript-c: file_id.diz does not exist, trying to extract it from %s\n", g->v.file.name);
		sprintf(target, "%s -qqjnCLL \"%s\" file_id.diz 2>.delme", unzip_bin, g->v.file.name);
		if (execute(target) != 0)
			d_log(1, "zipscript-c: No file_id.diz found (#%d): %s\n", errno, strerror(errno));
		else {
			if ((loc = findfile(dir, "file_id.diz.bad"))) {
				seekdir(dir, loc);
				dp = readdir(dir);
				unlink(dp->d_name);
			}
			chmod("file_id.diz", 0666);
		}
	}
	d_log(1, "zipscript-c: Reading diskcount from diz:\n");
	g->v.total.files = read_diz("file_id.diz");
	d_log(1, "zipscript-c:    Expecting %d files.\n", g->v.total.files);

	if (g->v.total.files == 0) {
		d_log(1, "zipscript-c:    Could not get diskcount from diz.\n");
		g->v.total.files = 1;
		unlink("file_id.diz");
	}
	g->v.total.files_missing = g->v.total.files;

	d_log(1, "zipscript-c: Storing new race data\n");
	writerace(g->l.race, &g->v, 0, F_CHECKED);
	d_log(1, "zipscript-c: Reading race data from file to memory\n");
	readrace(g->l.race, &g->v, g->ui, g->gi);
	if (g->v.total.files_missing < 0) {
		d_log(1, "zipscript-c: There seems to be more files in zip than we expected\n");
		g->v.total.files -= g->v.total.files_missing;
		g->v.total.files_missing = 0;
		g->v.misc.write_log = FALSE;
	}
	d_log(1, "zipscript-c: Setting message pointers\n");
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

	d_log(1, "zipscript-c: File type is: SFV\n");
	if ((matchpath(sfv_dirs, g->l.path)) || (matchpath(group_dirs, g->l.path))  ) {
		d_log(1, "zipscript-c: Directory matched with sfv_dirs/group_dirs\n");
	} else {
		d_log(1, "zipscript-c: WARNING! Directory did not match with sfv_dirs/group_dirs\n");
		if (strict_path_match == TRUE) {
			d_log(1, "zipscript-c: Strict mode on - exiting\n");
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
			d_log(1, "zipscript-c: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g->v.file.size));
			d_log(1, "zipscript-c: No double sfv allowed\n");
			msg->error = convert(&g->v, g->ui, g->gi, deny_double_msg);
			writelog(g, msg->error, general_doublesfv_type);
			sprintf(g->v.misc.error_msg, DOUBLE_SFV);
			mark_as_bad(g->v.file.name);
			exit_value = 2;
			return exit_value;
		} else if (findfileextcount(dir, ".sfv") > 1 && sfv_compare_size(".sfv", g->v.file.size) > 0) {
			d_log(1, "zipscript-c: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g->v.file.size));
			d_log(1, "zipscript-c: Reading remainders of old sfv\n");
			readsfv(g->l.sfv, &g->v, 1);
			cnt = g->v.total.files - g->v.total.files_missing;
			cnt2 = g->v.total.files;
			g->v.total.files_missing = g->v.total.files = 0;
			//readsfv_ffile(g->v.file.name, g->v.file.size, g->v);
			readsfv_ffile(&g->v);
			if ((g->v.total.files <= cnt2) || (g->v.total.files != (cnt + g->v.total.files_missing))) {
				d_log(1, "zipscript-c: Old sfv seems to match with more files than current one\n");
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
			d_log(1, "zipscript-c: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g->v.file.size));
			d_log(1, "zipscript-c: Hmm.. Seems the old .sfv was deleted. Allowing new one.\n");
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
	d_log(1, "zipscript-c: Parsing sfv and creating sfv data\n");
	if (copysfv(g->v.file.name, g->l.sfv, &g->v, g->l.path, 0)) {
		d_log(1, "zipscript-c: Found invalid entries in SFV.\n");
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
			d_log(1, "zipscript-c: Testing files marked as untested\n");
			testfiles(&g->l, &g->v, 0);
		}
	}
	d_log(1, "zipscript-c: Reading file count from SFV\n");
	readsfv(g->l.sfv, &g->v, 0);

#if (smart_sfv_write && sfv_cleanup)
	d_log(1, "zipscript-c: Rewriting sfv file according to smart_sfv_write\n");
	sfvdata_to_sfv(g->l.sfv, findfileext(dir, ".sfv"));
#endif
	
	if (g->v.total.files == 0) {
		d_log(1, "zipscript-c: SFV seems to have no files of accepted types, or has errors.\n");
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
		d_log(1, "zipscript-c: Reading race data from file to memory\n");
		readrace(g->l.race, &g->v, g->ui, g->gi);
	}
	if (del_completebar) {
		d_log(1, "zipscript-c: Making sure that release is not marked as complete\n");
		removecomplete();
	}
	d_log(1, "zipscript-c: Setting message pointers\n");
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
		d_log(1, "zipscript-c: WARNING! Not a known release type - Contact the authors! (1:%d)\n", g->v.misc.release_type);
		break;
	}

	if (!msg->sfv)
		d_log(1, "zipscript-c: Something's messed up - msg->sfv not set!\n");

	msg->halfway = msg->newleader = msg->race = msg->update = NULL;

	if (g->v.total.files_missing > 0) {
		if (msg->sfv != NULL) {
			d_log(1, "zipscript-c: Writing SFV message to %s\n", log);
			writelog(g, convert(&g->v, g->ui, g->gi, msg->sfv), sfv_type);
		}
	} else {
		if (g->v.misc.release_type == RTYPE_AUDIO) {
			d_log(1, "zipscript-c: Reading audio info for completebar\n");
			get_mpeg_audio_info(findfileext(dir, ".mp3"), &g->v.audio);
		}
	}

	return exit_value;
}

int
handle_nfo(GLOBAL *g, MSG *msg, DIR *dir) {
	
	unsigned char	no_check = FALSE;
	unsigned char	exit_value = EXIT_SUCCESS;

	no_check = TRUE;
	d_log(1, "zipscript-c: File type is: NFO\n");
#if ( deny_double_nfo )
	if (findfileextcount(dir, ".nfo") > 1) {
		sprintf(g->v.misc.error_msg, DUPE_NFO);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_nfo_type);
		if (!deny_double_nfo_warn) {
			d_log(1, "zipscript-c: Looks like there already is a nfo uploaded. Denying this one.\n");
			exit_value = 2;
		} else
			d_log(1, "zipscript-c: Looks like there already is a nfo uploaded. Warn on -allowing anyway.\n");
		return exit_value;
	}
#endif
#if ( deny_nfo_upload_in_zip )
	if (matchpath(zip_dirs, g->l.path)) {
		d_log(1, "handle_nfo: nfo-files not allowed in zip_dirs.\n");
		sprintf(g->v.misc.error_msg, NFO_DENIED);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_nfodenied_type);
		exit_value = 2;
		return exit_value;
	}

#endif
	return exit_value;
}

int
handle_sfv32(GLOBAL *g, MSG *msg, DIR *dir, char **argv, char *fileext, int *deldir)
{
	
	unsigned int	crc, s_crc = 0;
	unsigned char	no_check = FALSE;
	char           *target = 0;
	unsigned char	exit_value = EXIT_SUCCESS;

	d_log(1, "zipscript-c: File type is: ANY\n");

	if (check_rarfile(g->v.file.name)) {
		d_log(1, "handle_sfv32: File is password protected.\n");
		sprintf(g->v.misc.error_msg, PASSWORD_PROTECTED);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_password_type);
		exit_value = 2;
		return exit_value;
	}

	d_log(1, "zipscript-c: Converting crc (%s) from string to integer\n", argv[3]);
	crc = hexstrtodec(argv[3]);
	if (crc == 0) {
		d_log(1, "zipscript-c: We did not get crc from ftp daemon, calculating crc for %s now.\n", g->v.file.name);
		crc = calc_crc32(g->v.file.name);
	}
	if (fileexists(g->l.sfv)) {
		s_crc = readsfv(g->l.sfv, &g->v, 0);

#if (sfv_calc_single_fname == TRUE)
		if (s_crc == 0 && g->v.misc.in_sfvfile == TRUE) {
			d_log(1, "zipscript-c: CRC in SFV is 0 - trying to calculate it from the file\n");
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

		d_log(1, "zipscript-c: DEBUG: crc: %X - s_crc: %X - match:%d\n", crc, s_crc, g->v.misc.sfv_match);
		if (s_crc != crc && !(s_crc == 0 && g->v.misc.sfv_match)) {
			if (s_crc == 0 && !g->v.misc.sfv_match) {
				if (!strcomp(allowed_types, fileext)) {
#if (allow_files_not_in_sfv == TRUE)
					d_log(1, "zipscript-c: Filename was not found in the SFV, but allowing anyway\n");
					no_check = TRUE;
					return exit_value;
#endif
					d_log(1, "zipscript-c: Filename was not found in the SFV\n");
					strlcpy(g->v.misc.error_msg, NOT_IN_SFV, 80);
				} else {
					d_log(1, "zipscript-c: filetype is part of allowed_types.\n");
					no_check = TRUE;
					return exit_value;
				}
			} else {
				d_log(1, "zipscript-c: CRC-32 check failed\n");
				if (!hexstrtodec(argv[3]) && allow_file_resume) {
					d_log(1, "zipscript-c: Broken xfer detected - allowing file.\n");
					no_check = TRUE;
					msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
					writelog(g, msg->error, bad_file_crc_type);
					if (enable_unduper_script == TRUE) {
						if (!fileexists(unduper_script)) {
							d_log(1, "zipscript-c: Warning - undupe script (%s) does not exist.\n", unduper_script);
						}
						sprintf(target, unduper_script " \"%s\"", g->v.file.name);
						if (execute(target) == 0)
							d_log(1, "zipscript-c: undupe of %s successful.\n", g->v.file.name);
						else
							d_log(1, "zipscript-c: undupe of %s failed.\n", g->v.file.name);
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
			d_log(1, "zipscript-c: File was found in sfv, but had no crc.\n");
			s_crc = crc;
			update_sfvdata(g->l.sfv, g->v.file.name, s_crc);
#if (smart_sfv_write && sfv_cleanup)
			sfvdata_to_sfv(g->l.sfv, findfileext(dir, ".sfv"));
#endif

		}
#if (sfv_cleanup_lowercase == TRUE)
		if (check_dupefile(dir, g->v.file.name)) {
			d_log(1, "zipscript-c: dupe detected - same file, different case already exists.\n");
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
		d_log(1, "zipscript-c: Storing new race data\n");
		writerace(g->l.race, &g->v, crc, F_CHECKED);
	} else {
#if (force_sfv_first == TRUE )
# if (use_partial_on_noforce == TRUE)
		if (!matchpartialpath(noforce_sfv_first_dirs, g->l.path) && !matchpath(zip_dirs, g->l.path)) {
# else
		if (!matchpath(noforce_sfv_first_dirs, g->l.path) && !matchpath(zip_dirs, g->l.path)) {
# endif
			d_log(1, "zipscript-c: SFV needs to be uploaded first\n");
			strlcpy(g->v.misc.error_msg, SFV_FIRST, 80);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_nosfv_type);
			exit_value = 2;
			return exit_value;
		} else if (matchpath(zip_dirs, g->l.path) && (!fileexists(g->l.sfv))) {
			d_log(1, "zipscript-c: This looks like a file uploaded the wrong place - Not allowing it.\n");
			strlcpy(g->v.misc.error_msg, SFV_FIRST, 80);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_nosfv_type);
			exit_value = 2;
			return exit_value;
		} else {
			d_log(1, "zipscript-c: path matched with noforce_sfv_first or zip_dirs - allowing file.\n");
			printf(zipscript_SFV_skip);
			d_log(1, "zipscript-c: Storing new race data\n");
			writerace(g->l.race, &g->v, crc, F_NOTCHECKED);
		}
#else
		d_log(1, "zipscript-c: Could not check file yet - SFV is not present\n");
		printf(zipscript_SFV_skip);
		d_log(1, "zipscript-c: Storing new race data\n");
		writerace(g->l.race, &g->v, crc, F_NOTCHECKED);
#endif
	}

	d_log(1, "zipscript-c: Reading race data from file to memory\n");
	readrace(g->l.race, &g->v, g->ui, g->gi);

	d_log(1, "zipscript-c: Setting pointers\n");
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
		d_log(1, "zipscript-c: Trying to read audio header and tags\n");
		get_mpeg_audio_info(g->v.file.name, &g->v.audio);
		write_bitrate_in_race(g->l.race, &g->v);
		sprintf(g->v.audio.bitrate, "%i", read_bitrate_in_race(g->l.race, &g->v));
			if ((enable_mp3_script == TRUE) && (g->ui[g->v.user.pos]->files == 1)) {
				if (!fileexists(mp3_script)) {
					d_log(1, "zipscript-c: Warning -  mp3_script (%s) - file does not exists\n", mp3_script);
				}
				d_log(1, "zipscript-c: Executing mp3 script (%s %s)\n", mp3_script, convert(&g->v, g->ui, g->gi, mp3_script_cookies));
				sprintf(target, "%s %s", mp3_script, convert(&g->v, g->ui, g->gi, mp3_script_cookies));
				if (execute(target) != 0)
					d_log(1, "zipscript-c: Failed to execute mp3_script: %s\n", strerror(errno));
			}
			if (!matchpath(audio_nocheck_dirs, g->l.path)) {
#if ( audio_banned_genre_check )
				if (strcomp(banned_genres, g->v.audio.id3_genre)) {
					d_log(1, "zipscript-c: File is from banned genre\n");
					sprintf(g->v.misc.error_msg, BANNED_GENRE, g->v.audio.id3_genre);
					if (audio_genre_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log(1, "zipscript-c: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_genre_warn_msg);
							writelog(g, msg->error, general_badgenre_type);
						} else
							d_log(1, "zipscript-c: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_genre_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					*deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#elif ( audio_allowed_genre_check == TRUE )
				if (!strcomp(allowed_genres, g->v.audio.id3_genre)) {
					d_log(1, "zipscript-c: File is not in allowed genre\n");
					sprintf(g->v.misc.error_msg, BANNED_GENRE, g->v.audio.id3_genre);
					if (audio_genre_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log(1, "zipscript-c: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_genre_warn_msg);
							writelog(g, msg->error, general_badgenre_type);
						} else
							d_log(1, "zipscript-c: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_genre_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					*deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#endif
#if ( audio_year_check == TRUE )
				if (!strcomp(allowed_years, g->v.audio.id3_year)) {
					d_log(1, "zipscript-c: File is from banned year\n");
					sprintf(g->v.misc.error_msg, BANNED_YEAR, g->v.audio.id3_year);
					if (audio_year_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log(1, "zipscript-c: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_year_warn_msg);
							writelog(g, msg->error, general_badyear_type);
						} else
							d_log(1, "zipscript-c: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_year_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					*deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#endif
#if ( audio_cbr_check == TRUE )
				if (g->v.audio.is_vbr == 0) {
					if (!strcomp(allowed_constant_bitrates, g->v.audio.bitrate)) {
						d_log(1, "zipscript-c: File is encoded using banned bitrate\n");
						sprintf(g->v.misc.error_msg, BANNED_BITRATE, g->v.audio.bitrate);
						if (audio_cbr_warn == TRUE) {
							if (g->ui[g->v.user.pos]->files == 1) {
								d_log(1, "zipscript-c: warn on - logging to logfile\n");
								msg->error = convert(&g->v, g->ui, g->gi, audio_cbr_warn_msg);
								writelog(g, msg->error, general_badbitrate_type);
							} else
								d_log(1, "zipscript-c: warn on - have already logged to logfile\n");
						} else {
							mark_as_bad(g->v.file.name);
							msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
							if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
								writelog(g, msg->error, bad_file_bitrate_type);
							exit_value = 2;
						}
#if ( del_banned_release || enable_banned_script )
						*deldir = 1;
						exit_value = 2;
#endif
						break;
					}
				}
#endif
#if ( audio_allowed_vbr_preset_check == TRUE )
				if (g->v.audio.is_vbr && strlen(g->v.audio.vbr_preset) && !strcomp(allowed_vbr_presets, g->v.audio.vbr_preset)) {
					d_log(1, "zipscript-c: File is not in allowed vbr preset list (%s)\n", g->v.audio.vbr_preset);
					sprintf(g->v.misc.error_msg, BANNED_PRESET, g->v.audio.vbr_preset);
					if (audio_vbr_preset_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log(1, "zipscript-c: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_vbr_preset_warn_msg);
							writelog(g, msg->error, general_badpreset_type);
						} else
							d_log(1, "zipscript-c: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_vbr_preset_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					*deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#endif
			} else
				d_log(1, "zipscript-c: user is in a no audio check dir - skipping checks.\n");

		if (realtime_mp3_info != DISABLED) {
			d_log(1, "zipscript-c: Printing realtime_mp3_info.\n");
			printf("%s", convert(&g->v, g->ui, g->gi, realtime_mp3_info));
		}
		break;
	case RTYPE_VIDEO:
		d_log(1, "zipscript-c: Trying to read video header\n");
		if (!memcmp(fileext, "avi", 3)) {
			bzero(&g->m, sizeof(struct MULTIMEDIA));
			avinfo(g->v.file.name, &g->m);
			d_log(1, "zipscript-c: avinfo: video - %dx%d * %.0f of %s (%s).\n", g->m.height, g->m.width, g->m.fps, g->m.vids, g->m.audio);
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
		d_log(1, "zipscript-c: WARNING! Not a known release type - Contact the authors! (2:%d)\n", g->v.misc.release_type);
		break;
	}

	if (!msg->race)
		d_log(1, "zipscript-c: Something's messed up - msg->race not set!\n");

	if (exit_value == EXIT_SUCCESS) {
		d_log(1, "zipscript-c: Removing missing indicator\n");
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
		d_log(1, "zipscript-c: We are running from shell, falling back to default values for $USER, $GROUP, $TAGLINE, $SECTION and $SPEED\n");
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

		d_log(1, "zipscript-c: Reading section from env (%s)\n", getenv("SECTION"));
		snprintf(g->v.sectionname, 127, getenv("SECTION"));
		g->v.section = 0;
		temp_p_free = temp_p = strdup((const char *)gl_sections);	/* temp_p_free is needed since temp_p is modified by strsep */
		if ((temp_p) == NULL) {
			d_log(1, "zipscript-c: Can't allocate memory for sections\n");
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
		d_log(1, "zipscript-c: Failed to stat file: %s\n", strerror(errno));
		g->v.file.size = 0;
		g->v.total.stop_time = 0;
	} else {
		g->v.file.size = fileinfo->st_size;
		d_log(1, "zipscript-c: File size was: %d\n", g->v.file.size);
		g->v.total.stop_time = fileinfo->st_mtime;
	}

}

void
lock_release(GLOBAL *g, DIR *dir, DIR *parent)
{

	int		m, n;

	while (1) {
	
		if ((m = create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 3, 0))) {
			d_log(1, "zipscript-c: Failed to lock release.\n");
			
			if (m == 1) {
				d_log(1, "zipscript-c: version mismatch. Exiting.\n");
				printf("Error. You need to rm -fR ftp-data/pzs-ng/* before zipscript-c will work.\n");
				exit(EXIT_FAILURE);
			}
			
			if (m == PROGTYPE_RESCAN) {
				d_log(1, "zipscript-c: Detected rescan running - will try to make it quit.\n");
				update_lock(&g->v, 0, 0);
			}

			if (m == PROGTYPE_POSTDEL) {
				n = (signed int)g->v.lock.data_incrementor;
				d_log(1, "zipscript-c: Detected postdel running - sleeping for one second.\n");
				if (!create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 0, g->v.lock.data_queue))
					break;
				usleep(1000000);
				if (!create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 0, g->v.lock.data_queue))
					break;
				if ( n == (signed int)g->v.lock.data_incrementor) {
					d_log(1, "zipscript-c: Failed to get lock. Forcing unlock.\n");
					if (create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 2, g->v.lock.data_queue)) {
						d_log(1, "zipscript-c: Failed to force a lock.\n");
						d_log(1, "zipscript-c: Exiting with error.\n");
						exit(EXIT_FAILURE);
					}
					break;
				}

			} else {
			
				for ( n = 0; n <= max_seconds_wait_for_lock * 10; n++) {
					d_log(1, "zipscript-c: sleeping for .1 second before trying to get a lock.\n");
					usleep(100000);
					if (!(m = create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 0, g->v.lock.data_queue)))
						break;	
				}
				
				if (n >= max_seconds_wait_for_lock * 10) {
					if (m == PROGTYPE_RESCAN) {
						d_log(1, "zipscript-c: Failed to get lock. Forcing unlock.\n");
						if (create_lock(&g->v, g->l.path, PROGTYPE_ZIPSCRIPT, 2, g->v.lock.data_queue))
							d_log(1, "zipscript-c: Failed to force a lock.\n");
					} else
						d_log(1, "zipscript-c: Failed to get a lock.\n");
					if (!g->v.lock.data_in_use && !ignore_lock_timeout) {
						d_log(1, "zipscript-c: Exiting with error.\n");
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

void
set_uid_gid(void)
{

	if ( program_uid > 0 ) {
		d_log(1, "zipscript-c: Trying to change effective uid/gid\n");
		setegid(program_gid);
		seteuid(program_uid);
	} else if (!geteuid()) {
		d_log(1, "zipscript-c: +s mode detected - trying to change effective uid/gid to !root\n");
		if (setegid(getgid()) == -1)
			d_log(1, "zipscript-c: failed to change gid: %s\n", strerror(errno));
		if (seteuid(getuid()) == -1)
			d_log(1, "zipscript-c: failed to change uid: %s\n", strerror(errno));
	}

}

void
group_dir_users(GLOBAL *g)
{

	if (matchpath(group_dirs, g->l.path) && (hide_group_uploaders == TRUE)) {
		d_log(1, "zipscript-c: Hiding user in group-dir:\n");
		if (strlen(hide_gname) > 0) {
			snprintf(g->v.user.group, 18, "%s", hide_gname);
			d_log(1, "zipscript-c:    Changing groupname\n");
		}
		if (strlen(hide_uname) > 0) {
			snprintf(g->v.user.name, 18, "%s", hide_uname);
			d_log(1, "zipscript-c:    Changing username\n");
		}
		if (strlen(hide_uname) == 0) {
			d_log(1, "zipscript-c:    Making username = groupname\n");
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
		
		d_log(1, "zipscript-c: Directory matched with nocheck_dirs, or is not among sfv/zip/group dirs\n");
		d_log(1, "zipscript-c:   - nocheck_dirs  : '%s'\n", nocheck_dirs);
		d_log(1, "zipscript-c:   - speedtest_dirs: '%s'\n", speedtest_dirs);
		d_log(1, "zipscript-c:   - zip_dirs      : '%s'\n", zip_dirs);
		d_log(1, "zipscript-c:   - sfv_dirs      : '%s'\n", sfv_dirs);
		d_log(1, "zipscript-c:   - group_dirs    : '%s'\n", group_dirs);
		d_log(1, "zipscript-c:   - current path  : '%s'\n", g->l.path);
		
		return TRUE;

	} else
		return FALSE;

}

int
check_zerosize(GLOBAL *g, MSG *msg)
{
	
	if (g->v.file.size == 0) {
		d_log(1, "zipscript-c: File seems to be 0 bytes\n");
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
		d_log(1, "zipscript-c: Banned file detected (%s)\n", g->v.file.name);
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
process_file(GLOBAL *g, MSG *msg, DIR *dir, char **argv, char *fileext, char **nfofound, int *no_check, int *deldir)
{
		
	/* Process file */
	d_log(1, "zipscript-c: Verifying old racedata\n");
	if (!verify_racedata(g->l.race, &g->v))
		d_log(1, "zipscript-c:   Failed to open racedata - assuming this is a new race.\n");

	switch (get_filetype(g, fileext)) {

		case 0:	/* ZIP CHECK */
			return handle_zip(g, msg, dir);
			break;

		case 1:	/* SFV CHECK */
			return handle_sfv(g, msg, dir);
			break;
		
		case 2:	/* NFO CHECK */
			return handle_nfo(g, msg, dir);
			*nfofound = 0;
			break;

		case 3:	/* SFV BASED CRC-32 CHECK */
			return handle_sfv32(g, msg, dir, argv, fileext, deldir);
			break;

		case 4:	/* ACCEPTED FILE */
			d_log(1, "zipscript-c: File type: NO CHECK\n");
			*no_check = TRUE;
			break;
		/* END OF ACCEPTED FILE CHECK */

		case 255:	/* UNKNOWN - WE DELETE THESE, SINCE IT WAS
				 * ALSO IGNORED */
			d_log(1, "zipscript-c: File type: UNKNOWN [ignored in sfv]\n");
			sprintf(g->v.misc.error_msg, UNKNOWN_FILE, fileext);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			writelog(g, msg->error, bad_file_disallowed_type);
			return 2;
			break;
			/* END OF UNKNOWN CHECK */
			
	}

	return EXIT_SUCCESS;

}

void
check_release_type(GLOBAL *g, MSG *msg, RACETYPE *rtype, char *_complete[2])
{

	switch (g->v.misc.release_type) {
		case RTYPE_RAR:
			rtype->race = rar_announce_race_type;
			rtype->newleader = rar_announce_newleader_type;
			rtype->update = rar_announce_update_type;
			rtype->norace_halfway = rar_announce_norace_halfway_type;
			rtype->race_halfway = rar_announce_race_halfway_type;
			_complete[0] = rar_completebar;
			_complete[1] = CHOOSE2(g->v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
			msg->complete = CHOOSE2(g->v.total.users, rar_complete, rar_norace_complete);
			break;	/* rar */
		case RTYPE_OTHER:
			rtype->race = other_announce_race_type;
			rtype->newleader = other_announce_newleader_type;
			rtype->update = other_announce_update_type;
			rtype->norace_halfway = other_announce_norace_halfway_type;
			rtype->race_halfway = other_announce_race_halfway_type;
			_complete[0] = other_completebar;
			_complete[1] = CHOOSE2(g->v.total.users, other_announce_race_complete_type, other_announce_norace_complete_type);
			msg->complete = CHOOSE2(g->v.total.users, other_complete, other_norace_complete);
			break;	/* other */
		case RTYPE_AUDIO:
			rtype->race = audio_announce_race_type;
			rtype->newleader = audio_announce_newleader_type;
			if (!g->v.audio.is_vbr)
				rtype->update = audio_announce_cbr_update_type;
			else
				rtype->update = audio_announce_vbr_update_type;
			rtype->norace_halfway = audio_announce_norace_halfway_type;
			rtype->race_halfway = audio_announce_race_halfway_type;
			_complete[0] = audio_completebar;
			
			if (!g->v.audio.is_vbr) {
				_complete[1] = CHOOSE2(g->v.total.users, audio_cbr_announce_race_complete_type, audio_cbr_announce_norace_complete_type);
			} else {
				_complete[1] = CHOOSE2(g->v.total.users, audio_vbr_announce_race_complete_type, audio_vbr_announce_norace_complete_type);
			}

			msg->complete = CHOOSE2(g->v.total.users, audio_complete, audio_norace_complete);
			break;	/* audio */
		case RTYPE_VIDEO:
			rtype->race = video_announce_race_type;
			rtype->newleader = video_announce_newleader_type;
			rtype->update = video_announce_update_type;
			rtype->norace_halfway = video_announce_norace_halfway_type;
			rtype->race_halfway = video_announce_race_halfway_type;
			_complete[0] = video_completebar;
			_complete[1] = CHOOSE2(g->v.total.users, video_announce_race_complete_type, video_announce_norace_complete_type);
			msg->complete = CHOOSE2(g->v.total.users, video_complete, video_norace_complete);
			break;	/* video */
		case RTYPE_NULL:
			rtype->race = zip_announce_race_type;
			rtype->newleader = zip_announce_newleader_type;
			rtype->update = zip_announce_update_type;
			rtype->norace_halfway = zip_announce_norace_halfway_type;
			rtype->race_halfway = zip_announce_race_halfway_type;
			_complete[0] = zip_completebar;
			_complete[1] = CHOOSE2(g->v.total.users, zip_announce_race_complete_type, zip_announce_norace_complete_type);
			msg->complete = CHOOSE2(g->v.total.users, zip_complete, zip_norace_complete);
			break;	/* zip */
		default:
			rtype->race = rar_announce_race_type;
			rtype->newleader = rar_announce_newleader_type;
			rtype->update = rar_announce_update_type;
			rtype->norace_halfway = rar_announce_norace_halfway_type;
			rtype->race_halfway = rar_announce_race_halfway_type;
			_complete[0] = rar_completebar;
			_complete[1] = CHOOSE2(g->v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
			msg->complete = CHOOSE2(g->v.total.users, rar_complete, rar_norace_complete);
			d_log(1, "zipscript-c: WARNING! Not a known release type - Contact the authors! (3:%d)\n", g->v.misc.release_type);
			break;	/* rar */
	}

}

void
execute_script(char *script, char *arg, char *type)
{

	static char target[PATH_MAX];
	
	if (!fileexists(script))
		d_log(1, "zipscript-c: Warning - %s_script (%s) - file does not exists\n", type, script);

	d_log(1, "zipscript-c: Executing %s script\n", type);
	
	snprintf(target, PATH_MAX, "%s \"%s\"", script, arg);
	
	if (execute(target) != 0)
		d_log(1, "zipscript-c: Failed to execute %s_script: %s\n", type, strerror(errno));
		
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
create_missing_missing_nfo_ind(GLOBAL *g, char *inc_point[2], DIR *parent)
{

	if (!g->l.in_cd_dir) {
		d_log(1, "zipscript-c: Creating missing-nfo indicator %s\n", g->l.nfo_incomplete);
		create_incomplete_nfo2();
	} else if (!findfileextparent(parent, ".nfo")) {
		d_log(1, "zipscript-c: Creating missing-nfo indicator (base) %s\n", g->l.nfo_incomplete);
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
release_complete(GLOBAL *g, MSG *msg, DIR *dir, DIR *parent, char *_complete[2])
{

	char			*inc_point[2];
	char			target[NAME_MAX];
	char			*temp_p = 0;
	int				n = 0, cnt;

	d_log(1, "zipscript-c: Caching progress bar\n");
	buffer_progress_bar(&g->v);
	
	printf("%s", convert(&g->v, g->ui, g->gi, zipscript_footer_ok));
	
	d_log(1, "zipscript-c: Setting complete pointers\n");
	if ( g->v.misc.release_type == RTYPE_AUDIO ) {
		
		d_log(1, "zipscript-c: Symlinking audio\n");
		if (!strncasecmp(g->l.link_target, "VA", 2) && (g->l.link_target[2] == '-' || g->l.link_target[2] == '_'))
			memcpy(g->v.audio.id3_artist, "VA", 3);

		if (!matchpath(group_dirs, g->l.path)) {

#if ( audio_genre_sort == TRUE )
			d_log(1, "zipscript-c:   Sorting mp3 by genre (%s)\n", g->v.audio.id3_genre);
			if (g->v.audio.id3_genre)
				createlink(audio_genre_path, g->v.audio.id3_genre, g->l.link_source, g->l.link_target);
#endif

#if ( audio_artist_sort == TRUE )
			d_log(1, "zipscript-c:   Sorting mp3 by artist\n");
			if (g->v.audio.id3_artist) {
				d_log(1, "zipscript-c:     - artist: %s\n", g->v.audio.id3_artist);
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
			d_log(1, "zipscript-c:   Sorting mp3 by year (%s)\n", g->v.audio.id3_year);
			if (g->v.audio.id3_year)
				createlink(audio_year_path, g->v.audio.id3_year, g->l.link_source, g->l.link_target);
#endif

#if ( audio_group_sort == TRUE )
			d_log(1, "zipscript-c:   Sorting mp3 by group\n");
			temp_p = remove_pattern(g->l.link_target, "*-", RP_LONG_LEFT);
			temp_p = remove_pattern(temp_p, "_", RP_SHORT_LEFT);
			n = strlen(temp_p);
			if (n > 0 && n < 15) {
				d_log(1, "zipscript-c:   - Valid groupname found: %s (%i)\n", temp_p, n);
				createlink(audio_group_path, temp_p, g->l.link_source, g->l.link_target);
			}
#endif

		}
		
#if ( create_m3u == TRUE )
		if (findfileext(dir, ".sfv")) {
			d_log(1, "zipscript-c: Creating m3u\n");
			cnt = sprintf(target, findfileext(dir, ".sfv"));
			strlcpy(target + cnt - 3, "m3u", 4);
			create_indexfile(g->l.race, &g->v, target);
		} else
			d_log(1, "zipscript-c: Cannot create m3u, sfv is missing\n");
#endif

	}
	
	if (_complete[0]) {
		d_log(1, "zipscript-c: Removing old complete bar, if any\n");
		removecomplete();
	}

	d_log(1, "zipscript-c: Removing incomplete indicator (%s)\n", g->l.incomplete);
	complete(g);

	if (msg->complete != NULL) {
		d_log(1, "zipscript-c: Writing COMPLETE and STATS to %s\n", log);
		writelog(g, convert(&g->v, g->ui, g->gi, msg->complete), _complete[1]);
	}
	
	if (_complete[0]) {
		d_log(1, "zipscript-c: Creating complete bar\n");
		createstatusbar(convert(&g->v, g->ui, g->gi, _complete[0]));
		
#if (chmod_completebar)
		if (!matchpath(group_dirs, g->l.path))
			chmod(convert(&g->v, g->ui, g->gi, _complete[0]), 0222);
		else
			d_log(1, "zipscript-c: we are in a group_dir - will not chmod the complete bar.\n");
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

		if ((g->l.nfo_incomplete) && (!findfileext(dir, ".nfo")) &&
			(matchpath(check_for_missing_nfo_dirs, g->l.path) || n)) {
			
			create_missing_missing_nfo_ind(g, inc_point, parent);
		}

	}

}

void
release_incomplete(GLOBAL *g, MSG *msg, RACETYPE *rtype)
{

	if (g->v.total.files_missing == g->v.total.files >> 1 && g->v.total.files >= min_halfway_files &&
	   ((g->v.total.size * g->v.total.files) >= (min_halfway_size * 1024 * 1024)) && msg->halfway != NULL) {
		d_log(1, "zipscript-c: Writing HALFWAY to %s\n", log);
		writelog(g, convert(&g->v, g->ui, g->gi, msg->halfway), (g->v.total.users > 1 ? rtype->race_halfway : rtype->norace_halfway));
	}
	
	d_log(1, "zipscript-c: Caching progress bar\n");
	buffer_progress_bar(&g->v);

	if (!matchpath(group_dirs, g->l.path) || create_incomplete_links_in_group_dirs) {
		d_log(1, "zipscript-c: Creating incomplete indicator:\n", g->l.incomplete);
		d_log(1, "zipscript-c:    name: '%s', incomplete: '%s', path: '%s'\n", g->v.misc.release_name, g->l.incomplete, g->l.path);
		create_incomplete2();
	}

	d_log(1, "zipscript-c: Creating/moving progress bar\n");
	move_progress_bar(0, &g->v, g->ui, g->gi);
	printf("%s", convert(&g->v, g->ui, g->gi, zipscript_footer_ok));

}

