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
	unsigned int	crc = 0;
	FILE		*sfvfile;
	
	SFVDATA		sd;

	if (!(sfvfile = fopen(path, "r"))) {
		d_log("Failed to open sfv (%s): %s\n", path, strerror(errno));
		return 0;
	}

	/* release_type is stored in the beginning of sfvdata */
	fread(&raceI->misc.release_type, sizeof(short int), 1, sfvfile);
	
	d_log("Reading data from sfv (%s)\n", raceI->file.name);
	
	raceI->total.files = 0;
	while (fread(&sd, sizeof(SFVDATA), 1, sfvfile)) {
		raceI->total.files++;
		if (!strcasecmp(raceI->file.name, sd.fname)) {
			d_log("DEBUG: crc read from sfv-file %s : %X\n", sd.fname, sd.crc32);
			crc = sd.crc32;
		}
		if (getfcount && findfile(sd.fname))
			raceI->total.files_missing--;
	}
	
	fclose(sfvfile);
	
	raceI->total.files_missing += raceI->total.files;
	
	return crc;
}

void
update_sfvdata(const char *path, const unsigned int crc)
{
	FILE		*sfvfile;
	
	SFVDATA		sd;

	if (!(sfvfile = fopen(path, "r"))) {
		d_log("Failed to open sfvdata (%s): %s\n", path, strerror(errno));
		return;
	}

	sd.crc32 = crc;
	
	while (fread(&sd, sizeof(SFVDATA), 1, sfvfile)) {
		if (strcasecmp(path, sd.fname) == 0) {
			sd.crc32 = crc;
			break;
		}
	}
	
	fseek(sfvfile, -sizeof(SFVDATA), SEEK_CUR);
	fwrite(&sd, sizeof(SFVDATA), 1, sfvfile);
	fclose(sfvfile);
}

