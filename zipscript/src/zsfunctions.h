     #include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <regex.h>
#ifndef PATH_MAX
# define _LIMITS_H_
# include <sys/syslimits.h>
#endif

#define createzerofile(filename) fclose(fopen(filename, "a+"))

struct dirent	**dirlist;
unsigned	direntries = 0;


#if ( debug_mode == TRUE )

#include <stdarg.h>

void d_log(char *fmt, ...) {
 time_t		timenow;
 FILE		*file;
 va_list	ap;

 va_start(ap, fmt);
 timenow = time(NULL);

 if ((file = fopen(".debug", "a+")) != NULL ) {
	fprintf(file, "%.24s - ", ctime(&timenow));
	vfprintf(file, fmt, ap);
	fclose(file);
	}
}
#else
# define d_log
#endif



void create_missing(char *f, short l) {
 char	*fname;
 
 fname = m_alloc(l + 9);
 memcpy(fname, f, l);
 memcpy(fname + l, "-missing", 9);
 createzerofile(fname);
 m_free(fname);
}


/* BSD does not know O_SYNC, it has O_FSYNC instead */
#ifndef O_SYNC
# define O_SYNC O_FSYNC
#endif

/* Creates status bar file */
#if ( status_bar_type == 0 )
# define createstatusbar(bardata) createzerofile(bardata);
#endif
#if ( status_bar_type == 1 )
# define createstatusbar(bardata) mkdir(bardata, 0777);
#endif
#if ( status_bar_type == 2 )
# define createstatusbar(bardata)
#endif



/*
 * Modified: 01.16.2002
 */
char* findfileext(char *fileext) { 
 int	n, k;
 
 n = direntries;
 while(n--) {
	if ((k = D_NAMLEN(dirlist[n])) < 4 ) continue;
  	if ( strcasecmp(dirlist[n]->d_name + k - 4, fileext) == 0 ) return dirlist[n]->d_name;
	}
 return NULL;
}



/*
 * Modified: 11.12.2003 (daxxar)
 */
int findfileextcount(char *fileext) { 
 int	n, k, c = 0;
 
 n = direntries;
 while(n--) {
	if ((k = D_NAMLEN(dirlist[n])) < 4 ) continue;
  	if ( strcasecmp(dirlist[n]->d_name + k - 4, fileext) == 0 ) c++;
	}
 return c;
}



/*
 * Modified: 01.16.2002
 */
unsigned long hexstrtodec(unsigned char *s) {
 unsigned long  n = 0;
 unsigned char  r;

 while ( 1 ) {
	if ((unsigned char)*s >= 48 && (unsigned char)*s <= 57) {
		r = 48;
		} else if ((unsigned char)*s >= 65 && (unsigned char)*s <= 70 ) {
		r = 55;
		} else if ((unsigned char)*s >= 97 && (unsigned char)*s <= 102 ) {
		r = 87;
		} else if ((unsigned char)*s == 0 ) {
		return n;
		} else {
		return 0;
		}
	n <<= 4;
	n += *s++ - r;
	}
}

/*
 * dangling links
 */
int selector (struct dirent *d)
{
        struct stat st;
        if ((stat(d->d_name, &st) < 0)) return 0;
        return 1;
}

int selector2 (struct dirent *d)
{
        struct stat st;
        if ((stat(d->d_name, &st) < 0) || S_ISDIR(st.st_mode)) return 0;
        return 1;
}


/*
 * Modified: Unknown
 */
void rescandir() {
 if ( direntries > 0 ) {
	while ( direntries-- ) {
		free(dirlist[direntries]);
		}
	free(dirlist);
	}
 direntries = scandir(".", &dirlist, selector, 0);
}

void rescandir2() {
 if ( direntries > 0 ) {
	while ( direntries-- ) {
		free(dirlist[direntries]);
		}
	free(dirlist);
	}
 direntries = scandir(".", &dirlist, selector2, alphasort);
}


