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

#include "zsfunctions.h"
#include "race-file.h"
#include "objects.h"
#include "macros.h"
#include "convert.h"
#include "dizreader.h"
#include "ng-version.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

struct USERINFO **userI;
struct GROUPINFO **groupI;
struct VARS	raceI;
struct LOCATIONS locations;

void 
writelog(char *msg, char *status)
{
	FILE           *glfile;
	char           *date;
	char           *line, *newline;
	time_t		timenow;

	if (raceI.misc.write_log == TRUE && !matchpath(group_dirs, locations.path)) {
		timenow = time(NULL);
		date = ctime(&timenow);
		if (!(glfile = fopen(log, "a+"))) {
			d_log("Unable to open %s for read/write (append) - NO RACEINFO WILL BE WRITTEN!\n", log);
			return;
		}
		line = newline = msg;
		while (1) {
			switch (*newline++) {
			case 0:
				fprintf(glfile, "%.24s %s: \"%s\" %s\n", date, status, locations.path, line);
				fclose(glfile);
				return;
			case '\n':
				fprintf(glfile, "%.24s %s: \"%s\" %.*s\n", date, status, locations.path, (int)(newline - line - 1), line);
				line = newline;
				break;
			}
		}
	}
}

/*
 * GET NAME OF MULTICD RELEASE (CDx/D[Ii]S[CK]x/DVDx) (SYMLINK LOCATION +
 * INCOMPLETE FILENAME)
 */
void 
getrelname(char *directory)
{
	int		cnt       , l[2], n = 0, k = 2;
	char           *path[2];

	for (cnt = strlen(directory); k && cnt; cnt--) {
		if (directory[cnt] == '/') {
			k--;
			l[k] = n;
			path[k] = malloc(n + 1);
			strncpy(path[k], directory + cnt + 1, n);
			path[k][n] = 0;
			n = 0;
		} else {
			n++;
		}
	}

	if (subcomp(path[1])) {
//		raceI.misc.release_name = malloc(l[0] + 18);
		sprintf(raceI.misc.release_name, "%s/%s", path[0], path[1]);
		locations.incomplete = c_incomplete(incomplete_cd_indicator, path);
		locations.nfo_incomplete = i_incomplete(incomplete_base_nfo_indicator, path);
		locations.in_cd_dir = 1;
		if (k < 2)
			free(path[1]);
		if (k == 0)
			free(path[0]);
	} else {
//		raceI.misc.release_name = malloc(l[1] + 10);
		sprintf(raceI.misc.release_name, "%s", path[1]);
		locations.incomplete = c_incomplete(incomplete_indicator, path);
		locations.nfo_incomplete = i_incomplete(incomplete_nfo_indicator, path);
		locations.in_cd_dir = 0;
		if (k < 2)
			free(path[1]);
		if (k == 0)
			free(path[0]);
	}
}

void 
remove_nfo_indicator(char *directory)
{
	int		cnt, l[2], n = 0, k = 2;
	char           *path[2];

	locations.length_path = strlen(locations.path);

	for (cnt = locations.length_path - 1; k && cnt; cnt--) {
		if (directory[cnt] == '/') {
			k--;
			l[k] = n;
			path[k] = malloc(n + 1);
			strncpy(path[k], directory + cnt + 1, n);
			path[k][n] = 0;
			n = 0;
		} else
			n++;
	}
	locations.nfo_incomplete = i_incomplete(incomplete_nfo_indicator, path);
	if (locations.nfo_incomplete)
		unlink(locations.nfo_incomplete);
	locations.nfo_incomplete = i_incomplete(incomplete_base_nfo_indicator, path);
	if (locations.nfo_incomplete)
		unlink(locations.nfo_incomplete);
	if (k < 2)
		free(path[1]);
	if (k == 0)
		free(path[0]);
}

unsigned char 
get_filetype(char *ext)
{
	if (!memcmp(ext, "sfv", 4))
		return 1;
	if (!clear_file_file(&locations, raceI.file.name))
		return 4;
	if (!memcmp(ext, "zip", 4))
		return 0;
	if (!memcmp(ext, "nfo", 4))
		return 2;
	if (!strcomp(ignored_types, ext) || !strcomp(allowed_types, ext))
		return 3;

	return 255;
}



