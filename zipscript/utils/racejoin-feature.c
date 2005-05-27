#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <strings.h>
#include <string.h>

#include "race-file.h"

int main(int argc, char **argv)
{
	RACEDATA	rd, *tempd;
	FILE	       *f;
	int		u_total, u_count, n_flag;
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	if (!(f = fopen(argv[1], "r"))) {
		perror("fopen");
		return EXIT_FAILURE;
	}

	u_total = 0;
	tempd = 0;
	while ((fread(&rd, sizeof(RACEDATA), 1, f))) {
		u_count = 0;
		n_flag = 0;
		while (u_count < u_total) {
			if (!strncmp(tempd[u_count].uname, rd.uname, strlen(rd.uname)) && strlen(rd.uname) == strlen(tempd[u_count].uname)) {
				n_flag=1;
				if (tempd[u_count].start_time > rd.start_time)
					memcpy(&tempd[u_count], &rd, sizeof(RACEDATA));
			}
			u_count++;
		}
		if (!n_flag) {
			u_total++;
			if ((tempd = realloc(tempd, sizeof(RACEDATA) * u_total)))
				memcpy(&tempd[u_count], &rd, sizeof(RACEDATA));
			else {
				/* failed to allocate mem */
				u_total--;
			}
		}
	}
	fclose(f);
	printf("total racers: %d\n", u_total);
	u_count=0;
	while (u_count < u_total) {
		if (u_count)
			printf("%d. racer %s/%s joined %d after %s/%s\n",u_count + 1, tempd[u_count].uname, tempd[u_count].group, (int)tempd[u_count].start_time - (int)tempd[0].start_time, tempd[0].uname, tempd[0].group);
		else
			printf("%d. racer %s/%s started the race.\n",u_count + 1, tempd[u_count].uname, tempd[u_count].group);
		u_count++;
	}
	if (tempd)
		free(tempd);
	return EXIT_SUCCESS;
}
