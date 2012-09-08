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
#include <libgen.h>

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

	int		setfree = 1;
	char		startdir[PATH_MAX];

	if (argc == 2 && !strncmp(argv[1], "/", 1)) {
		setfree = 0;
		snprintf(startdir, PATH_MAX, "%s", argv[1]);
		printf("PZS-NG Cleanup: Running script in view mode only.\n");
		if (chroot(startdir) == -1) {
			printf("PZS-NG Cleanup: ERROR - Failed to chroot: %s.\n", strerror(errno));
			exit(0);
		}
		startdir[0] = '\0';
	} else {
		if (getcwd(startdir, PATH_MAX) == NULL) {
			printf("PZS-NG Cleanup: ERROR - Failed to getcwd: %s.\n", strerror(errno));
			exit(0);
		} else if (!strcmp(startdir, "/"))
			startdir[0] = '\0';
		printf("PZS-NG Cleanup: Running.\n");
	}

	cleanup(cleanupdirs, cleanupdirs_dated, setfree, startdir);

	if (((int)strlen(startdir) < 1 && argc < 2) || always_scan_audio_syms == TRUE) {

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

#if ( audio_language_sort == TRUE )
		scandirectory((char *)audio_language_path, setfree);
#endif

		printf("Finished successfully.\n");
	}
	exit(EXIT_SUCCESS);
}

