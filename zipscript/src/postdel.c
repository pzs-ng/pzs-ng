#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "zsfunctions.h"
#include "race-file.h"
#include "objects.h"
#include "macros.h"
#include "convert.h"
#include "dizreader.h"

#include "../conf/zsconfig.h"
#include "../../config.h"

struct USERINFO  **userI;
struct GROUPINFO **groupI;
struct VARS      raceI;
struct LOCATIONS locations;

void writelog(char *msg, char *status) {
 FILE   *glfile;
 char   *date;
 char   *line, *newline;
 time_t timenow;

 if ( raceI.misc.write_log == TRUE ) {
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


/* GET NAME OF MULTICD RELEASE (CDx/DISCx) (SYMLINK LOCATION + INCOMPLETE FILENAME)*/
void getrelname(char *directory) {
 int    cnt,
        l,
        n = 0,
        k = 2;
 char   *path[2];
  
 for ( cnt = strlen(directory) ; k && cnt ; cnt-- ) {
	if ( directory[cnt] == '/' ) {
		k--;
		path[k] = malloc(n + 1);
		strncpy(path[k], directory + cnt + 1, n);
		path[k][n] = 0;
   		n = 0;
		} else {
		n++;
		}
	}

 l = strlen(path[1]);

 if (( ! strncasecmp(path[1], "CD"  , 2) && l <= 4 ) ||
     ( ! strncasecmp(path[1], "DISC", 4) && l <= 6 ) ||
     ( ! strncasecmp(path[1], "DiSC", 4) && l <= 6 ) ||
     ( ! strncasecmp(path[1], "DVD" , 3) && l <= 5 )) {  
	n = strlen(path[0]);
	raceI.misc.release_name = malloc( n + 18 );
	sprintf(raceI.misc.release_name, "%s/\\002%s\\002", path[0], path[1]);
 	locations.incomplete = c_incomplete(incomplete_cd_indicator, path);
	} else {
	raceI.misc.release_name = malloc( l + 10 );
	sprintf(raceI.misc.release_name, "\\002%s\\002", path[1]);
	locations.incomplete = c_incomplete(incomplete_indicator, path);
	}
 free(path[1]);
 free(path[0]);
}



unsigned char get_filetype(char *ext) {

 if ( ! memcmp(ext, "sfv", 4)) return 1;
 if ( ! clear_file_file(&locations, raceI.file.name)) return 4;
 if ( ! memcmp(ext, "zip", 4)) return 0;
 if ( ! memcmp(ext, "nfo", 4)) return 2;
 if ( ! strcomp(ignored_types, ext)) return 3;

 return 255;
}



int main( int argc, char **argv ) {
 char		*fileext, *name_p, *temp_p;
 char		*target;
 int		n;
 unsigned char	empty_dir = 0;
 unsigned char	incomplete = 0;

 if (argc == 1) {
   d_log("no param\n");
   return 0;
 }

#if ( program_uid > 0 )
 d_log("Trying to change effective gid\n");
 setegid(program_gid);
 d_log("Trying to change effective uid\n");
 seteuid(program_uid);
#endif

 d_log("Reading directory structure\n");
 rescandir();

 if ( fileexists(argv[1] + 5) ) {
	d_log("File still exists\n");
	return 0;
	}

 umask(0666 & 000);

 d_log("Clearing arrays\n");
 bzero(&raceI.total, sizeof(struct race_total));
 raceI.misc.slowest_user[0] = 30000;
 raceI.misc.fastest_user[0] = 0;

 d_log("Reading user name from env\n");
 strncpy(raceI.user.name, getenv("USER"), 25);
 d_log("Reading group name from env\n");
 strncpy(raceI.user.group, getenv("GROUP"), 25);

 if ( ! *raceI.user.group ) {
	memcpy(raceI.user.group, "NoGroup", 8);
	}

 d_log("Allocating memory for variables\n");
 userI	= malloc(sizeof(struct USERINFO *) * 30);
 groupI	= malloc(sizeof(struct GROUPINFO *) * 30);

 locations.path   = malloc(PATH_MAX);
 getcwd(locations.path, PATH_MAX);
 locations.race = malloc(n = strlen(locations.path) + 9 + sizeof(storage)); 
 locations.sfv = malloc(n); 
 locations.leader = malloc(n);
 target = malloc(4096);

 d_log("Copying data locations into memory\n");
 raceI.file.name = argv[1] + 5;
 sprintf(locations.sfv, storage "/%s/sfvdata", locations.path);
 sprintf(locations.leader, storage "/%s/leader", locations.path);
 sprintf(locations.race, storage "/%s/racedata", locations.path);

 d_log("Caching release name\n");
 getrelname(locations.path);
 
 d_log("Parsing file extension from filename...\n");
 
 for ( temp_p = name_p = raceI.file.name; *name_p != 0 ; name_p++ ) {
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

 switch ( get_filetype(fileext) ) {
	case 0:
		d_log("File type is: ZIP\n");
		if ((raceI.misc.write_log = matchpath(zip_dirs, locations.path))) {
			raceI.misc.write_log = 1 - matchpath(group_dirs, locations.path);
			} else if (matchpath(sfv_dirs, locations.path)) {
			d_log("Directory matched with sfv_dirs\n");
			break;
			}
		if ( ! fileexists("file_id.diz") ) {
			temp_p = findfileext(".zip");
			if ( temp_p != NULL ) {
				d_log("file_id.diz does not exist, trying to extract it from %s\n", temp_p);
				sprintf(target, "/bin/unzip -qqjnCL %s file_id.diz", temp_p);
				execute(target);
				chmod("file_id.diz",0666);
				}
			}

		d_log("Reading diskcount from diz\n");
		raceI.total.files = read_diz("file_id.diz");
		if ( raceI.total.files == 0 ) {
			d_log("Could not get diskcount from diz\n");
			raceI.total.files = 1;
			}
		raceI.total.files_missing = raceI.total.files;

		d_log("Reading race data from file to memory\n");
		readrace_file(&locations, &raceI, userI, groupI);

		d_log("Caching progress bar\n");
		buffer_progress_bar(&raceI);

		d_log("Removing old complete bar, if any\n");
		removecomplete();
		if ( raceI.total.files_missing < 0 ) {
			raceI.total.files -= raceI.total.files_missing;
			raceI.total.files_missing = 0;
			}
		if ( ! raceI.total.files_missing ) {
			d_log("Creating complete bar\n");
			createstatusbar(convert(&raceI, userI, groupI, zip_completebar) ); 
			} else if ( raceI.total.files_missing < raceI.total.files ) {
			if ( raceI.total.files_missing == 1 ) {
				d_log("Writing INCOMPLETE to %s\n", log);
				writelog(convert(&raceI, userI, groupI, incompletemsg), "INCOMPLETE"); 
				}
			incomplete = 1;
			} else {
			empty_dir = 1;
			}
		break;
	case 1:
		d_log("Reading file count from SFV\n");
		readsfv_file(&locations, &raceI, 0);

		if ( fileexists(locations.race) ) {
			d_log("Reading race data from file to memory\n");
			readrace_file(&locations, &raceI, userI, groupI);
			}

		d_log("Caching progress bar\n");
		buffer_progress_bar(&raceI);
		if (raceI.total.files_missing == raceI.total.files) {
			empty_dir = 1;
			}
		break;
	case 2:
		break;
	case 3:
		d_log("Removing old complete bar, if any\n");
		removecomplete();
		raceI.misc.write_log = matchpath(sfv_dirs, locations.path) > 0 ? 1 - matchpath(group_dirs, locations.path) : 0;

		if ( fileexists(locations.race) ) {
			d_log("Reading race data from file to memory\n");
			readrace_file(&locations, &raceI, userI, groupI);
			}

		if ( fileexists(locations.sfv) ) {
#if ( create_missing_files == TRUE )
			strtolower(raceI.file.name);
			create_missing(raceI.file.name, name_p - raceI.file.name - 1);
#endif
			d_log("Reading file count from SFV\n");
			readsfv_file(&locations, &raceI, 0);

			d_log("Caching progress bar\n");
			buffer_progress_bar(&raceI);
			}

		if ( raceI.total.files_missing < raceI.total.files ) {
			if ( raceI.total.files_missing == 1 ) {
				d_log("Writing INCOMPLETE to %s\n", log);
				writelog(convert(&raceI, userI, groupI, incompletemsg), "INCOMPLETE");
				}
			incomplete = 1;
			} else {
			d_log("Removing old race data\n");
			unlink(locations.race);
			if ( findfileext(".sfv") == NULL ) {
				empty_dir = 1;
				} else {
				incomplete = 1;
				}
			}
		break;
	case 4:
		break;
	case 255:
		break;
	}


 if ( empty_dir == 1 ) {
	d_log("Removing all files and directories created by zipscript\n");
	removecomplete();
	if ( fileexists(locations.sfv)) {
		delete_sfv_file(&locations);
		}
	unlink(locations.incomplete);
	unlink("file_id.diz"); 
	unlink(locations.sfv);
	unlink(locations.race); 
	unlink(locations.leader); 
	move_progress_bar(1, &raceI);
	}

 if ( incomplete == 1 && raceI.total.files > 0 ) {
	d_log("Creating incomplete indicator\n");
	create_incomplete();
	d_log("Moving progress bar\n");
	move_progress_bar(0, &raceI);
	}

 d_log("Relasing memory\n");
 free(fileext);
 free(target);
 free(raceI.misc.release_name);
 free(locations.path);
 free(locations.race);
 free(locations.sfv);
 free(locations.leader);
 free(raceI.user.name);

 d_log("Exit\n");
 return 0;
}
