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

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifdef _SunOS_
#include "scandir.h"
#include "strsep.h"
#endif

int 
main(int argc, char **argv)
{
	GLOBAL		g; /* this motherfucker owns */
	DIR		*dir, *parent;
	struct dirent	*dp;
	
	char           *fileext, *name_p, *temp_p = NULL, *temp_p_free = NULL;
	char           *target = 0;
	char	       *ext = 0;
	char           *complete_msg = 0;
	char           *update_msg = 0;
	char           *race_msg = 0;
	char           *sfv_msg = 0;
	char           *update_type = 0;
	char           *newleader_msg = 0;
	char           *halfway_msg = 0;
	char           *complete_bar = 0;
	char           *error_msg = 0;
	unsigned int	crc, gnum = 0, unum = 0, s_crc = 0;
	unsigned char	exit_value = EXIT_SUCCESS;
	unsigned char	no_check = FALSE;
	char	       *sfv_type = 0;
	char	       *race_type = 0;
	char	       *newleader_type = 0;
	char	       *race_halfway_type = 0;
	char	       *norace_halfway_type = 0;
	char	       *inc_point[2];
	char           *complete_announce = 0;
	int		cnt, cnt2, n = 0, m = 0;
	int		write_log = 0;
	long		loc;
#if ( enable_complete_script || enable_accept_script )
	int		nfofound = 0;
#endif
#if ( del_banned_release )
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
		d_log("zipscript-c: Wrong number of arguments used\n");
		printf(" - - PZS-NG ZipScript-C v%s - -\n\nUsage: %s <filename> <path> <crc>\n\n", ng_version(), argv[0]);
		exit(1);
	}
	d_log("zipscript-c: Clearing arrays\n");
	bzero(&g.v.total, sizeof(struct race_total));
	g.v.misc.slowest_user[0] = 30000;
	g.v.misc.fastest_user[0] = g.v.misc.release_type = RTYPE_NULL;

	/* gettimeofday(&g.v.transfer_stop, (struct timezone *)0 ); */

	strlcpy(g.v.file.name, argv[1], NAME_MAX);
	strlcpy(g.l.path, argv[2], PATH_MAX);
	strlcpy(g.v.misc.current_path, g.l.path, PATH_MAX);
	d_log("zipscript-c: Changing directory to %s\n", g.l.path);
	chdir(g.l.path);

	d_log("zipscript-c: Reading data from environment variables\n");
	if ((getenv("USER") == NULL) || (getenv("GROUP") == NULL) || (getenv("TAGLINE") == NULL) || (getenv("SPEED") ==NULL) || (getenv("SECTION") == NULL)) {
		d_log("zipscript-c: We are running from shell, falling back to default values for $USER, $GROUP, $TAGLINE, $SECTION and $SPEED\n");
		/*
		 * strcpy(g.v.user.name, "Unknown");
		 * strcpy(g.v.user.group, "NoGroup");
		 */

		gnum = buffer_groups(GROUPFILE, 0);
		unum = buffer_users(PASSWDFILE, 0);
		fileinfo.st_uid = geteuid();
		fileinfo.st_gid = getegid();
		strlcpy(g.v.user.name, get_u_name(fileinfo.st_uid), 24);
		strlcpy(g.v.user.group, get_g_name(fileinfo.st_gid), 24);
		memcpy(g.v.user.tagline, "No Tagline Set", 15);
		g.v.file.speed = 2005;
		g.v.section = 0;
		sprintf(g.v.sectionname, "DEFAULT");
	} else {
		gnum = buffer_groups(GROUPFILE, 0);
		unum = buffer_users(PASSWDFILE, 0);
		sprintf(g.v.user.name, getenv("USER"));
		sprintf(g.v.user.group, getenv("GROUP"));
		if (!(int)strlen(g.v.user.group))
			memcpy(g.v.user.group, "NoGroup", 8);
		sprintf(g.v.user.tagline, getenv("TAGLINE"));
		if (!(int)strlen(g.v.user.tagline))
			memcpy(g.v.user.tagline, "No Tagline Set", 15);
		g.v.file.speed = (unsigned int)strtol(getenv("SPEED"), NULL, 0);
		if (!g.v.file.speed)
			g.v.file.speed = 1;

#if (debug_announce == TRUE)
		printf("DEBUG: Speed: %dkb/s (%skb/s)\n",  g.v.file.speed, getenv("SPEED"));
#endif

		d_log("zipscript-c: Reading section from env (%s)\n", getenv("SECTION"));
		snprintf(g.v.sectionname, 127, getenv("SECTION"));
		g.v.section = 0;
		temp_p_free = temp_p = strdup((const char *)gl_sections);	/* temp_p_free is needed since temp_p is modified by strsep */
		if ((temp_p) == NULL) {
			d_log("zipscript-c: Can't allocate memory for sections\n");
		} else {
			n = 0;
			while (temp_p) {
				if (!strcmp(strsep(&temp_p, " "), getenv("SECTION"))) {
					g.v.section = (unsigned char)n;
					break;
				} else
					n++;
			}
			free(temp_p_free);
		}
	}
	g.v.file.speed *= 1024;

	d_log("zipscript-c: Checking the file size of %s\n", g.v.file.name);
	if (stat(g.v.file.name, &fileinfo)) {
		d_log("zipscript-c: Failed to stat file: %s\n", strerror(errno));
		g.v.file.size = 0;
		g.v.total.stop_time = 0;
	} else {
		g.v.file.size = fileinfo.st_size;
		d_log("zipscript-c: File size was: %d\n", g.v.file.size);
		g.v.total.stop_time = fileinfo.st_mtime;
	}

	d_log("zipscript-c: Setting race times\n");
	if (g.v.file.size != 0)
		g.v.total.start_time = g.v.total.stop_time - ((unsigned int)(g.v.file.size) / g.v.file.speed);
	else
		g.v.total.start_time = g.v.total.stop_time > (g.v.total.stop_time - 1) ? g.v.total.stop_time : (g.v.total.stop_time -1);
	if ((int)(g.v.total.stop_time - g.v.total.start_time) < 1)
		g.v.total.stop_time = g.v.total.start_time + 1;

	n = (g.l.length_path = (int)strlen(g.l.path)) + 1;

	d_log("zipscript-c: Allocating memory for variables\n");
	g.l.race = m_alloc(n += 10 + (g.l.length_zipdatadir = sizeof(storage) - 1));
	g.l.sfv = m_alloc(n);
	g.l.leader = m_alloc(n);
	//g.l.link_target = m_alloc(n + 256);
	target = m_alloc(n + 256);
	g.ui = malloc(sizeof(struct USERINFO *) * 30);
	memset(g.ui, 0, sizeof(struct USERINFO *) * 30);
	g.gi = malloc(sizeof(struct GROUPINFO *) * 30);
	memset(g.gi, 0, sizeof(struct GROUPINFO *) * 30);

	d_log("zipscript-c: Copying data g.l into memory\n");
	sprintf(g.l.sfv, storage "/%s/sfvdata", g.l.path);
	sprintf(g.l.leader, storage "/%s/leader", g.l.path);
	sprintf(g.l.race, storage "/%s/racedata", g.l.path);
	g.v.user.pos = 0;
	sprintf(g.v.misc.old_leader, "none");

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

