#include <errno.h>
#include <fnmatch.h>
#include "zsfunctions.h"

#include "ng-version.h"

#include "abs2rel.h"

#ifdef _WITH_SS5
#include "constants.ss5.h"
#else
#include "constants.h"
#endif

#include "convert.h"
#include "race-file.h"
#include "crc.h"

#include <strl/strl.h>
#include <stdarg.h>

int		num_groups = 0, num_users = 0;

#ifdef USING_GLFTPD
        /* Only under glftpd do we have a uid/gid-lookup, so these
         * are only needed there. */
struct USER   **user;
struct GROUP  **group;
#endif

/*
 * d_log - create/put comments in a .debug file
 * Last revised by: js
 *        Revision: r1217
 */
void 
d_log(char *fmt,...)
{
#if ( debug_mode == TRUE )
	time_t		timenow;
	FILE           *file;
	va_list		ap;
#if ( debug_altlog == TRUE )
	static char	debugpath[PATH_MAX];
	static char	debugname[PATH_MAX];
#else
	static char	debugname[] = ".debug";
#endif
#endif

	if (fmt == NULL)
		return;
#if ( debug_mode == TRUE )
	va_start(ap, fmt);
	timenow = time(NULL);

#if ( debug_altlog == TRUE )
	getcwd(debugpath, PATH_MAX);
	snprintf(debugname, PATH_MAX, "%s/%s/debug",
	         storage, debugpath);
#endif

	if ((file = fopen(debugname, "a+"))) {
		fprintf(file, "%.24s - %.6d - %s - ", ctime(&timenow), getpid(), ng_version);
		vfprintf(file, fmt, ap);
		fclose(file);
#if ( debug_announce == TRUE)
	} else {
		printf("DEBUG: ");
		vprintf(fmt, ap);
	}
#else
	}
#endif
	chmod(debugname, 0666);
	va_end(ap);
#endif
	return;
}

/*
 * create_missing - create a <filename>-missing 0byte file.
 * Last revised by: js
 *        Revision: 1218
 */
void 
create_missing(char *f)
{
	char		fname[NAME_MAX];

	snprintf(fname, NAME_MAX, "%s-missing", f);
	createzerofile(fname);
}

/*
 * findfilext - find a filename with a matching extension in current dir.
 * Last Modified by: d1
 *         Revision: r1 (2002.01.16)
 */
char *
findfileext(DIR *dir, char *fileext)
{
	int			k;
	static struct dirent	*dp;

	errno = 0;
	rewinddir(dir);
	while ((dp = readdir(dir))) {
		if ((k = NAMLEN(dp)) < 4)
			continue;
		if (strcasecmp(dp->d_name + k - 4, fileext) == 0) {
			return dp->d_name;
		}
	}

	if (errno)
		d_log("zsfunctions.c: findfileext() - readdir(dir) returned an error: %s\n", strerror(errno));

	return NULL;
}

char *
findfileextsub(DIR *dir)
{
	DIR *dir2, *dir3;
	int			k;
	char cwd[1024], cwd2[1024];

	static struct dirent	*dp, *dp2;

        errno = 0;

	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		d_log("zsfunctions.c: findfileextsub() - Error getting path: %s\n", strerror(errno));
		return NULL;
	}
	rewinddir(dir);
	if((dir2=opendir(cwd)) == NULL) {
		d_log("zsfunctions.c: findfileextsub() - Error getting path: %s\n", strerror(errno));
		return NULL;
	}
	while ((dp = readdir(dir2))) {
		if (strcmp(dp->d_name,".") && strcmp(dp->d_name,".."))  {
			struct stat attribut;
			strcpy(cwd2,cwd);
			strcat(cwd2,"/");
			strcat(cwd2,dp->d_name);
			if (stat(cwd2, &attribut) == -1)
				d_log("zsfunctions.c: findfileextsub() - Error getting path\n");
			if (strcomp(sample_list, dp->d_name)) {
				if (S_ISDIR(attribut.st_mode)) {
					if ((dir3 = opendir(cwd2)) == NULL)
						d_log("zsfunctions.c: findfileextsub() - Error getting path\n");
	        			else {
						rewinddir(dir3);
						while ((dp2 = readdir(dir3))) {
							if ((k = NAMLEN(dp2)) < 4)
								continue;
							if (strcomp(video_types, dp2->d_name + k - 3))
								return dp2->d_name;
						}
						closedir(dir3);
					}
				}
			}
		}
		if ((k = NAMLEN(dp)) < 4)
			continue;
		if (strcomp(video_types, dp->d_name + k - 3))
			return dp->d_name;
	}
	closedir(dir2);

	if (errno)
		d_log("zsfunctions.c: findfileextsub() - closedir(dir) returned an error: %s\n", strerror(errno));

	return NULL;
}

char *
findfileextsubp(DIR *dir)
{
	DIR *dir2, *dir3;
	int			k;
	char cwd[1024], cwd2[1024], cwd3[1024];
	char * pch;
	static struct dirent	*dp, *dp2;

	errno = 0;

	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		d_log("zsfunctions.c: findfileextsubp() - Error getting path: %s\n", strerror(errno));
		return NULL;
	}

	pch=strrchr(cwd,47);
	strncpy(cwd2,cwd,pch-cwd);
	cwd2[pch-cwd]='\0';

	rewinddir(dir);

	if((dir2=opendir(cwd)) == NULL) {
		d_log("zsfunctions.c: findfileextsubp() - Error getting path: %s\n", strerror(errno));
		return NULL;
	}
	while ((dp = readdir(dir2))) {
		if (strcmp(dp->d_name,".") && strcmp(dp->d_name,".."))  {
			struct stat attribut;
			strcpy(cwd3,cwd2);
			strcat(cwd3,"/");
			strcat(cwd3,dp->d_name);
			if (stat(cwd3, &attribut) == -1)
				d_log("zsfunctions.c: findfileextsubp() - Error getting path\n");
			if (strcomp(sample_list, dp->d_name)) {
				if (S_ISDIR(attribut.st_mode)) {
					if ((dir3 = opendir(cwd3)) == NULL)
						d_log("zsfunctions.c: findfileextsubp() - Error getting path\n");
					else {
						rewinddir(dir3);
						while ((dp2 = readdir(dir3))) {
							if ((k = NAMLEN(dp2)) < 4)
								continue;
							if (strcomp(video_types, dp2->d_name + k - 3))
								return dp2->d_name;
						}
						closedir(dir3);
					}
				}
			}
		}
		if ((k = NAMLEN(dp)) < 4)
			continue;
		if (strcomp(video_types, dp->d_name + k - 3))
			return dp->d_name;
	}
	closedir(dir2);

	if (errno)
		d_log("zsfunctions.c: findfileextsubp() - readdir(dir) returned an error: %s\n", strerror(errno));

	return NULL;
}


