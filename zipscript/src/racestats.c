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

struct USERINFO **userI;
struct GROUPINFO **groupI;
struct VARS	raceI;
struct LOCATIONS locations;


char		error_msg [80], output[2048], output2[1024];
short int	varelease = 2,	/* Various Artists release */
		ERROR_CODE = 0;

void 
getrelname(char *directory)
{
	int		cnt       , l, n = 0, k = 2;
	char		directoryarray[2][PATH_MAX];

	for (cnt = strlen(directory) - 1; k && cnt; cnt--)
		if (directory[cnt] == '/') {
			k--;
			//directoryarray[k] = malloc(n + 1);
			strncpy(directoryarray[k], directory + cnt + 1, n);
			directoryarray[k][n] = 0;
			n = 0;
		} else
			n++;

	l = strlen(directoryarray[1]);

	if (subcomp(directoryarray[1])) {
		//n = strlen(directoryarray[0]);
		//raceI.misc.release_name = malloc(n + 21);
		sprintf(raceI.misc.release_name, "%s/%s", directoryarray[0], directoryarray[1]);
		//if (k < 2)
		//	free(directoryarray[1]);
	} else {
		//raceI.misc.release_name = malloc(l + 12);
		sprintf(raceI.misc.release_name, "%s", directoryarray[1]);
		//if (k == 0)
		//	free(directoryarray[0]);
	}
}

/*
 * CORE CODE - NOT MUCH HERE - JUST CALLS FUNCTIONS IN RIGHT ORDER & SET FEW
 * VARIABLES
 */
int 
main(int argc, char **argv)
{
	int		n;

	if (argc == 1) {
		printf("Usage: %s <path>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	locations.path = malloc(n = strlen(argv[1]) + 1);
	locations.race = malloc(n += 10 + strlen(storage));
	locations.sfv = malloc(n + 10);

	userI = malloc(sizeof(struct USERINFO *) * 30);
	memset(userI, 0, sizeof(struct USERINFO *) * 30);
	groupI = malloc(sizeof(struct GROUPINFO *) * 30);
	memset(groupI, 0, sizeof(struct GROUPINFO *) * 30);

	raceI.misc.slowest_user[0] = 30000;
	raceI.misc.fastest_user[0] =
		raceI.total.speed =
		raceI.total.files_missing =
		raceI.total.files =
		raceI.total.size =
		raceI.total.users =
		raceI.total.groups = 0;
	raceI.file.name = ".";

	strcpy(locations.path, argv[1]);

	n = strlen(locations.path);
	if (locations.path[n] == '/') {
		locations.path[n] = 0;
	}
	if (chdir(locations.path))
		goto END;

	getrelname(locations.path);

	sprintf(locations.race, storage "/%s/racedata", argv[1]);
	if (!fileexists(locations.race))
		goto END;

	readrace_file(&locations, &raceI, userI, groupI);
	sprintf(locations.sfv, storage "/%s/sfvdata", argv[1]);

	if (!fileexists(locations.sfv)) {
		if (fileexists(locations.sfv)) {
			raceI.total.files = read_diz("file_id.diz");
			raceI.total.files_missing += raceI.total.files;
		} else {
			raceI.total.files -= raceI.total.files_missing;
			raceI.total.files_missing = 0;
		}
	} else {
		readsfv_file(&locations, &raceI, 0);
	}

	sortstats(&raceI, userI, groupI);
	if (!raceI.total.users)
		goto END;

	printf("%s\n", convert(&raceI, userI, groupI, stats_line));

END:
	free(locations.path);
	free(locations.race);
	//free(raceI.misc.release_name);
	free(locations.sfv);

	exit(EXIT_SUCCESS);
}
