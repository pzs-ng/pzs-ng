#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "config.h"

#include "zsfunctions.h"
#include "race-file.h"
#include "objects.h"
#include "macros.h"

#ifdef _WITH_SS5
	#include "constants.ss5.h"
#else
	#include "constants.h"
#endif

#include "errors.h"
#include "multimedia.h"
#include "convert.h"
#include "dizreader.h"
#include "stats.h"
#include "complete.h"
#include "crc.h"

#include "../conf/zsconfig.h"

/* Remove the portion of PARAM matched by PATTERN according to OP, where OP
 * can have one of 4 values:
 * RP_LONG_LEFT    remove longest matching portion at start of PARAM
 * RP_SHORT_LEFT   remove shortest matching portion at start of PARAM
 * RP_LONG_RIGHT   remove longest matching portion at end of PARAM
 * RP_SHORT_RIGHT  remove shortest matching portion at end of PARAM
 */

#define RP_LONG_LEFT    1
#define RP_SHORT_LEFT   2
#define RP_LONG_RIGHT   3
#define RP_SHORT_RIGHT  4

struct ONLINE	*online;
struct USERINFO  **userI;
struct GROUPINFO **groupI;
struct VARS      raceI;
struct LOCATIONS locations;

void writelog(char *msg, char *status) {
	FILE   *glfile;
	char   *date;
	char   *line, *newline;
	time_t timenow;

	if ( raceI.misc.write_log == TRUE && !matchpath(group_dirs, locations.path)) {
		timenow = time(NULL);
		date = ctime(&timenow);
		if (!(glfile = fopen(log, "a+"))) {
			d_log("Unable to open %s for read/write (append) - NO RACEINFO WILL BE WRITTEN!\n",log);
			return;
		}

		line = newline = msg;
		while ( 1 ) {
			switch ( *newline++ ) {
			case 0:
				fprintf(glfile, "%.24s %s: \"%s\" %s\n", date, status, locations.path, line);
				fclose(glfile);
				return;
			case '\n':
				fprintf(glfile, "%.24s %s: \"%s\" %.*s\n", date, status, locations.path, (int)(newline - line - 1), line);
				line = newline;
				break;
			}
		}
	}
}

void remove_nfo_indicator(char *directory) {
	int	cnt,
	l[2],
	n = 0,
	k = 2;
	char	*path[2];

	for ( cnt = locations.length_path - 1 ; k && cnt ; cnt-- ) {
		if ( directory[cnt] == '/' ) {
			k--;
			l[k] = n;
			path[k] = malloc(n + 1);
			strncpy(path[k], directory + cnt + 1, n);
			path[k][n] = 0;
			n = 0;
		} else n++;
	}
	locations.nfo_incomplete = i_incomplete(incomplete_nfo_indicator, path);
	if (fileexists(locations.nfo_incomplete))
		unlink(locations.nfo_incomplete);
	locations.nfo_incomplete = i_incomplete(incomplete_cd_nfo_indicator, path);
	if (fileexists(locations.nfo_incomplete))
		unlink(locations.nfo_incomplete);
	if (k < 2) free(path[1]);
	if (k == 0) free(path[0]);
}

