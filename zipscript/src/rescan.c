#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

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
#include "../../config.h"

#define GROUPFILE "/etc/group"
#define PASSWDFILE "/etc/passwd"

struct GROUP {
	char		*name;
	gid_t		id;
};

struct USER {
	char		*name;
	uid_t		id;
};

int	groups = 0,
	users  = 0,
	ERROR_CODE;

static struct USER	**user;
static struct GROUP	**group;
struct USERINFO  **userI;
struct GROUPINFO **groupI;
struct VARS      raceI;
struct LOCATIONS locations;
struct stat      fileinfo;


/* WRITE TO GLFTPD LOG */
void writelog(char *msg, char *status) {
 FILE   *glfile;
 char   *date;
 time_t timenow;

 if ( raceI.misc.write_log ) {
	 timenow = time( NULL );
	date = ctime(&timenow);
	fprintf(glfile = fopen(log, "a+"), "%.24s %s: \"%s\" \"%s\"\n", date, status, locations.path, msg);
	fclose(glfile);
	}
}

void getrelname(char *directory) {
 int    cnt,
        l,
        n = 0,
        k = 2;
 char   *path[2];
 
 for ( cnt = locations.length_path - 1 ; k && cnt ; cnt-- )
  if ( directory[cnt] == '/' ) {
   k--;
   path[k] = malloc(n + 1);
   strncpy(path[k], directory + cnt + 1, n);
   path[k][n] = 0;
   n = 0;
  } else n++;
  
 if (k!=0) exit (2);
 l = strlen(path[1]);
 
 if (( ! strncasecmp(path[1], "CD"  , 2) && l <= 4 ) ||
     ( ! strncasecmp(path[1], "DISC", 4) && l <= 6 ) ||
     ( ! strncasecmp(path[1], "DiSC", 4) && l <= 6 ) ||
     ( ! strncasecmp(path[1], "DVD" , 3) && l <= 5 )) {
  n = strlen(path[0]);
  raceI.misc.release_name = malloc( n + 18 );
  locations.link_source = malloc(k = (locations.length_path - l)); k--;
  sprintf(raceI.misc.release_name, "%s/\\002%s\\002", path[0], path[1]);
  strncpy(locations.link_source, locations.path, k);
  locations.link_source[k] = 0;
  locations.link_target = path[0];
  locations.incomplete = c_incomplete( incomplete_cd_indicator, path );
  free(path[1]);
 } else {
  raceI.misc.release_name    = malloc( l + 10 );
  locations.link_source = malloc( locations.length_path + 1 );
  strcpy(locations.link_source, locations.path);
  sprintf(raceI.misc.release_name, "\\002%s\\002", path[1]);
  locations.link_target = path[1];
  locations.incomplete = c_incomplete( incomplete_indicator, path );
  free(path[0]);
 }
}




/* Buffer groups file */
void buffer_groups(char *groupfile) {
 char   *f_buf,
        *g_name;
 long   f, n, m,
        f_size,
        g_id,
        g_n_size,
        l_start = 0;
 int	GROUPS = 0;
 
 f = open( groupfile, O_NONBLOCK );
 fstat( f, &fileinfo );
 f_size = fileinfo.st_size;
 f_buf  = malloc( f_size );
 read( f, f_buf, f_size );
 
 for ( n = 0 ; n < f_size ; n++ ) if ( f_buf[n] == '\n' ) GROUPS++;
 group = malloc( GROUPS * sizeof( int ) );
   
 for ( n = 0 ; n < f_size ; n++ ) {
  if ( f_buf[n] == '\n' || n == f_size ) {
   f_buf[n] = 0;
   m        = l_start;
   while ( f_buf[m] != ':' && m < n ) m++;
   if ( m != l_start ) {
    f_buf[m] = 0;
    g_name   = f_buf + l_start;
    g_n_size = m - l_start;
    m        = n;
    while ( f_buf[m] != ':' && m > l_start ) m--;
    f_buf[m] = 0;
    while ( f_buf[m] != ':' && m > l_start ) m--;
    if ( m != n ) {
     g_id = atoi( f_buf + m + 1 );
     group[groups] = malloc( sizeof( struct GROUP ) );
     group[groups]->name = malloc( g_n_size + 1 );
     strcpy( group[groups]->name, g_name );
     group[groups]->id = g_id;
     groups++;
    }
   }
   l_start = n + 1;
  }
 }
 
 close( f );
 free( f_buf );
}


