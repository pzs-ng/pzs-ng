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
#include "../include/zsconfig.defaults.h"

#ifndef PATH_MAX
# define _LIMITS_H_
# include <sys/syslimits.h>
#endif

struct tm *timenow;
time_t     tnow;

/* Relic from old cleaner.. but it works, so why to change? */
void scandirectory(char *directoryname, int setfree) {
    struct dirent **namelist, **namelist2;
    int m, n, fd;
    printf("[%s]\n", directoryname);
    if ( chdir(directoryname) != -1 ) {
	if ((n = scandir(".", &namelist, 0, 0)) > 0)        
	    while(n--) if ( namelist[n]->d_name[0] != '.' ) {  
		chdir(namelist[n]->d_name);
		if ((m = scandir(".", &namelist2, 0, 0)) > 0) while (m--) if ( namelist2[m]->d_name[0] != '.' ) {
		    if ( (fd = open(namelist2[m]->d_name, O_NDELAY, 0777)) != -1 ) close(fd);
		    else if (setfree) {
			unlink(namelist2[m]->d_name);
			printf("Broken symbolic link \"%s\" removed.\n", namelist2[m]->d_name);
		    }
		}
		free(namelist2);
		chdir("..");
		if (setfree)
			rmdir(namelist[n]->d_name);
	    }
	free(namelist);
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

char * multi_nfo_name(char *s) {
    int	begin_multi[2], end_multi, n;
    char 	*p, *t, *r = 0;

    end_multi = 0;

    t = incomplete_base_nfo_indicator;

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
 */
char * single_name(char *s) {
    int	begin_single, end_single, size;
    char	*t = 0;

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

char * single_nfo_name(char *s) {
    int	begin_single, end_single, size;
    char	*t = 0;

    begin_single = end_single = 0;

    t = incomplete_nfo_indicator;

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

void incomplete_cleanup(char *path, int setfree) {
    struct dirent	**dirlist;
    struct stat		fileinfo;
    int			entries;
    regex_t		preg[4];   
    regmatch_t		pmatch[1];
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

    sprintf(temp, "%s", incomplete_base_nfo_indicator);
    locator = replace_cookies(temp);
    regcomp(&preg[2], locator, REG_NEWLINE|REG_EXTENDED);
    free(locator);

    sprintf(temp, "%s", incomplete_nfo_indicator);
    locator = replace_cookies(temp);
    regcomp(&preg[3], locator, REG_NEWLINE|REG_EXTENDED);
    free(locator);


    free(temp);

    printf("[%s]\n", path);

    if ( chdir(path) != -1 ) {
	if ((entries = scandir(".", &dirlist, 0, 0)) != -1 ) {
	    while ( entries-- ) {

		/* Multi CD */
		if ( regexec(&preg[0], dirlist[entries]->d_name, 1, pmatch, 0) == 0 ) {
		    if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[entries]) ) {
			temp=multi_name(dirlist[entries]->d_name);
			if ( stat(temp, &fileinfo) != 0) {
				if (setfree) {
					unlink(dirlist[entries]->d_name);
					printf("Broken symbolic link \"%s\" removed.\n", dirlist[entries]->d_name);
				}
			} else printf("Incomplete release: \"%s%s\".\n", path, temp);
			free(temp);
			continue;
		    }
		}

		if ( regexec(&preg[2], dirlist[entries]->d_name, 1, pmatch, 0) == 0 ) {
		    if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[entries]) ) {
			temp=multi_nfo_name(dirlist[entries]->d_name);
			if ( stat(temp, &fileinfo) != 0) {
				if (setfree) {
					unlink(dirlist[entries]->d_name);
					printf("Broken symbolic link \"%s\" removed.\n", dirlist[entries]->d_name);
				}
			}
			free(temp);
			continue;
		    }
		}

		/* Normal */
		if ( regexec(&preg[1], dirlist[entries]->d_name, 1, pmatch, 0) == 0 ) {
		    if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[entries]) ) {
			temp=single_name(dirlist[entries]->d_name);
			if ( stat(temp, &fileinfo) != 0 ) {
				if (setfree) {
					unlink(dirlist[entries]->d_name);
					printf("Broken symbolic link \"%s\" removed.\n", dirlist[entries]->d_name);
				}
			} else printf("Incomplete release: \"%s%s\".\n", path, temp);
			free(temp);
			continue;
		    }
		}

		if ( regexec(&preg[3], dirlist[entries]->d_name, 1, pmatch, 0) == 0 ) {
		    if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[entries]) ) {
			temp=single_nfo_name(dirlist[entries]->d_name);
			if ( stat(temp, &fileinfo) != 0 ) {
				if (setfree) {
					unlink(dirlist[entries]->d_name);
					printf("Broken symbolic link \"%s\" removed.\n", dirlist[entries]->d_name);
				}
			}
			free(temp);
			continue;
		    }
		}
		free(dirlist[entries]);
	    }
	    free(dirlist);
	} else {
	    fprintf(stderr, "Unable to scandir(%s)\n", path);
	}
    } else {
	fprintf(stderr, "Unable to chdir(%s)\n", path);
    }
    regfree(&preg[0]);
    regfree(&preg[1]);
    regfree(&preg[2]);
    regfree(&preg[3]);
}

void cleanup(char *pathlist, int setfree, char *startpath) {
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

		sprintf(path, "%s%.*s", startpath, (int)(newentry - entry), entry);
		strftime(data_today, PATH_MAX, path, time_today);
		strftime(data_yesterday, PATH_MAX, path, time_yesterday);


		if (strcmp(data_today, data_yesterday)) {
		    if ( check_yesterday == TRUE ) incomplete_cleanup(data_yesterday, setfree);
		    if ( check_today == TRUE ) incomplete_cleanup(data_today, setfree);
		} else 
		    incomplete_cleanup(data_today, setfree);

		if (!*newentry)
		    break;
		
		newentry++;
    }

    free(path);
    free(data_today);
    free(data_yesterday);
    free(time_today);
    free(time_yesterday);
}

int main (int argc, char **argv) {

    int		setfree		= 1;
    char 	*startdir	= 0;

    startdir=malloc(PATH_MAX);
    sprintf(startdir, "/");

    if (argc > 1) {
	setfree = 0;
	printf("%s: Running script in view mode only.\n", argv[0]);
	sprintf(startdir, "%s", argv[1]);
    }

    cleanup(cleanupdirs, setfree, startdir);

    if (argc < 2) {

#if ( audio_genre_sort == TRUE )
	scandirectory((char *)audio_genre_path, setfree);
#endif

#if ( audio_year_sort == TRUE ) 
	scandirectory((char *)audio_year_path, setfree);
#endif

#if ( audio_artist_sort == TRUE )
	scandirectory((char *)audio_artist_path, setfree);
#endif

#if ( audio_group_sort == TRUE )
	scandirectory((char *)audio_group_path, setfree);
#endif

	printf("Finished successfully.\n");
    }
    free(startdir);
    exit(EXIT_SUCCESS);
}