int
check_dupefile(DIR *dir, char *fname)
{
	int			 found = 0;
	static struct dirent	*dp;

        errno = 0;

	rewinddir(dir);
	while ((dp = readdir(dir))) {
		if (strcasecmp(dp->d_name, fname) == 0)
			found++;
	}

        if (errno)
            d_log("zipscript-c: check_dupefile() - readdir(dir) returned an error: %s\n", strerror(errno));

	return (found - 1);
}


/*
 * findfilextparent - find a filename with a matching extension in parent dir.
 * Last Modified by: psxc
 *         Revision: ?? (2004.10.06)
 */
char           *
findfileextparent(DIR *dir, char *fileext)
{
	int			k;
	static struct dirent	*dp;

        errno = 0;

	rewinddir(dir);
	while ((dp = readdir(dir))) {
		if ((k = NAMLEN(dp)) < 4)
			continue;
		if (strcasecmp(dp->d_name + k - 4, fileext) == 0) {
			return dp->d_name;
		}
	}

        if (errno)
            d_log("zipscript-c: findfileextparent() - readdir(dir) returned an error: %s\n", strerror(errno));

	return NULL;
}

/*
 * findfilextcount - count files with given extension
 * Last Modified by: daxxar
 *         Revision: ?? (2003.12.11)
 */
int 
findfileextcount(DIR *dir, char *fileext)
{
	int		fnamelen, c = 0;
	struct dirent	*dp;

        errno = 0;

	rewinddir(dir);
	while ((dp = readdir(dir))) {
		if ((fnamelen = NAMLEN(dp)) < 4)
			continue;
		if (!strcasecmp((dp->d_name + fnamelen - 4), fileext))
			c++;
	}

        if (errno)
            d_log("zipscript-c: findfileextparent() - readdir(dir) returned an error: %s\n", strerror(errno));

	return c;
}

int 
file_count(DIR *dir)
{
	int		c = 0;
	struct dirent	*dp;
	char		*fp;

	rewinddir(dir);
	while ((dp = readdir(dir))) {
		fp = dp->d_name + NAMLEN(dp) - 4;
		if (*dp->d_name == '.')
			continue;
		if (strcomp(ignored_types, fp) || strcomp(allowed_types, fp))
			continue;
		c++;
	}
	return c;
}

/*
 * hexstrtodec - make sure a valid hex number is present in sfv
 * Last modified by: psxc
 *         Revision: r1219
 */
unsigned int 
hexstrtodec(char *s)
{
	unsigned int	n = 0;
	unsigned char	r;

	if (((int)strlen(s) > 8 ) || (!(int)strlen(s)))
		return 0;

	while (1) {
		if ((unsigned char)*s >= 48 && (unsigned char)*s <= 57) {
			r = 48;
		} else if ((unsigned char)*s >= 65 && (unsigned char)*s <= 70) {
			r = 55;
		} else if ((unsigned char)*s >= 97 && (unsigned char)*s <= 102) {
			r = 87;
		} else if ((unsigned char)*s == 0) {
			return n;
		} else {
			return 0;
		}
		n <<= 4;
		n += *s++ - r;
	}
}

/*
 * selector - dangling links
 * Last modified by: js(?)
 *         Revision: ??
 */
/*
 * dangling links
 */
#if defined(__linux__)
int 
selector(const struct dirent *d)
#elif defined(__NetBSD__)
	int		selector   (const struct dirent *d)
#else
int 
selector(struct dirent *d)
#endif
{
	struct stat	st;
	if ((stat(d->d_name, &st) < 0))
		return 0;
	return 1;
}

/*
 * del_releasedir - remove all files in current dir.
 * Last modified by: psxc
 *         Revision: ??
 */
void 
del_releasedir(DIR *dir, char *relname)
{
	struct dirent *dp;

	while ((dp = readdir(dir)))
		unlink(dp->d_name);
	rmdir(relname);
}


/*
 * strtolower - make a string all lowercase
 * Last modified by: d1
 *         Revision: ?? (2002.01.16)
 */
void 
strtolower(char *s)
{
	while ((*s = tolower(*s)))
		s++;
}

/*
 * space_to_dot - replace spaces with dots
 * Last modified by: psxc
 *         Revision: 2113
 */
void 
space_to_dot(char *s)
{
	while (*s) {
		if (*s == ' ')
			*s = '.';
		s++;
		}
}

/*
 * unlink_missing - remove <filename>-missing and <filename>.bad
 * Last modified by: psxc
 *         Revision: r1221
 */
void 
unlink_missing(char *s)
{
	char		t[NAME_MAX];
	long		loc;
	DIR		*dir;
	struct dirent	*dp;

	snprintf(t, NAME_MAX, "%s-missing", s);
	unlink(t);
#if (sfv_cleanup_lowercase)
	strtolower(t);
	unlink(t);
#endif
	dir = opendir(".");
	if ((loc = findfile(dir, t))) {
		seekdir(dir, loc);
		dp = readdir(dir);
		unlink(dp->d_name);
	}

	snprintf(t, NAME_MAX, "%s.bad", s);
	unlink(t);
#if (sfv_cleanup_lowercase)
	strtolower(t);
	unlink(t);
#endif
	rewinddir(dir);
	if ((loc = findfile(dir, t))) {
		seekdir(dir, loc);
		dp = readdir(dir);
		unlink(dp->d_name);
	}
	closedir(dir);
}

/*
 * israr - define a file as rar.
 * Last modified by: d1
 *         Revision: ?? (2002.01.16)
 */
char 
israr(char *fileext)
{
	if ((*fileext == 'r' || *fileext == 's' || isdigit(*fileext)) &&
	    ((isdigit(*(fileext + 1)) && isdigit(*(fileext + 2))) ||
	     (*(fileext + 1) == 'a' && *(fileext + 2) == 'r')) &&
	    *(fileext + 3) == 0)
		return 1;
	return 0;
}

/*
 * Created    : 02.20.2002 Author     : dark0n3
 * 
 * Description: Checks if file is known mpeg/avi file
 */
/*
 * Obsolete - commented out - will be removed.
char 
isvideo(char *fileext)
{
	switch (*fileext++) {
		case 'm':
			if (!memcmp(fileext, "pg", 3) ||
			    !memcmp(fileext, "peg", 4) ||
			    !memcmp(fileext, "2v", 3) ||
			    !memcmp(fileext, "2p", 3))
				return 1;
			break;
		case 'a':
			if (!memcmp(fileext, "vi", 3))
				return 1;
			break;
	}

	return 0;
}
 */

/*
 * Modified: 2004-11-17 (psxc) - added support to modify the chars used in the progressbar
 */
void 
buffer_progress_bar(struct VARS *raceI)
{
	int		n;

	raceI->misc.progress_bar[14] = 0;	if (raceI->total.files > 0) {
		for (n = 0; n < (raceI->total.files - raceI->total.files_missing) * 14 / raceI->total.files; n++)
			raceI->misc.progress_bar[n] = *charbar_filled;
		for (; n < 14; n++)
			raceI->misc.progress_bar[n] = *charbar_missing;
	}
}

/*
 * Modified: 01.16.2002
 */
