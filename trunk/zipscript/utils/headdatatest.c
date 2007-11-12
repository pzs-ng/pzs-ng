#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>

#include "race-file.h"

int main(int argc, char **argv)
{
	HEADDATA sd;
	FILE *f;
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	if (!(f = fopen(argv[1], "r"))) {
		perror("fopen");
		return EXIT_FAILURE;
	}

	
	fread(&sd, sizeof(HEADDATA), 1, f);
	printf("data version  : %d\n", sd.data_version);
	printf("release type  : %d\n", sd.data_type);
	printf("locked        : %d\n", sd.data_in_use);
	printf("data inc.     : %d\n", sd.data_incrementor);
	printf("queue         : %d/%d\n", sd.data_qcurrent, sd.data_queue);
	printf("process pid   : %d\n", sd.data_pid);
	printf("completed flag: %d\n", sd.data_completed);

	fclose(f);

	return EXIT_SUCCESS;
}
