#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "zsfunctions.h"
#include "race-file.h"
#include "objects.h"
#include "macros.h"
#include "constants.h"
#include "errors.h"
#include "multimedia.h" 
#include "convert.h"
#include "dizreader.h"
#include "stats.h"
#include "complete.h"
#include "crc.h"

#include "../conf/zsconfig.h"
#include "../../config.h"

/* Remove the portion of PARAM matched by PATTERN according to OP, where OP
   can have one of 4 values:
        RP_LONG_LEFT    remove longest matching portion at start of PARAM
        RP_SHORT_LEFT   remove shortest matching portion at start of PARAM
        RP_LONG_RIGHT   remove longest matching portion at end of PARAM
        RP_SHORT_RIGHT  remove shortest matching portion at end of PARAM
*/

#define RP_LONG_LEFT    1
#define RP_SHORT_LEFT   2
#define RP_LONG_RIGHT   3
#define RP_SHORT_RIGHT  4

/*static struct ONLINE	*online;
static struct USERINFO  **userI;
static struct GROUPINFO **groupI;
static struct VARS      raceI;
static struct LOCATIONS locations;*/

struct ONLINE	*online;
struct USERINFO  **userI;
struct GROUPINFO **groupI;
struct VARS      raceI;
struct LOCATIONS locations;

/*
 * 
 *
 */
void writelog(char *msg, char *status) {
 FILE   *glfile;
 char   *date;
 char   *line, *newline;
 time_t timenow;

 if ( raceI.misc.write_log == TRUE && !matchpath(group_dirs, locations.path)) {
	timenow = time(NULL);
	date = ctime(&timenow);
	glfile = fopen(log, "a+");

	line = newline = msg;
	while ( 1 ) {
		switch ( *newline++ ) {
			case 0:
				fprintf(glfile, "%.24s %s: \"%s\" \"%s\"\n", date, status, locations.path, line);
				fclose(glfile);
				return;
			case '\n':
				fprintf(glfile, "%.24s %s: \"%s\" \"%.*s\"\n", date, status, locations.path, (int)(newline - line - 1), line);
				line = newline;
				break;
			}
		}
	}
}




void getrelname(char *directory) {
 int	cnt,
	l[2],
	n = 0,
	k = 2;
 char	*path[2];

 for ( cnt = locations.length_path - 1 ; k && cnt ; cnt-- ) if ( directory[cnt] == '/' ) {
	k--;
	l[k] = n;
	path[k] = malloc(n + 1);
	strncpy(path[k], directory + cnt + 1, n);
	path[k][n] = 0;
	n = 0;
	} else n++;

 if (( ! strncasecmp(path[1], "CD"  , 2) && l[1] <= 4 ) ||
     ( ! strncasecmp(path[1], "DISC", 4) && l[1] <= 6 ) ||
     ( ! strncasecmp(path[1], "DiSC", 4) && l[1] <= 6 ) ||
     ( ! strncasecmp(path[1], "DVD" , 3) && l[1] <= 5 )) {
	raceI.misc.release_name = malloc(l[0] + 18);
	locations.link_source = malloc(k = (locations.length_path - l[1]));
	sprintf(raceI.misc.release_name, "%s/\\002%s\\002", path[0], path[1]);
	sprintf(locations.link_source, "%.*s", k - 1, locations.path);
	locations.link_target = path[0];
	locations.incomplete = c_incomplete(incomplete_cd_indicator, path);
	free(path[1]);
	} else {
	raceI.misc.release_name = malloc(l[1] + 10);
	locations.link_source	= malloc(locations.length_path + 1);
	strcpy(locations.link_source, locations.path);
	sprintf(raceI.misc.release_name, "\\002%s\\002", path[1]);
	locations.link_target = path[1];
	locations.incomplete = c_incomplete(incomplete_indicator, path);
	free(path[0]);
	}
}



unsigned char get_filetype(char *ext) {
	if (!memcmp(ext, "zip", 4)) return 0;
	if (!memcmp(ext, "sfv", 4)) return 1;
	if (!memcmp(ext, "nfo", 4)) return 2;
	if (!strcomp(ignored_types, ext)) return 3;
	if ( strcomp(allowed_types, ext)) return 4;

	return 255;
}

