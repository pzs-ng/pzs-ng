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
 * 
 * Description	: Creates directory where all race information will be
 * stored.
 */
void 
maketempdir(struct LOCATIONS *locations)
{
	unsigned int	size;
	char           *fullpath, *path_p, *end_p;


	size = sizeof(storage) + locations->length_path;
	fullpath = m_alloc(size);
	memcpy(fullpath, storage, sizeof(storage));

	path_p = fullpath + sizeof(storage) - 1;
	memcpy(path_p, locations->path, locations->length_path + 1);

	for (end_p = fullpath + size; path_p < end_p; path_p++) {
		switch (*path_p) {
		case '/':
		case 0:
			*path_p = 0;
			mkdir(fullpath, 0777);
			*path_p = '/';
			break;
		}
	}
	m_free(fullpath);
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
 * Todo		: Add unduper
 */
void 
testfiles_file(struct LOCATIONS *locations, struct VARS *raceI, int rstatus)
{
	int		len;
	unsigned char	status;
	FILE           *file;
	char           *fname, *realfile, *target, *ext;
	unsigned int	Tcrc, crc;
	int		m= 0, l = 0;

	target = m_alloc(256);

	if ((file = fopen(locations->race, "r+"))) {
		realfile = raceI->file.name;
		if (rstatus)
			printf("\n");
		while (fread(&len, sizeof(int), 1, file) == 1) {

			fname = m_alloc(len);
			fread(fname, 1, len, file);
			fread(&status, 1, 1, file);
			fread(&crc, sizeof(int), 1, file);

			m = l = strlen(realfile);
			ext = realfile;
			while (ext[m] != '.' && m > 0)
				m--;
			if (ext[m] != '.')
				m = l;
			else
				m++;
			ext += m;

			if (status == F_NOTCHECKED) {
				raceI->file.name = fname;
				Tcrc = readsfv_file(locations, raceI, 0);
				if (crc != 0 && Tcrc == crc) {
					status = F_CHECKED;
				} else if (crc != 0 && strcomp(ignored_types, ext)) {
					status = F_IGNORED;
				} else if (crc != 0 && Tcrc == 0 && strcomp(allowed_types, ext)) {
					status = F_IGNORED;
				} else if ((crc != 0) && (Tcrc != crc) && (strcomp(allowed_types, ext) && !matchpath(allowed_types_exemption_dirs, locations->path))) {
					status = F_IGNORED;
				} else {
					mark_as_bad(fname);
					if (fname)
						unlink(fname);
					status = F_BAD;

#if ( create_missing_files )
					if (Tcrc != 0)
						create_missing(fname, len - 1);
#endif

					if (rstatus)
						printf("File: %s FAILED!\n", fname);

					d_log("marking %s bad.\n", fname);
					if (enable_unduper_script == TRUE) {
						sprintf(target, unduper_script " \"%s\"", fname);
						if (execute(target) == 0) {
							d_log("undupe of %s successful.\n", fname);
						} else {
							d_log("undupe of %s failed.\n", fname);
						}
					}
				}
			}
			fseek(file, -1 - sizeof(int), SEEK_CUR);
			fwrite(&status, 1, 1, file);
			fseek(file, 48 + 5 * sizeof(int), SEEK_CUR);
			m_free(fname);
		}

		raceI->file.name = realfile;
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
	/*
	 * 15.09.2004 - Removing/Hiding the feature of adding comments to
	 * .sfv files - psxc
	 * 
	 * if ( sfv_comment != NULL ) { write(fd_new, sfv_comment,
	 * sizeof(sfv_comment) - 1); }
	 */

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
						create_missing(line, len - 1);
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
							break;
						}
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
	FILE           *r;
	int		l         , n, m, c;
	int            *pos;
	int            *t_pos;
	unsigned char	s;
	char           *t;
	char          **fname;

	pos = m_alloc(sizeof(int) * raceI->total.files);
	t_pos = m_alloc(sizeof(int) * raceI->total.files);
	fname = m_alloc(sizeof(char *) * raceI->total.files);
	if (!(r = fopen(locations->race, "r"))) {
		d_log("Couldn't fopen %s: %s\n", locations->race, strerror(errno));
		exit(EXIT_FAILURE);
	}
	c = 0;

	/* Read filenames from race file */

	while (fread(&l, sizeof(int), 1, r) == 1) {
		t = m_alloc(l);
		fread(t, 1, l, r);
		fread(&s, 1, 1, r);
		if (s == F_CHECKED) {
			fname[c] = t;
			t_pos[c] = 0;
			c++;
		} else {
			m_free(t);
		}
		fseek(r, 48 + 5 * sizeof(int), SEEK_CUR);
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
			m_free(fname[m]);
		}
		fclose(r);
	} else {
		for (n = 0; n < c; n++)
			m_free(fname[n]);
	}

	m_free(fname);
	m_free(t_pos);
	m_free(pos);
}

