#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "zsfunctions.h"
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
		glfile = fopen(log, "a+");

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

void 
remove_nfo_indicator(char *directory)
{
	int		cnt       , l[2], n = 0, k = 2;
	char           *path[2];

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
	if (fileexists(locations.nfo_incomplete))
		unlink(locations.nfo_incomplete);
	locations.nfo_incomplete = i_incomplete(incomplete_base_nfo_indicator, path);
	if (fileexists(locations.nfo_incomplete))
		unlink(locations.nfo_incomplete);
	if (k < 2)
		free(path[1]);
	if (k == 0)
		free(path[0]);
}

void 
getrelname(char *directory)
{
	int		cnt       , l[2], n = 0, k = 2;
	char           *path[2];

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

	if (subcomp(path[1])) {
		raceI.misc.release_name = malloc(l[0] + 18);
		locations.link_source = malloc(n = (locations.length_path - l[1]));
		sprintf(raceI.misc.release_name, "%s/%s", path[0], path[1]);
		sprintf(locations.link_source, "%.*s", n - 1, locations.path);
		locations.link_target = path[0];
		locations.incomplete = c_incomplete(incomplete_cd_indicator, path);
		locations.nfo_incomplete = i_incomplete(incomplete_base_nfo_indicator, path);
		locations.in_cd_dir = 1;
	} else {
		raceI.misc.release_name = malloc(l[1] + 10);
		locations.link_source = malloc(locations.length_path + 1);
		strcpy(locations.link_source, locations.path);
		sprintf(raceI.misc.release_name, "%s", path[1]);
		locations.link_target = path[1];
		locations.incomplete = c_incomplete(incomplete_indicator, path);
		locations.nfo_incomplete = i_incomplete(incomplete_nfo_indicator, path);
		locations.in_cd_dir = 0;
	}
	if (k < 2)
		free(path[1]);
	if (k == 0)
		free(path[0]);
}

