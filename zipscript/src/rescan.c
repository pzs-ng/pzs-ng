#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "zsfunctions.h"
#include "helpfunctions.h"
#include "race-file.h"
#include "objects.h"
#include "macros.h"
#include "multimedia.h"
#include "convert.h"
#include "dizreader.h"
#include "stats.h"
#include "complete.h"
#include "crc.h"
#include "ng-version.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifndef HAVE_STRLCPY
# include "strl/strl.h"
#endif

int 
main(int argc, char *argv[])
{
	int		k, n, m, l;
	char           *ext, exec[4096], *complete_bar = 0, *inc_point[2];
	unsigned int	crc = 0;
	struct stat	fileinfo;

	GDATA		gdata;
	UDATA		udata;

	uid_t		f_uid;
	gid_t		f_gid;
	double		temp_time = 0;

	DIR		*dir = 0, *parent = 0;
	struct dirent	*dp;
	long		loc;
	short		rescan_quick = FALSE;
	char		one_name[NAME_MAX];
	time_t		timenow;

	GLOBAL		g;

#if ( program_uid > 0 )
	setegid(program_gid);
	seteuid(program_uid);
#endif

	umask(0666 & 000);

	d_log(1, "rescan: PZS-NG (rescan) v%s debug log.\n", ng_version());

#ifdef _ALT_MAX
	d_log(1, "rescan: PATH_MAX not found - using predefined settings! Please report to the devs!\n");
#endif

	if (argc > 1) {
		if (!strncasecmp(argv[1], "--quick", 7))
			rescan_quick = TRUE;
		else {
			strncpy(one_name, argv[1], NAME_MAX - 1);
			if (one_name[strlen(one_name) - 1] == '*')
				one_name[strlen(one_name) - 1] = '\0';
			else if (!fileexists(one_name)) {
				printf("\nPZS-NG Rescan v%s: No file named '%s' exists.\n\n", ng_version(), one_name);
				return 1;
			}
		}		
	} else
		bzero(one_name, NAME_MAX);

	//d_log(1, "rescan: Allocating memory for variables\n");
	//g.ui = ng_realloc2(g.ui, sizeof(struct USERINFO *) * 30, 1, 1, 1);
	//g.gi = ng_realloc2(g.gi, sizeof(struct GROUPINFO *) * 30, 1, 1, 1);

	getcwd(g.l.path, PATH_MAX);

	if ((matchpath(nocheck_dirs, g.l.path) || matchpath(speedtest_dirs, g.l.path) || (!matchpath(zip_dirs, g.l.path) && !matchpath(sfv_dirs, g.l.path) && !matchpath(group_dirs, g.l.path))) && rescan_nocheck_dirs_allowed == FALSE) {
		d_log(1, "rescan: Dir matched with nocheck_dirs, or is not in the zip/sfv/group-dirs\n");
		d_log(1, "rescan: Freeing memory, and exiting\n");
		ng_free(g.ui);
		ng_free(g.gi);
		return 0;
	}
	g.v.misc.slowest_user[0] = 30000;

	g.v.misc.write_log = FALSE;

	bzero(&g.v.total, sizeof(struct race_total));
	g.v.misc.fastest_user[0] = 0;
	g.v.misc.release_type = RTYPE_NULL;

	if (getenv("SECTION") == NULL) {
		sprintf(g.v.sectionname, "DEFAULT");
	} else {
		snprintf(g.v.sectionname, 127, getenv("SECTION"));
	}

	g.l.race = ng_realloc2(g.l.race, n = (int)strlen(g.l.path) + 10 + sizeof(storage), 1, 1, 1);
	g.l.sfv = ng_realloc2(g.l.sfv, n, 1, 1, 1);
	g.l.leader = ng_realloc2(g.l.leader, n, 1, 1, 1);
	g.l.length_path = (int)strlen(g.l.path);
	g.l.length_zipdatadir = sizeof(storage);

	getrelname(&g);
	buffer_groups(&gdata, GROUPFILE, 0);
	buffer_users(&udata, PASSWDFILE, 0);

	sprintf(g.l.sfv, storage "/%s/sfvdata", g.l.path);
	sprintf(g.l.leader, storage "/%s/leader", g.l.path);
	sprintf(g.l.race, storage "/%s/racedata", g.l.path);

	d_log(1, "rescan: Creating directory to store racedata in\n");
 	maketempdir(g.l.path);

	/*d_log(1, "rescan: Locking release\n");
	while (1) {
		if ((k = create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 3, 0))) {
			d_log(1, "rescan: Failed to lock release.\n");
			if (k == 1) {
				d_log(1, "rescan: version mismatch. Exiting.\n");
				printf("Error. You need to rm -fR ftp-data/pzs-ng/* before rescan will work.\n");
				exit(EXIT_FAILURE);
			}
			if (k == PROGTYPE_POSTDEL) {
				n = (signed int)g.v.lock.data_incrementor;
				d_log(1, "rescan: Detected postdel running - sleeping for one second.\n");
				if (!create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 0, g.v.lock.data_queue))
					break;
				usleep(1000000);
				if (!create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 0, g.v.lock.data_queue))
					break;
				if ( n == (signed int)g.v.lock.data_incrementor) {
					d_log(1, "rescan: Failed to get lock. Forcing unlock.\n");
					if (create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 2, g.v.lock.data_queue)) {
						d_log(1, "rescan: Failed to force a lock.\n");
						d_log(1, "postdel: Exiting with error.\n");
						exit(EXIT_FAILURE);
					}
					break;
				}
			} else {
				for ( k = 0; k <= max_seconds_wait_for_lock * 10; k++) {
					d_log(1, "rescan: sleeping for .1 second before trying to get a lock (queue: %d).\n", g.v.lock.data_queue);
					usleep(100000);
					if (!create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 0, g.v.lock.data_queue))
						break;
				}
				if (k >= max_seconds_wait_for_lock * 10) {
					d_log(1, "rescan: Failed to get lock. Will not force unlock.\n");
					exit(EXIT_FAILURE);
				}
			}
		}
		usleep(10000);
		if (update_lock(&g.v, 1, 0) != -1)
			break;
	}*/

	move_progress_bar(1, &g.v, g.ui, g.gi);
	if (g.l.incomplete)
		unlink(g.l.incomplete);
	if (del_completebar)
		removecomplete();

	dir = opendir(".");
	parent = opendir("..");

	if (!((rescan_quick && findfileext(".", ".sfv")) || *one_name)) {
		if (g.l.sfv)
			unlink(g.l.sfv);
		if (g.l.race)
			unlink(g.l.race);
	}
	printf("Rescanning files...\n");
	
	if (findfileext(".", ".sfv")) {
		strlcpy(g.v.file.name, findfileext(".", ".sfv"), NAME_MAX);
		maketempdir(g.l.path);
		stat(g.v.file.name, &fileinfo);

		if (copysfv(g.v.file.name, g.l.sfv, &g.v, g.l.path , 1)) {
			printf("rescan: Found invalid entries in SFV - Exiting.\n");

			while ((dp = readdir(dir))) {
				m = l = (int)strlen(dp->d_name);
				ext = find_last_of(dp->d_name, "-");
				if (!strncmp(ext, "-missing", 8))
					unlink(dp->d_name);
			}

			d_log(1, "rescan: Freeing memory, removing lock and exiting\n");
			unlink(g.l.sfv);
			unlink(g.l.race);
			ng_free(g.ui);
			ng_free(g.gi);
			ng_free(g.l.race);
			ng_free(g.l.sfv);
			ng_free(g.l.leader);
			
			//remove_lock(&g.v);

			return 0;
		}
		g.v.total.start_time = 0;
		rewinddir(dir);
		while ((dp = readdir(dir))) {
			if (*one_name && strncasecmp(one_name, dp->d_name, strlen(one_name)))
				continue;
			m = l = (int)strlen(dp->d_name);

			ext = find_last_of(dp->d_name, ".");
			if (*ext == '.')
				ext++;

			/*if (!update_lock(&g.v, 1, 0)) {
				d_log(1, "rescan: Another process wants the lock - will comply and remove lock, then exit.\n");
				remove_lock(&g.v);
				exit(EXIT_FAILURE);
			}*/

			if (
				!strcomp(ignored_types, ext) &&
				(!(strcomp(allowed_types, ext) && !matchpath(allowed_types_exemption_dirs, g.l.path))) &&
				strcasecmp("sfv", ext) &&
				strcasecmp("nfo", ext) &&
				strcasecmp("bad", ext) &&
				strcmp(dp->d_name + l - 8, "-missing") &&
				strncmp(dp->d_name, ".", 1)
				) {
				
				stat(dp->d_name, &fileinfo);

				if (S_ISDIR(fileinfo.st_mode))
					continue;
				if (ignore_zero_sized_on_rescan && !fileinfo.st_size)
					continue;

				f_uid = fileinfo.st_uid;
				f_gid = fileinfo.st_gid;

				strcpy(g.v.user.group, get_g_name(&gdata, f_gid));
				strcpy(g.v.user.name, get_u_name(&udata, f_uid));
				strlcpy(g.v.file.name, dp->d_name, NAME_MAX);
				g.v.file.speed = 2005 * 1024;
				g.v.file.size = fileinfo.st_size;

				temp_time = fileinfo.st_mtime;
				
				if (g.v.total.start_time == 0)
					g.v.total.start_time = temp_time;
				else
					g.v.total.start_time = (g.v.total.start_time < temp_time ? g.v.total.start_time : temp_time);
				
				g.v.total.stop_time = (temp_time > g.v.total.stop_time ? temp_time : g.v.total.stop_time);

				/* Hide users in group_dirs */
				if (matchpath(group_dirs, g.l.path) && (hide_group_uploaders == TRUE)) {
					d_log(1, "rescan: Hiding user in group-dir:\n");
					if ((int)strlen(hide_gname) > 0) {
						snprintf(g.v.user.group, 18, "%s", hide_gname);
						d_log(1, "rescan:    Changing groupname\n");
					}
					if ((int)strlen(hide_uname) > 0) {
						snprintf(g.v.user.name, 18, "%s", hide_uname);
						d_log(1, "rescan:    Changing username\n");
					}
					if ((int)strlen(hide_uname) == 0) {
						d_log(1, "rescan:    Making username = groupname\n");
						snprintf(g.v.user.name, 18, "%s", g.v.user.group);
					}
				}

				if (!rescan_quick || (g.l.race && !match_file(g.l.race, dp->d_name)))
					crc = calc_crc32(dp->d_name);
				else
					crc = 1;

				if (!S_ISDIR(fileinfo.st_mode)) {
					if (g.v.file.name)
						unlink_missing(g.v.file.name);
					if (l > 44) {
						if (crc == 1)
							printf("\nFile: %s CHECKED", dp->d_name + l - 44);
						else
							printf("\nFile: %s %.8x", dp->d_name + l - 44, crc);
					} else {
						if (crc == 1)
							printf("\nFile: %-44s CHECKED", dp->d_name);
						else
							printf("\nFile: %-44s %.8x", dp->d_name, crc);
					}
				}
				if(fflush(stdout))
					d_log(1, "rescan: ERROR: %s\n", strerror(errno));
				if (!rescan_quick || (g.l.race && !match_file(g.l.race, dp->d_name)))
					writerace(g.l.race, &g.v, crc, F_NOTCHECKED);
			}
		}
		printf("\n");
		testfiles(&g.l, &g.v, 1);
		printf("\n");
		readsfv(g.l.sfv, &g.v, 0);
		readrace(g.l.race, &g.v, g.ui, g.gi);
		sortstats(&g.v, g.ui, g.gi);
		buffer_progress_bar(&g.v);

		if (g.l.nfo_incomplete) {
			if (findfileext(".", ".nfo")) {
				d_log(1, "rescan: Removing missing-nfo indicator (if any)\n");
				remove_nfo_indicator(&g);
			} else {
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
				if ((matchpath(check_for_missing_nfo_dirs, g.l.path) || n) && (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs)) {
					if (!g.l.in_cd_dir) {
						d_log(1, "rescan: Creating missing-nfo indicator %s.\n", g.l.nfo_incomplete);
						create_incomplete_nfo();
					} else {
						if (findfileext("..", ".nfo")) {
							d_log(1, "rescan: Removing missing-nfo indicator (if any)\n");
							remove_nfo_indicator(&g);
						} else {
							d_log(1, "rescan: Creating missing-nfo indicator (base) %s.\n", g.l.nfo_incomplete);
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
		}
		if (g.v.misc.release_type == RTYPE_AUDIO)
			get_mpeg_audio_info(findfileext(".", ".mp3"), &g.v.audio);

		if ((g.v.total.files_missing == 0) && (g.v.total.files > 0)) {

			switch (g.v.misc.release_type) {
			case RTYPE_RAR:
				complete_bar = rar_completebar;
				break;
			case RTYPE_OTHER:
				complete_bar = other_completebar;
				break;
			case RTYPE_AUDIO:
				complete_bar = audio_completebar;
#if ( create_m3u == TRUE )
				n = sprintf(exec, findfileext(".", ".sfv"));
				strcpy(exec + n - 3, "m3u");
				create_indexfile(g.l.race, &g.v, exec);
#endif
				break;
			case RTYPE_VIDEO:
				complete_bar = video_completebar;
				break;
			}
			complete(&g);

			if (complete_bar) {
				createstatusbar(convert(&g.v, g.ui, g.gi, complete_bar));
#if (chmod_completebar)
				if (!matchpath(group_dirs, g.l.path)) {
					chmod(convert(&g.v, g.ui, g.gi, complete_bar), 0222);
				}
#endif
			}
		} else {
			if (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs) {
				create_incomplete();
			}
				move_progress_bar(0, &g.v, g.ui, g.gi);
		}
	} else if (findfileext(".", ".zip")) {
		strlcpy(g.v.file.name, findfileext(".", ".zip"), NAME_MAX);
		maketempdir(g.l.path);
		stat(g.v.file.name, &fileinfo);
		//n = direntries;
		crc = 0;
		rewinddir(dir);
		timenow = time(NULL);
		while ((dp = readdir(dir))) {
			m = l = (int)strlen(dp->d_name);
			
			ext = find_last_of(dp->d_name, ".");
			if (*ext == '.')
				ext++;

			if (!strcasecmp(ext, "zip")) {
				stat(dp->d_name, &fileinfo);
				f_uid = fileinfo.st_uid;
				f_gid = fileinfo.st_gid;

				if ((timenow == fileinfo.st_ctime) && (fileinfo.st_mode & 0111)) {
					d_log(1, "rescan.c: Seems this file (%s) is in the process of being uploaded. Ignoring for now.\n", dp->d_name);
					continue;
				}
				strcpy(g.v.user.name, get_u_name(&udata, f_uid));
				strcpy(g.v.user.group, get_g_name(&gdata, f_gid));
				strlcpy(g.v.file.name, dp->d_name, NAME_MAX);
				g.v.file.speed = 2005 * 1024;
				g.v.file.size = fileinfo.st_size;
				g.v.total.start_time = 0;

				if (!fileexists("file_id.diz")) {
					sprintf(exec, "%s -qqjnCLL \"%s\" file_id.diz 2>.delme", unzip_bin, g.v.file.name);
					if (execute(exec) != 0) {
						d_log(1, "rescan: No file_id.diz found (#%d): %s\n", errno, strerror(errno));
					} else {
						if ((loc = findfile(".", "file_id.diz.bad")))
							remove_at_loc(".", loc);
						chmod("file_id.diz", 0666);
					}
				}
				sprintf(exec, "%s -qqt \"%s\" >.delme", unzip_bin, g.v.file.name);
				if (system(exec) == 0) {
					writerace(g.l.race, &g.v, crc, F_CHECKED);
				} else {
					writerace(g.l.race, &g.v, crc, F_BAD);
					if (g.v.file.name)
						unlink(g.v.file.name);
				}
			}
		}
		g.v.total.files = read_diz("file_id.diz");
		if (!g.v.total.files) {
			g.v.total.files = 1;
			unlink("file_id.diz");
		}
		g.v.total.files_missing = g.v.total.files;
		readrace(g.l.race, &g.v, g.ui, g.gi);
		sortstats(&g.v, g.ui, g.gi);
		if (g.v.total.files_missing < 0) {
			g.v.total.files -= g.v.total.files_missing;
			g.v.total.files_missing = 0;
		}
		buffer_progress_bar(&g.v);
		if (g.v.total.files_missing == 0) {
			complete(&g);
			createstatusbar(convert(&g.v, g.ui, g.gi, zip_completebar));
#if (chmod_completebar)
			if (!matchpath(group_dirs, g.l.path)) {
				chmod(convert(&g.v, g.ui, g.gi, zip_completebar), 0222);
			}
#endif

		} else {
			if (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs) {
				create_incomplete();
			}
				move_progress_bar(0, &g.v, g.ui, g.gi);
		}
		if (g.l.nfo_incomplete) {
			if (findfileext(".", ".nfo")) {
				d_log(1, "rescan: Removing missing-nfo indicator (if any)\n");
				remove_nfo_indicator(&g);
			} else {
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
				if ((matchpath(check_for_missing_nfo_dirs, g.l.path) || n) && (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs)) {
					if (!g.l.in_cd_dir) {
						d_log(1, "rescan: Creating missing-nfo indicator %s.\n", g.l.nfo_incomplete);
						create_incomplete_nfo();
					} else {
						if (findfileext("..", ".nfo")) {
							d_log(1, "rescan: Removing missing-nfo indicator (if any)\n");
							remove_nfo_indicator(&g);
						} else {
							d_log(1, "rescan: Creating missing-nfo indicator (base) %s.\n", g.l.nfo_incomplete);
							create_incomplete_nfo();
						}
					}
				}
			}
		}
	} else if (mark_empty_dirs_as_incomplete_on_rescan) {
			create_incomplete();
			printf(" Empty dir - marking as incomplete.\n");
	}
	printf(" Passed : %i\n", (int)g.v.total.files - (int)g.v.total.files_missing);
	printf(" Failed : %i\n", (int)g.v.total.files_bad);
	printf(" Missing: %i\n", (int)g.v.total.files_missing);
	printf("  Total : %i\n", (int)g.v.total.files);

	//d_log(1, "rescan: Freeing memory and removing lock.\n");
	closedir(dir);
	closedir(parent);
	//updatestats_free(&g);
	ng_free(g.l.race);
	ng_free(g.l.sfv);
	ng_free(g.l.leader);

	if (fileexists(".delme"))
		unlink(".delme");

	//remove_lock(&g.v);

	buffer_groups(&gdata, GROUPFILE, 1);
	buffer_users(&udata, PASSWDFILE, 1);

	exit(0);
}
