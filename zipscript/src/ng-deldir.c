/*
 * Marks a directory as deleted in dirlog
 * @version 20110921
 * @author Sked
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <inttypes.h>
#include <errno.h>
#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifndef HAVE_STRLCPY
# include "strl/strl.h"
#endif

/* Force structure alignment to 4 bytes (for 64bit support). */
#if ( GLVERSION != 20164 )
#pragma pack(push, 4)
#endif

/* 32-bit time values (for 64bit support). */
typedef int32_t time32_t;

#if ( GLVERSION == 13232 )
struct dirlog {
    uint16_t    status;          /* 0 = NEWDIR, 1 = NUKE, 2 = UNNUKE, 3 = DELETED */
    time32_t    uptime;          /* Creation time since epoch (man 2 time) */
    uint16_t    uploader;        /* The userid of the creator */
    uint16_t    group;           /* The groupid of the primary group of the creator */
    uint16_t    files;           /* The number of files inside the dir */
    int32_t     bytes;           /* The number of bytes in the dir */
    char        dirname[255];    /* The name of the dir (fullpath) */
    char        dummy[8];        /* Unused, kept for compatibility reasons */
} __attribute__((deprecated));
#elif ( GLVERSION == 20164 )
struct dirlog {
    ushort      status;          /* 0 = NEWDIR, 1 = NUKE, 2 = UNNUKE, 3 = DELETED */
    time_t      uptime;          /* Creation time since epoch (man 2 time) */
    ushort      uploader;        /* The userid of the creator */
    ushort      group;           /* The groupid of the primary group of the creator */
    ushort      files;           /* The number of files inside the dir */
    unsigned long long bytes;    /* The number of bytes in the dir */
    char        dirname[255];    /* The name of the dir (fullpath) */
    struct dirlog *nxt;          /* Unused, kept for compatibility reasons */
    struct dirlog *prv;          /* Unused, kept for compatibility reasons */
};
#else
/* 20132 & 20032 & 20232 & 20264 */
struct dirlog {
    uint16_t    status;          /* 0 = NEWDIR, 1 = NUKE, 2 = UNNUKE, 3 = DELETED */
    time32_t    uptime;          /* Creation time since epoch (man 2 time) */
    uint16_t    uploader;        /* The userid of the creator */
    uint16_t    group;           /* The groupid of the primary group of the creator */
    uint16_t    files;           /* The number of files inside the dir */
    uint64_t    bytes;           /* The number of bytes in the dir */
    char        dirname[255];    /* The name of the dir (fullpath) */
    char        dummy[8];        /* Unused, kept for compatibility reasons */
};
#endif

/* Restore default structure alignment for non-critical structures. */
#if ( GLVERSION != 20164 )
#pragma pack(pop)
#endif

int 
main(int argc, char *argv[])
{
	FILE *fp, *fp2;
	char rlsname[1024], data2[1024], dirlog[1024];
	struct dirlog buffer;

	if (argc != 2) {
		printf("Please give a directory to undupe as well\n");
		return 1;
	}

	strlcpy(dirlog, dirlogpath, 1024);
	strlcpy(rlsname, argv[1], 1024);
	sprintf(data2, "%s/dirlog.%d", storage, (int)getuid());

	if (!(fp = fopen(dirlog, "r+b"))) {
		printf("FATAL ERROR: Unable to open dirlog (%s)\n", dirlog);
		return 1;
	}
	if (!(fp2 = fopen(data2, "w+b"))) {
		printf("FATAL ERROR: Unable to write to tempfile (%s)\n", data2);
		fclose(fp);
		return 1;
	}

	while (!feof(fp)) {
		if (fread(&buffer, sizeof(struct dirlog), 1, fp) < 1)
			break;
		/* If we find the rlsname, mark it deleted */
		if (strcmp(buffer.dirname, rlsname) == 0)
			buffer.status = 3;
		/* Write it to the new file, if we can */
		if (fwrite(&buffer, sizeof(struct dirlog), 1, fp2) < 1)
			break;
	}

	fclose(fp);
	fclose(fp2);

	/*
	 * Time to put back the remainder of the dirlog. We try renaming the
	 * tempfile first, if this fails we try streaming the contents of the
	 * tempfile to the original file. Streaming will work where renaming
	 * fails if the directory is not writable for the user.
	 */

	if (rename(data2, dirlog) == -1) {
		if (!(fp = fopen(data2, "r+b"))) {
			printf("FATAL ERROR: Unable to open tempfile (%s)\n", data2);
			return 1;
		}
		if (!(fp2 = fopen(dirlog, "w+b"))) {
			printf("FATAL ERROR: Unable to write to dirlog (%s)\n", dirlog);
			fclose(fp);
			return 1;
		}

		while (!feof(fp)) {
			if (fread(&buffer, sizeof(struct dirlog), 1, fp) < 1)
				break;
			if (fwrite(&buffer, sizeof(struct dirlog), 1, fp2) < 1)
				break;
		}

		fclose(fp);
		fclose(fp2);

		if (chmod(dirlog, 0666))
			printf("WARNING: Failed to chmod %s: %s\n", dirlog, strerror(errno));

		if (unlink(data2) > 0) {
			printf("FATAL ERROR: Unable to delete tempfile (%s)\n", data2);
			return 1;
		}
	}

	return 0;
}