void 
move_progress_bar(unsigned char delete, struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI)
{
	char           *bar;
	char	       *delbar = 0, regbuf[100];
	int		m = 0, regret;
	regex_t		preg;
	regmatch_t	pmatch[1];

	DIR		*dir;
	struct dirent *dp;

	if (raceI->misc.release_type == RTYPE_AUDIO) {
		d_log("move_progress_bar: del_progressmeter_mp3: %s\n", del_progressmeter_mp3);
		delbar = convert_sitename(del_progressmeter_mp3);
	} else {
		d_log("move_progress_bar: del_progressmeter: %s\n", del_progressmeter);
		delbar = convert_sitename(del_progressmeter);
	}
	d_log("move_progress_bar: delbar: %s\n", delbar);
	regret = regcomp(&preg, delbar, REG_NEWLINE | REG_EXTENDED);
	if (!regret) {
		if ((dir = opendir("."))) {

			if (delete) {
				while ((dp = readdir(dir))) {
					if ((int)strlen(dp->d_name) && regexec(&preg, dp->d_name, 1, pmatch, 0) == 0) {
						d_log("move_progress_bar: Found progress bar, removing\n");
						remove(dp->d_name);
						*dp->d_name = 0;
						m = 1;
					}
				}
				if (!m)
					d_log("move_progress_bar: Progress bar could not be deleted, not found!\n");
			} else {
				if (!raceI->total.files) {
					closedir(dir);
					regfree(&preg);
					return;
				}

				if (raceI->misc.release_type == RTYPE_AUDIO)
					bar = convert(raceI, userI, groupI, progressmeter_mp3);
				else
					bar = convert(raceI, userI, groupI, progressmeter);
				while ((dp = readdir(dir))) {
					if ((int)strlen(dp->d_name) && regexec(&preg, dp->d_name, 1, pmatch, 0) == 0) {
						if (!m) {
							d_log("move_progress_bar: Found progress bar, renaming.\n");
							rename(dp->d_name, bar);
							m = 1;
						} else {
							d_log("move_progress_bar: Found (extra) progress bar, removing\n");
							remove(dp->d_name);
							*dp->d_name = 0;
							m = 2;
						}
					}
				}
				if (!m) {
					d_log("move_progress_bar: Progress bar could not be moved, creating a new one now!\n");
					createstatusbar(bar);
				}
			}
			closedir(dir);
		} else
			d_log("move_progress_bar: opendir() failed : %s\n", strerror(errno));
		d_log("move_progress_bar: Freeing regpointer\n");
	} else {
		regerror(regret, &preg, regbuf, sizeof(regbuf));
		d_log("move_progress_bar: regex failed: %s\n", regbuf);
	}
	regfree(&preg);
}

/*
 * Modified: Unknown
 */
long
findfile(DIR *dir, char *filename)
{
	struct dirent	*dp;

	rewinddir(dir);
	while ((dp = readdir(dir))) {
//#if (sfv_cleanup_lowercase)
//		if (!strcasecmp(dp->d_name, filename))
//#else
//		if (!strcmp(dp->d_name, filename))
//#endif
		if (lenient_compare(dp->d_name, filename))
			return telldir(dir);
	}
	return 0;
}

void
removedotfiles(DIR *dir)
{
	struct dirent *dp;

	rewinddir(dir);
	while ((dp = readdir(dir)))
		if ((!strncasecmp(dp->d_name, ".", 1)) &&
		    ((int)strlen(dp->d_name) > 2))
			unlink(dp->d_name);
}

char *
findfilename(char *filename, char *dest, struct VARS *raceI)
{
	DIR		*dir;
	struct dirent 	*dp;

	dir = opendir(".");
	while ((dp = readdir(dir))) {
		if ((int)strlen(dp->d_name) && !strcasecmp(dp->d_name, filename)) {
			dest = ng_realloc(dest, (int)sizeof(dp->d_name) + 1, 1, 1, raceI, 0);
			strncpy(dest, dp->d_name, sizeof(dp->d_name));
			break;
		}
	}
	closedir(dir);
	return dest;
}

char *
check_nocase_linkname(char *dirname, char *linkname)
{
	DIR		*dir;
	struct dirent 	*dp;
	int		namelength = strlen(linkname);

	if ((dir = opendir(dirname)) == NULL) {
		d_log("check_nocase_linkname: failed to open %s\n", dirname);
		return linkname;
	}
	while ((dp = readdir(dir))) {
		if ((int)strlen(dp->d_name) == namelength && !strcasecmp(dp->d_name, linkname)) {
			d_log("check_nocase_linkname: found match: %s ~= %s\n", dp->d_name, linkname);
			linkname = ng_realloc(linkname, (int)sizeof(dp->d_name) + 1, 1, 1, NULL, 1);
			strncpy(linkname, dp->d_name, sizeof(dp->d_name));
			break;
		}
	}
	closedir(dir);
	d_log("check_nocase_linkname: returning %s\n", linkname);
	return linkname;
}

/*
 * Modified: 2011.07.07 (YYYY.MM.DD)
 *  by Sked
 */
void 
removecomplete()
{
	char		*mydelbar = 0, regbuf[100];
	regex_t		preg;
	regmatch_t	pmatch[1];
	int		regret;

	DIR		*dir;
	struct dirent	*dp;

        struct stat     fileinfo;
        char            deref_link[PATH_MAX];
        ssize_t         len;

        if (message_file_name != DISABLED && !message_file_name && lstat(message_file_name, &fileinfo) != -1) {
                if (S_ISLNK(fileinfo.st_mode) && stat(message_file_name, &fileinfo) != -1 && (len = readlink(message_file_name, deref_link, sizeof(deref_link)-1)) != -1) {
                        d_log("removecomplete: message_file_name is a symlink: %s points to %s\n", message_file_name, deref_link);
                        deref_link[len] = '\0';
                        unlink(deref_link);
                }
                unlink(message_file_name);
        }

	mydelbar = convert_sitename(del_completebar);
	d_log("removecomplete: del_completebar: %s\n", mydelbar);
	regret = regcomp(&preg, mydelbar, REG_NEWLINE | REG_EXTENDED);
	if (!regret) {
		if ((dir = opendir("."))) {
			while ((dp = readdir(dir))) {
				if (regexec(&preg, dp->d_name, 1, pmatch, 0) == 0) {
					if ((int)pmatch[0].rm_so == 0 && (int)pmatch[0].rm_eo == (int)NAMLEN(dp)) {
						remove(dp->d_name);
						*dp->d_name = 0;
					}
				}
			}
			closedir(dir);
		} else
			d_log("removecomplete: opendir failed : %s\n", strerror(errno));
	} else {
		regerror(regret, &preg, regbuf, sizeof(regbuf));
		d_log("move_progress_bar: regex failed: %s\n", regbuf);
	}
	regfree(&preg);
}

/*
 * Modified: 01.16.2002
 */