/* Buffer users file */
void buffer_users(char *passwdfile) {
 char   *f_buf,
        *u_name;
 long   f, n, m, l,
        f_size,
        u_id,
        u_n_size,
        l_start = 0;
 int	USERS = 0;
 
 f = open( passwdfile, O_NONBLOCK );
 fstat( f, &fileinfo );
 f_size = fileinfo.st_size;
 f_buf  = malloc( f_size );
 read( f, f_buf, f_size );
 
 for ( n = 0 ; n < f_size ; n++ ) if ( f_buf[n] == '\n' ) USERS++;
 user = malloc( USERS * sizeof( int ) );
   
 for ( n = 0 ; n < f_size ; n++ ) {
  if ( f_buf[n] == '\n' || n == f_size ) {
   f_buf[n] = 0;
   m        = l_start;
   while ( f_buf[m] != ':' && m < n ) m++;
   if ( m != l_start ) {
    f_buf[m] = 0;
    u_name   = f_buf + l_start;
    u_n_size = m - l_start;
    m        = n;
    for ( l = 0 ; l < 4 ; l ++ ) { 
     while ( f_buf[m] != ':' && m > l_start ) m--;
     f_buf[m] = 0;
    }
    while ( f_buf[m] != ':' && m > l_start ) m--;
    if ( m != n ) {
     u_id = atoi( f_buf + m + 1 );
     user[users] = malloc( sizeof( struct USER ) );
     user[users]->name = malloc( u_n_size + 1 );
     strcpy( user[users]->name, u_name );
     user[users]->id = u_id;
     users++;
    }
   }
   l_start = n + 1;
  }
 }
 
 close( f );
 free( f_buf );
}



char* get_g_name(int gid) {
 int	n;
 for ( n = 0 ; n < groups ; n++ ) if ( (int)group[n]->id / 100 == (int)gid / 100 ) return group[n]->name;
 return "NoGroup";
}

char* get_u_name(int uid) {
 int	n;
 for ( n = 0 ; n < users ; n++ ) if ( user[n]->id == uid ) return user[n]->name;
 return "Unknown";
}


int main () {
 int	n, m, l,
	complete_type = 0;
 char	*ext,
	exec[ 4096 ],
	*complete_bar = 0;
 unsigned long crc;
 
 uid_t	f_uid;
 gid_t	f_gid;

 #if ( enabled_suid )
  setegid(program_gid);
  seteuid(program_uid);
 #endif

 umask(0666 & 000);

 userI    = malloc( sizeof( int ) * 30 );
 groupI   = malloc( sizeof( int ) * 30 );

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
 raceI.user.name  = malloc( 25 );

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
 rescandir2(); // Rescan dir after deleting files..
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
//	printf("ext: %s\n",ext);
		 if ( ! strcomp( ignored_types, ext ) && strcmp( dirlist[n]->d_name + l - 8, "-missing" ) ) {
			 stat( dirlist[n]->d_name, &fileinfo );
			 f_uid = fileinfo.st_uid;
			 f_gid = fileinfo.st_gid;

			 strcpy(raceI.user.name, get_u_name(f_uid));
			 strcpy(raceI.user.group, get_g_name(f_gid));
			 raceI.file.speed = 2004 * 1024.;
			 raceI.file.name = dirlist[n]->d_name;
			 raceI.transfer_start.tv_sec = 0;
			 raceI.transfer_start.tv_usec = 0;
			 raceI.file.size = fileinfo.st_size;

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
	 rescandir(); // We need to rescan again
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
			 raceI.file.speed = 2004 * 1024.;
			 raceI.transfer_start.tv_sec = 0;
			 raceI.transfer_start.tv_usec = 0;
			 raceI.file.size = fileinfo.st_size;

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