void getrelname(char *directory) {
	int	cnt,
	l[2],
	n = 0,
	k = 2;
	char	*path[2];

	for ( cnt = locations.length_path - 1 ; k && cnt ; cnt-- ) {
		if ( directory[cnt] == '/' ) {
			k--;
			l[k] = n;
			path[k] = malloc(n + 1);
			strncpy(path[k], directory + cnt + 1, n);
			path[k][n] = 0;
			n = 0;
		} else n++;
	}

	if (( ! strncasecmp(path[1], "CD"  , 2) && l[1] <= 4 ) ||
		( ! strncasecmp(path[1], "DISC", 4) && l[1] <= 6 ) ||
		( ! strncasecmp(path[1], "DISK", 4) && l[1] <= 6 ) ||
		( ! strncasecmp(path[1], "DVD" , 3) && l[1] <= 5 ) ||
		( ! strncasecmp(path[1], "SUB" , 3) && l[1] <= 4 ) ||
		( ! strncasecmp(path[1], "SUBTITLES" , 9) && l[1] <= 9 )) {
		raceI.misc.release_name = malloc(l[0] + 18);
		locations.link_source = malloc(n = (locations.length_path - l[1]));
		sprintf(raceI.misc.release_name, "%s/%s", path[0], path[1]);
		sprintf(locations.link_source, "%.*s", n - 1, locations.path);
		locations.link_target = path[0];
		locations.incomplete = c_incomplete(incomplete_cd_indicator, path);
#if (show_missing_nfo_in_cd == TRUE)
		locations.nfo_incomplete = i_incomplete(incomplete_cd_nfo_indicator, path);
#else
		locations.nfo_incomplete = NULL;
#endif
		if (k < 2) free(path[1]);
	} else {
		raceI.misc.release_name = malloc(l[1] + 10);
		locations.link_source	= malloc(locations.length_path + 1);
		strcpy(locations.link_source, locations.path);
		sprintf(raceI.misc.release_name, "%s", path[1]);
		locations.link_target = path[1];
		locations.incomplete = c_incomplete(incomplete_indicator, path);
		locations.nfo_incomplete = i_incomplete(incomplete_nfo_indicator, path);
	}
		if (k == 0) free(path[0]);
}

unsigned char get_filetype(char *ext) {
    if (!strcasecmp(ext, "zip")) return 0;
    if (!strcasecmp(ext, "sfv")) return 1;
    if (!strcasecmp(ext, "nfo")) return 2;
    if ( strcomp(allowed_types, ext) && !matchpath(allowed_types_exemption_dirs, locations.path )) return 4;
    if (!strcomp(ignored_types, ext)) return 3;

    return 255;
}

#if ( audio_group_sort == TRUE )
static char *remove_pattern (param, pattern, op)
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
#endif

