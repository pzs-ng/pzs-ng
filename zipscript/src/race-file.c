#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "crc.h"
#include "race-file.h" 

#include "objects.h"
#include "macros.h"

#ifdef _WITH_SS5
#include "constants.ss5.h"
#else
#include "constants.h"
#endif

#include "stats.h"
#include "zsfunctions.h"

#include "helpfunctions.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifndef HAVE_STRLCPY
# include "strl/strl.h"
#endif


/*
 * Modified	: 02.19.2002 Author	: Dark0n3
 * Modified	: 06.02.2005 by		: js
 * Description	: Creates directory where all race information will be
 * stored.
 */
void 
maketempdir(char *path)
{
	char		full_path[PATH_MAX], *p;

	snprintf(full_path, PATH_MAX, "%s/%s", storage, path);

	/* work recursively */
	for (p = full_path; *p; p++) {
		if (*p == '/') {
			*p = '\0';
			mkdir(full_path, 0777);
			*p = '/';
		}
	}

	/* the final entry */
	mkdir(full_path, 0777);
}

/*
 * Modified	: 01.16.2002 Author	: Dark0n3
 * 
 * Description	: Reads crc for current file from preparsed sfv file.
 */
unsigned int 
readsfv(const char *path, struct VARS *raceI, int getfcount)
{
	int		insfv = 0;
	unsigned int	crc = 0;
	FILE		*sfvfile;
	DIR		*dir;
	
	SFVDATA		sd;

	if (!(sfvfile = fopen(path, "r"))) {
		d_log("readsfv: Failed to open sfv (%s): %s\n", path, strerror(errno));
		return 0;
	}

	if (!update_lock(raceI, 1, 0)) {
		d_log("readsfv: Lock is suggested removed. Will comply and exit\n");
		remove_lock(raceI);
		exit(EXIT_FAILURE);
	}

	raceI->misc.release_type = raceI->data_type;

	d_log("readsfv: Reading data from sfv for (%s)\n", raceI->file.name);
	
	dir = opendir(".");
	
	raceI->total.files = 0;
	while (fread(&sd, sizeof(SFVDATA), 1, sfvfile)) {
		raceI->total.files++;
#if (sfv_cleanup && sfv_cleanup_lowercase)
		if (!strcasecmp(raceI->file.name, sd.fname))
#else
		if (!strcmp(raceI->file.name, sd.fname))
#endif
		{
			d_log("readsfv: crc read from sfv-file (%s): %.8x\n", sd.fname, sd.crc32);
			crc = sd.crc32;
			insfv = 1;
		}
		if (getfcount && findfile(dir, sd.fname)) {
			raceI->total.files_missing--;
		}
	}
	
	closedir(dir);
	fclose(sfvfile);
	
	raceI->total.files_missing += raceI->total.files;
	
	if (insfv)
		errno = 1;
	else
		errno = 0;

	return crc;
}

void
update_sfvdata(const char *path, const unsigned int crc)
{
	int		fd, count;
	
	SFVDATA		sd;

	if ((fd = open(path, O_RDWR, 0666)) == -1) {
		d_log("update_sfvdata: Failed to open sfvdata (%s): %s\n", path, strerror(errno));
		return;
	}

	sd.crc32 = crc;
	count = 0;
	while (read(fd, &sd, sizeof(SFVDATA))) {
		if (strcasecmp(path, sd.fname) == 0) {
			sd.crc32 = crc;
			break;
		}
		count++;
	}
	
	lseek(fd, sizeof(SFVDATA) * count, SEEK_SET);
	write(fd, &sd, sizeof(SFVDATA));
	
	close(fd);
}

/* convert the sfvdata file source to the sfv dest */
void
sfvdata_to_sfv(const char *source, const char *dest)
{
	int		infd, outfd;
	char		crctmp[8];
	
	SFVDATA		sd;

	if ((infd = open(source, O_RDONLY)) == -1) {
		d_log("sfvdata_to_sfv: Failed to open (%s): %s\n", source, strerror(errno));
		return;
	}

	if ((outfd = open(source, O_CREAT | O_WRONLY, 0644)) == -1) {
		d_log("sfvdata_to_sfv: Failed to open (.tmpsfv): %s\n", strerror(errno));
		return;
	}

	while (read(infd, &sd, sizeof(SFVDATA))) {
		
		sprintf(crctmp, "%.8x", sd.crc32);
		
		write(outfd, sd.fname, NAME_MAX);
		write(outfd, " ", 1);
		write(outfd, &crctmp, 8),
#if (sfv_cleanup_crlf == TRUE )
		write(outfd, "\r", 1);
#endif
		write(outfd, "\n", 1);

	}
	
	close(infd);
	close(outfd);

	rename(".tmpsfv", dest);	
}