static char *
remove_pattern (param, pattern, op)
     char *param, *pattern;
     int op;
{
  register int len;
  register char *end;
  register char *p, *ret, c;

  if (param == NULL || *param == '\0')
    return (param);
  if (pattern == NULL || *pattern == '\0')      /* minor optimization */
    return (param);

  len = strlen (param);
  end = param + len;

  switch (op)
    {
      case RP_LONG_LEFT:        /* remove longest match at start */
        for (p = end; p >= param; p--)
          {
            c = *p; *p = '\0';
            if ((fnmatch (pattern, param, 0)) != FNM_NOMATCH)
              {
                *p = c;
                return (p);
              }
            *p = c;
          }
        break;

      case RP_SHORT_LEFT:       /* remove shortest match at start */
        for (p = param; p <= end; p++)
          {
            c = *p; *p = '\0';
            if (fnmatch (pattern, param, 0) != FNM_NOMATCH)
              {
                *p = c;
                return (p);
              }
            *p = c;
          }
        break;


      case RP_LONG_RIGHT:       /* remove longest match at end */
        for (p = param; p <= end; p++)
          {
            if (fnmatch (pattern, param, 0) != FNM_NOMATCH)
              {
                c = *p; *p = '\0';
                ret = param;
                *p = c;
                return (ret);
              }
          }
        break;

      case RP_SHORT_RIGHT:      /* remove shortest match at end */
        for (p = end; p >= param; p--)
          {
            if (fnmatch (pattern, param, 0) != FNM_NOMATCH)
              {
                c = *p; *p = '\0';
                ret = param;
                *p = c;
                return (ret);
              }
          }
        break;
    }
  return (param);  /* no match, return original string */
}




