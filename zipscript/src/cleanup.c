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

/* Relic from old cleaner.. but it works, so why to change? */
void scandirectory(char *directoryname) {
    struct dirent **namelist, **namelist2;
    int m, n, fd;
    printf("[%s]\n", directoryname);
    if ( chdir(directoryname) != -1 ) {
	if ((n = scandir(".", &namelist, 0, 0)) > 0)        
	    while(n--) if ( namelist[n]->d_name[0] != '.' ) {  
		chdir(namelist[n]->d_name);
		if ((m = scandir(".", &namelist2, 0, 0)) > 0) while (m--) if ( namelist2[m]->d_name[0] != '.' ) {
		    if ( (fd = open(namelist2[m]->d_name, O_NDELAY, 0777)) != -1 ) close(fd);
		    else {
			unlink(namelist2[m]->d_name);
			printf("Broken symbolic link \"%s\" removed.\n", namelist2[m]->d_name);
		    }
		}
		chdir("..");
		rmdir(namelist[n]->d_name);
	    }
    }
}

void replace_cookies(char *s, char *pos) {

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

}

/*
 * Name of release (Multi CD)
 */
void multi_name(char *s, char *q, char *retval) {
    int	begin_multi[2], end_multi, n;
    char 	*p, *r = 0;

    end_multi = 0;

    retval = q;

    while ( *retval == '.' || *retval == '/' ) retval++;
    p = retval;

    for ( n = 0 ; *retval ; retval++ ) if ( *retval == '%' && *(retval + 1) == '0' ) {
	begin_multi[0] = n;
	break;
    } else {
	n++;
    }

    retval = p;
    for ( n = 0 ; *retval ; retval++ ) if ( *retval == '%' && *(retval + 1) == '1' ) {
	begin_multi[1] = n;
	break;
    } else {
	n++;
    }

    if ( begin_multi[0] < begin_multi[1] ) {
	for ( retval = p + begin_multi[1] + 2; *retval ; retval++ ) end_multi++;

	p = malloc(6);
	s += begin_multi[0];
	s += sprintf(p, "%.*s", tolower(*s) == 'd' ? 5 : 3,  s);

	s += begin_multi[1] - begin_multi[0] - 2;
	sprintf(retval, "%.*s/%s", n - end_multi, s, p);
    } else {
	for ( retval = p + begin_multi[0] + 2; *retval ; retval++ ) end_multi++;

	s += begin_multi[1];
	n = strlen(s) - end_multi;

	retval = s + n - 3;
	p = malloc(6);
	if ( tolower(*retval) == 'c' ) {
	    sprintf(p, "%s", retval);
	} else {
	    r--;
	    sprintf(p, "%s", retval);
	}
	n = retval - s - (begin_multi[0] - begin_multi[1] - 2);
	sprintf(r, "%.*s/%s", n, s, p);
    }

    free(p);
}

/*
 * Name of release (Common)
 */
void single_name(char *s, char *inc, char *retval) {
    int	begin_single, end_single, size;
    char	*t;

    begin_single = end_single = 0;

    t = inc;

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
    sprintf(retval, "%.*s", size - 1, s + begin_single);
}

