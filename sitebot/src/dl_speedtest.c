#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "zsconfig.h"
#include "zsconfig.defaults.h"
#include <stdarg.h>

#define TRUE  1
#define FALSE 0
short int matchpath(char *, char *);
void debug_out(char *,...);

short int matchpath(char *instr, char *path) {
	int pos = 0, c = 0;

	if ( (int)strlen(instr) < 2 || (int)strlen(path) < 2 )
		return 0;

	do {
		switch (*instr) {
		case 0:
		case ' ':
			if ((int)strlen(path) == pos - 1 && *(path + pos - 2) != '/' && *(instr - 1) == '/')
				c = 1;
			if (!strncmp(instr - pos, path, pos - c)) {
				if (*(instr - 1) == '/')
					return 1;
				if ((int)strlen(path) >= pos) {
					if (*(path + pos) == '/' || *(path + pos) == '\0')
						return 1;
				} else
					return 1;
			}
			c = 0;
			pos = 0;
			break;
		default:
			++pos;
			break;
		}
	} while (*instr++);

	return 0;
}

void debug_out(char *fmt,...) {
#if ( debug_mode == TRUE )
	time_t          timenow;
	FILE           *file;
	va_list         ap;
	static char     debugname[] = ".debug";
#endif

        if (fmt == NULL)
                return;

#if ( debug_mode == TRUE )
        va_start(ap, fmt);
        timenow = time(NULL);

        if ((file = fopen(debugname, "a+"))) {
                fprintf(file, "%.24s - %.6d - dl_speedtest.c - ", ctime(&timenow), getpid());
                vfprintf(file, fmt, ap);
                fclose(file);
	}
        chmod(debugname, 0666);
        va_end(ap);
#endif
        return;
}

int
main (int argc, char **argv)
{
	char		filename[NAME_MAX], wdir[PATH_MAX], user[25], group[25];
	char		*p = NULL;
	struct stat	fileinfo;
	double		mbit, mbyte, mbps, mbytesps;
	FILE		*glfile;
	time_t		timenow = time(NULL);
	long long	speed;

	debug_out("dl_speedtest was initiated.\n");
	if ((argc < 2) || (strncasecmp(argv[1], "RETR ", 5))) {
		debug_out("%s: Did not receive args, or args were wrong. ($1='%s')\n", argv[0], argv[1]); 
		return 0;
	}
	p = argv[1] + 5;
	snprintf(filename, sizeof(filename), "%s", p);
	if (!getcwd(wdir, sizeof(wdir))) {
		debug_out("%s: Could not retrieve current path - getcwd() failed.\n", argv[0]);
		return 0;
	}
	if (!matchpath(speedtest_dirs, wdir)) {
		debug_out("%s: Current path does not match speedtest_dirs (%s not in %s).\n", argv[0], wdir, speedtest_dirs);
		return 0;
	}
	if (getenv("USER") && getenv("GROUP") && getenv("SPEED")) {
		snprintf(user, sizeof(user) - 1, "%s", getenv("USER"));
		if (!strlen(user))
			sprintf(user, "NoUser");
		snprintf(group, sizeof(group) - 1, "%s", getenv("GROUP"));
		if (!strlen(group))
			sprintf(group, "NoGroup");
		if ((speed = strtol(getenv("SPEED"), NULL, 10)) == 0) {
			mbps = 1 * 1024. * 8. / 1000. / 1000.;
			mbytesps = 1 * 1024. / 1024. / 1024.;
		} else {
			mbps = speed * 1024. * 8. / 1000. / 1000.;
			mbytesps = speed * 1024. / 1024. / 1024.;
		}
		if (stat(filename, &fileinfo)) {
			debug_out("%s: Could not stat() file %s.\n", argv[0], filename);
			return 0;
		}
		if (fileinfo.st_size == 0) {
			mbyte = (double)1 / 1024. / 1024.;
			 mbit = (double)1 / 1000. / 1000.;
		} else {
			mbyte = (double)fileinfo.st_size / 1024. / 1024.;
			mbit = (double)fileinfo.st_size / 1000. / 1000.;
		}
		if (!(glfile = fopen(log, "a+"))) {
			debug_out("%s: Unable to fopen() %s for appending.\n", argv[0], log);
			return 0;
		}
		fprintf(glfile, "%.24s DLTEST: \"%s\" {%s} {%s} {%.2f} {%.2f} {%.1f} {%.1f}\n", ctime(&timenow), wdir, user, group, mbps, mbytesps, mbyte, mbit);
		fclose(glfile);
	}
	return 0;
}
