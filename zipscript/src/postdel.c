#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "helpfunctions.h"
#include "zsfunctions.h"
#include "race-file.h"
#include "objects.h"
#include "macros.h"
#include "convert.h"
#include "dizreader.h"
#include "stats.h"
#include "ng-version.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#include "postdel.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifndef HAVE_STRLCPY
# include "strl/strl.h"
#endif

int 
main(int argc, char **argv)
{
	char		fileext[4];
	char		*name_p = 0;
	char		*temp_p;
	char		*target;
	char		*fname;
	char		*env_user;
	char		*env_group;
	int		n;
	unsigned char	empty_dir = 0;
	unsigned char	incomplete = 0;
	
	GLOBAL		g;
	
	DIR		*dir, *parent;

	if (argc == 1) {
		d_log("no param specified\n");
		return 0;
	}

	if (strlen(argv[1]) < 6 || strncmp(argv[1], "DELE ", 5)) {
		printf("pzs-ng postdel script.\n");
		printf(" - this is supposed to be run from glftpd.\n");
		printf(" - if you wish to run it yourself from chroot, \n");
		printf(" - use /bin/postdel \"DELE <filename>\"\n");
		printf(" - thank you. (remember the quotes!)\n");
		return 0;
	}

	d_log("Project-ZS Next Generation (pzs-ng) v%s debug log for postdel.\n", ng_version());

	fname = argv[1] + 5;	/* This way we simply skip the required
				 * 'DELE'-part of the argument (so we get
				 * filename) */

	d_log("Reading user name from env\n");
	if ((env_user = getenv("USER")) == NULL) {
		d_log("Could not find environment variable 'USER', setting value to 'Nobody'\n");
		env_user = "Nobody";
	}
	d_log("Reading group name from env\n");
	if ((env_group = getenv("GROUP")) == NULL) {
		d_log("Could not find environment variable 'GROUP', setting value to 'NoGroup'\n");
		env_group = "NoGroup";
	}
#if ( program_uid > 0 )
	d_log("Trying to change effective gid\n");
	setegid(program_gid);
	d_log("Trying to change effective uid\n");
	seteuid(program_uid);
#endif

	if (!strcmp(fname, "debug"))
		d_log("Reading directory structure\n");

	dir = opendir(".");
	parent = opendir("..");
	
	if (fileexists(fname)) {
		d_log("File (%s) still exists\n", fname);
#if (remove_dot_debug_on_delete == TRUE)
		if (strcmp(fname, "debug"))
			unlink(fname);
#endif
		return 0;
	}
	umask(0666 & 000);

	d_log("Clearing arrays\n");
	bzero(&g.v.total, sizeof(struct race_total));
	g.v.misc.slowest_user[0] = 30000;
	g.v.misc.fastest_user[0] = 0;

	/* YARR; THE PAIN OF MAGIC NUMBERS! */
	d_log("Copying env/predefined username to g.v. (%s)\n", env_user);
	strlcpy(g.v.user.name, env_user, 24);
	
	d_log("Copying env/predefined groupname to g.v. (%s)\n", env_group);
	strlcpy(g.v.user.group, env_group, 24);
	g.v.user.group[23] = 0;

	d_log("File to remove is: %s\n", fname);

	if (!*g.v.user.group)
		memcpy(g.v.user.group, "NoGroup", 8);

	d_log("Allocating memory for variables\n");

	g.ui = malloc(sizeof(struct USERINFO *) * 30);
	memset(g.ui, 0, sizeof(struct USERINFO *) * 30);
	g.gi = malloc(sizeof(struct GROUPINFO *) * 30);
	memset(g.gi, 0, sizeof(struct GROUPINFO *) * 30);

	getcwd(g.l.path, PATH_MAX);

	if (matchpath(nocheck_dirs, g.l.path) || (!matchpath(zip_dirs, g.l.path) && !matchpath(sfv_dirs, g.l.path) && !matchpath(group_dirs, g.l.path))) {
		d_log("Dir matched with nocheck_dirs, or is not in the zip/sfv/group-dirs\n");
		d_log("Freeing memory, and exiting\n");
		free(g.ui);
		free(g.gi);

		if (remove_dot_debug_on_delete)
			unlink(".debug");

		return 0;

	}
	g.l.race = malloc(n = strlen(g.l.path) + 10 + sizeof(storage));
	g.l.sfv = malloc(n);
	g.l.leader = malloc(n);
	target = malloc(4096);

	if (getenv("SECTION") == NULL)
		sprintf(g.v.sectionname, "DEFAULT");
	else
		snprintf(g.v.sectionname, 127, getenv("SECTION"));

	d_log("Copying data &g.l into memory\n");
	strlcpy(g.v.file.name, fname, PATH_MAX);
	sprintf(g.l.sfv, storage "/%s/sfvdata", g.l.path);
	sprintf(g.l.leader, storage "/%s/leader", g.l.path);
	sprintf(g.l.race, storage "/%s/racedata", g.l.path);

	d_log("Caching release name\n");
	getrelname(&g);
	d_log("DEBUG 0: incomplete: '%s', path: '%s'\n", g.l.incomplete, g.l.path);

	d_log("Parsing file extension from filename...\n");

	temp_p = find_last_of(g.v.file.name, ".");

	if (*temp_p != '.') {
		d_log("Got: no extension\n");
		temp_p = name_p;
	} else {
		d_log("Got: %s\n", temp_p);
		temp_p++;
	}
	name_p++;


	if (sizeof(temp_p) - 4 > 0) {
		temp_p = temp_p + sizeof(temp_p) - 4;
		snprintf(fileext, 4, "%s", temp_p);
	} else
		sprintf(fileext, "---");

	switch (get_filetype_postdel(&g, fileext)) {
	case 0:
		d_log("File type is: ZIP\n");
		if (matchpath(zip_dirs, g.l.path)) {
			if (matchpath(group_dirs, g.l.path)) {
				g.v.misc.write_log = 0;
			} else {
				g.v.misc.write_log = 1;
			}
		} else if (matchpath(sfv_dirs, g.l.path) && strict_path_match) {
			if (matchpath(group_dirs, g.l.path)) {
				g.v.misc.write_log = 0;
			} else {
				d_log("Directory matched with sfv_dirs\n");
				break;
			}
		}

		if (!fileexists("file_id.diz")) {
			temp_p = findfileext(dir, ".zip");
			if (temp_p != NULL) {
				d_log("file_id.diz does not exist, trying to extract it from %s\n", temp_p);
				sprintf(target, "%s -qqjnCLL %s file_id.diz", unzip_bin, temp_p);
				execute(target);
				chmod("file_id.diz", 0666);
			}
		}
		d_log("Reading diskcount from diz\n");
		g.v.total.files = read_diz("file_id.diz");
		if (g.v.total.files == 0) {
			d_log("Could not get diskcount from diz\n");
			g.v.total.files = 1;
			
		}
		g.v.total.files_missing = g.v.total.files;

		d_log("Reading race data from file to memory\n");
		readrace(g.l.race, &g.v, g.ui, g.gi);

		d_log("Caching progress bar\n");
		buffer_progress_bar(&g.v);

		d_log("Removing old complete bar, if any\n");
		removecomplete();
		if (g.v.total.files_missing < 0) {
			g.v.total.files -= g.v.total.files_missing;
			g.v.total.files_missing = 0;
		}
		if (!g.v.total.files_missing) {
			d_log("Creating complete bar\n");
			createstatusbar(convert(&g.v, g.ui, g.gi, zip_completebar));
		} else if (g.v.total.files_missing < g.v.total.files) {
			if (g.v.total.files_missing == 1) {
				d_log("Writing INCOMPLETE to %s\n", log);
				writelog(&g, convert(&g.v, g.ui, g.gi, incompletemsg), general_incomplete_type);
			}
			incomplete = 1;
		} else {
			empty_dir = 1;
		}
		remove_from_race(g.l.race, g.v.file.name);
		break;
	case 1: /* SFV */
		d_log("Reading file count from sfvdata\n");
		readsfv(g.l.sfv, &g.v, 0);

		if (fileexists(g.l.race)) {
			d_log("Reading race data from file to memory\n");
			readrace(g.l.race, &g.v, g.ui, g.gi);
		}
		d_log("Caching progress bar\n");
		buffer_progress_bar(&g.v);

		if (g.v.total.files_missing == g.v.total.files) {
			empty_dir = 1;
		}
		d_log("SFV was removed - removing progressbar/completebar and -missing pointers.\n");
		removecomplete();

		if (fileexists(g.l.sfv)) {
			delete_sfv(g.l.sfv);
			unlink(g.l.sfv);	
		}

		if (g.l.nfo_incomplete)
			unlink(g.l.nfo_incomplete);
		if (g.l.incomplete)
			unlink(g.l.incomplete);
		move_progress_bar(1, &g.v, g.ui, g.gi);
		break;
	case 3:
		d_log("Removing old complete bar, if any\n");
		removecomplete();
		g.v.misc.write_log = matchpath(sfv_dirs, g.l.path) > 0 ? 1 - matchpath(group_dirs, g.l.path) : 0;

		if (fileexists(g.l.race)) {
			d_log("Reading race data from file to memory\n");
			readrace(g.l.race, &g.v, g.ui, g.gi);
		} else {
			empty_dir = 1;
		}
		if (fileexists(g.l.sfv)) {
#if ( create_missing_files == TRUE )
#if ( sfv_cleanup == TRUE )
#if ( sfv_cleanup_lowercase == TRUE )
			strtolower(g.v.file.name);
#endif
#endif
			create_missing(g.v.file.name);
#endif
			d_log("Reading file count from SFV\n");
			readsfv(g.l.sfv, &g.v, 0);

			d_log("Caching progress bar\n");
			buffer_progress_bar(&g.v);
		}
		if (g.v.total.files_missing < g.v.total.files) {
			if (g.v.total.files_missing == 1) {
				d_log("Writing INCOMPLETE to %s\n", log);
				writelog(&g, convert(&g.v, g.ui, g.gi, incompletemsg), general_incomplete_type);
			}
			incomplete = 1;
		} else {
			d_log("Removing old race data\n");
			unlink(g.l.race);
			if (findfileext(dir, ".sfv") == NULL) {
				empty_dir = 1;
			} else {
				incomplete = 1;
			}
		}
		remove_from_race(g.l.race, g.v.file.name);
		break;
	case 4:
		if (!fileexists(g.l.race))
			empty_dir = 1;
		break;
	case 255:
		if (!fileexists(g.l.race))
			empty_dir = 1;
		break;
	case 2:
		if (!fileexists(g.l.race)) {
			empty_dir = 1;
		} else {
			d_log("Reading race data from file to memory\n");
			readrace(g.l.race, &g.v, g.ui, g.gi);
			d_log("Caching progress bar\n");
			buffer_progress_bar(&g.v);
			if (g.v.total.files_missing == g.v.total.files) {
				empty_dir = 1;
			}
		}
		break;
	}

	if (empty_dir == 1) {
		
		d_log("Removing all files and directories created by zipscript\n");
		removecomplete();
		
		if (fileexists(g.l.sfv))
			delete_sfv(g.l.sfv);
		if (g.l.nfo_incomplete)
			unlink(g.l.nfo_incomplete);
		if (g.l.incomplete)
			unlink(g.l.incomplete);
			
		unlink("file_id.diz");
		unlink(g.l.sfv);
		unlink(g.l.race);
		unlink(g.l.leader);
		
		move_progress_bar(1, &g.v, g.ui, g.gi);
		
#if (remove_dot_files_on_delete == TRUE)
		removedotfiles(dir);
#endif

	}

	if (incomplete == 1 && g.v.total.files > 0) {

		getrelname(&g);
		if (g.l.nfo_incomplete) {
			if (findfileext(dir, ".nfo")) {
				d_log("Removing missing-nfo indicator (if any)\n");
				remove_nfo_indicator(&g);
			} else if (matchpath(check_for_missing_nfo_dirs, g.l.path) && (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs)) {
				if (!g.l.in_cd_dir) {
					d_log("Creating missing-nfo indicator %s.\n", g.l.nfo_incomplete);
					create_incomplete_nfo();
				} else {
					
					if (findfileextparent(parent, ".nfo")) {
						d_log("Removing missing-nfo indicator (if any)\n");
						remove_nfo_indicator(&g);
					} else {
						d_log("Creating missing-nfo indicator (base) %s.\n", g.l.nfo_incomplete);
						create_incomplete_nfo();
					}
				}
			}
		}
		if (!matchpath(group_dirs, g.l.path) || create_incomplete_links_in_group_dirs) {
			d_log("Creating incomplete indicator\n");
			d_log("   incomplete: '%s', path: '%s'\n", g.l.incomplete, g.l.path);
			create_incomplete();
		}
		d_log("Moving progress bar\n");
		move_progress_bar(0, &g.v, g.ui, g.gi);
	}
	
	d_log("Releasing memory\n");
	closedir(dir);
	closedir(parent);
	updatestats_free(&g);
	free(target);
	free(g.l.race);
	free(g.l.sfv);
	free(g.l.leader);

	d_log("Exit\n");

	if ((empty_dir == 1) && (fileexists(".debug")) && (remove_dot_debug_on_delete == TRUE))
		unlink(".debug");

	return 0;
}

unsigned char 
get_filetype_postdel(GLOBAL *g, char *ext)
{
	if (!strcasecmp(ext, "sfv"))
		return 1;
	if (!clear_file(g->l.race, g->v.file.name))
		return 4;
	if (!strcasecmp(ext, "zip"))
		return 0;
	if (!strcasecmp(ext, "nfo"))
		return 2;
	if (!strcomp(ignored_types, ext) || !strcomp(allowed_types, ext))
		return 3;

	return 255;
}

