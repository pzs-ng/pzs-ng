/************************************************************************************************/
/*                                                                                              */
/* NG-CHOWN v0.1 (c) 2004 Team pzs-ng                                                           */
/*                                                                                              */
/* This util will cange the uid or gid of the dir/file passed as argument, or                   */
/* dir and all files inside, or just the files inside the dir. Uhm.. anyway,                    */
/* here's how you use it:                                                                       */
/*                                                                                              */
/* ng-chown <uid> <gid> <uid_flag> <gid_flag> <files_flag> <dir/file_flag> <name of dir/file>   */
/*                                                                                              */
/* <uid> == user id (number between 1 and 65535)                                                */
/* <gid> == group id (number between 1 and 65535)                                               */
/* <uid_flag> == if set to 1, the uid of the file/dir will be modified                          */
/* <gid_flag> == if set to 1, the gid of the file/dir will be modified                          */
/* <files_flag> == if set to 1, the files in the dir <name of dir/file> will be modified        */
/* <dir/file_flag> == if set to 1, the dir/file <name of dir/file> will be modified             */
/* <name of dir/file> == name of the file dir to which the changes is to be applied             */
/*                                                                                              */
/************************************************************************************************/

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef PATH_MAX
#define _LIMITS_H_
#include <sys/syslimits.h>
#endif
#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

struct dirent   **dirlist;
unsigned int direntries,n = 0;


#if defined(__linux__)
    int selector3 (const struct dirent *d)
#elif defined(__NetBSD__)
int selector3 (const struct dirent *d)
#else
int selector3 (struct dirent *d)
#endif
{
    struct stat st;
    if ((stat(d->d_name, &st) < 0) || S_ISDIR(st.st_mode)) return 0;
    return 1;
}

short int matchpath2(char *instr, char *path) {
    int pos = 0;

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

int myscandir(char *my_path, char *my_name) {
    if ( ( chdir(my_path) != -1 ) && ( ( matchpath2(group_dirs, my_path) ) || ( matchpath2(zip_dirs, my_path ) ) || ( matchpath2(sfv_dirs, my_path) ) ) ) {
	if ( direntries > 0 ) {
	    while ( direntries-- ) {
		free(dirlist[direntries]);
	    }
	    free(dirlist);
	}
	direntries = scandir(".", &dirlist, selector3, alphasort);
	return 0;
    } else {
	printf("%s - Error: Unable to chdir to %s\n", my_name, my_path);
	printf("%s - Syntax: %s <uid> <gid> <uid_flag> <gid_flag> <files_flag> <dir/file_flag> <name of dir/file>\n", my_name, my_name);
	return 1;
    }
}

int myscan(int my_result, int new_user, int new_group, int user_flag, int group_flag, int files_flag, int dir_flag, char *my_path, char *my_name) {
    int my_total = 0;

    /* change the uid or gid depending on the flag */
    if (user_flag == 0) {
	new_user = -1;
    }
    if (group_flag == 0) {
	new_group = -1;
    }

    if (files_flag == 1) {
	if ( myscandir(my_path, my_name) == 0 ) {
	    n = direntries;
	    while ( n-- ) {
		my_total += access(dirlist[n]->d_name, R_OK);
		chown(dirlist[n]->d_name, new_user, new_user);
	    }
	    if ((my_result == 0) && (dir_flag == 1)) {
		chown(my_path, new_user, new_group);
	    }
	    return my_total;
	} else {
	    return 1;
	}
    } else {
	if (dir_flag == 1) {
	    my_result = access(my_path, R_OK);
	    if (my_result == 0) {
		chown(my_path, new_user, new_group);
	    } else {
		printf("%s - Error: Unable to read file %s\n", my_name, my_path);
		printf("%s - Syntax: %s <uid> <gid> <uid_flag> <gid_flag> <files_flag> <dir/file_flag> <name of dir/file>\n", my_name, my_name);
	    }
	}
	return my_result;
    }
}

int main(int argc, char *argv[]) {
    int my_result = 1;
    int new_user;
    int new_group;
    int user_flag;
    int group_flag;
    int files_flag;
    int dir_flag;
    char my_path[PATH_MAX];

    if ( argc != 8 ) {
	printf("%s - Error: Wrong number of args given.\n", argv[0]);
	printf("%s - Syntax: %s <uid> <gid> <uid_flag> <gid_flag> <files_flag> <dir/file_flag> <name of dir/file>\n", argv[0], argv[0]);
	return 1;
    }

    new_user = atoi(argv[1]);    /* uid of user */
    new_group = atoi(argv[2]);   /* gid of user */
    user_flag = atoi(argv[3]);   /* change the uid of the files/dir if >0      */
    group_flag = atoi(argv[4]);  /* change the gid of the files/dir if >0      */
    files_flag = atoi(argv[5]);  /* make changes on the files in the dir if >0 */
    dir_flag = atoi(argv[6]);    /* make changes on the dir itself if >0       */

    if ( ( strlen(argv[7]) < PATH_MAX ) && ( new_user < 65536 ) && ( new_group < 65535 ) ) { 
	sprintf(my_path, "%s", argv[7]);
    } else {
	printf("%s - Error in argument(s)\n", argv[0]);
	printf("%s - Syntax: %s <uid> <gid> <uid_flag> <gid_flag> <files_flag> <dir/file_flag> <name of dir/file>\n", argv[0], argv[0]);
	return 1;
    }
    my_result = myscan(my_result, new_user, new_group, user_flag, group_flag, files_flag, dir_flag, my_path, argv[0]);
    if ( my_result == 0 ) {
	if ( (new_user > 0) || (new_group > 0) ) {
	    setuid(0);
	    setgid(0);
	    my_result = myscan(my_result, new_user, new_group, user_flag, group_flag, files_flag, dir_flag, my_path, argv[0]);
	}
    }
    return my_result;
}