/*
 * Modified: 01.16.2002
 */
void strtolower(char *s) {
 while ( (*s = tolower(*s)) ) s++;
}



/*
 * Modified: 01.16.2002
 */
char israr(char *fileext) {
 if ( (*fileext == 'r' || *fileext =='s' || isdigit(*fileext)) &&
   ((isdigit(*(fileext + 1)) && isdigit(*(fileext + 2))) ||
   (*(fileext + 1) == 'a' && *(fileext + 2) == 'r')) &&
   *(fileext + 3) == 0 ) return 1;
 return 0;
}


/*
 * Created    : 02.20.2002
 * Author     : dark0n3
 *
 * Description: Checks if file is known mpeg/avi file
 *
 *
 */
char isvideo(char *fileext) {

 switch ( *fileext++ ) {
	case 'm':
		if (! memcmp(fileext, "pg", 3) ||
		    ! memcmp(fileext, "peg", 4) ||
		    ! memcmp(fileext, "2v", 3) ||
		    ! memcmp(fileext, "2p", 3)) {
			return 1;
			}
		break;
	case 'a':
		if (! memcmp(fileext, "vi", 3)) {
			return 1;
			}
		break;
	}

 return 0;
}

 

/*
 * Modified: Unknown
 */
void buffer_progress_bar(struct VARS *raceI) {
 int	n;

 raceI->misc.progress_bar[14] = 0;
 if (raceI->total.files > 0) {
   for ( n = 0 ; n < (raceI->total.files - raceI->total.files_missing) * 14 / raceI->total.files ; n++) raceI->misc.progress_bar[n] = '#';
   for ( ; n < 14 ; n++) raceI->misc.progress_bar[n] = ':';
 }
}



/*
 * Modified: 01.16.2002
 */
void move_progress_bar(short delete, struct VARS *raceI) {
 char 		*bar;
 int		n;
 regex_t        preg;
 regmatch_t     pmatch[ 1 ];

 regcomp( &preg, del_progressmeter, REG_NEWLINE|REG_EXTENDED );
 n = direntries;

 if ( delete ) {
	while(n--) {
		if ( regexec( &preg, dirlist[n]->d_name, 1, pmatch, 0) == 0 ) {
			if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == D_NAMLEN(dirlist[n]) ) {
				remove(dirlist[n]->d_name);
				*dirlist[n]->d_name = 0;
				return;
				}
			}
		}
	} else {
	if ( ! raceI->total.files ) return;
	bar = convert(raceI, userI, groupI, progressmeter);
	while(n--) {
		if ( regexec( &preg, dirlist[n]->d_name, 1, pmatch, 0) == 0 ) {
			if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == D_NAMLEN(dirlist[n]) ) {
				 rename(dirlist[n]->d_name, bar);
				return;
				}
			}
		}
	createstatusbar(bar);
	}
}



/*
 * Modified: Unknown
 */
short findfile(char *filename) {
 int	n = direntries;

 while( n-- ) {
	if ( ! strcasecmp(dirlist[n]->d_name, filename)) {
		return 1;
		}
	}
 return 0;
}



/*
 * Modified: 01.16.2002
 */
void removecomplete() {
 int		n;
 regex_t	preg;
 regmatch_t	pmatch[1];

 unlink(".message");
 regcomp( &preg, del_completebar, REG_NEWLINE|REG_EXTENDED );
 n = direntries;
 while(n--) {
	if(regexec(&preg, dirlist[n]->d_name, 1, pmatch, 0 ) == 0) {
		if ((int)pmatch[0].rm_so == 0 && (int)pmatch[0].rm_eo == D_NAMLEN(dirlist[n])) {
			remove(dirlist[n]->d_name);
			*dirlist[n]->d_name = 0;
			}
		}
	}
}




/*
 * Modified: 01.16.2002
 */