/*
 * Modified	: 01.16.2002 Author	: Dark0n3
 * 
 * Description	: Deletes all -missing files with preparsed sfv.
 */
void 
delete_sfv(const char *path)
{
	char		*f = 0, missing_fname[NAME_MAX];
	FILE		*sfvfile;

	SFVDATA		sd;

	if (!(sfvfile = fopen(path, "r"))) {
		d_log("delete_sfv: Couldn't fopen %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	while (fread(&sd, sizeof(SFVDATA), 1, sfvfile)) {
		snprintf(missing_fname, NAME_MAX, "%s-missing", sd.fname);
		if ((f = findfilename(missing_fname, f)))
			unlink(missing_fname);
	}
	
	fclose(sfvfile);
}

/*
 * Modified	: 01.16.2002 Author	: Dark0n3
 * 
 * Description	: Reads name of old race leader and writes name of new leader
 * 		  into temporary file.
 * 
 */
void 
read_write_leader(const char *path, struct VARS *raceI, struct USERINFO *userI)
{
	int		fd;
	struct stat	sb;

	if ((fd = open(path, O_CREAT | O_RDWR, 0666)) == -1) {
		d_log("read_write_leader: open(%s): %s\n", path, strerror(errno));
		return;
	}
	
	if (!update_lock(raceI, 1, 0)) {
		d_log("read_write_leader: Lock is suggested removed. Will comply and exit\n");
		remove_lock(raceI);
		exit(EXIT_FAILURE);
	}

	fstat(fd, &sb);

	if (sb.st_size == 0) {
		*raceI->misc.old_leader = '\0';
	} else {
		read(fd, &raceI->misc.old_leader, 24);
		lseek(fd, 0L, SEEK_SET);
	}

	write(fd, userI->name, 24);
	
	close(fd);
}

/*
 * Modified	: 01.16.2002 Author	: Dark0n3
 * 
 * Description	: Goes through all untested files and compares crc of file
 * with one that is reported in sfv.
 * 
 */
void 
testfiles(struct LOCATIONS *locations, struct VARS *raceI, int rstatus)
{
	int		fd, lret, count;
	char		*realfile, *ext, target[256];
	unsigned int	Tcrc;
	struct stat	filestat;
	time_t		timenow;

	RACEDATA	rd;

	if ((fd = open(locations->race, O_CREAT | O_RDWR, 0666)) == -1) {
		if (errno != EEXIST) {
			d_log("testfiles: open(%s): %s\n", locations->race, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	

	realfile = raceI->file.name;

	if (rstatus)
		printf("\n");

	count = 0;
	while ((read(fd, &rd, sizeof(RACEDATA)))) {
		if (!update_lock(raceI, 1, 0)) {
			d_log("testfiles: Lock is suggested removed. Will comply and exit\n");
			remove_lock(raceI);
			exit(EXIT_FAILURE);
		}

		d_log("testfiles: Checking '%s'\n", rd.fname);
		ext = find_last_of(realfile, ".");
		if (*ext == '.')
			ext++;
		if (rd.status == F_NOTCHECKED) {
			strlcpy(raceI->file.name, rd.fname, NAME_MAX);
			Tcrc = readsfv(locations->sfv, raceI, 0);
			stat(rd.fname, &filestat);
			timenow = time(NULL);
			if (S_ISDIR(filestat.st_mode))
				rd.status = F_IGNORED;
			else if (rd.crc32 != 0 && Tcrc == rd.crc32)
				rd.status = F_CHECKED;
			else if (rd.crc32 != 0 && strcomp(ignored_types, ext))
				rd.status = F_IGNORED;
			else if (rd.crc32 != 0 && Tcrc == 0 && strcomp(allowed_types, ext))
				rd.status = F_IGNORED;
			else if ((rd.crc32 != 0) && (Tcrc != rd.crc32) &&
					   (strcomp(allowed_types, ext) &&
				   !matchpath(allowed_types_exemption_dirs, locations->path)))
				rd.status = F_IGNORED;
			else if	((timenow == filestat.st_ctime) && (filestat.st_mode & 0111)) {
				d_log("testfiles: Looks like this file (%s) is in the process of being uploaded. Ignoring.\n", rd.fname);
				rd.status = F_IGNORED;
				create_missing(rd.fname);
			} else {
				d_log("testfiles: Marking file (%s) as bad and removing it.\n", rd.fname);
				mark_as_bad(rd.fname);
				if (rd.fname)
					unlink(rd.fname);
				rd.status = F_BAD;

#if ( create_missing_files )
				if (Tcrc != 0)
					create_missing(rd.fname);
#endif

				if (rstatus)
					printf("File: %s FAILED!\n", rd.fname);

				d_log("testfiles: marking %s bad.\n", rd.fname);
				if (enable_unduper_script == TRUE) {
					if (!fileexists(unduper_script)) {
						d_log("Failed to undupe '%s' - '%s' does not exist.\n",
							  rd.fname, unduper_script);
					} else {
						sprintf(target, unduper_script " \"%s\"", rd.fname);
						if (execute(target) == 0)
							d_log("testfiles: undupe of %s successful.\n", rd.fname);
						else
							d_log("testfiles: undupe of %s failed.\n", rd.fname);
					}
				}
			}
			if (rd.status == F_BAD) {
				remove_from_race(locations->race, rd.fname);
			} else {
				if ((lret = lseek(fd, sizeof(RACEDATA) * count, SEEK_SET)) == -1) {
					d_log("testfiles: lseek: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
				write(fd, &rd, sizeof(RACEDATA));
				if (!((timenow == filestat.st_ctime) && (filestat.st_mode & 0111)))
					unlink_missing(rd.fname);
			}
			count++;
		}
	}
	strlcpy(raceI->file.name, realfile, strlen(realfile)+1);
	raceI->total.files = raceI->total.files_missing = 0;
	close(fd);
}

/*
 * Modified	: 01.20.2002 Author	: Dark0n3
 *
 * Description	: Parses file entries from sfv file and store them in a file.
 * 
 * Todo		: Add dupefile remover.
 *
 * Totally rewritten by js on 08.02.2005
 */
int
copysfv(const char *source, const char *target, struct VARS *raceI)
{
	int		infd, outfd, i, retval = 0;
	short int	music, rars, video, others, type;
	
	char		*ptr, fbuf[2048];
	FILE		*insfv;

	DIR		*dir;

	SFVDATA		sd;

#if ( sfv_dupecheck == TRUE )
	int		skip = 0;
	SFVDATA		tempsd;
#endif
	
#if ( sfv_cleanup == TRUE )
	int		tmpfd;
	char		crctmp[8];
	
	if ((tmpfd = open(".tmpsfv", O_CREAT | O_TRUNC | O_RDWR, 0644)) == -1)
		d_log("copysfv: open(.tmpsfv): %s\n", strerror(errno));
#endif

	if ((infd = open(source, O_RDONLY)) == -1) {
		d_log("copysfv: open(%s): %s\n", source, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if ((outfd = open(target, O_CREAT | O_TRUNC | O_RDWR, 0666)) == -1) {
		d_log("copysfv: open(%s): %s\n", target, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	video = music = rars = others = type = 0;

	dir = opendir(".");

	if (!update_lock(raceI, 1, 0)) {
		d_log("copysfv: Lock is suggested removed. Will comply and exit\n");
		remove_lock(raceI);
		exit(EXIT_FAILURE);
	}

	insfv = fdopen(infd, "r");
	while ((fgets(fbuf, sizeof(fbuf), insfv))) {
		
		/* remove comment */
		if ((ptr = find_first_of(fbuf, ";")))
			*ptr = '\0';

		strip_whitespaces(fbuf);
		ptr = prestrip_whitespaces(fbuf);
		if (ptr != fbuf)
			d_log("copysfv: prestripped whitespaces (%d chars)\n", ptr - fbuf);

		if (strlen(ptr) == 0)
			continue;
	
#if (sfv_cleanup == TRUE)
#if (sfv_cleanup_lowercase == TRUE)
		for (; *ptr; ptr++)
			*ptr = tolower(*ptr);
#endif
#endif
		sd.crc32 = 0;
		bzero(sd.fname, sizeof(sd.fname));
		if ((ptr = find_last_of(fbuf, " \t"))) {
			
			/* pass the " \t" */
			ptr++;
			
			/* what we have now is hopefully a crc */
			for (i = 0; isxdigit(*ptr) != 0; i++)
				ptr++;
			
			ptr -= i;
			if (i > 8 || i < 6) {
				/* we didn't get an 8 digit crc number */
#if (sfv_cleanup == TRUE)
				/* do stuff  */
#else
				retval = 1;
				goto END;
#endif
			} else {
				sd.crc32 = hexstrtodec(ptr);
				
				/* cut off crc string */
				*ptr = '\0';
				
				/* nobody should be stupid enough to have spaces
				 * at the end of the file name */
				strip_whitespaces(fbuf);
			}
		
		} else {
			/* we have a filename only. */
#if (sfv_cleanup == TRUE)
			/* do stuff  */
#else
			retval = 1;
			goto END;
#endif
		}

		/* we assume what's left is a filename */
		ptr = prestrip_whitespaces(fbuf);
		if (ptr != fbuf)
			d_log("copysfv: prestripped whitespaces (%d chars)\n", ptr - fbuf);

		if (strlen(ptr) > 0 && strlen(ptr) < NAME_MAX-9 ) {
			strlcpy(sd.fname, ptr, NAME_MAX-9);

			if (sd.fname != find_last_of(sd.fname, "\t/") || *sd.fname == '/') {
				d_log("copysfv: found '/' or TAB as part of filename in sfv - logging file as bad.\n");
				retval = 1;
				break;
			}

#if (sfv_calc_single_fname == TRUE)
			/* TODO */
			/* calculate file's crc if it exists */
			if (sd.crc32 == 0) {
				d_log("copysfv: Got filename (%s) without crc, trying to calculate.\n", sd.fname);
				sd.crc32 = calc_crc32(sd.fname);
			}
#endif
			
			/* get file extension */
			ptr = find_last_of(fbuf, ".");
			if (*ptr == '.')
				ptr++;
			
			if (!strcomp(ignored_types, ptr)) {

#if ( sfv_dupecheck == TRUE )
				/* read from sfvdata - no parsing */
				skip = 0;
				lseek(outfd, 0L, SEEK_SET);
				while (read(outfd, &tempsd, sizeof(SFVDATA)))
					if (strcmp(sd.fname, tempsd.fname) == 0)
						skip = 1;
				lseek(outfd, 0L, SEEK_END);

				if (skip)
					continue;
#endif

				d_log("copysfv:  File in sfv: '%s' (%x)\n", sd.fname, sd.crc32);

#if ( sfv_cleanup == TRUE )
				/* write good stuff to .tmpsfv */
				if (tmpfd != -1) {
					sprintf(crctmp, "%.8x", sd.crc32);
					write(tmpfd, sd.fname, strlen(sd.fname));
					write(tmpfd, " ", 1);
					write(tmpfd, crctmp, 8);
#if (sfv_cleanup_crlf == TRUE )
					write(tmpfd, "\r", 1);
#endif
					write(tmpfd, "\n", 1);
				}
#endif

				if (strncasecmp(ptr, "mp3", 4) == 0)
					music++;
				else if (israr(ptr))
					rars++;
				else if (isvideo(ptr))
					video++;
				else
					others++;
				
#if ( create_missing_files == TRUE )
				if (!findfile(dir, sd.fname))
					create_missing(sd.fname);
#endif

				write(outfd, &sd, sizeof(SFVDATA));
			}
		}
	}
	
	if (music > rars) {
		if (video > music)
			type = (video >= others ? 4 : 2);
		else
			type = (music >= others ? 3 : 2);
	} else {
		if (video > rars)
			type = (video >= others ? 4 : 2);
		else
			type = (rars >= others ? 1 : 2);
	}

#if ( sfv_cleanup == FALSE )
END:
#endif
	close(infd);
#if ( sfv_cleanup == TRUE )
	if (tmpfd != -1) {
		close(tmpfd);
		unlink(source);
		rename(".tmpsfv", source);
	}
#endif
	
	closedir(dir);
	close(outfd);
	if (!update_lock(raceI, 1, type)) {
		d_log("copysfv: Lock is suggested removed. Will comply and exit\n");
		remove_lock(raceI);
		exit(EXIT_FAILURE);
	}
	raceI->data_type = type;
	return retval;
}

/*
 * Modified	: 01.17.2002 Author	: Dark0n3
 * 
 * Description	: Creates a file that contains list of files in release in
 * alphabetical order.
 */
void 
create_indexfile(const char *racefile, struct VARS *raceI, char *f)
{
	int		fd;
	FILE		*r;
	int		l, n, m, c;
	int		pos[raceI->total.files],
			t_pos[raceI->total.files];
	char		fname[raceI->total.files][NAME_MAX];
	
	RACEDATA	rd;

	if ((fd = open(racefile, O_RDONLY)) == -1) {
		d_log("create_indexfile: open(%s): %s\n", racefile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!update_lock(raceI, 1, 0)) {
		d_log("create_indexfile: Lock is suggested removed. Will comply and exit\n");
		remove_lock(raceI);
		exit(EXIT_FAILURE);
	}

	/* Read filenames from race file */
	c = 0;
	while ((read(fd, &rd, sizeof(RACEDATA)))) {
		if (rd.status == F_CHECKED) {
			strlcpy(fname[c], rd.fname, NAME_MAX);
			t_pos[c] = 0;
			c++;
		}
	}
	close(fd);

	/* Sort with cache */
	for (n = 0; n < c; n++) {
		m = t_pos[n];
		for (l = n + 1; l < c; l++) {
			if (strcasecmp(fname[l], fname[n]) < 0)
				m++;
			else
				t_pos[l]++;
		}
		pos[m] = n;
	}

	/* Write to file and free memory */
	if ((r = fopen(f, "w"))) {
		for (n = 0; n < c; n++) {
			m = pos[n];
			fprintf(r, "%s\n", fname[m]);
		}
		fclose(r);
	}
}

/*
 * Modified	: 01.16.2002 Author	: Dark0n3
 * 
 * Description	: Marks file as deleted.
 *
 * Obsolete?
 */
short int 
clear_file(const char *path, char *f)
{
	int		n = 0, count;
	FILE           *file;

	RACEDATA	rd;

	count = 0;
	if ((file = fopen(path, "r+"))) {
		while (fread(&rd, sizeof(RACEDATA), 1, file)) {
			if (strncmp(rd.fname, f, NAME_MAX) == 0) {
				rd.status = F_DELETED;
				fseek(file, sizeof(RACEDATA) * count, SEEK_SET);
				fwrite(&rd, sizeof(RACEDATA), 1, file);
				n++;
			}
			count++;
		}
		fclose(file);
	}

	return n;
}

/*
 * Modified	: 02.19.2002 Author	: Dark0n3
 * 
 * Description	: Reads current race statistics from fixed format file.
 * 				: "path" is the location of a racedata file.
 */
void 
readrace(const char *path, struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI)
{
	int		fd, rlength = 0;

	RACEDATA	rd;

	if ((fd = open(path, O_RDONLY)) != -1) {

		if (!update_lock(raceI, 1, 0)) {
			d_log("readrace: Lock is suggested removed. Will comply and exit\n");
			remove_lock(raceI);
			exit(EXIT_FAILURE);
		}

		while ((rlength = read(fd, &rd, sizeof(RACEDATA)))) {
			if (rlength != sizeof(RACEDATA)) {
				d_log("readrace: Agh! racedata seems to be broken!\n");
				exit(EXIT_FAILURE);
			}
			switch (rd.status) {
				case F_NOTCHECKED:
				case F_CHECKED:
					updatestats(raceI, userI, groupI, rd.uname, rd.group,
						    rd.size, rd.speed, rd.start_time);
					break;
				case F_BAD:
					raceI->total.files_bad++;
					raceI->total.bad_size += rd.size;
					break;
				case F_NFO:
					raceI->total.nfo_present = 1;
					break;
			}
		}
		close(fd);
	}
}

/*
 * Modified	: 01.18.2002 Author	: Dark0n3
 * 
 * Description	: Writes stuff into race file.
 */
void 
writerace(const char *path, struct VARS *raceI, unsigned int crc, unsigned char status)
{
	int		fd, count, ret;

	RACEDATA	rd;

	/* create file if it doesn't exist */
	if ((fd = open(path, O_CREAT | O_RDWR, 0666)) == -1) {
		if (errno != EEXIST) {
			d_log("writerace: open(%s): %s\n", path, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if (!update_lock(raceI, 1, 0)) {
		d_log("writerace: Lock is suggested removed. Will comply and exit\n");
		remove_lock(raceI);
		exit(EXIT_FAILURE);
	}

	/* find an existing entry that we will overwrite */
	count = 0;
	while ((ret = read(fd, &rd, sizeof(RACEDATA)))) {
		if (ret == -1) {
			d_log("writerace: read(%s): %s\n", path, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (strncmp(rd.fname, raceI->file.name, NAME_MAX) == 0) {
			lseek(fd, sizeof(RACEDATA) * count, SEEK_SET);
			break;
		}
		count++;
	}

	rd.status = status;
	rd.crc32 = crc;
	
	strlcpy(rd.fname, raceI->file.name, NAME_MAX);
	strlcpy(rd.uname, raceI->user.name, 24);
	strlcpy(rd.group, raceI->user.group, 24);
	
	rd.size = raceI->file.size;
	rd.speed = raceI->file.speed;
	rd.start_time = raceI->total.start_time;

	write(fd, &rd, sizeof(RACEDATA));
	
	close(fd);
}

/* remove file entry from racedata file */
void
remove_from_race(const char *path, const char *f)
{
	int		fd, i, max;
	
	RACEDATA	rd, *tmprd = 0;
	
	if ((fd = open(path, O_RDONLY)) == -1) {
		d_log("remove_from_race: open(%s): %s\n", path, strerror(errno));
		return;
	}
	
	for (i = 0; (read(fd, &rd, sizeof(RACEDATA)));) {
		if (strcmp(rd.fname, f) != 0) {
			tmprd = realloc(tmprd, sizeof(RACEDATA)*(i+1));
			memcpy(&tmprd[i], &rd, sizeof(RACEDATA));
			i++;
		}
	}

	close(fd);
	
	if ((fd = open(path, O_TRUNC | O_WRONLY)) == -1) {
		d_log("remove_from_race: open(%s): %s\n", path, strerror(errno));
		if (tmprd)
			free(tmprd);
		return;
	}
	
	max = i;
	for (i = 0; i < max; i++)
		write(fd, &tmprd[i], sizeof(RACEDATA));
	
	close(fd);

	if (tmprd)
		free(tmprd);
}

int
verify_racedata(const char *path)
{
	int		fd, i, ret, max;
	
	RACEDATA	rd, *tmprd = 0;
	
	if ((fd = open(path, O_RDWR, 0666)) == -1) {
		d_log("verify_racedata: open(%s): %s\n", path, strerror(errno));
		return 0;
	}
	
	for (i = 0; (ret = read(fd, &rd, sizeof(RACEDATA)));) {
		if (fileexists(rd.fname)) {
			tmprd = realloc(tmprd, sizeof(RACEDATA)*(i+1));
			memcpy(&tmprd[i], &rd, sizeof(RACEDATA));
			i++;
		} else if (rd.fname) {
			d_log("verify_racedata: Oops! %s is missing - removing from racedata (ret=%d)\n", rd.fname, ret);
			create_missing(rd.fname);
		}
	}
	
	close(fd);
	
	if ((fd = open(path, O_TRUNC | O_WRONLY)) == -1) {
		d_log("verify_racedata: open(%s): %s\n", path, strerror(errno));
		if (tmprd)
			free(tmprd);
		return 0;
	}
	
	max = i;
	for (i = 0; i < max; i++)
		write(fd, &tmprd[i], sizeof(RACEDATA));
	
	close(fd);

	if (tmprd)
		free(tmprd);

	return 1;
}

/* Locking mechanism and version control.
 * Not yet fully functional, but we're getting there.
 * progtype == a code for what program calls the lock is found in constants.h
 * force_lock == int used to suggest/force a lock on the file.
 *		set to 1 to suggest a lock,2 to force a lock, 3 to put in queue.
 */

int
create_lock(struct VARS *raceI, const char *path, short int progtype, short int force_lock, short int queue)
{
	int		fd;
	HEADDATA	hd;
	struct stat	sb;

	/* this should really be moved out of the proc - we'll worry about it later */
	snprintf(raceI->headpath, PATH_MAX, "%s/%s/headdata", storage, path);

	if ((fd = open(raceI->headpath, O_CREAT | O_RDWR, 0666)) == -1) {
		d_log("create_lock: open(%s): %s\n", raceI->headpath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	fstat(fd, &sb);

	if (!sb.st_size) {							/* no lock file exists - let's create one with default values. */
		hd.data_version = sfv_version;
		raceI->data_type = hd.data_type = 0;
		raceI->data_in_use = hd.data_in_use = progtype;
		raceI->data_incrementor = hd.data_incrementor = 1;
		raceI->data_queue = hd.data_queue = 1;
		hd.data_qcurrent = 0;
		hd.data_pid = (short int)getpid();
		write(fd, &hd, sizeof(HEADDATA));
		close(fd);
		d_log("create_lock: lock set. (no previous lockfile found) pid: %d\n", hd.data_pid);
		return 0;
	} else {
		read(fd, &hd, sizeof(HEADDATA));
		if (hd.data_version != sfv_version) {
			d_log("create_lock: version of datafile mismatch. Stopping and suggesting a cleanup.\n");
			close(fd);
			return 1;
		}
		if ((time(NULL) - sb.st_ctime >= max_seconds_wait_for_lock)) {
			raceI->data_in_use = hd.data_in_use = progtype;
			raceI->data_incrementor = hd.data_incrementor = 1;
			raceI->data_queue = hd.data_queue = 1;
			hd.data_qcurrent = 0;
			hd.data_pid = (short int)getpid();
			lseek(fd, 0L, SEEK_SET);
			write(fd, &hd, sizeof(HEADDATA));
			close(fd);
			d_log("create_lock: lock set. (lockfile exceeded max life time) pid: %d\n", hd.data_pid);
			return 0;
		}
		if (hd.data_in_use) {						/* the lock is active */
			if (force_lock == 2) {
				raceI->data_queue = hd.data_queue = 1;
				hd.data_qcurrent = 0;
				d_log("create_lock: Unlock forced.\n");
			} else {
				if (force_lock == 3) {				/* we got a request to queue a lock if active */
					raceI->data_queue = hd.data_queue;	/* we give the current queue number to the calling process */
					hd.data_queue++;			/* we increment the number in the queue */
					lseek(fd, 0L, SEEK_SET);
					write(fd, &hd, sizeof(HEADDATA));
					d_log("create_lock: lock active - putting you in queue. (%d/%d)\n", hd.data_qcurrent, hd.data_queue);
				}
				close(fd);
				return hd.data_in_use;
			}
		}
		if (!hd.data_in_use) {						/* looks like the lock is inactive */
			if (force_lock == 2) {
				raceI->data_queue = hd.data_queue = 1;
				hd.data_qcurrent = 0;
				d_log("create_lock: Unlock forced.\n");
			} else if (force_lock == 3 && hd.data_queue > hd.data_qcurrent) {		/* we got a request to queue a lock if active, */
										/* and there seems to be others in queue. Will not allow the */
										/* process to lock, but wait for the queued process to do so. */
				raceI->data_queue = hd.data_queue;		/* we give the queue number to the calling process */
				hd.data_queue++;				/* we increment the number in the queue */
				raceI->data_incrementor = hd.data_incrementor;
				lseek(fd, 0L, SEEK_SET);
				write(fd, &hd, sizeof(HEADDATA));
				close(fd);
				d_log("create_lock: putting you in queue. (%d/%d)\n", hd.data_qcurrent, hd.data_queue);
				return -1;
			} else if (hd.data_queue && (queue > hd.data_qcurrent) && !force_lock) {
										/* seems there is a queue, and the calling process' place in */
										/* the queue is still less than current. */
				raceI->data_incrementor = hd.data_incrementor;	/* feed back the current incrementor */
				close(fd);
				return -1;
			}
		}
		if (force_lock == 1) {						/* lock suggested - reseting the incrementor to 0 */
			d_log("create_lock: Unlock suggested.\n");
			hd.data_incrementor = 0;
		} else {							/* either no lock and queue, or unlock is forced. */
			hd.data_incrementor = 1;
			hd.data_in_use = progtype;
		}
		raceI->data_incrementor = hd.data_incrementor;
		hd.data_pid = (short int)getpid();
		lseek(fd, 0L, SEEK_SET);
		write(fd, &hd, sizeof(HEADDATA));
		close(fd);
		raceI->data_in_use = progtype;
		raceI->data_type = 0;
		d_log("create_lock: lock set. pid: %d\n", hd.data_pid);
		return 0;
	}
}

/* Remove the lock
 */

void
remove_lock(struct VARS *raceI)
{
	int		fd;
	HEADDATA	hd;

	if ((fd = open(raceI->headpath, O_RDWR, 0666)) == -1) {
		d_log("remove_lock: open(%s): %s\n", raceI->headpath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	read(fd, &hd, sizeof(HEADDATA));
	hd.data_in_use = 0;
	hd.data_pid = 0;
	hd.data_incrementor = 0;
	if (hd.data_queue)							/* if queue, increase the number in current so the next */
		hd.data_qcurrent++;						/* process can start. */
	if (hd.data_queue < hd.data_qcurrent) {					/* If the next in line is bigger than the queue itself, */
		hd.data_queue = 0;						/* it should be fair to assume there is noone else in queue */
		hd.data_qcurrent = 0;						/* and reset the queue. Normally, this should not happen. */
	}
	lseek(fd, 0L, SEEK_SET);
	write(fd, &hd, sizeof(HEADDATA));
	close(fd);
	d_log("remove_lock: queue %d/%d\n", hd.data_qcurrent, hd.data_queue);
}

/* update a lock. This should be used after each file checked.
 * This procs task is mainly to 'touch' the lock and to check that nothing else wants the lock.
 * Please note:
 *   if counter == 0 a suggested lock-removal will be written. if >0 it's used as normal.
 *   if datatype != 0, this datatype will be written.
 */

int
update_lock(struct VARS *raceI, short int counter, short int datatype)
{
	int		fd, retval;
	HEADDATA	hd;
	struct stat	sb;

	if ((fd = open(raceI->headpath, O_RDWR, 0666)) == -1) {
		d_log("update_lock: open(%s): %s\n", raceI->headpath, strerror(errno));
		exit(EXIT_FAILURE);
	}
	read(fd, &hd, sizeof(HEADDATA));
	fstat(fd, &sb);

	if (hd.data_version != sfv_version) {
		d_log("create_lock: version of datafile mismatch. Stopping and suggesting a cleanup.\n");
		close(fd);
		return 1;
	}
	if ((hd.data_in_use != raceI->data_in_use) && counter) {
		d_log("update_lock: Lock not active or progtype mismatch - no choice but to exit.\n");
		close(fd);
		exit(EXIT_FAILURE);
	}
	if (!hd.data_incrementor) {
		d_log("update_lock: Lock suggested removed by a different process (%d/%d).\n", hd.data_incrementor, raceI->data_incrementor);
		retval = 0;
	} else {
		if (counter)
			hd.data_incrementor++;
		else
			hd.data_incrementor = 0;

		retval = hd.data_incrementor;
	}
	if (hd.data_pid != (short int)getpid() && hd.data_incrementor) {
		d_log("update_lock: Oops! Race condition - another process has the lock. pid: %d != %d\n", hd.data_pid, (short int)getpid());
		hd.data_queue = raceI->data_queue - 1;
		lseek(fd, 0L, SEEK_SET);
		write(fd, &hd, sizeof(HEADDATA));
		close(fd);
		return -1;
	}
	if (datatype && counter)
		hd.data_type = datatype;
	if ((retval && !lock_optimize) || !retval || !hd.data_incrementor || (time(NULL) - sb.st_ctime >= lock_optimize && hd.data_incrementor > 1)) {
		lseek(fd, 0L, SEEK_SET);
		write(fd, &hd, sizeof(HEADDATA));
		d_log("update_lock: updating lock (%d)\n", raceI->data_incrementor);
	}
	close(fd);
	if (counter) {
		raceI->data_incrementor = hd.data_incrementor;
		raceI->data_in_use = hd.data_in_use;
	}
	return retval;
}