void incomplete_cleanup(char *path, char *cd_inc, char *inc) {
    struct dirent	**dirlist;
    struct stat		fileinfo;
    int			entries;
    regex_t		preg[2];   
    regmatch_t		pmatch[1];
    char		*temp;
    char		*locator;

    temp = malloc(PATH_MAX);
    *temp = 0;
    locator = malloc(PATH_MAX);
    *locator = 0;

    sprintf(temp, "%s", cd_inc);
    replace_cookies(temp, locator);
    regcomp(&preg[0], locator, REG_NEWLINE|REG_EXTENDED);

    sprintf(temp, "%s", inc);
    replace_cookies(temp, locator);
    regcomp(&preg[1], locator, REG_NEWLINE|REG_EXTENDED);

    printf("[%s]\n", path);

    if ( chdir(path) != -1 ) {
	if ((entries = scandir(".", &dirlist, 0, 0)) != -1 ) {
	    while ( entries-- ) {

		/* Multi CD */
		if ( regexec(&preg[0], dirlist[entries]->d_name, 1, pmatch, 0) == 0 ) {
		    if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[entries]) ) {
			multi_name(dirlist[entries]->d_name, cd_inc, temp);
			if ( stat(temp, &fileinfo) != 0 ) {
			    unlink(dirlist[entries]->d_name);
			    printf("Broken symbolic link \"%s\" removed.\n", dirlist[entries]->d_name);
			} else printf("Incomplete release \"%s\".\n", temp);
			continue;
		    }
		}

		/* Normal */
		if ( regexec(&preg[1], dirlist[entries]->d_name, 1, pmatch, 0) == 0 ) {
		    if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[entries]) ) {
			single_name(dirlist[entries]->d_name, inc, temp);
			if ( stat(temp, &fileinfo) != 0 ) {
			    unlink(dirlist[entries]->d_name);
			    printf("Broken symbolic link \"%s\" removed.\n", dirlist[entries]->d_name);
			} else printf("Incomplete release \"%s\".\n", temp);
			continue;
		    }
		}
		free(dirlist[entries]);
	    }
	} else {
	    fprintf(stderr, "Unable to scandir(%s)\n", path);
	}
        free(dirlist);
    } else {
	fprintf(stderr, "Unable to chdir(%s)\n", path);
    }
    regfree(&preg[0]);
    regfree(&preg[1]);
    free(temp);
    free(locator);
}

void cleanup(char *pathlist) {
    char *data_today, *data_yesterday, *path, *newentry, *entry;

    struct tm *time_today, *time_yesterday;
    time_t     t_today, t_yesterday;
    
    path                = malloc(PATH_MAX);
    data_today          = malloc(PATH_MAX);
    data_yesterday      = malloc(PATH_MAX);

    time_today=malloc(sizeof(struct tm));
    time_yesterday=malloc(sizeof(struct tm));


    t_today = time( NULL );
    time_today = localtime_r( &t_today, time_today );

    t_yesterday = time( NULL ) - (60 * 60 * 24); /* 86400 seconds back == 1 day */
    time_yesterday = localtime_r( &t_yesterday, time_yesterday );

    newentry = pathlist;

    while ( 1 ) {
		for (entry = newentry; *newentry != ' ' && *newentry != 0; newentry++);

		sprintf(path, "%.*s", (int)(newentry - entry), entry);
		strftime(data_today, PATH_MAX, path, time_today);
		strftime(data_yesterday, PATH_MAX, path, time_yesterday);


		if (strcmp(data_today, data_yesterday)) {
		    if ( check_yesterday == TRUE ) {
			incomplete_cleanup(data_yesterday, incomplete_cd_indicator, incomplete_indicator);
			incomplete_cleanup(data_yesterday, incomplete_cd_nfo_indicator, incomplete_nfo_indicator);
		    }
		    if ( check_today == TRUE ) {
			incomplete_cleanup(data_today, incomplete_cd_indicator, incomplete_indicator);
			incomplete_cleanup(data_today, incomplete_cd_nfo_indicator, incomplete_nfo_indicator);
		    }
		} else {
		    incomplete_cleanup(data_today, incomplete_cd_indicator, incomplete_indicator);
		    incomplete_cleanup(data_today, incomplete_cd_nfo_indicator, incomplete_nfo_indicator);
		}

		if (!*newentry)
		    break;
		
		newentry++;
    }

    free(data_today);
    free(data_yesterday);
    free(path);
    free(time_today);
    free(time_yesterday);
}

int main(void) {

    if (cleanupdirs[0])
		cleanup(cleanupdirs);

#if ( audio_genre_sort == TRUE )
    scandirectory((char *)audio_genre_path);
#endif

#if ( audio_year_sort == TRUE ) 
    scandirectory((char *)audio_year_path);
#endif

#if ( audio_artist_sort == TRUE )
    scandirectory((char *)audio_artist_path);
#endif

#if ( audio_group_sort == TRUE )
    scandirectory((char *)audio_group_path);
#endif

    exit(EXIT_SUCCESS);
}
