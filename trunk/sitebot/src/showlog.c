/*
  showlog v1.0 by neoxed
  Displays the latest entries in the dirlog and nukelog
  in an easy-to-parse format for scripts.
  2005-01-01 - psxc:
               modded to be included in pzs-ng
               fixed some warnings.
               we may need to look at this later,
               on 64bit platforms.
  2007-04-04 - psxc:
               do not include unwanted cruft like cd1,
               sample, subs etc. We use the subdir_list
               option from zsconfig.h for this.

*/

#include <sys/file.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "zsconfig.h"
#include "zsconfig.defaults.h"

//#undef group_dirs
//#define group_dirs                      "/site/groups/"


/* Comment this line to compile showlog for glftpd v2.x */
/* (psxc) - well, we'll do, since it's defined already when compiling in pzs-ng." */
//#define GLVERSION 132

/* Default values */
#define GLCONF "/etc/glftpd.conf"
static char rootpath[MAXPATHLEN+1] = "/glftpd";
static char datapath[MAXPATHLEN+1] = "/ftp-data";
static int max_results = 10;
static int match_full = 0;
static int search_mode = 0;

#if ( GLVERSION == 132 )
struct dirlog {
	ushort status;		            /* 0 = NEWDIR, 1 = NUKE, 2 = UNNUKE, 3 = DELETED */
	time_t uptime;                  /* Creation time since epoch (man 2 time) */
	ushort uploader;                /* The userid of the creator */
	ushort group;                   /* The groupid of the primary group of the creator */
	ushort files;                   /* The number of files inside the dir */
	long bytes;                     /* The number of bytes in the dir */
	char dirname[255];              /* The name of the dir (fullpath) */
	struct dirlog *nxt;
	struct dirlog *prv;
} __attribute__((deprecated));
#else
struct dirlog {
	ushort status;                  /* 0 = NEWDIR, 1 = NUKE, 2 = UNNUKE, 3 = DELETED */
	time_t uptime;                  /* Creation time since epoch (man 2 time) */
	ushort uploader;                /* The userid of the creator */
	ushort group;                   /* The groupid of the primary group of the creator */
	ushort files;                   /* The number of files inside the dir */
	unsigned long long bytes;       /* The number of bytes in the dir */
	char dirname[255];              /* The name of the dir (fullpath) */
	struct dirlog *nxt;             /* Unused, kept for compatibility reasons */
	struct dirlog *prv;             /* Unused, kept for compatibility reasons */
};
#endif

struct nukelog {
	ushort status;			/* 0 = NUKED, 1 = UNNUKED */
	time_t nuketime;		/* The nuke time since epoch (man 2 time) */
	char nuker[12];			/* The name of the nuker */
	char unnuker[12];		/* The name of the unnuker */
	char nukee[12];			/* The name of the nukee */
	ushort mult;			/* The nuke multiplier */
	float bytes;			/* The number of bytes nuked */
	char reason[60];		/* The nuke reason */
	char dirname[255];		/* The dirname (fullpath) */
	struct nukelog *nxt;	/* Unused, kept for compatibility reasons */
	struct nukelog *prv;	/* Unused, kept for compatibility reasons */
};

enum {
	NO_ACTION = 1,
	SHOW_NEWDIRS,
	SHOW_NUKES,
	SHOW_UNNUKES
};

short int matchpath(char *instr, char *path);
void load_sysconfig(const char *config_file);
void show_newdirs(const char *pattern);
void show_nukes(const ushort status, const char *pattern);
char *trim(char *str);
void usage(const char *binary);
int wildcasecmp(const char *wild, const char *string);
short int subcomp(char *directory);