/* convert the sfvdata file source to the sfv dest */
void
sfvdata_to_sfv(const char *source, const char *dest)
{
	char		crctmp[8];
	FILE		*insfv, *outsfv;
	
	SFVDATA		sd;

	if (!(insfv = fopen(source, "r"))) {
		d_log("Failed to open (%s): %s\n", source, strerror(errno));
		return;
	}

	if (!(outsfv = fopen(".tmpsfv", "w"))) {
		d_log("Failed to open (.tmpsfv): %s\n", strerror(errno));
		return;
	}

	while (fread(&sd, sizeof(SFVDATA), 1, insfv)) {
		
		sprintf(crctmp, "%.8x", sd.crc32);
		
		fwrite(sd.fname, PATH_MAX, 1, outsfv);
		fwrite(" ", 1, 1, outsfv);
		fwrite(&crctmp, 8, 1, outsfv),
#if (sfv_cleanup_crlf == TRUE )
		fwrite("\r", 1, 1, outsfv);
#endif
		fwrite("\n", 1, 1, outsfv);
		
	}
	
	fclose(insfv);
	fclose(outsfv);

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
	char		*f, missing_fname[PATH_MAX];
	FILE		*sfvfile;

	SFVDATA		sd;

	if (!(sfvfile = fopen(path, "r"))) {
		d_log("Couldn't fopen %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	/* jump past release_type */
	fseek(sfvfile, sizeof(short int), SEEK_CUR);

	while (fread(&sd, sizeof(SFVDATA), 1, sfvfile)) {
		snprintf(missing_fname, PATH_MAX, "%s-missing", sd.fname);
		f = findfilename(missing_fname);
		if (f)
			unlink(f);
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
	FILE           *file;

	if ((file = fopen(path, "r+"))) {
		fread(&raceI->misc.old_leader, 1, 24, file);
		rewind(file);
		fwrite(userI->name, 1, 24, file);
	} else {
		*raceI->misc.old_leader = 0;
		if (!(file = fopen(path, "w+"))) {
			d_log("Couldn't write to %s: %s\n", path, strerror(errno));
			exit(EXIT_FAILURE);
		}
		fwrite(userI->name, 1, 24, file);
	}
	fclose(file);
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
	FILE		*file;
	char		*realfile, target[256], *ext;
	unsigned int	Tcrc;
	int		m = 0, l = 0;
	struct stat	filestat;

	RACEDATA	rd;
	
	if ((file = fopen(locations->race, "r+"))) {
		realfile = raceI->file.name;
		if (rstatus)
			printf("\n");
		while ((fread(&rd, 1, sizeof(RACEDATA), file))) {

			/* what does this do? */
			m = l = strlen(realfile);
			ext = realfile;
			while (ext[m] != '.' && m > 0)
				m--;
			if (ext[m] != '.')
				m = l;
			else
				m++;
			ext += m;

			if (rd.status == F_NOTCHECKED) {
				strlcpy(raceI->file.name, rd.fname, PATH_MAX);
				Tcrc = readsfv(locations->sfv, raceI, 0);
				stat(rd.fname, &filestat);
				if (S_ISDIR(filestat.st_mode)) {
					rd.status = F_IGNORED;
				} else if (rd.crc32 != 0 && Tcrc == rd.crc32) {
					rd.status = F_CHECKED;
				} else if (rd.crc32 != 0 && strcomp(ignored_types, ext)) {
					rd.status = F_IGNORED;
				} else if (rd.crc32 != 0 && Tcrc == 0 && strcomp(allowed_types, ext)) {
					rd.status = F_IGNORED;
				} else if ((rd.crc32 != 0) && (Tcrc != rd.crc32) &&
				           (strcomp(allowed_types, ext) &&
					   !matchpath(allowed_types_exemption_dirs, locations->path))) {
					rd.status = F_IGNORED;
				} else {
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

					d_log("marking %s bad.\n", rd.fname);
					if (enable_unduper_script == TRUE) {
						if (!fileexists(unduper_script)) {
							d_log("Failed to undupe '%s' - '%s' does not exist.\n",
							      rd.fname, unduper_script);
						} else {
							sprintf(target, unduper_script " \"%s\"", rd.fname);
							if (execute(target) == 0) {
								d_log("undupe of %s successful.\n", rd.fname);
							} else {
								d_log("undupe of %s failed.\n", rd.fname);
							}
						}
					}
				}
			}
			fseek(file, -sizeof(RACEDATA), SEEK_CUR);
			fwrite(&rd, 1, sizeof(RACEDATA), file);
		}

		strlcpy(raceI->file.name, realfile, strlen(realfile));
		raceI->total.files = raceI->total.files_missing = 0;
		fclose(file);
	}
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
copysfv(const char *source, const char *target)
{
	int		i, retval = 0;
	short int	music, rars, video, others, type;
	
	char		*ptr, fbuf[2048];
	FILE		*insfv, *outsfv;

	SFVDATA		sd;
	
#if ( sfv_dupecheck == TRUE )
	int		skip = 0;
	SFVDATA		tempsd;
#endif
	
#if ( sfv_cleanup == TRUE )
	char		crctmp[8];
	FILE		*tmpsfv = 0;
	
	if ((tmpsfv = fopen(".tmpsfv", "w+")) == NULL)
		d_log("Failed to open '.tmpsfv': %s\n", strerror(errno));
#endif

	if ((insfv = fopen(source, "r")) == NULL) {
		d_log("Failed to open '%s': %s\n", source, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if ((outsfv = fopen(target, "w+")) == NULL) {
		d_log("Failed to fopen '%s': %s\n", target, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	video = music = rars = others = type = 0;

	fwrite(&type, sizeof(short int), 1, outsfv);
	
	while ((fgets(fbuf, sizeof(fbuf), insfv))) {
		
		/* remove comment */
		if ((ptr = find_first_of(fbuf, ";")))
			*ptr = '\0';
			
		strip_whitespaces(fbuf);

		if (strlen(fbuf) == 0)
			continue;
	
#if (sfv_cleanup == TRUE)
#if (sfv_cleanup_lowercase == TRUE)
		for (ptr = fbuf; *ptr; ptr++)
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
			if (i != 8) {
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
		if (strlen(fbuf) > 0) {
			strlcpy(sd.fname, fbuf, PATH_MAX);

#if (sfv_calc_single_fname == TRUE)
			/* TODO */
			/* calculate file's crc if it exists */
			if (sd.crc32 == 0) {
				d_log("Got filename (%s) without crc, trying to calculate.\n", sd.fname);
				sd.crc32 = calc_crc32(sd.fname);
			}
#endif
			
			/* get file extension */
			ptr = find_last_of(fbuf, ".");
			if (*ptr == '.')
				ptr++;
			
			if (!strcomp(ignored_types, ptr)) {

#if ( sfv_dupecheck == TRUE )
				fseek(outsfv, sizeof(short int), SEEK_SET);
				
				/* read from sfvdata - no parsing */
				skip = 0;
				while (fread(&tempsd, sizeof(SFVDATA), 1, outsfv))
					if (strcmp(sd.fname, tempsd.fname) == 0)
						skip = 1;
						
				fseek(outsfv, 0L, SEEK_END);

				if (skip)
					continue;
#endif

				d_log("File in sfv: '%s' (%x)\n", sd.fname, sd.crc32);

#if ( sfv_cleanup == TRUE )
				/* write good stuff to .tmpsfv */
				if (tmpsfv) {
					sprintf(crctmp, "%.8x", sd.crc32);
					fwrite(sd.fname, strlen(sd.fname), 1, tmpsfv);
					fwrite(" ", 1, 1, tmpsfv);
					fwrite(crctmp, 8, 1, tmpsfv);
#if (sfv_cleanup_crlf == TRUE )
					fwrite("\r", 1, 1, tmpsfv);
#endif
					fwrite("\n", 1, 1, tmpsfv);
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
				if (!findfile(sd.fname))
					create_missing(sd.fname);
#endif

				fwrite(&sd, sizeof(SFVDATA), 1, outsfv);
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
	fclose(insfv);
#if ( sfv_cleanup == TRUE && sfv_error == FALSE )
	unlink(source);
	fclose(tmpsfv);
	rename(".tmpsfv", source);
#endif
	
	rewind(outsfv);
	fwrite(&type, sizeof(short int), 1, outsfv);
	fclose(outsfv);
	
	return retval;
}

/*
 * Modified	: 01.17.2002 Author	: Dark0n3
 * 
 * Description	: Creates a file that contains list of files in release in
 * alphabetical order.
 */
void 
create_indexfile(const char *path, struct VARS *raceI, char *f)
{
	FILE		*r;
	int		l, n, m, c;
	int		pos[raceI->total.files],
			t_pos[raceI->total.files];
	char		fname[raceI->total.files][PATH_MAX];
	
	RACEDATA	rd;

	if (!(r = fopen(path, "r"))) {
		d_log("Couldn't fopen %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Read filenames from race file */
	c = 0;
	while ((fread(&rd, 1, sizeof(RACEDATA), r))) {
		if (rd.status == F_CHECKED) {
			strlcpy(fname[c], rd.fname, PATH_MAX);
			t_pos[c] = 0;
			c++;
		}
	}
	fclose(r);

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
 */
short int 
clear_file(const char *path, char *f)
{
	int		n = 0;
	FILE           *file;

	RACEDATA	rd;

	if ((file = fopen(path, "r+"))) {
		while (fread(&rd, 1, sizeof(RACEDATA), file)) {
			if (strncmp(rd.fname, f, PATH_MAX) == 0) {
				rd.status = F_DELETED;
				fseek(file, -sizeof(RACEDATA), SEEK_CUR);
				fwrite(&rd, 1, sizeof(RACEDATA), file);
				n++;
			}
		}
		fclose(file);
	}

	return n;
}

/*
 * Modified	: 02.19.2002 Author	: Dark0n3
 * 
 * Description	: Reads current race statistics from fixed format file.
 */
void 
readrace(const char *path, struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI)
{
	FILE           *file;

	RACEDATA	rd;

	if ((file = fopen(path, "r"))) {
		while (fread(&rd, 1, sizeof(RACEDATA), file)) {
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
		fclose(file);
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
	int		id;
	FILE		*file;

	RACEDATA	rd;

	clear_file(path, raceI->file.name);

	/* create file if it doesn't exist */
	if ((id = open(path, O_CREAT | O_RDWR, 0666)) == -1) {
		if (errno != EEXIST) {
			d_log("Failed to create %s: %s\n", path, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
 
	if (!(file = fdopen(id, "r+"))) {
		d_log("Couldn't fopen racefile (%s): %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* find an existing entry that we will overwrite */
	while (fread(&rd, 1, sizeof(RACEDATA), file)) {
		if (strncmp(rd.fname, raceI->file.name, PATH_MAX) == 0) {
			fseek(file, -sizeof(RACEDATA), SEEK_CUR);
			break;
		}
	}

	rd.status = status;
	rd.crc32 = crc;
	
	strlcpy(rd.fname, raceI->file.name, PATH_MAX);
	strlcpy(rd.uname, raceI->user.name, 24);
	strlcpy(rd.group, raceI->user.group, 24);
	
	rd.size = raceI->file.size;
	rd.speed = raceI->file.speed;
	rd.start_time = raceI->total.start_time;

	fwrite(&rd, 1, sizeof(RACEDATA), file);
	
	fclose(file);
}