#if ( sfv_cleanup == TRUE )
	d_log("zipscript-c: sfv_cleanup: ON\n");
#if ( sfv_cleanup_lowercase == TRUE )
	d_log("zipscript-c: Copying (lowercased version of) extension to memory\n");
#else
	d_log("zipscript-c: Copying (unchanged version of) extension to memory\n");
#endif
#else
	d_log("zipscript-c: sfv_cleanup: OFF\n");
	d_log("zipscript-c: Copying (unchanged version of) extension to memory\n");
#endif
	fileext = malloc(name_p - temp_p);
	memcpy(fileext, temp_p, name_p - temp_p);
#if ( sfv_cleanup == TRUE )
#if ( sfv_cleanup_lowercase == TRUE )
	strtolower(fileext);
#endif
#endif
	d_log("zipscript-c: Reading directory structure\n");
	dir = opendir(".");
	parent = opendir("..");

	d_log("zipscript-c: Caching release name\n");
	getrelname(&g);

	d_log("zipscript-c: Creating directory to store racedata in\n");
	maketempdir(g.l.path);

	d_log("zipscript-c: Locking release\n");
	while(1) {
		if ((m = create_lock(&g.v, g.l.path, PROGTYPE_ZIPSCRIPT, 3, 0))) {
			d_log("zipscript-c: Failed to lock release.\n");
			if (m == 1) {
				d_log("zipscript-c: version mismatch. Exiting.\n");
				printf("Error. You need to rm -fR ftp-data/pzs-ng/* before zipscript-c will work.\n");
				exit(EXIT_FAILURE);
			}
			if (m == PROGTYPE_RESCAN) {
				d_log("zipscript-c: Detected rescan running - will try to make it quit.\n");
				update_lock(&g.v, 0, 0);
			}
			for ( n = 0; n <= max_seconds_wait_for_lock * 10; n++) {
				d_log("zipscript-c: sleeping for .1 second before trying to get a lock.\n");
				usleep(100000);
				if (!(m = create_lock(&g.v, g.l.path, PROGTYPE_ZIPSCRIPT, 0, g.v.lock.data_queue)))
					break;
				
			}
			if (n >= max_seconds_wait_for_lock * 10) {
				if (m == PROGTYPE_RESCAN) {
					d_log("zipscript-c: Failed to get lock. Forcing unlock.\n");
					if (create_lock(&g.v, g.l.path, PROGTYPE_ZIPSCRIPT, 2, g.v.lock.data_queue)) {
						d_log("zipscript-c: Failed to force a lock. No choice but to exit.\n");
						exit(EXIT_FAILURE);
					}
				} else {
					d_log("zipscript-c: Failed to get a lock. No choice but to exit.\n");
					exit(EXIT_FAILURE);
				}
			}
			rewinddir(dir);
			rewinddir(parent);
		}
		usleep(10000);
		if (update_lock(&g.v, 1, 0) != -1)
			break;
	}

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
		write_log = g.v.misc.write_log;
		g.v.misc.write_log = 1;
		mark_as_bad(g.v.file.name);
		error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
		if (exit_value < 2)
			writelog(&g, error_msg, bad_file_0size_type);
		exit_value = 2;
	}