short int 
matchpath(char *instr, char *path)
{
	int		pos = 0;

	if ( (int)strlen(instr) < 2 || (int)strlen(path) < 2 ) {
		d_log("matchpath: pathlength(s) too short - returning nomatch (not an error)\n");
		return 0;
	}
	do {
		switch (*instr) {
		case 0:
		case ' ':
			if (!strncmp(instr - pos, path, pos)) {
				if (*(instr - 1) == '/')
					return 1;
				if ((int)strlen(path) >= pos) {
					if (*(path + pos) == '/')
						return 1;
				} else
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

/*
 * Modified: 2011.08.02 (YYYY.MM.DD)
 * by Sked
 */
short int 
strcomp(char *instr, char *searchstr)
{
	int		pos = 0,	k;

	k = (int)strlen(searchstr);

	if ( (int)strlen(instr) == 0 || k == 0 )
		return 0;

	do {
		switch (*instr) {
		case 0:
		case ',':
		case ' ':
			if (k == pos && !strncasecmp(instr - pos, searchstr, pos)) {
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

short int 
matchpartialpath(char *instr, char *path)
{
	int	pos = 0;
	char	partstring[strlen(path) + 2];

	if ( (int)strlen(instr) < 2 || (int)strlen(path) < 2 )
		return 0;

	sprintf(partstring, "%s", path);
	do {
		switch (*instr) {
		case 0:
		case ' ':
			if (!strncasecmp(instr - pos, partstring + (int)strlen(partstring) - pos, pos))
				return 1;
			if ((pos > 1) && (*(instr - 1) == '/') && (!strncasecmp(instr - pos, partstring + (int)strlen(partstring) + 1 - pos, pos - 1)))
				return 1;
			pos = 0;
			break;
		default:
			pos++;
			break;
		}
	} while (*instr++);
	return 0;
}

/*
 * Given a comma-seperated list of strings (arg1),
 * check if any is a part of a given string (arg2)
 * when surrounded by any chars of a given string.
 * Modified: 2011.07.13 (YYYY.MM.DD)
 * by Sked
 */
short int 
matchpartialdirname(char *parts, char *dirname, char *dividers)
{
	int	pos = 0;
	int	i, k, l, hit;

	k = (int)strlen(dirname);
	l = (int)strlen(dividers);

	if ( (int)strlen(parts) == 0 || k == 0 )
		return 0;

	d_log("matchpartialdirname: parts: %s.\n", parts);
	d_log("matchpartialdirname: dirname: %s.\n", dirname);
	d_log("matchpartialdirname: dividers: %s.\n", dividers);

	do {
		switch (*parts) {
		case 0:
		case ',':
			d_log("matchpartialdirname: comparing first string of: %s.\n", parts - pos);
			do {
				hit = 0;
				i = 0;
				while (i<l && !hit) {
					if (dirname[0] && dividers[i] == dirname[0]) {
						hit = 1;
					}
					++i;
				}
				if (!l || hit) {
					d_log("matchpartialdirname: At divider, hit=%d, comparing to part of dirname: %s.\n", hit, dirname + hit);
					if (dirname[hit] && !strncasecmp(parts - pos, dirname + hit, pos)) {
						d_log("matchpartialdirname: We have a hit, checking ending divider with char: %c.\n", dirname[pos + 1]);
						if (!l) return 1;
						for (i=0;i<l;++i) {
							if (dirname[pos + 1] && dividers[i] == dirname[pos + 1]) {
								return 1;
							}
						}
					}
				}
			} while (*dirname++);
			pos = 0;
			dirname -= (k + 1);
			break;
		default:
			++pos;
			break;
		}
	} while (*parts++);
	return 0;
}

/* check for matching subpath
   psxc - 2004-12-18
 */
short int 
subcomp(char *directory, char *basepath)
{
	int 	k = (int)strlen(directory);
	int	m = (int)strlen(subdir_list);
	char	tstring[m + 1];
	char	bstring[k + 1];
	char	*tpos, *startpos, *endpos, *midpos;
	int	sublen = 0, dirlen = 0, seeklen = 0;

	if ( k < 2 )
		return 0;

	if (basepath)
		bzero(basepath, k + 1);
	else
		return 0;

	tpos = strrchr(directory, '/');
	if (tpos)
		strncpy(bstring, tpos + 1, k + 1);
	else
		strncpy(bstring, directory, k + 1);

	dirlen = strlen(bstring);
	strncpy(tstring, subdir_list, m + 1);
	startpos = tstring;
	do {
		endpos = strchr(startpos, ',');
		if (endpos)
			*endpos = '\0';
		else
			endpos = strchr(startpos, '\0');

		seeklen = strlen(startpos);
		if (!seeklen)
			break;

		midpos = strchr(startpos, '?');
		if (midpos)
			*midpos = '\0';

		sublen = strlen(startpos);
		if (!sublen)
			break;
		if ((sublen <= dirlen) && (dirlen <= seeklen) && !strncasecmp(bstring, startpos, sublen)) {
			d_log("subcomp: we are in a subdir.\n");
			if (tpos)
				strncpy(basepath, directory, tpos - directory);
			else
				strncpy(basepath, directory, k + 1);
			return 1;
		}
		startpos = endpos + 1;
		if (startpos >= (tstring + m))
			break;
	} while (1);
	d_log("subcomp: not in subdir.\n");
	strncpy(basepath, directory, k + 1);
	return 0;
}

/* Checks if file exists */
short int 
fileexists(char *f)
{
	if (access(f, R_OK) == -1)
		return 0;
	return 1;

}

/* Create symbolic link (related to mp3 genre/year/group etc)
 * Last modified by: psxc
 *         Revision: r1228
 */
void 
createlink(char *factor1, char *factor2, char *source, char *ltarget)
{

#if ( userellink == 1 )
	char		result	[MAXPATHLEN];
#endif
	char		org	[PATH_MAX];
	char	       *target = org;
	int		l1 = (int)strlen(factor1) + 1,
			l2 = (int)strlen(factor2) + 1,
			l3 = (int)strlen(ltarget) + 1;
	struct stat linkStat;

	memcpy(target, factor1, l1);
	target += l1 - 1;
	if (*(target - 1) != '/') {
		*(target) = '/';
		target += 1;
	}
	memcpy(target, factor2, l2);
	target += l2;
	memcpy(target - 1, "/", 2);

#if (spaces_to_dots)
	space_to_dot(org);
#endif
	if (lstat(org, &linkStat) == -1) {
		if (mkdir(org, 0777) == -1)
			d_log("createlink: Failed to mkdir -m777 %s : %s\n", org, strerror(errno));
	} else
		d_log("createlink: Looks like %s already exists - will not create it.\n", org);

if (access(org, W_OK) == -1)
	d_log("createlink: Warning: May be a problem with linking to %s : %s\n", org, strerror(errno));

#if ( userellink == 1 )
	abs2rel(source, org, result, MAXPATHLEN);
#endif

	memcpy(target, ltarget, l3);

# if ( delete_old_link == TRUE )
	if (lstat(org, &linkStat) != -1 && S_ISLNK(linkStat.st_mode))
		if (unlink(org) == -1)
			d_log("createlink: Failed to unlink '%s' : %s\n", org, strerror(errno));
# endif


#if ( userellink == 1 )
	if (symlink(result, org) == -1)
		d_log("createlink: Failed to symlink %s -> %s : %s\n", result, org, strerror(errno));
#else
	if (symlink(source, org) == -1)
		d_log("createlink: Failed to symlink %s -> %s : %s\n", source, org, strerror(errno));
#endif
}


void 
readsfv_ffile(struct VARS *raceI)
{
	int		fd, line_start = 0, index_start,
			ext_start, n;
	char		*buf = NULL, *fname;

	DIR		*dir;

	fd = open(raceI->file.name, O_RDONLY);
	buf = ng_realloc(buf, raceI->file.size + 2, 1, 1, raceI, 1);
	if (read(fd, buf, raceI->file.size) == -1) {
		d_log("readsfv_ffile: Failed to read() %s: %s\n", raceI->file.name, strerror(errno));
	}
	close(fd);

	dir = opendir(".");

	for (n = 0; n <= raceI->file.size; n++) {
		if (buf[n] == '\n' || n == raceI->file.size) {
			index_start = n - line_start;
			if (buf[line_start] != ';') {
				while (buf[index_start + line_start] != ' ' && index_start--);
				if (index_start > 0) {
					buf[index_start + line_start] = 0;
					fname = buf + line_start;
					ext_start = index_start;
#if (sfv_cleanup_lowercase == TRUE)
					while ((fname[ext_start] = tolower(fname[ext_start])) != '.' && ext_start > 0)
#else
					while (fname[ext_start] != '.' && ext_start > 0)
#endif
						ext_start--;
					if (fname[ext_start] != '.') {
						ext_start = index_start;
					} else {
						ext_start++;
					}
					index_start++;
					raceI->total.files++;
					if (!strcomp(ignored_types, fname + ext_start) || !strcomp("nfo", fname + ext_start)) {
//					if (!strcomp(ignored_types, fname + ext_start) && !(strcomp(allowed_types, fname + ext_start) && matchpath(allowed_types_exemption_dirs, raceI->misc.current_path))) {
						if (findfile(dir, fname)) {
							raceI->total.files_missing--;
						}
					}
				}
			}
			line_start = n + 1;
		}
	}
	raceI->total.files_missing = raceI->total.files + raceI->total.files_missing;
	if (raceI->total.files_missing < 0) {
		d_log("readsfv_ffile: GAKK! raceI->total.files_missing < 0\n");
		raceI->total.files_missing = 0;
	}
	ng_free(buf);
	closedir(dir);
}

void 
get_rar_info(struct VARS *raceI)
{
	FILE           *file;

	if ((file = fopen(raceI->file.name, "r"))) {
		fseek(file, 45, SEEK_CUR);
		if (!fread(&raceI->file.compression_method, 1, 1, file)) {
			d_log("get_rar_info: Failed to fread() %s.\n", raceI->file.name);
		}

		if ( ! (( 47 < raceI->file.compression_method ) && ( raceI->file.compression_method < 54 )) )
			raceI->file.compression_method = 88;
		fclose(file);
	}
}

/*
 * Modified   : 27.02.2005 Author     : js
 * 
 * Description: Executes external program and returns return value
 * 
 */
int 
execute(char *s)
{
	int	i = 0;

	if ((i = system(s)) == -1)
		d_log("execute (old): %s\n", strerror(errno));
	return i;
}

#ifdef USING_GLFTPD
/* Only under glftpd do we have a uid/gid-lookup, so these
 * are only needed there. */
char           *
get_g_name(int gid)
{
	int		n;
	for (n = 0; n < num_groups; n++)
		if ((int)group[n]->id / 100 == (int)gid / 100)
			return group[n]->name;
	return "NoGroup";
}

char           *
get_u_name(int uid)
{
	int		n;
	for (n = 0; n < num_users; n++)
		if (user[n]->id == (unsigned int)uid)
			return user[n]->name;
	return "Unknown";
}
#endif

#ifdef USING_GLFTPD
/* Only under glftpd do we have a uid/gid-lookup, so these
 * are only needed there. */

/* Buffer groups file */
int 
buffer_groups(char *groupfile, int setfree)
{
	char           *f_buf = NULL, *g_name;
	gid_t		g_id;
	ssize_t		f_buf_len;
	int		f, n, m, g_n_size, l_start = 0;
	int		GROUPS = 0;
	struct stat	fileinfo;

	if (setfree != 0) {
		for (n = 0; n < setfree; n++) {
			ng_free(group[n]->name);
			ng_free(group[n]);
		}
		ng_free(group);
		return 0;
	}

	f = open(groupfile, O_NONBLOCK);
	if (f == -1) {
	    d_log("buffer_groups: open(%s) failed: %s\n", groupfile, strerror(errno));
	    return 0;
	}

	if (fstat(f, &fileinfo) == -1) {
	    d_log("buffer_groups: fstat(%s) failed: %s\n", groupfile, strerror(errno));
	    close(f);
	    return 0;
	}
	f_buf = ng_realloc2(f_buf, fileinfo.st_size, 1, 1, 1);

	f_buf_len = read(f, f_buf, fileinfo.st_size);
	if (f_buf_len == -1) {
	    d_log("buffer_groups: read(%s) failed: %s\n", groupfile, strerror(errno));
	    ng_free(f_buf);
	    close(f);
	    return 0;
	}

	for (n = 0; n < f_buf_len; n++)
		if (f_buf[n] == '\n')
			GROUPS++;
	group = ng_realloc2(group, GROUPS * sizeof(struct GROUP *), 1, 1, 1);

	for (n = 0; n < f_buf_len; n++) {
		if (f_buf[n] == '\n' || n == f_buf_len) {
			f_buf[n] = 0;
			m = l_start;
			while (f_buf[m] != ':' && m < n)
				m++;
			if (m != l_start) {
				f_buf[m] = 0;
				g_name = f_buf + l_start;
				g_n_size = m - l_start;
				m = n;
				while (f_buf[m] != ':' && m > l_start)
					m--;
				f_buf[m] = 0;
				while (f_buf[m] != ':' && m > l_start)
					m--;
				if (m != n) {
					g_id = strtol(f_buf + m + 1, NULL, 10);
					group[num_groups] = ng_realloc2(group[num_groups], sizeof(struct GROUP), 1, 1, 1);
					group[num_groups]->name = ng_realloc2(group[num_groups]->name, g_n_size + 1, 1, 1, 1);
					strcpy(group[num_groups]->name, g_name);
					group[num_groups]->id = g_id;
					num_groups++;
				}
			}
			l_start = n + 1;
		}
	}

	close(f);
	ng_free(f_buf);
	return num_groups;
}

/* Buffer users file */
int
buffer_users(char *passwdfile, int setfree)
{ 
	char           *f_buf = NULL, *u_name;
	uid_t		u_id;
	ssize_t		f_buf_len;
	int		f, n, m, l, u_n_size, l_start = 0;
	int		USERS = 0;
	struct stat	fileinfo;

	if (setfree != 0) {
		for (n = 0; n < setfree; n++) {
			ng_free(user[n]->name);
			ng_free(user[n]);
		}
		ng_free(user);
		return 0;
	}

	f = open(passwdfile, O_NONBLOCK);
	if (f == -1) {
	    d_log("buffer_users: open(%s) failed: %s\n", passwdfile, strerror(errno));
	    return 0;
	}

	if (fstat(f, &fileinfo) == -1) {
	    d_log("buffer_users: fstat(%s) failed: %s\n", passwdfile, strerror(errno));
	    close(f);
	    return 0;
	}
	f_buf = ng_realloc2(f_buf, fileinfo.st_size, 1, 1, 1);

	f_buf_len = read(f, f_buf, fileinfo.st_size);
	if (f_buf_len == -1) {
	    d_log("buffer_users: read(%s) failed: %s\n", passwdfile, strerror(errno));
	    ng_free(f_buf);
	    close(f);
	    return 0;
	}

	for (n = 0; n < f_buf_len; n++)
		if (f_buf[n] == '\n')
			USERS++;
	user = ng_realloc2(user, USERS * sizeof(struct USER *), 1, 1, 1);

	for (n = 0; n < f_buf_len; n++) {
		if (f_buf[n] == '\n' || n == f_buf_len) {
			f_buf[n] = 0;
			m = l_start;
			while (f_buf[m] != ':' && m < n)
				m++;
			if (m != l_start) {
				f_buf[m] = 0;
				u_name = f_buf + l_start;
				u_n_size = m - l_start;
				m = n;
				for (l = 0; l < 4; l++) {
					while (f_buf[m] != ':' && m > l_start)
						m--;
					f_buf[m] = 0;
				}
				while (f_buf[m] != ':' && m > l_start)
					m--;
				if (m != n) {
					u_id = strtol(f_buf + m + 1, NULL, 10);
					user[num_users] = ng_realloc2(user[num_users], sizeof(struct USER), 1, 1, 1);
					user[num_users]->name = ng_realloc2(user[num_users], u_n_size + 1, 1, 1, 1);
					strcpy(user[num_users]->name, u_name);
					user[num_users]->id = u_id;
					num_users++;
				}
			}
			l_start = n + 1;
		}
	}

	close(f);
	ng_free(f_buf);
	return num_users;
}
#endif

unsigned long 
sfv_compare_size(char *fileext, unsigned long fsize)
{
	int		k = 0;
	unsigned long	l = 0;
	struct stat	filestat;

	DIR		*dir;
	struct dirent	*dp;

	dir = opendir(".");

	while ((dp = readdir(dir))) {
		if ((k = NAMLEN(dp)) < 4)
			continue;
		if (strcasecmp(dp->d_name + k - 4, fileext) == 0) {
			if (stat(dp->d_name, &filestat) != 0)
				filestat.st_size = 1;
			l = l + filestat.st_size;
			continue;
		}
	}

	if (!(l = l - fsize) > 0)
		l = 0;

	closedir(dir);

	return l;
}

void
mark_as_bad(char *filename)
{
#if (mark_file_as_bad)
	char	newname[NAME_MAX];

	if (!fileexists(filename)) {
		d_log("mark_as_bad: \"%s\" doesn't exist\n", filename);
		return;
	}
	sprintf(newname, "%s.bad", filename);
	if (rename(filename, newname)) {
		d_log("mark_as_bad: Error - failed to rename %s to %s\n", filename, newname);
	} else {
		createzerofile(filename);
		if (chmod(newname, 0644))
			d_log("mark_as_bad: Failed to chmod %s: %s\n", newname, strerror(errno));
	}
#endif
	d_log("mark_as_bad: File (%s) marked as bad\n", filename);
}

void 
writelog(GLOBAL *g, char *msg, char *status)
{
	FILE           *glfile;
	char           *date;
	char           *line, *newline;
	time_t		timenow;

	if (g->v.misc.write_log == TRUE && !matchpath(group_dirs, g->l.path)) {
		timenow = time(NULL);
		date = ctime(&timenow);
		if (!(glfile = fopen(log, "a+"))) {
			d_log("writelog: fopen(%s): %s\n", log, strerror(errno));
			return;
		}
		line = newline = msg;
		while (1) {
			switch (*newline++) {
			case 0:
				fprintf(glfile, "%.24s %s: \"%s\" %s\n", date, status, g->l.path, line);
				fclose(glfile);
				return;
			case '\n':
				fprintf(glfile, "%.24s %s: \"%s\" %.*s\n", date, status, g->l.path,(int)(newline - line - 1), line);
				line = newline;
				break;
			}
		}
	} else
			d_log("writelog: not writing to %s - path matched with group_dirs: %s\n", log, group_dirs);
}

void
buffer_paths(GLOBAL *g, char path[2][PATH_MAX], int *k, int len)
{
	int		cnt, n = 0;

	d_log("buffer_paths: g->l.path=%s\n", g->l.path);
	for (cnt = len; *k && cnt; cnt--) {
		if (g->l.path[cnt] == '/') {
			(*k)--;
			strlcpy(path[*k], g->l.path + cnt + 1, n+1);
			path[*k][n] = 0;
			n = 0;
		} else {
			n++;
		}
	}
}

void 
remove_nfo_indicator(GLOBAL *g)
{
	int		k = 2;
	char		path[2][PATH_MAX];

	buffer_paths(g, path, &k, ((int)strlen(g->l.path)-1));

	if (matchpath(incomplete_generic1_path, g->l.path))
		g->l.nfo_incomplete = incomplete(incomplete_generic1_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
	else if (matchpath(incomplete_generic2_path, g->l.path))
		g->l.nfo_incomplete = incomplete(incomplete_generic2_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
	else
		g->l.nfo_incomplete = incomplete(incomplete_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
	if (fileexists(g->l.nfo_incomplete))
		unlink(g->l.nfo_incomplete);
	if (matchpath(incomplete_generic1_path, g->l.path))
		g->l.nfo_incomplete = incomplete(incomplete_generic1_base_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
	else if (matchpath(incomplete_generic2_path, g->l.path))
		g->l.nfo_incomplete = incomplete(incomplete_generic2_base_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
	else
		g->l.nfo_incomplete = incomplete(incomplete_base_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
	if (fileexists(g->l.nfo_incomplete))
		unlink(g->l.nfo_incomplete);
}

void
remove_sample_indicator(GLOBAL *g)
{
        int             k = 2;
	char            path[2][PATH_MAX];

	buffer_paths(g, path, &k, ((int)strlen(g->l.path)-1));

	if (matchpath(incomplete_generic1_path, g->l.path))
		g->l.sample_incomplete = incomplete(incomplete_generic1_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
	else if (matchpath(incomplete_generic2_path, g->l.path))
	        g->l.sample_incomplete = incomplete(incomplete_generic2_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
	else
	        g->l.sample_incomplete = incomplete(incomplete_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
	if (fileexists(g->l.sample_incomplete))
	        unlink(g->l.sample_incomplete);
	if (matchpath(incomplete_generic1_path, g->l.path))
	        g->l.sample_incomplete = incomplete(incomplete_generic1_base_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
	else if (matchpath(incomplete_generic2_path, g->l.path))
	        g->l.sample_incomplete = incomplete(incomplete_generic2_base_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
	else
	        g->l.sample_incomplete = incomplete(incomplete_base_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
	if (fileexists(g->l.sample_incomplete))
	        unlink(g->l.sample_incomplete);
}

void 
getrelname(GLOBAL *g)
{
	int		k = 2, subc;
	char		path[2][PATH_MAX];

	buffer_paths(g, path, &k, ((int)strlen(g->l.path)-1));

	subc = subcomp(path[1], g->l.basepath);

	d_log("getrelname():\tsubc:\t\t%d\n", subc);
	d_log("\t\t\tpath[0]:\t%s\n", path[0]);
	d_log("\t\t\tpath[1]:\t%s\n", path[1]);
	d_log("\t\t\tg->l_path:\t%s\n", path[1]);

	if (subc) {
		snprintf(g->v.misc.release_name, PATH_MAX, "%s/%s", path[0], path[1]);
		strlcpy(g->l.link_source, g->l.path, PATH_MAX);
		strlcpy(g->l.link_target, path[1], PATH_MAX);
		if (matchpath(incomplete_generic1_path, g->l.path)) {
			g->l.incomplete = incomplete(incomplete_generic1_cd_indicator, path, &g->v, INCOMPLETE_NORMAL);
			g->l.nfo_incomplete = incomplete(incomplete_generic1_base_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
			g->l.sample_incomplete = incomplete(incomplete_generic1_base_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
			g->l.sfv_incomplete = incomplete(incomplete_generic1_base_sfv_indicator, path, &g->v, INCOMPLETE_SFV);
		} else if (matchpath(incomplete_generic2_path, g->l.path)) {
			g->l.incomplete = incomplete(incomplete_generic2_cd_indicator, path, &g->v, INCOMPLETE_NORMAL);
			g->l.nfo_incomplete = incomplete(incomplete_generic2_base_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
			g->l.sample_incomplete = incomplete(incomplete_generic2_base_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
			g->l.sfv_incomplete = incomplete(incomplete_generic2_base_sfv_indicator, path, &g->v, INCOMPLETE_SFV);
		} else {
			g->l.incomplete = incomplete(incomplete_cd_indicator, path, &g->v, INCOMPLETE_NORMAL);
			g->l.nfo_incomplete = incomplete(incomplete_base_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
			g->l.sample_incomplete = incomplete(incomplete_base_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
			g->l.sfv_incomplete = incomplete(incomplete_base_sfv_indicator, path, &g->v, INCOMPLETE_SFV);
		}
		g->l.in_cd_dir = 1;
	} else {
		strlcpy(g->v.misc.release_name, path[1], PATH_MAX);
		strlcpy(g->l.link_source, g->l.path, PATH_MAX);
		strlcpy(g->l.link_target, path[1], PATH_MAX);
		if (matchpath(incomplete_generic1_path, g->l.path)) {
			g->l.incomplete = incomplete(incomplete_generic1_indicator, path, &g->v, INCOMPLETE_NORMAL);
			g->l.nfo_incomplete = incomplete(incomplete_generic1_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
			g->l.sample_incomplete = incomplete(incomplete_generic1_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
			g->l.sfv_incomplete = incomplete(incomplete_generic1_sfv_indicator, path, &g->v, INCOMPLETE_SFV);
		} else if (matchpath(incomplete_generic2_path, g->l.path)) {
			g->l.incomplete = incomplete(incomplete_generic2_indicator, path, &g->v, INCOMPLETE_NORMAL);
			g->l.nfo_incomplete = incomplete(incomplete_generic2_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
			g->l.sample_incomplete = incomplete(incomplete_generic2_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
			g->l.sfv_incomplete = incomplete(incomplete_generic2_sfv_indicator, path, &g->v, INCOMPLETE_SFV);
		} else {
			g->l.incomplete = incomplete(incomplete_indicator, path, &g->v, INCOMPLETE_NORMAL);
			g->l.nfo_incomplete = incomplete(incomplete_nfo_indicator, path, &g->v, INCOMPLETE_NFO);
			g->l.sample_incomplete = incomplete(incomplete_sample_indicator, path, &g->v, INCOMPLETE_SAMPLE);
			g->l.sfv_incomplete = incomplete(incomplete_sfv_indicator, path, &g->v, INCOMPLETE_SFV);
		}
		g->l.in_cd_dir = 0;
	}

	d_log("\t\t\tlink_source:\t%s\n", g->l.link_source);
	d_log("\t\t\tlink_target:\t%s\n", g->l.link_target);
	d_log("\t\t\tg->l.incomplete:\t%s\n", g->l.incomplete);
}

unsigned char 
get_filetype(GLOBAL *g, char *ext)
{
	if (!strcasecmp(ext, "zip"))
		return 0;
	if (!strcasecmp(ext, "sfv"))
		return 1;
	if (!strcasecmp(ext, "nfo"))
		return 2;
	if (strcomp(allowed_types, ext) && !matchpath(allowed_types_exemption_dirs, g->l.path))
		return 4;
	if (!strcomp(ignored_types, ext))
		return 3;

	return 255;
}

#if ( audio_group_sort == TRUE )
char    *
remove_pattern(param, pattern, op)
	char           *param, *pattern;
	int		op;
{
	register int	len;
	register char  *end;
	register char  *p, *ret, c;

	if (param == NULL || *param == '\0')
		return (param);
	if (pattern == NULL || *pattern == '\0')	/* minor optimization */
		return (param);

	len = (int)strlen(param);
	end = param + len;

	switch (op) {
	case RP_LONG_LEFT:	/* remove longest match at start */
		for (p = end; p >= param; p--) {
			c = *p;
			*p = '\0';
			if ((fnmatch(pattern, param, 0)) != FNM_NOMATCH) {
				*p = c;
				return (p);
			}
			*p = c;
		}
		break;

	case RP_SHORT_LEFT:	/* remove shortest match at start */
		for (p = param; p <= end; p++) {
			c = *p;
			*p = '\0';
			if (fnmatch(pattern, param, 0) != FNM_NOMATCH) {
				*p = c;
				return (p);
			}
			*p = c;
		}
		break;


	case RP_LONG_RIGHT:	/* remove longest match at end */
		for (p = param; p <= end; p++) {
			if (fnmatch(pattern, param, 0) != FNM_NOMATCH) {
				c = *p;
				*p = '\0';
				ret = param;
				*p = c;
				return (ret);
			}
		}
		break;

	case RP_SHORT_RIGHT:	/* remove shortest match at end */
		for (p = end; p >= param; p--) {
			if (fnmatch(pattern, param, 0) != FNM_NOMATCH) {
				c = *p;
				*p = '\0';
				ret = param;
				*p = c;
				return (ret);
			}
		}
		break;
	}
	return (param);		/* no match, return original string */
}
#endif

void *
ng_realloc(void *mempointer, int memsize, int zero_it, int exit_on_error, struct VARS *raceI, int zero_pointer)
{
	if (zero_pointer)
		mempointer = malloc(memsize);
	else
		mempointer = realloc(mempointer, memsize);
	if (mempointer == NULL) {
		d_log("ng_realloc: realloc failed: %s\n", strerror(errno));
		if (exit_on_error) {
			remove_lock(raceI);
			exit(EXIT_FAILURE);
		}
	} else if (zero_it)
		bzero(mempointer, memsize);
	return mempointer;
}

void *
ng_realloc2(void *mempointer, int memsize, int zero_it, int exit_on_error, int zero_pointer)
{
	if (zero_pointer)
		mempointer = malloc(memsize);
	else
		mempointer = realloc(mempointer, memsize);
	if (mempointer == NULL) {
		d_log("ng_realloc2: realloc failed: %s\n", strerror(errno));
		if (exit_on_error) {
			exit(EXIT_FAILURE);
		}
	} else if (zero_it)
		bzero(mempointer, memsize);
	return mempointer;
}

void *
ng_free(void *mempointer)
{
	if (mempointer)
		free(mempointer);
	return 0;
}

int
copyfile(char *from_name, char *to_name)
{
  FILE *from, *to;
  char ch;
  int  retvar = 0;

  if((from = fopen(from_name, "rb"))==NULL) {
    d_log("copyfile: cannot open source file.\n");
    return 1;
  }
  if((to = fopen(to_name, "wb"))==NULL) {
    d_log("copyfile: cannot open destination file.\n");
    fclose(from);
    return 1;
  }
  while(!feof(from)) {
    ch = fgetc(from);
    if(ferror(from)) {
      d_log("copyfile: error reading source file.\n");
      retvar = 1;
      break;
    }
    if(!feof(from)) fputc(ch, to);
    if(ferror(to)) {
      d_log("copyfile: error writing destination file.\n");
      retvar = 1;
      break;
    }
  }
  if(fclose(from)==EOF) {
    d_log("copyfile: error closing source file.\n");
    retvar = 1;
  }
  if(fclose(to)==EOF) {
    d_log("copyfile: error closing destination file.\n");
    retvar = 1;
  }
  return retvar;
}

int
extractDirname(char *dirname, char *absoluteDirname)
{
        int cnt, n = 0;
        for (cnt = strlen(absoluteDirname); cnt; cnt--) {
                if (absoluteDirname[cnt] == '/') {
                        strncpy(dirname, absoluteDirname + cnt + 1, n + 1);
                        dirname[n] = 0;
                        break;
                } else {
                        n++;
                }
        }
        return cnt;
}

int make_sfv(char *reldir) {
	static struct dirent	*dp;
	static struct stat	stat_buf;
	DIR			*dirp;
	int			fd, fcount = 0;
	static char		buf[PATH_MAX + 3];
	char			*fp;

	if (!(dirp = opendir("."))) {
		d_log("make_sfv: Failed to open current dir: %s\n", strerror(errno));
		return 1;
	}
	if ((fd = open("pzs-ng.sfv", O_WRONLY|O_CREAT|O_TRUNC, 0666)) == -1) {
		d_log("make_sfv: Failed to create pzs-ng.sfv: %s\n", strerror(errno));
		(void)closedir(dirp);
		return 1;
	}
	while ((dp = readdir(dirp)) != NULL) {
		fp = dp->d_name + NAMLEN(dp) - 3;
		if (NAMLEN(dp) > 2 && *dp->d_name != '.' &&
		    !strcomp(ignored_types, fp) &&
		    (!strcomp(allowed_types, fp) || (strcomp(allowed_types, fp) && matchpath(allowed_types_exemption_dirs, reldir))) &&
		    strcmp(dp->d_name, "pzs-ng.sfv") &&
		    (dp->d_type == DT_REG || (!stat(dp->d_name, &stat_buf) && S_ISREG(stat_buf.st_mode) )) &&
		    strcmp(fp, "nfo")) {
			d_log("make_sfv: Adding \"%-20s 00000000\" to sfv\n", dp->d_name);
			snprintf(buf, sizeof(buf), "%-20s 00000000\n", dp->d_name);
			fcount++;
		        if ((write(fd, buf, strlen(buf))) == -1) {
		                d_log("make_sfv: write failed: %s\n", strerror(errno));
				(void)close(fd);
				(void)closedir(dirp);
				return 1;
			}
		} else
			d_log("make_sfv: Ignoring %s (check allowed_types and ignored_types if this is wrong)\n", dp->d_name);
	}
	if (!fcount) {
		d_log("make_sfv: Did not find anything to put in the sfv - removing sfv\n");
		unlink("pzs-ng.sfv");
	}
	(void)close(fd);
	(void)closedir(dirp);
	return 0;
}

unsigned int
match_lenient(DIR *dir, char *fname)
{
	unsigned int		crc = 0;
	static struct dirent   *dp;

	rewinddir(dir);
	while ((dp = readdir(dir))) {
		if (lenient_compare(dp->d_name, fname)) {
			crc = calc_crc32(dp->d_name);
			return crc;
		}
	}
	return 0;
}
unsigned int
insampledir(char *dirname)
{
	char	*t = strrchr(dirname, '/');
	char	sample[sizeof(sample_list)+1];
	char	*p = NULL;

	if (strlen(sample_list) < 2)
		return 0;
	if (t == NULL)
		t = dirname;
	else
		t++;
	strncpy(sample, sample_list, sizeof(sample_list));
	while (p != sample) {
		p = strrchr(sample, ' ');
		if (p != NULL) {
			*p = '\0';
			p++;
		} else
			p = sample;
		if (!strncasecmp(t,p,strlen(p)))
			if (strlen(t) == strlen(p))
				return 1;
	}
	return 0;
}

void
createstatusbar(const char *bar)
{
#if ( status_bar_type == BAR_OFF )
    (void)bar;
#else
    char *newbar, *tmp;
    tmp = newbar = malloc(strlen(bar) + 1);
    strcpy(newbar, bar);

    tmp = strtok(newbar, "\n");
    while (tmp != NULL)
    {
/* Creates status bar file */
#if ( status_bar_type == BAR_FILE )
        createzerofile(tmp);
#endif
#if ( status_bar_type == BAR_DIR )
        mkdir(tmp, 0777);
#endif

        tmp = strtok(NULL, "\n");
    }
    free(newbar);
#endif
}


/* chmods each file in a newline-separated list. */
int
chmod_each(const char *list, mode_t mode)
{
    int fail = 0;
    char *newlist, *item;
    item = newlist = malloc(strlen(list) + 1);
    strcpy(newlist, list);

    item = strtok(newlist, "\n");
    while (item != NULL)
    {
        if (chmod(item, mode) != 0)
            fail = -1;
        item = strtok(NULL, "\n");
    }
    free(newlist);

    return fail;
}
