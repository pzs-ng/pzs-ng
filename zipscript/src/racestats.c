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
#include "convert.h"
#include "dizreader.h"
#include "stats.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

int 
main(int argc, char **argv)
{
	int		n=0;
	GLOBAL		g;

	if (argc == 1) {
		printf("Usage: %s <path>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	g.l.race = malloc(n += 10 + strlen(storage));
	g.l.sfv = malloc(n + 10);

	g.ui = malloc(sizeof(struct USERINFO *) * 30);
	memset(g.ui, 0, sizeof(struct USERINFO *) * 30);
	g.gi = malloc(sizeof(struct GROUPINFO *) * 30);
	memset(g.gi, 0, sizeof(struct GROUPINFO *) * 30);

	g.v.misc.slowest_user[0] = 30000;
	g.v.misc.fastest_user[0] =
		g.v.total.speed =
		g.v.total.files_missing =
		g.v.total.files =
		g.v.total.size =
		g.v.total.users =
		g.v.total.groups = 0;

	g.v.file.name[0] = '.';
	g.v.file.name[1] = 0;

	strcpy(g.l.path, argv[1]);

	n = strlen(g.l.path);
	if (g.l.path[n] == '/') {
		g.l.path[n] = 0;
	}
	if (chdir(g.l.path))
		goto END;

	getrelname(&g);

	sprintf(g.l.race, storage "/%s/racedata", argv[1]);
	if (!fileexists(g.l.race))
		goto END;

	readrace_file(&g.l, &g.v, g.ui, g.gi);
	sprintf(g.l.sfv, storage "/%s/sfvdata", argv[1]);

	if (!fileexists(g.l.sfv)) {
		if (fileexists(g.l.sfv)) {
			g.v.total.files = read_diz("file_id.diz");
			g.v.total.files_missing += g.v.total.files;
		} else {
			g.v.total.files -= g.v.total.files_missing;
			g.v.total.files_missing = 0;
		}
	} else {
		readsfv_file(&g.l, &g.v, 0);
	}

	sortstats(&g.v, g.ui, g.gi);
	if (!g.v.total.users)
		goto END;

	printf("%s\n", convert(&g.v, g.ui, g.gi, stats_line));

END:
	free(g.l.race);
	free(g.l.sfv);

	exit(EXIT_SUCCESS);
}