#endif
	/* No check directories */
	if (matchpath(nocheck_dirs, g.l.path) || (!matchpath(zip_dirs, g.l.path) && !matchpath(sfv_dirs, g.l.path) && !matchpath(group_dirs, g.l.path))) {
		d_log("zipscript-c: Directory matched with nocheck_dirs, or is not among sfv/zip/group dirs\n");
		d_log("zipscript-c:   - nocheck_dirs: '%s'\n", nocheck_dirs);
		d_log("zipscript-c:   - zip_dirs    : '%s'\n", zip_dirs);
		d_log("zipscript-c:   - sfv_dirs    : '%s'\n", sfv_dirs);
		d_log("zipscript-c:   - group_dirs  : '%s'\n", group_dirs);
		d_log("zipscript-c:   - current path: '%s'\n", g.l.path);
		no_check = TRUE;
	} else {
		/* Process file */
		d_log("zipscript-c: Verifying old racedata\n");
		if (!verify_racedata(g.l.race))
			d_log("zipscript-c:   Failed to open racedata - assuming this is a new race.\n");

		switch (get_filetype(&g, fileext)) {
		case 0:	/* ZIP CHECK */
			d_log("zipscript-c: File type is: ZIP\n");
			d_log("zipscript-c: Testing file integrity with %s\n", unzip_bin);
			if (!fileexists(unzip_bin)) {
				d_log("zipscript-c: ERROR! Not able to check zip-files - %s does not exists!\n", unzip_bin);
				sprintf(g.v.misc.error_msg, BAD_ZIP);
				mark_as_bad(g.v.file.name);
				write_log = g.v.misc.write_log;
				g.v.misc.write_log = 1;
				error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
				if (exit_value < 2)
					writelog(&g, error_msg, bad_file_zip_type);
				exit_value = 2;
				break;
			} else {
				sprintf(target, "%s -qqt \"%s\"", unzip_bin, g.v.file.name);
				if (execute(target) != 0) {
					d_log("zipscript-c: Integrity check failed (#%d): %s\n", errno, strerror(errno));
					sprintf(g.v.misc.error_msg, BAD_ZIP);
					mark_as_bad(g.v.file.name);
					write_log = g.v.misc.write_log;
					g.v.misc.write_log = 1;
					error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
					if (exit_value < 2)
						writelog(&g, error_msg, bad_file_zip_type);
					exit_value = 2;
					break;
				}
			}
			d_log("zipscript-c: Integrity ok\n");
			printf(zipscript_zip_ok);

			if ((matchpath(zip_dirs, g.l.path)) || (matchpath(group_dirs, g.l.path))  ) {
				g.v.misc.write_log = 1;
				d_log("zipscript-c: Directory matched with zip_dirs/group_dirs\n");
			} else {
				d_log("zipscript-c: WARNING! Directory did not match with zip_dirs/group_dirs\n");
				if (strict_path_match == TRUE) {
					d_log("zipscript-c: Strict mode on - exiting\n");
					sprintf(g.v.misc.error_msg, UNKNOWN_FILE, fileext);
					mark_as_bad(g.v.file.name);
					write_log = g.v.misc.write_log;
					g.v.misc.write_log = 1;
					error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
					if (exit_value < 2)
						writelog(&g, error_msg, bad_file_wrongdir_type);
					exit_value = 2;
					break;
				}
			}
			if (!fileexists("file_id.diz")) {
				d_log("zipscript-c: file_id.diz does not exist, trying to extract it from %s\n", g.v.file.name);
				sprintf(target, "%s -qqjnCLL \"%s\" file_id.diz", unzip_bin, g.v.file.name);
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
			g.v.total.files = read_diz("file_id.diz");
			d_log("zipscript-c:    Expecting %d files.\n", g.v.total.files);

			if (g.v.total.files == 0) {
				d_log("zipscript-c:    Could not get diskcount from diz.\n");
				g.v.total.files = 1;
				unlink("file_id.diz");
			}
			g.v.total.files_missing = g.v.total.files;

			d_log("zipscript-c: Storing new race data\n");
			writerace(g.l.race, &g.v, 0, F_CHECKED);
			d_log("zipscript-c: Reading race data from file to memory\n");
			readrace(g.l.race, &g.v, g.ui, g.gi);
			if (g.v.total.files_missing < 0) {
				d_log("zipscript-c: There seems to be more files in zip than we expected\n");
				g.v.total.files -= g.v.total.files_missing;
				g.v.total.files_missing = g.v.misc.write_log = 0;
			}
			d_log("zipscript-c: Setting message pointers\n");
			race_msg = zip_race;
			update_msg = zip_update;
			halfway_msg = CHOOSE(g.v.total.users, zip_halfway, zip_norace_halfway);
			newleader_msg = zip_newleader;

			break;
			/* END OF ZIP CHECK */

		case 1:	/* SFV CHECK */
			d_log("zipscript-c: File type is: SFV\n");
			if ((matchpath(sfv_dirs, g.l.path)) || (matchpath(group_dirs, g.l.path))  ) {
				d_log("zipscript-c: Directory matched with sfv_dirs/group_dirs\n");
			} else {
				d_log("zipscript-c: WARNING! Directory did not match with sfv_dirs/group_dirs\n");
				if (strict_path_match == TRUE) {
					d_log("zipscript-c: Strict mode on - exiting\n");
					sprintf(g.v.misc.error_msg, UNKNOWN_FILE, fileext);
					mark_as_bad(g.v.file.name);
					write_log = g.v.misc.write_log;
					g.v.misc.write_log = 1;
					error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
					if (exit_value < 2)
						writelog(&g, error_msg, bad_file_wrongdir_type);
					exit_value = 2;
					break;
				}
			}

			if (fileexists(g.l.sfv)) {
				if (deny_double_sfv == TRUE && findfileextcount(dir, ".sfv") > 1 && sfv_compare_size(".sfv", g.v.file.size) > 0) {
					write_log = g.v.misc.write_log;
					g.v.misc.write_log = 1;
					d_log("zipscript-c: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g.v.file.size));
					d_log("zipscript-c: No double sfv allowed\n");
					error_msg = convert(&g.v, g.ui, g.gi, deny_double_msg);
					writelog(&g, error_msg, general_doublesfv_type);
					sprintf(g.v.misc.error_msg, DOUBLE_SFV);
					mark_as_bad(g.v.file.name);
					exit_value = 2;
					g.v.misc.write_log = write_log;
					break;
				} else if (findfileextcount(dir, ".sfv") > 1 && sfv_compare_size(".sfv", g.v.file.size) > 0) {
					d_log("zipscript-c: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g.v.file.size));
					d_log("zipscript-c: Reading remainders of old sfv\n");
					readsfv(g.l.sfv, &g.v, 1);
					cnt = g.v.total.files - g.v.total.files_missing;
					cnt2 = g.v.total.files;
					g.v.total.files_missing = g.v.total.files = 0;
					//readsfv_ffile(g.v.file.name, g.v.file.size, g.v);
					readsfv_ffile(&g.v);
					if ((g.v.total.files <= cnt2) || (g.v.total.files != (cnt + g.v.total.files_missing))) {
						write_log = g.v.misc.write_log;
						g.v.misc.write_log = 1;
						d_log("zipscript-c: Old sfv seems to match with more files than current one\n");
						strlcpy(g.v.misc.error_msg, "SFV does not match with files!", 80);
						error_msg = convert(&g.v, g.ui, g.gi, deny_double_msg);
						writelog(&g, error_msg, general_doublesfv_type);
						sprintf(g.v.misc.error_msg, DOUBLE_SFV);
						mark_as_bad(g.v.file.name);
						exit_value = 2;
						g.v.misc.write_log = write_log;
						break;
					}
					g.v.total.files = g.v.total.files_missing = 0;
				} else {
					d_log("zipscript-c: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g.v.file.size));
					d_log("zipscript-c: Hmm.. Seems the old .sfv was deleted. Allowing new one.\n");
					unlink(g.l.race);
					unlink(g.l.sfv);
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
			if (copysfv(g.v.file.name, g.l.sfv, &g.v)) {
				d_log("zipscript-c: Found invalid entries in SFV.\n");
				write_log = g.v.misc.write_log;
				g.v.misc.write_log = 1;
				error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
				if (exit_value < 2)
					writelog(&g, error_msg, bad_file_sfv_type);
				mark_as_bad(g.v.file.name);
				exit_value = 2;
				sprintf(g.v.misc.error_msg, EMPTY_SFV);
				unlink(g.l.race);
				unlink(g.l.sfv);

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

				break;
			}

#if (use_partial_on_noforce == TRUE)
			if ( (force_sfv_first == FALSE) || matchpartialpath(noforce_sfv_first_dirs, g.l.path))
#else
			if ( (force_sfv_first == FALSE) || matchpath(noforce_sfv_first_dirs, g.l.path))
#endif
			{
				if (fileexists(g.l.race)) {
					d_log("zipscript-c: Testing files marked as untested\n");
					testfiles(&g.l, &g.v, 0);
				}
			}
			d_log("zipscript-c: Reading file count from SFV\n");
			readsfv(g.l.sfv, &g.v, 0);

			if (g.v.total.files == 0) {
				d_log("zipscript-c: SFV seems to have no files of accepted types, or has errors.\n");
				sprintf(g.v.misc.error_msg, EMPTY_SFV);
				mark_as_bad(g.v.file.name);
				write_log = g.v.misc.write_log;
				g.v.misc.write_log = 1;
				error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
				if (exit_value < 2)
					writelog(&g, error_msg, bad_file_sfv_type);
				exit_value = 2;
				break;
			}
			printf(zipscript_sfv_ok);
			if (fileexists(g.l.race)) {
				d_log("zipscript-c: Reading race data from file to memory\n");
				readrace(g.l.race, &g.v, g.ui, g.gi);
			}
			if (del_completebar) {
				d_log("zipscript-c: Making sure that release is not marked as complete\n");
				removecomplete();
			}
			d_log("zipscript-c: Setting message pointers\n");
			sfv_type = general_announce_sfv_type;
			switch (g.v.misc.release_type) {
			case RTYPE_RAR:
				sfv_msg = rar_sfv;
				sfv_type = rar_announce_sfv_type;
				break;
			case RTYPE_OTHER:
				sfv_msg = other_sfv;
				sfv_type = other_announce_sfv_type;
				break;
			case RTYPE_AUDIO:
				sfv_msg = audio_sfv;
				sfv_type = audio_announce_sfv_type;
				break;
			case RTYPE_VIDEO:
				sfv_msg = video_sfv;
				sfv_type = video_announce_sfv_type;
				break;
			default :
				sfv_msg = rar_sfv;
				sfv_type = rar_announce_sfv_type;
				d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (1:%d)\n", g.v.misc.release_type);
				break;
			}

			if (!sfv_msg)
				d_log("zipscript-c.c: Something's messed up - sfv_msg not set!\n");

			halfway_msg = newleader_msg = race_msg = update_msg = NULL;

			g.v.misc.write_log = matchpath(sfv_dirs, g.l.path);
			if (g.v.total.files_missing > 0) {
				if (sfv_msg != NULL) {
					d_log("zipscript-c: Writing SFV message to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, sfv_msg), sfv_type);
				}
			} else {
				if (g.v.misc.release_type == RTYPE_AUDIO) {
					d_log("zipscript-c: Reading audio info for completebar\n");
					get_mpeg_audio_info(findfileext(dir, ".mp3"), &g.v.audio);
				}
			}
			break;
			/* END OF SFV CHECK */

		case 2:	/* NFO CHECK */
			no_check = TRUE;
			d_log("zipscript-c: File type is: NFO\n");
#if ( deny_double_nfo )
			if (findfileextcount(dir, ".nfo") > 1) {
				d_log("zipscript-c: Looks like there already is a nfo uploaded. Denying this one.\n");
				sprintf(g.v.misc.error_msg, DUPE_NFO);
				//mark_as_bad(g.v.file.name);
				write_log = g.v.misc.write_log;
				g.v.misc.write_log = 1;
				error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
				if (exit_value < 2)
					writelog(&g, error_msg, bad_file_nfo_type);
				exit_value = 2;
				break;
			}
#endif
			//writerace(g.l.race, &g.v, 0, F_NFO);

#if ( enable_nfo_script )
			if (!fileexists(nfo_script)) {
				d_log("zipscript-c: Warning - nfo_script (%s) - file does not exists\n", nfo_script);
			}
			d_log("zipscript-c: Executing nfo script (%s)\n", nfo_script);
			sprintf(target, nfo_script " \"%s\"", g.v.file.name);
			if (execute(target) != 0)
				d_log("zipscript-c: Failed to execute nfo_script: %s\n", strerror(errno));
#endif

			break;
			/* END OF NFO CHECK */

		case 3:	/* SFV BASED CRC-32 CHECK */
			d_log("zipscript-c: File type is: ANY\n");

			d_log("zipscript-c: Converting crc (%s) from string to integer\n", argv[3]);
			crc = hexstrtodec(argv[3]);
			if (crc == 0) {
				d_log("zipscript-c: We did not get crc from ftp daemon, calculating crc for %s now.\n", g.v.file.name);
				crc = calc_crc32(g.v.file.name);
			}
			if (fileexists(g.l.sfv)) {
				s_crc = readsfv(g.l.sfv, &g.v, 0);
				
#if (sfv_calc_single_fname == TRUE)
				if (s_crc == 0 && errno == 1) {
					d_log("zipscript-c: CRC in SFV is 0 - trying to calculate it from the file\n");
					s_crc = calc_crc32(g.v.file.name);
					update_sfvdata(g.v.file.name, s_crc);
				}
#endif
				d_log("zipscript-c: DEBUG: crc: %X - s_crc: %X\n", crc, s_crc);
				if (s_crc != crc) {
					if (s_crc == 0) {
						if (!strcomp(allowed_types, fileext)) {
#if (allow_files_not_in_sfv == TRUE)
							d_log("zipscript-c: Filename was not found in the SFV, but allowing anyway\n");
							no_check = TRUE;
							break;
#endif
							d_log("zipscript-c: Filename was not found in the SFV\n");
							strlcpy(g.v.misc.error_msg, NOT_IN_SFV, 80);
						} else {
							d_log("zipscript-c: filetype is part of allowed_types.\n");
							no_check = TRUE;
							break;
						}
					} else {
						d_log("zipscript-c: CRC-32 check failed\n");
						if (!hexstrtodec(argv[3]) && allow_file_resume) {
							d_log("zipscript-c: Broken xfer detected - allowing file.\n");
							no_check = TRUE;
							write_log = g.v.misc.write_log;
							g.v.misc.write_log = 1;
							error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
							writelog(&g, error_msg, bad_file_crc_type);
							if (enable_unduper_script == TRUE) {
								if (!fileexists(unduper_script)) {
									d_log("zipscript-c: Warning - undupe script (%s) does not exist.\n", unduper_script);
								}
								sprintf(target, unduper_script " \"%s\"", g.v.file.name);
								if (execute(target) == 0)
									d_log("zipscript-c: undupe of %s successful.\n", g.v.file.name);
								else
									d_log("zipscript-c: undupe of %s failed.\n", g.v.file.name);
							}
							break;
						}
						strlcpy(g.v.misc.error_msg, BAD_CRC, 80);
					}
					mark_as_bad(g.v.file.name);
					write_log = g.v.misc.write_log;
					g.v.misc.write_log = 1;
					error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
					if (exit_value < 2)
						writelog(&g, error_msg, bad_file_crc_type);
					exit_value = 2;
					break;
				}
#if (sfv_cleanup == TRUE && sfv_cleanup_lowercase == TRUE)
				if (check_dupefile(dir, g.v.file.name)) {
					d_log("zipscript-c: dupe detected - same file, different case already exists.\n");
					strlcpy(g.v.misc.error_msg, DOUBLE_SFV, 80);
					mark_as_bad(g.v.file.name);
					write_log = g.v.misc.write_log;
					g.v.misc.write_log = 1;
					error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
					if (exit_value < 2)
						writelog(&g, error_msg, bad_file_crc_type);
					exit_value = 2;
					break;
				}
#endif
				printf(zipscript_SFV_ok);
				d_log("zipscript-c: Storing new race data\n");
				writerace(g.l.race, &g.v, crc, F_CHECKED);
			} else {
#if ( force_sfv_first == TRUE )
#if (use_partial_on_noforce == TRUE)
				if (!matchpartialpath(noforce_sfv_first_dirs, g.l.path) && !matchpath(zip_dirs, g.l.path)) {
#else
				if (!matchpath(noforce_sfv_first_dirs, g.l.path) && !matchpath(zip_dirs, g.l.path)) {
#endif
					d_log("zipscript-c: SFV needs to be uploaded first\n");
					strlcpy(g.v.misc.error_msg, SFV_FIRST, 80);
					mark_as_bad(g.v.file.name);
					write_log = g.v.misc.write_log;
					g.v.misc.write_log = 1;
					error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
					if (exit_value < 2)
						writelog(&g, error_msg, bad_file_nosfv_type);
					exit_value = 2;
					break;
				} else if (matchpath(zip_dirs, g.l.path) && (!fileexists(g.l.sfv))) {
					d_log("zipscript-c: This looks like a file uploaded the wrong place - Not allowing it.\n");
					strlcpy(g.v.misc.error_msg, SFV_FIRST, 80);
					mark_as_bad(g.v.file.name);
					write_log = g.v.misc.write_log;
					g.v.misc.write_log = 1;
					error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
					if (exit_value < 2)
						writelog(&g, error_msg, bad_file_nosfv_type);
					exit_value = 2;
					break;
				} else {
					d_log("zipscript-c: path matched with noforce_sfv_first or zip_dirs - allowing file.\n");
					printf(zipscript_SFV_skip);
					d_log("zipscript-c: Storing new race data\n");
					writerace(g.l.race, &g.v, crc, F_NOTCHECKED);
				}
#else
				d_log("zipscript-c: Could not check file yet - SFV is not present\n");
				printf(zipscript_SFV_skip);
				d_log("zipscript-c: Storing new race data\n");
				writerace(g.l.race, &g.v, crc, F_NOTCHECKED);
#endif
			}

			g.v.misc.write_log = matchpath(sfv_dirs, g.l.path);

			d_log("zipscript-c: Reading race data from file to memory\n");
			readrace(g.l.race, &g.v, g.ui, g.gi);

			d_log("zipscript-c: Setting pointers\n");
			if (g.v.misc.release_type == RTYPE_NULL) {
				if (israr(fileext))
					g.v.misc.release_type = RTYPE_RAR;	/* .RAR / .R?? */
				else if (isvideo(fileext))
					g.v.misc.release_type = RTYPE_VIDEO;	/* AVI/MPEG */
				else if (!memcmp(fileext, "mp3", 4))
					g.v.misc.release_type = RTYPE_AUDIO;	/* MP3 */
				else
					g.v.misc.release_type = RTYPE_OTHER;	/* OTHER FILE */
			}
			switch (g.v.misc.release_type) {
			case RTYPE_RAR:
				get_rar_info(&g.v);
				race_msg = rar_race;
				update_msg = rar_update;
				halfway_msg = CHOOSE(g.v.total.users, rar_halfway, rar_norace_halfway);
				newleader_msg = rar_newleader;
				break;
			case RTYPE_OTHER:
				race_msg = other_race;
				update_msg = other_update;
				halfway_msg = CHOOSE(g.v.total.users, other_halfway, other_norace_halfway);
				newleader_msg = other_newleader;
				break;
			case RTYPE_AUDIO:
				d_log("zipscript-c: Trying to read audio header and tags\n");
				get_mpeg_audio_info(g.v.file.name, &g.v.audio);
#if ( exclude_non_sfv_dirs )
				if (g.v.misc.write_log == TRUE) {
#endif
					if ((enable_mp3_script == TRUE) && (g.ui[g.v.user.pos]->files == 1)) {
						if (!fileexists(mp3_script)) {
							d_log("zipscript-c: Warning -  mp3_script (%s) - file does not exists\n", mp3_script);
						}
						d_log("zipscript-c: Executing mp3 script (%s %s)\n", mp3_script, convert(&g.v, g.ui, g.gi, mp3_script_cookies));
						sprintf(target, "%s %s", mp3_script, convert(&g.v, g.ui, g.gi, mp3_script_cookies));
						if (execute(target) != 0)
							d_log("zipscript-c: Failed to execute mp3_script: %s\n", strerror(errno));
					}
					if (!matchpath(audio_nocheck_dirs, g.l.path)) {
#if ( audio_banned_genre_check )
						if (strcomp(banned_genres, g.v.audio.id3_genre)) {
							d_log("zipscript-c: File is from banned genre\n");
							sprintf(g.v.misc.error_msg, BANNED_GENRE, g.v.audio.id3_genre);
							if (audio_genre_warn == TRUE) {
								if (g.ui[g.v.user.pos]->files == 1) {
									d_log("zipscript-c: warn on - logging to logfile\n");
									write_log = g.v.misc.write_log;
									g.v.misc.write_log = 1;
									error_msg = convert(&g.v, g.ui, g.gi, audio_genre_warn_msg);
									writelog(&g, error_msg, general_badgenre_type);
									g.v.misc.write_log = write_log;
								} else
									d_log("zipscript-c: warn on - have already logged to logfile\n");
							} else {
								mark_as_bad(g.v.file.name);
								write_log = g.v.misc.write_log;
								g.v.misc.write_log = 1;
								error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
								if ((g.ui[g.v.user.pos]->files == 1) && (exit_value < 2))
									writelog(&g, error_msg, bad_file_genre_type);
								exit_value = 2;
							}
#if ( del_banned_release )
							deldir = 1;
							exit_value = 2;
#endif
							break;
						}
#elif ( audio_allowed_genre_check == TRUE )
						if (!strcomp(allowed_genres, g.v.audio.id3_genre)) {
							d_log("zipscript-c: File is not in allowed genre\n");
							sprintf(g.v.misc.error_msg, BANNED_GENRE, g.v.audio.id3_genre);
							if (audio_genre_warn == TRUE) {
								if (g.ui[g.v.user.pos]->files == 1) {
									d_log("zipscript-c: warn on - logging to logfile\n");
									write_log = g.v.misc.write_log;
									g.v.misc.write_log = 1;
									error_msg = convert(&g.v, g.ui, g.gi, audio_genre_warn_msg);
									writelog(&g, error_msg, general_badgenre_type);
									g.v.misc.write_log = write_log;
								} else
									d_log("zipscript-c: warn on - have already logged to logfile\n");
							} else {
								mark_as_bad(g.v.file.name);
								write_log = g.v.misc.write_log;
								g.v.misc.write_log = 1;
								error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
								if ((g.ui[g.v.user.pos]->files == 1) && (exit_value < 2))
									writelog(&g, error_msg, bad_file_genre_type);
								exit_value = 2;
							}
#if ( del_banned_release )
							deldir = 1;
							exit_value = 2;
#endif
							break;
						}
#endif
#if ( audio_year_check == TRUE )
						if (!strcomp(allowed_years, g.v.audio.id3_year)) {
							d_log("zipscript-c: File is from banned year\n");
							sprintf(g.v.misc.error_msg, BANNED_YEAR, g.v.audio.id3_year);
							if (audio_year_warn == TRUE) {
								if (g.ui[g.v.user.pos]->files == 1) {
									d_log("zipscript-c: warn on - logging to logfile\n");
									write_log = g.v.misc.write_log;
									g.v.misc.write_log = 1;
									error_msg = convert(&g.v, g.ui, g.gi, audio_year_warn_msg);
									writelog(&g, error_msg, general_badyear_type);
									g.v.misc.write_log = write_log;
								} else
									d_log("zipscript-c: warn on - have already logged to logfile\n");
							} else {
								mark_as_bad(g.v.file.name);
								write_log = g.v.misc.write_log;
								g.v.misc.write_log = 1;
								error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
								if ((g.ui[g.v.user.pos]->files == 1) && (exit_value < 2))
									writelog(&g, error_msg, bad_file_year_type);
								exit_value = 2;
							}
#if ( del_banned_release )
							deldir = 1;
							exit_value = 2;
#endif
							break;
						}
#endif
#if ( audio_cbr_check == TRUE )
						if (g.v.audio.is_vbr == 0) {
							if (!strcomp(allowed_constant_bitrates, g.v.audio.bitrate)) {
								d_log("zipscript-c: File is encoded using banned bitrate\n");
								sprintf(g.v.misc.error_msg, BANNED_BITRATE, g.v.audio.bitrate);
								if (audio_cbr_warn == TRUE) {
									if (g.ui[g.v.user.pos]->files == 1) {
										d_log("zipscript-c: warn on - logging to logfile\n");
										write_log = g.v.misc.write_log;
										g.v.misc.write_log = 1;
										error_msg = convert(&g.v, g.ui, g.gi, audio_cbr_warn_msg);
										writelog(&g, error_msg, general_badbitrate_type);
										g.v.misc.write_log = write_log;
									} else
										d_log("zipscript-c: warn on - have already logged to logfile\n");
								} else {
									mark_as_bad(g.v.file.name);
									write_log = g.v.misc.write_log;
									g.v.misc.write_log = 1;
									error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
									if ((g.ui[g.v.user.pos]->files == 1) && (exit_value < 2))
										writelog(&g, error_msg, bad_file_bitrate_type);
									exit_value = 2;
								}
#if ( del_banned_release )
								deldir = 1;
								exit_value = 2;
#endif
								break;
							}
						}
#endif
					} else
						d_log("zipscript-c: user is in a no audio check dir - skipping checks.\n");
#if ( exclude_non_sfv_dirs == TRUE )
				}
#endif
				if (realtime_mp3_info != DISABLED) {
					d_log("zipscript-c: Printing realtime_mp3_info.\n");
					printf("%s", convert(&g.v, g.ui, g.gi, realtime_mp3_info));
				}
				race_msg = audio_race;
				update_msg = audio_update;
				halfway_msg = CHOOSE(g.v.total.users, audio_halfway, audio_norace_halfway);
				newleader_msg = audio_newleader;
				break;
			case RTYPE_VIDEO:
				d_log("zipscript-c: Trying to read video header\n");
				if (!memcmp(fileext, "avi", 3))
					avi_video(g.v.file.name, &g.v.video);
				else
					mpeg_video(g.v.file.name, &g.v.video);
				race_msg = video_race;
				update_msg = video_update;
				halfway_msg = CHOOSE(g.v.total.users, video_halfway, video_norace_halfway);
				newleader_msg = video_newleader;
				break;
			default:
				get_rar_info(&g.v);
				race_msg = rar_race;
				update_msg = rar_update;
				halfway_msg = CHOOSE(g.v.total.users, rar_halfway, rar_norace_halfway);
				newleader_msg = rar_newleader;
				d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (2:%d)\n", g.v.misc.release_type);
				break;
			}

			if (!race_msg)
				d_log("zipscript-c.c: Something's messed up - race_msg not set!\n");

			if (exit_value == EXIT_SUCCESS) {
				d_log("zipscript-c: Removing missing indicator\n");
				unlink_missing(g.v.file.name);

				/*
				sprintf(target, "%s-missing", g.v.file.name);
#if ( sfv_cleanup == TRUE )
#if (sfv_cleanup_lowercase == TRUE)
				  strtolower(target);
#endif
#endif
				if (target)
					unlink(target);
				*/
			}
			break;
			/* END OF SFV BASED CRC-32 CHECK */

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
			write_log = g.v.misc.write_log;
			g.v.misc.write_log = 1;
			error_msg = convert(&g.v, g.ui, g.gi, bad_file_msg);
			if (exit_value < 2)
				writelog(&g, error_msg, bad_file_disallowed_type);
			exit_value = 2;
			break;
			/* END OF UNKNOWN CHECK */
		}
	}

	if (no_check == TRUE) {	/* File was not checked */
		printf(zipscript_any_ok);
		printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_skip));
	} else if (exit_value == EXIT_SUCCESS) {	/* File was checked */

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

			/*
			 * Modification by <daxxar@daxxar.com> Only write
			 * new leader if he leads with newleader_files_ahead
			 * or only one person is racing if enable_files_ahead
			 * :)
			 */
			if (!enable_files_ahead || ((g.v.total.users > 1 && g.ui[g.ui[0]->pos]->files >= (g.ui[g.ui[1]->pos]->files + newleader_files_ahead)) || g.v.total.users == 1)) {
				d_log("zipscript-c: Writing current leader to file\n");
				read_write_leader(g.l.leader, &g.v, g.ui[g.ui[0]->pos]);
			}
			if (g.v.total.users > 1) {
				if (g.ui[g.v.user.pos]->files == 1 && race_msg != NULL) {
					d_log("zipscript-c: Writing RACE to %s\n", log);
					race_type = general_announce_race_type;
					switch (g.v.misc.release_type) {
					case RTYPE_RAR:
						race_type = rar_announce_race_type;
						break;	/* rar */
					case RTYPE_OTHER:
						race_type = other_announce_race_type;
						break;	/* other */
					case RTYPE_AUDIO:
						race_type = audio_announce_race_type;
						break;	/* audio */
					case RTYPE_VIDEO:
						race_type = video_announce_race_type;
						break;	/* video */
					case RTYPE_NULL:
						race_type = zip_announce_race_type;
						break;	/* zip */
					default:
						race_type = rar_announce_race_type;
						d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (3:%d)\n", g.v.misc.release_type);
						break;	/* rar */
					}

					if (!race_type)
						d_log("zipscript-c.c: Something's messed up - race_type not set!\n");

					writelog(&g, convert(&g.v, g.ui, g.gi, race_msg), race_type);
				}

				/*
				 * Modification by <daxxar@daxxar.com>
				 * Only announce new leader if he leads with
				 * newleader_files_ahead files :-)
				 */
				if (g.v.total.files >= min_newleader_files && ((g.v.total.size * g.v.total.files) >= (min_newleader_size * 1024 * 1024)) && strcmp(g.v.misc.old_leader, g.ui[g.ui[0]->pos]->name) && newleader_msg != NULL && g.ui[g.ui[0]->pos]->files >= (g.ui[g.ui[1]->pos]->files + newleader_files_ahead) && g.v.total.files_missing) {
					d_log("zipscript-c: Writing NEWLEADER to %s\n", log);
					newleader_type = general_announce_newleader_type;
					switch (g.v.misc.release_type) {
					case RTYPE_RAR:
						newleader_type = rar_announce_newleader_type;
						break;	/* rar */
					case RTYPE_OTHER:
						newleader_type = other_announce_newleader_type;
						break;	/* other */
					case RTYPE_AUDIO:
						newleader_type = audio_announce_newleader_type;
						break;	/* audio */
					case RTYPE_VIDEO:
						newleader_type = video_announce_newleader_type;
						break;	/* video */
					case RTYPE_NULL:
						newleader_type = zip_announce_newleader_type;
						break;	/* zip */
					default:
						newleader_type = rar_announce_newleader_type;
						d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (4:%d)\n", g.v.misc.release_type);
						break;	/* rar */
					}

					if (!newleader_type)
						d_log("zipscript-c.c: Something's messed up - newleader_type not set!\n");

					writelog(&g, convert(&g.v, g.ui, g.gi, newleader_msg), newleader_type);
				}
			} else {

				if (g.ui[g.v.user.pos]->files == 1 && g.v.total.files >= min_update_files && ((g.v.total.size * g.v.total.files) >= (min_update_size * 1024 * 1024)) && update_msg != NULL) {
					d_log("zipscript-c: Writing UPDATE to %s\n", log);
					update_type = general_announce_update_type;
					switch (g.v.misc.release_type) {
					case RTYPE_RAR:
						update_type = rar_announce_update_type;
						break;	/* rar */
					case RTYPE_OTHER:
						update_type = other_announce_update_type;
						break;	/* other */
					case RTYPE_AUDIO:
						if (g.v.audio.is_vbr == 0) {
							update_type = audio_announce_cbr_update_type;
						} else {
							update_type = audio_announce_vbr_update_type;
						};
						break;	/* audio */
					case RTYPE_VIDEO:
						update_type = video_announce_update_type;
						break;	/* video */
					case RTYPE_NULL:
						update_type = zip_announce_update_type;
						break;	/* zip */
					default:
						update_type = rar_announce_update_type;
						d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (5:%d)\n", g.v.misc.release_type);
						break;	/* rar */
					}
					if (!update_type)
						d_log("zipscript-c.c: Something's messed up - update_type not set!\n");

					writelog(&g, convert(&g.v, g.ui, g.gi, update_msg), update_type);
				}
			}
		}
		if (g.v.total.files_missing > 0) {

			/* Release is incomplete */

			if (g.v.total.files_missing == g.v.total.files >> 1 && g.v.total.files >= min_halfway_files && ((g.v.total.size * g.v.total.files) >= (min_halfway_size * 1024 * 1024)) && halfway_msg != NULL) {
				d_log("zipscript-c: Writing HALFWAY to %s\n", log);
				norace_halfway_type = general_announce_norace_halfway_type;
				race_halfway_type = general_announce_race_halfway_type;
				switch (g.v.misc.release_type) {
				case RTYPE_RAR:
					norace_halfway_type = rar_announce_norace_halfway_type;
					race_halfway_type = rar_announce_race_halfway_type;
					break;	/* rar */
				case RTYPE_OTHER:
					norace_halfway_type = other_announce_norace_halfway_type;
					race_halfway_type = other_announce_race_halfway_type;
					break;	/* other */
				case RTYPE_AUDIO:
					norace_halfway_type = audio_announce_norace_halfway_type;
					race_halfway_type = audio_announce_race_halfway_type;
					break;	/* audio */
				case RTYPE_VIDEO:
					norace_halfway_type = video_announce_norace_halfway_type;
					race_halfway_type = video_announce_race_halfway_type;
					break;	/* video */
				case RTYPE_NULL:
					norace_halfway_type = zip_announce_norace_halfway_type;
					race_halfway_type = zip_announce_race_halfway_type;
					break;	/* zip */
				default:
					norace_halfway_type = rar_announce_norace_halfway_type;
					race_halfway_type = rar_announce_race_halfway_type;
					d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (6:%d)\n", g.v.misc.release_type);
					break;	/* rar */
				}

				if (!race_halfway_type)
					d_log("zipscript-c.c: Something's messed up - race_halfway_type not set!\n");

				writelog(&g, convert(&g.v, g.ui, g.gi, halfway_msg), (g.v.total.users > 1 ? race_halfway_type : norace_halfway_type));
			}
			/*
			 * It is _very_ unlikely that halfway would be
			 * announced on complete release ;)
			 */

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
			switch (g.v.misc.release_type) {
			case RTYPE_NULL:
				complete_bar = zip_completebar;
				complete_msg = CHOOSE(g.v.total.users, zip_complete, zip_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, zip_announce_race_complete_type, zip_announce_norace_complete_type);
				break;
			case RTYPE_RAR:
				complete_bar = rar_completebar;
				complete_msg = CHOOSE(g.v.total.users, rar_complete, rar_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
				break;
			case RTYPE_OTHER:
				complete_bar = other_completebar;
				complete_msg = CHOOSE(g.v.total.users, other_complete, other_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, other_announce_race_complete_type, other_announce_norace_complete_type);
				break;
			case RTYPE_AUDIO:
				complete_bar = audio_completebar;
				complete_msg = CHOOSE(g.v.total.users, audio_complete, audio_norace_complete);
				if (g.v.audio.is_vbr == 0) {
					complete_announce = CHOOSE(g.v.total.users, audio_cbr_announce_race_complete_type, audio_cbr_announce_norace_complete_type);
				} else {
					complete_announce = CHOOSE(g.v.total.users, audio_vbr_announce_race_complete_type, audio_vbr_announce_norace_complete_type);
				}

				d_log("zipscript-c: Symlinking audio\n");
				if (!strncasecmp(g.l.link_target, "VA", 2) && (g.l.link_target[2] == '-' || g.l.link_target[2] == '_'))
					memcpy(g.v.audio.id3_artist, "VA", 3);

				if (g.v.misc.write_log == TRUE && !matchpath(group_dirs, g.l.path)) {
#if ( audio_genre_sort == TRUE )
					d_log("zipscript-c:   Sorting mp3 by genre (%s)\n", g.v.audio.id3_genre);
					createlink(audio_genre_path, g.v.audio.id3_genre, g.l.link_source, g.l.link_target);
#endif
#if ( audio_artist_sort == TRUE )
					d_log("zipscript-c:   Sorting mp3 by artist\n");
					if (*g.v.audio.id3_artist) {
						d_log("zipscript-c:     - artist: %s\n", g.v.audio.id3_artist);
						if (memcmp(g.v.audio.id3_artist, "VA", 3)) {
							temp_p = malloc(2);
							snprintf(temp_p, 2, "%c", toupper(*g.v.audio.id3_artist));
							createlink(audio_artist_path, temp_p, g.l.link_source, g.l.link_target);
							free(temp_p);
						} else {
							createlink(audio_artist_path, "VA", g.l.link_source, g.l.link_target);
						}
					}
#endif
#if ( audio_year_sort == TRUE )
					d_log("zipscript-c:   Sorting mp3 by year (%s)\n", g.v.audio.id3_year);
					if (*g.v.audio.id3_year != 0) {
						createlink(audio_year_path, g.v.audio.id3_year, g.l.link_source, g.l.link_target);
					}
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
				break;
			case RTYPE_VIDEO:
				complete_bar = video_completebar;
				complete_msg = CHOOSE(g.v.total.users, video_complete, video_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, video_announce_race_complete_type, video_announce_norace_complete_type);
				break;
			default:
				complete_bar = rar_completebar;
				complete_msg = CHOOSE(g.v.total.users, rar_complete, rar_norace_complete);
				complete_announce = CHOOSE(g.v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
				d_log("zipscript-c: WARNING! Not a known release type - Contact the authors! (7:%d)\n", g.v.misc.release_type);
				break;
			}

			if (complete_bar) {
				d_log("zipscript-c: Removing old complete bar, if any\n");
				removecomplete();
			}

			d_log("zipscript-c: Removing incomplete indicator (%s)\n", g.l.incomplete);
			complete(&g);

			if (complete_msg != NULL) {
				d_log("zipscript-c: Writing COMPLETE and STATS to %s\n", log);
				writelog(&g, convert(&g.v, g.ui, g.gi, complete_msg), complete_announce);
			}
			if (complete_bar) {
				d_log("zipscript-c: Creating complete bar\n");
				createstatusbar(convert(&g.v, g.ui, g.gi, complete_bar));
#if (chmod_completebar)
				if (!matchpath(group_dirs, g.l.path)) {
					chmod(convert(&g.v, g.ui, g.gi, complete_bar), 0222);
				} else {
					d_log("zipscript-c: we are in a group_dir - will not chmod the complete bar.\n");
				}
#endif
			}

#if ( enable_complete_script == TRUE )
			nfofound = (int)findfileext(dir, ".nfo");
			if (!fileexists(complete_script)) {
				d_log("zipscript-c: Warning - complete_script (%s) - file does not exists\n", complete_script);
			}
			d_log("zipscript-c: Executing complete script\n");
			sprintf(target, complete_script " \"%s\"", g.v.file.name);
			if (execute(target) != 0)
				d_log("zipscript-c: Failed to execute complete_script: %s\n", strerror(errno));

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
					} else {
						if (!findfileextparent(parent, ".nfo")) {
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
		remove_from_race(g.l.race, g.v.file.name);
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

#if ( del_banned_release )
	if (deldir) {
		d_log("zipscript-c: del_banned_release is set - removing entire dir.\n");
		move_progress_bar(1, &g.v, g.ui, g.gi);
		if (g.l.incomplete)
			unlink(g.l.incomplete);
		rewinddir(dir);
		del_releasedir(dir, g.l.path);
	}
#endif

	d_log("zipscript-c: Releasing memory and removing lock\n");
	closedir(dir);
	closedir(parent);
	remove_lock(&g.v);
	
	buffer_groups(GROUPFILE, gnum);
	buffer_users(PASSWDFILE, unum);
	updatestats_free(&g);
	free(fileext);
	m_free(target);
	m_free(g.l.race);
	m_free(g.l.sfv);
	m_free(g.l.leader);

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
		d_log("zipscript-c: del_banned_release is set - removing entire dir.\n");
}
