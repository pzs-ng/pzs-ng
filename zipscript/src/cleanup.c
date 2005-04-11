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

#include "cleanup.h"

struct tm      *timenow;
time_t		tnow;

int 
main(int argc, char **argv)
{

	int		setfree = 1, days = 0;
	char		startdir[PATH_MAX] = "/", days_char[4];

	if (argc > 1) {
		if (!strncmp(argv[1], "/", 1)) {
			setfree = 0;
			sprintf(startdir, argv[1]);
		} else if (!strncmp(argv[1], "--days=", 7)) {
			bzero(days_char, 4);
			memcpy(days_char, argv[1] + 7, 3);
			days=atol(days_char);
			printf("PZS-NG Cleanup: days to cleanup: %i\n", days);
		} else {
			if (getcwd(startdir, PATH_MAX) == NULL) {
				printf("PZS-NG Cleanup: ERROR - Failed to getcwd.\n");
				exit (0);
			}
			printf("PZS-NG Cleanup: Running.\n");
		}
	}

	if (!days && !setfree)
		printf("PZS-NG Cleanup: Running script in view mode only.\n");

	cleanup(cleanupdirs, cleanupdirs_dated, setfree, startdir, days);

	if (argc < 2 || always_scan_audio_syms == TRUE || days) {

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
				break;
			case '\0':
				return new_string;
			}
			break;
		case '(':
		case ')':
		case '[':
		case ']':
			pos += sprintf(pos, "\\%c", *s);
			break;
		case '\0':
			return new_string;
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
	
	int		i;
	regex_t		preg[4];
	regmatch_t	pmatch[1];
	char		temp[PATH_MAX];
	char		*locator;

	snprintf(temp, PATH_MAX, "%s", incomplete_cd_indicator);
	locator = replace_cookies(temp);
	regcomp(&preg[0], locator, REG_NEWLINE | REG_EXTENDED);

	snprintf(temp, PATH_MAX, "%s", incomplete_indicator);
	locator = replace_cookies(temp);
	regcomp(&preg[1], locator, REG_NEWLINE | REG_EXTENDED);

	snprintf(temp, PATH_MAX, "%s", incomplete_base_nfo_indicator);
	locator = replace_cookies(temp);
	regcomp(&preg[2], locator, REG_NEWLINE | REG_EXTENDED);

	snprintf(temp, PATH_MAX, "%s", incomplete_nfo_indicator);
	locator = replace_cookies(temp);
	regcomp(&preg[3], locator, REG_NEWLINE | REG_EXTENDED);

	printf("[%s]\n", path);

	if (chdir(path) != -1) {

		if ((dir = opendir("."))) {
		
			while ((dp = readdir(dir)))
				for (i = 0; i < 4; i++)
					if (regexec(&preg[i], dp->d_name, 1, pmatch, 0) == 0)
						if (!(int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dp))
							if (checklink(path, dp->d_name, setfree))
								break;

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

int 
checklink(char *path, char *link_, int setfree)
{
	int		size, retval;
	static char	temp[PATH_MAX];
	struct stat	fileinfo;
					
	size = readlink(link_, temp, PATH_MAX);
	temp[size] = '\0';
	retval = 0;
	if (stat(link_, &fileinfo)) {
		if (setfree) {
			unlink(link_);
			printf("Broken symbolic link \"%s\" removed.\n", temp);
			retval = 1;
		}
	} else {
		printf("Incomplete release: \"%s%s\".\n", path, temp);
		retval = 1;
	}
	return retval;
}

void 
cleanup(char *pathlist, char *pathlist_dated, int setfree, char *startpath, int days)
{
	char		data_day[PATH_MAX],
			path[PATH_MAX];
	char	       *newentry, *entry;

	struct tm      *time_day;
	time_t		t_day;
	int		day_back = 0;

	if ((time_day = malloc(sizeof(struct tm))) == NULL) {
			printf("Error! Failed to allocate enough memory!\n");
			exit(1);
		}


	if (((int)strlen(startpath) > 1) && (setfree == 1) && !days) {
		/* Scanning current dir only */
		incomplete_cleanup(startpath, setfree);
	} else {
		newentry = pathlist;
		while (1) {
			for (entry = newentry; *newentry != ' ' && *newentry != 0; newentry++);
			sprintf(path, "%s%.*s", startpath, (int)(newentry - entry), entry);
			incomplete_cleanup(path, setfree);
			if (!*newentry)
				break;
			newentry++;
		}
		if (!days)
			days = days_back_cleanup;
		while (day_back <= (days - 1)) {
			newentry = pathlist_dated;
			t_day = time(NULL) - (60 * 60 * 24 * day_back);	/* 86400 seconds back == * 1 day */
			time_day = localtime_r(&t_day, time_day);
			while (1) {
				for (entry = newentry; *newentry != ' ' && *newentry != 0; newentry++);
				sprintf(path, "%s%.*s", startpath, (int)(newentry - entry), entry);
				strftime(data_day, PATH_MAX, path, time_day);
				incomplete_cleanup(data_day, setfree);
				if (!*newentry)
					break;
				newentry++;
			}
			day_back++;
		}
	}

	free(time_day);
}

