#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "objects.h"
#include "macros.h"

#ifdef _WITH_SS5
#include "constants.ss5.h"
#else
#include "constants.h"
#endif

#include "stats.h"
#include "zsfunctions.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

/*
 * Modified	: 01.16.2002 Author	: Dark0n3
 * 
 * Description	: Reads crc for current file from preparsed sfv file.
 */
unsigned int 
readsfv_file(struct LOCATIONS *locations, struct VARS *raceI, int getfcount)
{
	char           *fname;
	unsigned int	crc = 0;
	unsigned int	t_crc = 0;
	FILE           *sfvfile;
	unsigned int	len = 0;

	if (!(sfvfile = fopen(locations->sfv, "r"))) {
		d_log("Failed to open sfv (%s): %s\n", locations->sfv, strerror(errno));
		return 0;
	}

	fread(&raceI->misc.release_type, sizeof(short int), 1, sfvfile);
	d_log("Reading data from sfv (%s)\n", raceI->file.name);
	while (fread(&len, sizeof(int), 1, sfvfile) == 1) {
		fname = m_alloc(len);
		fread(fname, 1, len, sfvfile);
		fread(&t_crc, sizeof(int), 1, sfvfile);
		raceI->total.files++;
		if (!strcasecmp(raceI->file.name, fname)) {
			d_log("DEBUG: crc read from sfv-file %s : %X\n", fname, t_crc);
			crc = t_crc;
		}
		if (getfcount && findfile(fname))
			raceI->total.files_missing--;
		m_free(fname);
	}
	fclose(sfvfile);
	raceI->total.files_missing += raceI->total.files;
	return crc;
}

/*
 * Modified	: 01.16.2002 Author	: Dark0n3
 * 
 * Description	: Deletes all -missing files with preparsed sfv.
 */
