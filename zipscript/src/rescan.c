#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"

#include "zsfunctions.h"
#include "race-file.h"
#include "objects.h"
#include "macros.h"
#include "multimedia.h"
#include "convert.h"
#include "dizreader.h"
#include "stats.h"
#include "complete.h"
#include "crc.h"

#include "../conf/zsconfig.h"

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
	glfile = fopen(log, "a+");

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

void getrelname(char *directory) {
    int    cnt,
	   l[2],
	   n = 0,
	   k = 2;
    char   *path[2];

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
	    ( ! strncasecmp(path[1], "DISK", 4) && l[1] <= 6 ) ||
	    ( ! strncasecmp(path[1], "DVD" , 3) && l[1] <= 5 )) {
	raceI.misc.release_name = malloc(l[0] + 18);
	locations.link_source = malloc(n = (locations.length_path - l[1]));
	sprintf(raceI.misc.release_name, "%s/%s", path[0], path[1]);
	sprintf(locations.link_source, "%.*s", n - 1, locations.path);
	locations.link_target = path[0];
	locations.incomplete = c_incomplete(incomplete_cd_indicator, path);
	if (k < 2) free(path[1]);
    } else {
	raceI.misc.release_name = malloc(l[1] + 10);
	locations.link_source   = malloc(locations.length_path + 1);
	strcpy(locations.link_source, locations.path);
	sprintf(raceI.misc.release_name, "%s", path[1]);
	locations.link_target = path[1];
	locations.incomplete = c_incomplete(incomplete_indicator, path);
	if (k == 0) free(path[0]);
    }
}

