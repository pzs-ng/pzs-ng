#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>

#include "race-file.h"

int main(int argc, char **argv)
{
	RACEDATA rd;
	FILE *f;
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	if (!(f = fopen(argv[1], "r"))) {
		perror("fopen");
		return EXIT_FAILURE;
	}

	while ((fread(&rd, sizeof(RACEDATA), 1, f))) {
		printf("File:   %s\n", rd.fname);
		printf("CRC32:  %.8x\n", rd.crc32);
		printf("Size:   %llu\n", rd.size);
		printf("Time:   %i\n", (int)rd.start_time);
		printf("Status: %u\n", rd.status);
		printf("Uname:  %s\n", rd.uname);
		printf("Group:  %s\n\n", rd.group);
	}

	fclose(f);

	return EXIT_SUCCESS;
}