int main(int argc, char *argv[])
{
	char *config_file = GLCONF;
	char *pattern = NULL;
	int action = NO_ACTION, c;
	int free_config = 0;

	if (argc < 2) {
		usage(argv[0]);
	}

	/* Parse command line arguments */
	/* Usage: [-h] [-f] [-s] [-m <max #>] [-p <pattern>] [-r <glconf>] <-l, -n, or -u> */

	opterr = 0;
	while((c = getopt(argc, argv, "hfsm:p:r:lnu")) != -1) {
		switch(c) {
			case 'f':
				match_full = 1;
				break;
			case 'l':
				action = SHOW_NEWDIRS;
				break;
			case 'm':
				if ((max_results = atoi(optarg)) < 1) {
					max_results = 1;
				}
				break;
			case 'n':
				action = SHOW_NUKES;
				break;
			case 'p':
				pattern = strdup(optarg);
				break;
			case 'u':
				action = SHOW_UNNUKES;
				break;
			case 'r':
				config_file = strdup(optarg);
				free_config = 1;
				break;
			case 's':
				search_mode = 1;
				break;
			default:
				usage(argv[0]);
		}
	}

	load_sysconfig(config_file);

	switch(action) {
		case SHOW_NEWDIRS:
			show_newdirs(pattern);
			break;
		case SHOW_NUKES:
			show_nukes(0, pattern);
			break;
		case SHOW_UNNUKES:
			show_nukes(1, pattern);
			break;
		default:
			usage(argv[0]);
			break;
	}

	if (free_config && config_file != NULL) {
		free(config_file);
	}
	if (pattern != NULL) {
		free(pattern);
	}

	return 0;
}