int main () {
    int	n, m, l,
	complete_type = 0;
    char	*ext,
		exec[ 4096 ],
		*complete_bar = 0;
    unsigned int crc;
    struct stat fileinfo;

    uid_t	f_uid;
    gid_t	f_gid;

#if ( program_uid > 0 )
    setegid(program_gid);
    seteuid(program_uid);
#endif

    umask(0666 & 000);

    userI    = malloc( sizeof( struct USERINFO * ) * 30 );
    groupI   = malloc( sizeof( struct GROUPINFO * ) * 30 );

    raceI.misc.slowest_user[0] = 30000;

    bzero(&raceI.total, sizeof(struct race_total));
    raceI.misc.fastest_user[0] =
	raceI.misc.release_type = 0;

    locations.path = malloc( PATH_MAX );
    getcwd( locations.path, PATH_MAX );

    n = locations.length_path = strlen( locations.path );
    n += locations.length_zipdatadir = sizeof(storage) - 1;

    locations.race   = malloc( n += + 10 );
    locations.sfv    = malloc( n );
    locations.leader = malloc( n );

    getrelname(locations.path);
    buffer_groups( GROUPFILE );
    buffer_users( PASSWDFILE );

    sprintf(locations.sfv, storage "/%s/sfvdata", locations.path);
    sprintf(locations.leader, storage "/%s/leader", locations.path);
    sprintf(locations.race, storage "/%s/racedata", locations.path);

    rescandir();
    move_progress_bar( 1, &raceI );
    unlink( locations.incomplete );
    removecomplete();
    unlink( locations.race );
    unlink( locations.sfv );
    rescandir(); /* Rescan dir after deleting files.. */
    printf("Rescanning files...\n");

    if ( (raceI.file.name = findfileext(".sfv")) != NULL) {
	maketempdir(&locations);
	stat(raceI.file.name, &fileinfo);
	copysfv_file( raceI.file.name, locations.sfv, fileinfo.st_size );
	n = direntries;
	while ( n-- ) {
	    m = l = strlen(dirlist[n]->d_name);
	    ext   = dirlist[n]->d_name;
	    while ( ext[m] != '.' && m > 0 ) m--;
	    if ( ext[m] != '.' ) m = l; else m++;
	    ext += m;
	    /*	printf("ext: %s\n",ext); */
	    if ( ! strcomp( ignored_types, ext ) && strcmp( dirlist[n]->d_name + l - 8, "-missing" ) ) {
		stat( dirlist[n]->d_name, &fileinfo );
		f_uid = fileinfo.st_uid;
		f_gid = fileinfo.st_gid;

		strcpy(raceI.user.name, get_u_name(f_uid));
		strcpy(raceI.user.group, get_g_name(f_gid));
		raceI.file.name = dirlist[n]->d_name;
		raceI.file.speed = 2004 * 1024;
		raceI.file.size = fileinfo.st_size;
		raceI.total.start_time = 0;

		sprintf(exec, "%s-missing", raceI.file.name);
		strtolower(exec);
		unlink(exec);

		if ( l > 44 ) {
		    printf("\nFile: %s", dirlist[n]->d_name + l - 44 );
		} else {
		    printf("\nFile: %-44s", dirlist[n]->d_name );
		}
		fflush(stdout);
		crc = calc_crc32( dirlist[n]->d_name );
		writerace_file(&locations, &raceI, crc, F_NOTCHECKED);
	    }
	}
	printf("\n\n");
	testfiles_file( &locations, &raceI );
	rescandir(); /* We need to rescan again */
	readsfv_file( &locations, &raceI, 0 );
	readrace_file( &locations, &raceI, userI, groupI );
	sortstats( &raceI, userI, groupI );
	buffer_progress_bar( &raceI );
	if ( raceI.misc.release_type == 3 ) get_mpeg_audio_info(findfileext(".mp3"), &raceI.audio);

	if ( (raceI.total.files_missing == 0) & (raceI.total.files > 0) ) {
	    switch ( raceI.misc.release_type ) {
		case 1:
		    complete_type = rar_complete_type;
		    complete_bar  = rar_completebar;
		    break;
		case 2:
		    complete_type = other_complete_type;
		    complete_bar  = other_completebar;
		    break;
		case 3:
		    complete_type = audio_complete_type;
		    complete_bar  = audio_completebar;
#if ( enabled_create_m3u )
		    n = sprintf(exec, findfileext(".sfv"));
		    strcpy(exec + n - 3, "m3u");
		    create_indexfile_file(&locations, &raceI, exec);
#endif
		    break;
	        case 4:
		  complete_type = video_complete_type;
		  complete_bar  = video_completebar;
		  break;

	    }
	    complete( &locations, &raceI, userI, groupI, complete_type );
	    createstatusbar( convert(&raceI, userI, groupI, complete_bar) );
	} else {
	    create_incomplete();
	    move_progress_bar(0, &raceI);
	}
    } else if ( (raceI.file.name = findfileext(".zip")) != NULL) {
	maketempdir(&locations);
	stat( raceI.file.name, &fileinfo );
	n = direntries;
	crc = 0;
	while ( n-- ) {
	    m = l = strlen(dirlist[n]->d_name);
	    ext   = dirlist[n]->d_name;
	    while ( ext[m] != '.' && m > 0 ) m--;
	    if ( ext[m] != '.' ) m = l; else m++;
	    ext += m;
	    if ( ! strcasecmp( ext, "zip" ) ) {
		stat( dirlist[n]->d_name, &fileinfo );
		f_uid = fileinfo.st_uid;
		f_gid = fileinfo.st_gid;

		strcpy(raceI.user.name, get_u_name(f_uid));
		strcpy(raceI.user.group, get_g_name(f_gid));
		raceI.file.name = dirlist[n]->d_name;
		raceI.file.speed = 2004 * 1024;
		raceI.file.size = fileinfo.st_size;
		raceI.total.start_time = 0;

		if ( ! fileexists("file_id.diz") ) {
		    sprintf(exec, "/bin/unzip -qqjnCL %s file_id.diz > file_id.diz", raceI.file.name);
		    execute(exec);
		    chmod("file_id.diz",0666);
		}

		sprintf(exec, "/bin/unzip -qqt %s &> /dev/null", raceI.file.name);
		if ( system(exec) == 0 ) {
		    writerace_file(&locations, &raceI, crc, F_CHECKED);
		} else {
		    writerace_file(&locations, &raceI, crc, F_BAD);
		    unlink( raceI.file.name );
		}
	    }
	}
	raceI.total.files = read_diz("file_id.diz");
	if ( ! raceI.total.files ) {
	    raceI.total.files = 1;
	    unlink("file_id.diz");
	}
	raceI.total.files_missing = raceI.total.files;
	readrace_file(&locations, &raceI, userI, groupI);
	sortstats(&raceI, userI, groupI);
	if ( raceI.total.files_missing < 0 ) {
	    raceI.total.files -= raceI.total.files_missing;
	    raceI.total.files_missing = 0;
	}
	buffer_progress_bar(&raceI );
	if ( raceI.total.files_missing == 0 ) {
	    complete( &locations, &raceI, userI, groupI, zip_complete_type );
	    createstatusbar( convert(&raceI, userI, groupI, zip_completebar) );
	} else {
	    create_incomplete();
	    move_progress_bar(0, &raceI);
	}
    }
    printf(" Passed : %i\n", raceI.total.files - raceI.total.files_missing );
    printf(" Failed : %i\n", raceI.total.files_bad );
    printf(" Missing: %i\n", raceI.total.files_missing );
    printf("  Total : %i\n", raceI.total.files );

    exit( 0 );
}
