#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include "objects.h"
#include "macros.h"
#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifndef PATH_MAX
 #define _LIMITS_H_
 #ifdef _SunOS_
  #include <syslimits.h>
 #else
  #include <sys/syslimits.h>
 #endif
#endif

#ifdef _SunOS_
#include "scandir.h"
#endif

struct tm      *timenow;
time_t		tnow;

/* new try without expensive scandir() */
void
scandirectory(char *dirname, int setfree)
{
	int		fd;
	DIR		*dir1, *dir2;
	struct dirent	*dp1, *dp2;
	
	printf("[%s]\n", dirname);

	if (chdir(dirname) != -1) {
		if ((dir1 = opendir("."))) {
			while ((dp1 = readdir(dir1))) {
				if (dp1->d_name[0] != '.') {
					chdir(dp1->d_name);
					if ((dir2 = opendir("."))) {
						while ((dp2 = readdir(dir2))) {
							if (dp1->d_name[0] != '.') {
								if ((fd = open(dp2->d_name, O_NDELAY, 0777)) != -1) {
									close(fd);
								} else if (setfree) {
									unlink(dp2->d_name);
									printf("Broken symbolic link \"%s\" removed.\n", dp2->d_name);
								}
							}
						}
					}
					closedir(dir2);
					chdir("..");
					if (setfree)
						rmdir(dp1->d_name);
				}
			}
		}
		closedir(dir1);
	}
}

char           *
replace_cookies(char *s)
{
	static char	new_string[PATH_MAX], *pos;

	pos = new_string;

	while (*s == '.' || *s == '/')
		s++;

	for (; *s; s++)
		switch (*s) {
		case '%':
			s++;
			switch (*s) {
			case '1':
				pos += sprintf(pos, ".*");
				break;
			case '0':
				pos += sprintf(pos, ".*");
				break;
			case '%':
				*pos++ = '%';
			}
			break;
		case '(':
		case ')':
		case '[':
		case ']':
			pos += sprintf(pos, "\\%c", *s);
			break;
		default:
			*pos++ = *s;
		}
	*pos = 0;

	return new_string;
}

void 
incomplete_cleanup(char *path, int setfree)
{
	DIR		*dir;
	struct dirent	*dp;
	
	struct stat	fileinfo;
	int		tempsize = 0;
	regex_t		preg   [4];
	regmatch_t	pmatch[1];
	char		temp[PATH_MAX];
	char           *locator;

	sprintf(temp, "%s", incomplete_cd_indicator);
	locator = replace_cookies(temp);
	regcomp(&preg[0], locator, REG_NEWLINE | REG_EXTENDED);

	sprintf(temp, "%s", incomplete_indicator);
	locator = replace_cookies(temp);
	regcomp(&preg[1], locator, REG_NEWLINE | REG_EXTENDED);

	sprintf(temp, "%s", incomplete_base_nfo_indicator);
	locator = replace_cookies(temp);
	regcomp(&preg[2], locator, REG_NEWLINE | REG_EXTENDED);

	sprintf(temp, "%s", incomplete_nfo_indicator);
	locator = replace_cookies(temp);
	regcomp(&preg[3], locator, REG_NEWLINE | REG_EXTENDED);

	printf("[%s]\n", path);

	if (chdir(path) != -1) {
		if ((dir = opendir("."))) {
			while ((dp = readdir(dir))) {

				/* Multi CD */
				if (regexec(&preg[0], dp->d_name, 1, pmatch, 0) == 0) {
					if (!(int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dp)) {
						tempsize = readlink(dp->d_name, temp, PATH_MAX);
						temp[tempsize] = '\0';
						if (stat(dp->d_name, &fileinfo)) {
							if (setfree) {
								unlink(dp->d_name);
								printf("Broken symbolic link \"%s\" removed.\n", temp);
							}
						} else
							printf("Incomplete release: \"%s%s\".\n", path, temp);
					}
				} else if (regexec(&preg[2], dp->d_name, 1, pmatch, 0) == 0) {
					if (!(int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dp)) {
						tempsize = readlink(dp->d_name, temp, PATH_MAX);
						temp[tempsize] = '\0';
						if (stat(dp->d_name, &fileinfo)) {
							if (setfree) {
								unlink(dp->d_name);
								printf("Broken symbolic link \"%s\" removed.\n", temp);
							}
						} else
							printf("Incomplete release: \"%s%s\".\n", path, temp);
					}
				/* Normal */
				} else	if (regexec(&preg[1], dp->d_name, 1, pmatch, 0) == 0) {
					if (!(int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dp)) {
						tempsize = readlink(dp->d_name, temp, PATH_MAX);
						temp[tempsize] = '\0';
						if (stat(dp->d_name, &fileinfo)) {
							if (setfree) {
								unlink(dp->d_name);
								printf("Broken symbolic link \"%s\" removed.\n", temp);
							}
						} else
							printf("Incomplete release: \"%s%s\".\n", path, temp);
					}
				} else if (regexec(&preg[3], dp->d_name, 1, pmatch, 0) == 0) {
					if (!(int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dp)) {
						tempsize = readlink(dp->d_name, temp, PATH_MAX);
						temp[tempsize] = '\0';
						if (stat(dp->d_name, &fileinfo)) {
							if (setfree) {
								unlink(dp->d_name);
								printf("Broken symbolic link \"%s\" removed.\n", temp);
							}
						} else
							printf("Incomplete release: \"%s%s\".\n", path, temp);
					}
				}
			}
			closedir(dir);
		} else {
			fprintf(stderr, "opendir(%s): %s\n", path, strerror(errno));
		}
	} else {
		fprintf(stderr, "chdir(%s): %s\n", path, strerror(errno));
	}
	regfree(&preg[0]);
	regfree(&preg[1]);
	regfree(&preg[2]);
	regfree(&preg[3]);
}