short matchpath(char *instr, char *path) {
 int    pos = 0;
 
 do {
	switch ( *instr ) {
		case 0:
		case ' ':
			if (strncmp(instr - pos, path, pos - 1) == 0) {
				return 1;
				}
			pos = 0;
			break;
		default:
			pos++;
			break;
		}
	} while (*instr++);
 return 0;
}



/*
 * Modified: 01.16.2002
 */
short strcomp(char *instr, char *searchstr) {
 int	pos = 0,
	k;

 k = strlen( searchstr );

 do {
	switch ( *instr ) {
		case 0:
		case ',':
			if ( k == pos && ! strncasecmp( instr - pos, searchstr, pos ) ) {
				return 1;
				}
			pos = 0;
			break;
		default:
			pos++;
			break;
		}
	} while ( *instr++ );
 return 0;
}



/* Checks if file exists */
short fileexists(char *f) {
 if ( close(open(f, O_RDONLY)) == -1 ) return 0;
 return 1;
}



/* Create symbolic link */
void createlink(char *factor1, char *factor2, char *source, char *ltarget) {
 char	*org,
	*target;
 int	l1, l2, l3;

 l1 = strlen( factor1 ) + 1;
 l2 = strlen( factor2 ) + 1;
 l3 = strlen( ltarget ) + 1;

 org = target = m_alloc( l1 + l2 + l3 );

 memcpy( target, factor1, l1 );
 target += l1;
 *(target - 1) = '/';

 memcpy(target, factor2, l2);
 target += l2;

 memcpy(target - 1, "/", 2);

 mkdir(org, 0777);

 memcpy( target, ltarget, l3 );
 symlink(source, org);
 m_free( org );
}



void readsfv_ffile(char *filename, long buf_bytes ) {
 int    fd,
        line_start = 0,
        index_start,
        ext_start,
        n;
 char   *buf,
        *fname;

 fd  = open( filename, O_RDONLY );
 buf = m_alloc( buf_bytes + 2 );
 read(fd, buf, buf_bytes);
 close( fd );
  
 for ( n = 0 ; n <= buf_bytes; n++ ) {
	if ( buf[n] == '\n' || n == buf_bytes ) {
		index_start = n - line_start;
		if ( buf[line_start] != ';' ) {
			while ( buf[index_start + line_start] != ' ' && index_start-- );
			if ( index_start > 0 ) {
				buf[index_start + line_start] = 0;
				fname = buf + line_start;
				ext_start = index_start;
				while ( (fname[ext_start] = tolower(fname[ext_start])) != '.' && ext_start > 0 ) ext_start--;
				if ( fname[ext_start] != '.' ) {
					ext_start = index_start; 
					} else {
					ext_start++;
					}
				index_start++;
				if ( ! strcomp(ignored_types, fname + ext_start) ) {
					if ( findfile(fname) ) raceI.total.files_missing--;
					}
				}
			}
		line_start = n + 1;
		}
	}
 raceI.total.files_missing += raceI.total.files;
 m_free(buf);
}




void get_rar_info(char *filename) {
 FILE	*file;

 file = fopen(filename, "r");
 fseek(file, 45, SEEK_CUR);
 fread(&raceI.file.compression_method, 1, 1, file);
 fclose(file);
}



/*
 * Modified   : 02.07.2002
 * Author     : dark0n3
 *
 * Description: Executes extern program and returns return value
 *
 */
int execute(char *s) {
 int    n;
 int	args = 0;
 char	*argv[128]; /* Noone uses this many args anyways */

 argv[0] = s;
 while ( 1 ) {
	if ( *s == ' ' ) {
		*s = 0;
		args++;
		argv[args] = s + 1;
		} else if ( *s == 0 ) {
		args++;
		argv[args] = NULL;
		break;
		}
	s++;
	}

 switch ( fork() ) {
	case 0: 
		close(1);
		close(2);
		n = execv(argv[0], argv);
		exit(0);
		break;
	default:
		wait(&n);
		break;
	}

 return n >> 8;
}
