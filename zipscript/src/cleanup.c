#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

#include "objects.h"
#include "macros.h"
#include "../conf/zsconfig.h"

#ifndef PATH_MAX
# define _LIMITS_H_
# include <sys/syslimits.h>
#endif

struct tm *timenow;
time_t     tnow;


/* Relic from old cleaner.. but it works, so why to change?
 * 
 *
 *
 */
void scandirectory(char *directoryname) {
 struct dirent **namelist, **namelist2;  
 int m, n, fd;
 if ( chdir(directoryname) != -1 ) {
  if ((n = scandir(".", &namelist, 0, 0)) > 0)        
   while(n--) if ( namelist[n]->d_name[0] != '.' ) {  
    chdir(namelist[n]->d_name);
    if ((m = scandir(".", &namelist2, 0, 0)) > 0) while (m--) if ( namelist2[m]->d_name[0] != '.' ) {
     if ( (fd = open(namelist2[m]->d_name, O_NDELAY, 0777)) != -1 ) close(fd);
      else unlink(namelist2[m]->d_name);
    }
    chdir("..");
    rmdir(namelist[n]->d_name);
   }
 }
}


char * replace_cookies(char *s) {
 char	*new_string, *pos;

 new_string = pos = malloc(4096);

 while ( *s == '.' || *s == '/' ) s++;

 for ( ; *s ; s++ ) switch ( *s ) {
	case '%':
		s++;
		switch ( *s ) {
			case '1': pos += sprintf(pos, ".*"); break;
			case '0': pos += sprintf(pos, ".*"); break;
			case '%': *pos++ = '%';
			}
		break;
	case '(':
	case ')':
	case '[':
	case ']':
		pos += sprintf(pos, "\\%c", *s);
		break;
	default:
		*pos++ = *s;
	}
 *pos = 0;

 return new_string;
}



/*
 * Name of release (Multi CD)
 *
 *
 */
char * multi_name(char *s) {
 int	begin_multi[2], end_multi, n;
 char 	*p, *t, *r = 0;

 end_multi = 0;

 t = incomplete_cd_indicator;

 while ( *t == '.' || *t == '/' ) t++;
 p = t;

 for ( n = 0 ; *t ; t++ ) if ( *t == '%' && *(t+1) == '0' ) {
	begin_multi[0] = n;
	break;
	} else {
	n++;
	}

 t = p;
 for ( n = 0 ; *t ; t++ ) if ( *t == '%' && *(t+1) == '1' ) {
	begin_multi[1] = n;
	break;
	} else {
	n++;
	}

 if ( begin_multi[0] < begin_multi[1] ) {
	for ( t = p + begin_multi[1] + 2; *t ; t++ ) end_multi++;

	p = malloc(6);
	s += begin_multi[0];
	s += sprintf(p, "%.*s", tolower(*s) == 'd' ? 5 : 3,  s);

	s += begin_multi[1] - begin_multi[0] - 2;
	t = malloc((n = strlen(s)) + 7);
	sprintf(t, "%.*s/%s", n - end_multi, s, p);
	} else {
	for ( t = p + begin_multi[0] + 2; *t ; t++ ) end_multi++;

	s += begin_multi[1];
	n = strlen(s) - end_multi;

	t = s + n - 3;
	p = malloc(6);
	if ( tolower(*t) == 'c' ) {
		sprintf(p, "%s", t);
		} else {
		r--;
		sprintf(p, "%s", t);
		}
	n = t - s - (begin_multi[0] - begin_multi[1] - 2);
	t = malloc(n + 7);
	sprintf(r, "%.*s/%s", n, s, p);
	}

 free(p);
 return t;
}





/*
 * Name of release (Common)
 *
 *
 */
char * single_name(char *s) {
 int	begin_single, end_single, size;
 char	*t;

 begin_single = end_single = 0;

 t = incomplete_indicator;

 while ( *t == '.' || *t == '/' ) t++;

 for ( ; *t ; t++ ) if ( *t == '%' && *(t+1) == '0' ) {
	t += 2;
	break;
	} else {
	begin_single++;
	}
 for ( ; *t ; t++ ) if ( *t == '%' ) {
	t++;
	if ( *t == '%' ) end_single++;
	} else {
	end_single++;
	}

 size = strlen(s) - begin_single - end_single + 1;
 
 t = malloc(size);
 sprintf(t, "%.*s", size - 1, s + begin_single);

 return(t);
}



void incomplete_cleanup(char *path) {
 struct dirent **dirlist;
 struct stat	fileinfo;
 int		entries;
 regex_t        preg[2];   
 regmatch_t     pmatch[1];
 char		*temp;
 char		*locator;

 temp = malloc(sizeof(incomplete_cd_indicator) > sizeof(incomplete_indicator) ? sizeof(incomplete_cd_indicator) : sizeof(incomplete_indicator));

 sprintf(temp, "%s", incomplete_cd_indicator);
 locator = replace_cookies(temp);
 regcomp(&preg[0], locator, REG_NEWLINE|REG_EXTENDED);
 free(locator);

 sprintf(temp, "%s", incomplete_indicator);
 locator = replace_cookies(temp);
 regcomp(&preg[1], locator, REG_NEWLINE|REG_EXTENDED);
 free(locator);

 free(temp);

 if ( chdir(path) != -1 ) {
	if ((entries = scandir(".", &dirlist, 0, 0)) != -1 ) {
		while ( entries-- ) {

			/* Multi CD */

	                if ( regexec(&preg[0], dirlist[entries]->d_name, 1, pmatch, 0) == 0 ) {
				if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[entries]) ) {
					if ( stat(multi_name(dirlist[entries]->d_name), &fileinfo) != 0 ) {
						unlink(dirlist[entries]->d_name);
						}
					continue;
					}
				}

			/* Normal */

	                if ( regexec(&preg[1], dirlist[entries]->d_name, 1, pmatch, 0) == 0 ) {
				if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[entries]) ) {
					if ( stat(single_name(dirlist[entries]->d_name), &fileinfo) != 0 ) {
						unlink(dirlist[entries]->d_name);
						}
					continue;
					}
				}
			}
		} else {
		fprintf(stderr, "Unable to scandir(%s)\n", path);
		}
	} else {
	fprintf(stderr, "Unable to chdir(%s)\n", path);
	}
}



void cleanup(char *pathlist) {
 char	*data, *path, *newentry, *entry;

 data = malloc(PATH_MAX);
 path = malloc(PATH_MAX);

 newentry = pathlist;

 while ( 1 ) {
	for ( entry = newentry ; *newentry != ' ' && *newentry != 0 ; newentry++ );
	
	sprintf(path, "%.*s", (int)(newentry - entry), entry);
	strftime(data, PATH_MAX, path, timenow);

	incomplete_cleanup(data);

	if ( *newentry == 0 ) {
		break;
		}
	newentry++;
	}

 free(data);
 free(path);
}



int main(void) {
 tnow = time( NULL );
 timenow = localtime( &tnow ); 

/* chroot(site_root);*/
 #if ( audio_genre_sort == TRUE )
 scandirectory((char *)audio_genre_path);
 #endif
 #if ( audio_year_sort == TRUE ) 
 scandirectory((char *)audio_year_path);
 #endif
 #if ( audio_artist_sort == TRUE )
 scandirectory((char *)audio_artist_path);
 #endif
 if ( cleanupdirs[0] ) cleanup(cleanupdirs);
 exit(EXIT_SUCCESS);
}