void 
cleanup(char *pathlist, int setfree, char *startpath)
{
	char		data_today[PATH_MAX],
			data_yesterday[PATH_MAX],
			path[PATH_MAX];
	char	       *newentry, *entry;

	struct tm      *time_today, *time_yesterday;
	time_t		t_today, t_yesterday;

	if ((time_today = malloc(sizeof(struct tm))) == NULL) {
			printf("Error! Failed to allocate enough memory!\n");
			exit(1);
		}
	if ((time_yesterday = malloc(sizeof(struct tm))) == NULL) {
			printf("Error! Failed to allocate enough memory!\n");
			exit(1);
		}


	t_today = time(NULL);
	time_today = localtime_r(&t_today, time_today);

	t_yesterday = time(NULL) - (60 * 60 * 24);	/* 86400 seconds back ==
							 * 1 day */
	time_yesterday = localtime_r(&t_yesterday, time_yesterday);

	newentry = pathlist;

	if ((strlen(startpath) > 1) && (setfree == 1)) {
		/* Scanning current dir only */
		incomplete_cleanup(startpath, setfree);
	} else {
		while (1) {
			for (entry = newentry; *newentry != ' ' && *newentry != 0; newentry++);

			sprintf(path, "%s%.*s", startpath, (int)(newentry - entry), entry);
			strftime(data_today, PATH_MAX, path, time_today);
			strftime(data_yesterday, PATH_MAX, path, time_yesterday);

			if (strcmp(data_today, data_yesterday)) {
				if (check_yesterday == TRUE)
					incomplete_cleanup(data_yesterday, setfree);
				if (check_today == TRUE)
					incomplete_cleanup(data_today, setfree);
			} else
				incomplete_cleanup(data_today, setfree);
			if (!*newentry)
				break;

			newentry++;
		}
	}

	free(time_today);
	free(time_yesterday);
}

int 
main(int argc, char **argv)
{

	int		setfree = 1;
	char		startdir[PATH_MAX] = "/";

	if (argc > 1) {
		if (!strncmp(argv[1], "/", 1)) {
			setfree = 0;
			printf("%s: Running script in view mode only.\n", argv[0]);
			sprintf(startdir, argv[1]);
		} else {
			if (getcwd(startdir, PATH_MAX) == NULL) {
				printf("PZS-NG Cleanup: ERROR - Failed to getcwd.\n");
				exit (0);
			}
			printf("PZS-NG Cleanup: OK.\n");
		}
	}

	cleanup(cleanupdirs, setfree, startdir);

	if (argc < 2 || always_scan_audio_syms == TRUE) {

#if ( audio_genre_sort == TRUE )
		scandirectory((char *)audio_genre_path, setfree);
#endif

#if ( audio_year_sort == TRUE )
		scandirectory((char *)audio_year_path, setfree);
#endif

#if ( audio_artist_sort == TRUE )
		scandirectory((char *)audio_artist_path, setfree);
#endif

#if ( audio_group_sort == TRUE )
		scandirectory((char *)audio_group_path, setfree);
#endif

		printf("Finished successfully.\n");
	}
	exit(EXIT_SUCCESS);
}

