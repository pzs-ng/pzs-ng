#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>

#include "race-file.h"

int main(int argc, char **argv)
{
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

	printf(".---------------------------------------------------------.\n" \
	       "|                FILENAMES                   |  CRC#  | # |\n" \
	       "|--------------------------------------------|--------|---|\n");

	while ((fread(&sd, sizeof(SFVDATA), 1, f))) {
		printf("|%-44s|%.8x| %d |\n", sd.fname, sd.crc32, sd.fmatch);
	}

	printf("`---------------------------------------------------------'\n");
	fclose(f);

	return EXIT_SUCCESS;
}
