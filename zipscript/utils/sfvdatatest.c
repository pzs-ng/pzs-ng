#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>

#include "../include/race-file.h"

int main(int argc, char **argv)
{
	int release_type = 0;
	SFVDATA sd;
	FILE *f;
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	if (!(f = fopen(argv[1], "r"))) {
		perror("fopen");
		return EXIT_FAILURE;
	}

	fread(&release_type, sizeof(short int), 1, f);
	printf("Release type: %i\n\n", release_type);

	while ((fread(&sd, sizeof(SFVDATA), 1, f))) {
		printf("%s %.8x\n", sd.fname, sd.crc32);
	}

	fclose(f);

	return EXIT_SUCCESS;
}