int main( int argc, char **argv ) {
    char		*fileext, *name_p, *temp_p = 0;
    char		*target = 0;
    char		*complete_msg = 0;
    char		*update_msg = 0;
    char		*race_msg = 0;
    char		*sfv_msg = 0;
    char		*update_type = 0;
    char		*newleader_msg = 0;
    char		*halfway_msg = 0;
    char		*complete_bar = 0;
    char		*error_msg = 0;
    unsigned int	crc, s_crc = 0;
    unsigned char	exit_value = EXIT_SUCCESS;
    unsigned char	no_check = FALSE;
    unsigned char	complete_type = 0;
    char		*complete_announce = 0;
    int			cnt, cnt2, n = 0;
    int			write_log = 0;
    struct stat		fileinfo;

#if ( benchmark_mode == TRUE )
    struct timeval bstart, bstop;
    d_log("Reading time for benchmark\n");
    gettimeofday(&bstart, (struct timezone *)0 );
#endif

/* Adding version-number to head if .debug message
   15.09.2004 - psxc
 */
   d_log("Project-ZS Next Generation (pzs-ng) v%s debug log.\n", VERSION);

    umask(0666 & 000);

#if ( program_uid > 0 )
    d_log("Trying to change effective gid\n");
    setegid(program_gid);
    d_log("Trying to change effective uid\n");
    seteuid(program_uid);
#endif

    if ( argc != 4 ) {
	d_log("Wrong number of arguments used\n");
	printf(" - - PZS-NG ZipScript-C v" VERSION " - - dark0n3 (c) 2001 - 2004 - -\n\nUsage: %s <filename> <path> <crc>\n\n", argv[0]);
	exit(1);
    }

    d_log("Clearing arrays\n");
    bzero(&raceI.total, sizeof(struct race_total));
    raceI.misc.slowest_user[0] = 30000;
    raceI.misc.fastest_user[0] =
	raceI.misc.release_type = RTYPE_NULL;

    /*gettimeofday(&raceI.transfer_stop, (struct timezone *)0 );*/

    raceI.file.name=argv[1];
    locations.path = argv[2];
    d_log("Changing directory to %s\n", locations.path);
    chdir(locations.path);

    d_log("Reading data from environment variables\n");
    if (!(getenv("USER") && getenv("GROUP") && getenv("TAGLINE") && getenv("SPEED"))) {
	d_log("We are running from shell, falling back to default values for $USER, $GROUP, $TAGLINE, $SECTION and $SPEED\n");
	/*strcpy(raceI.user.name, "Unknown");
	  strcpy(raceI.user.group, "NoGroup");*/

	buffer_groups( GROUPFILE );
	buffer_users( PASSWDFILE );
	fileinfo.st_uid = geteuid();
	fileinfo.st_gid = getegid();
	strcpy(raceI.user.name, get_u_name(fileinfo.st_uid));
	strcpy(raceI.user.group, get_g_name(fileinfo.st_gid));

	memcpy(raceI.user.tagline, "No Tagline Set", 15);
	raceI.file.speed=2004;
	raceI.section=0;
    } else {
	sprintf(raceI.user.name, getenv("USER"));
	sprintf(raceI.user.group, getenv("GROUP"));
	if (strlen(raceI.user.group)==0) memcpy(raceI.user.group, "NoGroup", 8);
	sprintf(raceI.user.tagline, getenv("TAGLINE"));
	if (strlen(raceI.user.tagline)==0) memcpy(raceI.user.tagline, "No Tagline Set", 15);
	raceI.file.speed=(unsigned int)strtol(getenv("SPEED"),NULL,0);
	if (!raceI.file.speed) raceI.file.speed=1;

	d_log("Reading section from env\n");
	if ((temp_p=strdup(gl_sections)) == NULL) {
	    d_log("Can't allocate memory for sections\n");
	} else {
	    n=0;
	    while (temp_p)
		if (strcmp(strsep(&temp_p," "), getenv("SECTION")) == 0) {
		    raceI.section=(unsigned char)n;
		    break;
		} else n++;
	}

    }
    raceI.file.speed *= 1024;

    d_log("Checking the file size of %s\n", raceI.file.name);
    stat(raceI.file.name, &fileinfo);
    raceI.file.size=fileinfo.st_size;
    d_log("File size was: %d\n",raceI.file.size);

    d_log("Setting race times\n");
    raceI.total.stop_time=fileinfo.st_mtime;
    raceI.total.start_time=raceI.total.stop_time-((unsigned int)(raceI.file.size)/raceI.file.speed);
    if ((int)(raceI.total.stop_time - raceI.total.start_time) < 1)
	raceI.total.stop_time = raceI.total.start_time + 1;

    n = (locations.length_path = strlen(locations.path)) + 1;

    d_log("Allocating memory for variables\n");
    locations.race   = m_alloc(n += 10 + (locations.length_zipdatadir = sizeof(storage) - 1));
    locations.sfv    = m_alloc(n);
    locations.leader = m_alloc(n);

    target = m_alloc(n + 256);
    userI  = malloc(sizeof(struct USERINFO *) * 30);
    memset(userI, 0, sizeof(struct USERINFO *) * 30);
    groupI = malloc(sizeof(struct GROUPINFO *) * 30);
    memset(groupI, 0, sizeof(struct GROUPINFO *) * 30);

    d_log("Copying data locations into memory\n");
    sprintf(locations.sfv, storage "/%s/sfvdata", locations.path);
    sprintf(locations.leader, storage "/%s/leader", locations.path);
    sprintf(locations.race, storage "/%s/racedata", locations.path);

/*    d_log("Changing directory to %s\n", locations.path);
    chdir(locations.path);
*/

    /* Get file extension */

    d_log("Parsing file extension from filename... (%s)\n", argv[1]);
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

#if ( sfv_cleanup_lowercase == TRUE )
    d_log("Copying (lowercased version of) extension to memory\n");
#else
    d_log("Copying (unchanged version of) extension to memory\n");
#endif
    fileext = malloc(name_p - temp_p);
    memcpy(fileext, temp_p, name_p - temp_p);
#if ( sfv_cleanup_lowercase == TRUE )
    strtolower(fileext);
#endif
    d_log("Reading directory structure\n");
    rescandir();

    d_log("Caching release name\n");
    getrelname(locations.path);

    d_log("Creating directory to store racedata in\n");
    maketempdir(&locations);
    printf(zipscript_header);

    /* Hide users in group_dirs */
    if ( matchpath(group_dirs, locations.path ) && ( hide_group_uploaders == TRUE )) {
	d_log("Hiding user in group-dir\n");
	sprintf(raceI.user.name, raceI.user.group);
    }

    /* Empty file recieved */
    if ( raceI.file.size == 0 ) {
	d_log("File seems to be 0\n");
	sprintf(raceI.misc.error_msg, EMPTY_FILE);
	exit_value = 2;
    }

    /* No check directories */
    if ( matchpath(nocheck_dirs, locations.path) ) {
	d_log("Directory matched with nocheck_dirs\n");
	no_check = TRUE;
    } else {
	/* Process file */
	switch ( get_filetype(fileext) ) {
	    case 0: /* ZIP CHECK */
		d_log("File type is: ZIP\n");
		d_log("Testing file integrity with unzip\n");
		sprintf(target, "/bin/unzip -qqt \"%s\"", raceI.file.name);
		if (execute(target) != 0) {
		    d_log("Integrity check failed\n");
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
		/* END OF ZIP CHECK */

	    case 1: /* SFV CHECK */
		d_log("File type is: SFV\n");
		if ( fileexists(locations.sfv) ) {
		    if (deny_double_sfv == TRUE && findfileextcount(".sfv") > 1 ) {
			write_log = raceI.misc.write_log;
			raceI.misc.write_log = 1;
			d_log("No double sfv allowed\n");
			error_msg = convert(&raceI,userI,groupI,deny_double_msg);
			writelog(error_msg, general_doublesfv_type);
			sprintf(raceI.misc.error_msg, DOUBLE_SFV);
			exit_value = 2;
			raceI.misc.write_log = write_log;
			break;
		    } else if ( findfileextcount(".sfv") > 1 ) {
			d_log("Reading remainders of old sfv\n");
			readsfv_file(&locations, &raceI, 1);
			cnt = raceI.total.files - raceI.total.files_missing;
			cnt2 = raceI.total.files;
			raceI.total.files_missing = raceI.total.files = 0;
			readsfv_ffile(raceI.file.name, raceI.file.size);
			if ( ( raceI.total.files <= cnt2 ) || ( raceI.total.files != ( cnt + raceI.total.files_missing ) ) ) {
			    write_log = raceI.misc.write_log;
			    raceI.misc.write_log = 1;
			    d_log("Old sfv seems to match with more files than current one\n");
			    strcpy(raceI.misc.error_msg, "SFV does not match with files!");
			    error_msg = convert(&raceI,userI,groupI,deny_double_msg);
			    writelog(error_msg, general_doublesfv_type);
			    sprintf(raceI.misc.error_msg, DOUBLE_SFV);
			    exit_value = 2;
			    raceI.misc.write_log = write_log;
			    break;
			}
			raceI.total.files = raceI.total.files_missing = 0;
		    } else {
			d_log("Hmm.. Seems the old .sfv was deleted. Allowing new one.\n");
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
		switch ( raceI.misc.release_type ) {
		    case RTYPE_RAR:
				sfv_msg = rar_sfv;
				break;
		    case RTYPE_OTHER:
				sfv_msg = other_sfv;
				break;
		    case RTYPE_AUDIO:
				sfv_msg = audio_sfv;
				break;
		    case RTYPE_VIDEO:
				sfv_msg = video_sfv;
				break;
		}
		halfway_msg = newleader_msg = race_msg = update_msg = NULL;

		raceI.misc.write_log = matchpath(sfv_dirs, locations.path);
		if ( raceI.total.files_missing > 0 ) {
		    if ( sfv_msg != NULL ) {
				d_log("Writing SFV message to %s\n", log);
				writelog(convert(&raceI, userI, groupI, sfv_msg), general_sfv_type);
		    }
		} else {
		    if (raceI.misc.release_type == RTYPE_AUDIO) {
				d_log("Reading audio info for completebar\n");
				get_mpeg_audio_info(findfileext(".mp3"), &raceI.audio);
		    }
		}
		break;
		/* END OF SFV CHECK */

	    case 2: /* NFO CHECK */
		no_check = TRUE;
		d_log("File type is: NFO\n");
		writerace_file(&locations, &raceI, 0, F_NFO);

		if ((show_missing_nfo) && (findfileext(".nfo")) && (locations.nfo_incomplete)) {
			d_log("Removing missing-nfo indicator (if any)\n");
			remove_nfo_indicator(locations.path);
		}

#if ( enable_nfo_script == TRUE )
		    d_log("Executing nfo script (%s)\n", nfo_script);
		    sprintf(target, nfo_script " \"%s\"", raceI.file.name);
		    /*if ( execute_old(target) != 0 ) {*/
		    if ( execute(target) != 0 ) {
			d_log("Failed to execute nfo_script!");
		    }
#endif

		break;
		/* END OF NFO CHECK */

	    case 3: /* SFV BASED CRC-32 CHECK */
		d_log("File type is: ANY\n");

		d_log("Converting crc from string to integer\n");
		crc = hexstrtodec((unsigned char*)argv[3]);
		if ( crc == 0 ) {
		    d_log("We did not get crc from ftp daemon, calculating crc now...\n");
		    crc=calc_crc32(raceI.file.name);
		}
		if (fileexists(locations.sfv)) {
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

		d_log("Reading race data from file to memory\n");
		readrace_file(&locations, &raceI, userI, groupI);

		d_log("Setting pointers\n");
		if ( raceI.misc.release_type == RTYPE_NULL ) {
		    if (israr(fileext))
				raceI.misc.release_type = RTYPE_RAR; /* .RAR / .R?? */
		    else if (isvideo(fileext))
				raceI.misc.release_type = RTYPE_VIDEO; /* AVI/MPEG */
		    else if (!memcmp(fileext, "mp3", 4))
				raceI.misc.release_type = RTYPE_AUDIO; /* MP3 */
		    else
				raceI.misc.release_type = RTYPE_OTHER; /* OTHER FILE */
		}

		switch ( raceI.misc.release_type ) {
		    case RTYPE_RAR:
				get_rar_info(raceI.file.name);
				race_msg = rar_race;
				update_msg = rar_update;
				halfway_msg = CHOOSE(raceI.total.users, rar_halfway, rar_norace_halfway);
				newleader_msg = rar_newleader;
				break;
		    case RTYPE_OTHER:
				race_msg = other_race;
				update_msg = other_update;
				halfway_msg = CHOOSE(raceI.total.users, other_halfway, other_norace_halfway);
				newleader_msg = other_newleader;
				break;
		    case RTYPE_AUDIO:
				d_log("Trying to read audio header and tags\n");
				get_mpeg_audio_info(raceI.file.name, &raceI.audio);
#if ( exclude_non_sfv_dirs == TRUE )
	if ( raceI.misc.write_log == TRUE ) {
#endif
			    if (( enable_mp3_script == TRUE ) && ( userI[raceI.user.pos]->files == 1 )) {
					d_log("Executing mp3 script\n");
					sprintf(target, mp3_script " \"%s\" %s", raceI.file.name, convert(&raceI,userI,groupI,mp3_script_cookies));
					execute(target);
			    }

			    if ( ! matchpath(audio_nocheck_dirs, locations.path) ) {
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
								writelog(error_msg, general_badgenre_type);
								raceI.misc.write_log = write_log;
							} else
							    d_log("warn on - have already logged to logfile\n");
						} else
							exit_value = 2;
						break;
					}
#elif ( audio_allowed_genre_check == TRUE )
				if ( ! strcomp(allowed_genres, raceI.audio.id3_genre)) {
					d_log("File is not in allowed genre\n");
					sprintf(raceI.misc.error_msg, BANNED_GENRE, raceI.audio.id3_genre);
					if ( audio_genre_warn == TRUE ) {
						if ( userI[raceI.user.pos]->files == 1 ) {
							d_log("warn on - logging to logfile\n");
							write_log = raceI.misc.write_log;
							raceI.misc.write_log = 1;
							error_msg = convert(&raceI,userI,groupI,audio_genre_warn_msg);
							writelog(error_msg, general_badgenre_type);
							raceI.misc.write_log = write_log;
						} else
						d_log("warn on - have already logged to logfile\n");
						} else
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
							writelog(error_msg, general_badyear_type);
							raceI.misc.write_log = write_log;
						} else
							d_log("warn on - have already logged to logfile\n");
					} else
						exit_value = 2;
					break;
				}
#endif
#if ( audio_cbr_check == TRUE )
				if (raceI.audio.is_vbr == 0) {
					if ( ! strcomp(allowed_constant_bitrates, raceI.audio.bitrate)) {
						d_log("File is encoded using banned bitrate\n");
						sprintf(raceI.misc.error_msg, BANNED_BITRATE, raceI.audio.bitrate);
						if ( audio_cbr_warn == TRUE ) {
							if ( userI[raceI.user.pos]->files == 1 ) {
								d_log("warn on - logging to logfile\n");
								write_log = raceI.misc.write_log;
								raceI.misc.write_log = 1;
								error_msg = convert(&raceI,userI,groupI,audio_cbr_warn_msg);
								writelog(error_msg, general_badbitrate_type);
								raceI.misc.write_log = write_log;
							} else
								d_log("warn on - have already logged to logfile\n");
						} else
							exit_value = 2;
						break;
					}
				}
#endif
				} else
					d_log("user is in a no audio check dir - skipping checks.\n");
#if ( exclude_non_sfv_dirs == TRUE )
	}
#endif
				printf("%s", convert(&raceI, userI, groupI, realtime_mp3_info));
				race_msg = audio_race;
				update_msg = audio_update;
				halfway_msg = CHOOSE(raceI.total.users, audio_halfway, audio_norace_halfway);
				newleader_msg = audio_newleader;
				break;
		    case RTYPE_VIDEO:
				d_log("Trying to read video header\n");
				if (!memcmp(fileext, "avi", 3))
				    avi_video(raceI.file.name, &raceI.video);
				else
				    mpeg_video(raceI.file.name, &raceI.video);
				race_msg = video_race;
				update_msg = video_update;
				halfway_msg = CHOOSE(raceI.total.users, video_halfway, video_norace_halfway);
				newleader_msg = video_newleader;
				break;
		}

		if ( exit_value == EXIT_SUCCESS ) {
		    d_log("Removing missing indicator\n");
		    sprintf(target, "%s-missing", raceI.file.name);
#if (sfv_cleanup_lowercase == TRUE)
		    strtolower(target);
#endif
		    unlink(target);
		}

		break;
		/* END OF SFV BASED CRC-32 CHECK */

	    case 4: /* ACCEPTED FILE */
		d_log("File type: NO CHECK\n");
		no_check = TRUE;
		break;
		/* END OF ACCEPTED FILE CHECK */

	    case 255: /* UNKNOWN - WE DELETE THESE, SINCE IT WAS ALSO IGNORED */
		d_log("File type: UNKNOWN [ignored in sfv]\n");

		sprintf(raceI.misc.error_msg, UNKNOWN_FILE, fileext);
		exit_value = 2;
		break;
		/* END OF UNKNOWN CHECK */
	}
    }

    if ( no_check == TRUE ) { /* File was not checked */
	printf(zipscript_any_ok);
	printf("%s", convert(&raceI, userI, groupI, zipscript_footer_skip));
    } else if ( exit_value == EXIT_SUCCESS ) { /* File was checked */

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
		    writelog(convert(&raceI, userI, groupI, race_msg), general_race_type);
		}
		/* Modification by <daxxar@mental.mine.nu>
		 *  Only announce new leader if he leads with
		 *  newleader_files_ahead files :-)
		 */
		if ( raceI.total.files >= min_newleader_files && strcmp(raceI.misc.old_leader, userI[userI[0]->pos]->name) && newleader_msg != NULL && userI[userI[0]->pos]->files >= (userI[userI[1]->pos]->files + newleader_files_ahead) ) {
		    d_log("Writing NEWLEADER to %s\n", log);
		    writelog(convert(&raceI, userI, groupI, newleader_msg), general_newleader_type);
		}
	    } else {

		if ( userI[raceI.user.pos]->files == 1 && raceI.total.files >= min_update_files && update_msg != NULL ) {
			d_log("Writing UPDATE to %s\n", log);
			update_type = general_announce_update_type;
			switch( raceI.misc.release_type ) {
				case RTYPE_RAR: update_type = rar_announce_update_type;   break; /* rar */
				case RTYPE_OTHER: update_type = other_announce_update_type; break; /* other */
				case RTYPE_AUDIO: 
					if (raceI.audio.is_vbr == 0 ) {
						update_type = audio_announce_cbr_update_type;
					} else {
						update_type = audio_announce_vbr_update_type;
					};
					break; /* audio */
				case RTYPE_VIDEO: update_type = video_announce_update_type; break; /* video */
				case RTYPE_NULL: update_type = zip_announce_update_type; break; /* zip */
			}
			writelog(convert(&raceI, userI, groupI, update_msg), update_type );
		}
	    }
	}

	if ( raceI.total.files_missing > 0 ) {

	    /* Release is incomplete */

	    if ( raceI.total.files_missing == raceI.total.files >> 1 && raceI.total.files >= min_halfway_files && halfway_msg != NULL ) {
		d_log("Writing HALFWAY to %s\n", log);
		writelog(convert(&raceI, userI, groupI, halfway_msg), general_halfway_type);
	    }

	    /* It is _very_ unlikely that halfway would be announced on complete release ;) */

	    d_log("Caching progress bar\n");
	    buffer_progress_bar(&raceI);

	    d_log("Creating incomplete indicator %s\n", locations.incomplete);
	    create_incomplete();

	    d_log("Creating/moving progress bar\n");
	    move_progress_bar(0, &raceI);

	    printf("%s", convert(&raceI, userI, groupI, zipscript_footer_ok));

	} else if (( raceI.total.files_missing == 0 ) && ( raceI.total.files > 0 )) {

	    /* Release is complete */

	    d_log("Caching progress bar\n");
	    buffer_progress_bar(&raceI);
	    printf("%s", convert(&raceI, userI, groupI, zipscript_footer_ok));

	    d_log("Setting complete pointers\n");
	    switch ( raceI.misc.release_type ) {
		case RTYPE_NULL:
		    complete_bar = zip_completebar;
		    complete_msg = CHOOSE(raceI.total.users, zip_complete, zip_norace_complete);
		    complete_type = CHOOSE(raceI.total.users, zip_complete_type, zip_norace_complete_type);
		    complete_announce = CHOOSE(raceI.total.users, zip_announce_race_complete_type, zip_announce_norace_complete_type);
		    break;
		case RTYPE_RAR:
		    complete_bar = rar_completebar;
		    complete_msg = CHOOSE(raceI.total.users, rar_complete, rar_norace_complete);
		    complete_type = CHOOSE(raceI.total.users, rar_complete_type, rar_norace_complete_type);
		    complete_announce = CHOOSE(raceI.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
		    break;
		case RTYPE_OTHER:
		    complete_bar = other_completebar;
		    complete_msg = CHOOSE(raceI.total.users, other_complete, other_norace_complete);
		    complete_type = CHOOSE(raceI.total.users, other_complete_type, other_norace_complete_type);
		    complete_announce = CHOOSE(raceI.total.users, other_announce_race_complete_type, other_announce_norace_complete_type);
		    break;
		case RTYPE_AUDIO:
		    complete_bar = audio_completebar;
		    complete_msg = CHOOSE(raceI.total.users, audio_complete, audio_norace_complete);
		    complete_type = CHOOSE(raceI.total.users, audio_complete_type, audio_norace_complete_type);
		    complete_announce = CHOOSE(raceI.total.users, audio_announce_race_complete_type, audio_announce_norace_complete_type);


		    d_log("Symlinking audio\n");
		    if ( ! strncasecmp(locations.link_target, "VA", 2) && (locations.link_target[2] == '-' || locations.link_target[2] =='_'))
				memcpy(raceI.audio.id3_artist, "VA", 3);

		    if ( raceI.misc.write_log == TRUE ) {
#if ( audio_genre_sort == TRUE )
			d_log("Sorting mp3 by genre\n");
			createlink(audio_genre_path, raceI.audio.id3_genre, locations.link_source, locations.link_target);
#endif
#if ( audio_artist_sort == TRUE )
			d_log("Sorting mp3 by artist\n");
			if ( *raceI.audio.id3_artist ) {
			    if ( memcmp(raceI.audio.id3_artist, "VA", 3) ) {
				temp_p = malloc(2);
				sprintf(temp_p, "%c", toupper(*raceI.audio.id3_artist));
				createlink(audio_artist_path, temp_p, locations.link_source, locations.link_target);
				free(temp_p);
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
		    if (findfileext(".sfv")) {
			d_log("Creating m3u\n");
			cnt = sprintf(target, findfileext(".sfv"));
			strcpy(target + cnt - 3, "m3u");
			create_indexfile_file(&locations, &raceI, target);
		    } else d_log("Cannot create m3u, sfv is missing\n");
#endif
		    break;
		case RTYPE_VIDEO:
		    complete_bar = video_completebar;
		    complete_msg = CHOOSE(raceI.total.users, video_complete, video_norace_complete);
		    complete_type = CHOOSE(raceI.total.users, video_complete_type, video_norace_complete_type);
		    complete_announce = CHOOSE(raceI.total.users, video_announce_race_complete_type, video_announce_norace_complete_type);
		    break;
	    }

	    d_log("Removing old complete bar, if any\n");
	    removecomplete();

	    complete(&locations, &raceI, userI, groupI, complete_type);

	    if ( complete_msg != NULL ) {
		d_log("Writing COMPLETE and STATS to %s\n", log);
		writelog(convert(&raceI, userI, groupI, complete_msg), complete_announce);
		writetop(&raceI, userI, groupI, complete_type);
	    }

	    d_log("Creating complete bar\n");
	    createstatusbar(convert(&raceI, userI, groupI, complete_bar));

	    if ((show_missing_nfo) && (locations.nfo_incomplete)) {
		if  (findfileext(".nfo")) {
			d_log("Removing missing-nfo indicator (if any)\n");
			remove_nfo_indicator(locations.path);
	    	} else {
		    	d_log("Creating missing-nfo indicator %s.\n", locations.nfo_incomplete);
		    	create_incomplete_nfo();
		}
	    }

#if ( enable_complete_script == TRUE )
	    d_log("Executing complete script\n");
	    sprintf(target, complete_script " \"%s\"", raceI.file.name);
	    execute(target);
#endif
	} else {

	    /* Release is at unknown state */

	    raceI.total.files = -raceI.total.files_missing;
	    raceI.total.files_missing = 0;
	    printf("%s", convert(&raceI, userI, groupI, zipscript_footer_unknown));
	}
    } else {
	/* File is marked to be deleted */

	d_log("Logging file as bad\n");
	writerace_file(&locations, &raceI, 0, F_BAD);
	printf("%s", convert(&raceI, userI, groupI, zipscript_footer_error));
    }
#if ( enable_accept_script == TRUE )
    if ( exit_value == EXIT_SUCCESS ) {
	d_log("Executing accept script\n");
	sprintf(target, accept_script " \"%s\"", raceI.file.name);
	execute(target);
    }
#endif

    d_log("Releasing memory\n");
    free(locations.link_source);
    free(raceI.misc.release_name);
    m_free(target);
    free(fileext);
    m_free(locations.race);
    m_free(locations.sfv);
    m_free(locations.leader);
    m_free(locations.link_target);
    m_free(locations.incomplete);
    m_free(locations.nfo_incomplete);

#if ( benchmark_mode == TRUE )
    gettimeofday(&bstop, (struct timezone *)0);
    printf("Checks completed in %0.6f seconds\n", ((bstop.tv_sec - bstart.tv_sec) + (bstop.tv_usec - bstart.tv_usec) / 1000000.));
#endif

    d_log("Exit\n");
    return exit_value;
}