int main( int argc, char **argv ) {
 char		*fileext, *name_p, *temp_p;
 char		*target;
 char		*complete_msg = 0;
 char		*update_msg = 0;
 char		*race_msg = 0;
 char		*sfv_msg = 0;
 char		*newleader_msg = 0;
 char		*halfway_msg = 0;
 char		*complete_bar = 0;
 char		*error_msg;
 unsigned long	crc, s_crc;
 unsigned char	exit_value = 0;
 unsigned char	no_check = FALSE;
 unsigned char	complete_type = 0;
 int		cnt;
 int		n;
 int		write_log;
 struct	stat	fileinfo;

#if ( benchmark_mode == TRUE )
 struct timeval bstart, bstop;
 d_log("Reading time for benchmark\n");
 gettimeofday(&bstart, (struct timezone *)0 );
#endif

 umask(0666 & 000);
 
#if ( program_uid > 0 )
 d_log("Trying to change effective gid\n");
 setegid(program_gid);
 d_log("Trying to change effective uid\n");
 seteuid(program_uid);
#endif

 if ( argc != 4 ) {
	d_log("Wrong number of arguments used\n");
	printf(" - - ZipScript-C v" VERSION " - - dark0n3 (c) 2001 - 2004 - -\n\nUsage: %s <filename> <path> <crc>\n\n", argv[0]);
	exit(1);
	}

 d_log("Clearing arrays\n");
 bzero(&raceI.total, sizeof(struct race_total));
 raceI.misc.slowest_user[0] = 30000;
 raceI.misc.fastest_user[0] =
 raceI.misc.release_type = 0;

 //gettimeofday(&raceI.transfer_stop, (struct timezone *)0 );

 d_log("Reading data from environment Variables\n");
 sprintf(raceI.user.name, getenv("USER"));
 sprintf(raceI.user.group, getenv("GROUP"));
 if ( sprintf(raceI.user.group, getenv("GROUP")) == 0 ) {
        memcpy(raceI.user.group, "NoGroup", 8);
 }
 sprintf(raceI.user.tagline, getenv("TAGLINE"));
 raceI.file.speed=atoi(getenv("SPEED"));
// raceI.file.speed=strtol(getenv("SPEED"),NULL,0);
 if (raceI.file.speed==0) raceI.file.speed=1;
 raceI.file.speed*=1024;

 raceI.file.name = argv[1];
 d_log("Checking the file size of %s\n", raceI.file.name);
 stat(raceI.file.name, &fileinfo);
 raceI.file.size = fileinfo.st_size;
 /* Store mtime */
 raceI.transfer_stop.tv_sec = fileinfo.st_mtime;
 raceI.transfer_stop.tv_usec = 0;
 /* Store upload duration */
 raceI.transfer_start.tv_sec = raceI.transfer_stop.tv_sec - (raceI.file.size/raceI.file.speed);
 raceI.transfer_start.tv_usec = 0;





 n = (locations.length_path = strlen(locations.path = argv[2])) + 1;

 d_log("Allocating memory for variables\n");
 locations.race   = m_alloc(n += 10 + (locations.length_zipdatadir = sizeof(storage) - 1));
 locations.sfv    = m_alloc(n);
 locations.leader = m_alloc(n);

 target	= m_alloc(n + 256);
 userI  = malloc(sizeof(struct USERINFO *) * 30);     
 groupI = malloc(sizeof(struct GROUPINFO *) * 30);

 d_log("Copying data locations into memory\n");
 sprintf(locations.sfv, storage "/%s/sfvdata", locations.path);
 sprintf(locations.leader, storage "/%s/leader", locations.path);
 sprintf(locations.race, storage "/%s/racedata", locations.path);

 d_log("Changing directory to %s\n", locations.path);
 chdir(locations.path);


	/* Get file extension */

 d_log("Parsing file extension from filename...\n");
 for ( temp_p = name_p = argv[1]; *name_p != 0 ; name_p++ ) {
	if ( *name_p == '.' ) {
		temp_p = name_p;        
		}
	}

 if ( *temp_p != '.' ) {
	d_log("Got: no extension\n");
	temp_p = name_p;
	} else {
	d_log("Got: %s\n", temp_p);
	temp_p++;
	}
 name_p++;

 d_log("Copying lowercased version of extension to memory\n");
 fileext = malloc(name_p - temp_p);
 memcpy(fileext, temp_p, name_p - temp_p);
 strtolower(fileext);

 d_log("Reading directory structure\n");
 rescandir();

 d_log("Caching release name\n");
 getrelname(locations.path);

 d_log("Creating directory to store racedata in\n");
 maketempdir(&locations);
 printf(zipscript_header);

	/* No check directories */

 if ( matchpath(nocheck_dirs, locations.path) ) {
	d_log("Directory matched with nocheck_dirs\n");
	no_check = TRUE;
	}

	/* Empty file recieved	*/

 if ( raceI.file.size == 0 ) {
	d_log("File seems to be 0\n");
	sprintf(raceI.misc.error_msg, EMPTY_FILE);
	exit_value = 2;
	}

	/* Process file		*/

 switch ( get_filetype(fileext) ) {
	case 0: /* ZIP */
		d_log("File type is: ZIP\n");
		d_log("Testing file integrity with unzip\n");
		sprintf(target, "/bin/unzip -qqt %s", raceI.file.name);
		if (execute(target) != 0) {
			d_log("Integriy check failed\n");
			sprintf(raceI.misc.error_msg, BAD_ZIP);
			exit_value = 2;
			break;
			}
		d_log("Integrity ok\n");
		printf(zipscript_zip_ok);

		if ( (raceI.misc.write_log = matchpath(zip_dirs, locations.path)) ) {
			d_log("Directory matched with zip_dirs\n");
		} else if (matchpath(sfv_dirs, locations.path)) {
			d_log("Directory matched with sfv_dirs\n");
			no_check = FALSE;
			break;
		}

		if ( ! fileexists("file_id.diz") ) {
			d_log("file_id.diz does not exist, trying to extract it from %s\n", raceI.file.name);
			sprintf(target, "/bin/unzip -qqjnCL %s file_id.diz", raceI.file.name);
			execute(target);
			chmod("file_id.diz",0666);
		}

		d_log("Reading diskcount from diz\n");
		raceI.total.files = read_diz("file_id.diz");

		if ( raceI.total.files == 0 ) {
			d_log("Could not get diskcount from diz\n");
			raceI.total.files = 1;
			unlink("file_id.diz");
			}
		raceI.total.files_missing = raceI.total.files;

		d_log("Storing new race data\n");
		writerace_file(&locations, &raceI, 0, F_CHECKED);
		d_log("Reading race data from file to memory\n");
		readrace_file(&locations, &raceI, userI, groupI);
		if ( raceI.total.files_missing < 0 ) {
			d_log("There seems to be more files in zip than we expected\n");
			raceI.total.files -= raceI.total.files_missing;
			raceI.total.files_missing = raceI.misc.write_log = 0;
			}

		d_log("Setting message pointers\n");
		race_msg = zip_race;
		update_msg = zip_update;
		halfway_msg = CHOOSE(raceI.total.users, zip_halfway, zip_norace_halfway);
		newleader_msg = zip_newleader;

		break;
		/* END OF ZIP */


	case 1:	/* SFV */
		d_log("File type is: SFV\n");
		if ( fileexists(locations.sfv) ) {
			if (deny_double_sfv == TRUE && findfileextcount(".sfv") > 1 ) {
				write_log = raceI.misc.write_log;
				raceI.misc.write_log = 1;
				d_log("No double sfv allowed\n");
				error_msg = convert(&raceI,userI,groupI,deny_double_msg);
				writelog(error_msg, "DOUBLESFV");
				sprintf(raceI.misc.error_msg, DOUBLE_SFV);
				exit_value = 2;
				raceI.misc.write_log = write_log;
				break;
			} else {
				d_log("Reading remainders of old sfv\n");
				readsfv_file(&locations, &raceI, 1);
				cnt = raceI.total.files - raceI.total.files_missing;
				raceI.total.files_missing = raceI.total.files = 0;
				readsfv_ffile(raceI.file.name, raceI.file.size);
				if ( (raceI.total.files - raceI.total.files_missing) &&  ! cnt ) {
					write_log = raceI.misc.write_log;
					raceI.misc.write_log = 1;
					d_log("Old sfv seems to match with more files than current one\n");
					strcpy(raceI.misc.error_msg, "SFV does not match with files!");
					error_msg = convert(&raceI,userI,groupI,deny_double_msg);
					writelog(error_msg, "DOUBLESFV");
					sprintf(raceI.misc.error_msg, DOUBLE_SFV);
 					exit_value = 2;
					raceI.misc.write_log = write_log;
					break;
				}
				raceI.total.files = raceI.total.files_missing = 0;
			}
		 }
		d_log("Parsing sfv and creating sfv data\n");
		copysfv_file(raceI.file.name, locations.sfv, raceI.file.size);

#if ( force_sfv_first == FALSE )
		if (fileexists(locations.race)) {
			d_log("Testing files marked as untested\n");
			testfiles_file(&locations, &raceI);
			rescandir();
			}
#endif
		d_log("Reading file count from SFV\n");
		readsfv_file(&locations, &raceI, 0);

		if ( raceI.total.files == 0 ) {
			d_log("SFV seems to have no files of accepted types\n");
			sprintf(raceI.misc.error_msg, EMPTY_SFV);
			exit_value = 2;
			break;
			}

		printf(zipscript_sfv_ok);
		if (fileexists(locations.race)) {
			d_log("Reading race data from file to memory\n");
			readrace_file(&locations, &raceI, userI, groupI);
			}

		d_log("Making sure that release is not marked as complete\n");
		removecomplete();

		d_log("Setting message pointers\n");
		switch( raceI.misc.release_type ) {
			case 1:
				sfv_msg = rar_sfv;
				break;
			case 2:
				sfv_msg = other_sfv;
				break;
			case 3:
				sfv_msg = audio_sfv;
				break;
			case 4:
				sfv_msg = video_sfv;
				break;
			}
		halfway_msg = newleader_msg = race_msg = update_msg = NULL;

		raceI.misc.write_log = matchpath(sfv_dirs, locations.path);
		if ( raceI.total.files_missing > 0 ) {
			if ( sfv_msg != NULL ) {
				d_log("Writing SFV message to %s\n", log);
				writelog(convert(&raceI, userI, groupI, sfv_msg), "SFV");
			}
		} else {
			if ( raceI.misc.release_type == 3 ) {
				d_log("Reading audio info for completebar\n");
				get_mpeg_audio_info(findfileext(".mp3"), &raceI.audio);
			}
		}
		break;
	case 2:
		/* NFO */
		no_check = TRUE;
		d_log("File type is: NFO\n");
		writerace_file(&locations, &raceI, 0, F_NFO);

		if ( enable_nfo_script == TRUE )
			{
		        d_log("Executing nfo script\n");
		        sprintf(target, nfo_script " %s", raceI.file.name);
		        execute(target);
			}
                if ( enable_nfo_script2 == TRUE )
			{
			d_log("Executing 2nd nfo script\n");
			sprintf(target, nfo_script2 " %s", raceI.file.name);
			execute(target);
			}

		break;
		/* END OF NFO CHECK */


	case 3:
		/* SFV BASED CRC-32 CHECK */
		d_log("File type is: ANY\n");

		d_log("Converting crc from string to integer\n");
		crc = hexstrtodec((unsigned char*)argv[3]);
		if (fileexists(locations.sfv)) {
			if ( crc == 0 ) {
				d_log("We did not get crc from ftp daemon, calculating crc now...\n");
				crc=calc_crc32(raceI.file.name);
//				d_log("We did not get crc from ftp daemon\n");
//				sprintf(raceI.misc.error_msg, ZERO_CRC);
//				exit_value = 2;
//				break;
				}
//				else
			if ((s_crc = readsfv_file(&locations, &raceI, 0)) != crc ) {
				if ( s_crc == 0 ) {
					d_log("Filename was not found in the SFV\n");
					strcpy(raceI.misc.error_msg, NOT_IN_SFV);
					} else {
					d_log("CRC-32 check failed\n");
					strcpy(raceI.misc.error_msg, BAD_CRC);
					}
				exit_value = 2;
				break;
				}
			printf(zipscript_SFV_ok);
			d_log("Storing new race data\n");
			writerace_file(&locations, &raceI, crc, F_CHECKED);
			} else {
#if ( force_sfv_first == TRUE )
			d_log("SFV needs to be uploaded first\n");
			strcpy(raceI.misc.error_msg, SFV_FIRST);
			exit_value = 2;
			break;
#else
			d_log("Could not check file yet - SFV is not present\n");
			printf(zipscript_SFV_skip);
			d_log("Storing new race data\n");
			writerace_file(&locations, &raceI, crc, F_NOTCHECKED);
#endif
			}

		raceI.misc.write_log = matchpath(sfv_dirs, locations.path);

		d_log("Removing missing indicator\n");
		sprintf(target, "%s-missing", raceI.file.name);
		strtolower(target);
		unlink(target);

		d_log("Reading race data from file to memory\n");
		readrace_file(&locations, &raceI, userI, groupI);

		d_log("Setting pointers\n");
		if ( raceI.misc.release_type == 0 ) {
			if ( israr(fileext)) { /* RAR */
				raceI.misc.release_type = 1;
				} else if ( isvideo(fileext)) { /* AVI/MPEG */
				raceI.misc.release_type = 4;
				} else if ( ! memcmp(fileext, "mp3", 4)) { /* MP3 */
				raceI.misc.release_type = 3;
				} else { /* OTHER */
				raceI.misc.release_type = 2;
				}
			}

		switch ( raceI.misc.release_type ) {
			case 1:
				get_rar_info(raceI.file.name);
				race_msg = rar_race;
				update_msg = rar_update;
				halfway_msg = CHOOSE(raceI.total.users, rar_halfway, rar_norace_halfway);
				newleader_msg = rar_newleader;
				break;
			case 2:
				race_msg = other_race;
				update_msg = other_update;
				halfway_msg = CHOOSE(raceI.total.users, other_halfway, other_norace_halfway);
				newleader_msg = other_newleader;
				break;
			case 3:
				d_log("Trying to read audio header and tags\n");
				get_mpeg_audio_info(raceI.file.name, &raceI.audio);
#if ( exclude_non_sfv_dirs == TRUE )
				if ( raceI.misc.write_log == TRUE ) {
#endif
						if ( ! matchpath(no_audio_check_dirs, locations.path) ) {
#if ( audio_banned_genre_check == TRUE )
					if ( strcomp(banned_genres, raceI.audio.id3_genre)) {
						d_log("File is from banned genre\n");
						sprintf(raceI.misc.error_msg, BANNED_GENRE, raceI.audio.id3_genre);
						if ( audio_genre_warn == TRUE ) {
							if ( userI[raceI.user.pos]->files == 1 ) {
								d_log("warn on - logging to logfile\n");
								write_log = raceI.misc.write_log;
								raceI.misc.write_log = 1;
								error_msg = convert(&raceI,userI,groupI,audio_genre_warn_msg);
								writelog(error_msg, "BADGENRE");
								raceI.misc.write_log = write_log;
								} else {
								d_log("warn on - have already logged to logfile\n");
								}
							} else {
							exit_value = 2;
							}
						break;
						}
#endif
#if ( audio_allowed_genre_check == TRUE )
					if ( ! strcomp(allowed_genres, raceI.audio.id3_genre)) {
						d_log("File is not in allowed genre\n");
						sprintf(raceI.misc.error_msg, BANNED_GENRE, raceI.audio.id3_genre);
						if ( audio_genre_warn == TRUE ) {
							if ( userI[raceI.user.pos]->files == 1 ) {
								d_log("warn on - logging to logfile\n");
								write_log = raceI.misc.write_log;
								raceI.misc.write_log = 1;
								error_msg = convert(&raceI,userI,groupI,audio_genre_warn_msg);
								writelog(error_msg, "BADGENRE");
								raceI.misc.write_log = write_log;
								} else {
								d_log("warn on - have already logged to logfile\n");
								}
							} else {
							exit_value = 2;
							}
						break;
						}

#endif
#if ( audio_year_check == TRUE )
					if ( ! strcomp(allowed_years, raceI.audio.id3_year)) { 
						d_log("File is from banned year\n");
						sprintf(raceI.misc.error_msg, BANNED_YEAR, raceI.audio.id3_year);
						if ( audio_year_warn == TRUE ) {
							if ( userI[raceI.user.pos]->files == 1 ) {
								d_log("warn on - logging to logfile\n");
								write_log = raceI.misc.write_log;
								raceI.misc.write_log = 1;
								error_msg = convert(&raceI,userI,groupI,audio_year_warn_msg);
								writelog(error_msg, "BADYEAR");
								raceI.misc.write_log = write_log;
								} else {
								d_log("warn on - have already logged to logfile\n");
								}
							} else {
							exit_value = 2;
							}
						break;
					        }
#endif
#if ( audio_bitrate_check == TRUE ) 
					if ( ! strcomp(allowed_bitrates, raceI.audio.bitrate)) {
						d_log("File is encoded using banned bitrate\n");
						sprintf(raceI.misc.error_msg, BANNED_BITRATE, raceI.audio.bitrate);
						if ( audio_bitrate_warn == TRUE ) {
							if ( userI[raceI.user.pos]->files == 1 ) {
								d_log("warn on - logging to logfile\n");
								write_log = raceI.misc.write_log;
								raceI.misc.write_log = 1;
								error_msg = convert(&raceI,userI,groupI,audio_bitrate_warn_msg);
								writelog(error_msg, "BADBITRATE");
								raceI.misc.write_log = write_log;
								} else {
								d_log("warn on - have already logged to logfile\n");
								}
							} else {
							exit_value = 2;
							}
						break;
						}
#endif
						} else {
					d_log("user is in a no audio check dir - skipping checks.\n");
						}
#if ( exclude_non_sfv_dirs == TRUE )
					}
#endif
				printf(convert(&raceI, userI, groupI, realtime_mp3_info));
				race_msg = audio_race;
				update_msg = audio_update;
				halfway_msg = CHOOSE(raceI.total.users, audio_halfway, audio_norace_halfway);
				newleader_msg = audio_newleader;
				break;
			case 4:
				d_log("Trying to read video header\n");
				if ( ! memcmp(fileext, "avi", 3 )) {
					avi_video(raceI.file.name, &raceI.video);
					} else {
					mpeg_video(raceI.file.name, &raceI.video);
					}
				race_msg = video_race;
				update_msg = video_update;
				halfway_msg = CHOOSE(raceI.total.users, video_halfway, video_norace_halfway);
				newleader_msg = video_newleader;
				break;
			}
		/* END OF SFV CHECK */
		break;
	case 4:
		/* ACCEPTED FILE */
		d_log("File type: NO CHECK\n"); 
		no_check = TRUE;
		break;
	case 255:
		/* UNKNOWN - WE DELETE THESE, SINCE IT WAS ALSO IGNORED */
		d_log("File type: UNKNOWN [ignored in sfv]\n");

		sprintf(raceI.misc.error_msg, UNKNOWN_FILE, fileext);
		exit_value = 2;
		/* END OF UNKNOWN CHECK */
		break;
	}




 if ( no_check == TRUE ) {	/* File was not checked */
	printf(zipscript_any_ok);
	printf(convert(&raceI, userI, groupI, zipscript_footer_skip));
	} else if ( exit_value == 0 ) {		/* File was checked */

	if ( raceI.total.users > 0 ) {
		d_log("Sorting race stats\n");
		sortstats(&raceI, userI, groupI);
#if ( get_user_stats == TRUE )
		d_log("Reading day/week/month/all stats for racers\n");
		get_stats(&raceI, userI);
#endif
		d_log("Printing on-site race info\n");
		showstats(&raceI, userI, groupI);
		
		/* Modification by <daxxar@mental.mine.nu>
		*  Only write new leader if he leads with newleader_files_ahead
		*  or only one person is racing if enable_files_ahead :)
		*/
		if ( !enable_files_ahead || ((raceI.total.users > 1 && userI[userI[0]->pos]->files >= (userI[userI[1]->pos]->files + newleader_files_ahead)) || raceI.total.users == 1) ) {
			d_log("Writing current leader to file\n");
			read_write_leader_file(&locations, &raceI, userI[userI[0]->pos]);
		}
		
		if ( raceI.total.users > 1 ) {
			if ( userI[raceI.user.pos]->files == 1 && race_msg != NULL ) {
				d_log("Writing RACE to %s\n", log);
				writelog(convert(&raceI, userI, groupI, race_msg), "RACE");
				}
			/* Modification by <daxxar@mental.mine.nu>
			*  Only announce new leader if he leads with
			*  newleader_files_ahead files :-)
			*/
			if ( raceI.total.files >= min_newleader_files && strcmp(raceI.misc.old_leader, userI[userI[0]->pos]->name) && newleader_msg != NULL && userI[userI[0]->pos]->files >= (userI[userI[1]->pos]->files + newleader_files_ahead) ) {
				d_log("Writing NEWLEADER to %s\n", log);
				writelog(convert(&raceI, userI, groupI, newleader_msg), "NEWLEADER");
				}
			} else {
			if ( userI[raceI.user.pos]->files == 1 && raceI.total.files >= min_update_files && update_msg != NULL ) {
				d_log("Writing UPDATE to %s\n", log);
				writelog(convert(&raceI, userI, groupI, update_msg), "UPDATE");
				}
			}
		}

	if ( raceI.total.files_missing > 0 ) {

			/* Release is incomplete */

		if ( raceI.total.files_missing == raceI.total.files >> 1 && raceI.total.files >= min_halfway_files && halfway_msg != NULL ) {
			d_log("Writing COMPLETE to %s\n", log);
			writelog(convert(&raceI, userI, groupI, halfway_msg), "HALFWAY");
			}

			/* It is _very_ unlikely that halfway would be announced on complete release ;) */

		d_log("Caching progress bar\n");
		buffer_progress_bar(&raceI);

		d_log("Creating incomplete indicator\n");
		create_incomplete();

		d_log("Creating/moving progress bar\n");
		move_progress_bar(0, &raceI);

		printf(convert(&raceI, userI, groupI, zipscript_footer_ok));

	} else if (( raceI.total.files_missing == 0 ) && ( raceI.total.files > 0 )) {

			/* Release is complete */

		d_log("Caching progress bar\n");
		buffer_progress_bar(&raceI);
		printf(convert(&raceI, userI, groupI, zipscript_footer_ok));

		d_log("Setting complete pointers\n");
		switch ( raceI.misc.release_type ) {
			case 0:
				complete_bar = zip_completebar;
				complete_msg = CHOOSE(raceI.total.users, zip_complete, zip_norace_complete);
				complete_type = CHOOSE(raceI.total.users, zip_complete_type, zip_norace_complete_type);
				break;
			case 1:
				complete_bar = rar_completebar;
				complete_msg = CHOOSE(raceI.total.users, rar_complete, rar_norace_complete);
				complete_type = CHOOSE(raceI.total.users, rar_complete_type, rar_norace_complete_type);
				break;
			case 2:
				complete_bar = other_completebar;
 				complete_msg = CHOOSE(raceI.total.users, other_complete, other_norace_complete);
				complete_type = CHOOSE(raceI.total.users, other_complete_type, other_norace_complete_type);
				break;
			case 3:
				complete_bar = audio_completebar;
 				complete_msg = CHOOSE(raceI.total.users, audio_complete, audio_norace_complete);
				complete_type = CHOOSE(raceI.total.users, audio_complete_type, audio_norace_complete_type);
				
				d_log("Symlinking audio\n");
				if ( ! strncasecmp(locations.link_target, "VA", 2) && (locations.link_target[2] == '-' || locations.link_target[2] =='_')) {
					memcpy(raceI.audio.id3_artist, "VA", 3);
					}

				if ( raceI.misc.write_log == TRUE ) {
#if ( audio_genre_sort == TRUE )
					d_log("Sorting mp3 by genre\n");
					createlink(audio_genre_path, raceI.audio.id3_genre, locations.link_source, locations.link_target);
#endif
#if ( audio_artist_sort == TRUE )
					d_log("Sorting mp3 by artist\n");
					if ( *raceI.audio.id3_artist ) {
						if ( memcmp(raceI.audio.id3_artist, "VA", 3) ) {
							sprintf(raceI.audio.id3_artist, "%c", toupper(*raceI.audio.id3_artist));
							createlink(audio_artist_path, raceI.audio.id3_artist, locations.link_source, locations.link_target);
							} else {
							createlink(audio_artist_path, "VA", locations.link_source, locations.link_target);
							}
						}
#endif
#if ( audio_year_sort == TRUE )
					d_log("Sorting mp3 by year\n");
					if ( *raceI.audio.id3_year != 0 ) {
						createlink(audio_year_path, raceI.audio.id3_year, locations.link_source, locations.link_target);
						}
#endif
#if ( audio_group_sort == TRUE )
					d_log("Sorting mp3 by group\n");
					temp_p=remove_pattern(locations.link_target,"*-",RP_LONG_LEFT);
					temp_p=remove_pattern(temp_p,"_",RP_SHORT_LEFT);
					n=strlen(temp_p);
					if ( n > 0 && n < 15 ) {
						d_log("Valid groupname found: %s (%i)\n", temp_p, n);
						createlink(audio_group_path, temp_p, locations.link_source, locations.link_target);
						}
#endif
					}
#if ( create_m3u == TRUE )
				d_log("Creating m3u\n");
				cnt = sprintf(target, findfileext(".sfv"));
				strcpy(target + cnt - 3, "m3u");
				create_indexfile_file(&locations, &raceI, target);
#endif
				break;
			case 4:
				complete_bar = video_completebar;
 				complete_msg = CHOOSE(raceI.total.users, video_complete, video_norace_complete);
				complete_type = CHOOSE(raceI.total.users, video_complete_type, video_norace_complete_type);
				break;
			}

		d_log("Removing old complete bar, if any\n");
		removecomplete();
		d_log("Writing .message file\n");
		complete(&locations, &raceI, userI, groupI, complete_type);

		if ( complete_msg != NULL ) {
			d_log("Writing COMPLETE and STATS to %s\n", log);
			writelog(convert(&raceI, userI, groupI, complete_msg), "COMPLETE");
			writetop(&locations, &raceI, userI, groupI, complete_type);
			}

		d_log("Creating complete bar\n");
		createstatusbar(convert(&raceI, userI, groupI, complete_bar));

#if ( enable_complete_script == TRUE )
		d_log("Executing complete script\n");
		sprintf(target, complete_script " %s", raceI.file.name);
		execute(target);
#endif
#if ( enable_complete_script2 == TRUE )
		d_log("Executing 2nd complete script\n");
		sprintf(target, complete_script2 " %s", raceI.file.name);
		execute(target);
#endif
		} else {

			/* Release is at unknown state */

		raceI.total.files = -raceI.total.files_missing;
		raceI.total.files_missing = 0;
		printf(convert(&raceI, userI, groupI, zipscript_footer_unknown));
		}
	} else {
			/* File is marked to be deleted */

	d_log("Logging file as bad\n");
	writerace_file(&locations, &raceI, 0, F_BAD);
	printf(convert(&raceI, userI, groupI, zipscript_footer_error));
	}

#if ( enable_accept_script == TRUE )
 if ( exit_value == 0 ) {
	d_log("Executing accept script\n");
	sprintf(target, accept_script " %s", raceI.file.name);
	execute(target);
	}
#endif
#if ( enable_accept_script2 == TRUE )
 if ( exit_value == 0 ) {
	d_log("Executing 2nd accept script\n");
	sprintf(target, accept_script2 " %s", raceI.file.name);
	execute(target);
	}
#endif

 d_log("Relasing memory\n");
 free(locations.link_source);
 free(raceI.misc.release_name);
 m_free(target);
 m_free(fileext);
 m_free(locations.race);
 m_free(locations.sfv);
 m_free(locations.leader);

#if ( benchmark_mode == TRUE ) 
 gettimeofday(&bstop, (struct timezone *)0);
 printf("Checks completed in %0.6f seconds\n", ((bstop.tv_sec - bstart.tv_sec) + (bstop.tv_usec - bstart.tv_usec) / 1000000.));
#endif

 d_log("Exit\n");
 return exit_value;
}