/* new try without expensive scandir() */
void
scandirectory(char *dname, int setfree)
{
	int		fd;
	DIR		*dir1, *dir2;
	struct dirent	*dp1, *dp2;
	
	printf("[%s]\n", dname);

	if (chdir(dname) != -1) {
		if ((dir1 = opendir("."))) {
			while ((dp1 = readdir(dir1))) {
				if (dp1->d_name[0] != '.') {
					if (chdir(dp1->d_name) == -1) {
						printf("Failed to chdir(): %s\n", strerror(errno));
					}
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
					if (chdir("..") == -1) {
						printf("Failed to chdir(): %s\n", strerror(errno));
					}
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

int
regcomp_error(int regcomp_return, regex_t *preg, char *expression)
{
        char *regex_error;
        int size;
        if (regcomp_return != 0)
        {
            size = regerror(regcomp_return, preg, NULL, 0);
            regex_error = malloc(size);
            regerror(regcomp_return, preg, regex_error, size);
	    fprintf(stderr, "regcomp(.., %s, ..): %s\n", expression, regex_error);
            free(regex_error);
            return 1;
        }

        return 0;
}

void 
incomplete_cleanup(char *path, int setfree)
{
	DIR		*dir;
	struct dirent	*dp;
	struct stat	fileinfo;
	
	int		i, fd, size;
	regex_t		preg[8];
	regmatch_t	pmatch[1];
	char		incarr[8][PATH_MAX];
	char		*locator;
	static char	tempa[PATH_MAX],
			tempb[PATH_MAX];
        char            fulldir[PATH_MAX];

	printf("[%s]\n", path);

	if (chdir(path) != -1) {
		if ((dir = opendir("."))) {
			while ((dp = readdir(dir))) {
				snprintf(tempa, PATH_MAX, "%s/%s", path, dp->d_name);
				if (lstat(tempa, &fileinfo) != -1 && S_ISLNK(fileinfo.st_mode)) {
#if (debug_mode && debug_announce)
					printf("DEBUG: dp->d_name='%s'\n", dp->d_name);
#endif
					if ((fd = open(dp->d_name, O_NDELAY)) != -1) {
						close(fd);
	       					if ((size = readlink(tempa, tempb, PATH_MAX)) < 0) continue;
					        tempb[size] = '\0';
						if (chdir(tempb) == -1)
							fprintf(stderr, "chdir(%s): %s\n", tempb, strerror(errno));
						if (getcwd(fulldir, PATH_MAX) == NULL)
							fprintf(stderr, "getcwd(%s): %s\n", fulldir, strerror(errno));
						if (chdir(path) == -1)
							fprintf(stderr, "chdir(%s): %s\n", path, strerror(errno));
						if (matchpath(incomplete_generic1_path, fulldir)) {
							snprintf(incarr[0], PATH_MAX, "%s", incomplete_generic1_cd_indicator);
							snprintf(incarr[1], PATH_MAX, "%s", incomplete_generic1_indicator);
							snprintf(incarr[2], PATH_MAX, "%s", incomplete_generic1_base_nfo_indicator);
							snprintf(incarr[3], PATH_MAX, "%s", incomplete_generic1_nfo_indicator);
							snprintf(incarr[4], PATH_MAX, "%s", incomplete_generic1_base_sfv_indicator);
							snprintf(incarr[5], PATH_MAX, "%s", incomplete_generic1_sfv_indicator);
							snprintf(incarr[6], PATH_MAX, "%s", incomplete_generic1_base_sample_indicator);
							snprintf(incarr[7], PATH_MAX, "%s", incomplete_generic1_sample_indicator);
#if (debug_mode && debug_announce)
							printf("DEBUG: Matchpath hit for generic1: '%s'\n", incarr[1]);
#endif
						} else if (matchpath(incomplete_generic2_path, fulldir)) {
							snprintf(incarr[0], PATH_MAX, "%s", incomplete_generic2_cd_indicator);
							snprintf(incarr[1], PATH_MAX, "%s", incomplete_generic2_indicator);
							snprintf(incarr[2], PATH_MAX, "%s", incomplete_generic2_base_nfo_indicator);
							snprintf(incarr[3], PATH_MAX, "%s", incomplete_generic2_nfo_indicator);
							snprintf(incarr[4], PATH_MAX, "%s", incomplete_generic2_base_sfv_indicator);
							snprintf(incarr[5], PATH_MAX, "%s", incomplete_generic2_sfv_indicator);
							snprintf(incarr[6], PATH_MAX, "%s", incomplete_generic2_base_sample_indicator);
							snprintf(incarr[7], PATH_MAX, "%s", incomplete_generic2_sample_indicator);
#if (debug_mode && debug_announce)
							printf("DEBUG: Matchpath hit for generic2: '%s'\n", incarr[1]);
#endif
						} else {
							snprintf(incarr[0], PATH_MAX, "%s", incomplete_cd_indicator);
							snprintf(incarr[1], PATH_MAX, "%s", incomplete_indicator);
							snprintf(incarr[2], PATH_MAX, "%s", incomplete_base_nfo_indicator);
							snprintf(incarr[3], PATH_MAX, "%s", incomplete_nfo_indicator);
							snprintf(incarr[4], PATH_MAX, "%s", incomplete_base_sfv_indicator);
							snprintf(incarr[5], PATH_MAX, "%s", incomplete_sfv_indicator);
							snprintf(incarr[6], PATH_MAX, "%s", incomplete_base_sample_indicator);
							snprintf(incarr[7], PATH_MAX, "%s", incomplete_sample_indicator);
#if (debug_mode && debug_announce)
							printf("DEBUG: No matchpath hit: '%s'\n", incarr[1]);
#endif
						}

						for (i = 0; i < (int)(sizeof(incarr) / sizeof(incarr[i])); i++) {
							locator = basename(replace_cookies(incarr[i]));
							if (locator && !regcomp_error(regcomp(&preg[i], locator, REG_NEWLINE | REG_EXTENDED), &preg[i], locator)) {
#if (debug_mode && debug_announce)
								printf("DEBUG: locator for preg[%i]='%s'\n", i, locator);
#endif
								if (regexec(&preg[i], dp->d_name, 1, pmatch, 0) == 0)
									if (!(int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dp))
										if (checklink(dp->d_name, setfree)) {
											regfree(&preg[i]);
											break;
										}
								regfree(&preg[i]);
								continue;
							}
							regfree(&preg[i]);
							return;
						}
					} else if (setfree) {
						unlink(dp->d_name);
						printf("Broken symbolic link \"%s\" removed.\n", dp->d_name);
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

}

int 
checklink(char *link_, int setfree)
{
	int		size;
	static char	temp[PATH_MAX];
        char            fulldir[PATH_MAX], origdir[PATH_MAX];

	if ((size = readlink(link_, temp, PATH_MAX)) < 0) return 0;
	temp[size] = '\0';
	if (getcwd(origdir, PATH_MAX) == NULL)
		fprintf(stderr, "getcwd(%s): %s\n", origdir, strerror(errno));
        if ((chdir(temp) == -1) || (getcwd(fulldir, PATH_MAX) == NULL)) {
		if (setfree) {
			unlink(link_);
			printf("Broken symbolic link \"%s\" removed.\n", link_);
		}
	} else {
		printf("Incomplete release: \"%s\".\n", fulldir);
	}
	if (chdir(origdir) == -1)
		fprintf(stderr, "chdir(%s): %s\n", origdir, strerror(errno));
	return 1;
}

short int
matchpath(char *instr, char *path)
{
        int             pos = 0;
        if ( (int)strlen(instr) < 2 || (int)strlen(path) < 2 ) {
#if (debug_mode && debug_announce)
		printf("DEBUG: matchpath: pathlength(s) too short - returning nomatch (not an error)\n");
#endif
                return 0;
        }
        do {
                switch (*instr) {
                case 0:
                case ' ':
			if (!strncmp(instr - pos, path, pos)) {
				if (*(instr - 1) == '/')
					return 1;
				if (*(path + pos) == '/')
			        	return 1;
			}
                        pos = 0;
                        break;
                default:
                        pos++;
                        break;
                }
        } while (*instr++);
        return 0;
}

void 
cleanup(char *pathlist, char *pathlist_dated, int setfree, char *startpath)
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

	if (((int)strlen(startpath) > 1) && (setfree == 1)) {
		printf("Scanning current dir only\n");

		incomplete_cleanup(startpath, setfree);
	} else {
		newentry = pathlist;
		while (*newentry) {
			for (entry = newentry; *newentry != ' ' && *newentry; newentry++);
			sprintf(path, "%s%.*s", startpath, (int)(newentry - entry), entry);
			incomplete_cleanup(path, setfree);
			if (!*newentry)
				break;
			newentry++;
		}
		while (pathlist_dated && day_back <= (days_back_cleanup - 1)) {
			newentry = pathlist_dated;
			t_day = time(NULL) - (60 * 60 * 24 * day_back);	/* 86400 seconds back == * 1 day */
			time_day = localtime_r(&t_day, time_day);
			while (*newentry) {
				for (entry = newentry; *newentry != ' ' && *newentry; newentry++);
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
	if (time_day)
		free(time_day);
}
