/*
 MySQL support file for Zipscript-C
*/

#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "include/objects.h"
#include "zsconfig.h"

extern char*	findfile(char *filename);
extern short	israr (char *fileext);
extern short	fileexists(char *filename);
extern short	strcomp(char *instr, char *searchstr);
extern void	create_missing(char *f, short l);

MYSQL	mysql;
char	query[ PATH_MAX + 94 ];
char	sfvcrc[ 9 ];

#define NEW_INDEXTABLE	"CREATE TABLE IF NOT EXISTS Release_Index (IDX BIGINT NOT NULL AUTO_INCREMENT, PRIMARY KEY (IDX), PATH BLOB, INDEX PATH_INDEX(PATH(60)), R_TYPE INT)"
#define NEW_RACETABLE   "CREATE TABLE IF NOT EXISTS %s (F_NAME VARCHAR(255), U_NAME VARCHAR(24), U_GROUP VARCHAR(24), F_SIZE BIGINT, U_SPEED BIGINT, S_SEC BIGINT, S_USEC BIGINT, CRC CHAR(8), STATUS INT)"
#define NEW_SFVTABLE    "CREATE TABLE IF NOT EXISTS %s (unique (F_NAME), F_NAME VARCHAR(255), CRC CHAR(8))"
#define NEW_LEADERTABLE "CREATE TABLE IF NOT EXISTS %s (unique (N), N INT, U_NAME VARCHAR(24))"
#define SET_RACEVARS    "REPLACE INTO %s (F_NAME, U_NAME, U_GROUP, F_SIZE, U_SPEED, S_SEC, S_USEC, CRC, STATUS) VALUES (\"%s\", \"%s\", \"%s\", %u, %i, %i, %i, \"%s\", %i)"
#define SET_SFVVARS     "REPLACE INTO %s (F_NAME, CRC) VALUES(\"%s\", \"%.8s\")"
#define SET_LEADERVARS  "REPLACE INTO %s (N, U_NAME) VALUES(1, \"%s\")"
#define SET_INDEXVARS	"INSERT INTO Release_Index (PATH, R_TYPE) VALUES(\"%s\", 0)"
#define SET_R_TYPE	"UPDATE Release_Index SET R_TYPE = %i WHERE IDX = %s"
#define DROP            "DROP TABLE %s"


long get_index_mysql(struct LOCATIONS *locations) {
 MYSQL_RES	*result;
 MYSQL_ROW	row;
 long		index;

 mysql_query(&mysql, NEW_INDEXTABLE);
START:
 sprintf(query, "SELECT IDX FROM Release_Index WHERE PATH = \"%s\"", locations->path);
 mysql_query(&mysql, query);
 result = mysql_store_result(&mysql);
 if ( ! (row = mysql_fetch_row(result))) {
	 mysql_free_result(result);
	 sprintf(query, SET_INDEXVARS, locations->path);
	 mysql_query(&mysql, query);
	 goto START;
	}
 index = atol(row[0]);
 mysql_free_result(result);
 return index;
}



void readrace_mysql(struct LOCATIONS *locations, struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI) { 
 unsigned long  fsize; 
 long	uspeed,	startsec, startusec; 
 int	status;

 MYSQL_RES	*result; 
 MYSQL_ROW	row;

 sprintf(query, "SELECT * FROM %s ORDER BY S_SEC, S_USEC", locations->race);
 mysql_query(&mysql, query); 
 result = mysql_store_result(&mysql);

 while ((row = mysql_fetch_row(result))) {
	 fsize = strtoul(row[3], (char **)NULL, 10);
	 uspeed = atol( row[4] );
	 startsec = atol(row[5]);
	 startusec = atol(row[6]);
	 status = atoi(row[8]);
	 switch ( status ) {
		  case F_NOTCHECKED:
		  case F_CHECKED:
			 updatestats(raceI, userI, groupI, row[ 1 ], row[ 2 ], fsize, uspeed, startsec, startusec);
			break;
		  case F_BAD:
			 raceI->total_bad_files++;
			 raceI->total_bad_size += fsize;
			break;
		}
	}
 mysql_free_result(result);
}



void connect_mysql() {
 mysql_init(&mysql);
 mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "ZipScript-C");
 if ( mysql_real_connect(&mysql, sql_host, sql_user, sql_pass, sql_db, 0, NULL, 0) == 0 ) {
	 fprintf(stdout, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
	 exit(2);
	}
}


