#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "zsconfig.h"
#include "zsconfig.defaults.h"

short int matchpath(char *, char *);

short int
matchpath(char *instr, char *path)
{
	int             pos = 0;

	if ( (int)strlen(instr) < 2 || (int)strlen(path) < 2 ) {
		return 0;
	}
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

int
main (int argc, char **argv)
{
	char		filename[NAME_MAX], wdir[PATH_MAX];
	char	       *p = NULL;
	struct stat	fileinfo;
	double		mbit, mbyte, mbps;
	FILE		*glfile;
	time_t          timenow = time(NULL);

	if ((argc < 2) || (strncasecmp(argv[1], "RETR ", 5)))
		return 0;
	p = argv[1] + 5;
	snprintf(filename, sizeof(filename), p);
	if (!getcwd(wdir, sizeof(wdir)))
		return 0;
	if (!matchpath(speedtest_dirs, wdir))
		return 0;
	if (getenv("USER") && getenv("GROUP") && getenv("SPEED")) {
		mbps = (double)strtol(getenv("SPEED"), NULL, 0) * 1024. * 8. / 1000. / 1000.;
		if (stat(filename, &fileinfo))
			return 0;
		mbyte = (double)fileinfo.st_size / 1024. / 1024.;
		mbit = (double)fileinfo.st_size / 1000. / 1000.;
		if (!(glfile = fopen(log, "a+")))
			return 0;
		fprintf(glfile, "%.24s DLTEST: \"%s\" {%s} {%s} {%.1f} {%.2f} {%.2f}\n", ctime(&timenow), wdir, getenv("USER"), getenv("GROUP"), mbit, mbyte, mbps);
		fclose(glfile);
	}
	return 0;
}

