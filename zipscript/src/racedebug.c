#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "objects.h"

int main (int argc, char **argv) {
 unsigned int	*fsize;
 unsigned int	*uspeed;
 unsigned int	*startsec;
 unsigned int	*startusec;
 unsigned int	*crc;
 unsigned char  buf[49 + 5 * sizeof(int)];
 char		*status = 0;
 char		*fname;
 char		*p_buf;
 char		*uname;
 char		*ugroup;
 int		len;
 int		files = 0;
 FILE		*file;

 if ( argc != 2 ) {
	printf("Usage: %s <racedata file>\n", argv[0]);
	return 0;
	}

 if ( (file = fopen(argv[1], "r")) == NULL ) {
	printf("File does not exist or could not be opened\n");
        return 0;
        }

 printf("|-[Racedata lister for project-zs]\n");
 printf("|\n");

 while ( fread(&len, sizeof(int), 1, file) == 1 ) {
	fname = malloc(len);
	fread(fname, 1, len, file);
	if ( fread(buf, 1, sizeof(buf), file) != sizeof(buf) ) {
		printf("|-[CORRUPTED]\n");
		return 0;
		}
	files++;

	p_buf= (char *)(buf + 1);
	crc		= (unsigned int *)p_buf;		p_buf += sizeof(unsigned int);
	uname	= p_buf;						p_buf += 24;
	ugroup	= p_buf;						p_buf += 24;
	fsize	= (unsigned int *)p_buf;		p_buf += sizeof(unsigned int);
	uspeed	= (unsigned int *)p_buf;		p_buf += sizeof(unsigned int);
	startsec = (unsigned int *)p_buf;		p_buf += sizeof(unsigned int);
	startusec = (unsigned int *)p_buf;

	switch ( *buf ) {
		case F_NOTCHECKED:
			status = "Not checked";
			break;
                case F_CHECKED:
			status = "OK";
			break;
                case F_BAD:
			status = "BAD";
			break;
		case F_DELETED:
			status = "Deleted";
			break;
		}

	printf("|-[File #%i]\n", files);
	printf("| Filename: %s - Status: %s - CRC-32: %08X - Filesize: %u\n", fname, status, (unsigned int)*crc, (unsigned int)*fsize);
	printf("| User    : %s - Group: %s - Speed: %0.1fKb/s\n", uname, ugroup, *uspeed / 1024. );

	free(fname);
	}
 printf("|-[EOF]\n");
 fclose(file);
 return 0;
}