/*
 * Modified	: 01.16.2002 Author	: Dark0n3
 * 
 * Description	: Marks file as deleted.
 */
short int 
clear_file_file(struct LOCATIONS *locations, char *f)
{
	int		len       , n;
	unsigned char	status;
	FILE           *file;
	char           *fname;

	if ((file = fopen(locations->race, "r+"))) {
		n = 0;
		while (fread(&len, sizeof(int), 1, file) == 1) {
			fname = m_alloc(len);
			fread(fname, 1, len, file);
			fread(&status, 1, 1, file);

			if ((status == F_CHECKED || status == F_NOTCHECKED || status == F_NFO) && !memcmp(f, fname, len)) {
				status = F_DELETED;
				n++;
				fseek(file, -1, SEEK_CUR);
				fwrite(&status, 1, 1, file);
			}
			fseek(file, 48 + 5 * sizeof(int), SEEK_CUR);
			m_free(fname);
		}
		fclose(file);
		if (n)
			return 1;
	}
	return 0;
}

/*
 * Modified	: 02.19.2002 Author	: Dark0n3
 * 
 * Description	: Reads current race statistics from fixed format file.
 */
void 
readrace_file(struct LOCATIONS *locations, struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI)
{
	off_t		fsize;
	unsigned int	uspeed;
	unsigned int	start_time;
	unsigned char  *p_buf;
	unsigned char	buf[1 + 2 * 24 + 3 * sizeof(int) + sizeof(off_t)];
	unsigned int	len;
	FILE           *file;
	char           *uname;
	char           *ugroup;

	if (!(file = fopen(locations->race, "r"))) {
		return;
	}
	while (fread(&len, sizeof(int), 1, file) == 1) {
		fseek(file, len, SEEK_CUR);
		fread(buf, 1, sizeof(buf), file);

		p_buf = buf + 1 + sizeof(int);
		uname = (char *)p_buf;
		p_buf += 24;
		ugroup = (char *)p_buf;
		p_buf += 24;
		memcpy(&fsize, p_buf, sizeof(off_t));
		p_buf += sizeof(off_t);
		memcpy(&uspeed, p_buf, sizeof(int));
		p_buf += sizeof(int);
		memcpy(&start_time, p_buf, sizeof(int));

		switch (*buf) {
		case F_NOTCHECKED:
		case F_CHECKED:
			updatestats(raceI, userI, groupI, uname, ugroup, (off_t) fsize, (unsigned int)uspeed, (unsigned int)start_time);
			break;
		case F_BAD:
			raceI->total.files_bad++;
			raceI->total.bad_size += fsize;
			break;
		case F_NFO:
			raceI->total.nfo_present = 1;
			break;
		}
	}
	fclose(file);
}

/*
 * Modified	: 01.18.2002 Author	: Dark0n3
 * 
 * Description	: Writes stuff into race file.
 */
void 
writerace_file(struct LOCATIONS *locations, struct VARS *raceI, unsigned int crc, unsigned char status)
{
	FILE           *file;
	unsigned int	len;
	unsigned int	sz;
	unsigned char  *buf;
	unsigned char  *p_buf;

	clear_file_file(locations, raceI->file.name);

	if (!(file = fopen(locations->race, "a+"))) {
		d_log("Couldn't fopen racefile (%s): %s\n", locations->race, strerror(errno));
		exit(EXIT_FAILURE);
	}
	len = strlen(raceI->file.name) + 1;
	sz = len + 1 + 2 * 24 + 4 * sizeof(int) + sizeof(off_t);
	p_buf = buf = m_alloc(sz);

	memcpy(p_buf, &len, sizeof(int));
	p_buf += sizeof(int);
	memcpy(p_buf, raceI->file.name, len);
	p_buf += len;
	*p_buf++ = status;
	memcpy(p_buf, &crc, sizeof(int));
	p_buf += sizeof(int);
	memcpy(p_buf, raceI->user.name, 24);
	p_buf += 24;
	memcpy(p_buf, raceI->user.group, 24);
	p_buf += 24;
	memcpy(p_buf, &raceI->file.size, sizeof(off_t));
	p_buf += sizeof(off_t);
	memcpy(p_buf, &raceI->file.speed, sizeof(int));
	p_buf += sizeof(int);
	memcpy(p_buf, &raceI->total.start_time, sizeof(int));

	fwrite(buf, 1, sz, file);
	fclose(file);
	m_free(buf);
}