int 
main(int argc, char **argv)
{
	char           *fileext;
	char           *name_p;
	char           *temp_p;
	char           *target;
	char           *fname;
	char           *env_user;
	char           *env_group;
	int		n;
	unsigned char	empty_dir = 0;
	unsigned char	incomplete = 0;

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
	env_user = getenv("USER");
	if (env_user == NULL) {
		d_log("Could not find environment variable 'USER', setting value to 'Nobody'\n");
		env_user = "Nobody";
	}
	d_log("Reading group name from env\n");
	env_group = getenv("GROUP");
	if (env_group == NULL) {
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
	rescandir();

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
	bzero(&raceI.total, sizeof(struct race_total));
	raceI.misc.slowest_user[0] = 30000;
	raceI.misc.fastest_user[0] = 0;

	/* YARR; THE PAIN OF MAGIC NUMBERS! */
	d_log("Copying env/predefined username to raceI. (%s)\n", env_user);
	strncpy(raceI.user.name, env_user, 24);
	raceI.user.name[23] = 0;
	d_log("Copying env/predefined groupname to raceI. (%s)\n", env_group);
	strncpy(raceI.user.group, env_group, 24);
	raceI.user.group[23] = 0;

	d_log("File to remove is: %s\n", fname);

	if (!*raceI.user.group)
		memcpy(raceI.user.group, "NoGroup", 8);

	d_log("Allocating memory for variables\n");
	userI = malloc(sizeof(struct USERINFO *) * 30);
	memset(userI, 0, sizeof(struct USERINFO *) * 30);
	groupI = malloc(sizeof(struct GROUPINFO *) * 30);
	memset(groupI, 0, sizeof(struct GROUPINFO *) * 30);

	locations.path = malloc(PATH_MAX);
	raceI.misc.release_name = malloc(PATH_MAX);
	getcwd(locations.path, PATH_MAX);

	if (matchpath(nocheck_dirs, locations.path) || (!matchpath(zip_dirs, locations.path) && !matchpath(sfv_dirs, locations.path) & !matchpath(group_dirs, locations.path))) {
		d_log("Dir matched with nocheck_dirs, or is not in the zip/sfv/group-dirs\n");
		d_log("Freeing memory, and exiting\n");
		free(userI);
		free(groupI);
		free(locations.path);
		return 0;
	}
	locations.race = malloc(n = strlen(locations.path) + 10 + sizeof(storage));
	locations.sfv = malloc(n);
	locations.leader = malloc(n);
	target = malloc(4096);

	d_log("Copying data locations into memory\n");
	raceI.file.name = fname;
	sprintf(locations.sfv, storage "/%s/sfvdata", locations.path);
	sprintf(locations.leader, storage "/%s/leader", locations.path);
	sprintf(locations.race, storage "/%s/racedata", locations.path);

	d_log("Caching release name\n");
	getrelname(locations.path);
	d_log("DEBUG 0: incomplete: '%s', path: '%s'\n", locations.incomplete, locations.path);

	d_log("Parsing file extension from filename...\n");

	for (temp_p = name_p = raceI.file.name; *name_p != 0; name_p++) {
		if (*name_p == '.')
			temp_p = name_p;
	}

	if (*temp_p != '.') {
		d_log("Got: no extension\n");
		temp_p = name_p;
	} else {
		d_log("Got: %s\n", temp_p);
		temp_p++;
	}
	name_p++;

	d_log("Copying lowercased version of extension to memory\n");
	fileext = malloc(name_p - temp_p);
	memcpy(fileext, temp_p, name_p - temp_p);
	strtolower(fileext);

	switch (get_filetype(fileext)) {
	case 0:
		d_log("File type is: ZIP\n");
		if ((raceI.misc.write_log = matchpath(zip_dirs, locations.path))) {
			raceI.misc.write_log = 1 - matchpath(group_dirs, locations.path);
		} else if (matchpath(sfv_dirs, locations.path)) {
			d_log("Directory matched with sfv_dirs\n");
			break;
		}
		if (!fileexists("file_id.diz")) {
			temp_p = findfileext(".zip");
			if (temp_p != NULL) {
				d_log("file_id.diz does not exist, trying to extract it from %s\n", temp_p);
				sprintf(target, "%s -qqjnCLL %s file_id.diz", unzip_bin, temp_p);
				execute(target);
				chmod("file_id.diz", 0666);
			}
		}
		d_log("Reading diskcount from diz\n");
		raceI.total.files = read_diz("file_id.diz");
		if (raceI.total.files == 0) {
			d_log("Could not get diskcount from diz\n");
			raceI.total.files = 1;
		}
		raceI.total.files_missing = raceI.total.files;

		d_log("Reading race data from file to memory\n");
		readrace_file(&locations, &raceI, userI, groupI);

		d_log("Caching progress bar\n");
		buffer_progress_bar(&raceI);

		d_log("Removing old complete bar, if any\n");
		removecomplete();
		if (raceI.total.files_missing < 0) {
			raceI.total.files -= raceI.total.files_missing;
			raceI.total.files_missing = 0;
		}
		if (!raceI.total.files_missing) {
			d_log("Creating complete bar\n");
			createstatusbar(convert(&raceI, userI, groupI, zip_completebar));
		} else if (raceI.total.files_missing < raceI.total.files) {
			if (raceI.total.files_missing == 1) {
				d_log("Writing INCOMPLETE to %s\n", log);
				writelog(convert(&raceI, userI, groupI, incompletemsg), general_incomplete_type);
			}
			incomplete = 1;
		} else {
			empty_dir = 1;
		}
		break;
	case 1:
		d_log("Reading file count from SFV\n");
		readsfv_file(&locations, &raceI, 0);

		if (fileexists(locations.race)) {
			d_log("Reading race data from file to memory\n");
			readrace_file(&locations, &raceI, userI, groupI);
		}
		d_log("Caching progress bar\n");
		buffer_progress_bar(&raceI);
		if (raceI.total.files_missing == raceI.total.files) {
			empty_dir = 1;
		}
		break;
	case 3:
		d_log("Removing old complete bar, if any\n");
		removecomplete();
		raceI.misc.write_log = matchpath(sfv_dirs, locations.path) > 0 ? 1 - matchpath(group_dirs, locations.path) : 0;

		if (fileexists(locations.race)) {
			d_log("Reading race data from file to memory\n");
			readrace_file(&locations, &raceI, userI, groupI);
		}
		if (fileexists(locations.sfv)) {
#if ( create_missing_files == TRUE )
#if ( sfv_cleanup == TRUE )
#if ( sfv_cleanup_lowercase == TRUE )
			strtolower(raceI.file.name);
#endif
#endif
			create_missing(raceI.file.name, name_p - raceI.file.name - 1);
#endif
			d_log("Reading file count from SFV\n");
			readsfv_file(&locations, &raceI, 0);

			d_log("Caching progress bar\n");
			buffer_progress_bar(&raceI);
		}
		if (raceI.total.files_missing < raceI.total.files) {
			if (raceI.total.files_missing == 1) {
				d_log("Writing INCOMPLETE to %s\n", log);
				writelog(convert(&raceI, userI, groupI, incompletemsg), general_incomplete_type);
			}
			incomplete = 1;
		} else {
			d_log("Removing old race data\n");
			unlink(locations.race);
			if (findfileext(".sfv") == NULL) {
				empty_dir = 1;
			} else {
				incomplete = 1;
			}
		}
		break;
	case 4:
//		break;
	case 255:
//		break;
	case 2:
		if (!fileexists(locations.race)) {
			empty_dir = 1;
		} else {
			d_log("Reading race data from file to memory\n");
			readrace_file(&locations, &raceI, userI, groupI);
			d_log("Caching progress bar\n");
			buffer_progress_bar(&raceI);
			if (raceI.total.files_missing == raceI.total.files) {
				empty_dir = 1;
			}
		}
		break;
	}

	if (empty_dir == 1) {
		d_log("Removing all files and directories created by zipscript\n");
		removecomplete();
		if (fileexists(locations.sfv))
			delete_sfv_file(&locations);
		if (locations.nfo_incomplete)
			unlink(locations.nfo_incomplete);
		if (locations.incomplete)
			unlink(locations.incomplete);
//		if (fileexists("file_id.diz"))
			unlink("file_id.diz");
//		if (fileexists(locations.sfv))
			unlink(locations.sfv);
//		if (fileexists(locations.race))
			unlink(locations.race);
//		if (fileexists(locations.leader))
			unlink(locations.leader);
		move_progress_bar(1, &raceI);
#if (remove_dot_files_on_delete == TRUE)
		removedotfiles();
#endif
	}
	if (incomplete == 1 && raceI.total.files > 0) {

		getrelname(locations.path);
		if (locations.nfo_incomplete) {
			if (findfileext(".nfo")) {
				d_log("Removing missing-nfo indicator (if any)\n");
				remove_nfo_indicator(locations.path);
			} else if (matchpath(check_for_missing_nfo_dirs, locations.path) && (!matchpath(group_dirs, locations.path) || create_incomplete_links_in_group_dirs)) {
				if (!locations.in_cd_dir) {
					d_log("Creating missing-nfo indicator %s.\n", locations.nfo_incomplete);
					create_incomplete_nfo();
				} else {
					rescanparent();
					if (findfileextparent(".nfo")) {
						d_log("Removing missing-nfo indicator (if any)\n");
						remove_nfo_indicator(locations.path);
					} else {
						d_log("Creating missing-nfo indicator (base) %s.\n", locations.nfo_incomplete);
						create_incomplete_nfo();
					}
				}
			}
		}
		if (!matchpath(group_dirs, locations.path) || create_incomplete_links_in_group_dirs) {
			d_log("Creating incomplete indicator\n");
			d_log("   incomplete: '%s', path: '%s'\n", locations.incomplete, locations.path);
			create_incomplete();
		}
		d_log("Moving progress bar\n");
		move_progress_bar(0, &raceI);
	}
	d_log("Releasing memory\n");
	free(fileext);
	free(target);
	free(raceI.misc.release_name);
	free(locations.path);
	free(locations.race);
	free(locations.sfv);
	free(locations.leader);
	free(userI);
	free(groupI);

	d_log("Exit\n");

	if ((empty_dir == 1) && (fileexists(".debug")) && (remove_dot_debug_on_delete == TRUE))
		unlink(".debug");

	return 0;
}