char* readsfv_mysql(struct LOCATIONS *locations, struct VARS *raceI, int getfcount) {
 MYSQL_RES	*result;
 MYSQL_ROW	row;

 sprintf(query, "SELECT * FROM %s", locations->sfv);

 mysql_query(&mysql, query);
 result = mysql_store_result(&mysql);
 strcpy( sfvcrc, "." );
 while ((row = mysql_fetch_row(result))) {
	 raceI->total_files++;
	 if ( ! strcasecmp(locations->filename, row[0]) ) strncpy(sfvcrc, row[1], 8);
	 if ( getfcount ) if ( findfile(row[0])) raceI->missing_files--;
	}
 raceI->missing_files += raceI->total_files;
 mysql_free_result(result);

 sprintf(query, "SELECT R_TYPE FROM Release_Index WHERE IDX = %s", locations->sfv + 2);
 mysql_query(&mysql, query);
 result = mysql_store_result(&mysql);
 row = mysql_fetch_row(result);
 raceI->release_type = atoi(row[0]);
 mysql_free_result(result);

 return sfvcrc;
}


void delete_sfv_mysql(struct LOCATIONS *locations) {
 MYSQL_RES *result;
 MYSQL_ROW row;
 char      tmp[FILE_MAX];

 sprintf(query, "SELECT * FROM %s", locations->sfv);
 mysql_query(&mysql, query);
 result = mysql_store_result(&mysql);

 while ((row = mysql_fetch_row(result))) {
	 sprintf(tmp, "%s-missing", row[0]);
	 unlink(tmp);
	}
 mysql_free_result(result);
}


short table_exists(struct LOCATIONS *locations, char *table) {
 sprintf(query, "SELECT * FROM %s", table);
 if ( mysql_query(&mysql, query) == 0 ) {
	 mysql_free_result(mysql_store_result(&mysql));
	 return 1;
	}
 return 0;
}


void disconnect_mysql() {
 mysql_close(&mysql);
}



void copysfv_mysql(struct LOCATIONS *locations, char *source, char *target, long buf_bytes) {
 int    fd,
	line_start = 0,
	index_start,
	ext_start,
	n, m;
 char	*buf,
	*fname,
	*crc;
 int	rars = 0,
	mp3s = 0,
	others = 0;
    
 sprintf(query, NEW_SFVTABLE, target);
 mysql_query(&mysql, query);

 fd  = open( source, O_SYNC );
 buf = m_alloc( buf_bytes + 2 );
 read(fd, buf, buf_bytes);
 close( fd );

 if ( buf[buf_bytes - 1] != '\n' ) {
	 buf[buf_bytes] = '\n';
	 buf_bytes++; 
	}
 
 for ( n = 0 ; n < buf_bytes ; n++ ) if ( buf[n] == '\n' ) {   
	 index_start = n - line_start;
	 if ( buf[line_start] != ';' ) {
		 while ( buf[index_start + line_start] != ' ' && index_start-- );
		 if ( index_start > 0 ) {
			 buf[index_start + line_start] = 0;
			 crc = fname = buf + line_start;
			 ext_start = index_start;
			 for ( m = 0 ; m < index_start ; m++ ) fname[m] = tolower(fname[m]);
			 while ( fname[ext_start] != '.' && ext_start-- );
			 if ( ext_start <= 0 ) ext_start = index_start; else ext_start++;
 
			 index_start++;
			 crc += index_start;
			 if ( ! strcomp(ignored_types, fname + ext_start) ) {
				 if ( ! strcmp(fname + ext_start, "mp3") ) {
					 mp3s++;
					} else if ( israr(fname + ext_start) ) {
					 rars++;
					} else {
					 others++;
					}
#if ( enabled_create_missing_files )
				 if ( ! findfile(fname) ) {
					 create_missing( fname, index_start - 1 );
					}
#endif
				 sprintf(query, SET_SFVVARS, target, fname, crc);
				 mysql_query(&mysql, query);
				}
			}
		}
	 line_start = n + 1;
	}

 if ( mp3s > rars ) {
	 n = mp3s > others ? 3 : 2;
	} else {
	 n = rars > others ? 1 : 2;
	}

 m_free( buf );
 sprintf(query, SET_R_TYPE, n, target + 2);
 mysql_query(&mysql, query);
}