int 
main()
{
	int		n         , m, l, complete_type = 0;
	char           *ext, exec[4096], *complete_bar = 0;
	unsigned int	crc;
	struct stat	fileinfo;

	uid_t		f_uid;
	gid_t		f_gid;
	double		temp_time = 0;

#if ( program_uid > 0 )
	setegid(program_gid);
	seteuid(program_uid);
#endif

	umask(0666 & 000);

	d_log("PZS-NG (rescan) v%s debug log.\n", ng_version());
	d_log("Allocating memory for variables\n");
	userI = malloc(sizeof(struct USERINFO *) * 30);
	memset(userI, 0, sizeof(struct USERINFO *) * 30);
	groupI = malloc(sizeof(struct GROUPINFO *) * 30);
	memset(groupI, 0, sizeof(struct GROUPINFO *) * 30);

	locations.path = malloc(PATH_MAX);
	getcwd(locations.path, PATH_MAX);

	if ((matchpath(nocheck_dirs, locations.path) || (!matchpath(zip_dirs, locations.path) && !matchpath(sfv_dirs, locations.path) && !matchpath(group_dirs, locations.path))) && rescan_nocheck_dirs_allowed == FALSE) {
		d_log("Dir matched with nocheck_dirs, or is not in the zip/sfv/group-dirs\n");
		d_log("Freeing memory, and exiting\n");
		free(userI);
		free(groupI);
		free(locations.path);
		return 0;
	}
	raceI.misc.slowest_user[0] = 30000;

	bzero(&raceI.total, sizeof(struct race_total));
	raceI.misc.fastest_user[0] = 0;
	raceI.misc.release_type = RTYPE_NULL;

	locations.race = malloc(n = strlen(locations.path) + 10 + sizeof(storage));
	locations.sfv = malloc(n);
	locations.leader = malloc(n);
	locations.length_path = strlen(locations.path);
	locations.length_zipdatadir = sizeof(storage);

	getrelname(locations.path);
	buffer_groups(GROUPFILE);
	buffer_users(PASSWDFILE);

	sprintf(locations.sfv, storage "/%s/sfvdata", locations.path);
	sprintf(locations.leader, storage "/%s/leader", locations.path);
	sprintf(locations.race, storage "/%s/racedata", locations.path);

	rescandir();
	move_progress_bar(1, &raceI);
	if (locations.incomplete)
		unlink(locations.incomplete);
	removecomplete();
	if (locations.race)
		unlink(locations.race);
	if (locations.sfv)
		unlink(locations.sfv);
	printf("Rescanning files...\n");
	rescandir();		/* Rescan dir after deleting files.. */

	if ((raceI.file.name = findfileext(".sfv")) != NULL) {
		maketempdir(&locations);
		stat(raceI.file.name, &fileinfo);
		if (copysfv_file(raceI.file.name, locations.sfv, fileinfo.st_size)) {
			printf("Found invalid entries in SFV - Exiting.\n");

			rescandir();
			n = direntries;
			while (n--) {
				m = l = strlen(dirlist[n]->d_name);
				ext = dirlist[n]->d_name;
				while (ext[m] != '-' && m > 0)
					m--;
				if (ext[m] != '-')
					m = l;
				else
					m++;
				ext += m;
				if (!strncmp(ext, "missing", 7))
					unlink(dirlist[n]->d_name);
			}

			d_log("Freeing memory, and exiting\n");
			unlink(locations.sfv);
			unlink(locations.race);
//			unlink(raceI.file.name);
			free(userI);
			free(groupI);
			free(locations.path);
			free(locations.race);
			free(locations.sfv);
			free(locations.leader);
			return 0;
		}
		n = direntries;
		raceI.total.start_time = 0;
		while (n--) {
			m = l = strlen(dirlist[n]->d_name);
			ext = dirlist[n]->d_name;
			while (ext[m] != '.' && m > 0)
				m--;
			if (ext[m] != '.')
				m = l;
			else
				m++;
			ext += m;
			if (!strcomp(ignored_types, ext) && (!(strcomp(allowed_types, ext) && !matchpath(allowed_types_exemption_dirs, locations.path))) && strcasecmp("sfv", ext) && strcasecmp("nfo", ext) && strcasecmp("bad", ext) && strcmp(dirlist[n]->d_name + l - 8, "-missing") && strncmp(dirlist[n]->d_name, ".", 1)) {
				stat(dirlist[n]->d_name, &fileinfo);
				f_uid = fileinfo.st_uid;
				f_gid = fileinfo.st_gid;

				strcpy(raceI.user.name, get_u_name(f_uid));
				strcpy(raceI.user.group, get_g_name(f_gid));
				raceI.file.name = dirlist[n]->d_name;
				raceI.file.speed = 2004 * 1024;
				raceI.file.size = fileinfo.st_size;

				temp_time = fileinfo.st_mtime;
				if (raceI.total.start_time == 0)
					raceI.total.start_time = temp_time;
				else
					raceI.total.start_time = (raceI.total.start_time < temp_time ? raceI.total.start_time : temp_time);
				raceI.total.stop_time = (temp_time > raceI.total.stop_time ? temp_time : raceI.total.stop_time);

				/* Hide users in group_dirs */
				if (matchpath(group_dirs, locations.path) && (hide_group_uploaders == TRUE)) {
					d_log("Hiding user in group-dir:\n");
					if (strlen(hide_gname) > 0) {
						snprintf(raceI.user.group, 18, "%s", hide_gname);
						d_log("   Changing groupname\n");
					}
					if (strlen(hide_uname) > 0) {
						snprintf(raceI.user.name, 18, "%s", hide_uname);
						d_log("   Changing username\n");
					}
					if (strlen(hide_uname) == 0) {
						d_log("   Making username = groupname\n");
						snprintf(raceI.user.name, 18, "%s", raceI.user.group);
					}
				}

				if (raceI.file.name)
					unlink_missing(raceI.file.name);
				if (l > 44) {
					printf("\nFile: %s", dirlist[n]->d_name + l - 44);
				} else {
					printf("\nFile: %-44s", dirlist[n]->d_name);
				}
				fflush(stdout);
				crc = calc_crc32(dirlist[n]->d_name);
				writerace_file(&locations, &raceI, crc, F_NOTCHECKED);
			}
		}
		printf("\n");
		testfiles_file(&locations, &raceI, 1);
		printf("\n");
//
		temprescandir( 0 );	/* We need to rescan again */
//
		readsfv_file(&locations, &raceI, 0);
		readrace_file(&locations, &raceI, userI, groupI);
		sortstats(&raceI, userI, groupI);
		buffer_progress_bar(&raceI);

		if (locations.nfo_incomplete) {
			if (findfileext(".nfo")) {
				d_log("Removing missing-nfo indicator (if any)\n");
				remove_nfo_indicator(locations.path);
			} else if (matchpath(check_for_missing_nfo_dirs, locations.path)) {
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
		if (raceI.misc.release_type == RTYPE_AUDIO)
			get_mpeg_audio_info(findfileext(".mp3"), &raceI.audio);

		if ((raceI.total.files_missing == 0) & (raceI.total.files > 0)) {
			switch (raceI.misc.release_type) {
			case RTYPE_RAR:
				complete_bar = rar_completebar;
				break;
			case RTYPE_OTHER:
				complete_bar = other_completebar;
				break;
			case RTYPE_AUDIO:
				complete_bar = audio_completebar;
#if ( enabled_create_m3u )
				n = sprintf(exec, findfileext(".sfv"));
				strcpy(exec + n - 3, "m3u");
				create_indexfile_file(&locations, &raceI, exec);
#endif
				break;
			case RTYPE_VIDEO:
				complete_bar = video_completebar;
				break;
			}
			complete(&locations, &raceI, userI, groupI, complete_type);
			createstatusbar(convert(&raceI, userI, groupI, complete_bar));
		} else {
			create_incomplete();
			move_progress_bar(0, &raceI);
		}
	} else if ((raceI.file.name = findfileext(".zip")) != NULL) {
		maketempdir(&locations);
		stat(raceI.file.name, &fileinfo);
		n = direntries;
		crc = 0;
		while (n--) {
			m = l = strlen(dirlist[n]->d_name);
			ext = dirlist[n]->d_name;
			while (ext[m] != '.' && m > 0)
				m--;
			if (ext[m] != '.')
				m = l;
			else
				m++;
			ext += m;
			if (!strcasecmp(ext, "zip")) {
				stat(dirlist[n]->d_name, &fileinfo);
				f_uid = fileinfo.st_uid;
				f_gid = fileinfo.st_gid;

				strcpy(raceI.user.name, get_u_name(f_uid));
				strcpy(raceI.user.group, get_g_name(f_gid));
				raceI.file.name = dirlist[n]->d_name;
				raceI.file.speed = 2004 * 1024;
				raceI.file.size = fileinfo.st_size;
				raceI.total.start_time = 0;

				if (!fileexists("file_id.diz")) {
					sprintf(exec, "%s -qqjnCLL %s file_id.diz", unzip_bin, raceI.file.name);
					execute(exec);
					chmod("file_id.diz", 0666);
				}
				sprintf(exec, "%s -qqt %s &> /dev/null", unzip_bin, raceI.file.name);
				if (system(exec) == 0) {
					writerace_file(&locations, &raceI, crc, F_CHECKED);
				} else {
					writerace_file(&locations, &raceI, crc, F_BAD);
					if (raceI.file.name)
						unlink(raceI.file.name);
				}
			}
		}
		raceI.total.files = read_diz("file_id.diz");
		if (!raceI.total.files) {
			raceI.total.files = 1;
			unlink("file_id.diz");
		}
		raceI.total.files_missing = raceI.total.files;
		readrace_file(&locations, &raceI, userI, groupI);
		sortstats(&raceI, userI, groupI);
		if (raceI.total.files_missing < 0) {
			raceI.total.files -= raceI.total.files_missing;
			raceI.total.files_missing = 0;
		}
		buffer_progress_bar(&raceI);
		if (raceI.total.files_missing == 0) {
			complete(&locations, &raceI, userI, groupI, complete_type);
			createstatusbar(convert(&raceI, userI, groupI, zip_completebar));
		} else {
			create_incomplete();
			move_progress_bar(0, &raceI);
		}
		if (locations.nfo_incomplete) {
			if (findfileext(".nfo")) {
				d_log("Removing missing-nfo indicator (if any)\n");
				remove_nfo_indicator(locations.path);
			} else if (matchpath(check_for_missing_nfo_dirs, locations.path)) {
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
	}
	printf(" Passed : %i\n", (int)raceI.total.files - (int)raceI.total.files_missing);
	printf(" Failed : %i\n", (int)raceI.total.files_bad);
	printf(" Missing: %i\n", (int)raceI.total.files_missing);
	printf("  Total : %i\n", (int)raceI.total.files);

	d_log("Freeing memory.\n");
	temprescandir( 1 );	/* We need to rescan again */
	free(locations.path);
	free(locations.race);
	free(locations.sfv);
	free(locations.leader);
	free(userI);
	free(groupI);

	exit(0);
}
