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
#include "audiosort.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifndef HAVE_STRLCPY
# include "strl/strl.h"
#endif

void print_syntax(int chdir_allowed); /* Defined at the bottom of this file. */

int 
main(int argc, char *argv[])
{
	int		n, l, complete_type = 0, not_allowed = 0, argv_mode = 0;

#ifdef USING_GLFTPD
        int             gnum = 0, unum = 0;
	char		myflags[20];
#endif

	char           *ext, exec[4096], *complete_bar = 0, *inc_point[2];
	unsigned int	crc;
	struct stat	fileinfo;

	uid_t		f_uid;
	gid_t		f_gid;
	double		temp_time = 0;

	DIR		*dir, *parent;
	struct dirent	*dp;
	long		loc;
	time_t		timenow;
#if (test_for_password || extract_nfo || zip_clean)
	off_t		tempstream;
#endif

	short		rescan_quick = rescan_default_to_quick;
	char		one_name[NAME_MAX];
	char		*temp_p = NULL;
	int		chdir_allowed = 0, argnum = 0;
	GLOBAL		g;
#if (enable_rescan_script)
	char		target[PATH_MAX+NAME_MAX];
#endif

#if ( program_uid > 0 )
	setegid(program_gid);
	seteuid(program_uid);
#endif

	umask(0666 & 000);

	d_log("rescan: PZS-NG (rescan v2) %s debug log.\n", ng_version);
	d_log("rescan: Rescan executed by: (uid/gid) %d/%d\n", geteuid(), getegid());

#ifdef _ALT_MAX
	d_log("rescan: PATH_MAX not found - using predefined settings! Please report to the devs!\n");
#endif

	d_log("rescan: Allocating memory for variables\n");
	g.ui = ng_realloc2(g.ui, sizeof(*g.ui) * 30, 1, 1, 1);
	g.gi = ng_realloc2(g.gi, sizeof(*g.gi) * 30, 1, 1, 1);

	bzero(one_name, NAME_MAX);

#ifdef USING_GLFTPD
	if (getenv("FLAGS")) {
		strlcpy(myflags, getenv("FLAGS"), sizeof(myflags));
		n = strlen(myflags);
		while (n > 0) {
			--n;
			l = strlen(rescan_chdir_flags);
			while(l > 0) {
				--l;
				if (myflags[n] == rescan_chdir_flags[l])
					chdir_allowed = 1;
			}
		}
	}
	if (!geteuid())
		chdir_allowed = 1;
#endif

	/* With glftpd we can use env vars, rest of the world: commandline. */
#ifndef USING_GLFTPD
	if (argc < 7) {
		print_syntax(chdir_allowed);
		ng_free(g.ui);
		ng_free(g.gi);
		return 0;
	}
	argnum = 6;

	if (chdir(argv[5]) != 0) {
		printf("Could not chdir to <cwd = '%s'>, ftpd agnostic mode: %s\n", argv[5], strerror(errno));
		d_log("rescan: Could not chdir to <cwd = '%s'>, ftpd agnostic mode: %s\n", argv[5], strerror(errno));
		ng_free(g.ui);
		ng_free(g.gi);
		return 1;
        }
#else
	argnum = 1;
#endif

	while ((argnum < argc) && argc > 1) {
		if (!strncasecmp(argv[argnum], "--quick", 7))
			rescan_quick = TRUE;
		else if (!strncasecmp(argv[argnum], "--normal", 8))
			rescan_quick = FALSE;
		else if (!strncasecmp(argv[argnum], "--dir=", 6) && (strlen(argv[argnum]) > 7) && chdir_allowed) {
			temp_p = argv[argnum] + 6;
			if ((!matchpath(nocheck_dirs, temp_p)) && (matchpath(zip_dirs, temp_p) || matchpath(sfv_dirs, temp_p)) && !matchpath(group_dirs, temp_p)) {
				if (chdir(temp_p)) {
					d_log("rescan: Failed to chdir() to %s : %s\n", temp_p, strerror(errno));
					not_allowed = 1;
				}
			} else {
				printf("Not allowed to chdir() to %s\n", temp_p);
				ng_free(g.ui);
				ng_free(g.gi);
				return 1;
			}
			printf("PZS-NG Rescan %s: Rescanning %s\n", ng_version, temp_p);
			argv_mode = 1;

		} else if (!strncasecmp(argv[argnum], "--chroot=", 9) && (strlen(argv[argnum]) > 10) && chdir_allowed) {
			if (temp_p == NULL) {
				temp_p = argv[argnum] + 9;
				if (chroot(temp_p) == -1) {
					d_log("rescan: Failed to chroot() to %s : %s\n", temp_p, strerror(errno));
					not_allowed = 1;
				}
			} else {
				temp_p = argv[argnum] + 9;
				printf("Not allowed to chroot() to %s\n", temp_p);
				ng_free(g.ui);
				ng_free(g.gi);
				return 1;
			}
			printf("PZS-NG Rescan %s: Chroot'ing to %s\n", ng_version, temp_p);
			argv_mode = 1;

		} else if (!strncasecmp(argv[argnum], "--help", 6) || !strncasecmp(argv[argnum], "/?", 2) || !strncasecmp(argv[argnum], "--?", 3)) {
                        print_syntax(chdir_allowed);
			ng_free(g.ui);
			ng_free(g.gi);
                        return 0;
		} else {
			strlcpy(one_name, argv[argnum], sizeof(one_name));
			rescan_quick = FALSE;
			printf("PZS-NG Rescan %s: Rescanning in FILE mode\n", ng_version);
			if (one_name[strlen(one_name) - 1] == '*') {
				one_name[strlen(one_name) - 1] = '\0';
			} else if (!fileexists(one_name)) {
				d_log("PZS-NG Rescan: No file named '%s' exists.\n", one_name);
				one_name[0] = '\0';
				not_allowed = 1;
			}
			argv_mode = 1;
		}
		argnum++;
	}
	if (one_name[0] == '\0') {
		if (rescan_quick == TRUE) {
			printf("PZS-NG Rescan %s: Rescanning in QUICK mode.\n", ng_version);
		} else {
			printf("PZS-NG Rescan %s: Rescanning in NORMAL mode.\n", ng_version);
		}
	}
	printf("PZS-NG Rescan %s: Use --help for options.\n\n", ng_version);

	if (not_allowed) {
		ng_free(g.ui);
		ng_free(g.gi);
		return 1;
	}

	if (!getcwd(g.l.path, PATH_MAX)) {
		d_log("rescan: getcwd() failed: %s\n", strerror(errno));
	}
	if (subcomp(g.l.path, g.l.basepath) && (g.l.basepath[0] == '\0'))
		strlcpy(g.l.basepath, g.l.path, sizeof(g.l.basepath));
	if (strncmp(g.l.path, g.l.basepath, PATH_MAX))
		d_log("rescan: We are in subdir of %s\n", g.l.basepath);
        strlcpy(g.v.misc.current_path, g.l.path, sizeof(g.v.misc.current_path));
        strlcpy(g.v.misc.basepath, g.l.basepath, sizeof(g.v.misc.basepath));

	if ((matchpath(nocheck_dirs, g.l.path) && !rescan_nocheck_dirs_allowed) || (matchpath(group_dirs, g.l.path) && argv_mode) || (!matchpath(nocheck_dirs, g.l.path) && !matchpath(zip_dirs, g.l.path) && !matchpath(sfv_dirs, g.l.path) && !matchpath(group_dirs, g.l.path)) || insampledir(g.l.path)) {
		d_log("rescan: Dir matched with nocheck_dirs/sample_list, or is not in the zip/sfv/group-dirs.\n");
		d_log("rescan: Freeing memory, and exiting.\n");
		printf("Notice: Unable to rescan this dir - check config.\n\n");
		ng_free(g.ui);
		ng_free(g.gi);
		return 0;
	}
	g.v.misc.slowest_user[0] = ULONG_MAX;

	bzero(&g.v.total, sizeof(struct race_total));
	g.v.misc.fastest_user[0] = 0;
	g.v.misc.release_type = RTYPE_NULL;
	g.v.misc.write_log = 0;

#ifdef USING_GLFTPD
	if (getenv("SECTION") == NULL) {
		sprintf(g.v.sectionname, "DEFAULT");
	} else {
		snprintf(g.v.sectionname, sizeof(g.v.sectionname), "%s", getenv("SECTION"));
	}
#else
        snprintf(g.v.sectionname, sizeof(g.v.sectionname), argv[4]);
#endif

	g.l.length_path = (int)strlen(g.l.path);
	g.l.length_zipdatadir = sizeof(storage);
	n = g.l.length_path + g.l.length_zipdatadir + 11;
	g.l.race = ng_realloc2(g.l.race, n, 1, 1, 1);
	g.l.sfv = ng_realloc2(g.l.sfv, n - 1, 1, 1, 1);
	g.l.sfvbackup = ng_realloc2(g.l.sfvbackup, n + 1, 1, 1, 1);
	g.l.leader = ng_realloc2(g.l.leader, n - 2, 1, 1, 1);
	g.l.sfv_incomplete = 0;

	getrelname(&g);

#ifdef USING_GLFTPD
	gnum = buffer_groups(GROUPFILE, 0);
	unum = buffer_users(PASSWDFILE, 0);
#endif

	sprintf(g.l.sfv, storage "/%s/sfvdata", g.l.path);
	sprintf(g.l.sfvbackup, storage "/%s/sfvbackup", g.l.path);
	sprintf(g.l.leader, storage "/%s/leader", g.l.path);
	sprintf(g.l.race, storage "/%s/racedata", g.l.path);
	d_log("rescan: Creating directory to store racedata in\n");
 	maketempdir(g.l.path);

	d_log("rescan: Locking release\n");
	while (1) {
		if ((l = create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 3, 0))) {
			d_log("rescan: Failed to lock release.\n");
			if (l == 1) {
				d_log("rescan: version mismatch. Exiting.\n");
				printf("Error. You need to \"rm -fR ftp-data/pzs-ng/\" before rescan will work.\n");
				ng_free(g.ui);
				ng_free(g.gi);
				ng_free(g.l.sfv);
				ng_free(g.l.sfvbackup);
				ng_free(g.l.leader);
				ng_free(g.l.race);
#ifdef USING_GLFTPD
				buffer_groups(GROUPFILE, gnum);
				buffer_users(PASSWDFILE, unum);
#endif
				exit(EXIT_FAILURE);
			}
			if (l == PROGTYPE_POSTDEL) {
				n = (signed int)g.v.data_incrementor;
				d_log("rescan: Detected postdel running - sleeping for one second.\n");
				if (!create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 0, g.v.data_queue))
					break;
				usleep(1000000);
				if (!create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 0, g.v.data_queue))
					break;
				if ( n == (signed int)g.v.data_incrementor) {
					d_log("rescan: Failed to get lock. Forcing unlock.\n");
					if (create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 2, g.v.data_queue)) {
						d_log("rescan: Failed to force a lock.\n");
						d_log("rescan: Exiting with error.\n");
						ng_free(g.ui);
						ng_free(g.gi);
						ng_free(g.l.sfv);
						ng_free(g.l.sfvbackup);
						ng_free(g.l.leader);
						ng_free(g.l.race);
#ifdef USING_GLFTPD
						buffer_groups(GROUPFILE, gnum);
						buffer_users(PASSWDFILE, unum);
#endif
						exit(EXIT_FAILURE);
					}
					break;
				}
			} else {
				for (l = 0; l <= max_seconds_wait_for_lock * 10; ++l) {
					d_log("rescan: sleeping for .1 second before trying to get a lock (queue: %d).\n", g.v.data_queue);
					usleep(100000);
					if (!create_lock(&g.v, g.l.path, PROGTYPE_RESCAN, 0, g.v.data_queue))
						break;
				}
				if (l >= max_seconds_wait_for_lock * 10) {
					d_log("rescan: Failed to get lock. Will not force unlock.\n");
					ng_free(g.ui);
					ng_free(g.gi);
					ng_free(g.l.sfv);
					ng_free(g.l.sfvbackup);
					ng_free(g.l.leader);
					ng_free(g.l.race);
#ifdef USING_GLFTPD
					buffer_groups(GROUPFILE, gnum);
					buffer_users(PASSWDFILE, unum);
#endif
					exit(EXIT_FAILURE);
				}
			}
		}
		usleep(10000);
		if (update_lock(&g.v, 1, 0) != -1)
			break;
	}

	move_progress_bar(1, &g.v, g.ui, g.gi);
	if (g.l.incomplete)
		unlink(g.l.incomplete);
	removecomplete(g.v.misc.release_type);

	dir = opendir(".");
	parent = opendir("..");

	if (!((rescan_quick && findfileext(dir, ".sfv")) || *one_name)) {
		if (g.l.sfv)
			unlink(g.l.sfv);
		if (g.l.race)
			unlink(g.l.race);
	}
	printf("Rescanning files...\n");

	if (findfileext(dir, ".zip")) {
		if (!fileexists(unzip_bin)) {
			printf("rescan: ERROR! Not able to check zip-files - %s does not exist!\n", unzip_bin);
			closedir(dir);
			closedir(parent);
			ng_free(g.ui);
			ng_free(g.gi);
			ng_free(g.l.sfv);
			ng_free(g.l.sfvbackup);
			ng_free(g.l.leader);
			ng_free(g.l.race);
#ifdef USING_GLFTPD
			buffer_groups(GROUPFILE, gnum);
			buffer_users(PASSWDFILE, unum);
#endif
			remove_lock(&g.v);
			exit(EXIT_FAILURE);
		} else {
			crc = 0;
			rewinddir(dir);
			timenow = time(NULL);
			while ((dp = readdir(dir))) {
				ext = find_last_of(dp->d_name, ".");
				if (*ext == '.')
					ext++;
				if (!strcasecmp(ext, "zip")) {
					stat(dp->d_name, &fileinfo);
					f_uid = fileinfo.st_uid;
					f_gid = fileinfo.st_gid;
					if ((timenow == fileinfo.st_ctime) && (fileinfo.st_mode & 0111)) {
						d_log("rescan.c: Seems this file (%s) is in the process of being uploaded. Ignoring for now.\n", dp->d_name);
						continue;
					}
#ifdef USING_GLFTPD
					strlcpy(g.v.user.name, get_u_name(f_uid), sizeof(g.v.user.name));
					strlcpy(g.v.user.group, get_g_name(f_gid), sizeof(g.v.user.group));
#else
					strlcpy(g.v.user.name, argv[1], sizeof(g.v.user.name));
					strlcpy(g.v.user.group, argv[2], sizeof(g.v.user.group));
#endif
					strlcpy(g.v.file.name, dp->d_name, sizeof(g.v.file.name));
					g.v.file.speed = 2005 * 1024;
					g.v.file.size = fileinfo.st_size;
					g.v.total.start_time = 0;
#if (test_for_password || extract_nfo)
					tempstream = telldir(dir);
					if ((!findfileextcount(dir, ".nfo") || findfileextcount(dir, ".zip")) && !mkdir(".unzipped", 0777))
						snprintf(exec, sizeof(exec), "%s -qqjo \"%s\" -d .unzipped 2>.delme", unzip_bin, g.v.file.name);
					else
						snprintf(exec, sizeof(exec), "%s -qqt \"%s\" 2>.delme", unzip_bin, g.v.file.name);
					seekdir(dir, tempstream);
#else
					snprintf(exec, sizeof(exec), "%s -qqt \"%s\" 2>.delme", unzip_bin, g.v.file.name);
#endif
					if (system(exec) == 0 || (allow_error2_in_unzip == TRUE && errno < 3 )) {
						writerace(g.l.race, &g.v, crc, F_CHECKED);
					} else {
						writerace(g.l.race, &g.v, crc, F_BAD);
						if (g.v.file.name)
							unlink(g.v.file.name);
						removedir(".unzipped");
						continue;
					}
#if (test_for_password || extract_nfo || zip_clean)
					tempstream = telldir(dir);
                        	        if ((!findfileextcount(dir, ".nfo") || findfileextcount(dir, ".zip")) && check_zipfile(".unzipped", g.v.file.name, findfileextcount(dir, ".nfo"))) {
                                	        d_log("rescan: File %s is password protected.\n", g.v.file.name);
						writerace(g.l.race, &g.v, crc, F_BAD);
						if (g.v.file.name)
							unlink(g.v.file.name);
						seekdir(dir, tempstream);
						continue;
	                                }
					seekdir(dir, tempstream);
#endif
					if (!fileexists("file_id.diz")) {
						snprintf(exec, sizeof(exec), "%s -qqjnCLL \"%s\" file_id.diz 2>.delme", unzip_bin, g.v.file.name);
						if (execute(exec) != 0) {
							d_log("rescan: No file_id.diz found (#%d): %s\n", errno, strerror(errno));
						} else {
							if (fileexists("file_id.diz.bad")) {
								loc = findfile(dir, "file_id.diz.bad");
								seekdir(dir, loc);
								dp = readdir(dir);
								unlink(dp->d_name);
							}
							if (chmod("file_id.diz", 0666))
								d_log("rescan: Failed to chmod %s: %s\n", "file_id.diz", strerror(errno));
						}
					}
				}
			}

			if (fileexists(".delme"))
				unlink(".delme");

			g.v.total.files = read_diz();
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
				complete(&g, complete_type);
				if (zip_completebar) {
					createstatusbar(convert(&g.v, g.ui, g.gi, zip_completebar));
#if (chmod_completebar)
					if (!matchpath(group_dirs, g.l.path)) {
						if (chmod_each(convert(&g.v, g.ui, g.gi, zip_completebar), 0222))
							d_log("rescan: Failed to chmod a statusbar: %s\n", strerror(errno));
					}
				}
#endif

			} else {
				if (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs) {
					if (create_incomplete()) {
						d_log("rescan: create_incomplete() returned something\n");
					}
				}
				move_progress_bar(0, &g.v, g.ui, g.gi);
			}
			if (g.l.nfo_incomplete) {
				if (findfileext(dir, ".nfo")) {
					d_log("rescan: Removing missing-nfo indicator (if any)\n");
					remove_nfo_indicator(&g);
				} else if (matchpath(check_for_missing_nfo_dirs, g.l.path) && (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs)) {
					if (!g.l.in_cd_dir) {
						d_log("rescan: Creating missing-nfo indicator %s.\n", g.l.nfo_incomplete);
						if (create_incomplete_nfo()) {
							d_log("rescan: create_incomplete_nfo() returned something\n");
						}
					} else {
						if (findfileextparent(parent, ".nfo")) {
							d_log("rescan: Removing missing-nfo indicator (if any)\n");
							remove_nfo_indicator(&g);
						} else {
							d_log("rescan: Creating missing-nfo indicator (base) %s.\n", g.l.nfo_incomplete);
							if (create_incomplete_nfo()) {
								d_log("rescan: create_incomplete_nfo() returned something\n");
							}
						}
					}
				}
			}
		}
	} else if ((temp_p = findfileext(dir, ".sfv")) || (create_missing_sfv && file_count(dir))) {
		if (!temp_p && create_missing_sfv && file_count(dir)) {
			d_log("rescan: No sfv found - creating one.\n");
			make_sfv(g.l.path);
			if (!(temp_p = findfileext(dir, ".sfv"))) {
				d_log("rescan: Freeing memory, removing lock and exiting.\n");
				unlink(g.l.sfv);
				if (fileexists(g.l.sfvbackup))
				unlink(g.l.sfvbackup);
				unlink(g.l.race);
				closedir(dir);
				closedir(parent);
				ng_free(g.ui);
				ng_free(g.gi);
				ng_free(g.l.sfv);
				ng_free(g.l.sfvbackup);
				ng_free(g.l.leader);
				ng_free(g.l.race);
#ifdef USING_GLFTPD
				buffer_groups(GROUPFILE, gnum);
				buffer_users(PASSWDFILE, unum);
#endif
				remove_lock(&g.v);
				return 0;
			}
		}
#if ( create_missing_sfv_link == TRUE )
		d_log("rescan: Removing missing-sfv indicator (if any)\n");
		unlink(g.l.sfv_incomplete);
#endif
		strlcpy(g.v.file.name, temp_p, sizeof(g.v.file.name));

		maketempdir(g.l.path);
		stat(g.v.file.name, &fileinfo);

		if (copysfv(g.v.file.name, g.l.sfv, &g.v)) {
			printf("Found invalid entries in SFV - Exiting.\n");

			while ((dp = readdir(dir))) {
				ext = find_last_of(dp->d_name, "-");
				if (!strcasecmp(ext, "-missing"))
					unlink(dp->d_name);
			}

			d_log("rescan: Freeing memory, removing lock and exiting\n");
			unlink(g.l.sfv);
			if (fileexists(g.l.sfvbackup))
			unlink(g.l.sfvbackup);
			unlink(g.l.race);
			closedir(dir);
			closedir(parent);
			ng_free(g.ui);
			ng_free(g.gi);
			ng_free(g.l.sfv);
			ng_free(g.l.sfvbackup);
			ng_free(g.l.leader);
			ng_free(g.l.race);
#ifdef USING_GLFTPD
			buffer_groups(GROUPFILE, gnum);
			buffer_users(PASSWDFILE, unum);
#endif
			remove_lock(&g.v);

			return 0;
		}
		g.v.total.start_time = 0;
		rewinddir(dir);
		while ((dp = readdir(dir))) {
			if (*one_name && strncasecmp(one_name, dp->d_name, strlen(one_name)))
				continue;

			l = (int)strlen(dp->d_name);

			ext = find_last_of(dp->d_name, ".-");
			if (*ext == '.')
				ext++;

			if (!update_lock(&g.v, 1, 0)) {
				d_log("rescan: Another process wants the lock - will comply and remove lock, then exit.\n");
				closedir(dir);
				closedir(parent);
				ng_free(g.ui);
				ng_free(g.gi);
				ng_free(g.l.sfv);
				ng_free(g.l.sfvbackup);
				ng_free(g.l.leader);
				ng_free(g.l.race);
#ifdef USING_GLFTPD
				buffer_groups(GROUPFILE, gnum);
				buffer_users(PASSWDFILE, unum);
#endif
				remove_lock(&g.v);
				exit(EXIT_FAILURE);
			}

			if (
				!strcomp(ignored_types, ext) &&
				(!(strcomp(allowed_types, ext) && !matchpath(allowed_types_exemption_dirs, g.l.path))) &&
				strcasecmp("sfv", ext) &&
				strcasecmp("nfo", ext) &&
				strcasecmp("bad", ext) &&
				strcasecmp("-missing", ext) &&
				strncmp(dp->d_name, ".", 1)
				) {

				stat(dp->d_name, &fileinfo);

				if (S_ISDIR(fileinfo.st_mode))
					continue;
				if (ignore_zero_sized_on_rescan && !fileinfo.st_size)
					continue;

				f_uid = fileinfo.st_uid;
				f_gid = fileinfo.st_gid;

#ifdef USING_GLFTPD
				strlcpy(g.v.user.name, get_u_name(f_uid), sizeof(g.v.user.name));
				strlcpy(g.v.user.group, get_g_name(f_gid), sizeof(g.v.user.group));
#else
				strlcpy(g.v.user.name, argv[1], sizeof(g.v.user.name));
				strlcpy(g.v.user.group, argv[2], sizeof(g.v.user.group));
#endif

				strlcpy(g.v.file.name, dp->d_name, sizeof(g.v.file.name));
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
					d_log("rescan: Hiding user in group-dir:\n");
					if ((int)strlen(hide_gname) > 0) {
						snprintf(g.v.user.group, sizeof(g.v.user.group), "%s", hide_gname);
						d_log("rescan:    Changing groupname\n");
					}
					if ((int)strlen(hide_uname) > 0) {
						snprintf(g.v.user.name, sizeof(g.v.user.name), "%s", hide_uname);
						d_log("rescan:    Changing username\n");
					}
#if (show_users_in_group_dirs == FALSE)
					if ((int)strlen(hide_uname) == 0) {
						d_log("rescan:    Making username = groupname\n");
						snprintf(g.v.user.name, sizeof(g.v.user.name), "%s", g.v.user.group);
					}
#endif
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
					d_log("rescan: ERROR: %s\n", strerror(errno));
				if (!rescan_quick || (g.l.race && !match_file(g.l.race,	dp->d_name)) || !fileexists(dp->d_name))
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
			if (findfileext(dir, ".nfo")) {
				d_log("rescan: Removing missing-nfo indicator (if any)\n");
				remove_nfo_indicator(&g);
			} else if (matchpath(check_for_missing_nfo_dirs, g.l.path) && (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs)) {
				if (!g.l.in_cd_dir) {
					d_log("rescan: Creating missing-nfo indicator %s.\n", g.l.nfo_incomplete);
					if (create_incomplete_nfo()) {
						d_log("rescan: create_incomplete_nfo() returned something\n");
					}
				} else {
					if (findfileextparent(parent, ".nfo")) {
						d_log("rescan: Removing missing-nfo indicator (if any)\n");
						remove_nfo_indicator(&g);
					} else {
						d_log("rescan: Creating missing-nfo indicator (base) %s.\n", g.l.nfo_incomplete);

						/* This is not pretty, but should be functional. */
						if ((inc_point[0] = find_last_of(g.l.path, "/")) != g.l.path)
							*inc_point[0] = '\0';
						if ((inc_point[1] = find_last_of(g.v.misc.release_name, "/")) != g.v.misc.release_name)
							*inc_point[1] = '\0';
						if (create_incomplete_nfo()) {
							d_log("rescan: create_incomplete_nfo() returned something\n");
						}
						if (*inc_point[0] == '\0')
							*inc_point[0] = '/';
						if (*inc_point[1] == '\0')
							*inc_point[1] = '/';
					}
				}
			}
		}
#if (create_missing_sample_link)
		if (g.l.sample_incomplete) {
			if (findfileextsub(".", sample_types, sample_list) || matchpartialdirname(missing_sample_check_ignore_list, g.v.misc.release_name, missing_sample_check_ignore_dividers)) {
				d_log("rescan: Removing missing-sample indicator (if any)\n");
				remove_sample_indicator(&g);
			} else if (matchpath(check_for_missing_sample_dirs, g.l.path) && (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs)) {
				if (!g.l.in_cd_dir) {
					d_log("rescan: Creating missing-sample indicator %s.\n", g.l.sample_incomplete);
					if (create_incomplete_sample()) {
						d_log("rescan: create_incomplete_sample() returned something\n");
					}
				} else {
					if (findfileextsub("..", sample_types, sample_list)) {
						d_log("rescan: Removing missing-sample indicator (if any)\n");
						remove_sample_indicator(&g);
					} else {
						d_log("rescan: Creating missing-sample indicator (base) %s.\n", g.l.sample_incomplete);

						/* This is not pretty, but should be functional. */
						if ((inc_point[0] = find_last_of(g.l.path, "/")) != g.l.path)
							*inc_point[0] = '\0';
						if ((inc_point[1] = find_last_of(g.v.misc.release_name, "/")) != g.v.misc.release_name)
							*inc_point[1] = '\0';
						if (create_incomplete_sample()) {
							d_log("rescan: create_incomplete_sample() returned something\n");
						}
						if (*inc_point[0] == '\0')
							*inc_point[0] = '/';
						if (*inc_point[1] == '\0')
							*inc_point[1] = '/';
					}
				}
			}
		}
#endif
		if (g.v.misc.release_type == RTYPE_AUDIO) {
			get_audio_info(findfileextfromlist(dir, audio_types), &g.v.audio);
			/* Sort if we're not in a group-dir/nosort-dir. */
			if (!matchpath(group_dirs, g.l.path) && !matchpath(audio_nosort_dirs, g.l.path)) {
				printf(" Resorting release.\n");
				audioSort(&g.v.audio, g.l.link_source, g.l.link_target);
			}
		}
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
				n = snprintf(exec, sizeof(exec), "%s", findfileext(dir, ".sfv"));
				strcpy(exec + n - 3, "m3u");
				create_indexfile(g.l.race, &g.v, exec);
#endif
				break;
			case RTYPE_VIDEO:
				complete_bar = video_completebar;
				break;
			}
			complete(&g, complete_type);

			if (complete_bar) {
				createstatusbar(convert(&g.v, g.ui, g.gi, complete_bar));
#if (chmod_completebar)
				if (!matchpath(group_dirs, g.l.path)) {
                                        if (chmod_each(convert(&g.v, g.ui, g.gi, complete_bar), 0222))
                                                d_log("rescan: Failed to chmod a statusbar: %s\n", strerror(errno));
				}
#endif
			}
#if (enable_rescan_script)
			d_log("rescan: Executing rescan_script script\n");
			if (!fileexists(rescan_script)) {
				d_log("rescan: Warning - rescan_script (%s) - file does not exist!\n", rescan_script);
			} else {
				snprintf(target, sizeof(target), rescan_script " \"%s\"", g.v.file.name);
				if (execute(target) != 0)
					d_log("rescan: Failed to execute rescan_script: %s\n", strerror(errno));
			}
#endif
		} else {
			if (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs) {
				if (create_incomplete()) {
					d_log("rescan: create_incomplete() returned something\n");
				}
			}
				move_progress_bar(0, &g.v, g.ui, g.gi);
		}
	} else {
		int empty = 1;
#if (create_missing_sfv_link == TRUE)
		if ((!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs) && g.l.sfv_incomplete && !matchpath(nocheck_dirs, g.l.path) && !matchpath(allowed_types_exemption_dirs, g.l.path)) {
			rewinddir(dir);
			while ((dp = readdir(dir))) {
				stat(dp->d_name, &fileinfo);
				if (S_ISREG(fileinfo.st_mode)) {
					ext = find_last_of(dp->d_name, ".");
					if (*ext == '.')
						ext++;
					if (*ext && get_filetype(&g, ext) == 3) {
						d_log("rescan: Creating missing-sfv indicator %s.\n", g.l.sfv_incomplete);
						if (create_incomplete_sfv())
							d_log("rescan: create_incomplete_sfv() returned something.\n");
						empty = 0;
						break;
					}
				}
			}
		}
#endif
		if (empty && mark_empty_dirs_as_incomplete_on_rescan) {
			if (create_incomplete()) {
				d_log("rescan: create_incomplete() returned something\n");
			}
			printf(" Empty dir - marking as incomplete.\n");
		}
	}

	closedir(dir);
	closedir(parent);

	printf(" Passed : %i\n", (int)g.v.total.files - (int)g.v.total.files_missing);
	printf(" Failed : %i\n", (int)g.v.total.files_bad);
	printf(" Missing: %i\n", (int)g.v.total.files_missing);
	printf("  Total : %i\n", (int)g.v.total.files);

	if (g.v.total.files && !g.v.total.files_missing) {
		g.v.misc.data_completed = 1;
	} else {
		g.v.misc.data_completed = 0;
	}

	d_log("rescan: Freeing memory and removing lock.\n");
	remove_lock(&g.v);
	ng_free(g.l.race);
	ng_free(g.l.sfv);
	ng_free(g.l.sfvbackup);
	ng_free(g.l.leader);

	updatestats_free(&g);

#ifdef USING_GLFTPD
	buffer_groups(GROUPFILE, gnum);
	buffer_users(PASSWDFILE, unum);
#endif

	exit(0);
}

void print_syntax(int chdir_allowed)
{
    printf("PZS-NG Rescan %s options:\n\n", ng_version);
#ifndef USING_GLFTPD
    printf("  [non-glftpd] The first 4 arguments must be: <user> <group> <tagline> <section> <current working dir>, after that - normal options (or none)\n");
#endif
    printf("  --quick         - scan in quick mode - only files not previously marked as ok by the zipscript is scanned\n");
    printf("  --normal        - scan in normal mode - all files will be rescanned regardless of their status\n");
    if (chdir_allowed)
        printf("  --chroot=<PATH> - chroot to PATH before beginning to rescan.\n");
    printf("  --dir=<PATH>    - cd to (chroot'ed) PATH before beginning to rescan.\n");
    printf("  <FILE><*>    - scan only file named FILE or files beginning with FILE*.\n\n");
}