void read_write_leader_mysql(struct LOCATIONS *locations, struct VARS *raceI, struct USERINFO *userI) {
 MYSQL_RES *result;
 MYSQL_ROW row;

 sprintf(query, NEW_LEADERTABLE, locations->leader);
 mysql_query(&mysql, query);

 sprintf(query, "SELECT * FROM %s", locations->leader);
 mysql_query(&mysql, query);
 result = mysql_store_result(&mysql);

 if ((row = mysql_fetch_row(result))) sprintf(raceI->old_leader, row[0]);
 
 sprintf(query, SET_LEADERVARS, locations->leader, userI->name);
 mysql_query(&mysql, query);
 mysql_free_result(result);
}



void testfiles_mysql(struct LOCATIONS *locations, struct VARS *raceI) {
 char		*filename,
		*crc, *Tcrc;
 MYSQL_RES	*result,
		*Tresult;
 MYSQL_ROW	row, Trow;
  
 sprintf(query, "SELECT * FROM %s WHERE STATUS=%i", locations->race, F_NOTCHECKED);
 mysql_query(&mysql, query);
 result = mysql_store_result(&mysql);
 while ((row = mysql_fetch_row(result))) {
	 filename = row[0];
	 crc = row[7];

	 sprintf(query, "SELECT * FROM %s WHERE F_NAME=\"%s\"", locations->sfv, filename);
	 mysql_query( &mysql, query );

	 Tresult = mysql_store_result( &mysql );
	 if ((Trow = mysql_fetch_row( Tresult ))) {
		 Tcrc = row[1];
		} else {
		 Tcrc = ".";
		}

	 if ( ! strncasecmp(Tcrc, crc, 8) ) {
		 sprintf(query, "UPDATE %s SET STATUS=%i WHERE F_NAME=\"%s\" && STATUS=%i", locations->race, F_CHECKED, filename, F_NOTCHECKED);
		 mysql_query(&mysql, query);
		} else {
#if ( enabled_create_missing_files )
		 if ( Tcrc[0] != '.' ) create_missing( row[0], strlen( row[0] ) );
#endif
		 unlink(filename); 
		 sprintf(query, "UPDATE %s SET STATUS=%i WHERE F_NAME=\"%s\" && STATUS=%i", locations->race, F_BAD, filename, F_NOTCHECKED);
		 mysql_query(&mysql, query);
		}
	 mysql_free_result( Tresult );
	}
 mysql_free_result(result);
}


void remove_table_mysql(char *table) {
 sprintf(query, DROP, table);
 mysql_query(&mysql, query);
}


short clear_file_mysql(struct LOCATIONS *locations) {
 sprintf(query, "DELETE FROM %s WHERE F_NAME=\"%s\" AND (STATUS=%i OR STATUS=%i)", locations->race, locations->filename, F_CHECKED, F_NOTCHECKED);
 mysql_query(&mysql, query);
 if ( mysql_affected_rows( &mysql ) > 0 ) {
	 return 1;
	}
 return 0;
}



void create_indexfile_mysql(struct LOCATIONS *locations, char *filename) {
 MYSQL_RES	*result;
 MYSQL_ROW	row;
 FILE		*index;

 index = fopen(filename, "w");
 sprintf(query, "SELECT F_NAME FROM %s WHERE STATUS=%i ORDER BY F_NAME", locations->race, F_CHECKED);
 mysql_query(&mysql, query);
 result = mysql_store_result(&mysql); 

 while ((row = mysql_fetch_row(result))) if ( fileexists(row[0]) ) fprintf(index, "%s\n", row[0]);
 mysql_free_result(result);
 fclose(index);
}



void writerace_mysql(struct LOCATIONS *locations, struct VARS *raceI, char *crc, int status) {
 clear_file_mysql(locations);

 sprintf(query, NEW_RACETABLE, locations->race);
 mysql_query(&mysql, query);

 sprintf(query, SET_RACEVARS, locations->race, locations->filename, raceI->user, raceI->user_group, raceI->file_size, raceI->speed, raceI->transfer_start.tv_sec, raceI->transfer_start.tv_usec, crc, status); 
 mysql_query(&mysql, query);
}
