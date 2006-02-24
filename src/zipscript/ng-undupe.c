/*
 * UnDUPE.C - Clean the dupefile of unwanted data Works as far as i know
 * evilution @ efnet 02-25-99
 *
 * Fixed USAGE spelling error - Turranius 2004-01-04
 *
 * Modded/stripped for use with pzs-ng - psxc 2004-07-14
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include "zsconfig.h"
#include "zsconfig.defaults.h"
#include "glstructs.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifndef HAVE_STRLCPY
# include "strlcpy.h"
#endif

int
main(int argc, char *argv[])
{
	FILE           *fp, *fp2;
	char		dupename  [1024], data2[1024], dupefile[1024];

	struct dupefile buffer;

	if (argc != 2) {
		printf("Please give a filename to undupe as well\n");
		return 1;
	}
	strlcpy(dupefile, dupepath, 1024);
	strlcpy(dupename, argv[1], 1024);

	sprintf(data2, "%s/dupefile.%d", storage, (int)getuid());

	if (!(fp = fopen(dupefile, "r+b"))) {
		printf("FATAL ERROR: Unable to open dupefile (%s)\n", dupefile);
		return 1;
	}
	if (!(fp2 = fopen(data2, "w+b"))) {
		printf("FATAL ERROR: Unable to write to tempfile (%s)\n", data2);
		return 1;
	}
	while (!feof(fp)) {

		if (fread(&buffer, sizeof(struct dupefile), 1, fp) < 1)
			break;
		/* If we found the file, delete it */
		if (strcmp(buffer.filename, dupename) == 0)
			fflush(fp);
		/* if not, write it to the new file */
		if (strcmp(buffer.filename, dupename) != 0)
			if (fwrite(&buffer, sizeof(struct dupefile), 1, fp2) < 1)
				break;
	}

	fclose(fp);
	fclose(fp2);

	/*
	 * Time to put back the remainder of the dupefile. Instead of
	 * renaming the file as was done before, we stream the content
	 * back - this is a workaround for a world writable logs
	 * directory...
	 */

	if (!(fp = fopen(data2, "r+b"))) {
		printf("FATAL ERROR: Unable to open tempfile (%s)\n", data2);
		return 1;
	}
	if (!(fp2 = fopen(dupefile, "w+b"))) {
		printf("FATAL ERROR: Unable to write to dupefile (%s)\n", dupefile);
		return 1;
	}
	while (!feof(fp)) {
		if (fread(&buffer, sizeof(struct dupefile), 1, fp) < 1)
			break;
		if (fwrite(&buffer, sizeof(struct dupefile), 1, fp2) < 1)
			break;
	}

	fclose(fp);
	fclose(fp2);

	chmod(dupefile, 0666);
	if (unlink(data2) > 0) {
		printf("FATAL ERROR: Unable to delete tempfile (%s)\n", data2);
		return 1;
	}
	return 0;
}