short int
matchpath(char *instr, char *path)
{
        int             pos = 0;

        if ( (int)strlen(instr) < 2 || (int)strlen(path) < 2 )
                return 0;
        do {
                switch (*instr) {
                case 0:
                case ' ':
                        if (!strncmp(instr - pos, path, pos - 1)) {
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


/* load_sysconfig - Loads data from glftpd configuration file. */
void load_sysconfig(const char *config_file)
{
	FILE *fp;
	char lvalue[64];
	char rvalue[MAXPATHLEN];
	char work_buff[MAXPATHLEN];
	int x, y;

	if ((fp = fopen(config_file, "r")) == NULL) {
		fprintf(stderr, "Unable to open the config file (%s), using default values.\n", config_file);
		return;
	}

	while(fgets(work_buff, sizeof(work_buff), fp) != NULL) {
		/* Clip out comments */
		for(x = 0; x < (signed)strlen(work_buff); x++) {
			if (work_buff[x] == '#') {
				work_buff[x] = '\0';
			}
		}

		/* Trim */
		trim(work_buff);

		/* Clear out old values */
		memset(lvalue, 0, sizeof(lvalue));
		memset(rvalue, 0, sizeof(rvalue));

		/* Parse lvalue */
		y = 0;
		for(x = 0; x < (signed)strlen(work_buff) && work_buff[x] != ' '; x++) {
			if (isprint(work_buff[x])) {
				lvalue[y++] = work_buff[x];
			}
		}

		/* Parse rvalue */
		y = 0;
		x++;
		for (; x < (signed)strlen(work_buff); x++) {
			if (isprint(work_buff[x])) {
				rvalue[y++] = work_buff[x];
			}
		}

		if (strcasecmp(lvalue, "datapath") == 0) {
			strncpy(datapath, rvalue, sizeof(datapath) - 1);
			datapath[sizeof(datapath) - 1] = 0;
		}
		if (strcasecmp(lvalue, "rootpath") == 0) {
			strncpy(rootpath, rvalue, sizeof(rootpath) - 1);
			rootpath[sizeof(rootpath) - 1] = 0;
		}
	}

	fclose(fp);
	return;
}


/* trim - Trim whitespace from a string. */
char *trim(char *str)
{
	char *ibuf, *obuf;

	if (str) {
		for (ibuf = obuf = str; *ibuf;) {
			while(*ibuf && isspace(*ibuf)) {
				ibuf++;
			}
			if (*ibuf && (obuf != str)) {
				*(obuf++) = ' ';
			}
			while(*ibuf && !isspace(*ibuf)) {
				*(obuf++) = *(ibuf++);
			}
		}
		*obuf = '\0';
	}
	return (str);
}


/* show_newdirs - Display the latest specified entries in the dirlog. */
void show_newdirs(const char *pattern)
{
	FILE *fp;
	char dirlog_path[MAXPATHLEN+1];
	snprintf(dirlog_path, sizeof(dirlog_path), "%s%s/logs/dirlog", rootpath, datapath);

    if ((fp = fopen(dirlog_path, "rb")) == NULL) {
        printf("Failed to open dirlog (%s): %s\n", dirlog_path, strerror(errno));
        exit(1);
    } else {
    	struct dirlog buffer;
    	char *p;
		int i = 0;

    	fseek(fp, 0L, SEEK_END);
    	while(i < max_results) {
		if (fseek(fp, -(sizeof(struct dirlog)), SEEK_CUR) != 0) {
			break;
		}
		if (fread(&buffer, sizeof(struct dirlog), 1, fp) < 1) {
			break;
		} else {
			fseek(fp, -(sizeof(struct dirlog)), SEEK_CUR);
		}

		/* Only display newdirs unless search_mode is specified (-s) */
		if (!search_mode && buffer.status != 0) {
			continue;
		}
		if (pattern != NULL) {
			/* Pointer to the base of the directory path */
			if (!match_full && (p = strrchr(buffer.dirname, '/')) != NULL)
				p++;
			else
				p = buffer.dirname;
			if (!wildcasecmp(pattern, p))
				continue;
		}

		/* Format: status|uptime|uploader|group|files|kilobytes|dirname */
		if (!matchpath(group_dirs, buffer.dirname) && !subcomp(buffer.dirname)) {
#ifndef USE_GLFTPD132
			printf("%d|%u|%d|%d|%d|%.0f|",
				buffer.status, (unsigned int)buffer.uptime, buffer.uploader, buffer.group,
				buffer.files, (float)buffer.bytes/1024.);
#else
			printf("%d|%u|%d|%d|%d|%.0f|",
				buffer.status, (unsigned int)buffer.uptime, buffer.uploader, buffer.group,
				buffer.files, (float)buffer.bytes/1024.);
#endif
			puts(buffer.dirname);
			i++;
		}
    	}
    	fclose(fp);
    }
    return;
}


/* show_nukes - Display the latest specified entries in the nukelog. */
void show_nukes(const ushort status, const char *pattern)
{
	FILE *fp;
	char nukelog_path[MAXPATHLEN+1];
	snprintf(nukelog_path, sizeof(nukelog_path), "%s%s/logs/nukelog", rootpath, datapath);

    if ((fp = fopen(nukelog_path, "rb")) == NULL) {
        printf("Failed to open nukelog (%s): %s\n", nukelog_path, strerror(errno));
        exit(1);
    } else {
    	struct nukelog buffer;
    	char *p;
		int i = 0;

    	fseek(fp, 0L, SEEK_END);
    	while(i < max_results) {
			if (fseek(fp, -(sizeof(struct nukelog)), SEEK_CUR) != 0) {
				break;
			}
			if (fread(&buffer, sizeof(struct nukelog), 1, fp) < 1) {
				break;
			} else {
				fseek(fp, -(sizeof(struct nukelog)), SEEK_CUR);
			}

			/* Only display nukes/unnukes unless search_mode is specified (-s) */
			if (!search_mode && buffer.status != status) {
				continue;
			}

			if (pattern != NULL) {
				/* Pointer to the base of the directory path */
				if (!match_full && (p = strrchr(buffer.dirname, '/')) != NULL) {
					p++;
				} else {
					p = buffer.dirname;
				}
				if (!wildcasecmp(pattern, p)) {
					continue;
				}
			}

			/* Format: status|nuketime|nuker|unnuker|nukee|multiplier|reason|kilobytes|dirname */
			printf("%d|%u|%s|%s|%s|%d|%s|%.0f|",
				buffer.status, (unsigned int)buffer.nuketime, buffer.nuker, buffer.unnuker,
				buffer.nukee, buffer.mult, buffer.reason, buffer.bytes*1024.0);
			puts(buffer.dirname);
			i++;
    	}
    	fclose(fp);
    }
    return;
}


/* usage - Display the various parameters for showlog */
void usage(const char *binary)
{
	printf("Usage: %s [-h] [-f] [-s] [-m <max #>] [-p <pattern>] [-r <glconf>] <-l, -n, or -u>\n\n", binary);
	printf("Options:\n");
	printf("  -h  This help screen.\n");
	printf("  -f  Match the full path rather than the base name (default off).\n");
	printf("  -m  Maximum number of results to display (default %d).\n", max_results);
	printf("  -p  Display only the matching entries, you may use wildcards (?,*).\n");
	printf("  -r  Path to the glftpd configuration file (default " GLCONF ").\n");
	printf("  -s  Search mode, display all entries disregarding their status (new, deleted, nuked, etc.).\n\n");
	printf("Required Parameters:\n");
	printf("  -l  Display the latest dirlog entries.\n");
	printf("  -n  Display the latest nukes from the nukelog.\n");
	printf("  -u  Display the latest unnukes from the nukelog.\n\n");
	printf("  **  Only specify one required parameter.\n");
	exit(1);
}


/* http://www.codeproject.com/string/wildcmp.asp
 * modded by neoxed for case insensitivity
 */
int wildcasecmp(const char *wild, const char *string)
{
	const char *cp = 0, *mp = 0;

	while(*string && *wild != '*') {
		if (*wild != '?' && tolower(*wild) != tolower(*string)) {
			return 0;
		}
		wild++;
		string++;
	}

	while(*string) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if (*wild == '?' || tolower(*wild) == tolower(*string)) {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}

	while(*wild == '*') {
		wild++;
	}
	return !*wild;
}

/* check for matching subpath
   psxc - 2004-12-18
 */
short int
subcomp(char *directory)
{
	int     k = (int)strlen(directory);
	int     m = (int)strlen(subdir_list);
	int     pos = 0, l = 0, n = 0, j = 0;
	char    tstring[m + 1];
	char    bstring[k + 1];
	char    *tpos;
	char	basepath[PATH_MAX];

	if ( k < 2 )
		return 0;

	if (basepath)
		bzero(basepath, k + 1);
	if ( directory[0] == '/') {
		while (n < k) {
			if (directory[n] == '/')
				pos = n + 1;
			n++;
		}
		tpos = directory + pos;
		strncpy(bstring, tpos, k);
		pos = 0;
		n = 0;
	} else
		strncpy(bstring, directory, k + 1);
	do {
		switch (subdir_list[l]) {
		case 0:
			break;
		case ',':
			tstring[j] = '\0';
			if (n <= j && !strncasecmp(tstring, bstring, j - n)) {
				if (basepath && ((k - (j -n + 1)) > 0))
					strncpy(basepath, directory, k - (j - n + 1));
				else if (basepath)
					basepath[0] = '\0';
				return 1;
			}
			pos = l;
			n = 0;
			j=0;
			break;
		case '?':
			tstring[j] = subdir_list[l];
			tstring[j+1] = '\0';
			n++;
			j++;
			break;
		default:
			tstring[j] = subdir_list[l];
			tstring[j+1] = '\0';
			pos++;
			j++;
			break;
		}
		m--;
		l++;
	} while (m);
	if (n <= j && !strncasecmp(tstring, bstring, j - n)) {
		if (basepath && ((k - (j -n + 1)) > 0))
			strncpy(basepath, directory, k - (j - n + 1));
		else if (basepath)
			basepath[0] = '\0';
		return 1;
	}
	if (basepath)
		basepath[0] = '\0';
	return 0;
}