void 
delete_sfv_file(struct LOCATIONS *locations)
{
	char           *fname, *fnname;
	FILE           *sfvfile;
	unsigned int	len;

	if (!(sfvfile = fopen(locations->sfv, "r"))) {
		d_log("Couldn't fopen %s: %s\n", locations->sfv, strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(sfvfile, sizeof(short int), SEEK_CUR);

	while (fread(&len, sizeof(int), 1, sfvfile) == 1) {
		fname = fnname = m_alloc(len + 8);
		fread(fname, 1, len, sfvfile);
		fseek(sfvfile, sizeof(int), SEEK_CUR);
		memcpy(fname + len - 1, "-missing", 9);
		if (fname)
			unlink(fname);
		fnname = findfilename(fname);
		if (fnname)
			unlink(fnname);
		m_free(fname);
	}
	fclose(sfvfile);
}

/*
 * Modified	: 02.19.2002 Author	: Dark0n3
 *		: 06.02.2005 by		: js
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
 * Description	: Reads name of old race leader and writes name of new leader
 * into temporary file.
 * 
 * Todo		: Make this unneccessary (write info to another file)
 */
void 
read_write_leader_file(struct LOCATIONS *locations, struct VARS *raceI, struct USERINFO *userI)
{
	FILE           *file;

	if ((file = fopen(locations->leader, "r+"))) {
		fread(&raceI->misc.old_leader, 1, 24, file);
		rewind(file);
		fwrite(userI->name, 1, 24, file);
	} else {
		*raceI->misc.old_leader = 0;
		if (!(file = fopen(locations->leader, "w+"))) {
			d_log("Couldn't write to %s: %s\n", locations->leader, strerror(errno));
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
testfiles_file(struct LOCATIONS *locations, struct VARS *raceI, int rstatus)
{
	FILE		*file;
	char		*realfile, target[256], *ext;
	unsigned int	Tcrc;
	int		m= 0, l = 0;
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
				Tcrc = readsfv_file(locations, raceI, 0);
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
 */
int
copysfv_file(char *source, char *target, off_t buf_bytes)
{
	int		fd;
	char           *buf;
	char           *line;
	char           *newline;
	char           *eof;
	char           *fext;
	char           *crc;
	char		crclen;
	unsigned int	video = 0;
	unsigned int	music = 0;
	unsigned int	rars = 0;
	unsigned int	others = 0;
	int		len = 0;
	short int	n = 0;
	unsigned int	t_crc;
	short int	sfv_failed = 0;
#if (sfv_cleanup == TRUE )
	int		sfv_error = FALSE;
#endif
#if ( sfv_dupecheck == TRUE )
	char           *fname[MAXIMUM_FILES_IN_RELEASE];	/* Semi-stupid. We limit @
								 * MAXIMUM_FILES_IN_RELEASE, statically. */
	unsigned int	files = 0;
	unsigned char	exists;
#endif
#if ( sfv_cleanup == TRUE && sfv_error == FALSE )
	int		fd_new = open(".tmpsfv", O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd_new == -1) {
		d_log("Failed to create temporary sfv file (%d) - setting cleanup of sfv to false and tries to continue. (error: %s)\n", fd_new, strerror(errno));
		sfv_error = TRUE;
	}
#endif

	if ((fd = open(source, O_RDONLY)) == -1) {
		d_log("Failed to open %s: %s\n", source, strerror(errno));
		exit(EXIT_FAILURE);
	}
	buf = m_alloc(buf_bytes);
	read(fd, buf, buf_bytes);
	close(fd);
	eof = buf + buf_bytes;

	if ((fd = open(target, O_CREAT | O_TRUNC | O_WRONLY, 0666)) == -1) {
		d_log("Failed to create %s: %s\n", target, strerror(errno));
		exit(EXIT_FAILURE);
	}
	write(fd, &n, sizeof(short int));

	for (newline = buf; newline < eof; newline++) {
		for (line = newline; *newline != '\n' && newline < eof; newline++) {
#if (sfv_cleanup == TRUE)
#if (sfv_cleanup_lowercase == TRUE)
			*newline = tolower(*newline);
#endif
#endif
		}
		if (*line != ';' && newline > line) {
			*newline = crclen = 0;
			for (crc = newline - 1; isxdigit(*crc) == 0 && crc > line; crc--)
				*crc = 0;
			for (; *crc != ' ' && *crc != '\t' && crc > line; crc--)
				crclen++;
			if ((*crc == ' ' || *crc == '\t') && crc > line && crclen == 8) {
				for (fext = crc; *fext != '.' && fext > line; fext--);
				if (*fext == '.')
					fext++;
				*crc++ = 0;
				if (!strcomp(ignored_types, fext) && strcasecmp("sfv", fext) && strcasecmp("nfo", fext) && (t_crc = hexstrtodec((unsigned char *)crc)) > 0) {
					len = crc - line;
#if ( sfv_dupecheck == TRUE )
					exists = 0;
					for (n = 0; (unsigned int)n < files; n++) {
						if (!memcmp(fname[n], line, len)) {
							exists = 1;
							break;
						}
					}

					if (exists == 1) {
						continue;
					}
					fname[files++] = line;
#endif
					if ((strlen(line) + 1) < (unsigned int)len) {
						d_log("ERROR: NULL encountered in filename (%s)\n", line);
						if (strict_sfv_check == TRUE) {
							sfv_failed = 1;
						d_log("       Strict mode on - logging sfv as bad.\n");
#if (sfv_cleanup == TRUE)
							sfv_error = TRUE;
#endif
						}
						continue;
					}

					d_log("file in sfv - %s (%s)\n", line, crc);

#if ( sfv_cleanup == TRUE && sfv_error == FALSE )
					write(fd_new, line, len - 1);
					write(fd_new, " ", 1);
					write(fd_new, crc, 8);
#if (sfv_cleanup_crlf == TRUE && sfv_error == FALSE )
					write(fd_new, "\r", 1);
#endif
					write(fd_new, "\n", 1);
#endif
					if (!memcmp(fext, "mp3", 4) && !sfv_failed) {
						music++;
					} else if (israr(fext) && !sfv_failed) {
						rars++;
					} else if (isvideo(fext) && !sfv_failed) {
						video++;
					} else if (!sfv_failed) {
						others++;
					}
#if ( create_missing_files == TRUE )
					if (!findfile(line) && !sfv_failed) {
						create_missing(line);
					}
#endif
					if (!sfv_failed) {
						write(fd, &len, sizeof(int));
						write(fd, line, len);
						write(fd, &t_crc, sizeof(int));
					}
				}
			} else {
				crc = line;
				while (*crc) {
					if (*crc == ' ' || *crc == '\n' || *crc == '\r' || *crc == '\t') {
						crc++;
					} else {
						d_log("ERROR: Found an entry in SFV without checksum (%s).\n", line);
						if (strict_sfv_check == TRUE) {
							sfv_failed = 1;
							d_log("       Strict mode on - logging sfv as bad.\n");
#if (sfv_cleanup == TRUE)
							sfv_error = TRUE;
#endif
						}
						break;
					}
				}
			}
		}
	}

	lseek(fd, 0, 0);

	if (music > rars) {
		if (video > music) {
			n = (video >= others ? 4 : 2);
		} else {
			n = (music >= others ? 3 : 2);
		}
	} else {
		if (video > rars) {
			n = (video >= others ? 4 : 2);
		} else {
			n = (rars >= others ? 1 : 2);
		}
	}

	if (!sfv_failed)
		write(fd, &n, sizeof(short int));
	close(fd);

#if ( sfv_cleanup == TRUE && sfv_error == FALSE )
	if (source)
		unlink(source);
	rename(".tmpsfv", source);
	close(fd_new);
#endif

	m_free(buf);
	return sfv_failed;
}

/*
 * Modified	: 01.17.2002 Author	: Dark0n3
 * 
 * Description	: Creates a file that contains list of files in release in
 * alphabetical order.
 */
void 
create_indexfile_file(struct LOCATIONS *locations, struct VARS *raceI, char *f)
{
	FILE		*r;
	int		l, n, m, c;
	int		pos[raceI->total.files],
			t_pos[raceI->total.files];
	char		fname[raceI->total.files][PATH_MAX];
	
	RACEDATA	rd;

	if (!(r = fopen(locations->race, "r"))) {
		d_log("Couldn't fopen %s: %s\n", locations->race, strerror(errno));
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
			if (strcasecmp(fname[l], fname[n]) < 0) {
				m++;
			} else {
				t_pos[l]++;
			}
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
clear_file_file(struct LOCATIONS *locations, char *f)
{
	int		n = 0;
	FILE           *file;

	RACEDATA	rd;

	if ((file = fopen(locations->race, "r+"))) {
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
readrace_file(struct LOCATIONS *locations, struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI)
{
	FILE           *file;

	RACEDATA	rd;

	if ((file = fopen(locations->race, "r"))) {
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
writerace_file(struct LOCATIONS *locations, struct VARS *raceI, unsigned int crc, unsigned char status)
{
	FILE		*file;

	RACEDATA	rd;

	clear_file_file(locations, raceI->file.name);

	if (!(file = fopen(locations->race, "a+"))) {
		d_log("Couldn't fopen racefile (%s): %s\n", locations->race, strerror(errno));
		exit(EXIT_FAILURE);
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
